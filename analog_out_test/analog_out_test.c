/*
 *  "analog_out_test.c"
 *
 *
 *
 */
#include <avr/io.h>
#include "analog_out_test.h"
#include <util/delay.h>

void init_pwm(void) {
    DDRB = (1 << 7);

    /* TIMER 2 */
    DDRD |= _BV(PD6); /* PWM out */
    DDRD |= _BV(PD7); /* inv. PWM out */


    TCCR2A = _BV(WGM20) | /* fast PWM/MAX */
            _BV(WGM21) | /* fast PWM/MAX */
            _BV(COM2A1); /* A output */
    TCCR2B = _BV(CS20); /* no prescaling */
}

void pwm_duty(uint8_t x) {
    OCR2A = x;
}

#define SIGA 64
#define SIGB 192

int main() {
	uint8_t pwm = SIGA;
	init_pwm();
	while(1) {
		pwm_duty(pwm);
		PORTB = ~PORTB;
		_delay_ms(1000);
		if(pwm == SIGA) {
			pwm = SIGB;
		} else {
			pwm = SIGA;
		}
	}
}