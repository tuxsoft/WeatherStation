# 3D Printed Weather Station

A WeMoss based 3D printed WiFi weather station

![TuxSoft Weather Station](https://github.com/tuxsoft/WeatherStation/images/station.png)

I have been meaning to build a weather station for a few years then recently I saw i
Dan Bemowski's https://www.thingiverse.com/thing:2757369

This got me inspired, and I used his mounting idea and designed a new weather station from
scratch. This also gave me a chance to play with the ESP8266 Arduino integration, I like it !

I have created my own simple JSON format, I like the KISS approach !

Included is a driver for weeWX, TuxSoft3DP to install it download the zip file and use the command

`wee_extensions --install tuxsoft3dp.zip`

I have choosen to also implement Weatherflow V40 protocol since that should provide ready to use
software solutions, in particular I am investigating weeWx.

![WeatherFlow](https://weatherflow.github.io/SmartWeather/api/udp/v40/)

![weeWx](http://www.weewx.com/)

![Weatherflow Driver for weeWx by Arthur Emerson](https://github.com/captain-coredump/weatherflow-udp)
To use this driver D2 must be pulled LOW 

## Hardware:

 * 1 x Stainless or Brass M5 30mm bolt
 * 1 x Brass M5 50mm bolt
 * 2 x M5 Nuts and washers to match above
 * 2 x 608ZZ ceramic "spinner" bearings.
 * 2 x 6mm x 3mm  neodymium magnets (round)
 * 1 x 3144 Hall effect sensor board Eg. KY-003
 * 1 x BPI-3C1-05 IR Photo Interirupter (Slotted Optical Switch)
 * 1 x 10K resistor
 * 1 x GY-271 Magnometer -- QMC5883L not Adafruit version
 * 1 x BMP280 I2C Barometer Module
 * 1 x DHT22 Humidity Sensor
 * 1 x ML8511 UV sensor
 * 1 x WeMos D1 Esp-Wroom-02 with integrated 18650 battery
 * 1 x 220K resistor (See note A)
 * 1 x 100K resistor (See note A)
 * 1 x 18650 battery
 * 1 x Small DIY solar panel https://www.aliexpress.com/item/Solar-Panel-5V-6V-12V-Mini-Solar-System-DIY-For-Battery-Cell-Phone-Chargers-Portable-Solar/32823762479.html

 I chose the 12V 1.5W and plan to use a buck if needed, it has not arrived yet so can only tell
 once Chinese product arrives what you actually get :)

 Modifications, I resoldered the hall sensor to be close to the PCB and replaced the connectors
 with silicone wires directly into the sensor. The rain guage is small and a tight fit. You also
 need to ensure that the tipper activated on one side and not the other. 

 You can "weaken" your magnet in the tipper if needed by applying heat from a soldering iron for
 a few seconds. Play it by ear and get a few magnets.

 On the tipper make sure you cover your magnet and sensor board with silicone sealant, this is
 required to prevent corosion.
 
 ML8511 -- WARNING !!! READ THIS !!! (See note A)

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
 built into the board for the analogue input. Like all things from China this must be verified before assumtion.


## 3D Printing

 When printing some filaments are semi transparent to IR light, so printing the anemometer encoder
 wheel can be a problem, I used so aluminium tape on the top of the encoder wheel.


 You need to enable supports, NB When you assemble the weather vane, one of the neodymium magnets must
 be placed in the cone then the support rod inserted. The orientation will determin the calibration of
 the Weather Cock (vane)

## Weather Proofing

 This is DIY, silicone sealant and Rust-O-Leam Rule, common sense is a must.

## Calibration

 To be done

## Misc Notes

 It is *very important* not to mount the rain gauge to close to the weather vane as the magnet in the tipper
 will affect the magnometer used by the weather vane. By the same token using ceramic bearings and brass nuts
 and bolts are the preferred materials.
 
 Removing those cheap and nasty steel "pins" in the sensors and replacing them with good quality copper wires
 might also be a benefit.
 
 Most importantly this is a fun project for me, its my first experience in weather stations of any kind and
 as such no warranties of any sort are made for anything.. nada but that said I hope it proves reliable and useful.
 
