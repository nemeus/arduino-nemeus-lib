/* Basic example for the GPS
 *
 *  Uses MicroNMEA Library
 *  In main loop, prints the coordinates
 *  as soon as a fix is found.
 *
 */

#include <MicroNMEA.h>

// Refer to streams logically
Stream & gps = Serial1;
Stream & console = SerialUSB;

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
int recPin = 0;  // the pin receiving the serial input data
long baudRate;   // global in case useful elsewhere in a sketch

void print_gps_fix(Stream & out);

void setup(void)
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

  SerialUSB.println(">>Sketch: Test GPS ");

  pinMode(GPS_RESET_PIN, OUTPUT);
  digitalWrite(GPS_RESET_PIN, HIGH);

  pinMode(recPin, INPUT);      // make sure serial in is a input pin
  digitalWrite (recPin, HIGH); // pull up enabled just for noise protection
  
  baudRate = detRate(recPin);  // Function finds a standard baudrate of either
                               // 1200,2400,4800,9600,14400,19200,28800,38400,57600,115200 
                               // Returns 0 if none or under 1200 baud  

  SerialUSB.print("Detected baudrate at ");
  SerialUSB.println(baudRate);

  /* GPS uart */
  Serial1.begin(baudRate);

  console.println("Resetting GPS module ...");
  gpsHardwareReset();
  console.println("... done");

  /* examples of command that can be sent to GPS */
//  MicroNMEA::sendSentence(gps,"$PSIMIPR,R");          // Get serial speed setting of GPS
//  MicroNMEA::sendSentence(gps,"$PSIMIPR,W,115200");   // Set serial speed setting of GPS (need reset to be applied) 
  
  /* Compatibility mode off */
  MicroNMEA::sendSentence(gps, "$PONME,,,1,0");

  /* Clear the list of messages which are sent. */
  MicroNMEA::sendSentence(gps, "$PORZB");

  /* Send RMC and GGA messages. */
  MicroNMEA::sendSentence(gps, "$PORZB,RMC,1,GGA,1");

  pinMode(GPS_PPS_PIN, INPUT_PULLUP);
  attachInterrupt(GPS_PPS_PIN, ppsHandler, RISING);
}

void loop(void)
{
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

