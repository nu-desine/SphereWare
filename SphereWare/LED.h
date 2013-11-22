#ifndef __BRITE_H__ 
#define __BRITE_H__ 

#include <avr/io.h>
#include <util/delay.h>

// Defines for direct port access
#define CLKPORT PORTC
#define ENAPORT PORTC
#define LATPORT PORTE
#define DATPORT PORTF
#define CLKREG DDRC 
#define ENAREG DDRC
#define LATREG DDRE
#define DATREG DDRF
#define CLKPIN 7 
#define ENAPIN 6 
#define LATPIN 6 
#define DATPIN 7 

// Variables for communication
unsigned long led_command_packet;
int LED_CommandMode;
int LED_BlueCommand;
int LED_RedCommand;
int LED_GreenCommand;

// Variables for LED configuration
uint8_t LED_Status, LED_Pressure_Status; // << this should be a bool
uint8_t LED_Clock_Status, LED_Clock_Running, LED_Fade_Step;
uint16_t LED_Tempo;

// Colour values (min, mid, max)
uint16_t LED_Colour_Values[3][3]; // [Colour number/type] [RGB]

// Define number of ShiftBrite modules
#define NUM_OF_LEDS 1

void LED_Init(void);
void LED_Set_Colour(uint16_t red, uint16_t green, uint16_t blue);
void LED_Set_Current(uint16_t red, uint16_t green, uint16_t blue);
void LED_Set_Status(uint8_t status);
void LED_Set_Pressure_Status(uint8_t);

void LED_Set_Colour_Values (uint8_t colour, uint8_t red, uint8_t green, uint8_t blue);

static void LED_SendPacket(void);
static void LED_Latch(void);
static void LED_WriteArray(int led_channels[NUM_OF_LEDS][3]);

// Example loop to cycle all LEDs through primary colors
void LED_TestLoop(void);

#endif// __BRITE_H__ 
