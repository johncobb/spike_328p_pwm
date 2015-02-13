/*
 * fast_pwm.c
 *
 *  Created on: Nov 11, 2014
 *      Author: jcobb
 */

#ifndef FAST_PWM_C_
#define FAST_PWM_C_

void fast_pwm_terminal_in_cb(uint8_t c);
void fast_pwm_init();
void fast_pwm_tick();
void fast_pwm_setval(uint16_t val, uint8_t channel);

#endif /* FAST_PWM_C_ */
