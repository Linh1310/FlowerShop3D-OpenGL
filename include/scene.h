
#ifndef FLOWER_SCENE_H
#define FLOWER_SCENE_H

// ========================================
// 1. KHAI BAO HAM VE HOA TREN KE
// ========================================

// Ve cac chau hoa tren hai gia ke
// Moi gia ke co 3 tang trung bay
// So luong va vi tri hoa duoc xu ly trong scene.cpp
void drawFlowerScene();

// ========================================
// 2. KHAI BAO HAM IN THONG TIN BO TRI
// ========================================

// In thong tin cac hang hoa ra Terminal
// Gom loai hoa, do cao va so chau moi hang
void printFlowerSceneLayout();
// ===== Chon va thao tac tren chau hoa (Trang) =====
void sceneSelectPot(int index);        // chon chau theo so thu tu, -1 = bo chon
void sceneSelectStep(int dir);         // +1: chau sau, -1: chau truoc
void sceneRotateSelected(float deg);   // xoay chau dang chon quanh truc Y
void sceneScaleSelected(float delta);  // phong to / thu nho chau dang chon
void sceneResetSelection();            // bo chon, tra goc xoay va kich thuoc ve ban dau

int   sceneSelectedFlowerType();   // 0 = tulip, 1 = cuc, -1 = chua chon
float sceneSelectedAngle();        // goc xoay hien tai cua chau dang chon

int         sceneSelectedIndex();   // -1 = chua chon
int         scenePotCount();        // tong so chau
const char* sceneSelectedName();    // ten hang hoa cua chau dang chon

#endif
