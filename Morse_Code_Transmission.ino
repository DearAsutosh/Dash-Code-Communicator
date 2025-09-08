#include <WiFi.h>
#include <HTTPClient.h>

#define BUTTON_PIN 15
#define LED_PIN 2

const unsigned long DOT_THRESHOLD = 250;   // dot < 250 ms
const unsigned long LETTER_GAP   = 1000;   // 1 second = end of letter
const unsigned long WORD_GAP     = 2000;   // 2 seconds = end of word
const unsigned long DEBOUNCE_MS = 30;      // 30ms debounce

bool buttonState = HIGH;          // current stable state
bool lastButtonState = HIGH;      // previous stable state
unsigned long lastDebounceTime = 0;

unsigned long pressStart = 0;
unsigned long lastRelease = 0;

String letterBuffer = "";          // buffer for one letter

const char* ssid = "POCO M4 5G";       
const char* password = "kahibini";   
const char* serverURL = "http://10.99.126.61:5000/morse";  

void sendToServer(String payload) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");
    int code = http.POST(payload);
    if (code > 0) Serial.println("Server: " + http.getString());
    http.end();
  }
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);

  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

void loop() {
  int reading = digitalRead(BUTTON_PIN);
  unsigned long now = millis();

  // Debounce logic
  if (reading != lastButtonState) {
    lastDebounceTime = now;
  }

  if ((now - lastDebounceTime) > DEBOUNCE_MS) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        // Button pressed
        pressStart = now;
        digitalWrite(LED_PIN, HIGH);
      } else {
        // Button released
        digitalWrite(LED_PIN, LOW);
        unsigned long pressDuration = now - pressStart;
        lastRelease = now;

        // Add symbol to letter buffer
        String morseSymbol = (pressDuration < DOT_THRESHOLD) ? "." : "-";
        letterBuffer += morseSymbol;
        Serial.println("Buffered symbol: " + morseSymbol);
      }
    }
  }

  lastButtonState = reading;
  
  // Detect letter / word gaps
  if (buttonState == HIGH && lastRelease > 0) {
    unsigned long idleTime = now - lastRelease;

    if (idleTime > WORD_GAP) {
      if (letterBuffer.length() > 0) {
        sendToServer("{\"type\":\"symbol\",\"symbol\":\"" + letterBuffer + "\"}");
        letterBuffer = "";
      }
      Serial.println("Word gap detected");
      sendToServer("{\"type\":\"gap\",\"gap\":\"word\"}");
      lastRelease = 0;
    } else if (idleTime > LETTER_GAP) {
      if (letterBuffer.length() > 0) {
        sendToServer("{\"type\":\"symbol\",\"symbol\":\"" + letterBuffer + "\"}");
        letterBuffer = "";
      }
      Serial.println("Letter gap detected");
      sendToServer("{\"type\":\"gap\",\"gap\":\"letter\"}");
      lastRelease = 0;
    }
  }
}
