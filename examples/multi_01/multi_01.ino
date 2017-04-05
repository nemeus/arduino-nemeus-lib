
/* Example used to test main features of the arduino board
 *
 *  BLE chip need to be uploaded with 'serial' binary first
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

#include <NemeusLib.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_ADXL345_U.h>
#include <avr/dtostrf.h>
#include "binary_const.h"  //This allows us to write the registers in binary. EX B8(01010101).

Adafruit_BMP280 bme;		// I2C
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

Stream & gps     = Serial1;
Stream & console = SerialUSB;
Stream & ble     = Serial;

#define LED_RED 5
#define LED_GREEN 3
#define LED_BLUE 2
#define BUTTON 11
#define FRAME_SIZE  12
/* Set LoRaWan mode (false => ABP true => OTAA) */
#define LORA_MODE false

#define BUF_SIZE 256

char buffer[BUF_SIZE];
uint8_t ret;

static int button_state = 0;
static int accel_state = 0;
static int bmp_state = 0;
static int sigfox_state = 0;
static int LoRa_state = 0;
static long button_state_length = 0;
static long button_tmp_time = 0;
uint16_t frameCounter = 0;


void isr_button();

void print_accel_data(Stream & out);
void print_bmp_data(Stream & out);
void print_gps_fix(Stream & out);

/* -------------------------- 
 GPS functions and variables  
 ----------------------------*/
#include <MicroNMEA.h>
#define GPS_PPS_PIN 6
#define GPS_RESET_PIN A0

char nmeaBuffer[85];
MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));
volatile bool ppsTriggered = false;
volatile bool fixFound     = false;

/* Interrupt wired on PPS pin */
void ppsHandler(void);

/* GPS reset routine */
void gpsHardwareReset();

/* ----------------------------------- 
 Accelerometer functions and variables  
 -------------------------------------*/

#define INTPIN    A1   // The arduino pin  connected to INT1 (ADXL345)
#define Device    (0x53)  // Address of ADXL345 with SD0 tied to ground and, SDA SC, SLC 3.3V.
#define Read      (6)   // Will read two bytes from x,y,z axis.

/*Register Map for accelerometer*/
const int R_DEVID = 0; // Device ID
const int R_THRESH_TAP = 29; //Tap threshold
const int R_OFSX = 30;  //X-axis offset
const int R_OFSY = 31;  //Y-axis offset
const int R_OFSZ = 32;  //Z-axis offset
const int R_DUR = 33;  //Tap duration
const int R_LATENT = 34;  //Tap latency
const int R_WINDOW = 35;  //Tap window
const int R_TAP_AXES = 42;  //Axis control for single tap/double tap
const int R_ACT_TAP_STATUS = 43;  //Source of single tap/double tap
const int R_BW_RATE = 44; //Data rate and power mode control
const int R_POWER_CTL = 45; //Power saving features control
const int R_INT_ENABLE = 46;  //Interrupt enable control
const int R_INT_MAP = 47; //Interrupt map control 
const int R_INT_SOURCE = 48;  //Source of interrupt
const int R_DATA_FORMAT = 49; //Data format control
const int R_DATAX0 = 50;  //X-Axis Data 0
const int R_DATAX1 = 51;  //X-Axis Data 1
const int R_DATAY0 = 52;  //Y-Axis Data 0
const int R_DATAY1 = 53;  //Y-Axis Data 1
const int R_DATAZ0 = 54;  //Z-Axis Data 0
const int R_DATAZ1 = 55;  //Z-Axis Data 1
const int R_FIFO_CTL = 56;  //FIFO control
const int R_FIFO_STATUS = 57; //FIFO status

/* Writes val to address register on Device */
void writeTo(int device, byte address, byte val);
/* Reads a single byte and returns the red value */
byte readByte(int device, byte address);

int recPin = 0;  // the pin receiving the serial input data
long baudRate;   // global in case useful elsewhere in a sketch


