#include <GL/freeglut.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include "ui.h"
#include "camera.h"
#include "lighting.h"
#include "shop.h"
#include "scene.h"   // bật khi Linh thêm sceneSelectPot / sceneRotateSelected
#include "model3d.h"
#include "interaction.h"
#include "order.h"
#include "people.h"
#include <cstdio>

struct Button {
    int x, y, w, h;
    const char* label;
    void (*onClick)();
    int material = -1;
};

static const int MAX_BTN = 40;
static Button buttons[MAX_BTN];
static int btnCount = 0;
static int hoverIdx = -1;
static int winW = 1000, winH = 700;
static float uiScale = 1.0f;
static constexpr int ORDER_W = 264;
static constexpr int ORDER_H = 453;
static int orderButtonStart = 0;
static bool controlsOpen = false;
static bool sceneMouseHeld = false;
static bool orbiting = false;
static int lastMouseX = 0;
static int lastMouseY = 0;

static void toggleControls() {
    controlsOpen = !controlsOpen;
    hoverIdx = -1;
}

static void addBtn(int col, int row, const char* label, void (*fn)()) {
    buttons[btnCount++] = {10 + col * 106, 44 + row * 34, 100, 28, label, fn};
}

static bool showInfo = true;

static int orderX() { return winW - ORDER_W - 10; }

static int buttonX(int i) {
    return buttons[i].x + (i >= orderButtonStart ? orderX() : 0);
}

static void buildButtons() {
    if (btnCount > 0) return;
    buttons[btnCount++] = {10, 10, 131, 28, nullptr, toggleControls};
    addBtn(0, 0, "< Trai",        [] { cameraRotate(-5, 0); });
    addBtn(1, 0, "Phai >",        [] { cameraRotate(+5, 0); });
    addBtn(0, 1, "^ Len",         [] { cameraRotate(0, +5); });
    addBtn(1, 1, "v Xuong",       [] { cameraRotate(0, -5); });
    addBtn(0, 2, "Zoom +",        [] { cameraZoom(-1); });
    addBtn(1, 2, "Zoom -",        [] { cameraZoom(+1); });
    addBtn(0, 3, "Reset",         [] { cameraReset(); sceneResetSelection(); });
    addBtn(1, 3, "Doi phep chieu", [] { cameraNextProjection(); });   // thay cho dong "Persp/Ortho"
    addBtn(0, 4, "Den On/Off",    [] { toggleLighting(); });
    addBtn(1, 4, "Flat/Smooth",   [] { toggleShading(); });
    addBtn(0, 5, "Mo/Dong cua",   [] { shopToggleDoor(); });
    addBtn(1, 5, "An tuong truoc",[] { shopToggleFrontWall(); });
    addBtn(0, 6, "< Chau truoc",  [] { sceneSelectStep(-1); });
    addBtn(1, 6, "Chau sau >",    [] { sceneSelectStep(+1); });
    addBtn(0, 7, "Xoay hoa <",    [] { sceneRotateSelected(-10); });
    addBtn(1, 7, "Xoay hoa >",    [] { sceneRotateSelected(+10); });
    addBtn(0, 8, "Phong to hoa",  [] { sceneScaleSelected(+0.1f); });
    addBtn(1, 8, "Thu nho hoa",   [] { sceneScaleSelected(-0.1f); });
    addBtn(0, 9, "Bo chon",       [] { sceneResetSelection(); });
    addBtn(1, 9, "Thoat",         [] { exit(0); });
    addBtn(0, 11, "Khung/Mat",     [] { cycleInspectMode(); });
    addBtn(1, 11, "Nhan dinh",     [] { inspectToggleLabels(); });
    addBtn(0, 10, "Ngay/Dem",     [] { toggleDayNight(); });
    addBtn(1, 10, "An/Hien info", [] { showInfo = !showInfo; });
    addBtn(0, 12, "Nguon hoa", [] { inspectToggleSource(); });
    orderButtonStart = btnCount;
    buttons[btnCount++] = {14, 261, 147, 28, "Them hoa", orderAddSelected};
    buttons[btnCount++] = {167, 261, 83, 28, "Dat lai", orderClear};
    buttons[btnCount++] = {14, 301, 115, 25, "Giay hong", [] { orderChooseMaterial(WrapMaterial::Pink); }, 0};
    buttons[btnCount++] = {135, 301, 115, 25, "Giay kraft", [] { orderChooseMaterial(WrapMaterial::Kraft); }, 1};
    buttons[btnCount++] = {14, 331, 115, 25, "No kem", [] { orderChooseMaterial(WrapMaterial::Cream); }, 2};
    buttons[btnCount++] = {135, 331, 115, 25, "No xanh", [] { orderChooseMaterial(WrapMaterial::Green); }, 3};
    buttons[btnCount++] = {14, 367, 236, 29, "Goi hoa", orderWrapBouquet};
    buttons[btnCount++] = {14, 403, 236, 29, nullptr, orderDoorAction};
}

