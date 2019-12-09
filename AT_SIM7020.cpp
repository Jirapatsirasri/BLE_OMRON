
//AT Command Dictionary for SIMCOM SIM7020 ver.1.0.4
#include "AT_SIM7020.h"  
#include "board.h"
//#define hwResetPin 26
//#define buadrate 9600

/****************************************/
/**          Initialization            **/
/****************************************/
AT_SIM7020::AT_SIM7020(){}

void AT_SIM7020::setupModule(String address,String port){
  previous_check=millis();
  //if(isHwReset) pinMode(hwResetPin, OUTPUT);  //pwrkey
	pinMode(hwResetPin, OUTPUT);
  reboot_module();
  if (serialConfig){
    serialPort.begin(buadrate,configParam,rxPin,txPin);
    _Serial = &serialPort;  
  }
  else{
    serialPort.begin(buadrate);
    _Serial = &serialPort;
  }

  if(debug) Serial.println(F("====BEGIN===="));  

  check_module_ready();
  if (!hw_connected){
    en_post=false;
    en_get=false;
  }

  echoOff();

  while(!enterPIN()){
    //Serial.println(F("Check your SIM PIN"));
  }

  Serial.print(F(">>Connecting "));

  /*_Serial->println(F("AT+CMEE=1"));
  while(1){
    data_input=_Serial->readStringUntil('\n');
    if(data_input.indexOf(F("OK"))!=-1) break;
  } */ 

  if(attachNetwork(address, port)){  
    //createUDPSocket(address,port);
    if(!createUDPSocket(address,port)){
      if(debug) Serial.println("Cannot create socket");
    }
  }

  Serial.println(F("OK"));
  previous=millis();
}

void AT_SIM7020::setupModule(){
  previous_check=millis();
  pinMode(hwResetPin, OUTPUT);
  reboot_module();

  if (serialConfig){
    serialPort.begin(buadrate,configParam,rxPin,txPin);
    _Serial = &serialPort;  
  }
  else{
    serialPort.begin(buadrate);
    _Serial = &serialPort;
  }

  if(debug) Serial.println(F("====BEGIN===="));  

  check_module_ready();
  if (!hw_connected){
    en_post=false;
    en_get=false;
  }

  echoOff();

  while(!enterPIN()){
    //Serial.println(F("Check your SIM PIN"));
  }

  Serial.print(F(">>Connecting "));

/*  _Serial->println(F("AT+CMEE=1"));
  while(1){
    data_input=_Serial->readStringUntil('\n');
    if(data_input.indexOf(F("OK"))!=-1) break;
  }*/

  //attachNetwork(address, port);

  Serial.println(F("OK"));
  previous=millis();
}
/****************************************/
/**          Public                    **/
/****************************************/

void AT_SIM7020::check_module_ready(){
  echoOff();
  _Serial->println(F("AT"));
  delay(100);
  while(1){
    if(_Serial->available()){
      data_input = _Serial->readStringUntil('\n');
      if(data_input.indexOf(F("OK"))!=-1){
        hw_connected=true;
        break;
      }
    }
    else{
      unsigned int current_check=millis();
      if (current_check-previous_check>5000){
        Serial.println(F("Error to connect NB Module try reset or check your hardware"));
        previous_check=current_check;
        hw_connected=false;
        //break;
      }
      else{
        _Serial->println(F("AT"));
        delay(100);
      }
    }
  }

  _serial_flush();
}

void AT_SIM7020::reboot_module(){
  //if(isHwReset){
    digitalWrite(hwResetPin, LOW);
    delay(800);
    digitalWrite(hwResetPin, HIGH);
    delay(1000);
  //}
  
}

/*void AT_SIM7020::get_cmdlist(){
  _Serial->println(F("AT+CLAC"));
  Serial.println(F("------(Command list)------"));
  delay(200);
  while(1){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
      if(debug) Serial.println("cmd list : "+data_input);
      if(data_input.indexOf(F("AT"))!=-1) Serial.println(data_input);
      else if(data_input.indexOf(F("OK"))!=-1) break;
      else if(data_input.indexOf(F("ERROR"))!=-1){
        Serial.println(F("Cannot use this command"));
        break;
      }
    }
    //delay(100);
  }  
  Serial.println(F("--------------------------"));
}*/

