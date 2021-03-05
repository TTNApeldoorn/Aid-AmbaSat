/*******************************************************************************
* AmbaSat-1 Sensor 04 - OPT3001
* 5th February 2020
* Version 1.0
* Adapted for Ambasat Apeldoorn by Marcel Meek, March 2021
* Filename: LifghtSensor.cpp
*
* Copyright (c) 2020 AmbaSat Ltd
* https://ambasat.com
*
* Licensed under Creative Commons Attribution-ShareAlike 3.0
*
* Use this application to test the circuitry of your AmbaSat-1 Satellite and Sensor 04 - OPT3001
* Once the code is uploaded to your AmbaSat-1 board, view the debug output to confirm readings
*
* Based on code from ClosedCube
* ******************************************************************************/

#include "LightSensor.h"

// ==============================================================================
void LightSensor::begin() {
    Wire.begin();

    opt3001.begin(OPT3001_ADDRESS);
    /* MM comment out due to lack of program space
    Serial.print("OPT3001 Manufacturer ID");
    Serial.println(opt3001.readManufacturerID());
    Serial.print("OPT3001 Device ID");
    Serial.println(opt3001.readDeviceID()); */

    configureSensor();
    /* MM comment out due to lack of program space
    printResult("High-Limit", opt3001.readHighLimit());
    printResult("Low-Limit", opt3001.readLowLimit());
    Serial.println("----");  */      
}

// ==============================================================================
float LightSensor::readLux()
{
	OPT3001 result = opt3001.readResult();
    return result.lux;
}

// ==============================================================================
void LightSensor::configureSensor() 
{
    OPT3001_Config newConfig;
    
    newConfig.RangeNumber = B1100;	
    newConfig.ConvertionTime = B0;
    newConfig.Latch = B1;
    newConfig.ModeOfConversionOperation = B11;

    OPT3001_ErrorCode errorConfig = opt3001.writeConfig(newConfig);
   
    if (errorConfig != NO_ERROR) {
        Serial.print("OPT3001 err:");
        Serial.println(  errorConfig); 
    }
 /*  MM comment out due to lack of program space
    else 
    {
        OPT3001_Config sensorConfig = opt3001.readConfig();
        Serial.println("OPT3001 Current Config:");
        Serial.println("------------------------------");
        
        Serial.print("Conversion ready (R):");
        Serial.println(sensorConfig.ConversionReady,HEX);

        Serial.print("Conversion time (R/W):");
        Serial.println(sensorConfig.ConvertionTime, HEX);

        Serial.print("Fault count field (R/W):");
        Serial.println(sensorConfig.FaultCount, HEX);

        Serial.print("Flag high field (R-only):");
        Serial.println(sensorConfig.FlagHigh, HEX);

        Serial.print("Flag low field (R-only):");
        Serial.println(sensorConfig.FlagLow, HEX);

        Serial.print("Latch field (R/W):");
        Serial.println(sensorConfig.Latch, HEX);

        Serial.print("Mask exponent field (R/W):");
        Serial.println(sensorConfig.MaskExponent, HEX);

        Serial.print("Mode of conversion operation (R/W):");
        Serial.println(sensorConfig.ModeOfConversionOperation, HEX);

        Serial.print("Polarity field (R/W):");
        Serial.println(sensorConfig.Polarity, HEX);

        Serial.print("Overflow flag (R-only):");
        Serial.println(sensorConfig.OverflowFlag, HEX);

        Serial.print("Range number (R/W):");
        Serial.println(sensorConfig.RangeNumber, HEX);

        Serial.println("------------------------");
        
    } */
}
