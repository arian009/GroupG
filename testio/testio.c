/* 
 * "testio.c"
 * PIN USAGE
 *    PORT A - 0:AIn0, 1:AIn1, 2:AIn2, 3:DIn0, 4:DIn1, 5:Din2
 *    PORT B - LCD Control Port
 *    PORT D - LCD Data Port
 * 
 * 
 */
#include "testio.h"
#include "avr/io.h"
#include "lcd.h"
#include <util/delay.h>

void init(State* state) {
	init_lcd();
	init_adc();
	DDRA = (1 << 3) | (1 << 4) | (1 << 5);
	PORTA = (1 << 3) | (1 << 4) | (1 << 5);
	set_orientation(North);
	
	rectangle r2 = {5, 45, 5, 45};
	rectangle r3 = {5, 45, 50, 90};
	rectangle r4 = {5, 45, 95, 135};
	state->r2 = r2;
	state->r3 = r3;
	state->r4 = r4;
	state->x10 = (get_width() / 4);
	state->x11 = state->x10 + (get_width() / 6);
	state->x21 = state->x11 + (get_width() / 6);
	state->x31 = state->x21 + (get_width() / 6);
}

void input(InputParams* dest) {
	uint16_t result;
	channel_adc(0);
	result = read_adc();
	dest->analog0 = (result * 3.3) / 1024;
	
	channel_adc(1);
	result = read_adc();
	dest->analog1 = (result * 3.3) / 1024;
	
	channel_adc(2);
	result = read_adc();
	dest->analog2 = (result * 3.3) / 1024;
	
	dest->digit0 = !(PINA & (1 << 3));
	dest->digit1 = !(PINA & (1 << 4));
	dest->digit2 = !(PINA & (1 << 5));
}

void process(State* state, InputParams* src, OutputParams* dest) {
	draw_voltage(state->x10, state->x11, state->lastVoltage1, src->analog0);
	draw_voltage(state->x11, state->x21, state->lastVoltage2, src->analog1);
	draw_voltage(state->x21, state->x31, state->lastVoltage3, src->analog2);
	state->lastVoltage1 = src->analog0;
	state->lastVoltage2 = src->analog1;
	state->lastVoltage3 = src->analog2;
	
	if(src->digit0) {
		fill_rectangle(state->r2, RED);
	} else {
		fill_rectangle(state->r2, BLACK);
	}
	if(src->digit1) {
		fill_rectangle(state->r3, GREEN);
	} else {
		fill_rectangle(state->r3, BLACK);
	}
	if(src->digit2) {
		fill_rectangle(state->r4, BLUE);
	} else {
		fill_rectangle(state->r4, BLACK);
	}
}

void output(OutputParams* src) {

}

int main(void) {
	InputParams inParams;
	OutputParams outParams;
	State state;
	//rectangle back = {0, get_width(), 0, get_height()};
	init(&state);
	do {
		input(&inParams);
		process(&state, &inParams, &outParams);
		output(&outParams);
		_delay_ms(1);
	} while(1);
}

void draw_voltage(int x0, int x1, double lastVoltage, double voltage) {
	rectangle r1 = {x0, x1, 0, 0};
	if(lastVoltage > voltage) {
		r1.bottom = get_height() * (lastVoltage / 3.3);
		r1.top = get_height() * (voltage / 3.3);
		fill_rectangle(r1, BLACK);
		r1.top = 0;
		r1.bottom = get_height() * (voltage / 3.3);
		fill_rectangle(r1, getRGB(255 * (voltage / 3.3), 255 * (1 - (voltage / 3.3)), 0));
	} else if(lastVoltage < voltage) {
		r1.top = 0;
		r1.bottom = get_height() * (voltage / 3.3);
		fill_rectangle(r1, getRGB(255 * (voltage / 3.3), 255 * (1 - (voltage / 3.3)), 0));
	}
}

/* Colour definitions RGB565
 * (r / 8, g / 4, b / 8)
 */
uint16_t getRGB(int r, int g, int b) {
	return (b & 0b11111000) | ((g & 0b11111100) << 3) | ((r & 0b11111000) << 8);
}

void init_adc(void) {
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); //Enable ADC. Set prescaler 64.
}

void channel_adc(uint8_t n) {
	ADMUX = n;
}

uint16_t read_adc(void) {
	ADCSRA |= (1 << ADSC);
	while((ADCSRA & _BV(ADSC)) != 0);
	return ADC;
}