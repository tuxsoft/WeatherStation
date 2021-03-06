#ifndef __TUXSOFT3DP_H__
#define __TUXSOFT3DP_H__

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

*/


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


void txTux (float outTemp,
			float barometer,
			float windSpeed,
			float windDir,
			float windGust,
			float windGustDir,
			float outHumidity,
			float UV,
			float rain);

#endif
