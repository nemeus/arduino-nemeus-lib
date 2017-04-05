/*This is an example for the Nemeus library using RF radio to perform
 * continuous FSK transmit
*/
#include <NemeusLib.h>
#include <Wire.h>

#define LED_RED 5
#define LED_GREEN 3
#define LED_BLUE 2

/* Define a counter variable */
uint8_t ret;

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

  /* Init nemeus library */
  if(nemeusLib.init() != NEMEUS_SUCCESS)
  {
    SerialUSB.print("Nemeus device is not responding!");
    while(1)
    {
    }
  }

  /* Set RF Tx parameter (if needed) */
  /* This parameter is the default parameter. No need to set it except if you want to change. */
  RadioTxParam txParam;

  /* LoRa mode */
  txParam.setMode(RADIO_FSK_MODE);
  /* Tx Power */
  txParam.setTxPower(14);
  /* Tx Frequency tp 868100000 Hz */
  txParam.setFrequency(868100000);

  /* Send config to device */
  ret = nemeusLib.radio()->setRadioTxParam(txParam);

  if(ret == NEMEUS_SUCCESS)
  {
    SerialUSB.println("TX radio parameters set!!");
  }
  else
  {
    SerialUSB.println("TX radio parameters set error!!");
  }

  /* Turn ON Radio */
  ret = nemeusLib.radio()->ON();

  if(ret == NEMEUS_SUCCESS)
  {
    SerialUSB.println("RADIO ON!!!");
  }
  else
  {
    SerialUSB.println("RADIO ON command error!!");
  }

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  /* Start FSK continuous transmission */
  ret = nemeusLib.radio()->Radio::continuousTx();
  SerialUSB.print(">>Example: Response is : ");
  SerialUSB.println(ret);

}


void loop()
{
  /* just blink the LED */
  digitalWrite(LED_GREEN, HIGH);
  delay(500);
  digitalWrite(LED_GREEN, LOW);
  delay(500);
}
