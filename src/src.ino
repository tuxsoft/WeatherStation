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

  I have choosen to use a simple JSON based protocol (TuxSoft3DP) you can also enable
  weatherflow if you ground D3 .. I decided to support weaterflow because it looks like
  a clean and simple interface, hopefully more software support will be available.
  
  Looking at weeWx right now. One thing they lack is internal temp, I will add some
  custom messages to get different status from the station for my own use, its pretty
  simple when using JSON

  Rain: .. r=34mm Area = 0.03637932 M2  so 36.38ml = 1mm of rain 
           Tipper takes 1.9ml to tip so each tip is 0.052mm
*/

// Your vane tip magnet orientation N or S might require this
#define INVERT_VANE	1
#define TIP_VOL	0.052	// Might be worth calibrating this

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <time.h>
#include <BME280I2C.h>
#include <DHT.h>

#include "WeatherCock.h"
#include "TuxSoft3DP.h"
#include "WeatherFlow.h"

#define STATIC_IP	1
#define MY_IP IPAddress(192,168,1,40)
#define MY_DNS IPAddress(192,168,1,254)
#define MY_GW IPAddress(192,168,1,254)

BME280I2C::Settings settings (
	BME280::OSR_X1,
	BME280::OSR_X1,
	BME280::OSR_X1,
	BME280::Mode_Forced,
	BME280::StandbyTime_1000ms,
	BME280::Filter_Off,
	BME280::SpiEnable_False,
	BME280I2C::I2CAddr_0x77 // I2C address. I2C specific.
);

BME280I2C bmp(settings);

DHT dht (D5, DHT22);
WeatherCock vane;

int otaMode = 0;
int lastP = 0;
// Global weather parms sent by post to server
int windTime = 0;
int windDirection = 0;
int rainTipper = 0;
int sampleRainTipper = 0;
int tipState = 0;
float humidity = 80.0;
float uvIndex = 0;
float celcius = 20;
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

void stopWiFi ()
{
	delay (2000);	// Let things finish

	WiFi.mode (WIFI_OFF);
	WiFi.forceSleepBegin ();
    delay (10);
	Serial.println ("WiFi - OFF");
}

void startWiFi ()
{
	WiFi.forceSleepWake ();
    delay (10);
	float dbm = 0;
	WiFi.setOutputPower (0.0);
	WiFi.mode (WIFI_STA);
	WiFi.begin (ssid, passwd);

	unsigned char loopCnt = 0;
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print ("c.");
		if (++loopCnt == 10)
		{
			loopCnt = 0;
			if (++dbm < 20)
			{
				WiFi.setOutputPower (dbm);
			}
			else
			{
				dbm = 0;
			}
		}
	}
	Serial.print ("WiFi - PWR ");
	Serial.println (dbm);
}



