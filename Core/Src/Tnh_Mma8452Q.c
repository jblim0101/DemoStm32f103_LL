/*
 * Tnh_Mma8452Q.c
 *
 *  Created on: 2019. 11. 6.
 *      Author: HCLIM
 */
#include "Tnh_Mma8452Q.h"

unsigned char Mma8452Standby(void)
{
	unsigned char c;
	if  (TNH_I2C_Mem_Read(I2C1,MMA8452Q_ADDR_RD,0x2A,1,&c,1,1000))
		return 1;
	c &= 0xFE;
	if (TNH_I2C_Mem_Write(I2C1,MMA8452Q_ADDR_WR, 0x2A, 1, &c, 1, 1000))
		return 1;
	return 0;
}
unsigned char Mma8452Active(void)
{
	unsigned char c;
	if  (TNH_I2C_Mem_Read(I2C1,MMA8452Q_ADDR_RD,0x2A,1,&c,1,1000))
		return 1;
	c |= 0x01;
	if (TNH_I2C_Mem_Write(I2C1,MMA8452Q_ADDR_WR, 0x2A, 1, &c, 1, 1000))
		return 1;
	return 0;
}

void Mma8452_InitForAccMode(unsigned char byMaxRange)
{	//byMaxRange: 8452_MAX_2G, 8452_MAX_4G , 8452_MAX_2G
	unsigned char c[5];

	Mma8452Standby();  // Must be in standby to change registers

/*
	if (byMaxRange<3)
		I2cWriteByte(SENSOR_ADDR,0x0E,byMaxRange);	//Ver1.3 Default 4G ?��?�� //[b4]HPF,[b1:0]- 01:4g
*/
	c[0]=0;
	TNH_I2C_Mem_Write(I2C1, MMA8452Q_ADDR_WR, 0x21, 1, c, 1, 1000);	//X,Y,Z Single Pulse Detect
	TNH_I2C_Mem_Write(I2C1, MMA8452Q_ADDR_WR, 0x23, 1, c, 1, 1000);	//X Pulse Threshold
	TNH_I2C_Mem_Write(I2C1, MMA8452Q_ADDR_WR, 0x24, 1, c, 1, 1000);	//Y Pulse Threshold
	TNH_I2C_Mem_Write(I2C1, MMA8452Q_ADDR_WR, 0x25, 1, c, 1, 1000);	//Z Pulse Threshold
	TNH_I2C_Mem_Write(I2C1, MMA8452Q_ADDR_WR, 0x29, 1, c, 1, 1000);
#if 1
	c[0]=0;
	c[0]=0x04;	TNH_I2C_Mem_Write(I2C1, MMA8452Q_ADDR_WR, 0x2B, 1, c, 1, 1000);	//AUTO_SLEEP
	c[0]=0x01;	TNH_I2C_Mem_Write(I2C1, MMA8452Q_ADDR_WR, 0x2C, 1, c, 1, 1000);	//CTRL_REG3, [b0]-Open Drain INT Port
	c[0]=0x81;	TNH_I2C_Mem_Write(I2C1, MMA8452Q_ADDR_WR, 0x2D, 1, c, 1, 1000);	//CTRL_REG4, [b0]-Data Ready Interrupt enable b7:AUTO_SLEEP
	c[0]=0x01;	TNH_I2C_Mem_Write(I2C1, MMA8452Q_ADDR_WR, 0x2E, 1, c, 1, 1000);	//CTRL_REG5, [b0]-Use INT1 pin
#else
	c[0]=0x04; c[1]=0x01; c[2]=0x81; c[[3]=0x01;
	TNH_I2C_Mem_Write(I2C1, MMA8452Q_ADDR_WR, 0x2E, 1, c, 4, 1000);
#endif
	Mma8452Active();  // Set to active to start reading

}


void Sensor8452_ReadAcc(uint16_t *x,uint16_t *y, uint16_t *z)
//unsigned char reg_addr 0x01�? Fix
{
	unsigned char byRxd[7];
	int i;
	TNH_I2C_Mem_Read(I2C1, MMA8452Q_ADDR_RD, 0x01, 1, byRxd, 6, 1000);

	i=byRxd[0];
	i<<=8;
	i|=byRxd[1];
	*x=i;

	i=byRxd[2];
	i<<=8;
	i|=byRxd[3];
	*y=i;

	i=byRxd[4];
	i<<=8;
	i|=byRxd[5];
	*z=i;
}
