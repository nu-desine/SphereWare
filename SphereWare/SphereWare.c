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

/** \
 *
 *  Main source file for the SphereWare. This file contains the main tasks and
 *  is responsible for the initial application hardware configuration.
 */

#include "SphereWare.h"
#include <avr/eeprom.h> 

#define FIRST_PAD 0  
#define LAST_PAD 47 

#define THRESHOLD_UNDER_8 120
#define THRESHOLD 105
#define SETTLING_TIME 300
#define HYSTERISIS_ADJUST 25
#define STICKY_TIMEOUT 10
#define ANTI_STICKY_ADJUST 50

#define HYSTERISIS_ADJUST_UNDER_16 35

#define THRESHOLD_OVER_39 40
#define SETTLING_TIME_OVER_39 300
#define HYSTERISIS_ADJUST_OVER_39 10
#define ANTI_STICKY_ADJUST_OVER_39 0
#define STICKY_TIMEOUT_OVER_39 0

int16_t filtered_val[LAST_PAD+1];
int16_t init_val[LAST_PAD+1];
int16_t init_val_se[LAST_PAD+1]; //single ended
uint8_t r2r_val[LAST_PAD+1];
bool anti_sticky_applied[LAST_PAD+1];
bool hysterisis_applied[LAST_PAD+1];
bool velocity_sent[LAST_PAD+1];
bool being_played[LAST_PAD+1+6]; //+6 to include elite controls and reset
bool thresholds_raised = false;

uint8_t pad_order[LAST_PAD+1]; 

//interrupt callback
ISR(TIMER1_COMPA_vect)
{
    //service the USB interface, send the data over HID
    GenericHID_Task();
    USB_USBTask();
} 


/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */
int main(void)
{

    uint16_t sticky_count[LAST_PAD+1];
    memset(sticky_count, 0, sizeof(uint16_t) * (LAST_PAD+1));

    SetupHardware();

    // turn LED blue
    LED_Set_Colour(0,0,1023);

    sei(); //enable interrupts

    bool test_passed = true;
    bool ee_test_passed = (eeprom_read_byte ((const uint8_t *)1)) == 0xA;

    if (ee_test_passed)
        goto set_led;

    int32_t val_array[LAST_PAD+1];
    int32_t diff_array[LAST_PAD+1];
    int16_t prev_val[LAST_PAD+1];
    memset(val_array, 0, sizeof(int32_t) * (LAST_PAD+1));
    memset(diff_array, 0, sizeof(int32_t) * (LAST_PAD+1));
    memset(prev_val, 0, sizeof(int16_t) * (LAST_PAD+1));
    int32_t failed = 1023;

    int16_t thresholds[8][2] = {{620, 650}
                               ,{600, 640}
                               ,{580, 610}
                               ,{540, 580}
                               ,{490, 520}
                               ,{450, 490}
                               ,{390, 410}
                               ,{240, 350}};

    //while (1) 
#define NUM_AVG 100 
    for (uint8_t i = 0; i < NUM_AVG; i++)
    {
        uint16_t led_sum = 0;

        for (uint8_t pad = FIRST_PAD; pad <= LAST_PAD; pad++) 
        {

            MUX_Select(pad);
            _delay_us(400);
            int16_t val = ADC_Read(SINGLE_ENDED, ADC4);
            val_array[pad] += val; 
            if (i > 0)
            { 
                diff_array[pad] += (prev_val[pad] - val);
            }
            prev_val[pad] = val;

        }
    }
    for (uint8_t pad = FIRST_PAD; pad <= LAST_PAD; pad++) 
    {
        val_array[pad] /= NUM_AVG;
    }

    for (int i = 0; i < 48; i+=8)
    {
        for (int j = 0; j < 8; j++)
        {
            uint8_t pad = i+j;

            if (j > 0)
            {
                if (val_array[pad] > val_array[pad - 1])
                {
                    test_passed = false;
                    failed = pad;
                    goto end;
                }
            }

            if ((val_array[pad] < thresholds[j][0]) || (val_array[pad] >= thresholds[j][1]))
            {
                test_passed = false;
                failed = pad;
                goto end;
            }


            if ((diff_array[pad] > 0) && (diff_array[pad] > 20))
            {
                test_passed = false;
                failed = pad + 100;
                goto end;
            }
            else if ((diff_array[pad] < 0) && (diff_array[pad] < -20))
            {
                test_passed = false;
                failed = pad + 200;
                goto end;
            }

        }
    }

    R2R_Write(0);

    bool r2r_ok = true;
    int16_t prev_r2r_val = 0;
    for(int i = 0; i < 32; ++i)
    {
        R2R_Write(i);
        _delay_ms(1);
        int16_t val = ADC_Read(0,1);

        r2r_ok &= (val >= prev_r2r_val) && (val > 450) && (val < 550);

        prev_r2r_val = val;
    }

    if (!r2r_ok)
        test_passed = false;

    if (!GenericHID_Get_PingAck())
    {
        test_passed = false;
        failed = 66;
    }

end:
    // turn LED yellow 
    LED_Set_Colour(511,511,0);

    while(bit_is_set(PINE, PE2));//wait

    if (test_passed)
        eeprom_write_byte((const uint8_t *)1, 0xA);

set_led:
        if (test_passed)
            LED_Set_Colour(0,1023,0);
        else
            LED_Set_Colour(1023,0,0);


        while (1)
        {
            cli();
            GenericHID_Write_ButtonData(100);
            sei();

            //for (uint8_t pad = 32; pad <= 39; pad++) 
            //{
            //    MUX_Select(pad);
            //    _delay_ms(10);
            //    int16_t val = ADC_Read(SINGLE_ENDED, ADC4);
            //    GenericHID_Write_DebugData(pad, val);
            //    GenericHID_Write_DebugData(0, failed);
            //    sei();
            //}
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
