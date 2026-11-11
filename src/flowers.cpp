
#include "flowers.h"
#include "bezier.h"
#include <GL/glut.h>
#include <algorithm>
#include <cmath>
#include <cstdio>

// ========================================
// 1. KHAI BAO THONG SO VA DU LIEU
// ========================================

namespace {

using flower3d::Vec3;
using flower3d::PetalProfile;
using flower3d::cubicBezier;
using flower3d::drawBezierPetal;

const float PI = 3.14159265358979323846f;

// ========================================
// 2. CAC HAM TINH TOAN VECTOR 3D
// ========================================

// Cong hai vector
Vec3 add(Vec3 a, Vec3 b) {
    return {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    };
}

// Tru hai vector
Vec3 sub(Vec3 a, Vec3 b) {
    return {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
}

// Nhan vector voi mot so
Vec3 mul(Vec3 a, float s) {
    return {
        a.x * s,
        a.y * s,
        a.z * s
    };
}

// Tinh tich co huong
Vec3 cross(Vec3 a, Vec3 b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

// Chuan hoa vector
Vec3 unit(Vec3 a) {
    float length = std::sqrt(
        a.x * a.x + a.y * a.y + a.z * a.z
    );

    if (length > 1e-7f) {
        return mul(a, 1.0f / length);
    }

    return {0, 0, 1};
}

// ========================================
// 3. THIET LAP MAU SAC VA VAT LIEU
// ========================================


void setMaterial(float r, float g, float b) {

    // Mau goc cua doi tuong
    glColor3f(r, g, b);

    // Mau vat lieu khi co anh sang
    GLfloat diffuse[] = {
        r, g, b, 1.0f
    };

    // Anh sang phan chieu tren be mat
    GLfloat specular[] = {
        0.10f, 0.10f, 0.10f, 1.0f
    };

    // Thanh phan mau khong phu thuoc anh sang
    GLfloat emission[] = {
        r * 0.25f,
        g * 0.25f,
        b * 0.25f,
        1.0f
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

    glMaterialfv(
        GL_FRONT_AND_BACK,
        GL_EMISSION,
        emission
    );

    glMaterialf(
        GL_FRONT_AND_BACK,
        GL_SHININESS,
        12.0f
    );
}


// ========================================
// 4. CAC HAM VE HINH HOC CO BAN
// ========================================

// Tao GLU quadric
GLUquadric* quad() {
    static GLUquadric* q = nullptr;

    if (!q) {
        q = gluNewQuadric();
        gluQuadricNormals(q, GLU_SMOOTH);
    }

    return q;
}

// Ve hinh tru theo truc Y
void cylinderY(float r0, float r1, float h, float y) {
    glPushMatrix();

    glTranslatef(0, y, 0);
    glRotatef(-90, 1, 0, 0);

    gluCylinder(quad(), r0, r1, h, 24, 1);

    glPopMatrix();
}

// Ve mat tron nam ngang
void diskY(float radius, float y) {
    glPushMatrix();

    glTranslatef(0, y, 0);
    glRotatef(-90, 1, 0, 0);

    gluDisk(quad(), 0, radius, 24, 1);

    glPopMatrix();
}

// Ve hinh cau keo dan
void ellipsoid(float rx, float ry, float rz) {
    glPushMatrix();

    glScalef(rx, ry, rz);
    glutSolidSphere(1.0, 18, 14);

    glPopMatrix();
}

// ========================================
// 5. VE THAN HOA CONG BANG BEZIER
// ========================================

void drawCurvedStem(float top, float bend, float radius) {
    Vec3 p0 = {0, 0.14f, 0};
    Vec3 p1 = {-bend, 0.22f, 0.004f};
    Vec3 p2 = {bend, top - 0.08f, -0.012f};
    Vec3 p3 = {0, top, 0};

    // Mau xanh cua than
    setMaterial(0.13f, 0.42f, 0.18f);

    const int N = 18;
    const int S = 12;

    for (int i = 0; i < N; i++) {
        glBegin(GL_QUAD_STRIP);

        for (int j = 0; j <= S; j++) {
            float angle = 2 * PI * j / S;

            for (int k = 0; k <= 1; k++) {
                float t = float(i + k) / N;

                Vec3 center = cubicBezier(
                    p0, p1, p2, p3, t
                );

                Vec3 tangent = unit(sub(
                    cubicBezier(
                        p0, p1, p2, p3,
                        std::min(1.0f, t + 0.006f)
                    ),
                    cubicBezier(
                        p0, p1, p2, p3,
                        std::max(0.0f, t - 0.006f)
                    )
                ));

                Vec3 axisX = unit(cross(
                    tangent, {0, 0, 1}
                ));

                Vec3 axisZ = unit(cross(
                    tangent, axisX
                ));

                Vec3 n = add(
                    mul(axisX, std::cos(angle)),
                    mul(axisZ, std::sin(angle))
                );

                Vec3 v = add(
                    center,
                    mul(n, radius * (1.0f - 0.15f * t))
                );

                glNormal3f(n.x, n.y, n.z);
                glVertex3f(v.x, v.y, v.z);
            }
        }

        glEnd();
    }
}

// ========================================
// 6. VE LA HOA TULIP
// ========================================

void drawTulipLeaves() {
    const PetalProfile leaf = {
        {0, 0, 0},
        {0, 0.070f, 0.012f},
        {0, 0.175f, 0.046f},
        {0, 0.215f, 0.006f},
        0.051f,
        0.015f,
        18,
        8
    };

    for (int i = 0; i < 2; i++) {
        glPushMatrix();

        glTranslatef(0, 0.17f, 0);

        glRotatef(
            i == 0 ? 67.0f : -67.0f,
            0, 0, 1
        );

        glRotatef(
            i == 0 ? 12.0f : -18.0f,
            0, 1, 0
        );

        setMaterial(
            i == 0 ? 0.16f : 0.21f,
            0.52f,
            0.25f
        );

        drawBezierPetal(leaf);

        glPopMatrix();
    }
}

// ========================================
// 7. VE LA HOA CUC KIM
// ========================================

Vec3 chrysLeafPoint(float t, float s) {
    t = std::clamp(t, 0.0f, 1.0f);
    s = std::clamp(s, -1.0f, 1.0f);

    Vec3 p = cubicBezier(
        {0, 0, 0},
        {0, 0.04f, 0.008f},
        {0, 0.105f, 0.032f},
        {0, 0.145f, 0.004f},
        t
    );

    float width = 0.038f * std::pow(
        std::max(0.0f, std::sin(PI * t)),
        0.8f
    );

    width *= 1.0f
           + 0.12f * std::sin(12.0f * PI * t);

    p.x += s * width;

    p.z += 0.010f
         * std::sin(PI * t)
         * (1.0f - s * s);

    return p;
}

// Tinh phap tuyen va ve dinh cua la
void chrysLeafVertex(float t, float s) {
    const float e = 0.002f;

    Vec3 dt = sub(
        chrysLeafPoint(t + e, s),
        chrysLeafPoint(t - e, s)
    );

    Vec3 ds = sub(
        chrysLeafPoint(t, s + e),
        chrysLeafPoint(t, s - e)
    );

    Vec3 n = unit(cross(ds, dt));
    Vec3 p = chrysLeafPoint(t, s);

    glNormal3f(n.x, n.y, n.z);
    glVertex3f(p.x, p.y, p.z);
}

// Ve ba la hoa cuc
void drawChrysLeaves() {
    for (int leaf = 0; leaf < 3; leaf++) {
        glPushMatrix();

        glTranslatef(
            0,
            0.19f + leaf * 0.056f,
            0
        );

        glRotatef(
            leaf == 1 ? -67.0f : 60.0f,
            0, 0, 1
        );

        glRotatef(
            leaf * 65.0f,
            0, 1, 0
        );

        setMaterial(
            0.18f,
            0.48f + leaf * 0.025f,
            0.22f
        );

        for (int i = 0; i < 18; i++) {
            glBegin(GL_TRIANGLE_STRIP);

            for (int j = 0; j <= 8; j++) {
                float s = -1.0f + 2.0f * j / 8;

                chrysLeafVertex(float(i) / 18, s);
                chrysLeafVertex(float(i + 1) / 18, s);
            }

            glEnd();
        }

        glPopMatrix();
    }
}

// ========================================
// 8. TAO CANH HOA TULIP
// ========================================

Vec3 tulipPoint(
    float t,
    float s,
    float height,
    float width,
    float curl
) {
    t = std::clamp(t, 0.0f, 1.0f);
    s = std::clamp(s, -1.0f, 1.0f);

    Vec3 p = cubicBezier(
        {0, 0, 0.020f},
        {0, height * 0.30f, 0.019f},
        {0, height * 0.77f, 0.057f * curl},
        {0, height, 0.096f * curl},
        t
    );

    float outline = 1.0f;

    if (t < 0.22f) {
        float v = t / 0.22f;

        outline = v * v * (3.0f - 2.0f * v);
    }
    else if (t > 0.78f) {
        float v = (t - 0.78f) / 0.22f;

        outline = std::sqrt(
            std::max(0.0f, 1.0f - v * v)
        );
    }

    p.x += s * width * outline;

    p.z += 0.023f
         * std::sin(PI * t)
         * s * s;

    return p;
}

// Tinh phap tuyen va ve dinh canh tulip
void tulipVertex(
    float t,
    float s,
    float height,
    float width,
    float curl
) {
    const float e = 0.002f;

    Vec3 dt = sub(
        tulipPoint(t + e, s, height, width, curl),
        tulipPoint(t - e, s, height, width, curl)
    );

    Vec3 ds = sub(
        tulipPoint(t, s + e, height, width, curl),
        tulipPoint(t, s - e, height, width, curl)
    );

    Vec3 n = unit(cross(ds, dt));

    Vec3 p = tulipPoint(
        t, s, height, width, curl
    );

    glNormal3f(n.x, n.y, n.z);
    glVertex3f(p.x, p.y, p.z);
}

// Ve luoi canh tulip
void drawTulipPetal(
    float height,
    float width,
    float curl
) {
    for (int i = 0; i < 22; i++) {
        glBegin(GL_TRIANGLE_STRIP);

        for (int j = 0; j <= 10; j++) {
            float s = -1.0f + 2.0f * j / 10;

            tulipVertex(
                float(i) / 22,
                s, height, width, curl
            );

            tulipVertex(
                float(i + 1) / 22,
                s, height, width, curl
            );
        }

        glEnd();
    }
}

// ========================================
// 9. VE HOA TULIP MAU HONG
// ========================================

void drawTulip() {
    drawCurvedStem(0.35f, 0.023f, 0.010f);
    drawTulipLeaves();

    glPushMatrix();

    glTranslatef(0, 0.35f, 0);

    // Ba canh trong: Hong sang
    for (int i = 0; i < 3; i++) {
        glPushMatrix();

        glRotatef(
            20.0f + i * 120.0f,
            0, 1, 0
        );

        // MAU MOI: Hong sang
        setMaterial(1.00f, 0.60f, 0.75f);

        drawTulipPetal(
            0.196f, 0.069f, 0.75f
        );

        glPopMatrix();
    }

    // Ba canh ngoai: Hong dam hon
    for (int i = 0; i < 3; i++) {
        glPushMatrix();

        glRotatef(
            80.0f + i * 120.0f,
            0, 1, 0
        );

        // MAU MOI: Hong tuoi
        setMaterial(0.96f, 0.39f, 0.58f);

        drawTulipPetal(
            0.245f, 0.085f, 1.16f
        );

        glPopMatrix();
    }

    // Ve nhuy hoa
    setMaterial(0.93f, 0.68f, 0.15f);

    glPushMatrix();

    glTranslatef(0, 0.052f, 0);

    ellipsoid(
        0.017f, 0.027f, 0.017f
    );

    glPopMatrix();
    glPopMatrix();
}

// ========================================
// 10. VE HOA CUC KIM MAU TRANG
// ========================================

void drawChrysanthemum() {
    drawCurvedStem(0.425f, 0.028f, 0.008f);
    drawChrysLeaves();

    glPushMatrix();

    glTranslatef(0, 0.425f, 0.015f);

    glRotatef(-18.0f, 1, 0, 0);

    // Ba vong canh: 18 + 16 + 12 = 46
    const int counts[3] = {
        18, 16, 12
    };

    const float lengths[3] = {
        0.145f, 0.115f, 0.085f
    };

    const float widths[3] = {
        0.020f, 0.017f, 0.013f
    };

    for (int ring = 0; ring < 3; ring++) {
        for (int i = 0; i < counts[ring]; i++) {
            glPushMatrix();

            glRotatef(
                360.0f * i / counts[ring]
                    + (ring % 2) * 10.0f,
                0, 0, 1
            );

            glTranslatef(
                0, 0.023f, 0.003f * ring
            );

            // MAU MOI: Cac lop canh trang
            if (ring == 0) {
                // Trang tinh
                setMaterial(1.00f, 1.00f, 1.00f);
            }
            else if (ring == 1) {
                // Trang hoi xam rat nhe
                setMaterial(0.97f, 0.97f, 0.98f);
            }
            else {
                // Trang sang
                setMaterial(1.00f, 0.99f, 0.99f);
            }

            float L = lengths[ring];

            PetalProfile petal = {
                {0, 0, 0},
                {0, 0.32f * L, 0.012f},
                {0, 0.78f * L, 0.023f},
                {0, L, 0.008f},
                widths[ring],
                0.010f,
                16,
                7
            };

            drawBezierPetal(petal);

            glPopMatrix();
        }
    }

    // Nhuy vang
    setMaterial(0.96f, 0.72f, 0.18f);

    glPushMatrix();

    glTranslatef(0, 0, 0.029f);

    ellipsoid(
        0.040f, 0.040f, 0.020f
    );

    glPopMatrix();

    // Muoi hat nho tren nhuy
    setMaterial(0.73f, 0.46f, 0.13f);

    for (int i = 0; i < 10; i++) {
        float a = 2 * PI * i / 10;

        glPushMatrix();

        glTranslatef(
            0.026f * std::cos(a),
            0.026f * std::sin(a),
            0.048f
        );

        ellipsoid(
            0.004f, 0.004f, 0.003f
        );

        glPopMatrix();
    }

    glPopMatrix();
}

} // namespace

// ========================================
// 11. VE CHAU HOA DAT NUNG
// ========================================

void drawFlowerPot() {
    // Than chau
    setMaterial(0.70f, 0.39f, 0.29f);

    cylinderY(
        0.075f, 0.102f, 0.125f, 0
    );

    diskY(0.075f, 0);

    // Mieng chau
    setMaterial(0.85f, 0.55f, 0.41f);

    cylinderY(
        0.108f, 0.108f, 0.023f, 0.117f
    );

    diskY(0.108f, 0.14f);

    // Dat trong chau
    setMaterial(0.28f, 0.20f, 0.14f);

    diskY(0.095f, 0.141f);
}

// ========================================
// 12. HAM VE HOA CHUNG
// ========================================

void drawFlower(FlowerType type) {

    // 1. KIEM TRA PHIEN BAN CODE
    static bool checked = false;

    if (!checked) {
        printf("DEBUG: FLOWERS.CPP MOI DANG CHAY!\n");
        fflush(stdout);

        glutSetWindowTitle("FlowerShop3D - COLOR TEST NEW");

        checked = true;
    }

    // 2. LUU TRANG THAI OPENGL
    glPushAttrib(
        GL_ENABLE_BIT |
        GL_LIGHTING_BIT |
        GL_CURRENT_BIT |
        GL_POLYGON_BIT
    );

    glPushMatrix();

    // 3. CAU HINH VE HOA
    glEnable(GL_NORMALIZE);

    
    glEnable(GL_LIGHTING);

    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_CULL_FACE);

    glLightModeli(
        GL_LIGHT_MODEL_TWO_SIDE,
        GL_TRUE
    );

    // 4. VE CHAU HOA
    drawFlowerPot();

    // 5. VE HOA THEO LOAI
    if (type == FlowerType::TULIP) {
        drawTulip();
    }
    else if (type == FlowerType::DAISY) {
        drawChrysanthemum();
    }

    // 6. KHOI PHUC TRANG THAI OPENGL
    glPopMatrix();
    glPopAttrib();
}
