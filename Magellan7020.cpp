/*
Magellan7020 v4.0.0 NB-IoT Playgroud Platform .
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
only Magellan7020 IoT Platform 
https://www.aisMagellan7020.io

*** the payload limit up on dynamic memory of your board   
*** if you use arduino uno it limit 100
    character for payload string 
 
    example
      payload = {"temperature":25.5}
      the payload is contain 20 character

Author:   
Create Date: 1 May 2017. 
Modified: 17 June 2019.

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
(*v2.3.8 add feature get rssi)
(*v2.3.9 add example code and get rssi+imsi before connect)
(*v4.0.0 re-structure)
Released for private use.
*/

#include "Magellan7020.h"
Magellan7020 nbiotv4;

AT_SIM7020 at_udp;

//const char serverIP[] = "103.20.205.85";  
//const char serverIP[] = "35.245.33.132";
// const char serverIP[] = "52.163.218.116";
// const char port[] = "5684";

String serverIP = "52.163.218.116";
//String serverIP = "34.70.176.7";
String port = "5683";

// String serverIP = "13.67.66.193";
// String port = "5686";
//add
  int count_success=0;
Magellan7020::Magellan7020(){
}
/****************************************/
/**          Initialization            **/
/****************************************/
void Magellan7020::callback(String &datax)
{
  Serial.print(F("msg "));
  Serial.println(datax);
}

bool Magellan7020::begin(){
  if(debug) at_udp.debug=true;
  bool created=false;  
  Serial.println();
  Serial.println(F("               AIS NB-IoT Magellan 7020 V4.0.2"));

  at_udp.setupModule(serverIP,port);
  //imei.reserve(15);
  imsi.reserve(15);
  // //imei = at_udp.getIMEI();
  imsi = at_udp.getIMSI();
  // //model = at_udp.read_model();
  // if (model==F("BC95G"))
  //  {
  //    BC95G=true;
  //  } 
  // //at_udp.getSignal();
  
  deviceIP = at_udp.getDeviceIP();
  // //at_udp.pingIP(F("8.8.8.8"));
  // at_udp.createUDPSocket(port);
  // //at_udp.at_send(serverIP,port,2,"3132");
  // //data_input = at_udp.waitResponse();
  // //Serial.println("data_input : "+data_input);
  // return created;
}
/****************************************/
/**          Internal                  **/
/****************************************/

// ========================= SEND AND BUFFER ============================
void Magellan7020::printHEX(char *str){
  char *hstr;
  hstr=str;
  char out[3];
  memset(out,'\0',2);
  int i=0;
  bool flag=false;
  while(*hstr){
    flag=itoa((int)*hstr,out,16);
    
    if(flag){
      at_udp.printChar(out);

      if(debug){
        Serial.print(out);
      }      
    }
    hstr++;
  }
}

void Magellan7020::printmsgID(unsigned int messageID){
  char Msg_ID[3];
  
  utoa(highByte(messageID),Msg_ID,16);
  if(highByte(messageID)<16){
    if(debug) Serial.print(F("0"));
    at_udp.printContent(F("0"));
    if(debug) Serial.print(Msg_ID);
    at_udp.printChar(Msg_ID);
  }
  else{
    at_udp.printChar(Msg_ID);
    if(debug)  Serial.print(Msg_ID);
  }

  utoa(lowByte(messageID),Msg_ID,16);
  if(lowByte(messageID)<16){
    if(debug)  Serial.print(F("0"));
    at_udp.printContent(F("0"));
    if(debug)  Serial.print(Msg_ID);
    at_udp.printChar(Msg_ID);
  }
  else{
    at_udp.printChar(Msg_ID);
    if(debug)  Serial.print(Msg_ID);
  }
}

// void Magellan7020::print_pathlen(unsigned int path_len,char *init_str){   
//     unsigned int extend_len=0;

//     if(path_len>13){
//       extend_len=path_len-13;

//       char extend_L[3];
//       itoa(lowByte(extend_len),extend_L,16);
//       at_udp._Serial_print(init_str);
//       at_udp._Serial_print(F("d"));

//       if(debug) Serial.print(init_str);
//       if(debug) Serial.print(F("d"));

