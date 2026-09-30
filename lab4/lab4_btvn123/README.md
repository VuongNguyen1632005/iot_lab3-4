# LAB 4

Cài Python và bật **Add Python to PATH**. Cài thư viện một lần: `python -m pip install requests flask`

## Bài 1

Mở thư mục `homework01` trong Terminal:

```powershell
python sensor_sender.py
```

Kết quả: gửi nhiệt độ và độ ẩm lên Webhook.site mỗi 10 giây. Dừng bằng `Ctrl+C`. Nếu URL hết hạn, sửa `WEBHOOK_URL` trong code Bài 1 và Bài 3.

---

## Bài 2

Mở `homework02` trong Terminal.

Terminal 1:

```powershell
python app.py
```

Terminal 2:

```powershell
python send_sensor.py
```

Kết quả: Flask nhận JSON và trả HTTP 200.

---

## Bài 3

Mở riêng thư mục `homework03` trong VS Code.

1. Chọn **PlatformIO: Build**.
2. Chọn **Wokwi: Start Simulator**.
3. Nhấn **PLAY**.
4. Xem **Serial Monitor** và Webhook.site.
