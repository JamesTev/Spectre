#include <SPI.h>
#include <Wire.h>
#include <ESPmDNS.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include "config.h"

#define led 32

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Constants
const char* ssid = "Teversham 2.4GHz";
const char* password = "083655655000";

const char* mdnsName = "spectreEsp32"; // Domain name for the mDNS responder

// Globals
WebSocketsServer webSocket = WebSocketsServer(802);

// Sensor interface:
#define analogOutPin  A2    // Analog output
#define siPin  12           // Start Integration
#define clockPin 27         // Clock 
#define exposurePot  A3     // Integration time/exposure control

#define NPIXELS 128  // No. of pixels in array

byte readings[NPIXELS]; // Field for measured values <0-255>
int sampleTime;
int exposureTime;

bool clientConn = false;

void setup(void)
{
  pinMode(siPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  //pinMode (AOpin, INPUT);

  digitalWrite(siPin, LOW);   // IDLE state
  digitalWrite(clockPin, LOW);  // IDLE state

  Serial.begin (115200);

  initOLED();

  Serial.println("Connecting");
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
  }

  // Print our IP address
  Serial.println("Connected!");
  Serial.print("My IP address: ");
  Serial.println(WiFi.localIP());

  startMDNS();                 // Start the mDNS responder

  // Start WebSocket server and assign callback
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
}

void loop (void)
{
  if(clientConn or true){ // ignore this statement for now
      exposureTime = analogRead(exposurePot)*5;
      sampleSensor(readings, exposureTime);
      webSocket.broadcastBIN(readings, NPIXELS);

      Serial.write ((byte)0);            // sync byte = 0
      String hexString = "";
      for (int i = 0; i < NPIXELS; i++) {
        //       Serial.write ((byte)Pixel[i]+1);
          printHex(readings[i]);
          char hexCar[2];
      }
      Serial.println("");
  }

  webSocket.loop();
  delay(400); // just for debug to slow down serial output
}

// Called when receiving any WebSocket message
void onWebSocketEvent(uint8_t num,
                      WStype_t type,
                      uint8_t * payload,
                      size_t length) {

  // Figure out the type of WebSocket event
  switch (type) {

    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;

    // New client has connected
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connection from ", num);
        Serial.println(ip.toString());

        clientConn = true;
        
        display.setCursor(0, 20);
        display.print("client connected: ");
        display.setCursor(0, 30);
        display.print(ip.toString());
        display.display();

        String params = "__params__{";
        params+="\"exposure\":";
        params+=exposureTime;
        params+= ", \"dummyVar\":";
        params+=100;
        params+="}";

        webSocket.sendTXT(num, params);
      }
      break;

    // Echo text message back to client
    case WStype_TEXT:
      Serial.printf("[%u] Text: %s\n", num, payload);
      webSocket.sendTXT(num, payload);
      break;

    // For everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}

void printHex(uint8_t num) {
  char hexCar[2];

  sprintf(hexCar, "%02X", num);
  Serial.print(hexCar);
}

void sampleSensor(byte readingsBuffer[], int exposure) {
    digitalWrite(clockPin, LOW);
    digitalWrite(siPin, HIGH);
    digitalWrite(clockPin, HIGH);
    digitalWrite(siPin, LOW);

    sampleTime = micros();
    
    digitalWrite(clockPin, LOW);
 
    for (int j = 0; j < 128; j++)
       {
        digitalWrite(clockPin, HIGH);
        digitalWrite(clockPin, LOW);
       }
 
    delayMicroseconds(exposure);
 
    digitalWrite(siPin, HIGH);
    digitalWrite(clockPin, HIGH);
    digitalWrite(siPin, LOW);
 
    sampleTime = micros() - sampleTime;
    
    digitalWrite(clockPin, LOW);
    
    for (int j = 0; j < NPIXELS; j++)
       {
        delayMicroseconds(20);
        
        readingsBuffer[j] = min(analogRead(analogOutPin)/16, 255); // downsample to 8 bit
        
        digitalWrite(clockPin, HIGH);
        digitalWrite(clockPin, LOW);
       }
 
    delayMicroseconds(20);
}

void startMDNS() { // Start the mDNS responder
  MDNS.begin(mdnsName);                        // start the multicast domain name server
  Serial.print("mDNS responder started: http://");
  Serial.print(mdnsName);
  Serial.println(".local");
}

void initOLED(){
   if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
      Serial.println(F("SSD1306 allocation failed"));
    }
    delay(500);
    display.clearDisplay();
  
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    // Display static text
    display.println("Team Spectre");
    display.display();
    //display.startscrollright(0x00, 0x0F); 
}
