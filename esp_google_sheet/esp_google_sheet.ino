#include <WiFi.h>
#include <HTTPClient.h>

#include "esp_system.h"
#include "driver/adc.h"


const char* ssid[] = { "Mirza Emon", "Emon","Bristy" };
const char* password[] = { "19712071", "emon1234","bristy1234" };



const char* serverName = "https://script.google.com/macros/s/AKfycbzTPcWhxWVlC7stLNDygWaCrzKDeKFVAMwyRVsCHoT6X_H9trhBc3JnD65WqJLismZo/exec";

String deviceID = "ESP32_Emon";

// Send temperature and humidity data (POST)
void sendData(float temp, float hall_data) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    String jsonData = "{\"device\":\"" + deviceID + "\",\"temperature\":" + String(temp) + ",\"humidity\":" + String(hall_data) + "}";

    int httpResponseCode = http.POST(jsonData);
    Serial.print("Send Data - HTTP Response code: ");
    Serial.println(httpResponseCode);

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

// Get F1 cell value from Google Sheet (GET)
void getData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);  // GET request

    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String payload = http.getString();
      Serial.println("Received Data: " + payload);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}


float readInternalTemp() {
  int adcVal = adc1_get_raw(ADC1_CHANNEL_6);
  float voltage = (adcVal / 4095.0) * 3.3;  // 12-bit ADC, 3.3V ref

  // Approximate formula (depends on chip, just rough)
  float temperature = (voltage - 0.5) * 100.0;  // °C

  return temperature;
}


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid[0], password[0]);

  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_0);  // ADC1 channel 6(GPIO34)

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected!");
}

void loop() {
  // Test values (random)

  int hallValue = hallRead();
  float temperature = readInternalTemp();



  sendData(hallValue, temperature);  // POST data to Google Sheet
  getData();                         // GET only F1 cell data

  delay(1000);  // wait 5 seconds before next loop
}
