#ifndef UI_H
#define UI_H

void uiReshape(int w, int h);                       // gọi trong reshape()
void drawUI();                                      // gọi cuối display(), trước swap
void onMouse(int button, int state, int x, int y);  // glutMouseFunc
void onMouseMove(int x, int y);                     // glutPassiveMotionFunc (hiệu ứng hover)

#endif