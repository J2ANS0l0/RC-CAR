#include "esp_camera.h"

#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

const char *ssid = "";
const char *password = "";

#define MAX_UDP_CHUNK 1024

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Iniciando...");

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 40;
  config.frame_size = FRAMESIZE_QQVGA;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Error al iniciar cámara: 0x%x\n", err);
    return;
  }

  Serial.println("Cámara iniciada correctamente");

  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  Serial.print("Conectando a WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado");
  Serial.print("IP local: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  Serial.println("🔄 Capturando imagen...");
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("❌ Error al capturar imagen.");
    delay(2000);
    return;
  }

  Serial.printf("📸 Imagen capturada: %u bytes\n", fb->len);

  if (fb->len > 3000) {
    Serial.println("⚠️ Imagen demasiado grande, omitiendo fragmentación.");
    esp_camera_fb_return(fb);
    delay(3000);
    return;
  }

  uint16_t packetId = random(0xFFFF);
  size_t offset = 0;
  size_t fragmentCount = 0;

  while (offset < fb->len && fragmentCount < 5) {  // máximo 5 fragmentos
    size_t chunkSize = min((size_t)MAX_UDP_CHUNK, fb->len - offset);

    // Simulamos el contenido del paquete UDP:
    Serial.printf("📦 Fragmento #%d\n", fragmentCount);
    Serial.printf(" - packetId: %u\n", packetId);
    Serial.printf(" - offset: %u\n", offset);
    Serial.printf(" - tamaño de datos: %u bytes\n", chunkSize);

    // Mostrar los primeros 16 bytes en hex (como preview del contenido)
    Serial.print(" - datos (hex): ");
    for (size_t i = 0; i < min((size_t)16, chunkSize); i++) {
      Serial.printf("%02X ", fb->buf[offset + i]);
    }
    Serial.println();

    offset += chunkSize;
    fragmentCount++;
    delay(1);
  }

  Serial.printf("✅ Fragmentación terminada (%d fragmentos)\n", fragmentCount);
  esp_camera_fb_return(fb);
  delay(3000);
}
