//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
/*
 * 
 *
 * Created: 10.04.2018 
 *  Author: 
 */ 
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
#ifndef _EEP_H        // Блокировка повторного включение модуля.
#define _EEP_H 
//-----------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/interrupt.h>

//-----------------------------------------------------------------------------
//Адрес в ЕЕПРОМ откуда начинается конфигурация.
#define EEPROM_STARTING_ADDRESS_CONFIGURATION	5
//Количество байт в конфигурации.
#define EEPROM_SIZE_CONFIGURATION				8
//Количество записей конфигурации.
#define EEPROM_QUANTITU_CONFIGURATION			3
//Адрес конца при записи конфигурации.
#define EEPROM_FINISH_ADDRESS_CONFIGURATION		EEPROM_STARTING_ADDRESS_CONFIGURATION + (EEPROM_SIZE_CONFIGURATION * EEPROM_QUANTITU_CONFIGURATION)

//Параметры для сохранения установленных на контроль линий в EEPROM.
#define EEPROM_START_ADD_SET_LINE	(EEPROM_FINISH_ADDRESS_CONFIGURATION) + 2	//Адрес в ЕЕПРОМ куда писать
#define EEPROM_SIZE_SET_LINE		1											//Количество байт на запись
#define EEPROM_QUANTITU_SET_LINE	3											//Количество записей

#define EEPROM_FINISH_ADD_SET_LINE		EEPROM_START_ADD_SET_LINE + (EEPROM_SIZE_SET_LINE * EEPROM_QUANTITU_SET_LINE)
//-----------------------------------------------------------------------------
//Струтура для хранения конфигурации.
//typedef volatile union {
typedef union {
	struct{
		//----------------------------------------
		//0-й байт.
		uint8_t Address;		//Адрес блока.
		//1-й байт.
		uint8_t Group;			//Группа.
		//2-й байт.
		uint8_t ChargeCheck;	//Вкл/выкл системы конроля заряда АКБ.
		//3-й байт.
		uint8_t RS485Check;		//Вкл/выкл конроля RS485.
		//4-й байт.
		uint8_t TimeAutoCheck;	//Время автоматического контроля линий.
		//5-й байт.
		uint8_t Buzzer;			//Активация мищалки.
		//6-байт.
		uint8_t	LevelLC;		//Установка порога срабатывания LC.
		//7-байт.
		uint8_t	Reservation;	//Установка резервирования блока.
		//----------------------------------------
	}STR;
	uint8_t BLK[EEPROM_SIZE_CONFIGURATION];
}__Configuration_TypeDef;
//-----------------------------------------------------------------------------
//Прототипы функций.
//void eeprom_reset_all(void);
void eep_write_configuration (__Configuration_TypeDef *configuration);
void eep_read_configuration  (__Configuration_TypeDef *configuration);
void eep_write_set_lines_to_control(uint8_t byte);
uint8_t eep_read_set_lines_to_control(void);
//-----------------------------------------------------------------------------
#endif   // Закрывающий #endif к блокировке повторного включения
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------

