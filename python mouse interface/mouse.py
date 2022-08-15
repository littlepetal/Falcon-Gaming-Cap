import pyautogui as pg
import math
import numpy as np
import random
import time
import socket

# TODO
# need auto calibration
# absolute or relative modes
# left and right click
# initial handshake
# data protocol

remoteIP = "10.0.0.26"
remotePort = 4210

udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
udp.bind(("", 4210))

packetSize = 16

screenSize = 3440,1440

# Any duration less than this is rounded to 0.0 to instantly move the mouse.
pg.MINIMUM_DURATION = 0  # Default: 0.1
# Minimal number of seconds to sleep between mouse moves.
pg.MINIMUM_SLEEP = 0  # Default: 0.05
# The number of seconds to pause after EVERY public function call.
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


def moveRand(dist):
    pg.moveRel(random.randint(-1*dist,dist),random.randint(-1*dist,dist))


def moveInCircle(radius):
    pg.moveRel(0,-radius)
    for i in range(100*360):
        pg.moveRel(5*math.cos(np.deg2rad(i)),5*math.sin(np.deg2rad(i)))


def jitter():
    for i in range(100000):
        print(i)
        moveRand(1)


def plotSin():
    pg.moveTo(0,screenSize[1]/2)
    for x in range(screenSize[0]):
        time.sleep(0.001)
        y = screenSize[1]/2 + math.sin(x/500)*(screenSize[1]/2)
        pg.moveTo(x,y)


udp.sendto("Hello".encode(), (remoteIP, remotePort))

vals = [None] * 3
leftClicked = False

try:
    while True:
        data, addr = udp.recvfrom(packetSize)
        data = data.decode()
        data = data.split("\r\n")[0]
        y,x,lc = data.split(",")
        x = int(x)
        y = 4096-int(y)
        lc = int(lc)

        #xPos = translate(x,0,4095,0,screenSize[0])
        #yPos = translate(y,0,4095,0,screenSize[1])
        #pg.moveTo(xPos,yPos)

        print((x-1827),(y-2275))
        pg.moveRel((x-1827)/100,(y-2275)/100)

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
