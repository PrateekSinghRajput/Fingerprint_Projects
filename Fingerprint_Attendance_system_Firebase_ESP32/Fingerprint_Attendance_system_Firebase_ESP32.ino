#include <SoftwareSerial.h>
#include <FirebaseESP32.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Adafruit_Fingerprint.h>

// OLED display setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// NTP Client setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000); // UTC+5:30 for India

#define FIREBASE_HOST "attendance-ed3eb-default-rtdb.firebaseio.com"
#define RXD2 16  
#define TXD2 17    

// WiFi credentials
#define WIFI_SSID "Prateek"
#define WIFI_PASSWORD "justdoelectronics@#123456789"

// Firebase configuration
//const char* FIREBASE_PROJECT_ID = "attendance-ed3eb";
const char* FIREBASE_CLIENT_EMAIL = "firebase-adminsdk-fbsvc@attendance-ed3eb.iam.gserviceaccount.com";
const char* FIREBASE_PRIVATE_KEY = "-----BEGIN PRIVATE KEY-----\nMIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQCn1Xz/OP7N796s\nLZYX3L/Z7A6aLhSzOcPIbRuYdRryPVrqlxY5xL4JPAZYR0jDFH4aaLvXWsYbaVV6\nMp+UIzuumqJ/kWcSXvQLEOhVT1eiNxPa5h7dfnVQi12lnwh+AZOYBqG4rs09KsNZ\nIrFNknKWc0S2fW6w4Q7oeio1nRZcQTNw4H3t1SvzbBcl9D3X4UYXjKSZ7SKkernU\nImRArtlqRTgjxZug4o66FwXk58vfiwEtmGj0bpiSuT8LPeS2ipCqGSaU4ic9EATE\nR4jCUWU8r1iqK4zRFPesfmtgpZO3utj+LILJW4p2Oj5am/YTxEnpdG8KFVyTERXZ\ncPdho4J5AgMBAAECggEAHd/25NBurNY8nw3rSHqbLO2jyMaYmTGYW9ouHhSEVCff\nQeBcjLJseGTWpUqlFVKVwtovS52FI5Z7qGmP9dTSuBkfAVGu1PqRetHK5Xqe2Qlw\nT5gEWP18YPa51/Qn1RQvQ1G04lS9RFzTEpAGGw6NNqI0kj5gIHyjRHcQgeQhOZD4\nUwDLzyD1F+GcQ7ywnmQaOAVcZL+r18oBj6+NYjCie/vab0x3SWJ89h8WZpXWs5Is\nCzW88qWTeUvh6NrzfixNp7JzAn/nU6UPKWenIu32KvwyG7NfqwIv3TbAruyLNWwf\nWPjQ5CN+E4rS3Zh8nAKqnF3KPhc1hcmPjKYRP+eOzQKBgQDoRuRl1QPlnEB/Dj6i\nWzqHvnd++8robAxMn9G+dvkplxadJvd+N04HrmD+kbgRv06DBvmTbRH6BOCIToxO\nhG07ZvdRsM3d4xXXPjRGjolAVu1kWUENDTAyjvcViYLx7e0XFoqokbZ4nr7E6Cyo\noHjIXlLzoOCCDwqu5GUVQ7SwLQKBgQC4+avNkyWuAp/TbFK+pVZUqQDF9XjCJWMD\nYL1FknPueV825oL5irMSgEvJ9s+afqqm7S4Hrh2K4MoLefJjC7MXlkRUVdB+GB5b\nk0HL8lR3dETMACvbwQWmuAKtVcRZg7o4iP+FN00+JaavcBykzmtUGHTA/o6OGAMP\nKGe31JC+/QKBgQDXUzDUPf6Y0prvrfLea5f80tLQAvd9MvzL47R6zqv4pDmwgbkZ\nZoeNk1gTZl6vBRvxV3vx8UCi9/Z2JernttfUI4XOUwrlUWij2uR3LBeMb4kuZ5do\nU/uEQwhHDO04/Ym2x4zO7BWuB+ttepY71Fdj2B0qAM9fgjYzH0OdzOAqGQKBgQCJ\nE0xDnYXQu9Ug4ic3bGLbfho6mLwSc446VJguVGdhE8SBdLUeo7AsR7LiSepyvOdN\nqHnx6w+teQOBGa+LwoHHOu4L6UQGDW2Bw1CBCGWb2CebXY0i3pmHgNlLBkHpa/q2\nJp/kYqT1JwWtANHZN7bFrRHoazvNBPJPa0vhKL7TKQKBgHkvv23sLda7UeXFdavS\nzw0WdHD5XutnYnlBjoCr/Q/OOYJ7uB93SEVHtuwmX4xKs+2M+6N1VB29F3uyEfw7\n1DxSf0Ho9MNSJ9qwaWaln7AjrEIIkXIzZ+EuaerxNKVEAubjhQM9cyw3QzbcnZvt\nPhCUoeSRTWMctNDu5purTZDz\n-----END PRIVATE KEY-----\n";
const char* FIREBASE_DATABASE_URL = "https://attendance-ed3eb-default-rtdb.firebaseio.com/";

