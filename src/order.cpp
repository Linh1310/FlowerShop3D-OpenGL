#include <GL/freeglut.h>
#include <algorithm>
#include "flowers.h"
#include "model3d.h"
#include "order.h"
#include "people.h"
#include "scene.h"
#include "shop.h"

namespace {
constexpr FlowerOrder recipes[] = {{2, 1}, {1, 2}, {3, 1}, {2, 2}, {1, 3}};
constexpr int recipeCount = sizeof recipes / sizeof recipes[0];
constexpr float tableOffsets[][2] = {
    {-0.16f, -0.16f}, {0.16f, -0.16f},
    {-0.16f, 0.15f}, {0.16f, 0.15f}
};
struct TablePot {
    FlowerType type;
    float landing;
};
TablePot tablePots[sizeof tableOffsets / sizeof tableOffsets[0]]{};
int currentOrder = 1;
FlowerOrder packed{};
const char* message = "Chon chau hoa tren ke de bat dau.";
}

const FlowerOrder& orderRequested() { return recipes[(currentOrder - 1) % recipeCount]; }
const FlowerOrder& orderPacked() { return packed; }
int orderNumber() { return currentOrder; }
const char* orderMessage() { return message; }

bool orderComplete() {
    const FlowerOrder& requested = orderRequested();
    return packed.tulips == requested.tulips && packed.daisies == requested.daisies;
}

void orderAddSelected() {
    if (inspectActive() || !customerIsOrdering() || orderComplete()) return;
    const int flowerType = sceneSelectedFlowerType();
    if (flowerType < 0) {
        message = "Hay chon mot chau hoa tren ke.";
        return;
    }

    const FlowerOrder& requested = orderRequested();
    const bool tulip = flowerType == 0;
    int& count = tulip ? packed.tulips : packed.daisies;
    const int needed = tulip ? requested.tulips : requested.daisies;
    if (count == needed) {
        message = tulip ? "Don nay da du tulip." : "Don nay da du cuc kim.";
        return;
    }

    tablePots[packed.tulips + packed.daisies] = {
        tulip ? FlowerType::TULIP : FlowerType::DAISY, 0.0f
    };
    ++count;
    staffPackFlower();
    if (orderComplete()) {
        customerOrderReady();
        message = "Da soan xong! Bam Giao don.";
    }
    else message = tulip ? "Da them mot chau tulip." : "Da them mot chau cuc kim.";
}

void orderClear() {
    if (!customerIsOrdering() || orderComplete()) return;
    packed = {};
    message = "Da dat lai don. Chon chau de soan lai.";
}

void orderNext() {
    if (!orderComplete()) return;
    customerCollectOrder();
    ++currentOrder;
    packed = {};
    message = "Da giao don. Khach dang ra cua.";
}

void orderCancel(bool timedOut) {
    if (!customerIsOrdering() || orderComplete()) return;
    customerDismiss(timedOut);
    ++currentOrder;
    packed = {};
    message = timedOut ? "Het gio! Khach gian, bo ve." : "Da huy don va tien khach.";
}

bool orderDoorEnabled() { return customerIsOrdering() || customerCanEnter(); }

const char* orderDoorLabel() {
    if (customerCanEnter()) return "Goi khach vao";
    if (!customerIsOrdering()) return "Khach dang ra cua...";
    if (orderComplete()) return "Giao don / Tien khach";
    return "Tien khach ra";
}

void orderDoorAction() {
    if (customerCanEnter()) {
        customerEnter();
        message = "Khach moi dang di vao.";
    } else if (orderComplete()) orderNext();
    else orderCancel(false);
}

void updateOrderDisplay(float elapsed) {
    for (int i = 0; i < packed.tulips + packed.daisies; ++i) {
        tablePots[i].landing = std::min(1.0f, tablePots[i].landing + elapsed / 0.75f);
    }
}

void drawOrderFlowers() {
    const ShopPoint table = shopPackingTableTop();
    glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_CURRENT_BIT);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    for (int i = 0; i < packed.tulips + packed.daisies; ++i) {
        const TablePot& pot = tablePots[i];
        const float lift = (1.0f - pot.landing) * (1.0f - pot.landing);
        glPushMatrix();
        glTranslatef(table.x + tableOffsets[i][0] + 0.15f * lift,
                     table.y + 0.44f * lift,
                     table.z + tableOffsets[i][1] - 0.23f * lift);
        glScalef(0.49f, 0.49f, 0.49f);
        drawFlower(pot.type);
        glPopMatrix();
    }
    glPopAttrib();
}
