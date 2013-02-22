#ifndef __GENERICHID_H__ 
#define __GENERICHID_H__ 

#include "Descriptors.h"
#include "Config/AppConfig.h"
#include "MIDI.h"
//#include <LUFA/Drivers/USB/USB.h>

void GenericHID_Task(void);
void GenericHID_Write_PadData(uint8_t pad_number, int16_t pad_value, uint8_t pad_velocity);
void GenericHID_Write_ButtonDialData(uint8_t buttons, uint8_t dials);
void GenericHID_ProcessReport(uint8_t* DataArray);

#endif// __GENERICHID_H__ 
