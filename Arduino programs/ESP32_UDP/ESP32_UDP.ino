#include <WiFi.h>
#include <WiFiUdp.h>


// Replace with your network credentials (STATION)
const char* ssid = "Belong90A3F8";
const char* password = "stcga8sfx8arxguy";

WiFiUDP Udp;

unsigned int localUdpPort = 4210;  //  port to listen on

char incomingPacket[255] = {0};  // buffer for incoming packets
char outBuffer[16] = {0};

// UDP data packet format (32bit total)
struct UDP_data_packet{
   uint32_t X : 14;
   uint32_t Y : 14;
   uint32_t LC : 1;
   uint32_t RC : 1;
   uint32_t aux_1 : 1;
   uint32_t aux_2 : 1;
};


const int xPin = 36;
const int yPin = 39;
const int swPin = 34;


void initWiFi() {
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  Serial.print("Connecting to WiFi ..");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  
  Serial.println("\n" + WiFi.localIP());
}



int waitForPacket(WiFiUDP* Udp, char* incomingPacket, unsigned int localUdpPort){
  
  bool readPacket = false;
  while(!readPacket){
    
    Udp->begin(localUdpPort);
    Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);

    // we recv one packet from the remote so we can know its IP and port
    while (!readPacket) {
      int packetSize = Udp->parsePacket();
      if (packetSize)
       {
        
        // receive incoming UDP packets
        Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp->remoteIP().toString().c_str(), Udp->remotePort());
        
        int len = Udp->read(incomingPacket, 255);
        if (len > 0)
        {
          incomingPacket[len] = 0;
        }
        
        Serial.printf("UDP packet contents: %s\n", incomingPacket);
        readPacket = true;
        
      }  
    } 
  }
}



void sendACK(WiFiUDP* Udp, unsigned int localUdpPort){

  memset(outBuffer, 0x00, sizeof(outBuffer));
  sprintf(outBuffer, "ACK");

  Udp->beginPacket(Udp->remoteIP(), Udp->remotePort());
  Udp->write((const uint8_t*)outBuffer, 16);
  Udp->endPacket();
  
}



bool waitForHandshake(WiFiUDP* Udp, char* incomingPacket, unsigned int localUdpPort){

  waitForPacket(Udp, incomingPacket, localUdpPort);
  sendACK(Udp, localUdpPort);
  
}


void analogAverage(int* data, int pin, int nSamples){

  long acc = 0;
  int avg = 0;

  for(int i = 0; i < nSamples; i++){
    acc += analogRead(pin);
  }

  avg = (int)(acc/nSamples);
  *data = avg;
  
}


void setup() {
  pinMode(swPin, INPUT);
  Serial.begin(115200);
  Serial.println("Serial connected");
  initWiFi();
  waitForHandshake(&Udp, &incomingPacket[0], localUdpPort);
  //waitForPacket(&Udp, &incomingPacket[0], localUdpPort);
  //sendACK(&Udp, localUdpPort);
  Serial.println("Starting UDP stream to " + Udp.remoteIP().toString() + ":" + Udp.remotePort());
}



int data[] = {0};
struct UDP_data_packet packet = {0,0,0,0,0,0};



void loop() {

  analogAverage(&data[0], xPin, 100);
  analogAverage(&data[1], yPin, 100);
  data[2] = !(digitalRead(swPin));

  data[0] -= 1821;  // subtract the bias
  data[1] -= 1825;  // subtract the bias
  
  //memset(outBuffer, 0x00, sizeof(outBuffer));
  //sprintf(outBuffer, "%d,%d,%d\r\n", data[0],data[1],data[2]);

  packet = {data[0],data[1],data[2],0,0,0};
  
  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  Udp.write((const uint8_t*)&packet, 4);
  //Udp.write((const uint8_t*)outBuffer, 16);
  Udp.endPacket();

  //Serial.print(outBuffer);
  //Serial.print();

  //~10ms to run cycle before delay -> ~100Hz
  //delay(0);
  
}
