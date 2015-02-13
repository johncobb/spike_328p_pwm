/*
 * soft_pwm.c
 *
 *  Created on: Oct 20, 2014
 *      Author: jcobb
 */

#include <stdbool.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "soft_pwm.h"


//! global buffers
uint8_t compare[CHMAX];
volatile uint8_t compbuff[CHMAX];
uint8_t pwm_to_motor = 32;


// log debugging
static const char _tag[] PROGMEM = "soft_pwm: ";

static volatile char term_in = 0;

void soft_pwm_terminal_in_cb(uint8_t c)
{
	term_in = c;

}

static bool sweep = false;
static uint8_t sweep_direction = 0;
static void servo_sweep();

// input handler
static void handle_input(uint8_t input);

void soft_pwm_init()
{

	DDRD = PORTD_MASK;            // set port pins to output
	DDRB = PORTB_MASK;            // set port pins to output

	for(int i=0 ; i<CHMAX ; i++)      // initialize all channels
	{
		compare[i] = pwm_to_motor;           // set default PWM values
		compbuff[i] = pwm_to_motor;          // set default PWM values
	}

	TIFR0 = _BV(TOV0);           // clear interrupt flag
	TIMSK0 = _BV(TOIE0);         // enable overflow interrupt 31.25Kz based on 8MHz clock
	//TCCR0B = _BV(CS00);         // start timer, no prescale

	// Prescalar Clock/8
	TCCR0B |= _BV(CS10);
	//sei();         // enable interrupts

}

void soft_pwm_tick()
{
	if(term_in > 0) {
		handle_input(term_in);
		term_in = 0;
	}
}

static void servo_sweep()
{
	if(sweep_direction == 0) {
		fast_pwm_setval(0, 0);
		sweep_direction = 1;
	} else {
		fast_pwm_setval(255, 0);
		sweep_direction = 0;
	}
	set_timer(500);
}

static void handle_input(uint8_t input)
{
	uint8_t idx = (uint8_t) (input - '0');

	uint8_t duty_cycle = 28 + (idx * 28);

	soft_pwm_setval(duty_cycle, 0);
}

void soft_pwm_setval(uint8_t val, uint8_t motor)
{
	compbuff[motor] = val;
}

ISR(TIMER0_OVF_vect)
{
	for(int i=0; i<CHMAX; i++)      // initialize all channels
	{
		compare[i] = pwm_to_motor;           // set default PWM values
		compbuff[i] = pwm_to_motor;          // set default PWM values
	}

	// set the mask for the pins we'll be working with
	static uint8_t portb_pins = PORTB_MASK;
	static uint8_t portd_pins = PORTD_MASK;

	static uint8_t softcount = SOFTCOUNTMAX;

	PORTB = portb_pins;            // update outputs
	PORTD = portd_pins;            // update outputs

	if(++softcount == SOFTCOUNTMAX){

		// the compare values only when counter = 0.
		compare[0] = compbuff[0];   // verbose code for speed
		compare[1] = compbuff[1];
		compare[2] = compbuff[2];
		compare[3] = compbuff[3];

		portb_pins = PORTB_MASK;     // set all port pins high
		portd_pins = PORTD_MASK;     // set all port pins high

		softcount = 0;
	}

	// clear port pin on compare match (executed on next interrupt)
	if(compare[0] == softcount) CH0_CLEAR;
	if(compare[1] == softcount) CH1_CLEAR;
	if(compare[2] == softcount) CH2_CLEAR;
	if(compare[3] == softcount) CH3_CLEAR;
}




