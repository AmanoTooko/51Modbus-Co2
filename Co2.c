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

		ES = FALSE;													  //关串口1中断
		if (memcmp(Co2RecvBuf[4], co2suffix, sizeof(co2suffix)) == 0) //后六位为有效数据，前4位可以读取
		{
			co2Data = 0;
			co2Data += ((Co2RecvBuf[0] - 0x30) * 1000);
			co2Data += ((Co2RecvBuf[1] - 0x30) * 100);
			co2Data += ((Co2RecvBuf[2] - 0x30) * 10);
			co2Data += ((Co2RecvBuf[3] - 0x30));
		}
		else
		{
			//错误处理
		}
		ES = TRUE; //开串口1中断
	}
}

void ModbusMain()
{
	uint8 OkFlag = FALSE;
	if (ModbusRecvCompleteFlag == TRUE)
	{

		IE2 &= 0xFE; //关串口2中断
		OkFlag = CheckModbusComm();
		IE2 |= 0x01;		//开串口2中断
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
	ET = 1;
}

//定时器0 1ms 中断
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
