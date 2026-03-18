#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

void setup() {
  Serial.begin(9600);  // Hardware Serial → ESP32 TX (RX0/D0)
  lcd.init();
  lcd.backlight();
}

void loop() {
  if (Serial.available()) {
    String t = Serial.readStringUntil('\n');  // ESP32 থেকে আসা line

    // split time & date
    t.replace(", ", ","); // simplify parsing

    int firstComma = t.indexOf(',');
    int secondComma = t.indexOf(',', firstComma + 1);

    String timePart = t.substring(0, firstComma);       // HH:MM:SS AM/PM
    String dayPart  = t.substring(firstComma + 1, secondComma); // Sun
    String datePart = t.substring(secondComma + 1);     // 10-03-2026

    // LCD Row 0 → Time + AM/PM
    /*lcd.setCursor(0,0);
    lcd.print("                "); // clear row*/
    lcd.setCursor(0,0);
    lcd.print(timePart);

    // LCD Row 1 → Day + Date
    /*lcd.setCursor(0,1);
    lcd.print("                "); // clear row*/
    lcd.setCursor(0,1);
    lcd.print(dayPart + " " + datePart);


    Serial.println(timePart);
    Serial.println(dayPart + " " + datePart);
  }

  
}