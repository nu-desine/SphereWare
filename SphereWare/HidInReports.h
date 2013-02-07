#ifndef __HIDINREPORTS_H__ 
#define __HIDINREPORTS_H__ 

#include "Descriptors.h"
#include "Config/AppConfig.h"
#include <LUFA/Drivers/USB/USB.h>

//==============================================================================
/** Sends a HID report to the host.
 
 @param report_data        the array of data that needs to be sent as a report
 */
void HidInReports_Send_Report (uint8_t* report_data);

//==============================================================================
/** Creates and sends a HID report containing pad data.
 
 @param pad_number        the pad number (0 - 47)
 @param pad_value        the pads pressure value (0 - ?)
 @param pad_velocity        the pads velocity value (0 - 127)
 */
void HidInReports_Create_Pad_Report (uint8_t pad_number, 
                                     int pad_value, 
                                     uint8_t pad_velocity);

//==============================================================================
/** Creates and sends a HID report containing elite button data.
 
 @param button_number        the button number (0 - 2)
 @param button_value        the button value (0 - 1)
 */
void HidInReports_Create_Button_Report (uint8_t button_number, 
                                        uint8_t button_value);

//==============================================================================
/** Creates and sends a HID report containing elite dial data.
 
 Elite dials should act as relative controllers which increment or decrement 
 the previous value regardless of their exact position. 
 
 They should send either a value of 1 - 63 if turned clockwise (incremented) with 
 a larger value being sent the quicker the encoder is turned, or 127-64 if turned 
 anti-clockwise (decremented) with a smaller value being sent the quicker the encoder 
 is turned.
 
 @param dial_number        the dial number (0 - 1)
 @param dial_value        the dials relative value (1 - 127)
 */
void HidInReports_Create_Dial_Report (uint8_t dial_number, 
                                     uint8_t dial_value);

//==============================================================================
/** Creates and sends a HID report containing AlphaLive setup data.
 
 When AlphaLive is opened, or when the AlphaSphere is connected whilst AlphaLive
 is running, it will send the device a report asking it to send back this report.
 
 The report contains two pieces of data - the current firmware version number so 
 that AlphaLive can determine if a firmware update is needed, and a flag to indicate
 what type of AlphaSphere is connected ('regular' or 'elite', or eventually 'me')
 so that AlphaLive can display the correct GUI the provide the correct functionality. 
  
 @param firmware_version        the current firmware version
 @param device_type             a flag that indicates whether this a regular AlphaSphere (0),
                                an AlphaSphere elite (1), or an AlphaSphere me (2)
 */
void HidInReports_Create_Host_Setup_Report (uint8_t firmware_version, 
                                            uint8_t device_type);

void HidInReports_Init (void);
#endif// __HIDINREPORTS_H__ 
