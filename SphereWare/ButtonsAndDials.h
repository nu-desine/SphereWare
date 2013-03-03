#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include <avr/io.h>
#include <stdbool.h>

void ButtonsAndDials_Init(void);
void ButtonsAndDials_Read(uint8_t pad, bool * not_being_played);
static void Encoder_Read(uint8_t number, bool * not_being_turned);

#endif// __BUTTONS_H__
