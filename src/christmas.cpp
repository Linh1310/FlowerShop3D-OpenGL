#include "christmas.h"
#include "flowers.h"

#include <GL/glut.h>

// ========================================
// 1. THIET LAP MAU SAC
// ========================================

namespace {

void setDecorColor(float r, float g, float b) {

    glColor3f(r, g, b);

    GLfloat diffuse[] = {
        r, g, b, 1.0f
    };

    GLfloat specular[] = {
        0.18f, 0.18f, 0.18f, 1.0f
    };

    glMaterialfv(
        GL_FRONT_AND_BACK,
        GL_AMBIENT_AND_DIFFUSE,
        diffuse
    );

    glMaterialfv(
        GL_FRONT_AND_BACK,
        GL_SPECULAR,
        specular
    );

    glMaterialf(
        GL_FRONT_AND_BACK,
        GL_SHININESS,
        20.0f
    );
}

// ========================================
// 2. VE MOT TANG LA THONG HINH NON
// ========================================

void drawTreeLayer(
    float radius,
    float height,
    float y,
    float r,
    float g,
    float b
) {

    setDecorColor(r, g, b);

    glPushMatrix();

    glTranslatef(0, y, 0);

    // Doi truc Z cua cone thanh truc Y
    glRotatef(-90.0f, 1, 0, 0);

    glutSolidCone(
        radius,
        height,
        28,
        6
    );

    glPopMatrix();
}

// ========================================
// 3. VE QUA CHAU NOEL
// ========================================

void drawBauble(
    float x,
    float y,
    float z,
    float radius,
    float r,
    float g,
    float b
) {

    setDecorColor(r, g, b);

    glPushMatrix();

    glTranslatef(x, y, z);

    glutSolidSphere(
        radius,
        14,
        12
    );

    glPopMatrix();
}

// ========================================
// 4. VE HOP QUA HINH LAP PHUONG
// ========================================

void drawGiftBox(
    float x,
    float z,
    float size,
    float bodyR,
    float bodyG,
    float bodyB,
    float ribbonR,
    float ribbonG,
    float ribbonB
) {

    glPushMatrix();

    // Dat hop tren mat san
    glTranslatef(
        x,
        size / 2.0f,
        z
    );

    // Than hop qua
    setDecorColor(bodyR, bodyG, bodyB);

    glPushMatrix();

    glScalef(size, size, size);

    glutSolidCube(1.0f);

    glPopMatrix();

    // Mau ru y bang
    setDecorColor(ribbonR, ribbonG, ribbonB);

    // Dai ru y bang tren mat hop
    glPushMatrix();

    glTranslatef(
        0,
        size / 2.0f + 0.006f,
        0
    );

    glScalef(
        size * 0.17f,
        0.012f,
        size * 1.02f
    );

    glutSolidCube(1.0f);

    glPopMatrix();

    glPushMatrix();

    glTranslatef(
        0,
        size / 2.0f + 0.006f,
        0
    );

    glScalef(
        size * 1.02f,
        0.012f,
        size * 0.17f
    );

    glutSolidCube(1.0f);

    glPopMatrix();

    // Dai ru y bang o mat truoc
    glPushMatrix();

    glTranslatef(
        0,
        0,
        size / 2.0f + 0.004f
    );

    glScalef(
        size * 0.17f,
        size,
        0.012f
    );

    glutSolidCube(1.0f);

    glPopMatrix();

    // Hai vong no nho tren hop qua
    glPushMatrix();

    glTranslatef(
        -size * 0.055f,
        size / 2.0f + 0.025f,
        0
    );

    glScalef(
        size * 0.10f,
        size * 0.045f,
        size * 0.07f
    );

    glutSolidSphere(1.0f, 12, 8);

    glPopMatrix();

    glPushMatrix();

    glTranslatef(
        size * 0.055f,
        size / 2.0f + 0.025f,
        0
    );

    glScalef(
        size * 0.10f,
        size * 0.045f,
        size * 0.07f
    );

    glutSolidSphere(1.0f, 12, 8);

    glPopMatrix();

    glPopMatrix();
}

} // namespace

// ========================================
// 5. VE TOAN BO GOC TRANG TRI NOEL
// ========================================

