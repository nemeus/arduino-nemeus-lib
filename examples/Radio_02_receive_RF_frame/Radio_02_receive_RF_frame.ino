/*This is an example for the Nemeus library using RF radio to receive
 * LoRa frames.
 * The Rx parameters are:
 *  - freq: 868100000 Hz
 *  - bandwidth: 125000 Hz
 *  - Data Rate : 7
 *  - Code Rate : 1
 * This example waits for RF frame.
 */

#include <NemeusLib.h>
#include <Wire.h>

#define FRAME_SIZE 8

/* Define a counter variable */
uint16_t frameCounter = 0;
uint8_t ret;

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

  SerialUSB.println(">>Sketch: Test Connexion OK ");

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

  /* Set RF Tx parameter (if needed) */
  /* This parameter are the default parameter. No need to set it except if you want to change. */
  RadioRxParam rxParam;

  /* LoRa mode */
  rxParam.setMode(RADIO_LORA_MODE);
  /* Tx Frequency tp 868100000 Hz */
  rxParam.setFrequency(868100000);
  /* Bandwidth to 125000 Hz */
  rxParam.setBandwidth(125000);
  /* Data Rate to 7 */
  rxParam.setDataRate(7);
  /* Code Rate to  */
  rxParam.setCodeRate(1);

  /* Send config to device */
  ret = nemeusLib.radio()->setRadioRxParam(rxParam);

  if(ret == NEMEUS_SUCCESS)
  {
    SerialUSB.println("RX radio parameters set!!");
  }
  else
  {
    SerialUSB.println("RX radio parameters set error!!");
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

  /* Set device in continuous RX mode */
  ret = nemeusLib.radio()->continuousRx();

}

void loop()
{
  nemeusLib.pollDevice(5000);
  nemeusLib.printTraces();
}

/* ---------------- Functions ---------------- */
void onReceive(const char *string)
{
  SerialUSB.print("mm002 >> ");
  SerialUSB.println(string);
}