String AT_SIM7020::getSignal(){
  delay(200);
  _serial_flush();
  int rssi = 0;
  String data_csq = "";
  data_input = "";
  _Serial->println(F("AT+CSQ"));
  delay(500);
  while(1)  {    
    if(_Serial->available()){
      data_input = _Serial->readStringUntil('\n');
      if(data_input.indexOf(F("OK"))!=-1){
        break;
      }
      else{
        if(data_input.indexOf(F("+CSQ"))!=-1){
          int start_index = data_input.indexOf(F(":"));
          int stop_index  = data_input.indexOf(F(","));
          data_csq = data_input.substring(start_index+1,stop_index);
          if (data_csq == "99"){
            data_csq = "N/A";
          }
          else{
            rssi = data_csq.toInt();
            rssi = (2*rssi)-113;
            data_csq = String(rssi);
          }
        }
      }
    }
  }
  if(debug)Serial.print(F(">>Signal : "));
  if(debug)Serial.print(data_csq);
  if(debug)Serial.println(F(" dbm"));
  return data_csq;
}

String AT_SIM7020::getIMEI(){
  String imei;
  _Serial->println(F("AT+CGSN=1"));
  while(1){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
      //if(debug) Serial.println("IMEI: "+data_input);
      if(data_input.indexOf(F("+CGSN:"))!=-1){
        data_input.replace(F("+CGSN: "),"");
        imei = data_input;
      }
      else if(data_input.indexOf(F("OK"))!=-1 && imei!="") break;
    }
  }
  if(debug)Serial.print(F(">>IMEI : "));
  if(debug)Serial.println(imei);
  return imei;
}

String AT_SIM7020::getIMSI(){
  String imsi="";
  _Serial->println(F("AT+CIMI"));
  while(1){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
      //if(debug) Serial.println("data in IMSI : "+data_input);
      if(data_input.indexOf(F("OK"))!=-1 && imsi.indexOf(F("52003"))!=-1) break;
      else imsi+=data_input;
    }
  }
  imsi.replace(F("OK"),"");
  imsi.trim();
  if(debug)Serial.print(F(">>IMSI : "));
  if(debug)Serial.println(imsi); 
  return imsi;
}

String AT_SIM7020::getDeviceIP(){
  String deviceIP;
  _Serial->println(F("AT+CGPADDR=1"));
  while(1){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
      //if(debug)Serial.println("data : "+data_input);
      if(data_input.indexOf(F("+CGPADDR"))!=-1){
        int index = data_input.indexOf(F(":"));
        int index2 = data_input.indexOf(F(","));
        deviceIP = data_input.substring(index2+1,data_input.length());
      }
      else if(data_input.indexOf(F("OK"))!=-1) break;
    }
  }
  deviceIP.replace(F("\""),"");
  deviceIP.trim();
  if(debug)Serial.print(F(">>Device IP : "));
  if(debug)Serial.println(deviceIP);
  return deviceIP;
}

void AT_SIM7020::pingIP(String IP){
  pingRESP pingr;
  String data = "";
  _Serial->println("AT+CIPPING="+IP);

  while(1){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
      //if(debug)Serial.println("ping : "+data_input);
      if(data_input.indexOf(F("ERROR"))!=-1){
        break;
      }
      else if(data_input.indexOf(F("+CIPPING: 4"))!=-1){
        data=data_input;
        break;
      }
    }
  }

  if(data!=""){
    int index = data.indexOf(F(","));
    int index2 = data.indexOf(F(","),index+1);
    int index3 = data.indexOf(F(","),index2+1);
    pingr.status = true;
    pingr.addr = data.substring(index+1,index2);
    pingr.rtt = data.substring(index2+1,index3);
    pingr.ttl = data.substring(index3+1,data.length());
    //Serial.println("# Ping Success");
    Serial.println(">>Ping IP : "+pingr.addr + ", ttl= " + pingr.ttl + ", replyTime= " + pingr.rtt);

  }else { Serial.println(">>Ping Failed");}
  _serial_flush();
  data_input="";
}

void AT_SIM7020::sendMsgHEX(String address,String port,String payload){
  send_msg(address,port,payload.length(),payload);
}

void AT_SIM7020::sendMsgSTR(String address,String port,String payload){
  send_msg(address,port,0,"\""+payload+"\"");
}

void AT_SIM7020::printContent(String payload){
  _Serial->print(payload);
}

void AT_SIM7020::printHeader(String address,String port,unsigned int len){
  if(debug) Serial.println("Send to "+address+","+port);
  _Serial->print(F("AT+CSOSEND=0,"));
  _Serial->print(len);
  _Serial->print(F(","));
}

void AT_SIM7020::println(){
  _Serial->println();
}

