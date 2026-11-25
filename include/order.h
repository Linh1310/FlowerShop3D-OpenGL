#ifndef FLOWER_ORDER_H
#define FLOWER_ORDER_H

struct FlowerOrder {
    int tulips;
    int daisies;
};

const FlowerOrder& orderRequested();
const FlowerOrder& orderPacked();
int orderNumber();
bool orderComplete();
const char* orderMessage();
void orderAddSelected();
void orderClear();
void orderNext();
void orderCancel(bool timedOut);
void orderDoorAction();
const char* orderDoorLabel();
bool orderDoorEnabled();
void updateOrderDisplay(float elapsed);
void drawOrderFlowers();

#endif