//       if(extend_len<=15){
//         at_udp._Serial_print(F("0"));
//         at_udp._Serial_print(extend_L);

//         if(debug) Serial.print(F("0"));
//         if(debug) Serial.print(extend_L);
//       }
//       else{
//         at_udp._Serial_print(extend_L);
//         if(debug) Serial.print(extend_L);
//       }
      

//     }
//     else{
//       if(path_len<=9){
//         char hexpath_len[2];
//         memset(hexpath_len,'\0',1);   
//         sprintf(hexpath_len,"%i",path_len);
//         at_udp._Serial_print(init_str);
//         at_udp._Serial_print(hexpath_len);
//         if(debug) Serial.print(init_str);
//         if(debug) Serial.print(hexpath_len);

//       }
//       else{
//         if(path_len==10) {
//           at_udp._Serial_print(init_str);
//           at_udp._Serial_print(F("a"));
//           if(debug) Serial.print(init_str);
//           if(debug) Serial.print(F("a"));
//         }
//         if(path_len==11){
//           at_udp._Serial_print(init_str);
//           at_udp._Serial_print(F("b"));
//           if(debug) Serial.print(init_str);
//           if(debug) Serial.print(F("b"));
//         } 
//         if(path_len==12){
//           at_udp._Serial_print(init_str);
//           at_udp._Serial_print(F("c"));
//           if(debug) Serial.print(init_str);
//           if(debug) Serial.print(F("c"));
//         } 
//         if(path_len==13){
//           at_udp._Serial_print(init_str);
//           at_udp._Serial_print(F("d"));
//           if(debug) Serial.print(init_str);
//           if(debug) Serial.print(F("d"));
//         } 
        
//       }
//    }  
// }

void Magellan7020::print_pathlen2(unsigned int path_len,String init_str)
{   
    unsigned int extend_len=0;
    if(path_len>13){
      extend_len=path_len-13;

      char extend_L[3];
      itoa(lowByte(extend_len),extend_L,16);
      at_udp.printContent(init_str);
      at_udp.printContent(F("d"));

      if(debug) Serial.print(init_str);
      if(debug) Serial.print(F("d"));

      if(extend_len<=15){
        at_udp.printContent(F("0"));
        at_udp.printChar(extend_L);


        if(debug) Serial.print(F("0"));
        if(debug) Serial.print(extend_L);
      }
      else{
        at_udp.printChar(extend_L);
        if(debug) Serial.print(extend_L);
      }
      

    }
    else{
      if(path_len<=9)
      {
        char hexpath_len[2];
        memset(hexpath_len,'\0',1);   
        sprintf(hexpath_len,"%i",path_len);
        at_udp.printContent(init_str);
        at_udp.printChar(hexpath_len);
        if(debug) Serial.print(init_str);
        if(debug) Serial.print(hexpath_len);

      }
      else
      {
        if(path_len==10) 
        {
          at_udp.printContent(init_str);
          at_udp.printContent(F("a"));
          if(debug) Serial.print(init_str);
          if(debug) Serial.print(F("a"));
        }
        if(path_len==11)
        {
          at_udp.printContent(init_str);
          at_udp.printContent(F("b"));
          if(debug) Serial.print(init_str);
          if(debug) Serial.print(F("b"));
        } 
        if(path_len==12)
        {
          at_udp.printContent(init_str);
          at_udp.printContent(F("c"));
          if(debug) Serial.print(init_str);
          if(debug) Serial.print(F("c"));
        } 
        if(path_len==13)
        {
          at_udp.printContent(init_str);
          at_udp.printContent(F("d"));
          if(debug) Serial.print(init_str);
          if(debug) Serial.print(F("d"));
        } 
        
      }
      //Serial.println("exittt");
   }  
}

// void Magellan7020::cnt_auth_len(char *auth){
//   char *hstr;
//   hstr=auth;
//   char out[3];
//   memset(out,'\0',2);
//   int i=0;
//   bool flag=false;
//   while(*hstr){
//     auth_len++;  
//     hstr++;
//   }
// }

// unsigned int Magellan7020::option_len(unsigned int path_len){
//   unsigned int len=0;
//     if (path_len>0){
//         if (path_len>13){
//           len=2;
//         }
//         else{
//           len=1;
//         }
//       }
//   return len+path_len;
// }

