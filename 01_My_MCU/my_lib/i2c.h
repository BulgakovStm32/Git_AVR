/*
 *
 *
 *  Created on:
 *      Author:
 */

#ifndef _i2c_H
#define _i2c_H
//*******************************************************************************************
//*******************************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>
//#include <avrlibtypes.h>
//#include <avrlibdefs.h>

//*******************************************************************************************
//*******************************************************************************************

#define I2C_PORT	PORTC	   // ���� ��� ����� ���� TWI
#define I2C_DDR		DDRC
#define I2C_SCL		(1<<PINC0) // ���� ��������������� �������
#define I2C_SDA		(1<<PINC1)

//**********************************************************
#define I2C_MASTER_ADDR				0x32 //����� �� ������� ����� ����������
#define I2C_I_AM_SLAVE				0    //���� �� ��� � ������� �������� �� 1. 
										 //���� ������ �������� �� 0.

#define I2C_MASTER_BYTE_RX			1	 //�������� ������������ ������ ������ Slave, �.�. ������� ���� ����.
#define I2C_MASTER_BYTE_TX			1    //�������� ����������� ������ ������ Slave , �.�. ������� ���� ������ �� ������.

#define I2C_MAX_BUF_SIZE			3    //�������� ������ Master ������ RX-TX. ������� �� ���� ����� ����� ������ �� ����� ������
#define I2C_MAX_PAGE_ADDR_LENGTH	2	 //������������ �������� ������ ��������. ������ ����� �������� ��� ���� ��� ��� �����.
										 //������� �� ���� ������ ��� ������ ����������.
//**********************************************************
//������
#define I2C_MODE_MASK	0b00001100		// ����� ������
#define I2C_MODE_SARP	0b00000000		// Start-Addr_R-Read-Stop  							��� ����� �������� ������. �������� �� ������ ��� �� ������ � �������� ������
#define I2C_MODE_SAWP	0b00000100		// Start-Addr_W-Write-Stop 							��� ����� ������� ������. � ��� ����� � ������ � ������� ��������. 
#define I2C_MODE_SAWSARP 0b00001000		// Start-Addr_W-WrPageAdr-rStart-Addr_R-Read-Stop 	��� ����� � ��������������� ������� �������� ������ ��������
//**********************************************************
//���� ������
#define I2C_ERR_MASK	0b00110011		// ����� ���� ������
#define I2C_ERR_NO		0b00000000		// All Right! 						-- ��� ����, �������� �������. 
#define I2C_ERR_NA		0b00010000		// Device No Answer 				-- ����� �� ��������. �.�. ���� �����, ���� ��� ��� �� �����.
#define I2C_ERR_LP		0b00100000		// Low Priority 					-- ��� ����������� ����������� �������, ���� �� ��������� ��������
#define I2C_ERR_NACK	0b00000010		// Received NACK. End Transmittion. -- ��� ������� NACK. ������ � ���.
#define I2C_ERR_BF		0b00000001		// BUS FAIL 						-- ���������� ������ ����. � ���� ��� �������. ����� ����������� ������� ����������������� ����
//**********************************************************
//����� ���������
#define I2C_INTERRUPTED		0b10000000	// Transmiting Interrupted		�������� ��������� ����� ���������
#define I2C_NO_INTERRUPTED	0b01111111  // Transmiting No Interrupted	�������� ������ ����� ���������

#define I2C_BUSY		0b01000000  	// Trans is Busy				�������� ����� "���������� �����, ������ �� �������". 
#define I2C_FREE		0b10111111  	// Trans is Free				�������� ������ ����� ���������.
//**********************************************************

#define MACRO_i2c_WhatDo_MasterOut 	(MasterOutFunc)();		// ������� ��� ������ ������. ���� ��� �������, �� ����� ���� ��� ������
#define MACRO_i2c_WhatDo_SlaveOut   (SlaveOutFunc)();
#define MACRO_i2c_WhatDo_ErrorOut   (ErrorOutFunc)();

//*******************************************************************************************
//*******************************************************************************************

typedef void (*IIC_F)(void);//��� ��������� �� �������

extern IIC_F MasterOutFunc;									// ���������� � �������. 
extern IIC_F SlaveOutFunc;
extern IIC_F ErrorOutFunc;


extern uint8_t i2c_Do;											
extern uint8_t i2c_InBuff[I2C_MASTER_BYTE_RX];
extern uint8_t i2c_OutBuff[I2C_MASTER_BYTE_TX];

extern uint8_t i2c_SlaveIndex;

extern uint8_t i2c_Buffer[I2C_MAX_BUF_SIZE];
extern uint8_t i2c_index;
extern uint8_t i2c_ByteCount;

extern uint8_t i2c_SlaveAddress;
extern uint8_t i2c_PageAddress[I2C_MAX_PAGE_ADDR_LENGTH];

extern uint8_t i2c_PageAddrIndex;
extern uint8_t i2c_PageAddrCount;

//*******************************************************************************************
//*******************************************************************************************
void I2C_Init(void);
void I2C_Init_Slave(IIC_F Addr);

void    I2C_SetState(uint8_t state);
uint8_t I2C_GetState(void);

uint8_t I2C_EppWriteByte(uint8_t SAddr,uint16_t Addr, uint8_t Byte);

//*******************************************************************************************
//*******************************************************************************************
#endif

