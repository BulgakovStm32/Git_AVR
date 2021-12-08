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

#define I2C_PORT	PORTC	   // Порт где сидит нога TWI
#define I2C_DDR		DDRC
#define I2C_SCL		(1<<PINC0) // Биты соответствующих выводов
#define I2C_SDA		(1<<PINC1)

//**********************************************************
#define I2C_MASTER_ADDR				0x32 //Адрес на который будем отзываться
#define I2C_I_AM_SLAVE				0    //Если мы еще и слейвом работаем то 1. 
										 //Если только мастером то 0.

#define I2C_MASTER_BYTE_RX			1	 //Величина принимающего буфера режима Slave, т.е. сколько байт жрем.
#define I2C_MASTER_BYTE_TX			1    //Величина Передающего буфера режима Slave , т.е. сколько байт отдаем за сессию.

#define I2C_MAX_BUF_SIZE			3    //Величина буфера Master режима RX-TX. Зависит от того какой длины строки мы будем гонять
#define I2C_MAX_PAGE_ADDR_LENGTH	2	 //Максимальная величина адреса страницы. Обычно адрес страницы это один или два байта.
										 //Зависит от типа ЕЕПРОМ или другой микросхемы.
//**********************************************************
//Режимы
#define I2C_MODE_MASK	0b00001100		// Маска режима
#define I2C_MODE_SARP	0b00000000		// Start-Addr_R-Read-Stop  							Это режим простого чтения. Например из слейва или из епрома с текущего адреса
#define I2C_MODE_SAWP	0b00000100		// Start-Addr_W-Write-Stop 							Это режим простой записи. В том числе и запись с адресом страницы. 
#define I2C_MODE_SAWSARP 0b00001000		// Start-Addr_W-WrPageAdr-rStart-Addr_R-Read-Stop 	Это режим с предварительной записью текущего адреса страницы
//**********************************************************
//Коды ошибок
#define I2C_ERR_MASK	0b00110011		// Маска кода ошибок
#define I2C_ERR_NO		0b00000000		// All Right! 						-- Все окей, передача успешна. 
#define I2C_ERR_NA		0b00010000		// Device No Answer 				-- Слейв не отвечает. Т.к. либо занят, либо его нет на линии.
#define I2C_ERR_LP		0b00100000		// Low Priority 					-- нас перехватили собственным адресом, либо мы проиграли арбитраж
#define I2C_ERR_NACK	0b00000010		// Received NACK. End Transmittion. -- Был получен NACK. Бывает и так.
#define I2C_ERR_BF		0b00000001		// BUS FAIL 						-- Аппаратная ошибка шины. И этим все сказано. Можно попробовать сделать переинициализацию шины
//**********************************************************
//Флаги состояний
#define I2C_INTERRUPTED		0b10000000	// Transmiting Interrupted		Битмаска установки флага занятости
#define I2C_NO_INTERRUPTED	0b01111111  // Transmiting No Interrupted	Битмаска снятия флага занятости

#define I2C_BUSY		0b01000000  	// Trans is Busy				Битмаска флага "Передатчик занят, руками не трогать". 
#define I2C_FREE		0b10111111  	// Trans is Free				Битмаска снятия флага занятости.
//**********************************************************

#define MACRO_i2c_WhatDo_MasterOut 	(MasterOutFunc)();		// Макросы для режимо выхода. Пока тут функция, но может быть что угодно
#define MACRO_i2c_WhatDo_SlaveOut   (SlaveOutFunc)();
#define MACRO_i2c_WhatDo_ErrorOut   (ErrorOutFunc)();

//*******************************************************************************************
//*******************************************************************************************

typedef void (*IIC_F)(void);//Тип указателя на функцию

extern IIC_F MasterOutFunc;									// Подрбрости в сишнике. 
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

