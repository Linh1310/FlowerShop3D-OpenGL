#ifndef FLOWER_SHOP_PEOPLE_H
#define FLOWER_SHOP_PEOPLE_H

constexpr float CUSTOMER_WAIT_LIMIT = 35.0f;
constexpr float CUSTOMER_HEART_LIMIT = 12.0f;

void updatePeople(float elapsed);
void drawPeople();
void staffPackFlower();
bool customerIsOrdering();
bool customerCanEnter();
bool customerIsWaiting();
float customerWaitElapsed();
float customerDoorAngle();
int customerHearts();
void customerEnter();
void customerOrderReady();
void customerCollectOrder();
void customerDismiss(bool angry);

#endif
