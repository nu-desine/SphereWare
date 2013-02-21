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

void ButtonsAndDials_Read(uint8_t pad)
{
    static uint8_t buttons = 0;
    static uint8_t dials   = 0;
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
                dials &= 0b00001100;
                enc1_encoded =  ((pins >> PD6) & 1) << 1 | (pins & 1);
                if (enc1_encoded != prev_enc1_encoded)
                {
                    enc1 = read_encoder_1(enc1_encoded);
                    if ((enc1 > 0) && (enc1 != prev_enc1))
                    {
                        dials |= enc1 & 0b11;
                        //HidInReports_Create_Dial_Report(0, enc1);
                    }
                    prev_enc1 = enc1;
                    prev_enc1_encoded = enc1_encoded;
                }
            }
            break;
        case ENC2B:
            {
                uint8_t pins = PIND;
                dials &= 0b00000011;
                enc2_encoded =  ((pins >> PD6) & 1) << 1 | ((pins >> PD1) & 1);
                if (enc2_encoded != prev_enc2_encoded)
                {
                    enc2 = read_encoder_2(enc2_encoded);
                    if ((enc2 > 0) && (enc2 != prev_enc2))
                    {
                        dials |= (enc2 & 0b11) << 2;
                        //HidInReports_Create_Dial_Report(1, enc2);
                    }
                    prev_enc2 = enc2;
                    prev_enc2_encoded = enc2_encoded;
                }
            }
            break;

    }
    GenericHID_Write_ButtonDialData( buttons, dials);
}
