#include <Arduino.h>
#include <WiFi.h>

// Khai báo SSID và Password WiFi của bạn
const char* ssid     = "Wokwi-GUEST";
const char* password = ""; // Để trống mật khẩu

void setup() {
  // Khởi tạo Serial Monitor với baud rate 115200
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n--- DONG BO WIFI STATION ---");

  // 1. Cấu hình chế độ Station (STA)
  WiFi.mode(WIFI_STA);

  // 2. Bắt đầu kết nối WiFi với SSID và Password đã khai báo
  WiFi.begin(ssid, password);
  Serial.print("Dang ket noi toi WiFi: ");
  Serial.println(ssid);

  // 3. Chờ kết nối thành công (In dấu '.' trong quá trình chờ)
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // 4. In thông báo thành công và địa chỉ IP nhận được
  Serial.println("");
  Serial.println("-> Ket noi WiFi thanh cong!");
  Serial.print("Dia chi IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Trong bài học cơ bản này, loop() không cần thực hiện thêm tác vụ
}