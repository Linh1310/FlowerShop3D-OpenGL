#include <GL/freeglut.h>
#include <cmath>
#include "camera.h"

static const float DEF_YAW = 30.0f, DEF_PITCH = 25.0f, DEF_DIST = 11.0f;
static float yaw = DEF_YAW, pitch = DEF_PITCH, dist = DEF_DIST;
static int   projMode = 0;              // 0 phoi canh | 1 truc giao | 2 xien
static int   winW = 1000, winH = 700;

static void updateProjection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)winW / (float)winH;

    if (projMode == 0) {
        gluPerspective(55.0, aspect, 0.1, 100.0);       // chieu phoi canh
    } else {
        float h = dist * 0.5f;
        glOrtho(-h * aspect, h * aspect, -h, h, 0.1, 100.0);   // chieu truc giao
        if (projMode == 2) {
            // Chieu xien (cabinet): tia chieu nghieng, L = cot(alpha) = 0.5, phi = 45 do
            // x' = x + L*cos(phi)*(do sau - dist),  y' = y + L*sin(phi)*(do sau - dist)
            const float L = 0.5f, phi = 45.0f * 3.14159265f / 180.0f;
            float sx = -L * cosf(phi), sy = -L * sinf(phi);
            GLfloat shear[16] = {
                1, 0, 0, 0,
                0, 1, 0, 0,
                sx, sy, 1, 0,
                sx * dist, sy * dist, 0, 1
            };
            glMultMatrixf(shear);
        }
    }
    glMatrixMode(GL_MODELVIEW);
}

void cameraInit() { cameraReset(); }

void cameraReshape(int w, int h) {
    winW = w;
    winH = (h > 0) ? h : 1;
    glViewport(0, 0, winW, winH);
    updateProjection();
}

void applyCamera() {
    const float D2R = 3.14159265f / 180.0f;
    float ry = yaw * D2R, rp = pitch * D2R;
    float ex = dist * cosf(rp) * sinf(ry);
    float ey = 1.2f + dist * sinf(rp);
    float ez = dist * cosf(rp) * cosf(ry);
    gluLookAt(ex, ey, ez,   0.0, 1.2, 0.0,   0.0, 1.0, 0.0);
}

void cameraRotate(float dYaw, float dPitch) {
    yaw += dYaw;
    while (yaw >= 360.0f) yaw -= 360.0f;
    while (yaw <    0.0f) yaw += 360.0f;
    pitch += dPitch;
    if (pitch > 85.0f) pitch = 85.0f;
    if (pitch < 3.0f)  pitch = 3.0f;
}

void cameraZoom(float delta) {
    dist += delta * (dist / 11.0f);          // gan thi buoc nho, xa thi buoc lon
    if (dist < 1.0f)  dist = 1.0f;
    if (dist > 25.0f) dist = 25.0f;
    if (projMode != 0) updateProjection();
}

void cameraSetDistance(float d) {
    dist = d;
    if (dist < 1.0f)  dist = 1.0f;
    if (dist > 25.0f) dist = 25.0f;
    if (projMode != 0) updateProjection();
}

void cameraReset() {
    yaw = DEF_YAW; pitch = DEF_PITCH; dist = DEF_DIST;
    projMode = 0;
    updateProjection();
}

void cameraNextProjection() { projMode = (projMode + 1) % 3; updateProjection(); }
void cameraTogglePerspective() { cameraNextProjection(); }

bool  isPerspective()   { return projMode == 0; }
float cameraYaw()       { return yaw; }
float cameraPitch()     { return pitch; }
float cameraDistance()  { return dist; }

const char* cameraProjectionName() {
    static const char* n[] = { "Phoi canh", "Truc giao", "Xien (cabinet)" };
    return n[projMode];
}
const char* cameraProjectionFormula() {
    static const char* f[] = {
        "x' = x0*D/z0 ; y' = y0*D/z0",
        "x' = x0 ; y' = y0",
        "x' = x0 + 0.5*z0*cos45 ; y' = y0 + 0.5*z0*sin45"
    };
    return f[projMode];
}