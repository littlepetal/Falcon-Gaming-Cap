import numpy as np
import time
import socket




port = 4210

rxPacketSize = 255;


def get_local_ip():
    IP = (([ip for ip in socket.gethostbyname_ex(socket.gethostname())[2] if not ip.startswith("127.")] or [[(s.connect(("8.8.8.8", 53)), s.getsockname()[0], s.close()) for s in [socket.socket(socket.AF_INET, socket.SOCK_DGRAM)]][0][1]]) + ["no IP found"])[0]
    return IP


def find_device_ip(udp, LanIPformat):
    for i in range(256):
        try:
            remoteIP = f'{LanIPformat}{i}'
            print(f'Scanning IP: {remoteIP}')
            udp.sendto("HS".encode(), (remoteIP, port))
            data, addr = udp.recvfrom(rxPacketSize)
            print(data)
            if(bytes("ACK",'utf-8') in data):
                return addr

        except socket.timeout:
            pass

    return None



def main():

    LanIPformat = get_local_ip().split(".")
    LanIPformat = f'{LanIPformat[0]}.{LanIPformat[1]}.{LanIPformat[2]}.'

    udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp.settimeout(0.25)
    udp.bind(("", port))

    remoteIP = find_device_ip(udp, LanIPformat)
    if remoteIP != None:
        print(f'Found device at: {remoteIP}')



main()
