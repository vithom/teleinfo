#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <LibTeleinfo.h>

const char* ssid = "Hogwarts";
const char* password = "soleymieu1";

ESP8266WebServer server(80);

#define RX_PIN 14 
#define TX_PIN 12 // Non utilisé (D6)

// Configuration : 1200 bauds, 7 bits, Parité Paire (Even), 1 stop bit
SoftwareSerial linkySerial;

TInfo tinfo;

void flash_led() {
  digitalWrite(LED_BUILTIN, LOW);  // Turn the LED on (Note that LOW is the voltage level
  delay(100);                      // Wait for a second
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  delay(100);                   
}

void NewFrame(ValueList * me)
{
  flash_led();
  // sendJson(me, true);
}

void setup() {
  // Set CPU speed to 160MHz
  system_update_cpu_freq(160);
  
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);  // Initialize the LED_BUILTIN pin as an output

  linkySerial.begin(1200, SWSERIAL_7E1, RX_PIN, TX_PIN);

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

  tinfo.init();

  tinfo.attachNewFrame(NewFrame);

  Serial.println("Système prêt. Attente des données Linky (Mode Historique)...");
}

// loop function runs over and over  again forever
void loop() {
  server.handleClient();

  if (linkySerial.available()) {
    tinfo.process(linkySerial.read());
  }
}

void handle_root() {
  Serial.println("Route: /");
  server.send(200, "text/plain", "OK");
}

void handle_frame()
{
  Serial.println("Route: /last_frame");

  String response = "";

  response += F("{");
  
  ValueList * me = tinfo.getList();
    
  bool firstdata = true;
  // Got at least one ?
  if (me) {

    // Loop thru the node
    while (me->next) {
      // go to next node
      me = me->next;

      // First elemement, no comma
      if (firstdata)
        firstdata = false;
      else
        response += F(", ");

      response += F("\"");
      response += me->name;
      response += F("\":");

      // we have at least something ?
      if (me->value && strlen(me->value))
      {
        boolean isNumber = true;
        uint8_t c;
        char * p = me->value;

        // check if value is number
        while (*p && isNumber) {
          if ( *p < '0' || *p > '9' )
            isNumber = false;
          p++;
        }

        // this will add "" on not number values
        if (!isNumber) {
          response += F("\"");
          response += me->value;
          response += F("\"");
        }
        // this will remove leading zero on numbers
        else
          response += atol(me->value);
      }
    }
  }
  
  response += F("}");

  Serial.println(response);

  server.send(200, "text/json", response);
} 
