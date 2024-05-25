#include <NewPing.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>

#define TRIG_PIN_1 5  // GPIO5 D1
#define ECHO_PIN_1 4  // GPIO4 D2

#define TRIG_PIN_2 14 // GPIO14 D5
#define ECHO_PIN_2 12 // GPIO12 D6

#define TRIG_PIN_3 13 // GPIO13 D7
#define ECHO_PIN_3 15 // GPIO15 D8

NewPing sensor1(TRIG_PIN_1, ECHO_PIN_1, 50);
NewPing sensor2(TRIG_PIN_2, ECHO_PIN_2, 50);
NewPing sensor3(TRIG_PIN_3, ECHO_PIN_3, 50);

// const char *ssid = "Totalplay-81AA";
// const char *password = "81AACCCAP4qzVGuG";

//const char *ssid = "labred";
//const char *password = "labred2017";

const char *ssid = "Redmi Note 10 Pro";
const char *password = "Je2852585";

// const char *ssid = "#WUAMC";
// const char *password = "wificua6";

AsyncWebServer server(80);
// 192.168.172
IPAddress local_IP(192, 168, 172, 100); // Dirección IP estática
IPAddress gateway(192, 168, 172, 1);
IPAddress subnet(255, 255, 255, 0);

void setup() {
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT); // Configura el LED integrado como salida

  connectToWiFi(); // Conectarse a la red WiFi

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");

  server.on("/mediciones", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Recibida solicitud GET en /mediciones");

    String content = "{\"distancia1\":" + String(sensor1.ping_cm() * 1.0,2) + ",\"distancia2\":" + String(sensor2.ping_cm() * 1.0,2) + ",\"distancia3\":" + String(sensor3.ping_cm() * 1.0,2) + "}";
    
    request->send(200, "application/json", content);
  });

  server.on("/guardarmediciones", HTTP_POST, [](AsyncWebServerRequest *request){
    Serial.println("Recibida solicitud POST en /guardarmediciones");

    // Verificar si hay datos en el cuerpo de la solicitud POST
    if(request->hasParam("plain", true)){
      // Leer el cuerpo de la solicitud POST
      String content = request->getParam("plain", true)->value();
      
      // Imprimir el contenido recibido
      Serial.println("Contenido recibido en la solicitud POST:");
      Serial.println(content);

      // Enviar una respuesta
      String responseContent = "Datos recibidos correctamente.";
      request->send(200, "text/plain", responseContent);
    } else {
      // No se encontró el parámetro "plain" en la solicitud
      Serial.println("Error: No se encontró el parámetro 'plain' en la solicitud POST.");
      request->send(400); // Bad Request
    }
  });

  server.on("/guardarmediciones", HTTP_OPTIONS, [](AsyncWebServerRequest *request){
    Serial.println("Recibida solicitud OPTIONS en /guardarmediciones");

    request->send(200); 
  });

  server.begin();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Se ha perdido la conexión WiFi. Intentando reconexión...");
    connectToWiFi(); // Intentar reconectar
    digitalWrite(LED_BUILTIN, LOW); // Apaga el LED integrado durante la reconexión
    delay(1000); // Espera 1 segundo antes de intentar reconectar
  } else {
    digitalWrite(LED_BUILTIN, HIGH); // Enciende el LED integrado si la conexión WiFi está activa
    delay(100); // Espera 0.1 segundo
    digitalWrite(LED_BUILTIN, LOW); // Apaga el LED integrado
    delay(900); // Espera 0.9 segundos para un ciclo de parpadeo de 1 segundo
  }
}

void connectToWiFi() {
  Serial.println("Conectando a la red WiFi...");
  WiFi.mode(WIFI_STA); // Modo estación (cliente)

  WiFi.config(local_IP, gateway, subnet);

  WiFi.begin(ssid, password);

  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 20) {
    delay(1000);
    Serial.println("Intentando conectar...");
    intentos++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Conexión WiFi establecida");
    Serial.print("Dirección IP asignada: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Error al conectar a la red WiFi. Verifica las credenciales.");
  }
}
