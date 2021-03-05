
/*******************************************************************************
* Interface for AmbaSat-1 LoRa
* Based on AmbaSat-TTN
* Interface added by Marcel Meek, March 2021
* Filename: Lora.h
*******************************************************************************/


#ifndef _LORA_H_
#define _LORA_H_

extern void loraSetup();
extern void loraSend( int port, unsigned char* payload, int len );
extern void loraLoop();

#endif