/* Basic example for LoRaWAN OTAA
 *
 *  Uses Nemeus Library
 *  Offers the possibility to change the app UID and the app KEY
 *  In main loop,sends a frame using LoRaWAN.
 *
 */

#include <NemeusLib.h>
#include <Wire.h>

/* Define APPUID and APPKEY, change value here if needed */
#define APPUID    "70B3D53260000100"
//#define APPKEY "000102030405060708090A0B0C0D0E0F"  //Uncomment if you want to change the APPKEY
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

  SerialUSB.println(">>Sketch: Sending frame using LoRaWAN OTAA ");

  delay(1000);

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

  nemeusLib.setVerbose(true);

  /* Enable verbose traces */
  ret = nemeusLib.setVerbose(true);

  /* Turn off LoRaWAN if not */
  ret = nemeusLib.loraWan()->OFF();

  /* Read personnal parameters */
  DevPerso_t *devPerso = nemeusLib.loraWan()->readDevPerso();

  /* Compare with values defined */
  if (strcmp(devPerso->appUID, (char*)APPUID) == 0) 
  {
    SerialUSB.println("No changement for the APPUID");
  }
  else 
  {
    SerialUSB.println("Changing APPUID...");
    ret = nemeusLib.loraWan()->setAppUID((char*)APPUID);
  }
#ifdef  APPKEY
  SerialUSB.println("Changing the APPKEY...");
  ret = nemeusLib.loraWan()->setAppKey((char*)APPKEY);
#endif

  /* Turn ON Radio */
  ret = nemeusLib.loraWan()->ON('A', true);

  if(ret == NEMEUS_SUCCESS)
  {
    SerialUSB.println("LoRaWAN ON - Class A - OTAA!!!");
  }
  else
  {
    SerialUSB.println("LoRaWAN ON OTAA failure!");
    while(1)
    {
    }
  }

  /* Read ABP perso */
  DevPerso_t *abpPerso = nemeusLib.loraWan()->readAbpPerso();
}

void loop()
{
  /* Get Maximum payload */
  int maxPayloadSize = nemeusLib.loraWan()->getMaximumPayloadSize();

  static char buffer[MAXPAYLOADSIZE];

  for (int i = 0; i < maxPayloadSize; i++)
  {
    buffer[i] = pattern[i % 4];
  }
  buffer[maxPayloadSize] = 0;
  /* Send a frame (BINARY mode, Repetition = 1, mac Port = 2, ACK,  Encrypted) */
  ret = nemeusLib.loraWan()->sendFrame(0, 1, 2, buffer, 1, 1);

  nemeusLib.pollDevice(5000);
  nemeusLib.printTraces();

}

/* ---------------- Functions ---------------- */

void onReceive(const char *string)
{
  SerialUSB.print("mm002 >> ");
  SerialUSB.println(string);
}
