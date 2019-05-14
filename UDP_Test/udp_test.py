#!/usr/bin/python3
# Multicast client
# Adapted from: http://chaos.weblogs.us/archives/164

import socket
import json


ANY = "0.0.0.0" 
MCAST_ADDR = "224.0.0.7"
MCAST_PORT = 55555

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)

# Allow multiple sockets to use the same PORT number
sock.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)

# Bind to the port that we know will receive multicast data
sock.bind((ANY,MCAST_PORT))

# Tell the kernel that we want to add ourselves to a multicast group
# The address for the multicast group is the third param
#status = sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, socket.inet_aton(MCAST_ADDR) + socket.inet_aton(ANY))

sock.settimeout (2)

while 1:
    try:
        data, addr = sock.recvfrom(1024)
    except socket.error as e:
        pass
    else:
        if (len(data) > 0):
            if b'OTA' in data:
                print ('********************************************************> OTA ')

            if b'TS3DP' in data:
                print ("From: ", addr)
                print ("TuxSoft3DP Data: ", data)
                pkt = json.loads (data)
                print ("outTemp", pkt["outTemp"])
                print ("windSpeed", pkt["windSpeed"])
                print ("windDir", pkt["windDir"])
                print ("windGust", pkt["windGust"])
            else:
#                print ("From: ", addr)
                print ("Raw Data: ", data)

