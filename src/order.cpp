#include <GL/freeglut.h>
#include "flowers.h"
#include "model3d.h"
#include "order.h"
#include "scene.h"
#include "shop.h"

namespace {
constexpr FlowerOrder recipes[] = {{2, 1}, {1, 2}, {3, 1}, {2, 2}, {1, 3}};
constexpr int recipeCount = sizeof recipes / sizeof recipes[0];
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
    if (inspectActive() || orderComplete()) return;
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

    ++count;
    if (orderComplete()) message = "Da soan xong! Bam Don tiep.";
    else message = tulip ? "Da them mot chau tulip." : "Da them mot chau cuc kim.";
}

void orderClear() {
    packed = {};
    message = "Da dat lai don. Chon chau de soan lai.";
}

void orderNext() {
    if (!orderComplete()) return;
    ++currentOrder;
    packed = {};
    message = "Don moi da san sang.";
}

void drawOrderFlowers() {
    const ShopPoint table = shopPackingTableTop();
    constexpr float offsets[][2] = {
        {-0.16f, -0.16f}, {0.16f, -0.16f},
        {-0.16f, 0.15f}, {0.16f, 0.15f}
    };

    glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_CURRENT_BIT);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    for (int i = 0; i < packed.tulips + packed.daisies; ++i) {
        glPushMatrix();
        glTranslatef(table.x + offsets[i][0], table.y, table.z + offsets[i][1]);
        glScalef(0.49f, 0.49f, 0.49f);
        drawFlower(i < packed.tulips ? FlowerType::TULIP : FlowerType::DAISY);
        glPopMatrix();
    }
    glPopAttrib();
}
