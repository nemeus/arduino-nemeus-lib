/* Basic example for LoRaWAN ABP
 *
 *  Uses Nemeus Library
 *  Offers the possibility to change the device address
 *  In main loop,sends a frame using LoRaWAN.
 *
 */

#include <NemeusLib.h>
#include <Wire.h>

/* Define device address, change value here if wanted */
#define DEVADDR "60000000"

#define MAXPAYLOADSIZE 512
/* Define a counter variable */
uint16_t frameCounter = 0;
uint8_t ret;

char pattern[4] = { 'C', 'A', 'F', 'E' };

/* Reception callback for RF frames */
void onReceive(const char *string);

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

  SerialUSB.println(">>Sketch: Sending frame using LoRaWAN ABP ");

  /* Reset the modem */
  if ( nemeusLib.resetModem() != NEMEUS_SUCCESS)
  {
    SerialUSB.print("Nemeus device is not responding!");
    while(1)
    {
    }
  }

  /* Init nemeus library */
  if(nemeusLib.init() != NEMEUS_SUCCESS)
  {
    SerialUSB.print("Nemeus device is not responding!");
    while(1)
    {
    }
  }

  /* Register a callback for reception */
  nemeusLib.register_at_response_callback(&onReceive);

  /* Enable verbose traces */
  ret = nemeusLib.setVerbose(true);

  /* Get DevUID */
  String devUID = nemeusLib.loraWan()->readDevUID();

  /* Turn ON Radio */
  ret = nemeusLib.loraWan()->ON('A', false);

  if(ret == NEMEUS_SUCCESS)
  {
    SerialUSB.println("LoRaWAN ON - Class A - ABP!!!");
  }
  else
  {
    SerialUSB.println("LoRaWAN ON command error!!");
  }

  /* Read personnal parameters */
  DevPerso_t *devPerso = nemeusLib.loraWan()->readDevPerso();

  /* Compare with value defined */
  if (strcmp(devPerso->devAddr, (char*)DEVADDR)==0 )
  {
    SerialUSB.println("No changement for the device address");
  }
  else 
  {
    SerialUSB.println("Changing device address...");
    ret = nemeusLib.loraWan()->setDevAddr((char*)DEVADDR);
  }

}

void loop()
{
  nemeusLib.pollDevice(5000);

  /* Get Maximum payload */
  int maxPayloadSize = nemeusLib.loraWan()->getMaximumPayloadSize();

  static char buffer[MAXPAYLOADSIZE];

  for (int i = 0; i < maxPayloadSize; i++)
  {
    buffer[i] = pattern[i % 4];
  }
  buffer[maxPayloadSize] = 0;

  /* Send a frame (BINARY mode, Repetition = 1, mac Port = 1, ACK,  Encrypted) */
  ret = nemeusLib.loraWan()->sendFrame(0, 1, 1, buffer, 1, 1);

  nemeusLib.pollDevice(5000);
  nemeusLib.printTraces();

}

/* ---------------- Functions ---------------- */

void onReceive(const char *string)
{
  SerialUSB.print("mm002 >> ");
  SerialUSB.println(string);
}
