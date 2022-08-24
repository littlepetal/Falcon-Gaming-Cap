import socket
import struct


def initUDP(port):
    # create udp socket and bind port
    global udp
    udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp.bind(("", port))
    return udp


def twosComp(data):
    numBits = 14
    if (data & (1 << (numBits - 1))) != 0:
        data = data - (1 << numBits)
    return data


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

    decoded = (x,y,l,r,aux1,aux2)
    return decoded


def txPacket(bits, udp, IP, port):
    udp.sendto(bits, (IP, port))


def txString(string, udp, IP, port):
    txPacket(string.encode(), udp, IP, port)


def rxPacket(udp, len, timeout = 1):

    oldTimeout = udp.gettimeout()
    udp.settimeout(timeout)

    try:
        ret = udp.recvfrom(len)
    except socket.timeout:
        print('Timed out waiting for packet')
        ret = None
    except Exception as e:
        print("error: ", e)
        ret=None

    udp.settimeout(oldTimeout)

    return ret


def rxString(udp, len, timeout = 1):

    ret = rxPacket(udp, len, timeout)

    if ret != None:
        data, addr = ret
        string = data.decode()
        return string,addr

    return ret


def startStream(udp, remoteIP, port):
    txString("BGN", udp, remoteIP, port)
    print("Starting UDP stream")


def killStream(udp, remoteIP, port):
    txString("END", udp, remoteIP, port)
    print("Killing UDP stream")
    udp.close()
