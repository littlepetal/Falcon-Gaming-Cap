/**
   What is this file?
   This is an arduino example that calculates accurate roll,pitch,yaw from raw gyro/accelerometer data
   It has a calibration stage which removes most of the gyro drift and a complementary filter
   that combines gyro and accelerometer angles to produce roll/pitch angles that don't drift (like the gyro angle) and aren't noisy
   (like the accel angles). As there is no magnetic compass on the nano iot 33, it's not possible to 'complement' the yaw
   - hence yaw will drift and is 'relative'.
*/


// Sketch from : https://github.com/arduino-libraries/Arduino_LSM6DS3/blob/5eac7f5e6145c4747da27698faf3a548d2893a2b/examples/RollPitchYaw/RollPitchYaw.ino

#include <Arduino_LSM6DS3.h>
//#include <Wire.h>

// Filter library from: https://github.com/MartinBloedorn/libFilter
#include <filters.h>


float accelX,            accelY,             accelZ,            // units m/s/s i.e. accelZ if often 9.8 (gravity)
      gyroX,             gyroY,              gyroZ,             // units dps (degrees per second)
      gyroDriftX,        gyroDriftY,         gyroDriftZ,        // units dps
      gyroRoll,          gyroPitch,          gyroYaw,           // units degrees (expect major drift)
      gyroCorrectedRoll, gyroCorrectedPitch, gyroCorrectedYaw,  // units degrees (expect minor drift)
      accRoll,           accPitch,           accYaw,            // units degrees (roll and pitch noisy, yaw not possible)
      complementaryRoll, complementaryPitch, complementaryYaw;  // units degrees (excellent roll, pitch, yaw minor drift)

long lastTime;
long lastInterval;

// Filter values
float filterRoll, filterPitch, filterYaw;

// Define filter specifications
const float cutoff_freq   = 5.0;  //Cutoff frequency in Hz          maybe set this higher because the data isn't very noisy, might decrese the phase lag
const float sampling_time = 0.005; //Sampling time in seconds.      not sure how this works but wouldn't it make sense to use 10ms?   from the update 100Hz rate
IIR::ORDER  order  = IIR::ORDER::OD3; // Order (OD1 to OD4)         

Filter lowpass_filter(cutoff_freq, sampling_time, order);


void setup() {

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  while (!Serial);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  calibrateIMU(250, 250);

  lastTime = micros();

}

/*
  the gyro's x,y,z values drift by a steady amount. if we measure this when arduino is still
  we can correct the drift when doing real measurements later
*/
void calibrateIMU(int delayMillis, int calibrationMillis) {

  int calibrationCount = 0;

  delay(delayMillis); // to avoid shakes after pressing reset button

  float sumX, sumY, sumZ;
  int startTime = millis();
  while (millis() < startTime + calibrationMillis) {
    if (readIMU()) {
      // in an ideal world gyroX/Y/Z == 0, anything higher or lower represents drift
      sumX += gyroX;
      sumY += gyroY;
      sumZ += gyroZ;

      calibrationCount++;
    }
  }

  if (calibrationCount == 0) {
    Serial.println("Failed to calibrate");
  }

  gyroDriftX = sumX / calibrationCount;
  gyroDriftY = sumY / calibrationCount;
  gyroDriftZ = sumZ / calibrationCount;

}

/**
   Read accel and gyro data.
   returns true if value is 'new' and false if IMU is returning old cached data
*/
bool readIMU() {
  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable() ) {
    IMU.readAcceleration(accelX, accelY, accelZ);
    IMU.readGyroscope(gyroX, gyroY, gyroZ);
    return true;
  }
  return false;
}

// the loop function runs over and over again forever
void loop() {

  if (readIMU()) {
    long currentTime = micros();
    lastInterval = currentTime - lastTime; // expecting this to be ~104Hz +- 4%
//    Serial.println(lastInterval);
    lastTime = currentTime;

    doCalculations();
    filterCalculations();
    printCalculations();
    delay(5); // Loop time to match apprx with sampling time??

  }

}

/**
   I'm expecting, over time, the Arduino_LSM6DS3.h will add functions to do most of this,
   but as of 1.0.0 this was missing.
*/
void doCalculations() {
  accRoll = atan2(accelY, accelZ) * 180 / M_PI;
  accPitch = atan2(-accelX, sqrt(accelY * accelY + accelZ * accelZ)) * 180 / M_PI;

  float lastFrequency = (float) 1000000.0 / lastInterval;
  gyroRoll = gyroRoll + (gyroX / lastFrequency);
  gyroPitch = gyroPitch + (gyroY / lastFrequency);
  gyroYaw = gyroYaw + (gyroZ / lastFrequency);

  gyroCorrectedRoll = gyroCorrectedRoll + ((gyroX - gyroDriftX) / lastFrequency);
  gyroCorrectedPitch = gyroCorrectedPitch + ((gyroY - gyroDriftY) / lastFrequency);
  gyroCorrectedYaw = gyroCorrectedYaw + ((gyroZ - gyroDriftZ) / lastFrequency);

  complementaryRoll = complementaryRoll + ((gyroX - gyroDriftX) / lastFrequency);
  complementaryPitch = complementaryPitch + ((gyroY - gyroDriftY) / lastFrequency);
  complementaryYaw = complementaryYaw + ((gyroZ - gyroDriftZ) / lastFrequency);

  // complementary confidence gains
  complementaryRoll = 0.98 * complementaryRoll + 0.02 * accRoll;
  complementaryPitch = 0.98 * complementaryPitch + 0.02 * accPitch;
}


// CHANGE*
void filterCalculations() {
  filterPitch = lowpass_filter.filterIn(complementaryPitch);
//  filterYaw = lowpass_filter.filterIn(complementaryYaw);
  
}

/**
   This comma separated format is best 'viewed' using 'serial plotter' or processing.org client (see ./processing/RollPitchYaw3d.pde example)
*/
void printCalculations() {
//  Serial.print(gyroRoll);
//  Serial.print(',');
//  Serial.print(gyroPitch);
//  Serial.print(',');
//  Serial.print(gyroYaw);
//  Serial.print(',');
//  Serial.print(gyroCorrectedRoll);
//  Serial.print(',');
//  Serial.print(gyroCorrectedPitch);
//  Serial.print(',');
//  Serial.print(gyroCorrectedYaw);
//  Serial.print(',');
//  Serial.print(accRoll);
//  Serial.print(',');
//  Serial.print(accPitch);
//  Serial.print(',');
//  Serial.print(accYaw);
//  Serial.print(',');
//  Serial.print(complementaryRoll);
//  Serial.print(',');
//  Serial.print(complementaryPitch);
//  Serial.print(',');
  // Change*
  Serial.print(complementaryPitch);
  Serial.print(',');
  Serial.print(complementaryYaw);
//  Serial.print(',');
//  Serial.print(filterYaw);
  Serial.println("");
}
