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

/** \file
 *
 *  USB Device Descriptors, for library use when in USB device mode. Descriptors are special
 *  computer-readable structures which the host requests upon device enumeration, to determine
 *  the device's capabilities and functions.
 */

#include "Descriptors.h"

/** HID class report descriptor. This is a special descriptor constructed with values from the
 *  USBIF HID class specification to describe the reports and capabilities of the HID device. This
 *  descriptor is parsed by the host and its contents used to determine what data (and in what encoding)
 *  the device will send, and what it may be sent back from the host. Refer to the HID specification for
 *  more details on HID report descriptors.
 */
const USB_Descriptor_HIDReport_Datatype_t PROGMEM GenericReport[] =
{

    HID_RI_USAGE_PAGE(16, 0xFF00), /* Vendor Page 0 */
    HID_RI_USAGE(8, 0x01), /* Vendor Usage 1 */
    HID_RI_COLLECTION(8, 0x01), /* Vendor Usage 1 */
        HID_RI_USAGE(8, 0x02), /* Vendor Usage 2 */
        HID_RI_LOGICAL_MINIMUM(8, 0x00),
        HID_RI_LOGICAL_MAXIMUM(8, 0xFF),
        HID_RI_REPORT_SIZE(8, 0x08),
        HID_RI_REPORT_COUNT(8, GENERIC_REPORT_SIZE),
        HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
        HID_RI_USAGE(8, 0x03), /* Vendor Usage 3 */
        HID_RI_LOGICAL_MINIMUM(8, 0x00),
        HID_RI_LOGICAL_MAXIMUM(8, 0xFF),
        HID_RI_REPORT_SIZE(8, 0x08),
        HID_RI_REPORT_COUNT(8, GENERIC_REPORT_SIZE),
        HID_RI_OUTPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE | HID_IOF_NON_VOLATILE),
    HID_RI_END_COLLECTION(0),
};

/** Device descriptor structure. This descriptor, located in FLASH memory, describes the overall
 *  device characteristics, including the supported USB version, control endpoint size and the
 *  number of device configurations. The descriptor is read out by the USB host when the enumeration
 *  process begins.
 */
const USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
    .Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

    .USBSpecification       = VERSION_BCD(02.00),

    .Class                  = USB_CSCP_IADDeviceClass,
    .SubClass               = USB_CSCP_IADDeviceSubclass,
    .Protocol               = USB_CSCP_IADDeviceProtocol,

    .Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,

    .VendorID               = 0x1d50,
    .ProductID              = 0x6021,
    .ReleaseNumber          = VERSION_BCD(00.01),

    .ManufacturerStrIndex   = 0x01,
    .ProductStrIndex        = 0x02,
    .SerialNumStrIndex      = NO_DESCRIPTOR,

    .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

/** Configuration descriptor structure. This descriptor, located in FLASH memory, describes the usage
 *  of the device in one of its supported configurations, including information about any device interfaces
 *  and endpoints. The descriptor is read out by the USB host during the enumeration process when selecting
 *  a configuration so that the host may correctly communicate with the USB device.
 */
