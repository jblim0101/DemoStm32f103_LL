// ==================================================================
//programed by TNH
//----------------------------------------------------------------------------------
//Rev 0.1 Start :2019.11.09
//using Low Level I2C for STM32F10x
//-----------------------------------------------------------------------------------
//===================================================================
#include "TnhI2c_Stm32F1xx.h"

//---------------------------------------------------------------------------------------------------------------
uint8_t I2c_WaitOnSB_UntilTimeout(I2C_TypeDef *I2Cx, uint32_t Timeout)
{	/* Wait until SB flag is set */
	uint32_t tickstart = HAL_GetTick();
	while (!LL_I2C_IsActiveFlag_SB(I2Cx)) {	//RESET equal to 0
		if ((HAL_GetTick()-tickstart)>=Timeout)
			return HAL_TIMEOUT;
	}
	return HAL_OK;
}
//---------------------------------------------------------------------------------------------------------------
uint8_t I2c_WaitOnClearBUSY_UntilTimeout(I2C_TypeDef *I2Cx, uint32_t Timeout)
{
	uint32_t tickstart = HAL_GetTick();
	while (LL_I2C_IsActiveFlag_BUSY(I2Cx)) {	//RESET equal to 0
		if ((HAL_GetTick()-tickstart)>=Timeout)
			return HAL_TIMEOUT;
	}
	return HAL_OK;

}
//---------------------------------------------------------------------------------------------------------------
uint8_t I2c_WaitOnADDR_UntilTimeout(I2C_TypeDef *I2Cx, uint32_t Timeout)
{
	uint32_t tickstart = HAL_GetTick();
	while (!LL_I2C_IsActiveFlag_ADDR(I2Cx)) {	//RESET equal to 0
		if ((HAL_GetTick()-tickstart)>=Timeout)
			return HAL_TIMEOUT;
	}
	return HAL_OK;

}
//---------------------------------------------------------------------------------------------------------------
uint8_t I2c_WaitOnTXE_UntilTimeout(I2C_TypeDef *I2Cx, uint32_t Timeout)
{
	uint32_t tickstart = HAL_GetTick();
	while (!LL_I2C_IsActiveFlag_TXE(I2Cx)) {	//RESET equal to 0
		if ((HAL_GetTick()-tickstart)>=Timeout)
			return HAL_TIMEOUT;
	}
	return HAL_OK;
}

//---------------------------------------------------------------------------------------------------------------
uint8_t I2c_WaitOnBTF_UntilTimeout(I2C_TypeDef *I2Cx, uint32_t Timeout)
{
	uint32_t tickstart = HAL_GetTick();
	while (!LL_I2C_IsActiveFlag_BTF(I2Cx)) {	//RESET equal to 0
		if ((HAL_GetTick()-tickstart)>=Timeout)
			return HAL_TIMEOUT;
	}
	return HAL_OK;
}
//---------------------------------------------------------------------------------------------------------------
uint8_t I2c_WaitOnRXNE_UntilTimeout(I2C_TypeDef *I2Cx, uint32_t Timeout )
{
	uint32_t tickstart = HAL_GetTick();
	while (!LL_I2C_IsActiveFlag_RXNE(I2Cx)) {	//RESET equal to 0
		if ((HAL_GetTick()-tickstart)>=Timeout)
			return HAL_TIMEOUT;
	}
	return HAL_OK;
}

//---------------------------------------------------------------------------------------------------------------
uint8_t I2c_ReadByte( I2C_TypeDef *I2Cx )
{	//I2C RXNE -> Data not empty = set if there is a received data 
	return ( LL_I2C_ReceiveData8(I2Cx) );              // return with read byte
}


