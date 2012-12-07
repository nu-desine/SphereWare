#ifndef __DIGPOT_H__
#define __DIGPOT_H__

#include <avr/io.h>

// Defines for direct port access
#define DIGPOT_SS  0 
#define SCK  1 
#define MOSI 2 
#define MISO 3

void DigPot_Init(void);
void DigPot_Write(uint8_t wiper_number, uint16_t data);
uint8_t DigPot_Read(uint8_t wiper_number);
void DigPot_Increment(uint8_t wiper_number);
void DigPot_Decrement(uint8_t wiper_number);

#endif// __DIGPOT_H__
