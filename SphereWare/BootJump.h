#ifndef __BOOTJUMP_H__
#define __BOOTJUMP_H__

#include <avr/wdt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <LUFA/Common/Common.h>
#include <LUFA/Drivers/USB/USB.h>

#define FLASH_SIZE_BYTES 32 * 1024
#define BOOTLOADER_SEC_SIZE_BYTES 4 * 1024
#define BOOTLOADER_START_ADDRESS (FLASH_SIZE_BYTES - BOOTLOADER_SEC_SIZE_BYTES)
#define MAGIC_BOOT_KEY 0xDC42

//void BootJump_Check(void) ATTR_INIT_SECTION(3);
void BootJump_Jump_To_Bootloader(void);

#endif// __BOOTJUMP_H__
