## TuxSoft3DP Udp weeWX extension
Copyright 2018, Ross Linder rcprivate62@gmail.com

Learnt how to do this thanks to the [!WeatherFlow driver for weeWx](https://github.com/captain-coredump/weatherflow-udp)
by Arthur Emerson, vreihen@yahoo.com

Distributed under terms of the GPLv3

# Installation

sudo wee_extension --install tuxsoft3dp.zip 

# Removal

sudo wee_extension --uninstall TuxSoft3DP


# Low down

The TuxSoft 3D Printed weather stations emits a UDP packet every 20 seconds
to your local network, this driver gathers that data and presents it to weeWX
!! NB !! Remeber if D2 is LOW then the Weather Station will emit WeatherFlow data instead
and you should use Arthurs driver.

The sensormap for Arthurs driver is

	[[sensor_map]]
		outTemp = air_temperature.WS001.obs_air
		outHumidity = relative_humidity.WS001.obs_air
		pressure = station_pressure.WS001.obs_air
		windSpeed = wind_speed.WS001.rapid_wind
		windDir = wind_direction.WS001.rapid_wind
		UV = uv.WS001.obs_sky
		rain = rain_accumulated.WS001.obs_sky
		radiation = solar_radiation.WS001.obs_sky


