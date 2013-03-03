#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include <avr/io.h>

void ButtonsAndDials_Init(void);
void ButtonsAndDials_Read(uint8_t pad);
static void Encoder_Read(uint8_t number);

#endif// __BUTTONS_H__
