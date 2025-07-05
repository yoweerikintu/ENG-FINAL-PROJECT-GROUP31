#include <Arduino.h>
#include "HX711.h"
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// ----------------- Pin Definitions -----------------
#define LOADCELL_DOUT_PIN 2
#define LOADCELL_SCK_PIN 3
#define DHTPIN 6
#define DHTTYPE DHT11
#define STATUS_LED 13

// ESP32-CAM Status Input Pins
#define RIPE_STATUS_PIN 7     // RIPE
#define RAW_STATUS_PIN 9      // RAW (Unripe)
#define EMPTY_STATUS_PIN 8    // EMPTY

// ----------------- Objects and Globals -----------------
HX711 scale;
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial gsmSerial(10, 11);

float weight = 0.0;
float temperature = 27.0;
float humidity = 0.0;
unsigned long smsinterval = 0;
bool alertSent = false;

// ----------------- SMS Sending Function -----------------
void sendSMSAlert(String message) {
  gsmSerial.println("AT+CMGF=1");
  delay(100);
  gsmSerial.println("AT+CMGS=\"+256754021799\"");
  delay(100);
  gsmSerial.println(message);
  delay(100);
  gsmSerial.write(26);
  delay(1000);
}

void setup() {
  pinMode(STATUS_LED, OUTPUT);
  pinMode(RIPE_STATUS_PIN, INPUT);
  pinMode(RAW_STATUS_PIN, INPUT);
  pinMode(EMPTY_STATUS_PIN, INPUT);

  Serial.begin(57600);
  gsmSerial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Booting...");
  delay(2000);

  // Check GSM module
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Checking GSM...");
  bool gsmReady = false;

  for (int i = 0; i < 5; i++) {
    gsmSerial.println("AT");
    delay(500);
    if (gsmSerial.available()) {
      String r = gsmSerial.readString();
      if (r.indexOf("OK") != -1) {
        gsmReady = true;
        break;
      }
    }
    sendSMSAlert("System starting");
    delay(1000);
  }

  if (!gsmReady) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("GSM Error");
    lcd.setCursor(0, 1);
    lcd.print("Check SIM800");
    digitalWrite(STATUS_LED, HIGH);
    while (true);
  }

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(-459.542);  // <-- Adjust this
  scale.tare();
  dht.begin();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Ready!");
  digitalWrite(STATUS_LED, LOW);
  delay(2000);
}

void loop() {
  humidity = dht.readHumidity();
  weight = scale.get_units(5);
  temperature = temperature - 0.01944; // Adjusted temp

  if (isnan(temperature) || isnan(humidity)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error!");
    delay(2000);
    return;
  }

  bool isRipe = digitalRead(RIPE_STATUS_PIN);
  bool isRaw = digitalRead(RAW_STATUS_PIN);
  bool isEmpty = !digitalRead(EMPTY_STATUS_PIN);  // Inverted logic: LOW means empty

  // --- LCD Output ---
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:" + String(temperature, 1) + "C ");
  lcd.print("W:" + String(weight, 0) + "g");

  lcd.setCursor(0, 1);
  lcd.print("H:" + String(humidity, 0) + "% ");
  if (isEmpty) {
    lcd.print("EMPTY");
  } else if (isRipe) {
    lcd.print("RIPE");
  } else if (isRaw) {
    lcd.print("RAW");
  } else {
    lcd.print("RIPE");  // Default to RIPE if no other status is detected
  }

  delay(3000);

  // --- Alert Logic ---
  if (isRipe && !alertSent) {
    sendSMSAlert("Ripe tomatoes present");
    alertSent = true;
  } else if (!isRipe) {
    alertSent = false;
  }

  // --- Periodic Update ---
  if ((millis() - smsinterval) > 300000) {
    String state;
    if (isEmpty) state = "EMPTY";
    else if (isRipe) state = "RIPE";
    else if (isRaw) state = "RAW";
    else state = "RIPE";  // Default to RIPE if no other status is detected

    String smsalert = "H:" + String(humidity, 0) + "% T:" + String(temperature, 1) + "C " + "Status:" + state;
    sendSMSAlert(smsalert);
    smsinterval = millis();
  }
}
