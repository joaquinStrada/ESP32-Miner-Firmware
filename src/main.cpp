#include <Arduino.h>
#include <config.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "mbedtls/md.h"

// Variables
String poolUrl = "";
int poolPort = 0;
String walletAddress = "";
String mqttUser = "";
String mqttPass = "";
String mqttTopic = "";

//************************
//** F U N C I O N E S ***
//************************
void setup_wifi(void);
void setup_miner(void);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  setup_wifi();
  setup_miner();
}

void loop() {
  // put your main code here, to run repeatedly:
  
}

void setup_wifi() {
  delay(10);

  // Nos conectamos al wifi
  Serial.println("WiFi Setup");
  Serial.print("Conectando a SSID: ");
  Serial.print(WiFi_SSID);

  WiFi.begin(WiFi_SSID, WiFi_PASS);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("");
  Serial.println("Conectado a red WiFI!!!");
  Serial.print("Direccion Ip: ");
  Serial.println(WiFi.localIP());
}

void setup_miner() {
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("No estas conectado a la red wifi!!!");
    return;
  }
  
  // Hacemos una peticion al servidor
  Serial.println("Obteniendo los datos del minero");
  HTTPClient http;
  String data_send = "serie=" + String(SERIE_MINER) + "&password=" + String(PASS_MINER);

  http.begin(API_LOGIN);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int code_response = http.POST(data_send);

  if (code_response <= 0)
  {
    Serial.print("Error enviando post: ");
    Serial.println(code_response);
    return;
  } else if (code_response != 200) {
    Serial.print("Error http post: ");
    Serial.println(code_response);
    return;
  }
  
  String response = http.getString();

  Serial.print("Response: ");
  Serial.println(response);

  // Parseando los datos
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, response);

  if (error)
  {
    Serial.print("Error parseando los datos: ");
    Serial.println(error.c_str());
    return;
  } else if (doc["error"]) {
    Serial.print("Error http post: ");
    Serial.println(String((const char *)doc["message"]));
  }
  
  JsonObject data = doc["data"];

  poolUrl = String((const char *)data["poolUrl"]);
  poolPort = data["poolPort"];
  walletAddress = String((const char *)data["walletAddress"]);
  mqttUser = String((const char *)data["mqttUser"]);
  mqttPass = String((const char *)data["mqttPassword"]);;
  mqttTopic = String((const char*)data["mqttTopic"]);

  // Mostramos los datos por pantalla
  Serial.print("Url de la pool: ");
  Serial.println(poolUrl);
  Serial.print("Puerto de la pool: ");
  Serial.println(poolPort);
  Serial.print("Direccion de la wallet: ");
  Serial.println(walletAddress);
  Serial.print("Usuario de mqtt: ");
  Serial.println(mqttUser);
  Serial.print("Contraseña de mqtt: ");
  Serial.println(mqttPass);
  Serial.print("Topico de mqtt: ");
  Serial.println(mqttTopic);
}