// C++ code
//
int LED_BRANCO = 12;
int BTN_ONOFF1 = 2;
int SONORO = 13;

int ON = 0;
int OFF = 1;

void setup()
{
    Serial.begin(9600);  
    pinMode(LED_BRANCO, OUTPUT);
    pinMode(BTN_ONOFF1, INPUT_PULLUP);
    pinMode(SONORO, OUTPUT);
}

void loop()
{
    lerOnOff();
    Serial.print("Potencia : ");
}


void piscaLed(int led)
{
  digitalWrite(led, HIGH);
  delay(50); // Wait for 1000 millisecond(s)
  digitalWrite(led, LOW);
  //delay(1000); // Wait for 1000 millisecond(s)
  //Serial.println(led);
}

void lerOnOff()
{
    if (digitalRead(BTN_ONOFF1) == ON)
    {
        piscaLed(LED_BRANCO);
        tone(SONORO,1500,50);
    }

}
