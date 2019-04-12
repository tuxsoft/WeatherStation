/*
  3D Printered WeMoss bassed weather station
 
  This is my own driver, its simple clean and easy to debug.

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
#include "TuxSoft3DP.h"

/*
 The fields I need to match up to weeWix

	'outTemp'
	'inTemp'
	'barometer'
	'pressure'
	'windSpeed'
	'windDir'
	'windGust'
	'windGustDir'
	'outHumidity'
	'inHumidity'
	'radiation'
	'UV'
	'rain'
*/


extern WiFiUDP Udp;
extern int otaMode;
char packetBuffer[4];

/*
 * The weeWx or other appliucation will generate the time stamp
 */

void txTux (float outTemp,
			float barometer,
			float windSpeed,
			float windDir,
			float windGust,
			float windGustDir,
			float outHumidity,
			float UV,
			float rain)
{
	String data = String ("{\"TS3DP\":1");				// Chech this is you want to distiguish multiple stations
	data += ",\"outTemp\":" + String (outTemp);
	data += ",\"barometer\":" + String (barometer);
	data += ",\"windSpeed\":" + String (windSpeed);
	data += ",\"windDir\":" + String (windDir);
	data += ",\"windGust\":" + String (windGust);
	data += ",\"windGustDir\":" + String (windGustDir);
	data += ",\"outHumidity\":" + String (outHumidity);
	data += ",\"UV\":" + String (UV);
	data += ",\"rain\":" + String (rain);
	data += "}";

	uint32_t bca = WiFi.localIP ();
	bca |= 0xFF000000;
	IPAddress bcast (bca);
//	Serial.println (bcast.toString ());

	Udp.beginPacketMulticast (bcast, 55555, WiFi.localIP());
	Udp.write (data.c_str(), data.length());
	Udp.endPacket ();
	Udp.flush ();
	Serial.print ("Tx.");

	otaMode = 0;
	for (int r = 0; r < 200 && otaMode == 0; r++)
	{
		int packetSize = Udp.parsePacket();
		if (packetSize)
		{
			// read the packet into packetBufffer
			Udp.read(packetBuffer, 4);
			if (packetBuffer[0] == 'O' &&
				packetBuffer[1] == 'T' &&
				packetBuffer[2] == 'A')
			{
				Serial.println ("OTA Mode..");

				Udp.beginPacketMulticast (bcast, 55551, WiFi.localIP());
				Udp.write ("{\"OTA\":\"enabled\"}", 17);
				Udp.endPacket ();
				Udp.flush ();
				otaMode = 1;
			}

			packetBuffer[0] = 0;
			packetBuffer[1] = 0;
			packetBuffer[2] = 0;
			packetBuffer[3] = 0;
		}
		delay (10);
	}
}
