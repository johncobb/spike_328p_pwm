/*
 * pwm.c
 *
 *  Created on: Oct 13, 2014
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

#define LARGEST_8BIT_NUMBER		255
#define LARGEST_16BIT_NUMBER	65535
#define PWM_DEFAULT				32

// TIME = PERIOD/FREQUENCY
// 0.000032 = 256/8,000,000

void timer0_isr_emulation();

static volatile uint8_t duty_cycle_channel0 = PWM_DEFAULT;
static volatile uint8_t duty_cycle_channel1 = PWM_DEFAULT;
static volatile uint8_t duty_cycle_channel2 = PWM_DEFAULT;
static volatile uint8_t duty_cycle_channel3 = PWM_DEFAULT;
static volatile uint8_t duty_cycle_channel4 = PWM_DEFAULT;
static volatile uint8_t duty_cycle_channel5 = PWM_DEFAULT;

static const char _tag[] PROGMEM = "pwm: ";

static volatile char term_in = 0;

void pwm_terminal_in_cb(uint8_t c)
{
	term_in = c;

}

// input handler
static void handle_input(uint8_t input);

// Hadware PMW
// Since we are using TCCR0A and TCCR0B in clock they need to be commented
// Another option is to use soft_pwm


void pwm_init()
{

	// setup our terminal callback

	DDRD |= (1<<DDD3);
	DDRD |= (1<<DDD5);
	DDRD |= (1<<DDD6);

	DDRB |= (1<<DDB1);
	DDRB |= (1<<DDB2);
	DDRB |= (1<<DDB3);


	// Fast PWM overflow on REG A and REG B

	// Commented due to clock
	TCCR0A = _BV(COM0A1) | _BV(COM0B1) | _BV(WGM00);
	TCCR0B = _BV(CS00);
	TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM10);
	TCCR1B = _BV(CS10);
	TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM20);
	TCCR2B = _BV(CS20);

	// Enable 3 Timers

	// Commented due to clock
	// enable timer1 interrupt
	TIMSK1 |= _BV(TOIE1);
	TIMSK0 &= _BV(TOIE0);

	// Set interrupt enable
	sei();

	// Set initial duty cycle
	OCR0A = duty_cycle_channel0;
	OCR0B = duty_cycle_channel1;
	OCR1A = duty_cycle_channel5;
	OCR1B = duty_cycle_channel4;
	OCR2A = duty_cycle_channel3;
	OCR2B = duty_cycle_channel2;
}

void pwm_setval(uint8_t val, uint8_t channel)
{
	if(channel == 0){
		duty_cycle_channel0 = val;
	} else if(channel == 1) {
		duty_cycle_channel1 = val;
	} else if (channel == 2) {
		duty_cycle_channel2 = val;
	} else if (channel == 3) {
			duty_cycle_channel3 = val;
	} else if (channel == 4) {
			duty_cycle_channel4 = val;
	} else if (channel == 5) {
			duty_cycle_channel5 = val;
	}
}

volatile uint16_t frequency_counter = 0;

//#define UPDATE_FREQUENCY	500	// 500Hz for motor
#define UPDATE_FREQUENCY	50	// 50Hz for servo
#define FACTOR				32  // 8000000/256
/*
#define FACTOR		8
#define FACTOR		4
#define FACTOR		1
*/

ISR(TIMER1_OVF_vect)
{
	frequency_counter++;

	if(frequency_counter==(FACTOR*1000/UPDATE_FREQUENCY))
	{
		frequency_counter = 0;
		OCR0A = duty_cycle_channel0;
		OCR0B = duty_cycle_channel1;
		OCR1A = duty_cycle_channel5;
		OCR1B = duty_cycle_channel4;
		OCR2A = duty_cycle_channel3;
		OCR2B = duty_cycle_channel2;
	}

	// emulate timer
	timer0_isr_emulation();

}

void timer0_isr_emulation()
{

	// if 31 == 31 bit-wise
	if((frequency_counter & 0x1f) == 0)
	{
		isr_tick();
	}
}


void pwm_tick()
{
	if(term_in > 0) {
		handle_input(term_in);
		term_in = 0;
	}
}

static void handle_input(uint8_t input)
{

}

/*
int8_t pwm_sinus_array[256];
for(int i=0; i<10; i++)
{
	pwm_sinus_array[i] = sin(2.0 * i / 10 * 3.14159265) * 127;
}
*/

