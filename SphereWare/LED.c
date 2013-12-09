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
#include "LED.h"
#include <avr/interrupt.h>

uint16_t setting[3];
// Set pins to outputs and initial states
void LED_Init() {
    CLKREG |= (1 << CLKPIN);
    ENAREG |= (1 << ENAPIN);
    LATREG |= (1 << LATPIN);
    DATREG |= (1 << DATPIN);

    LATPORT |= ~(1 << LATPIN);
    ENAPORT |= ~(1 << ENAPIN);

    LED_Status = LED_Pressure_Status = 1;
    LED_Mode = LED_Clock_Status = LED_Clock_Running = 0;
    LED_Fade_Step = 100;
    LED_Tempo = 120;
    LED_Set_Current(127, 127, 127); // << what does this do? is it still needed?
    
    for (int i = 0; i < 3; i ++)
    {
        LED_Static_Colour_Values[i] = 0;
    }
    
    //set default colours
    LED_Set_Colour_Values (0, 0, 0, 255);
    LED_Set_Colour_Values (1, 0, 255, 0);
    LED_Set_Colour_Values (2, 255, 0, 0);
}

static void LED_SendPacket() {

    cli(); //disable interrupts, the ISR is touching this data through LED_Set_Current
    if (LED_CommandMode == 0b01) {
        LED_RedCommand   = setting[0];
        LED_GreenCommand = setting[1];
        LED_BlueCommand  = setting[2];
    }
    sei(); //enable interrupts

    led_command_packet = LED_CommandMode & 0b11;
    led_command_packet = (led_command_packet << 10)  | (LED_BlueCommand & 1023);
    led_command_packet = (led_command_packet << 10)  | (LED_RedCommand & 1023);
    led_command_packet = (led_command_packet << 10)  | (LED_GreenCommand & 1023);

    for (int j = 0; j < 32; j++) {
        if ((led_command_packet >> (31 - j)) & 1) {
            DATPORT |= (1 << DATPIN);
        } 
        else {
            DATPORT &= ~(1 << DATPIN);
        }
        CLKPORT |= (1 << CLKPIN);
        CLKPORT &= ~(1 << CLKPIN); 
    } 
}

void LED_Set_Current(uint16_t red, uint16_t green, uint16_t blue)
{
    setting[0] = red;
    setting[1] = green;
    setting[2] = blue;
}

static void LED_Latch() {

    _delay_us(1);
    LATPORT |= (1 << LATPIN);
    ENAPORT |= (1 << ENAPIN);
    _delay_us(1);
    ENAPORT &= ~(1 << ENAPIN);
    LATPORT &= ~(1 << LATPIN);

}

static void LED_WriteArray(int led_channels[NUM_OF_LEDS][3]) 
{


    LED_CommandMode = 0b00; // Write to PWM control registers

    for (int i = 0; i < NUM_OF_LEDS; i++) {
        LED_RedCommand = led_channels[i][0];
        LED_GreenCommand = led_channels[i][1];
        LED_BlueCommand = led_channels[i][2];
        LED_SendPacket();
    }

    LED_Latch();

    LED_CommandMode = 0b01; // Write to current control registers

    for (int z = 0; z < NUM_OF_LEDS; z++) LED_SendPacket();   

    LED_Latch();

}


// Example loop to cycle all LEDs through primary colors
void LED_TestLoop(void) {

    // Create LED value storage array
    int led_channels[NUM_OF_LEDS][3] = {0};

    for (int i = 0; i < NUM_OF_LEDS; i++) {
        led_channels[i][0] = 1023;
        led_channels[i][1] = 0;
        led_channels[i][2] = 0;
    }

    LED_WriteArray(led_channels);

    _delay_ms(200);

    for (int i = 0; i < NUM_OF_LEDS; i++) {
        led_channels[i][0] = 0;
        led_channels[i][1] = 1023;
        led_channels[i][2] = 0;
    }

    LED_WriteArray(led_channels);

    _delay_ms(200);

    for (int i = 0; i < NUM_OF_LEDS; i++) {
        led_channels[i][0] = 0;
        led_channels[i][1] = 0;
        led_channels[i][2] = 1023;
    }

    LED_WriteArray(led_channels);

    _delay_ms(200);

}

void LED_Set_Colour(uint16_t red, uint16_t green, uint16_t blue)
{
    int led_channels[NUM_OF_LEDS][3];
    for (int i = 0; i < NUM_OF_LEDS; i++) 
    {
        led_channels[i][0] = red;
        led_channels[i][1] = green;
        led_channels[i][2] = blue;
    }
    LED_WriteArray(led_channels);
}

