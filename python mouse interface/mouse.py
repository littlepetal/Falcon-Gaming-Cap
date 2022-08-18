import pyautogui as pg
import math
import numpy as np
import random
import time
import socket
import struct
import network_scan_tool

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



# the port to use
port = 4210
# the resolution of your monitor (for scaling so it can work on any screen size)
screenSize = 3440,1440
#packetSize = 16
packetSize = 4;
udpPacketBitField = [14,14,1,1,1,1];


def safeClose():
    # send end transmission flag "END"
    try:
        udp.close()
        print("Safely ended stream and closed port")
    except:
        pass
    print("Closing...")
    quit()


# find the ip of the arduino
remoteLocation = network_scan_tool.find_device(port)

if remoteLocation == None:
    safeClose()

remoteIP,remotePort = remoteLocation
if remotePort != port:
    print(f'Error: remote port {remotePort} does not match local port {port}')
    safeClose()


# create udp socket and bind port
udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
udp.bind(("", port))


pg.MINIMUM_DURATION = 0  # Default: 0.1
pg.MINIMUM_SLEEP = 0  # Default: 0.05
pg.PAUSE = 0  # Default: 0.1


def map(value, leftMin, leftMax, rightMin, rightMax):
    leftSpan = leftMax - leftMin
    rightSpan = rightMax - rightMin
    valueScaled = float(value - leftMin) / float(leftSpan)
    return int(rightMin + (valueScaled * rightSpan))


def centreMouse():
    pg.moveTo(screenSize[0]/2, screenSize[1]/2)


def moveMouse(x,y):
    pg.moveTo(x,y)


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



def main():

    vals = [None] * 3
    leftClicked = False

    # send begin transmission flag "BGN"
    udp.sendto("BGN".encode(), (remoteIP, port))
    print("Starting UDP stream")

    try:
        while True:
            data, addr = udp.recvfrom(packetSize)
            x,y,lc,rc,aux1,aux2 = decodeBits(data)

            x = int(x)
            y = int(y)
            lc = int(lc)

            xCal = x - 1826 #subtract the bias
            yCal = y - 1834

            print(xCal,yCal,lc,rc,aux1,aux2)

            #xPos = map(x,0,4095,0,screenSize[0])
            #yPos = map(y,0,4095,0,screenSize[1])
            #moveMouse(xPos,yPos)

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
        safeClose()


main()


safeClose()
