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
#include "HidInReports.h"

uint8_t pad_report[GENERIC_REPORT_SIZE];

void HidInReports_Init (void)
{
    pad_report[0] = 0x01;
    for (int i = 1; i < GENERIC_REPORT_SIZE; ++i)
    {
        pad_report[i] = 0;
    }
}

void HidInReports_Send_Report (uint8_t* report_data)
{
    /* Device must be connected and configured for the task to run */
    if (USB_DeviceState != DEVICE_STATE_Configured)
        return;
    
    Endpoint_SelectEndpoint(GENERIC_IN_EPADDR);
    
    /* Check to see if the host is ready to accept another packet */
    if (Endpoint_IsINReady())
    {
        uint8_t data[GENERIC_REPORT_SIZE];
        
        for (int i = 0; i < GENERIC_REPORT_SIZE; i++)
        {
            data[i] = report_data[i];
        }
        
        /* Write Report Data */
        Endpoint_Write_Stream_LE(&data, sizeof(data), NULL);
        
        /* Finalize the stream transfer to send the last packet */
        Endpoint_ClearIN();
    }
}

#define SIZE_OF_PAD_DATA 3 

void HidInReports_Create_Pad_Report (uint8_t pad_number, 
                                     int pad_value, 
                                     uint8_t pad_velocity)
{
    uint8_t pad_address = pad_number * SIZE_OF_PAD_DATA;
    
    pad_report[1 + pad_address] = pad_value;
    pad_report[2 + pad_address] = (pad_value >> 8) & 0xFF;
    pad_report[3 + pad_address] = pad_velocity;
    
    HidInReports_Send_Report (pad_report);
}

void HidInReports_Create_Button_Report (uint8_t button_number, 
                                        uint8_t button_value)
{
    uint8_t report_data[GENERIC_REPORT_SIZE];
    
    report_data[0] = 0x02; //button data command ID
    report_data[1] = button_number;
    report_data[2] = button_value;
    
    HidInReports_Send_Report (report_data);
}

void HidInReports_Create_Dial_Report (uint8_t dial_number, 
                                      uint8_t dial_value)
{
    uint8_t report_data[GENERIC_REPORT_SIZE];
    
    report_data[0] = 0x03; //dial data command ID
    report_data[1] = dial_number;
    report_data[2] = dial_value;
    
    HidInReports_Send_Report (report_data);
}

void HidInReports_Create_Host_Setup_Report (uint8_t firmware_version, 
                                            uint8_t device_type)
{
    uint8_t report_data[GENERIC_REPORT_SIZE];
    
    report_data[0] = 0x04; //host setup data command ID
    report_data[1] = firmware_version;
    report_data[2] = device_type;
    
    HidInReports_Send_Report (report_data);
}
