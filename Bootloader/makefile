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
BOARD        = USBKEY
F_CPU        = 16000000
F_USB        = $(F_CPU)
OPTIMIZATION = s
TARGET       = BootloaderHID
SRC          = $(TARGET).c Descriptors.c ../SphereWare/LED.c $(LUFA_SRC_USB)
LUFA_PATH    = ../LUFA
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -IConfig/ -I../SphereWare
LD_FLAGS     = -Wl,--section-start=.text=$(BOOT_START_OFFSET)

# Flash size and bootloader section sizes of the target, in KB. These must
# match the target's total FLASH size and the bootloader size set in the
# device's fuses.
FLASH_SIZE_KB        := 32
BOOT_SECTION_SIZE_KB := 4

# Bootloader address calculation formulas (requires the "bc" unix utility)
# Do not modify these macros, but rather modify the depedant values above.
BOOT_START_OFFSET     = 0x$(shell echo "obase=16; ($(FLASH_SIZE_KB) - $(BOOT_SECTION_SIZE_KB)) * 1024" | bc)
BOOT_SEC_OFFSET       = 0x$(shell echo "obase=16; (($(FLASH_SIZE_KB) * 1024) - $(strip $(1)))" | bc)

# Default target
all:

boot:
	avrdude -p atmega32u4 -P usb -c avrisp2 -B 1 -U flash:w:BootloaderHID-1.0.hex:i -e



# Include LUFA build script makefiles
include $(LUFA_PATH)/Build/lufa_core.mk
include $(LUFA_PATH)/Build/lufa_sources.mk
include $(LUFA_PATH)/Build/lufa_build.mk
include $(LUFA_PATH)/Build/lufa_cppcheck.mk
include $(LUFA_PATH)/Build/lufa_doxygen.mk
include $(LUFA_PATH)/Build/lufa_avrdude.mk
include $(LUFA_PATH)/Build/lufa_atprogram.mk
