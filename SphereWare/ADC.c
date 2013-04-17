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

#include "ADC.h"

uint8_t prev_chan;
uint8_t prev_mode;

/**  Initialise the ADC
  */
void ADC_Init(void)
{
    //Enable the ADC and set the ADC clock prescale to 128, 16Mhz/128 = 125kHz
    ADCSRA |= (0 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN);
    ADCSRB |= (1 << ADHSM);

    ADC_SetRef(REF_VCC);

    prev_chan = 1;
    prev_mode = 1;
    ADC_Set(0, 0);
}


/** Set the ADC reference voltage
  */
void ADC_SetRef(Ref_Mode mode)
{
    ADMUX &= ~(1 << REFS1) & ~(1 << REFS0);
    switch(mode)
    {
        case REF_2V56:
            ADMUX |= (1 << REFS1) | (1 << REFS0);
            break;
        case REF_VCC:
            ADMUX |= (0 << REFS1) | (1 << REFS0);
            break;
        case REF_EXTERNAL:
            break;
    }
}



/** Set the correct mode and channel on the ADC MUX[5..0]
    */ 
void ADC_Set(ADC_Mode mode, ADC_Channel chan)
{
  if (mode != prev_mode) 
  {
      //set the correct MUX[5..0] for this mode and channel
      ADMUX  &= 0b11100000;
      ADCSRB &= 0b11011111;
      ADMUX  |= (mode | chan) & 0b011111;
      ADCSRB |= mode & 0b100000;
      prev_chan = chan;
      prev_mode = mode;
      _delay_us(500);
  }
  else if (chan != prev_chan) //just set the channel
  {
      ADMUX  &= 0b11111100;
      ADMUX  |= chan & 0b11;
      prev_chan = chan;
      _delay_us(500);
  }

    //or don't set anything
}

/** Read the ADC optionally setting mode and ADC channel
    */
int16_t ADC_Read(ADC_Mode mode, ADC_Channel chan) 
{
    uint8_t low, high;

    ADC_Set(mode, chan);

    //start the conversion
    ADCSRA |= (1 << ADSC);

    //wait for conversion to finish
    while (bit_is_set(ADCSRA, ADSC));

    //read in this order as both registers are locked when reading ADCL and unlocked
    //when finished reading ADCH
    low     =   ADCL;
    high    =   ADCH;


    if ((mode & 0b110000) && (high >> 1)) //differential mode, negative value
        return -(511 - (low | ((high & 1) << 8)));
    else
        return low | (high << 8);
}
