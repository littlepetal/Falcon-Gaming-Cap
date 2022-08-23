
// UDP
#include "ArdUDP.h"
#include "IMURollPitchYaw.h"

#define LEFT_BUTTON_PIN 5
ArdPyUDP udpComm;
stringDataPacket myPacket;
RPY pitchYawData;

int testFlag;
bool buttonState = false;

UDP_data_packet packet = {0,0,0,0,0,0};
int data[4] = {0,0,0,0}; // pitch, yaw, left click, right click

int serialConnectionTry = 10;

void setup() {

    pinMode(LED_BUILTIN, OUTPUT);
    // Button pin
    pinMode(LEFT_BUTTON_PIN,INPUT_PULLUP);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    Serial.begin(9600);
    while (!Serial) {
      for (int i=0; i < serialConnectionTry; i++)
      {
        delay(100);
      }
      break;
    }

    // set up IMU
    pitchYawData.IMUsetup();
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    

   udpComm.init();
   udpComm.UDPSetup();

    

    // A flag to check if UDP and IMU has properly setup
    testFlag = 0;
    // Setup complete blink
    
}

void loop() {
    
    

    // Get pitch and yaw data
    pitchYawData.updatePitchYaw();
    float pitch = pitchYawData.getPitch();
    float yaw = pitchYawData.getYaw();

    // Place pitch yaw data into packet
    data[0] = (int) (yaw * 90); // x data
    data[1] = (int) (pitch * 90); // y data
    data[2] = 0;
//    uint32_t leftClick = 0;

    // Read button state
    buttonState = digitalRead(LEFT_BUTTON_PIN);
    if (!buttonState) {
      data[2] = 1; // left click enabled
    }
    packet = {data[0], data[1], data[2], 0, 0, 0};


   if (testFlag == 0) {
     testFlag = 1;
     Serial.println("UDP and IMU setup!");
     digitalWrite(LED_BUILTIN, HIGH);
   }
   char checkFlag = udpComm.receieveUDP();
   if (checkFlag == 'n') {
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    pitchYawData.IMUsetup();
    digitalWrite(LED_BUILTIN, HIGH);
   }
   udpComm.writeUDP32(packet);

}
