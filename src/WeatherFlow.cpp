/*
  3D Printered WeMoss bassed weather station
 
  See: https://weatherflow.github.io/SmartWeather/api/udp/v40/
 
  Copyright (c) 2018 Ross Linder. All right reserved.
 
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

  I decided not to use a JSON class to save space.
*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <time.h>
#include "WeatherFlow.h"

extern void enableWiFi ();

// Global weather parms sent by post to server
extern int windDirection;
extern int humidity;
extern int uvIndex;
extern int celcius;
extern int barometer;

extern WiFiUDP Udp;

static const char *intro = "{\"serial_number\":\"WS001\",\"hub_sn\":\"HB001\",";


static void sendPacket (const char *data, int dlen)
{
	uint32_t bca = WiFi.localIP ();
	bca |= 0xFF000000;
	IPAddress bcast (bca);
//	Serial.println (bcast.toString ());

	Udp.beginPacketMulticast (bcast, 50222, WiFi.localIP());
	Udp.write (data, dlen);
	Udp.endPacket ();
	Udp.flush ();
	Serial.print ("wTx.");
}


void rainStartEvent ()
{
	if ((WiFi.status() != WL_CONNECTED))
	{
		return;
	}

	String data = String (intro);
	data += "\"type\":\"evt_precip\",\"evt\":[";
	data += String (time (nullptr));
	data += "]}";
	sendPacket (data.c_str(), data.length());
}


void rapidWind (float speed, int direction)
{
	if ((WiFi.status() != WL_CONNECTED))
	{
		return;
	}

	String data = String (intro);
	data += "\"type\":\"rapid_wind\",\"ob\":[";
	data += String (time (nullptr));
	data += "," + String (speed) + "," + String (direction);
	data += "]}";
	sendPacket (data.c_str(), data.length());
}

void observationAir ()
{
	if ((WiFi.status() != WL_CONNECTED))
	{
		return;
	}

	String data = String (intro);
	data += "\"type\":\"obs_air\",\"obs\":[[";
	data += String (time (nullptr));
	data += "," + String (barometer) + "," + String (celcius) + "," + String (humidity) + ",0,0,0," + String (AIR_INTERVAL);
	data += "]],\"firmware_revision\":1}";
	sendPacket (data.c_str(), data.length());
}

void observationSky (float srain, float arain, float windLull, float windAvg, float windGust)
{
	if ((WiFi.status() != WL_CONNECTED))
	{
		return;
	}

	String data = String (intro);
	data += "\"type\":\"obs_sky\",\"obs\":[[";
	data += String (time (nullptr));
	data += ",0," + String (uvIndex) + "," + String (srain) + ",";
	data += String (windLull) + "," + String (windAvg) + "," + String (windGust) + "," + String (windDirection);
	data += ",null," + String (SKY_INTERVAL) + ",null," + String (arain) + ",";
	data += (arain == 0) ? "0" : "1";
	data += "," + String (WIND_AVG);
	data += "]],\"firmware_revision\":1}";

	sendPacket (data.c_str(), data.length());
}

