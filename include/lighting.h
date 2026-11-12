#ifndef LIGHTING_H
#define LIGHTING_H

void initLighting();       // gọi 1 lần sau glutCreateWindow()
void applyLighting();      // gọi mỗi frame, SAU gluLookAt
void toggleLighting();     // bật/tắt đèn
void toggleShading();      // Flat <-> Smooth (Gouraud)
void toggleDayNight();     // ngày <-> đêm

bool isLightingOn();
bool isSmoothOn();
bool isNightMode();
void applySkyColor();      // dat lai mau nen theo ngay/dem
#endif