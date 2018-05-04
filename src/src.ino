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

  Why is this file called src.ino --  because the Arduino IDE like it that way if
  your sub directory is called src

  Design considerations:

  Choice not to use EEPROM to save averages, I mean things never crash right and if
  they do we can't change the weather.

  I choose to use weaterflow because it looks like a clean and simple interface, hopefully
  more software support will be available. Looking at weeWx right now. One thing they lack
  is internal temp, I will add some custom messages to get different status from the station
  for my own use, its pretty simple when using JSON

  Rain: .. r=34mm Area = 0.03637932 M2  so 36.38ml = 1mm of rain 
           Tipper takes 1.9ml to tip so each tip is 0.052mm
*/

#define TIP_VOL	0.052

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <time.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>

#include "WeatherCock.h"
#include "TuxSoft3DP.h"
#include "WeatherFlow.h"

DHT dht (D5, DHT22);
Adafruit_BMP280 bmp;	// I2C version
WeatherCock vane;

int lastP = 0;
// Global weather parms sent by post to server
int windTime = 0;
int windDirection = 0;
int rainTipper = 0;
int sampleRainTipper = 0;
int tipState = 0;
float humidity = 80.0;
int uvIndex = 0;
int celcius = 20;
int barometer = 771;

#include "ssid.h"
WiFiUDP Udp;


/*
 Interrupt handler for the anemometer
 */

void windTriggerH ()
{
	static long summer = 0;
	static unsigned char cntr = 0;

	int t = millis ();
	if (t - lastP < 10)
	{
		return;
	}

	summer = summer + (t - lastP);
	lastP = t;
	if (++cntr == 10)
	{
		windTime = summer / 10;
		summer = 0;
		cntr = 0;
	}
}

/*
 Get the time from network NTP server, my server is hard coded
 */

void getTime ()
{
	configTime (0,/*2 * 3600*/ 0, "192.168.1.254", "pool.ntp.org");
	while (!time(nullptr))
	{
		Serial.print ("t");
		delay(500);
	}
}

//From: http://forum.arduino.cc/index.php?topic=3922.0
float mapfloat (float x, float in_min, float in_max, float out_min, float out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void setup()
{
	Wire.begin ();
	Serial.begin (115200);
	vane.begin ();

	pinMode (D2, INPUT_PULLUP);		// Mode + TuxSoft3DP  :  - Weatherflow
	pinMode (D7, INPUT_PULLUP);
	pinMode (D6, INPUT);

	WiFi.mode (WIFI_STA);
	WiFi.begin (ssid, passwd);

	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print ("c.");
	}

	getTime ();

	// GMT+2
	Serial.println("");
	time_t now = time (nullptr);
	Serial.println (now);
	Serial.println (ctime (&now));

	Udp.begin (50223);

	if (!bmp.begin())
	{
		Serial.println ("Can't init BMP280 Barometer");
	}

	attachInterrupt (digitalPinToInterrupt(D7), windTriggerH, FALLING);
	tipState = digitalRead (D6);
}

/*
 Calculate the wind speed applying a calibration curve
 */

float windSpeed (int ms)
{
	if (ms == 0)
	{
		return 0;
	}

	// Probably need a calibration curve, but for now assume 100% good
	// Anemometer diameter is 492mm

	return 492.0 / ms;
}


