/*
 * log.h
 *
 *  Created on: Oct 20, 2014
 *      Author: jcobb
 */

#ifndef LOG_H_
#define LOG_H_

typedef void (*debug_rx_cb_t)(uint8_t);
typedef void (*debug_out_cb_t)(uint8_t);





//#define DEBUG_PORT       PORTE
//#define DEBUG_USART      USARTE0
//#define DEBUG_ISR_VECTOR USARTE0_RXC_vect

//
//#define DEBUG_PORT       PORTD
//#define DEBUG_USART      USART_UDRE_vect

#define DEBUG_ISR_VECTOR USART_RX_vect

void debug_test(void);
void _debug_init(debug_rx_cb_t);
void _debug_stringz( char* data );
void _debug_byte( unsigned char byte );
void _debug_byte_as_hex( unsigned char byte );
void _debug_set_rx_cb(debug_rx_cb_t);
void _debug_set_out_cb(debug_out_cb_t);
void _debug_hex_dump( uint8_t * data, int length);
void _debug_log(const char * prefix, const char * fmt, ...);
#define debug_init(x) _debug_init(x);
#define debug_stringz(x) _debug_stringz(x);
#define debug_byte(x) _debug_byte(x);
#define debug_byte_as_hex(x) _debug_byte_as_hex(x);
#define debug_set_rx_cb(x) _debug_set_rx_cb(x);
#define debug_set_out_cb(x) _debug_set_out_cb(x);
#define debug_hex_dump(x,y) _debug_hex_dump(x,y);
#define debug_log(x,y,...) _debug_log(x,y,##__VA_ARGS__);

#define LOG(fmt, ...)	{ debug_log(_tag, PSTR(fmt), ##__VA_ARGS__); }
#define LOG_np(fmt, ...) { debug_log(0, PSTR(fmt), ##__VA_ARGS__); }
#define LOGHEX(x,y) { debug_hex_dump(x,y); }





#endif /* LOG_H_ */
