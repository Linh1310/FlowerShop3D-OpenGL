#include <GL/freeglut.h>
#include "lighting.h"
#include "shop.h"

static bool lightingOn = true;
static bool smoothOn   = true;
static bool nightOn    = false;

static int lampCount() {
    int n = shopLampCount();
    return (n > 7) ? 7 : n;          // OpenGL cố định chỉ có GL_LIGHT0..7
}

// Đặt màu nền trời, ánh sáng chung và màu đèn thả trần theo ngày/đêm
static void applyDayNight() {
    GLfloat amb[4], sun[4], lamp[4];
    if (nightOn) {
        glClearColor(0.04f, 0.05f, 0.12f, 1.0f);
        GLfloat a[] = {0.06f, 0.06f, 0.12f, 1.0f};
        GLfloat s[] = {0.05f, 0.06f, 0.14f, 1.0f};
        GLfloat l[] = {1.00f, 0.82f, 0.50f, 1.0f};
        for (int i = 0; i < 4; i++) { amb[i] = a[i]; sun[i] = s[i]; lamp[i] = l[i]; }
    } else {
        glClearColor(0.75f, 0.85f, 0.95f, 1.0f);
        GLfloat a[] = {0.25f, 0.25f, 0.25f, 1.0f};
        GLfloat s[] = {0.45f, 0.45f, 0.42f, 1.0f};
        GLfloat l[] = {0.80f, 0.70f, 0.50f, 1.0f};
        for (int i = 0; i < 4; i++) { amb[i] = a[i]; sun[i] = s[i]; lamp[i] = l[i]; }
    }
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sun);
    for (int i = 0; i < lampCount(); i++)
        glLightfv(GL_LIGHT1 + i, GL_DIFFUSE, lamp);
}

void initLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_LIGHT0);                       // ánh sáng chung (cửa sổ)

    GLfloat lampAmb[] = {0.0f, 0.0f, 0.0f, 1.0f};
    for (int i = 0; i < lampCount(); i++) {    // mỗi đèn thả trần = 1 nguồn sáng
        GLenum L = GL_LIGHT1 + i;
        glLightfv(L, GL_AMBIENT, lampAmb);
        glLightf(L, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(L, GL_LINEAR_ATTENUATION,   0.10f);
        glEnable(L);
    }
    applyDayNight();
}

void applyLighting() {
    // Vị trí đèn phải đặt sau gluLookAt thì mới đứng yên trong thế giới
    GLfloat pos0[] = {0.3f, 1.0f, 0.5f, 0.0f};       // w = 0: đèn hướng
    glLightfv(GL_LIGHT0, GL_POSITION, pos0);

    for (int i = 0; i < lampCount(); i++) {
        ShopPoint p = shopLampPos(i);
        GLfloat pos[] = {p.x, p.y, p.z, 1.0f};       // w = 1: đèn điểm
        glLightfv(GL_LIGHT1 + i, GL_POSITION, pos);
    }
}

void toggleLighting() {
    lightingOn = !lightingOn;
    if (lightingOn) glEnable(GL_LIGHTING); else glDisable(GL_LIGHTING);
}

void toggleShading() {
    smoothOn = !smoothOn;
    glShadeModel(smoothOn ? GL_SMOOTH : GL_FLAT);
}

void toggleDayNight() {
    nightOn = !nightOn;
    applyDayNight();
}

void applySkyColor() {
    if (nightOn) glClearColor(0.04f, 0.05f, 0.12f, 1.0f);
    else         glClearColor(0.75f, 0.85f, 0.95f, 1.0f);
}

bool isLightingOn() { return lightingOn; }
bool isSmoothOn()   { return smoothOn; }
bool isNightMode()  { return nightOn; }