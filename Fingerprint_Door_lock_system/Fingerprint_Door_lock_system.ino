
//Prateek Singh
//www.prateeks.in

#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); //Serial Communications

//Created instances
LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

int relayPin = 6;
int red = 9;
int yellow = 8;
int green = 10;

void setup()
{
  pinMode(relayPin, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);
  digitalWrite(relayPin, LOW);
  digitalWrite(red, LOW);
  digitalWrite(yellow, HIGH);
  digitalWrite(green, LOW);
  Serial.begin(9600);
  while (!Serial);
  delay(100);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Fingerprint Door");
  lcd.setCursor(0, 1);
  lcd.print("lock by prateek");
  delay(3000);
  lcd.clear();

  // set the data rate for the sensor serial port
  finger.begin(57600);

  if (finger.verifyPassword()) {
    lcd.setCursor(0, 0);
    lcd.print("  FingerPrint ");
    lcd.setCursor(0, 1);
    lcd.print("Sensor Connected");
  }

  else  {
    lcd.setCursor(0, 0);
    lcd.print("Unable to found");
    lcd.setCursor(0, 1);
    lcd.print("Sensor");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Check Connections");

    while (1) {
      delay(1);
    }
  }
  lcd.clear();
}

void loop()                     // run over and over again
{
  getFingerprintIDez();
  delay(50);            //don't need to run this at full speed.
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  {
    lcd.setCursor(0, 0);
    lcd.print("  Waiting For");
    lcd.setCursor(0, 1);
    lcd.print("  Valid Finger");
    return -1;
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  Messy Image");
    lcd.setCursor(0, 1);
    lcd.print("  Try Again");
    delay(3000);
    lcd.clear();
    return -1;
  }

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Not Valid Finger");
    delay(3000);
    lcd.clear();
    digitalWrite(red, HIGH);
    delay(3000);
    digitalWrite(red, LOW);
    return -1;
  }

  // found a match!
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Door Unlocked");
  lcd.setCursor(0, 1);
  lcd.print("    Welcome");
  digitalWrite(relayPin, HIGH);
  digitalWrite(green, HIGH);
  delay(3000);
  digitalWrite(relayPin, LOW);
  digitalWrite(green, LOW);
  lcd.clear();
  return finger.fingerID;
}