const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
{
    .Config =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

            .TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
            .TotalInterfaces        = 3,

            .ConfigurationNumber    = 1,
            .ConfigurationStrIndex  = NO_DESCRIPTOR,

            .ConfigAttributes       = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),

            .MaxPowerConsumption    = USB_CONFIG_POWER_MA(500)
        },

    .Audio_IAD =
    {
        .Header = {.Size = sizeof(USB_Descriptor_Interface_Association_t), .Type = DTYPE_InterfaceAssociation},

        .FirstInterfaceIndex = 0,
        .TotalInterfaces = 2,

        .Class = AUDIO_CSCP_AudioClass,
        .SubClass = AUDIO_CSCP_ControlSubclass,
        .Protocol = AUDIO_CSCP_ControlProtocol,

        .IADStrIndex = NO_DESCRIPTOR
    },
    .Audio_ControlInterface =
    {
        .Header                   = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

        .InterfaceNumber          = 0,
        .AlternateSetting         = 0,

        .TotalEndpoints           = 0,

        .Class                    = AUDIO_CSCP_AudioClass,
        .SubClass                 = AUDIO_CSCP_ControlSubclass,
        .Protocol                 = AUDIO_CSCP_ControlProtocol,

        .InterfaceStrIndex        = NO_DESCRIPTOR
    },

    .Audio_ControlInterface_SPC =
    {
        .Header                   = {.Size = sizeof(USB_Audio_Descriptor_Interface_AC_t), .Type = DTYPE_CSInterface},
        .Subtype                  = AUDIO_DSUBTYPE_CSInterface_Header,

        .ACSpecification          = VERSION_BCD(01.00),
        .TotalLength              = sizeof(USB_Audio_Descriptor_Interface_AC_t),

        .InCollection             = 1,
        .InterfaceNumber          = 1,
    },

    .Audio_StreamInterface =
    {
        .Header                   = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

        .InterfaceNumber          = 1,
        .AlternateSetting         = 0,

        .TotalEndpoints           = 2,

        .Class                    = AUDIO_CSCP_AudioClass,
        .SubClass                 = AUDIO_CSCP_MIDIStreamingSubclass,
        .Protocol                 = AUDIO_CSCP_StreamingProtocol,

        .InterfaceStrIndex        = NO_DESCRIPTOR
    },

    .Audio_StreamInterface_SPC =
    {
        .Header                   = {.Size = sizeof(USB_MIDI_Descriptor_AudioInterface_AS_t), .Type = DTYPE_CSInterface},
        .Subtype                  = AUDIO_DSUBTYPE_CSInterface_General,

        .AudioSpecification       = VERSION_BCD(01.00),

        .TotalLength              =  offsetof(USB_Descriptor_Configuration_t, HID_Interface)
        - offsetof(USB_Descriptor_Configuration_t, Audio_StreamInterface_SPC)  

    },

    .MIDI_In_Jack_Emb =
    {
        .Header                   = {.Size = sizeof(USB_MIDI_Descriptor_InputJack_t), .Type = DTYPE_CSInterface},
        .Subtype                  = AUDIO_DSUBTYPE_CSInterface_InputTerminal,

        .JackType                 = MIDI_JACKTYPE_Embedded,
        .JackID                   = 0x01,

        .JackStrIndex             = NO_DESCRIPTOR
    },

    .MIDI_In_Jack_Ext =
    {
        .Header                   = {.Size = sizeof(USB_MIDI_Descriptor_InputJack_t), .Type = DTYPE_CSInterface},
        .Subtype                  = AUDIO_DSUBTYPE_CSInterface_InputTerminal,

        .JackType                 = MIDI_JACKTYPE_External,
        .JackID                   = 0x02,

        .JackStrIndex             = NO_DESCRIPTOR
    },

    .MIDI_Out_Jack_Emb =
    {
        .Header                   = {.Size = sizeof(USB_MIDI_Descriptor_OutputJack_t), .Type = DTYPE_CSInterface},
        .Subtype                  = AUDIO_DSUBTYPE_CSInterface_OutputTerminal,

        .JackType                 = MIDI_JACKTYPE_Embedded,
        .JackID                   = 0x03,

        .NumberOfPins             = 1,
        .SourceJackID             = {0x02},
        .SourcePinID              = {0x01},

        .JackStrIndex             = NO_DESCRIPTOR
    },

    .MIDI_Out_Jack_Ext =
    {
        .Header                   = {.Size = sizeof(USB_MIDI_Descriptor_OutputJack_t), .Type = DTYPE_CSInterface},
        .Subtype                  = AUDIO_DSUBTYPE_CSInterface_OutputTerminal,

        .JackType                 = MIDI_JACKTYPE_External,
        .JackID                   = 0x04,

        .NumberOfPins             = 1,
        .SourceJackID             = {0x01},
        .SourcePinID              = {0x01},

        .JackStrIndex             = NO_DESCRIPTOR
    },

    .MIDI_In_Jack_Endpoint =
    {
        .Endpoint =
        {
            .Header              = {.Size = sizeof(USB_Audio_Descriptor_StreamEndpoint_Std_t), .Type = DTYPE_Endpoint},

            .EndpointAddress     = (ENDPOINT_DIR_OUT | MIDI_STREAM_OUT_EPADDR),
            .Attributes          = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
            .EndpointSize        = MIDI_STREAM_EPSIZE,
            .PollingIntervalMS   = 0x01
        },

        .Refresh                  = 0,
        .SyncEndpointNumber       = 0
    },

    .MIDI_In_Jack_Endpoint_SPC =
    {
        .Header                   = {.Size = sizeof(USB_MIDI_Descriptor_Jack_Endpoint_t), .Type = DTYPE_CSEndpoint},
        .Subtype                  = AUDIO_DSUBTYPE_CSEndpoint_General,

        .TotalEmbeddedJacks       = 0x01,
        .AssociatedJackID         = {0x01}
    },

    .MIDI_Out_Jack_Endpoint =
    {
        .Endpoint =
        {
            .Header              = {.Size = sizeof(USB_Audio_Descriptor_StreamEndpoint_Std_t), .Type = DTYPE_Endpoint},

            .EndpointAddress     = (ENDPOINT_DIR_IN | MIDI_STREAM_IN_EPADDR),
            .Attributes          = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
            .EndpointSize        = MIDI_STREAM_EPSIZE,
            .PollingIntervalMS   = 0x01
        },

        .Refresh                  = 0,
        .SyncEndpointNumber       = 0
    },

    .MIDI_Out_Jack_Endpoint_SPC =
    {
        .Header                   = {.Size = sizeof(USB_MIDI_Descriptor_Jack_Endpoint_t), .Type = DTYPE_CSEndpoint},
        .Subtype                  = AUDIO_DSUBTYPE_CSEndpoint_General,

        .TotalEmbeddedJacks       = 0x01,
        .AssociatedJackID         = {0x03}
    },






    .HID_Interface =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

            .InterfaceNumber        = 0x02,
            .AlternateSetting       = 0x00,

            .TotalEndpoints         = 2,

            .Class                  = HID_CSCP_HIDClass,
            .SubClass               = HID_CSCP_NonBootSubclass,
            .Protocol               = HID_CSCP_NonBootProtocol,

            .InterfaceStrIndex      = NO_DESCRIPTOR
        },

    .HID_GenericHID =
        {
            .Header                 = {.Size = sizeof(USB_HID_Descriptor_HID_t), .Type = HID_DTYPE_HID},

            .HIDSpec                = VERSION_BCD(01.11),
            .CountryCode            = 0x00,
            .TotalReportDescriptors = 1,
            .HIDReportType          = HID_DTYPE_Report,
            .HIDReportLength        = sizeof(GenericReport)
        },

    .HID_ReportINEndpoint =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

            .EndpointAddress        = GENERIC_IN_EPADDR,
            .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
            .EndpointSize           = GENERIC_EPSIZE,
            .PollingIntervalMS      = 0x01
        },

    .HID_ReportOUTEndpoint =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

            .EndpointAddress        = GENERIC_OUT_EPADDR,
            .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
            .EndpointSize           = GENERIC_EPSIZE,
            .PollingIntervalMS      = 0x01
        }
};

