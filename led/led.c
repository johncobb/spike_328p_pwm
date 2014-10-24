/*
 * led.c
 *
 *  Created on: Oct 20, 2014
 *      Author: jcobb
 */
#include <avr/io.h>
#include "led.h"


void led_init()
{
	DDRB |= _BV(PB5);
}

void led_tick()
{

	PORTB ^= _BV(PB5);
}


