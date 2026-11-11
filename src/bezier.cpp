
#include "bezier.h"
#include <GL/glut.h>
#include <algorithm>
#include <cmath>

namespace flower3d {

namespace {

// ========================================
// 1. KHAI BAO HANG SO
// ========================================

constexpr float PI = 3.14159265358979323846f;

// ========================================
// 2. CAC HAM TINH TOAN VECTOR 3D
// ========================================

// Tru hai vector
Vec3 subtract(const Vec3& a, const Vec3& b) {
    return {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
}

// Tich co huong cua hai vector
Vec3 cross(const Vec3& a, const Vec3& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

// Chuan hoa vector ve do dai 1
Vec3 normal(const Vec3& a) {
    float length = std::sqrt(
        a.x * a.x +
        a.y * a.y +
        a.z * a.z
    );

    // Tranh chia cho 0
    if (length < 1e-7f) {
        return {0, 0, 1};
    }

    return {
        a.x / length,
        a.y / length,
        a.z / length
    };
}

// ========================================
// 3. TINH DIEM TREN BE MAT CANH HOA
// ========================================

// t: Vi tri theo chieu dai canh (0 -> 1)
// s: Vi tri theo chieu rong canh (-1 -> 1)

Vec3 petalPoint(
    const PetalProfile& p,
    float t,
    float s
) {
    // Gioi han tham so trong khoang hop le
    t = std::clamp(t, 0.0f, 1.0f);
    s = std::clamp(s, -1.0f, 1.0f);

    // Tinh diem tren duong song canh Bezier
    Vec3 point = cubicBezier(
        p.p0,
        p.p1,
        p.p2,
        p.p3,
        t
    );

    // Do rong thay doi theo chieu dai canh
    // Hep o hai dau, rong nhat o giua
    float envelope = std::pow(
        std::max(0.0f, std::sin(PI * t)),
        1.25f
    );

    // Mo rong canh theo truc X
    point.x += s * p.halfWidth * envelope;

    // Tao do khum cua be mat theo truc Z
    point.z += p.cupDepth
             * std::sin(PI * t)
             * (1.0f - s * s);

    return point;
}

// ========================================
// 4. TINH PHAP TUYEN VA VE DINH
// ========================================

// Phap tuyen duoc dung de tinh anh sang
// cho be mat canh hoa trong OpenGL

void emitVertex(
    const PetalProfile& p,
    float t,
    float s
) {
    // Do lech nho de tinh vector tiep tuyen
    constexpr float E = 0.002f;

    // Vector theo chieu dai canh
    Vec3 along = subtract(
        petalPoint(p, t + E, s),
        petalPoint(p, t - E, s)
    );

    // Vector theo chieu rong canh
    Vec3 across = subtract(
        petalPoint(p, t, s + E),
        petalPoint(p, t, s - E)
    );

    // Tich co huong tao vector phap tuyen
    Vec3 n = normal(cross(across, along));

    // Toa do dinh tren be mat canh hoa
    Vec3 position = petalPoint(p, t, s);

    // Gui phap tuyen va dinh cho OpenGL
    glNormal3f(n.x, n.y, n.z);

    glVertex3f(
        position.x,
        position.y,
        position.z
    );
}

} // namespace

// ========================================
// 5. THUAT TOAN BEZIER BAC 3
// ========================================

// Cong thuc:
// B(t) = (1-t)^3 * P0
//      + 3(1-t)^2*t * P1
//      + 3(1-t)*t^2 * P2
//      + t^3 * P3
//
// P0, P1, P2, P3: Bon diem dieu khien
// t: Tham so tu 0 den 1

Vec3 cubicBezier(
    const Vec3& p0,
    const Vec3& p1,
    const Vec3& p2,
    const Vec3& p3,
    float t
) {
    t = std::clamp(t, 0.0f, 1.0f);

    float u = 1.0f - t;

    // Bon he so cua da thuc Bezier
    float a = u * u * u;
    float b = 3.0f * u * u * t;
    float c = 3.0f * u * t * t;
    float d = t * t * t;

    // Tinh toa do X, Y, Z
    return {
        a * p0.x + b * p1.x + c * p2.x + d * p3.x,
        a * p0.y + b * p1.y + c * p2.y + d * p3.y,
        a * p0.z + b * p1.z + c * p2.z + d * p3.z
    };
}

// ========================================
// 6. VE LUOI CANH HOA 3D
// ========================================

// Chia canh hoa thanh cac dai tam giac
// Su dung GL_TRIANGLE_STRIP de tao be mat

void drawBezierPetal(const PetalProfile& p) {

    // So doan chia theo chieu dai va rong
    int lengthSteps = std::max(4, p.along);
    int widthSteps = std::max(3, p.across);

    // Duyet tung doan theo chieu dai
    for (int i = 0; i < lengthSteps; i++) {

        float t0 = float(i) / lengthSteps;
        float t1 = float(i + 1) / lengthSteps;

        glBegin(GL_TRIANGLE_STRIP);

        // Duyet tung diem theo chieu rong
        for (int j = 0; j <= widthSteps; j++) {

            float s = -1.0f
                    + 2.0f * j / widthSteps;

            // Hai dinh tren hai duong t0 va t1
            emitVertex(p, t0, s);
            emitVertex(p, t1, s);
        }

        glEnd();
    }
}

} // namespace flower3d
