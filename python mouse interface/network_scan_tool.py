import time
import socket

# network scanning tool created by Darragh McMullen
# searches your network for devices listening on the given port
# returns the local IPv4 address and port of the device

port = 4210; #default port
rxPacketSize = 255;
socketTimeout = 0.01;

def get_local_ip():
    IP = (([ip for ip in socket.gethostbyname_ex(socket.gethostname())[2] if not ip.startswith("127.")] or [[(s.connect(("8.8.8.8", 53)), s.getsockname()[0], s.close()) for s in [socket.socket(socket.AF_INET, socket.SOCK_DGRAM)]][0][1]]) + ["no IP found"])[0]
    return IP


def find_device_ip(udp, LanIPformat, port):
    for i in range(256):
        try:
            remoteIP = f'{LanIPformat}{i}'
            print(f'Scanning IP: {remoteIP}')
            udp.sendto("HS".encode(), (remoteIP, port))
            data, addr = udp.recvfrom(rxPacketSize)
            if(bytes("ACK",'utf-8') in data):
                return addr

        except socket.timeout:
            pass

    return None


def find_device(port):

    LanIPformat = get_local_ip().split(".")
    LanIPformat = f'{LanIPformat[0]}.{LanIPformat[1]}.{LanIPformat[2]}.'

    udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp.settimeout(socketTimeout)
    udp.bind(("", port))

    startTime = time.time()
    remoteIP = find_device_ip(udp, LanIPformat, port)
    if remoteIP != None:
        endTime = time.time()
        totalTime = round(endTime-startTime,2)
        print(f'Found device at: {remoteIP}, took {totalTime}s')
    else:
        endTime = time.time()
        totalTime = round(endTime-startTime,2)
        print(f'Unable to locate device on the network. Took {totalTime}s')

    udp.close()

    return remoteIP


if __name__ == '__main__':
    find_device(port)
