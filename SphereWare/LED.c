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
	
	//turn the LED blue
	int LEDChannels[NumLEDs][3];
  LEDChannels[0][0] = 0;
  LEDChannels[0][1] = 0;
  LEDChannels[0][2] = 1023;
	WriteLEDArray(LEDChannels);
	
}

void LED_SendPacket() {

  if (LED_CommandMode == 0b01) {
    LED_RedCommand = 127;
    LED_GreenCommand = 110;
    LED_BlueCommand = 110;
  }

  LED_CommandPacket = LED_CommandMode & 0b11;
  LED_CommandPacket = (LED_CommandPacket << 10)  | (LED_BlueCommand & 1023);
  LED_CommandPacket = (LED_CommandPacket << 10)  | (LED_RedCommand & 1023);
  LED_CommandPacket = (LED_CommandPacket << 10)  | (LED_GreenCommand & 1023);

  for (int j = 0; j < 32; j++) {
    if ((LED_CommandPacket >> (31 - j)) & 1) {
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

void WriteLEDArray(int LEDChannels[NumLEDs][3]) 
{


  LED_CommandMode = 0b00; // Write to PWM control registers

  for (int i = 0; i < NumLEDs; i++) {
    LED_RedCommand = LEDChannels[i][0];
    LED_GreenCommand = LEDChannels[i][1];
    LED_BlueCommand = LEDChannels[i][2];
    LED_SendPacket();
  }
  
  LED_Latch();
  
  LED_CommandMode = 0b01; // Write to current control registers

  for (int z = 0; z < NumLEDs; z++) LED_SendPacket();   

  LED_Latch();

}


// Example loop to cycle all LEDs through primary colors
void LED_TestLoop(void) {

	// Create LED value storage array
	int LEDChannels[NumLEDs][3] = {0};

  for (int i = 0; i < NumLEDs; i++) {
    LEDChannels[i][0] = 1023;
    LEDChannels[i][1] = 0;
    LEDChannels[i][2] = 0;
  }
  
  WriteLEDArray(LEDChannels);
  
  _delay_ms(200);
  
  for (int i = 0; i < NumLEDs; i++) {
    LEDChannels[i][0] = 0;
    LEDChannels[i][1] = 1023;
    LEDChannels[i][2] = 0;
  }

  WriteLEDArray(LEDChannels);

  _delay_ms(200);

  for (int i = 0; i < NumLEDs; i++) {
    LEDChannels[i][0] = 0;
    LEDChannels[i][1] = 0;
    LEDChannels[i][2] = 1023;
  }

  WriteLEDArray(LEDChannels);
  
  _delay_ms(200);
  
}
