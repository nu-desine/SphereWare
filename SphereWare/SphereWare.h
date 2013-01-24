/*
             LUFA Library
     Copyright (C) Dean Camera, 2012.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2012  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Header file for GenericHID.c.
 */

#ifndef _GENERICHID_H_
#define _GENERICHID_H_

    /* Includes: */
        #include <avr/io.h>
        #include <avr/wdt.h>
        #include <avr/power.h>
        #include <avr/interrupt.h>
        #include <stdbool.h>
        #include <string.h>

        #include "Descriptors.h"
        #include "Config/AppConfig.h"

        #include "ADC.h"
        #include "BootJump.h"
        #include "DAC.h"
        #include "LED.h"
        #include "MUX.h"
        #include "MIDI.h"
        #include "HidInReports.h"
        
        #include <LUFA/Drivers/USB/USB.h>

typedef enum {
    SW3,
    SW2,
    ENC1B,
    SW4,
    ENC2B
} ButtonOrDial; 

    /* Function Prototypes: */
        void SetupHardware(void);
        void HID_Task(void);

        void EVENT_USB_Device_Connect(void);
        void EVENT_USB_Device_Disconnect(void);
        void EVENT_USB_Device_ConfigurationChanged(void);
        void EVENT_USB_Device_ControlRequest(void);
        void EVENT_USB_Device_StartOfFrame(void);

        void ProcessGenericHIDReport(uint8_t* DataArray);
        void CreateGenericHIDReport(uint8_t* DataArray);

#endif

