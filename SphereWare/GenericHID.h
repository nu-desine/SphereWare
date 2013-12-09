#ifndef __GENERICHID_H__ 
#define __GENERICHID_H__ 

#include "Descriptors.h"
#include "Config/AppConfig.h"
//#include <LUFA/Drivers/USB/USB.h>

void GenericHID_Task(void);
void GenericHID_Write_PadData(uint8_t pad_number, int16_t pad_value, uint8_t pad_velocity);
void GenericHID_Write_PressureOnly(uint8_t pad_number, int16_t pad_value);
void GenericHID_Write_ButtonData(uint8_t buttons);
void GenericHID_Write_DebugData(uint8_t pad_number, int16_t pad_value);
void GenericHID_ProcessReport(uint8_t* DataArray);
void GenericHID_Write_MidiData(uint8_t* DataArray);
void GenericHID_Adjust_Dial(uint8_t dial_number, int8_t amount);
void GenericHID_Adjust_Dial_Debug(uint8_t dial_number, int8_t amount, uint16_t state);
void GenericHID_Clear(void);

#endif// __GENERICHID_H__ 
