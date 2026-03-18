#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27,16,2); 
SoftwareSerial espSerial(2,3); // RX=D2, TX=D3 (TX not used)

void setup() {
  Serial.begin(9600);        
  espSerial.begin(9600);     

  lcd.init();
  lcd.backlight();
  lcd.clear();

  Serial.println("Arduino ready");
}

void loop() {
  if (espSerial.available()) {
    String t = espSerial.readStringUntil('\n');
    t.trim(); // remove leading/trailing whitespace

    // split time, day, date
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

    // clear and print
    lcd.setCursor(0,0);
    lcd.print("                ");
    lcd.setCursor(0,0);
    lcd.print(timePart);

    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print(dayPart + " " + datePart);

    Serial.println(t);
  }
}
