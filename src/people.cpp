#include <GL/freeglut.h>
#include <algorithm>
#include <cmath>
#include "order.h"
#include "people.h"
#include "shop.h"

namespace {
struct Color { float r, g, b; };
struct Outfit {
    Color skin, hair, shirt, pants;
};

constexpr Color shoes{0.17f, 0.12f, 0.11f};
constexpr Color apron{0.20f, 0.43f, 0.33f};
constexpr Color apronPocket{0.29f, 0.53f, 0.39f};
constexpr Color nameTag{0.99f, 0.78f, 0.45f};
constexpr Outfit florist{{0.87f, 0.58f, 0.43f}, {0.20f, 0.12f, 0.10f},
                         {0.97f, 0.84f, 0.70f}, {0.26f, 0.28f, 0.28f}};
constexpr Outfit visitors[] = {
    {{0.98f, 0.71f, 0.54f}, {0.30f, 0.16f, 0.10f}, {0.42f, 0.54f, 0.78f}, {0.29f, 0.35f, 0.49f}},
    {{0.61f, 0.37f, 0.27f}, {0.15f, 0.10f, 0.10f}, {0.82f, 0.41f, 0.50f}, {0.34f, 0.30f, 0.47f}},
    {{0.91f, 0.66f, 0.50f}, {0.45f, 0.27f, 0.13f}, {0.75f, 0.50f, 0.24f}, {0.36f, 0.44f, 0.36f}}
};
constexpr int visitorCount = sizeof visitors / sizeof visitors[0];
constexpr float pi = 3.14159265f;

enum class Visit { Entering, Waiting, Collecting, Leaving, Between };
Visit visit = Visit::Entering;
float clockTime = 0.0f;
float routeProgress = 0.0f;
float stageTime = 0.0f;
float walkingPhase = 0.0f;
float packingTime = 0.0f;
int outfitIndex = 0;

void ball(float x, float y, float z, float width, float height, float depth, Color color) {
    glColor3f(color.r, color.g, color.b);
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(width, height, depth);
    glutSolidSphere(1.0, 12, 8);
    glPopMatrix();
}

void drawBag() {
    constexpr Color kraft{0.78f, 0.57f, 0.36f};
    glPushMatrix();
    glTranslatef(0.0f, -0.11f, 0.015f);
    glColor3f(kraft.r, kraft.g, kraft.b);
    glutSolidTorus(0.012, 0.062, 5, 12);
    glTranslatef(0.0f, -0.135f, 0.0f);
    glScalef(0.23f, 0.25f, 0.12f);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawLeg(float side, float step, const Outfit& clothes) {
    glPushMatrix();
    glTranslatef(side * 0.099f, 0.65f, 0.0f);
    glRotatef(-step * 27.0f, 1, 0, 0);
    ball(0, -0.145f, 0, 0.081f, 0.179f, 0.078f, clothes.pants);
    glTranslatef(0, -0.29f, 0);
    glRotatef(std::max(0.0f, -step) * 36.0f, 1, 0, 0);
    ball(0, -0.14f, 0, 0.063f, 0.17f, 0.063f, clothes.pants);
    ball(0, -0.294f, 0.054f, 0.072f, 0.056f, 0.12f, shoes);
    glPopMatrix();
}

void drawArm(float side, float swing, float reach, float wave, bool bag,
             const Outfit& clothes) {
    glPushMatrix();
    glTranslatef(side * 0.205f, 1.13f, 0);
    glRotatef(side * (11.0f + 34.0f * wave), 0, 0, 1);
    glRotatef(swing - 52.0f * reach - 43.0f * wave, 1, 0, 0);
    ball(0, -0.095f, 0, 0.065f, 0.135f, 0.066f, clothes.shirt);
    glTranslatef(0, -0.21f, 0);
    glRotatef(-12.0f - 20.0f * reach - 83.0f * wave, 1, 0, 0);
    ball(0, -0.078f, 0, 0.046f, 0.106f, 0.048f, clothes.skin);
    glTranslatef(0, -0.183f, 0);
    ball(0, 0, 0, 0.053f, 0.06f, 0.041f, clothes.skin);
    if (bag) drawBag();
    glPopMatrix();
}

void drawHead(const Outfit& clothes, bool staff, float nod) {
    glPushMatrix();
    glTranslatef(0, 1.31f, 0);
    glRotatef(nod, 1, 0, 0);
    ball(0, 0.145f, 0.012f, 0.145f, 0.169f, 0.133f, clothes.skin);
    ball(0, 0.216f, -0.032f, 0.151f, 0.127f, 0.136f, clothes.hair);
    ball(-0.057f, 0.239f, 0.074f, 0.105f, 0.051f, 0.071f, clothes.hair);
    if (staff) ball(0, 0.268f, -0.133f, 0.092f, 0.081f, 0.078f, clothes.hair);
    for (float side : {-1.0f, 1.0f}) {
        ball(side * 0.143f, 0.137f, 0.004f, 0.025f, 0.039f, 0.023f, clothes.skin);
        ball(side * 0.05f, 0.157f, 0.135f, 0.016f, 0.021f, 0.009f, shoes);
    }
    ball(0, 0.125f, 0.143f, 0.022f, 0.023f, 0.023f, clothes.skin);
    ball(0, 0.084f, 0.131f, 0.034f, 0.007f, 0.008f, {0.58f, 0.26f, 0.22f});
    glPopMatrix();
}

void drawPerson(float x, float z, float heading, float scale, float step, float bob,
                const Outfit& clothes, bool staff, float reach, float wave, bool bag) {
    glPushMatrix();
    glTranslatef(x, 0.004f + bob, z);
    glRotatef(heading, 0, 1, 0);
    glScalef(scale, scale, scale);
    drawLeg(-1, step, clothes);
    drawLeg(1, -step, clothes);

    glTranslatef(0, 0.68f, 0);
    glRotatef(14.0f * reach, 1, 0, 0);
    glTranslatef(0, -0.68f, 0);
    ball(0, 0.68f, 0, 0.176f, 0.12f, 0.106f, clothes.pants);
    ball(0, 0.965f, 0.004f, 0.23f, 0.30f, 0.127f, clothes.shirt);
    ball(0, 1.271f, 0.012f, 0.067f, 0.079f, 0.064f, clothes.skin);
    if (staff) {
        ball(0, 0.84f, 0.084f, 0.184f, 0.248f, 0.073f, apron);
        ball(0, 1.031f, 0.105f, 0.136f, 0.136f, 0.051f, apron);
        ball(0.018f, 0.839f, 0.152f, 0.077f, 0.061f, 0.023f, apronPocket);
        ball(-0.055f, 1.080f, 0.149f, 0.045f, 0.022f, 0.013f, nameTag);
    }
    drawArm(-1, -step * 30.0f, reach, wave, false, clothes);
    drawArm(1, step * (bag ? 9.0f : 30.0f), reach * 0.83f, 0, bag, clothes);
    drawHead(clothes, staff, 12.0f * reach - 5.0f * wave);
    glPopMatrix();
}

void drawCustomer() {
    if (visit == Visit::Between) return;
    const float t = routeProgress;
    const float remaining = 1.0f - t;
    // Quadratic Bezier from the doorway to the open aisle beside the counter.
    const float x = 2.95f * t * t + 0.15f * 2.0f * t * remaining;
    const float z = 3.77f * remaining * remaining + 0.1f * 2.0f * t * remaining
                    - 1.35f * t * t;
    const float dx = 0.15f * remaining + 2.80f * t;
    const float dz = -3.67f * remaining - 1.45f * t;
    const float travelHeading = std::atan2(dx, dz) * 180.0f / pi;
    const float turnToCounter = std::max(0.0f, (t - 0.86f) / 0.14f);
    const bool leaving = visit == Visit::Leaving;
    const bool moving = leaving || visit == Visit::Entering;
    const float heading = leaving ? travelHeading + 180.0f
                                  : travelHeading + (90.0f - travelHeading) * turnToCounter;
    const float step = moving ? std::sin(walkingPhase) : 0.0f;
    const float bob = moving ? 0.044f * std::abs(step) : 0.011f * std::sin(clockTime * 3.5f);
    const bool wave = (visit == Visit::Waiting && orderComplete()) || visit == Visit::Collecting;
    const float gesture = wave ? 0.72f + 0.20f * std::sin(clockTime * 10.0f) : 0.0f;
    drawPerson(x, z, heading, 1.05f, step, bob, visitors[outfitIndex], false, 0, gesture,
               leaving || visit == Visit::Collecting);
}
}

void staffPackFlower() { packingTime = 1.12f; }

void customerCollectOrder() {
    if (visit == Visit::Entering || visit == Visit::Waiting) {
        visit = Visit::Collecting;
        stageTime = 0.55f;
    }
}

void updatePeople(float elapsed) {
    clockTime += elapsed;
    packingTime = std::max(0.0f, packingTime - elapsed);

    switch (visit) {
        case Visit::Entering:
            walkingPhase += elapsed * 9.0f;
            routeProgress = std::min(1.0f, routeProgress + elapsed / 5.4f);
            if (routeProgress >= 1.0f) visit = Visit::Waiting;
            break;
        case Visit::Waiting:
            break;
        case Visit::Collecting:
            stageTime -= elapsed;
            if (stageTime <= 0.0f) visit = Visit::Leaving;
            break;
        case Visit::Leaving:
            walkingPhase += elapsed * 9.0f;
            routeProgress = std::max(0.0f, routeProgress - elapsed / 4.5f);
            if (routeProgress <= 0.0f) {
                visit = Visit::Between;
                stageTime = 0.65f;
            }
            break;
        case Visit::Between:
            stageTime -= elapsed;
            if (stageTime <= 0.0f) {
                outfitIndex = (outfitIndex + 1) % visitorCount;
                walkingPhase = 0.0f;
                visit = Visit::Entering;
            }
            break;
    }
}

void drawPeople() {
    glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_CURRENT_BIT);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    const ShopPoint table = shopPackingTableTop();
    const float reach = std::sin(pi * packingTime / 1.12f);
    drawPerson(table.x + 0.34f, table.z - 0.65f, -28.0f, 1.0f,
               0.0f, 0.009f * std::sin(clockTime * 3.0f), florist, true, reach, 0.0f, false);
    drawCustomer();
    glPopAttrib();
}
