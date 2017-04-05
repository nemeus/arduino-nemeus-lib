/* Basic example with interrupt for the accelerometer
 *
 *  Uses Adafruit_ADXL345 Library
 *  In main loop, handles TAP, double TAP, activity, inactivity and free fall
 *
 */

#include <Wire.h>
#include <NemeusLib.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

#include "binary_const.h"  //This allows us to write the registers in binary. EX B8(01010101).

#define Device    (0x53)  // Address of ADXL345 with SD0 tied to ground and, SDA SC, SLC 3.3V.
#define Read      (6)   // Will read two bytes from x,y,z axis.

#define INTPIN    A1   // The arduino pin  connected to INT1 (ADXL345)
#define LEDINACT  2   // Arduino pin 2 (BLUE LED) will turn on when inactivity is detected
#define LEDST     3   // Arduino pin 3 (GREEN LED) will turn on when Single Tap is detected
#define LEDACT    5   // Arduino pin 5 (RED LED) will turn on when activity is detected


/*Register Map*/
const int R_DEVID = 0; // Device ID
const int R_THRESH_TAP = 29; //Tap threshold
const int R_OFSX = 30;  //X-axis offset
const int R_OFSY = 31;  //Y-axis offset
const int R_OFSZ = 32;  //Z-axis offset
const int R_DUR = 33;  //Tap duration
const int R_LATENT = 34;  //Tap latency
const int R_WINDOW = 35;  //Tap window
const int R_THRESH_ACT = 36;  //Activity threshold
const int R_THRESH_INACT = 37;  //Inactivity threshold
const int R_TIME_INACT = 38;  //Inactivity time
const int R_ACT_INACT_CTL = 39; //Axis enable control for activity and inactivity detection
const int R_THRESH_FF = 40; //Free-fall threshold
const int R_TIME_FF = 41; //Free-fall time
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

/*Register Map*/
byte buff[Read];    //Read = 6 and buff will store 6 bytes "BUFFER"
char str[512];      // Turns data into a string to send to Serial Port "BUFFER"

/* Define a counter variable */
uint16_t frameCounter = 0;
uint8_t ret;

/* Writes val to address register on Device */
void writeTo(int device, byte address, byte val);
/* Reads num bytes starting from address register on Device into buff array */
void readFrom(int device, byte address, int num, byte buff[]);
/* Reads a single byte and returns the red value */
byte readByte(int device, byte address);
/* Displays the taped axes */
void printTapAxes();

/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

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

  SerialUSB.println(">>Sketch: Accelerometer Test (tap, double tab, activity, inactivity and free fall)");
  Wire.begin();

  /* Initialize the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    SerialUSB.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while(1);
  }

  /* Set input and outputs */
  pinMode(INTPIN, INPUT); // Makes INTPIN (PIN 2) input
  pinMode(LEDINACT, OUTPUT);
  pinMode(LEDACT, OUTPUT);
  pinMode(LEDST, OUTPUT);

  /* INTERRUPTS */
  writeTo(Device, R_INT_MAP, 0);  //Sends all interrupts to ADLX345 INT1
  writeTo(Device, R_INT_ENABLE, B8(01111100));  // enables single,double,inactivty,activity & free fall.

  /* free fall config */
  writeTo(Device, R_TIME_FF, 0x14); // set free fall time value as datasheet suggested
  writeTo(Device, R_THRESH_FF, 0x05); // set free fall threshold as datasheet suggested

  /* single tap config */
  writeTo(Device, R_DUR, 0x1F);
  writeTo(Device, R_THRESH_TAP, 48);
  writeTo(Device, R_TAP_AXES, B8(111)); // enables tap detection in x,y,z axes

  /* double tap config */
  writeTo(Device, R_LATENT, 0x10);
  writeTo(Device, R_WINDOW, 0xFF);

  /* inactivity config */
  writeTo(Device, R_TIME_INACT, 10);  // 1s / LSB
  writeTo(Device, R_THRESH_INACT, 3); // 62.5mg / LSB

  /* activity config */
  writeTo(Device, R_THRESH_ACT, 8); // 62.5mg/ LSB

  /* activity and inactivity control */
  writeTo(Device, R_ACT_INACT_CTL, B8(11111111)); // enables inactivity and activity on x,z,y using ac-coupled operation

  /* set the ADXL345 in measurment and sleep mode-save power but still detect activity
     Link bit is set to 1 so inactivity and activity aren't concurrent */
  writeTo(Device, R_POWER_CTL, B8(111100));
}

