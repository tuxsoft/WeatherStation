#!/usr/bin/env python
# Copyright 2018 Ross Linder, rcprivate62@gmail.com
# Thanks to Arthur Emerson, vreihen@yahoo.com I used his driver as a
# laerning point for this.
#
# Distributed under the terms of the GNU Public License (GPLv3)
# See the README for links to Arthur's work above

import time
import weewx.units
import weewx.drivers
import syslog
import threading

import sys, getopt
import socket
import json

# Driver Info
DRIVER_VERSION = "1.00"
HARDWARE_NAME = "TuxSoft3DP"
DRIVER_NAME = 'TuxSoft3DP'

def loader(config_dict, engine):
    return TuxSoft3DPDriver(**config_dict[DRIVER_NAME])

def logmsg(level, msg):
    syslog.syslog(level, 'tuxsoft3dp: %s: %s' %
                  (threading.currentThread().getName(), msg))

def logdbg(msg):
    logmsg(syslog.LOG_DEBUG, msg)

def loginf(msg):
    logmsg(syslog.LOG_INFO, msg)

def logerr(msg):
    logmsg(syslog.LOG_ERR, msg)

def sendMyLoopPacket (raw):
    packet = dict()
    if 'TS3DP' in raw:
        data = json.loads (raw)
        # I ignore the station number for now
        packet = {'dateTime' : time.time(), 'usUnits' : weewx.METRICWX,
                  'outTemp' : data['outTemp'],
                  'barometer' : data['barometer'],
                  'windSpeed' : data['windSpeed'],
                  'windDir' : data['windDir'],
                  'windGust' : data['windGust'],
                  'windGustDir' : data['windGustDir'],
                  'outHumidity' : data['outHumidity'],
                  'UV' : data['UV'],
                  'rain' : data['rain']}

    return packet


class TuxSoft3DPDriver (weewx.drivers.AbstractDevice):

    def __init__(self, **stn_dict):
        loginf ('driver version is %s' % DRIVER_VERSION)
        self.bcast = stn_dict.get('bcast', '0.0.0.0')
        self.port = int(stn_dict.get('port', 55555))

    def hardware_name(self):
        return HARDWARE_NAME


    def genLoopPackets(self):
        loginf ('Listenng for packets on port %s' % (self.port))

        sock = socket.socket (socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        sock.setsockopt (socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
        sock.bind ((self.bcast, self.port))
        sock.settimeout (30)
#        sock.setblocking(0)

        while True:
            try:
                data,addr = sock.recvfrom (1024)
            except socket.error as e:
#                logerr ('UDP error' % e)
                pass
            else:
                if len(data) > 0 :
                    loginf (data);
                    rep = sendMyLoopPacket (data)
                    if len (rep) > 2 :
                        yield rep