void AT_SIM7020::printInt(int payload){
  _Serial->print(payload);
}

void AT_SIM7020::printChar(char *payload){
  _Serial->print(payload);
}

void AT_SIM7020::send_msg(String address,String port,unsigned int len,String payload){
  if (debug) Serial.println(F("##########################################"));
    if (debug) Serial.print(F("# Sending Data : "));
    if (debug) Serial.println(payload);
    if (debug) Serial.print(F("# IP : "));
    if (debug) Serial.print(address);
    if (debug) Serial.print(F(" PORT : "));
    if (debug) Serial.print(port);
    if (debug) Serial.println();

    printHeader(address,port,len);
    printContent(payload);
    println();
}

/*void AT_SIM7020::at_getBuffer(String socket,String nBuffer){
  _Serial->print(F("AT+NSORF="));
  _Serial->print(socket);
  _Serial->print(F(","));
  _Serial->println(nBuffer);
}*/

int AT_SIM7020::RegisCallback(reponseCallback callbackFunc){
     int r = -1;

     if (callback_p == NULL){
          callback_p = callbackFunc;
          r = 0;
     }
     //else - already in use! Fail!

     return r;
}

int AT_SIM7020::RegisMQCallback(MQTTClientCallback callbackFunc){
     int r = -1;

     if (MQcallback_p == NULL){
          MQcallback_p = callbackFunc;
          r = 0;
     }
     //else - already in use! Fail!

     return r;
}

void AT_SIM7020:: manageResponse(String &retdata,String server){ 
  if(end){  
    end=false;

    //String server="52.163.218.116";
    if(data_input.indexOf(F("+CSONMI:"))!=-1){  //serverIP
      String left_buffer="";

      //pack data to char array
      char buf[data_input.length()+1];
      memset(buf,'\0',data_input.length());
      data_input.toCharArray(buf, sizeof(buf));

      char *p = buf;
      char *str;
      int i=0;
      int j=0;
      while ((str = strtok_r(p, ",", &p)) != NULL){   // delimiter is the comma
        //Serial.println(str);
        j=2;  // number of comma

        if(i==j){
          retdata=str;
          if(debug) Serial.println("# Receive : "+retdata);
        }
        if(i==j+1){
          left_buffer=str;
        }
        i++;
      }              
        data_input=F("");
      }       
    
  }

}

String AT_SIM7020:: getAPN(){
  String out="";
  _Serial->println(F("AT+CGDCONT?"));

  while(1){
    if(_Serial->available()){
      data_input = _Serial->readStringUntil('\n');
      //if(debug) Serial.println("getAPN : "+data_input);
      if(data_input.indexOf(F("+CGDCONT: 1"))!=-1){
        int index=0;
        int index2=0;
        index = data_input.indexOf(F(":"));
        index2 = data_input.indexOf(F(","));

        index = data_input.indexOf(F(","),index2+1);
        index2 = data_input.indexOf(F(","),index+1);
        out = data_input.substring(index+2,index2-1);
        Serial.println(">>Get APN : " + out);
      }
      if(data_input.indexOf(F("OK"))!=-1){
        break;
      }
    }
  }
  _serial_flush();
  data_input="";
  return out;
}

void AT_SIM7020:: waitResponse(String &retdata,String server){ 

  if(_Serial->available()){
    char data=(char)_Serial->read();
    if(data=='\n' || data=='\r'){
      if(k>1){  //=====
        end=true;
        k=0;
      }
      k++;
    }
    else{
      data_input+=data;
    }
  }
  if (end){
    //Serial.println(">> "+data_input);
    manageResponse(retdata,server);   
  }
  
}

bool AT_SIM7020::checkNetworkConnection(){
  //if(debug) Serial.println("chkNetworkStart");
  bool status=false;
  _Serial->println(F("AT+CGATT?"));
  delay(800);
  for(int i=0;i<60;i++){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
      if(debug)Serial.println("cgatt? : "+data_input);
      if(data_input.indexOf(F("+CGATT: 1"))!=-1){
        status=true;
      }
      else if(data_input.indexOf(F("+CGATT: 0"))!=-1){
        status=false;
      }
      else if(data_input.indexOf(F("OK"))!=-1) {
        break;        
      }
      else if(data_input.indexOf(F("ERROR"))!=-1) {
        break;        
      }
      //Serial.println(i);
    }
    //_Serial->println(F("AT+CGATT?"));
  }
  data_input="";
  return status;
}

