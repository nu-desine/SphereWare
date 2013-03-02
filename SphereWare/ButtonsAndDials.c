#include "ButtonsAndDials.h"
#include <avr/io.h>
#include "HidInReports.h"

#define ELITE_MASK 0b00100011

typedef enum {
    SW3,
    SW2,
    ENC1B,
    SW4,
    ENC2B
} ButtonOrDial; 
bool button_1 = false;
bool button_2 = false;
bool button_3 = false;
uint8_t enc1_encoded = 0;
uint8_t enc2_encoded = 0;
uint8_t enc1 = 0;
uint8_t enc2 = 0;

bool prev_button_1 = false;
bool prev_button_2 = false;
bool prev_button_3 = false;
uint8_t prev_enc1_encoded = 0;
uint8_t prev_enc2_encoded = 0;
uint8_t prev_enc1 = 0;
uint8_t prev_enc2 = 0;

void ButtonsAndDials_Init(void)
{
    PORTD = (1 << PD6) | (1 << PD0) | (1 << PD1);
}



uint8_t read_encoder_1(uint8_t decoder_val)
{
  static uint8_t old_AB = 0;
  /**/
  old_AB <<= 2;                   //remember previous state
  old_AB |= decoder_val & 0b11; //add current state
  if ((old_AB & 0b111111) == 0b001011)
      return 0b01;
  else if ((old_AB & 0b111111) == 0b000111)
      return 0b10;
  else
      return 0;
}

uint8_t read_encoder_2(uint8_t decoder_val)
{
  static uint8_t old_AB = 0;
  /**/
  old_AB <<= 2;                   //remember previous state
  old_AB |= decoder_val & 0b11; //add current state
  if ((old_AB & 0b111111) == 0b001011)
      return 0b01;
  else if ((old_AB & 0b111111) == 0b000111)
      return 0b10;
  else
      return 0;
}

uint8_t ButtonsAndDials_Read(uint8_t pad)
{
    static uint8_t buttons = 0;
    static uint8_t dials   = 0;
    static uint32_t dial_state[2] = {0, 0};
    static int8_t dir = 0;
    //the MUX for the elite buttons and rotary encoders is on the same 
    //MUX_A,B,C lines as the pads MUXs but it's always selected (INH tied to GND)
    uint8_t elite_mux_num = pad & 0b111;

    switch (elite_mux_num)
    {
        case SW2: //button 3
            buttons &= 0b00000011;
            buttons |= !(((PIND >> PD6)) & 1) << 2;
            break;
        case SW3: // button 2
            buttons &= 0b00000101;
            buttons |= !(((PIND >> PD6)) & 1) << 1;
            break;
        case SW4: // button 1
            buttons &= 0b00000110;
            buttons |= !(((PIND >> PD6)) & 1);
            break;
        case ENC1B:
            {
                uint8_t pins = PIND;
                pins = ((pins >> PD6) & 1) << 1 | (pins & 1);
                //if ((dial_state[0] & 0b11) != pins)
                {
                    dial_state[0] = ((dial_state[0] << 2) | pins);
                    switch (dial_state[0] & 0xFF)
                    {
                        case 0b11111101:
                            GenericHID_Adjust_Dial(0, 1, dial_state[0]);
                            dir = 1;
                            break;
                        case 0b01111101:
                        case 0b00111101:
                        case 0b10111101:
                            if (dir != -1)
                            {
                                GenericHID_Adjust_Dial(0, 3, dial_state[0]);
                                dir = 1;
                            }
                            else
                                GenericHID_Adjust_Dial(0, -3, dial_state[0]);
                            break;

                        case 0b01101101:
                        case 0b00101101:
                        case 0b10101101:
                        case 0b01011101:
                        case 0b00011101:
                        case 0b10011101:
                        case 0b01001101:
                        case 0b00001101:
                        case 0b10001101:
                            if (dir != -1)
                            {
                                GenericHID_Adjust_Dial(0, 10, dial_state[0]);
                                if (((dial_state[0] >> 8) & 0xFF) == 0xFF)
                                    dir = 1;
                            }
                            else
                            {
                                GenericHID_Adjust_Dial(0, -10, dial_state[0]);
                            }
                            break;

                        case 0b11111110:
                            GenericHID_Adjust_Dial(0, -1, dial_state[0]);
                            dir = -1;
                            break;

                        case 0b01111110:
                        case 0b00111110:
                        case 0b10111110:
                            if (dir != 1)
                            {
                                GenericHID_Adjust_Dial(0, -3, dial_state[0]);
                                dir = -1;
                            }
                            else
                            {
                                GenericHID_Adjust_Dial(0, 3, dial_state[0]);
                            }
                            break;

                        case 0b01001110:
                        case 0b00001110:
                        case 0b10001110:
                        case 0b01011110:
                        case 0b00011110:
                        case 0b10011110:
                        case 0b01101110:
                        case 0b00101110:
                        case 0b10101110:
                            if (dir != 1)
                            {
                                GenericHID_Adjust_Dial(0, -10, dial_state[0]);
                                if (((dial_state[0] >> 8) & 0xFF) == 0xFF)
                                    dir = -1;
                            }
                            else
                                GenericHID_Adjust_Dial(0, 10, dial_state[0]);
                            break;
                        case 0b11111111:
                            dir = 0;
                            break;
                    }
                    //switch (dial_state[0])
                    //{
                    //    case 0b0100101101001011:
                    //        GenericHID_Increment_Dial(0, dial_state[0]);
                    //        GenericHID_Increment_Dial(0, dial_state[0]);
                    //        GenericHID_Increment_Dial(0, dial_state[0]);
                    //        GenericHID_Increment_Dial(0, dial_state[0]);
                    //        break;
                    //    case 0b1000011110000111:
                    //        GenericHID_Decrement_Dial(0, dial_state[0]);
                    //        GenericHID_Decrement_Dial(0, dial_state[0]);
                    //        GenericHID_Decrement_Dial(0, dial_state[0]);
                    //        GenericHID_Decrement_Dial(0, dial_state[0]);
                    //        break;
                    //}
                }
            }
            break;
        case ENC2B:
            //{
            //    uint8_t pins = PIND;
            //    dials &= 0b00000011;
            //    dials |= (((pins >> PD6) & 1) << 1 | ((pins >> PD1) & 1)) << 2;
            //}
            //{
            //    uint8_t pins = PIND;
            //    dials &= 0b00000011;
            //    enc2_encoded =  ((pins >> PD6) & 1) << 1 | ((pins >> PD1) & 1);
            //    if (enc2_encoded != prev_enc2_encoded)
            //    {
            //        enc2 = read_encoder_2(enc2_encoded);
            //        if ((enc2 > 0) && (enc2 != prev_enc2))
            //        {
            //            dials |= (enc2 & 0b11) << 2;
            //            //HidInReports_Create_Dial_Report(1, enc2);
            //        }
            //        prev_enc2 = enc2;
            //        prev_enc2_encoded = enc2_encoded;
            //    }
            //}
            break;

    }
    return dials;//buttons | (dials << 3);
}
