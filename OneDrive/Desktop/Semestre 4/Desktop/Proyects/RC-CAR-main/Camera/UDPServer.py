import socket

PC_IP = "0.0.0.0"  # Listen on all available interfaces
PC_PORT = 12345  # Must match the ESP32's destination port

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((PC_IP, PC_PORT))

print(f"Listening for UDP packets on port {PC_PORT}...")

while True:
    data, addr = sock.recvfrom(1024)  # Buffer size: 1024 bytes
    print(f"Received from {addr}: {data.decode('utf-8')}")