// =====================================================================
//  shop_test.cpp - chương trình thử RIÊNG cho module shop (của Thủy).
//  Dùng để xem cửa hàng khi chưa có camera/ánh sáng của Trang.
//  KHÔNG đổi tên thành main.cpp và không thay main.cpp của Trang.
//
//  Phím: mũi tên = xoay | + / - = zoom | D = mở/đóng cửa
//        F = ẩn/hiện tường trước | L = bật/tắt đèn | ESC = thoát
// =====================================================================
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include "shop.h"

static float g_yaw = 35.0f, g_pitch = 30.0f, g_dist = 10.0f;
static bool  g_light = true;

static void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    const float PI = 3.14159265f;
    float ry = g_yaw * PI / 180.0f, rp = g_pitch * PI / 180.0f;
    float ex = g_dist * cosf(rp) * sinf(ry);
    float ey = 1.2f + g_dist * sinf(rp);
    float ez = g_dist * cosf(rp) * cosf(ry);
    gluLookAt(ex, ey, ez,   0.0, 1.2, 0.0,   0.0, 1.0, 0.0);

    // đèn tạm (sau này dùng lighting.cpp của Trang + shopLampPos)
    GLfloat pos[] = {0.0f, 3.2f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, pos);

    drawShop();
    glutSwapBuffers();
}

static void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(55.0, (double)w / h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

static void keyboard(unsigned char k, int, int) {
    switch (k) {
        case 27: exit(0); break;
        case '+': case '=': g_dist -= 0.5f; if (g_dist < 2) g_dist = 2; break;
        case '-': g_dist += 0.5f; break;
        case 'd': case 'D': shopToggleDoor(); break;
        case 'f': case 'F': shopToggleFrontWall(); break;
        case 'l': case 'L':
            g_light = !g_light;
            if (g_light) glEnable(GL_LIGHTING); else glDisable(GL_LIGHTING);
            break;
    }
    glutPostRedisplay();
}

static void special(int k, int, int) {
    switch (k) {
        case GLUT_KEY_LEFT:  g_yaw -= 5; break;
        case GLUT_KEY_RIGHT: g_yaw += 5; break;
        case GLUT_KEY_UP:    g_pitch += 5; if (g_pitch > 85) g_pitch = 85; break;
        case GLUT_KEY_DOWN:  g_pitch -= 5; if (g_pitch < 5) g_pitch = 5; break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(960, 640);
    glutCreateWindow("Shop test - Thuy");

    glClearColor(0.78f, 0.72f, 0.85f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat amb[]  = {0.35f, 0.35f, 0.35f, 1.0f};
    GLfloat diff[] = {0.90f, 0.90f, 0.85f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);

    initShop();
    shopToggleFrontWall();   // mặc định ẩn tường trước để nhìn thấy bên trong

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMainLoop();
    return 0;
}