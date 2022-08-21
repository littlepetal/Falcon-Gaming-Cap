#include "ArdUDP.h"

void ArdPyUDP::init() {
  keyIndex = 0;             // your network key index number (needed only for WEP)
  status = WL_IDLE_STATUS;
  localPort = 4210;      // local port to listen on
  connectionMade = false;
}


void ArdPyUDP::UDPSetup() {
  // Connect arduino to WPA/WPA2 network
  char ssid[] = SECRET_SSID;    // your network SSID (name)
  char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);  
  }
  Serial.println("Connected to WiFi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  Udp.begin(localPort);

  // Do we need this?
//  waitForPacket(&Udp, &packetBuffer[0], localPort);
//  sendACK(&Udp, localPort); 

}

void ArdPyUDP::sendACK(WiFiUDP* Udp, unsigned int localUdpPort){

 memset(ReplyBuffer, 0x00, sizeof(ReplyBuffer));
 sprintf(ReplyBuffer, "ACK");

 Udp->beginPacket(Udp->remoteIP(), Udp->remotePort());
 Udp->write((const uint8_t*)ReplyBuffer, 16);
 Udp->endPacket();

 
}

int ArdPyUDP::waitForPacket(WiFiUDP* Udp, char* incomingPacket, unsigned int localUdpPort){
 
 bool readPacket = false;
 
  while(!readPacket){
   Udp->begin(localUdpPort);
   // we recv one packet from the remote so we can know its IP and port
    while (!readPacket) {
     
      int packetSize = Udp->parsePacket();
     
      if (packetSize) {
        int len = Udp->read(incomingPacket, 255);
        if (len > 0) {
          incomingPacket[len] = 0;
        }
        readPacket = true;
      }  
    } 
  } 
}



void ArdPyUDP::receieveUDP() {

 // if there's data available, read a packet
 int packetSize = Udp.parsePacket();
 if (packetSize) {
   Serial.print("Received packet of size ");
   Serial.println(packetSize);
   Serial.print("From ");
   remoteIp = Udp.remoteIP();
   Serial.print(remoteIp);
   Serial.print(", port ");
   Serial.println(Udp.remotePort());

   // read the packet into packetBufffer
   int len = Udp.read(packetBuffer, 255);
   if (len > 0) {
     packetBuffer[len] = 0;
   }
   Serial.println("Contents:");
   Serial.println(packetBuffer);

   if (strcmp(packetBuffer, "BGN") == 0) {
    connectionMade = true;
    remoteIp = Udp.remoteIP();
   }
   
 }
}

void ArdPyUDP::writeUDP(UDPDataPacket packet) {

  if (connectionMade) {
    Udp.beginPacket(remoteIp, Udp.remotePort());
    Udp.write(packet.data, sizeof(packet.data));
    Udp.endPacket();
  }
  
}


void ArdPyUDP::printWifiStatus() {
 // print the SSID of the network you're attached to:
 Serial.print("SSID: ");
 Serial.println(WiFi.SSID());

 // print your board's IP address:
 IPAddress ip = WiFi.localIP();
 Serial.print("IP Address: ");
 Serial.println(ip);

 // print the received signal strength:
 long rssi = WiFi.RSSI();
 Serial.print("signal strength (RSSI):");
 Serial.print(rssi);
 Serial.println(" dBm");
}
