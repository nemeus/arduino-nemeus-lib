/* Basic example for Sigfox
 *
 *  Uses Nemeus Library
 *  In main loop,sends frames using sigfox.
 *
 */


#include <NemeusLib.h>
#include <Wire.h>

#define MAXPAYLOADSIZE 32
#define BUTTON 11

/* Define a counter variable */
uint16_t frameCounter = 0;
uint8_t ret;

char pattern[4] = { 'C', 'A', 'F', 'E' };
static int button_state = 0;
static long button_tmp_time = 0;
static long rem_time = 0;
static long period = 720000;
static int waiting_state = 0;

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

  SerialUSB.println(">>Sketch: Sending frames using SIGFOX ");

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

  /* Turn ON Radio */
  ret = nemeusLib.sigfox()->ON(NULL);
  /* Uncomment the followingline to disable SigFox duty cycle (testing purpose only!) */
  // ret = nemeusLib.sigfox()->ON('0');

  if(ret == NEMEUS_SUCCESS)
  {
    SerialUSB.println("Sigfox ON !!!");
  }
  else
  {
    SerialUSB.println("Sigfox ON command error !!!");
  }
  /* Get Maximum payload */
  int maxPayloadSize = nemeusLib.sigfox()->getMaximumPayloadSize();

  static char buffer[MAXPAYLOADSIZE];

  for (int i = 0; i < maxPayloadSize; i++)
  {
    buffer[i] = pattern[i % 4];
  }

  buffer[maxPayloadSize] = 0;
  /* Send a frame (BINARY mode, Repetition = 1, mac Port = 1, NO ACK,  Encrypted) */
  ret = nemeusLib.sigfox()->sendFrame(0,buffer, 0);
  button_tmp_time = 0;
  nemeusLib.pollDevice(5000);
  nemeusLib.printTraces();

  /* Button is in pull down */
  pinMode(BUTTON, INPUT);

  /* Detect button event */
  attachInterrupt(BUTTON, isr_button, CHANGE);

}

void loop()
{

  if(button_state == 1)
  {
    if(button_tmp_time < period && waiting_state ==1)
    {
      SerialUSB.println("There is a duty cycle limitation"); 
      waiting_state=0;
    }
    else if (button_tmp_time > period)
    {
      nemeusLib.pollDevice(5000);

      /* Get Maximum payload */
      int maxPayloadSize = nemeusLib.sigfox()->getMaximumPayloadSize();

      static char buffer[MAXPAYLOADSIZE];

      for (int i = 0; i < 2*maxPayloadSize; i++)
      {
        buffer[i] = pattern[i % 4];
      }

      buffer[2*maxPayloadSize] = 0;
      /* Send a frame (BINARY mode, Repetition = 1, mac Port = 1, NO ACK,  Encrypted) */
      ret = nemeusLib.sigfox()->sendFrame(0, buffer, 0);
      period = period + 720000;
      nemeusLib.pollDevice(5000);
      nemeusLib.printTraces();
    }

    button_tmp_time = millis();
  }

}

/* ---------------- Functions ---------------- */

void onReceive(const char *string)
{
  SerialUSB.print("mm002 >> ");
  SerialUSB.println(string);
}

void isr_button()
{
  if(digitalRead(BUTTON))
  {
    button_state = 1;
    waiting_state = 1;

  }
  else
  {
    button_state = 0;
    // waiting_state = 0;
  }
}
