
// UDP
#include "ArdUDP.h"

ArdPyUDP udpComm;
UDPDataPacket myPacket;

void setup() {
    Serial.begin(9600);

    udpComm.init();
    udpComm.UDPSetup();
}

void loop() {
    String s = "Hallo";
    udpComm.sendReceieveloop();
    strcpy(myPacket.data, s.c_str());
    udpComm.writeUDP(myPacket);
}
