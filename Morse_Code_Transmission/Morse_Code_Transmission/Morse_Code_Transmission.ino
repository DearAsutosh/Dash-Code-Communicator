#include <WiFi.h>
#include <HTTPClient.h>

// ---------------- WiFi Config ----------------
const char *ssid = "POCO M4 5G";
const char *password = "kahibini";
const char *serverUrl = "http://10.191.255.61:5000/dash";  // Node server endpoint

// ---------------- Dash Code Input Config ----------------
#define BTN_A_PIN 16
#define BTN_B_PIN 17

const unsigned long DEBOUNCE_MS = 50;
const unsigned long LONG_PRESS_MS = 600;
const unsigned long SENTENCE_TIMEOUT_MS = 2000;
const unsigned long SIMULT_WINDOW_MS = 150;

struct Btn {
  uint8_t pin;
  int lastRaw;
  int stableState;
  unsigned long lastDebounceTime;
  bool pressed;
  unsigned long pressStart;
  bool consumed;
};

Btn A = { BTN_A_PIN, HIGH, HIGH, 0, false, 0, false };
Btn B = { BTN_B_PIN, HIGH, HIGH, 0, false, 0, false };

bool spaceEmittedForPair = false;
unsigned long lastActivity = 0;
String currentSentence = "";

// ---------------- ESP Info ----------------
String espId;
String espUser = "Asutosh's ESP";  // change per soldier

// ---------------- Setup ----------------
void setup() {
  Serial.begin(115200);
  pinMode(A.pin, INPUT_PULLUP);
  pinMode(B.pin, INPUT_PULLUP);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

  // Unique ID from chip MAC
  espId = String((uint64_t)ESP.getEfuseMac(), HEX);
  lastActivity = millis();
}

// ---------------- Main Loop ----------------
void loop() {
  unsigned long now = millis();
  int rawA = digitalRead(A.pin);
  int rawB = digitalRead(B.pin);

  // --- Debounce Button A ---
  if (rawA != A.lastRaw) {
    A.lastDebounceTime = now;
    A.lastRaw = rawA;
  }
  if ((now - A.lastDebounceTime) > DEBOUNCE_MS) {
    if (rawA != A.stableState) {
      A.stableState = rawA;
      if (A.stableState == LOW) {
        onPressDown(A, B, now);
      } else {
        onRelease(A, B, now);
      }
    }
  }

  // --- Debounce Button B ---
  if (rawB != B.lastRaw) {
    B.lastDebounceTime = now;
    B.lastRaw = rawB;
  }
  if ((now - B.lastDebounceTime) > DEBOUNCE_MS) {
    if (rawB != B.stableState) {
      B.stableState = rawB;
      if (B.stableState == LOW) {
        onPressDown(B, A, now);
      } else {
        onRelease(B, A, now);
      }
    }
  }

  // --- Sentence timeout ---
  if (currentSentence.length() > 0 && (now - lastActivity) >= SENTENCE_TIMEOUT_MS) {
    Serial.println("\n[SENTENCE END]");
    Serial.println("Final Sentence: " + currentSentence);
    sendToServer(currentSentence);  // Send full sentence to server
    currentSentence = "";
    spaceEmittedForPair = false;
  }
}

// ---------------- Event Handlers ----------------
void onPressDown(Btn &me, Btn &other, unsigned long now) {
  me.pressed = true;
  me.pressStart = now;
  me.consumed = false;

  if (other.pressed) {
    unsigned long diff = abs((long)(me.pressStart - other.pressStart));
    if (diff <= SIMULT_WINDOW_MS && !spaceEmittedForPair) {
      emitSpace(now);
      me.consumed = true;
      other.consumed = true;
      spaceEmittedForPair = true;
    }
  }
}

void onRelease(Btn &me, Btn &other, unsigned long now) {
  if (me.consumed) {
    me.pressed = false;
    if (!other.pressed) spaceEmittedForPair = false;
    return;
  }

  unsigned long duration = now - me.pressStart;
  char symbol = '?';
  if (&me == &A) {
    symbol = (duration >= LONG_PRESS_MS) ? '-' : '.';
  } else {
    symbol = (duration >= LONG_PRESS_MS) ? '#' : '*';
  }

  appendSymbol(symbol, now);
  me.pressed = false;
  if (!other.pressed) spaceEmittedForPair = false;
}

// ---------------- Helpers ----------------
void appendSymbol(char sym, unsigned long now) {
  Serial.print(sym);
  currentSentence += String(sym);
  lastActivity = now;
}

void emitSpace(unsigned long now) {
  Serial.println("\n[SPACE]");
  currentSentence += " ";
  lastActivity = now;
}

void sendToServer(String payload) {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");

  // ✅ Send JSON exactly as Node server expects
  String json = "{";
  json += "\"espId\":\"" + espId + "\",";
  json += "\"username\":\"" + espUser + "\",";
  json += "\"code\":\"" + payload + "\"";
  json += "}";

  int httpResponseCode = http.POST(json);

  if (httpResponseCode > 0) {
    Serial.printf("[Server] Response: %d\n", httpResponseCode);
    String response = http.getString();
    Serial.println("Server says: " + response);
  } else {
    Serial.printf("[Server] Error: %s\n", http.errorToString(httpResponseCode).c_str());
  }
  http.end();
}
