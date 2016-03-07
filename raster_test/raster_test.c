/* 
 * "flash.c"
 * d
 * 
 */
#include "avr/io.h"
#include "lcd.h"
#include <util/delay.h>
#include <stdlib.h>

int main(void) {
	init_lcd();
	set_orientation(North);
	rectangle r = {0, 19, 0, 19};
	uint16_t* data = malloc(sizeof(uint16_t) * 20 * 20);
	uint16_t i;
	for(i = 0; i < 2*20*20; i++) {
		*(data + i) = RED;
	}
	do {
		fill_rectangle_indexed(r, data);
		_delay_ms(1);
	} while(1);
}