void uiReshape(int w, int h) {
    w = std::max(w, 1);
    h = std::max(h, 1);
    uiScale = std::min({1.0f, w / 720.0f, h / 610.0f});
    winW = static_cast<int>(w / uiScale);
    winH = static_cast<int>(h / uiScale);
    hoverIdx = -1;
}

static void drawText(int x, int y, const char* s, void* font = GLUT_BITMAP_HELVETICA_12) {
    glRasterPos2i(x, y);
    glutBitmapString(font, (const unsigned char*)s);
}

static void fillRect(int x, int y, int w, int h) {
    glBegin(GL_QUADS);
        glVertex2i(x, y);     glVertex2i(x + w, y);
        glVertex2i(x + w, y + h); glVertex2i(x, y + h);
    glEnd();
}

static void drawOrderPanel() {
    const int x = orderX();
    char line[128];
    const FlowerOrder& requested = orderRequested();
    const FlowerOrder& packed = orderPacked();
    glColor4f(0.07f, 0.17f, 0.14f, 0.96f);
    fillRect(x, 10, ORDER_W, ORDER_H);
    glColor3f(0.83f, 0.59f, 0.47f);
    fillRect(x, 10, ORDER_W, 4);
    glColor3f(1.0f, 0.94f, 0.83f);
    snprintf(line, sizeof line, "Soan don hoa #%d", orderNumber());
    drawText(x + 14, 42, line, GLUT_BITMAP_HELVETICA_18);
    glColor3f(1.0f, 0.65f, 0.70f);
    snprintf(line, sizeof line, "Tim: %d", customerHearts());
    drawText(x + 198, 41, line);

    glColor3f(0.15f, 0.27f, 0.22f);
    fillRect(x + 14, 53, 236, 63);
    glColor3f(0.99f, 0.82f, 0.68f);
    drawText(x + 25, 71, orderOccasion());
    glColor3f(0.94f, 0.92f, 0.85f);
    drawText(x + 25, 89, orderRequestLine());
    drawText(x + 25, 106, orderWrapRequest());

    glColor3f(0.15f, 0.27f, 0.22f);
    fillRect(x + 14, 122, 236, 62);
    glColor3f(0.81f, 0.87f, 0.79f);
    drawText(x + 26, 137, "Hoa khach dat");
    drawText(x + 176, 137, "Da chon");
    glColor3f(0.98f, 0.77f, 0.79f);
    drawText(x + 26, 156, "Tulip hong");
    snprintf(line, sizeof line, "%d / %d", packed.tulips, requested.tulips);
    drawText(x + 187, 156, line);
    glColor3f(1.0f, 0.94f, 0.83f);
    drawText(x + 26, 175, "Cuc kim");
    snprintf(line, sizeof line, "%d / %d", packed.daisies, requested.daisies);
    drawText(x + 187, 175, line);

    const int selected = sceneSelectedIndex();
    if (selected < 0) snprintf(line, sizeof line, "Bam vao chau hoa tren ke");
    else snprintf(line, sizeof line, "Chau %d / %d - %s",
                  selected + 1, scenePotCount(),
                  sceneSelectedFlowerType() == 0 ? "Tulip hong" : "Cuc kim");
    drawText(x + 14, 201, line);
    glColor3f(0.77f, 0.85f, 0.76f);
    drawText(x + 14, 222, orderMessage());
    if (orderComplete()) {
        drawText(x + 14, 244, customerIsWaiting() ? "Khach da san sang nhan hoa."
                                                 : "Cho khach toi quay de giao hoa.");
    } else if (customerIsWaiting() && orderIsPreparing()) {
        const float remaining = CUSTOMER_WAIT_LIMIT - customerWaitElapsed();
        snprintf(line, sizeof line, "Khach cho %d giay", static_cast<int>(std::ceil(remaining)));
        if (remaining < 10.0f) glColor3f(1.0f, 0.59f, 0.54f);
        drawText(x + 14, 244, line);
        glColor3f(0.20f, 0.31f, 0.27f);
        fillRect(x + 14, 250, 236, 5);
        if (remaining < 10.0f) glColor3f(0.92f, 0.36f, 0.29f);
        else glColor3f(0.59f, 0.78f, 0.51f);
        fillRect(x + 14, 250, static_cast<int>(236 * remaining / CUSTOMER_WAIT_LIMIT), 5);
    } else if (customerIsOrdering() && !customerIsWaiting()) {
        drawText(x + 14, 244, "Khach dang di vao.");
    } else if (customerCanEnter()) {
        drawText(x + 14, 244, "Ban co the goi khach moi.");
    }
    glColor3f(0.77f, 0.85f, 0.76f);
    drawText(x + 14, 450, "Chuot phai them hoa. Bam giay/no tren ban.");
}

