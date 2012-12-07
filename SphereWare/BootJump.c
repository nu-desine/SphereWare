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
#include "BootJump.h"
uint32_t Boot_Key ATTR_NO_INIT;
//void BootJump_Check(void)
//{
//	// If the reset source was the bootloader and the key is correct, clear it and jump to the bootloader
//	if ((MCUSR & (1 << WDRF)) && (Boot_Key == MAGIC_BOOT_KEY))
//	{
//		Boot_Key = 0;
//		((void (*)(void))BOOTLOADER_START_ADDRESS)();
//	}
//}

void BootJump_Jump_To_Bootloader(void)
{
	// If USB is used, detach from the bus and reset it
	USB_Disable();
	// Disable all interrupts
	cli();
	// Wait 500ms for the USB detachment to register on the host
	Delay_MS(500);
	// Set the bootloader key to the magic value and force a reset
	Boot_Key = MAGIC_BOOT_KEY;
	wdt_enable(WDTO_250MS);
	for (;;);
}
