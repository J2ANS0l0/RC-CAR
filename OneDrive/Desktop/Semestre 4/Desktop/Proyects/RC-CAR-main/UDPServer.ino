#include <WiFi.h>
#include <WiFiUdp.h>

// WiFi Config
const char* SSID = "";
const char* PASSWORD = "";

// UDP Config
WiFiUDP udp;
const int UDP_PORT = 12345;
const int PACKET_SIZE = 4;  // Esperando 4 bytes (RT, LT, Jleft, Jright)

void setup() {
    Serial.begin(115200);

    // Conectar a WiFi
    WiFi.begin(SSID, PASSWORD);
    Serial.print("Conectando a WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println("\nConectado a WiFi");
    Serial.print("ESP32 IP Address: ");
    Serial.println(WiFi.localIP());

    udp.begin(UDP_PORT);
    Serial.print("Escuchando en el puerto UDP ");
    Serial.println(UDP_PORT);
}

void loop() {
    int packetSize = udp.parsePacket();
    if (packetSize == PACKET_SIZE) {
        byte buffer[PACKET_SIZE];
        udp.read(buffer, PACKET_SIZE);

        // Extraer valores de los controles
        uint8_t rt_value = buffer[0];  // Right Trigger (0-255)
        uint8_t lt_value = buffer[1];  // Left Trigger (0-255)
        uint8_t jleft = buffer[2];     // Joystick Left (0-255)
        uint8_t jright = buffer[3];    // Joystick Right (0-255)

        // Imprimir valores recibidos
        Serial.print("RT: ");
        Serial.print(rt_value);
        Serial.print(" | LT: ");
        Serial.print(lt_value);
        Serial.print(" | Jleft: ");
        Serial.print(jleft);
        Serial.print(" | Jright: ");
        Serial.println(jright);
    }
}
