# Testing the sending and recieving of string to arduino

import network_scan_tool
import UDPstream

# from main.py
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

port = 4210
udp = UDPstream.initUDP(port)

remoteIP = input("Enter IP address of arduino: ")

UDPstream.txString("BGN", udp, remoteIP, port)
print("Starting UDP stream")


# infinite loop, ctrl+c to quit
try:    
    while True:
        # wait for data
        bufferSize = 2048
        recvData = UDPstream.rxString(udp, bufferSize, 0.02)
        # check if recieved and handle timeout error
        if recvData != None:
            data,addr = recvData
            print(data)
        else:
            pass

except KeyboardInterrupt:
        softQuit()

softQuit()

