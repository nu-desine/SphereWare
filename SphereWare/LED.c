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

// Set pins to outputs and initial states
void LED_Init() {
    //pinMode(datapin, OUTPUT);
    //pinMode(latchpin, OUTPUT);
    //pinMode(enablepin, OUTPUT);
    //pinMode(clockpin, OUTPUT);
    //digitalWrite(latchpin, LOW);
    //digitalWrite(enablepin, LOW);
    CLKREG |= (1 << CLKPIN);
    ENAREG |= (1 << ENAPIN);
    LATREG |= (1 << LATPIN);
    DATREG |= (1 << DATPIN);

    LATPORT |= ~(1 << LATPIN);
    ENAPORT |= ~(1 << ENAPIN);

}

void LED_SendPacket() {

    if (LED_CommandMode == 0b01) {
        LED_RedCommand = 127;
        LED_GreenCommand = 110;
        LED_BlueCommand = 110;
    }

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

void LED_Latch() {

    _delay_us(1);
    LATPORT |= (1 << LATPIN);
    ENAPORT |= (1 << ENAPIN);
    _delay_us(1);
    ENAPORT &= ~(1 << ENAPIN);
    LATPORT &= ~(1 << LATPIN);

}

void LED_WriteArray(int led_channels[NUM_OF_LEDS][3]) 
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
