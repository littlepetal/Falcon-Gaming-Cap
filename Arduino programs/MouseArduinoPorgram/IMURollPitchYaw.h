// Sketch from : https://github.com/arduino-libraries/Arduino_LSM6DS3/blob/5eac7f5e6145c4747da27698faf3a548d2893a2b/examples/RollPitchYaw/RollPitchYaw.ino
#pragma once
#include <Arduino_LSM6DS3.h>  // this library automatically sets acc resolution to 0.122mg and gyro res to 70mdps
//#include <Wire.h>           // by directly interfacing with the LSM6DS3 you can get 0.061mg and 4.375mdps resolution

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

    void calibrateIMU(int delayMillis, int calibrationMillis);
    bool readIMU();
    void doCalculations();
    void filterCalculations();
    void printCalculations();
    void zeroValues();

};
