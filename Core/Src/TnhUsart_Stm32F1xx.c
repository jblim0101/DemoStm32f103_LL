/*
 * TnhUsart_Stm32F1xx.c
 *
 *  Created on: 2019. 11. 9.
 *      Author: HCLIM
 */
#include "TnhUsart_Stm32F1xx.h"
//------------------------------------------------------------
//-------------TNH USART Basic Function-----------------------
//------------------------------------------------------------
uint8_t Usart_WriteByte (USART_TypeDef *USARTx, uint8_t byData)
{
	/*wait TXE*/
	while(!LL_USART_IsActiveFlag_TXE(USARTx));
	LL_USART_TransmitData8( USARTx, byData);
	return 0;
}

//------------------------------------------------------------
uint8_t Usart_WriteData (USART_TypeDef *USARTx, uint8_t *pData, uint8_t byDataLen)
{
	uint8_t *pDt;
	uint8_t byDtLen=byDataLen;
	pDt=pData;
	while (byDtLen)
	{
		/*wait TXE*/
		while(!LL_USART_IsActiveFlag_TXE(USARTx));
		LL_USART_TransmitData8( USARTx, *pDt++);
		byDtLen--;
	}
	return 0;
}

//------------------------------------------------------------
uint8_t Usart_ReadByte (USART_TypeDef *USARTx)
{
	/*wait TXE*/
	while(!LL_USART_IsActiveFlag_RXNE(USARTx));
	return (LL_USART_ReceiveData8( USARTx));
}
//-------------------------------------------------------------
void Usart_RxRcvd_MoveToBuf(USART_TypeDef *USARTx,_USART_BUF *pUsartBuf)
{
	//UART로 입력된 One Data를 Buffer로 이동하고 Buf의 포인트는 다음 Data를 저장할 위치를 지정하게 한다.
	//Buffer가 차게된면 초기 위치을 가리키게된다. -> Buffer의  Data 로테이션 처리

	pUsartBuf->byRxBuf[pUsartBuf->byRxBufPos]=LL_USART_ReceiveData8(USARTx);
	pUsartBuf->byRxBufPos++;
	if (pUsartBuf->byRxBufPos>=RX1BUF_SIZE) pUsartBuf->byRxBufPos=0;
}
//------------------------------------------------------------
uint8_t Usart_RxRcvd_Length(_USART_BUF *pUsartBuf)
{
	//UART 수신 버퍼에 저장된 Data 갯수를 리턴
	uint8_t by=0;
	if (pUsartBuf->byRxBufStPos <= pUsartBuf->byRxBufPos) {
		by=pUsartBuf->byRxBufPos-pUsartBuf->byRxBufStPos;
	} else {
		by=RX1BUF_SIZE-pUsartBuf->byRxBufStPos;
		by+=pUsartBuf->byRxBufPos;
	}
	return by;
}

//------------------------------------------------------------
uint8_t UsartMoveRxData( _USART_BUF *pUsartBuf, uint8_t *pDistBuf)
{
	//Buffer가 순환방식으로 처리 되며 수신 처리된 Data 갯수를 return
	uint8_t by;
	uint8_t byDataLen;
	/*Calcurate RX Data No*/
	byDataLen = Usart_RxRcvd_Length(pUsartBuf);
	by=byDataLen;
	if (by!=0) {
		while(by!=0) {
			*pDistBuf++=pUsartBuf->byRxBuf[pUsartBuf->byRxBufStPos];
			//Usart_WriteByte(USART3, pUsartBuf->byRxBuf[pUsartBuf->byRxBufStPos]);
			pUsartBuf->byRxBufStPos++;
			if (pUsartBuf->byRxBufStPos >= RX1BUF_SIZE) pUsartBuf->byRxBufStPos=0;
			by--;
		}
	}
	//수신 index를 초기화하여 순환을 최소화 함.
	pUsartBuf->byRxBufStPos=0;
	pUsartBuf->byRxBufPos=0;

	return byDataLen;
}

void UsartTxE_SendData(USART_TypeDef *USARTx, _USART_BUF *pUsartBuf)
{
	//TXE Interrupt Event 발생시 USART의 TX버퍼의 Next Data를 TX Data Regist로 전달
	//---Data 시작지정자를 확인하여 보낼 Data가 있는 지 확인한다.
	if (pUsartBuf->byTxBufStPos != pUsartBuf->byTxBufPos) {

		LL_USART_TransmitData8(USARTx, pUsartBuf->byTxBuf[pUsartBuf->byTxBufStPos++]);
		//---마지막 Data 처리이면 Data Buf의 position 지정자를 초기화하고 TXE 인터럽터 중지
		if (pUsartBuf->byTxBufStPos==pUsartBuf->byTxBufPos) {
			pUsartBuf->byTxBufStPos=0;
			pUsartBuf->byTxBufPos=0;
			LL_USART_DisableIT_TXE(USARTx);
		}
	}
}


