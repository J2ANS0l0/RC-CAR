#define pwm1Pin 18  // Cambia el pin según tu conexión
#define pwmChannel 0
#define pwmFrequency 5000
#define pwmResolution 8

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  ledcAttach(pwmPin, pwmFrequency, pwmResolution); // Método actualizado en ESP Core V3
}

void loop() {
  int dutyCycle = 100; // Ajusta el valor (0-255)
  ledcWrite(pwmPin, dutyCycle);
}