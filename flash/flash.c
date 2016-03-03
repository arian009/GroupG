#include "avr/io.h"
#include <util/delay.h>
#include <avr/interrupt.h>

ISR(INT0_vect) {
	PORTB = ~PORTB;
}

int main(void) {
	DDRD = 0x00;
	PORTD = 0xFF;
	DDRB = 0xFF;
	//PORTC = 0xFF;
	PORTB = 1 << 7;
	EICRA = (0 << ISC11) | (1 << ISC10) | (0 << ISC01) | (1 << ISC00);
	EIMSK = (0 << INT1) | (0 << INT1) | (1 << INT0);
	sei();
	while(1);
	return 1;
}