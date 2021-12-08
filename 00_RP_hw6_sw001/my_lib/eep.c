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

#include "my_lib\eep.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void eep_write_byte(uint8_t byte, uint8_t address){
	
	while (EECR & (1<<EEWE)) //Ожидание готовности EEPROM.
	{}						 //
	//---------------------------
	EEAR = (uint16_t)address;
	EEDR = byte;
	
	cli(); //Запрет прерываний.
	
	EECR |= (1<<EEMWE);
	EECR |= (1<<EEWE);
	
	sei(); //Разрешение прерываний.
}
//-----------------------------------------------------------------------------
static uint8_t eep_read_byte(uint8_t address){
	
	while (EECR & (1<<EEWE)) //Ожидание готовности EEPROM.
	{}						 //
	//---------------------------
	cli(); //Запрет прерываний.
	
	EEAR = (uint16_t)address;
	EECR |= (1<<EERE);
	
	sei(); //Разрешение прерываний.
	
	return EEDR;
}
//-----------------------------------------------------------------------------
//Сбров всех ячеек ЕЕПРОМ.
//void eep_reset_all(void){
	//
	//uint16_t i;
	////---------------------------
	//for (i = 0; i < 512; i++)
		//{
			//eep_write_byte(0xFF, i);
		//}
//}
//-----------------------------------------------------------------------------
//Запись конфигурации в ЕЕПРОМ. Конфигурация записывается три раза для Мажоритарное определение.
// Это нужно для защиты от порчи ЕЕПРОМ.
void eep_write_configuration(__Configuration_TypeDef *configuration){
		
	uint8_t i;
	uint8_t n;
	//---------------------------
	for(n = EEPROM_STARTING_ADDRESS_CONFIGURATION; n < EEPROM_FINISH_ADDRESS_CONFIGURATION; n += EEPROM_SIZE_CONFIGURATION)
		{
			for(i = 0; i < EEPROM_SIZE_CONFIGURATION; i++)
				{
					eep_write_byte(configuration->BLK[i], (n + i));
				}	
		}		
}
//-----------------------------------------------------------------------------
void eep_read_configuration(__Configuration_TypeDef *configuration){

	uint8_t i;
	uint8_t n;
	__Configuration_TypeDef config[EEPROM_QUANTITU_CONFIGURATION];
	//---------------------------
	//Считывание трёх конфигураций.
	for (i = 0; i < EEPROM_QUANTITU_CONFIGURATION; i++)
		{
			for(n = 0; n < EEPROM_SIZE_CONFIGURATION; n++)
				{
					config[i].BLK[n] = eep_read_byte(EEPROM_STARTING_ADDRESS_CONFIGURATION + (i * EEPROM_SIZE_CONFIGURATION) + n);
				}
		}
	//-------------------------------
	//Мажоритарное определение конфигурации по трем записям.
	for (i = 0; i < EEPROM_SIZE_CONFIGURATION; i++)
		{
			configuration->BLK[i] = (config[0].BLK[i] & config[1].BLK[i]) |
									(config[1].BLK[i] & config[2].BLK[i]) |
									(config[0].BLK[i] & config[2].BLK[i]);
		}
	//-------------------------------
}
//-----------------------------------------------------------------------------
//Сохранение в взятых на контроль линий EEPROM.
void eep_write_set_lines_to_control(uint8_t byte){
	
	//---------------------------
	eep_write_byte(byte, EEPROM_START_ADD_SET_LINE);
 	eep_write_byte(byte, EEPROM_START_ADD_SET_LINE + 1);
 	eep_write_byte(byte, EEPROM_START_ADD_SET_LINE + 2);	
	//---------------------------
}
//-----------------------------------------------------------------------------
uint8_t eep_read_set_lines_to_control(void){
	
	uint8_t set_line_1 = eep_read_byte(EEPROM_START_ADD_SET_LINE);
	uint8_t set_line_2 = eep_read_byte(EEPROM_START_ADD_SET_LINE + 1);
	uint8_t set_line_3 = eep_read_byte(EEPROM_START_ADD_SET_LINE + 2);
	//---------------------------
	return ((set_line_1 & set_line_2) | (set_line_2 & set_line_3) | (set_line_1 & set_line_3));
	//---------------------------
}
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------


