#!/usr/bin/python3
# Multicast client
# Adapted from: http://chaos.weblogs.us/archives/164

import socket
import json


ANY = "0.0.0.0" 
MCAST_ADDR = "224.0.0.7"
MCAST_PORT = 55555
WMCAST_PORT = 55551

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
wsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)

# Allow multiple sockets to use the same PORT number
sock.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
wsock.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)

# Bind to the port that we know will receive multicast data
sock.bind((ANY,MCAST_PORT))
wsock.bind((ANY,WMCAST_PORT))

# Tell the kernel that we want to add ourselves to a multicast group
# The address for the multicast group is the third param
status = sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, socket.inet_aton(MCAST_ADDR) + socket.inet_aton(ANY))

status = wsock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, socket.inet_aton(MCAST_ADDR) + socket.inet_aton(ANY))

# setblocking(0) is equiv to settimeout(0.0) which means we poll the socket.
# But this will raise an error if recv() or send() can't immediately find or send data. 
#sock.setblocking(0)
#wsock.setblocking(0)
# Rather use a timeout to reduce CPU usage
sock.settimeout (30)
sock.settimeout (30)

while 1:
    try:
        data, addr = sock.recvfrom(1024)
    except socket.error as e:
        pass
    else:
        if (len(data) > 0) :
            print ("From: ", addr)
            print ("TuxSoft3DP Data: ", data)
            pkt = json.loads (data)
            print ("outTemp", pkt["outTemp"])
            print ("windSpeed", pkt["windSpeed"])
            print ("windDir", pkt["windDir"])
            print ("windGust", pkt["windGust"])

    try:
        data, addr = wsock.recvfrom(1024)
    except socket.error as e:
        pass
    else:
        print ("From: ", addr)
        print ("OTA Mode ? : ", data)
