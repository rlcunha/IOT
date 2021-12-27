// --------------------------------------------------- //
// DECLARACAO DOS INCLUDES
// --------------------------------------------------- //
/* 1. Define the WiFi credentials */
#include "wifi_config.h"
#include "db_config.h"
#include "mqtt_config.h"

#define DEBUG
unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;

// --------------------------------------------------- //
// DECLARACAO DAS ROTINAS INICIAIS (RODA NA PRIMEIRA VEZ)
// --------------------------------------------------- //
void setup()
{
  //************************
  // inicializa a porta com para testes
  //************************  
  #ifdef DEBUG
    Serial.begin(115200);
  #endif

  //************************
  // INICIALIZA TODAS AS ROTINAS DOS SERVIÇOS USADOS
  //************************
    
  //Inicializa wifimanager
  initWifiManager();
  //Caso tenha conectado no roteador local, será conectado no wifi 
  initWifi();
  if (!hasWifi)
  {
    return;
  }
  //Inicializa acesso ao banco de dados NoSQL
  initFirebase();
  //Inicializa serviço de mqtt
  initMqtt();

  //************************
  // Atribui valores para variaveis 
  //************************
  if (Firebase.ready()){
    FirebaseJson json;   
       
    String sClient = "MibiTech"; //Firebase.RTDB.getString(&fbdo, "/client") ? fbdo.to<const char *>() : fbdo.errorReason().c_str();

    Serial.println("Gravando Firebase");

    String sHasEsp = Firebase.RTDB.getString(&fbdo, "/MibiTech/device/MIBITECH"+sClip_id) ? "true" : "false";

    if (sHasEsp != "true"){
      Serial.println("Has ESP : MIBITECH"+sClip_id);
      json.add("/device/MIBITECH"+sClip_id+"/localIP", sIpConnected);
      Firebase.RTDB.updateNode(&fbdo, "/"+sClient, &json) ? "ok" : fbdo.errorReason().c_str();
      json.add("/device/MIBITECH"+sClip_id+"/type", sType);    
      Firebase.RTDB.updateNode(&fbdo, "/"+sClient, &json) ? "ok" : fbdo.errorReason().c_str();    
    }
    else{
      Serial.println("No Has ESP : MIBITECH"+sClip_id);
      json.add("/device/MIBITECH"+sClip_id+"/localIP", sIpConnected);
      Firebase.RTDB.updateNode(&fbdo, "/"+sClient, &json) ? "ok" : fbdo.errorReason().c_str();
      json.add("/device/MIBITECH"+sClip_id+"/mqtt/Server", mqttServer);
      Firebase.RTDB.updateNode(&fbdo, "/"+sClient, &json) ? "ok" : fbdo.errorReason().c_str();
      json.add("/device/MIBITECH"+sClip_id+"/mqtt/topic", mqttServer);
      Firebase.RTDB.updateNode(&fbdo, "/"+sClient, &json) ? "ok" : fbdo.errorReason().c_str();
      json.add("/device/MIBITECH"+sClip_id+"/mqttServer", mqttServer);
      Firebase.RTDB.updateNode(&fbdo, "/"+sClient, &json) ? "ok" : fbdo.errorReason().c_str();    
      json.add("/device/MIBITECH"+sClip_id+"/database", DATABASE_URL);
      Firebase.RTDB.updateNode(&fbdo, "/"+sClient, &json) ? "ok" : fbdo.errorReason().c_str();
      json.add("/device/MIBITECH"+sClip_id+"/boot", "0");
      Firebase.RTDB.updateNode(&fbdo, "/"+sClient, &json) ? "ok" : fbdo.errorReason().c_str();
      json.add("/device/MIBITECH"+sClip_id+"/type", sType);    
      Firebase.RTDB.updateNode(&fbdo, "/"+sClient, &json) ? "ok" : fbdo.errorReason().c_str();    
      json.add("/device/MIBITECH"+sClip_id+"/task/1", "timeStart");
      Firebase.RTDB.updateNode(&fbdo, "/"+sClient, &json) ? "ok" : fbdo.errorReason().c_str();      
    }

    Serial.println("Gravando Firebase");

    String sBoot = Firebase.RTDB.getString(&fbdo, "/MibiTech/device/MIBITECH"+sClip_id+"/boot") ? fbdo.to<const char *>() : "0";

    Serial.println("Gravando Firebase - Boot : ");
    Serial.println(sBoot);    
    if (sBoot == "1"){
      wifiManager.resetSettings();  
    }
  }

  
}

