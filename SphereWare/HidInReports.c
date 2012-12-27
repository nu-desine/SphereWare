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

void HidInReports_Create_Pad_Report (uint8_t pad_number, 
                                     int pad_value, 
                                     uint8_t pad_velocity)
{
    uint8_t report_data[GENERIC_REPORT_SIZE];
    
    report_data[0] = 0x01; //pad data command ID
    report_data[1] = pad_number;
    report_data[2] = pad_value & 0xFF;
    report_data[3] = (pad_value >> 8) & 0xFF;
    report_data[4] = pad_velocity;
    
    HidInReports_Send_Report (report_data);
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
