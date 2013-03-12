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

#define FIRST_PAD 0  
#define LAST_PAD 47 

#define THRESHOLD_UNDER_8 120
#define THRESHOLD 105
#define SETTLING_TIME 300
#define HYSTERISIS_ADJUST 15
#define STICKY_TIMEOUT 100
#define ANTI_STICKY_ADJUST 50

#define THRESHOLD_OVER_39 10
#define SETTLING_TIME_OVER_39 300
#define HYSTERISIS_ADJUST_OVER_39 10
#define ANTI_STICKY_ADJUST_OVER_39 80
#define STICKY_TIMEOUT_OVER_39 600

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

static void BubbleSort(uint8_t array_size)
{
  uint8_t i, j, temp;

  for (i = (array_size - 1); i > 0; i--)
  {
    for (j = 1; j <= i; j++)
    {
      if (r2r_val[j-1] > r2r_val[j])
      {
        temp = r2r_val[j-1];
        r2r_val[j-1] = r2r_val[j];
        r2r_val[j] = temp;

        temp = pad_order[j-1];
        pad_order[j-1] = pad_order[j];
        pad_order[j] = temp;
      }
    }
  }
}

void Calibrate (void)
{
    int16_t val;
    for (int pad = FIRST_PAD; pad < 40; pad++)
    {
        r2r_val[pad] = 0;
        MUX_Select(pad);
        Delay(pad);
        filtered_val[pad] = 0;
        if (pad < 40)
        {
            for (int i = 0; i < 64; i++)
            {
                R2R_Write(i);
                Delay(pad);
                val = -ADC_Read(DIFF_1_X10, ADC4);

                if (val > -400)
                {
                    r2r_val[pad] = i;
                    if (pad < 8)
                        init_val[pad] = val + THRESHOLD_UNDER_8;
                    else
                        init_val[pad] = val + THRESHOLD;
                    sei(); //enable interrupt
                    break;
                }
            }
        }
        pad_order[pad] = pad;
    }
    for (int pad = 40; pad <= LAST_PAD; pad++)
    {
        MUX_Select(pad);
        Delay(pad);
        init_val_se[pad] = ADC_Read(SINGLE_ENDED, ADC4) - THRESHOLD_OVER_39; 
        pad_order[pad] = pad;
    }

    BubbleSort(40);
    GenericHID_Clear();
    memset(being_played,        0, sizeof(bool) * (LAST_PAD+1+6));
    memset(anti_sticky_applied, 0, sizeof(bool) * (LAST_PAD+1));
    memset(hysterisis_applied,  0, sizeof(bool) * (LAST_PAD+1));
    memset(velocity_sent,       0, sizeof(bool) * (LAST_PAD+1));
    memset(filtered_val,        0, sizeof(int16_t) * (LAST_PAD+1));

}

//interrupt callback
ISR(TIMER1_COMPA_vect)
{
    // detect wether being operated within last 20 seconds
    // go into standby mode if not
    static int count = 0;
    bool any_played = false;

    for (int pad = FIRST_PAD; pad <= (LAST_PAD + 6); pad++)
    {
        any_played |= being_played[pad];
    }

    if (!any_played)
    {
        //count++;
        if (count > 20000)
        {
            count = 0;
            if (!thresholds_raised)
            {
                for (int pad = FIRST_PAD; pad <= LAST_PAD; pad++)
                {
                    init_val[pad] += 100; 
                }
                thresholds_raised = true;
            }
            //LED_Set_Current(0,0,0);

        }

    }
    else
    {
        if (thresholds_raised)
        {
            for (int pad = FIRST_PAD; pad <= LAST_PAD; pad++)
            {
                init_val[pad] -= 100; 
            }
            thresholds_raised = false;
        }
        //LED_Set_Current(127,127,127);
        count = 0;
    }

    //service the USB interface, send the data over HID
    GenericHID_Task();
    USB_USBTask();
} 


void Delay(uint8_t pad)
{
    cli(); //disable interrupts
    bool tr = thresholds_raised;
    sei(); //enable interrupts
    if (pad < 40)
    {
        if (tr)
        {
            _delay_ms(1);
        }
        else
        {
            _delay_us(SETTLING_TIME);
        }
    } 
    else 
    {
        if (tr)
        {
            _delay_ms(2);
        }
        else
        {
            _delay_us(SETTLING_TIME_OVER_39);
        }
    }
}