// ---------------------------------------------------------- //
// ROTINA INFINITA QUE RODA ATÉ DAR ERRO OU SAIDA ESPONTANEA
// ---------------------------------------------------------- //
void loop()
{
  if (!client.connected()){
    reconectarMqtt();    
  }
  
  //Flash string (PROGMEM and FPSTR), Arduino String, C++ string, const char, char array, string literal are supported
  //in all Firebase and FirebaseJson functions, unless F() macro is not supported.

  if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

    Serial.printf("Set bool... %s\n", Firebase.RTDB.setBool(&fbdo, "/test/bool", count % 2 == 0) ? "ok" : fbdo.errorReason().c_str());

    Serial.printf("Get bool... %s\n", Firebase.RTDB.getBool(&fbdo, "/test/bool") ? fbdo.to<bool>() ? "true" : "false" : fbdo.errorReason().c_str());

    bool bVal;
    Serial.printf("Get bool ref... %s\n", Firebase.RTDB.getBool(&fbdo, "/test/bool", &bVal) ? bVal ? "true" : "false" : fbdo.errorReason().c_str());

    Serial.printf("Set int... %s\n", Firebase.RTDB.setInt(&fbdo, "/test/int", count) ? "ok" : fbdo.errorReason().c_str());

    Serial.printf("Get int... %s\n", Firebase.RTDB.getInt(&fbdo, "/test/int") ? String(fbdo.to<int>()).c_str() : fbdo.errorReason().c_str());

    int iVal = 0;
    Serial.printf("Get int ref... %s\n", Firebase.RTDB.getInt(&fbdo, "/test/int", &iVal) ? String(iVal).c_str() : fbdo.errorReason().c_str());

    Serial.printf("Set float... %s\n", Firebase.RTDB.setFloat(&fbdo, "/test/float", count + 10.2) ? "ok" : fbdo.errorReason().c_str());

    Serial.printf("Get float... %s\n", Firebase.RTDB.getFloat(&fbdo, "/test/float") ? String(fbdo.to<float>()).c_str() : fbdo.errorReason().c_str());

    Serial.printf("Set double... %s\n", Firebase.RTDB.setDouble(&fbdo, "/test/double", count + 35.517549723765) ? "ok" : fbdo.errorReason().c_str());

    Serial.printf("Get double... %s\n", Firebase.RTDB.getDouble(&fbdo, "/test/double") ? String(fbdo.to<double>()).c_str() : fbdo.errorReason().c_str());

    Serial.printf("Set string... %s\n", Firebase.RTDB.setString(&fbdo, "/test/string", "Hello World!") ? "ok" : fbdo.errorReason().c_str());

    Serial.printf("Get string... %s\n", Firebase.RTDB.getString(&fbdo, "/test/string") ? fbdo.to<const char *>() : fbdo.errorReason().c_str());

    //For the usage of FirebaseJson, see examples/FirebaseJson/BasicUsage/Create.ino
    FirebaseJson json;

    if (count == 0)
    {
      json.set("value/round/" + String(count), "cool!");
      json.set("vaue/ts/.sv", "timestamp");
      Serial.printf("Set json... %s\n", Firebase.RTDB.set(&fbdo, "/test/json", &json) ? "ok" : fbdo.errorReason().c_str());
    }
    else
    {
      json.add(String(count), "smart!");
      Serial.printf("Update node... %s\n", Firebase.RTDB.updateNode(&fbdo, "/test/json/value/round", &json) ? "ok" : fbdo.errorReason().c_str());
    }

    //For generic set/get functions.

    //For generic set, use Firebase.RTDB.set(&fbdo, <path>, <any variable or value>)

    //For generic get, use Firebase.RTDB.get(&fbdo, <path>).
    //And check its type with fbdo.dataType() or fbdo.dataTypeEnum() and
    //cast the value from it e.g. fbdo.to<int>(), fbdo.to<std::string>().

    //The function, fbdo.dataType() returns types String e.g. string, boolean,
    //int, float, double, json, array, blob, file and null.

    //The function, fbdo.dataTypeEnum() returns type enum (number) e.g. fb_esp_rtdb_data_type_null (1),
    //fb_esp_rtdb_data_type_integer, fb_esp_rtdb_data_type_float, fb_esp_rtdb_data_type_double,
    //fb_esp_rtdb_data_type_boolean, fb_esp_rtdb_data_type_string, fb_esp_rtdb_data_type_json,
    //fb_esp_rtdb_data_type_array, fb_esp_rtdb_data_type_blob, and fb_esp_rtdb_data_type_file (10)

    publicarTopico("sala/lampadaSala",String(count));

    count++;
  }
}

/// PLEASE AVOID THIS ////

//Please avoid the following inappropriate and inefficient use cases
/**
 * 
 * 1. Call get repeatedly inside the loop without the appropriate timing for execution provided e.g. millis() or conditional checking,
 * where delay should be avoided.
 * 
 * Everytime get was called, the request header need to be sent to server which its size depends on the authentication method used, 
 * and costs your data usage.
 * 
 * Please use stream function instead for this use case.
 * 
 * 2. Using the single FirebaseData object to call different type functions as above example without the appropriate 
 * timing for execution provided in the loop i.e., repeatedly switching call between get and set functions.
 * 
 * In addition to costs the data usage, the delay will be involved as the session needs to be closed and opened too often
 * due to the HTTP method (GET, PUT, POST, PATCH and DELETE) was changed in the incoming request. 
 * 
 * 
 * Please reduce the use of swithing calls by store the multiple values to the JSON object and store it once on the database.
 * 
 * Or calling continuously "set" or "setAsync" functions without "get" called in between, and calling get continuously without set 
 * called in between.
 * 
 * If you needed to call arbitrary "get" and "set" based on condition or event, use another FirebaseData object to avoid the session 
 * closing and reopening.
 * 
 * 3. Use of delay or hidden delay or blocking operation to wait for hardware ready in the third party sensor libraries, together with stream functions e.g. Firebase.RTDB.readStream and fbdo.streamAvailable in the loop.
 * 
 * Please use non-blocking mode of sensor libraries (if available) or use millis instead of delay in your code.
 * 
 * 4. Blocking the token generation process.
 * 
 * Let the authentication token generation to run without blocking, the following code MUST BE AVOIDED.
 * 
 * while (!Firebase.ready()) <---- Don't do this in while loop
 * {
 *     delay(1000);
 * }
 * 
 */
