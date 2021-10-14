// --- WIFI ----
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
const char* ssid = "risagami2G";
const char* password = "cesabe01";

WiFiClient mibiIotClient;
PubSubClient client(mibiIotClient);

const char* mqtt_broker = "mqtt.eclipseprojects.io";
const char* topicTemperatura = "mibitech/temperatura";
const char* topicUmidade = "mibitech/umidade";
// Create a random client ID
const char* clientIdMqtt = "mibi-tech-smarthome-temperatura";

#include <DHT.h>
#define DHTPIN D3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int umidade;
int temperatura;

// --- Display ---
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     LED_BUILTIN // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// --------------------------------------------------- //
// --- Setup
// --------------------------------------------------- //
void setup() {
  Serial.begin(115200);
  configurarDisplay();
  conectarWifi();
  client.setServer(mqtt_broker,1883);
}
// --------------------------------------------------- //
// --- Loop ---
// --------------------------------------------------- //
void loop() {
  if (!client.connected()){
    reconectarMqtt();    
  }
  mostrarTemperaturaUnidade();
  publicarTemperaturaUmidadeTopico();
}

// --- Funçoes Auxiliares

// -- Configuração e Conexao WIFI
void conectarWifi(){
  delay(10);
  display.setTextSize(2);
  display.setCursor(0,0);
  display.print("Conectando");
  display.display();
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.print(".");
    display.display();
  }    
}

// --- reconecar ao Cliente MQTT
void reconectarMqtt(){
  while (!client.connected()) {
    // Attempt to connect
    if (client.connect(clientIdMqtt)) {
      Serial.println("connected");
//      // Once connected, publish an announcement...
//      client.publish("outTopic", "hello world");
//      // ... and resubscribe
//      client.subscribe("inTopic");
    } 
//    else {
//      Serial.print("failed, rc=");
//      Serial.print(client.state());
//      Serial.println(" try again in 5 seconds");
//      // Wait 5 seconds before retrying
//      delay(5000);
//    }

  }
}

// --- Publica (MQTT) Temperatura / Umidade
void publicarTemperaturaUmidadeTopico(){
  if(temperatura<200){
    client.publish(topicTemperatura, String(temperatura).c_str(), true);        
  }
  if(umidade<200){
    client.publish(topicUmidade, String(umidade).c_str(), true);        
  }
}

void configurarDisplay(){
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
}

void mostrarTemperaturaUnidade(){
  umidade = dht.readHumidity();
  temperatura = dht.readTemperature(false); 
  medirDados("TEMPERATURA",(temperatura), " C");
  medirDados("UMIDADE",(umidade)," %");
}

/// --- Medição de temperatura e umidade
void medirDados(const char* texto1, int medicao, const char* texto2){
  if(medicao < 200){
    display.clearDisplay();
  
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0,0);             // Start at top-left corner
    display.println(texto1);
  
    //display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
    //display.println(3.141592);
  
    display.setCursor(30,30);             // Start at top-left corner
    display.setTextSize(3.5);             // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.print(medicao); 
    display.println(texto2); 
    Serial.println(medicao);
    display.display();
    delay(2000);
  }
}
