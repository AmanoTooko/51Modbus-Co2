/*****************************************************************************
Copyright:https://github.com/AmanoTooko/
File name: uart.c
Description: 初始化串口1和串口2，提供串口的收发函数。
串口1 P3.0 P3.1 38400bps 二氧化碳传感器 每3秒接收10个字节。
串口2 P1.2 P1.3 9600bps 485接口 不定期收发
Author: Y.Morino
Version: 1
Date: 2019/11/28
*****************************************************************************/
#include "uart.h"
#include "IO.h"
bit B_TI, B_TI2;
uint8 modbusSendBuf[MODBUS_RECV_LEN];
uint8 modbusRecvBuf[MODBUS_RECV_LEN];
uint8 Co2RecvBuf[CO2_RECV_LEN]; //发送接收缓冲区
bit ModbusRecvFlag, Co2RecvFlag;
uint8 uart1_wr; //写指针
uint8 uart1_rd; //读指针
uint8 uart2_wr; //写指针
uint8 uart2_rd; //读指针
uint8 Co2TimeoutCnt, ModbusTimeoutCnt;

void Uart2Init(void) //9600bps@22.1184MHz
{
	AUXR &= 0xF7; //波特率不倍速
	S2CON = 0x50; //8位数据,可变波特率
	AUXR |= 0x04; //独立波特率发生器时钟为Fosc,即1T
	BRT = 0xB8;   //设定独立波特率发生器重装值
	AUXR |= 0x10; //启动独立波特率发生器
}

void Uart1Init(void) //38400bps@22.1184MHz
{
	PCON &= 0x7F; //波特率不倍速
	SCON = 0x50;  //8位数据,可变波特率
	AUXR |= 0x40; //定时器1时钟为Fosc,即1T
	AUXR &= 0xFE; //串口1选择定时器1为波特率发生器
	TMOD &= 0x0F; //清除定时器1模式位
	TMOD |= 0x20; //设定定时器1为8位自动重装方式
	TL1 = 0xEE;   //设定定时初值
	TH1 = 0xEE;   //设定定时器重装值
	ET1 = 0;	  //禁止定时器1中断
	TR1 = 1;	  //启动定时器1
}

void UartInit(void)
{
	Uart1Init();
	Uart2Init();

	EA = 1;   //串口1中断
	IE2 |= 1; //允许串口2中断
	ES = 1;   //全局中断
}

void UART1_TxByte(unsigned char dat)
{
	B_TI = 0;
	SBUF = dat;
	while (!B_TI)
		;
	B_TI = 0;
}
void UART2_TxByte(unsigned char dat)
{
	B_TI2 = 0;
	S2BUF = dat;
	while (!B_TI2)
		;
	B_TI2 = 0;
}

void PrintString1(unsigned char code *puts, uint8 length) //发送一串字符串
{
	uint8 index = 0;

	for (index = 0; index < length; index++)
		UART1_TxByte(puts[index]);
}

void PrintString2(unsigned char *puts, uint8 length) //发送一串字符串
{
	uint8 index = 0;
	Start485Send();
	for (index = 0; index < length; index++)
		UART2_TxByte(puts[index]);
	Stop485Send();
}

void UART0_RCV(void) interrupt 4
{
	if (RI)
	{
		RI = 0;
		Co2RecvBuf[uart1_wr] = SBUF;
		Co2RecvFlag = TRUE;
		Co2TimeoutCnt = 10;

		if (++uart1_wr >= CO2_RECV_LEN)
		{
			uart1_wr = 0;
		}
	}

	if (TI)
	{
		TI = 0;
		B_TI = 1;
	}
}

/**********************************************/
void UART2_RCV(void) interrupt 8
{
	if (RI2)
	{
		CLR_RI2();
		modbusRecvBuf[uart2_wr] = S2BUF;
		ModbusRecvFlag = TRUE;
		ModbusTimeoutCnt = 10;
		if (++uart2_wr >= MODBUS_RECV_LEN)
		{

			uart2_wr = 0;
		}
	}

	if (TI2)
	{
		CLR_TI2();
		B_TI2 = 1;
	}
}