//---------------------------------------------------------------------------------------------------------------
uint8_t I2c_WriteByte( I2C_TypeDef *I2Cx , uint8_t data_out )
{
	LL_I2C_TransmitData8(I2Cx, data_out	);
	while( LL_I2C_IsActiveFlag_BTF(I2Cx) );   // wait until write cycle is complete
	return 0;
}
//---------------------------------------------------------------------------------------------------------------
uint8_t I2c_ReadAck( I2C_TypeDef *I2Cx )
{
	if ( LL_I2C_IsActiveFlag_AF(I2Cx) ) // test for ACK condition received
		return 1;
	else return 0;              // if WCOL bit is not set return non-negative #

}

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
uint8_t I2c32_RequestWrite( I2C_TypeDef *I2Cx, uint8_t DevAddr, uint32_t SubAddr, uint8_t SubAddrLen)
{	//Start Bit부터  DevAddress- SubAddress 전송까지 지냉
	uint8_t bySubAddrLen=SubAddrLen;
	uint32_t u32SubAddr=SubAddr;
	uint8_t bySubAddr[4];
	//---------Sub Addr Process------
	bySubAddr[0]=(uint8_t)(u32SubAddr & 0xFF);
	u32SubAddr>>=8;
	bySubAddr[1]=(uint8_t)(u32SubAddr & 0xFF);
	u32SubAddr>>=8;
	bySubAddr[2]=(uint8_t)(u32SubAddr & 0xFF);
	u32SubAddr>>=8;
	bySubAddr[3]=(uint8_t)(u32SubAddr & 0x0FF);
	//--------------------------------
	/* Generate Start */
	SET_BIT(I2Cx->CR1, I2C_CR1_START);

	/* Wait until SB flag is set */
	if (I2c_WaitOnSB_UntilTimeout(I2Cx,5))
		return HAL_TIMEOUT;

	/* Send Dev(slave) address */
	LL_I2C_TransmitData8(I2Cx ,(DevAddr & 0xFE));	//send address cleared bit0

	/* Wait until ADDR flag is set */
	if (I2c_WaitOnADDR_UntilTimeout(I2Cx,5))
		return HAL_TIMEOUT;
	//while (! LL_I2C_IsActiveFlag_ADDR(I2Cx) );

	/* Clear ADDR flag */
	LL_I2C_ClearFlag_ADDR(I2Cx);

	/* Send Sub address */
	while (bySubAddrLen--) {
		/* Wait until TXE flag is set */
		if (I2c_WaitOnTXE_UntilTimeout(I2Cx,5)) return HAL_TIMEOUT;
		/* Send Sub(Memory) address */
		LL_I2C_TransmitData8(I2Cx ,bySubAddr[bySubAddrLen]);	//send address cleared bit0
	}
    return 0;
}
//---------------------------------------------------------------------------------------------------------------

uint8_t I2c_WriteData( I2C_TypeDef *I2Cx, uint8_t *pData, uint8_t byDtLen )
{
	uint8_t Length=byDtLen;

	while ( Length )           // perform getcI2C() for 'length' number of bytes
	{
		/* Wait until TXE flag is set */
		if (I2c_WaitOnTXE_UntilTimeout(I2Cx,5)) return HAL_TIMEOUT;
        /* Write data to DR */
		LL_I2C_TransmitData8(I2Cx ,*pData++);	//send Data0
		Length--;
		if (LL_I2C_IsActiveFlag_BTF(I2Cx) && (Length != 0U))
		{
	        /* Write data to DR */
			LL_I2C_TransmitData8(I2Cx ,*pData++);	//send Data0
			Length--;
		}
	}
    /* Wait until BTF flag is set */
	if (I2c_WaitOnBTF_UntilTimeout(I2Cx,5)) return HAL_TIMEOUT;

    /* Generate Stop */
    SET_BIT(I2Cx->CR1, I2C_CR1_STOP);
    return 0;
}
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

