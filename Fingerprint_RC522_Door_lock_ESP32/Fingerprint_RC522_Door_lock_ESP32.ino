#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <Wire.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

HardwareSerial serialPort(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&serialPort);

#define SS_PIN 2
#define RST_PIN 4
#define lock 14
MFRC522 mfrc522(SS_PIN, RST_PIN);

#define BLYNK_AUTH_TOKEN "SrE_DSaDN-h0a15flCjgPGP9q2spuaSw"

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "prateeksingh";  //Enter your WIFI name
char pass[] = "kumar@12345";   //Enter your WIFI password

BLYNK_WRITE(V0) {
  bool value1 = param.asInt();
  if (value1 == 0) {
    digitalWrite(lock, LOW);
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("   Door Locked   ");
  } else {
    digitalWrite(lock, HIGH);
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(" Door Un-Locked ");
  }
}

void setup() {

  Serial.begin(9600);
  finger.begin(57600);
  SPI.begin();
  mfrc522.PCD_Init();
  pinMode(lock, OUTPUT);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  Serial.println();
  lcd.init();
  lcd.backlight();

  if (finger.verifyPassword()) {
    lcd.setCursor(0, 0);
    lcd.print("  FingerPrint ");
    lcd.setCursor(0, 1);
    lcd.print("Sensor Connected");
  } else {
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
  lcd.setCursor(0, 0);
  lcd.print(" Scan Your R&F ");
}

void loop() {
  rfid();
  getFingerprintIDez();
  Blynk.run();
  delay(2000);
}

void rfid() {

  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();

  if (content.substring(1) == "EC BA 05 39") {
    Serial.println("Access Granted");
    Serial.println();
    delay(500);
    lcd.setCursor(0, 1);
    lcd.print(" Door Un-Locked ");

    digitalWrite(lock, HIGH);
    delay(3000);
    digitalWrite(lock, LOW);
    delay(100);
    lcd.setCursor(0, 1);
    lcd.print("   Door Locked   ");
  } else {
    lcd.setCursor(0, 1);
    lcd.print("   Door Locked   ");
  }
}

int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    return -1;
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("  Try Again");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Scan Your R&F ");
    return -1;
  }

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Not Valid Finger");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Scan Your R&F ");

    return -1;
  }

  // found a match!
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(" Door Un-Locked ");
  digitalWrite(lock, HIGH);
  delay(3000);
  digitalWrite(lock, LOW);
  delay(100);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Scan Your R&F ");
  lcd.setCursor(0, 1);
  lcd.print("   Door Locked   ");
  return finger.fingerID;
}