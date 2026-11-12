#include <GL/freeglut.h>
#include <cstdlib>
#include "interaction.h"
#include "camera.h"
#include "lighting.h"
#include "shop.h"
#include "scene.h"
#include "model3d.h"

void cycleInspectMode() {
    if (!inspectActive() && sceneSelectedIndex() < 0) sceneSelectStep(+1);   // chua chon thi chon chau dau
    bool wasOff = !inspectActive();
    inspectCycleMode();
    if (wasOff) cameraSetDistance(5.0f);               // keo camera lai gan bong hoa
    else if (!inspectActive()) cameraSetDistance(11.0f); // ve lai canh cua hang
}

void onKeyboard(unsigned char key, int, int) {
    switch (key) {
        case '+': case '=': cameraZoom(-1.0f); break;
        case '-': case '_': cameraZoom(+1.0f); break;

        // Chọn chậu trước / sau
        case '[': sceneSelectStep(-1); break;
        case ']': sceneSelectStep(+1); break;

        // Xoay chậu đang chọn
        case 'a': case 'A': sceneRotateSelected(-10.0f); break;
        case 'd': case 'D': sceneRotateSelected(+10.0f); break;

        // Phóng to / thu nhỏ chậu đang chọn
        case 'w': case 'W': sceneScaleSelected(+0.1f); break;
        case 'q': case 'Q': sceneScaleSelected(-0.1f); break;

        case 'l': case 'L': toggleLighting();          break;
        case 's': case 'S': toggleShading();           break;   // Flat/Smooth
        case 'p': case 'P': cameraTogglePerspective(); break;
        case 'o': case 'O': shopToggleDoor();          break;
        case 'f': case 'F': shopToggleFrontWall();     break;   // nhìn cắt ngang
        case 'r': case 'R': cameraReset(); sceneResetSelection(); break;

        case 'm': case 'M': cycleInspectMode();      break;
        case 'v': case 'V': inspectToggleLabels(); 
                case 'n': case 'N': toggleDayNight();          break;
                
        break;

        case 27:            exit(0);
    }
    glutPostRedisplay();
}

void onSpecialKey(int key, int, int) {
    switch (key) {
        case GLUT_KEY_LEFT:  cameraRotate(-5.0f, 0.0f); break;
        case GLUT_KEY_RIGHT: cameraRotate(+5.0f, 0.0f); break;
        case GLUT_KEY_UP:    cameraRotate(0.0f, +5.0f); break;
        case GLUT_KEY_DOWN:  cameraRotate(0.0f, -5.0f); break;
    }
    glutPostRedisplay();
}