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

#ifndef __MIDI_H__
#define __MIDI_H__

#include <avr/io.h>
#include <LUFA/Drivers/USB/USB.h>
#include "Descriptors.h"

#define USART_BAUDRATE 31250 
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1) 

uint8_t timing_count;

//==============================================================================
/** MIDI initialisation function.
 
 Sets up the microcontroller for sending UART MIDI data.
 */
void MIDI_Init(void);

//==============================================================================
/** Creates and sends a UART MIDI message.
 
 @param DataArray        the array of data that needs to be sent as a UART MIDI message
 */
void MIDI_Send_Uart_Midi (uint8_t* DataArray);

//==============================================================================
/** Creates and sends a USB MIDI message.
 
 @param DataArray        the array of data that needs to be sent as a USB MIDI message
 */
void MIDI_Send_Usb_Midi (uint8_t* DataArray);

//==============================================================================
/** Checks to see if MIDI message has been receive on the USB MIDI input.
 */
void MIDI_Recieve_Usb_Midi();

//==============================================================================
/** Processes a MIDI message recieved on the USB MIDI input.
 
 @param DataArray        the MIDI message
 */
void MIDI_Process_Usb_Midi (uint8_t* DataArray);

void MIDI_Uart_Put (char s);

#endif// __MIDI_H__
