#define pinLED 7
#define pinPIR 8

void setup() 
{
  Serial.begin(9600); 
  pinMode(pinLED, OUTPUT);
  pinMode(pinPIR, INPUT);
}

void loop() 
{ 
  if (digitalRead(pinPIR) == HIGH)
    {
    digitalWrite(pinLED, HIGH); 
    Serial.println("Perto");
    } 
  else 
    {
    digitalWrite(pinLED, LOW);
    Serial.println("Longe");
    }
}
