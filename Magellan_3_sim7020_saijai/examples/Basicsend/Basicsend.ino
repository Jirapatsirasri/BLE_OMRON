#include "AltSoftSerial.h"
#include <SoftwareSerial.h>

//char serverIP[] = "http://iot.dld-test.com";

unsigned int Msg_ID=0;

long interval=10000;
unsigned long previousMillis = 0;


AltSoftSerial NBserial;

char auth[]=""; //Place the WRITE KEY

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  NBserial.begin(9600);
  NBbegin();

  previousMillis = millis();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();
  check_rsp();

  String Temp=String(random(0,100));
  String Humid=String(random(0,100));
  String pressure=String(random(0,100));
  
  String payload="{\"Temp\":"+Temp+
                 ",\"Humid\":"+Humid+
                 ",\"pressure\":"+pressure+"}";
   
  if(currentMillis - previousMillis >= interval)
  {
    Serial.println("send data");
    send_data(payload);

    previousMillis = currentMillis;     //Memory last time to send
  }
}
void NBbegin()
{
  Serial.println(F(">>############### Welcome NB-IoT Basic network connect ########################"));
  Serial.println(F(">>Reboot Module"));
  NBserial.println(F("AT+NRB"));
  delay(6000);
  //Serial.println(F(">>Setup CFUN"));
  NBserial.println(F("AT+CFUN=1"));
  delay(5000);
  Serial.println(F(">>Setup Network"));
  NBserial.println(F("AT+CGDCONT=1,\"IP\",\"IOTURL\""));
  delay(5000);
  Serial.println(F(">>connect network"));
  NBserial.println(F("AT+CGATT=1"));
  delay(5000);
  Serial.println(F(">>create socket"));
  NBserial.println(F("AT+NSOCR=DGRAM,17,5683,1"));
  delay(5000);
  NBserial.println(F("AT+NCONFIG=AUTOCONNECT,TRUE"));
  delay(5000);
  Serial.println(F(">>Check signal"));
  NBserial.println(F("AT+CSQ=?"));
  delay(5000);
  Serial.println(F(">>Ping test"));
  NBserial.println(F("AT+NPING=8.8.8.8"));
  //while(NBserial.available())
  //{
  //    Serial.write(NBserial.read());
  //}
}
void check_rsp()
{
  String input;
  if (NBserial.available())
  {
    //Serial.write(NBserial.read());
    input = NBserial.readStringUntil('\n');
    Serial.println(input);
    if(input.indexOf(F("+NPING"))!=-1)
    {
      int index1 = input.indexOf(F(","));
      int index2 = input.indexOf(F(","));
      index1 = input.indexOf(F(","),index2+1);
      index2 = input.indexOf(F(","),index1+1);
      Serial.print(F(">>Maximum time in ms to wait for echo respoonse : "));
      Serial.println(input.substring(index1+1,index2-1));
    }
    else if(input.indexOf(F("+NPINGERR"))!=-1)
    {
      Serial.println(F(">>ping ERROR please reset and check NB-IoT network")); 
    }
    else if(input.indexOf(F("+NSONMI"))!=-1)
    {
      NBserial.println(F("AT+NSORF=0,100"));
    }
  }
  
  
}

void printHEX(char *str)
{
  char *hstr;
  hstr=str;
  char out[3]="";
  int i=0;
  bool flag=false;
  while(*hstr)
  {
    flag=itoa((int)*hstr,out,16);
    
    if(flag)
    {
      NBserial.print(out);
      Serial.print(out);
    }
    hstr++;
  }
}

void printmsgID(unsigned int messageID)
{
  char Msg_ID[3];
  
  utoa(highByte(messageID),Msg_ID,16);
  if(highByte(messageID)<16)
  {
    Serial.print(F("0"));
    NBserial.print(F("0"));
    Serial.print(Msg_ID);
    NBserial.print(Msg_ID);
  }
  else
  {
    NBserial.print(Msg_ID);
    Serial.print(Msg_ID);
  }

  utoa(lowByte(messageID),Msg_ID,16);
  if(lowByte(messageID)<16)
  {
    Serial.print(F("0"));
    NBserial.print(F("0"));
    Serial.print(Msg_ID);
    NBserial.print(Msg_ID);
  }
  else
  {
    NBserial.print(Msg_ID);
    Serial.print(Msg_ID);
  }
}

void send_data(String payload)
{
  Msg_ID++;
  char data[payload.length()+1]="";
  payload.toCharArray(data,payload.length()+1);
  
  //NBserial.print(F("AT+NSOST=0,52.74.30.1,5683,"));
  NBserial.print(F("AT+NSOST=0,103.20.205.85,5683,"));
  NBserial.print(49+payload.length());
  Serial.print(F("4002"));
  NBserial.print(F(",4002"));
  printmsgID(Msg_ID);
  NBserial.print(F("b54e42496f54"));
  NBserial.print(F("0d17"));
  printHEX(auth);
  NBserial.print(F("ff"));  //49
  printHEX(data);  
  NBserial.println();
}
