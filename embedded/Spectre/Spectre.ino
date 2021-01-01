#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include "config.h"

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
#define led 32

// Constants
const char* ssid = "Teversham 2.4GHz";
const char* password = "083655655000";

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


  // Start WebSocket server and assign callback
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
}

void loop (void)
{

  exposureTime = analogRead(exposurePot)*3;
  sampleSensor(readings, exposureTime);
  
  Serial.write ((byte)0);            // sync byte = 0
  String hexString = "";
  for (int i = 0; i < NPIXELS; i++) {
    //       Serial.write ((byte)Pixel[i]+1);
    printHex(readings[i]);
    char hexCar[2];
  }
  Serial.println('\n');
  webSocket.broadcastBIN(readings, NPIXELS);
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

        display.setCursor(0, 0);
        display.print("Connected: ");
        display.print(ip.toString());

//        char * s;
//        sprintf(s, "{'exposure':%d}", exposureTime);
//
//        webSocket.sendTXT(num, s);
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
        
        readingsBuffer[j] = analogRead(analogOutPin)/4; // downsample to 8 bit
        
        digitalWrite(clockPin, HIGH);
        digitalWrite(clockPin, LOW);
       }
 
    delayMicroseconds(20);
}

void initOLED() {

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(800);

  // Clear the buffer.
  display.clearDisplay();
  display.display();

  // text display tests
  display.setTextSize(0.75);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Team Spectre\n");
}
