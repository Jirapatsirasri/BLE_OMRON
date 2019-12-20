//add library BLE
#include "BLEDevice.h"
#include "BLEScan.h"
#include "BLEAdvertisedDevice.h"

//add address to EEPROM
#include <EEPROM.h>

//add library Aisplatform
#include "Magellan7020.h"

static const char* LOG_TAG = "Client";

const char* ManufacturerData_OMRON = "d502";
String addressArray_OMRON[15];
BLEScan* pBLEScan;
String ManufacturerData;
String address_BLE;
bool check_EEPROM_ADDRESS = false;
bool calculator_ready = false;
bool doScan = false;
int count = 0;
bool debug = false;

//Add mag3
Magellan7020 magel;
String token;
char auth[] = "your-key";


bool switch_passive_to_active_ble = false;
bool switch_active_to_passive_ble = false;

void writeString(char add, String data);
String read_String(char add);

//Part Write & Read String to EEPROM on ESP32 (Save Data when reset data not loss)
void writeString(char add, String data)
{
  int _size = data.length();
  int i;
  for (i = 0; i < _size; i++)
  {
    EEPROM.write(add + i, data[i]);
  }
  EEPROM.write(add + _size, '\0'); //Add termination null character for String Data
  EEPROM.commit();
}

String read_String(char add)
{
  int i;
  char data[100]; //Max 100 Bytes
  int len = 0;
  unsigned char k;
  k = EEPROM.read(add);
  while (k != '\0' && len < 500) //Read until null character
  {
    k = EEPROM.read(add + len);
    data[len] = k;
    len++;
  }
  data[len] = '\0';
  return String(data);
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    /**
        Called for each advertising BLE server.
    */
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.print(F("BLE Advertised Device found: "));
      Serial.println(advertisedDevice.toString().c_str());
      Serial.print(F("RSSI : "));
      Serial.println(advertisedDevice.getRSSI());
      char* Manufacturer = BLEUtils::buildHexData(nullptr, (uint8_t *)advertisedDevice.getManufacturerData().data(), advertisedDevice.getManufacturerData().length());
      ManufacturerData = Manufacturer;
      free(Manufacturer);
      String Manu = ManufacturerData.substring(0, 4);
      if ( Manu == ManufacturerData_OMRON) {
        Serial.println(F(" ***** Device OMRON Found *****"));
        BLEDevice::getScan()->stop();
        BLEAddress* serverAddress = new BLEAddress(advertisedDevice.getAddress());
        address_BLE = serverAddress->toString().c_str();
        free(serverAddress);
        check_EEPROM_ADDRESS = true;
        vTaskDelay(10);
        calculator_ready = true;
      }//Found our server
    } // onResult
}; // MyAdvertisedDeviceCallbacks

void setup() {
  Serial.begin(115200);
  EEPROM.begin(180);

  //        //clear data in eeprom
  //        for (int i = 0; i < 400; i++) {
  //          writeString(i, "");
  //        }

  //check debug
  debug = true;

  //add Magellan begin
  magel.begin();
  magel.debug = true;
  token = magel.thingsregister();

  if (debug) Serial.println(F("********** Start BLE Scan **********"));
  BLEDevice::init("Gateway ESP32");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(200);
  pBLEScan->setWindow(100);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(0);

}

