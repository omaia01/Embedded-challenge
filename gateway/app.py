# External libraries
import socket
import json
import os
import ssl
import paho.mqtt.client as mqtt

# Environment variables
MQTT_HOST = os.getenv("MQTT_HOST", "test.mosquitto.org")
MQTT_PORT = int(os.getenv("MQTT_PORT", "8883"))

MQTT_TOPIC_CMD = os.getenv(
    "MQTT_TOPIC_CMD",
    "from_cloud/command"
)

MQTT_TOPIC_EVENT = os.getenv(
    "MQTT_TOPIC_EVENT",
    "from_device/events"
)

MQTT_CERT = os.getenv(
    "MQTT_CERT",
    "./certs/mosquitto.org.crt"
)

TCP_HOST = os.getenv("QR_HOST", "localhost")
TCP_PORT = int(os.getenv("QR_PORT", "5000"))


# MQTT Client
client = mqtt.Client(
    mqtt.CallbackAPIVersion.VERSION2
)

# TLS configuration
client.tls_set(
    ca_certs=MQTT_CERT,
    certfile=None,
    keyfile=None,
    tls_version=ssl.PROTOCOL_TLS_CLIENT
)


def on_connect(client, userdata, flags, reason_code, properties):
    """ Callback function for MQTT connection event """
    if reason_code == 0:
        print("[MQTT] Connected!")
        client.subscribe(MQTT_TOPIC_CMD)
        print(f"[MQTT] Subscribed to {MQTT_TOPIC_CMD}")

    else:
        print(f"[MQTT] Failed ({reason_code})")


def on_message(client, userdata, message):
    """ Callback function for MQTT message event """
    command = message.payload.decode().strip()
    print(f"[MQTT] Received: {command}")
    response = send_command(command)

    if response is not None:
        client.publish(
            MQTT_TOPIC_EVENT,
            json.dumps(response)
        )
        print(f"[MQTT] Published: {response}")


def send_command(command: str):
    """ Send a command to the QR service via TCP and return the response """
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client:

            client.connect((TCP_HOST, TCP_PORT))
            print(f"[TCP] Connected to {TCP_HOST}:{TCP_PORT}")
            client.sendall((command + "\n").encode())

            response = client.recv(4096).decode().strip()

            print("[TCP] Raw response:")
            print(response)

            return json.loads(response)

    except Exception as e:
        print(f"[ERROR] {e}")
        return None


def main():
    """ Start the gateway """
    print("Gateway started")

    client.on_connect = on_connect
    client.on_message = on_message

    client.connect( MQTT_HOST, MQTT_PORT, 60)
    client.loop_forever()


if __name__ == "__main__":
    main()