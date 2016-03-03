/* 
 * "flash.c"
 * d
 * 
 */
#include "avr/io.h"
#include "lcd.h"
#include <util/delay.h>

int main(void) {
	init_lcd();
	set_orientation(North);
	rectangle r = {get_width() * 0.2, get_width() * 0.8, get_height() * 0.2, get_height() * 0.8};
	do {
		fill_rectangle(r, YELLOW);
		_delay_ms(1);
	} while(1);
}