bool AT_SIM7020::attachNetwork(String address,String port){
  bool status=false;
  //if(debug) Serial.println("Start attach network");
  if(!checkNetworkConnection()){
    for(int i=0;i<60;i++){
      setPhoneFunction();
      connectNetwork();
      delay(2000);
      if(checkNetworkConnection()){ 
        status=true;
        break;
      }
      Serial.print(F("."));
    }
  }
  else status=true;
    
  _serial_flush();
  _Serial->flush();
  return status;
}

void AT_SIM7020::printHEX(char *str){
  char *hstr;
  hstr=str;
  char out[3];
  memset(out,'\0',2);
  int i=0;
  bool flag=false;
  while(*hstr){
    flag=itoa((int)*hstr,out,16);
    
    if(flag){
      printChar(out);

      if(debug){
        Serial.print(out);
      }      
    }
    hstr++;
  }
}

void AT_SIM7020::setupMQTT(String server,String port,String clientID,String username,String password){
  //Disconnect Client
  //AT+QMTDISC=0<CR><LF>
  Serial.println(F(">>Disconnecting server"));
  _Serial->println(F("AT+CMQDISCON=0"));
  while(1){
  if(_Serial->available()){
    data_input = _Serial->readStringUntil('\n');
    Serial.println(data_input);
    if(data_input.indexOf(F("OK"))!=-1){
      delay(1000);
      break;
    }
    if(data_input.indexOf(F("ERROR"))!=-1){
      break;
    }
   }
  }
  _Serial->flush();
  delay(1000);

  data_input="";
  //AT+QMTOPEN=0,"128.199.189.72",19000<CR><LF>
  Serial.println(F(">>Open MQTT Connection"));
  _Serial->print(F("AT+CMQNEW="));
  _Serial->print(F("\""));
  _Serial->print(server);
  _Serial->print(F("\""));
  _Serial->print(F(","));
  _Serial->print(F("\""));
  _Serial->print(port);
  _Serial->print(F("\""));
  _Serial->print(F(","));
  _Serial->print(12000);
  _Serial->print(F(","));
  _Serial->print(1000);
  _Serial->println();

  Serial.print(F("AT+CMQNEW="));
  Serial.print(F("\""));
  Serial.print(server);
  Serial.print(F("\""));
  Serial.print(F(","));
  Serial.print(F("\""));
  Serial.print(port);
  Serial.print(F("\""));
  Serial.print(F(","));
  Serial.print(12000);
  Serial.print(F(","));
  Serial.print(100);
  Serial.println();
  //_Serial->println(F("AT+QMTOPEN=0,\"128.199.189.72\",19000"));
  while(1){
  if(_Serial->available()){
    data_input = _Serial->readStringUntil('\n');
    Serial.println(data_input);
    if(data_input.indexOf(F("+CMQNEW:"))!=-1){
      break;
    }
   }
  }
  delay(1000);
  //AT+QMTCONN=0,"Mythings"<CR><LF>
  Serial.println(F(">>Create Things"));
  _Serial->print(F("AT+CMQCON=0,"));
  _Serial->print(3);
  _Serial->print(F(","));
  _Serial->print(F("\""));
  _Serial->print(clientID);
  _Serial->print(F("\""));
  _Serial->print(F(","));
  _Serial->print(600);
  _Serial->print(F(","));
  _Serial->print(0);
  _Serial->print(F(","));
  _Serial->print(0);

  Serial.print(F("AT+CMQCON=0,"));
  Serial.print(3);
  Serial.print(F(","));
  Serial.print(F("\""));
  Serial.print(clientID);
  Serial.print(F("\""));
  Serial.print(F(","));
  Serial.print(600);
  Serial.print(F(","));
  Serial.print(0);
  Serial.print(F(","));
  Serial.print(0);

  if (username.length()>0)
  {
    _Serial->print(F(","));
    _Serial->print(F("\""));
    _Serial->print(username);
    _Serial->print(F("\""));
    _Serial->print(F(","));
    _Serial->print(F("\""));
    _Serial->print(password);
    _Serial->print(F("\""));

    Serial.print(F(","));
    Serial.print(F("\""));
    Serial.print(username);
    Serial.print(F("\""));
    Serial.print(F(","));
    Serial.print(F("\""));
    Serial.print(password);
    Serial.print(F("\""));
  }

  Serial.println();
  _Serial->println();
  //_Serial->println(F("AT+QMTCONN=0,\"Mythings\""));
  while(1){
  if(_Serial->available()){
    data_input = _Serial->readStringUntil('\n');
    Serial.println(data_input);
    if(data_input.indexOf(F("OK"))!=-1){
      break;
    }
   }
  }
}