// User data structure
struct User {
  uint16_t fingerprintID;
  String name;
  String phoneNumber;
};

// Authorized users with their phone numbers
const User authorizedUsers[] = {
  {1, "Prateek", "+918830584864"},  // Fingerprint ID, Name, Phone
  {2, "Rohan",   "+918928760856"},
  {3, "Shyam",   "+911234567891"},
  {4, "Emily",   "+911234567892"},
  {5, "Michael", "+911234567893"}
};

SoftwareSerial fingerprintSerial(RXD2, TXD2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerprintSerial);
FirebaseData fbdo;
FirebaseConfig config;
FirebaseAuth auth;

// Function prototypes
void displayMessage(String line1, String line2 = "");
uint16_t getFingerprintID();
String getFormattedDate();
String getFormattedTime();
void initializeGSM();
void sendSMS(String phoneNumber, String message);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting setup...");
  
  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Initializing...");
  display.display();
  
  fingerprintSerial.begin(57600);
  Serial.println("Initialized Fingerprint sensor and OLED");

  // Initialize WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Connecting WiFi");
  display.display();
  
  int wifiAttempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    wifiAttempts++;
    if (wifiAttempts > 20) {
      Serial.println("\nWiFi connection failed!");
      display.clearDisplay();
      display.setCursor(0,0);
      display.println("WiFi Failed!");
      display.display();
      delay(2000);
      ESP.restart();
    }
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize NTP client
  timeClient.begin();
  timeClient.update();

  // Configure Firebase
  config.service_account.data.client_email = FIREBASE_CLIENT_EMAIL;
  config.service_account.data.project_id = FIREBASE_PROJECT_ID;
  config.service_account.data.private_key = FIREBASE_PRIVATE_KEY;
  config.database_url = FIREBASE_DATABASE_URL;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Initialize fingerprint sensor
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Fingerprint Error");
    display.display();
    while (1) { delay(1); }
  }

  if (Firebase.ready()) {
    Serial.println("Firebase connected!");
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("System Ready");
    display.display();
  } else {
    Serial.println("Firebase connection failed!");
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Firebase Error");
    display.display();
  }

  initializeGSM();
}

