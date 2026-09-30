"""Server Flask nhận JSON cảm biến trên máy tính."""

import json
from flask import Flask, jsonify, request

app = Flask(__name__)


@app.post("/api/sensor")
def receive_sensor():
    # Đọc và in JSON để quan sát ngay trên terminal của server.
    data = request.get_json(silent=True)
    if not isinstance(data, dict):
        return jsonify(status="error", message="JSON không hợp lệ"), 400

    print("\nDa nhan du lieu:")
    print(json.dumps(data, ensure_ascii=False, indent=2))
    return jsonify(
        status="success",
        message="Data received successfully",
    ), 200


if __name__ == "__main__":
    app.run(host="127.0.0.1", port=5000, debug=False)
