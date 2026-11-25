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

1. Bấm chuột trái trực tiếp vào hoa hoặc chậu trên kệ để chọn, rồi bấm **Thêm vào đơn**. Bấm chuột phải vào hoa để chọn và thêm ngay. Cuộn chuột để phóng to hoặc thu nhỏ.
2. Khách thả tim nếu bạn soạn xong trước khi họ chờ quá 12 giây. Sau 35 giây chờ ở quầy, khách nổi giận, hủy đơn và bỏ về.
3. Bấm **Giao đơn / Tiễn khách** khi đủ hoa. Bạn cũng có thể bấm **Tiễn khách ra** để hủy một đơn đang soạn. Khi khách cũ đã ra cửa, bấm **Gọi khách vào** để đón khách tiếp theo.
4. Phím `[` và `]` vẫn chọn chậu. **Enter** thêm hoa hoặc giao đơn đã hoàn tất.

Bản demo mở với tường trước ẩn để lộ bàn gói và các kệ. Nhấn **F** để hiện lại tường, biển hiệu và xem cửa tự mở khi khách đi qua.

Khi bạn thêm hoa, nhân viên cúi xuống bàn gói và chậu hạ xuống mặt bàn. Khi đơn đã đủ, khách vẫy tay và nhận túi lúc bạn giao đơn.
