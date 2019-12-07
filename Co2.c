#define TIMER_LOW 0x9A
#define TIMER_HIGH 0xA9

#include <string.h>
#include "modbus.h"
#include "uart.h"
#include "IO.h"

bit ModbusRecvCompleteFlag, Co2RecvCompleteFlag, msFlag;
uint16 co2Data = 0;
uint8 co2suffix[6] = {0x20, 0x70, 0x70, 0x6d, 0x0d, 0x0a};

void Co2Main()
{

	if (Co2RecvCompleteFlag == TRUE)
	{

		ES = FALSE;													  //�ش���1�ж�
		if (memcmp(Co2RecvBuf[4], co2suffix, sizeof(co2suffix)) == 0) //����λΪ��Ч���ݣ�ǰ4λ���Զ�ȡ
		{
			co2Data = 0;
			co2Data += ((Co2RecvBuf[0] - 0x30) * 1000);
			co2Data += ((Co2RecvBuf[1] - 0x30) * 100);
			co2Data += ((Co2RecvBuf[2] - 0x30) * 10);
			co2Data += ((Co2RecvBuf[3] - 0x30));
		}
		else
		{
			//������
		}
		ES = TRUE; //������1�ж�
	}
}

void ModbusMain()
{
	uint8 OkFlag = FALSE;
	if (ModbusRecvCompleteFlag == TRUE)
	{

		IE2 &= 0xFE; //�ش���2�ж�
		OkFlag = CheckModbusComm();
		IE2 |= 0x01;		//������2�ж�
		if (OkFlag == TRUE) //�յ���Ч485��ȡ��Ϣ��׼������Co2����
		{
			SendCo2Data(co2Data);
		}
		else
		{
			uart2_wr = 0;
		}
	}
}

void Timer0Init(void) //1����@22.1184MHz
{
	AUXR |= 0x80;	 //��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;	 //���ö�ʱ��ģʽ
	TL0 = TIMER_LOW;  //���ö�ʱ��ֵ
	TH0 = TIMER_HIGH; //���ö�ʱ��ֵ
	TF0 = 0;		  //���TF0��־
	TR0 = 1;		  //��ʱ��0��ʼ��ʱ
	ET = 1;
}

//��ʱ��0 1ms �ж�
void timer0IntProc() interrupt 1
{
	TL0 = TIMER_LOW;
	TH0 = TIMER_HIGHT;
	msFlag = TRUE;
}

void timerMain(void)
{
	if (msFlag == TRUE)
	{
		if (ModbusRecvFlag == TRUE)
		{
			ModbusTimeoutCnt--;
			if (ModbusTimeoutCnt == 0)
			{
				ModbusRecvCompleteFlag = TRUE;
				uart2_wr = 0;
			}
		}
		if (Co2RecvFlag == TRUE)
		{
			Co2TimeoutCnt--;
			if (Co2TimeoutCnt == 0)
			{
				Co2RecvCompleteFlag = TRUE;
				uart1_wr = 0;
			}
		}
	}
}
void InitMain(void)
{
	InitADIO();
	Timer0Init();
	UartInit();
}

void main(void)
{
	InitMain();

	while (1)
	{
		timerMain();
		Co2Main();
		ModbusMain();
	}
}
