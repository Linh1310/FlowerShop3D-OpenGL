# FlowerShop3D-OpenGL
Interactive 3D Flower Shop Simulation using C++ and OpenGL/GLUT | Computer Graphics Course Project

## Chạy bản demo soạn đơn trên Windows

Cài GCC và FreeGLUT một lần trong terminal MSYS2 UCRT64.

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-freeglut
```

Sau khi lấy nhánh `feature/flower-shop-wrapping-demo`, mở PowerShell hoặc terminal VS Code tại thư mục chứa mã nguồn và chạy:

```powershell
.\run_demo.cmd
```

Script build và mở `flower_shop_wrapping.exe`. Bạn cũng có thể nhấn **Ctrl+Shift+B** trong VS Code để chỉ build. Nếu MSYS2 nằm ngoài `C:\msys64`, dùng terminal UCRT64 hoặc đưa thư mục `ucrt64\bin` vào PATH trước khi chạy.

1. Đọc lời nhắn của khách rồi bấm chuột phải vào hoa trên kệ để thêm. Bạn cũng có thể bấm trái để chọn rồi bấm **Thêm hoa**.
2. Bấm vào mẫu giấy và cuộn nơ trên bàn hoặc chọn màu tương ứng trên phiếu. Bấm **Gói hoa** khi đã lấy đủ hoa. Giấy sẽ gập lên quanh bó và nhân viên thắt nơ.
3. Bấm **Giao hoa** khi khách đã tới quầy. Nhân viên mang bó tới giao và khách cầm hoa ra về. Bấm **Gọi khách vào** để tiếp khách mới.
4. Khách thả tim nếu bạn chuẩn bị đúng trong 22 giây kể từ khi họ tới quầy. Sau 55 giây chờ, họ hủy đơn và rời tiệm.
5. Bấm **Điều khiển** để mở danh sách nút camera, ánh sáng và chế độ xem. Cuộn chuột để phóng to hoặc thu nhỏ.

Bản demo mở với tường trước ẩn để lộ bàn gói và các kệ. Nhấn **F** để hiện lại tường, biển hiệu và xem cửa tự mở khi khách đi qua.
