#include "stm32f1xx_ll_i2c.h"
#define I2C32_MODE 1

#ifndef _TNH_I2C_STM32F1XX_H
#define _TNH_I2C_STM32F1XX_H
#ifdef __cplusplus
 extern "C" {
#endif 

 /* Includes ------------------------------------------------------------------*/
 #include "stm32f1xx_hal_def.h"

//--------------Read byte from register--------------------------
uint8_t I2c_WaitOnSB_UntilTimeout(I2C_TypeDef *I2Cx, uint32_t Timeout);
uint8_t I2c_WaitOnClearBUSY_UntilTimeout(I2C_TypeDef *I2Cx, uint32_t Timeout);
uint8_t I2c_WaitOnADDR_UntilTimeout(I2C_TypeDef *I2Cx, uint32_t Timeout);
uint8_t I2c_WaitOnTXE_UntilTimeout(I2C_TypeDef *I2Cx, uint32_t Timeout);
uint8_t I2c_WaitOnBTF_UntilTimeout(I2C_TypeDef *I2Cx, uint32_t Timeout);
uint8_t I2c_WaitOnRXNE_UntilTimeout(I2C_TypeDef *I2Cx, uint32_t Timeout);

//--------------Read byte from register--------------------------
uint8_t I2c_ReadByte( I2C_TypeDef *I2Cx );
//--------------Write byte to register
uint8_t I2c_WriteByte( I2C_TypeDef *I2Cx , uint8_t data_out );
uint8_t I2c_ReadAck( I2C_TypeDef *I2Cx );

//--------------Read in a string from I2C module
uint8_t I2c32_RequestWrite( I2C_TypeDef *I2Cx, uint8_t DevAddr, uint32_t SubAddr, uint8_t SubAddrLen);
uint8_t I2c_WriteData( I2C_TypeDef *I2Cx, uint8_t *pData, uint8_t byDtlen );
uint8_t I2c32_RequestRead( I2C_TypeDef *I2Cx, uint8_t DevAddr, uint32_t SubAddr, uint8_t SubAddrLen);
uint8_t I2c_ReadData( I2C_TypeDef *I2Cx, uint8_t *rdptr, uint8_t byDtlen );
//-------------- I2C Main Ffunction--------------------------
uint8_t TNH_I2C_Mem_Write ( I2C_TypeDef *I2Cx, uint8_t DevAddr, uint32_t SubAddr, uint8_t SubAddrLen, uint8_t *pDt, uint8_t byDtLen, uint16_t timeout );
uint8_t TNH_I2C_Mem_Read ( I2C_TypeDef *I2Cx, uint8_t DevAddr, uint32_t SubAddr, uint8_t SubAddrLen, uint8_t *pDt, uint8_t byDtLen, uint16_t timeout );

//---------------------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif

#endif /* _TNH_I2C_STM32F1XX_H */

