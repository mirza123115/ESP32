#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

bool deviceConnected = false;
unsigned long lastCheckTime = 0;

String dataBuffer = "";
char currentTag = '\0';

int aValue = 0;
int bValue = 0;

int ledPin = 2;
int pwmChannel = 0;
int freq = 5000;
int resolution = 8;


const int buttonPins[10] = { 14, 15, 16, 17, 18, 19, 21, 22, 23, 33 };
const int pinX = 26;
const int pinY = 27;


void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_emon");
  pinMode(2, OUTPUT);
  Serial.println("✅ Bluetooth Ready. Waiting for device...");
  Serial.println("A\tB");

  ledcSetup(pwmChannel, freq, resolution);
  ledcAttachPin(ledPin, pwmChannel);


  for (int i = 0; i < 10; i++) {
    pinMode(buttonPins[i], OUTPUT);
    digitalWrite(buttonPins[i], LOW);
  }

  pinMode(pinX, OUTPUT);
  pinMode(pinY, OUTPUT);
  digitalWrite(pinX, LOW);
  digitalWrite(pinY, LOW);
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

    // Optional: যদি Bluetooth থেকে সরাসরি 'x' বা 'y' আসে
    if (c == 'x') {
      digitalWrite(2, HIGH);
      Serial.println("Pin 2 HIGH (x received)");
    } else if (c == 'y') {
      digitalWrite(2, LOW);
      Serial.println("Pin 2 LOW (y received)");
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
    } else if (dataBuffer == "y") {
      digitalWrite(pinY, LOW);
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

    ledcWrite(pwmChannel, value);
  }
}


//app link https://drive.google.com/file/d/1ozPuvFMpvmbpqWMGSdvZvghlmXdbldYg/view?usp=drive_link
