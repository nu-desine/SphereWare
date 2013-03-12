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

void ButtonsAndDials_Read(uint8_t pad, bool * being_played)
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
            being_played[SW2] = ((buttons >> 3) & 1);
            GenericHID_Write_ButtonData(buttons);
            if (buttons & 0b00001000)
                _delay_us(100);
            break;
        case SW3: // button 2
            buttons &= 0b00001011;
            buttons |= !(((PIND >> PD6)) & 1) << 2;
            being_played[SW3] = ((buttons >> 2) & 1);
            GenericHID_Write_ButtonData(buttons);
            if (buttons & 0b00000100)
                _delay_us(100);
            break;
        case SW4: // button 1
            buttons &= 0b00001101;
            buttons |= !(((PIND >> PD6)) & 1) << 1;
            being_played[SW4] = ((buttons >> 1) & 1);
            GenericHID_Write_ButtonData(buttons);
            if (buttons & 0b00000010)
                _delay_us(100);
            break;
        case ELITE:
            buttons &= 0b00001110;
            buttons |= !(((PIND >> PD6)) & 1);
            GenericHID_Write_ButtonData(0xFF);
            break;
        case ENC1B:
            Encoder_Read(0, &being_played[ENC1B]);
            break;
        case ENC2B:
            Encoder_Read(1, &being_played[ENC2B]);
            break;
    }
    if (!(buttons & 0b1)) //non elite, never report being played
        being_played[elite_mux_num] = false;
}

/*
   The encoder has to be read at a reasonable rate. The previous pin state is 
   shifted up to keep a history. 
*/ 
static void Encoder_Read(uint8_t number, bool * being_turned)
{
    static uint8_t dial_state[2] = {0, 0};
    uint8_t pins = PIND;
    pins = ((pins >> PD6) & 1) << 1 | ((pins >> number) & 1);
    dial_state[number] = ((dial_state[number] << 2) | pins);
    switch (dial_state[number])
    {
        case 0b11111101:
            GenericHID_Adjust_Dial(number, -1);
            *being_turned = true;
            break;
        case 0b11111110:
            GenericHID_Adjust_Dial(number, 1);
            *being_turned = true;
            break;
        default:
            *being_turned = false;
    }
}