/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */
int main(void)
{

    uint8_t sticky_count[LAST_PAD+1];
    memset(sticky_count, 0, sizeof(uint8_t) * (LAST_PAD+1));  

    SetupHardware();

    // turn LED blue
    LED_Set_Colour(0,0,1023);

    //detect wether this is an elite unit
    MUX_Select(5);
    ButtonsAndDials_Read(5, NULL);
        
    Calibrate();

    sei(); //enable interrupts

    while (1) 
    {
        uint16_t led_sum = 0;

        for (uint8_t i = FIRST_PAD; i <= LAST_PAD; i++) 

        {

            uint8_t pad = pad_order[i];

            if (!bit_is_set(PINE, PE2))
            {
                being_played[LAST_PAD+6] = true;
                // turn LED blue
                LED_Set_Colour(0,0,1023);
                cli(); //disable interrupts
                thresholds_raised = false;
                Calibrate();
                sei(); //enable interrupts
                while(!bit_is_set(PINE, PE2)); //wait
            }
            else
                being_played[LAST_PAD+6] = false;

            R2R_Write(r2r_val[i]);

            cli(); //disable interrupts
            for (int i = 0; i < 5; i++)
            {
                MUX_Select(i | (pad & 0b111111000));
                ButtonsAndDials_Read(i, &being_played[LAST_PAD+1]);
            }
            sei(); //enable interrupts

            MUX_Select(pad);
            Delay(pad);

            if (pad < 40) 
            { 
                int16_t val = -ADC_Read(DIFF_1_X10, ADC4) - init_val[pad];
                //GenericHID_Write_PadData(pad, r2r_val[i], i);

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

                        cli(); //disable interrupts
                        GenericHID_Write_PadData(pad, velocity, velocity);
                        being_played[pad] = true;
                        sei(); //enable interrupts

                        velocity_sent[pad] = true;
                        filtered_val[pad] = velocity;
                        led_sum += filtered_val[pad];
                        init_val[pad] -= HYSTERISIS_ADJUST;
                    }
                    else if (velocity_sent[pad])
                    {
                        filtered_val[pad] = ((filtered_val[pad] * 0.50) + (val * 0.50));


                        if (filtered_val[pad] >= 511)
                        {
                            filtered_val[pad] = 511;
                            if (!anti_sticky_applied[pad])
                            {
                                cli();
                                init_val[pad] += ANTI_STICKY_ADJUST;
                                anti_sticky_applied[pad] = true;
                                sei();
                            }
                        }

                        cli(); //disable interrupts
                        GenericHID_Write_PressureOnly(pad, filtered_val[pad]);
                        being_played[pad] = true;
                        sei(); //enable interrrupts

                        led_sum += filtered_val[pad];
                    }

                }
                else if (velocity_sent[pad])
                {
                    cli(); //disable interrupts
                    if (anti_sticky_applied[pad])
                    {
                        sticky_count[pad]++;
                        if (sticky_count[pad] > STICKY_TIMEOUT)
                        {
                            init_val[pad] -= ANTI_STICKY_ADJUST;
                            anti_sticky_applied[pad] = false;
                            sticky_count[pad] = 0;
                        }
                    }
                    GenericHID_Write_PadData(pad, 0, 0);
                    being_played[pad] = false;
                    sei(); //enable interrrupts

                    velocity_sent[pad] = false;
                    init_val[pad] += HYSTERISIS_ADJUST;
                }
            }
            else // if pad >= 40
            {

                int16_t val = init_val_se[pad] - ADC_Read(SINGLE_ENDED, ADC4);

                if (val > 0)
                {
                    if (!hysterisis_applied[pad])
                    {
                        init_val_se[pad] += HYSTERISIS_ADJUST_OVER_39;
                        hysterisis_applied[pad] = true;
                    }
                    cli(); //disable interrupts
                    filtered_val[pad] = ((filtered_val[pad] * 0.50) + ((val * 2) * 0.50));

                    if (filtered_val[pad] >= 511)
                    {
                        filtered_val[pad] = 511;
                        if (!anti_sticky_applied[pad])
                        {
                            cli();
                            init_val_se[pad] -= ANTI_STICKY_ADJUST_OVER_39;
                            anti_sticky_applied[pad] = true;
                            sei();
                        }
                    }

                    GenericHID_Write_PadData(pad, filtered_val[pad], 127);
                    being_played[pad] = true;
                    sei(); //enable interrupts
                    led_sum += filtered_val[pad];
                }
                else
                {
                    if (hysterisis_applied[pad])
                    {
                        init_val_se[pad] -= HYSTERISIS_ADJUST_OVER_39;
                        hysterisis_applied[pad] = false;
                    }

                    cli(); //disable interrupts
                    if (anti_sticky_applied[pad])
                    {
                        sticky_count[pad]++;
                        if (sticky_count[pad] > STICKY_TIMEOUT_OVER_39)
                        {
                            init_val_se[pad] += ANTI_STICKY_ADJUST_OVER_39;
                            anti_sticky_applied[pad] = false;
                            sticky_count[pad] = 0;
                        }
                    }
                    GenericHID_Write_PadData(pad, 0, 0);
                    being_played[pad] = false;
                    sei(); //enable interrupts
                }
            }
        }
        //fade the led blue->green for 0-511 and green->red for 511-1023 total pressure

        if (led_sum > 0)
            led_sum = (led_sum << 1) | 1;
        else
            led_sum = 0;

        if (led_sum <= 1023)
        {
            LED_Set_Colour(0, led_sum, (1023 - led_sum));
        }
        else  
        {
            led_sum -= 1022;

            if (led_sum > 1023)
                led_sum = 1023;

            LED_Set_Colour(led_sum, (1023 - led_sum), 0);
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
