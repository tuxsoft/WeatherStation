# 3D Printed Weather Station

A WeMos based 3D printed WiFi weather station

![TuxSoft Weather Station](https://github.com/tuxsoft/WeatherStation/images/station.png)

I have been meaning to build a weather station for a few years then recently I saw i
Dan Bemowski's https://www.thingiverse.com/thing:2757369

This got me inspired, and I used his mounting idea and designed a new weather station from
scratch. This also gave me a chance to play with the ESP8266 Arduino integration, I like it !

I have created my own simple JSON format, I like the KISS approach !

Included is a driver for weeWX, TuxSoft3DP to install it download the zip file and use the command

*`wee_extensions --install tuxsoft3dp.zip`*

I have choosen to also implement Weatherflow V40 protocol since that should provide ready to use
software solutions, in particular I am investigating weeWx.

![WeatherFlow](https://weatherflow.github.io/SmartWeather/api/udp/v40/)

![weeWx](http://www.weewx.com/)

![Weatherflow Driver for weeWx by Arthur Emerson](https://github.com/captain-coredump/weatherflow-udp)
To use this driver D3 must be pulled LOW 

## Software:

The main components is the embedded firmware in the Wemoss, in addition there is are a few simple python
modules that comprise of the weewx driver (TuxSoft3DP) and a simple GUI based Weather monitor that provides
a wind direction indicator and some basic information that is updated as received by the station. The other
module is a simple UDP packet monitor primarily used for debugging and to detect when OTA mode is enabled.

The Wemoss code is compiled in with the Arduino (1.8.9) IDE, things that you might like to modify are..

 * Everything
 * INVERT_VANE if you end up installing your magnet backwartds (see src.ino)
 * Static or DHCP IP (see src.ino)
 * The hostname for OTA (see src.ino look for ESP.getChipId )
 * WindSpeed() its not tested just trivially calculated (see src.ino)
 * The volume of your tipper, it might be different to mine after printing and sealing.

Originally I had decided to send weather data every 20 seconds and not turn the WiFi off, this in my
case consumed approximately 70mA continually since in reality the solar panel only gives 120ma in full
sun the recharge rate was not enough to sustain the system. I changed the code to send data only every
2 minutes and to sleep the WiFi modem in between. This reduced the power to 33mA and spikes it up to 70mA
for a few seconds while data is being transferred. This technique is great but for OTA to work it is a PIA.

To overcome this problem I sacrificed a few seconds of power after each packet transmission and spend a
a few seconds looking for a UDP request to enable OTA, if found OTA is enabled for 60 seconds.

**OTA HOWTO**

You need Python and netcat then open two terminal windows along with your Arduino IDE ready, In one terminal 
run *`udp_test.py`*  in the other terminal have the following command primed and ready to go
*`echo -n OTA | nc -u -w0 192.168.1.40 55550`*  on windows ignore -n

Next whemn you see a weather packet received immediately send the OTA with netcat, you should then see a
second packet that confirms OTA was enabled. Now send the update via OTA using the Arduino IDE

You can see this procedure ![on YouTube](https://youtu.be/7__c9c8BN8w)

## Hardware:

 * 1.5M 15mm copper pipe (class 1 or better)
 * 1 x Elbow 90Deg 15mm copper
 * 5 x 15mm copper equal T junction
 * 3 x 15mm copper straight junctions
 * 3 x stainless hose clamps to fit 15mm pipe
 * Some method to mount the 15mm tube to your desired location.
 * 3 x Stainless M3x6 self tapping screws for rain gauge
 * 12 x Stainless M3x10 self tapping screws for shield
 * 1 x Stainless or Brass M5 30mm bolt
 * 1 x Brass M5 50mm bolt
 * 2 x M5 Nuts and washers to match above
 * 2 x 608ZZ ceramic "spinner" bearings.
 * 2 x 6mm x 3mm  neodymium magnets (round)
 * 1 x 3144 Hall effect sensor board Eg. KY-003
 * 1 x BPI-3C1-05 IR Photo Interrupter (Slotted Optical Switch)
 * 1 x 10K resistor
 * 1 x GY-271 Magnetometer -- QMC5883L not Adafruit version
 * 1 x BMP280 I2C Barometer Module
 * 1 x DHT22 Humidity Sensor
 * 1 x ML8511 UV sensor
 * 1 x WeMos D1 Esp-Wroom-02 with integrated 18650 battery
 * 1 x 220K resistor (See note A)
 * 1 x 100K resistor (See note A)
 * 1 x 18650 battery
 * 1 x Small DIY solar panel https://www.aliexpress.com/item/Solar-Panel-5V-6V-12V-Mini-Solar-System-DIY-For-Battery-Cell-Phone-Chargers-Portable-Solar/32823762479.html
 * 1 x buck360 or similar to regulate solar to 5V

 I chose the 12V 1.5W and plan to use a buck if needed, it has not arrived yet so can only tell
 once Chinese product arrives what you actually get :)

 Modifications, I re-soldered the hall sensor to be close to the PCB and replaced the connectors
 with silicone wires directly into the sensor. The rain gauge is small and a tight fit. You also
 need to ensure that the tipper activated on one side and not the other. 

 You can "weaken" your magnet in the tipper if needed by applying heat from a soldering iron for
 a few seconds. Play it by ear and get a few magnets.

 On the tipper make sure you cover your magnet and sensor board with silicone sealant, this is
 required to prevent corrosion.
 
 **ML8511 -- WARNING !!! READ THIS !!! (See note A)**

 The ESP8266 ADC range is from zero to one volt, the output from the ML8511 will be to 3.3V so
 a resitor divider is needed to get it into range as follows ..


  ESP-A0 ----+---[220K]-----< ML8511 
             |
			 +---[100K]-----< GND


 You will also need to solder the 10K pullup resistor to the BPI-3C1-05 between 3.3V and D7 on the ESP.
 I did this and added a 3 pin header then hot glued it to tack it into position. Epoxy was used to permanently
 mount the IR switch.

 You will need to cobble up all cables to fit your mounting, in the images folder I have pics of what I did.

## Note A

 I have not yet received the WeMoss with battery, but from what I can tell there is already a 220K/100K divider
 built into the board for the analogue input. Like all things from China this must be verified before assumption.
 Update,indeed the divider was Included on the board.


## 3D Printing

 When printing some filaments are semi transparent to IR light, so printing the anemometer encoder
 wheel can be a problem, I used so aluminium tape on the top of the encoder wheel.


 You need to enable supports, NB When you assemble the weather vane, one of the neodymium magnets must
 be placed in the cone then the support rod inserted. The orientation will determine the calibration of
 the Weather Cock (vane)

## Weather Proofing

 This is DIY, silicone sealant and Rust-O-Leam Rule, common sense is a must.

## Calibration

 The rain gauge radius is 34mm so its area is 0.03637932 M2, this gives 36.38 ml as the volume
 that equates to 1mm of rain. I measure my tipper and it takes 1.9mm to tip thus each tip is
  0.05227 mm of rain, 0.52 therefor is my chosen value. Field testing will confirm if its close enough.

 The small size of the gauge reduces its accuracy but I think it is good enough to measure trends, also
 I imagine the high winds we often get affect a tipper as well.

## Misc Notes

 It is *very important* not to mount the rain gauge to close to the weather vane as the magnet in the tipper
 will affect the manometer used by the weather vane. By the same token using ceramic bearings and brass nuts
 and bolts are the preferred materials.
 
 Removing those cheap and nasty steel "pins" in the sensors and replacing them with good quality copper wires
 might also be a benefit.
 
 Most importantly this is a fun project for me, its my first experience in weather stations of any kind and
 as such no warranties of any sort are made for anything.. nada but that said I hope it proves reliable and useful.
 
# The mounting assembly

 Cut the copper pipe int the following lengths and label them accordingly..
 
 * A - 200mm
 * B - 220mm
 * C - 100mm
 * D - 120mm
 * E -  50mm
 * F - 100mm
 * G -  50mm
 * H - 150mm
 * I -  30mm

 The rest of the pipe you will use  to attach to the mount you desire.

 Slot the 3 straight junctions on one side as shown, this will allow you to clamp them to the printed
 mounting points.

 Start by soldering the unslotted ends of the junctions, one to each pipe labeled **A**, **C**, and **F**

 Beh .. look at the images and fabricate the mounts, take care to orientate your solar panel correctly in
 relation to the weather vane

