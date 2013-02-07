/*
   LUFA Library
   Copyright (C) Dean Camera, 2012.

   dean [at] fourwalledcubicle [dot] com
   www.lufa-lib.org
   */

/*
   Copyright 2012  Dean Camera (dean [at] fourwalledcubicle [dot] com)

   Permission to use, copy, modify, distribute, and sell this
   software and its documentation for any purpose is hereby granted
   without fee, provided that the above copyright notice appear in
   all copies and that both that the copyright notice and this
   permission notice and warranty disclaimer appear in supporting
   documentation, and that the name of the author not be used in
   advertising or publicity pertaining to distribution of the
   software without specific, written prior permission.

   The author disclaim all warranties with regard to this
   software, including all implied warranties of merchantability
   and fitness.  In no event shall the author be liable for any
   special, indirect or consequential damages or any damages
   whatsoever resulting from loss of use, data or profits, whether
   in an action of contract, negligence or other tortious action,
   arising out of or in connection with the use or performance of
   this software.
   */

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

/** \file
 *
 *  Main source file for the GenericHID demo. This file contains the main tasks of the demo and
 *  is responsible for the initial application hardware configuration.
 */

#include "SphereWare.h"

#define FIRST_PAD 0  
#define LAST_PAD 47 
#define LOOK_AT_PAD 0


//this is the calibration procedure
void Calibrate (uint8_t* r2r_value_array)
{
    int16_t val;
    for (int pad = FIRST_PAD; pad <= LAST_PAD; ++pad)
    {
        MUX_Select(pad);
        _delay_ms(1);
        for (int i = 0; i < 64; ++i)
        {
            R2R_Write(i);
            _delay_ms(1);
            val = ADC_Read(DIFF_0_X10, ADC4);

            if (val < 40)
            {
                r2r_value_array[pad] = i;
                break;
            }
        }
        //keep the USB connection alive
        USB_USBTask();
        HID_Task();
    }
}



/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */
int main(void)
{

    bool led_on = true;
    int led_channels[NUM_OF_LEDS][3];
    int16_t val = 0;
    int16_t peak[LAST_PAD+1] = {0};
    uint8_t r2r_values[LAST_PAD+1];
    bool velocity_sent[LAST_PAD+1] = {false};
    uint8_t sample_count[LAST_PAD+1] = {0};

    SetupHardware();

    sei();

    // turn LED blue
    for (int i = 0; i < NUM_OF_LEDS; ++i)
    {
        led_channels[i][0] = 0;
        led_channels[i][1] = 0;
        led_channels[i][2] = 1023;
    }

    LED_WriteArray(led_channels);

    Calibrate(r2r_values);

    MUX_Select(FIRST_PAD);
    R2R_Write(r2r_values[FIRST_PAD]);
    _delay_us(90);

    while (1) 
    {
        MUX_Select(FIRST_PAD);
        R2R_Write(r2r_values[FIRST_PAD]);
        _delay_us(10);

        for (int pad = FIRST_PAD; pad <= LAST_PAD; ++pad)
        {
            _delay_us(10);
            val = ADC_Read(DIFF_0_X10, ADC4);
            //if (pad == LOOK_AT_PAD)
            //    HidInReports_Create_Pad_Report(pad, val, val);
            if (val < 0)
            {
                if (!velocity_sent[pad])
                {
                    for (int i = 0; i < 200; ++i)
                    {
                        val = ADC_Read(DIFF_0_X10, ADC4);
                        if (val < peak[pad])
                        {
                            peak[pad] = val;
                        }
                    }
                    if ((-peak[pad] >> 2) > 0)
                    {
                        //if (pad == LOOK_AT_PAD)
                        HidInReports_Create_Pad_Report(pad, -peak[pad], -peak[pad] >> 2);
                        peak[pad] = 0;
                        velocity_sent[pad] = true;
                    }
                }
                else
                {
                    //if (pad == LOOK_AT_PAD)
                    HidInReports_Create_Pad_Report(pad, -val, 0);
                }
            }
            else
            {
                if (velocity_sent[pad])
                {
                    HidInReports_Create_Pad_Report(pad, 0, 0);
                }
                velocity_sent[pad] = false;
                sample_count[pad] = 0;
                peak[pad] = 0;
            }

            if (pad < LAST_PAD)
            {
                MUX_Select(pad+1);
                R2R_Write(r2r_values[pad+1]);
            }

            USB_USBTask();
            HID_Task();
        }

    }

}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    /* Disable clock division */
    clock_prescale_set(clock_div_1);

    /* Hardware Initialization */
    USB_Init();
    LED_Init();
    ADC_Init();
    R2R_Init();
    MIDI_Init();

    //PE2 button as input pulled high
    DDRE |= (1 << PE2);
    PORTE |= (1 << PE2);

    MUX_Init();
}

