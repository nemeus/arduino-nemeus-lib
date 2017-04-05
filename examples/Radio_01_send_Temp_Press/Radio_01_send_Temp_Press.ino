/*This is an example for the Nemeus library using RF radio to send
 * temperature and pressure get from BMP085 Barometric Pressure & Temp Sensor
 * This example send on RF (Freq: 868100000 in LoRa mode) a frame every 5 seconds.
 * This frame is composed of :
 *     - Counter on 2 bytes
 *     - Temperature in deci°C on 2 bytes
 *     - Pressure in Pascal on 4 bytes

 */
#include <NemeusLib.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>

#define FRAME_SIZE 8

/* Define a counter variable */
uint16_t frameCounter = 0;
uint8_t ret;

Adafruit_BMP280 bmp;

void setup()
{
  /* serial monitor */
  SerialUSB.begin(115200);

  while(!SerialUSB)
  {
    ;        /*SerialUSB not ready */
  }

  SerialUSB.println(">>Sketch: Test Connexion OK ");

  /* Init nemeus library */
  if(nemeusLib.init() != NEMEUS_SUCCESS)
  {
    SerialUSB.print("Nemeus device is not responding!");
    while(1)
    {
    }
  }

  /* Init BMP180 sensor library */
  if(!bmp.begin())
  {

    /* There was a problem detecting the BMP280 ... check your connection */
    SerialUSB.println("Ooops, no BMP280 detected ... Check your wiring!");
    while(1)
    {
    }
  }

  /* Set RF Tx parameter (if needed) */
  /* This parameter is the default parameter. No need to set it except if you want to change. */
  RadioTxParam txParam;

  /* LoRa mode */
  txParam.setMode(RADIO_LORA_MODE);
  /* Tx Power */
  txParam.setTxPower(14);
  /* Tx Frequency tp 868100000 Hz */
  txParam.setFrequency(868100000);
  /* Bandwidth to 125000 Hz */
  txParam.setBandwidth(125000);
  /* Data Rate to 7 */
  txParam.setDataRate(7);
  /* Code Rate to  */
  txParam.setCodeRate(1);

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

}


void loop()
{
  char frameToSend[FRAME_SIZE];
  float temperature;
  int dTempToInt;
  int32_t pressure;
  int index = 0;

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

  /* Get temperature and pressure from BMP180 sensor */
  temperature = bmp.readTemperature();
  dTempToInt = (int) (temperature * 10);
  pressure = bmp.readPressure();

  SerialUSB.print("Temperature = ");
  SerialUSB.print(temperature);
  SerialUSB.println(" *C");

  SerialUSB.print("Pressure = ");
  SerialUSB.print(pressure);
  SerialUSB.println(" Pa");

  /* Format a frame */
  /* Counter on 2 bytes */
  /* Temperature integer (Temp*10) on 2 bytes */
  /* Pressure on 4 bytes */
  index = sprintf(frameToSend, "%04X", frameCounter);
  frameCounter++;
  index += sprintf(frameToSend + index, "%04X", dTempToInt);
  index += sprintf(frameToSend + index, "%08X", pressure);

  /* Send the payload in binary mode without repetition */
  ret = nemeusLib.radio()->sendFrame(0, frameToSend, 0);
  SerialUSB.print(">>Example: Response is : ");
  SerialUSB.println(ret);

  /* Turn OFF Radio */
  ret = nemeusLib.radio()->OFF();

  if(ret == NEMEUS_SUCCESS)
  {
    SerialUSB.println("RADIO OFF!!!");
  }
  else
  {
    SerialUSB.println("RADIO OFF command error!!");
  }

  delay(5000);
}
