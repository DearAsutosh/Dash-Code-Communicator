#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------------- WiFi Config ----------------
const char* ssid = "POCO M4 5G";
const char* password = "kahibini";

// ---------------- Server Config ----------------
const char* serverHost = "10.191.255.61";
const int serverPort = 5000;
const char* serverPath = "/events";

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
  int hour = timestamp.substring(11, 13).toInt();
  int minute = timestamp.substring(14, 16).toInt();

  hour += 5; minute += 30;
  if (minute >= 60) { minute -= 60; hour++; }
  if (hour >= 24) hour -= 24;

  String ampm = "AM";
  if (hour >= 12) { ampm = "PM"; if (hour > 12) hour -= 12; }
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

  display.setCursor(20, 20);
  display.println("SilentLink");
  display.setCursor(15, 40);
  display.println("by : CodeCommandos");
  display.display();
  delay(2000);

  // Connect to WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  Serial.println("IP: " + WiFi.localIP().toString());

  // Connect to SSE server
  connectSSE();
}

// ------------ Loop ------------
void loop() {
  if (client.connected() && client.available()) {
    String line = client.readStringUntil('\n');
    line.trim();  // remove \r or spaces

    if (line.startsWith("data:")) {
      String jsonData = line.substring(5);

      StaticJsonDocument<512> doc;
      DeserializationError error = deserializeJson(doc, jsonData);
      if (!error) {
        String username = doc["username"].as<String>();
        String decoded  = doc["decoded"].as<String>();
        String timestamp = doc["timestamp"].as<String>();
        String formattedTime = formatTime(timestamp);

        Serial.printf("User: %s | Msg: %s | Time: %s\n", 
                      username.c_str(), decoded.c_str(), formattedTime.c_str());

        // Blink LED
        for (int i = 0; i < 3; i++) {
          digitalWrite(ledPin, HIGH); delay(200);
          digitalWrite(ledPin, LOW);  delay(200);
        }

        // Display on OLED
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

  // Reconnect if disconnected
  if (!client.connected()) {
    Serial.println("Disconnected from server, reconnecting...");
    connectSSE();
  }
}

// ------------ SSE Connect Function ------------
void connectSSE() {
  Serial.println("Connecting to SSE server...");

  if (client.connect(serverHost, serverPort)) {
    Serial.println("Connected to server!");

    // Send proper SSE headers
    client.print(String("GET ") + serverPath + " HTTP/1.1\r\n" +
                 "Host: " + serverHost + "\r\n" +
                 "Accept: text/event-stream\r\n" +
                 "Connection: keep-alive\r\n\r\n");

    Serial.println("SSE request sent.");
  } else {
    Serial.println("Connection failed, retrying in 5s...");
    delay(5000);
  }
}
