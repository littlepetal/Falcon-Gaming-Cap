#pragma once

// #include <Arduino.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>


// Network port setup
#define SECRET_SSID ""
#define SECRET_PASS ""

struct UDPDataPacket {
  char data[256];
};

class ArdPyUDP {
  public:
    void init();
    void UDPSetup();
    void receieveUDP();
    void writeUDP(UDPDataPacket data);
  private:
    int keyIndex;             // your network key index number (needed only for WEP)
    int status = WL_IDLE_STATUS;
    WiFiUDP Udp;
    unsigned int localPort;      // local port to listen on
    // Buffer data
    char packetBuffer[256]; //buffer to hold incoming packet
    char  ReplyBuffer[256] = "acknowledged";       // a string to send back
    bool connectionMade;
    void printWifiStatus();
    void sendACK(WiFiUDP* Udp, unsigned int localUdpPort);
    int waitForPacket(WiFiUDP* Udp, char* incomingPacket, unsigned int localUdpPort);
    IPAddress remoteIp;
    
};
