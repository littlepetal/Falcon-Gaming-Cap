
// UDP
#include "ArdUDP.h"
#include "IMURollPitchYaw.h"

ArdPyUDP udpComm;
UDPDataPacket myPacket;
RPY pitchYawData;

void setup() {
    Serial.begin(9600);

    udpComm.init();
    udpComm.UDPSetup();

    pitchYawData.init();
    pitchYawData.IMUsetup();
}

void loop() {
    String s = "Hallo";
    
    pitchYawData.updatePitchYaw();
    float pitch = pitchYawData.getPitch();
    float yaw = pitchYawData.getYaw();
    
    udpComm.sendReceieveloop();
    strcpy(myPacket.data, s.c_str());
    udpComm.writeUDP(myPacket);
}
