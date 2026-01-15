#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// Receiver MAC: 68:25:DD:32:38:08
uint8_t receiverMac[] = {0x68, 0x25, 0xDD, 0x32, 0x38, 0x08};

Adafruit_MPU6050 mpu;

typedef struct __attribute__((packed)) {
  float ax, ay, az;   // m/s^2
  uint32_t ms;        // millis timestamp
} AccelPacket;

AccelPacket pkt;

bool sendingEnabled = true;

// New ESP32 core (IDF 5.x) send callback signature
void onSent(const wifi_tx_info_t *tx_info, esp_now_send_status_t status) {
  // Optional debug:
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Send OK" : "Send FAIL");
}

void setup() {
  Serial.begin(115200);
  delay(300);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true, true);
  delay(100);

  Serial.print("Sender ESP32 MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.println("Press SPACE to toggle sending ON/OFF");

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    while (true) delay(1000);
  }
  esp_now_register_send_cb(onSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    while (true) delay(1000);
  }

  Wire.begin(); // SDA=21, SCL=22 (default)
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050. Check wiring/address.");
    while (true) delay(1000);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  Serial.println("Sender ready.");
}

void loop() {
  // If user types a space, toggle sending
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == ' ') {
      sendingEnabled = !sendingEnabled;
      Serial.print("Sending = ");
      Serial.println(sendingEnabled ? "ON" : "OFF");
    }
    // ignore other chars (like \r \n)
  }

  if (!sendingEnabled) {
    delay(20);
    return;
  }

  sensors_event_t a, g, t;
  mpu.getEvent(&a, &g, &t);

  pkt.ax = a.acceleration.x;
  pkt.ay = a.acceleration.y;
  pkt.az = a.acceleration.z;
  pkt.ms = millis();

  esp_err_t result = esp_now_send(receiverMac, (uint8_t*)&pkt, sizeof(pkt));
  if (result != ESP_OK) {
    Serial.print("Send error: ");
    Serial.println((int)result);
  }

  delay(20); // ~50 Hz
}
