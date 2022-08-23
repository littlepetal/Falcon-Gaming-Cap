import pyautogui as pg
import network_scan_tool
import UDPstream
import threading
import time
import keyboard
#import gui
#import math


# TODO
# need auto calibration <- should be easy
# initial handshake <- needs to be more robust
# stream start/stop <- need to add udp rx and command parser on arduino side
# update rate and packet loss detection (1.5x packet time before declared lost?, implement via timeout?)

# arduino side: LED status indicator

# AHHHHH!?
pg.FAILSAFE = False

# the port to use
port = 4210
# the resolution of your monitor(s) (if using multiple, simply add the x resolution together)
# screenSize = 3440,1440
screenSize = 1920,1080
# relative or absolute mouse control modes ('abs'/'rel')
#change*
# mode = 'rel'
mode = 'abs'

# range of data from ardiuno
xRange = -30, 30
yRange = -30,30
# xRange = -1820,1820
# yRange = -1820,1820

# size of the compressed packets in bytes
packetSize = 4;

# need these to reduce mouse movement delay
pg.MINIMUM_DURATION = 0
pg.MINIMUM_SLEEP = 0
pg.PAUSE = 0

# mouse button pressed flags (allows for click and drag)
leftClicked = False
rightClicked = False


xOffset = 0
yOffset = 0

# safely kills the UDP stream and closes the program
def softQuit(udp = None):
    # send end transmission flag "END"
    if udp:
        try:
            UDPstream.killStream(udp, remoteIP, port)
            print("Safely ended stream and closed port")
        except:
            print("Error: unable to kill UDP stream")

    print("Closing...")

    #gui.close_app()
    #app[0].quit()
    quit()


# find the ip of the arduino using network scan tool
def findArduino(port):
    remoteLocation = network_scan_tool.find_device(port)

    if remoteLocation == None:
        softQuit()

    return remoteLocation

# maps value from one range to another
def map(value, leftMin, leftMax, rightMin, rightMax):
    leftSpan = leftMax - leftMin
    rightSpan = rightMax - rightMin
    valueScaled = float(value - leftMin) / float(leftSpan)
    return int(rightMin + (valueScaled * rightSpan))

# centres the mouse on your screen(s)
def centreMouse():
    pg.moveTo(screenSize[0]/2, screenSize[1]/2)


def recalibrateMouse(x, y):
    global xOffset
    global yOffset

    xOffset = x
    yOffset = y
    pg.moveTo(screenSize[0]/2, screenSize[1]/2)
    

# moves the mouse (absolute coords)
def moveMouse(x,y):
    pg.moveTo(x,y)

def doMouseControl(x,y,lc,rc):

    global leftClicked
    global rightClicked
    global mode

    # absolute mode
    if mode == 'abs':
        xPos = map(x,xRange[0],xRange[1],0,screenSize[0])
        yPos = map(y,yRange[0],yRange[1],0,screenSize[1])
        moveMouse(xPos,yPos)

    # relative mode
    elif mode == 'rel':
        # change*
        # pg.moveRel((x)/100,(y)/100)
        pg.moveRel((x)/10,(y)/10)

    # mouse click handlers
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



def main():

    #global app
    #app = [None]
    #ui_th = threading.Thread(target=gui.uiThread, args=(app,))
    #ui_th.start()

    droppedPackets = 0

    global remoteIP, remotePort, udp

    # use the network scan tool to find the arduino's IP address
    # change*
    remoteIP,remotePort = findArduino(port)
    # print(remoteIP)
    # exit()
    # remotePort = 4210
    # remoteIP = input("Enter IP: ")

    # create UDP socket
    udp = UDPstream.initUDP(port)

    UDPstream.startStream(udp, remoteIP, remotePort)

    # infinite loop, ctrl+c to quit
    try:
        while True:
            # wait for data
            recvData = UDPstream.rxPacket(udp, packetSize, 0.02)
            # check if recieved and handle timeout error
            if recvData != None:
                data,addr = recvData

                # decode the data packet
                x,y,lc,rc,aux1,aux2 = UDPstream.decodeBits(data)
                x = -x/90 # post processing
                y = -y/90
                print(x,y,lc,rc,aux1,aux2)
                if keyboard.is_pressed("a"):
                    recalibrateMouse(x, y)
                if keyboard.is_pressed("q"):
                    softQuit(udp)
                # send to mouse control function
                # doMouseControl(x,y,lc,rc)
                # Divide by 90 for proper resolution
                
                # For recentering the mouse
                x = x - xOffset
                y = y - yOffset
                doMouseControl(x,y,lc,0)
                
                # pg.moveTo(100,100)

            else:
                droppedPackets += 1


    except KeyboardInterrupt:
        softQuit(udp)


if __name__ == '__main__':
    main()


softQuit(udp)
