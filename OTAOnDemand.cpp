#include "OTAOnDemand.h"

OTAOnDemand::OTAOnDemand() {
	m_firmwareVersion = 1;
	m_firmwareServerUrl = "";
	m_isDebug = false;
}

OTAOnDemand::~OTAOnDemand() {
}

OTAOnDemand::OTAOnDemand(int firmwareVersion, String firmwareServerUrl, bool isDebug) {
	m_firmwareVersion = firmwareVersion;
	m_firmwareServerUrl = firmwareServerUrl;
	m_isDebug = isDebug;
}



//*******************************************************************************
/// On demand Self Updating OTA
//*******************************************************************************
void OTAOnDemand::checkForUpdates() {
	// Each device has its own MAC Address
	// The server will have one folder per device with format MACAddress
	// Inside the folder, there will be a latest.version file which contains
	// a single 32bit integer, nothing else
	// This will be used to compare current version and locate the new 
	// firmaware we need to update to.
	// NOTE: the new Sketch should set the getFirmwareVersion() to match, otherwise
	// we will be in a cycle updating each time it boots.

	// Example: http://192.168.254.16/FirmwareOTA/0000d3fdff3f/latest.version
	// Content: 1001
	// http://192.168.254.16/FirmwareOTA/0000d3fdff3f/0000d3fdff3f-1000.bin
	// http://192.168.254.16/FirmwareOTA/0000d3fdff3f/0000d3fdff3f-1001.bin

	String mac = getMAC();
	String fwURL = String( getFirmwareServerUrl() );
	fwURL.concat( "/" );
	fwURL.concat( mac );
	String fwVersionURL = fwURL;
	fwVersionURL.concat( "/latest.version" );

	if (m_isDebug) {
		Serial.println( "Checking for firmware updates." );
		Serial.print( "MAC address: " );
		Serial.println( mac );
		Serial.print( "Firmware version URL: " );
		Serial.println( fwVersionURL );
	}

	HTTPClient httpClient;
	httpClient.begin( fwVersionURL );
	int httpCode = httpClient.GET();
	if( httpCode == 200 ) {
		String newFWVersion = httpClient.getString();

		if (m_isDebug) {
			Serial.print( "Current firmware version: " );
			Serial.println( getFirmwareVersion() );
			Serial.print( "Available firmware version: " );
			Serial.println( newFWVersion );
		}

		int newVersion = newFWVersion.toInt();

		if( newVersion > getFirmwareVersion() ) {
			if (m_isDebug) Serial.println( "Preparing to update" );

			// Looking for Firmware file with format MACAddress-Version.bin
			String fwImageURL = fwURL;
			fwImageURL.concat( "/" );
			fwImageURL.concat( mac );
			fwImageURL.concat( "-" );
			fwImageURL.concat( newFWVersion );
			fwImageURL.concat( ".bin" );
			if (m_isDebug) Serial.println( "Using firmware file " +  fwImageURL);
			t_httpUpdate_return ret = ESPhttpUpdate.update( fwImageURL );

			switch(ret) {
				case HTTP_UPDATE_FAILED:
				Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
				break;

				case HTTP_UPDATE_NO_UPDATES:
				Serial.println("HTTP_UPDATE_NO_UPDATES");
				break;
			}
		}
		else {
			Serial.println( "Already on latest version" );
		}
	}
	else {
		if (m_isDebug){
			Serial.print( "Firmware version check failed, got HTTP response code " );
			Serial.println( httpCode );
		}
	}
	httpClient.end();
}



int OTAOnDemand::getFirmwareVersion()
{
	return m_firmwareVersion;
}

String OTAOnDemand::getFirmwareServerUrl()
{
	return m_firmwareServerUrl;
}


//*******************************************************************************
/// getMAC for OTA
//*******************************************************************************
String OTAOnDemand::getMAC()
{
	byte mac[6];
	WiFi.macAddress(mac);
	char result[14];

	snprintf(result, sizeof(result), "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	return String(result);
}