static bool buttonEnabled(int i) {
    if (i < orderButtonStart) return i == 0 || controlsOpen;
    if (inspectActive()) return false;
    if (buttons[i].onClick == orderDoorAction) return orderDoorEnabled();
    return customerIsOrdering() && orderIsPreparing();
}

static void drawInfoPanel() {
    char line[128];
    const int W = 300, H = 118;
    int x = winW - W - 10, y = winH - H - 10;

    glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
    glBegin(GL_QUADS);
        glVertex2i(x, y);         glVertex2i(x + W, y);
        glVertex2i(x + W, y + H); glVertex2i(x, y + H);
    glEnd();

    glColor3f(1, 1, 1);
    drawText(x + 10, y + 20, "THONG TIN");

    snprintf(line, sizeof line, "Den: %s | Shading: %s",
             isLightingOn() ? "BAT" : "TAT",
             isSmoothOn() ? "Smooth (Gouraud)" : "Flat");
    drawText(x + 10, y + 40, line);

    snprintf(line, sizeof line, "Camera: %s | Moi truong: %s",
             cameraProjectionName(),
             isNightMode() ? "Dem" : "Ngay");
    drawText(x + 10, y + 60, line);

    if (sceneSelectedIndex() >= 0) {
        snprintf(line, sizeof line, "Chau dang chon: %d / %d",
                 sceneSelectedIndex() + 1, scenePotCount());
        drawText(x + 10, y + 80, line);
        drawText(x + 10, y + 100, sceneSelectedName());
    } else {
        drawText(x + 10, y + 80, "Chua chon chau (bam Chau sau >)");
    }
}

