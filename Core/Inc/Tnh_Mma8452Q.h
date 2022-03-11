/*
 * Tnh_Mma8452Q.h
 *
 *  Created on: 2019. 11. 6.
 *      Author: HCLIM
 */

#ifndef INC_TNH_MMA8452Q_H_
#define INC_TNH_MMA8452Q_H_
//#include "main.h"
#include "TnhI2c_Stm32F1xx.h"

#define MMA8452Q_ADDR_WR 0x38
#define MMA8452Q_ADDR_RD 0x39
#define MAX2G_8452 0
#define MAX4G_8452 1
#define MAX8G_8452 2


unsigned char Mma8452Standby(void);
unsigned char Mma8452Active(void);
void Mma8452_InitForAccMode(unsigned char byMaxRange);
void Sensor8452_ReadAcc(uint16_t *x,uint16_t *y, uint16_t *z);

#endif /* INC_TNH_MMA8452Q_H_ */
