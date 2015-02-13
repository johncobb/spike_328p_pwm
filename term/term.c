/*
 * term.c

 *
 *  Created on: Oct 21, 2014
 *      Author: jcobb
 */
#include <avr/io.h>
#include <math.h>
#include <avr/pgmspace.h>
#include "term.h"
#include "../util/log.h"
#include "../pwm/pwm.h"

static const char _tag[] PROGMEM = "term: ";


uint8_t pwm_array[10];

uint16_t pwm_val_array[5] = {128, 129, 130};


void term_handle_pwm_input(uint8_t input)
{
	// Convert input to uint8_t
	uint8_t idx = (uint8_t) (input - '0');

	pwm_setval(pwm_val_array[idx], 0);
	pwm_setval(pwm_val_array[idx], 1);
	pwm_setval(pwm_val_array[idx], 2);
	pwm_setval(pwm_val_array[idx], 3);
	pwm_setval(pwm_val_array[idx], 4);
	pwm_setval(pwm_val_array[idx], 5);



	/*
	pwm_setval(pwm_array[idx], 0);
	pwm_setval(pwm_array[idx], 1);
	pwm_setval(pwm_array[idx], 2);
	pwm_setval(pwm_array[idx], 3);
	pwm_setval(pwm_array[idx], 4);
	pwm_setval(pwm_array[idx], 5);
	 */
	LOG("pwm_setval: %d\r\n", pwm_array[idx]);



}