uint8_t I2c32_RequestRead( I2C_TypeDef *I2Cx, uint8_t DevAddr, uint32_t SubAddr, uint8_t SubAddrLen)
{	//Start Bit부터  DevAddress- SubAddress 전송까지 지냉

	uint8_t bySubAddrLen=SubAddrLen;
	uint32_t u32SubAddr=SubAddr;
	uint8_t bySubAddr[4];
	//---------Sub Addr Process------
	bySubAddr[0]=(uint8_t)(u32SubAddr & 0xFF);
	u32SubAddr>>=8;
	bySubAddr[1]=(uint8_t)(u32SubAddr & 0xFF);
	u32SubAddr>>=8;
	bySubAddr[2]=(uint8_t)(u32SubAddr & 0xFF);
	u32SubAddr>>=8;
	bySubAddr[3]=(uint8_t)(u32SubAddr & 0x0FF);
	//--------------------------------
	/* Enable Acknowledge */
	SET_BIT(I2Cx->CR1, I2C_CR1_ACK);

	/* Generate Start */
	SET_BIT(I2Cx->CR1, I2C_CR1_START);

	/* Wait until SB flag is set */
	if (I2c_WaitOnSB_UntilTimeout(I2Cx, 5))
		return HAL_TIMEOUT;
	//while( !LL_I2C_IsActiveFlag_SB(I2Cx) );

	/* Send Dev(slave) address */
	LL_I2C_TransmitData8(I2Cx ,(DevAddr & 0xFE));	//send address cleared bit0

	/* Wait until ADDR flag is set */
	if (I2c_WaitOnADDR_UntilTimeout(I2Cx,5))
		return HAL_TIMEOUT;

	/* Clear ADDR flag */
	LL_I2C_ClearFlag_ADDR(I2Cx);


	/* Send Sub address */
	while (bySubAddrLen--) {
		/* Wait until TXE flag is set */
		if (I2c_WaitOnTXE_UntilTimeout(I2Cx,5))
			return HAL_TIMEOUT;
		/* Send Sub(Memory) address */
		LL_I2C_TransmitData8(I2Cx ,bySubAddr[bySubAddrLen]);	//send address cleared bit0
	}
	//---------ReStart & DevAddr for Read
	/* Wait until TXE flag is set */
	if (I2c_WaitOnTXE_UntilTimeout(I2Cx,5)) return HAL_TIMEOUT;

	/* Generate Restart */
	SET_BIT(I2Cx->CR1, I2C_CR1_START);

	/* Wait until SB flag is set */
	if (I2c_WaitOnSB_UntilTimeout(I2Cx, 5))
		return HAL_TIMEOUT;

	/* Send Dev(slave) address to Read */
	LL_I2C_TransmitData8(I2Cx ,(DevAddr | 0x01));	//send address cleared bit0

	/* Wait until ADDR flag is set */
	if (I2c_WaitOnADDR_UntilTimeout(I2Cx, 5))
		return HAL_TIMEOUT;
	return HAL_OK;
}

//---------------------------------------------------------------------------------------------------------------
uint8_t I2c_ReadData( I2C_TypeDef *I2Cx, uint8_t *pDt, uint8_t byDtLen )
{

	uint8_t Length = byDtLen;
	if (Length==0) {
		/* Clear ADDR flag */
		LL_I2C_ClearFlag_ADDR(I2Cx);

		/* Generate Stop */
		SET_BIT(I2Cx->CR1, I2C_CR1_STOP);

	} else if (Length==1) {
		/* Disable Acknowledge */
		CLEAR_BIT(I2Cx->CR1, I2C_CR1_ACK);
		/* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
		      software sequence must complete before the current byte end of transfer */
		/* Disable IRQs */
	    __disable_irq();
		/* Clear ADDR flag */
		LL_I2C_ClearFlag_ADDR(I2Cx);

	    /* Generate Stop */
	    SET_BIT(I2Cx->CR1, I2C_CR1_STOP);
		/* Re-enable IRQs */
		__enable_irq();
	} else if (Length==2) {
		/* Enable Pos */
		SET_BIT(I2Cx->CR1, I2C_CR1_POS);

		/* Disable IRQs */
	     __disable_irq();
		/* Clear ADDR flag */
		LL_I2C_ClearFlag_ADDR(I2Cx);

		/* Disable Acknowledge */
	     CLEAR_BIT(I2Cx->CR1, I2C_CR1_ACK);
		/* Re-enable IRQs */
		__enable_irq();
	} else {
		/* Enable Acknowledge */
		SET_BIT(I2Cx->CR1, I2C_CR1_ACK);
		
		/* Clear ADDR flag */
		LL_I2C_ClearFlag_ADDR(I2Cx);
	}
	
	while ( Length )           // perform getcI2C() for 'length' number of bytes
	{
		if (Length <= 3)
		{
			/* One byte */
			if (Length==1) {
		         /* Wait until RXNE flag is set */
				if (I2c_WaitOnRXNE_UntilTimeout(I2Cx,5)) return HAL_TIMEOUT;
				/* Read data from DR */
				*pDt++=LL_I2C_ReceiveData8(I2Cx);
				Length--;
			}
			/* Two byte */
			else if (Length==2)
			{
				//------DATA N-1--------------
				/* Wait until BTF flag is set */
				if (I2c_WaitOnBTF_UntilTimeout(I2Cx,5)) return HAL_TIMEOUT;

				/* Disable IRQs */
			     __disable_irq();

				/* Generate Stop */
				SET_BIT(I2Cx->CR1, I2C_CR1_STOP);

				/* Read data from DR */
				*pDt++=LL_I2C_ReceiveData8(I2Cx);
				Length--;

				/* Re-enable IRQs */
				__enable_irq();

				//------DATA N--------------
				/* Read data from DR */
				*pDt++=LL_I2C_ReceiveData8(I2Cx);
				Length--;

			}
			/* Tree byte */
			else if (Length==3)
			{
				//------DATA N-2--------------
				/* Wait until BTF flag is set */
				if (I2c_WaitOnBTF_UntilTimeout(I2Cx,5)) return HAL_TIMEOUT;

				/* Disable Acknowledge */
				CLEAR_BIT(I2Cx->CR1, I2C_CR1_ACK);

				/* Disable IRQs */
			     __disable_irq();

				/* Read data from DR */
				*pDt++=LL_I2C_ReceiveData8(I2Cx);
				Length--;

				//------DATA N-1--------------
				/* Wait until BTF flag is set */
				if (I2c_WaitOnBTF_UntilTimeout(I2Cx,5)) return HAL_TIMEOUT;

				/* Generate Stop */
				SET_BIT(I2Cx->CR1, I2C_CR1_STOP);

				/* Read data from DR */
				*pDt++=LL_I2C_ReceiveData8(I2Cx);
				Length--;

				/* Re-enable IRQs */
				__enable_irq();

				//------DATA N--------------
				/* Read data from DR */
				*pDt++=LL_I2C_ReceiveData8(I2Cx);
				Length--;

			}
		}
		else
		{
	        /* Wait until RXNE flag is set */
			if (I2c_WaitOnRXNE_UntilTimeout(I2Cx,5)) return HAL_TIMEOUT;

			/* Read data from DR */
			*pDt++=LL_I2C_ReceiveData8(I2Cx);
			Length--;
			/* Wait until BTF flag is set */
			if (LL_I2C_IsActiveFlag_BTF(I2Cx))
			{
				/* Read data from DR */
				*pDt++=LL_I2C_ReceiveData8(I2Cx);
				Length--;

			}
		}
	}
	return HAL_OK;
}

