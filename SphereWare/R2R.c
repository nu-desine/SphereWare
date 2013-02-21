#include "R2R.h"

void R2R_Init(void)
{
     DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3);
     DDRF |= (1 << PF0) | (1 << PF5);
     R2R_Write(0);
}

void R2R_Write(uint8_t val)
{
    //clear relevant pins and write to them in one step per register to avoid noise
    PORTB = (PORTB & 0xF0) | ((val >> 2) & 0x0F);
    PORTF = (PORTF & (~(1 << PF0) & ~(1 << PF5))) 
          | (((val >> 1) & 1) << PF0)
          | (((val     ) & 1) << PF5);
}

