#include "MadgwickAHRS.h" // Modified to make make smaller, make quarts public and make input to updateIMU() in rad/s
#include <Adafruit_MPU6050.h> // Accelerometer
#include <Adafruit_Sensor.h> // Accelerometer
#include <Wire.h> // For I2C

#define SAMPLE_FREQ 100

// TODO: Update these as appropriate for your specific MPU
#define BIAS_GYRO_X 0.026
#define BIAS_GYRO_Y -0.034
#define BIAS_GYRO_Z 0.025

Madgwick filter; // To convert to roll, pitch, yaw values
Adafruit_MPU6050 mpu; // Accelerometer

unsigned long microsPerReading, microsPrevious;

void printQuatsToSerial(sensors_event_t a, sensors_event_t g);

void setup() {
  Serial.begin(115200);

  while (!Serial){delay(10);}

  Serial.println("Serial connection established.");
  Serial.println("Connecting to accelerometer.");

  // Try to initialize mpu6050
  if (!mpu.begin()) {
    while (1) {Serial.print('.'); delay(10);}
  }

  Serial.println("Accelerometer connected.");

  // set accelerometer range to +-2G (Options: 2, 4, 8, 16) 
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);

  // set low pass filter bandwidth to 260 Hz (Options: 5, 10, 21, 44, 94, 184, 260)
  mpu.setFilterBandwidth(MPU6050_BAND_260_HZ);

  filter.begin(SAMPLE_FREQ);

  delay(100);

  // initialize variables to pace updates to correct rate
  microsPerReading = 1000000 / SAMPLE_FREQ;
  microsPrevious = micros();
}

// ------------------------- MAIN LOOP ------------------------- //
void loop() {
  unsigned long microsNow = micros();

  if (microsNow - microsPrevious >= microsPerReading) {
    /* Get new sensor events with the readings */
    sensors_event_t a, g, _t; // Acceleration, gyroscopic reading, temperature (from MPU6050)
    mpu.getEvent(&a, &g, &_t);

    // Uncomment to help find biases
    // Serial.printf("gx: %f, gy: %f, gz: %f\n", g.gyro.x, g.gyro.y, g.gyro.z);

    printQuatsToSerial(a, g);

    // increment previous time, so we keep proper pace
    microsPrevious = microsPrevious + microsPerReading;
  }
}

// ------------------------ ^MAIN LOOP^ ------------------------ //

void printQuatsToSerial(sensors_event_t a, sensors_event_t g) {
  // Update quaternion values
  filter.updateIMU(
      g.gyro.x + BIAS_GYRO_X,
      g.gyro.y + BIAS_GYRO_Y,
      g.gyro.z + BIAS_GYRO_Z,
      a.acceleration.x,
      a.acceleration.y,
      a.acceleration.z
  );

  // Print in form (q0,q1,q2,q3)
  Serial.printf("(%f,%f,%f,%f)\n", filter.q0, filter.q1, filter.q2, filter.q3);
}
