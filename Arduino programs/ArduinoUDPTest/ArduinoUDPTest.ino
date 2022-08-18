

#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>

#define SECRET_SSID "DevanSpot"
#define SECRET_PASS "ojsb6982"

int status = WL_IDLE_STATUS;
//#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key index number (needed only for WEP)

unsigned int localPort = 4210;      // local port to listen on

char packetBuffer[256]; //buffer to hold incoming packet
char  ReplyBuffer[] = "acknowledged";       // a string to send back

WiFiUDP Udp;

void sendACK(WiFiUDP* Udp, unsigned int localUdpPort){

  memset(ReplyBuffer, 0x00, sizeof(ReplyBuffer));
  sprintf(ReplyBuffer, "ACK");

  Udp->beginPacket(Udp->remoteIP(), Udp->remotePort());
  Udp->write((const uint8_t*)ReplyBuffer, 16);
  Udp->endPacket();
  
}

int waitForPacket(WiFiUDP* Udp, char* incomingPacket, unsigned int localUdpPort){
  
  bool readPacket = false;
  
  while(!readPacket){
    
    Udp->begin(localUdpPort);
//    Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);

    // we recv one packet from the remote so we can know its IP and port
    while (!readPacket) {
      
      int packetSize = Udp->parsePacket();
      
      if (packetSize)
       {
        
        // receive incoming UDP packets
//        Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp->remoteIP().toString().c_str(), Udp->remotePort());
        
        int len = Udp->read(incomingPacket, 255);
        if (len > 0)
        {
          incomingPacket[len] = 0;
        }
        
//        Serial.printf("UDP packet contents: %s\n", incomingPacket);
        readPacket = true;
        
      }  
    } 
  }
}

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
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

  waitForPacket(&Udp, &packetBuffer[0], localPort);
  sendACK(&Udp, localPort);
}

void loop() {

  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = Udp.remoteIP();
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

    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();
  }
}


void printWifiStatus() {
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
