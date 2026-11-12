#include <GL/freeglut.h>
#include <cstdlib>
#include "ui.h"
#include "camera.h"
#include "lighting.h"
#include "shop.h"
#include "scene.h"   // bật khi Linh thêm sceneSelectPot / sceneRotateSelected
#include "model3d.h"
#include "interaction.h"
#include <cstdio>

struct Button {
    int x, y, w, h;
    const char* label;
    void (*onClick)();
};

static const int MAX_BTN = 32;
static Button buttons[MAX_BTN];
static int btnCount = 0;
static int hoverIdx = -1;
static int winW = 1000, winH = 700;

static void addBtn(int col, int row, const char* label, void (*fn)()) {
    const int W = 100, H = 28, GAP = 6, X0 = 10, Y0 = 10;
    Button b;
    b.x = X0 + col * (W + GAP);
    b.y = Y0 + row * (H + GAP);
    b.w = W; b.h = H;
    b.label = label; b.onClick = fn;
    buttons[btnCount++] = b;
}

static bool showInfo = true;

static void buildButtons() {
    if (btnCount > 0) return;
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
}

void uiReshape(int w, int h) { winW = w; winH = (h > 0) ? h : 1; }

static void drawText(int x, int y, const char* s) {
    glRasterPos2i(x, y);
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)s);
}

static void drawInfoPanel() {
    char line[128];
    const int W = 300, H = 118;
    int x = winW - W - 10, y = 10;

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

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);   // tự khôi phục đèn, depth, màu
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

    for (int i = 0; i < btnCount; i++) {
        const Button& b = buttons[i];

        if (i == hoverIdx) glColor4f(0.95f, 0.55f, 0.65f, 0.90f);
        else               glColor4f(0.15f, 0.15f, 0.20f, 0.75f);
        glBegin(GL_QUADS);
            glVertex2i(b.x, b.y);
            glVertex2i(b.x + b.w, b.y);
            glVertex2i(b.x + b.w, b.y + b.h);
            glVertex2i(b.x, b.y + b.h);
        glEnd();

        glColor3f(1, 1, 1);                          // viền
        glBegin(GL_LINE_LOOP);
            glVertex2i(b.x, b.y);
            glVertex2i(b.x + b.w, b.y);
            glVertex2i(b.x + b.w, b.y + b.h);
            glVertex2i(b.x, b.y + b.h);
        glEnd();

        const unsigned char* t = (const unsigned char*)b.label;
        int tw = glutBitmapLength(GLUT_BITMAP_HELVETICA_12, t);
        glRasterPos2i(b.x + (b.w - tw) / 2, b.y + b.h / 2 + 5);
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
    for (int i = 0; i < btnCount; i++) {
        const Button& b = buttons[i];
        if (x >= b.x && x <= b.x + b.w && y >= b.y && y <= b.y + b.h) return i;
    }
    return -1;
}

void onMouse(int button, int state, int x, int y) {
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;
    int i = hitTest(x, y);
    if (i >= 0) {
        buttons[i].onClick();
        glutPostRedisplay();
    }
}

void onMouseMove(int x, int y) {
    int i = hitTest(x, y);
    if (i != hoverIdx) { hoverIdx = i; glutPostRedisplay(); }
}