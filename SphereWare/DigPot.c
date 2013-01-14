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
#include "DigPot.h" 

void DigPot_Init(void)
{
	// Set MOSI SCK and SS output, all others input 
	DDRB |= (1 << SCK)|(1 << DIGPOT_SS)|(1 << MOSI);

	// pull select line high
	PORTB |= (1 << DIGPOT_SS);

	// Enable SPI, Master, mode 0, set clock rate fck/2 = 8Mhz
	SPCR = (1 << SPE)|(1 << MSTR)|(0 << SPR0)|(0 << SPR1)|(0 << CPHA)|(0 << CPOL);
    SPSR |= (1 << SPI2X);

}

void DigPot_Write(uint8_t wiper_number, uint16_t data)
{
	PORTB &= ~(1 << DIGPOT_SS);
	// only use 10 bits
	data &= 0x3FF;
	SPDR = (wiper_number << 4) | (data >> 8);
	while(!(SPSR & (1 << SPIF))); // wait
	SPDR = data;
	while(!(SPSR & (1 << SPIF))); // wait
	PORTB |= (1 << DIGPOT_SS);
}

uint8_t DigPot_Read(uint8_t wiper_number)
{
	PORTB &= ~(1 << DIGPOT_SS);
	SPDR = 0x0F | (wiper_number << 4);
	while(!(SPSR & (1 << SPIF))); // wait
	SPDR = 0xFF;
	while(!(SPSR & (1 << SPIF))); // wait
	PORTB |= (1 << DIGPOT_SS);
	return SPDR;
}

void DigPot_Increment(uint8_t wiper_number)
{
	PORTB &= ~(1 << DIGPOT_SS);
	SPDR = (wiper_number << 4) | 0b0100;
	while(!(SPSR & (1 << SPIF))); // wait
	PORTB |= (1 << DIGPOT_SS);
}

void DigPot_Decrement(uint8_t wiper_number)
{
	PORTB &= ~(1 << DIGPOT_SS);
	SPDR = (wiper_number << 4) | 0b1000;
	while(!(SPSR & (1 << SPIF))); // wait
	PORTB |= (1 << DIGPOT_SS);
}
