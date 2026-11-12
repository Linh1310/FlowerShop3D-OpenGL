
#include "scene.h"
#include "flowers.h"
#include "shop.h"

#include <GL/glut.h>
#include <cmath>
#include <cstdio>

namespace {

// ========================================
// 1. KHAI BAO CAU TRUC DU LIEU
// ========================================

// Thong tin mot hang hoa tren gia ke
struct ShelfRow {
    int firstSlot;       // Vi tri dau tien
    int lastSlot;        // Vi tri cuoi cung
    float y;             // Do cao cua tang ke
    int potCount;        // So chau hoa
    FlowerType type;     // Loai hoa
    float facing;        // Goc xoay cua chau
    const char* name;    // Ten hang hoa
};

// ========================================
// 2. KHAI BAO THONG SO VA BO TRI HOA
// ========================================

// Moi gia ke co 3 tang trung bay
// Ke sau: 6 chau / tang
// Ke trai: 5 chau / tang
// Tong cong: 33 chau

// Do cao mat tren cua cac van ke:
// Tang 1: 0.125
// Tang 2: 0.825
// Tang 3: 1.525

const ShelfRow ROWS[] = {

    // KE SAU: 3 TANG, MOI TANG 6 CHAU

    // Tang duoi: Hoa cuc kim
    {0, 3, 0.125f, 6, FlowerType::DAISY, 0.0f,
     "Back shelf - Level 1: Cream Chrysanthemum"},

    // Tang giua: Hoa tulip
    {0, 3, 0.825f, 6, FlowerType::TULIP, 0.0f,
     "Back shelf - Level 2: Pink Tulip"},

    // Tang tren: Hoa cuc kim
    {4, 7, 1.525f, 6, FlowerType::DAISY, 0.0f,
     "Back shelf - Level 3: Cream Chrysanthemum"},

    // KE TRAI: 3 TANG, MOI TANG 5 CHAU

    // Tang duoi: Hoa tulip
    {8, 9, 0.125f, 5, FlowerType::TULIP, 90.0f,
     "Left shelf - Level 1: Pink Tulip"},

    // Tang giua: Hoa cuc kim
    {8, 9, 0.825f, 5, FlowerType::DAISY, 90.0f,
     "Left shelf - Level 2: Cream Chrysanthemum"},

    // Tang tren: Hoa tulip
    {10, 11, 1.525f, 5, FlowerType::TULIP, 90.0f,
     "Left shelf - Level 3: Pink Tulip"}
};

// So hang hoa
const int ROW_COUNT =
    sizeof(ROWS) / sizeof(ROWS[0]);

// Khoang cach tu chau ngoai cung den mep ke
const float END_MARGIN = 0.30f;

// Ti le kich thuoc chau hoa
const float FLOWER_SCALE = 0.98f;

// ========================================
// TRANG THAI CHON / XOAY / PHONG TO CHAU
// ========================================
const int MAX_POTS = 64;
int   selectedPot = -1;              // -1 = chua chon chau nao
float potAngle[MAX_POTS] = {0};      // goc xoay them cua tung chau
float potGrow[MAX_POTS]  = {0};      // do phong to them (0 = binh thuong)
int   potCounter = 0;                // dem thu tu chau trong luc ve

int totalPots() {
    int n = 0;
    for (int i = 0; i < ROW_COUNT; i++) n += ROWS[i].potCount;
    return n;
}
 // Gioi han phong to theo tang ke (tang tren cung it vuong hon)
float maxGrowFor(int id) {
    int acc = 0;
    for (int i = 0; i < ROW_COUNT; i++) {
        acc += ROWS[i].potCount;
        if (id < acc) return (ROWS[i].y > 1.4f) ? 0.60f : 0.12f;
    }
    return 0.12f;
}
// Vong vang duoi day chau dang duoc chon
void drawSelectionRing() {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT);
    glDisable(GL_LIGHTING);
    glLineWidth(3.0f);
    glColor3f(1.0f, 0.85f, 0.1f);
    glBegin(GL_LINE_LOOP);
    for (int k = 0; k < 40; k++) {
        float a = 2.0f * 3.14159265f * k / 40;
        glVertex3f(0.25f * std::cos(a), 0.02f, 0.25f * std::sin(a));
    }
    glEnd();
    glPopAttrib();
}

// ========================================
// 3. HAM TINH TOAN VA VE MOT HANG HOA
// ========================================

