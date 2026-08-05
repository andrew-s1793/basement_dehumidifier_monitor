#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <secrets.h>
 
// ---- Fill these in with your info, or create and include a secrets.h file like I did ----
//const char* WIFI_SSID     = "YOUR_WIFI_NAME";
//const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
//const char* NTFY_TOPIC    = "YOUR_RANDOM_TOPIC_NAME";
 
// ---- Pins ----
#define DHT_PIN   4
#define DHT_TYPE  DHT22
#define LDR_PIN   34   // ADC1 pin -- do not use an ADC2 pin, WiFi breaks those readings
 
// ---- Tunables (calibrate these once the hardware is running) ----
const float HUMIDITY_ALARM_SETPOINT = 70.0;  // % RH -- fire alert above this
const float HUMIDITY_RESET_SETPOINT = 65.0;  // % RH -- must drop below this to re-arm
const int   HUMIDITY_DEBOUNCE_COUNT = 10;    // consecutive high readings required
const unsigned long READ_INTERVAL_MS = 30000; // 30s between DHT22 reads
 
int   LDR_TANK_FULL_THRESHOLD = 2310; // Midpoint between no tank full LED (~570) and tank full LED on (~4053)
const int LDR_DEBOUNCE_COUNT = 3;     // consecutive high readings required
 
DHT dht(DHT_PIN, DHT_TYPE);
 
// ---- State ----
int  humidityHighStreak = 0;
bool humidityAlarmActive = false;
int  ldrHighStreak = 0;
bool tankFullAlarmActive = false;
unsigned long lastReadMs = 0;
 
void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected: " + WiFi.localIP().toString());
}
 
// One function, two callers -- title/message/tag differ per alert type.
void sendNtfy(const String& title, const String& message, const String& tags) {
  if (WiFi.status() != WL_CONNECTED) return;
  HTTPClient http;
  String url = String("https://ntfy.sh/") + NTFY_TOPIC;
  http.begin(url);
  http.addHeader("Title", title);
  http.addHeader("Tags", tags);
  int code = http.POST(message);
  Serial.printf("ntfy POST -> HTTP %d\n", code);
  http.end();
}
 
void checkHumidity(float humidity) {
  if (isnan(humidity)) return;
 
  if (humidity >= HUMIDITY_ALARM_SETPOINT) {
    humidityHighStreak++;
  } else if (humidity < HUMIDITY_RESET_SETPOINT) {
    humidityHighStreak = 0;
    if (humidityAlarmActive) {
      humidityAlarmActive = false;
      Serial.println("Humidity alarm cleared.");
    }
  }
 
  if (!humidityAlarmActive && humidityHighStreak >= HUMIDITY_DEBOUNCE_COUNT) {
    humidityAlarmActive = true;
    sendNtfy(
      "Basement humidity high",
      "Sustained humidity above " + String(HUMIDITY_ALARM_SETPOINT, 0) + "% RH. Dehumidifier may be falling behind.",
      "sweat_drops"
    );
  }
}
 
void checkTankFull(int ldrValue) {
  if (ldrValue >= LDR_TANK_FULL_THRESHOLD) {
    ldrHighStreak++;
  } else {
    ldrHighStreak = 0;
    if (tankFullAlarmActive) {
      tankFullAlarmActive = false;
      Serial.println("Tank-full alarm cleared.");
    }
  }
 
  if (!tankFullAlarmActive && ldrHighStreak >= LDR_DEBOUNCE_COUNT) {
    tankFullAlarmActive = true;
    sendNtfy(
      "Dehumidifier tank full",
      "Tank-full indicator LED is on. Empty the tank.",
      "warning"
    );
  }
}
 
void setup() {
  Serial.begin(115200);
  dht.begin();
  connectWiFi();
}
 
void loop() {
  if (millis() - lastReadMs < READ_INTERVAL_MS) return;
  lastReadMs = millis();
 
  float humidity = dht.readHumidity();
  int   ldrValue = analogRead(LDR_PIN);
 
  Serial.printf("Humidity: %.1f%%  LDR: %d\n", humidity, ldrValue);
 
  checkHumidity(humidity);
  checkTankFull(ldrValue);
}
