#include <SoftwareSerial.h>
#include <LibTeleinfo.h>


// On utilise D5 pour la réception (RX)
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

  sendJson(me, true);
}

void sendJson(ValueList * me, boolean all)
{
  bool firstdata = true;

  // Got at least one ?
  if (me) {
    // Json start
    Serial.print(F("{"));

    // Loop thru the node
    while (me->next) {
      // go to next node
      me = me->next;

      // uniquement sur les nouvelles valeurs ou celles modifiées 
      // sauf si explicitement demandé toutes
      if ( all || ( me->flags & (TINFO_FLAGS_UPDATED | TINFO_FLAGS_ADDED) ) )
      {
        // First elemement, no comma
        if (firstdata)
          firstdata = false;
        else
          Serial.print(F(", ")) ;

        Serial.print(F("\"")) ;
        Serial.print(me->name) ;
        Serial.print(F("\":")) ;

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
            Serial.print(F("\"")) ;
            Serial.print(me->value) ;
            Serial.print(F("\"")) ;
          }
          // this will remove leading zero on numbers
          else
            Serial.print(atol(me->value));
        }
      }
    }
   // Json end
   Serial.println(F("}")) ;
  }
}

void setup() {
  Serial.begin(115200); // Console USB

  pinMode(LED_BUILTIN, OUTPUT);
  
  // Initialisation du port Linky en 7E1
  // Le dernier paramètre "false" signifie que le signal n'est pas inversé
  // linkySerial.begin(1200, SWSERIAL_7E1, RX_PIN, TX_PIN, false);
  linkySerial.begin(1200, SWSERIAL_7E1, RX_PIN, TX_PIN);

  tinfo.init();

  // tinfo.attachData(DataCallback);
  tinfo.attachNewFrame(NewFrame);

  flash_led();
  flash_led();

  Serial.println("-");
  Serial.println("---");
  Serial.println("Système prêt. Attente des données Linky (Mode Historique)...");
}

void loop() {
  if (linkySerial.available()) {
    tinfo.process(linkySerial.read());

    // char c = linkySerial.read();
    // Le mode historique envoie un '0x02' pour début de trame 
    // et '0x03' pour fin de trame. On affiche tout sur le port série USB.
    // Serial.print(c);
  }
}