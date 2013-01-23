/* Copyright (c) nu desine 2012 

    This file is part of SphereWare.

    SphereWare is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SphereWare is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "DAC.h" 

uint16_t dac_state; 

void DAC_Init(void)
{
	// Set MOSI SCK SS and LDAC output, all others input 
	DDRB |= (1 << SCK)|(1 << DIGPOT_SS)|(1 << MOSI);
    DDRF |= (1 << LDAC);

	// pull select line high
	PORTB |= (1 << DIGPOT_SS);

	// Enable SPI, Master, mode 0, set clock rate fck/2 = 8Mhz
	SPCR = (1 << SPE)|(1 << MSTR)|(0 << SPR0)|(0 << SPR1)|(0 << CPHA)|(0 << CPOL);
    SPSR |= (1 << SPI2X);
    DAC_Write(0);
}

void DAC_Write(uint16_t data)
{
	PORTB &= ~(1 << DIGPOT_SS);
	// only use 12 bits
	data &= 0xFFF;
	SPDR = data & 0xFF;
	while(!(SPSR & (1 << SPIF))); // wait
	SPDR = (data >> 8) & 0b00111111;
	while(!(SPSR & (1 << SPIF))); // wait
	PORTB |= (1 << DIGPOT_SS);

    PORTF &= ~(1 << LDAC);
    _delay_us(5);
    PORTF |= (1 << LDAC);
    dac_state = data;
}

void DAC_Increment(void)
{
    DAC_Write(dac_state + 1);
}

void DAC_Decrement(void)
{
    DAC_Write(dac_state - 1);
}

uint16_t DAC_GetState(void)
{
    return dac_state;
}