/** Event handler for the USB_Connect event. Starts the library USB task to begin the 
 * enumeration and USB management process.
 */
void EVENT_USB_Device_Connect(void)
{
    /* Indicate USB enumerating */
}

/** Event handler for the USB_Disconnect event. Stops the USB management task.
*/
void EVENT_USB_Device_Disconnect(void)
{
    /* Indicate USB not ready */
}

/** Event handler for the USB_ConfigurationChanged event. This is fired when the host sets the current configuration
 *  of the USB device after enumeration, and configures the generic HID device endpoints.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
{
    bool ConfigSuccess = true;

    /* Setup HID Report Endpoints */
    ConfigSuccess &= Endpoint_ConfigureEndpoint(GENERIC_IN_EPADDR, EP_TYPE_INTERRUPT, GENERIC_EPSIZE, 1);
    ConfigSuccess &= Endpoint_ConfigureEndpoint(GENERIC_OUT_EPADDR, EP_TYPE_INTERRUPT, GENERIC_EPSIZE, 1);

    /* Setup MIDI Data Endpoints */
    ConfigSuccess &= Endpoint_ConfigureEndpoint(MIDI_STREAM_IN_EPADDR, EP_TYPE_BULK, MIDI_STREAM_EPSIZE, 1);
    ConfigSuccess &= Endpoint_ConfigureEndpoint(MIDI_STREAM_OUT_EPADDR, EP_TYPE_BULK, MIDI_STREAM_EPSIZE, 1);

    /* Indicate endpoint configuration success or failure */
}

/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequest(void)
{
    /* Handle HID Class specific requests */
    switch (USB_ControlRequest.bRequest)
    {
        case HID_REQ_GetReport:
            if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
            {
                uint8_t GenericData[GENERIC_REPORT_SIZE];
                CreateGenericHIDReport(GenericData);

                Endpoint_ClearSETUP();

                /* Write the report data to the control endpoint */
                Endpoint_Write_Control_Stream_LE(&GenericData, sizeof(GenericData));
                Endpoint_ClearOUT();
            }

            break;
        case HID_REQ_SetReport:
            if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
            {
                uint8_t GenericData[GENERIC_REPORT_SIZE];

                Endpoint_ClearSETUP();

                /* Read the report data from the control endpoint */
                Endpoint_Read_Control_Stream_LE(&GenericData, sizeof(GenericData));
                Endpoint_ClearIN();

                ProcessGenericHIDReport(GenericData);
            }

            break;
    }
}

/** Function to process the last received report from the host.
 *
 *  \param[in] DataArray  Pointer to a buffer where the last received report has been stored
 */
void ProcessGenericHIDReport(uint8_t* DataArray)
{
    /*
       This is where you need to process reports sent from the host to the device. This
       function is called each time the host has sent a new report. DataArray is an array
       holding the report sent from the host.
       */

    // Received a MIDI message report
    if (DataArray[0] == 0x06) 
    {
        MIDI_Send_Usb_Midi (DataArray);
        MIDI_Send_Uart_Midi (DataArray);
    }

    // Received a host setup data request report
    else if (DataArray[0] == 0x05) 
    {
        HidInReports_Create_Host_Setup_Report (1, 0);
    }
}

/** Function to create the next report to send back to the host at the next reporting interval.
 *
 *  \param[out] DataArray  Pointer to a buffer where the next report data should be stored
 */
void CreateGenericHIDReport(uint8_t* DataArray)
{
    /*
       This is where you need to create reports to be sent to the host from the device. This
       function is called each time the host is ready to accept a new report. DataArray is
       an array to hold the report to the host.
       */
}

void HID_Task(void)
{
    /* Device must be connected and configured for the task to run */
    if (USB_DeviceState != DEVICE_STATE_Configured)
        return;

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
            ProcessGenericHIDReport(GenericData);
        }

        /* Finalize the stream transfer to send the last packet */
        Endpoint_ClearOUT();
    }
}

