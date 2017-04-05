/* Basic example for RGB led
 *
 *  Simply blinks quickly every color in loop,
 *  and print it to the serial monitor.
 *
 */

#define LED_RED 5
#define LED_GREEN 3
#define LED_BLUE 2

void setup()
{

  /* serial monitor */
  SerialUSB.begin(115200);

#ifdef CONSOLE_CHECK
  while(!SerialUSB)
  {
    ;      /*SerialUSB not ready */
  }
  SerialUSB.println(">>Console Ready");
#endif

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
}

void loop()
{
  SerialUSB.println("Led RED ");
  digitalWrite(LED_RED, HIGH);
  delay(500);
  digitalWrite(LED_RED, LOW);

  SerialUSB.println("Led GREEN ");
  digitalWrite(LED_GREEN, HIGH);
  delay(500);
  digitalWrite(LED_GREEN, LOW);

  SerialUSB.println("Led BLUE ");
  digitalWrite(LED_BLUE, HIGH);
  delay(500);
  digitalWrite(LED_BLUE, LOW);

  SerialUSB.println("Led PINK ");
  digitalWrite(LED_BLUE, HIGH);
  digitalWrite(LED_RED, HIGH);
  delay(500);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_BLUE, LOW);

  SerialUSB.println("Led YELLOW ");
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  delay(500);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  
  SerialUSB.println("Led TURQUOISE ");
  digitalWrite(LED_BLUE, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  delay(500);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_GREEN, LOW);

  SerialUSB.println("Led WHITE ");
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);
  delay(500);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);

}
