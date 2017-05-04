#define D0 0
#define D1 1
#define D2 2
#define D3 3
#define D4 4
#define D5 5
#define D12 12
#define D13 13
#define D14 14
#define D15 15
#define D16 16
#define PIN_NAME {"D0", "D1", "D2", "D3", "D4", "D5", "D12", "D13", "D14"}
#define PIN_ID {D0, D1, D2, D3, D4, D5, D12, D13, D14}
#define GET_ARRAY_LEN(array,len){len = (sizeof(array) / sizeof(array[0]));}

void handleEvent() {
  File file = SPIFFS.open("/index.html", "r");
  size_t sent = server.streamFile(file, "text/html");
  file.close();
  return;
}
void handlePin() {
  //Serial.println("handlePin");
  if(server.hasArg("mode")) {
    String handleMode = server.arg("mode");
    Serial.print("handleMode");
    Serial.println(handleMode);
    
    String pinName[] = PIN_NAME;
    int pinid[] = PIN_ID;
    if(handleMode == "get_pin") {
      if(server.hasArg("pin")) {
        int pin = server.arg("pin").toInt();
        StaticJsonBuffer<128> jsonBuffer;
        JsonObject& jsonRoot = jsonBuffer.createObject();
        jsonRoot["name"] = pinName[pinIndexOf(pinid, pin)];
        jsonRoot["id"] = pinid[pin];
        jsonRoot["digital"] = digitalRead(pin);
        jsonRoot["analog"] = analogRead(pin);
        char resultBuffer[128];
        jsonRoot.printTo(resultBuffer, sizeof(resultBuffer));
        server.send ( 200, "text/html", resultBuffer);
        return;
      }
    } else if(handleMode == "set_pin") {
      if(server.hasArg("pin")) {
        int pin = server.arg("pin").toInt();
        String statue = "fail";
        Serial.println ("set_pin");
        if(server.hasArg("pin_mode") && server.hasArg("value")) {
          String pinSetMode = server.arg("pin_mode");
          int pinValue = server.arg("value").toInt();
          statue = pinValue;
          Serial.print("pin:");
          Serial.println(pin);
          
          if(pinSetMode == "digital") {
            pinValue = (pinValue > 1 ? 1 : pinValue);
            Serial.print("pinValue:");
            Serial.println(pinValue);
            digitalWrite(pin, pinValue);
          } else if(pinSetMode == "analog") {
            pinValue = (pinValue > 256 ? 1 : pinValue);
            Serial.print("pinValue:");
            Serial.println(pinValue);
            analogWrite(pin, pinValue);
          }
        }
        server.send ( 200, "text/html", statue);
      }
    } else if(handleMode == "get_pinmap") {
      Serial.println("get_pinmap:");
      int pinSize;
      GET_ARRAY_LEN(pinName, pinSize);
      StaticJsonBuffer<1024> jsonBuffer;
      JsonObject& jsonRoot = jsonBuffer.createObject();
      JsonArray& data = jsonRoot.createNestedArray("pin_map");
      for(int i = 0;i < pinSize;i++) {
        JsonObject& pinJsonRoot = jsonBuffer.createObject();
        pinJsonRoot["pin_name"] = pinName[i];
        pinJsonRoot["pin_id"] = pinid[i];
        pinJsonRoot["digital"] = digitalRead(pinid[i]);
        pinJsonRoot["analog"] = analogRead(pinid[i]);
        data.add(pinJsonRoot);
      }
      char resultBuffer[1024];
      jsonRoot.printTo(resultBuffer, sizeof(resultBuffer));
      server.send ( 200, "text/html", resultBuffer);
      return;
    }
  }
}
int pinIndexOf(int pinindex[], int pin) {
  for(int i = 0;i < sizeof(pinindex);i++) {
    if(pinindex[i] == pin) {
      return i;
    }
  }
  return -1;
}
