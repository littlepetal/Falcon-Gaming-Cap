import pyautogui as pg
import math
import numpy as np
import random
import time
import socket
import struct

# this script first sends "Hello" to the arduino
# it then listens for a UDP stream containing 3 16bit values
# first value is x with range (0-4095), then y (0-4095), then leftClicked (0 or 1)
# the mouse is then controlled based on the x and y values


# TODO
# need auto calibration
# absolute or relative modes
# left and right click
# initial handshake
# data protocol


# the IP of the arduino
remoteIP = "10.0.0.26"
# the port to use
port = 4210
# the resolution of your monitor (for scaling so it can work on any screen size)
screenSize = 3440,1440


udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
udp.bind(("", port))

#packetSize = 16
packetSize = 4;
udpPacketBitField = [14,14,1,1,1,1];


pg.MINIMUM_DURATION = 0  # Default: 0.1
pg.MINIMUM_SLEEP = 0  # Default: 0.05
pg.PAUSE = 0  # Default: 0.1


def translate(value, leftMin, leftMax, rightMin, rightMax):
    # Figure out how 'wide' each range is
    leftSpan = leftMax - leftMin
    rightSpan = rightMax - rightMin
    # Convert the left range into a 0-1 range (float)
    valueScaled = float(value - leftMin) / float(leftSpan)
    # Convert the 0-1 range into a value in the right range.
    return int(rightMin + (valueScaled * rightSpan))


def centreMouse():
    pg.moveTo(screenSize[0]/2, screenSize[1]/2)


def decodeBits(bits):
    var = struct.unpack("<BBBB", bits)

    aux2 = ( var[3] >> 7 ) & 0b01
    aux1 = ( var[3] >> 6 ) & 0b01
    r = ( var[3] >> 5 ) & 0b01
    l = ( var[3] >> 4 ) & 0b01

    y1 = var[3] & 0b1111
    y2 = var[2]
    y3 = ( var[1] >> 6 ) & 0b1111
    y = ( y1 << 10 ) | ( y2 << 2 ) | ( y3 )

    x1 = var[1] & 0b00111111
    x2 = var[0]
    x = ( x1 << 8 ) | ( x2 )

    decoded = (x,y,l,r,aux1,aux2)

    return decoded


udp.sendto("Hello".encode(), (remoteIP, port))

vals = [None] * 3
leftClicked = False

try:
    while True:
        data, addr = udp.recvfrom(packetSize)
        #data = data.decode()
        #data = data.split("\r\n")[0]
        #y,x,lc = data.split(",")
        x,y,lc,rc,aux1,aux2 = decodeBits(data)
        x = int(x)
        y = int(y)
        lc = int(lc)

        xCal = x - 1826 #subtract the bias
        yCal = y - 1834

        print(xCal,yCal,lc,rc,aux1,aux2)

        #xPos = translate(x,0,4095,0,screenSize[0])
        #yPos = translate(y,0,4095,0,screenSize[1])
        #pg.moveTo(xPos,yPos)

        pg.moveRel((xCal)/100,(yCal)/100)

        if(lc and not leftClicked):
            pg.mouseDown(button='left')
            leftClicked = True
        elif(not lc and leftClicked):
            pg.mouseUp(button='left')
            leftClicked = False
        else:
            pass


except KeyboardInterrupt:
    pass


udp.close()