static void drawInspectPanel() {
    char line[160];
    const int W = 480, H = 138;
    int x = 10, y = winH - H - 10;

    glColor4f(0.0f, 0.0f, 0.0f, 0.65f);
    glBegin(GL_QUADS);
        glVertex2i(x, y);         glVertex2i(x + W, y);
        glVertex2i(x + W, y + H); glVertex2i(x, y + H);
    glEnd();

    glColor3f(1.0f, 0.9f, 0.4f);
    drawText(x + 10, y + 20, "MO HINH 3D (CG.4)");

    glColor3f(1, 1, 1);
        snprintf(line, sizeof line, "Che do: %s | Nguon: %s", inspectModeName(),
             inspectUsingReal() ? "hoa that (Linh)" : "mo hinh danh sach");
    drawText(x + 10, y + 40, line);
    if (inspectUsingReal())
        snprintf(line, sizeof line, "Hoa that: ve bang glutSolid + Bezier, khong luu danh sach");
    else
        snprintf(line, sizeof line, "Dinh: %d | Canh: %d | Mat: %d",
                 inspectNumVertex(), inspectNumEdge(), inspectNumFace());
    drawText(x + 10, y + 60, line);
    snprintf(line, sizeof line, "Bien doi: Ti le (x2.5) -> Quay Y (%.0f do) -> Tinh tien",
             sceneSelectedAngle());
    drawText(x + 10, y + 80, line);
    snprintf(line, sizeof line, "Chieu %s: %s", cameraProjectionName(), cameraProjectionFormula());
    drawText(x + 10, y + 100, line);
    snprintf(line, sizeof line, "Quan sat: theta=%.0f  phi=%.0f  R=%.1f",
             cameraYaw(), cameraPitch(), cameraDistance());
    drawText(x + 10, y + 120, line);
}

void drawUI() {
    buildButtons();

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Chuyển sang hệ tọa độ 2D theo pixel, gốc ở góc trên trái (khớp với chuột)
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, winW, winH, 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    if (!inspectActive()) drawOrderPanel();
    if (controlsOpen) {
        glColor4f(0.08f, 0.15f, 0.13f, 0.94f);
        fillRect(6, 6, 214, 478);
    }
    for (int i = 0; i < btnCount; i++) {
        const bool orderButton = i >= orderButtonStart;
        if (orderButton && inspectActive()) continue;
        if (!orderButton && i != 0 && !controlsOpen) continue;
        const Button& b = buttons[i];
        const int x = buttonX(i);
        const bool enabled = buttonEnabled(i);
        const bool selectedMaterial = b.material >= 0 &&
                                      orderUsingMaterial(static_cast<WrapMaterial>(b.material));

        if (!enabled) glColor3f(0.14f, 0.23f, 0.20f);
        else if (orderButton && (i == hoverIdx || b.onClick == orderAddSelected ||
                                  b.onClick == orderWrapBouquet ||
                                  (b.onClick == orderDoorAction &&
                                   (orderComplete() || customerCanEnter()))))
            glColor3f(0.69f, 0.41f, 0.34f);
        else if (orderButton) glColor3f(0.21f, 0.35f, 0.29f);
        else if (i == hoverIdx) glColor4f(0.95f, 0.55f, 0.65f, 0.90f);
        else               glColor4f(0.15f, 0.15f, 0.20f, 0.75f);
        fillRect(x, b.y, b.w, b.h);
        if (b.material >= 0) {
            glColor3fv(wrapMaterialColor(static_cast<WrapMaterial>(b.material)));
            fillRect(x + 9, b.y + 7, 12, 11);
        }

        if (selectedMaterial) glColor3f(1.0f, 0.87f, 0.60f);
        else if (enabled) glColor3f(1, 1, 1);
        else glColor3f(0.53f, 0.63f, 0.57f);
        if (!orderButton || selectedMaterial) {
            glBegin(GL_LINE_LOOP);
                glVertex2i(x, b.y);
                glVertex2i(x + b.w, b.y);
                glVertex2i(x + b.w, b.y + b.h);
                glVertex2i(x, b.y + b.h);
            glEnd();
        }

        const char* label = b.label;
        if (i == 0) label = controlsOpen ? "-  Dong dieu khien" : "+  Dieu khien";
        if (b.onClick == orderDoorAction) label = orderDoorLabel();
        const unsigned char* t = (const unsigned char*)label;
        int tw = glutBitmapLength(GLUT_BITMAP_HELVETICA_12, t);
        glRasterPos2i(x + (b.w - tw) / 2, b.y + b.h / 2 + 5);
        glutBitmapString(GLUT_BITMAP_HELVETICA_12, t);
    }

    if (showInfo) drawInfoPanel();
    if (inspectActive()) drawInspectPanel();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopAttrib();
}

