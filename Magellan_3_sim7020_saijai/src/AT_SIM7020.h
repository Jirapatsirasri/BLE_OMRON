
//AT Command Dictionary for SIMCOM SIM7020
#include <Arduino.h>
#include <Stream.h>


struct pingRESP{
	bool status;
	String addr;
	String ttl;
	String rtt;
};

typedef void (*MQTTClientCallback)(String &topic, String &payload);
typedef void (*reponseCallback)(String &datax);

class AT_SIM7020{
public:
	AT_SIM7020();
	bool debug;

	//============ callback ==================
	reponseCallback callback_p;
	MQTTClientCallback MQcallback_p;

	//==============Initial===================
	void setupModule(String port,String address);
	void setupModule();
	void check_module_ready();	
    void reboot_module();
    void pingIP(String IP);
	bool closeUDPSocket();
	bool checkNetworkConnection();
	bool attachNetwork(String port,String address);

    //==========Get value from module=========
    String getFirmwareVersion();
	String getIMEI();
    String getIMSI();
    String getSignal();
    String getDeviceIP();
    void get_cmdlist();
    String getAPN();
    String retTopic;
  	String retPayload;

    //==========Data send/rec.================
    //void waitResponse(void (*callback_func)(String msg_data));
    int RegisCallback(reponseCallback callbackFunc);
    void waitResponse(String &retdata,String server);
    void printHeader(String address,String port,unsigned int len);
    void printContent(String payload);
    void printChar(char *payload);
    void printInt(int payload);
    void println();
    void sendMsgHEX(String address,String port,String payload);
    void sendMsgSTR(String address,String port,String payload);
    void send_msg(String address,String port,unsigned int len,String payload);
    void at_getBuffer(String socket,String nBuffer);
    
    String read_model();
    void _Serial_print(String input);
    void _Serial_print(unsigned int data);
    void _Serial_println();
	void _serial_flush();

	//============= MQTT =====================
	void setupMQTT(String server,String port,String clientID,String username,String password);
	void publish(String topic,String payload);
	void subscribe(String topic);
	void MQTTresponse();
	int RegisMQCallback(MQTTClientCallback callbackFunc);
    
private:
	//==============Buffer====================
	String data_input;
	String data_buffer;

	//==============Flag======================
	bool hw_connected=false;
	bool en_get=true;
	bool en_post=true;
	bool end=false;
	bool en_chk_buff=true;

	//==============Parameter=================
	unsigned int previous_check=0;
	unsigned long previous=0;
	bool en_rcv=false;

	//============Counter value===============
	byte k=0;
	byte cnt_rcv_resp=0;
	//==============Function==================
	void echoOff();
	bool setPhoneFunction();
	void connectNetwork();
	bool createUDPSocket(String port,String address);
	//void read_model();
	void set_PSM_report(unsigned int n);
	void manageResponse(String &retdata,String server);
	//void send_msg(String address,String port,String payload);
	bool enterPIN();
	void printHEX(char *str);
	void hex2ascii(String data_payload);

protected:
	Stream *_Serial;
	
};