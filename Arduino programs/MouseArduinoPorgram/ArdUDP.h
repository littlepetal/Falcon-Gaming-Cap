#pragma once

// #include <Arduino.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>


// Network port setup
#define SECRET_SSID "Shah_Networks"
#define SECRET_PASS "ShahFamily1358"

struct stringDataPacket {
  char data[256];
};

// UDP data packet format (32bit total)
struct UDP_data_packet{
   uint32_t X : 14;
   uint32_t Y : 14;
   uint32_t LC : 1;
   uint32_t RC : 1;
   uint32_t aux_1 : 1;
   uint32_t aux_2 : 1;
};


class ArdPyUDP {
  public:
    void init();
    void UDPSetup();
    char receieveUDP();
    void writeUDP(stringDataPacket packet);
    void writeUDP32(UDP_data_packet packet);
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
    void sendACK();
    int waitForPacket(WiFiUDP* Udp, char* incomingPacket, unsigned int localUdpPort);
    IPAddress remoteIp;
    
};
