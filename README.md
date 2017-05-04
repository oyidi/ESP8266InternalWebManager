# ESP8266InternalWebManager
ESP8266 Manager with Arduino IDE

ESP8266的内置管理器，可以对wifi接入点和引脚的输出输入进行管理和查询。

##### 基本需求：
 
 Arduino版本1.8.2:https://www.arduino.cc/en/main/software
 
 使用的ESP8266 core for Arduino的版本为2.3.0：https://github.com/esp8266/Arduino
 
 为需要引入ArduinoJson库:https://github.com/bblanchon/ArduinoJson

##### 使用方法：

 1、烧写程序，Flash Size为4M(1M SPIFFS)，然后通过ESP8266 Data Sketch Data Upload将内置的服务器页面烧写到SPIFFS中；
 
 2、重启ESP8266之后使用wifi设备连接ESP8266，密码adminadmin，在浏览器中输入192.168.128.1进入管理界面，在wifi管理中输入需要连接的wifi热点名称及密码；
 
 3、连接上ESP8266之也可以对引脚的输入输出进行管理，支持analog以及digital输出。
