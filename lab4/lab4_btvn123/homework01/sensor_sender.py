"""Gửi dữ liệu cảm biến giả lập lên Webhook.site mỗi 10 giây."""

import random
import time
from datetime import datetime

import requests

# Thay bằng URL Webhook.site của bạn nếu URL hiện tại hết hạn.
WEBHOOK_URL = "https://webhook.site/d46f3a6d-de17-4679-a649-ef20172f80c5"


def main():
    try:
        while True:
            timestamp = datetime.now().astimezone().isoformat(timespec="seconds")
            temperature = round(random.uniform(20, 40), 1)
            humidity = round(random.uniform(50, 90), 1)
            data = {
                "device_id": "python_sensor_01",
                "timestamp": timestamp,
                "temperature": temperature,
                "humidity": humidity,
            }

            print("\n--------------------------------")
            print(f"Thời gian: {timestamp}")
            print(f"Nhiệt độ: {temperature} °C")
            print(f"Độ ẩm: {humidity} %")
            print("Đang gửi dữ liệu...")
            try:
                response = requests.post(WEBHOOK_URL, json=data, timeout=10)
                print(f"HTTP Status: {response.status_code}")
                response.raise_for_status()
                print("Gửi thành công!")
            except requests.RequestException as error:
                print(f"Gửi thất bại: {error}")
            print("--------------------------------")
            time.sleep(10)
    except KeyboardInterrupt:
        print("\nĐã dừng chương trình.")


if __name__ == "__main__":
    main()
