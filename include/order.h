#ifndef FLOWER_ORDER_H
#define FLOWER_ORDER_H

struct FlowerOrder {
    int tulips;
    int daisies;
};

enum class WrapMaterial { Pink, Kraft, Cream, Green };
constexpr int FIRST_WRAP_TARGET = 64;
constexpr int WRAP_MATERIAL_COUNT = 4;

const FlowerOrder& orderRequested();
const FlowerOrder& orderPacked();
int orderNumber();
bool orderComplete();
bool orderIsPreparing();
bool orderDelivering();
const char* orderOccasion();
const char* orderRequestLine();
const char* orderWrapRequest();
const float* wrapMaterialColor(WrapMaterial material);
bool orderUsingMaterial(WrapMaterial material);
const char* orderMessage();
void orderAddSelected();
void orderClear();
void orderChooseMaterial(WrapMaterial material);
void orderWrapBouquet();
void orderNext();
void orderCancel(bool timedOut);
void orderDoorAction();
const char* orderDoorLabel();
bool orderDoorEnabled();
void updateOrderDisplay(float elapsed);
float orderDeliveryMotion();
void drawOrderBouquet();
void drawOrderFlowers(bool picking = false);

#endif
