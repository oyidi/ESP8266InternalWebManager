void handleWifiAccess() {
  if(server.hasArg("a")) {
    String action = server.arg("a");
    if(action == "set_wlan") {
      String newSSID = server.arg("ssid");
      String newPasswd = server.arg("passwd");
      StaticJsonBuffer<128> jsonBuffer;
      JsonObject& jsonRoot = jsonBuffer.createObject();
      jsonRoot["SSID"] = newSSID;
      jsonRoot["Passwd"] = newPasswd;
      char resultBuffer[128];
      jsonRoot.printTo(resultBuffer, sizeof(resultBuffer));
      Serial.print("new wifi json:");
      Serial.println(resultBuffer);
      File file = SPIFFS.open("/ssid.json", "w");
      file.print(resultBuffer);
      file.close();
      
      Serial.println("set new wifi access");
      Serial.print("ssid:");
      Serial.println(newSSID);
      Serial.print("passwd:");
      Serial.println(newPasswd);
      //ssid = newSSID.c_str();
      //password = newPasswd.c_str();
      ssid = newSSID;
      password = newPasswd;
      needConenct = true;
      lastConnectTime = 0;
      server.send ( 200, "text/html", newSSID);
    } else if(action == "get_wlan") {
      Serial.println("get_wlan");
      String ssidProfile = readStringWithFile("/ssid.json");
      Serial.println(ssidProfile);
      server.send ( 200, "text/html", ssidProfile);
    }
  }
}
void handleSetup() {
  if(server.hasArg("p")) {
    String profile = server.arg("p");
    if(profile == "get_wlan") {
      String ssidProfile = readStringWithFile("/ssid.json");
      server.send ( 200, "text/html", ssidProfile);
    }
  }
}
