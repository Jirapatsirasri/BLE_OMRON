/*
Magellan v3.3.3 NB-IoT Playgroud Platform .
support Quectel BC95 
NB-IoT with AT command

supported board and hardware competible
  Arduino UNO please use software serial on pin8=RX pin9=TX and pin4=RST
  Arduino MEGA please use software serial on pin48=RX pin46=TX and pin4=RST 
  NUCLEO_L476RG please use Hardware serial on pin2=RX pin8=TX and pin4=RST

Develop with coap protocol 
reference with 
https://tools.ietf.org/html/rfc7252

support post and get method
only Magellan IoT Platform 
https://www.aismagellan.io

*** the payload limit up on dynamic memory of your board   
*** if you use arduino uno it limit 100
    character for payload string 
 
    example
      payload = {"temperature":25.5}
      the payload is contain 20 character

Author:   Phisanupong Meepragob
Create Date:     1 May 2017. 
Modified: 26 June 2019.
Modify by: Phisanupong Meepragob

(*bug fix can not get response form buffer but can send data to the server)
(*v1.58.1 bug fix get method retransmit message)
(*v2.1.0 bug fix multiple definition variable with AIS_NB_BC95)
(*v2.1.1 bug fix get method retransmit)
(*v2.1.2 bug fix REBOOT CAUSE by hardware (BC95 hardware reset))
(*v2.2.2 bug fix check hardware connection with NB-Shield module)
(*v2.3.0 add feature get data from dashboard)
(*v2.3.1 bug fix disable print state when server send repeat response)
(*v2.3.2 bug fix init test AT command and include hardware check information)
(*v2.3.3 add IMSI information)
(*v2.3.4 bug fix display matching with msg id )
(*v2.3.5 bug fix repeat scan if hardware connection error! )
(*v2.3.6 bug fix lost communication with NB-IoT Module! )
(*v2.3.7 bug fix time retransmit and count timeout 3 time to reset)
(*v3.0.1 Change end point and This version create for Magellan2.0 )
(*v3.1.1 Supported Module BC95G )
(*v3.2.1 Supported Magellan V3 message contruct )
(*v3.3.1 Add new feature Magellan V3 and add NUCLEO_L053R8 board )
(*v3.3.2 add register unique value to thingsregister function)
(*v3.3.3 bug fix add register value )
Released for private use.
*/

#include "Magellan.h"
Magellan nbiot;

#if defined( __ARDUINO_X86__)
    #define Serial_PORT Serial1
#elif defined(ARDUINO_NUCLEO_L476RG)
    #define Serial_PORT Serial1
#elif defined(ARDUINO_NUCLEO_L053R8)
    HardwareSerial mySerial(USART1);
#elif defined(ARDUINO_AVR_UNO) || (ARDUINO_AVR_MEGA2560)
    AltSoftSerial moduleserial;
#else 
    AltSoftSerial moduleserial;
#endif 

bool BC95G=false;

const char serverIP[] = "52.163.218.116";
//const char serverIP[] = "103.20.205.85";
//const char serverIP[] = "103.20.205.104";
//const char serverIP[] = "104.215.191.117";
//const char serverIP[] = "49.228.131.49";

Magellan::Magellan()
{
}
/*------------------------------
  Connect to NB-IoT Network
              &
  Initial Magellan
  ------------------------------
*/
bool Magellan::begin(char *auth)
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
  #elif defined(ARDUINO_NUCLEO_L053R8)
      mySerial.begin(9600);
      _Serial = &mySerial;
  #elif defined(ARDUINO_AVR_UNO)
      moduleserial.begin(9600);
      _Serial = &moduleserial;
  #elif defined(ARDUINO_AVR_MEGA2560)
      moduleserial.begin(9600);
      _Serial = &moduleserial;
      Serial.println(F("PLEASE USE PIN RX=48 & TX=46"));
  #else 
      moduleserial.begin(9600);
      _Serial = &moduleserial;
  #endif 


  Serial.println(F("               AIS BC95 NB-IoT Magellan V3.3.1"));

  /*---------------------------------------
      Initial BC95 Module 
    ---------------------------------------
  */
  previous_check=millis();

  if(LastError!=""){
    Serial.print(F("LastError"));
    Serial.println(LastError);    
  }
  check_module_ready();

  if (!hw_connected)
  {
    en_post=false;
    en_get=false;
    return false;
  }
  reboot_module();
  setup_module();
 
  auth_len=0;
  pathauth=auth;
  cnt_auth_len(pathauth);

  if(auth_len>13){
    path_hex_len=2;
  }
  else{
    path_hex_len=1;
  }
  
  previous=millis();
  created=true;
  previous_time_get=millis();


  return created;
}

