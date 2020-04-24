//ATTINY - SEND

#include "RF24.h"

#define NOW_ATTINY true



#if NOW_ATTINY
  RF24 radio(3,4);
#else
  RF24 radio(9,10);
#endif


byte address[][6] = {"1Node", "2Node"};

byte counter = 1;
const char command[] = "P";
    
void setup() {
  #if !NOW_ATTINY
    Serial.begin(9600);
  #endif

  radio.begin();

  radio.setPayloadSize(32);
  radio.openWritingPipe(address[0]);
  radio.setChannel(0x60);

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS

  radio.powerUp();
  radio.stopListening();
}


void loop(void) {
  #if !NOW_ATTINY
    Serial.print(command); Serial.print(" "); Serial.println(millis());
  #endif

  radio.write(&command, sizeof(command));
  delay(500);
}