void LED_Set_Status(uint8_t status)
{
    LED_Status = status;
    
    if (LED_Mode == 0)
    {
        if (status == 0)
        {
            LED_Set_Colour(0, 0, 0);
        }
        else
        {
            LED_Set_Colour(LED_Colour_Values[0][0],
                           LED_Colour_Values[0][1],
                           LED_Colour_Values[0][2]);
        }
    }
}

void LED_Set_Pressure_Status(uint8_t status)
{
    LED_Pressure_Status = status;
    
    
    if (LED_Mode == 0)
    {
        if (status == 0 && LED_Status == 1)
        {
            LED_Set_Colour(LED_Colour_Values[0][0],
                           LED_Colour_Values[0][1],
                           LED_Colour_Values[0][2]);
        }
    }
}

void LED_Set_Colour_Values (uint8_t colour, uint8_t red, uint8_t green, uint8_t blue)
{
    /*
     
     The red, green, and blue values coming in here are in the range of 0 - 255.
     The values being sent to the LED can be in the range of 0 - 1023.
     However the sum of the 3 values being sent to the LED can not exceed 1023.
     Therefore, we want to scale up the values so that the LED is the brightest it can
     be for any given colour, however at the same time not allowing the sum of the values
     to go beyond the max limit.
     
     We cannot simply divide the excess value by 3 and delete that from each colour value 
     as this could change the actual colour. The correct way to do this is as follows:
     
     1. Scale all 3 colour values to be in the range of 0 - 1023.
     2. Work out the sum of all three scaled values.
     3. If this is below/equal to 1023, use these values. If the sum is greater than 1023,
        go to the next step.
     4. Find the percentage of each of the values in the sum of the values.
     5. Find the excess value (sum of values - 1023).
     6. Find the value of each percentage found in step 4 applied to the excess value.
     7. Decrease each value found in step 6 from the corresponding colours values.
     8. The 3 values found in step 7 are the new colour values.
     
     */
    
    //scale the values to the 0 - 1023 range
    float red_scaled = (1023.0 * (float)red) / 255.0;
    float green_scaled = (1023.0 * (float)green) / 255.0;
    float blue_scaled = (1023.0 * (float)blue) / 255.0;
    
    //work out the sum of the values
    float colours_sum = red_scaled + green_scaled + blue_scaled;
    
    if (colours_sum > 1023)
    {
        //get the percentages of the values against the total sum
        float red_perc = red_scaled / colours_sum;
        float green_perc = green_scaled / colours_sum;
        float blue_perc = blue_scaled / colours_sum;
        
        //get the excess value
        int excess_value = colours_sum - 1023;
        
        //apply the percentages to the excess value and decrease from the colour values
        red_scaled = red_scaled - (excess_value * red_perc);
        green_scaled = green_scaled - (excess_value * green_perc);
        blue_scaled = blue_scaled - (excess_value * blue_perc);
    }
    
    //set the new colour values
    LED_Colour_Values [colour][0] = red_scaled;
    LED_Colour_Values [colour][1] = green_scaled;
    LED_Colour_Values [colour][2] = blue_scaled;
}

void LED_Set_Static_Colour (uint8_t colour, uint8_t colour_value)
{
    //see the above function for a description of this scaling algorithm...
    
    //get the current static colour vales
    float red_scaled = LED_Static_Colour_Values[0];
    float green_scaled = LED_Static_Colour_Values[1];
    float blue_scaled = LED_Static_Colour_Values[2];
    
    //scale the changed value to the 0 - 1023 range
    if (colour == 0)
        red_scaled = (1023.0 * (float)colour_value) / 127.0;
    else if(colour == 1)
        green_scaled = (1023.0 * (float)colour_value) / 127.0;
    else if (colour = 2)
        blue_scaled = (1023.0 * (float)colour_value) / 127.0;
    
    //work out the sum of the values
    float colours_sum = red_scaled + green_scaled + blue_scaled;
    
    if (colours_sum > 1023)
    {
        //get the percentages of the values against the total sum
        float red_perc = red_scaled / colours_sum;
        float green_perc = green_scaled / colours_sum;
        float blue_perc = blue_scaled / colours_sum;
        
        //get the excess value
        int excess_value = colours_sum - 1023;
        
        //apply the percentages to the excess value and decrease from the colour values
        red_scaled = red_scaled - (excess_value * red_perc);
        green_scaled = green_scaled - (excess_value * green_perc);
        blue_scaled = blue_scaled - (excess_value * blue_perc);
    }
    
    //set the new colour
    LED_Static_Colour_Values[0] = red_scaled;
    LED_Static_Colour_Values[1] = green_scaled;
    LED_Static_Colour_Values[2] = blue_scaled;
    
    //Initially I tried changing the colour of the LED here as soon as
    //a value was received from the MIDI input. However this caused weird
    //errors with the firmware->software comms. Therefore now the actual
    //LED is changed colour in the programs main loop in SphereWare.
}