//------------------------------------------------------------
char Usart_WriteDataByTXE(USART_TypeDef *USARTx, unsigned char *data, unsigned char len)
{
	//TX용 버퍼에 Data를 len만큼 추가 함
	//정상처리 일 때 0리턴
	unsigned char by;
	_USART_BUF *pUsartBuf;
//	if (USARTx==USART3) pUsartBuf=&StUart1;
//	else if (USARTx==USART2) pUsartBuf=&StUart2;
//	else if (USARTx==USART3) pUsartBuf=&StUart3;
#if USART_BUF_1
	if (USARTx==USART1) pUsartBuf=&StUart1;
#endif
#if USART_BUF_2
	if (USARTx==USART2) pUsartBuf=&StUart2;
#endif
#if USART_BUF_3
	if (USARTx==USART3) pUsartBuf=&StUart3;
#endif

	by=pUsartBuf->byTxBufPos+len;
	//Ver0.2---------------------------------------
	if (by>= TX1BUF_SIZE) {
		if (len<=TX1BUF_SIZE)
			while (1) {
				if (pUsartBuf->byTxBufPos==0) break;
			}
		else return -1;	//전송Data 초과오류
	}
#if INTTX_AFTER_EMPTY
	while (1) {
		if (pUsartBuf->byTxBufPos==pUsartBuf->byTxBufStPos) break;
	}
#endif
	//------------------------------------------------
	for (by=0;by<len;by++) {
		pUsartBuf->byTxBuf[pUsartBuf->byTxBufPos++]=data[by];
		if (pUsartBuf->byTxBufPos>=TX1BUF_SIZE) pUsartBuf->byTxBufPos=0;
	}
	LL_USART_EnableIT_TXE(USARTx);
	return 0;
}

/*
void USART3_RxRcvd_MoveToBuf(void)
{
	StUart3.byRxBuf[StUart3.byRxBufPos]=LL_USART_ReceiveData8(USART3);
	StUart3.byRxBufPos++;
	if (StUart3.byRxBufPos>=RX1BUF_SIZE) StUart3.byRxBufPos=0;
}
*/
//------------------------------------------------------------
//-------------TNH USART Protocol Function--------------------
//------------------------------------------------------------
void SmartMsgToPc(USART_TypeDef *USARTx, uint8_t cmd, uint8_t dt0, uint8_t dt1)
{
	uint8_t cbuf[6];
	uint8_t by, bysum=0;
	//LED_USART_TX_ON;	//SMART CMD TX LED ON
	cbuf[0]=0x02;	//STX
	cbuf[1]=cmd;	//TnhFrame CMD
	cbuf[2]=dt0;	//TnhFrame DATA0
	cbuf[3]=dt1;	//TnhFrame DATA0
	//---------------ChecSum Start------------------------------
	bysum=0;
	for (by=0;by<4;by++) {
		bysum=cbuf[by];	//CMD
	}
	cbuf[4]=bysum;
	//---------------ChecSum EndD------------------------------
	cbuf[5]=0x03;	//ETX
	Usart_WriteData(USARTx,cbuf,6);
}
void SmartU16MsgToPc(USART_TypeDef *USARTx, uint8_t byCmd, uint16_t iValue)
{
	uint16_t i=iValue;
	uint8_t buf[7],cRc=0;

	buf[0]=0x02;
	buf[1]=byCmd;	//Axis
	buf[2]=i&0xFF;
	i>>=8;
	buf[3]=i&0xFF;

	for(i=0;i<4;i++)
		cRc+=buf[i];
	buf[4]=cRc;
	buf[5]=0x03;
	Usart_WriteData(USARTx,buf,6);
}

//------------------------------------------------------------
void SmartTnhDataToPc(USART_TypeDef *USARTx, uint8_t cmd, uint8_t *pData, uint8_t byDataLen)
{
	uint8_t cbuf[36];
	uint8_t by,byad,bylen, bysum=0;
	//LED_USART_TX_ON;	//SMART CMD TX LED ON
	cbuf[0]=0x02;	//STX
	cbuf[1]=byDataLen+1;	//Frame Length
	cbuf[2]=cmd;	//TnhFrame CMD
	bylen=byDataLen;	//Data Length
	byad=3;
	for (by=0;by<bylen;by++) {
		cbuf[byad++]=*pData++;	//CMD
	}
	//---------------ChecSum------------------------------
	bylen+=3;	//STX-LEN-CMD-DATA , Data Length+3
	for (by=0;by<bylen;by++)
		bysum+=cbuf[by];
	cbuf[byad++]=bysum;	//CheckSum
	cbuf[byad++]=0x03;	//ETX
	Usart_WriteData(USARTx,cbuf,byad);

}
