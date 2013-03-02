#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include <avr/io.h>

void ButtonsAndDials_Init(void);
uint8_t ButtonsAndDials_Read(uint8_t pad);
uint8_t read_encoder_1(uint8_t decoder_val);
uint8_t read_encoder_2(uint8_t decoder_val);

#endif// __BUTTONS_H__
