#ifndef __HIDINREPORTS_H__ 
#define __HIDINREPORTS_H__ 

#include "Descriptors.h"
#include "Config/AppConfig.h"
#include <LUFA/Drivers/USB/USB.h>

void HidInReports_Send_Report (uint8_t* report_data);

void HidInReports_Create_Pad_Report (uint8_t pad_number, 
                                     int pad_value, 
                                     uint8_t pad_velocity);

void HidInReports_Create_Button_Report (uint8_t button_number, 
                                        uint8_t button_value);

void HidInReports_Create_Dial_Report (uint8_t dial_number, 
                                     uint8_t dial_value);

#endif// __HIDINREPORTS_H__ 
