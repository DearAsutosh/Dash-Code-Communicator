#include <WiFi.h>
#include <HTTPClient.h>

// ---------------- WiFi Config ----------------
const char *ssid = "POCO M4 5G";
const char *password = "kahibini";
const char *serverUrl = "http://10.191.255.61:5000/dash";  // Node server endpoint

// ---------------- Dash Code Input Config ----------------
#define BTN_A_PIN 16
#define BTN_B_PIN 17
#define LED_PIN 2   // Blue LED

const unsigned long DEBOUNCE_MS = 50;
const unsigned long LONG_PRESS_MS = 600;
const unsigned long MODE_HOLD_MS = 3000;
const unsigned long CLICK_TIMEOUT_MS = 500;  // max interval between consecutive clicks
const unsigned long SENTENCE_TIMEOUT_MS = 2000;

// ---------------- Button Struct ----------------
struct Btn {
  uint8_t pin;
  int lastRaw;
  int stableState;
  unsigned long lastDebounceTime;
  bool pressed;
  unsigned long pressStart;
  unsigned long lastRelease;
  int clickCount;
  unsigned long clickTimer;
};

Btn A = { BTN_A_PIN, HIGH, HIGH, 0, false, 0, 0, 0, 0 };
Btn B = { BTN_B_PIN, HIGH, HIGH, 0, false, 0, 0, 0, 0 };

unsigned long lastActivity = 0;
String currentSentence = "";

// ---------------- ESP Info ----------------
String espId;
String espUser = "Asutosh's ESP";  

// ---------------- Mode ----------------
bool templateMode = false;   // false = normal dashcode, true = template mode

// ---------------- Template DashCode ----------------
// Button A templates
String A_SINGLE = "*.....-..-.*  .-*...";      // HELP ME
String A_DOUBLE = "*.* .*. *.*";                  // SOS
String A_TRIPLE = "....-*...**-..-...**.-**.-."; // EMERGENCY

// Button B templates
String B_SINGLE = "***..*..****-**.--";                  // ATTACK
String B_DOUBLE = "*-******...-...**-"; // DANGER
String B_TRIPLE = "..****.--...  -**.*.-.-...**-";  //TAKE COVER            // CUSTOM/3rd template

// ---------------- Setup ----------------
void setup() {
  Serial.begin(115200);
  pinMode(A.pin, INPUT_PULLUP);
  pinMode(B.pin, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

  espId = String((uint64_t)ESP.getEfuseMac(), HEX);
  lastActivity = millis();
}

// ---------------- Main Loop ----------------
void loop() {
  unsigned long now = millis();
  int rawA = digitalRead(A.pin);
  int rawB = digitalRead(B.pin);

  // --- Debounce ---
  handleDebounce(A, rawA, B, now);
  handleDebounce(B, rawB, A, now);

  // --- Check click timers in template mode ---
  if (templateMode) {
    checkClickTimer(A, now);
    checkClickTimer(B, now);
  }

  // --- Sentence timeout (normal mode) ---
  if (!templateMode && currentSentence.length() > 0 && (now - lastActivity) >= SENTENCE_TIMEOUT_MS) {
    Serial.println("\n[SENTENCE END]");
    Serial.println("Final Sentence: " + currentSentence);
    sendToServer(currentSentence);
    currentSentence = "";
  }
}

// ---------------- Debounce Function ----------------
void handleDebounce(Btn &me, int raw, Btn &other, unsigned long now) {
  if (raw != me.lastRaw) {
    me.lastDebounceTime = now;
    me.lastRaw = raw;
  }
  if ((now - me.lastDebounceTime) > DEBOUNCE_MS) {
    if (raw != me.stableState) {
      me.stableState = raw;
      if (me.stableState == LOW) onPressDown(me, now);
      else onRelease(me, now);
    }
  }
}

// ---------------- Event Handlers ----------------
void onPressDown(Btn &me, unsigned long now) {
  me.pressed = true;
  me.pressStart = now;
}

void onRelease(Btn &me, unsigned long now) {
  unsigned long duration = now - me.pressStart;

  // --- Mode Toggle ---
  if (duration >= MODE_HOLD_MS) {
    templateMode = !templateMode;
    digitalWrite(LED_PIN, templateMode ? HIGH : LOW);
    Serial.println(templateMode ? "[MODE] Template Mode ON" : "[MODE] Normal Dash Mode ON");
    me.pressed = false;
    return;
  }

  if (templateMode) {
    // --- Register a click ---
    me.clickCount++;
    me.clickTimer = now;
    me.pressed = false;
    return;
  }

  // --- Normal DashCode Mode ---
  char symbol = '?';
  if (&me == &A) symbol = (duration >= LONG_PRESS_MS) ? '-' : '.';
  else symbol = (duration >= LONG_PRESS_MS) ? '#' : '*';

  appendSymbol(symbol, now);
  me.pressed = false;
}

// ---------------- Click Timer Check ----------------
void checkClickTimer(Btn &btn, unsigned long now) {
  if (btn.clickCount > 0 && (now - btn.clickTimer) > CLICK_TIMEOUT_MS) {
    // Time to resolve single/double/triple
    if (btn.clickCount == 1) {
      if (&btn == &A) {
        Serial.println("[TEMPLATE - SINGLE] HELP");
        sendToServer(A_SINGLE);
      } else {
        Serial.println("[TEMPLATE - SINGLE] ATTACK");
        sendToServer(B_SINGLE);
      }
    } else if (btn.clickCount == 2) {
      if (&btn == &A) {
        Serial.println("[TEMPLATE - DOUBLE] SOS");
        sendToServer(A_DOUBLE);
      } else {
        Serial.println("[TEMPLATE - DOUBLE] DANGER");
        sendToServer(B_DOUBLE);
      }
    } else if (btn.clickCount >= 3) {
      if (&btn == &A) {
        Serial.println("[TEMPLATE - TRIPLE] EMERGENCY");
        sendToServer(A_TRIPLE);
      } else {
        Serial.println("[TEMPLATE - TRIPLE] CUSTOM");
        sendToServer(B_TRIPLE);
      }
    }
    btn.clickCount = 0;
  }
}

// ---------------- Helpers ----------------
void appendSymbol(char sym, unsigned long now) {
  Serial.print(sym);
  currentSentence += String(sym);
  lastActivity = now;
}

void sendToServer(String payload) {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");

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
