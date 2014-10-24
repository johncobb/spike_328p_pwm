/*
 * soft_pwm.h
 *
 *  Created on: Oct 20, 2014
 *      Author: jcobb
 */

#ifndef SOFT_PWM_H_
#define SOFT_PWM_H_

#define CHMAX			4  	// Max number of pwm channels
#define PWM_DEFAULT		75	// Initial PWM value
#define SOFTCOUNTMAX	625

extern uint8_t pwm_to_motor;


// masks for the motors
#define PORTB_MASK  (1 << PB1)|(1 << PB2)|(1 << PB3)
#define PORTD_MASK  (1 << PD3)

#define CH0_CLEAR (pinlevelB &= ~(1 << PB1)) // map CH0 to PB1
#define CH1_CLEAR (pinlevelB &= ~(1 << PB2)) // map CH1 to PB2
#define CH2_CLEAR (pinlevelB &= ~(1 << PB3)) // map CH2 to PB3
#define CH3_CLEAR (pinlevelD &= ~(1 << PD3)) // map CH3 to PD3

void soft_pwm_init();
void soft_pwm_tick();
void soft_pwm_setval(uint8_t val, uint8_t motor);

#endif /* SOFT_PWM_H_ */
