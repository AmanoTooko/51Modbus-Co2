/*****************************************************************************
Copyright:https://github.com/AmanoTooko/
File name: uart.c
Description: ��ʼ������1�ʹ���2���ṩ���ڵ��շ�������
����1 P3.0 P3.1 38400bps ������̼������ ÿ3�����10���ֽڡ�
����2 P1.2 P1.3 9600bps 485�ӿ� �������շ�
Author: Y.Morino
Version: 1
Date: 2019/11/28
*****************************************************************************/
#include "uart.h"
#include "IO.h"
bit B_TI, B_TI2;
uint8 modbusSendBuf[MODBUS_RECV_LEN];
uint8 modbusRecvBuf[MODBUS_RECV_LEN];
uint8 Co2RecvBuf[CO2_RECV_LEN]; //���ͽ��ջ�����
bit ModbusRecvFlag, Co2RecvFlag;
uint8 uart1_wr; //дָ��
uint8 uart1_rd; //��ָ��
uint8 uart2_wr; //дָ��
uint8 uart2_rd; //��ָ��
uint8 Co2TimeoutCnt, ModbusTimeoutCnt;

void Uart2Init(void) //9600bps@22.1184MHz
{
	AUXR &= 0xF7; //�����ʲ�����
	S2CON = 0x50; //8λ����,�ɱ䲨����
	AUXR |= 0x04; //���������ʷ�����ʱ��ΪFosc,��1T
	BRT = 0xB8;   //�趨���������ʷ�������װֵ
	AUXR |= 0x10; //�������������ʷ�����
}

void Uart1Init(void) //38400bps@22.1184MHz
{
	PCON &= 0x7F; //�����ʲ�����
	SCON = 0x50;  //8λ����,�ɱ䲨����
	AUXR |= 0x40; //��ʱ��1ʱ��ΪFosc,��1T
	AUXR &= 0xFE; //����1ѡ��ʱ��1Ϊ�����ʷ�����
	TMOD &= 0x0F; //�����ʱ��1ģʽλ
	TMOD |= 0x20; //�趨��ʱ��1Ϊ8λ�Զ���װ��ʽ
	TL1 = 0xEE;   //�趨��ʱ��ֵ
	TH1 = 0xEE;   //�趨��ʱ����װֵ
	ET1 = 0;	  //��ֹ��ʱ��1�ж�
	TR1 = 1;	  //������ʱ��1
}

void UartInit(void)
{
	Uart1Init();
	Uart2Init();

	EA = 1;   //����1�ж�
	IE2 |= 1; //������2�ж�
	ES = 1;   //ȫ���ж�
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

void PrintString1(unsigned char code *puts, uint8 length) //����һ���ַ���
{
	uint8 index = 0;

	for (index = 0; index < length; index++)
		UART1_TxByte(puts[index]);
}

void PrintString2(unsigned char *puts, uint8 length) //����һ���ַ���
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
