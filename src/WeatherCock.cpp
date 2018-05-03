/*
  3D Printered WeMoss bassed weather station
 
  So this magnometer POC has given me such a hard time until I found a data sheet
  that works the old English name "WeatherCock" seems very apt !
 
  See: http://img.filipeflop.com/files/download/Datasheet-QMC5883L-1.0%20.pdf
 
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

#include <Arduino.h>
#include "WeatherCock.h"


WeatherCock::WeatherCock (uint8_t address)
{
	m_Addr = address;
}

void WeatherCock::begin (uint8_t osr, uint8_t rng, uint8_t odr, uint8_t mode)
{
	reset ();
	setReg (PRD_REG, 0x01);		// Recomended 

	setReg (CNTRL_REG1, (osr | rng | odr | mode));
}

void WeatherCock::reset ()
{
	// SOFT_RST ROL_PNT _ _ _ _ _ INT_ENB
	setReg (CNTRL_REG2, 0x80);
	delay (8);
}

uint8_t WeatherCock::readXYZ (int16_t* x,int16_t* y,int16_t* z)
{
	Wire.beginTransmission (m_Addr);
	Wire.write (DRDY_REG);
	Wire.endTransmission ();

	uint8_t drdy = Wire.read();

	if (!(drdy & DRDY_OK) || !(drdy & DRDY_OVL))
	{
		*x = 0;
		*y = 0;
		*z = 0;
		return 0;
	}

	Wire.beginTransmission (m_Addr);
	Wire.write (DOUTX_REGL);		// Will read all data regs sequentially
	Wire.endTransmission ();

	Wire.requestFrom (m_Addr, 6);
	*x = Wire.read ();
	*x |= Wire.read () << 8;

	*y = Wire.read ();
	*y |= Wire.read () << 8;

	*z = Wire.read();
	*z |= Wire.read () << 8; 

	return 1;
}

/*
 * Its so sketchy that I belive this is correct
 */

int WeatherCock::temp (int base)
{
	Wire.beginTransmission (m_Addr);
	Wire.write (TEMP_DREGL);
	Wire.endTransmission ();

	int16_t t;
	Wire.requestFrom (m_Addr, 2);
	t = Wire.read ();
	t |= Wire.read () << 8;

	return base+(float)(int)t /100.0;
}


void WeatherCock::setReg (uint8_t no, uint8_t val)
{
	Wire.beginTransmission (m_Addr);
	Wire.write (no);
	Wire.write (val);
	Wire.endTransmission ();
}
