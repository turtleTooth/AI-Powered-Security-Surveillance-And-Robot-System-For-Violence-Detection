#include <WiFi.h>
#include <WiFiManager.h>
#include <ESPAsyncWebServer.h>
#include <EEPROM.h>

AsyncWebServer server(80);

const int motorPin1 = 12;
const int motorPin2 = 14;
const int motorPin3 = 27;
const int motorPin4 = 26;

const int maxMovements = 100;
const int movementDelay = 500; // Delay between movements in milliseconds

struct Movement {
  int command;
  int duration;
};

Movement movements[maxMovements];
int movementCount = 0;

const char* ssid = "Vigilante";
const char* password = "00000000";

void moveForward() {
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);
}

void moveBackward() {
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, HIGH);
}

void turnRight() {
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);
}

void turnLeft() {
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, HIGH);
}

void stopRobot() {
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
}

void handleRequest(AsyncWebServerRequest *request) {
  String message = "";

  if (request->hasParam("command")) {
    String command = request->getParam("command")->value();

    if (command == "forward") {
      moveForward();
      message = "Moving forward";
      if (movementCount < maxMovements) {
        movements[movementCount].command = 1; // Forward command
        movements[movementCount].duration = movementDelay;
        movementCount++;
      }
    } else if (command == "backward") {
      moveBackward();
      message = "Moving backward";
      if (movementCount < maxMovements) {
        movements[movementCount].command = 2; // Backward command
        movements[movementCount].duration = movementDelay;
        movementCount++;
      }
    } else if (command == "left") {
      turnLeft();
      message = "Turning left";
      if (movementCount < maxMovements) {
        movements[movementCount].command = 3; // Left command
        movements[movementCount].duration = movementDelay;
        movementCount++;
      }
    } else if (command == "right") {
      turnRight();
      message = "Turning right";
      if (movementCount < maxMovements) {
        movements[movementCount].command = 4; // Right command
        movements[movementCount].duration = movementDelay;
        movementCount++;
      }
    } else if (command == "stop") {
      stopRobot();
      message = "Stopping";
      if (movementCount < maxMovements) {
        movements[movementCount].command = 0; // Stop command
        movements[movementCount].duration = movementDelay;
        movementCount++;
      }
    }
  }

  request->send(200, "text/plain", message);
}

void repeatMovements() {
  for (int i = 0; i < movementCount; i++) {
    switch (movements[i].command) {
      case 0:
        stopRobot();
        break;
      case 1:
        moveForward();
        break;
      case 2:
        moveBackward();
        break;
      case 3:
        turnLeft();
        break;
      case 4:
        turnRight();
        break;
    }
    delay(movements[i].duration);
    stopRobot();
    delay(100);
  }
}

void setup() {
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);

  WiFi.softAP(ssid, password);

  IPAddress ipAddress = WiFi.softAPIP();
  Serial.print("Access Point IP address: ");
  Serial.println(ipAddress);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><body>";

    // Buttons for instructions arranged like a controller
    html += "<div style='display:flex; flex-direction:column; align-items:center;'>";
    html += "<div style='display:flex; justify-content:center;'>";
    html += "<button ontouchstart='sendCommand(event, \"forward\")' ontouchend='sendCommand(event, \"stop\")' onmousedown='sendCommand(event, \"forward\")' onmouseup='sendCommand(event, \"stop\")' style='font-size:20px; margin:5px; padding:10px 20px;'>FORWARD</button>";
    html += "</div>";
    html += "<div style='display:flex; justify-content:space-between;'>";
    html += "<button ontouchstart='sendCommand(event, \"left\")' ontouchend='sendCommand(event, \"stop\")' onmousedown='sendCommand(event, \"left\")' onmouseup='sendCommand(event, \"stop\")' style='font-size:20px; margin:5px; padding:10px 20px;'>LEFT</button>";
    html += "<button ontouchstart='sendCommand(event, \"right\")' ontouchend='sendCommand(event, \"stop\")' onmousedown='sendCommand(event, \"right\")' onmouseup='sendCommand(event, \"stop\")' style='font-size:20px; margin:5px; padding:10px 20px;'>RIGHT</button>";
    html += "</div>";
    html += "<div style='display:flex; justify-content:center;'>";
    html += "<button ontouchstart='sendCommand(event, \"backward\")' ontouchend='sendCommand(event, \"stop\")' onmousedown='sendCommand(event, \"backward\")' onmouseup='sendCommand(event, \"stop\")' style='font-size:20px; margin:5px; padding:10px 20px;'>BACKWARD</button>";
    html += "</div>";
    html += "</div>";

    // Record and Repeat buttons
    html += "<div style='display:flex; justify-content:center;'>";
    html += "<button onclick='recordMovements()' style='font-size:16px; margin:5px; padding:10px 20px;'>Record</button>";
    html += "<button onclick='repeatMovements()' style='font-size:16px; margin:5px; padding:10px 20px;'>Repeat</button>";
    html += "</div>";

    // JavaScript functions to send commands, record, and repeat movements
    html += "<script>";
    html += "function sendCommand(event, command) {";
    html += "   event.preventDefault();"; // Prevent default button behavior
    html += "   var xhttp = new XMLHttpRequest();";
    html += "   xhttp.open('GET', '/command?command=' + command, true);";
    html += "   xhttp.send();";
    html += "}";
    html += "function recordMovements() {";
    html += "   var xhttp = new XMLHttpRequest();";
    html += "   xhttp.open('GET', '/record', true);";
    html += "   xhttp.send();";
    html += "}";
    html += "function repeatMovements() {";
    html += "   var xhttp = new XMLHttpRequest();";
    html += "   xhttp.open('GET', '/repeat', true);";
    html += "   xhttp.send();";
    html += "}";
    html += "</script>";

    html += "</body></html>";

    request->send(200, "text/html", html);
  });

  server.on("/command", HTTP_GET, handleRequest);
  server.on("/record", HTTP_GET, [](AsyncWebServerRequest *request){
    movementCount = 0;
    request->send(200, "text/plain", "Recording movements");
  });
  server.on("/repeat", HTTP_GET, [](AsyncWebServerRequest *request){
    repeatMovements();
    request->send(200, "text/plain", "Repeating movements");
  });

  server.begin();
}

void loop() {
  // Check if there are any incoming client requests
  delay(10);
}