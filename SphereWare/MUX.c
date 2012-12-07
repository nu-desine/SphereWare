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
#include "MUX.h"


#define MUX_A PD2
#define MUX_B PD4
#define MUX_C PD5

#define MUX_INH_1 PF6
#define MUX_INH_2 PD7
#define MUX_INH_3 PB4
#define MUX_INH_4 PB5
#define MUX_INH_5 PB6
#define MUX_INH_6 PB7


void MUX_Init(void)
{
	DDRF |= (1 << MUX_INH_1);
	DDRD |= (1 << MUX_A) | (1 << MUX_B) | (1 << MUX_C) | (1 << MUX_INH_2);
	DDRB |= (1 << MUX_INH_3) 
		   |  (1 << MUX_INH_4) 
		 	 |  (1 << MUX_INH_5) 
		 	 |  (1 << MUX_INH_6);

	PORTF |= (1 << MUX_INH_1);
	PORTD |= (1 << MUX_INH_2);
	PORTB |= (1 << MUX_INH_3) 
		    |  (1 << MUX_INH_4) 
				|  (1 << MUX_INH_5) 
				|  (1 << MUX_INH_6);
	//PORTD &= ~(1 << MUX_INH_2);
	//PORTD |= (1 << MUX_A) | (1 << MUX_B);
}

void MUX_Select(uint8_t channel)
{
	uint8_t inhibit;
	
	PORTD &= ~(1 << MUX_A) 
	      &  ~(1 << MUX_B) 
	      &  ~(1 << MUX_C);

	PORTD |= (((channel     ) & 1) << MUX_A) 
		    |  (((channel >> 1) & 1) << MUX_B) 
				|  (((channel >> 2) & 1) << MUX_C);
 
	
	// the inhibit is active low so we invert this when doing the port IO 
	inhibit = 1 << (channel >> 3);

	// we  would like activate two select lines at once switching well in advance of reading
 	// we could  do this as adjacent MUXs are on separate ADC inputs but unfortunately 16 sensors consume too much power
	// this would allow for settling after power- and MUX-switch 

	//inhibit = 0b11 << (channel >> 3);
	//if (inhibit == 0b1100000)
	//	inhibit = 0b100001; 

	PORTF |= (1 << MUX_INH_1);
	PORTD |= (1 << MUX_INH_2);
	PORTB |= (1 << MUX_INH_3) 
		    |  (1 << MUX_INH_4) 
				|  (1 << MUX_INH_5) 
				|  (1 << MUX_INH_6);

	PORTF &= ~((inhibit     ) << MUX_INH_1);
	PORTD	&= ~((inhibit >> 1) << MUX_INH_2);
	PORTB &= ~((inhibit >> 2) << MUX_INH_3) 
		    &  ~((inhibit >> 3) << MUX_INH_4) 
				&  ~((inhibit >> 4) << MUX_INH_5) 
				&  ~((inhibit >> 5) << MUX_INH_6);
}

