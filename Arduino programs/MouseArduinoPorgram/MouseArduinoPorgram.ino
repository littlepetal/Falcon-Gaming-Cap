
// UDP
#include "ArdUDP.h"
#include "IMURollPitchYaw.h"

ArdPyUDP udpComm;
UDPDataPacket myPacket;
RPY pitchYawData;

int testFlag;

void setup() {
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }

    udpComm.init();
    udpComm.UDPSetup();

    Serial.println("Test 1");
//    pitchYawData.init();
//    pitchYawData.IMUsetup();
    Serial.println("Test 2");
    // for test purposes
    testFlag = 0;
}



void loop() {
    
    
//    pitchYawData.updatePitchYaw();
//    float pitch = pitchYawData.getPitch();
//    float yaw = pitchYawData.getYaw();
    
    String s = "Hallo";
    strcpy(myPacket.data, s.c_str());

    if (testFlag == 0) {
      testFlag = 1;
      Serial.println("UDP setup!");
    }
    udpComm.receieveUDP();
    udpComm.writeUDP(myPacket);
}
