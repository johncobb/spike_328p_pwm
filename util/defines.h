/*
 * defines.h
 *
 *  Created on: Oct 20, 2014
 *      Author: jcobb
 */

#ifndef DEFINES_H_
#define DEFINES_H_

#include <stdbool.h>


typedef bool boolean;
typedef uint8_t byte;

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))



#endif /* DEFINES_H_ */
