
#include "IMURollPitchYaw.h"
#include <filters.h>



// Low pass filter specifications
const float cutoff_freq   = 2.0;  //Cutoff frequency in Hz (5)         maybe set this higher because the data isn't very noisy, might decrese the phase lag
const float sampling_time = 0.005; //Sampling time in seconds.      not sure how this works but wouldn't it make sense to use 10ms?   from the update 100Hz rate
IIR::ORDER  order = IIR::ORDER::OD2; // Order (OD1 to OD4)         

// Create low pass filter classes
Filter lowPassPitch(cutoff_freq, sampling_time, order);
Filter lowPassYaw(cutoff_freq, sampling_time, order);

void RPY::IMUsetup() {

  zeroValues();
  
 lowPassPitch.flush();
 lowPassYaw.flush();
 if (!IMU.begin()) {
   Serial.println("Failed to initialize IMU!");
   while (1);
 }

 calibrateIMU(250, 250);

 lastTime = micros();
 

}


void RPY::zeroValues() {

  lastInterval = 0;
  
  gyroRoll = 0;
  gyroPitch = 0;
  gyroYaw = 0;

  gyroCorrectedRoll = 0;
  gyroCorrectedPitch = 0;
  gyroCorrectedYaw = 0;

  complementaryRoll = 0;
  complementaryPitch = 0;
  complementaryYaw = 0;
}

/*
 the gyro's x,y,z values drift by a steady amount. if we measure this when arduino is still
 we can correct the drift when doing real measurements later
*/
void RPY::calibrateIMU(int delayMillis, int calibrationMillis) {

 int calibrationCount = 0;

 delay(delayMillis); // to avoid shakes after pressing reset button

 float sumX =0;
 float sumY = 0; 
 float sumZ = 0;
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
bool RPY::readIMU() {
 if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable() ) {
   IMU.readAcceleration(accelX, accelY, accelZ);
   IMU.readGyroscope(gyroX, gyroY, gyroZ);

   return true;
 }
 return false;
}

// update the values of roll pitch and yaw
void RPY::updatePitchYaw()
{
  if (readIMU()) {
    long currentTime = micros();
    lastInterval = currentTime - lastTime; // expecting this to be ~104Hz +- 4%
    lastTime = currentTime;

    doCalculations();
    filterCalculations();
    delay(5); // Loop time to match apprx with sampling time??

  }
}


/**
  I'm expecting, over time, the Arduino_LSM6DS3.h will add functions to do most of this,
  but as of 1.0.0 this was missing.
*/
void RPY::doCalculations() {
  // simple trig from ratio of acceleration
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

 complementaryRoll = 0.98 * complementaryRoll + 0.02 * accRoll;
 complementaryPitch = 0.98 * complementaryPitch + 0.02 * accPitch;
}


// Low pass filter on the pitch and yaw data
void RPY::filterCalculations() {
 filterPitch = lowPassPitch.filterIn(complementaryPitch);
 filterYaw = lowPassYaw.filterIn(complementaryYaw);
}


// Return pitch yaw data
float RPY::getPitch()
{
  return filterPitch;
//  return gyroCorrectedPitch;
}

float RPY::getYaw()
{
  return filterYaw;
//  return gyroZ;
}

/**
  This comma separated format is best 'viewed' using 'serial plotter' or processing.org client (see ./processing/RollPitchYaw3d.pde example)
*/
void RPY::printCalculations() {
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
 Serial.print(complementaryPitch);
 Serial.print(',');
 // Change*
 Serial.print(filterPitch);
//  Serial.print(complementaryYaw);
 Serial.println("");
}
