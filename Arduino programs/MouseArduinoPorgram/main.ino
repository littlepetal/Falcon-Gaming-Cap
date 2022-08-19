// #include <WebServer.h>
// #include <WiFi.h>
// #include <WiFiUdp.h>

// // ACTUAL NAME OF FILE : ESP32_UDP

// // Replace with your network credentials (STATION)
// const char* ssid = "DevanPort";
// const char* password = "stcga8sfx8arxguy";

// WiFiUDP Udp;

// unsigned int localUdpPort = 4210;  //  port to listen on

// char incomingPacket[255] = {0};  // buffer for incoming packets
// char outBuffer[16] = {0};

// // UDP data packet format (32bit total)
// struct UDP_data_packet{
//    uint32_t X : 14;
//    uint32_t Y : 14;
//    uint32_t LC : 1;
//    uint32_t RC : 1;
//    uint32_t aux_1 : 1;
//    uint32_t aux_2 : 1;
// };


// void initWiFi() {
  
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);
  
//   Serial.print("Connecting to WiFi ..");
  
//   while (WiFi.status() != WL_CONNECTED) {
//     Serial.print('.');
//     delay(1000);
//   }
  
//   Serial.println("\n" + WiFi.localIP());
// }

// void initUdp(WiFiUDP* Udp, unsigned int localUdpPort){
//   Udp->begin(localUdpPort);
// }


// int waitForPacket(WiFiUDP* Udp, char* incomingPacket, unsigned int localUdpPort){
  
//   bool readPacket = false;
  
//   while(!readPacket){
    
//     Udp->begin(localUdpPort);
//     Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);

//     // we recv one packet from the remote so we can know its IP and port
//     while (!readPacket) {
      
//       int packetSize = Udp->parsePacket();
      
//       if (packetSize)
//        {
        
//         // receive incoming UDP packets
//         Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp->remoteIP().toString().c_str(), Udp->remotePort());
        
//         int len = Udp->read(incomingPacket, 255);
//         if (len > 0)
//         {
//           incomingPacket[len] = 0;
//         }
        
//         Serial.printf("UDP packet contents: %s\n", incomingPacket);
//         readPacket = true;
        
//       }  
//     } 
//   }
// }


// void sendACK(WiFiUDP* Udp, unsigned int localUdpPort){

//   memset(outBuffer, 0x00, sizeof(outBuffer));
//   sprintf(outBuffer, "ACK");

//   Udp->beginPacket(Udp->remoteIP(), Udp->remotePort());
//   Udp->write((const uint8_t*)outBuffer, 16);
//   Udp->endPacket();
  
// }


// void analogAverage(int* data, int pin, int nSamples){

//   long acc = 0;
//   int avg = 0;

//   for(int i = 0; i < nSamples; i++){
//     acc += analogRead(pin);
//   }

//   avg = (int)(acc/nSamples);
//   *data = avg;
  
// }

// void setup() {
//   pinMode(swPin, INPUT);
//   Serial.begin(115200);
//   Serial.println("Serial connected");
//   initWiFi();
//   //initUdp(&Udp, localUdpPort);
//   waitForPacket(&Udp, &incomingPacket[0], localUdpPort);
//   sendACK(&Udp, localUdpPort);
// }


// int data[4] = {0, 0, 0, 0};
// struct UDP_data_packet packet = {0,0,0,0,0,0};


// void loop() {

//   // analogAverage(&data[0], xPin, 100);
//   // analogAverage(&data[1], yPin, 100);
//   // data[2] = !(digitalRead(swPin));
  
//   memset(outBuffer, 0x00, sizeof(outBuffer));
//   //sprintf(outBuffer, "%d,%d,%d\r\n", data[0],data[1],data[2]);

//   float pitch = 32.32;
//   float yaw = -56.92;

//   // TODO:
//   // multiply by 90 to increase resolution
//   data[0] = (int) (pitch); 
//   data[1] = (int) (yaw);

//   packet = {data[0],data[1],0,0,0,0};
  
//   Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
//   Udp.write((const uint8_t*)&packet, 4);
//   //Udp.write((const uint8_t*)outBuffer, 16);
//   Udp.endPacket();

//   //Serial.print(outBuffer);
//   //Serial.print();

//   //~10ms to run cycle before delay -> ~100Hz
//   //delay(0);
  
// }