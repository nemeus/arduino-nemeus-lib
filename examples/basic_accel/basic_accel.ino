/* Basic example for the accelerometer
 *
 *  Uses Adafruit_ADXL345 Library
 *  In main loop, prints the current
 *  acceleration on 3 axes to the serial
 *  monitor.
 *
 */

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

/* Display sensor's information like name, version, id, resolution, max and min values*/
void displaySensorDetails(void);
/* Display the data rate */
void displayDataRate(void);
/* Display the  range (2g, 4g, 8g or 16g)*/
void displayRange(void);


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

  SerialUSB.println(">>Sketch: Accelerometer Test");

  /* Initialize the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connection */
    SerialUSB.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while(1);
  }

  /* Set the range to whatever is appropriate for your project */
  accel.setRange(ADXL345_RANGE_16_G);
  // displaySetRange(ADXL345_RANGE_8_G);
  // displaySetRange(ADXL345_RANGE_4_G);
  // displaySetRange(ADXL345_RANGE_2_G);

  /* Display some basic information on this sensor */
  displaySensorDetails();

  /* Display additional settings (outside the scope of sensor_t) */
  displayDataRate();
  displayRange();
  SerialUSB.println("");
}

void loop(void)
{
  /* Get a new sensor event */
  sensors_event_t event;

  accel.getEvent(&event);

  /* Display the results (acceleration is measured in m/s^2) */
  SerialUSB.print("X: ");
  SerialUSB.print(event.acceleration.x);
  SerialUSB.print("  ");
  SerialUSB.print("Y: ");
  SerialUSB.print(event.acceleration.y);
  SerialUSB.print("  ");
  SerialUSB.print("Z: ");
  SerialUSB.print(event.acceleration.z);
  SerialUSB.print("  ");
  SerialUSB.println("m/s^2 ");
  delay(500);
}

/* ---------------- Functions ---------------- */

void displaySensorDetails(void)
{
  sensor_t sensor;

  accel.getSensor(&sensor);
  SerialUSB.println("------------------------------------");
  SerialUSB.print("Sensor:       ");
  SerialUSB.println(sensor.name);
  SerialUSB.print("Driver Ver:   ");
  SerialUSB.println(sensor.version);
  SerialUSB.print("Unique ID:    ");
  SerialUSB.println(sensor.sensor_id);
  SerialUSB.print("Max Value:    ");
  SerialUSB.print(sensor.max_value);
  SerialUSB.println(" m/s^2");
  SerialUSB.print("Min Value:    ");
  SerialUSB.print(sensor.min_value);
  SerialUSB.println(" m/s^2");
  SerialUSB.print("Resolution:   ");
  SerialUSB.print(sensor.resolution);
  SerialUSB.println(" m/s^2");
  SerialUSB.println("------------------------------------");
  SerialUSB.println("");
  delay(500);
}


void displayDataRate(void)
{
  SerialUSB.print("Data Rate:    ");

  switch (accel.getDataRate())
  {
    case ADXL345_DATARATE_3200_HZ:
      SerialUSB.print("3200 ");
      break;
    case ADXL345_DATARATE_1600_HZ:
      SerialUSB.print("1600 ");
      break;
    case ADXL345_DATARATE_800_HZ:
      SerialUSB.print("800 ");
      break;
    case ADXL345_DATARATE_400_HZ:
      SerialUSB.print("400 ");
      break;
    case ADXL345_DATARATE_200_HZ:
      SerialUSB.print("200 ");
      break;
    case ADXL345_DATARATE_100_HZ:
      SerialUSB.print("100 ");
      break;
    case ADXL345_DATARATE_50_HZ:
      SerialUSB.print("50 ");
      break;
    case ADXL345_DATARATE_25_HZ:
      SerialUSB.print("25 ");
      break;
    case ADXL345_DATARATE_12_5_HZ:
      SerialUSB.print("12.5 ");
      break;
    case ADXL345_DATARATE_6_25HZ:
      SerialUSB.print("6.25 ");
      break;
    case ADXL345_DATARATE_3_13_HZ:
      SerialUSB.print("3.13 ");
      break;
    case ADXL345_DATARATE_1_56_HZ:
      SerialUSB.print("1.56 ");
      break;
    case ADXL345_DATARATE_0_78_HZ:
      SerialUSB.print("0.78 ");
      break;
    case ADXL345_DATARATE_0_39_HZ:
      SerialUSB.print("0.39 ");
      break;
    case ADXL345_DATARATE_0_20_HZ:
      SerialUSB.print("0.20 ");
      break;
    case ADXL345_DATARATE_0_10_HZ:
      SerialUSB.print("0.10 ");
      break;
    default:
      SerialUSB.print("???? ");
      break;
  }
  SerialUSB.println(" Hz");
}

void displayRange(void)
{
  SerialUSB.print("Range:         +/- ");

  switch (accel.getRange())
  {
    case ADXL345_RANGE_16_G:
      SerialUSB.print("16 ");
      break;
    case ADXL345_RANGE_8_G:
      SerialUSB.print("8 ");
      break;
    case ADXL345_RANGE_4_G:
      SerialUSB.print("4 ");
      break;
    case ADXL345_RANGE_2_G:
      SerialUSB.print("2 ");
      break;
    default:
      SerialUSB.print("?? ");
      break;
  }
  SerialUSB.println(" g");
}

