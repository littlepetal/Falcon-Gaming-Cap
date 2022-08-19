// Sketch from : https://github.com/arduino-libraries/Arduino_LSM6DS3/blob/5eac7f5e6145c4747da27698faf3a548d2893a2b/examples/RollPitchYaw/RollPitchYaw.ino
#pragma once
#include <Arduino_LSM6DS3.h>
//#include <Wire.h>

// Filter library from: https://github.com/MartinBloedorn/libFilter
#include <filters.h>


class RPY {

  public:
    void init();
    void IMUsetup();
    void updatePitchYaw();

    float getPitch();
    float getYaw();    
    

  private:
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
    float filterPitch, filterYaw;

    // Define filter specifications
    float cutoff_freq;  //Cutoff frequency in Hz          maybe set this higher because the data isn't very noisy, might decrese the phase lag
    float sampling_time; //Sampling time in seconds.      not sure how this works but wouldn't it make sense to use 10ms?   from the update 100Hz rate
    IIR::ORDER  order; // Order (OD1 to OD4)

    Filter* lowPassPitch; 
    Filter* lowPassYaw;        

    void calibrateIMU(int delayMillis, int calibrationMillis);
    bool readIMU();
    void doCalculations();
    void filterCalculations();
    void printCalculations();



};
