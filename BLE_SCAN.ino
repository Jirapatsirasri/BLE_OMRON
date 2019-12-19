//add library BLE
#include "BLEDevice.h"
#include "BLEScan.h"
#include "BLEAdvertisedDevice.h"

//add address to EEPROM
#include <EEPROM.h>

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

  //Part Calculator
  if (calculator_ready == true) {
    //calculator address ble
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
    vTaskDelay(10);
    String type = ManufacturerData.substring(4, 6);
    // Type sensor omron
    //----- Advertising -----
    //*** USB ***
    if (ManufacturerData.length() == 42) {
      Serial.println(F(" - USB TYPE"));

      //Calculate Mode : Sensor data
      //Mode Sensor Data (----- Mode 1 -----)
      if (type == "01") {
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
        vTaskDelay(10);
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
        vTaskDelay(10);
      }
      if (type = "03") {
        //Mode 3(Advertising) : Calculation mode(Sensor data)
        Serial.println(F("Advertising data"));
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
    }
    calculator_ready = false;
    vTaskDelay(100);
    doScan = true;
  }

  if (doScan == true) {
    Serial.println(F("------------- Rescan ---------------"));
    if (switch_passive_to_active_ble == true && doScan == true) {
      Serial.println(F("Mode 3 : Switch Passive to Active BLE"));
      vTaskDelay(100);
      BLEDevice::getScan()->setActiveScan(true);
      BLEDevice::getScan()->start(0);
      switch_active_to_passive_ble = false;
      switch_passive_to_active_ble = false;
      doScan = false;
    } else if (switch_active_to_passive_ble == true && doScan == true) {
      Serial.println(F("Mode 3 : Switch Active to Passive BLE"));
      vTaskDelay(100);
      BLEDevice::getScan()->setActiveScan(false);
      BLEDevice::getScan()->start(0);
      switch_active_to_passive_ble = false;
      switch_passive_to_active_ble = false;
      doScan = false;
    } else {
      vTaskDelay(100);
      Serial.println(F("Scan Active scan"));
      BLEDevice::getScan()->setActiveScan(true);
      BLEDevice::getScan()->start(0);
      switch_active_to_passive_ble = false;
      switch_passive_to_active_ble = false;
      doScan = false;
    }
  }
  vTaskDelay(500);
}


