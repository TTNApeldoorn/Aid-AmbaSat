/*******************************************************************************
* Apeldoorn Ambasat project, March 2021
* Ambasat TTN code isolated from main, by Marcel Meek
* Filename: Lora.cpp
*
* Copyright (c) 2020 AmbaSat Ltd
* https://ambasat.com
*
* This application will submit test data to The Things Network (TTN)
* Data is routed by TTN to the AmbaSat Dashboard
* You MUST set your unique AmbaSat-1 KEYS & DEVICE ID in main.h
*
* Licensed under Creative Commons Attribution-ShareAlike 3.0
*
* This example will send Temperature, humidity & battery information
* using frequency and encryption settings matching those of
* the The Things Network.
*
* This example uses ABP (Activation-by-personalisation), where a DevAddr and
* Session keys are preconfigured (unlike OTAA, where a DevEUI and
* application key is configured, while the DevAddr and session keys are
* assigned/generated in the over-the-air-activation procedure).
**
* Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in
* g1, 0.1% in g2), but not the TTN fair usage policy.

* To use this code, first register your application and device with
* the things network, to set or generate an AppEUI, DevEUI and AppKey.
* Multiple devices can use the same AppEUI, but each device has its own
* DevEUI and AppKey.
*
* Supporting code/libraries:
*
* Based on code originally created by
* Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
*
* Permission is hereby granted, free of charge, to anyone
* obtaining a copy of this document and accompanying files,
* to do whatever they want with them without any restriction,
* including, but not limited to, copying, modification and redistribution.
* NO WARRANTY OF ANY KIND IS PROVIDED.
*
* AmbaSat-1 Low Earth Orbit Space Satellite
* Copyright (c) 2020 AmbaSat Ltd
* https://ambasat.com
*
* NOTE. You MUST set your unique AmbaSat-1 KEYS & DEVICE ID in this file below
*
*******************************************************************************/
#include <Arduino.h>
#include <lmic.h>
#include <hal/hal.h>
#include "Lora.h"

#define ADR_MODE 1

/***************************** !!! TTN */
// The Network Session Key
static const PROGMEM u1_t NWKSKEY[16] = { 0xCE,0x7D,0x2A,0x02,0x95,0x6F,0x79,0x30,0x9D,0x58,0x97,0xC3,0x39,0x56,0x14,0x1B };  // ambasat02
//static const PROGMEM u1_t NWKSKEY[16] = {0x35,0xBE,0x7B,0x0D,0xFF,0xCA,0xFF,0x03,0x3A,0xCE,0x97,0xBE,0x14,0x2A,0xAD,0x6E}; //<< Sandbox
//static const PROGMEM u1_t NWKSKEY[16] = {0x94,0x0D,0x98,0x2B,0xE5,0xBD,0xA5,0x25,0x6E,0xD6,0xCD,0x07,0xF9,0x94,0x76,0xE3};  //<< Ambasat dashboard

// LoRaWAN AppSKey, application session key
static const u1_t PROGMEM APPSKEY[16] = { 0xB3,0xE0,0xEA,0x0C,0xCF,0xE6,0xF4,0xDB,0x5E,0xBE,0x56,0xEA,0x6C,0x24,0xD1,0x48 };  // ambasat02
//static const u1_t PROGMEM APPSKEY[16] = {0x82,0x89,0xA6,0xE3,0x19,0x05,0x6D,0x53,0x6E,0x83,0xB0,0xFC,0xB3,0xC7,0xB4,0x89}; //<< Sandbox
//static const u1_t PROGMEM APPSKEY[16] = {0xF0,0x7C,0x9D,0x19,0x64,0xC9,0x2E,0x6A,0x0B,0x52,0xE8,0x3F,0xE0,0x29,0x08,0x36};   //<< Ambasat dashboard

// LoRaWAN end-device address (DevAddr)
//static const u4_t DEVADDR = 0x26031337 ;
static const u4_t DEVADDR = 0x26011D73;      // ambasat02
//static const u4_t DEVADDR = 0x26013502 ;  //<< Sandbox
//static const u4_t DEVADDR =  0x26011154;    //<< Ambasat dashboard

/********************************/

// These callbacks are only used in over-the-air activation, so they are
// left empty here (cannot be left out completely unless
// DISABLE_JOIN is set in config.h, otherwise the linker will complain).
void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

//static osjob_t sendjob;
static osjob_t initjob;

// Pin mapping
const lmic_pinmap lmic_pins = {
  .nss = 10,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 7,
  .dio = {2, A2, LMIC_UNUSED_PIN},
};

bool joined = false;
bool sleeping = false;

// initial job
static void initfunc (osjob_t* j)
{
    // reset MAC state
    LMIC_reset();

    // start joining
    LMIC_startJoining();
    // init done - onEvent() callback will be invoked...
}


