#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "Telapia";
const char* password = "Tastesgood";
const int buzzerPin = 13;  // Replace with the pin connected to the buzzer
AsyncWebServer server(80);

void triggerAlarm() {
  // Code to trigger the alarm (e.g., activate a buzzer or send a signal)
  digitalWrite(buzzerPin, HIGH);  // Activate the buzzer
  delay(2000);  // Buzzer activation duration
  digitalWrite(buzzerPin, LOW);  // Deactivate the buzzer
}

void setup() {
  Serial.begin(115200);

  pinMode(buzzerPin, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/trigger-alarm", HTTP_GET, [](AsyncWebServerRequest *request){
    triggerAlarm();
    request->send(200);
  });

  server.begin();
}

void loop() {
  // Other code in the loop if needed
}