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
#ifndef _EEP_H        // ���������� ���������� ��������� ������.
#define _EEP_H 
//-----------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/interrupt.h>

//-----------------------------------------------------------------------------
//����� � ������ ������ ���������� ������������.
#define EEPROM_STARTING_ADDRESS_CONFIGURATION	5
//���������� ���� � ������������.
#define EEPROM_SIZE_CONFIGURATION				8
//���������� ������� ������������.
#define EEPROM_QUANTITU_CONFIGURATION			3
//����� ����� ��� ������ ������������.
#define EEPROM_FINISH_ADDRESS_CONFIGURATION		EEPROM_STARTING_ADDRESS_CONFIGURATION + (EEPROM_SIZE_CONFIGURATION * EEPROM_QUANTITU_CONFIGURATION)

//��������� ��� ���������� ������������� �� �������� ����� � EEPROM.
#define EEPROM_START_ADD_SET_LINE	(EEPROM_FINISH_ADDRESS_CONFIGURATION) + 2	//����� � ������ ���� ������
#define EEPROM_SIZE_SET_LINE		1											//���������� ���� �� ������
#define EEPROM_QUANTITU_SET_LINE	3											//���������� �������

#define EEPROM_FINISH_ADD_SET_LINE		EEPROM_START_ADD_SET_LINE + (EEPROM_SIZE_SET_LINE * EEPROM_QUANTITU_SET_LINE)
//-----------------------------------------------------------------------------
//�������� ��� �������� ������������.
//typedef volatile union {
typedef union {
	struct{
		//----------------------------------------
		//0-� ����.
		uint8_t Address;		//����� �����.
		//1-� ����.
		uint8_t Group;			//������.
		//2-� ����.
		uint8_t ChargeCheck;	//���/���� ������� ������� ������ ���.
		//3-� ����.
		uint8_t RS485Check;		//���/���� ������� RS485.
		//4-� ����.
		uint8_t TimeAutoCheck;	//����� ��������������� �������� �����.
		//5-� ����.
		uint8_t Buzzer;			//��������� �������.
		//6-����.
		uint8_t	LevelLC;		//��������� ������ ������������ LC.
		//7-����.
		uint8_t	Reservation;	//��������� �������������� �����.
		//----------------------------------------
	}STR;
	uint8_t BLK[EEPROM_SIZE_CONFIGURATION];
}__Configuration_TypeDef;
//-----------------------------------------------------------------------------
//��������� �������.
//void eeprom_reset_all(void);
void eep_write_configuration (__Configuration_TypeDef *configuration);
void eep_read_configuration  (__Configuration_TypeDef *configuration);
void eep_write_set_lines_to_control(uint8_t byte);
uint8_t eep_read_set_lines_to_control(void);
//-----------------------------------------------------------------------------
#endif   // ����������� #endif � ���������� ���������� ���������
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------

