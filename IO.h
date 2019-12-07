#include "STC12C5A60S2.h"
#include "Common.h"
#ifndef _IO_H
#define _IO_H

sbit LED_STATE=P4^5;
sbit EN_485=P1^1;

sbit CO2_RST=P1^5;
sbit CO2_PSEN=P1^6;
sbit CO2_ADC=P1^7;
sbit CO2_MACL=P1^4;


#endif

extern void InitADIO(void);
extern void LEDON(void);
extern void LEDOFF(void);
extern void LEDBlink(void);
extern void Start485Send(void);
extern void Stop485Send(void);
extern uint8 Get485Addr(void);
extern uint GetADC10bit(void);