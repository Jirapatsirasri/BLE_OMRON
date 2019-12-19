#include "MagMQTT.h"



#if defined( __ARDUINO_X86__)
    #define Serial_PORT Serial1
#elif defined(ARDUINO_NUCLEO_L476RG)
    #define Serial_PORT Serial1
#elif defined(ARDUINO_AVR_UNO) || (ARDUINO_AVR_MEGA2560)
    AltSoftSerial module_serial;
#else 
    AltSoftSerial module_serial;
#endif 



MagMQTT::MagMQTT()
{
}

bool MagMQTT::begin(char *auth)
{

  bool created=false;

  #if defined( __ARDUINO_X86__)
      Serial1.begin(9600);
      _Serial = &Serial1;
      //Serial.println(F("PLEASE USE Hardware Serial"))
  #elif defined(ARDUINO_NUCLEO_L476RG)
      Serial1.begin(9600);
      _Serial = &Serial1;
      //Serial.println(F("PLEASE USE PIN RX=48 & TX=46"))
  #elif defined(ARDUINO_AVR_UNO)
      module_serial.begin(9600);
      _Serial = &module_serial;
  #elif defined(ARDUINO_AVR_MEGA2560)
      module_serial.begin(9600);
      _Serial = &module_serial;
      Serial.println(F("PLEASE USE PIN RX=48 & TX=46"));
  #else 
      module_serial.begin(9600);
      _Serial = &module_serial;
  #endif 

  check_module_ready();
  reboot_module();


  setup_module();
  setupMQTT();
}

