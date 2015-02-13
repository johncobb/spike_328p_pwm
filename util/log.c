/*
 * log.c
 *
 *  Created on: Oct 20, 2014
 *      Author: jcobb
 */

#define BAUD 38400

#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include "clock.h"
#include "log.h"

static const char _tag[] PROGMEM = "debug: ";

void _debug_byte(unsigned char byte);

const unsigned char DEBUG_hex[] PROGMEM = "0123456789ABCDEF";

debug_rx_cb_t debug_rx_cb;
debug_out_cb_t debug_out_cb;

int _debug_putch(char data, FILE *dummy);

void _debug_init(debug_rx_cb_t cb)
{
	//UBRR0H = UBRRH_VALUE;
	//UBRR0L = UBRRL_VALUE;


	uint8_t baudrate = 51; // 9600
	UBRR0H = (unsigned char) (baudrate>>8);
	UBRR0L = (unsigned char) baudrate;

	UCSR0B |= (1<<RXCIE0);


//#if USE_2X
	//UCSR0A |= _BV(U2X0);
//#else
	//UCSR0A &= ~(_BV(U2X0));
//#endif

	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

	_debug_set_rx_cb(cb);
	fdevopen(_debug_putch, NULL);
}


ISR(DEBUG_ISR_VECTOR)
{
	char data = UDR0;
	if (debug_rx_cb != 0) debug_rx_cb(data);
}

void _debug_set_rx_cb(debug_rx_cb_t cb)
{
	debug_rx_cb = cb;
}

void _debug_set_out_cb(debug_out_cb_t func)
{
	debug_out_cb = func;
}

int _debug_putch(char data, FILE *dummy)
{
	_debug_byte((uint8_t) data);

	if (debug_out_cb)
	debug_out_cb((uint8_t)data);

	return 0;
}

void _debug_stringz( char* data )
{
	unsigned char c = *data;

	while (c) {
		while (!( UCSR0A & (1<<UDRE0)));
		UDR0 = c;
		c = *(++data);
	}
}

void _debug_byte(uint8_t b)
{
	while (!( UCSR0A & (1<<UDRE0)));
	UDR0 = b;
}

void _debug_byte_as_hex( unsigned char byte )
{
	unsigned char c;

	c = pgm_read_byte(&DEBUG_hex[(byte >> 4)]);
	while ( !( UCSR0A & (1<<UDRE0)));
	UDR0 = c;

	c = pgm_read_byte(&DEBUG_hex[(byte & 0x0f)]);
	while ( !( UCSR0A & (1<<UDRE0)));
	UDR0 = c;
}

void _debug_hex_dump( uint8_t * data, int length)
{
	for (int i=0;i<length;i++)
	{
		//printf_P(PSTR("%02X "), (uint8_t)data[i]);
		_debug_byte_as_hex((unsigned char)data[i]);
		_debug_byte((uint8_t)' ');
	}
	_debug_byte((uint8_t)'\r');
	_debug_byte((uint8_t)'\n');
	//printf_P(PSTR("\r\n"));
}

void _debug_log(const char * prefix, const char * fmt, ...)
{
	if (prefix)
	{
		printf_P(PSTR("%lu "), clock_millis);
		printf_P(prefix);
	}
	va_list argptr;
	va_start(argptr, fmt);
	// Important to use vfprintf_P due to variatic signature
	// Note that there is no vprintf_P - we have to manually direct to stdout
	vfprintf_P(stdout, fmt, argptr);
	va_end(argptr);
};

#define MAX_DECIMALS 4
int find_real_part(float num);
void _debug_log_float (float to_print);

int find_i_part(int real_length, float num)
{
	int i = 10,j;
	int real_part = find_real_part(num);
	//truncate real part
	float temp_float = num - real_part;

	//adjust decimal for MAX_DECIMALS
	for(j = 0; j< MAX_DECIMALS; j++)
	{
		temp_float = temp_float*i;
	}
	//return just the MAX_DECIMAL length of the float
	return (int)(temp_float*i);
}

int find_real_part(float num)
{
	//truncate decimal
	return (int)num;
}

void _debug_log_float (float to_print)
{
	int real_part, i_part;
	char output[20], temp[10], init[2] = "";

	memset(output, '\0', sizeof(output));
	memset(temp, '\0', sizeof(temp));

	//Start with Real Part
	real_part = find_real_part(to_print);
	itoa(real_part, temp, 10);
	strcat(output, temp);

	i_part = find_i_part(strlen(temp), to_print);
	itoa(i_part, temp, 10);

	//Place the decimal point
	strcat(output, ".");
	//Add i Part
	strcat(output, temp);
	//Add new line
	//strcat(output, "\r\n");


	LOG("%s", output);
	strcpy(output, init);
}

void debug_test(void)
{
	LOG("HELLO WORLD\r\n");
	// Serial
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) )
	UDR0 = 'C';
	_delay_ms(500);
	UDR0 = 'P';
	_delay_ms(500);
	UDR0 = 'H';
	_delay_ms(500);



}