void AT_SIM7020::publish(String topic,String payload)
{
  Serial.println(F(">>publish"));
  Serial.print(F(">>topic="));
  Serial.println(topic);
  Serial.print(F(">>payload="));
  Serial.println(payload);
  
  Serial.print(F("AT+CMQPUB=0,\""));
  _Serial->print(F("AT+CMQPUB=0,\""));

  Serial.print(topic);
  Serial.print(F("\""));

  _Serial->print(topic);
  _Serial->print(F("\""));

  
  Serial.print(",");
  Serial.print(0);

  _Serial->print(F(","));
  _Serial->print(0);

  Serial.print(",");
  Serial.print(0);

  _Serial->print(F(","));
  _Serial->print(0);

  Serial.print(",");
  Serial.print(0);

  _Serial->print(F(","));
  _Serial->print(0);
  
  Serial.print(",");
  Serial.print(payload.length()*2);
  
  _Serial->print(F(","));
  _Serial->print(payload.length()*2);
  
  Serial.print(",");
  _Serial->print(F(","));

  char data[payload.length()+1];
  memset(data,'\0',payload.length());
  payload.toCharArray(data,payload.length()+1);

  printHEX(data);

  Serial.println();
  _Serial->println();

  //Serial.println(F(">>END"));

  while(1){
  if(_Serial->available()){
    data_input = _Serial->readStringUntil('\n');
    Serial.println(data_input);
    if(data_input.indexOf(F("OK"))!=-1){
      break;
    }
    if (data_input.indexOf(F("ERROR"))!=-1)
    {
      break;
    }
   }
  }
}
void AT_SIM7020::subscribe(String topic)
{
  //SUB
  //AT+QMTSUB=0,1,"we",0<CR><LF>
  Serial.println(F(">>Subscribe"));
  Serial.print(F("AT+CMQSUB=0,\""));
  _Serial->print(F("AT+CMQSUB=0,\""));
  Serial.print(topic);
  _Serial->print(topic);
  Serial.println(F("\",1"));
  _Serial->println(F("\",1"));

  while(1){
  if(_Serial->available()){
    data_input = _Serial->readStringUntil('\n');
    Serial.println(data_input);
    if(data_input.indexOf(F("OK"))!=-1){
      break;
    }
   }
  }
}

void AT_SIM7020::hex2ascii(String data_payload){
  data_buffer="";

}

void AT_SIM7020::MQTTresponse(){

  if (_Serial->available()){
    //Serial.println("available");
        //Serial.print("data serial ");
        //Serial.println(data_input);
      char data=(char)_Serial->read();
      if(data=='\n' || data=='\r'){
          //Serial.print("data_input end is ");
          //Serial.println(data_input);
          end=true;
      }
      else{
        data_input+=data;
        
      }
  }
    if (end){
      if (data_input.indexOf(F("+CMQPUB"))!=-1){
        Serial.print("data_input is ");
        Serial.println(data_input);

        char buf[data_input.length()+1];
        memset(buf,'\0',data_input.length());
        data_input.toCharArray(buf, sizeof(buf));

        char *p = buf;
        char *str;
        int i=0;
        int j=0;
        while ((str = strtok_r(p, ",", &p)) != NULL){
          // delimiter is the comma
         Serial.println(str);
          if(i==1){
            retTopic=str;
            int topiclen=retTopic.length();
            retTopic.remove(0,2);
            retTopic.remove(topiclen-3,topiclen);
          }
          if (i==6){
            // String hexpayload=str;
            // String buffer="";
            //payload="";
            // for(unsigned int k=2;k<hexpayload.length()+1;k+=2)
            // {
            //   char str_ascii=(char) strtol(&hexpayload.substring(k-2,k)[0], NULL, 16);
            //   Serial.print(str_ascii);
            //   buffer+=str_ascii;
            //   Serial.print(buffer);     
            // }
            retPayload=str;
            int payloadlen=retPayload.length();
            retPayload.remove(0,2);
            retPayload.remove(payloadlen-3,payloadlen);
            //payload=p_payload;

            if (MQcallback_p != NULL){
              Serial.print("topic ");
              Serial.println(retTopic);
              Serial.print("payload");
              Serial.println(retPayload);
              MQcallback_p(retTopic,retPayload);
            }
          }
          i++;
        }
      }

      data_input="";
      end=false;
    }

}
/****************************************/
/**          Private                   **/
/****************************************/
void AT_SIM7020::connectNetwork(){  
  _Serial->println(F("AT+CGATT=1"));
  delay(1000);
  for(int i=0;i<30;i++){
    if(_Serial->available()){
      data_input =  _Serial->readStringUntil('\n');
      if(debug) Serial.println("cgatt=1: "+data_input);
      if(data_input.indexOf(F("OK"))!=-1) break;
      else if(data_input.indexOf(F("ERROR"))!=-1){ 
       // _Serial->println(F("AT+CGATT=1"));
        //delay(1000);
        break;
      }
    }
  }
  //data_input="";
}

