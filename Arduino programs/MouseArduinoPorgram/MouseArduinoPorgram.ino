
// UDP
#include "ArdUDP.h"
#include "IMURollPitchYaw.h"

ArdPyUDP udpComm;
stringDataPacket myPacket;
RPY pitchYawData;

int testFlag;



UDP_data_packet packet = {0,0,0,0,0,0};
int data[4] = {0,0,0,0}; // pitch, yaw, left click, right click

void setup() {
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }

   udpComm.init();
   udpComm.UDPSetup();

  

//    Serial.println("Test 1");
//    pitchYawData.init();
    pitchYawData.IMUsetup();
//    Serial.println("Test 2");
    // for test purposes
    testFlag = 0;
}



void loop() {
    
    
    pitchYawData.updatePitchYaw();
    float pitch = pitchYawData.getPitch();
    float yaw = pitchYawData.getYaw();

    data[0] = (int) (yaw * 90); // x data
    data[1] = (int) (pitch * 90); // y data

    packet = {data[0], data[1], 0, 0, 0, 0};

   if (testFlag == 0) {
     testFlag = 1;
     Serial.println("UDP setup!");
   }
   udpComm.receieveUDP();
   udpComm.writeUDP32(packet);

}
