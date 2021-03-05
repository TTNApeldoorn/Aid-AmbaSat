/*******************************************************************************
* Interface for AmbaSat-1 Sensor 04 - OPT3001
* Based on code from ClosedCube
* Interface added by Marcel Meek, March 2021
* Filename: LightSensor.h
*******************************************************************************/

#ifndef _LIGHTSENSOR_H_
#define _LIGHTSENSOR_H_

#include <Wire.h>
#include <ClosedCube_OPT3001.h>

#define OPT3001_ADDRESS 0x44  // OPT3001 I2C address is 68


class LightSensor {

  public:
    void begin();
    float readLux();
    void configureSensor();
   
  private:
    ClosedCube_OPT3001 opt3001;
};

#endif