void setup()
{
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  /* Button is in pull down */
  pinMode(BUTTON, INPUT);
  pinMode(INTPIN, INPUT); // Makes INTPIN (PIN 2) input

  /* Detect button event */
  attachInterrupt(BUTTON, isr_button, CHANGE);

  /* serial monitor */
  SerialUSB.begin(115200);

#ifdef CONSOLE_CHECK
  while(!SerialUSB)
  {
    ;      /*SerialUSB not ready */
  }
  SerialUSB.println(">>Console Ready");
#endif

  SerialUSB.println("PROD testu");

  if(!bme.begin())
  {   
    /* There was a problem detecting the BMP280 ... check your connection */
    SerialUSB.println("Ooops, no BMP280 detected ... Check your wiring!");
    while(1);
  }

  /* Initialize the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connection */
    SerialUSB.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while(1);
  }

  Serial.begin(9600);

  while(!Serial)
  {
    ;       /*Serial not ready */
  }

  SerialUSB.println("Serial ready");

 /* INTERRUPTS FOR ACCELEROMETER*/
  writeTo(Device, R_INT_MAP, 0);  //Sends all interrupts to ADLX345 INT1
  writeTo(Device, R_INT_ENABLE, B8(01111100));  // enables single,double,inactivty,activity & free fall.

   /* single tap config */
  writeTo(Device, R_DUR, 0x1F);
  writeTo(Device, R_THRESH_TAP, 48);
  writeTo(Device, R_TAP_AXES, B8(111)); // enables tap detection in x,y,z axes

  /* double tap config */
  writeTo(Device, R_LATENT, 0x10);
  writeTo(Device, R_WINDOW, 0xFF);

  SerialUSB.println(">>Sketch: GPS configuration ");

  pinMode(GPS_RESET_PIN, OUTPUT);
  digitalWrite(GPS_RESET_PIN, HIGH);

  pinMode(recPin, INPUT);      // make sure serial in is a input pin
  digitalWrite (recPin, HIGH); // pull up enabled just for noise protection
  
  baudRate = detRate(recPin);  // Function finds a standard baudrate of either
                               // 1200,2400,4800,9600,14400,19200,28800,38400,57600,115200 
                               // by having sending circuit send "U" characters.
                               // Returns 0 if none or under 1200 baud  

  SerialUSB.print("Detected baudrate at ");
  SerialUSB.println(baudRate);

  /* GPS uart */
  Serial1.begin(baudRate);

  console.println("Resetting GPS module ...");
  gpsHardwareReset();
  console.println("... done");

  MicroNMEA::sendSentence(SerialUSB, "$PSIMIPR,R");
  
  /* Compatibility mode off */
  MicroNMEA::sendSentence(gps, "$PONME,,,1,0");

  /* Clear the list of messages which are sent. */
  MicroNMEA::sendSentence(gps, "$PORZB");

  /* Send RMC and GGA messages. */
  MicroNMEA::sendSentence(gps, "$PORZB,RMC,1,GGA,1");

  pinMode(GPS_PPS_PIN, INPUT_PULLUP);
  attachInterrupt(GPS_PPS_PIN, ppsHandler, RISING);


  /* Register a callback for reception */
  nemeusLib.register_at_response_callback(&onReceive);
  
  /* Reset the modem */
  if ( nemeusLib.resetModem() != NEMEUS_SUCCESS)
  {
    SerialUSB.println("Nemeus device is not responding!");
    while(1)
    {
    }
  }
  
  /* Init nemeus library */
  if(nemeusLib.init() != NEMEUS_SUCCESS)
  {
    SerialUSB.println("Nemeus device is not responding!");
    while(1)
    {
    }
  }

  /* Enable verbose traces */
  ret = nemeusLib.setVerbose(true);

  /* Turn ON Radio in LORA_MODE (false => ABP ; true => OTAA) */
  ret = nemeusLib.loraWan()->ON('A', LORA_MODE);
  
  if(ret == NEMEUS_SUCCESS)
  {
    SerialUSB.println("LoRaWAN ON - Class A !!!");
  }
  else
  {
    SerialUSB.println("LoRaWAN ON command error!!");
  }

   /* Turn ON Radio */
 // ret = nemeusLib.sigfox()->ON(NULL);
  /* Uncomment the following line to disable SigFox duty cycle (testing purpose only!) */
   ret = nemeusLib.sigfox()->ON(0);

  if(ret == NEMEUS_SUCCESS)
  {
    SerialUSB.println("Sigfox ON !!!");
  }
  else
  {
    SerialUSB.println("Sigfox ON command error !!!");
  }  

  
}

