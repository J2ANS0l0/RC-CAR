import pygame
import socket
import time
import struct

# Initialize pygame
pygame.init()
pygame.joystick.init()

# Open first available controller
controller = pygame.joystick.Joystick(0)
controller.init()
print("Controller Name:", controller.get_name())  # Confirm controller identity

# UDP Config 192.168.67.126
ESP_IP = "10.20.101.156"  # Change this to your ESP32's IP
ESP_PORT = 12345
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def clamp(value, min_val, max_val):
    """Clamp a value between min and max."""
    return max(min_val, min(value, max_val))

def encode_controller_data():
    pygame.event.pump()  # Update controller state

    # Get trigger states (DualSense may return -1.0 to 1.0)
    right_trigger = controller.get_axis(pygame.CONTROLLER_AXIS_TRIGGERRIGHT)  # RT
    left_trigger = controller.get_axis(pygame.CONTROLLER_AXIS_TRIGGERLEFT)   # LT
    
    # Get left joystick x-axis (-1.0 to 1.0)
    left_stick_x = controller.get_axis(pygame.CONTROLLER_AXIS_LEFTX)
    
    # Convert triggers to 0-255 (properly rounded and clamped)
    rt_value = clamp(round((right_trigger + 1.0) * 127.5), 0, 255)
    lt_value = clamp(round((left_trigger + 1.0) * 127.5), 0, 255)
    
    # Convert joystick to separate left/right values (0-255)
    # Left range (when stick is left: 0-255, right: 0)
    jleft = clamp(round((-left_stick_x + 1.0) * 127.5), 0, 255) if left_stick_x < 0 else 0
    # Right range (when stick is right: 0-255, left: 0)
    jright = clamp(round((left_stick_x + 1.0) * 127.5), 0, 255) if left_stick_x > 0 else 0

    # Print states for debugging
    print(f"RT: {rt_value}, LT: {lt_value}, Jleft: {jleft}, Jright: {jright}")

    # Pack into bytes (RT, LT, Jleft, Jright)
    data_bytes = struct.pack('<BBBB', rt_value, lt_value, jleft, jright)
    return data_bytes

def main():
    try:
        while True:
            data_bytes = encode_controller_data()
            # Send UDP packet
            sock.sendto(data_bytes, (ESP_IP, ESP_PORT))
            # Sleep to avoid flooding
            time.sleep(0.033) # 30Hz - Parecido a videojuegos segun ChatGPT
    except KeyboardInterrupt:
        print("Exiting...")
    finally:
        sock.close()
        pygame.quit()

if __name__ == "__main__":
    main()