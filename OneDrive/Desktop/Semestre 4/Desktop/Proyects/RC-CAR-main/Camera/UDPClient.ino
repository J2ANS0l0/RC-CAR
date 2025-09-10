#include <WiFi.h>
#include <WiFiUdp.h>

const char* SSID = "Philip";
const char* PASSWORD = "simedas100pesos";

WiFiUDP udp;
const char* PC_IP = "172.20.10.5";  // Replace with your computer's local IP
const int PC_PORT = 12345;  // Port on which Python script is listening
const int ESP_PORT = 12346; // ESP32's UDP port (not strictly needed)

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("\nConnected to WiFi");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  udp.begin(ESP_PORT);
}

void loop() {
  // Example: Send "Hello from ESP32" to the computer
  const char message[] = "Hello from ESP32";
  udp.beginPacket(PC_IP, PC_PORT);
  udp.write(reinterpret_cast<const uint8_t*>(message), sizeof(message));
  udp.endPacket();
  
  Serial.println("Message sent!");
  
  delay(33);  // Send every second (adjust as needed)
}