void loop()
{
  char rcv;
  int intSource = readByte(Device, R_INT_SOURCE);

  /* Handle TAP and DOUBLE-TAP */
  if(intSource & B8(100000))  /* DOUBLE-TAP detected */
  {
    if (accel_state == 0)
    {
      accel_state =1;
      print_accel_data(console);
      print_accel_data(ble);
    }
  }
  else if(intSource & B8(1000000)) /*SINGLE-TAP detected */
  {
    /* If double-tap has been detected we don't want to display single-tap information */
    if (bmp_state == 0)
    {
      bmp_state =1;
      print_bmp_data(console);
      print_bmp_data(ble);
    }
  }

  /* Follow button_state with Sigfox or LoRa frame sending depending on duration */
  if(button_state == 1)
  {
     button_state_length = millis() - button_tmp_time;
     delay(200);
     if (button_state_length < 500 && button_state == 0)
     {
       digitalWrite(LED_GREEN, HIGH);
       digitalWrite(LED_BLUE, LOW);
       digitalWrite(LED_RED, LOW);
     }
     if (button_state_length > 500 && button_state_length < 2000 && button_state == 0)
     {
       digitalWrite(LED_GREEN, LOW);
       digitalWrite(LED_BLUE, HIGH);
       digitalWrite(LED_RED, LOW);     
       if (sigfox_state  == 0)
       {
         sigfox_state = 1;
         send_sigfox_frame();
       }   
       
     }
     else if (button_state_length > 2000 )
     {
       digitalWrite(LED_GREEN, LOW);
       digitalWrite(LED_BLUE, LOW);
       digitalWrite(LED_RED, HIGH);
       if ( LoRa_state  == 0)
       {
         LoRa_state = 1;
         send_LoRa_frame();
       }    
     }

  }
  else
  {
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_RED, LOW);
    accel_state = 0;
    bmp_state = 0;
    sigfox_state = 0;
    LoRa_state = 0;
  }

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

  if(ppsTriggered)
  {
    ppsTriggered = false;
    if(nmea.isValid()) 
    {
      fixFound = true;
      print_gps_fix(console);
      nmea.clear();
    }
    else
    {
      fixFound = false;
    }
  }

  while(!ppsTriggered && gps.available())
  {
    char c = gps.read();
    if (fixFound == false)
    {
      console.print(c);
    }
    nmea.process(c);
  }

}

/* ---------------- Functions ---------------- */

void isr_button()
{
  if(digitalRead(BUTTON))
  {
    button_state = 1;
    button_tmp_time = millis();
  }
  else
  {
    button_state = 0;
  }
}

void print_accel_data(Stream & out)
{
  /* Get a new sensor event */
  sensors_event_t event;

  accel.getEvent(&event);

  out.println("<--- ACCEL ---");
  /* Display the results (acceleration is measured in m/s^2) */
  out.print("X: ");
  out.print(event.acceleration.x);
  out.print("  ");
  out.print("Y: ");
  out.print(event.acceleration.y);
  out.print("  ");
  out.print("Z: ");
  out.print(event.acceleration.z);
  out.print("  ");
  out.println("m/s^2 ");
}