bool AT_SIM7020::setPhoneFunction(){
  bool status=false;
  _Serial->println(F("AT+CFUN=1"));
   while(1){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
      if(data_input.indexOf(F("OK"))!=-1){
        status=true;
        break;
      }
      else if(data_input.indexOf(F("ERROR"))!=-1){
        status=false;
        break;
      }
    }
  }
  //_serial_flush();
  return status;
}

bool AT_SIM7020::createUDPSocket(String address,String port){
  bool status=false;
  _Serial->println(F("AT+CSOC=1,2,1"));
  delay(500);
  while(1){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
      //if(debug)Serial.println("createUDPSocket status : "+data_input);
      if(data_input.indexOf(F("OK"))!=-1){
        //break;
      }
      else if(data_input.indexOf(F("+CSOC:"))!=-1){
        status=true;
        break;
      }
    }
  }

  if(status){
    _Serial->print(F("AT+CSOCON=0,"));
    _Serial->print(port);
    _Serial->print(F(","));
    _Serial->println(address);
    while(1){
      if(_Serial->available()){
        data_input=_Serial->readStringUntil('\n');
        //if(debug)Serial.println("CSOCON : "+data_input);
        if(data_input.indexOf(F("OK"))!=-1){
          break;
        }
        else if(data_input.indexOf(F("ERROR"))!=-1) {
          status = false;
          break;
        }
      }
    }
  }
  return status;
}

bool AT_SIM7020::closeUDPSocket(){
  _Serial->println(F("AT+CSOCL=0"));
  while(1){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
      if(data_input.indexOf(F("OK"))!=-1){
        break;
      }
    }
  }
}

/*void AT_SIM7020::set_PSM_report(unsigned int n){
  _Serial->print(F("AT+NPSMR"));
  _Serial->println(n);
   while(1){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
      if(data_input.indexOf(F("OK"))!=-1) break;
    }
  }
}*/

void AT_SIM7020::_serial_flush(){
  while(1){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
      //if(debug)Serial.println("flush : "+data_input);
    }
    else{
      data_input="";
      break;
    }
  }
  _Serial->flush();
}

void AT_SIM7020::echoOff(){
  _Serial->println(F("ATE0"));
  while(1){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
      if(data_input.indexOf(F("OK"))!=-1) break;
    }
  }
}

bool AT_SIM7020::enterPIN(){
  bool status=false;
  _Serial->println(F("AT+CPIN?"));
  while(1){
    if(_Serial->available()){
      data_input = _Serial->readStringUntil('\n');
      if(debug) Serial.println("CPIN: "+data_input);
      if(data_input.indexOf(F("+CPIN:"))!=-1){
        if(data_input.indexOf(F("READY"))!=-1) status=true;
      }
      if(data_input.indexOf(F("OK"))!=-1) break;
    }
  }
  //if(debug) Serial.println("flush from cpin");
  _serial_flush();
  //if(debug) Serial.println("end flush from cpin");
  return status;
}

/*void AT_SIM7020::send_cmd(const char* cmd){
  Serial2.println(cmd);
}

int AT_SIM7020::recvResp(char *buff, int count, unsigned int timeout){
  int i=0;
  unsigned long start,prev;
  start = millis();
  while(1){
    while(Serial2.available()){
      buff[i++]=Serial2.read();
      //i++;
      prev = millis();
      
      if(i >= count){
        Serial.print("i out : ");
        Serial.println(buff);
        return i;
      }      
    }

    if(timeout){
      if(millis()-start > timeout*1000){
        Serial.print("timeout : ");
        Serial.println(buff);
        break;        
      }
    }
  }
}

bool AT_SIM7020::sendCmd(const char* cmd, const char* resp, unsigned int timeout = DEFAULT_TIMEOUT){
  char buff[100];
  send_cmd(cmd);
  recvResp(buff,100,5);
  if(NULL != strstr(buff,resp)){
    return  true;
  }else{
    return  false;
  }
}*/