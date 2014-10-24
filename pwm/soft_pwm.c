/*
 * soft_pwm.c
 *
 *  Created on: Oct 20, 2014
 *      Author: jcobb
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "soft_pwm.h"


//! global buffers
uint8_t compare[CHMAX];
volatile uint8_t compbuff[CHMAX];
uint8_t pwm_to_motor = 32;

uint8_t motor0 = 32;
uint8_t motor1 = 32;
uint8_t motor2 = 32;
uint8_t motor3 = 32;


void soft_pwm_init()
{
	uint8_t i;

	DDRD = PORTD_MASK;            // set port pins to output
	DDRB = PORTB_MASK;            // set port pins to output

	for(i=0 ; i<CHMAX ; i++)      // initialize all channels
	{
		compare[i] = pwm_to_motor;           // set default PWM values
		compbuff[i] = pwm_to_motor;          // set default PWM values
	}

	TIFR0 = (1 << TOV0);           // clear interrupt flag
	TIMSK0 = (1 << TOIE0);         // enable overflow interrupt 31.25Kz
	TCCR0B = (1 << CS00);         // start timer, no prescale

	//sei();         // enable interrupts

}

void soft_pwm_tick()
{

}

void soft_pwm_setval(uint8_t val, uint8_t motor)
{

}

/*
ISR(TIMER0_OVF_vect){
	uint8_t i;
	for(i=0 ; i<CHMAX ; i++)      // initialize all channels
	{
		compare[i] = pwm_to_motor;           // set default PWM values
		compbuff[i] = pwm_to_motor;          // set default PWM values
	}

	static uint8_t pinlevelB=PORTB_MASK, pinlevelD=PORTD_MASK;
	static uint16_t softcount = SOFTCOUNTMAX;

	PORTB = pinlevelB;            // update outputs
	PORTD = pinlevelD;            // update outputs

	if(++softcount > SOFTCOUNTMAX){

		// the compare values only when counter = 0.
		compare[0] = compbuff[0];   // verbose code for speed
		compare[1] = compbuff[1];
		compare[2] = compbuff[2];
		compare[3] = compbuff[3];

		pinlevelB = PORTB_MASK;     // set all port pins high
		pinlevelD = PORTD_MASK;     // set all port pins high

		softcount = 0;
	}
	// clear port pin on compare match (executed on next interrupt)
	if(compare[0] == softcount) CH0_CLEAR;
	if(compare[1] == softcount) CH1_CLEAR;
	if(compare[2] == softcount) CH2_CLEAR;
	if(compare[3] == softcount) CH3_CLEAR;
}
*/