void MagMQTT::reboot_module()
{

  Serial.println(F(">>Rebooting "));
  _Serial->println(F("AT+NRB"));
  delay(1000);

  while(1){
    if(_Serial->available()){
      data_input = _Serial->readStringUntil('\n');
      if(data_input.indexOf(F("OK"))!=-1){
        Serial.println(data_input);
        break;
      }
      else{
        if (data_input.indexOf(F("REBOOT"))!=-1)
        {
          Serial.println(data_input);
        }
        else{
          Serial.print(F("."));
        }
      }
    }
  }
  delay(10000);

}
void MagMQTT::setup_module()
{
  data_input="";
  Serial.println(F(">>Setting"));
  _Serial->println(F("AT+CMEE=1"));
  delay(500);
  Serial.print(F(".")); 
}
void MagMQTT::check_module_ready()
{
  _Serial->println(F("AT"));
  delay(100);
  _Serial->println(F("AT"));
  while(1){
    if(_Serial->available()){
      data_input = _Serial->readStringUntil('\n');
      if(data_input.indexOf(F("OK"))!=-1){
        hw_connected=true;
        Serial.println(F("Module OK"));
        break;
      }
    }
    else{
      unsigned int current_check=millis();
      if (current_check-previous_check>5000)
      {
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
void MagMQTT::_serial_flush()
{
  while(1){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
    }
    else{
      break;
    }
  }
}

void MagMQTT::setupMQTT()
{
	//Disconnect Client
	//AT+QMTDISC=0<CR><LF>
	Serial.println(F(">>Disconnect Client"));
	_Serial->println(F("AT+QMTDISC=0"));
	while(1){
	if(_Serial->available()){
	  data_input = _Serial->readStringUntil('\n');
	  Serial.println(data_input);
	  if(data_input.indexOf(F("OK"))!=-1){
	    break;
	  }
	  if(data_input.indexOf(F("ERROR"))!=-1){
	    break;
	  }
	 }
	}
	delay(10000);
	//AT+QMTDISC=0<CR><LF>
	//AT+QMTCFG="version",0,4<CR><LF>
	Serial.println(F(">>Setup MQTT version"));
	_Serial->println(F("AT+QMTCFG=\"version\",0,4"));
	while(1){
	if(_Serial->available()){
	  data_input = _Serial->readStringUntil('\n');
	  Serial.println(data_input);
	  if(data_input.indexOf(F("OK"))!=-1){
	    break;
	  }
	 }
	}
	delay(10000);
	//AT+QMTOPEN=0,"128.199.189.72",19000<CR><LF>
	Serial.println(F(">>Open MQTT Connection"));
	_Serial->print(F("AT+QMTOPEN=0,"));
	_Serial->print(F("\""));
	printchar(server);
	_Serial->print(F("\""));
	_Serial->print(F(","));
	printchar(port);
	_Serial->println();
	//_Serial->println(F("AT+QMTOPEN=0,\"128.199.189.72\",19000"));
	while(1){
	if(_Serial->available()){
	  data_input = _Serial->readStringUntil('\n');
	  Serial.println(data_input);
	  if(data_input.indexOf(F("OK"))!=-1){
	    break;
	  }
	 }
	}
	delay(10000);
	//AT+QMTCONN=0,"Mythings"<CR><LF>
	Serial.println(F(">>Create Things"));
	_Serial->print(F("AT+QMTCONN=0,"));
	_Serial->print(F("\""));
	printchar(clientID);
	_Serial->print(F("\""));

	if (strlen(username)>1)
	{
		_Serial->print(F(","));
		_Serial->print(F("\""));
		printchar(username);
		_Serial->print(F("\""));
		_Serial->print(F(","));
		_Serial->print(F("\""));
		printchar(password);
		_Serial->print(F("\""));
	}

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
	delay(10000);
}

void MagMQTT::printchar(char *str)
{
  char *hstr;
  hstr=str;
  while(*hstr)
  {
  	Serial.print(*hstr);
  	_Serial->print(*hstr);

    hstr++;
  }
}

void MagMQTT::publish(String topic,String payload)
{
	//PUB Atmost one
	//AT+QMTPUB=0,0,0,0,"we"<CR><LF>
	Serial.println(F(">>publish"));
	Serial.print(F("AT+QMTPUB=0,0,0,0,\""));
	_Serial->print(F("AT+QMTPUB=0,0,0,0,\""));
	Serial.print(topic);
	_Serial->print(topic);
	Serial.println(F("\""));
	_Serial->println(F("\""));
	Serial.print(F(">>payload="));
	Serial.println(payload);
	_Serial->print(payload);
	Serial.println(F(">>END"));
	Serial.write(26);
	_Serial->write(26);
	while(1){
	if(_Serial->available()){
	  data_input = _Serial->readStringUntil('\n');
	  Serial.println(data_input);	
	  if(data_input.indexOf(F("OK"))!=-1){

	  }
	 }
	else{
		break;
	}
	}
	//PUB Atlest one
	//AT+QMTPUB=0,1,1,0,"we"<CR><LF>

	//PUB Exactly one
	//AT+QMTPUB=0,1,2,0,"we"<CR><LF>


	//PUB Retain 
	//AT+QMTPUB=0,1,2,1,"we"<CR><LF>
}


void MagMQTT::subscribe(String topic)
{
	//SUB
	//AT+QMTSUB=0,1,"we",0<CR><LF>
	Serial.println(F(">>Subscribe"));
	Serial.print(F("AT+QMTSUB=0,1,\""));
	_Serial->print(F("AT+QMTSUB=0,1,\""));
	Serial.print(topic);
	_Serial->print(topic);
	Serial.println(F("\",0"));
	_Serial->println(F("\",0"));
}



int MagMQTT::RegisCallback(MQTTClientCallback MQTT_func)
{
     int r = -1;

     if (callback_p == NULL)
     {
          callback_p = MQTT_func;
          r = 0;
     }
     //else - already in use! Fail!

     return r;
}

void MagMQTT::Loop()
{

	String topic="NULL";
	String payload="NULL";

	if (_Serial->available())
	{
	    char data=(char)_Serial->read();
	    if(data=='\n' || data=='\r')
	    {
	      if(k>2)
	      {
	        end=true;
	        //Serial.print("data_input is ");
	        //Serial.println(data_input);
	        k=0;
	      }
	      k++;
	    }
	    else
	    {
	      data_input+=data;
	      
	    }
	}
    if (end)
    {
    	Serial.print("data_input is ");
    	Serial.println(data_input);

        int index1 = data_input.indexOf(F(","));
        if(index1!=-1) 
        {
          int index2 = data_input.indexOf(F(","),index1+1);
          index1 = data_input.indexOf(F(","),index2+1);

		  String prev_buffer=data_input.substring(index2+1,index1);
		  topic=prev_buffer;

		  index2 = data_input.indexOf(F(","),index1+1);
		  prev_buffer=data_input.substring(index1+1,index2);
		  payload=prev_buffer;
		  
		  Serial.println(topic);
		  Serial.println(payload);
		  //Serial.println(data_input.substring(2,3));
      	}
      	
      	
		if (callback_p != NULL)
		{
			callback_p(topic,payload);
		}
		else{
			Serial.println("NULL");
		}
		data_input="";
		end=false;
    }
	
}