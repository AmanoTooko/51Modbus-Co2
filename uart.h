#include "common.h"
#define CO2_RECV_LEN 10
#define MODBUS_RECV_LEN 8
#define TI2 (S2CON & 0x02) != 0
#define RI2 (S2CON & 0x01) != 0
#define CLR_TI2() S2CON &= ~0x02
#define CLR_RI2() S2CON &= ~0x01
extern uint8 modbusSendBuf[MODBUS_RECV_LEN], modbusRecvBuf[MODBUS_RECV_LEN], Co2RecvBuf[CO2_RECV_LEN]; //发送接收缓冲区
extern bit ModbusRecvFlag, Co2RecvFlag;
extern void PrintString2(unsigned char *puts,uint8 length);
extern uint8 uart1_wr; //写指针
extern uint8 uart1_rd; //读指针
extern uint8 uart2_wr; //写指针
extern uint8 uart2_rd; //读指针
extern uint8 Co2TimeoutCnt, ModbusTimeoutCnt;
extern void UartInit(void);
extern void UART2_TxByte(unsigned char dat);
extern void PrintString1(unsigned char code *puts,uint8 length);