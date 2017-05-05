String readStringWithFile(String filename) {
  File file = SPIFFS.open(filename, "r");
  String str = "";
  file.seek(0,SeekSet);
  for(int i = 0;i < file.size();i++) {
    char ssidmsg = file.read();
    //Serial.print(ssidmsg);
    str += ssidmsg;
  }
  file.close();
  return str;
}
void showWifiStatue(int state) {
  if(state == WL_CONNECTED) {
    Serial.println("WL_CONNECTED"); 
  } else if(state == WL_NO_SHIELD){
    Serial.println("WL_NO_SHIELD"); 
  } else if(state == WL_IDLE_STATUS){
    Serial.println("WL_IDLE_STATUS"); 
  } else if(state == WL_NO_SSID_AVAIL){
    Serial.println("WL_NO_SSID_AVAIL"); 
  } else if(state == WL_SCAN_COMPLETED){
    Serial.println("WL_SCAN_COMPLETED"); 
  } else if(state == WL_CONNECT_FAILED){
    Serial.println("WL_CONNECT_FAILED"); 
  } else if(state == WL_CONNECTION_LOST){
    Serial.println("WL_CONNECTION_LOST"); 
  } else if(state == WL_DISCONNECTED){
    Serial.println("WL_DISCONNECTED"); 
  }
}
void readSSIDJson() {
  String json = readStringWithFile("/ssid.json");
  Serial.print("ssid json:");
  Serial.println(json);
  char jsonchar[json.length()];
  json.toCharArray(jsonchar, json.length()+1);
  //Serial.println(jsonchar);
  StaticJsonBuffer<64> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(jsonchar);
  if (!root.success()) {
    Serial.println("parseObject() failed");
  } else {
    String ssidname = root["SSID"];
    String ssidpw = root["Passwd"];
    //ssid = ssidname.c_str();
    //password = ssidpw.c_str();
    ssid = ssidname;
    password = ssidpw;
    Serial.println("============JSON============");
    Serial.print("ssid:");
    Serial.println(ssid);
    Serial.print("passwd:");
    Serial.println(password);
    Serial.println("============JSON============");
    
  }
}
void handleUdpFinder() {
  int noBytes = finderUdp.parsePacket();
  String received_command = "";
  if (noBytes) {
    finderUdp.read(finderPacketBuffer,noBytes);
    for (int i = 1;i <= noBytes;i++) {
      received_command = received_command + char(finderPacketBuffer[i - 1]);
      if (i % 32 == 0){}
      else ;
    }
    Serial.println(received_command);
    if(received_command.indexOf("{") != -1 && received_command.indexOf("}") != -1 ) {
      StaticJsonBuffer<64> jsonBuffer;
      JsonObject& jsonRoot = jsonBuffer.parseObject(received_command);
      int action = jsonRoot["a"];
      if(action == 0) {
        jsonRoot["a"] = 1;
        jsonRoot["cid"] = (String)ESP.getChipId();
        finderUdp.beginPacket(finderUdp.remoteIP(), finderUdp.remotePort());
        char resultBuffer[64];
        jsonRoot.printTo(resultBuffer, sizeof(resultBuffer));
        finderUdp.print(resultBuffer);
        finderUdp.endPacket();
      } else if(action == 2) {
        int handlePin = jsonRoot["pin"];
        int pinStatue = jsonRoot["d"];
      }
    }
  }
}
String getContentType(String filename){
  if(server.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}
void handleNotFound() {
  String path = server.uri();
  Serial.print("load url:");
  Serial.println(path);
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return;
  }
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }
  server.send ( 404, "text/plain", message );
}