void print_bmp_data(Stream & out)
{
  char str_tmp[10];
  int index=0;

  out.println("<--- BMP280 ---");
  
  dtostrf(bme.readTemperature(), 4, 2, str_tmp);   /* 4 is mininum width, 2 is precision; float value is copied onto str_tmp*/
  index = sprintf(buffer,"T: %s C°\n", str_tmp);
  dtostrf(bme.readPressure(), 4, 2, str_tmp);   /* 4 is mininum width, 2 is precision; float value is copied onto str_tmp*/
  index += sprintf(buffer+index,"P: %s Pa\n", str_tmp);
  dtostrf(bme.readAltitude(), 4, 2, str_tmp);   /* 4 is mininum width, 2 is precision; float value is copied onto str_tmp*/
  index += sprintf(buffer+index,"A: %s m\n", str_tmp);
  out.println(buffer);
}

void ppsHandler(void)
{
  ppsTriggered = true;
}

void gpsHardwareReset()
{
  /* Empty input buffer */
  while(gps.available())
  {
    gps.read();
  }

  digitalWrite(GPS_RESET_PIN, LOW);
  delay(50);
  digitalWrite(GPS_RESET_PIN, HIGH);

  /* Reset is complete when the first
     valid message is received */
  while(1)
  {
    while(gps.available())
    {
      char c = gps.read();

      if(nmea.process(c))
      {
        return;
      }
    }
  }
}

void print_gps_fix(Stream & out)
{
  /* Output GPS information from previous second */
  out.print("Valid fix: ");
  out.println(nmea.isValid()? "yes" : "no");

  out.print("Nav. system: ");
  if(nmea.getNavSystem())
  {
    out.println(nmea.getNavSystem());
  }
  else
  {
    out.println("none");
  }

  out.print("Num. satellites: ");
  out.println(nmea.getNumSatellites());

  out.print("HDOP: ");
  out.println(nmea.getHDOP() / 10., 1);

  out.print("Date/time: ");
  out.print(nmea.getYear());
  out.print('-');
  out.print(int (nmea.getMonth()));

  out.print('-');
  out.print(int (nmea.getDay()));

  out.print('T');
  out.print(int (nmea.getHour()));

  out.print(':');
  out.print(int (nmea.getMinute()));

  out.print(':');
  out.println(int (nmea.getSecond()));

  long latitude_mdeg = nmea.getLatitude();
  long longitude_mdeg = nmea.getLongitude();

  out.print("Latitude (deg): ");
  out.println(latitude_mdeg / 1000000., 6);

  out.print("Longitude (deg): ");
  out.println(longitude_mdeg / 1000000., 6);

  long alt;

  out.print("Altitude (m): ");
  if(nmea.getAltitude(alt))
  {
    out.println(alt / 1000., 3);
  }
  else
  {
    out.println("not available");
  }

  /* no flag indication of validity for course/speed 
     Not valid if value is LONG_MIN */
  if(nmea.getSpeed() != LONG_MIN)
  {
    out.print("Speed: ");
    out.println(nmea.getSpeed() / 1000., 3);
  }

  if(nmea.getCourse() != LONG_MIN)
  {
    out.print("Course: ");
    out.println(nmea.getCourse() / 1000., 3);
  }
  out.println("-----------------------");
}

