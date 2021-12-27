// --------------------------------------------------- //
// DECLARACAO DOS INCLUDES
// --------------------------------------------------- //
#include <PubSubClient.h>

// --------------------------------------------------- //
// DECLARACAO DE OBJETOS
// --------------------------------------------------- //
WiFiClient mibiIotClient;
PubSubClient client(mibiIotClient);

// --------------------------------------------------- //
// DECLARACAO DE VARIAVEIS E CONSTANTES (LOCAL)
// --------------------------------------------------- //
//informações do broker MQTT - Verifique as informações geradas pelo CloudMQTT
const char* mqttServer = "driver.cloudmqtt.com";
const char* mqttUser = "ohcqhlgs";              //user
const char* mqttPassword = "rV_EeIl0XIaw";      //password
const int mqttPort = 18686;                     //port

// --------------------------------------------------- //
// DECLARACAO DAS FUNÇÕES
// --------------------------------------------------- //
//*****************
void callback(char* topic, byte* payload, unsigned int length) {
//*****************
//armazena msg recebida em uma sring
  payload[length] = '\0';
  String strMSG = String((char*)payload);
 
  Serial.print("Mensagem chegou do tópico: ");
  Serial.println(topic);
  Serial.print("Mensagem:");
  Serial.println(strMSG);
  Serial.println();
  Serial.println("-----------------------");
}
//*****************
// --- reconecar ao Cliente MQTT
//*****************
void reconectarMqtt(){
  //Enquanto estiver desconectado
  while (!client.connected()) {
    #ifdef DEBUG
    Serial.println("Tentando conectar ao servidor MQTT");
    #endif
     
    bool conectado = strlen(mqttUser) > 0 ?
                     client.connect("ESP8266Client", mqttUser, mqttPassword) :
                     client.connect("ESP8266Client");
 
    if(conectado) {
      #ifdef DEBUG
      Serial.println("Conectado!");
      #endif
      //subscreve no tópico
//      client.subscribe(mqttTopicSubTemp, 1); //nivel de qualidade: QoS 1
//      client.subscribe(mqttTopicSubUmi, 1); //nivel de qualidade: QoS 1
    } else {
      #ifdef DEBUG
      Serial.print("Falha durante a conexão.Code: ");
      Serial.println( String(client.state()).c_str());
      Serial.println("Tentando novamente em 10 s");
      #endif
      //Aguarda 10 segundos 
      delay(10000);
    }
  }
}
//*****************
// Inicializa o serviço do MQTT
//*****************
void initMqtt(void){
  Serial.println("MQTT - Conectado Servidor...");
  client.setServer(mqttServer,mqttPort);
  Serial.println("MQTT - Configurado CallBack...");
  client.setCallback(callback);
}
//*****************
// Envia mensagens para o servidor de MQTT 
//*****************
void publicarTopico(String mqttTopicSub,String msgMqtt){
  client.publish(mqttTopicSub.c_str(), msgMqtt.c_str());        
}
