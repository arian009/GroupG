/*  Author: Steve Gunn
 * Licence: This work is licensed under the Creative Commons Attribution License.
 *           View this license at http://creativecommons.org/about/licenses/
 */
 
#include "ili934x.h"
#include "font.h"
#include "lcd.h"
#include <stdlib.h>

lcd display = {LCDWIDTH, LCDHEIGHT, North, 0, 0, WHITE, BLACK};
//uint16_t* raster;

void init_lcd()
{
	/* Disable JTAG in software, so that it does not interfere with Port C  */
	/* It will be re-enabled after a power cycle if the JTAGEN fuse is set. */
	MCUCR |= (1<<JTD);
	MCUCR |= (1<<JTD);
	
	/* Configure ports */
	CTRL_DDR = 0x7F;
	DATA_DDR = 0xFF;
	
	init_display_controller();
	
	//raster = malloc(sizeof(uint16_t) * LCDWIDTH * LCDHEIGHT);
}

void set_orientation(orientation o)
{
	display.orient = o;
	write_cmd(MEMORY_ACCESS_CONTROL);
	if (o==North) { 
		display.width = LCDWIDTH;
		display.height = LCDHEIGHT;
		write_data(0x48);
	}
	else if (o==West) {
		display.width = LCDHEIGHT;
		display.height = LCDWIDTH;
		write_data(0xE8);
	}
	else if (o==South) {
		display.width = LCDWIDTH;
		display.height = LCDHEIGHT;
		write_data(0x88);
	}
	else if (o==East) {
		display.width = LCDHEIGHT;
		display.height = LCDWIDTH;
		write_data(0x28);
	}
	write_cmd(COLUMN_ADDRESS_SET);
	write_data16(0);
	write_data16(display.width-1);
	write_cmd(PAGE_ADDRESS_SET);
	write_data16(0);
	write_data16(display.height-1);
}

orientation get_orientation() {
	return display.orient;
}

uint16_t get_width() {
	return display.width;
}

uint16_t get_height() {
	return display.height;
}

void fill_rectangle(rectangle r, uint16_t col)
{
	uint16_t x, y;
	write_cmd(COLUMN_ADDRESS_SET);
	write_data16(r.left);
	write_data16(r.right);
	write_cmd(PAGE_ADDRESS_SET);
	write_data16(r.top);
	write_data16(r.bottom);
	write_cmd(MEMORY_WRITE);
	for(x=r.left; x<=r.right; x++)
		for(y=r.top; y<=r.bottom; y++)
			write_data16(col);
}

/*void draw_check() {
	uint16_t x, y;
	for(x = 0; x < LCDWIDTH; x++) {
		for(y = 0; y < LCDHEIGHT; y++) {
			if((x*y) % 2 == 0) {
				*(raster + (x * LCDHEIGHT) + y) = BLACK;
			} else {
				*(raster + (x * LCDHEIGHT) + y) = WHITE;
			}
		}
	}
}

void clear_raster() {
	uint16_t x, y;
	for(x = 0; x < LCDWIDTH; x++) {
		for(y = 0; y < LCDHEIGHT; y++) {
			*(raster + (x * LCDHEIGHT) + y) = BLACK;
		}
	}
}

void flip_buffer() {
	rectangle r = {0, display.width-1, 0, display.height-1};
	fill_rectangle_indexed(r, raster);
}*/

void fill_rectangle_indexed(rectangle r, uint16_t* col) {
	uint16_t x, y;
	write_cmd(COLUMN_ADDRESS_SET);
	write_data16(r.left);
	write_data16(r.right);
	write_cmd(PAGE_ADDRESS_SET);
	write_data16(r.top);
	write_data16(r.bottom);
	write_cmd(MEMORY_WRITE);
	for(x=r.left; x<=r.right; x++) {
		for(y=r.top; y<=r.bottom; y++) {
			write_data16(*col);
			col++;
		}
	}
}

void fill_rectangle_indexed_scale(rectangle r, uint16_t* col, int s) {
	uint16_t x, y, i, j, k;
	write_cmd(COLUMN_ADDRESS_SET);
	//write_data16(r.left);
	//write_data16(r.left + ((r.right - r.left + 1) * s) - 1);
	write_data16(r.left * s);
	write_data16(r.right * s);
	write_cmd(PAGE_ADDRESS_SET);
	//write_data16(r.top);
	//write_data16(r.top + ((r.bottom - r.top + 1) * s) - 1);
	write_data16(r.top * s);
	write_data16(r.bottom * s);
	write_cmd(MEMORY_WRITE);
	for(x = 0; x < (r.right - r.left); x++) {
		for(i = 0; i < s; i++) {
			for(y = 0; y < (r.bottom - r.top); y++) {
				k = (x * (r.bottom - r.top)) + y;
				for(j = 0; j < s; j++) {
					write_data16(*(col + k));
				}
			}
		}
	}
}

void fill_rectangle_bitmap(rectangle r, uint16_t* bitmap, uint16_t foreground, uint16_t background) {
	uint16_t x, y, i;
	write_cmd(COLUMN_ADDRESS_SET);
	write_data16(r.left);
	write_data16(r.right);
	write_cmd(PAGE_ADDRESS_SET);
	write_data16(r.top);
	write_data16(r.bottom);
	write_cmd(MEMORY_WRITE);
	i = 0;
	for(x = r.left; x <= r.right; x++) {
		for(y = r.top; y <= r.bottom; y++) {
			if((*bitmap) & (1 << i)) {
				write_data16(foreground);
			} else {
				write_data16(background);
			}
			if(++i == 16) {
				i = 0;
				bitmap++;
			}
		}
	}
}

void clear_screen()
{
	display.x = 0;
	display.y = 0;
	rectangle r = {0, display.width-1, 0, display.height-1};
	fill_rectangle(r, display.background);
}

void display_char(char c)
{
	uint16_t x, y;
	PGM_P fdata; 
	uint8_t bits, mask;
	uint16_t sc=display.x, ec=display.x + 4, sp=display.y, ep=display.y + 7;
	if (c < 32 || c > 126) return;
	fdata = (c - ' ')*5 + font5x7;
	write_cmd(PAGE_ADDRESS_SET);
	write_data16(sp);
	write_data16(ep);
	for(x=sc; x<=ec; x++) {
		write_cmd(COLUMN_ADDRESS_SET);
		write_data16(x);
		write_data16(x);
		write_cmd(MEMORY_WRITE);
		bits = pgm_read_byte(fdata++);
		for(y=sp, mask=0x01; y<=ep; y++, mask<<=1)
			write_data16((bits & mask) ? display.foreground : display.background);
	}
	write_cmd(COLUMN_ADDRESS_SET);
	write_data16(x);
	write_data16(x);
	write_cmd(MEMORY_WRITE);
	for(y=sp; y<=ep; y++)
		write_data16(display.background);

	display.x += 6;
	if (display.x >= display.width) { display.x=0; display.y+=8; }
}

void display_string(char *str)
{
	uint8_t i;
	for(i=0; str[i]; i++) 
		display_char(str[i]);
}
