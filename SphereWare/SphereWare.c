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
void Calibrate (uint8_t* r2r_val, int16_t * init_val, int16_t * init_val_single_ended)
{
    int16_t val;
    for (int pad = FIRST_PAD; pad <= LAST_PAD; pad++)
    {
        r2r_val[pad] = 0;
        MUX_Select(pad);
        _delay_ms(1);
        if (pad < 40)
        {
            for (int i = 0; i < 64; i++)
            {
                R2R_Write(i);
                _delay_ms(1);
                val = -ADC_Read(DIFF_1_X10, ADC4);

                if (val > -400)
                {
                    r2r_val[pad] = i;
                    init_val[pad] = val + 50;
                    break;
                }
            }
        }
        else //if pad >= 40
        {
            for (int i = 0; i < 64; i++)
            {
                R2R_Write(i);
                _delay_ms(1);
                val = -ADC_Read(DIFF_1_X10, ADC4);

                if (val > 40)
                {
                    r2r_val[pad] = i;
                    init_val[pad] = val + 50;
                    break;
                }
            }
        }
    }
    for (int pad = FIRST_PAD; pad <= LAST_PAD; pad++)
    {
        MUX_Select(pad);
        init_val_single_ended[pad] = ADC_Read(SINGLE_ENDED, ADC4);
    }

}

//interrupt callback
ISR(TIMER1_COMPA_vect)
{
    GenericHID_Task();
    USB_USBTask();
} 


/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */
int main(void)
{

    bool level_adjusted[LAST_PAD+1];
    int led_channels[NUM_OF_LEDS][3];
    uint8_t r2r_val[LAST_PAD+1];
    int16_t init_val[LAST_PAD+1];
    int16_t init_val_se[LAST_PAD+1];
    int16_t filtered_val[LAST_PAD+1];

    SetupHardware();

    sei();

    Calibrate(r2r_val, init_val, init_val_se);

    // turn LED blue
    for (int i = 0; i < NUM_OF_LEDS; i++)
    {
        led_channels[i][0] = 0;
        led_channels[i][1] = 0;
        led_channels[i][2] = 1023;
    }

    LED_WriteArray(led_channels);

    MUX_Select(FIRST_PAD);
    R2R_Write(r2r_val[FIRST_PAD]);
    _delay_us(90);

    bool velocity_sent[LAST_PAD+1];

    while (1) 
    {

        MUX_Select(FIRST_PAD);
        R2R_Write(r2r_val[FIRST_PAD]);
        _delay_us(50);
        

        for (uint8_t pad = FIRST_PAD; pad <= LAST_PAD; pad++) 
        {
            MUX_Select(pad);
            R2R_Write(r2r_val[pad]);
            if (!(pad % 8))
                _delay_us(200);
            _delay_us(250);
            ButtonsAndDials_Read(pad);

            if (pad < 40) 
            { 
                int16_t val = -ADC_Read(DIFF_1_X10, ADC4) - init_val[pad];
                //GenericHID_Write_DebugData(pad, init_val[pad]);

                if (val > 0)
                {
                    if (!velocity_sent[pad])
                    {
                        int16_t velocity;
                        int16_t peak = val;
                        for (int i = 0; i < 200; i++)
                        {
                            val = -ADC_Read(DIFF_1_X10, ADC4) - init_val[pad];
                            if (val > peak)
                            {
                                peak = val;
                            }
                        }
                        velocity = peak >> 1;
                        if (velocity > 127)
                            velocity = 127;
                        GenericHID_Write_PadData(pad, velocity, velocity);
                        velocity_sent[pad] = true;
                        filtered_val[pad] = velocity;
                    }
                    else 
                    {
                        if (val > (480 - init_val[pad]))
                            val = -ADC_Read(DIFF_1_X1, ADC4) - 48 + 480 - init_val[pad];


                        filtered_val[pad] = ((filtered_val[pad] * 0.70) + (val * 0.30));


                        if (filtered_val[pad] > 511)
                            filtered_val[pad] = 511;


                        GenericHID_Write_PressureOnly(pad, filtered_val[pad]);
                    }

                }
                else if (velocity_sent[pad])
                {
                    GenericHID_Write_PadData(pad, 0, 0);
                    velocity_sent[pad] = false;
                }
            }
            else // if pad >= 40
            {
                int16_t val = -ADC_Read(DIFF_1_X10, ADC4) - init_val[pad];

                if (val > 0)
                {
                    if (!velocity_sent[pad])
                    {
                        int16_t velocity;
                        int16_t peak;

                        peak = val;

                        for (int i = 0; i < 200; i++)
                        {
                            val = ADC_Read(DIFF_1_X10, ADC4) - init_val[pad];
                            if (val > peak)
                            {
                                peak = val;
                            }
                        }
                        velocity = peak >> 1;
                        if (velocity > 127)
                            velocity = 127;

                        GenericHID_Write_PadData(pad, velocity, velocity);
                        velocity_sent[pad] = true;
                        filtered_val[pad] = velocity * 1.5;
                    }
                    else 
                    {
                        if (val > (480 - init_val[pad]))
                            val = -ADC_Read(DIFF_1_X1, ADC4) - 48 + 480 - init_val[pad];

                        filtered_val[pad] = ((filtered_val[pad] * 0.70) + (val * 0.30)) * 1.5;

                        if (filtered_val[pad] > 511)
                        {
                            filtered_val[pad] = 511;
                        }

                        GenericHID_Write_PressureOnly(pad, filtered_val[pad]);
                    }

                }
                else if (velocity_sent[pad])
                {
                    GenericHID_Write_PadData(pad, 0, 0);
                    velocity_sent[pad] = false;
                }
            }

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
    MUX_Init();
    LED_Init();
    ADC_Init();
    R2R_Init();
    MIDI_Init();
    ButtonsAndDials_Init();

    //PE2, reset button (SW1) as input pulled high
    DDRE |= (1 << PE2);
    PORTE |= (1 << PE2);

    // enable interrupt for USB and HID tasks
    TCCR1B |= (1 << WGM12); // Configure timer 1 for CTC mode 
    TIMSK1 |= (1 << OCIE1A); // Enable CTC interrrupt

    OCR1AL = 250; // Set CTC compare to 1000Hz for 16Mhz/64
    TCCR1B |= ((1 << CS10) | (1 << CS11)); // Start timer at Fcpu/64 
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

    ///* Handle HID Class specific requests */
    //switch (USB_ControlRequest.bRequest)
    //{
    //    case HID_REQ_GetReport:
    //        if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
    //        {
    //            uint8_t GenericData[GENERIC_REPORT_SIZE];
    //            CreateGenericHIDReport(GenericData);

    //            Endpoint_ClearSETUP();

    //            /* Write the report data to the control endpoint */
    //            Endpoint_Write_Control_Stream_LE(&GenericData, sizeof(GenericData));
    //            Endpoint_ClearOUT();
    //        }

    //        break;
    //    case HID_REQ_SetReport:
    //        if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
    //        {
    //            uint8_t GenericData[GENERIC_REPORT_SIZE];

    //            Endpoint_ClearSETUP();

    //            /* Read the report data from the control endpoint */
    //            Endpoint_Read_Control_Stream_LE(&GenericData, sizeof(GenericData));
    //            Endpoint_ClearIN();

    //            ProcessGenericHIDReport(GenericData);
    //        }

    //        break;
    //}
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

}

