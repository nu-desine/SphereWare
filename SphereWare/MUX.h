#ifndef __MUX_H__
#define __MUX_H__

#include <avr/io.h>

void MUX_Init(void);
void MUX_Select(uint8_t channel);

#endif// __MUX_H__
