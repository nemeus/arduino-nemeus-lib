/* Basic example for temperature & pressure sensor
 *
 *  Uses Adafruit_BMP280 library.
 *  Simply prints temperature, pressure and
 *  altitude to the serial monitor.
 *
 */

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bme;		// I2C

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

  SerialUSB.println(">>Sketch: BMP280 test");

  if(!bme.begin())
  {
    /* There was a problem detecting the BMP280 ... check your connection */
    SerialUSB.println("Ooops, no BMP280 detected ... Check your wiring!");
    while(1);
  }
}

void loop()
{
  SerialUSB.print("Temperature = ");
  SerialUSB.print(bme.readTemperature());
  SerialUSB.println(" *C");

  SerialUSB.print("Pressure = ");
  SerialUSB.print(bme.readPressure());
  SerialUSB.println(" Pa");

  SerialUSB.print("Approx altitude = ");
  /* this should be adjusted to your local forcase */
  SerialUSB.print(bme.readAltitude(1013.25));
  SerialUSB.println(" m");

  SerialUSB.println();
  delay(2000);
}
