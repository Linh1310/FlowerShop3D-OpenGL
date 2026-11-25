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
void drawOrderFlowers();

#endif
