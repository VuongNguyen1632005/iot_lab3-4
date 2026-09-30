import requests

url = "https://webhook.site/01dad985-c613-4d32-ab79-df5b082e1283"

headers = {
    "Content-Type": "application/json",
    "Authorization": "Bearer token_123abc"
}

data = {
    "device_id": "sensor_01",
    "temperature": 25.5
}

response = requests.post(
    url,
    json=data,
    headers=headers
)

print("Status code:", response.status_code)
print("Response:", response.text)