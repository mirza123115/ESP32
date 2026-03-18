#include <WiFi.h>
#include "time.h"

const char* ssid = "Mirza Emon";
const char* password = "19712071";

const char* ntpServer = "time.google.com";
const long gmtOffset_sec = 21600;  // UTC+6

void setup() {
  Serial.begin(115200);      // ESP32 Serial Monitor
  // Serial2 TX → Arduino D0 (RX0)
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX2=16 (ESP32 side, optional), TX2=17 → Arduino D0

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  configTime(gmtOffset_sec, 0, ntpServer);
}

void loop() {
  struct tm timeinfo;

  if(!getLocalTime(&timeinfo)){
    return;
  }

  char buffer[32];

  // 12-hour format + AM/PM + Day + Date
  strftime(buffer, sizeof(buffer), "%I:%M:%S %p, %a, %d-%m-%Y", &timeinfo);

  Serial2.println(buffer);   // Arduino RX0/D0 তে পাঠাচ্ছে
  1    // ESP32 Serial Monitor তে দেখাবে

  delay(1000);
}