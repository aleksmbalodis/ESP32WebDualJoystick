#include <WebServer.h>
#include <AsyncTCP.h>
#include <stdio.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "SPIFFS.h"

#define LED 18 //Iebuvetais LED0
#define LED1 19 // Iebuvetais LED1

int brightness = 0;    // LED Spilgtums
int fadeAmount = 5;    // Cik LED spilgtumu mainit

// PWM mainigie
const int freq = 5000;
const int ledChannel = 0;
const int ledChannel1 = 1;
const int resolution = 10; //Resolution 8, 10, 12, 15

// Tagad Laiks
unsigned long currentTime = millis();
// Ieprieksejais laiks
unsigned long previousTime = 0;
// Timeout ms
const long timeoutTime = 2000;
// Mainigais HTTP requestiem
String header;

// WiFi Access Point Mainigie
const char * APssid = "ESP32-WROOM";
const char * APpassword = "";
IPAddress APlocal_IP(192, 168, 4, 1);
IPAddress APgateway(192, 168, 4, 1);
IPAddress APsubnet(255, 255, 255, 0);

// WiFi STAtion mainigie
const char * STAssid = "TF303"; // Network to be joined as a station SSID
const char * STApassword = "aaaaaaaa8"; // Network to be joined as a station password
IPAddress STAlocal_IP(192, 168, 0, 27);
IPAddress STAgateway(192, 168, 0, 1);
IPAddress STAsubnet(255, 255, 255, 0);

// UDP Mainigie
unsigned int localUdpPort = 4210;
char incomingPacket[255];
char replyPacket[] = "..";
char command[4][4];
int commandInt[4];
char * commandPosition;

//UDP un WiFi Serveru bibloteku instance
WiFiUDP udp;
WebServer server(80);

int orangex;
int orangey;
int cyanx;
int cyany;

void handleJSData() {
  //Iegust X un Y no web joysticka
  orangex = server.arg(0).toInt();
  orangey = server.arg(1).toInt();
  cyanx = server.arg("x2").toInt();
  cyany = server.arg("y2").toInt();

  //Atgriez HTTP 200
  server.send(200, "text/plain", "");
}

void setup() {
  Serial.begin(115200);

  //LED Konfiguracija
  pinMode(LED,OUTPUT);
  ledcSetup(ledChannel, freq, resolution);
  ledcSetup(ledChannel1, freq, resolution);
  ledcAttachPin(LED, ledChannel);
  ledcAttachPin(LED1, ledChannel1);
  

  //WiFi rezims
  WiFi.mode(WIFI_AP);

  Serial.println("ESP32 AP & STAtion & UDP sistemas tests");
  Serial.print("Soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(APlocal_IP, APgateway, APsubnet) ? "OK" : "Neiet!"); //Sakonfigure tiklu
  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(APssid, APpassword) ? "OK" : "Neiet!"); // Sakonfigure AP
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP()); //IP addrese

  // Uztaisa UDP Portu
  Serial.println("begin UDP port");
  udp.begin(localUdpPort);
  Serial.print("local UDP port: ");
  Serial.println(localUdpPort);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
 //Palaiz SPIFFS, lai varetu dot statiskus HTML failus.  */
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS Neiet");
  } else {
    Serial.println("SPIFFS OK");
  }
  //Iestata statiskos failus no SPIFFS prieks html un js
  server.serveStatic("/", SPIFFS, "/joystick.html");
  server.serveStatic("/virtualjoystick.js", SPIFFS, "/virtualjoystick.js");
  //Izsauc handleJSData funkciju kad JS pieklust sim URL
  server.on("/jsData.html", handleJSData);
  server.begin();
}

void loop() {
  server.handleClient();
  int brightness = map(orangey, -120, 120, 0, 1024);
  int brightness1 = map(cyany, -120, 120, 0, 1024);
  ledcWrite(ledChannel, brightness);
  ledcWrite(ledChannel1, brightness1);
  Serial.printf("OX: %d OY: %d CX: %d CY: %d\n", orangex, orangey, cyanx, cyany);
}
