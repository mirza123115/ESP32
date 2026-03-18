#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27,16,2); 
SoftwareSerial espSerial(2,3); // RX=D2, TX not used

String prevTime = "";
String prevDate = "";

void setup() {
  Serial.begin(9600);        
  espSerial.begin(9600);     

  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop() {
  if (espSerial.available()) {
    String t = espSerial.readStringUntil('\n');
    t.trim(); 

    int firstComma = t.indexOf(',');
    int secondComma = t.indexOf(',', firstComma + 1);

    String timePart = "";
    String dayPart = "";
    String datePart = "";

    if (firstComma > 0) {
      timePart = t.substring(0, firstComma);
      timePart.trim();
    }

    if (secondComma > firstComma) {
      dayPart = t.substring(firstComma + 1, secondComma);
      dayPart.trim();
    }

    if (secondComma >= 0) {
      datePart = t.substring(secondComma + 1);
      datePart.trim();
    }

    // Row 0 - time update only if changed
    if (timePart != prevTime) {
      lcd.setCursor(0,0);
      lcd.print(timePart);  
      if (timePart.length() < 16) {
        for (int i = timePart.length(); i < 16; i++) lcd.print(" "); // overwrite leftover
      }
      prevTime = timePart;
    }

    // Row 1 - day + date update only if changed
    String row2 = dayPart + " " + datePart;
    if (row2 != prevDate) {
      lcd.setCursor(0,1);
      lcd.print(row2);
      if (row2.length() < 16) {
        for (int i = row2.length(); i < 16; i++) lcd.print(" ");
      }
      prevDate = row2;
    }
  }
}
