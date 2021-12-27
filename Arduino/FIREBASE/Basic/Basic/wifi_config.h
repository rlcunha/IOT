// --------------------------------------------------- //
// DECLARACAO DOS INCLUDES
// --------------------------------------------------- //
/* 1. Define the WiFi credentials */
#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
ESP8266WebServer server(80);
#else
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
WebServer server(80);
#endif
#include <WiFiManager.h> 

// --------------------------------------------------- //
// DECLARACAO DE OBJETOS
// --------------------------------------------------- //
// wifiManager
WiFiManager wifiManager;

// --------------------------------------------------- //
// DECLARACAO DE VARIAVEIS E CONSTANTES (LOCAL)
// --------------------------------------------------- //
static bool hasWifi = false;
String sSSIDConnected;
String sIpConnected;
String passConnected;
String sClip_id;
#ifdef ESP8266
  String sType = "ESP8266";
#else
  String sType = "ESP32";
#endif
// --------------------------------------------------- //
// DECLARACAO DAS FUNÇÕES
// --------------------------------------------------- //
//*****************
// callback que indica que o ESP entrou no modo AP
//*****************
void configModeCallback(WiFiManager *myWiFiManager) {  
  Serial.println("Entrou no modo de configuração");
  Serial.print("Ip Modo config");
  Serial.println(WiFi.softAPIP()); //imprime o IP do AP
  Serial.print("ssid modo config");
  Serial.println(myWiFiManager->getWiFiSSID()); //imprime o IP do AP
  Serial.print("Nome da rede criada : ");
  Serial.println(myWiFiManager->getConfigPortalSSID()); //imprime o SSID criado da rede
//  ssidConnected = myWiFiManager.getSSID();
//  passConnected = myWiFiManager.getPassword();
}
//*****************
//**** callback que indica que salvamos uma nova rede para se conectar (modo estação)
//*****************
void saveConfigCallback () {
  Serial.println("Configuração salva");
  Serial.print("Device Name : MIBI");
  Serial.println(sClip_id);
  Serial.println("salvamos uma nova rede para se conectar (modo estação)");
}
//*****************
// Função de inicialização do WiFi
//*****************
static void initWifi()
{
  Serial.println("Connecting...");
  //WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  hasWifi = true;
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  sIpConnected = WiFi.localIP().toString().c_str(); 
  Serial.println(WiFi.localIP()); 
}
//*****************
// Função de inicialização do WIFI MANAGER
//*****************
static void initWifiManager(){
  #ifdef ESP8266
    if(sClip_id.length()<=0){
      sClip_id = String(ESP.getChipId());
    }
  #endif
  
  #ifdef ESP32
    if(sClip_id.length()<=0){
      uint32_t id = 0;
      for(int i=0; i<17; i=i+8) {
        id |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
      }
      sClip_id = String(id);
      //Serial.printf("%08X\n", id);
    }
  #endif
  
  //utilizando esse comando, as configurações são apagadas da memória
  //caso tiver salvo alguma rede para conectar automaticamente, ela é apagada.
  //wifiManager.resetSettings();

  //callback para quando entra em modo de configuração AP
  wifiManager.setAPCallback(configModeCallback); 
  //callback para quando se conecta em uma rede, ou seja, quando passa a trabalhar em modo estação
  wifiManager.setSaveConfigCallback(saveConfigCallback); 
  //cria uma rede 
  wifiManager.autoConnect(("MIBIHOME_ESP"+sClip_id).c_str()); 
  
}