/** Language descriptor structure. This descriptor, located in FLASH memory, is returned when the host requests
 *  the string descriptor with index 0 (the first index). It is actually an array of 16-bit integers, which indicate
 *  via the language ID table available at USB.org what languages the device supports for its string descriptors.
 */
const USB_Descriptor_String_t PROGMEM LanguageString =
{
    .Header                 = {.Size = USB_STRING_LEN(1), .Type = DTYPE_String},

    .UnicodeString          = {LANGUAGE_ID_ENG}
};

/** Manufacturer descriptor string. This is a Unicode string containing the manufacturer's details in human readable
 *  form, and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const USB_Descriptor_String_t PROGMEM ManufacturerString =
{
    .Header                 = {.Size = USB_STRING_LEN(9), .Type = DTYPE_String},

    .UnicodeString          = L"nu desine"
};

/** Product descriptor string. This is a Unicode string containing the product's details in human readable form,
 *  and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const USB_Descriptor_String_t PROGMEM ProductString =
{
    .Header                 = {.Size = USB_STRING_LEN(11), .Type = DTYPE_String},

    .UnicodeString          = L"AlphaSphere"
};

/** This function is called by the library when in device mode, and must be overridden (see library "USB Descriptors"
 *  documentation) by the application code so that the address and size of a requested descriptor can be given
 *  to the USB library. When the device receives a Get Descriptor request on the control endpoint, this function
 *  is called so that the descriptor details can be passed back and the appropriate descriptor sent back to the
 *  USB host.
 */
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint8_t wIndex,
                                    const void** const DescriptorAddress)
{
    const uint8_t  DescriptorType   = (wValue >> 8);
    const uint8_t  DescriptorNumber = (wValue & 0xFF);

    const void* Address = NULL;
    uint16_t    Size    = NO_DESCRIPTOR;

    switch (DescriptorType)
    {
        case DTYPE_Device:
            Address = &DeviceDescriptor;
            Size    = sizeof(USB_Descriptor_Device_t);
            break;
        case DTYPE_Configuration:
            Address = &ConfigurationDescriptor;
            Size    = sizeof(USB_Descriptor_Configuration_t);
            break;
        case DTYPE_String:
            switch (DescriptorNumber)
            {
                case 0x00:
                    Address = &LanguageString;
                    Size    = pgm_read_byte(&LanguageString.Header.Size);
                    break;
                case 0x01:
                    Address = &ManufacturerString;
                    Size    = pgm_read_byte(&ManufacturerString.Header.Size);
                    break;
                case 0x02:
                    Address = &ProductString;
                    Size    = pgm_read_byte(&ProductString.Header.Size);
                    break;
            }

            break;
        case HID_DTYPE_HID:
            Address = &ConfigurationDescriptor.HID_GenericHID;
            Size    = sizeof(USB_HID_Descriptor_HID_t);
            break;
        case HID_DTYPE_Report:
            Address = &GenericReport;
            Size    = sizeof(GenericReport);
            break;
    }

    *DescriptorAddress = Address;
    return Size;
}

