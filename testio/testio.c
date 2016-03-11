/* 
 * "testio.c"
 * PIN USAGE
 *    PORT A - 0:AIn0, 1:AIn1, 2:AIn2, 3:DIn0, 4:DIn1, 5:Din2
 *    PORT B - LCD Control Port
 *    PORT C - 0:Dout0, 1:Dout1, 2:Dout2
 *    PORT D - LCD Data Port
 * 
 * 
 */
#include "testio.h"
#include "avr/io.h"
#include "lcd.h"
#include <util/delay.h>

/**
 * Called once on initialization - Initialize the io functions and the controller state.
 */
void init(State* state) {
	init_lcd();
	init_adc();
	DDRA = (1 << 3) | (1 << 4) | (1 << 5); //Setup up digital input ports as input
	PORTA = (1 << 3) | (1 << 4) | (1 << 5); //Enable pull up resistors on input ports	
	set_orientation(North);
	
	rectangle r2 = {5, 45, 5, 45}; //The three squares denoting digital inputs
	rectangle r3 = {5, 45, 50, 90};
	rectangle r4 = {5, 45, 95, 135};
	state->r2 = r2; //Initialize the state.
	state->r3 = r3;
	state->r4 = r4;
	rectangle r5 = {get_width() - 50, get_width() - 5, 5, 45}; //The three squares denoting digital outputs
	rectangle r6 = {get_width() - 50, get_width() - 5, 50, 90};
	rectangle r7 = {get_width() - 50, get_width() - 5, 95, 135};
	state->r5 = r5;
	state->r6 = r6;
	state->r7 = r7;
	state->x10 = (get_width() / 4); //Computes the x positions of the analogue bars
	state->x11 = state->x10 + (get_width() / 6);
	state->x21 = state->x11 + (get_width() / 6);
	state->x31 = state->x21 + (get_width() / 6);
}

/**
 * Called at the start of each loop iteration - read in all input channels and store them in input variable.
 */
void input(InputParams* dest) {
	uint16_t result;
	channel_adc(0); //Read from channel 0.
	result = read_adc(); //Read in.
	dest->analog0 = (result * 3.3) / 1024; //Compute voltage level.
	
	channel_adc(1); //As above
	result = read_adc();
	dest->analog1 = (result * 3.3) / 1024;
	
	channel_adc(2);
	result = read_adc();
	dest->analog2 = (result * 3.3) / 1024;
	
	dest->digit0 = !(PINA & (1 << 3)); //Pulled low when button is pressed.
	dest->digit1 = !(PINA & (1 << 4));
	dest->digit2 = !(PINA & (1 << 5));
}

/**
 * Called after the input is resolved - compute based on input variables and current state and set output variables.
 */
void process(State* state, InputParams* src, OutputParams* dest) {
	draw_voltage(state->x10, state->x11, state->lastVoltage1, src->analog0); //Draw three voltage bars.
	//state->runningTotal = (state->runningTotal + src->analog1) / 2;
	//draw_voltage(state->x11, state->x21, 3.3, state->runningTotal);
	draw_voltage(state->x11, state->x21, state->lastVoltage2, src->analog1);
	draw_voltage(state->x21, state->x31, state->lastVoltage3, src->analog2);
	state->lastVoltage1 = src->analog0; //Store last voltages. To prevent flickering.
	state->lastVoltage2 = src->analog1; //Not strictly necessary. Example usage of state.
	state->lastVoltage3 = src->analog2;
	
	if(!src->digit0) {
		fill_rectangle(state->r2, RED); //Draw Square
	} else {
		fill_rectangle(state->r2, BLACK); //Clear Square
	}
	if(!src->digit1) {
		fill_rectangle(state->r3, GREEN);
	} else {
		fill_rectangle(state->r3, BLACK);
	}
	if(!src->digit2) {
		fill_rectangle(state->r4, BLUE);
	} else {
		fill_rectangle(state->r4, BLACK);
	}
	
	dest->digit0 = !(src->digit0 & src->digit1); //Simple digital output function
	dest->digit1 = !(src->digit0 | src->digit1);
	dest->digit2 = !(src->digit0 ^ src->digit1);
	
	if(dest->digit0) {
		fill_rectangle(state->r5, RED); //Draw Square
	} else {
		fill_rectangle(state->r5, BLACK); //Clear Square
	}
	if(dest->digit1) {
		fill_rectangle(state->r6, GREEN);
	} else {
		fill_rectangle(state->r6, BLACK);
	}
	if(dest->digit2) {
		fill_rectangle(state->r7, BLUE);
	} else {
		fill_rectangle(state->r7, BLACK);
	}
}

/**
 * Called after processing complete - set all output channels to specified values.
 */
void output(OutputParams* src) {
	PORTC = (src->digit0 << 0) | (src->digit1 << 1) | (src->digit2 << 2);
}

int main(void) {
	InputParams inParams;
	OutputParams outParams;
	State state;
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

/*
 * Converts 3 integer rgb values in 0-255 range to the 565 format. Colour definitions RGB565
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