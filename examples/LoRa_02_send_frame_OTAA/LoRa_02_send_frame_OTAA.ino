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
/* Reception callback for Downlink frames */
void onReceiveDownlink(uint8_t port , boolean more, const char * hexaPayload, int rssi, int snr);

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
  /* Register a callback for downlink frames */
  nemeusLib.loraWan()->register_downlink_callback(&onReceiveDownlink);
  
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

/*
 * If piggyback setting is disabled and device class is A, the server will be polled automatically to receive more downlink frames.
 * A downlink frame unsolicited response is always sent after a Tx to indicate the end of Rx windows.
 */
void onReceiveDownlink(uint8_t port , boolean more, const char * hexaPayload, int rssi, int snr)
{
  SerialUSB.println("Downlink frame received");
  SerialUSB.print("Port:");
  SerialUSB.println(port);
  SerialUSB.print("Pending frames:");
  if(more == 1)
  {
    SerialUSB.println("False");
  }
  else
  {
    SerialUSB.println("True");
  }
  SerialUSB.print("Payload:");
  SerialUSB.println(hexaPayload);
  SerialUSB.print("RSSI:");
  SerialUSB.println(rssi);
  SerialUSB.print("SNR:");
  SerialUSB.println(snr);
}
