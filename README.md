# FlowerShop3D-OpenGL
Interactive 3D Flower Shop Simulation using C++ and OpenGL/GLUT | Computer Graphics Course Project

## Chạy bản demo soạn đơn trên Windows

Cài GCC và FreeGLUT trong MSYS2 UCRT64, rồi mở terminal MSYS2 UCRT64 tại thư mục dự án.

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-freeglut
g++ -std=c++17 -Iinclude main.cpp src/bezier.cpp src/camera.cpp src/flowers.cpp src/interaction.cpp src/lighting.cpp src/model3d.cpp src/order.cpp src/people.cpp src/scene.cpp src/shop.cpp src/ui.cpp -o flower_shop.exe -lfreeglut -lopengl32 -lglu32
./flower_shop.exe
```

Trong VS Code, dùng **Ctrl+Shift+B** để build nếu thư mục `C:\msys64\ucrt64\bin` đã nằm trong PATH.

1. Bấm **Chậu trước** hoặc **Chậu sau** ở bảng soạn đơn để chọn hoa trên kệ. Có thể dùng phím `[` và `]`.
2. Bấm **Thêm vào đơn** hoặc nhấn **Enter**. Bản sao của chậu hoa xuất hiện trên bàn tròn.
3. Khi đã đủ hoa, bấm **Đơn tiếp** hoặc nhấn **Enter**. Bấm **Đặt lại** để làm lại đơn hiện tại.

Bản demo mở với tường trước ẩn để lộ bàn gói và các kệ. Nhấn **F** để hiện lại tường và biển hiệu.

Khách đi từ cửa tới quầy. Khi bạn thêm hoa, nhân viên cúi xuống bàn gói và chậu hạ xuống mặt bàn. Khi đơn đã đủ, khách vẫy tay. Bấm **Đơn tiếp** để khách nhận túi, rời tiệm và nhường chỗ cho khách mới.
