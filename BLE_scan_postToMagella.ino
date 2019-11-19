#include "BLEDevice.h"
#include "BLEScan.h"
#include "Magellan_NB_ACER.h"
#include "nvs_flash.h"

//add address to EEPROM
#include <EEPROM.h>

static const char* LOG_TAG = "Client";

Magellan_NB_ACER magel;
char auth[] = "11b528c0-bf1e-11e9-810a-f990cf998f9d";
const char* ManufacturerData_OMRON = "d502";
String ManufacturerData;
String addressArray_OMRON[10];
String address_BLE;
static BLEAddress* serverAddress;
BLEScan* pBLEScan;

bool calculator_ready = false;
bool doScan = false;
bool ReScan = false;
int count = 0;

//define the number of bytes to access
#define EEPROM_SIZE 50

//Magellan String
String payload_1;
String payload_2;
String payload_3;
String payload_4;



class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    /**
        Called for each advertising BLE server.
    */
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.print(F("BLE Advertised Device found: "));
      Serial.println(advertisedDevice.toString().c_str());
      ManufacturerData = (String)BLEUtils::buildHexData(nullptr, (uint8_t *)advertisedDevice.getManufacturerData().data(), advertisedDevice.getManufacturerData().length());
      ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
      ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
      String Manu = ManufacturerData.substring(0, 4);
      //Serial.println(Manu); 
      if ( Manu == ManufacturerData_OMRON) {
        Serial.println(F(" ***** Device OMRON Found *****"));
        BLEDevice::getScan()->stop();
        serverAddress = new BLEAddress(advertisedDevice.getAddress());
        address_BLE = serverAddress->toString().c_str();
        //check address
        Serial.println("Address BLE : " + address_BLE);
        if (addressArray_OMRON[9] == "") {  //check array à¸—à¸µà¹ˆ 3 à¸§à¹ˆà¸²à¸‡à¸¡à¸±à¹‰à¸¢
          int x = 0;
          for (int i = 0; i < 10; i++) {
            if (addressArray_OMRON[i] == address_BLE) {
              x++;
            }
            if (addressArray_OMRON[i] == "" && x == 0) {
              addressArray_OMRON[i] = address_BLE;
              break;
            }

          }
        }


        calculator_ready = true;
      } // Found our server
    } // onResult
}; // MyAdvertisedDeviceCallbacks

void setup() {
  Serial.begin(115200);
  magel.begin(auth);
  Serial.println(F("********** Start BLE Scan **********"));
  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(200);
  pBLEScan->setWindow(10);
  pBLEScan->setActiveScan(false);
  pBLEScan->start(0, false);
}

