# BLE_OMRON

Magellan3 use nb-iot Sim7020 

Setting

* Arduino
  
  Tools
  
    -Board : "ESP32 Dev Module"
  
    -Flash Mode : "DIO"
  
    -Flash Frequency : "80MHz"
  
    -CPU Frequency : "240MHz (WiFi/BT)"
  
    -Flash Size : "4MB (32Mb)"
  
    -PSRAM : "Disabled"
  
    -Partition Scheme : "Huge APP (3MB No OTA/1MB SPIFFS)"
  
    -Upload Speed : "921600"

*Increase size on loop (fix  stackoverflow) in file main.cpp 
  
  -Location 
  
     C:\Users\<Name-User>\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.2\cores\esp32\main.cpp
  
  -main.cpp 
  
    -Change size (>=10000)
  
      Defaults : xTaskCreateUniversal(loopTask, "loopTask", 8192, NULL, 1, &loopTaskHandle, CONFIG_ARDUINO_RUNNING_CORE);
  
      Change : xTaskCreateUniversal(loopTask, "loopTask", 10240, NULL, 1, &loopTaskHandle, CONFIG_ARDUINO_RUNNING_CORE);

File

1.BLE_Scan.ino : Example to scan omron sensor and calculator data

2.BLE_scan_postToMagellan.ino : support OMRON USB sensor(mode1&mode2&mode3) and EP sensor send to Magellan3
