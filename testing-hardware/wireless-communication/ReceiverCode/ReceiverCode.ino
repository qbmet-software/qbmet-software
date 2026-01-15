//RECIEVER CODE

#include <WiFi.h>
#include <esp_now.h>

typedef struct __attribute__((packed)) {
  float ax, ay, az;   // m/s^2
  uint32_t ms;        // millis timestamp from sender
} AccelPacket;

AccelPacket pkt;

void onRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  if (len != (int)sizeof(AccelPacket)) return;

  memcpy(&pkt, data, sizeof(pkt));

  char macStr[18];
  snprintf(macStr, sizeof(macStr),
           "%02X:%02X:%02X:%02X:%02X:%02X",
           info->src_addr[0], info->src_addr[1], info->src_addr[2],
           info->src_addr[3], info->src_addr[4], info->src_addr[5]);

  Serial.print("From ");
  Serial.print(macStr);
  Serial.print(" | t=");
  Serial.print(pkt.ms);
  Serial.print(" ms | ax=");
  Serial.print(pkt.ax, 3);
  Serial.print(" ay=");
  Serial.print(pkt.ay, 3);
  Serial.print(" az=");
  Serial.println(pkt.az, 3);
}

void setup() {
  Serial.print("TEST");
  Serial.begin(115200);
  delay(300);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true, true);
  delay(100);

  Serial.print("Receiver ESP32 MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    while (true) delay(1000);
  }

  esp_now_register_recv_cb(onRecv);

  Serial.println("Receiver ready. Waiting for accel packets...");
}

void loop() {
  delay(1000);
}
