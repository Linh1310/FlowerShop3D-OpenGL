// =====================================================================
//  shop.cpp - Module cửa hàng (Thủy)
//  Dựng: sàn, tường, cửa, cửa sổ, 2 kệ, quầy thanh toán, bàn ghế,
//        biển hiệu, đèn thả trần.
//  Kỹ thuật: glutSolidCube + glScalef/glTranslatef/glRotatef,
//            glPushMatrix/glPopMatrix, gluCylinder, glutSolidCone.
// =====================================================================
#include "shop.h"
#include <GL/glut.h>
#include <cmath>
#include <vector>

static const float PI = 3.14159265f;

// ---------- Bảng màu ----------
static const float C_WALL[3]      = {0.95f, 0.92f, 0.85f};
static const float C_FLOOR[3]     = {0.55f, 0.55f, 0.58f};
static const float C_RUG[3]       = {0.55f, 0.33f, 0.25f};
static const float C_WOOD[3]      = {0.76f, 0.60f, 0.42f};
static const float C_DARKWOOD[3]  = {0.45f, 0.30f, 0.18f};
static const float C_COUNTER[3]   = {0.86f, 0.72f, 0.52f};
static const float C_BLACK[3]     = {0.12f, 0.12f, 0.14f};
static const float C_SIGN[3]      = {0.15f, 0.35f, 0.25f};
static const float C_SIGNTEXT[3]  = {0.98f, 0.95f, 0.85f};
static const float C_LAMP[3]      = {0.20f, 0.20f, 0.22f};

// ---------- Kích thước các chi tiết ----------
static const float WALL_T   = 0.2f;    // độ dày tường
static const float DOOR_W   = 1.4f;
static const float DOOR_H   = 2.2f;
static const float SHELF_H  = 2.2f;
static const float BOARD_T  = 0.05f;   // độ dày ván kệ

// ---------- Trạng thái ----------
static GLUquadric*             g_quad = 0;
static float                   g_doorAngle = 0.0f;
static bool                    g_doorOpen = false;
static bool                    g_showFrontWall = true;
static std::vector<ShopPoint>  g_slots;

static const ShopPoint LAMPS[3] = {
    {-2.0f, 2.7f, 0.5f},
    { 0.0f, 2.7f, 0.5f},
    { 2.0f, 2.7f, 0.5f}
};

// =====================================================================
//  Hàm vẽ cơ bản
// =====================================================================
static void setColor(const float c[3]) { glColor3f(c[0], c[1], c[2]); }

// Hộp tâm (cx,cy,cz), kích thước (sx,sy,sz)
static void box(float cx, float cy, float cz,
                float sx, float sy, float sz, const float c[3]) {
    setColor(c);
    glPushMatrix();
    glTranslatef(cx, cy, cz);
    glScalef(sx, sy, sz);
    glutSolidCube(1.0);
    glPopMatrix();
}

// Hộp theo toạ độ min/max (dễ ghép tường có lỗ cửa)
static void boxMM(float x0, float x1, float y0, float y1,
                  float z0, float z1, const float c[3]) {
    box((x0 + x1) / 2, (y0 + y1) / 2, (z0 + z1) / 2,
        x1 - x0, y1 - y0, z1 - z0, c);
}

// Trụ đứng, đáy ở (x,y,z), cao h, có nắp trên
static void cylinderY(float x, float y, float z, float r, float h,
                      const float c[3]) {
    setColor(c);
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(-90.0f, 1, 0, 0);          // trục z của quadric -> trục y
    gluCylinder(g_quad, r, r, h, 24, 1);
    glTranslatef(0, 0, h);
    gluDisk(g_quad, 0.0, r, 24, 1);
    glPopMatrix();
}

// Tấm kính trong suốt (vẽ khi đã tắt đèn + bật blending)
static void glassPane(float x0, float x1, float y0, float y1, float z) {
    glBegin(GL_QUADS);
    glVertex3f(x0, y0, z);
    glVertex3f(x1, y0, z);
    glVertex3f(x1, y1, z);
    glVertex3f(x0, y1, z);
    glEnd();
}

// =====================================================================
//  Các thành phần của cửa hàng
// =====================================================================
static void drawFloor() {
    boxMM(-5.0f, 5.0f, -0.05f, 0.0f, -4.0f, 4.0f, C_FLOOR);
    // thảm/bục trang trí phía trước
    boxMM(-3.0f, 1.0f, 0.0f, 0.03f, 0.6f, 3.4f, C_RUG);
}

static void drawBackAndSideWalls() {
    const float H = SHOP_HEIGHT;
    boxMM(-5.0f - WALL_T, 5.0f + WALL_T, 0, H, -4.0f - WALL_T, -4.0f, C_WALL); // sau
    boxMM(-5.0f - WALL_T, -5.0f, 0, H, -4.0f, 4.0f, C_WALL);                    // trái
    boxMM(5.0f, 5.0f + WALL_T, 0, H, -4.0f, 4.0f, C_WALL);                      // phải
}