void loop()
{

    int intSource = readByte(Device, R_INT_SOURCE);

    if(intSource & B8(100))
    {
      digitalWrite(LEDACT, HIGH);
      digitalWrite(LEDST, HIGH);
      SerialUSB.println("** FREE FALL! **");  
    }

    if(intSource & B8(1000))
    {
      SerialUSB.println("** Inactivity **");
      digitalWrite(LEDINACT, HIGH);
      int bwRate = readByte(Device, R_BW_RATE);

      writeTo(Device, R_BW_RATE, bwRate | B8(10000));
    }
    else
    {
      digitalWrite(LEDINACT, LOW);
    }

    if(intSource & B8(10000))
    {
      SerialUSB.println("** Activity **");
      int powerCTL = readByte(Device, R_POWER_CTL);

      writeTo(Device, R_POWER_CTL, powerCTL & B8(11110011));  // Keeps everything the same execpt turns sleep off and measurment off
      delay(10);
      writeTo(Device, R_POWER_CTL, powerCTL & B8(11111011));  // Turns measurement mode on. Everything else is the same
      int bwRate = readByte(Device, R_BW_RATE);

      writeTo(Device, R_BW_RATE, bwRate & B8(01111));
      digitalWrite(LEDACT, HIGH);
    }
    else      //(ACT =1)
    {
      digitalWrite(LEDACT, LOW);
    }

    if(intSource & B8(100000))
    {
      digitalWrite(LEDINACT, HIGH);
      digitalWrite(LEDACT, HIGH);
      digitalWrite(LEDST, HIGH);
      SerialUSB.println("** Double Tap **");
      printTapAxes();
      SerialUSB.println("");
    }
    else if(intSource & B8(1000000))
    {
      /* If double-tap has been detected we don't want to display single-tap information */
      digitalWrite(LEDST, HIGH);
      SerialUSB.println("** Single Tap **");
      printTapAxes();
      SerialUSB.println("");
    }
    else
    {
      digitalWrite(LEDST, LOW);
    }
    delay(150);

}


/* ---------------- Functions ---------------- */

/* Writes val to address register on Device */
void writeTo(int device, byte address, byte val)
{
  Wire.beginTransmission(device); //start transmission to Device
  Wire.write(address);    // send register address
  Wire.write(val);    // send value to write
  Wire.endTransmission(); //end transmission
}

/* reads num bytes starting from address register on Device in to buff array */
void readFrom(int device, byte address, int num, byte buff[])
{
  Wire.beginTransmission(device); //start transmission to Device
  Wire.write(address);    //sends address to read from
  Wire.endTransmission(); //end transmission

  Wire.beginTransmission(device); //start transmission to Device
  Wire.requestFrom(device, num);  // request 6 bytes from Device

  int i = 0;

  while(Wire.available()) //Device may send less than requested (abnormal)
  {
    buff[i] = Wire.read();  // receive a byte
    i++;
  }

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

/* Displays the taped axes */
void printTapAxes()
{
  int tapStatus = readByte(Device, R_ACT_TAP_STATUS);

  if(tapStatus & B8(100))
  {
    SerialUSB.print("x ");
  }

  if(tapStatus & B8(10))
  {
    SerialUSB.print("y ");
  }

  if(tapStatus & B8(1))
  {
    SerialUSB.print("z ");
  }
}
