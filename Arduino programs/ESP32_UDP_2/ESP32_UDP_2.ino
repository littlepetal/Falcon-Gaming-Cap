#include <WiFi.h>
#include <WiFiUdp.h>


// Replace with your network credentials (STATION)
const char* ssid = "Belong90A3F8";
const char* password = "stcga8sfx8arxguy";
unsigned int localUdpPort = 4210;  //  port to listen on

// create WiFiUDP object
WiFiUDP Udp;

// buffers for in and outgoing packets
char inBuffer[255] = {0};
char outBuffer[255] = {0};

// UDP data packet format (32bit total)
struct UDP_data_packet{
   uint32_t X : 14;
   uint32_t Y : 14;
   uint32_t LC : 1;
   uint32_t RC : 1;
   uint32_t aux_1 : 1;
   uint32_t aux_2 : 1;
};

// analog pin definitions
const int xPin = 36;
const int yPin = 39;
const int swPin = 34;

// initialise wifi (connect to network)
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


// non-blocking UDP read packet function, returns the length of the packet read (0 if none)
int readPacket(WiFiUDP* Udp, char* inBuffer, bool printDebug){
  
  memset(inBuffer, 0x00, sizeof(inBuffer));
  
  int packetSize = Udp->parsePacket();
  
  if(packetSize){
    int len = Udp->read(inBuffer, packetSize);
    if(len){
      inBuffer[len] = 0;
      if(printDebug){
        Serial.println("Received " + String(packetSize) + " bytes from " + Udp->remoteIP().toString() + ":" + Udp->remotePort());
        Serial.println("UDP packet contents: " + String(inBuffer));
      }
    }
  }

  return packetSize;
}


// send "ACK" packet
void sendACK(WiFiUDP* Udp, unsigned int localUdpPort){

  memset(outBuffer, 0x00, sizeof(outBuffer));
  sprintf(outBuffer, "ACK");

  Udp->beginPacket(Udp->remoteIP(), Udp->remotePort());
  Udp->write((const uint8_t*)outBuffer, 3);
  Udp->endPacket();
  
}

// block until handshake received, then reply with ACK
bool waitForHandshake(WiFiUDP* Udp, char* inBuffer, unsigned int localUdpPort){

  bool HS = false;
  Serial.print("Waiting for handshake ...");
  
  while(!HS){
    int newPacket = readPacket(Udp, inBuffer, false);
    if(newPacket){
      String cmd = String(inBuffer).substring(0,2);
      if(cmd == "HS"){
        HS = true;
      }
    }
  }

  Serial.println("\nHandshake successful!");
  Serial.println("Paired with client at " + Udp->remoteIP().toString() + ":" + Udp->remotePort());

  sendACK(Udp, localUdpPort);
  
}

// average analog data for more reliable data
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
  
  Udp.begin(localUdpPort);
  Serial.println("Now listening at " + WiFi.localIP().toString() + ":" + localUdpPort);
  
  waitForHandshake(&Udp, &inBuffer[0], localUdpPort);
  
}


int data[] = {0};
struct UDP_data_packet packet = {0,0,0,0,0,0};
bool streaming = false;


void loop() {

  // try to read new packet
  int newPacket = readPacket(&Udp, &inBuffer[0], true);
    // if packet, check it
    if(newPacket){
      String cmd = String(inBuffer).substring(0,3);
      if(cmd == "BGN"){ // start the stream
        Serial.println("Starting UDP stream to " + Udp.remoteIP().toString() + ":" + Udp.remotePort());
        streaming = true;
      }
      if(cmd == "END"){ // kill the stream
        Serial.println("Stopping UDP stream to " + Udp.remoteIP().toString() + ":" + Udp.remotePort());
        streaming = false;
      }
    }

  // if streaming true, send data
  if(streaming){
    analogAverage(&data[0], xPin, 100);
    analogAverage(&data[1], yPin, 100);
    data[2] = !(digitalRead(swPin));
  
    data[0] -= 1821;  // subtract the bias
    data[1] -= 1825;  // subtract the bias
  
    packet = {data[0],data[1],data[2],0,0,0};
    
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write((const uint8_t*)&packet, 4);
    Udp.endPacket();
  }

}
