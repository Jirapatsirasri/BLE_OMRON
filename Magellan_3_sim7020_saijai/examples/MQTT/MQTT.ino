#include "MagMQTT.h"
MagMQTT magel;  

char auth[]="your-key";     //Token Key you can get from magellan platform
//unsigned char server[]="128.199.189.72";
//unsigned char port[]="19000";
//unsigned char username[]="";
//unsigned char password[]="";
//unsigned char clientID[]="things1";

//unsigned char server[]="13.67.66.157";
//unsigned char port[]="1883";
//unsigned char username[]="thing-bd7aeb60-1ad0-11e9-ba49-b9ff8ec4dc6b";
//unsigned char password[]="bd7aeb60-1ad0-11e9-ba49-b9ff8ec4dc6b";
//unsigned char clientID[]="things1";

unsigned char server[]="m11.cloudmqtt.com";
unsigned char port[]="19000";
unsigned char username[]="dezecjxi";
unsigned char password[]="aMJMNwogxlRY";
unsigned char clientID[]="Thing3";

void callback(String &topic,String &payload)
{
  Serial.println("msg "+topic+":"+payload);
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  magel.server=server;
  magel.port=port;
  magel.username=username;
  magel.password=password;
  magel.clientID=clientID;
  
  magel.begin(auth);           //init Magellan LIB
  
  magel.RegisCallback(callback);
  
//  String topic="monitor/bd7aeb60-1ad0-11e9-ba49-b9ff8ec4dc6b";
//  String payload="{\"data\":1}";
  String topic="data";
  String payload="1";
  magel.publish(topic,payload);

  magel.subscribe(topic);
}

void loop() {
  // put your main code here, to run repeatedly:
 magel.Loop();
}