//void Magellan7020::print_uripath(String uripath){
//     unsigned int uripathlen=uripath.length();
//     if (uripathlen>0){
//           print_pathlen(uripathlen,"0");
          
//           char data[uripath.length()+1];
//           memset(data,'\0',uripath.length());
//           uripath.toCharArray(data,uripath.length()+1);

//           printHEX(data);
//       }
// }

void Magellan7020::print_uripath2(String uripath,String optnum)
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

// String Magellan7020::post_data(String payload,String option1,String option2,String option3){
//   unsigned int timeout[5]={2000,4000,8000,16000,36000};
//   //unsigned int timeout[5]={12000,14000,18000,26000,42000};
//   unsigned int prev_send=millis();

//   if(!get_process && en_post){
//     data_buffer="";
//     previous_send=millis();
//     send_ACK=false;
//     ACK=false;
//     success=false;
//     token=random(0,32767);
//     post_token=token;
//     if(debug) Serial.println(F("Load new payload"));
//     Msg_ID++;
//     for (byte i = 0; i <= 4; ++i){
//       post_process=true;
//       en_chk_buff=false;
//       post_ID=Msg_ID;
//       Msgsend(payload,option1,option2,option3);
//       //delay(500);
//       while(true){
//         en_chk_buff=true;
//         at_udp.waitResponse();
//         unsigned int currenttime=millis();
//         if (currenttime-previous_send>timeout[i] || success){
//           if(debug) Serial.println(currenttime-previous_send);
//           previous_send=currenttime;
//           en_post=true;
//           en_get=true;
//           post_process=false;
//           break;
//         }
//       }
//       if (success){
//         break;
//       }
//       else{
//         if(i+1<5){
//           if(printstate) Serial.print(F(">> Retransmit"));
//           if(printstate) Serial.println(i+1);
//           if(printstate) Serial.println(timeout[i+1]);        
//         }
//       }
//       if (!success){
//         Serial.print(F("timeout : "));
//         data_input="";
//         count_timeout++;
//         Serial.println(count_timeout);
//         if(printstate) Serial.println();

//         if(count_timeout>=3){
//           count_timeout = 0;
//           do{
//             at_udp.check_module_ready();
//           }
//           while(!hw_connected);
//           at_udp.reboot_module();
//           at_udp.setupModule(serverIP);
//         }       
//       }
//     }   
//   }
//   post_process=false;
//   at_udp._serial_flush();
//   return data_buffer;
// }

String Magellan7020::post_data3(String payload,option *coapOption,unsigned int totaloption)
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
      //void (*parser_callback)(String msg_data) = &miniresponse;
      Msgsend3(payload,coapOption,totaloption);
      String server=serverIP;
      while(true)
      {
        en_chk_buff=true;

        at_udp.waitResponse(data_resp,server);
        manageResponse(data_resp);

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
        //add
        count_success++;
        if(debug) Serial.print(F("Success : "));
        if(debug) Serial.println(count_success);
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
                at_udp.check_module_ready();
            }
            while(!hw_connected);
            //at_udp.setupModule(address,port);
         }
        
     }
    }

    
   
  post_process=false;
  at_udp._serial_flush();
  return rcvdata;
}

// void Magellan7020::Msgsend(String payload,String option1,String option2,String option3){

//   char data[payload.length()+1];
//   memset(data,'\0',payload.length());
//   payload.toCharArray(data,payload.length()+1);
  
//   if(en_post){

//       if(printstate) Serial.print(F(">> post: Msg_ID "));
//       if(printstate) Serial.print(Msg_ID);
//       if(printstate) Serial.print(F(" "));
//       if(printstate) Serial.print(payload);
//       Serial.println();      

//       unsigned int buff_len=auth_len+11+2+path_hex_len+payload.length(); //header 2 + 
//       unsigned int option1_len=option1.length();
//       unsigned int option2_len=option2.length();
//       unsigned int option3_len=option3.length();

//       buff_len+=option_len(option1_len);
//       buff_len+=option_len(option2_len);
//       buff_len+=option_len(option3_len);

//       at_udp.at_send(serverIP,port);
//       at_udp.at_send(String(buff_len));
//       at_udp.at_send(F(",4202"));
//       printmsgID(Msg_ID);                         // 2
//       printmsgID(post_token);                     //print token 2
//       at_udp.at_send(F("b54e42496f54"));
     