void loop() {
  if (!Firebase.ready()) {
    Serial.println("Firebase not ready");
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Firebase Error");
    display.display();
    delay(2000);
    return;
  }

  // Check for fingerprint
  uint16_t fingerprintID = getFingerprintID();
  delay(50);  // Small delay
  
  if (fingerprintID != 0) {  // 0 means no match found
    Serial.println("\nFingerprint ID: " + String(fingerprintID));
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Fingerprint ID:");
    display.setCursor(0,10);
    display.println(String(fingerprintID));
    display.display();

    // Check if fingerprint is authorized
    int userIndex = -1;
    for (int i = 0; i < 5; i++) {
      if (fingerprintID == authorizedUsers[i].fingerprintID) {
        userIndex = i;
        break;
      }
    }

    if (userIndex != -1) {
      String userName = authorizedUsers[userIndex].name;
      String userPhone = authorizedUsers[userIndex].phoneNumber;
      
      Serial.println("User: " + userName);
      display.clearDisplay();
      display.setCursor(0,0);
      display.println("Welcome");
      display.setCursor(0,10);
      display.println(userName);
      display.display();

      // Get current time
      timeClient.update();
      String currentTime = getFormattedTime();
      String currentDate = getFormattedDate();
      String timestamp = String(timeClient.getEpochTime());

      // Check if user already has an "in" time today
      String dailyPath = "/daily_attendance/" + currentDate + "/" + String(fingerprintID);
      FirebaseJsonData outTimeData;
      
      if (Firebase.get(fbdo, dailyPath)) {
        // User has existing entry for today
        FirebaseJson json = fbdo.jsonObject();
        FirebaseJsonData inTimeData;
        json.get(inTimeData, "in_time");
        
        if (inTimeData.success) {
          json.get(outTimeData, "out_time");
          if (!outTimeData.success) {
            // User is checking out (has in_time but no out_time)
            json.set("out_time", currentTime);
           // json.set("out_timestamp", timestamp);
            
            if (Firebase.updateNode(fbdo, dailyPath, json)) {
              Serial.println("Out time recorded");
              displayMessage(userName, "Checked OUT");
              
              // Send SMS for checkout
              String smsMessage = userName + " checked OUT\nTime: " + currentTime + "\nDate: " + currentDate;
              sendSMS(userPhone, smsMessage);
            } else {
              Serial.println("Error updating out time: " + fbdo.errorReason());
              displayMessage("Error", "Updating out time");
            }
          } else {
            // User already has both in and out times
            displayMessage(userName, "Already recorded");
          }
        }
      } else {
        // First check-in of the day
        FirebaseJson json;
        json.set("name", userName);
        json.set("in_time", currentTime);
        //json.set("in_timestamp", timestamp);
        json.set("date", currentDate);
        
        if (Firebase.setJSON(fbdo, dailyPath, json)) {
          Serial.println("In time recorded");
          displayMessage(userName, "Checked IN");
          
          // Send SMS for checkin
          String smsMessage = userName + " checked IN\nTime: " + currentTime + "\nDate: " + currentDate;
          sendSMS(userPhone, smsMessage);
        } else {
          Serial.println("Error recording in time: " + fbdo.errorReason());
          displayMessage("Error", "Recording in time");
        }
      }
    } else {
      Serial.println("Unauthorized fingerprint!");
      displayMessage("Unauthorized!", "Access Denied");
      
      // Send alert to admin
      String smsMessage = "Unauthorized access\nFingerprint ID: " + String(fingerprintID) + "\nTime: " + getFormattedTime();
      sendSMS(authorizedUsers[0].phoneNumber, smsMessage);
    }
    delay(2000); // Delay between scans
  }
}

// Returns fingerprint ID if found, 0 if not found
uint16_t getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return 0;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return 0;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return 0;
  
  return finger.fingerID;
}

String getFormattedDate() {
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime(&epochTime);
  
  return String(ptm->tm_year+1900) + "-" + 
         String(ptm->tm_mon+1) + "-" + 
         String(ptm->tm_mday);
}

String getFormattedTime() {
  timeClient.update();
  time_t rawTime = timeClient.getEpochTime();
  struct tm *ti = localtime(&rawTime);

  return String(ti->tm_hour) + ":" + 
         String(ti->tm_min) + ":" + 
         String(ti->tm_sec);
}

void initializeGSM() {
  Serial.println("Initializing GSM...");
  fingerprintSerial.println("AT");
  delay(1000);
  fingerprintSerial.println("AT+CMGF=1");
  delay(1000);
  fingerprintSerial.println("AT+CNMI=1,2,0,0,0");
  delay(1000);
  Serial.println("GSM Ready");
}

void sendSMS(String phoneNumber, String message) {
  Serial.println("Sending to: " + phoneNumber);
  fingerprintSerial.println("AT+CMGS=\"" + phoneNumber + "\"");
  delay(1000);
  fingerprintSerial.print(message);
  delay(1000);
  fingerprintSerial.write(26); // CTRL+Z to send
  delay(1000);
  Serial.println("SMS Sent");
}

void displayMessage(String line1, String line2) {
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(line1);
  if(line2 != "") {
    display.setCursor(0,10);
    display.println(line2);
  }
  display.display();
}