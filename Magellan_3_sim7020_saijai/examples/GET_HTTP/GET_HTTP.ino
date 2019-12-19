/*
	Recommend to use this example with more than 2048 byte of dynamic memory 
	e.g. arduino Mega , Nucleo-64	

	*Noted this example is require install the ArduinoJson Library
		   and please consider to your API data
		   has exceed the dynamic memory of your MCU. 
*/

#include "Magellan.h"
#include <ArduinoJson.h>                            // Please install ArduinoJson Library to use this example

Magellan magel;
char auth[]="your-key";                             //Token Key you can get from magellan platform


String payload;
String data="0";

unsigned int previous_time=0;

void get_timezone(){
  DynamicJsonBuffer jsonBuffer;

  data=magel.get_api("API","http://api.timezonedb.com/v2/get-time-zone?key=67W74SNF6T2S&format=json&by=zone&zone=Asia/Bangkok");
  Serial.print("data is ");
  Serial.println(data);
  
  char json[data.length()+1]="";
  data.toCharArray(json,data.length()+1); 
  JsonObject& root = jsonBuffer.parseObject(json);
  
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }
  
  const char* status_api = root["status"];
  const char* abbreviation = root["abbreviation"];
  const char* countryCode = root["countryCode"];
  const char* nextAbbreviation = root["nextAbbreviation"];
  const char* timestamp = root["timestamp"];
  const char* dst = root["dst"];
  const char* dstStart = root["dstStart"];
  const char* formatted = root["formatted"];
  const char* countryName = root["countryName"];
  const char* message = root["message"];
  const char* gmtOffset = root["gmtOffset"];
  const char* dstEnd = root["dstEnd"];
  const char* zoneName = root["zoneName"];

  // Print values.
  Serial.println(status_api);
  Serial.println(abbreviation);
  Serial.println(countryCode);
  Serial.println(nextAbbreviation);
  Serial.println(timestamp);
  Serial.println(dst);
  Serial.println(dstStart);
  Serial.println(formatted);
  Serial.println(countryName);
  Serial.println(message);
  Serial.println(gmtOffset);
  Serial.println(dstEnd);
  Serial.println(zoneName);
  
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  magel.begin(auth);                                //initial Magellan LIB
  magel.debug=false;
  magel.printstate=false;
  // Initial create data on Magellan post some data
  payload="{\"Temperature\":0,\"Humidity\":0,\"sw\":0}";       //please provide payload with json format
  magel.post(payload);                                         //post data payload to Magellan IoT platform
 
  //Try to get time zone bangkok with HTTP API 
  get_timezone();
}

void loop() {
  // put your main code here, to run repeatedly:
  //Try to get sw data
  Serial.println(magel.get("sw"));


  
}