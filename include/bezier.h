
#ifndef FLOWER_BEZIER_H
#define FLOWER_BEZIER_H

// ========================================
// 1. KHAI BAO KHONG GIAN TEN
// ========================================

namespace flower3d {

// ========================================
// 2. KHAI BAO CAU TRUC DU LIEU
// ========================================

// Mot diem hoac vector trong khong gian 3D
struct Vec3 {
    float x;
    float y;
    float z;
};

// Cac thong so de ve mot canh hoa 3D
struct PetalProfile {
    // Bon diem dieu khien duong cong Bezier
    Vec3 p0;
    Vec3 p1;
    Vec3 p2;
    Vec3 p3;

    float halfWidth;  // Nua chieu rong canh hoa
    float cupDepth;   // Do khum cua canh hoa

    int along;        // So phan chia theo chieu dai
    int across;       // So phan chia theo chieu rong
};

// ========================================
// 3. KHAI BAO HAM TINH BEZIER
// ========================================

// Tinh diem tren duong cong Bezier bac 3
// p0, p1, p2, p3: Bon diem dieu khien
// t: Tham so xac dinh vi tri tren duong cong (0 -> 1)

Vec3 cubicBezier(
    const Vec3& p0,
    const Vec3& p1,
    const Vec3& p2,
    const Vec3& p3,
    float t
);

// ========================================
// 4. KHAI BAO HAM VE CANH HOA
// ========================================

// Tao luoi canh hoa 3D bang duong cong Bezier
void drawBezierPetal(const PetalProfile& profile);

} // namespace flower3d

#endif