void setup()
{
	Wire.begin ();
	Serial.begin (115200);
	Serial.println ("TuxSoft3DP Weather Ver 1.0");
	vane.begin ();

	pinMode (D3, INPUT_PULLUP);		// Mode + TuxSoft3DP  :  - Weatherflow
	pinMode (D7, INPUT_PULLUP);
	pinMode (D6, INPUT);

	dht.begin ();

#if STATIC_IP
	WiFi.config (MY_IP, MY_DNS, MY_GW);
#endif

	startWiFi ();

	// Setup for OTA
	if (ESP.getChipId() == 0x1330CC)
	{
		ArduinoOTA.setHostname ("weather");
	}
	else
	{
		ArduinoOTA.setHostname ("weather-dev");
	}

	ArduinoOTA.onStart ([](){ Serial.println("Start"); });
	ArduinoOTA.onEnd ([]() { Serial.println("\nEnd"); });

	ArduinoOTA.onProgress ([](unsigned int progress, unsigned int total)
	{
		Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
	});
	
	ArduinoOTA.onError([](ota_error_t error)
	{
		Serial.printf("Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
		else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
		else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
		else if (error == OTA_END_ERROR) Serial.println("End Failed");
	});
	ArduinoOTA.begin();


	getTime ();

	// GMT+2
	Serial.println("");
	Serial.printf("Chip ID = %08X\n", ESP.getChipId());
	time_t now = time (nullptr);
	Serial.println (now);
	Serial.println (ctime (&now));

	Udp.begin (55550);

	if (!bmp.begin())
	{
		Serial.print ("Can't ");
	}
	Serial.println ("Init BMP280 Barometer");

	// Don't change this the wxweather driver converts to preference
//	BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
//	BME280::PresUnit presUnit(BME280::PresUnit_Pa);

	attachInterrupt (digitalPinToInterrupt(D7), windTriggerH, FALLING);
	tipState = digitalRead (D6);

	stopWiFi ();
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

	// K = 4.8

	return 1000 / ms * 4.8;
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

	static float pres, temp, dummy;

	// Tux3DP parms
	static float txWindGust = 0;
	static float txWindGustDir = 0;

	int d6 = digitalRead (D6);
	if (tipState != d6)
	{
		tipState = d6;
		sampleRainTipper++;
		if (rainTipper++ == 0)
		{
			if (digitalRead (D3) == LOW)
			{
				Serial.println ("rainStartEvent");
				rainStartEvent ();
			}
		}
	}

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
#if INVERT_VANE
	angle = 360 - angle;
#endif

	if (langle != angle)
	{
		langle = angle;
		Serial.print("Heading (degrees): "); 
		Serial.println(angle);
	}
	// Just for info
	if (ltmp != (int)celcius)
	{
		Serial.print ("Temp: ");
		Serial.println (celcius);
		ltmp = celcius;
		Serial.print (" Bmp: ");
		Serial.println (pres);
	}
	if (windTime != 0 && lwt != windTime)
	{
		if (windTime - lwt > 100)
		{
			txWindGust = windSpeed (windTime);
			txWindGustDir = angle;
			if (digitalRead (D3) == LOW)
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
//	if (timeinfo->tm_sec % 20 == 0 && lsec != timeinfo->tm_sec)	// Tux data @ 20 seconds
	if (timeinfo->tm_min % 2 == 0 && lmin != timeinfo->tm_min)	// Tux data @ 2 min
	{
		startWiFi ();

		bmp.read (pres, temp, dummy);
		if (!isnan(temp))
		{
			celcius = temp;
			barometer = pres;
		}

		// Read the current UV Index
		int mlv = analogRead (A0);
		for(int a = 0 ; a < 7 ; a++)
		{
			mlv += analogRead (A0);
		}

		uvIndex = mapfloat (((float)mlv * 3.3)/(8*1023.0), 0.99, 2.8, 0.0, 15.0); //Convert the voltage to a UV intensity level
		Serial.print("mlv: "); 
		Serial.print(mlv);
		Serial.print(" UV: "); 
		Serial.println(uvIndex);

		// Only update a valid value
		float tfloat = dht.readHumidity();
		if (!isnan (tfloat))
		{
			humidity = tfloat;
		}
		Serial.print("Humidity: "); 
		Serial.println(humidity);

		if (digitalRead (D3) == HIGH)
		{
			txTux (celcius,
				   barometer,
				   windSpeed (windAvg),
				   angle,
				   txWindGust,
				   txWindGustDir,
				   humidity,
				   uvIndex,
				   rainTipper * TIP_VOL);
		}

		txWindGust = 0;

		if (otaMode != 0)	// Try OTA for 1 Min
		{
			int retry;
			for (retry = 0; retry < 600; retry++)
			{
				ArduinoOTA.handle();
				delay (100);
			}
			otaMode = 0;
		}

		stopWiFi ();
	}


	if (timeinfo->tm_min % AIR_INTERVAL == 0 && timeinfo->tm_sec == 0 && lmin != timeinfo->tm_min)
	{
		bmp.read (pres, temp, dummy);
		celcius = temp;
		barometer = pres;
		if (isnan (celcius))
		{
			celcius = 0;
			barometer = 0;
		}
		// Humidity done by Tux

		if (digitalRead (D3) == LOW)
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

		if (digitalRead (D3) == LOW)
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
		startWiFi ();
		getTime ();		// Refresh clock 
		stopWiFi ();
	}
	lsec = timeinfo->tm_sec;
	lmin = timeinfo->tm_min;

	delay(250);
}