void loop() {

  //Check Address
  if (check_EEPROM_ADDRESS == true) {
    String receivedData = address_BLE;
    int x = 0;
    int y = 0;
    Serial.print(F("Receive Data : "));
    Serial.println(receivedData);
    Serial.println(F("Loop check eeprom"));
    vTaskDelay(10);
    //***** check address match ? in EEPROM *****
    if (read_String(0) != "") {
      if (read_String(0) == receivedData) {
        addressArray_OMRON[0] = receivedData;
        y++;
        Serial.print(F("Address in Array 0 = "));
        Serial.println(addressArray_OMRON[0]);
        Serial.print(F("Address 1 = "));
        Serial.println(addressArray_OMRON[0]);
      }
      if (read_String(18) == receivedData) {
        addressArray_OMRON[1] = receivedData;
        y++;
        Serial.print(F("Address in Array 1 = "));
        Serial.println(addressArray_OMRON[1]);
        Serial.print(F("Address 2 = "));
        Serial.println(addressArray_OMRON[1]);
      }
      if (read_String(36) == receivedData) {
        addressArray_OMRON[2] = receivedData;
        y++;
        Serial.print(F("Address in Array 2 = "));
        Serial.println(addressArray_OMRON[2]);
        Serial.print(F("Address 3 = "));
        Serial.println(addressArray_OMRON[2]);
      }
      if (read_String(54) == receivedData) {
        addressArray_OMRON[3] = receivedData;
        y++;
        Serial.print(F("Address in Array 3 = "));
        Serial.println(addressArray_OMRON[3]);
        Serial.print(F("Address 4 = "));
        Serial.println(addressArray_OMRON[3]);
      }
      if (read_String(72) == receivedData) {
        addressArray_OMRON[4] = receivedData;
        y++;
        Serial.print(F("Address in Array 4 = "));
        Serial.println(addressArray_OMRON[4]);
        Serial.print(F("Address 5 = "));
        Serial.println(addressArray_OMRON[5]);
      }
      if (read_String(90) == receivedData) {
        addressArray_OMRON[5] = receivedData;
        y++;
        Serial.print(F("Address in Array 5 = "));
        Serial.println(addressArray_OMRON[5]);
        Serial.print(F("Address 6 = "));
        Serial.println(addressArray_OMRON[5]);
      }
      if (read_String(108) == receivedData) {
        addressArray_OMRON[6] = receivedData;
        y++;
        Serial.print(F("Address in Array 6 = "));
        Serial.println(addressArray_OMRON[6]);
        Serial.print(F("Address 7 = "));
        Serial.println(addressArray_OMRON[6]);
      }
      if (read_String(126) == receivedData) {
        addressArray_OMRON[7] = receivedData;
        y++;
        Serial.print(F("Address in Array 7 = "));
        Serial.println(addressArray_OMRON[7]);
        Serial.print(F("Address 8 = "));
        Serial.println(addressArray_OMRON[7]);
      }
      if (read_String(144) == receivedData) {
        addressArray_OMRON[8] = receivedData;
        y++;
        Serial.print(F("Address in Array 8 = "));
        Serial.println(addressArray_OMRON[8]);
        Serial.print(F("Address 9 = "));
        Serial.println(addressArray_OMRON[8]);
      }
      if (read_String(162) == receivedData) {
        addressArray_OMRON[9] = receivedData;
        y++;
        Serial.print(F("Address in Array 9 = "));
        Serial.println(addressArray_OMRON[9]);
        Serial.print(F("Address 10 = "));
        Serial.println(addressArray_OMRON[9]);
      }
    }
    // ----- NOT ADDRESS & ADD ADDRESS TO EEPROM -----
    //***** Address 1 *****
    if (read_String(0) == "" && x == 0 && y == 0) {
      Serial.println(F("Address 1 = NULL"));
      writeString(0, receivedData);
      x++;
      vTaskDelay(10);
      Serial.print(F("Address 1 : "));
      Serial.println(read_String(0));
      addressArray_OMRON[0] = receivedData;
      Serial.print(F("Array_OMRON 0 = "));
      Serial.println(addressArray_OMRON[0]);
      vTaskDelay(100);
    }
    //***** Address 2 *****
    if (read_String(18) == "" && x == 0 && y == 0) {
      Serial.println(F("Address 2 = NULL"));
      writeString(18, receivedData);
      x++;
      vTaskDelay(10);
      Serial.print(F("Address 2 : "));
      Serial.println(read_String(18));
      addressArray_OMRON[1] = receivedData;
      Serial.print(F("Array_OMRON 1 = "));
      Serial.println(addressArray_OMRON[1]);
      vTaskDelay(100);
    }
    //***** Address 3 *****
    if (read_String(36) == "" && x == 0 && y == 0) {
      Serial.println(F("Address 3 = NULL"));
      writeString(36, receivedData);
      x++;
      vTaskDelay(10);
      Serial.print(F("Address 3 : "));
      Serial.println(read_String(36));
      addressArray_OMRON[2] = receivedData;
      Serial.print(F("Array_OMRON 2 = "));
      Serial.println(addressArray_OMRON[2]);
      vTaskDelay(100);
    }
    //***** Address 4 *****
    if (read_String(54) == "" && x == 0 && y == 0) {
      Serial.println(F("Address 4 = NULL"));
      writeString(54, receivedData);
      x++;
      vTaskDelay(10);
      Serial.print(F("Address 4 : "));
      Serial.println(read_String(54));
      addressArray_OMRON[3] = receivedData;
      Serial.print(F("Array_OMRON 3 = "));
      Serial.println(addressArray_OMRON[3]);
      vTaskDelay(100);
    }
    //***** Address 5 *****
    if (read_String(72) == "" && x == 0 && y == 0) {
      Serial.println(F("Address 5 : NULL"));
      writeString(72, receivedData);
      x++;
      vTaskDelay(10);
      Serial.print(F("Address 5 : "));
      Serial.println(read_String(72));
      addressArray_OMRON[4] = receivedData;
      Serial.print(F("Array_OMRON 4 = "));
      Serial.println(addressArray_OMRON[4]);
      vTaskDelay(100);
    }
    //***** Address 6 *****
    if (read_String(90) == "" && x == 0 && y == 0) {
      Serial.println(F("Address 6 : NULL"));
      writeString(90, receivedData);
      x++;
      vTaskDelay(10);
      Serial.print(F("Address 6 : "));
      Serial.println(read_String(90));
      addressArray_OMRON[5] = receivedData;
      Serial.print(F("Array_OMRON 5 = "));
      Serial.println(addressArray_OMRON[5]);
      vTaskDelay(100);
    }
    //***** Address 7 *****
    if (read_String(108) == "" && x == 0 && y == 0) {
      Serial.println(F("Address 7 : NULL"));
      writeString(108, receivedData);
      x++;
      vTaskDelay(10);
      Serial.print(F("Address 7 : "));
      Serial.println(read_String(108));
      addressArray_OMRON[6] = receivedData;
      Serial.print(F("Array_OMRON 6 = "));
      Serial.println(addressArray_OMRON[6]);
      vTaskDelay(100);
    }
    //***** Address 8 *****
    if (read_String(126) == "" && x == 0 && y == 0) {
      Serial.println(F("Address 8 : NULL"));
      writeString(126, receivedData);
      x++;
      vTaskDelay(10);
      Serial.print(F("Address 8 : "));
      Serial.println(read_String(126));
      addressArray_OMRON[7] = receivedData;
      Serial.print(F("Array_OMRON 7 = "));
      Serial.println(addressArray_OMRON[7]);
      vTaskDelay(100);
    }
    //***** Address 9 *****
    if (read_String(144) == "" && x == 0 && y == 0) {
      Serial.println(F("Address 9 : NULL"));
      writeString(144, receivedData);
      x++;
      vTaskDelay(10);
      Serial.print(F("Address 9 : "));
      Serial.println(read_String(144));
      addressArray_OMRON[8] = receivedData;
      Serial.print(F("Array_OMRON 8 = "));
      Serial.println(addressArray_OMRON[8]);
      vTaskDelay(100);
    }
    //***** Address 10 *****
    if (read_String(162) == "" && x == 0 && y == 0) {
      Serial.println(F("Address 10 : NULL"));
      writeString(162, receivedData);
      x++;
      vTaskDelay(10);
      Serial.print(F("Address 10 : "));
      Serial.println(read_String(162));
      addressArray_OMRON[9] = receivedData;
      Serial.print(F("Array_OMRON 9 = "));
      Serial.println(addressArray_OMRON[9]);
      vTaskDelay(100);
    }
    ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
    ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
    vTaskDelay(10);
    if (debug) {
      Serial.println(F(""));
      Serial.print(F("Address 1 : "));
      Serial.println(read_String(0));
      Serial.print(F("Address 2 : "));
      Serial.println(read_String(18));
      Serial.print(F("Address 3 : "));
      Serial.println(read_String(36));
      Serial.print(F("Address 4 : "));
      Serial.println(read_String(54));
      Serial.print(F("Address 5 : "));
      Serial.println(read_String(72));
      Serial.print(F("Address 6 : "));
      Serial.println(read_String(90));
      Serial.print(F("Address 7 : "));
      Serial.println(read_String(108));
      Serial.print(F("Address 8 : "));
      Serial.println(read_String(126));
      Serial.print(F("Address 9 : "));
      Serial.println(read_String(144));
      Serial.print(F("Address 10 : "));
      Serial.println(read_String(162));
    }
    check_EEPROM_ADDRESS = false;
    vTaskDelay(100);
  }

  //-------------------- Part Calculator --------------------------
  if (calculator_ready == true) {
    //calculator address ble
    vTaskDelay(10);
    if (debug) Serial.println(F("----- Calculator Ready -----"));
    if (debug) Serial.println(address_BLE);
    if (debug) Serial.println(ManufacturerData);
    String address_1 = address_BLE.substring(0, 2);
    String address_2 = address_BLE.substring(3, 5);
    String address_3 = address_BLE.substring(6, 8);
    String address_4 = address_BLE.substring(9, 11);
    String address_5 = address_BLE.substring(12, 14);
    String address_6 = address_BLE.substring(15, 17);
    String address_total = address_1 + "." + address_2 + "." + address_3 + "." + address_4 + "." + address_5 + "." + address_6;
    String address_total_S = "\"" + address_total + "\"";
    vTaskDelay(100);
    String type = ManufacturerData.substring(4, 6);

    // Type sensor omron
    //----- Advertising -----
    //*** USB ***
    if (ManufacturerData.length() == 42) {
      Serial.println(F(" - USB TYPE"));

      //Calculate Mode : Sensor data
      //Mode Sensor Data (----- Mode 1 -----)
      if (type == "01" || type == "03") {
        //Temp Calculator
        String temp_1 = ManufacturerData.substring(8, 10);
        String temp_2 = ManufacturerData.substring(10, 12);
        String temp = temp_2 + temp_1;
        int temp_dec = strtol(temp.c_str(), NULL, HEX);
        float temp_total = temp_dec / 100.f;
        Serial.print(F("Temp : "));
        Serial.println(String(temp_total));

        //Hum Calculator
        String hum_1 = ManufacturerData.substring(12, 14);
        String hum_2 = ManufacturerData.substring(14, 16);
        String hum = hum_2 + hum_1;
        int hum_dec = strtol(hum.c_str(), NULL, HEX);
        float hum_total = hum_dec / 100.f;
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
        vTaskDelay(100);

        //Send data to Magellan 3
        if (addressArray_OMRON[0] == address_BLE) {
          Serial.println(F("prepare to send data_1"));
          vTaskDelay(100);
          String payload_1 = "{\"Address_1\":" + address_total_S + ",\"Temp_1\":" + String(temp_total) + ",\"Hum_1\":" + String(hum_total) + ",\"Light_1\":" + String(light_total) + ",\"Pressure_1\":" + String(baro_total) + ",\"Noise_1\":" + String(sound_total) + ",\"eTVOC_1\":" + String(etvoc_total) + ",\"eCO2_1\":" + String(eco2_total) + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          vTaskDelay(100);
          magel.report(payload_1, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[1] == address_BLE) {
          Serial.println(F("prepare to send data_2"));
          vTaskDelay(100);
          String payload_2 = "{\"Address_2\":" + address_total_S + ",\"Temp_2\":" + String(temp_total) + ",\"Hum_2\":" + String(hum_total) + ",\"Light_2\":" + String(light_total) + ",\"Pressure_2\":" + String(baro_total) + ",\"Noise_2\":" + String(sound_total) + ",\"eTVOC_2\":" + String(etvoc_total) + ",\"eCO2_2\":" + String(eco2_total) + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          vTaskDelay(100);
          magel.report(payload_2, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[2] == address_BLE) {
          Serial.println(F("prepare to send data_3"));
          vTaskDelay(100);
          String payload_3 = "{\"Address_3\":" + address_total_S + ",\"Temp_3\":" + String(temp_total) + ",\"Hum_3\":" + String(hum_total) + ",\"Light_3\":" + String(light_total) + ",\"Pressure_3\":" + String(baro_total) + ",\"Noise_3\":" + String(sound_total) + ",\"eTVOC_3\":" + String(etvoc_total) + ",\"eCO2_3\":" + String(eco2_total) + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          vTaskDelay(100);
          magel.report(payload_3, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[3] == address_BLE) {
          Serial.println(F("prepare to send data_4"));
          String payload_4 = "{\"Address_4\":" + address_total_S + ",\"Temp_4\":" + String(temp_total) + ",\"Hum_4\":" + String(hum_total) + ",\"Light_4\":" + String(light_total) + ",\"Pressure_4\":" + String(baro_total) + ",\"Noise_4\":" + String(sound_total) + ",\"eTVOC_4\":" + String(etvoc_total) + ",\"eCO2_4\":" + String(eco2_total) + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          magel.report(payload_4, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[4] == address_BLE) {
          Serial.println(F("prepare to send data_5"));
          String payload_5 = "{\"Address_5\":" + address_total_S + ",\"Temp_5\":" + String(temp_total) + ",\"Hum_5\":" + String(hum_total) + ",\"Light_5\":" + String(light_total) + ",\"Pressure_5\":" + String(baro_total) + ",\"Noise_5\":" + String(sound_total) + ",\"eTVOC_5\":" + String(etvoc_total) + ",\"eCO2_5\":" + String(eco2_total) + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          magel.report(payload_5, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[5] == address_BLE) {
          Serial.println(F("prepare to send data_6"));
          String payload_6 = "{\"Address_6\":" + address_total_S + ",\"Temp_6\":" + String(temp_total) + ",\"Hum_6\":" + String(hum_total) + ",\"Light_6\":" + String(light_total) + ",\"Pressure_6\":" + String(baro_total) + ",\"Noise_6\":" + String(sound_total) + ",\"eTVOC_6\":" + String(etvoc_total) + ",\"eCO2_6\":" + String(eco2_total) + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          magel.report(payload_6, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[6] == address_BLE) {
          Serial.println(F("prepare to send data_7"));
          String payload_7 = "{\"Address_7\":" + address_total_S + ",\"Temp_7\":" + String(temp_total) + ",\"Hum_7\":" + String(hum_total) + ",\"Light_7\":" + String(light_total) + ",\"Pressure_7\":" + String(baro_total) + ",\"Noise_7\":" + String(sound_total) + ",\"eTVOC_7\":" + String(etvoc_total) + ",\"eCO2_7\":" + String(eco2_total) + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          magel.report(payload_7, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[7] == address_BLE) {
          Serial.println(F("prepare to send data_8"));
          String payload_8 = "{\"Address_8\":" + address_total_S + ",\"Temp_8\":" + String(temp_total) + ",\"Hum_8\":" + String(hum_total) + ",\"Light_8\":" + String(light_total) + ",\"Pressure_8\":" + String(baro_total) + ",\"Noise_8\":" + String(sound_total) + ",\"eTVOC_8\":" + String(etvoc_total) + ",\"eCO2_8\":" + String(eco2_total) + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          magel.report(payload_8, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[8] == address_BLE) {
          Serial.println(F("prepare to send data_9"));
          String payload_9 = "{\"Address_9\":" + address_total_S + ",\"Temp_9\":" + String(temp_total) + ",\"Hum_9\":" + String(hum_total) + ",\"Light_9\":" + String(light_total) + ",\"Pressure_9\":" + String(baro_total) + ",\"Noise_9\":" + String(sound_total) + ",\"eTVOC_9\":" + String(etvoc_total) + ",\"eCO2_9\":" + String(eco2_total) + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          magel.report(payload_9, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[9] == address_BLE) {
          Serial.println(F("prepare to send data_10"));
          String payload_10 = "{\"Address_10\":" + address_total_S + ",\"Temp_10\":" + String(temp_total) + ",\"Hum_10\":" + String(hum_total) + ",\"Light_10\":" + String(light_total) + ",\"Pressure_10\":" + String(baro_total) + ",\"Noise_10\":" + String(sound_total) + ",\"eTVOC_10\":" + String(etvoc_total) + ",\"eCO2_10\":" + String(eco2_total) + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          magel.report(payload_10, token);
          vTaskDelay(100);
        }
        vTaskDelay(100);
        switch_passive_to_active_ble = true;
      }

      //Calculate Mode : Calculator data
      //Mode Calculator data (----- Mode 2 -----)
      else if (type == "02") {
        //Discomfort Index Calculator
        String DI_1 = ManufacturerData.substring(8, 10);
        String DI_2 = ManufacturerData.substring(10, 12);
        String DI = DI_2 + DI_1;
        int DI_dec = strtol(DI.c_str(), NULL, HEX);
        float DI_total = DI_dec / 100.f;
        Serial.print(F("Discomfort Index : "));
        Serial.println(String(DI_total));

        //Heat Stroke Calculator
        String Heat_1 = ManufacturerData.substring(12, 14);
        String Heat_2 = ManufacturerData.substring(14, 16);
        String Heat = Heat_2 + Heat_1;
        int Heat_dec = strtol(Heat.c_str(), NULL, HEX);
        float Heat_total = Heat_dec / 100.f;
        Serial.print(F("Heat Stroke : "));
        Serial.println(String(Heat_total));

        //SI value Calculator
        String SI_1 = ManufacturerData.substring(18, 20);
        String SI_2 = ManufacturerData.substring(20, 22);
        String SI = SI_2 + SI_1;
        int SI_dec = strtol(SI.c_str(), NULL, HEX);
        float SI_total = SI_dec / 10.f;
        Serial.print(F("SI : "));
        Serial.println(String(SI_total));

        //PGA Calculator
        String PGA_1 = ManufacturerData.substring(22, 24);
        String PGA_2 = ManufacturerData.substring(24, 26);
        String PGA = PGA_2 + PGA_1;
        int PGA_dec = strtol(PGA.c_str(), NULL, HEX);
        float PGA_total = PGA_dec / 10.f;
        Serial.print(F("PGA : "));
        Serial.println(String(PGA_total));

        //Seismic intensity Calculator
        String Seis_1 = ManufacturerData.substring(26, 28);
        String Seis_2 = ManufacturerData.substring(28, 30);
        String Seis = Seis_2 + Seis_1;
        int Seis_dec = strtol(Seis.c_str(), NULL, HEX);
        float Seis_total = Seis_dec / 1000.f;
        Serial.print(F("Seismic Intensity : "));
        Serial.println(String(Seis_total));

        //Acceleration (X-axis) Calculator
        String x_1 = ManufacturerData.substring(30, 32);
        String x_2 = ManufacturerData.substring(32, 34);
        String x = x_2 + x_1;
        int x_dec = strtol(x.c_str(), NULL, HEX);
        float x_total = x_dec / 10.f;
        Serial.print(F("X-axis : "));
        Serial.println(String(x_total));

        //Acceleration (Y-axis) Calculator
        String y_1 = ManufacturerData.substring(34, 36);
        String y_2 = ManufacturerData.substring(36, 38);
        String y = y_2 + y_1;
        int y_dec = strtol(y.c_str(), NULL, HEX);
        float y_total = y_dec / 10.f;
        Serial.print(F("Y-axis : "));
        Serial.println(String(y_total));

        //Acceleration (Z-axis) Calculator
        String z_1 = ManufacturerData.substring(38, 40);
        String z_2 = ManufacturerData.substring(40, 42);
        String z = z_2 + z_1;
        int z_dec = strtol(z.c_str(), NULL, HEX);
        float z_total = z_dec / 10.f;
        Serial.print(F("Z-axis : "));
        Serial.println(String(z_total));

        //Vibration information Calculator
        String VI = ManufacturerData.substring(16, 18);
        String VI_status = "";
        if (VI == "00") {
          "\"" + address_total + "\"";
          VI_status = "\"NONE\"";
          Serial.println(F("NONE"));
        } else if (VI == "01") {
          VI_status = "\"VIBRATION\"";
          Serial.println(F("VIBRATION"));
        } else if (VI == "02") {
          VI_status = "\"EARTHQUAKE\"";
          Serial.println(F("EARTHQUAKE"));
        }
        vTaskDelay(100);

        //Send data to Magellan 3
        if (addressArray_OMRON[0] == address_BLE) {
          Serial.println(F("prepare to send data_1"));
          String payload_1 = "{\"Address_1\":" + address_total_S + ",\"Dis_1\":" + String(DI_total) + ",\"Heat_1\":" + String(Heat_total) + ",\"SI_1\":" + String(SI_total) + ",\"PGA_1\":" + String(PGA_total) + ",\"Seis_1\":" + String(Seis_total) + ",\"X_1\":" + String(x_total) + ",\"Y_1\":" + String(y_total) + ",\"Z_1\":" + String(z_total) + ",\"VI_1\":" + VI_status + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          vTaskDelay(100);
          magel.report(payload_1, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[1] == address_BLE) {
          Serial.println(F("prepare to send data_2"));
          String payload_2 = "{\"Address_2\":" + address_total_S + ",\"Dis_2\":" + String(DI_total) + ",\"Heat_2\":" + String(Heat_total) + ",\"SI_2\":" + String(SI_total) + ",\"PGA_2\":" + String(PGA_total) + ",\"Seis_2\":" + String(Seis_total) + ",\"X_2\":" + String(x_total) + ",\"Y_2\":" + String(y_total) + ",\"Z_2\":" + String(z_total) + ",\"VI_1\":" + VI_status + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          vTaskDelay(100);
          magel.report(payload_2, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[2] == address_BLE) {
          Serial.println(F("prepare to send data_3"));
          String payload_3 = "{\"Address_3\":" + address_total_S + ",\"Dis_3\":" + String(DI_total) + ",\"Heat_3\":" + String(Heat_total) + ",\"SI_3\":" + String(SI_total) + ",\"PGA_3\":" + String(PGA_total) + ",\"Seis_3\":" + String(Seis_total) + ",\"X_3\":" + String(x_total) + ",\"Y_3\":" + String(y_total) + ",\"Z_3\":" + String(z_total) + ",\"VI_3\":" + VI_status + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          vTaskDelay(100);
          magel.report(payload_3, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[3] == address_BLE) {
          Serial.println(F("prepare to send data_4"));
          String payload_4 = "{\"Address_4\":" + address_total_S + ",\"Dis_4\":" + String(DI_total) + ",\"Heat_4\":" + String(Heat_total) + ",\"SI_4\":" + String(SI_total) + ",\"PGA_4\":" + String(PGA_total) + ",\"Seis_4\":" + String(Seis_total) + ",\"X_4\":" + String(x_total) + ",\"Y_4\":" + String(y_total) + ",\"Z_4\":" + String(z_total) + ",\"VI_4\":" + VI_status + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          magel.report(payload_4, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[4] == address_BLE) {
          Serial.println(F("prepare to send data_5"));
          String payload_5 = "{\"Address_5\":" + address_total_S + ",\"Dis_5\":" + String(DI_total) + ",\"Heat_5\":" + String(Heat_total) + ",\"SI_5\":" + String(SI_total) + ",\"PGA_5\":" + String(PGA_total) + ",\"Seis_5\":" + String(Seis_total) + ",\"X_5\":" + String(x_total) + ",\"Y_5\":" + String(y_total) + ",\"Z_5\":" + String(z_total) + ",\"VI_5\":" + VI_status + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          magel.report(payload_5, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[5] == address_BLE) {
          Serial.println(F("prepare to send data_6"));
          String payload_6 = "{\"Address_6\":" + address_total_S + ",\"Dis_6\":" + String(DI_total) + ",\"Heat_6\":" + String(Heat_total) + ",\"SI_6\":" + String(SI_total) + ",\"PGA_6\":" + String(PGA_total) + ",\"Seis_6\":" + String(Seis_total) + ",\"X_6\":" + String(x_total) + ",\"Y_6\":" + String(y_total) + ",\"Z_6\":" + String(z_total) + ",\"VI_6\":" + VI_status + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          magel.report(payload_6, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[6] == address_BLE) {
          Serial.println(F("prepare to send data_7"));
          String payload_7 = "{\"Address_7\":" + address_total_S + ",\"Dis_7\":" + String(DI_total) + ",\"Heat_7\":" + String(Heat_total) + ",\"SI_7\":" + String(SI_total) + ",\"PGA_7\":" + String(PGA_total) + ",\"Seis_7\":" + String(Seis_total) + ",\"X_7\":" + String(x_total) + ",\"Y_7\":" + String(y_total) + ",\"Z_7\":" + String(z_total) + ",\"VI_7\":" + VI_status + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          magel.report(payload_7, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[7] == address_BLE) {
          Serial.println(F("prepare to send data_8"));
          String payload_8 = "{\"Address_8\":" + address_total_S + ",\"Dis_8\":" + String(DI_total) + ",\"Heat_8\":" + String(Heat_total) + ",\"SI_8\":" + String(SI_total) + ",\"PGA_8\":" + String(PGA_total) + ",\"Seis_8\":" + String(Seis_total) + ",\"X_8\":" + String(x_total) + ",\"Y_8\":" + String(y_total) + ",\"Z_8\":" + String(z_total) + ",\"VI_8\":" + VI_status + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          magel.report(payload_8, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[8] == address_BLE) {
          Serial.println(F("prepare to send data_9"));
          String payload_9 = "{\"Address_9\":" + address_total_S + ",\"Dis_9\":" + String(DI_total) + ",\"Heat_9\":" + String(Heat_total) + ",\"SI_9\":" + String(SI_total) + ",\"PGA_9\":" + String(PGA_total) + ",\"Seis_9\":" + String(Seis_total) + ",\"X_9\":" + String(x_total) + ",\"Y_9\":" + String(y_total) + ",\"Z_9\":" + String(z_total) + ",\"VI_9\":" + VI_status + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          magel.report(payload_9, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[9] == address_BLE) {
          Serial.println(F("prepare to send data_10"));
          String payload_10 = "{\"Address_10\":" + address_total_S + ",\"Dis_10\":" + String(DI_total) + ",\"Heat_10\":" + String(Heat_total) + ",\"SI_10\":" + String(SI_total) + ",\"PGA_10\":" + String(PGA_total) + ",\"Seis_10\":" + String(Seis_total) + ",\"X_10\":" + String(x_total) + ",\"Y_10\":" + String(y_total) + ",\"Z_10\":" + String(z_total) + ",\"VI_10\":" + VI_status + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          magel.report(payload_10, token);
          vTaskDelay(100);
        }
        vTaskDelay(100);
        switch_passive_to_active_ble = true;
      }
    }

    //Mode 3 : Scan response Data
    else if (ManufacturerData.length() == 56 || ManufacturerData.length() == 58) {
      if (type == "03") {
        //Discomfort Index Calculator
        String DI_1 = ManufacturerData.substring(8, 10);
        String DI_2 = ManufacturerData.substring(10, 12);
        String DI = DI_2 + DI_1;
        int DI_dec = strtol(DI.c_str(), NULL, HEX);
        float DI_total = DI_dec / 100.f;
        Serial.print(F("Discomfort Index : "));
        Serial.println(String(DI_total));

        //Heat Stroke Calculator
        String Heat_1 = ManufacturerData.substring(12, 14);
        String Heat_2 = ManufacturerData.substring(14, 16);
        String Heat = Heat_2 + Heat_1;
        int Heat_dec = strtol(Heat.c_str(), NULL, HEX);
        float Heat_total = Heat_dec / 100.f;
        Serial.print(F("Heat Stroke : "));
        Serial.println(String(Heat_total));

        //SI value Calculator
        String SI_1 = ManufacturerData.substring(18, 20);
        String SI_2 = ManufacturerData.substring(20, 22);
        String SI = SI_2 + SI_1;
        int SI_dec = strtol(SI.c_str(), NULL, HEX);
        float SI_total = SI_dec / 10.f;
        Serial.print(F("SI : "));
        Serial.println(String(SI_total));

        //PGA Calculator
        String PGA_1 = ManufacturerData.substring(22, 24);
        String PGA_2 = ManufacturerData.substring(24, 26);
        String PGA = PGA_2 + PGA_1;
        int PGA_dec = strtol(PGA.c_str(), NULL, HEX);
        float PGA_total = PGA_dec / 10.f;
        Serial.print(F("PGA : "));
        Serial.println(String(PGA_total));

        //Seismic intensity Calculator
        String Seis_1 = ManufacturerData.substring(26, 28);
        String Seis_2 = ManufacturerData.substring(28, 30);
        String Seis = Seis_2 + Seis_1;
        int Seis_dec = strtol(Seis.c_str(), NULL, HEX);
        float Seis_total = Seis_dec / 1000.f;
        Serial.print(F("Seismic Intensity : "));
        Serial.println(String(Seis_total));

        //Acceleration (X-axis) Calculator
        String x_1 = ManufacturerData.substring(30, 32);
        String x_2 = ManufacturerData.substring(32, 34);
        String x = x_2 + x_1;
        int x_dec = strtol(x.c_str(), NULL, HEX);
        float x_total = x_dec / 10.f;
        Serial.print(F("X-axis : "));
        Serial.println(String(x_total));

        //Acceleration (Y-axis) Calculator
        String y_1 = ManufacturerData.substring(34, 36);
        String y_2 = ManufacturerData.substring(36, 38);
        String y = y_2 + y_1;
        int y_dec = strtol(y.c_str(), NULL, HEX);
        float y_total = y_dec / 10.f;
        Serial.print(F("Y-axis : "));
        Serial.println(String(y_total));

        //Acceleration (Z-axis) Calculator
        String z_1 = ManufacturerData.substring(38, 40);
        String z_2 = ManufacturerData.substring(40, 42);
        String z = z_2 + z_1;
        int z_dec = strtol(z.c_str(), NULL, HEX);
        float z_total = z_dec / 10.f;
        Serial.print(F("Z-axis : "));
        Serial.println(String(z_total));

        //Vibration information Calculator
        String VI = ManufacturerData.substring(16, 18);
        String VI_status = "";
        if (VI == "00") {
          "\"" + address_total + "\"";
          VI_status = "\"NONE\"";
          Serial.println(F("NONE"));
        } else if (VI == "01") {
          VI_status = "\"VIBRATION\"";
          Serial.println(F("VIBRATION"));
        } else if (VI == "02") {
          VI_status = "\"EARTHQUAKE\"";
          Serial.println(F("EARTHQUAKE"));
        }
        vTaskDelay(100);
        
        //Send data to Magellan 3
        if (addressArray_OMRON[0] == address_BLE) {
          Serial.println(F("prepare to send data_1"));
          vTaskDelay(100);
          String payload_1 = "{\"Address_1\":" + address_total_S + ",\"Dis_1\":" + String(DI_total) + ",\"Heat_1\":" + String(Heat_total) + ",\"SI_1\":" + String(SI_total) + ",\"PGA_1\":" + String(PGA_total) + ",\"Seis_1\":" + String(Seis_total) + ",\"X_1\":" + String(x_total) + ",\"Y_1\":" + String(y_total) + ",\"Z_1\":" + String(z_total) + ",\"VI_1\":" + VI_status+ "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          vTaskDelay(100);
          magel.report(payload_1, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[1] == address_BLE) {
          Serial.println(F("prepare to send data_2"));
          vTaskDelay(100);
          String payload_2 = "{\"Address_2\":" + address_total_S + ",\"Dis_2\":" + String(DI_total) + ",\"Heat_2\":" + String(Heat_total)+ ",\"SI_2\":" + String(SI_total) + ",\"PGA_2\":" + String(PGA_total) + ",\"Seis_2\":" + String(Seis_total) + ",\"X_2\":" + String(x_total) + ",\"Y_2\":" + String(y_total) + ",\"Z_2\":" + String(z_total) + ",\"VI_1\":" + VI_status  + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          vTaskDelay(100);
          magel.report(payload_2, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[2] == address_BLE) {
          Serial.println(F("prepare to send data_3"));
          vTaskDelay(100);
          String payload_3 = "{\"Address_3\":" + address_total_S + ",\"Dis_3\":" + String(DI_total) + ",\"Heat_3\":" + String(Heat_total)+ ",\"SI_3\":" + String(SI_total) + ",\"PGA_3\":" + String(PGA_total)  + ",\"Seis_3\":" + String(Seis_total) + ",\"X_3\":" + String(x_total) + ",\"Y_3\":" + String(y_total) + ",\"Z_3\":" + String(z_total) + ",\"VI_3\":" + VI_status+ "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          vTaskDelay(100);
          magel.report(payload_3, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[3] == address_BLE) {
          Serial.println(F("prepare to send data_4"));
          String payload_4 = "{\"Address_4\":" + address_total_S + ",\"Dis_4\":" + String(DI_total) + ",\"Heat_4\":" + String(Heat_total) + ",\"SI_4\":" + String(SI_total) + ",\"PGA_4\":" + String(PGA_total) + ",\"Seis_4\":" + String(Seis_total) + ",\"X_4\":" + String(x_total) + ",\"Y_4\":" + String(y_total) + ",\"Z_4\":" + String(z_total) + ",\"VI_4\":" + VI_status + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          magel.report(payload_4, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[4] == address_BLE) {
          Serial.println(F("prepare to send data_5"));
          String payload_5 = "{\"Address_5\":" + address_total_S + ",\"Dis_5\":" + String(DI_total) + ",\"Heat_5\":" + String(Heat_total) + ",\"SI_5\":" + String(SI_total) + ",\"PGA_5\":" + String(PGA_total) + ",\"Seis_5\":" + String(Seis_total) + ",\"X_5\":" + String(x_total) + ",\"Y_5\":" + String(y_total) + ",\"Z_5\":" + String(z_total) + ",\"VI_5\":" + VI_status + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          magel.report(payload_5, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[5] == address_BLE) {
          Serial.println(F("prepare to send data_6"));
          String payload_6 = "{\"Address_6\":" + address_total_S + ",\"Dis_6\":" + String(DI_total) + ",\"Heat_6\":" + String(Heat_total) + ",\"SI_6\":" + String(SI_total) + ",\"PGA_6\":" + String(PGA_total) + ",\"Seis_6\":" + String(Seis_total) + ",\"X_6\":" + String(x_total) + ",\"Y_6\":" + String(y_total) + ",\"Z_6\":" + String(z_total) + ",\"VI_6\":" + VI_status + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          magel.report(payload_6, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[6] == address_BLE) {
          Serial.println(F("prepare to send data_7"));
          String payload_7 = "{\"Address_7\":" + address_total_S + ",\"Dis_7\":" + String(DI_total) + ",\"Heat_7\":" + String(Heat_total) + ",\"SI_7\":" + String(SI_total) + ",\"PGA_7\":" + String(PGA_total) + ",\"Seis_7\":" + String(Seis_total) + ",\"X_7\":" + String(x_total) + ",\"Y_7\":" + String(y_total) + ",\"Z_7\":" + String(z_total) + ",\"VI_7\":" + VI_status + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          magel.report(payload_7, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[7] == address_BLE) {
          Serial.println(F("prepare to send data_8"));
          String payload_8 = "{\"Address_8\":" + address_total_S + ",\"Dis_8\":" + String(DI_total) + ",\"Heat_8\":" + String(Heat_total) + ",\"SI_8\":" + String(SI_total) + ",\"PGA_8\":" + String(PGA_total) + ",\"Seis_8\":" + String(Seis_total) + ",\"X_8\":" + String(x_total) + ",\"Y_8\":" + String(y_total) + ",\"Z_8\":" + String(z_total) + ",\"VI_8\":" + VI_status + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          magel.report(payload_8, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[8] == address_BLE) {
          Serial.println(F("prepare to send data_9"));
          String payload_9 = "{\"Address_9\":" + address_total_S + ",\"Dis_9\":" + String(DI_total) + ",\"Heat_9\":" + String(Heat_total) + ",\"SI_9\":" + String(SI_total) + ",\"PGA_9\":" + String(PGA_total) + ",\"Seis_9\":" + String(Seis_total) + ",\"X_9\":" + String(x_total) + ",\"Y_9\":" + String(y_total) + ",\"Z_9\":" + String(z_total) + ",\"VI_9\":" + VI_status + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          magel.report(payload_9, token);
          vTaskDelay(100);
        }
        if (addressArray_OMRON[9] == address_BLE) {
          Serial.println(F("prepare to send data_10"));
          String payload_10 = "{\"Address_10\":" + address_total_S + ",\"Dis_10\":" + String(DI_total) + ",\"Heat_10\":" + String(Heat_total) + ",\"SI_10\":" + String(SI_total) + ",\"PGA_10\":" + String(PGA_total) + ",\"Seis_10\":" + String(Seis_total) + ",\"X_10\":" + String(x_total) + ",\"Y_10\":" + String(y_total) + ",\"Z_10\":" + String(z_total) + ",\"VI_10\":" + VI_status + "}";
          ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
          ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
          magel.report(payload_10, token);
          vTaskDelay(100);
        }
        vTaskDelay(10);
        switch_active_to_passive_ble = true;
      }
    }

    // *** BAG / EP ***
    else if (ManufacturerData.length() == 44) {
      Serial.println(F(" - BAG or EP TYPE"));

      //Temp Calculator
      String temp_1 = ManufacturerData.substring(6, 8);
      String temp_2 = ManufacturerData.substring(8, 10);
      String temp = temp_2 + temp_1;
      int temp_dec = strtol(temp.c_str(), NULL, HEX);
      float temp_total = temp_dec / 100.f;
      Serial.print(F("Temp : "));
      Serial.println(String(temp_total));

      //Hum Calculator
      String hum_1 = ManufacturerData.substring(10, 12);
      String hum_2 = ManufacturerData.substring(12, 14);
      String hum = hum_2 + hum_1;
      int hum_dec = strtol(hum.c_str(), NULL, HEX);
      float hum_total = hum_dec / 100.f;
      Serial.print(F("Hum : "));
      Serial.println(String(hum_total));

      //Light Calculator
      String light_1 = ManufacturerData.substring(14, 16);
      String light_2 = ManufacturerData.substring(16, 18);
      String light = light_2 + light_1;
      int light_total = strtol(light.c_str(), NULL, HEX);
      Serial.print(F("Light : "));
      Serial.println(String(light_total));

      //UV Calculator
      String uv_1 = ManufacturerData.substring(18, 20);
      String uv_2 = ManufacturerData.substring(20, 22);
      String uv = uv_2 + uv_1;
      int uv_dec = strtol(uv.c_str(), NULL, HEX);
      float uv_total = uv_dec / 100.f;
      Serial.print(F("UV : "));
      Serial.println(String(uv_total));

      //Pressure Calculator
      String pres_1 = ManufacturerData.substring(22, 24);
      String pres_2 = ManufacturerData.substring(24, 26);
      String pres = pres_2 + pres_1;
      int pres_dec = strtol(pres.c_str(), NULL, HEX);
      float pres_total = pres_dec / 10.f;
      Serial.print(F("Pressure : "));
      Serial.println(String(pres_total));

      //Sound Noise Calculator
      String noise_1 = ManufacturerData.substring(26, 28);
      String noise_2 = ManufacturerData.substring(28, 30);
      String noise = noise_2 + noise_1;
      int noise_dec = strtol(noise.c_str(), NULL, HEX);
      float noise_total = noise_dec / 100.f;
      Serial.print(F("Sound Noise : "));
      Serial.println(String(noise_total));

      //Discomfort Calculator
      String com_1 = ManufacturerData.substring(30, 32);
      String com_2 = ManufacturerData.substring(32, 34);
      String com = com_2 + com_1;
      int com_dec = strtol(com.c_str(), NULL, HEX);
      float com_total = com_dec / 100.f;
      Serial.print(F("Discomfort : "));
      Serial.println(String(com_total));

      //Heat stroke Calculator
      String heat_1 = ManufacturerData.substring(34, 36);
      String heat_2 = ManufacturerData.substring(36, 38);
      String heat = heat_2 + heat_1;
      int heat_dec = strtol(heat.c_str(), NULL, HEX);
      float heat_total = heat_dec / 100.f;
      Serial.print(F("Heat stroke : "));
      Serial.println(String(heat_total));

      //RFU Calculator
      String rfu_1 = ManufacturerData.substring(38, 40);
      String rfu_2 = ManufacturerData.substring(40, 42);
      String rfu = rfu_2 + rfu_1;
      int rfu_dec = strtol(rfu.c_str(), NULL, HEX);
      float rfu_total = rfu_dec / 100.f;
      Serial.print(F("RFU : "));
      Serial.println(String(rfu_total));

      //Battery voltage Calculator
      String bat = ManufacturerData.substring(42, 44);
      int bat_total = strtol(bat.c_str(), NULL, HEX);
      Serial.print(F("Battery Voltage : "));
      Serial.println(String(bat_total));
      vTaskDelay(10);

      //Send data to Magellan
      if (addressArray_OMRON[0] == address_BLE) {
        Serial.println(F("prepare to send data_1"));
        String payload_1 = "{\"Address_1\":" + address_total_S + ",\"Temp_1\":" + String(temp_total) + ",\"Hum_1\":" + String(hum_total) + ",\"Light_1\":" + String(light_total) + ",\"Pressure_1\":" + String(pres_total) + ",\"Noise_1\":" + String(noise_total) + ",\"Discomfort_1\":" + String(com_total) + ",\"HeatStroke_1\":" + String(heat_total) + ",\"RFU_1\":" + String(rfu_total) + ",\"Battery_1\":" + String(bat_total) +  "}";
        ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
        ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
        magel.report(payload_1, token);
        vTaskDelay(100);
      }
      if (addressArray_OMRON[1] == address_BLE) {
        Serial.println(F("prepare to send data_2"));
        String payload_2 = "{\"Address_2\":" + address_total_S + ",\"Temp_2\":" + String(temp_total) + ",\"Hum_2\":" + String(hum_total) + ",\"Light_2\":" + String(light_total) + ",\"Pressure_2\":" + String(pres_total) + ",\"Noise_2\":" + String(noise_total) + ",\"Discomfort_2\":" + String(com_total) + ",\"HeatStroke_2\":" + String(heat_total) + ",\"RFU_2\":" + String(rfu_total) + ",\"Battery_2\":" + String(bat_total) +  "}";
        ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
        ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
        magel.report(payload_2, token);
        vTaskDelay(100);
      }
      if (addressArray_OMRON[2] == address_BLE) {
        Serial.println(F("prepare to send data_3"));
        String payload_3 = "{\"Address_3\":" + address_total_S + ",\"Temp_3\":" + String(temp_total) + ",\"Hum_3\":" + String(hum_total) + ",\"Light_3\":" + String(light_total) + ",\"Pressure_3\":" + String(pres_total) + ",\"Noise_3\":" + String(noise_total) + ",\"Discomfort_3\":" + String(com_total) + ",\"HeatStroke_3\":" + String(heat_total) + ",\"RFU_3\":" + String(rfu_total) + ",\"Battery_3\":" + String(bat_total) +  "}";
        ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
        ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
        magel.report(payload_3, token);
        vTaskDelay(100);
      }
      if (addressArray_OMRON[3] == address_BLE) {
        Serial.println(F("prepare to send data_4"));
        String payload_4 = "{\"Address_4\":" + address_total_S + ",\"Temp_4\":" + String(temp_total) + ",\"Hum_4\":" + String(hum_total) + ",\"Light_4\":" + String(light_total) + ",\"Pressure_4\":" + String(pres_total) + ",\"Noise_4\":" + String(noise_total) + ",\"Discomfort_4\":" + String(com_total) + ",\"HeatStroke_4\":" + String(heat_total) + ",\"RFU_4\":" + String(rfu_total) + ",\"Battery_4\":" + String(bat_total) +  "}";
        ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
        ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
        magel.report(payload_4, token);
        vTaskDelay(100);
      }
      if (addressArray_OMRON[4] == address_BLE) {
        Serial.println(F("prepare to send data_5"));
        String payload_5 = "{\"Address_5\":" + address_total_S + ",\"Temp_5\":" + String(temp_total) + ",\"Hum_5\":" + String(hum_total) + ",\"Light_5\":" + String(light_total) + ",\"Pressure_5\":" + String(pres_total) + ",\"Noise_5\":" + String(noise_total) + ",\"Discomfort_5\":" + String(com_total) + ",\"HeatStroke_5\":" + String(heat_total) + ",\"RFU_5\":" + String(rfu_total) + ",\"Battery_5\":" + String(bat_total) +  "}";
        ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
        ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
        magel.report(payload_5, token);
        vTaskDelay(100);
      }
      if (addressArray_OMRON[5] == address_BLE) {
        Serial.println(F("prepare to send data_6"));
        String payload_6 = "{\"Address_6\":" + address_total_S + ",\"Temp_6\":" + String(temp_total) + ",\"Hum_6\":" + String(hum_total) + ",\"Light_6\":" + String(light_total) + ",\"Pressure_6\":" + String(pres_total) + ",\"Noise_6\":" + String(noise_total) + ",\"Discomfort_6\":" + String(com_total) + ",\"HeatStroke_6\":" + String(heat_total) + ",\"RFU_6\":" + String(rfu_total) + ",\"Battery_6\":" + String(bat_total) +  "}";
        ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
        ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
        magel.report(payload_6, token);
        vTaskDelay(100);
      }
      if (addressArray_OMRON[6] == address_BLE) {
        Serial.println(F("prepare to send data_7"));
        String payload_7 = "{\"Address_7\":" + address_total_S + ",\"Temp_7\":" + String(temp_total) + ",\"Hum_7\":" + String(hum_total) + ",\"Light_7\":" + String(light_total) + ",\"Pressure_7\":" + String(pres_total) + ",\"Noise_7\":" + String(noise_total) + ",\"Discomfort_7\":" + String(com_total) + ",\"HeatStroke_7\":" + String(heat_total) + ",\"RFU_7\":" + String(rfu_total) + ",\"Battery_7\":" + String(bat_total) +  "}";
        ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
        ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
        magel.report(payload_7, token);
        vTaskDelay(100);
      }
      if (addressArray_OMRON[7] == address_BLE) {
        Serial.println(F("prepare to send data_8"));
        String payload_8 = "{\"Address_8\":" + address_total_S + ",\"Temp_8\":" + String(temp_total) + ",\"Hum_8\":" + String(hum_total) + ",\"Light_8\":" + String(light_total) + ",\"Pressure_8\":" + String(pres_total) + ",\"Noise_8\":" + String(noise_total) + ",\"Discomfort_8\":" + String(com_total) + ",\"HeatStroke_8\":" + String(heat_total) + ",\"RFU_8\":" + String(rfu_total) + ",\"Battery_8\":" + String(bat_total) +  "}";
        ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
        ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
        magel.report(payload_8, token);
        vTaskDelay(100);
      }
      if (addressArray_OMRON[8] == address_BLE) {
        Serial.println(F("prepare to send data_9"));
        String payload_9 = "{\"Address_9\":" + address_total_S + ",\"Temp_9\":" + String(temp_total) + ",\"Hum_9\":" + String(hum_total) + ",\"Light_9\":" + String(light_total) + ",\"Pressure_9\":" + String(pres_total) + ",\"Noise_9\":" + String(noise_total) + ",\"Discomfort_9\":" + String(com_total) + ",\"HeatStroke_9\":" + String(heat_total) + ",\"RFU_9\":" + String(rfu_total) + ",\"Battery_9\":" + String(bat_total) +  "}";
        ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
        ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
        magel.report(payload_9, token);
        vTaskDelay(100);
      }
      if (addressArray_OMRON[9] == address_BLE) {
        Serial.println(F("prepare to send data_10"));
        String payload_10 = "{\"Address_10\":" + address_total_S + ",\"Temp_10\":" + String(temp_total) + ",\"Hum_10\":" + String(hum_total) + ",\"Light_10\":" + String(light_total) + ",\"Pressure_10\":" + String(pres_total) + ",\"Noise_10\":" + String(noise_total) + ",\"Discomfort_10\":" + String(com_total) + ",\"HeatStroke_10\":" + String(heat_total) + ",\"RFU_10\":" + String(rfu_total) + ",\"Battery_10\":" + String(bat_total) +  "}";
        ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
        ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
        magel.report(payload_10, token);
        vTaskDelay(100);
      }
      vTaskDelay(100);
      switch_passive_to_active_ble = true;
    }
    calculator_ready = false;
    vTaskDelay(100);
    doScan = true;
  }

  if (doScan == true) {
    vTaskDelay(100);
    Serial.println(F("------------- Rescan ---------------"));
    if (switch_passive_to_active_ble == true && doScan == true) {
      Serial.println(F("Mode 3 : Switch Passive to Active BLE"));
      ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
      ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
      BLEDevice::getScan()->setActiveScan(true);
      BLEDevice::getScan()->start(0);
      switch_active_to_passive_ble = false;
      switch_passive_to_active_ble = false;
      doScan = false;
    } else if (switch_active_to_passive_ble == true && doScan == true) {
      Serial.println(F("Mode 3 : Switch Active to Passive BLE"));
      ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
      ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
      BLEDevice::getScan()->setActiveScan(false);
      BLEDevice::getScan()->start(0);
      switch_active_to_passive_ble = false;
      switch_passive_to_active_ble = false;
      doScan = false;
    } else {
      vTaskDelay(100);
      Serial.println(F("Scan Active scan"));
      ESP_LOGI(LOG_TAG, "free heap ammount: %d", esp_get_free_heap_size());
      ESP_LOGI(LOG_TAG, "free heap minimum ammount: %d", esp_get_minimum_free_heap_size());
      BLEDevice::getScan()->setActiveScan(true);
      BLEDevice::getScan()->start(0);
      switch_active_to_passive_ble = false;
      switch_passive_to_active_ble = false;
      doScan = false;
    }
  }
  vTaskDelay(500);
}
