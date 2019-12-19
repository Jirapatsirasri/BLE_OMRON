#include "Magellan.h"
Magellan magel;
char auth[]="your-key";                             //Token Key you can get from magellan platform

String payload;
String data="0";
String data1="0";
String data2="0";
String data3="0";
unsigned int previous_time=0;
String Temperature="0";
String Humidity="0";

void get_timezone(){

  data=magel.get_api("API","http://api.timezonedb.com/v2/get-time-zone?key=67W74SNF6T2S&format=json&by=zone&zone=Asia/Bangkok");
  Serial.print("data is ");
  Serial.println(data);
  
}

void setup() {
  Serial.begin(9600);
  magel.begin(auth);                                //initial Magellan LIB
  magel.printstate=true;
  // Initial create data on Magellan
  payload="{\"Temperature\":0,\"Humidity\":0,\"sw\":0}";       //please provide payload with json format
  magel.post(payload);                                         //post data payload to Magellan IoT platform

  previous_time=millis();
}

void loop() {
  
  data1=magel.get("Temperature");
  Serial.print("Temperature is ");
  Serial.println(data1);

  data2=magel.get("Humidity");
  Serial.print("Humidity is ");
  Serial.println(data2);
  
  String Temperature=String(random(0,100));
  String Humidity=String(random(0,100));

  data3=magel.get_dashboard("sw");
  Serial.print("sw is ");
  Serial.println(data3);

  payload="{\"Temperature\":"+Temperature+",\"Humidity\":"+Humidity+",\"sw\":"+data3+"}";       //please provide payload with json format
  magel.post(payload);                                         //post data payload to Magellan IoT platform
  
}