import pygame
import socket
import time
import struct

# Initialize pygame
pygame.init()

# Create a display surface (required for keyboard events)
screen = pygame.display.set_mode((400, 200))
pygame.display.set_caption("Keyboard Controller - Press ESC to exit")

# UDP Config
ESP_IP = "10.20.101.156"  # Change this to your ESP32's IP
ESP_PORT = 12345
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Keyboard state variables
keys_pressed = set()

def clamp(value, min_val, max_val):
    """Clamp a value between min and max."""
    return max(min_val, min(value, max_val))

def encode_keyboard_data():
    """
    Mapeo de teclas:
    - W/S: Right Trigger (RT) - Acelerar/Frenar
    - Q/E: Left Trigger (LT) - Función secundaria
    - A/D: Joystick Left/Right - Dirección
    """
     
    # Initialize values
    rt_value = 0
    lt_value = 0
    jleft = 0
    jright = 0
    
    # Right Trigger (RT) - W/S keys
    if pygame.K_w in keys_pressed:
        lt_value = 255  # Full forward
    elif pygame.K_s in keys_pressed:
        rt_value = 255  # Half reverse (or you can use 0 for no reverse)
    
    # Joystick Left/Right - A/D keys
    if pygame.K_a in keys_pressed:
        jright = 255  # Full left
    elif pygame.K_d in keys_pressed:
        jleft = 255  # Full right
    
    # Alternative: Use arrow keys
    if pygame.K_UP in keys_pressed:
        lt_value = 255
    elif pygame.K_DOWN in keys_pressed:
        rt_value = 255
    
    if pygame.K_LEFT in keys_pressed:
        jright = 255
    elif pygame.K_RIGHT in keys_pressed:
        jleft= 255
    
    # Print states for debugging
    print(f"RT: {rt_value}, LT: {lt_value}, Jleft: {jleft}, Jright: {jright}")
    
    # Pack into bytes (RT, LT, Jleft, Jright)
    data_bytes = struct.pack('<BBBB', rt_value, lt_value, jleft, jright)
    return data_bytes

def handle_events():
    """Handle pygame events and update key states."""
    global keys_pressed
    
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            return False
        elif event.type == pygame.KEYDOWN:
            keys_pressed.add(event.key)
            # Exit on ESC key
            if event.key == pygame.K_ESCAPE:
                return False
        elif event.type == pygame.KEYUP:
            keys_pressed.discard(event.key)
    
    return True

def update_display():
    """Update the display with current key states."""
    screen.fill((50, 50, 50))  # Dark gray background
    
    font = pygame.font.Font(None, 24)
    
    # Display instructions
    instructions = [
        "Keyboard Controls:",
        "W/↑: Accelerate (RT)",
        "S/↓: Brake (RT)",
        "A/←: Turn Left",
        "D/→: Turn Right",
        "Q: Left Trigger",
        "E: Left Trigger (half)",
        "ESC: Exit"
    ]
    
    y_offset = 10
    for instruction in instructions:
        color = (255, 255, 255)  # White text
        
        # Highlight active keys
        if "W" in instruction and pygame.K_w in keys_pressed:
            color = (0, 255, 0)  # Green
        elif "S" in instruction and pygame.K_s in keys_pressed:
            color = (255, 255, 0)  # Yellow
        elif "A" in instruction and pygame.K_a in keys_pressed:
            color = (0, 255, 255)  # Cyan
        elif "D" in instruction and pygame.K_d in keys_pressed:
            color = (255, 0, 255)  # Magenta
        elif "↑" in instruction and pygame.K_UP in keys_pressed:
            color = (0, 255, 0)  # Green
        elif "↓" in instruction and pygame.K_DOWN in keys_pressed:
            color = (255, 255, 0)  # Yellow
        elif "←" in instruction and pygame.K_LEFT in keys_pressed:
            color = (0, 255, 255)  # Cyan
        elif "→" in instruction and pygame.K_RIGHT in keys_pressed:
            color = (255, 0, 255)  # Magenta
        elif "Q" in instruction and pygame.K_q in keys_pressed:
            color = (255, 128, 0)  # Orange
        elif "E" in instruction and pygame.K_e in keys_pressed:
            color = (255, 128, 0)  # Orange
        
        text = font.render(instruction, True, color)
        screen.blit(text, (10, y_offset))
        y_offset += 25
    
    pygame.display.flip()

def main():
    clock = pygame.time.Clock()
    running = True
    
    try:
        while running:
            # Handle events
            running = handle_events()
            
            # Update display
            update_display()
            
            # Encode and send data
            data_bytes = encode_keyboard_data()
            sock.sendto(data_bytes, (ESP_IP, ESP_PORT))
            
            # Maintain 30Hz rate
            clock.tick(30)
            
    except KeyboardInterrupt:
        print("Exiting...")
    finally:
        sock.close()
        pygame.quit()

if __name__ == "__main__":
    main()