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
#include "MIDI.h" 

void MIDI_Init(void)
{
	UCSR1B |= (1 << TXEN1);
    UBRR1H = (BAUD_PRESCALE >> 8);  
    UBRR1L = BAUD_PRESCALE;
}

void MIDI_Send_Uart_Midi (uint8_t* DataArray)
{
    uint8_t statusByte = DataArray[1];
    uint8_t dataByte1 = DataArray[2];
    uint8_t dataByte2 = DataArray[3];
    
    while ((UCSR1A & (1 << UDRE1)) == 0) {};
    MIDI_Uart_Put (statusByte); 
    MIDI_Uart_Put (dataByte1); 
    MIDI_Uart_Put (dataByte2);
}

void MIDI_Send_Usb_Midi (uint8_t* DataArray)
{
    
    //test message
//    uint8_t statusByte = 0xa0;
//    uint8_t dataByte1 = 0x3c;
//    uint8_t dataByte2 = 0x6e;
//    uint8_t midiCommand = statusByte & 0xf0;
    
    uint8_t statusByte = DataArray[1];
    uint8_t dataByte1 = DataArray[2];
    uint8_t dataByte2 = DataArray[3];
    uint8_t midiCommand = statusByte & 0xf0;    
    // Device must be connected and configured for the task to run
    if (USB_DeviceState != DEVICE_STATE_Configured)
        return;
    
    // Select correct endpoint
    Endpoint_SelectEndpoint(MIDI_STREAM_IN_EPADDR);
    
    if (statusByte)
    {
        MIDI_EventPacket_t MIDIEvent = (MIDI_EventPacket_t)
        {
            .Event       = MIDI_EVENT(0, midiCommand),
            .Data1       = statusByte,
            .Data2       = dataByte1,
            .Data3       = dataByte2,
        };
        
        // Write the MIDI event packet to the endpoint
        Endpoint_Write_Stream_LE(&MIDIEvent, sizeof(MIDIEvent), NULL);
        
        // Send the data in the endpoint to the host
        Endpoint_ClearIN();
    }
}

void MIDI_Uart_Put (char s)
{
    while (!(UCSR1A & (1 << UDRE1)));
    UDR1 = s;  
}