//       print_pathlen(auth_len,"0");

//       printHEX(pathauth);

//       print_uripath(option1);
//       print_uripath(option2);
//       print_uripath(option3);

//       //_Serial->print(F("1132"));                 //content-type json
//       at_udp.at_send(F("ff")); 
//       printHEX(data);       

//       if(printstate) Serial.println();
      
//   }   
// }

void Magellan7020::Msgsend3(String payload,option *coapOption,unsigned int totaloption)
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

      // if (BC95G)
      // {
      //   if (debug) Serial.println(F("BC95G"));
      //   at_udp._Serial_print(F("AT+NSOST=1,"));
      // }
      // else
      // {
      //   if (debug) Serial.println(F("BC95"));
      //   at_udp._Serial_print(F("AT+NSOST=0,"));
      // }

      //at_udp._Serial_print(serverIP);
      //at_udp._Serial_print(F(",5683,"));
      //at_udp.at_send(serverIP,"5683");

      if(debug) Serial.print(F("AT+CSOSEND=0,"));
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

      //at_udp._Serial_print(buff_len);

      at_udp.printHeader(serverIP,port,buff_len*2);

      if(debug) Serial.print(buff_len*2);
      if(debug) Serial.print(F(",4202"));

      //at_udp._Serial_print(F(",4202"));                 // 2
      at_udp.printContent(F("4202"));

      //at_udp.at_send(F(",4202"));
      //at_udp._Serial_print(F(",4002"));
      printmsgID(Msg_ID);                         // 2
      printmsgID(post_token);                     //print token 2
      //at_udp._Serial_print(F("b54e42496f54"));        
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
            init_opt=F("a");
            break;

          case 11:
            init_opt=F("b");
            break;

          case 12:
            init_opt=F("c");
            break;

          case 13:
            init_opt=F("d");
            break;

          case 14:
            init_opt=F("e");
            break;

          case 15:
            init_opt=F("f");
            break;

          default:
                init_opt=String(outopt);
          
        }

        print_uripath2(stroption2->stroption,init_opt);
        

        stroption2++;
      }

      at_udp.printContent(F("1132"));                 //content-type json 2
      if(debug) Serial.print(F("1132"));                 //content-type json 2
      if (payload.length()>0)
      {
        at_udp.printContent(F("ff")); 
        if(debug) Serial.print(F("ff"));             // 1
        printHEX(data);  
      }
      at_udp.println();
      if(printstate) Serial.println();
      
      at_udp.printContent("");
  }   
}

