#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

bool deviceConnected = false;
unsigned long lastCheckTime = 0;

String dataBuffer = "";
char currentTag = '\0';

int aValue = 0;
int bValue = 0;


int ledPin1 = 2;      // প্রথম LED পিন
int ledPin2 = 4;      // দ্বিতীয় LED পিন
int pwmChannel1 = 0;  // প্রথম চ্যানেল
int pwmChannel2 = 1;  // দ্বিতীয় চ্যানেল


int freq = 200;
int resolution = 8;


const int buttonPins[10] = { 13, 14, 16, 17, 18, 19, 26, 27, 32, 33 };
const int pinX = 23;



void setup() {

  Serial.begin(115200);
  SerialBT.begin("ESP32_Mirza_Emon");

  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(pinX, OUTPUT);

  digitalWrite(pinX, LOW);


  Serial.println("✅ Bluetooth Ready. Waiting for device...");
  Serial.println("A\tB");

  ledcSetup(pwmChannel1, freq, resolution);
  ledcAttachPin(ledPin1, pwmChannel1);

  ledcSetup(pwmChannel2, freq, resolution);
  ledcAttachPin(ledPin2, pwmChannel2);

  for (int i = 0; i < 10; i++) {
    pinMode(buttonPins[i], OUTPUT);
    digitalWrite(buttonPins[i], LOW);
  }
}


void loop() {
  // connection check every 500ms
  if (millis() - lastCheckTime >= 500) {
    lastCheckTime = millis();
    if (SerialBT.hasClient() && !deviceConnected) {
      deviceConnected = true;
      Serial.println("🔗 Bluetooth device connected!");
    } else if (!SerialBT.hasClient() && deviceConnected) {
      deviceConnected = false;
      Serial.println("❌ Bluetooth device disconnected!");
    }
  }

  while (SerialBT.available()) {
    char c = SerialBT.read();
    Serial.print("Recv char: '");
    Serial.print(c);
    Serial.println("'");  // Debug: দেখাবে প্রতিটি ক্যারেক্টার আসছে কি না

    if (c == 'A' || c == 'B' || c == 'n') {
      // আগের ডাটা প্রসেস করো, যদি থাকে
      if (dataBuffer.length() > 0 && currentTag != '\0') {
        Serial.print("Processing dataBuffer: ");
        Serial.print(dataBuffer);
        Serial.print(" with tag ");
        Serial.println(currentTag);

        processData();
      }

      currentTag = c;
      dataBuffer = "";

    } else if (c == '\n' || c == '\r') {
      if (dataBuffer.length() > 0 && currentTag != '\0') {
        Serial.print("End of line detected. Processing dataBuffer: ");
        Serial.print(dataBuffer);
        Serial.print(" with tag ");
        Serial.println(currentTag);

        processData();

        currentTag = '\0';
        dataBuffer = "";
      }
    } else {
      dataBuffer += c;
    }
  }

  // Optional: Serial Monitor থেকে Bluetooth এ ডাটা পাঠানো
  while (Serial.available()) {
    char c = Serial.read();
    SerialBT.write(c);
    Serial.print("Sent char: '");
    Serial.print(c);
    Serial.println("'");
  }
}

void processData() {

  int value = dataBuffer.toInt();

  if (currentTag == 'A') {
    aValue = value;
  } else if (currentTag == 'B') {
    bValue = value;
  } else if (currentTag == 'n') {
    if (dataBuffer == "x") {
      digitalWrite(pinX, HIGH);
      Serial.println("Pin 23 HIGH (x received)");
    } else if (dataBuffer == "y") {
      digitalWrite(pinX, LOW);
      Serial.println("Pin 23 LOW (y received)");
    } else {
      int btn = dataBuffer.toInt();
      if (btn >= 1 && btn <= 10) {
        int index = btn - 1;
        int state = digitalRead(buttonPins[index]);
        digitalWrite(buttonPins[index], !state);
        Serial.print("Toggled pin for n");
        Serial.println(btn);
      }
    }
  } else {
    Serial.print("Button ");
    Serial.print(dataBuffer);
    Serial.println(" pressed");
  }


  if (currentTag == 'A' || currentTag == 'B') {
    Serial.print("A: ");
    Serial.print(aValue);
    Serial.print("\tB: ");
    Serial.println(bValue);

    ledcWrite(pwmChannel1, (aValue < 10) ? 0 : map(aValue, 5, 255, 25, 255));

    ledcWrite(pwmChannel2, (bValue < 10) ? 0 : map(bValue, 5, 255, 25, 255));

  }
}
