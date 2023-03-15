#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
const char* ssid     = "Truffle";
const char* password = "12345678900";

WiFiServer server(80);

void setup()
{
    lcd.init(); 
    lcd.backlight();    
    Serial.begin(115200);
    pinMode(5, OUTPUT);

    delay(10);

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();

}

int value = 0;

void loop(){
 WiFiClient client = server.available();

  if (client) {                             
    Serial.println("New Client.");           
    String currentLine = "";                
    while (client.connected()) {           
      if (client.available()) {             
        char c = client.read();                           
        if (c == '\n') {                 
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.print("Click <a href=\"/H\">here</a> to turn the LED on pin 5 on.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn the LED on pin 5 off.<br>");

            client.println();
            break;
          } else {   
            currentLine = "";
          }
        } else if (c != '\r') { 
          currentLine += c;    
        }

        
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(5, HIGH);               
           currentLine="";
        }
        if (currentLine.endsWith("/L")) {
          digitalWrite(5, LOW); 
          Serial.print(currentLine);
          Serial.println(currentLine.substring(5,currentLine.length()-2));
          lcd.setCursor(0, 0);
          lcd.print("Fire Detected");
          lcd.setCursor(0, 0);
          lcd.print(currentLine.substring(5,currentLine.length()-2));
           currentLine="";
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