// Tường trước: ghép từ nhiều khối để chừa lỗ cửa ra vào và 2 cửa sổ
static void drawFrontWall() {
    const float H = SHOP_HEIGHT;
    const float z0 = 4.0f, z1 = 4.0f + WALL_T;
    const float hw = DOOR_W / 2;
    // cửa sổ: x in [-4,-2] và [2,4], y in [1.0, 2.4]
    boxMM(-5.0f - WALL_T, -4.0f, 0, H, z0, z1, C_WALL);
    boxMM(-4.0f, -2.0f, 0, 1.0f, z0, z1, C_WALL);
    boxMM(-4.0f, -2.0f, 2.4f, H, z0, z1, C_WALL);
    boxMM(-2.0f, -hw, 0, H, z0, z1, C_WALL);
    boxMM(-hw, hw, DOOR_H, H, z0, z1, C_WALL);           // phía trên cửa
    boxMM(hw, 2.0f, 0, H, z0, z1, C_WALL);
    boxMM(2.0f, 4.0f, 0, 1.0f, z0, z1, C_WALL);
    boxMM(2.0f, 4.0f, 2.4f, H, z0, z1, C_WALL);
    boxMM(4.0f, 5.0f + WALL_T, 0, H, z0, z1, C_WALL);

    // bệ cửa sổ
    boxMM(-4.0f, -2.0f, 0.95f, 1.0f, z0, z1 + 0.1f, C_WOOD);
    boxMM(2.0f, 4.0f, 0.95f, 1.0f, z0, z1 + 0.1f, C_WOOD);

    // khung cửa (nhô ra 2 phía để không bị z-fighting với tường)
    boxMM(-hw - 0.08f, -hw, 0, DOOR_H + 0.08f, z0 - 0.02f, z1 + 0.02f, C_DARKWOOD);
    boxMM(hw, hw + 0.08f, 0, DOOR_H + 0.08f, z0 - 0.02f, z1 + 0.02f, C_DARKWOOD);
    boxMM(-hw - 0.08f, hw + 0.08f, DOOR_H, DOOR_H + 0.08f, z0 - 0.02f, z1 + 0.02f, C_DARKWOOD);
}

// Cánh cửa quay quanh bản lề bên trái (x = -0.7), mở vào trong
static void drawDoor() {
    glPushMatrix();
    glTranslatef(-DOOR_W / 2, 0.0f, 4.0f + WALL_T / 2);
    glRotatef(g_doorAngle, 0, 1, 0);
    box(DOOR_W / 2, DOOR_H / 2, 0, DOOR_W, DOOR_H, 0.06f, C_DARKWOOD);
    // tay nắm
    setColor(C_COUNTER);
    glPushMatrix();
    glTranslatef(DOOR_W - 0.15f, 1.0f, 0.07f);
    glutSolidSphere(0.04, 12, 12);
    glPopMatrix();
    glPopMatrix();
}

// Kệ vẽ trong hệ toạ độ cục bộ: dọc theo trục x cục bộ, mặt trước hướng +z
static void drawShelfLocal(float len, float dep) {
    const float t = BOARD_T;
    box(-len / 2, SHELF_H / 2, 0, t, SHELF_H, dep, C_WOOD);            // hông trái
    box( len / 2, SHELF_H / 2, 0, t, SHELF_H, dep, C_WOOD);            // hông phải
    box(0, SHELF_H / 2, -dep / 2 + t / 2, len, SHELF_H, t, C_DARKWOOD); // vách sau
    const float ys[4] = {0.1f, 0.8f, 1.5f, SHELF_H};                    // 4 ván
    for (int i = 0; i < 4; i++)
        box(0, ys[i], 0, len, t, dep, C_WOOD);
}

static void drawShelves() {
    // Kệ A: sát tường sau
    glPushMatrix();
    glTranslatef(-1.5f, 0, -3.75f);
    drawShelfLocal(4.0f, 0.5f);
    glPopMatrix();
    // Kệ B: sát tường trái, xoay 90 độ để mặt trước hướng vào trong (+x)
    glPushMatrix();
    glTranslatef(-4.75f, 0, 0.0f);
    glRotatef(90.0f, 0, 1, 0);
    drawShelfLocal(3.0f, 0.5f);
    glPopMatrix();
}

static void drawCounter() {
    box(4.2f, 0.55f, -1.5f, 1.2f, 1.1f, 3.0f, C_DARKWOOD);      // thân quầy
    box(4.2f, 1.125f, -1.5f, 1.3f, 0.05f, 3.1f, C_COUNTER);     // mặt quầy
    box(4.2f, 1.22f, -2.2f, 0.35f, 0.12f, 0.30f, C_BLACK);      // máy tính tiền
    box(4.2f, 1.40f, -2.3f, 0.30f, 0.22f, 0.03f, C_BLACK);      // màn hình
}

static void drawTableAndStools() {
    // bàn tròn
    cylinderY(2.5f, 0.0f, 2.0f, 0.05f, 0.65f, C_BLACK);
    cylinderY(2.5f, 0.65f, 2.0f, 0.50f, 0.05f, C_WOOD);
    // 2 ghế đẩu
    for (int i = 0; i < 2; i++) {
        float sx = (i == 0) ? 1.8f : 3.2f;
        cylinderY(sx, 0.0f, 2.0f, 0.04f, 0.40f, C_BLACK);
        cylinderY(sx, 0.40f, 2.0f, 0.18f, 0.05f, C_DARKWOOD);
    }
}

