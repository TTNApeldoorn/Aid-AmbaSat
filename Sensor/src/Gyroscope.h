/*******************************************************************************
* Interface for AmbaSat-1 LSM9DS1 9DOF sensor.
* Accelerometer, Gyroscope and Magnetometer
* Interface added by Marcel Meek, March 2021
* Filename: Gyroscope.h
*******************************************************************************/

#ifndef _GYROSCOPE_H_
#define _GYROSCOPE_H_

// The SFE_LSM9DS1 library requires both Wire and SPI be included BEFORE including the 9DS1 library.
#include <Wire.h>
#include <SPI.h>
#include <SparkFunLSM9DS1.h>


class Gyroscope {
  public:
    Gyroscope() {};
    void begin();
    void readAll();
    void printCalculatedValues();
    void printAttitude();

    short ax()   { return( imu.ax); };
    short ay()   { return( imu.ay); };
    short az()   { return( imu.az); };
    short mx()   { return( imu.mx); };
    short my()   { return( imu.my); };
    short mz()   { return( imu.mz); };
    short gx()   { return( imu.gx); };
    short gy()   { return( imu.gy); };
    short gz()   { return( imu.gz); };
   
  private:
    void printXYZ( const char* txt, float x, float y, float z);
    LSM9DS1 imu; 
};

#endif