static int hitTest(int x, int y) {
    x = static_cast<int>(x / uiScale);
    y = static_cast<int>(y / uiScale);
    for (int i = 0; i < btnCount; i++) {
        const Button& b = buttons[i];
        const int left = buttonX(i);
        if (buttonEnabled(i) && x >= left && x < left + b.w &&
            y >= b.y && y < b.y + b.h) return i;
    }
    return -1;
}

static bool overPanel(int x, int y) {
    x = static_cast<int>(x / uiScale);
    y = static_cast<int>(y / uiScale);
    const bool orderPanel = !inspectActive() && x >= orderX() && x < winW - 10 &&
                            y >= 10 && y < 10 + ORDER_H;
    const bool infoPanel = showInfo && x >= winW - 310 && x < winW - 10 &&
                           y >= winH - 128 && y < winH - 10;
    const bool controlsPanel = controlsOpen && x >= 6 && x < 220 && y >= 6 && y < 484;
    return orderPanel || infoPanel || controlsPanel;
}

static void pickScene(int x, int y, bool add) {
    if (inspectActive() || overPanel(x, y)) return;
    const int target = sceneTargetAtPixel(x, y);
    if (target < 0) return;
    if (target >= FIRST_WRAP_TARGET) {
        const int material = target - FIRST_WRAP_TARGET;
        if (material < WRAP_MATERIAL_COUNT)
            orderChooseMaterial(static_cast<WrapMaterial>(material));
    } else {
        sceneSelectPot(target);
        if (add) orderAddSelected();
    }
    glutPostRedisplay();
}

void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN && !sceneMouseHeld) {
        if (hitTest(x, y) < 0) pickScene(x, y, true);
        return;
    }
    if (button != GLUT_LEFT_BUTTON) return;
    if (state == GLUT_DOWN) {
        sceneMouseHeld = false;
        orbiting = false;
        const int control = hitTest(x, y);
        if (control >= 0) {
            buttons[control].onClick();
            glutPostRedisplay();
        } else if (!overPanel(x, y)) {
            sceneMouseHeld = true;
            lastMouseX = x;
            lastMouseY = y;
        }
    } else if (state == GLUT_UP && sceneMouseHeld) {
        onMouseDrag(x, y);
        sceneMouseHeld = false;
        if (!orbiting && hitTest(x, y) < 0) pickScene(x, y, false);
    }
}

void onMouseDrag(int x, int y) {
    if (!sceneMouseHeld) return;
    const int dx = x - lastMouseX;
    const int dy = y - lastMouseY;
    if (!orbiting && std::abs(dx) < 5 && std::abs(dy) < 5) return;
    orbiting = true;
    lastMouseX = x;
    lastMouseY = y;
    hoverIdx = -1;
    constexpr float degreesPerPixel = 0.28f;
    cameraRotate(-dx * degreesPerPixel, dy * degreesPerPixel);
    glutPostRedisplay();
}

void onMouseMove(int x, int y) {
    int i = hitTest(x, y);
    if (i != hoverIdx) { hoverIdx = i; glutPostRedisplay(); }
}

void onMouseWheel(int, int direction, int, int) {
    cameraZoom(direction > 0 ? -1.0f : 1.0f);
    glutPostRedisplay();
}
