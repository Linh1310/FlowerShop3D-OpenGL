
#ifndef FLOWERS_H
#define FLOWERS_H

// ========================================
// 1. KHAI BAO CAC LOAI HOA
// ========================================

// Enum dung de phan biet tung loai hoa
enum class FlowerType {
    TULIP,       // Hoa tulip hong
    DAISY,       // Hoa cuc kim trang kem
    SUNFLOWER    // Gia tri giu lai de tuong thich
};

// ========================================
// 2. KHAI BAO HAM VE CHAU HOA
// ========================================

// Ve chau hoa tai goc toa do (0, 0, 0)
// Day chau nam tren mat phang y = 0
void drawFlowerPot();

// ========================================
// 3. KHAI BAO HAM VE HOA 3D
// ========================================

// Ve mot chau hoa theo loai duoc chon
// type = TULIP: Ve hoa tulip
// type = DAISY: Ve hoa cuc kim
void drawFlower(FlowerType type);

#endif
