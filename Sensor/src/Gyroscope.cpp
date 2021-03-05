/*******************************************************************************
* AmbaSat-1 LSM9DS1 9DOF sensor. Accelerometer, Gyroscope and Magnetometer

* 20th August 2020
* Version 1.0
* Adapted for Ambasat Apeldoorn by Marcel Meek, March 2021
* Filename: Gyroscope.cpp
*
* Copyright (c) 2020 AmbaSat Ltd
* https://ambasat.com
*
* Licensed under Creative Commons Attribution-ShareAlike 3.0
*
* Use this application to test the circuitry of your AmbaSat-1 Satellite and LSM9DS1 Gyro/Magneto/Acccel)
* Once the code is uploaded to your AmbaSat-1 board, view the debug output to confirm readings
*
* =================================================================================
* Requires the SparkFun LSM9DS1 IMU library 
*
* Based upon original code from:
*
*  Sparkfun: LSM9DS1_Basic_I2C.ino
*  SFE_LSM9DS1 Library Simple Example Code - I2C Interface
*  by Jim Lindblom @ SparkFun Electronics
*  Original Creation Date: April 30, 2015
*  https://github.com/sparkfun/LSM9DS1_Breakout
*
* For more information, see:
* https://learn.sparkfun.com/tutorials/lsm9ds1-breakout-hookup-guide/all
*
* ******************************************************************************/


#include "Gyroscope.h"

//////////////////////////
// LSM9DS1 Library Init //
//////////////////////////


///////////////////////
// Example I2C Setup //
///////////////////////
// SDO_XM and SDO_G are both pulled high, so our addresses are:
 #define LSM9DS1_M	0x1E // Would be 0x1C if SDO_M is LOW
 #define LSM9DS1_AG	0x6B // Would be 0x6A if SDO_AG is LOW


//// Earth's magnetic field varies by location. Add or subtract a declination to get a more accurate heading. 
// Calculate yours here: http://www.geomag.bgs.ac.uk/data_service/models_compass/wmm_calc.html

#define DECLINATION -0.62 // Declination (degrees), northallerton, North Yorks 

// ==============================================================================
void Gyroscope::begin( ) {
    Wire.begin();

    if (imu.begin() == false) // with no arguments, this uses default addresses (AG:0x6B, M:0x1E) and i2c port (Wire).
    {
        Serial.println("Err LSM9DS1");
        while (1);
    }
}

// ==============================================================================
void Gyroscope::readAll()
{
  // Update the sensor values whenever new data is available
  if ( imu.gyroAvailable() )
  {
    // To read from the gyroscope,  first call the
    // readGyro() function. When it exits, it'll update the
    // gx, gy, and gz variables with the most current data.
    imu.readGyro();
  }
  if ( imu.accelAvailable() )
  {
    // To read from the accelerometer, first call the
    // readAccel() function. When it exits, it'll update the
    // ax, ay, and az variables with the most current data.
    imu.readAccel();
  }
  if ( imu.magAvailable() )
  {
    // To read from the magnetometer, first call the
    // readMag() function. When it exits, it'll update the
    // mx, my, and mz variables with the most current data.
    imu.readMag();
  }
}


// ==============================================================================
void Gyroscope::printCalculatedValues() {
    printXYZ( "Accel:", imu.calcAccel(imu.ax), imu.calcAccel(imu.ay), imu.calcAccel(imu.az));
    printXYZ( "Gyro:", imu.calcGyro(imu.gx), imu.calcGyro(imu.gy), imu.calcGyro(imu.gz));
    printXYZ( "Magn:", imu.calcMag(imu.mx), imu.calcMag(imu.my), imu.calcMag(imu.mz));
}

void Gyroscope::printXYZ( const char* txt, float x, float y, float z) {
     Serial.println( txt + String(x) + " " + String(y) + " " + String(z));
}
 
// Calculate pitch, roll, and heading.
// Pitch/roll calculations taken from this app note:
// http://cache.freescale.com/files/sensors/doc/app_note/AN3461.pdf?fpsp=1

// Heading calculations taken from this app note:
// http://www51.honeywell.com/aero/common/documents/myaerospacecatalog-documents/Defense_Brochures-documents/Magnetic__Literature_Application_notes-documents/AN203_Compass_Heading_Using_Magnetometers.pdf

// ==============================================================================
void Gyroscope::printAttitude() {

    float ax = imu.ax;
    float ay = imu.ay;
    float az = imu.az;
    // The LSM9DS1's mag x and y axes are opposite to the accelerometer, so my, mx are substituted for each other.
    float mx = -imu.mx;
    float my = -imu.my;
 
    float roll = atan2(ay, az);
    float pitch = atan2(-ax, sqrt(ay * ay + az * az));

    float heading;

    if (my == 0)
        heading = (mx < 0) ? PI : 0;
    else
        heading = atan2(mx, my);

    heading -= DECLINATION * PI / 180;

    if (heading > PI) heading -= (2 * PI);
    else if (heading < -PI) heading += (2 * PI);

    // Convert everything from radians to degrees:
    heading *= 180.0 / PI;
    pitch *= 180.0 / PI;
    roll  *= 180.0 / PI;

    Serial.println( "Heading:" + String(heading) + " Pitch:" + String(pitch) + " Roll:" + String(roll));
}