// =========================================================================================================================================
// onEvent
// =========================================================================================================================================
void onEvent (ev_t ev)
{
    //int i, j;

    switch (ev)
    {
        case EV_SCAN_TIMEOUT:
            Serial.println(F("EV_SCAN_TIMEOUT"));
        break;
        case EV_BEACON_FOUND:
            Serial.println(F("EV_BEACON_FOUND"));
        break;
        case EV_BEACON_MISSED:
            Serial.println(F("EV_BEACON_MISSED"));
        break;
        case EV_BEACON_TRACKED:
            Serial.println(F("EV_BEACON_TRACKED"));
        break;
        case EV_JOINING:
            Serial.println(F("EV_JOINING"));
        break;
        case EV_JOINED:
            Serial.println(F("EV_JOINED"));
            // Disable link check validation (automatically enabled
            // during join, but not supported by TTN at this time).
            LMIC_setLinkCheckMode(0);
            //digitalWrite(LedPin, LOW);
            // after Joining a job with the values will be sent.
            joined = true;
        break;
        case EV_RFU1:
            Serial.println(F("EV_RFU1"));
        break;
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
        break;
        case EV_REJOIN_FAILED:
            Serial.println(F("EV_REJOIN_FAILED"));
            // Re-init
            os_setCallback(&initjob, initfunc);
        break;
        case EV_TXCOMPLETE:
            sleeping = true;

            if (LMIC.dataLen)
            {
                // data received in rx slot after tx
                // if any data received, a LED will blink
                // this number of times, with a maximum of 10
                Serial.print(F("Data Received: "));
                Serial.println(LMIC.frame[LMIC.dataBeg], HEX);

                /* MM comment out, due to lack of programspace
                i=(LMIC.frame[LMIC.dataBeg]);
                // i (0..255) can be used as data for any other application
                // like controlling a relay, showing a display message etc.
                if (i>10)
                {
                    i=10;     // maximum number of BLINKs
                }

                for(j = 0; j < i ; j++)
                {
                    digitalWrite(LedPin, HIGH);
                    delay(200);
                    digitalWrite(LedPin, LOW);
                    delay(400);
                } */
            }

            Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            delay(50);  // delay to complete Serial Output before Sleeping

            // Schedule next transmission
            // next transmission will take place after next wake-up cycle in main loop
        break;
        case EV_LOST_TSYNC:
            Serial.println(F("EV_LOST_TSYNC"));
        break;
        case EV_RESET:
            Serial.println(F("EV_RESET"));
        break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            Serial.println(F("EV_RXCOMPLETE"));
        break;
        case EV_LINK_DEAD:
            Serial.println(F("EV_LINK_DEAD"));
        break;
        case EV_LINK_ALIVE:
            Serial.println(F("EV_LINK_ALIVE"));
        break;
        default:
            Serial.println(F("Unknown event"));
        break;
    }
}

// =========================================================================================================================================
// do_send
// MM changed in loraSend
// =========================================================================================================================================

//void do_send(osjob_t* j)
void loraSend( int port, unsigned char* payload, int len )
{
    // Check that there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND)
    {
        Serial.println("Current OP_TXRXPEND is running so not sending");
    }
    else
    {
        // Prepare upstream data transmission at the next possible time.
        //LMIC_setTxData2(1, message.getBytes(), message.getLength(), 0);
        LMIC_setTxData2( port, payload, len, 0);
        Serial.println("Sending payload len=" + len);
    }
}

// =========================================================================================================================================
// setup
// MM changed in loraSetup
// =========================================================================================================================================
void loraSetup()
{

    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();

    // Set static session parameters. Instead of dynamically establishing a session
    // by joining the network, precomputed session parameters are be provided.
    uint8_t appskey[sizeof(APPSKEY)];
    uint8_t nwkskey[sizeof(NWKSKEY)];
    memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
    memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
    LMIC_setSession (0x1, DEVADDR, nwkskey, appskey);

    #if defined(CFG_eu868)
        // Set up the channels used by the Things Network, which corresponds
        // to the defaults of most gateways. Without this, only three base
        // channels from the LoRaWAN specification are used, which certainly
        // works, so it is good for debugging, but can overload those
        // frequencies, so be sure to configure the full frequency range of
        // your network here (unless your network autoconfigures them).
        // Setting up channels should happen after LMIC_setSession, as that
        // configures the minimal channel set.
        // NA-US channels 0-71 are configured automatically
        LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
        LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      // g-band
        LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
        LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
        LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
        LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
        LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
        LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
        LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);      // g2-band
        // TTN defines an additional channel at 869.525Mhz using SF9 for class B
        // devices' ping slots. LMIC does not have an easy way to define set this
        // frequency and support for class B is spotty and untested, so this
        // frequency is not configured here.
    #elif defined(CFG_us915)
        // NA-US channels 0-71 are configured automatically
        // but only one group of 8 should (a subband) should be active
        // TTN recommends the second sub band, 1 in a zero based count.
        // https://github.com/TheThingsNetwork/gateway-conf/blob/master/US-global_conf.json

        LMIC_setupChannel(0, 923200000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
        LMIC_setupChannel(1, 923400000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
        LMIC_setupChannel(2, 923600000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
        LMIC_setupChannel(3, 923800000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
        LMIC_setupChannel(4, 924000000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
        LMIC_setupChannel(5, 924200000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
        LMIC_setupChannel(6, 924400000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
        LMIC_setupChannel(7, 924600000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
        LMIC_setupChannel(8, 924800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);      // g2-band

        LMIC_selectSubBand(1);

    #endif

    // Disable link check validation
    LMIC_setLinkCheckMode(0);

    // TTN uses SF9 for its RX2 window.
    LMIC.dn2Dr = DR_SF9;

    // Set data rate and transmit power for uplink (note: txpow seems to be ignored by the library)
    LMIC_setDrTxpow(DR_SF7, 14);
}

// =========================================================================================================================================
// Lora Loop
// added by MM, loop moved from main
// =========================================================================================================================
void loraLoop() {
    while( sleeping == false)  {
        os_runloop_once();
    }
    sleeping = false;
}