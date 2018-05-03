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
*/


#ifndef __WEATHER_COCK__
#define __WEATHER_COCK__

/*
 * So this POC has given me such a hard time until I found a data sheet that works
 * the old English name "WeatherCock" seems very apt !
 *
 * http://img.filipeflop.com/files/download/Datasheet-QMC5883L-1.0%20.pdf
 *
 * Writen in 2018 by R. Linder 
 * To the extent possible under law, the author have dedicated all copyright and related
 * and neighboring rights to this software to the public domain worldwide.
 * This software is distributed without any warranty.
 */

#include "Arduino.h"
#include <Wire.h>

#define DOUTX_REGL	0x00	// [7:0]
#define DOUTX_REGM	0x01	// [15:8]

#define DOUTY_REGL	0x02
#define DOUTY_REGM	0x03

#define DOUTZ_REGL	0x04
#define DOUTZ_REGM	0x05

#define DRDY_REG	0x06		// _ _ _ _ _ DOR OVL DRDY

#define TEMP_DREGL	0x07
#define TEMP_DREGM	0x08

#define CNTRL_REG1	0x09		// OSR:OSR RNG:RNG ODR:ODR MODE:MODE
#define CNTRL_REG2	0x0A		// SOFT_RST ROL_PNT _ _ _ _ _ INT_ENB
#define PRD_REG		0x0B		// SET/RESET Period FBR [7:0]

#define MODE_STANDBY	0
#define MODE_CONT		1

#define ODR_10HZ		(0 << 2)
#define ODR_50HZ        (1 << 2)
#define ODR_100HZ       (2 << 2)
#define ODR_200HZ       (3 << 2)

#define RNG_2G          (0 << 4)
#define RNG_8G          (1 << 4)

#define OSR_512         (0 << 6)
#define OSR_256         (1 << 6)
#define OSR_128         (2 << 6)
#define OSR_64          (3 << 6)

#define DRDY_OK			1
#define DRDY_OVL		2
#define DRDY_DOR		4

class WeatherCock
{
public:

	WeatherCock (uint8_t address=0x0d);	// Default addr

	void begin (uint8_t osr=OSR_512, uint8_t rng=RNG_8G, uint8_t odr=ODR_10HZ, uint8_t mode=MODE_CONT);
	void reset();

	uint8_t readXYZ (int16_t* x,int16_t* y,int16_t* z);

	int temp (int base=35);			// Probably temp in the china factory 


private:

	void setReg (uint8_t Reg,uint8_t val);
	uint8_t m_Addr = 0x0d;

};

#endif
