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

1. Đọc lời nhắn của khách rồi bấm chuột phải vào hoa trên kệ để thêm. Bạn cũng có thể bấm trái để chọn rồi bấm **Thêm hoa**.
2. Bấm vào mẫu giấy và cuộn nơ trên bàn hoặc chọn màu tương ứng trên phiếu. Bấm **Gói hoa** khi đã lấy đủ hoa. Giấy sẽ gập lên quanh bó và nhân viên thắt nơ.
3. Bấm **Giao hoa** khi khách đã tới quầy. Nhân viên mang bó tới giao và khách cầm hoa ra về. Bấm **Gọi khách vào** để tiếp khách mới.
4. Khách thả tim nếu bạn chuẩn bị đúng trong 22 giây kể từ khi họ tới quầy. Sau 55 giây chờ, họ hủy đơn và rời tiệm.
5. Bấm **Điều khiển** để mở danh sách nút camera, ánh sáng và chế độ xem. Cuộn chuột để phóng to hoặc thu nhỏ.

Bản demo mở với tường trước ẩn để lộ bàn gói và các kệ. Nhấn **F** để hiện lại tường, biển hiệu và xem cửa tự mở khi khách đi qua.
