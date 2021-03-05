/*******************************************************************************
* AmbaSat Apeldoorn
* Author: Marcel Meek
* Filename: main.cpp

* Main program based on the Ambasat 2020 example components:
* - Ambasat-Gyroscope
* - AmbaSat-Sensor-04-OPT3001
* - AmbaSat-TTN
*
* Copyright (c) 2020 AmbaSat Ltd
* https://ambasat.com
*
*******************************************************************************/

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <SPI.h>
#include "LowPower.h"           // Low Power Library, www.rocketscream.com
#include "i2c.h"
#include <Arduino.h>
#include <settings.h>

#include "Lora.h"               // interface for Ambasat TTN
#include "Gyroscope.h"          // class interface Ambasat-Gyroscope
#include "LightSensor.h"        // class interface AmbaSat-Sensor-04-OPT3001

#define LedPin 9

// -----------------------------------------------------------------------------
// TTN payload data structure - see https://www.thethingsnetwork.org/docs/devices/bytes.html
// -----------------------------------------------------------------------------
struct
{
    float lux;
    int16_t ax, ay, az;       // raw Accel (translated to [g] in TTN payload formatter)
    int16_t gx, gy, gz;       // raw Gyro  (translated to [deg/s] in TTN payload formatter)
    int16_t mx, my, mz;       // raw Magnetic (translated to [Gauss] in TTN payload formatter)
    int16_t vcc;              // millivolts
} payLoad;

Gyroscope  gyroscope;
LightSensor lightSensor;

// one sleep cycle is 8 sec.
int sleepcycles = SLEEP_COUNT;  

// -----------------------------------------------------------------------------
// readVcc
// -----------------------------------------------------------------------------
long readVcc()
{
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif

  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
  uint8_t high = ADCH; // unlocks both

  long result = (high<<8) | low;

  result = 1125300L / result;
  return result; // Vcc in millivolts
}

// =========================================================================================================================================
// setup
// =========================================================================================================================================
void setup()
{
    Serial.begin(SERIAL_BAUD);
    pinMode(LED_PIN, OUTPUT);
    delay(100);

    gyroscope.begin();
    lightSensor.begin();
    loraSetup();

    Serial.println("End setup");
}

// =========================================================================================================================================
//  loop
// =========================================================================================================================================
void loop() {
    Serial.println("loop");

    gyroscope.readAll();
    payLoad.ax = gyroscope.ax();
    payLoad.ay = gyroscope.ay();
    payLoad.az = gyroscope.az();
    payLoad.gx = gyroscope.gx();
    payLoad.gy = gyroscope.gy();
    payLoad.gz = gyroscope.gz();
    payLoad.mx = gyroscope.mx();
    payLoad.my = gyroscope.my();
    payLoad.mz = gyroscope.mz();

    payLoad.lux = lightSensor.readLux();
    payLoad.vcc = readVcc();

#ifdef DEBUG
      Serial.println("Vcc:" + String(payLoad.vcc));
      Serial.println("Lux:" + String(payLoad.lux));
      gyroscope.printCalculatedValues();
      gyroscope.printAttitude();
#endif

    digitalWrite(LedPin, HIGH);
    loraSend( 1, (unsigned char*)&payLoad, sizeof(payLoad)); // send the sensor payload to TTN
    
    loraLoop();
    digitalWrite(LedPin, LOW);
    Serial.flush();

    for (int i=0; i < SLEEP_COUNT; i++)  {
        LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);    //sleep 8 seconds * SLEEP_COUNT
    }
}
