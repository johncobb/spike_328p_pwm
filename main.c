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
#include "led/led.h"


// timeout helper
volatile clock_time_t future = 0;
bool timeout();
void set_timer(clock_time_t timeout);


// log debugging
static const char _tag[] PROGMEM = "main: ";
volatile char term_in = 0;

void terminal_in_cb(uint8_t c)
{
	term_in = c;
	LOG("input=%c\r\n", c);

	term_handle_pwm_input(c);
}


int main() {


	led_init();
	debug_init(terminal_in_cb);

	pwm_init(); // May interfere with clock or other clock driven sources

	// TODO: DO NOT init_clock! pwm.c emulates the isr routine to update
	// the timer in clock.c
	//clock_init(); // May interfere with pwm or other clock driven sources

	sei();

	LOG("pwm console ready...\r\n");

	while (1) {

		pwm_tick();

		if(timeout())
		{
			led_tick();
			set_timer(1000);
		}

		_delay_ms(50);
	}

	return 0;
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