String Magellan7020::get_data3(option *coapoption,unsigned int totaloption,String Proxy)
{

  int timeout[5]={8000,16000,32000,64000,128000};
  rcvdata="";
  data_buffer="";
  String server=serverIP;
  if(!post_process && en_get){
    Msg_ID++;
    get_ID=Msg_ID;
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
              /*if(debug) Serial.println(String(current_time-previous_time_get));
              if(debug) Serial.println(String(success));
              if(debug) Serial.println(String(ACK && more_flag));
              if(debug) Serial.println(String(NOTFOUND));*/
              break;
            }
            at_udp.waitResponse(data_resp,server);
            manageResponse(data_resp);
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

void Magellan7020::Msgget3(option *coapOption,unsigned int totaloption,String Proxy)
{
 
  option *stroption1;
  stroption1=coapOption;

  option *stroption2;
  stroption2=coapOption;

  String Resource="";

  char data[Resource.length()+1];
  memset(data,'\0',Resource.length());
  Resource.toCharArray(data,Resource.length()+1); 
  GETCONTENT=false;
  ACK=false;

  if(printstate) Serial.print(F(">> GET data : Msg_ID "));
  if(printstate) Serial.print(Msg_ID);
  if(printstate) Serial.print(F(" "));
  if(printstate) Serial.print(Resource);
  
  /*if (BC95G)
  {
    if (debug) Serial.println(F("BC95G"));
    at_udp.printContent(F("AT+NSOST=1,"));
  }
  else
  {
    if (debug) Serial.println(F("BC95"));
    at_udp.printContent(F("AT+NSOST=0,"));
  }*/
 

  //if(debug) Serial.print(F("AT+NSOST=0,")); 


  unsigned int path_len=Resource.length();
  unsigned int buff_len=2+2+2+2;


  for(unsigned int i=0;i<totaloption;i++){
    buff_len+=stroption1->optlen+1;
    if(stroption1->optlen>13){
      buff_len+=1;
    }
    stroption1++;
  }

  if(debug) Serial.print(serverIP);
  if(debug) Serial.print(F(",5683,")); 
  at_udp.printHeader(serverIP,port,buff_len*2);

  if (debug) Serial.print(buff_len*2);
  if(debug) Serial.print(F(",4201"));
  at_udp.printContent(F("4201"));
  printmsgID(Msg_ID);
  printmsgID(get_token);                     //print token

  unsigned int lastopt=0;
  unsigned int optnum=0;
  unsigned int outopt=0;

  for(unsigned int i=0;i<totaloption;i++){ 
    optnum=stroption2->optionnum;
    outopt=optnum-lastopt;
    String init_opt;

    lastopt=optnum;

    switch(outopt){
      case 10:
        init_opt=F("a");
        break;

      case 11:
        init_opt=F("b");
        break;

      case 12:
        init_opt=F("c");
        break;

      case 13:
        init_opt=F("d");
        break;

      case 14:
        init_opt=F("e");
        break;

      case 15:
        init_opt=F("f");
        break;

      default:
            init_opt=String(outopt);
      
    }

    print_uripath2(stroption2->stroption,init_opt);

    stroption2++;
  }

  at_udp.printContent(F("8104"));      //Block size 256 recommended for BC95 with CoAP Header
  if(debug) Serial.print(F("8104"));          // 2 

  //at_udp._Serial.print(F("8105"));      //Block size 512 recommended for BC95
  //if(debug) Serial.print(F("8105"));
  //at_udp._Serial.print(F("8106"));    //Block size 1024
  //if(debug) Serial.print(F("8106"));
  

  at_udp.println();

  at_udp._serial_flush();

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
void Magellan7020::print_rsp_header(String Msgstr)
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
                          if(debug) Serial.println(F("match token get"));
                          if(debug) Serial.print(rsptoken);
                          //if(!more_flag) success=true;
                          success=true;
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
                          if(debug) Serial.println(F("match token post"));
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
      //case BAD_REQUEST: if(printstate && en_print) Serial.println(F("4.00 BAD_REQUEST"));
                    //Msgstr.remove(0, 8); 
                    //break;
      //case FORBIDDEN: if(printstate && en_print) Serial.println(F("4.03 FORBIDDEN"));
                    //Msgstr.remove(0, 8); 
                    //break;
      case NOT_FOUND: 
                    if(printstate && en_print) Serial.println(F("4.04 NOT_FOUND"));
                    GETCONTENT=false; 
                    NOTFOUND=true;
                    RCVRSP=false;
                    break;
      //case METHOD_NOT_ALLOWED: 
                    //RCVRSP=false;
                    //if(printstate && en_print) Serial.println(F("4.05 METHOD_NOT_ALLOWED")); 
                    //break;
      //case NOT_ACCEPTABLE: if(printstate && en_print) Serial.println(F("4.06 NOT_ACCEPTABLE")); 
                    //break;
      case REQUEST_ENTITY_INCOMPLETE: //if(printstate && en_print) Serial.println(F("4.08 REQUEST_ENTITY_INCOMPLETE")); 
                    break;
      case PRECONDITION_FAILED: //if(printstate && en_print) Serial.println(F("4.12 PRECONDITION_FAILED")); 
                    break;
      case REQUEST_ENTITY_TOO_LARGE: //if(printstate && en_print) Serial.println(F("4.13 REQUEST_ENTITY_TOO_LARGE")); 
                    break;
      //case UNSUPPORTED_CONTENT_FORMAT: if(printstate && en_print) Serial.println(F("4.15 UNSUPPORTED_CONTENT_FORMAT")); 
                    //break;
      case INTERNAL_SERVER_ERROR: if(printstate && en_print) Serial.println(F("5.00 INTERNAL_SERVER_ERROR")); 
                    break;
      case NOT_IMPLEMENTED: //if(printstate && en_print) Serial.println(F("5.01 NOT_IMPLEMENTED")); 
                    break;
      //case BAD_GATEWAY: if(printstate && en_print) Serial.println(F("5.02 BAD_GATEWAY")); 
                    //break;
      //case SERVICE_UNAVAILABLE: if(printstate && en_print) Serial.println(F("5.03 SERVICE_UNAVAILABLE")); 
                    //break;
      //case GATEWAY_TIMEOUT: if(printstate && en_print) Serial.println(F("5.04 GATEWAY_TIMEOUT")); 
                    //break;
      //case PROXY_NOT_SUPPORTED: if(printstate && en_print) Serial.println(F("5.05 PROXY_NOT_SUPPORTED")); 
                    //break;

      default : //Optional
                GETCONTENT=false;
   }

   if(printstate && en_print) Serial.print(F("   Msg_ID "));

   
   if(printstate && en_print) Serial.println(resp_msgID);
}

