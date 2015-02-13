/*
 * main.c
 *
 *  Created on: Oct 13, 2014
 *      Author: jcobb
 */

// BUG FOR MISSING SYMBOLS
// https://bugs.eclipse.org/bugs/show_bug.cgi?id=364227


#define F_CPU	8000000


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "util/defines.h"
#include "util/clock.h"
#include "util/log.h"
#include "term/term.h"
#include "pwm/pwm.h"
#include "pwm/fast_pwm.h"
#include "pwm/soft_pwm.h"
#include "led/led.h"


// timeout helper
volatile clock_time_t future = 0;
bool timeout();
void set_timer(clock_time_t timeout);


// log debugging
static const char _tag[] PROGMEM = "main: ";
volatile char main_term_in = 0;

static void pwm_handle_intput(uint8_t input);

void terminal_in_cb(uint8_t c)
{
	main_term_in = c;
}


int main() {


	led_init();
	debug_init(terminal_in_cb);
	//debug_init(fast_pwm_terminal_in_cb);
	pwm_init(); // May interfere with clock or other clock driven sources
	//fast_pwm_init();

	LOG("spike_328p_pwm started...\r\n");


	// TODO: DO NOT init_clock! pwm.c emulates the isr routine to update
	// the timer in clock.c
	//clock_init(); // May interfere with pwm or other clock driven sources

	sei();

	uint8_t duty_cycle = 1;

	while (1) {

		//fast_pwm_tick();

		if(main_term_in > 0) {
			pwm_handle_intput(main_term_in);
			main_term_in = 0;
		}
		pwm_tick();




		if(timeout()) {
			led_tick();
			set_timer(50);
		}

		_delay_ms(1);
	}

	return 0;
}
#define duty_min 185
#define duty_max 255

volatile uint8_t duty = duty_min;

static void pwm_handle_intput(uint8_t input)
{
	//uint8_t idx = (uint8_t) (input - '0');

	LOG("raw_input=%d\r\n", input);

	// handle input pulsing full range
	if(input == 49 || input == 49) {
		duty++;
		if(duty == duty_max) {
			duty = duty_max;
		}
	}
	else if(input == 48 || input == 48)  {
		duty--;
		if(duty < duty_min) {
			duty = duty_min;
		}

	}


	pwm_setval(duty, 0);
	pwm_setval(duty, 1);
	pwm_setval(duty, 2);
	pwm_setval(duty, 3);
	pwm_setval(duty, 4);
	pwm_setval(duty, 5);

	LOG("pwm_setval: %d\r\n", duty);
}

void set_timer(clock_time_t timeout)
{
	future = clock_time() + timeout;
}

// timeout routine to demonstrate clock_time
// being kept by pwm isr interrupt
bool timeout()
{
	bool timeout = false;

	if(clock_time() >= future)
	{
		set_timer(1000);
		timeout = true;

	}

	return timeout;
}



