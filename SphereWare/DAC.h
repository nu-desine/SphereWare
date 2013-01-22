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

#ifndef __DAC_H__
#define __DAC_H__

#include <avr/io.h>
#include <util/delay.h>

// Defines for direct port access
#define DIGPOT_SS  0 
#define SCK  1 
#define MOSI 2 
#define MISO 3

#define LDAC 1

void DAC_Init(void);
void DAC_Write(uint16_t data);
void DAC_Increment(void);
void DAC_Decrement(void);
uint16_t DAC_GetState(void);

#endif// __DAC_H__
