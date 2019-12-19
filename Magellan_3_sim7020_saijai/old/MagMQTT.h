#ifndef MagMQTT_h
#define MagMQTT_h


#include <Arduino.h>
#include <Stream.h>


#define MODE_STRING 0
#define MODE_STRING_HEX 1


#if defined( __ARDUINO_X86__)
    #define ATLSOFTSERIAL 0
#elif defined(ARDUINO_NUCLEO_L476RG)
    #define ATLSOFTSERIAL 0
#elif defined(ARDUINO_AVR_UNO) || (ARDUINO_AVR_MEGA2560)
	#define ATLSOFTSERIAL 1
#else 
    #define ATLSOFTSERIAL 1
#endif 

#if ATLSOFTSERIAL 
	#include "AltSoftSerial.h"
#endif

typedef void (*MQTTClientCallback)(String &topic, String &payload);
static MQTTClientCallback callback_p = NULL;

class MagMQTT
{

public:

	MagMQTT();
	bool debug;
	unsigned char *server;
	unsigned char *port;
	unsigned char *username;
	unsigned char *password;
	unsigned char *clientID;


	bool begin(char *auth);
	void publish(String topic,String payload);
	void subscribe(String topic);
	
	int RegisCallback(MQTTClientCallback MQTT_func);
	
	void Loop();



private:
	String data_input;
	bool hw_connected=false;
	unsigned int previous_check=0;
	bool end;
	unsigned int k;


	

	void setupMQTT();
	void reboot_module();
	void setup_module();
	void check_module_ready();
	
	void _serial_flush();
	void printchar(char *str);

protected:
	 Stream *_Serial;
};



#endif