long detRate(int recpin)  // function to return valid received baud rate
                          // Note that the serial monitor has no 600 baud option and 300 baud
                          // doesn't seem to work with version 22 hardware serial library
{
  long baud, rate = 10000, x;
  console.println("Waiting GPS serial characters...");
  while (pulseIn(recpin, HIGH) == 0);
  for (int i = 0; i < 10; i++) {
      x = pulseIn(recpin, HIGH);
      rate = (x < rate) ? x : rate;
//      console.println(x);
  }
   
  if (rate < 12)
  {
    baud = 115200;
  }
  else if (rate < 20)
  {
    baud = 57600;
  }
  else if (rate < 29)
  {
    baud = 38400;
  }
  else if (rate < 40)
  {
    baud = 28800;
  }
  else if (rate < 60)
  {
    baud = 19200;
  }
  else if (rate < 80)
  {
    baud = 14400;
  }
  else if (rate < 150)
  {
    baud = 9600;
  }
  else if (rate < 300)
  {
    baud = 4800;
  }
  else if (rate < 600)
  {
    baud = 2400;
  }
  else if (rate < 1200)
  {
    baud = 1200;
  }
  else
  {
    baud = 0;  
  }
  return baud;
} 

void onReceive(const char *string)
{
  SerialUSB.print("mm002 >> ");
  SerialUSB.println(string);
}


void send_sigfox_frame()
{
  char frameToSend[FRAME_SIZE];
  float temperature;
  int dTempToInt;
  int32_t pressure;
  int index = 0;
  SerialUSB.println("Sending frame using SIGFOX");

  nemeusLib.pollDevice(5000);

  temperature = bme.readTemperature();
  dTempToInt = (int) (temperature * 10);
  pressure = bme.readPressure();

  SerialUSB.print("Temperature = ");
  SerialUSB.print(temperature);
  SerialUSB.println(" *C");

  SerialUSB.print("Pressure = ");
  SerialUSB.print(pressure);
  SerialUSB.println(" Pa");

  /* Format a frame */
  /* Temperature integer (Temp*10) on 2 bytes */
  /* Pressure on 4 bytes */

  index += sprintf(frameToSend + index, "%04X", dTempToInt);
  index += sprintf(frameToSend + index, "%08X", pressure);
  
  /* Send a frame (BINARY mode, Repetition = 1, mac Port = 1, ACK,  Encrypted) */
  ret = nemeusLib.sigfox()->sendFrame(0,frameToSend, 1);
}

void send_LoRa_frame()
{
  char frameToSend[FRAME_SIZE];
  float temperature;
  int dTempToInt;
  int32_t pressure;
  int index = 0;
  SerialUSB.println("Sending frame using LoRaWAN");

  nemeusLib.pollDevice(5000);
  temperature = bme.readTemperature();
  dTempToInt = (int) (temperature * 10);
  pressure = bme.readPressure();

  SerialUSB.print("Temperature = ");
  SerialUSB.print(temperature);
  SerialUSB.println(" *C");

  SerialUSB.print("Pressure = ");
  SerialUSB.print(pressure);
  SerialUSB.println(" Pa");

  /* Format a frame */
  /* Temperature integer (Temp*10) on 2 bytes */
  /* Pressure on 4 bytes */

  index += sprintf(frameToSend + index, "%04X", dTempToInt);
  index += sprintf(frameToSend + index, "%08X", pressure);

  /* Send a frame (BINARY mode, Repetition = 1, mac Port = 1, ACK,  Encrypted) */
  ret = nemeusLib.loraWan()->sendFrame(0, 1, 1, frameToSend, 1, 1);
}


/* Writes val to address register on Device */
void writeTo(int device, byte address, byte val)
{
  Wire.beginTransmission(device); //start transmission to Device
  Wire.write(address);    // send register address
  Wire.write(val);    // send value to write
  Wire.endTransmission(); //end transmission
}


/* read a single byte and returns the read value */
byte readByte(int device, byte address)
{
  Wire.beginTransmission(device); //start transmission to Device
  Wire.write(address);    //sends address to read from
  Wire.endTransmission(); //end transmission

  Wire.beginTransmission(device); //start transmission to Device
  Wire.requestFrom(device, 1);  // request 1 byte from Device

  int read = 0;

  if(Wire.available())
  {
    read = Wire.read(); // receive a byte
  }
  Wire.endTransmission(); //end transmission
  return read;
}

