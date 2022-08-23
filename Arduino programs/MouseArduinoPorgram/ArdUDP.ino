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

}

// Send ACK packet back (to help with locating board with network finder)
void ArdPyUDP::sendACK() {
  char replyBuffer[20];
 memset(replyBuffer, 0x00, sizeof(replyBuffer));
 sprintf(replyBuffer, "ACK");

 Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
 Udp.write((const uint8_t*)replyBuffer, 16);
 Udp.endPacket();
 Serial.println("Sent acknowledgement");
 
}


// Recieve any incoming udp packet and print the contents into serial
char ArdPyUDP::receieveUDP() {

  char exitFlag = 'y';
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
    Serial.println("Stream begun.");
;   }
   else if (strcmp(packetBuffer, "HS") == 0) {
    sendACK();
   }
   else if (strcmp(packetBuffer, "END") == 0) {
    connectionMade = false; // Stop streaming data
    Serial.println("Stream stopped.");
   }
   else if (strcmp(packetBuffer, "DRI") == 0) 
   {
      exitFlag = 'n';
      Serial.println("Client ask for recalibration");
   }
   return exitFlag;

   
 }
}


// To send a string packet
void ArdPyUDP::writeUDP(stringDataPacket packet) {

  if (connectionMade) {
    Udp.beginPacket(remoteIp, Udp.remotePort());
    Udp.write(packet.data, sizeof(packet.data));
    Udp.endPacket();
  }
  
}

// Function to write 32 bit packets of data to python program
void ArdPyUDP::writeUDP32(UDP_data_packet packet) {

  if (connectionMade) {
    Udp.beginPacket(remoteIp, Udp.remotePort());
    Udp.write((const uint8_t*)&packet, 4);
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
