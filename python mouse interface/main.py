import pyautogui as pg
import math
import numpy as np
import random
import time
import socket
import struct
import network_scan_tool


# TODO
# need auto calibration
# absolute or relative modes
# left and right click
# initial handshake


# the port to use
port = 4210
# the resolution of your monitor
screenSize = 3440,1440


packetSize = 4;

pg.MINIMUM_DURATION = 0
pg.MINIMUM_SLEEP = 0
pg.PAUSE = 0


def softQuit():
    # send end transmission flag "END"
    try:
        udp.close()
        print("Safely ended stream and closed port")
    except:
        pass
    print("Closing...")
    quit()


# find the ip of the arduino
def findArduino(port):
    remoteLocation = network_scan_tool.find_device(port)

    if remoteLocation == None:
        softQuit()

    return remoteLocation


def initUDP():
    # create udp socket and bind port
    udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp.bind(("", port))
    return udp


def map(value, leftMin, leftMax, rightMin, rightMax):
    leftSpan = leftMax - leftMin
    rightSpan = rightMax - rightMin
    valueScaled = float(value - leftMin) / float(leftSpan)
    return int(rightMin + (valueScaled * rightSpan))


def twosComp(data):
    numBits = 14
    if (data & (1 << (numBits - 1))) != 0:
        data = data - (1 << numBits)
    return data


def centreMouse():
    pg.moveTo(screenSize[0]/2, screenSize[1]/2)


def moveMouse(x,y):
    pg.moveTo(x,y)


def decodeBits(bits):
    var = struct.unpack("<BBBB", bits)

    aux2 = int( ( var[3] >> 7 ) & 0b01 )
    aux1 = int( ( var[3] >> 6 ) & 0b01 )
    r = int( ( var[3] >> 5 ) & 0b01 )
    l = int( ( var[3] >> 4 ) & 0b01 )

    y1 = var[3] & 0b1111
    y2 = var[2]
    y3 = ( var[1] >> 6 ) & 0b1111
    y = ( ( y1 << 10 ) | ( y2 << 2 ) | ( y3 ) ) #| 0b0000000000000000
    y = twosComp(y)

    x1 = var[1] & 0b00111111
    x2 = var[0]
    x = ( ( x1 << 8 ) | ( x2 ) ) #| 0b0000000000000000
    x = twosComp(x)

    # if using arduino IMU data /90
    # y /= 90
    # x /= 90

    decoded = (x,y,l,r,aux1,aux2)
    return decoded


def main():

    vals = [None] * 3
    leftClicked = False
    rightClicked = False

    remoteIP,remotePort = findArduino(port)

    # create UDP socket
    udp = initUDP()

    # send begin transmission flag "BGN"
    udp.sendto("BGN".encode(), (remoteIP, port))
    print("Starting UDP stream")

    try:
        while True:
            data, addr = udp.recvfrom(packetSize)
            x,y,lc,rc,aux1,aux2 = decodeBits(data)
            print(x,y,lc,rc,aux1,aux2)

            #xPos = map(x,0,4095,0,screenSize[0])
            #yPos = map(y,0,4095,0,screenSize[1])
            #moveMouse(xPos,yPos)

            pg.moveRel((x)/100,(y)/100)

            if(lc and not leftClicked):
                pg.mouseDown(button='left')
                leftClicked = True
            elif(not lc and leftClicked):
                pg.mouseUp(button='left')
                leftClicked = False
            else:
                pass

            if(rc and not rightClicked):
                pg.mouseDown(button='right')
                rightClicked = True
            elif(not rc and rightClicked):
                pg.mouseUp(button='right')
                rightClicked = False
            else:
                pass


    except KeyboardInterrupt:
        softQuit()


if __name__ == '__main__':
    main()


softQuit()
