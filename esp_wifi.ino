#include <WiFi.h>
#include <HTTPClient.h>


#include "esp_system.h"
#include "driver/adc.h"

String ssid[] = { "Mirza Emon", "Emon", "Bristy" };
int ssid_num = sizeof(ssid) / sizeof(ssid[0]);
String passwords[] = { "19712071", "emon1234", "bristy1234" };
int i = 0;


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

void wifi_check(String wifi_name[], String wifi_passwords[]) {
  int i = 0;

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Trying to connect to: ");
    Serial.println(wifi_name[i]);
    WiFi.begin(wifi_name[i].c_str(), wifi_passwords[i].c_str());

    unsigned long startAttemptTime = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 4000) {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nConnected!");
      break;
    } else {
      Serial.println("\nFailed, trying next network.");
      i++;
      if (i >= ssid_num) i = 0;
    }
  }
}


void setup() {
  Serial.begin(115200);
  wifi_check(ssid, passwords);


  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_0);  // ADC1 channel 6(GPIO34)
}

void loop() {


  if (WiFi.status() != WL_CONNECTED) {
    wifi_check(ssid, passwords);
  }


  int hallValue = hallRead();
  float temperature = readInternalTemp();


  sendData(hallValue, temperature);  // POST data to Google Sheet
  getData();                         // GET only F1 cell data

  delay(1000);  // wait 5 seconds before next loop
}
