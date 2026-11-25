#include <GL/freeglut.h>
#include <algorithm>
#include <cmath>
#include "flowers.h"
#include "model3d.h"
#include "order.h"
#include "people.h"
#include "scene.h"
#include "shop.h"

namespace {
struct Recipe {
    FlowerOrder flowers;
    WrapMaterial paper, ribbon;
    const char* occasion;
    const char* request;
    const char* wrap;
};
constexpr Recipe recipes[] = {
    {{2, 1}, WrapMaterial::Pink, WrapMaterial::Cream, "Qua sinh nhat",
     "Tang sinh nhat ban minh nhe!", "Giay hong + no kem"},
    {{1, 2}, WrapMaterial::Kraft, WrapMaterial::Green, "Loi cam on",
     "Minh muon gui loi cam on.", "Giay kraft + no xanh"},
    {{2, 2}, WrapMaterial::Kraft, WrapMaterial::Cream, "Hoa tham nha",
     "Goi moc mac de minh dem tham nha.", "Giay kraft + no kem"}
};
constexpr int recipeCount = sizeof recipes / sizeof recipes[0];
constexpr float colors[][3] = {
    {0.92f, 0.51f, 0.60f}, {0.73f, 0.48f, 0.30f},
    {0.98f, 0.87f, 0.63f}, {0.31f, 0.58f, 0.43f}
};
constexpr float offsets[][2] = {
    {-0.16f, -0.16f}, {0.16f, -0.16f}, {-0.16f, 0.13f}, {0.16f, 0.13f}
};
constexpr float pi = 3.14159265f;
constexpr float wrapDuration = 1.15f;
constexpr float handoffDuration = 2.60f;
struct TablePot { FlowerType type; float landing; };
TablePot tablePots[sizeof offsets / sizeof offsets[0]]{};
enum class Stage { Preparing, Folding, Ready, Delivering, Delivered };

int currentOrder = 1;
FlowerOrder packed{};
Stage stage = Stage::Preparing;
int paper = -1;
int ribbon = -1;
float foldTime = 0.0f;
float deliveryTime = 0.0f;
const char* message = "Chon chau hoa tren ke de bat dau.";

const Recipe& currentRecipe() { return recipes[(currentOrder - 1) % recipeCount]; }
int flowerCount() { return packed.tulips + packed.daisies; }

bool hasFlowers() {
    const FlowerOrder& requested = currentRecipe().flowers;
    return packed.tulips == requested.tulips && packed.daisies == requested.daisies;
}

void clearBouquet() {
    packed = {};
    paper = -1;
    ribbon = -1;
    foldTime = 0.0f;
    deliveryTime = 0.0f;
    stage = Stage::Preparing;
}

bool canHandOver() { return customerIsWaiting() && stage == Stage::Ready; }

float ease(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

void cube(float width, float height, float depth) {
    glPushMatrix();
    glScalef(width, height, depth);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawWrapChoices(bool picking) {
    if (!customerIsOrdering() || !orderIsPreparing()) return;
    const ShopPoint table = shopPackingTableTop();
    for (int i = 0; i < WRAP_MATERIAL_COUNT; ++i) {
        if (picking) glStencilFunc(GL_ALWAYS, FIRST_WRAP_TARGET + 1 + i, 0xff);
        glPushMatrix();
        glTranslatef(table.x + (i % 2 == 0 ? -0.13f : 0.13f), table.y + 0.025f,
                     table.z + (i < 2 ? 0.265f : 0.408f));
        if (i == paper || i == ribbon) {
            glColor3f(0.98f, 0.87f, 0.62f);
            cube(0.215f, 0.020f, 0.128f);
        }
        glTranslatef(0, 0.020f, 0);
        glColor3fv(colors[i]);
        if (i < 2) {
            cube(0.183f, 0.028f, 0.100f);
        } else {
            glRotatef(90, 1, 0, 0);
            glutSolidTorus(0.020, 0.041, 8, 16);
            cube(0.105f, 0.032f, 0.027f);
        }
        glPopMatrix();
    }
    if (picking) glStencilFunc(GL_ALWAYS, 0, 0xff);
}

void drawPaper(float folded) {
    const float bottom = 0.057f;
    const float top = 0.39f - 0.18f * folded;
    for (int i = 0; i < 8; ++i) {
        const float a = i * pi / 4.0f;
        const float b = (i + 1) * pi / 4.0f;
        const float shade = i % 2 == 0 ? 1.0f : 0.86f;
        glColor3f(colors[paper][0] * shade, colors[paper][1] * shade, colors[paper][2] * shade);
        glBegin(GL_QUAD_STRIP);
        glNormal3f(std::cos((a + b) / 2), 0.55f, std::sin((a + b) / 2));
        glVertex3f(bottom * std::cos(a), 0.035f, bottom * std::sin(a));
        glVertex3f(top * std::cos(a), 0.045f + folded * (i % 2 ? 0.23f : 0.34f), top * std::sin(a));
        glVertex3f(bottom * std::cos(b), 0.035f, bottom * std::sin(b));
        glVertex3f(top * std::cos(b), 0.045f + folded * (i % 2 ? 0.34f : 0.23f), top * std::sin(b));
        glEnd();
    }
}

void drawRibbon(float tightened) {
    if (tightened <= 0.0f) return;
    glColor3fv(colors[ribbon]);
    glPushMatrix();
    glTranslatef(0, 0.147f, 0);
    glRotatef(90, 1, 0, 0);
    glutSolidTorus(0.012, 0.114, 10, 28);
    glPopMatrix();
    for (float side : {-1.0f, 1.0f}) {
        glPushMatrix();
        glTranslatef(side * 0.055f, 0.15f, 0.116f);
        glRotatef(side * 21.0f, 0, 0, 1);
        glScalef(tightened * 1.25f, tightened * 0.60f, tightened);
        glutSolidTorus(0.012, 0.035, 9, 20);
        glPopMatrix();
    }
    glPushMatrix();
    glTranslatef(0, 0.15f, 0.120f);
    glutSolidSphere(0.021, 10, 8);
    glPopMatrix();
}
}

const FlowerOrder& orderRequested() { return currentRecipe().flowers; }
const FlowerOrder& orderPacked() { return packed; }
const char* orderOccasion() { return currentRecipe().occasion; }
const char* orderRequestLine() { return currentRecipe().request; }
const char* orderWrapRequest() { return currentRecipe().wrap; }
const float* wrapMaterialColor(WrapMaterial material) { return colors[static_cast<int>(material)]; }
bool orderUsingMaterial(WrapMaterial material) {
    const int id = static_cast<int>(material);
    return id == paper || id == ribbon;
}
int orderNumber() { return currentOrder; }
const char* orderMessage() { return message; }
bool orderIsPreparing() { return stage == Stage::Preparing; }
bool orderComplete() { return stage == Stage::Ready; }
bool orderDelivering() { return stage == Stage::Delivering; }
float orderDeliveryMotion() { return deliveryTime; }

void orderAddSelected() {
    if (inspectActive() || !customerIsOrdering() || !orderIsPreparing()) return;
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
    tablePots[flowerCount()] = {tulip ? FlowerType::TULIP : FlowerType::DAISY, 0.0f};
    ++count;
    staffPackFlower();
    if (hasFlowers()) message = "Du hoa. Chon giay, no roi bam Goi hoa.";
    else message = tulip ? "Da them mot chau tulip." : "Da them mot chau cuc kim.";
}

void orderClear() {
    if (!customerIsOrdering() || !orderIsPreparing()) return;
    clearBouquet();
    message = "Da dat lai don. Chon chau de soan lai.";
}

void orderChooseMaterial(WrapMaterial material) {
    if (!customerIsOrdering() || !orderIsPreparing()) return;
    const int id = static_cast<int>(material);
    if (id < 2) paper = id;
    else ribbon = id;
    message = hasFlowers() ? "Da chon mau. Bam Goi hoa khi san sang."
                           : "Da chon mau. Lay du hoa tren ke.";
}

void orderWrapBouquet() {
    if (!customerIsOrdering() || !orderIsPreparing()) return;
    if (!hasFlowers()) {
        message = "Chua du hoa theo don.";
        return;
    }
    if (paper < 0 || ribbon < 0) {
        message = "Chon ca giay va no tren ban.";
        return;
    }
    if (paper != static_cast<int>(currentRecipe().paper) ||
        ribbon != static_cast<int>(currentRecipe().ribbon)) {
        message = "Mau goi chua dung loi nhan cua khach.";
        return;
    }
    stage = Stage::Folding;
    staffPackFlower();
    message = "Nhan vien dang gap giay va that no.";
}

void orderNext() {
    if (!canHandOver()) return;
    stage = Stage::Delivering;
    deliveryTime = 0.0f;
    message = "Nhan vien mang bo hoa toi cho khach.";
}

void orderCancel(bool timedOut) {
    if (!customerIsOrdering() || !orderIsPreparing()) return;
    customerDismiss(timedOut);
    ++currentOrder;
    clearBouquet();
    message = timedOut ? "Het gio! Khach gian, bo ve." : "Da huy don va tien khach.";
}

bool orderDoorEnabled() {
    return customerCanEnter() || canHandOver() || (customerIsOrdering() && orderIsPreparing());
}

const char* orderDoorLabel() {
    if (customerCanEnter()) return "Goi khach vao";
    if (orderDelivering()) return "Dang giao hoa...";
    if (!customerIsOrdering()) return "Khach dang ra cua...";
    if (orderComplete()) return "Giao hoa";
    if (!orderIsPreparing()) return "Dang goi hoa...";
    return "Tien khach ra";
}

void orderDoorAction() {
    if (customerCanEnter()) {
        if (stage == Stage::Delivered) {
            ++currentOrder;
            clearBouquet();
        }
        customerEnter();
        message = "Khach moi dang di vao.";
    } else if (orderComplete()) orderNext();
    else orderCancel(false);
}

void updateOrderDisplay(float elapsed) {
    for (int i = 0; orderIsPreparing() && i < flowerCount(); ++i)
        tablePots[i].landing = std::min(1.0f, tablePots[i].landing + elapsed / 0.75f);
    if (stage == Stage::Folding) {
        foldTime = std::min(wrapDuration, foldTime + elapsed);
        if (foldTime >= wrapDuration) {
            stage = Stage::Ready;
            message = "Da goi xong. Bam Giao hoa.";
        }
    }
    if (stage == Stage::Delivering || stage == Stage::Delivered) {
        deliveryTime = std::min(4.4f, deliveryTime + elapsed);
        if (stage == Stage::Delivering && deliveryTime >= handoffDuration) {
            customerOrderReady();
            customerCollectOrder();
            stage = Stage::Delivered;
            message = "Khach da nhan bo hoa.";
        }
    }
}

void drawOrderBouquet() {
    if (paper < 0 || ribbon < 0) return;
    const float folded = ease(foldTime / wrapDuration);
    glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_CURRENT_BIT);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glDisable(GL_CULL_FACE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    drawPaper(folded);
    for (int i = 0; i < flowerCount(); ++i) {
        const float spread = 1.0f - 0.55f * folded;
        glPushMatrix();
        glTranslatef(offsets[i][0] * spread, 0.03f, offsets[i][1] * spread);
        glRotatef(-offsets[i][1] * 80.0f, 1, 0, 0);
        glRotatef(-offsets[i][0] * 125.0f, 0, 0, 1);
        glScalef(0.80f, 0.80f, 0.80f);
        drawFlower(tablePots[i].type, false);
        glPopMatrix();
    }
    drawRibbon(ease((folded - 0.6f) / 0.4f));
    glPopAttrib();
}

void drawOrderFlowers(bool picking) {
    const ShopPoint table = shopPackingTableTop();
    glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_CURRENT_BIT);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    if (customerIsOrdering() && orderIsPreparing() && paper >= 0) {
        glPushMatrix();
        glTranslatef(table.x, table.y, table.z - 0.035f);
        glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT);
        glDisable(GL_CULL_FACE);
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
        drawPaper(0.0f);
        glPopAttrib();
        glPopMatrix();
    }
    drawWrapChoices(picking);
    for (int i = 0; orderIsPreparing() && i < flowerCount(); ++i) {
        const TablePot& pot = tablePots[i];
        const float lift = (1.0f - pot.landing) * (1.0f - pot.landing);
        glPushMatrix();
        glTranslatef(table.x + offsets[i][0] + 0.15f * lift,
                     table.y + 0.44f * lift,
                     table.z + offsets[i][1] - 0.23f * lift);
        glScalef(0.49f, 0.49f, 0.49f);
        drawFlower(pot.type);
        glPopMatrix();
    }
    if (stage == Stage::Folding || stage == Stage::Ready || stage == Stage::Delivering) {
        const float walk = ease((deliveryTime - 0.36f) / 1.72f);
        const float give = ease((deliveryTime - 2.08f) / 0.52f);
        const float lift = ease(deliveryTime / 0.36f);
        glPushMatrix();
        glTranslatef(table.x + 0.37f * lift + 0.56f * walk - 0.27f * give,
                     table.y + 0.20f * lift - 0.15f * give,
                     table.z - 0.78f * lift - 2.05f * walk - 0.70f * give);
        drawOrderBouquet();
        glPopMatrix();
    }
    glPopAttrib();
}
