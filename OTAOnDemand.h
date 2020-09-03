#pragma once
#ifndef OTAONDEMAND
#define OTAONDEMAND

#if defined(ESP8266)
	#include <ESP8266HTTPClient.h>
	#include <ESP8266httpUpdate.h>
#elif defined(ESP32)		
	// TODO
#endif	

class OTAOnDemand
{

public:
	// default constructor
	OTAOnDemand();
	// default destructor
	~OTAOnDemand();


	OTAOnDemand(int firmwareVersion, String firmwareServerUrl, bool isDebug = false);

	// Performs OTA on demand update using firmware files in a local server
	void checkForUpdates();

	// Returns current Firmware version
	int getFirmwareVersion();

	// Returns current Firmware server URL
	String getFirmwareServerUrl();


private:
	int	 	              m_firmwareVersion;
	String  	          m_firmwareServerUrl;
	bool				  m_isDebug;
	
	// Gets MAC address in a format for OTA on demand updates
	// returns
	//   String MAC Address
	String getMAC();

};

#endif