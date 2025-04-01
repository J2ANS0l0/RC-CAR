#include <WiFi.h>
#include <WiFiUdp.h>

// Motor 1 Pins
#define motor1pin1 4
#define motor1pin2 5
#define pwm1Pin 18

// Motor 2 Pins
#define motor2pin1 12
#define motor2pin2 27
#define pwm2Pin 19

#define pwmChannel 0
#define pwmFrequency 5000
#define pwmResolution 8

// WiFi Config
const char* SSID = "";
const char* PASSWORD = "";

// UDP Config
WiFiUDP udp;
const int UDP_PORT = 12345;
const int PACKET_SIZE = 4;  // Esperando 4 bytes (R2, L2, JL, JR)

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

    pinMode(motor1pin1, OUTPUT);
    pinMode(motor1pin2, OUTPUT);
    pinMode(motor2pin1, OUTPUT);
    pinMode(motor2pin2, OUTPUT);
    ledcAttach(pwm1Pin, pwmFrequency, pwmResolution); // Método actualizado en ESP Core V3
    ledcAttach(pwm2Pin, pwmFrequency, pwmResolution); // Método actualizado en ESP Core V3
}

void loop() {
    int packetSize = udp.parsePacket();
    if (packetSize == PACKET_SIZE) {
        byte buffer[PACKET_SIZE];
        udp.read(buffer, PACKET_SIZE);

        // Extraer valores de los controles
        uint8_t L2 = buffer[1];     // Left Trigger (0-255)
        uint8_t R2 = buffer[0];     // Right Trigger (0-255)
        uint8_t JL = buffer[2];     // Joystick Left (0-255)
        uint8_t JR = buffer[3];     // Joystick Right (0-255)
        
        int dutyCycle1 = max(JL,JR);
        int dutyCycle2 = abs(L2-R2);

        bool r2 = R2 > 0;
        bool l2 = L2 > 0;
        bool jl = JL > 0;
        bool jr = JR > 0;

        digitalWrite(motor1pin1, jl);
        digitalWrite(motor1pin2, jr);
        ledcWrite(pwm1Pin, dutyCycle1);

        digitalWrite(motor2pin1, r2);
        digitalWrite(motor2pin2, l2);
        ledcWrite(pwm2Pin, dutyCycle2);
        
        // Imprimir valores recibidos
        //Serial.print("LT: ");
        //Serial.print(L2);
        //Serial.print(" | RT: ");
        //Serial.print(R2);
        //Serial.print(" | Jleft: ");
        //Serial.print(JL);
        //Serial.print(" | Jright: ");
        //Serial.println(JR);
    }
}