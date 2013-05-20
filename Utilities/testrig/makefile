first:
	avrdude -p atmega32u4 -P usb -c avrisp2 -B 10 -e
	avrdude -p atmega32u4 -P usb -c avrisp2 -B 10 -U efuse:w:0xCB:m -U hfuse:w:0xD8:m -U lfuse:w:0xFF:m
	avrdude -p atmega32u4 -P usb -c avrisp2 -B 1 -U flash:w:BootAndTest-1.01.hex:i
