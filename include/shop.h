#ifndef SHOP_H
#define SHOP_H

// =====================================================================
//  shop.h - Module cửa hàng (Thủy)
//
//  HỆ TỌA ĐỘ CHUNG (đề xuất cho cả nhóm):
//    - 1 đơn vị = 1 mét
//    - Trục Y hướng lên, sàn nằm ở y = 0
//    - Tâm cửa hàng ở gốc (0,0,0)
//    - X: ngang  [-5, +5]   Z: sâu  [-4, +4]   Y: cao [0, 3.5]
//    - Tường sau z = -4, cửa chính + cửa sổ ở tường trước z = +4
//    - Tường trái x = -5, quầy thanh toán sát tường phải x = +5
// =====================================================================

const float SHOP_WIDTH  = 10.0f;   // theo trục X
const float SHOP_DEPTH  = 8.0f;    // theo trục Z
const float SHOP_HEIGHT = 3.5f;    // theo trục Y

struct ShopPoint {
    float x, y, z;
};

// Gọi 1 lần sau glutCreateWindow() (tạo quadric, tính vị trí các ô kệ...)
void initShop();

// Gọi trong hàm display() của main.cpp. Hàm tự push/pop trạng thái GL
// nên không làm ảnh hưởng phần code của Mim và Trang.
void drawShop();

// Tương tác (Trang gắn phím): mở/đóng cửa, ẩn/hiện tường trước (nhìn cắt ngang)
void shopToggleDoor();
void shopToggleFrontWall();

// Các ô trên kệ để Mim đặt chậu hoa (scene.cpp):
// shopSlot(i) trả về tâm MẶT TRÊN của ván kệ, nơi đáy chậu đặt lên.
int       shopSlotCount();
ShopPoint shopSlot(int i);

// Vị trí các bóng đèn thả trần để Trang đặt nguồn sáng (lighting.cpp)
int       shopLampCount();
ShopPoint shopLampPos(int i);

#endif