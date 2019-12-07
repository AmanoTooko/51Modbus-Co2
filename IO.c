/*****************************************************************************
Copyright:https://github.com/AmanoTooko/
File name: IO.c
Description: 控制外部LED输出，检测地址输入，读取AD输入
Author: Y.Morino
Version: 2
Date: 2019/11/30
*****************************************************************************/

#include "IO.h"

void InitADIO(void)
{
	P1M0 = 0x02;
	P1M1 = 0x00;  //P1^7设置为开漏用于输入AD检测
	P4SW |= 0x20; //P4推挽输出
	LED_STATE = LOW;
	EN_485 = LOW;

	P1ASF = (1 << 7); //P1^7设置为AD输入
	ADC_CONTR = ADC_360T | ADC_ON;
}

void LEDON(void)
{
	LED_STATE = LOW;
}

void LEDOFF(void)
{
	LED_STATE = HIGH;
}

void LEDBlink(void)
{
	LED_STATE = ~LED_STATE;
}
void Start485Send(void)
{
	EN_485 = HIGH;
}

void Stop485Send(void)
{
	EN_485 = LOW;
}

uint8 Get485Addr(void)
{
	return ~P2;
}
//此函数由于电路图变更未使用
uint GetADC10bit(void)
{
	uint adc;
	uchar i;

	ADC_RES = 0;
	ADC_RESL = 0;

	ADC_CONTR = (ADC_CONTR & 0xe0) | ADC_START | 7;

	//13T/loop, 40*13=520T=23.5us @ 22.1184M
	i = 250;
	do
	{
		if (ADC_CONTR & ADC_FLAG)
		{
			ADC_CONTR &= ~ADC_FLAG;
			adc = (uint)ADC_RES;
			adc = (adc << 2) | (ADC_RESL & 3);
			return adc;
		}
	} while (--i);
	return 1024;
}