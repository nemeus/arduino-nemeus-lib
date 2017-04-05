/* Example for geting the device personnal parameters
 *
 *  Uses Nemeus Library
 *  In main loop, reads the device parameters 
 *  and prints them on the serial monitor 
 *
 */

#include <NemeusLib.h>

#include <Wire.h>
#undef min
#undef max
#include <string>
#include <WString.h>

using namespace std;

uint8_t ret;
int nb_bytes;
bool callback = true;

/* Reception callback for RF frames */
void onReceive(const char *string);

/* Print traces to serial monitor */
void printTraces();

/* Print OTAA and ABP paramaeters to serial monitor*/
void printParam();

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

  SerialUSB.println(">>Sketch: Read device personnal parameters ");

  /* Register a callback for reception */
  nemeusLib.register_at_response_callback(&onReceive);

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

  /* Uncomment following line if you want to enable verbose traces */ 
  //ret = nemeusLib.setVerbose(true);

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

   /* Print Personnal OTAA and ABP parameters */
      printParam();
  
  /* Turn off LoRaWAN  */
  ret = nemeusLib.loraWan()->OFF();
}

void loop()
{
  nemeusLib.pollDevice(5000);
  /* Uncomment following line if you want to print traces */
  //printTraces();
}

/* ---------------- Functions ---------------- */

void onReceive(const char *string)
{
    SerialUSB.print("mm002 >> ");
    SerialUSB.println(string);
}

void printTraces()
{
  const int BUFFER_SIZE = 256;
  char buffer[BUFFER_SIZE];

  while(nemeusLib.availableTraces() > 0)
  {
    SerialUSB.print("mm002 >> .");
    nb_bytes = nemeusLib.readLine(buffer, BUFFER_SIZE);
    SerialUSB.write(buffer, nb_bytes);
  }

}

void printParam()
{
  /* Read personnal parameters */
  DevPerso_t *devPerso = nemeusLib.loraWan()->readDevPerso();
  SerialUSB.print(">>OTAA provisionning: ");
  SerialUSB.print("\n  devUID: ");
  SerialUSB.println(devPerso->devUID);
  SerialUSB.print("  appUID: ");
  SerialUSB.println(devPerso->appUID);
  SerialUSB.print("  appKey: ");
  SerialUSB.println(devPerso->appKey);
  SerialUSB.print(">>ABP provisionning: ");
  SerialUSB.print("\n  devAddr: ");
  SerialUSB.println(devPerso->devAddr);
  SerialUSB.print("  appSKey: ");
  SerialUSB.println(devPerso->appSKey);
  SerialUSB.print("  nwkSKey: ");
  SerialUSB.println(devPerso->nwkSKey);
  
}

