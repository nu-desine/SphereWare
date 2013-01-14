#ifndef __ADC_H__
#define __ADC_H__

#include <avr/io.h>
#include <util/delay.h>

typedef enum {
	SINGLE_ENDED	= 0b000100,
	DIFF_1_X1    	= 0b010100,
	DIFF_0_X10   	= 0b101000,
	DIFF_1_X10   	= 0b101100,
	DIFF_0_X40   	= 0b110000,
	DIFF_1_X40   	= 0b110100,
	DIFF_0_X200  	= 0b111000,
	DIFF_1_X200  	= 0b111100,
	PREVIOUS_MODE = 0b000001
} ADC_Mode;

typedef enum {
	ADC4     = 0b00,
	ADC5     = 0b01,
	ADC_PREV = 0b11
} ADC_Channel;

typedef enum {
    REF_2V56,
    REF_VCC,
    REF_EXTERNAL
} Ref_Mode;

void ADC_Init(void);
void ADC_SetRef(Ref_Mode mode);
void ADC_Set(ADC_Mode mode, ADC_Channel chan);
int16_t ADC_Read(ADC_Mode mode, ADC_Channel chan);

#endif// __ADC_H__