static void drawLamps() {
    for (int i = 0; i < 3; i++) {
        const ShopPoint& p = LAMPS[i];
        cylinderY(p.x, p.y + 0.15f, p.z, 0.01f, SHOP_HEIGHT - p.y - 0.15f, C_LAMP); // dây
        setColor(C_LAMP);
        glPushMatrix();
        glTranslatef(p.x, p.y - 0.15f, p.z);
        glRotatef(-90.0f, 1, 0, 0);          // đỉnh nón hướng lên
        glutSolidCone(0.25, 0.30, 20, 2);
        glPopMatrix();
    }
}

// Chữ nét (stroke font) căn giữa theo x, đứng trên mặt phẳng z cố định
static void drawStrokeText(const char* s, float cx, float y, float z,
                           float scale, const float c[3]) {
    float w = 0;
    for (const char* p = s; *p; p++) w += glutStrokeWidth(GLUT_STROKE_ROMAN, *p);
    w *= scale;
    glColor3f(c[0], c[1], c[2]);
    glLineWidth(2.0f);
    glPushMatrix();
    glTranslatef(cx - w / 2, y, z);
    glScalef(scale, scale, scale);
    for (const char* p = s; *p; p++) glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
    glPopMatrix();
}

// =====================================================================
//  Ô đặt chậu hoa trên kệ
// =====================================================================
static void addShelfSlots(float cx, float cz, float thetaDeg,
                          float len, int perTier) {
    const float th = thetaDeg * PI / 180.0f;
    const float c = cosf(th), s = sinf(th);
    const float tierY[2] = {0.8f, 1.5f};
    for (int t = 0; t < 2; t++) {
        for (int i = 0; i < perTier; i++) {
            float lx = -len / 2 + len / perTier * (i + 0.5f);   // toạ độ cục bộ
            ShopPoint p;
            p.x = cx + lx * c;        // lz = 0 nên bỏ số hạng lz * s
            p.z = cz - lx * s;        // (công thức khớp glRotatef quanh trục Y)
            p.y = tierY[t] + BOARD_T / 2;
            g_slots.push_back(p);
        }
    }
}

// =====================================================================
//  API công khai
// =====================================================================
void initShop() {
    if (!g_quad) {
        g_quad = gluNewQuadric();
        gluQuadricNormals(g_quad, GLU_SMOOTH);
    }
    glEnable(GL_NORMALIZE);   // cần vì glScalef không đều làm lệch pháp tuyến

    g_slots.clear();
    addShelfSlots(-1.5f, -3.75f, 0.0f, 4.0f, 4);    // kệ A: 2 tầng x 4 = 8 ô
    addShelfSlots(-4.75f, 0.0f, 90.0f, 3.0f, 2);    // kệ B: 2 tầng x 2 = 4 ô
}

void drawShop() {
    glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_CURRENT_BIT |
                 GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_LINE_BIT);

    // glColor3f sẽ điều khiển màu vật liệu khi bật đèn
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    drawFloor();
    drawBackAndSideWalls();
    drawShelves();
    drawCounter();
    drawTableAndStools();
    drawLamps();
    if (g_showFrontWall) {
        drawFrontWall();
        drawDoor();
        // biển hiệu phía ngoài, trên cửa
        boxMM(-1.4f, 1.4f, 2.5f, 3.2f, 4.0f + WALL_T, 4.0f + WALL_T + 0.06f, C_SIGN);
    }

    // ---- Phần không dùng ánh sáng: chữ và kính trong suốt ----
    glDisable(GL_LIGHTING);
    if (g_showFrontWall) {
        drawStrokeText("FLOWER SHOP", 0.0f, 2.72f, 4.0f + WALL_T + 0.07f,
                       0.0025f, C_SIGNTEXT);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);               // kính vẽ cuối, không ghi depth
        glColor4f(0.60f, 0.80f, 0.95f, 0.30f);
        glassPane(-4.0f, -2.0f, 1.0f, 2.4f, 4.0f + WALL_T / 2);
        glassPane( 2.0f,  4.0f, 1.0f, 2.4f, 4.0f + WALL_T / 2);
    }

    glPopAttrib();
}

void shopToggleDoor() {
    g_doorOpen = !g_doorOpen;
    g_doorAngle = g_doorOpen ? 80.0f : 0.0f;
}

void shopToggleFrontWall() { g_showFrontWall = !g_showFrontWall; }

int shopSlotCount() { return (int)g_slots.size(); }

ShopPoint shopSlot(int i) {
    if (i < 0 || i >= (int)g_slots.size()) {
        ShopPoint zero = {0, 0, 0};
        return zero;
    }
    return g_slots[i];
}

int shopLampCount() { return 3; }

ShopPoint shopLampPos(int i) {
    if (i < 0 || i >= 3) i = 0;
    return LAMPS[i];
}