/*
 * TnhUsart_Stm32F1xx.h
 *
 *  Created on: 2019. 11. 9.
 *      Author: HCLIM
 */

#ifndef INC_TNHUSART_STM32F1XX_H_
#define INC_TNHUSART_STM32F1XX_H_

#include "stm32f1xx_ll_usart.h"
#define USART_BUF_1 1
#define USART_BUF_2 1
#define USART_BUF_3 1
//-------------------------------------------------------------------
//#define RX1FRAME_SIZE 135	//TNH CMD+LEN_ADDR+ADDR(4) + LEN_DATA + DATA(32) = 39
#define RX1BUF_SIZE 256  //139		//STX + LEN + I2C FRAME(39) + CSUM + ETX = 43
#define TX1BUF_SIZE 256  //133		//STX + LEN + TNH_CMD + I2C DATA(32) + CSUM + ETX = 37
#define INTTX_AFTER_EMPTY 1 //TXEIE를 이용한 손신의 경우 이전 프레임을 송신완료 후 처리 설정  -  Usart_WriteDataByInt


typedef struct {
	unsigned char byRxBufStPos;		//Data Start Position 0~RX1BUF_SIZE-1
	unsigned char byRxBufPos;		// Equal to Data Length and Current Position in RxBuf to receive
	unsigned char byRxBuf[RX1BUF_SIZE];
	unsigned char byRxFrameSize;
	//unsigned char byRxFrame[RX1FRAME_SIZE];
	unsigned char byTxBufStPos;		//Data Start Position 0~TX1BUF_SIZE-1
	unsigned char byTxBufPos;		// Equal to Data Length
	unsigned char byTxBuf[TX1BUF_SIZE];
} _USART_BUF;
//-------------------------------------------------------------------
#if USART_BUF_1
_USART_BUF StUart1;
#endif
#if USART_BUF_2
_USART_BUF StUart2;
#endif
#if USART_BUF_3
_USART_BUF StUart3;
#endif
uint8_t FlagUsartStat;
//-------------------------------------------------------------------
uint8_t Usart_WriteByte (USART_TypeDef *USARTx, uint8_t byData);
uint8_t Usart_WriteData (USART_TypeDef *USARTx, uint8_t *pData, uint8_t byDataLen);
uint8_t Usart_ReadByte (USART_TypeDef *USARTx);

void Usart_RxRcvd_MoveToBuf(USART_TypeDef *USARTx, _USART_BUF *pStUsartBuf);
uint8_t Usart_RxRcvd_Length(_USART_BUF *pUsartBuf);

uint8_t UsartMoveRxData( _USART_BUF *pUsartBuf, uint8_t *pDistBuf);

//void USART3_RxRcvd_MoveToBuf(void);

//Data를 정해진 길이만큼 해당 Usart의 Tx Data BUFFER로 이동 시키고 TXE interrupt 활성화
char Usart_WriteDataByTXE(USART_TypeDef *USARTx, uint8_t *data,uint8_t len);
char Usart_WriteBufByTXE(USART_TypeDef *USARTx);	//USART_BUF의 byTxBuf[]를 TXE interrupt를 이용하여 전송

//TXE Interrupt Event 발생시 USART의 TX버퍼의 Next Data를 TX Data Register로 전달
void UsartTxE_SendData(USART_TypeDef *USARTx, _USART_BUF *pUsartBuf);

//-------------------------------------------------------------------

void SmartMsgToPc(USART_TypeDef *USARTx, uint8_t cmd, uint8_t dt0, uint8_t dt1);
void SmartU16MsgToPc(USART_TypeDef *USARTx, uint8_t cmd, uint16_t iValue);

void SmartTnhDataToPc(USART_TypeDef *USARTx, uint8_t cmd, uint8_t *pData, uint8_t byDataLen);
//-------------------------------------------------------------------

#endif /* INC_TNHUSART_STM32F1XX_H_ */
