//ATTiny - RECEIVE

//#include <printf.h>
//#include "nRF24L01.h"
#include "RF24.h"

#define NOW_ATTINY true

#if NOW_ATTINY
  RF24 radio(3,3);
  #define RELAY_PIN 4
#else
  RF24 radio(9,10);
  #define RELAY_PIN LED_BUILTIN
#endif

#define MANUALY_PIN 3



byte address[][6] = {"1Node", "2Node"};

char currentCommand[32] = "";
char const REMOTE_COMMAND[] = "P";

const byte timeWindowSize = 30; // Временное окно в секундах
const byte minCommandsInWindow = 2; // Сколько раз минимум нужно получить команд в течение временного окна для включения реле

const unsigned long millisInWindow = 1000 *  (long)timeWindowSize; // Размер временного окна в миллисекундах
unsigned long timeWindowStart = 0; // Когда начато текущее временное окно
byte commandsInWindow = 0; // Сколько получено команд в текущем временном окне

const int delayInMillis = 1000;


void turnRelay(bool nowRelayOn) {
  digitalWrite(RELAY_PIN, nowRelayOn ? HIGH : LOW);
  
  #if !NOW_ATTINY
    Serial.print("NOW RELAY IS "); Serial.println(nowRelayOn ? "ON" : "OFF");
  #endif

  timeWindowStart = millis();
  commandsInWindow = 0;
}


void setup() {
  #if !NOW_ATTINY
    Serial.begin(9600);
  #endif
  
  radio.begin();

  radio.setPayloadSize(32);
  radio.openReadingPipe(1, address[0]);
  radio.setChannel(0x60);

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS

  radio.powerUp();
  radio.startListening();

  pinMode(MANUALY_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  Serial.println("Starting ...");
  Serial.print("timeWindowSize: ");Serial.println(timeWindowSize);
  Serial.print("millisInWindow: ");Serial.println(millisInWindow);
}


void loop() {
  unsigned long millisAlreadyInCurWindow = millis() - timeWindowStart;

  #if !NOW_ATTINY
    Serial.print("...test ");Serial.println(millisAlreadyInCurWindow);
  #endif

  if (millisAlreadyInCurWindow > millisInWindow) {
    // Если дошли до конца временного окна, значит в процессе не набрали минимального кол-ва команд. Поэтому, отключаем реле.
    turnRelay(false);
  }

  if ( radio.available() ) {
    radio.read( &currentCommand, sizeof(currentCommand) );
    if (strcmp(REMOTE_COMMAND, currentCommand) == 0) {
			#if !NOW_ATTINY
				Serial.println("Command received!");
			#endif
      commandsInWindow++;
    }
		else {
			#if !NOW_ATTINY
				Serial.print("Unknown command: ");Serial.println(currentCommand);
			#endif
		}
  }

  // Если уже соблюдено минимальное кол-во команд, то нет смысла дожидаться окончания временного окна
  if (commandsInWindow > minCommandsInWindow) turnRelay(true);

  delay(delayInMillis);
}