void loop()
{
	static int langle = -1;
	static int lwt = 0;
	static int ltmp = 0;
	static int windLull = 0;
	static int windAvg = 0;
	static int windGust = 0;
	static int windHSample = 0;
	static int windLSample = 0;
	static int lmin = -1;
	static int lsec = -1;

	// Tux#DP parms
	static float txWindGust = 0;
	static float txWindGustDir = 0;

	int d6 = digitalRead (D6);
	if (tipState != d6)
	{
		tipState = d6;
		sampleRainTipper++;
		if (rainTipper++ == 0)
		{
			Serial.println ("rainStartEvent");
			rainStartEvent ();
		}
	}

	int tmp = vane.temp();

	int16_t x,y,z;
	if (vane.readXYZ (&x,&y,&z) == 0)
	{
		if (x == 0 && y == 0 && z == 0)
		{
			Serial.println ("** No Data **");
		}
		delay (50);
		return;
	}

	float heading = atan2(x, y); // Result is in radians
	if(heading < 0)    heading += 2*PI;  // Correct for when signs are reversed.
	if(heading > 2*PI) heading -= 2*PI;  // Correct for when heading exceeds 360-degree, especially when declination is included
	int angle = int(heading * 180/M_PI); // Convert radians to degrees for more a more usual result

	if (langle != angle)
	{
		langle = angle;
		Serial.print("Heading (degrees): "); 
		Serial.println(angle);
	}
	if (ltmp != tmp)
	{
		Serial.print ("Temp: ");
		Serial.println (tmp);
		ltmp = tmp;
		Serial.print ("BMP280 Temp: ");
		Serial.println (celcius);
	}
	if (windTime != 0 && lwt != windTime)
	{
		if (windTime - lwt > 100)
		{
			txWindGust = windSpeed (windTime);
			txWindGustDir = angle;
			if (digitalRead (D2) == LOW)
			{
				rapidWind (windSpeed (windTime), angle);
			}
		}
		lwt = windTime;
		return;
	}
	if (millis() - lastP > 5000 && windTime != 0)
	{
		windTime = 0;
		Serial.println ("SWind: 0");
	}

	time_t now = time (nullptr);
	struct tm *timeinfo = localtime (&now);

	// Update parms
	if (txWindGust < windSpeed (windHSample))
	{
			txWindGust = windSpeed (windHSample);
			txWindGustDir = angle;
	}
	if (timeinfo->tm_sec % 20== 0 && lsec != timeinfo->tm_sec)	// Tux data @ 20 seconds
	{
		// Read the current UV Index
		int mlv = analogRead (A0);
		uvIndex = mapfloat ((mlv/1023.0)*3.3, 0.99, 2.8, 0.0, 15.0); //Convert the voltage to a UV intensity level

		humidity = dht.readHumidity();
		if (isnan (humidity))
		{
			humidity = 12.34;	// Junk
		}
		if (digitalRead (D2) == HIGH)
		{
			txTux (ltmp, /*bmp.readTemperature (),*/		// TODO: put back once sensor arrives
				   bmp.readPressure (),
				   windSpeed (windAvg),
				   angle,
				   txWindGust,
				   txWindGustDir,
				   humidity,
				   uvIndex,
				   rainTipper * TIP_VOL);
		}

		txWindGust = 0;
	}


	if (timeinfo->tm_min % AIR_INTERVAL == 0 && timeinfo->tm_sec == 0 && lmin != timeinfo->tm_min)
	{
		barometer = bmp.readPressure ();
		celcius = bmp.readTemperature ();
		// Humidity done by Tux

		if (digitalRead (D2) == LOW)
		{
			observationAir ();
		}
	}

	if (timeinfo->tm_sec % WIND_AVG == 0 && lsec != timeinfo->tm_sec)
	{
		if (windGust < windHSample)
		{
			windGust = windHSample;
		}
		if (windLull > windLSample)
		{
			windLull = windLSample;
		}
		windHSample = 60000;
		windLSample = 0;
	}
	else
	{
		if (windTime > windLSample)
		{
			windLSample = windTime;
		}
		if (windTime < windHSample && windTime != 0)
		{
			windHSample = windTime;
		}
	}
	windAvg = (windAvg + windTime) / 2;

	if (timeinfo->tm_min % SKY_INTERVAL == 0 && timeinfo->tm_sec == 0 && lmin != timeinfo->tm_min)
	{

		if (digitalRead (D2) == LOW)
		{
			observationSky (rainTipper * TIP_VOL, sampleRainTipper * TIP_VOL,
							windSpeed (windLull), windSpeed (windAvg), windSpeed (windGust));
		}
		windGust = 0;
		windLull = 0;
		windHSample = 60000;
		windLSample = 0;
		windAvg = 0;
		sampleRainTipper = 0;
	}

	if (timeinfo->tm_hour == 0 && timeinfo->tm_min == 0 && timeinfo->tm_sec == 0 && lsec != 0)
	{
		rainTipper = 0;
		getTime ();		// Refresh clock 
	}
	lsec = timeinfo->tm_sec;
	lmin = timeinfo->tm_min;

	delay(250);
}
