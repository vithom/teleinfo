#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <LibTeleinfo.h>

const char* ssid = "Hogwarts";
const char* password = "soleymieu1";

ESP8266WebServer server(80);

#define RX_PIN 14 
#define TX_PIN 12 // Non utilisé (D6)

void flash_led() {
  digitalWrite(LED_BUILTIN, LOW);  // Turn the LED on (Note that LOW is the voltage level
  delay(100);                      // Wait for a second
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  delay(100);                   
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);  // Initialize the LED_BUILTIN pin as an output

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

    // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);  // Turn the LED on
    delay(300);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(300);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  digitalWrite(LED_BUILTIN, LOW);

  server.on("/", handle_root);
  server.on("/frame", handle_frame);

  server.begin();
  Serial.println("HTTP server started");

  Serial.println("Système prêt. Attente des données Linky (Mode Historique)...");
}

// loop function runs over and over  again forever
void loop() {
  server.handleClient();
}

void handle_root() {
  Serial.println("Route: /");
  server.send(200, "text/plain", "OK");
}

void handle_frame()
{
  Serial.println("Route: /last_frame");

  String response = "";

  response += F("{\"ADCO\":123456789,\"OPTARIF\":\"HC..\",\"ISOUSC\":30,\"HCHC\":12345,\"HCHP\":67890,\"PTEC\":\"HP..\",\"DEMAIN\":\"\",\"IINST\":15,\"IMAX\":45,\"PAPP\":3500,\"HHPHC\":\"HP\"}");

  Serial.println(response);

  flash_led();

  server.send(200, "text/json", response);
} 
