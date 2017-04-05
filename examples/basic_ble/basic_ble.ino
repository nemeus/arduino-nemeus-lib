/* Basic example for the BLE chip
 *
 *  You need a master BLE (Android phone, for instance)
 *  You can use "nRF UART 2.0" app from Android store.
 *
 *  Displays received BLE received characters
 *  to the serial monitor, and loopback them.
 *  Sends received characters from serial monitor
 *  to BLE master.
 *
 */

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

  SerialUSB.println("BLE test");

  Serial.begin(9600);

  while(!Serial)
  {
    ;       /*Serial not ready */
  }

  SerialUSB.println("Serial ready");
}

void loop()
{
  char rcv;

  /* Check if we received something from BLE chip */
  while(Serial.available())
  {
    rcv = Serial.read();
    /* Display it */
    SerialUSB.print(rcv);
    /* loopback it to BLE chip */
    Serial.print(rcv);
  }

  /* take a look on serial monitor */
  while(SerialUSB.available())
  {
    rcv = SerialUSB.read();

    /* Echo it to serial monitor */
    SerialUSB.print(rcv);
    /* Send it to BLE chip */
    Serial.print(rcv);
  }
}
