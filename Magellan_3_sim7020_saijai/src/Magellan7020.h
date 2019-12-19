#ifndef Magellan7020_h
#define Magellan7020_h


#include <Arduino.h>
//#include <Stream.h>
#include "AT_SIM7020.h"


  const byte maxretrans=4;	

  //-------Msg Type------------
  const char con[]="40";
  const char contk[]="42";
  const char non_con[]="50";
  const char ack[]="60";
  const char acktk[]="62";
  const char rst[]="70";

  //-------Method Type---------
  //const char EMPTY[]="00";
  const char GET[]="01";
  const char POST[]="02";
  const char PUT[]="03";
  const char DELETE[]="04";


enum rspcode {
	EMPTY=00,
	CREATED=65,
	DELETED=66,
	VALID=67,
	CHANGED=68,
	CONTENT=69,
	CONTINUE=95,
	BAD_REQUEST=128,
	FORBIDDEN=131,
	NOT_FOUND=132,
	METHOD_NOT_ALLOWED=133,
	NOT_ACCEPTABLE=134,
	REQUEST_ENTITY_INCOMPLETE=136,
	PRECONDITION_FAILED=140,
	REQUEST_ENTITY_TOO_LARGE=141,
	UNSUPPORTED_CONTENT_FORMAT=143,
	INTERNAL_SERVER_ERROR=160,
	NOT_IMPLEMENTED=161,
	BAD_GATEWAY=162,
	SERVICE_UNAVAILABLE=163,
	GATEWAY_TIMEOUT=164,
	PROXY_NOT_SUPPORTED=165
};

typedef struct option {
    String stroption;
    unsigned int optlen;
    unsigned int optionnum;
} option; 

class Magellan7020
{


public:
	Magellan7020();

	bool debug;
	//bool default_server;
	bool printstate=true;
	void get_cmdlist();
    //################# Magellan Platform ########################
    //String msg="";
    bool begin();
    String post(String payload);
    String get(String Resource);
    String thingsregister();
    String report(String payload,String Token);
    String getConfig(String Resource,String Token);
    String getDelta(String Resource,String Token);
    //String get_dashboard(String Resource);
    //String get_api(String Resource,String Proxy);
    //String linenotify(String token,String msg);



private:

	char *pathauth;
	unsigned int Msg_ID=0;
	//################### Buffer #######################
	String data_input;
	//String data_buffer;
	//String send_buffer;
	String rcvdata="";
	String imei="";
	String imsi="";
	String deviceIP;
	String model;
	//String LastError="";
	//String LastPort="";
	String data_buffer="";
	String data_resp="";
	//################### counter value ################
	byte k=0;
	//################## flag ##########################
	bool breboot_flag=false;
	bool end=false;
	bool send_NSOMI=false;
	bool flag_rcv=true;
	bool en_get=true;
	bool en_post=true;
	bool en_send=true;
	bool en_chk_buff=false;
	bool getpayload=false;
	bool Created=false;
	bool sendget=false;
	bool NOTFOUND=false;
	bool GETCONTENT=false;
	bool RCVRSP=false;
	bool success=false;
	bool connected=false;
	bool get_process=false;
	bool post_process=false;
	bool ACK=false;
	bool EMP=false;
	bool send_ACK=false;
	bool more_flag=false;
	bool hw_connected=false;
    bool flag_start=false;
    bool flag_end=false;
    bool BC95G=false;
    //bool cnt_rcv_resp=false;
    //bool more_flag=false;
	//################### Parameter ####################
	//unsigned int resp_cnt=0;
	unsigned int resp_msgID=0;
	unsigned int post_ID=0;
	unsigned int post_token=0;
	unsigned int get_ID=0;
	unsigned int get_token=0;
	unsigned int auth_len=0;
	unsigned int path_hex_len=0;
	//-------------------- timer ----------------------
	unsigned int previous_send=0;
	unsigned int previous_check=0;
	unsigned long previous=0;
	unsigned int wait_time=5;
	unsigned int previous_time_get=0;
	//unsigned int last_msgid=0;
	unsigned int token=0;
	unsigned int rsptoken=0;
	unsigned int count_timeout=0;
	//################### Diagnostic & Report ###########
	byte maxtry=0;
	byte cnt_retrans=0;
	byte cnt_retrans_get=0;
	byte cnt_error=0;
	byte cnt_cmdgetrsp=0;
	byte cnt_rcv_resp=0;
	//String LastError="";
	//###################################################

	//################# Function #######################
	void setup_module();
	void check_module_ready();
	void _serial_flush();
    void printHEX(char *str);
    void printmsgID(unsigned int messageID);
    void cnt_auth_len(char *auth);
	void print(char *str);
	void print_rsp_header(String Msgstr);
	void print_rsp_Type(String Msgstr,unsigned int msgID);
	unsigned int option_len(unsigned int path_len);
	void print_pathlen2(unsigned int path_len,String init_str);
	void print_uripath(String uripath);
	void print_uripath2(String uripath,String optnum);

	void manageResponse(String rx);
	void miniresponse(String rx);
	String post_data(String payload,String option1,String option2,String option3);
	String post_data3(String payload,option *coapOption,unsigned int totaloption);
	String get_data(String Resource,String Proxy);
	String get_data3(option *coapoption,unsigned int totaloption,String Proxy);

	void Msgsend(String payload,String option1,String option2,String option3);
	void Msgsend3(String payload,option *coapOption,unsigned int totaloption);
	void Msgget(String Resource,String Proxy);
	void Msgget3(option *coapOption,unsigned int totaloption,String Proxy);
	void print_pathlen(unsigned int path_len,char *init_str);
	//void  waitResponse();
	String packmsg(char coaptype[],char method[],unsigned int msgid,unsigned int msgtoken,char *options,String payload);

	void callback(String &datax);
	
protected:
	 Stream *_Serial;
};

extern Magellan7020 nbiotv4;

#endif