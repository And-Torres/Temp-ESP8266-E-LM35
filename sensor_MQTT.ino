#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Configurações da rede Wi-Fi
const char* ssid = "NOME_DA_REDE";
const char* password = "SENHA";

// Configurações do servidor MQTT
const char* mqttServer = "test.mosquitto.org";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";

// Informações do cliente MQTT
const char* mqttTopic = "topic/temperatura";
const char* mqttClientId = "esp8266mod";

// Cliente WiFi e MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Função chamada quando uma mensagem MQTT é recebida
void callback(char* topic, byte* payload, unsigned int length) {
  // Converter o payload em uma string
  String atual;
  for (int i = 0; i < length; i++) {
    atual += (char)payload[i];
  }
  
  // Imprimir o tópico e a mensagem recebida
  Serial.print("Tópico: ");
  Serial.println(topic);
  Serial.print("Atual: ");
  Serial.println(atual);
}

void setup() {
  // Inicializar a comunicação serial
  Serial.begin(115200);

  // Conectar-se à rede WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi!");

  // Configurar o servidor MQTT
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  // Conectar-se ao servidor MQTT
  while (!client.connected()) {
    if (client.connect(mqttClientId, mqttUser, mqttPassword)) {
      Serial.println("Conectado ao servidor MQTT!");
    } else {
      Serial.print("Falha na conexão MQTT. Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }

  // Assinar o tópico MQTT
  client.subscribe(mqttTopic);
}

void loop() {
  // Verificar se há mensagens MQTT recebidas
  client.loop();

  // Ler a temperatura
  int valorObtido = analogRead(A0);
  float milivolts = (valorObtido / 1024.0) * 3300;
  float celsius = milivolts / 10;

  // Publicar o valor da temperatura no tópico MQTT
  String atualToSend = String(celsius, 2);
  client.publish(mqttTopic, atualToSend.c_str());
  
  // Aguardar um intervalo antes de publicar novamente
  delay(10000);
}
