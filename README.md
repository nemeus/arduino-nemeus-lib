# arduino-nemeus-lib
Nemeus Library for Arduino (Smart IoT Sensor Architecture)

## Smart IoT Sensor
The Smart IOT sensor is dedicated to prototyping IoT applications using wireless technologies such as SigFox™, LoRa™, Bluetooth™ Low Energy, GPS/GNSS. In order to offer the fastest development time for application’s proof of concept, this product is 100% compatible with the Arduino IDE.
The board is made out of an host Atmel ARM Cortex M0+ MCU directly driving on-market, industry certified modules to enable connectivity. Sensors such as BMP280 (temperature/pressure) and ADXL-345 (accelerometer) are directly available on the board to meet user's application, as well as GPIO, including SPI and I2C to cover the widest range of peripherals. 

## Examples 

### LoRaWAN_ReadDevPerso 
Example for getting the device personnal parameters.
### LoRa_01_send_frame_in_ABP
Example sending frame in main loop using LoRaWAN in ABP mode. 
### LoRa_02_send_frame_OTAA
Example sending frame in main loop using LoRaWAN in OTAA mode.
### Radio_01_send_Temp_Press
Example using RF radio to send temperature and pressure get from BMP085 Barometric Pressure & Temp Sensor.
### Radio_02_receive_RF_frame
Example using RF radio to receive LoRa frames.
### Sigfox_01_send_frame
Example sending frame in main loop using Sigfox.
### basic_accel
Example for the accelerometer printing in main loop the current acceleration on 3 axes to the serial.
### basic_ble
Displays BLE received characters to the serial monitor, and loopback them. Sends received characters from serial monitor to BLE master.
### basic_gps
Example for the GPS printing the coordonates as soon as a fix is found.
### basic_pushbutton
Example for the Push Button lighting up the LED when button is pressed.
### basic_radio_fsk_cont_tx
Example using RF radio to perform continuous FSK transmit.
### basic_rgbled
Blinks quickly every color in loop and print it to the serial monitor.
### basic_temp
Prints temperature, pressure and altitude to the serial monitor.
### multi_01
Example used to test main features of the arduino board.





