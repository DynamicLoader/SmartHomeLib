/*
	File : SmartHomeClient.h
	Description: SmartHome MQTT interface defination
	Author: Lucy2003
	Last Modified: 2022/1/28

*/

#ifndef __SMARTHOME_CLIENT_H__
#define __SMARTHOME_CLIENT_H__

#include <stdint.h>

namespace SmartHome{

	class Adapter{
		public:
			 Adapter(){}
			 Adapter(const char* host,uint16_t port,const char* user,const char* pwd){}
			 virtual ~Adapter(){}
			
			/* 	Network functions */

			//Will message Qos=0,Retained=1;
			virtual bool connect(const char* willTopic,const uint8_t* willMsg,size_t len)=0;
			virtual bool isConnected()=0;
			virtual bool disconnect()=0;
			
			//Subscribe Qos=1
			virtual bool subscribe(const char* topic)=0;
			virtual bool unsubscribe(const char* topic)=0;
			
			virtual bool publish(const char* topic,const uint8_t* msg,size_t len,bool retained)=0;
			
			virtual uint32_t getUptimeNetwork()=0;
			virtual uint32_t getUptimeMQTT()=0;
			
			// len must be set to current msg length
			virtual bool getMsg(char* topic,uint8_t* msg,size_t& len)=0;

			/*  System functions */
		
	};
	
	
}

#endif