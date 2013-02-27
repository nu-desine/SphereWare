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
#include "GenericHID.h"

volatile uint8_t hid_in_buffer[GENERIC_REPORT_SIZE] = {1};

void GenericHID_Task(void)
{
    static bool flip = 0;

    /* Device must be connected and configured for the task to run */
    if (USB_DeviceState != DEVICE_STATE_Configured)
        return;

    Endpoint_SelectEndpoint(GENERIC_IN_EPADDR);
    
    /* Check to see if the host is ready to accept another packet */
    if (Endpoint_IsINReady())
    {
        uint8_t data[64];

        
        for (int i = 0; i < 64; i++)
        {
            data[i] = hid_in_buffer[i + (flip * 64)];
        }

        flip = !flip;
        
        /* Write Report Data */
        Endpoint_Write_Stream_LE(&data, sizeof(data), NULL);
        
        /* Finalize the stream transfer to send the last packet */
        Endpoint_ClearIN();
    }

    Endpoint_SelectEndpoint(GENERIC_OUT_EPADDR);
    
    /* Check to see if a packet has been sent from the host */
    if (Endpoint_IsOUTReceived())
    {
        /* Check to see if the packet contains data */
        if (Endpoint_IsReadWriteAllowed())
        {
            /* Create a temporary buffer to hold the read in report from the host */
            uint8_t GenericData[GENERIC_REPORT_SIZE];
            
            /* Read Generic Report Data */
            Endpoint_Read_Stream_LE(&GenericData, sizeof(GenericData), NULL);
            
            /* Process Generic Report Data */
            GenericHID_ProcessReport(GenericData);
        }
        
        /* Finalize the stream transfer to send the last packet */
        Endpoint_ClearOUT();
    }
}

void GenericHID_Write_PadData (uint8_t pad_number, int16_t pad_value, uint8_t pad_velocity)
{
    cli(); //disable interrupts

    hid_in_buffer[1 + (pad_number * 2)] = pad_velocity & 0x7F | ((pad_value & 1) << 7);
    hid_in_buffer[2 + (pad_number * 2)] = pad_value >> 1;

    sei(); //enable interrupts
}

void GenericHID_Write_ButtonDialData(uint8_t buttons, uint8_t dials) 
{
    cli(); //disable interrupts

    hid_in_buffer[97] = ((buttons & 0b111) << 1) | (dials << 4) | 1; //TODO add elite detection and info

    sei(); //enable interrupts
}

void GenericHID_ProcessReport(uint8_t* DataArray)
{
    /*
       This is where you need to process reports sent from the host to the device. This
       function is called each time the host has sent a new report. DataArray is an array
       holding the report sent from the host.
       */
    
    if (DataArray[0] == 0x01) 
    {
        int noOfMessages = DataArray[1];
        
        //decode each message within the HID report
        for (int i = 0; i < noOfMessages; i++)
        {
            //get the first byte index of the message
            int messageIndex  = (i * 4) + 2;
            
            //==== MIDI Message ====
            if (DataArray[messageIndex] == 0)
            {
                uint8_t message[4];
                message[0] = 0x00;
                message[1] = DataArray[messageIndex+1];
                message[2] = DataArray[messageIndex+2];
                message[3] = DataArray[messageIndex+3];
                
                MIDI_Send_Usb_Midi (message);
                MIDI_Send_Uart_Midi (message);
            }
            
        }
        
    }

}

/** Function to create the next report to send back to the host at the next reporting interval.
 *
 *  \param[out] DataArray  Pointer to a buffer where the next report data should be stored
 */
void GenericHID_CreateReport(uint8_t* DataArray)
{
    /*
       This is where you need to create reports to be sent to the host from the device. This
       function is called each time the host is ready to accept a new report. DataArray is
       an array to hold the report to the host.
       */
}