/*
void pwm_init()
{

	OCR0A = 128;

	TCCR0A |= _BV(COM0A1);
	TCCR0A |= _BV(WGM01) | _BV(WGM00);

	TCCR0B |= _BV(CS01);
}

ISR(PCINT0_vect)
{

}
*/



/*
https://sites.google.com/site/qeewiki/books/avr-guide/common-timer-theory
https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328

// CS BITS
CS02    CS01    CS00     DESCRIPTION
0       0       0        Timer/Counter0 Disabled
0       0       1        No Prescaling
0       1       0        Clock / 8
0       1       1        Clock / 64
1       0       0        Clock / 256
1       0       1        Clock / 1024

CS12     CS11    CS10    DESCRIPTION
0       0       0        Timer/Counter1 Disabled
0       0       1        No Prescaling
0       1       0        Clock / 8
0       1       1        Clock / 64
1       0       0        Clock / 256
1       0       1        Clock / 1024

CS22     CS21    CS20    DESCRIPTION
0       0       0        Timer/Counter2 Disabled
0       0       1        No Prescaling
0       1       0        Clock / 8
0       1       1        Clock / 32
1       0       0        Clock / 64
1       0       1        Clock / 128
1       1       0        Clock / 256
1       1       1        Clock / 1024


// WAVEFORM GENERATOR BITS
        WGM02   WGM01   WGM00    DESCRIPTION            TOP
0       0       0       0        Normal                 0xFF
1       0       0       1        PWM, Phase Corrected   0xFF
2       0       1       0        CTC                    OCR0A
3       0       1       1        Fast PWM               0xFF
4       1       0       0        Reserved               -
5       1       0       1        Fast PWM, Phase Corr   OCR0A
6       1       1       0        Reserved               -
7       1       1       1        Fast PWM               OCR0A

MODE    WGM13   WGM12   WGM11   WGM10    DESCRIPTION                     TOP
0        0       0      0       0        Normal                          0xFFFF
1       0       0       0       1        PWM, Phase Corrected, 8bit      0x00FF
2       0       0       1       0        PWM, Phase Corrected, 9bit      0x01FF
3       0       0       1       1        PWM, Phase Corrected, 10bit     0x03FF
4       0       1       0       0        CTC                             OCR1A
5       0       1       0       1        Fast PWM, 8bit                   0x00FF
6       0       1       1       0        Fast PWM, 9bit                   0x01FF
7       0       1       1       1        Fast PWM, 10bit                  0x03FF
8       1       0       0       0        PWM, Phase and Frequency Corr    ICR1
9       1       0       0       1        PWM, Phase and Frequency Corr    OCR1A
10      1       0       1       0        PWM, Phase Correct               ICR1
11      1       0       1       1        PWM, Phase Correct              OCR1A
12      1       1       0       0        CTC                             ICR1
13      1       1       0       1        RESERVED
14      1       1       1       0        Fast PWM                         ICR1
15      1       1       1       1        Fast PWM                         OCR1A

MODE    WGM21   WGM20    DESCRIPTION              TOP
0       0       0        Normal                  0xFF
1       0       1        PWM Phase Corrected
2       1       0        CTC                      OCR2
3       1       1        Fast PWM



x = Timer Number
        7 bit    6 bit   5 bit   4 bit   3 bit   2 bit   1 bit   0 bit     Description
TCCRxA  COMxA1   COMxA0  COMxB1  COMxB0  -       -       WGMx1   WGMx0     Timer/Counter Control Register x A (x=0,2)

TCCR1B  ICNC1    ICES1   -       WGM13   WGM12   CS12    CS11   CS10
TCCRxB  FOCxA    FOCxB   -       -       WGMx2   CSx2    CSx1    CSx0      Timer/Counter Control Register x B

TIMSKx  -        -       -       -       -       OCIExB  OCIExA  TOIEx     Timer/Counter Interrupt Mask Register
TIFRx   -        -       -       -       -       OCFxB   OCFxA   TOVx      Timer/Counter Interrupt Flag Register
TCNTx                                                                      Timer/Counter Register (stores the counter value)
OCRxA                                                                      Output Compare Register x A
OCRxB                                                                      Output Compare Register x B


*/