void Magellan::reboot_module()
{

  Serial.println(F(">>Rebooting "));
  _Serial->println(F("AT+NRB"));
  delay(100);

  while(1){
    if(_Serial->available()){
      data_input = _Serial->readStringUntil('\n');
      if(data_input.indexOf(F("OK"))!=-1){
        Serial.println(data_input);
        imsi=data_input;
        break;
      }
      else{
        if (data_input.indexOf(F("REBOOT_"))!=-1)
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

  Serial.print(F(">>IMSI "));
  _Serial->println(F("AT+CIMI"));
  previous=millis();
  while(1){
    
    if(_Serial->available()){
      data_input = _Serial->readStringUntil('\n');
      Serial.println(data_input);
      
      unsigned int current=millis();
      //Serial.println(current-previous);
      if(current-previous>1000){
        //Serial.println("break");
        previous=current;
        break;
      }
      // if (data_input.indexOf('OK')!=-1)
      // {
      //   //Serial.println("break");
      //   break;
      // }
      if(data_input.indexOf('52003')!=-1)
      {
        imsi=data_input;
        if (imsi.length()>15)
        {
          imsi.remove(15,imsi.length());
        }
        break;
      }
      else
      {
        _Serial->println(F("AT+CIMI"));
      }

      //if(data_input.indexOf(F("OK"))!=-1){
      //  break;
      //}
      //else{
      //  Serial.print(data_input);
      //}
    }
  }
  Serial.println();
  delay(100);

}

void Magellan::setup_module()
{
  data_input="";
  Serial.print(F(">>Setting"));
  _Serial->println(F("AT+CMEE=1"));
  delay(500);
  Serial.print(F("."));
  Serial.print(F("Dis Huiwai"));
  _Serial->println(F("AT+QREGSWT=2"));
  delay(500);
  Serial.print(F("Set PSM report"));
  _Serial->println(F("AT+NPSMR=1")); //open PSM Report
  delay(500);
  Serial.print(F("Read Model"));
  _Serial->println(F("AT+CGMM"));
  delay(500);
  previous=millis();
  //Serial.println(previous);
  while(1){
    if(_Serial->available()){
      data_input = _Serial->readStringUntil('\n');
      if(debug) Serial.println(data_input);
      if(data_input.indexOf(F("BC95G"))!=-1){
        Serial.println(F("BC95Gxxx"));
        BC95G=true;
        break;
      }
    }
    else{
        unsigned int current=millis();

        //Serial.println(current-previous);
        previous=current;
        break;
        
    }
  }
  previous=millis();
  data_input="";
  /*--------------------------------------
      Config module parameter
    --------------------------------------
  */
  _Serial->println(F("AT+CFUN=1"));
  delay(6000);

  Serial.print(F("."));
  _Serial->println(F("AT+NCONFIG=AUTOCONNECT,true"));
  delay(500);

  Serial.println(F("OK"));
  _Serial->println(F("AT+CGATT=1"));
  delay(500);
  Serial.print(F(">>Connecting"));
  _serial_flush();
  /*--------------------------------------
      Check network connection
    --------------------------------------
  */

  while(1){
    if(_Serial->available()){
      data_input = _Serial->readStringUntil('\n');
      if(debug) Serial.println(data_input);
      if(data_input.indexOf(F("+CGATT:1"))!=-1){
        break;
      }
    }
    _Serial->println(F("AT+CGATT?"));
    Serial.print(F("."));
    delay(2000);
  }
  data_input="";
  /*-------------------------------------
      Create network socket
    -------------------------------------
  */
  Serial.println(F("OK"));
  _Serial->println(F("AT+NSOCR=DGRAM,17,5684,1"));
  delay(500);
  _serial_flush();
  data_input="";
    
}

void Magellan::check_module_ready()
{
  _Serial->println(F("AT"));
  delay(100);
  _Serial->println(F("AT"));
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

void Magellan::_serial_flush()
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

/*------------------------------
    CoAP Message menagement 
  ------------------------------
*/
void Magellan::printHEX(char *str)
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
      _Serial->print(out);

      if(debug)
      {
        Serial.print(out);
      }
      
    }
    hstr++;
  }
}
void Magellan::printmsgID(unsigned int messageID)
{
  char Msg_ID[3];
  
  utoa(highByte(messageID),Msg_ID,16);
  if(highByte(messageID)<16)
  {
    if(debug) Serial.print(F("0"));
    _Serial->print(F("0"));
    if(debug) Serial.print(Msg_ID);
    _Serial->print(Msg_ID);
  }
  else
  {
    _Serial->print(Msg_ID);
    if(debug)  Serial.print(Msg_ID);
  }

  utoa(lowByte(messageID),Msg_ID,16);
  if(lowByte(messageID)<16)
  {
    if(debug)  Serial.print(F("0"));
    _Serial->print(F("0"));
    if(debug)  Serial.print(Msg_ID);
    _Serial->print(Msg_ID);
  }
  else
  {
    _Serial->print(Msg_ID);
    if(debug)  Serial.print(Msg_ID);
  }
} 
void Magellan::print_pathlen(unsigned int path_len,char *init_str)
{   
    unsigned int extend_len=0;

    if(path_len>13){
      extend_len=path_len-13;

      char extend_L[3];
      itoa(lowByte(extend_len),extend_L,16);
      _Serial->print(init_str);
      _Serial->print(F("d"));

      if(debug) Serial.print(init_str);
      if(debug) Serial.print(F("d"));

      if(extend_len<=15){
        _Serial->print(F("0"));
        _Serial->print(extend_L);

        if(debug) Serial.print(F("0"));
        if(debug) Serial.print(extend_L);
      }
      else{
        _Serial->print(extend_L);
        if(debug) Serial.print(extend_L);
      }
      

    }
    else{
      if(path_len<=9)
      {
        char hexpath_len[2]="";   
        sprintf(hexpath_len,"%i",path_len);
        _Serial->print(init_str);
        _Serial->print(hexpath_len);
        if(debug) Serial.print(init_str);
        if(debug) Serial.print(hexpath_len);

      }
      else
      {
        if(path_len==10) 
        {
          _Serial->print(init_str);
          _Serial->print(F("a"));
          if(debug) Serial.print(init_str);
          if(debug) Serial.print(F("a"));
        }
        if(path_len==11)
        {
          _Serial->print(init_str);
          _Serial->print(F("b"));
          if(debug) Serial.print(init_str);
          if(debug) Serial.print(F("b"));
        } 
        if(path_len==12)
        {
          _Serial->print(init_str);
          _Serial->print(F("c"));
          if(debug) Serial.print(init_str);
          if(debug) Serial.print(F("c"));
        } 
        if(path_len==13)
        {
          _Serial->print(init_str);
          _Serial->print(F("d"));
          if(debug) Serial.print(init_str);
          if(debug) Serial.print(F("d"));
        } 
        
      }
   }  
}

void Magellan::print_pathlen2(unsigned int path_len,String init_str)
{   
    unsigned int extend_len=0;

    if(path_len>13){
      extend_len=path_len-13;

      char extend_L[3];
      itoa(lowByte(extend_len),extend_L,16);
      _Serial->print(init_str);
      _Serial->print(F("d"));

      if(debug) Serial.print(init_str);
      if(debug) Serial.print(F("d"));

      if(extend_len<=15){
        _Serial->print(F("0"));
        _Serial->print(extend_L);

        if(debug) Serial.print(F("0"));
        if(debug) Serial.print(extend_L);
      }
      else{
        _Serial->print(extend_L);
        if(debug) Serial.print(extend_L);
      }
      

    }
    else{
      if(path_len<=9)
      {
        char hexpath_len[2]="";   
        sprintf(hexpath_len,"%i",path_len);
        _Serial->print(init_str);
        _Serial->print(hexpath_len);
        if(debug) Serial.print(init_str);
        if(debug) Serial.print(hexpath_len);

      }
      else
      {
        if(path_len==10) 
        {
          _Serial->print(init_str);
          _Serial->print(F("a"));
          if(debug) Serial.print(init_str);
          if(debug) Serial.print(F("a"));
        }
        if(path_len==11)
        {
          _Serial->print(init_str);
          _Serial->print(F("b"));
          if(debug) Serial.print(init_str);
          if(debug) Serial.print(F("b"));
        } 
        if(path_len==12)
        {
          _Serial->print(init_str);
          _Serial->print(F("c"));
          if(debug) Serial.print(init_str);
          if(debug) Serial.print(F("c"));
        } 
        if(path_len==13)
        {
          _Serial->print(init_str);
          _Serial->print(F("d"));
          if(debug) Serial.print(init_str);
          if(debug) Serial.print(F("d"));
        } 
        
      }
   }  
}

void Magellan::cnt_auth_len(char *auth)
{
  char *hstr;
  hstr=auth;
  char out[3]="";
  int i=0;
  bool flag=false;
  while(*hstr)
  {
    auth_len++;
    
    
    hstr++;
  }
}

unsigned int Magellan::option_len(unsigned int path_len)
{
  unsigned int len=0;
    if (path_len>0)
      {
        if (path_len>13)
        {
          len=2;
        }
        else
        {
          len=1;
        }
      }
  return len+path_len;
}

void Magellan::print_uripath(String uripath)
{
    unsigned int uripathlen=uripath.length();
    if (uripathlen>0)
      {
          print_pathlen(uripathlen,"0");
          
          char data[uripath.length()+1]="";
          uripath.toCharArray(data,uripath.length()+1);

          printHEX(data);
      }
}

void Magellan::print_uripath2(String uripath,String optnum)
{
    unsigned int uripathlen=uripath.length();
    if (uripathlen>0)
      {
          print_pathlen2(uripathlen,optnum);
          
          char data[uripath.length()+1];
          memset(data,'\0',uripath.length());
          uripath.toCharArray(data,uripath.length()+1);

          printHEX(data);
      }
}
/*----------------------------- 
    CoAP Method POST
  -----------------------------
*/
String Magellan::post(String payload)
{
  #if defined(ARDUINO_AVR_PRO) || (ARDUINO_AVR_UNO)
    if(payload.length()>100){
      Serial.println("Warning payload size exceed the limit of memory");
    }
    else{
      return post_data(payload,"","","");
    }
  #else
    return post_data(payload,"","","");
  #endif
  
}

String Magellan::post2(String payload)
{
  #if defined(ARDUINO_AVR_PRO) || (ARDUINO_AVR_UNO)
    if(payload.length()>200){
      Serial.println("Warning payload size exceed the limit of memory");
    }
    else{
      return post_data2(payload,"Data","NBIoT","V1.0");
    }
  #else
    return post_data2(payload,"Data","NBIoT","V1.0");
  #endif
  
}

String Magellan::post3(String payload,String token)
{
  option coapoption[2];
  unsigned int totaloption=2;

  String stropt[2];
  stropt[0]="NBIoT";

  coapoption[0].stroption=stropt[0];
  coapoption[0].optlen=stropt[0].length();
  coapoption[0].optionnum=11;

  stropt[1]=token;
  coapoption[1].stroption=stropt[1];
  coapoption[1].optlen=stropt[1].length();
  coapoption[1].optionnum=11;

  // stropt[2]="v1";
  // coapoption[2].stroption="v1";
  // coapoption[2].optlen=len(stropt);
  // coapoption[2].optionnum=11;

  // stropt[3]="fe514860-47c5-11e9-96dd-9fb5d8a71344";
  // coapoption[3].stroption=stropt;
  // coapoption[3].optlen=len(stropt);
  // coapoption[3].optionnum=11;

  // stropt[4]="10.0.0.1";
  // coapoption[4].stroption=stropt;
  // coapoption[4].optlen=len(stropt);
  // coapoption[4].optionnum=11;

  #if defined(ARDUINO_AVR_PRO) || (ARDUINO_AVR_UNO)
    if(payload.length()>200){
      Serial.println("Warning payload size exceed the limit of memory");
    }
    else{
      return post_data3(payload,coapoption,totaloption);
    }
  #else
    return post_data3(payload,coapoption,totaloption);
  #endif
  
}

//------------------------ Magellan 2.0 ------------------
String Magellan::post_report(String payload)
{
  #if defined(ARDUINO_AVR_PRO) || (ARDUINO_AVR_UNO)
    if(payload.length()>200){
      Serial.println("Warning payload size exceed the limit of memory");
    }
    else{
      return post_data2(payload,"Report","NBIoT","V1.0");
    }
  #else
    return post_data2(payload,"Report","NBIoT","V1.0");
  #endif
  
}

//------------------------ Magellan 3.0 -------------------
String Magellan::thingsregister()
{
  Serial.print(F("imsi register:"));  
  Serial.println(imsi);
  option coapoption[5];
  unsigned int totaloption=5;

  String stropt[5];

  stropt[0]="register";
  coapoption[0].stroption=stropt[0];
  coapoption[0].optlen=stropt[0].length();
  coapoption[0].optionnum=11;

  stropt[1]="sim";
  coapoption[1].stroption=stropt[1];
  coapoption[1].optlen=stropt[1].length();
  coapoption[1].optionnum=11;

  stropt[2]="v1";
  coapoption[2].stroption=stropt[2];
  coapoption[2].optlen=stropt[2].length();
  coapoption[2].optionnum=11;

  stropt[3]=imsi;
  coapoption[3].stroption=stropt[3];
  coapoption[3].optlen=stropt[3].length();
  coapoption[3].optionnum=11;

  stropt[4]="10.0.0.1";
  coapoption[4].stroption=stropt[4];
  coapoption[4].optlen=stropt[4].length();
  coapoption[4].optionnum=11;


  return post_data3("",coapoption,totaloption);
  
  
}

String Magellan::report(String payload,String Token)
{
  
  option coapoption[5];
  unsigned int totaloption=5;

  String stropt[5];

  stropt[0]="report";
  coapoption[0].stroption=stropt[0];
  coapoption[0].optlen=stropt[0].length();
  coapoption[0].optionnum=11;

  stropt[1]="sim";
  coapoption[1].stroption=stropt[1];
  coapoption[1].optlen=stropt[1].length();
  coapoption[1].optionnum=11;

  stropt[2]="v1";
  coapoption[2].stroption=stropt[2];
  coapoption[2].optlen=stropt[2].length();
  coapoption[2].optionnum=11;

  stropt[3]=Token;
  coapoption[3].stroption=stropt[3];
  coapoption[3].optlen=stropt[3].length();
  coapoption[3].optionnum=11;

  stropt[4]="10.0.0.1";
  coapoption[4].stroption=stropt[4];
  coapoption[4].optlen=stropt[4].length();
  coapoption[4].optionnum=11;

  #if defined(ARDUINO_AVR_PRO) || (ARDUINO_AVR_UNO)
    if(payload.length()>200){
      Serial.println("Warning payload size exceed the limit of memory");
    }
    else{
      return post_data3(payload,coapoption,totaloption);
    }
  #else
    return post_data3(payload,coapoption,totaloption);
  #endif
  
}


String Magellan::linenotify(String token,String msg)
{
  return post_data(msg,"Linenotify",token,"");
}
String Magellan::post_data(String payload,String option1,String option2,String option3)
{
  unsigned int timeout[5]={12000,14000,18000,26000,42000};
  unsigned int prev_send=millis();

  if(!get_process && en_post){
    data_buffer="";
    previous_send=millis();
    send_ACK=false;
    ACK=false;
    success=false;
    token=random(0,32767);
    post_token=token;
    if(debug) Serial.println(F("Load new payload"));
    Msg_ID++;
    for (byte i = 0; i <= 4; ++i)
    {
      post_process=true;
      en_chk_buff=false;
      post_ID=Msg_ID;
      Msgsend(payload,option1,option2,option3);

      
      while(true)
      {
        en_chk_buff=true;
        waitResponse();
        unsigned int currenttime=millis();
        if (currenttime-previous_send>timeout[i] || success)
        {
          if(debug) Serial.println(currenttime-previous_send);
          previous_send=currenttime;
          en_post=true;
          en_get=true;
          post_process=false;
          break;
        }
      }
      if (success)
      {
        break;
      }
      else{
            if(i+1<5){
              if(printstate) Serial.print(F(">> Retransmit"));
              if(printstate) Serial.println(i+1);
              if(printstate) Serial.println(timeout[i+1]);
            }

      }

     }
     if (!success)
     {
       	Serial.print(F("timeout : "));
        data_input="";
        count_timeout++;
        Serial.println(count_timeout);
   		if(printstate) Serial.println();

	       if(count_timeout>=3)
	       {
	       	count_timeout = 0;
		        do{
		            check_module_ready();
		        }
		        while(!hw_connected);
		        reboot_module();
		        setup_module();
	       }
        
     }
    }

    

  post_process=false;
  _Serial->flush();
  return data_buffer;
}


String Magellan::post_data2(String payload,String option1,String option2,String option3)
{
  unsigned int timeout[5]={12000,14000,18000,26000,42000};
  unsigned int prev_send=millis();

  if(!get_process && en_post){
    data_buffer="";
    previous_send=millis();
    send_ACK=false;
    ACK=false;
    success=false;
    token=random(0,32767);
    post_token=token;
    if(debug) Serial.println(F("Load new payload"));
    Msg_ID++;
    for (byte i = 0; i <= 4; ++i)
    {
      post_process=true;
      en_chk_buff=false;
      post_ID=Msg_ID;
      //Msgsend(payload,option1,option2,option3);
      Msgsend2(payload,option1,option2,option3);
      
      while(true)
      {
        en_chk_buff=true;
        waitResponse();
        unsigned int currenttime=millis();
        if (currenttime-previous_send>timeout[i] || success)
        {
          if(debug) Serial.println(currenttime-previous_send);
          previous_send=currenttime;
          en_post=true;
          en_get=true;
          post_process=false;
          break;
        }
      }
      if (success)
      {
        break;
      }
      else{
            if(i+1<5){
              if(printstate) Serial.print(F(">> Retransmit"));
              if(printstate) Serial.println(i+1);
              if(printstate) Serial.println(timeout[i+1]);
            }

      }

     }
     if (!success)
     {
        Serial.print(F("timeout : "));
        data_input="";
        count_timeout++;
        Serial.println(count_timeout);
      if(printstate) Serial.println();

         if(count_timeout>=3)
         {
          count_timeout = 0;
            do{
                check_module_ready();
            }
            while(!hw_connected);
            reboot_module();
            setup_module();
         }
        
     }
    }

    

  post_process=false;
  _Serial->flush();
  return data_buffer;
}

String Magellan::post_data3(String payload,option *coapOption,unsigned int totaloption)
{
  unsigned int timeout[5]={12000,14000,18000,26000,42000};
  unsigned int prev_send=millis();
  rcvdata="";
  data_buffer="";
  if(!get_process && en_post){
    
    previous_send=millis();
    send_ACK=false;
    ACK=false;
    success=false;
    token=random(0,32767);
    post_token=token;
    if(debug) Serial.println(F("Load new payload"));
    Msg_ID++;
    for (byte i = 0; i <= 4; ++i)
    {
      post_process=true;
      en_chk_buff=false;
      post_ID=Msg_ID;
      //Msgsend(payload,option1,option2,option3);
      Msgsend3(payload,coapOption,totaloption);
      
      while(true)
      {
        en_chk_buff=true;
        waitResponse();
        unsigned int currenttime=millis();
        if (currenttime-previous_send>timeout[i] || success)
        {
          if(debug) Serial.println(currenttime-previous_send);
          previous_send=currenttime;
          en_post=true;
          en_get=true;
          post_process=false;
          break;
        }
      }
      if (success)
      {
        break;
      }
      else{
            if(i+1<5){
              if(printstate) Serial.print(F(">> Retransmit"));
              if(printstate) Serial.println(i+1);
              if(printstate) Serial.println(timeout[i+1]);
            }

      }

     }
     if (!success)
     {
        Serial.print(F("timeout : "));
        data_input="";
        count_timeout++;
        Serial.println(count_timeout);
      if(printstate) Serial.println();

         if(count_timeout>=3)
         {
          count_timeout = 0;
            do{
                check_module_ready();
            }
            while(!hw_connected);
            reboot_module();
            setup_module();
         }
        
     }
    }

    

  post_process=false;
  _Serial->flush();
  return rcvdata;
}
/*----------------------------- 
    CoAP Method GET
  -----------------------------
*/
String Magellan::get(String Resource,char *strmagtoken)
{
  return get_data(Resource,"",strmagtoken);
}

String Magellan::get2(String Resource)
{
  return get_data2(Resource,"Data","NBIoT","V1.0","");
}

//--------------- Magellan 3 -------------------------
String Magellan::getConfig(String Resource,String Token)
{

  String ipaddr="10.0.0.1";
  option coapoption[6];
  unsigned int totaloption=6;

  String stropt[6];

  stropt[0]="config";
  coapoption[0].stroption=stropt[0];
  coapoption[0].optlen=stropt[0].length();
  coapoption[0].optionnum=11;

  stropt[1]="sim";
  coapoption[1].stroption=stropt[1];
  coapoption[1].optlen=stropt[1].length();
  coapoption[1].optionnum=11;

  stropt[2]="v1";
  coapoption[2].stroption=stropt[2];
  coapoption[2].optlen=stropt[2].length();
  coapoption[2].optionnum=11;

  stropt[3]=Token;
  coapoption[3].stroption=stropt[3];
  coapoption[3].optlen=stropt[3].length();
  coapoption[3].optionnum=11;

  stropt[4]=ipaddr;
  coapoption[4].stroption=stropt[4];
  coapoption[4].optlen=stropt[4].length();
  coapoption[4].optionnum=11;

  stropt[5]=Resource;
  coapoption[5].stroption=stropt[5];
  coapoption[5].optlen=stropt[5].length();
  coapoption[5].optionnum=15;

  return get_data3(coapoption,totaloption,"");
}

String Magellan::getDelta(String Resource,String Token)
{

  String ipaddr="10.0.0.1";

  option coapoption[6];
  unsigned int totaloption=6;

  String stropt[6];

  stropt[0]="delta";
  coapoption[0].stroption=stropt[0];
  coapoption[0].optlen=stropt[0].length();
  coapoption[0].optionnum=11;

  stropt[1]="sim";
  coapoption[1].stroption=stropt[1];
  coapoption[1].optlen=stropt[1].length();
  coapoption[1].optionnum=11;

  stropt[2]="v1";
  coapoption[2].stroption=stropt[2];
  coapoption[2].optlen=stropt[2].length();
  coapoption[2].optionnum=11;

  stropt[3]=Token;
  coapoption[3].stroption=stropt[3];
  coapoption[3].optlen=stropt[3].length();
  coapoption[3].optionnum=11;

  stropt[4]=ipaddr;
  coapoption[4].stroption=stropt[4];
  coapoption[4].optlen=stropt[4].length();
  coapoption[4].optionnum=11;

  stropt[5]=Resource;
  coapoption[5].stroption=stropt[5];
  coapoption[5].optlen=stropt[5].length();
  coapoption[5].optionnum=15;

  return get_data3(coapoption,totaloption,"");
}


String Magellan::get_sw(String Resource)
{
  String data_rt=get_data2(Resource,"Data","NBIoT","V1.0","");
  if (data_rt.indexOf(F("Don't change!!"))!=-1)
  {
    /* code */
    if(debug) Serial.println("Don't change!!");
  }
  else{
    post_report(Resource);
  }
  
  return data_rt;
}

String Magellan::get_dashboard(String Resource)
{
  return get_data("dashboard."+Resource,"","");
}
String Magellan::get_api(String Resource,String Proxy)
{
  #if defined(ARDUINO_AVR_PRO) || (ARDUINO_AVR_UNO)
    return "Warning this module has not enough memory";
  #else
    return get_data(Resource,Proxy,"");
  #endif

}
String Magellan::get_data(String Resource,String Proxy,char *strmagtoken)
{

  int timeout[5]={8000,16000,32000,64000,128000};
  rcvdata="";
  data_buffer="";
  if(!post_process && en_get){
    do{
      token=random(0,32767);
      get_token=token;
      success=false;
      for (int i = 0; i <=4; ++i)
        {
          get_process=true;
          data_buffer="";

          //char *pathtoken;
          //pathtoken=strmagtoken;
          Msgget(Resource,Proxy,strmagtoken);          
          //}
          en_chk_buff=false;
          while(true)
          {
            en_chk_buff=true;
            unsigned int current_time=millis();
            

            if(current_time-previous_time_get>timeout[i] || success || (ACK && more_flag) || NOTFOUND){
              previous_time_get=current_time;
              if(i==4){
                if(printstate) Serial.println(F(">> Get timeout"));
                get_process=false;
              }
              if(debug) Serial.println(F("break"));
              break;
            }
            waitResponse();
            
          }


          if((rcvdata.length()>0 && GETCONTENT && !more_flag) || success || NOTFOUND){
            get_process=false;
            break;
          }
          else{
            if (!more_flag)
            {
              if(printstate) Serial.print(F(">> Retransmit"));
              if(printstate) Serial.println(i+1);
              if(printstate) Serial.println(timeout[i]);
            }

          }
        }
      }while(more_flag);
  }
  return rcvdata;
}

String Magellan::get_data2(String Resource,String option1,String option2,String option3,String Proxy)
{

  int timeout[5]={8000,16000,32000,64000,128000};
  rcvdata="";
  data_buffer="";
  if(!post_process && en_get){
    do{
      token=random(0,32767);
      get_token=token;
      success=false;
      for (int i = 0; i <=4; ++i)
        {
          get_process=true;
          data_buffer="";
          Msgget2(Resource,option1,option2,option3,Proxy);          
          //}
          en_chk_buff=false;
          while(true)
          {
            en_chk_buff=true;
            unsigned int current_time=millis();
            

            if(current_time-previous_time_get>timeout[i] || success || (ACK && more_flag) || NOTFOUND){
              previous_time_get=current_time;
              if(i==4){
                if(printstate) Serial.println(F(">> Get timeout"));
                get_process=false;
              }
              if(debug) Serial.println(F("break"));
              break;
            }
            waitResponse();
            
          }


          if((rcvdata.length()>0 && GETCONTENT && !more_flag) || success || NOTFOUND){
            get_process=false;
            break;
          }
          else{
            if (!more_flag)
            {
              if(printstate) Serial.print(F(">> Retransmit"));
              if(printstate) Serial.println(i+1);
              if(printstate) Serial.println(timeout[i]);
            }

          }
        }
      }while(more_flag);
  }
  return rcvdata;
}

String Magellan::get_data3(option *coapoption,unsigned int totaloption,String Proxy)
{

  int timeout[5]={8000,16000,32000,64000,128000};
  rcvdata="";
  data_buffer="";
  if(!post_process && en_get){
    do{
      token=random(0,32767);
      get_token=token;
      success=false;
      for (int i = 0; i <=4; ++i)
        {
          get_process=true;
          data_buffer="";
          Msgget3(coapoption,totaloption,Proxy);          
          //}
          en_chk_buff=false;
          while(true)
          {
            en_chk_buff=true;
            unsigned int current_time=millis();
            

            if(current_time-previous_time_get>timeout[i] || success || (ACK && more_flag) || NOTFOUND){
              previous_time_get=current_time;
              if(i==4){
                if(printstate) Serial.println(F(">> Get timeout"));
                get_process=false;
              }
              if(debug) Serial.println(F("break"));
              break;
            }
            waitResponse();
            
          }


          if((rcvdata.length()>0 && GETCONTENT && !more_flag) || success || NOTFOUND){
            get_process=false;
            break;
          }
          else{
            if (!more_flag)
            {
              if(printstate) Serial.print(F(">> Retransmit"));
              if(printstate) Serial.println(i+1);
              if(printstate) Serial.println(timeout[i]);
            }

          }
        }
      }while(more_flag);
  }
  return rcvdata;
}
/*-----------------------------
    Massage Package
  -----------------------------
*/
void Magellan::Msgsend(String payload,String option1,String option2,String option3)
{

  // send_buffer=payload;
  // char data[send_buffer.length()+1]="";
  // send_buffer.toCharArray(data,send_buffer.length()+1);

  char data[payload.length()+1]="";
  payload.toCharArray(data,payload.length()+1);
  
  if(en_post){

      if(printstate) Serial.print(F(">> post: Msg_ID "));
      if(printstate) Serial.print(Msg_ID);
      if(printstate) Serial.print(F(" "));
      if(printstate) Serial.print(payload);

      if (BC95G)
      {
        if (debug) Serial.println("BC95G");
        _Serial->print(F("AT+NSOST=1,"));
      }
      else
      {
        if (debug) Serial.println("BC95");
        _Serial->print(F("AT+NSOST=0,"));
      }

      _Serial->print(serverIP);
      _Serial->print(F(",5683,"));

      unsigned int buff_len=auth_len+11+2+path_hex_len+payload.length(); //header 2 + 
      unsigned int option1_len=option1.length();
      unsigned int option2_len=option2.length();
      unsigned int option3_len=option3.length();

      buff_len+=option_len(option1_len);
      buff_len+=option_len(option2_len);
      buff_len+=option_len(option3_len);

      _Serial->print(buff_len);
      if(debug) Serial.print(F(",4202"));

      _Serial->print(F(",4202"));                 // 2
      printmsgID(Msg_ID);                         // 2
      printmsgID(post_token);                     //print token 2
      _Serial->print(F("b54e42496f54"));          // 6
      if(debug) Serial.print(F("b54e42496f54"));
      print_pathlen(auth_len,"0");

      printHEX(pathauth);

      print_uripath(option1);
      print_uripath(option2);
      print_uripath(option3);

      //_Serial->print(F("1132"));                 //content-type json
      _Serial->print(F("ff")); 
      if(debug) Serial.print(F("ff"));             // 1
      printHEX(data);  
      _Serial->println();

      if(printstate) Serial.println();
      
  }   
}

void Magellan::Msgsend2(String payload,String option1,String option2,String option3)
{

  // send_buffer=payload;
  // char data[send_buffer.length()+1]="";
  // send_buffer.toCharArray(data,send_buffer.length()+1);
  char data[payload.length()+1]="";
  payload.toCharArray(data,payload.length()+1);
  
  if(en_post){

      if(printstate) Serial.print(F(">> post: Msg_ID "));
      if(printstate) Serial.print(Msg_ID);
      if(printstate) Serial.print(F(" "));
      if(printstate) Serial.print(payload);

      _Serial->print(F("AT+NSOST=0,"));
      _Serial->print(serverIP);
      _Serial->print(F(",5683,"));

      //unsigned int buff_len=auth_len+11+2+path_hex_len+payload.length(); //header 2 + 
      unsigned int buff_len=5+2+2+payload.length(); //header 2 + 
      unsigned int option1_len=option1.length();
      unsigned int option2_len=option2.length();
      unsigned int option3_len=option3.length();

      buff_len+=option_len(option1_len);
      buff_len+=option_len(option2_len);
      buff_len+=option_len(option3_len);

      _Serial->print(buff_len);
      if(debug) Serial.print(F(",4202"));

      _Serial->print(F(",4202"));                 // 2
      //_Serial->print(F(",4002"));
      printmsgID(Msg_ID);                         // 2
      printmsgID(post_token);                     //print token 2
      //_Serial->print(F("b54e42496f54"));        
      //if(debug) Serial.print(F("b54e42496f54"));
      //print_pathlen(auth_len,"0");

      //printHEX(pathauth);
      print_pathlen(option1_len,"b");
      
      char data3[option1.length()+1]="";
      option1.toCharArray(data3,option1.length()+1);
      printHEX(data3);
      //print_uripath(option1);
      print_uripath(option2);
      print_uripath(option3);

      _Serial->print(F("1132"));                 //content-type json 2
      _Serial->print(F("ff")); 
      if(debug) Serial.print(F("ff"));             // 1
      printHEX(data);  
      _Serial->println();

      if(printstate) Serial.println();
      
  }   
}

void Magellan::Msgsend3(String payload,option *coapOption,unsigned int totaloption)
{

  option *stroption1;
  stroption1=coapOption;

  option *stroption2;
  stroption2=coapOption;
  // send_buffer=payload;
  // char data[send_buffer.length()+1]="";
  // send_buffer.toCharArray(data,send_buffer.length()+1);
  char data[payload.length()+1];
  memset(data,'\0',payload.length());
  payload.toCharArray(data,payload.length()+1);
  
  if(en_post){

      if(printstate) Serial.print(F(">> post: Msg_ID "));
      if(printstate) Serial.print(Msg_ID);
      if(printstate) Serial.print(F(" "));
      if(printstate) Serial.print(payload);

      if (BC95G)
      {
        if (debug) Serial.println("BC95G");
        _Serial->print(F("AT+NSOST=1,"));
      }
      else
      {
        if (debug) Serial.println("BC95");
        _Serial->print(F("AT+NSOST=0,"));
      }

      _Serial->print(serverIP);
      _Serial->print(F(",5683,"));

      if(debug) Serial.print(F("AT+NSOST=0,"));
      if(debug) Serial.print(serverIP);
      if(debug) Serial.print(F(",5683,"));
      //unsigned int buff_len=auth_len+11+2+path_hex_len+payload.length(); //header 2 + 
      //unsigned int buff_len=5+2+2+payload.length(); //header 2 + 
      unsigned int buff_len=2+2+2+2+payload.length(); //header 2 + 

      if (payload.length()>0)
      {
        buff_len+=1;
      }

      for(unsigned int i=0;i<totaloption;i++)
      {
        buff_len+=stroption1->optlen+1;
        if(stroption1->optlen>13)
        {
          buff_len+=1;
        }
        stroption1++;
      }

      // buff_len+=option_len(option1_len);
      // buff_len+=option_len(option2_len);
      // buff_len+=option_len(option3_len);

      _Serial->print(buff_len);
      if(debug) Serial.print(buff_len);
      if(debug) Serial.print(F(",4202"));

      _Serial->print(F(",4202"));                 // 2
      //_Serial->print(F(",4002"));
      printmsgID(Msg_ID);                         // 2
      printmsgID(post_token);                     //print token 2
      //_Serial->print(F("b54e42496f54"));        
      //if(debug) Serial.print(F("b54e42496f54"));
      //print_pathlen(auth_len,"0");

      //printHEX(pathauth);
      //print_pathlen(option1_len,"b");
      
      // char data3[option1.length()+1]="";
      // option1.toCharArray(data3,option1.length()+1);
      // printHEX(data3);
      // //print_uripath(option1);
      // print_uripath(option2);
      // print_uripath(option3);

      unsigned int lastopt=0;
      unsigned int optnum=0;
      unsigned int outopt=0;

      for(unsigned int i=0;i<totaloption;i++)
      { 
        optnum=stroption2->optionnum;
        outopt=optnum-lastopt;
        String init_opt;

        // Serial.print(">>> optnum");
        // Serial.print(optnum);
        // Serial.println("<<<");
        // Serial.print(">>> lastopt");
        // Serial.print(lastopt);
        // Serial.println("<<<");
        // Serial.print(">>> outopt");
        // Serial.print(outopt);
        // Serial.println("<<<");

        lastopt=optnum;

        switch(outopt)
        {
          case 10:
            init_opt="a";
            break;

          case 11:
            init_opt="b";
            break;

          case 12:
            init_opt="c";
            break;

          case 13:
            init_opt="d";
            break;

          case 14:
            init_opt="e";
            break;

          case 15:
            init_opt="f";
            break;

          default:
                init_opt=String(outopt);
          
        }

        print_uripath2(stroption2->stroption,init_opt);

        stroption2++;
      }

      _Serial->print(F("1132"));                 //content-type json 2
      if(debug) Serial.print(F("1132"));                 //content-type json 2
      if (payload.length()>0)
      {
        _Serial->print(F("ff")); 
        if(debug) Serial.print(F("ff"));             // 1
        printHEX(data);  
      }
      _Serial->println();
      if(printstate) Serial.println();
      
  }   
}

void Magellan::Msgget(String Resource,String Proxy,char *magtoken)
{
  
  char data[Resource.length()+1]="";
  Resource.toCharArray(data,Resource.length()+1); 
  GETCONTENT=false;
  ACK=false;
  Msg_ID++;
  get_ID=Msg_ID;
  if(printstate) Serial.print(F(">> GET data : Msg_ID "));
  if(printstate) Serial.print(Msg_ID);
  if(printstate) Serial.print(" ");
  if(printstate) Serial.print(Resource);
  
  _Serial->print(F("AT+NSOST=0,"));
  _Serial->print(serverIP);
  _Serial->print(F(",5683,"));

  if(debug) Serial.print(F("AT+NSOST=0,")); 
  if(debug) Serial.print(serverIP);
  if(debug) Serial.print(F(",5683,")); 

  unsigned int path_len=Resource.length();
  unsigned int buff_len=0;
  if(path_len>13){
    buff_len=52+path_len;
  }
  else{
    buff_len=51+path_len;
  }

  if(Resource.indexOf(F("API"))!=-1){
    buff_len=buff_len+2+Proxy.length();
    if(debug) Serial.print(buff_len);
  }

  _Serial->print(buff_len+2);
  if(debug) Serial.print(F(",4201"));
  _Serial->print(F(",4201"));
  printmsgID(Msg_ID);
  printmsgID(get_token);                     //print token
  _Serial->print(F("b54e42496f54"));
  if(debug) Serial.print(F("b54e42496f54"));
  _Serial->print(F("0d17"));
  if(debug) Serial.print(F("0d17"));
  printHEX(magtoken);

  print_pathlen(path_len,"4");

  printHEX(data);

  _Serial->print(F("8104"));      //Block size 256 recommended for BC95 with CoAP Header
  if(debug) Serial.print(F("8104"));

  //_Serial->print(F("8105"));      //Block size 512 recommended for BC95
  //if(debug) Serial.print(F("8105"));
  //_Serial->print(F("8106"));    //Block size 1024
  //if(debug) Serial.print(F("8106"));
  
  /* ##########################
      Proxy Uri additional
     ########################## 
  */ 
  if(Resource.indexOf(F("API"))!=-1){

      char data[Proxy.length()+1]="";
      Proxy.toCharArray(data,Proxy.length()+1); 

      //if(debug) Serial.print(F("420184"));    //Block size 256 recommended for BC95 with CoAP Header
      //_Serial->print(F("420184")); 
      //if(debug) Serial.print(F("420185"));  //Block size 512 recommended for BC95
      //_Serial->print(F("420185"));   
      //print_pathlen(Proxy.length(),"8");
      print_pathlen(Proxy.length(),"c");
      printHEX(data);
  }



  _Serial->println();

  _Serial->flush();

  if(printstate) Serial.println();
  sendget=true;
}

void Magellan::Msgget2(String Resource,String option1,String option2,String option3,String Proxy)
{
  
  char data[Resource.length()+1]="";
  Resource.toCharArray(data,Resource.length()+1); 
  GETCONTENT=false;
  ACK=false;
  Msg_ID++;
  get_ID=Msg_ID;
  if(printstate) Serial.print(F(">> GET data : Msg_ID "));
  if(printstate) Serial.print(Msg_ID);
  if(printstate) Serial.print(" ");
  if(printstate) Serial.print(Resource);
  
  _Serial->print(F("AT+NSOST=0,"));
  _Serial->print(serverIP);
  _Serial->print(F(",5683,"));

  if(debug) Serial.print(F("AT+NSOST=0,")); 
  if(debug) Serial.print(serverIP);
  if(debug) Serial.print(F(",5683,")); 

  unsigned int path_len=Resource.length();
  unsigned int buff_len=0;
  if(path_len>13){
    buff_len=2+path_len;
  }
  else{
    buff_len=1+path_len;
  }
  //buff_len+=50;
  unsigned int option1_len=option1.length();
  unsigned int option2_len=option2.length();
  unsigned int option3_len=option3.length();
  
  buff_len+=option_len(option1_len);
  buff_len+=option_len(option2_len);
  buff_len+=option_len(option3_len);

  if(Resource.indexOf(F("API"))!=-1){
    buff_len=buff_len+2+Proxy.length();
    if(debug) Serial.print(buff_len);
  }

  _Serial->print(buff_len+2);                 // +Block size(2)
  if(debug) Serial.print(F(",4201"));
  _Serial->print(F(",4201"));
  printmsgID(Msg_ID);
  printmsgID(get_token);                     //print token
  print_pathlen(option1_len,"b");
  char data3[option1.length()+1]="";
  option1.toCharArray(data3,option1.length()+1);
  printHEX(data3);
  print_uripath(option2);
  print_uripath(option3);
  _Serial->print(F("0d17"));
  if(debug) Serial.print(F("0d17"));
  printHEX(pathauth);
  print_pathlen(path_len,"4");
  printHEX(data);
  _Serial->print(F("8104"));      //Block size 256 recommended for BC95 with CoAP Header
  if(debug) Serial.print(F("8104"));          // 2 

  //_Serial->print(F("8105"));      //Block size 512 recommended for BC95
  //if(debug) Serial.print(F("8105"));
  //_Serial->print(F("8106"));    //Block size 1024
  //if(debug) Serial.print(F("8106"));
  
  /* ##########################
      Proxy Uri additional
     ########################## 
  */ 
  if(Resource.indexOf(F("API"))!=-1){

      char data[Proxy.length()+1]="";
      Proxy.toCharArray(data,Proxy.length()+1); 

      //if(debug) Serial.print(F("420184"));    //Block size 256 recommended for BC95 with CoAP Header
      //_Serial->print(F("420184")); 
      //if(debug) Serial.print(F("420185"));  //Block size 512 recommended for BC95
      //_Serial->print(F("420185"));   
      //print_pathlen(Proxy.length(),"8");
      print_pathlen(Proxy.length(),"c");
      printHEX(data);
  }



  _Serial->println();

  _Serial->flush();

  if(printstate) Serial.println();
  sendget=true;
}

void Magellan::Msgget3(option *coapOption,unsigned int totaloption,String Proxy)
{
 
  option *stroption1;
  stroption1=coapOption;

  option *stroption2;
  stroption2=coapOption;

  String Resource="";

  char data[Resource.length()+1]="";
  Resource.toCharArray(data,Resource.length()+1); 
  GETCONTENT=false;
  ACK=false;
  Msg_ID++;
  get_ID=Msg_ID;
  if(printstate) Serial.print(F(">> GET data : Msg_ID "));
  if(printstate) Serial.print(Msg_ID);
  if(printstate) Serial.print(" ");
  if(printstate) Serial.print(Resource);
  
  if (BC95G)
  {
    if (debug) Serial.println("BC95G");
    _Serial->print(F("AT+NSOST=1,"));
  }
  else
  {
    if (debug) Serial.println("BC95");
    _Serial->print(F("AT+NSOST=0,"));
  }

  _Serial->print(serverIP);
  _Serial->print(F(",5683,"));

  //if(debug) Serial.print(F("AT+NSOST=0,")); 
  if(debug) Serial.print(serverIP);
  if(debug) Serial.print(F(",5683,")); 

  unsigned int path_len=Resource.length();
  unsigned int buff_len=2+2+2+2;

  for(unsigned int i=0;i<totaloption;i++)
  {
    buff_len+=stroption1->optlen+1;
    if(stroption1->optlen>13)
    {
      buff_len+=1;
    }
    stroption1++;
  }

  _Serial->print(buff_len);
  if(debug) Serial.print(F(",4201"));
  _Serial->print(F(",4201"));
  printmsgID(Msg_ID);
  printmsgID(get_token);                     //print token


  unsigned int lastopt=0;
  unsigned int optnum=0;
  unsigned int outopt=0;

  for(unsigned int i=0;i<totaloption;i++)
  { 
    optnum=stroption2->optionnum;
    outopt=optnum-lastopt;
    String init_opt;

    lastopt=optnum;

    switch(outopt)
    {
      case 10:
        init_opt="a";
        break;

      case 11:
        init_opt="b";
        break;

      case 12:
        init_opt="c";
        break;

      case 13:
        init_opt="d";
        break;

      case 14:
        init_opt="e";
        break;

      case 15:
        init_opt="f";
        break;

      default:
            init_opt=String(outopt);
      
    }

    print_uripath2(stroption2->stroption,init_opt);

    stroption2++;
  }

  _Serial->print(F("8104"));      //Block size 256 recommended for BC95 with CoAP Header
  if(debug) Serial.print(F("8104"));          // 2 

  //_Serial->print(F("8105"));      //Block size 512 recommended for BC95
  //if(debug) Serial.print(F("8105"));
  //_Serial->print(F("8106"));    //Block size 1024
  //if(debug) Serial.print(F("8106"));
  

  _Serial->println();

  _Serial->flush();

  if(printstate) Serial.println();
  sendget=true;
}
/*-------------------------------------
    Response Message management

    Example Message
    Type: ACK
    MID: 000001
    Code: Created
    Payload:200
  -------------------------------------
*/
void Magellan::print_rsp_header(String Msgstr)
{

  if(debug) Serial.println(Msgstr);

  resp_msgID = (unsigned int) strtol( &Msgstr.substring(4,8)[0], NULL, 16);
  print_rsp_Type(Msgstr.substring(0,2),resp_msgID);

  bool en_print=(post_process && resp_msgID==post_ID) || (get_process && resp_msgID==get_ID);

  switch((int) strtol(&Msgstr.substring(2,4)[0], NULL, 16))
   {
      case EMPTY:
                    EMP=true;
                    Msgstr.remove(0, 8);
                    break;
      case CREATED: 
                    EMP=false;
                    NOTFOUND=false;
                    GETCONTENT=false;
                    RCVRSP=true;

                    if (Msgstr.length()/2>4)
                    {
                      rsptoken=(unsigned int) strtol( &Msgstr.substring(8,12)[0], NULL, 16);
                      if (post_process && post_token==rsptoken)
                      {
                        if(debug) Serial.println(F("match token"));
                        if(debug) Serial.print(rsptoken);
                        success=true;
                        
                      }

                      Msgstr.remove(0, 12);
                    }
                    else{
                      Msgstr.remove(0, 8);
                    }

                    if(printstate && en_print) Serial.println(F("2.01 CREATED")); 
                    break;
      case DELETED: //if(printstate && en_print) Serial.println(F("2.02 DELETED")); 
                    break;
      case VALID: //if(printstate && en_print) Serial.println(F("2.03 VALID"));
                  break;
      case CHANGED: //if(printstate && en_print) Serial.println(F("2.04 CHANGED"));
                  break;
      case CONTENT: 
                    EMP=false;
                    NOTFOUND=false;
                    GETCONTENT=true;
                    RCVRSP=false;
                    if(get_process){
                      String blocksize=Msgstr.substring(20,22);
                      if(blocksize.indexOf(F("0C"))!=-1)
                        {
                          more_flag=true;
                        } 
                      if(blocksize.indexOf(F("04"))!=-1){ 
                          more_flag=false;
                        }

                      if (Msgstr.length()/2>4)
                      {
                        rsptoken=(unsigned int) strtol( &Msgstr.substring(8,12)[0], NULL, 16);
                        if (get_process && get_token==rsptoken)
                        {
                          if(debug) Serial.println(F("match token"));
                          if(debug) Serial.print(rsptoken);
                          if(!more_flag) success=true;
                          
                        }
                      }
                      Msgstr.remove(0, 8);
                    }
                    

                    if(post_process){

                      if (Msgstr.length()/2>4)
                      {
                        rsptoken=(unsigned int) strtol( &Msgstr.substring(8,12)[0], NULL, 16);
                        if (post_process && post_token==rsptoken)
                        {
                          if(debug) Serial.println(F("match token"));
                          if(debug) Serial.print(rsptoken);
                          success=true;
                          
                        }

                        Msgstr.remove(0, 12);
                      }
                      else{
                        Msgstr.remove(0, 8);
                      }
                    }

                    if(printstate && en_print) Serial.println(F("2.05 CONTENT")); 
                    break;
      case CONTINUE: //if(printstate && en_print) Serial.println(F("2.31 CONTINUE"));
                    Msgstr.remove(0, 8); 
                    break;
      case BAD_REQUEST: if(printstate && en_print) Serial.println(F("4.00 BAD_REQUEST"));
                    Msgstr.remove(0, 8); 
                    break;
      case FORBIDDEN: if(printstate && en_print) Serial.println(F("4.03 FORBIDDEN"));
                    Msgstr.remove(0, 8); 
                    break;
      case NOT_FOUND: 
                    if(printstate && en_print) Serial.println(F("4.04 NOT_FOUND"));
                    GETCONTENT=false; 
                    NOTFOUND=true;
                    RCVRSP=false;
                    break;
      case METHOD_NOT_ALLOWED: 
                    RCVRSP=false;
                    if(printstate && en_print) Serial.println(F("4.05 METHOD_NOT_ALLOWED")); 
                    break;
      case NOT_ACCEPTABLE: if(printstate && en_print) Serial.println(F("4.06 NOT_ACCEPTABLE")); 
                    break;
      case REQUEST_ENTITY_INCOMPLETE: //if(printstate && en_print) Serial.println(F("4.08 REQUEST_ENTITY_INCOMPLETE")); 
                    break;
      case PRECONDITION_FAILED: //if(printstate && en_print) Serial.println(F("4.12 PRECONDITION_FAILED")); 
                    break;
      case REQUEST_ENTITY_TOO_LARGE: //if(printstate && en_print) Serial.println(F("4.13 REQUEST_ENTITY_TOO_LARGE")); 
                    break;
      case UNSUPPORTED_CONTENT_FORMAT: if(printstate && en_print) Serial.println(F("4.15 UNSUPPORTED_CONTENT_FORMAT")); 
                    break;
      case INTERNAL_SERVER_ERROR: if(printstate && en_print) Serial.println(F("5.00 INTERNAL_SERVER_ERROR")); 
                    break;
      case NOT_IMPLEMENTED: //if(printstate && en_print) Serial.println(F("5.01 NOT_IMPLEMENTED")); 
                    break;
      case BAD_GATEWAY: if(printstate && en_print) Serial.println(F("5.02 BAD_GATEWAY")); 
                    break;
      case SERVICE_UNAVAILABLE: if(printstate && en_print) Serial.println(F("5.03 SERVICE_UNAVAILABLE")); 
                    break;
      case GATEWAY_TIMEOUT: if(printstate && en_print) Serial.println(F("5.04 GATEWAY_TIMEOUT")); 
                    break;
      case PROXY_NOT_SUPPORTED: if(printstate && en_print) Serial.println(F("5.05 PROXY_NOT_SUPPORTED")); 
                    break;

      default : //Optional
                GETCONTENT=false;
   }

   if(printstate && en_print) Serial.print(F("   Msg_ID "));

   
   if(printstate && en_print) Serial.println(resp_msgID);
}

void Magellan::print_rsp_Type(String Msgstr,unsigned int msgID)
{
  bool en_print=(post_process && resp_msgID==post_ID) || (get_process && resp_msgID==get_ID);


  if(Msgstr.indexOf(ack)!=-1 || Msgstr.indexOf(acktk)!=-1)
  {
    
    if(printstate && en_print) Serial.print(F("<< ACK: "));
    if((resp_msgID==get_ID || resp_msgID==post_ID) && !EMP){
		        ACK=true;
	  }
    
    flag_rcv=true;
    en_post=true;
    en_get=true;
    
    send_ACK=false;
    cnt_cmdgetrsp=0;
  }
  if(Msgstr.indexOf(con)!=-1 || Msgstr.indexOf(contk)!=-1)
  {
    if(printstate && en_print) Serial.print(F("<< CON: "));
    //resp_cnt++;

    en_post=false;
    en_get=false;

    if(debug) Serial.println(F("Send ack"));
      if (BC95G)
      {
        _Serial->println(F("AT+NSORF=1,512"));
      }
      else
      {
        _Serial->println(F("AT+NSORF=0,512"));
      }
    _Serial->print(serverIP);
    _Serial->print(F(",5683,"));
    _Serial->print(F("4"));
    _Serial->print(F(",6000"));
    printmsgID(msgID);
    _Serial->println();

    send_ACK=true;   

    ACK=false;
    cnt_cmdgetrsp=0;
  }
  if(Msgstr.indexOf(rst)!=-1)
  {
    if(printstate && en_print) Serial.print(F("<< RST: "));
    flag_rcv=true;
    ACK=false;
    cnt_cmdgetrsp=0;
  }
  if(Msgstr.indexOf(non_con)!=-1)
  {
    if(printstate && en_print) Serial.print(F("<< Non-Con: "));
    flag_rcv=true;
    ACK=false;
    cnt_cmdgetrsp=0;
  }
}

/*-----------------------------------
  Get response data from BC95 Buffer 
  -----------------------------------  
*/
void Magellan::miniresponse(String rx)
{
  
  print_rsp_header(rx);
  

  bool en_print=(post_process && resp_msgID==post_ID) || (get_process && resp_msgID==get_ID);

  String payload_rx=rx.substring(12,rx.length());
  String data_payload="";
  unsigned int indexff=0;

  
  indexff=payload_rx.indexOf(F("FF"));
  

  if(payload_rx.indexOf(F("FFF"))!=-1)
  {
      data_payload=payload_rx.substring(indexff+3,payload_rx.length());

      if(printstate && en_print) Serial.print(F("   RSP:"));
      //if(!more_flag) data_buffer="";                                          //clr buffer
      data_buffer="";
      for(unsigned int k=2;k<data_payload.length()+1;k+=2)
      {
        char str=(char) strtol(&data_payload.substring(k-2,k)[0], NULL, 16);
        if(printstate && en_print) Serial.print(str);

        if(GETCONTENT or RCVRSP){
            if (post_process && post_token==rsptoken || get_process && get_token==rsptoken){
              data_buffer+=str;
            }
          
        }
      }
      if(GETCONTENT)
      {
        rcvdata+=data_buffer;
        //if(!more_flag) data_buffer="";
        data_buffer="";
        getpayload=true;
      } 
      if(printstate && en_print) Serial.println(F(""));
  }
  else
  {
      data_payload=payload_rx.substring(indexff+2,payload_rx.length());
      if(printstate && en_print) Serial.print(F("   RSP:"));
      //if(!more_flag) data_buffer="";
      data_buffer="";                                       //clr buffer
      for(unsigned int k=2;k<data_payload.length()+1;k+=2)
      {
        char str=(char) strtol(&data_payload.substring(k-2,k)[0], NULL, 16);
        if(printstate && en_print) Serial.print(str);

        if(GETCONTENT or RCVRSP){
          if (post_process && post_token==rsptoken || get_process && get_token==rsptoken){
            data_buffer+=str;
          }
        }
      }
      if(GETCONTENT) {
        rcvdata+=data_buffer;
        //if(!more_flag) data_buffer="";
        data_buffer="";
        getpayload=true;
      } 
      if(printstate && en_print) Serial.println(F(""));     
  }

  if(success)
  { 
    if(printstate && en_print) Serial.println(F("------------ End ------------"));
    /*
    while(1){
      if(_Serial->available()){
        data_input=_Serial->readStringUntil('\n');
      }
      else{
        break;
      }
    }
    */
  }
}

void Magellan:: waitResponse()
{ 
  unsigned long current=millis();
  if(en_chk_buff && (current-previous>=500) && !(_Serial->available()))
  {
      if (BC95G)
      {
        _Serial->println(F("AT+NSORF=1,512"));
      }
      else
      {
        _Serial->println(F("AT+NSORF=0,512"));
      }
      cnt_cmdgetrsp++;
      previous=current;
  }

  if(_Serial->available())
  {
    char data=(char)_Serial->read();
    if(data=='\n' || data=='\r')
    {
      if(k>2)
      {
        end=true;
        k=0;
      }
      k++;
    }
    else
    {
      data_input+=data;
    }
  }
  if(end){
      if(debug) Serial.println(data_input); 
      if(data_input.indexOf(F("+NSONMI:"))!=-1)
      {
          if(debug) Serial.print(F("send_NSOMI "));
          if(data_input.indexOf(F("+NSONMI:"))!=-1)
          {
            if (BC95G)
            {
              _Serial->println(F("AT+NSORF=1,512"));
            }
            else
            {
              _Serial->println(F("AT+NSORF=0,512"));
            }
            data_input=F("");
            send_NSOMI=true;
            if(printstate) Serial.println();
          }
          end=false;
      }
      else
        {


          end=false;
          if(data_input.indexOf(F("52.163.218.116"))!=-1)
          //if(data_input.indexOf(F("0,103.20.205.104"))!=-1)
          //if(data_input.indexOf(F("0,103.20.205.85"))!=-1)
          //if(data_input.indexOf(F("103.20.205.85"))!=-1)
          //if(data_input.indexOf(F("0,104.215.191.117"))!=-1)
          //if(data_input.indexOf(F("0,49.228.131.49"))!=-1)
          {
            int index1 = data_input.indexOf(F(","));
            if(index1!=-1) 
            {
              int index2 = data_input.indexOf(F(","),index1+1);
              index1 = data_input.indexOf(F(","),index2+1);
              index2 = data_input.indexOf(F(","),index1+1);
              index1 = data_input.indexOf(F(","),index2+1);

              String prev_buffer=data_input.substring(index2+1,index1);

              if(cnt_rcv_resp>=1 && prev_buffer.indexOf(F("FF"))!=-1 && more_flag){

                miniresponse(data_input.substring(index2+1,index1));
              }
              else{
                miniresponse(data_input.substring(index2+1,index1));
              }

              index2 = data_input.indexOf(F(","),index1+1);
              if(data_input.substring(index1+1,index2)!="0"){
                  if(debug) Serial.println(F("found buffer"));
                 if (BC95G)
                  {
                    _Serial->println(F("AT+NSORF=1,512"));
                  }
                  else
                  {
                    _Serial->println(F("AT+NSORF=0,512"));
                  }
                  cnt_rcv_resp++;
              }
              else{
                cnt_rcv_resp=0;
              }
              
            }
          }
          else if((data_input.indexOf(F("ERROR"))!=-1))
          {
            if((data_input.indexOf(F("+CME ERROR")))!=-1)
            {
              int index1 = data_input.indexOf(F(":"));
              if(index1!=-1) {
                LastError=data_input.substring(index1+1,data_input.length());

                if((LastError.indexOf(F("159")))!=-1)
                {
                   Serial.print(F("Uplink busy"));
                   data_buffer="";				//Ver 1.54  clr buffer   
                }
              }
            }

          }
          else if ((data_input.indexOf(F("REBOOT")))!=-1)
          {
              breboot_flag=true;
          }

          else if(breboot_flag && data_input.indexOf(F("OK"))!=-1)
          {
                Serial.print(F(">>HW RST"));
                post_process=false;
                get_process=false;
                breboot_flag=false;
                setup_module();
          }

          send_NSOMI=false;
          data_input=F("");
          }       
        }
}



