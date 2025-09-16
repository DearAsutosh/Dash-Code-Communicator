#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------------- WiFi Config ----------------
const char* ssid = "POCO M4 5G";
const char* password = "kahibini";
const char* serverUrl = "http://10.191.255.61:5000/events";  // SSE endpoint

// ---------------- OLED Config ----------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1  
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------------- LED Pin ----------------
const int ledPin = 2; // onboard LED

WiFiClient client;

// ------------ Helper: Parse and Format Time ------------
String formatTime(String timestamp) {
  // Example input: "2025-09-10T19:42:01.036Z"
  int hour = timestamp.substring(11, 13).toInt();
  int minute = timestamp.substring(14, 16).toInt();

  // Adjust timezone (example: +5:30 IST)
  hour += 5;
  minute += 30;

  if (minute >= 60) {
    minute -= 60;
    hour++;
  }
  if (hour >= 24) hour -= 24;

  // Format hh:mm AM/PM
  String ampm = "AM";
  if (hour >= 12) {
    ampm = "PM";
    if (hour > 12) hour -= 12;
  }
  if (hour == 0) hour = 12;

  char buf[10];
  sprintf(buf, "%02d:%02d %s", hour, minute, ampm.c_str());
  return String(buf);
}

// ------------ Setup ------------
void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed!");
    for(;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Intro Screen
  display.setCursor(20, 20);
  display.println("SilentLink");
  display.setCursor(15, 40);
  display.println("by : CodeCommandos");
  display.display();
  delay(2000);

  // WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  Serial.println(WiFi.localIP());

  // Show waiting screen
  display.clearDisplay();
  display.setCursor(10, 25);
  display.println("Waiting for DashCode...");
  display.display();

  if (!client.connect("10.191.255.61", 5000)) {
    Serial.println("Connection to server failed!");
  }
  client.print(String("GET /events HTTP/1.1\r\nHost: 10.191.255.61\r\n\r\n"));
}

// ------------ Loop ------------
void loop() {
  if (client.available()) {
    String line = client.readStringUntil('\n');
    if (line.startsWith("data:")) {
      Serial.println("\n--- Raw SSE Message ---");
      Serial.println(line);

      String jsonData = line.substring(5);
      StaticJsonDocument<512> doc;
      DeserializationError error = deserializeJson(doc, jsonData);

      if (!error) {
        String username = doc["username"].as<String>();
        String decoded  = doc["decoded"].as<String>();
        String timestamp = doc["timestamp"].as<String>();

        String formattedTime = formatTime(timestamp);

        // Debugging
        Serial.printf("User: %s | Msg: %s | Time: %s\n", 
                      username.c_str(), decoded.c_str(), formattedTime.c_str());

        // Blink LED 3 times
        for (int i = 0; i < 3; i++) {
          digitalWrite(ledPin, HIGH);
          delay(200);
          digitalWrite(ledPin, LOW);
          delay(200);
        }

        // Show on OLED
        display.clearDisplay();
        display.setCursor(25, 0);
        display.println("SilentLink");
        display.setCursor(15, 10);
        display.println("by : CodeCommandos");
        display.drawLine(0, 20, 128, 20, SSD1306_WHITE);

        display.setCursor(0, 28);
        display.printf("From: %s", username.c_str());
        display.setCursor(0, 40);
        display.printf("Msg : %s", decoded.c_str());
        display.setCursor(0, 52);
        display.printf("Time: %s", formattedTime.c_str());

        display.display();
      }
    }
  }
}
