/*
 * fast_pwm.c
 *
 *  Created on: Nov 11, 2014
 *      Author: jcobb
 */
#include <avr/pgmspace.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "../util/clock.h"
#include "../util/log.h"
#include "pwm.h"
#include "fast_pwm.h"

// code based upon Patrick Hood-Daniel's http://www.youtube.com/watch?v=9WeewNNGs5E
// Microcontrollers - AVR - Controlling a Hobby Servo Using the Internal PWM


// p. 133 datasheet
// Waveform Generation Mode (WGM Mode 14) = Fast PWM  set bits WGM13 WGM12 WGM11
// counts 0 to top of ICR1

// Period ICR1 =19,999
// Prescalar
// Output Mode Inverted COM1A1 COM1A0 establishes OCR1A as output
// Set ICR 19999 - 2000 to get 2ms. (Servo is good between 2000 and 1000)


// Servo operating frequency 50Hz
// 8Mhz Clock
// Clock div 8 takes speed to 1Mhz
// Servo operates at 50Hz
// Clock/prescalar/servo_speed
// 8,000,000/8/50 = 20,000 cycles or 20ms.
// 0-19,999

// log debugging
static const char _tag[] PROGMEM = "fast_pwm: ";

static volatile char term_in = 0;

// input handler
static void handle_input(uint8_t input);

// timeout helper
static volatile clock_time_t future = 0;
static void set_timer(clock_time_t timeout);
static bool timeout();

static bool sweep = false;
static uint8_t sweep_direction = 0;
static void servo_sweep();

#define DUTY_MIN 	800
#define DUTY_MAX	2200
#define DUTY_MID	(DUTY_MAX-DUTY_MIN)/2
#define DUTY_STEP 	(DUTY_MAX-DUTY_MIN)/180

uint16_t duty_cycle = DUTY_MIN;

void fast_pwm_init()
{
	// setup our terminal callback
	debug_init(fast_pwm_terminal_in_cb);

	// Set PB1 pin
	DDRB |= _BV(DDB1);

	// WGM Mode 14
	TCCR1A |=  _BV(WGM11);
	TCCR1B |= _BV(WGM13) | _BV(WGM12);

	// Set output from OCR1A to be inverted
	TCCR1A |= _BV(COM1A0) | _BV(COM1A1);

	// Prescalar Clock/8
	TCCR1B |= _BV(CS11);

	// Set top count to
	ICR1 = 19999;

	// set to 2ms. pulse
	OCR1A = 19999-2000; // 18,000

}

void fast_pwm_tick()
{
	if(term_in > 0) {
		handle_input(term_in);
		term_in = 0;
	}

	if(timeout()) {
		// simple servo sweep test
		if(sweep)
			servo_sweep();

	}
}

void fast_pwm_setval(uint16_t val, uint8_t channel)
{
	OCR1A = ICR1 - val;
}

void fast_pwm_terminal_in_cb(uint8_t c)
{
	term_in = c;
}

static void handle_input(uint8_t input)
{
	LOG("raw_input=%d\r\n", input);

	if((char*) input == 's') {
		sweep = true;
		return;
	} else {
		sweep = false;
	}

	// handle input 0-9 pulsing full range
	if(input >=48 && input <=57) {
		// Convert input to uint8_t
		uint8_t idx = (uint8_t) (input - '0');

		duty_cycle = DUTY_MIN + (idx * 156);

		//OCR1A = ICR1 - duty_cycle;
		fast_pwm_setval(duty_cycle, 0);

	}

	// + key
	if(input == 43) {
		duty_cycle+=DUTY_STEP;
		if(duty_cycle > DUTY_MAX) {
			duty_cycle = DUTY_MAX;
		}

		fast_pwm_setval(duty_cycle, 0);
		//OCR1A = duty_cycle;

	} // - key
	else if (input == 45) {
		duty_cycle-=DUTY_STEP;
		if(duty_cycle < DUTY_MIN)
			duty_cycle = DUTY_MIN;
		fast_pwm_setval(duty_cycle, 0);
		//OCR1A = duty_cycle;
	}

	LOG("pwm_setval: %d\r\n", duty_cycle);

}

static void servo_sweep()
{
	if(sweep_direction == 0) {
		fast_pwm_setval(DUTY_MIN, 0);
		sweep_direction = 1;
	} else {
		fast_pwm_setval(DUTY_MAX, 0);
		sweep_direction = 0;
	}
	set_timer(500);
}

static void set_timer(clock_time_t timeout)
{
	future = clock_time() + timeout;
}

// timeout routine to demonstrate clock_time
// being kept by pwm isr interrupt
static bool timeout()
{
	bool timeout = false;

	if(clock_time() >= future)
	{
		set_timer(1000);
		timeout = true;

	}

	return timeout;
}




