#include "STC12C5A60S2.h"
#ifndef __COMMON_H
#define __COMMON_H
#define HIGH 1
#define LOW 0

#define ADC_START	(1 << 3)
#define ADC_FLAG	(1 << 4)
#define ADC_360T	(1 << 5)
#define ADC_ON		(1 << 7)

#define TRUE 1
#define FALSE 0

typedef unsigned int uint;
typedef unsigned char uchar ;
typedef unsigned char uint8 ;
typedef unsigned short uint16;
typedef unsigned long uint32;
#endif