#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <FS.h>
#include <ArduinoJson.h>

String ssid = "";
String password = "";

ESP8266WebServer server ( 80 );

byte finderPacketBuffer[128];

WiFiUDP finderUdp;

int connectCount = 0;
bool needConenct = false;
unsigned long apShutDownTime = 6000000L;

unsigned long lastConnectTime = 0;
unsigned long retryConnectTime = 60000;


void readTimerProfile();
void updateNTP();
void readSSIDJson();
void setup() {
  Serial.begin ( 115200 );
  Serial.println("");
  SPIFFS.begin();
  // 设置内网
  IPAddress softLocal(192,168,128,1);
  IPAddress softGateway(192,168,128,1);
  IPAddress softSubnet(255,255,255,0);
  WiFi.softAPConfig(softLocal, softGateway, softSubnet);
  String apName = ("AntsNet_"+(String)ESP.getChipId());
  const char *softAPName = apName.c_str();
  WiFi.softAP(softAPName, "adminadmin");
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  Serial.print("softAPName: ");
  Serial.println(apName);
  // 从spifs读取wifi设置并连接wifi
  Serial.println("");
  Serial.println("=====Read SSID Info=====");
  readSSIDJson();
  Serial.println("=====Read SSID Info=====");
  Serial.println("begin wifi");
  Serial.print("ssid:");
  Serial.println(ssid.c_str());
  Serial.print("passwd:");
  Serial.println(password.c_str());
  WiFi.begin ( ssid.c_str(), password.c_str() );
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 1000 );
    Serial.print ( "." );
    //showWifiStatue(WiFi.status());
    //Serial.println(WiFi.status());
    if(connectCount > 30) {
      Serial.println( "Connect fail!" );
      break;
    }
    connectCount += 1;
  }
  lastConnectTime = millis();
  if(WiFi.status() == WL_CONNECTED) {
    Serial.println ( "" );
    Serial.print ( "Connected to " );
    Serial.println ( ssid );
    Serial.print ( "IP address: " );
    Serial.println ( WiFi.localIP() );
    connectCount = 0;
  }
  Serial.println ( "HTTP server started" );
  finderUdp.begin(8266);

  server.on ("/", handleEvent);
  server.on("/io", HTTP_GET, handlePin);
  server.on("/wifi_access", HTTP_POST, handleWifiAccess);
  server.onNotFound ( handleNotFound );
  server.begin();
  Serial.println("Start Success.");
}
void loop() {
  // 更新内网查询udp
  handleUdpFinder();
  // wifi重连
  if(WiFi.status() != WL_CONNECTED || needConenct == true) {
    if(millis() - lastConnectTime > retryConnectTime) {
      Serial.println( "Try reCconnect" );
      connectCount = 0;
      Serial.print("ssid:");
      Serial.println(ssid.c_str());
      Serial.print("passwd:");
      Serial.println(password.c_str());
      WiFi.begin ( ssid.c_str(), password.c_str() );
      while ( WiFi.status() != WL_CONNECTED ) {
        delay ( 500 );
        connectCount += 1;
        if(connectCount > 30) {
          Serial.println( F("Connect fail! Wait next time") );
          break;
        }
        Serial.print ( "." );
      }
      if(WiFi.status() == WL_CONNECTED) {
        Serial.println ( "" );
        Serial.print ( "Connected to " );
        Serial.println ( ssid );
        Serial.print ( "IP address: " );
        Serial.println ( WiFi.localIP() );
      }
      lastConnectTime = millis();
      needConenct = false;
    }
  }
  if(apShutDownTime != 0) {
    if(millis() > apShutDownTime) {
      WiFi.softAPdisconnect();
      apShutDownTime = 0;
      Serial.println(F("AP Shut Down"));
    }
  }
  // 路由更新
  server.handleClient();
}
