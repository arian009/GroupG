if not "%2" == "" (
	copy "lcdlib\lcd.h" %1
	avr-gcc -mmcu=atmega644p -DF_CPU=12000000 -Wall -Os %1\*.c %1\*.h -o %1\%1.elf -L./lcdlib/ -llcd
	rm -f "%1\lcd.h"
) else (
	avr-gcc -mmcu=atmega644p -DF_CPU=12000000 -Wall -Os  %1\*.c %1\*.h -o %1\%1.elf
)
avr-objcopy -O ihex "%1\%1.elf" "%1\%1.hex"
rm -f "%1\%1.elf"
avrdude -c usbasp -p m644p -U flash:w:"%1\%1.hex"
rm -f "%1\%1.hex"
pause