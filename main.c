/*****************************************************************************
Copyright:https://github.com/AmanoTooko/
File name: main.c
Description: 主函数，轮流查询Co2串口信息和modbus信息，当有信息时进行控制。
Author: Y.Morino
Version: 1
Date: 2019/11/30
*****************************************************************************/
#define TIMER_LOW 0x9A
#define TIMER_HIGH 0xA9
#define WDT 0x37
#include "modbus.h"
#include "uart.h"
#include "IO.h"
#include <string.h>

bit ModbusRecvCompleteFlag, Co2RecvCompleteFlag, msFlag;
uint16 co2Data = 0;
uint16 LEDCount = 0;

void Co2Main()
{

	if (Co2RecvCompleteFlag == TRUE)
	{
		WDT_CONTR = WDT;
		ES = 0; //关串口1中断
		if (Co2RecvBuf[4] == 0x20 &&
			Co2RecvBuf[5] == 0x70 &&
			Co2RecvBuf[6] == 0x70 &&
			Co2RecvBuf[7] == 0x6D &&
			Co2RecvBuf[8] == 0x0D &&
			Co2RecvBuf[9] == 0x0A)
		{
			co2Data = 0;
			if (Co2RecvBuf[0] >= 0x30)
				co2Data += ((Co2RecvBuf[0] - 0x30) * 1000);
			if (Co2RecvBuf[1] >= 0x30)
				co2Data += ((Co2RecvBuf[1] - 0x30) * 100);
			if (Co2RecvBuf[2] >= 0x30)
				co2Data += ((Co2RecvBuf[2] - 0x30) * 10);
			if (Co2RecvBuf[3] >= 0x30)
				co2Data += ((Co2RecvBuf[3] - 0x30));

			uart1_wr = 0;
		}

		//错误处理

		Co2RecvCompleteFlag = FALSE;
		ES = 1; //开串口1中断
	}
}
void ModbusMain()
{
	uint8 OkFlag = FALSE;
	if (ModbusRecvCompleteFlag == TRUE)
	{

		IE2 &= 0xFE; //关串口2中断
		OkFlag = CheckModbusComm();
		IE2 |= 0x01; //开串口2中断
		ModbusRecvCompleteFlag = FALSE;
		if (OkFlag == TRUE) //收到有效485读取消息，准备发送Co2数据
		{

			SendCo2Data(co2Data);
		}
		else
		{
			uart2_wr = 0;
		}
	}
}

void Timer0Init(void) //1毫秒@22.1184MHz
{
	AUXR |= 0x80;	 //定时器时钟1T模式
	TMOD &= 0xF0;	 //设置定时器模式
	TL0 = TIMER_LOW;  //设置定时初值
	TH0 = TIMER_HIGH; //设置定时初值
	TF0 = 0;		  //清除TF0标志
	TR0 = 1;		  //定时器0开始计时
	ET0 = 1;		  //定时器0中断开启
}

//定时器0 1ms 中断
void timer0IntProc() interrupt 1
{
	TL0 = TIMER_LOW;
	TH0 = TIMER_HIGH;
	msFlag = TRUE;
}

void timerMain(void)
{
	if (msFlag == TRUE)
	{
		msFlag = FALSE;

		if (ModbusRecvFlag == TRUE)
		{
			ModbusTimeoutCnt--;
			if (ModbusTimeoutCnt == 0)
			{
				ModbusRecvCompleteFlag = TRUE;
				ModbusRecvFlag = FALSE;
				uart2_wr = 0;
			}
		}
		if (Co2RecvFlag == TRUE)
		{
			Co2TimeoutCnt--;
			if (Co2TimeoutCnt == 0)
			{

				Co2RecvCompleteFlag = TRUE;
				Co2RecvFlag = FALSE;
				uart1_wr = 0;
			}
		}

		if (LEDCount++ >= 10000) //LED 闪烁周期
		{
			LEDCount = 0;
			LEDBlink();
		}
	}
}

void InitMain(void)
{
	IPH = 0x10;
	IP = 0x12;

	InitADIO();
	Timer0Init();
	UartInit();
	WDT_CONTR = WDT;
}

void main()
{
	InitMain();

	while (1)
	{

		timerMain();
		Co2Main();
		ModbusMain();
	}
}
