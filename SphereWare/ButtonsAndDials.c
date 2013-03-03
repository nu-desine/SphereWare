#include "ButtonsAndDials.h"
#include <avr/io.h>
#include "GenericHID.h"

void ButtonsAndDials_Init(void)
{
    PORTD = (1 << PD6) | (1 << PD0) | (1 << PD1);
}


/*
   The MUX for the elite buttons and rotary encoders is on the same 
   MUX_A,B,C lines as the pads MUXs but it's always selected (INH tied to GND)

   This function has to be fed MUX 5 to detect whether the elite buttons and dials
   are there at all. After doing this once it can be cycled between MUX0..4 for 
   normal operation.
*/

void ButtonsAndDials_Read(uint8_t pad)
{
    typedef enum {
        SW3,
        SW2,
        ENC1B,
        SW4,
        ENC2B,
        ELITE
    }; 
    static uint8_t buttons = 0;
    uint8_t elite_mux_num = pad & 0b111;

    switch (elite_mux_num)
    {
        case SW2: //button 3
            buttons &= 0b00000111;
            buttons |= !(((PIND >> PD6)) & 1) << 3;
            GenericHID_Write_ButtonData(buttons);
            break;
        case SW3: // button 2
            buttons &= 0b00001011;
            buttons |= !(((PIND >> PD6)) & 1) << 2;
            GenericHID_Write_ButtonData(buttons);
            break;
        case SW4: // button 1
            buttons &= 0b00001101;
            buttons |= !(((PIND >> PD6)) & 1) << 1;
            GenericHID_Write_ButtonData(buttons);
            break;
        case ELITE:
            buttons &= 0b00001110;
            buttons |= !(((PIND >> PD6)) & 1);
            GenericHID_Write_ButtonData(0xFF);
            break;
        case ENC1B:
            Encoder_Read(0);
            break;
        case ENC2B:
            Encoder_Read(1);
            break;
    }
}

/*
   The encoder has to be read at a reasonable rate. The previous pin state is 
   shifted up to keep a history. Fast turns are detected by looking at 
   the upper byte in the state history and seeing if it has not had time to 
   return to 0b11 (unturned state) 4 reads in a row (!= 0xFF).
*/ 
static void Encoder_Read(uint8_t number)
{
    static uint16_t dial_state[2] = {0, 0};
    uint8_t pins = PIND;
    pins = ((pins >> PD6) & 1) << 1 | ((pins >> number) & 1);
    dial_state[number] = ((dial_state[number] << 2) | pins);
    switch (dial_state[number] & 0xFF)
    {
        case 0b11111101:
            if (((dial_state[number] >> 8) & 0xFF) != 0xFF)
                GenericHID_Adjust_Dial(number, -10);
                //GenericHID_Adjust_Dial_Debug(number, -10, dial_state[number]);
            else
                GenericHID_Adjust_Dial(number, -1);
                //GenericHID_Adjust_Dial_Debug(number, -1, dial_state[number]);
            break;
        case 0b11111110:
            if (((dial_state[number] >> 8) & 0xFF) != 0xFF)
                GenericHID_Adjust_Dial(number, 10);
                //GenericHID_Adjust_Dial_Debug(number, 10, dial_state[number]);
            else
                GenericHID_Adjust_Dial(number, 1);
                //GenericHID_Adjust_Dial_Debug(number, 1, dial_state[number]);
            break;
    }
}
