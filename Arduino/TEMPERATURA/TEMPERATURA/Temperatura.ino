// --- WIFI ----
#include <ESP8266WiFi.h>

// --- WIFIMANAGER ----
//needed for library wifimanager
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

// --- MQTT ----
#include <PubSubClient.h>

#define DEBUG

const char* ssid = "risagami2G";
const char* password = "cesabe01";

WiFiClient mibiIotClient;
PubSubClient client(mibiIotClient);

//informações do broker MQTT - Verifique as informações geradas pelo CloudMQTT
const char* mqttServer = "driver.cloudmqtt.com";
const char* mqttUser = "ohcqhlgs";              //user
const char* mqttPassword = "rV_EeIl0XIaw";      //password
const int mqttPort = 18686;                     //port
const char* mqttTopicSubTemp = "mibitech/temperatura";
const char* mqttTopicSubUmi = "mibitech/umidade";

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

  //WiFiManager
  WiFiManager wifiManager;

  //wifiManager.resetSettings();
  
  wifiManager.autoConnect("AutoConnectAP");
  Serial.println("wifimanager connected...yeey :)");
  
  conectarWifi();

  //MQTT
  client.setServer(mqttServer,mqttPort);
  client.setCallback(callback);

  configurarDisplay();
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
  //Enquanto estiver desconectado
  while (!client.connected()) {
    #ifdef DEBUG
    Serial.print("Tentando conectar ao servidor MQTT");
    #endif
     
    bool conectado = strlen(mqttUser) > 0 ?
                     client.connect("ESP8266Client", mqttUser, mqttPassword) :
                     client.connect("ESP8266Client");
 
    if(conectado) {
      #ifdef DEBUG
      Serial.println("Conectado!");
      #endif
      //subscreve no tópico
      client.subscribe(mqttTopicSubTemp, 1); //nivel de qualidade: QoS 1
      client.subscribe(mqttTopicSubUmi, 1); //nivel de qualidade: QoS 1
    } else {
      #ifdef DEBUG
      Serial.println("Falha durante a conexão.Code: ");
      Serial.println( String(client.state()).c_str());
      Serial.println("Tentando novamente em 10 s");
      #endif
      //Aguarda 10 segundos 
      delay(10000);
    }
  }
}

// --- Publica (MQTT) Temperatura / Umidade
void publicarTemperaturaUmidadeTopico(){
  if(temperatura<200){
    client.publish(mqttTopicSubTemp, String(temperatura).c_str(), true);        
  }
  if(umidade<200){
    client.publish(mqttTopicSubUmi, String(umidade).c_str(), true);        
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
  exibirDadosDisplay("TEMPERATURA",(temperatura), " C");
  exibirDadosDisplay("UMIDADE",(umidade)," %");
}

/// --- Medição de temperatura e umidade
void exibirDadosDisplay(const char* texto1, int medicao, const char* texto2){
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

void callback(char* topic, byte* payload, unsigned int length) {
 
  //armazena msg recebida em uma sring
  payload[length] = '\0';
  String strMSG = String((char*)payload);
 
  #ifdef DEBUG
  Serial.print("Mensagem chegou do tópico: ");
  Serial.println(topic);
  Serial.print("Mensagem:");
  Serial.print(strMSG);
  Serial.println();
  Serial.println("-----------------------");
  #endif
 
  //aciona saída conforme msg recebida 
  if (strMSG == "1"){         //se msg "1"
    Serial.print("Mensagem:");
     //digitalWrite(L1, LOW);  //coloca saída em LOW para ligar a Lampada - > o módulo RELE usado tem acionamento invertido. Se necessário ajuste para o seu modulo
  }else if (strMSG == "0"){   //se msg "0"
    Serial.print("Mensagem: 0");
     //digitalWrite(L1, HIGH);   //coloca saída em HIGH para desligar a Lampada - > o módulo RELE usado tem acionamento invertido. Se necessário ajuste para o seu modulo
  }
}
 
