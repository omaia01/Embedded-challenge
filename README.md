# Embedded Challenge

## Project Structure

embedded-challenge/
│
├── qr-service/
│   ├── src/
│   ├── Dockerfile
│   └── CMakeLists.txt
│
├── gateway/
│   ├── app.py
│   ├── requirements.txt
│   ├── certs/
│   └── Dockerfile
│
├── docker-compose.yml
└── README.md

## How to Run

### 1. Start the fake serial device (host)
```bash
socat -d -d \
pty,raw,echo=0,link=/tmp/ttyS1 \
pty,raw,echo=0,link=/tmp/ttyS2
```

### 2. Start the application
```bash
docker compose up --build
```

You should see:

```
[INFO] Starting QR Service
Socket created successfully!
Socket bound to port 5000
Listening on port 5000...
Waiting for client...

Gateway started
[MQTT] Connected!
[MQTT] Subscribed to from_cloud/command
```

### 4. Connect using MQTT Explorer
Broker:
```
test.mosquitto.org
```

Port:
```
8883
```

Enable:
- TLS
- Disable certificate validation (or import the provided certificate)

### 5. Publish commands
Publish to:
```
from_cloud/command
```

Supported commands:
```
INIT
PING
START
STOP
```

Subscribe to:
```
from_device/events
```
Responses are published as JSON messages.

### 6. Stop
Press:
```
Ctrl + C
```

or

```bash
docker compose down
```

# Notes
The QR reader accesses the serial port through the `SERIAL_PORT` environment variable.

# Author
Bruno Maia