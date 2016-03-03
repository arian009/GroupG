/* 
 * "testio.h"
 * 
 * 
 */

#ifndef _TESTIO_H_
#define _TESTIO_H_

#include <stdint.h>
#include "lcd.h"

typedef struct {
	double analog0, analog1, analog2;
	int digit0, digit1, digit2;
} InputParams;

typedef struct {
	
} OutputParams;

typedef struct {
	double lastVoltage1;
	double lastVoltage2;
	double lastVoltage3;
	rectangle r2;
	rectangle r3;
	rectangle r4;
	int x10;
	int x11;
	int x21;
	int x31;
} State;

void init(State* state);
void input(InputParams* dest);
void process(State* state, InputParams* src, OutputParams* dest);
void output(OutputParams* src);

uint16_t getRGB(int r, int g, int b);
void init_adc(void);
void channel_adc(uint8_t n);
uint16_t read_adc(void);
void draw_voltage(int x0, int x1, double lastVoltage, double voltage);

#endif