void Magellan7020::print_rsp_Type(String Msgstr,unsigned int msgID)
{
  bool en_print=(post_process && resp_msgID==post_ID) || (get_process && resp_msgID==get_ID);

  if(debug) Serial.print(F("print rsp type:"));
  if(debug) Serial.println(Msgstr);
  if(debug) Serial.println(Msgstr.indexOf(acktk));

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
void Magellan7020::manageResponse(String rx)
{

  if(rx.indexOf(F("FF"))!=-1 || rx.indexOf(F("ff"))!=-1)
  {
    miniresponse(rx);
    data_resp="";
  }
  else
  {
    //if(rx!="")
    //{
      //Serial.print("else data response is ");
      //Serial.println(rx);
    //}
  }

}

void Magellan7020::miniresponse(String rx)
{
  if(debug) Serial.print(F("RX="));
  if(debug) Serial.println(rx);
  
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

/****************************************/
/**          External                  **/
/****************************************/
void Magellan7020::get_cmdlist(){
  at_udp.get_cmdlist();
}

//------------------------ Magellan7020 3.0 -------------------
String Magellan7020::thingsregister()
{
  //Serial.print(F("imsi:"));  
  //Serial.println(imsi);
  //Serial.print(F("deviceIP:"));  
  //Serial.println(deviceIP);
  option coapoption[5];
  unsigned int totaloption=5;

  String stropt[5];

  stropt[0]=F("register");
  coapoption[0].stroption=stropt[0];
  coapoption[0].optlen=stropt[0].length();
  coapoption[0].optionnum=11;

  stropt[1]=F("sim");
  coapoption[1].stroption=stropt[1];
  coapoption[1].optlen=stropt[1].length();
  coapoption[1].optionnum=11;

  stropt[2]=F("v1");
  coapoption[2].stroption=stropt[2];
  coapoption[2].optlen=stropt[2].length();
  coapoption[2].optionnum=11;

  //char chrImsi[15+1];
  //memset(chrImsi,'\0',15);
  //imsi.toCharArray(chrImsi,15+1);

  //Serial.println(imsi);
  //Serial.println(imsi.length());

  if (imsi.indexOf(F("52003"))!=-1 and imsi.length()>15)
  {
    int indexst=imsi.indexOf(F("52003"));
    imsi=imsi.substring(indexst,16);
  }
  stropt[3]=imsi;
  //stropt[3]="520039400002365";
  
  coapoption[3].stroption=stropt[3];
  coapoption[3].optlen=stropt[3].length();
  coapoption[3].optionnum=11;

  //stropt[4]=F("10.0.0.1");
  stropt[4]=deviceIP;
  coapoption[4].stroption=stropt[4];
  coapoption[4].optlen=stropt[4].length();
  coapoption[4].optionnum=11;


  return post_data3("",coapoption,totaloption);
}

String Magellan7020::report(String payload,String Token)
{
  
  option coapoption[5];
  unsigned int totaloption=5;

  String stropt[5];

  stropt[0]=F("report");
  coapoption[0].stroption=stropt[0];
  coapoption[0].optlen=stropt[0].length();
  coapoption[0].optionnum=11;

  stropt[1]=F("sim");
  coapoption[1].stroption=stropt[1];
  coapoption[1].optlen=stropt[1].length();
  coapoption[1].optionnum=11;

  stropt[2]=F("v1");
  coapoption[2].stroption=stropt[2];
  coapoption[2].optlen=stropt[2].length();
  coapoption[2].optionnum=11;

  stropt[3]=Token;
  coapoption[3].stroption=stropt[3];
  coapoption[3].optlen=stropt[3].length();
  coapoption[3].optionnum=11;

  //stropt[4]=F("10.0.0.1");
  stropt[4]=deviceIP;
  coapoption[4].stroption=stropt[4];
  coapoption[4].optlen=stropt[4].length();
  coapoption[4].optionnum=11;

  #if defined(ARDUINO_AVR_PRO) || (ARDUINO_AVR_UNO)
    if(payload.length()>200){
      Serial.println(F("exceed payload size"));
    }
    else{
      return post_data3(payload,coapoption,totaloption);
    }
  #else
    return post_data3(payload,coapoption,totaloption);
  #endif
}

String Magellan7020::getConfig(String Resource,String Token)
{
  String return_value="";
  if (Token!="")
  {

  //String ipaddr=F("10.0.0.1");
  option coapoption[6];
  unsigned int totaloption=6;

  String stropt[6];

  stropt[0]=F("config");
  coapoption[0].stroption=stropt[0];
  coapoption[0].optlen=stropt[0].length();
  coapoption[0].optionnum=11;

  stropt[1]=F("sim");
  coapoption[1].stroption=stropt[1];
  coapoption[1].optlen=stropt[1].length();
  coapoption[1].optionnum=11;

  stropt[2]=F("v1");
  coapoption[2].stroption=stropt[2];
  coapoption[2].optlen=stropt[2].length();
  coapoption[2].optionnum=11;

  stropt[3]=Token;
  coapoption[3].stroption=stropt[3];
  coapoption[3].optlen=stropt[3].length();
  coapoption[3].optionnum=11;

  //stropt[4]=ipaddr;
  stropt[4]=deviceIP;
  coapoption[4].stroption=stropt[4];
  coapoption[4].optlen=stropt[4].length();
  coapoption[4].optionnum=11;

  stropt[5]=Resource;
  coapoption[5].stroption=stropt[5];
  coapoption[5].optlen=stropt[5].length();
  coapoption[5].optionnum=15;

  return_value=get_data3(coapoption,totaloption,"");
  }
  else
  {
    Serial.println(F("miss token"));
  }
  return return_value;

  }

String Magellan7020::getDelta(String Resource,String Token)
{

  //String ipaddr=F("10.0.0.1");
  option coapoption[6];
  unsigned int totaloption=6;

  String stropt[6];

  stropt[0]=F("delta");
  coapoption[0].stroption=stropt[0];
  coapoption[0].optlen=stropt[0].length();
  coapoption[0].optionnum=11;

  stropt[1]=F("sim");
  coapoption[1].stroption=stropt[1];
  coapoption[1].optlen=stropt[1].length();
  coapoption[1].optionnum=11;

  stropt[2]=F("v1");
  coapoption[2].stroption=stropt[2];
  coapoption[2].optlen=stropt[2].length();
  coapoption[2].optionnum=11;

  stropt[3]=Token;
  coapoption[3].stroption=stropt[3];
  coapoption[3].optlen=stropt[3].length();
  coapoption[3].optionnum=11;

  //stropt[4]=ipaddr;  // Device IP
  stropt[4]=deviceIP;
  coapoption[4].stroption=stropt[4];
  coapoption[4].optlen=stropt[4].length();
  coapoption[4].optionnum=11;

  stropt[5]=Resource;
  coapoption[5].stroption=stropt[5];
  coapoption[5].optlen=stropt[5].length();
  coapoption[5].optionnum=15;

  return get_data3(coapoption,totaloption,"");
}

//--------------- Magellan7020 1.0 -------------------------
// String  Magellan7020::post(String payload){
//   #if defined(ARDUINO_AVR_PRO) || (ARDUINO_AVR_UNO)
//     if(payload.length()>100){
//       Serial.println("Warning payload size exceed the limit of memory");
//     }
//     else{
//       return post_data(payload,"","","");
//     }
//   #else
//     return post_data(payload,"","","");
//   #endif
  
// }

