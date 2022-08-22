#define button 5
boolean buttonState;
void setup()
{
 pinMode(LED_BUILTIN, OUTPUT); 
 pinMode(button,INPUT_PULLUP);
 Serial.begin(9600);
}
void loop()
{
 buttonState = digitalRead(button);
 Serial.println(buttonState);

 if(buttonState == HIGH) {
    digitalWrite(LED_BUILTIN, LOW);
 }
 else {
    digitalWrite(LED_BUILTIN, HIGH);
 }
}