void drawChristmasDecor(float x, float z) {

    // Luu trang thai OpenGL
    glPushAttrib(
        GL_ENABLE_BIT |
        GL_LIGHTING_BIT |
        GL_CURRENT_BIT |
        GL_POLYGON_BIT
    );

    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);

    glColorMaterial(
        GL_FRONT_AND_BACK,
        GL_AMBIENT_AND_DIFFUSE
    );

    glPushMatrix();

    // Di chuyen den vi tri cay thong
    glTranslatef(x, 0, z);

    // Phong to toan bo goc trang tri Noel
    glScalef(1.5f, 1.5f, 1.5f);

    
    // ====================================
    // 5.1. CHAU CAY THONG
    // ====================================

    glPushMatrix();

    glScalef(
        1.25f,
        1.25f,
        1.25f
    );

    drawFlowerPot();

    glPopMatrix();

    // ====================================
    // 5.2. BON TANG TAN THONG
    // ====================================

    // Tang duoi
    drawTreeLayer(
        0.36f, 0.52f, 0.17f,
        0.08f, 0.32f, 0.16f
    );

    // Tang thu hai
    drawTreeLayer(
        0.29f, 0.46f, 0.46f,
        0.10f, 0.38f, 0.19f
    );

    // Tang thu ba
    drawTreeLayer(
        0.215f, 0.38f, 0.73f,
        0.12f, 0.42f, 0.21f
    );

    // Tang tren cung
    drawTreeLayer(
        0.145f, 0.32f, 0.98f,
        0.14f, 0.46f, 0.23f
    );

    // ====================================
    // 5.3. QUA CHAU DO VA VANG
    // ====================================

    // Tang duoi
    drawBauble(
        -0.12f, 0.32f, 0.20f,
        0.035f, 0.85f, 0.08f, 0.12f
    );

    drawBauble(
         0.12f, 0.32f, 0.20f,
        0.035f, 0.95f, 0.72f, 0.18f
    );

    drawBauble(
         0.0f, 0.43f, 0.16f,
        0.032f, 0.85f, 0.08f, 0.12f
    );

    // Tang thu hai
    drawBauble(
        -0.11f, 0.58f, 0.16f,
        0.033f, 0.95f, 0.72f, 0.18f
    );

    drawBauble(
         0.11f, 0.58f, 0.16f,
        0.033f, 0.85f, 0.08f, 0.12f
    );

    drawBauble(
         0.0f, 0.69f, 0.13f,
        0.030f, 0.95f, 0.72f, 0.18f
    );

    // Tang thu ba
    drawBauble(
        -0.07f, 0.84f, 0.13f,
        0.030f, 0.85f, 0.08f, 0.12f
    );

    drawBauble(
         0.07f, 0.84f, 0.13f,
        0.030f, 0.95f, 0.72f, 0.18f
    );

    // Tang tren cung
    drawBauble(
        0.0f, 1.10f, 0.085f,
        0.026f, 0.85f, 0.08f, 0.12f
    );

    // ====================================
    // 5.4. NGoi SAO TREN DINH CAY
    // ====================================

    setDecorColor(
        0.95f, 0.75f, 0.20f
    );

    glPushMatrix();

    glTranslatef(
        0,
        1.36f,
        0
    );

    glScalef(
        0.08f,
        0.11f,
        0.055f
    );

    glutSolidOctahedron();

    glPopMatrix();

    // ====================================
    // 5.5. BON HOP QUA DUOI GOC CAY
    // ====================================

    // Hop 1: Do, ru y bang vang
    drawGiftBox(
        -0.29f, 0.00f, 0.18f,
        0.75f, 0.09f, 0.13f,
        0.95f, 0.75f, 0.20f
    );

    // Hop 2: Kem, ru y bang do
    drawGiftBox(
         0.29f, 0.02f, 0.20f,
        0.94f, 0.84f, 0.66f,
        0.78f, 0.10f, 0.13f
    );

    // Hop 3: Vang, ru y bang xanh
    drawGiftBox(
        -0.22f, 0.30f, 0.16f,
        0.91f, 0.68f, 0.16f,
        0.10f, 0.34f, 0.17f
    );

    // Hop 4: Xanh, ru y bang vang
    drawGiftBox(
         0.22f, 0.31f, 0.17f,
        0.10f, 0.34f, 0.17f,
        0.95f, 0.75f, 0.20f
    );

    glPopMatrix();

    // Khoi phuc trang thai OpenGL
    glPopAttrib();
}