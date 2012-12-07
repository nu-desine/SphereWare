/* Copyright (c) nu desine 2012 

    This file is part of SphereWare.

    SphereWare is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SphereWare is program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

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
