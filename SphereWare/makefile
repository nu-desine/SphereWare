#
#             LUFA Library
#     Copyright (C) Dean Camera, 2012.
#
#  dean [at] fourwalledcubicle [dot] com
#           www.lufa-lib.org
#
# --------------------------------------
#         LUFA Project Makefile.
# --------------------------------------

MCU          = atmega32u4
ARCH         = AVR8
BOARD        = NONE
F_CPU        = 16000000
F_USB        = $(F_CPU)
OPTIMIZATION = s
TARGET       = SphereWare
SRC          = $(TARGET).c ButtonsAndDials.c Descriptors.c ADC.c BootJump.c DAC.c LED.c MUX.c MIDI.c HidInReports.c $(LUFA_SRC_USB)
LUFA_PATH    = ../LUFA
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -IConfig/
LD_FLAGS     =

# Default target
all:

first:
	avrdude -p atmega32u4 -P usb -c avrisp2 -B 10 -e
	avrdude -p atmega32u4 -P usb -c avrisp2 -B 10 -U efuse:w:0xCB:m -U hfuse:w:0xD8:m -U lfuse:w:0xFF:m
	avrdude -p atmega32u4 -P usb -c avrisp2 -B 1 -U flash:w:SphereWare-test-5.1.hex:i


# Include LUFA build script makefiles
include $(LUFA_PATH)/Build/lufa_core.mk
include $(LUFA_PATH)/Build/lufa_sources.mk
include $(LUFA_PATH)/Build/lufa_build.mk
include $(LUFA_PATH)/Build/lufa_cppcheck.mk
include $(LUFA_PATH)/Build/lufa_doxygen.mk
include $(LUFA_PATH)/Build/lufa_dfu.mk
include $(LUFA_PATH)/Build/lufa_hid.mk
include $(LUFA_PATH)/Build/lufa_avrdude.mk
include $(LUFA_PATH)/Build/lufa_atprogram.mk
