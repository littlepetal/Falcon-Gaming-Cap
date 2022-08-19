
#include "IMURollPitchYaw.h"
#include <filters.h>

void RPY::init() {
  cutoff_freq   = 5.0;  //Cutoff frequency in Hz          maybe set this higher because the data isn't very noisy, might decrese the phase lag
  sampling_time = 0.005; //Sampling time in seconds.      not sure how this works but wouldn't it make sense to use 10ms?   from the update 100Hz rate  IIR::order  = IIR::ORDER::OD3; // Order (OD1 to OD4)
  
  Filter lowpass_filter1(cutoff_freq, sampling_time, order);
  *lowPassPitch = lowpass_filter1;
  Filter lowpass_filter2(cutoff_freq, sampling_time, order);
  *lowPassYaw = lowpass_filter2;       

}

void RPY::IMUsetup() {

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
void RPY::calibrateIMU(int delayMillis, int calibrationMillis) {

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
bool RPY::readIMU() {
 if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable() ) {
   IMU.readAcceleration(accelX, accelY, accelZ);
   IMU.readGyroscope(gyroX, gyroY, gyroZ);
   return true;
 }
 return false;
}

void RPY::updatePitchYaw()
{
  if (readIMU()) {
    long currentTime = micros();
    lastInterval = currentTime - lastTime; // expecting this to be ~104Hz +- 4%
    lastTime = currentTime;

    doCalculations();
    filterCalculations();
    // printCalculations();
    delay(5); // Loop time to match apprx with sampling time??

  }
}


/**
  I'm expecting, over time, the Arduino_LSM6DS3.h will add functions to do most of this,
  but as of 1.0.0 this was missing.
*/
void RPY::doCalculations() {
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


// CHANGE*
void RPY::filterCalculations() {
 filterPitch = lowPassPitch->filterIn(complementaryPitch);
 filterYaw = lowPassYaw->filterIn(complementaryYaw);
}

float RPY::getPitch()
{
  return filterPitch;
}

float RPY::getYaw()
{
  return filterYaw;
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
