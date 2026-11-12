#ifndef CAMERA_H
#define CAMERA_H

void cameraInit();
void applyCamera();
void cameraReshape(int w, int h);
void cameraRotate(float dYaw, float dPitch);
void cameraZoom(float delta);
void cameraSetDistance(float d);
void cameraReset();
void cameraNextProjection();        // phoi canh -> truc giao -> xien
void cameraTogglePerspective();     // giu ten cu, goi cameraNextProjection

bool        isPerspective();
float       cameraYaw();            // theta
float       cameraPitch();          // phi
float       cameraDistance();       // R
const char* cameraProjectionName();
const char* cameraProjectionFormula();

#endif