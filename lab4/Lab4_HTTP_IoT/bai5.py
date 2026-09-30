import requests

url = "https://webhook.site/01dad985-c613-4d32-ab79-df5b082e1283"

data = {
    "device_id": "sensor_01",
    "temperature": 25.5
}

try:
    response = requests.post(
        url,
        json=data,
        timeout=3
    )
    response.raise_for_status()
    print("Gửi request thành công!")
    print("Status code:", response.status_code)
    print("Response:", response.text)

except requests.exceptions.Timeout:
    print("Lỗi Timeout: Thời gian Server phản hồi quá 3s ")

except requests.exceptions.ConnectionError:
    print("Lỗi Connection: Không thể kết nối đến server")

except requests.exceptions.RequestException as e:
    print("Lỗi Request:", e)