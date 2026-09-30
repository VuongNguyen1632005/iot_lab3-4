"""Gửi một mẫu JSON tới server Flask của Bài 2."""

import json
from datetime import datetime

import requests

SERVER_URL = "http://127.0.0.1:5000/api/sensor"

# Dữ liệu mẫu để server nhận và hiển thị.
data = {
    "timestamp": datetime.now().astimezone().isoformat(timespec="seconds"),
    "node_id": "python_node_01",
    "sensors": {
        "temperature": 28.5,
        "humidity": 70,
        "light_intensity": 650,
    },
    "battery_level": 90,
}


try:
    response = requests.post(SERVER_URL, json=data, timeout=5)
    print(f"HTTP Status: {response.status_code}")
    print(json.dumps(response.json(), indent=2))
except requests.RequestException as error:
    print(f"Không gửi được dữ liệu: {error}")