//---------------------------------------------------------------------------------------------------------------
uint8_t TNH_I2C_Mem_Write ( I2C_TypeDef *I2Cx, uint8_t DevAddr, uint32_t SubAddr, uint8_t SubAddrLen, uint8_t *pDt, uint8_t byDtLen, uint16_t timeout)
{
    /* Wait until BUSY flag is reset */
	if (I2c_WaitOnClearBUSY_UntilTimeout(I2Cx,5))
		return HAL_TIMEOUT ;

	/* Check if the I2C is already enabled */
    if ((I2Cx->CR1 & I2C_CR1_PE) != I2C_CR1_PE) {
    	LL_I2C_Enable(I2Cx);
    }

    /* Disable Pos */
    CLEAR_BIT(I2Cx->CR1, I2C_CR1_POS);

    /* Send Slave Address and Memory Address */
    if (I2c32_RequestWrite(I2Cx, DevAddr, SubAddr, SubAddrLen) != 0)
    {
    	return 0x01;
    }
	I2c_WriteData(I2Cx, pDt, byDtLen);

	/* Wait until BTF flag is set */
	if (I2c_WaitOnBTF_UntilTimeout(I2Cx,5)) return HAL_TIMEOUT;
    return 0;

}

uint8_t TNH_I2C_Mem_Read ( I2C_TypeDef *I2Cx, uint8_t DevAddr, uint32_t SubAddr, uint8_t SubAddrLen, uint8_t *pDt, uint8_t byDtLen, uint16_t timeout)
{
    /* Wait until BUSY flag is reset */
	//while (LL_I2C_IsActiveFlag_BUSY(I2Cx)) ;
    /* Wait until BUSY flag is reset */
	if (I2c_WaitOnClearBUSY_UntilTimeout(I2Cx,25))
		return HAL_TIMEOUT ;

	/* Check if the I2C is already enabled */
    if ((I2Cx->CR1 & I2C_CR1_PE) != I2C_CR1_PE) {
    	LL_I2C_Enable(I2Cx);
    }

    /* Disable Pos */
    CLEAR_BIT(I2Cx->CR1, I2C_CR1_POS);

    /* Send Slave Address and Memory Address */
    if (I2c32_RequestRead(I2Cx, DevAddr, SubAddr, SubAddrLen) != 0)
    {
    	return 0x01;
    }
	I2c_ReadData(I2Cx, pDt, byDtLen);
    return 0;

}