// Sap xep cac chau cach deu tren mot tang ke
void drawRow(const ShelfRow& row) {

    // Lay toa do hai vi tri dau va cuoi
    ShopPoint first = shopSlot(row.firstSlot);
    ShopPoint last = shopSlot(row.lastSlot);

    // Do chenh lech toa do theo X va Z
    float dx = last.x - first.x;
    float dz = last.z - first.z;

    // Khoang cach giua hai vi tri
    float lengthBetween = std::sqrt(
        dx * dx + dz * dz
    );

    // So khoang giua cac vi tri
    int intervals = row.lastSlot - row.firstSlot;

    // Kiem tra du lieu hop le
    if (
        intervals <= 0 ||
        lengthBetween < 0.001f ||
        row.potCount < 2
    ) {
        return;
    }

    // Vector don vi chi huong doc theo ke
    float ux = dx / lengthBetween;
    float uz = dz / lengthBetween;

    // Tinh chieu dai thuc cua ke
    float fullLength =
        lengthBetween + lengthBetween / intervals;

    // Chieu dai su dung de dat chau
    float usable = fullLength - 2.0f * END_MARGIN;

    // Toa do tam cua hang hoa
    float cx = (first.x + last.x) * 0.5f;
    float cz = (first.z + last.z) * 0.5f;

    // ====================================
    // VE TUNG CHAU HOA TREN HANG
    // ====================================

    for (int i = 0; i < row.potCount; i++) {

        // t thay doi tu 0 den 1
        float t = float(i) / (row.potCount - 1);

        // Vi tri chau so voi tam hang
        float offset = (t - 0.5f) * usable;

        // Toa do chau trong khong gian 3D
        float x = cx + ux * offset;
        float z = cz + uz * offset;

        glPushMatrix();

        // Dua chau den vi tri tren ke
        glTranslatef(x, row.y, z);

        // Xoay chau theo huong gia ke
        glRotatef(
            row.facing,
            0, 1, 0
        );

        // Tao chenh lech nhe ve kich thuoc
        float variation =
            (i % 2 == 0) ? 1.0f : 0.97f;

        float scale = FLOWER_SCALE * variation;

        glScalef(
            scale,
            scale,
            scale
        );

        // Xoay bong tulip cho tu nhien
        if (row.type == FlowerType::TULIP) {
            glRotatef(
                i * 11.0f,
                0, 1, 0
            );
        }

        // Ve chau va bong hoa
                // ---- Chon / xoay / phong to (Trang) ----
        int id = potCounter++;
        if (id < MAX_POTS) {
            glRotatef(potAngle[id], 0, 1, 0);
            float g = 1.0f + potGrow[id];
            glScalef(g, g, g);
        }
        if (id == selectedPot) drawSelectionRing();

        drawFlower(row.type);

        // Khoi phuc ma tran
        glPopMatrix();
    }
}

} // namespace

// ========================================
// 4. VE TAT CA HOA TREN GIA KE
// ========================================

void drawFlowerScene() {

    // Can du 12 slot do shop.cpp cung cap
    if (shopSlotCount() < 12) {
        return;
    }

        potCounter = 0;    // danh so lai chau moi frame

    // Ve lan luot tung hang hoa
    for (int i = 0; i < ROW_COUNT; i++) {
        drawRow(ROWS[i]);
    }
}

// ========================================
// 5. IN THONG TIN BO TRI HOA
// ========================================

void printFlowerSceneLayout() {

    printf("\n=== FLOWER SHOP: 3 LEVELS PER SHELF ===\n");

    printf(
        "Slots from shop: %d\n",
        shopSlotCount()
    );

    // Kiem tra so vi tri tren ke
    if (shopSlotCount() < 12) {
        printf("ERROR: 12 shelf slots expected\n");
        return;
    }

    int totalPots = 0;

    // In thong tin tung hang hoa
    for (int i = 0; i < ROW_COUNT; i++) {

        const ShelfRow& row = ROWS[i];

        totalPots += row.potCount;

        printf(
            "Row %d: %s | y=%.3f | pots=%d\n",
            i + 1,
            row.name,
            row.y,
            row.potCount
        );
    }

    // Tong so chau hoa
    printf(
        "TOTAL POTS: %d\n",
        totalPots
    );
}

// ========================================
// 6. CHON VA THAO TAC CHAU HOA
// ========================================
void sceneSelectPot(int index) {
    selectedPot = (index >= 0 && index < totalPots()) ? index : -1;
}

void sceneSelectStep(int dir) {
    int n = totalPots();
    if (n <= 0) return;
    if (selectedPot < 0) selectedPot = (dir > 0) ? 0 : n - 1;
    else selectedPot = (selectedPot + dir + n) % n;
}

void sceneRotateSelected(float deg) {
    if (selectedPot >= 0 && selectedPot < MAX_POTS) potAngle[selectedPot] += deg;
}

void sceneScaleSelected(float delta) {
    if (selectedPot < 0 || selectedPot >= MAX_POTS) return;
    potGrow[selectedPot] += delta;
    if (potGrow[selectedPot] < -0.4f) potGrow[selectedPot] = -0.4f;
    if (potGrow[selectedPot] > maxGrowFor(selectedPot)) potGrow[selectedPot] = maxGrowFor(selectedPot);
}

void sceneResetSelection() {
    selectedPot = -1;
    for (int i = 0; i < MAX_POTS; i++) { potAngle[i] = 0; potGrow[i] = 0; }
}

int sceneSelectedFlowerType() {
    if (selectedPot < 0) return -1;
    int acc = 0;
    for (int i = 0; i < ROW_COUNT; i++) {
        acc += ROWS[i].potCount;
        if (selectedPot < acc) return (ROWS[i].type == FlowerType::TULIP) ? 0 : 1;
    }
    return -1;
}

float sceneSelectedAngle() {
    return (selectedPot >= 0 && selectedPot < MAX_POTS) ? potAngle[selectedPot] : 0.0f;
}

int sceneSelectedIndex() { return selectedPot; }
int scenePotCount()      { return totalPots(); }

const char* sceneSelectedName() {
    if (selectedPot < 0) return "";
    int acc = 0;
    for (int i = 0; i < ROW_COUNT; i++) {
        if (selectedPot < acc + ROWS[i].potCount) return ROWS[i].name;
        acc += ROWS[i].potCount;
    }
    return "";
}