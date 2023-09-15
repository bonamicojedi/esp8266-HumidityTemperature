#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SimpleDHT.h>

// WiFi - Coloque aqui suas configurações de WI-FI
const char ssid[] = "S20Bona";
const char psw[] = "123abc123";

// Site remoto - Coloque aqui os dados do site que vai receber a requisição GET
const char http_site[] = "server-esp8266.onrender.com";
const int http_port = 443;
const char http_path[] = "/cadastrar";

// Variáveis globais

unsigned long previousMillis = 0;
const long interval = 5000;

IPAddress server(216, 24, 57, 253);  // Endereço IP do servidor - http_site
int pinDHT11 = D0;
SimpleDHT11 dht11;

void setup() {
  delay(5000);
  Serial.begin(9600);
  Serial.println("NodeMCU - Gravando dados no BD via GET");
  Serial.println("Aguardando conexão");

  // Tenta conexão com Wi-Fi
  WiFi.begin(ssid, psw);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.print("\nWi-Fi conectado com sucesso: ");
  Serial.println(ssid);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    byte temp = 0;
    byte humid = 0;
    if (dht11.read(pinDHT11, &temp, &humid, NULL)) {
      Serial.println("Falha na leitura do sensor.");
      return;
    }

    Serial.println("Gravando dados no BD: ");
    Serial.print((int)temp);
    Serial.print(" *C, ");
    Serial.print((int)humid);
    Serial.println(" %");

    if (!sendDataToServer((int)temp, (int)humid)) {
      Serial.println("GET request failed");
    }
  }
}


bool sendDataToServer(int temp, int humid) {
  WiFiClientSecure client;
  client.setInsecure();

  String url = "https://" + String(http_site) + http_path + "/" + String(temp) + "/" + String(humid);

  Serial.println("fazendo request");
  Serial.println(url);


  if (client.connect(http_site, http_port)) {
    Serial.println("conectado ao cliente");
    //client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + http_site + "\r\n" + "Connection: close\r\n\r\n");
    client.print(String("GET /cadastrar/") + (temp) +"/"+(humid)+ " HTTP/1.1\r\n" + "Host: " + http_site + "\r\n" + "Connection: close\r\n\r\n");

    while (client.connected()) {
      Serial.println("esperando por resposta do servidor");
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        break;
      }
    }

    while (client.available()) {
      Serial.println("esperando resto da resposta");
      String line = client.readStringUntil('\n');
      Serial.println(line);
    }

    Serial.println("concluído");

    client.stop();
    return true;
  } 
    
  Serial.println("Falha na conexão com o servidor");
  return false;
}
