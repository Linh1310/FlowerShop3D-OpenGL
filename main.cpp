#include <GL/freeglut.h>
#include "ui.h"  
#include "camera.h"
#include "lighting.h"
#include "interaction.h"
#include "shop.h"
#include "scene.h"
#include "model3d.h"

static void display() {
        if (inspectActive()) glClearColor(0.08f, 0.08f, 0.12f, 1.0f);   // nen toi cho de nhin luoi
    else                 applySkyColor();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    applyCamera();
    applyLighting();

    if (inspectActive() && sceneSelectedIndex() < 0) inspectOff();   // bo chon -> thoat che do xem

    if (inspectActive()) {
        drawInspect(sceneSelectedFlowerType(), sceneSelectedAngle());   // chi ve bong hoa dang chon
    } else {
        drawShop();
        drawFlowerScene();
    }

    drawUI();
    glutSwapBuffers();
}

static void reshape(int w, int h) { cameraReshape(w, h); uiReshape(w, h);   }

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 700);
    glutCreateWindow("Flower Shop 3D");

    glEnable(GL_DEPTH_TEST);                 // khử mặt khuất
    glClearColor(0.75f, 0.85f, 0.95f, 1.0f);

    initShop();                              // phải sau glutCreateWindow
    initLighting();
    cameraInit();
    printFlowerSceneLayout();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(onKeyboard);
    glutSpecialFunc(onSpecialKey);
    glutMouseFunc(onMouse);
    glutMainLoop();
    return 0;
}