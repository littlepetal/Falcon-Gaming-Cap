import pyautogui as pg
import network_scan_tool
import UDPstream

#import math
#import time


# TODO
# need auto calibration <- should be easy
# initial handshake <- needs to be more robust
# stream start/stop <- need to add udp rx and command parser on arduino side


# the port to use
port = 4210
# the resolution of your monitor(s) (if using multiple, simply add the x resolution together)
screenSize = 3440,1440
# relative or absolute mouse control modes ('abs'/'rel')
mode = 'rel'

# range of data from ardiuno
xRange = -1820,1820
yRange = -1820,1820

# size of the compressed packets in bytes
packetSize = 4;

# need these to reduce mouse movement delay
pg.MINIMUM_DURATION = 0
pg.MINIMUM_SLEEP = 0
pg.PAUSE = 0

# safely kills the UDP stream and closes the program
def softQuit():
    # send end transmission flag "END"
    try:
        udp.close()
        #UDPstream.killStream(udp)
        print("Safely ended stream and closed port")
    except:
        pass
    print("Closing...")
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

# moves the mouse (absolute coords)
def moveMouse(x,y):
    pg.moveTo(x,y)


def main():

    # mouse button pressed flags (allows for click and drag)
    leftClicked = False
    rightClicked = False

    # use the network scan tool to find the arduino's IP address
    remoteIP,remotePort = findArduino(port)

    # create UDP socket
    udp = UDPstream.initUDP(port)

    # send begin transmission flag "BGN"
    UDPstream.txString("BGN", udp, remoteIP, port)
    print("Starting UDP stream")

    # infinite loop, ctrl+c to quit
    try:
        while True:
            # wait for data
            recvData = UDPstream.rxPacket(udp, packetSize, 0.02)
            # check if recieved and handle timeout error
            if recvData != None:
                data,addr = recvData
            else:
                pass

            # decode the data packet
            x,y,lc,rc,aux1,aux2 = UDPstream.decodeBits(data)
            print(x,y,lc,rc,aux1,aux2)

            # absolute mode
            if mode == 'abs':
                xPos = map(x,xRange[0],xRange[1],0,screenSize[0])
                yPos = map(y,yRange[0],yRange[1],0,screenSize[1])
                moveMouse(xPos,yPos)

            # relative mode
            elif mode == 'rel':
                pg.moveRel((x)/100,(y)/100)

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


    except KeyboardInterrupt:
        softQuit()


if __name__ == '__main__':
    main()


softQuit()
