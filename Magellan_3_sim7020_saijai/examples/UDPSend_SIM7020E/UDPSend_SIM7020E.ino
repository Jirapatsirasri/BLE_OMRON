#include "AT_SIM7020.h"
String address = "";
String port = "";
String payload = "HelloWorld";
String data_return;
unsigned long previousMillis = 0;
const long interval = 6000;  //millisecond
long cnt=0;
AT_SIM7020 AT;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  AT.debug=true;
  AT.setupModule(address,port);
  AT.getIMEI();
  AT.getIMSI();
  AT.getSignal();
  AT.getDeviceIP();
  AT.pingIP(address);
  AT.getAPN();
  previousMillis = millis();

}

void loop() {
  data_return="";
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
    {
      cnt++;     
           
      // Send data in String 
      AT.sendMsgSTR(address,port,payload+String(cnt));
      
      //Send data in HexString     
      //AT.sendMsgHEX(address,port,"3132");
      previousMillis = currentMillis;
  
    }
  AT.waitResponse(data_return,address);
  if(data_return!="")Serial.println("receive : "+data_return);
}