void loop() {

  if (calculator_ready == true) {
    //calculator address ble
    String address_1 = address_BLE.substring(0, 2);
    String address_2 = address_BLE.substring(3, 5);
    String address_3 = address_BLE.substring(6, 8);
    String address_4 = address_BLE.substring(9, 11);
    String address_5 = address_BLE.substring(12, 14);
    String address_6 = address_BLE.substring(15, 17);
    String address_total = address_1 + "." + address_2 + "." + address_3 + "." + address_4 + "." + address_5 + "." + address_6;
    String address_total_S = "\"" + address_total + "\"";

    //    ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
    //    ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
    // Type sensor omron

    //*** USB ***
    if (ManufacturerData.length() == 42) {
      Serial.println(F(" - USB TYPE"));

      //Temp Calculator
      String temp_1 = ManufacturerData.substring(8, 10);
      String temp_2 = ManufacturerData.substring(10, 12);
      String temp = temp_2 + temp_1;
      int temp_dec = strtol(temp.c_str(), NULL, HEX);
      float temp_total = temp_dec / 100.f;
      //Serial.println(temp);
      Serial.print(F("Temp : "));
      Serial.println(String(temp_total));

      //Hum Calculator
      String hum_1 = ManufacturerData.substring(12, 14);
      String hum_2 = ManufacturerData.substring(14, 16);
      String hum = hum_2 + hum_1;
      int hum_dec = strtol(hum.c_str(), NULL, HEX);
      float hum_total = hum_dec / 100.f;
      //Serial.println(hum);
      Serial.print(F("Humidity : "));
      Serial.println(String(hum_total));

      //Light Calculator
      String light_1 = ManufacturerData.substring(16, 18);
      String light_2 = ManufacturerData.substring(18, 20);
      String light = light_2 + light_1;
      int light_total = strtol(light.c_str(), NULL, HEX);
      Serial.print(F("Light : "));
      Serial.println(String(light_total));

      //Barometric Pressure Calculator
      String baro_1 = ManufacturerData.substring(20, 22);
      String baro_2 = ManufacturerData.substring(22, 24);
      String baro_3 = ManufacturerData.substring(24, 26);
      String baro_4 = ManufacturerData.substring(26, 28);
      String baro = baro_4 + baro_3 + baro_2 + baro_1;
      int baro_dec = strtol(baro.c_str(), NULL, HEX);
      float baro_total = baro_dec / 1000.f;
      Serial.print(F("Barometric Pressure : "));
      Serial.println(String(baro_total));

      //Sound Noise Calculator
      String sound_1 = ManufacturerData.substring(28, 30);
      String sound_2 = ManufacturerData.substring(30, 32);
      String sound = sound_2 + sound_1;
      int sound_dec = strtol(sound.c_str(), NULL, HEX);
      float sound_total = sound_dec / 100.f;
      Serial.print(F("Sound Noise : "));
      Serial.println(String(sound_total));

      //eTVOC Calculator
      String etvoc_1 = ManufacturerData.substring(32, 34);
      String etvoc_2 = ManufacturerData.substring(34, 36);
      String etvoc = etvoc_2 + etvoc_1;
      int etvoc_total = strtol(etvoc.c_str(), NULL, HEX);
      Serial.print(F("eTVOC : "));
      Serial.println(String(etvoc_total));

      //eCO2 Calculator
      String eco2_1 = ManufacturerData.substring(36, 38);
      String eco2_2 = ManufacturerData.substring(38, 40);
      String eco2 = eco2_2 + eco2_1;
      int eco2_total = strtol(eco2.c_str(), NULL, HEX);
      Serial.print(F("eCO2 : "));
      Serial.println(String(eco2_total));

      ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
      ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());

      //Send data to Magellan
      if (addressArray_OMRON[0] == address_BLE) {
        Serial.println(F("prepare to send data_1"));
        payload_1 = "{\"Address_1\":" + address_total_S + ",\"Temp_1\":" + String(temp_total) + ",\"Hum_1\":" + String(hum_total) + ",\"Light_1\":" + String(light_total) + ",\"Pressure_1\":" + String(baro_total) + ",\"Noise_1\":" + String(sound_total) + ",\"eTVOC_1\":" + String(etvoc_total) + ",\"eCO2_1\":" + String(eco2_total) + "}";
        ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
        ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
        magel.post(payload_1);
        delay(100);
        //              Serial.println(payload_1.length());
        //              ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
        //              ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
      }
      if (addressArray_OMRON[1] == address_BLE) {
        Serial.println(F("prepare to send data_2"));
        payload_2 = "{\"Address_2\":" + address_total_S + ",\"Temp_2\":" + String(temp_total) + ",\"Hum_2\":" + String(hum_total) + ",\"Light_2\":" + String(light_total) + ",\"Pressure_2\":" + String(baro_total) + ",\"Noise_2\":" + String(sound_total) + ",\"eTVOC_2\":" + String(etvoc_total) + ",\"eCO2_2\":" + String(eco2_total) + "}";
        ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
        ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
        magel.post(payload_2);
        delay(100);
        //              Serial.println(payload_2.length());
        //              ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
        //              ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
      }
      if (addressArray_OMRON[2] == address_BLE) {
        Serial.println(F("prepare to send data_3"));
        payload_3 = "{\"Address_3\":" + address_total_S + ",\"Temp_3\":" + String(temp_total) + ",\"Hum_3\":" + String(hum_total) + ",\"Light_3\":" + String(light_total) + ",\"Pressure_3\":" + String(baro_total) + ",\"Noise_3\":" + String(sound_total) + ",\"eTVOC_3\":" + String(etvoc_total) + ",\"eCO2_3\":" + String(eco2_total) + "}";
        ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
        ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
        magel.post(payload_3);
        delay(100);
        //              Serial.println(payload_3.length());
        //              ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
        //              ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
      }
    }
    // *** BAG / EP ***
    else if (ManufacturerData.length() == 44) {
      Serial.println(F(" - BAG or EP TYPE"));

      //Temp Calculator
      String temp_1 = ManufacturerData.substring(7, 9);
      String temp_2 = ManufacturerData.substring(9, 11);
      String temp = temp_2 + temp_1;
      int temp_dec = strtol(temp.c_str(), NULL, HEX);
      float temp_total = temp_dec / 100.f;
      Serial.print(F("Temp : "));
      Serial.println(String(temp_total));

      //Hum Calculator
      String hum_1 = ManufacturerData.substring(11, 13);
      String hum_2 = ManufacturerData.substring(13, 15);
      String hum = hum_2 + hum_1;
      int hum_dec = strtol(hum.c_str(), NULL, HEX);
      float hum_total = hum_dec / 100.f;
      Serial.print(F("Hum : "));
      Serial.println(String(hum_total));

      //Light Calculator
      String light_1 = ManufacturerData.substring(15, 17);
      String light_2 = ManufacturerData.substring(17, 19);
      String light = light_2 + light_1;
      int light_total = strtol(light.c_str(), NULL, HEX);
      Serial.print(F("Light : "));
      Serial.println(String(light_total));

      //UV Calculator
      String uv_1 = ManufacturerData.substring(19, 21);
      String uv_2 = ManufacturerData.substring(21, 23);
      String uv = uv_2 + uv_1;
      int uv_dec = strtol(uv.c_str(), NULL, HEX);
      float uv_total = uv_dec / 100.f;
      Serial.print(F("UV : "));
      Serial.println(String(uv_total));

      //Pressure Calculator
      String pres_1 = ManufacturerData.substring(23, 25);
      String pres_2 = ManufacturerData.substring(25, 27);
      String pres = pres_2 + pres_1;
      int pres_dec = strtol(pres.c_str(), NULL, HEX);
      float pres_total = pres_dec / 10.f;
      Serial.print(F("Pressure : "));
      Serial.println(String(pres_total));

      //Sound Noise Calculator
      String noise_1 = ManufacturerData.substring(27, 29);
      String noise_2 = ManufacturerData.substring(29, 31);
      String noise = noise_2 + noise_1;
      int noise_dec = strtol(noise.c_str(), NULL, HEX);
      float noise_total = noise_dec / 100.f;
      Serial.print(F("Sound Noise : "));
      Serial.println(String(noise_total));

      //Discomfort Calculator
      String com_1 = ManufacturerData.substring(31, 33);
      String com_2 = ManufacturerData.substring(33, 35);
      String com = com_2 + com_1;
      int com_dec = strtol(com.c_str(), NULL, HEX);
      float com_total = com_dec / 100.f;
      Serial.print(F("Discomfort : "));
      Serial.println(String(com_total));

      //Heat stroke Calculator
      String heat_1 = ManufacturerData.substring(35, 37);
      String heat_2 = ManufacturerData.substring(37, 39);
      String heat = heat_2 + heat_1;
      int heat_dec = strtol(heat.c_str(), NULL, HEX);
      float heat_total = heat_dec / 100.f;
      Serial.print(F("Heat stroke : "));
      Serial.println(String(heat_total));

      //RFU Calculator
      String rfu_1 = ManufacturerData.substring(39, 41);
      String rfu_2 = ManufacturerData.substring(41, 43);
      String rfu = rfu_2 + rfu_1;
      int rfu_dec = strtol(rfu.c_str(), NULL, HEX);
      float rfu_total = rfu_dec / 100.f;
      Serial.print(F("RFU : "));
      Serial.println(String(rfu_total));

      //Battery voltage Calculator
      String bat = ManufacturerData.substring(43, 44);
      int bat_total = strtol(bat.c_str(), NULL, HEX);
      Serial.print(F("Battery Voltage : "));
      Serial.println(String(bat_total));

      ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
      ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());

      //Send data to Magellan
      if (addressArray_OMRON[0] == address_BLE) {
        Serial.println(F("prepare to send data_1"));
        payload_1 = "{\"Address_1\":" + address_total_S + ",\"Temp_1\":" + String(temp_total) + ",\"Hum_1\":" + String(hum_total) + ",\"Light_1\":" + String(light_total) + ",\"Pressure_1\":" + String(pres_total) + ",\"Noise_1\":" + String(noise_total) + ",\"Discomfort_1\":" + String(com_total) + ",\"HeatStroke_1\":" + String(heat_total) + ",\"RFU_1\":" + String(rfu_total) + ",\"Battery_1\":" + String(bat_total) +  "}";
        ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
        ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
        magel.post(payload_1);
        delay(100);
        //              Serial.println(payload_1.length());
        //              ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
        //              ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
      }
      if (addressArray_OMRON[1] == address_BLE) {
        Serial.println(F("prepare to send data_2"));
        payload_2 = "{\"Address_2\":" + address_total_S + ",\"Temp_2\":" + String(temp_total) + ",\"Hum_2\":" + String(hum_total) + ",\"Light_2\":" + String(light_total) + ",\"Pressure_2\":" + String(pres_total) + ",\"Noise_2\":" + String(noise_total) + ",\"Discomfort_2\":" + String(com_total) + ",\"HeatStroke_2\":" + String(heat_total) + ",\"RFU_2\":" + String(rfu_total) + ",\"Battery_2\":" + String(bat_total) +  "}";
        ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
        ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
        magel.post(payload_2);
        delay(100);
        //              Serial.println(payload_2.length());
        //              ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
        //              ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
      }
      if (addressArray_OMRON[2] == address_BLE) {
        Serial.println(F("prepare to send data_3"));
        payload_3 = "{\"Address_3\":" + address_total_S + ",\"Temp_3\":" + String(temp_total) + ",\"Hum_3\":" + String(hum_total) + ",\"Light_3\":" + String(light_total) + ",\"Pressure_3\":" + String(pres_total) + ",\"Noise_3\":" + String(noise_total) + ",\"Discomfort_3\":" + String(com_total) + ",\"HeatStroke_3\":" + String(heat_total) + ",\"RFU_3\":" + String(rfu_total) + ",\"Battery_3\":" + String(bat_total) +  "}";
        ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
        ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
        magel.post(payload_3);
        delay(100);
        //              Serial.println(payload_3.length());
        //              ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
        //              ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
      }
    }
    calculator_ready = false;
    count++;
    Serial.println(count);
    delay(100);
    doScan = true;
  }
  if (doScan == true) {
    Serial.println(F("loop do scan"));
    BLEDevice::getScan()->start(0);
    if (count == 400) {
      count = 0;
      BLEDevice::getScan()->stop();
      delay(10);
      ESP.restart();
    }
  }
  delay(100);
}

