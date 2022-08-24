// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A1);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (2.8 / 1023.0);
  // print out the value you read:
  Serial.println(voltage);

  if(voltage < 0.2*2.8) {
    Serial.println("Battery LOW");
  }
  else{
    Serial.println("Battery HIGH");
  }
}
