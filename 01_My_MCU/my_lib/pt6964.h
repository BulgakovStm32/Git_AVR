//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
/*
 * 
 *
 * Created: 
 *  Author: 
 */ 
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
#ifndef _PT6964_H_		// Блокировка повторного включение модуля.
#define _PT6964_H_
//-----------------------------------------------------------------------------
#define F_CPU 16000000UL //Частота кварца.

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "hef4052.h"
#include "relay.h"
#include "line_chacker.h"
//-----------------------------------------------------------------------------
#define STBPIN	PA0
#define STBPORT PORTA
#define STBDDR	DDRA

#define CLKPIN	PA1
#define CLKPORT PORTA
#define CLKDDR	DDRA

#define DIOPIN	 PA2
#define DIOPORT  PORTA
#define DIODDR	 DDRA
#define DIOPININ PINA

#define CLKSPEED 2

#define HIGH(port, pin) port |= (1 << pin)
#define LOW(port, pin)	port &= ~(1 << pin)
//----------------------------------------
//#define Button_1	1<<0
//#define Button_2	1<<1
//#define Button_3	1<<2
//#define Button_4	1<<3
//#define Button_5	1<<4
//#define Button_6	1<<5
//#define Button_7	1<<6
//#define Button_8	1<<7
//----------------------------------------
#define Led_1		(1<<0)
#define Led_2		(1<<1)
#define Led_3		(1<<2)
#define Led_4		(1<<3)
#define Led_5		(1<<4)
#define Led_6		(1<<5)
#define Led_7		(1<<6)
#define Led_8		(1<<7)
//----------------------------------------
//Цвета светодиодов.
#define GREEN		0
#define RED			1
//----------------------------------------
//Кнопки Line Cheker.
#define LC1_BUTTON	(1<<0)
#define LC2_BUTTON	(1<<1)
#define LC3_BUTTON	(1<<2)
#define LC4_BUTTON	(1<<3)
#define LC5_BUTTON	(1<<4)
#define LC6_BUTTON	(1<<5)
#define LC7_BUTTON	(1<<6)
#define LC8_BUTTON	(1<<7)
//----------------------------------------
//Кнопки Speaker.
#define SP1_BUTTON	(1<<0)
#define SP2_BUTTON	(1<<1)
#define SP3_BUTTON	(1<<2)
#define SP4_BUTTON	(1<<3)
#define SP5_BUTTON	(1<<4)
#define SP6_BUTTON	(1<<5)
#define SP7_BUTTON	(1<<6)
#define SP8_BUTTON	(1<<7)
//----------------------------------------
//Кнопки ALL и MIC
#define ALL_BUTTON	(1<<1)
#define EM_BUTTON	(1<<4)
//----------------------------------------
//Светодиоды Speaker.
//#define SP1_LED		1<<0
//#define SP2_LED		1<<1
//#define SP3_LED		1<<2
//#define SP4_LED		1<<3
//#define SP5_LED		1<<4
//#define SP6_LED		1<<5
//#define SP7_LED		1<<6
//#define SP8_LED		1<<7
//----------------------------------------
//Светодиоды Line Cheker.
#define LC1_LED		(1<<0)
#define LC2_LED		(1<<1)
#define LC3_LED		(1<<2)
#define LC4_LED		(1<<3)
#define LC5_LED		(1<<4)
#define LC6_LED		(1<<5)
#define LC7_LED		(1<<6)
#define LC8_LED		(1<<7)
//----------------------------------------
#define CHECKBIT1	(1<<2)
#define CHECKBIT2	(1<<5)
//----------------------------------------
#define LINE_CHECKER_BUTTON_BIT_MASK	0
#define SPEAKER_BUTTON_BIT_MASK			1
//----------------------------------------
#define LINE_CHECKER_LED	2
#define SPEAKER_LED			4
//#define ALL_LED				6
//----------------------------------------
#define LINE_CHECKER_BUTTON	2
#define SPEAKER_BUTTON		4
#define ALL_EM_BUTTON		6 
//-----------------------------------------------------------------------------
#define COMMAND1_7DIGITS_10SEG	0b00000011
#define COMMAND2_WRITE			0b01000000
#define COMMAND2_INCREMEN_ADDR	0b01000100
#define COMMAND2_READ			0b01000010
#define COMMAND3_ADDRESS_00		0b11000000
#define COMMAND4_DISPLAY_ON		0b10001000//0b10001111
#define COMMAND4_DISPLAY_OFF	0b10000111

#define COMMAND3				0b11000000
#define COMMAND3_ADDRESS_01		0b11000001
#define COMMAND3_ADDRESS_02		0b11000010
#define COMMAND3_ADDRESS_03		0b11000011
#define COMMAND3_ADDRESS_04		0b11000100
#define COMMAND3_ADDRESS_05		0b11000101
#define COMMAND3_ADDRESS_06		0b11000110
#define COMMAND3_ADDRESS_07		0b11000111
//-----------------------------------------------------------------------------
#define Seg_a	(1<<0)
#define Seg_b	(1<<1)
#define Seg_c	(1<<2)
#define Seg_d	(1<<3)
#define Seg_e	(1<<4)
#define Seg_f	(1<<5)
#define Seg_g	(1<<6)
//-----------------------------------------------------------------------------
#define drawOF			0x0CU
#define drawOn			0x0BU
#define drawH			0xC0U
//-----------------------------------------------------------------------------
#define	ALL_LED_ON		0xFFU
#define	ALL_LED_OFF		0x00U
//-----------------------------------------------------------------------------
#define CONFIG_END		0xA5U
#define CONFIG_START	0x00U
//-----------------------------------------------------------------------------
//Яркость светодиодов на лицевой панели по умолчанию.
#define DEFAULT_BRIGHT_DISPLAY	2
//-----------------------------------------------------------------------------
//Максимальные и минимальные значения для параметров конфигурации.
#define CONFIGURATION_ADDRESS_MAX			64U
#define CONFIGURATION_ADDRESS_MIN			1U

#define CONFIGURATION_GROUP_MAX				8U
#define CONFIGURATION_GROUP_MIN				1U	

#define CONFIGURATION_CHARGE_CHECK_MAX		1U
#define CONFIGURATION_CHARGE_CHECK_MIN		0U

#define CONFIGURATION_RS485_CHECK_MAX		1U
#define CONFIGURATION_RS485_CHECK_MIN		0U

#define CONFIGURATION_TIME_AUTO_CHECK_MAX	24U
#define CONFIGURATION_TIME_AUTO_CHECK_MIN	0U

#define CONFIGURATION_PRIORITY2_MAX			1U
#define CONFIGURATION_TPRIORITY2_MIN		0U

#define CONFIGURATION_BUZZER_MAX			1U
#define CONFIGURATION_BUZZER_MIN			0U

#define CONFIGURATION_LEVEL_LC_MAX			LEVEL_65_PERCENT//5U - это если используется порог 5%.
#define CONFIGURATION_LEVEL_LC_MIN			LEVEL_10_PERCENT

#define CONFIG_PARAMETER_RESERVATION_MAX	2U
#define CONFIG_PARAMETER_RESERVATION_MIN	0U

#define CONFIG_PARAMETER_PRIORITY_SET_MAX	2U
#define CONFIG_PARAMETER_PRIORITY_SET_MIN	1U

#define CONFIG_PARAMETER_DISPLAY_BRIGHT_MAX	7U
#define CONFIG_PARAMETER_DISPLAY_BRIGHT_MIN	0U
//-----------------------------------------------------------------------------
//Струтура хранения состояния кнопок.
//typedef volatile union {
typedef union {
	struct{
		//----------------------------------------
		unsigned char LineChekerButtonTemporarySost;//Кнопки LineCheker	
		unsigned char LineChekerButtonSost;			//
		//----------------------------------------
		unsigned char SpeakerButtonTemporarySost;	//Кнопки Speaker.
		unsigned char SpeakerButtonSost;			//
		//----------------------------------------
		unsigned char AllMicButtonTemporarySost;	//Кнопки All и Mic.
		unsigned char AllMicButtonSost;				//
		//----------------------------------------
	}STR;
	unsigned char BLK[6];//volatile unsigned char BLK[6];
}__ButtonSost_TypeDef;
//-----------------------------------------------------------------------------
//Структура состояния кнопок, вычитанная из PT6964.
#define  QUANTITY_READ_BYTE		5 //Количество вычитываемых байт из PT6964.
//typedef volatile union {
typedef volatile union {
	struct{
		//----------------------------------------
		//0-й байт.
		unsigned char	LC1_BUT:		1;	//0-й бит.
		unsigned char	SP1_BUT:		1;	//
		unsigned char	CheckBit1:		1;	//Бит используется как проверочный.
		unsigned char	LC2_BUT:		1;	//
		unsigned char	SP2_BUT:		1;	//
		unsigned char	CheckBit2:		1;	//Бит используется как проверочный.
		unsigned char	not_relevant1:	2;	//
		//----------------------------------------
		//1-й байт.
		unsigned char	LC3_BUT:		1;	//0-й бит.
		unsigned char	SP3_BUT:		1;	//
		unsigned char	CheckBit3:		1;	//Бит используется как проверочный.
		unsigned char	LC4_BUT:		1;	//
		unsigned char	SP4_BUT:		1;	//
		unsigned char	CheckBit4:		1;	//Бит используется как проверочный.
		unsigned char	not_relevant2:	2;	//
		//----------------------------------------
		//2-й байт.
		unsigned char	LC5_BUT:		1;	//0-й бит.
		unsigned char	SP5_BUT:		1;	//
		unsigned char	CheckBit5:		1;	//Бит используется как проверочный.
		unsigned char	LC6_BUT:		1;	//
		unsigned char	SP6_BUT:		1;	//
		unsigned char	CheckBit6:		1;	//Бит используется как проверочный.
		unsigned char	not_relevant3:	2;	//
		//----------------------------------------
		//3-й байт.
		unsigned char	LC7_BUT:		1;	//0-й бит.
		unsigned char	SP7_BUT:		1;	//
		unsigned char	CheckBit7:		1;	//Бит используется как проверочный.
		unsigned char	LC8_BUT:		1;	//
		unsigned char	SP8_BUT:		1;	//
		unsigned char	CheckBit8:		1;	//Бит используется как проверочный.
		unsigned char	not_relevant4:	2;	//
		//----------------------------------------
		//4-й байт.
		unsigned char	not_care1:		1;	//0-й бит.
		unsigned char	ALL_BUT:		1;	//
		unsigned char	CheckBit9:		1;	//Бит используется как проверочный.
		unsigned char	not_care2:		1;	//
		unsigned char	EM_BUT:			1;	//
		unsigned char	CheckBit10:		1;	//Бит используется как проверочный.
		unsigned char	not_relevant5:	2;	//	
		//----------------------------------------	
	}bit;
	unsigned char BLK[QUANTITY_READ_BYTE];//volatile unsigned char BLK[QUANTITY_READ_BYTE];
}__ButtonFacePanel_TypeDef;
//-----------------------------------------------------------------------------
//Cтруктура данных для управления светодиодами на лицевой панеле.
#define  QUANTITY_FACE_PANEL_BYTE	11U 

//typedef volatile union {
typedef union {
	struct{
	//----------------------------------------
	//0-й байт.	
		unsigned char	LC1_GREEN_LED:	1;	//0-й бит.
		unsigned char	LC1_RED_LED:	1;	//
		unsigned char	LC2_GREEN_LED:	1;	//
		unsigned char	LC2_RED_LED:	1;	//
		unsigned char	LC3_GREEN_LED:	1;	//
		unsigned char	LC3_RED_LED:	1;	//
		unsigned char	LC4_GREEN_LED:	1;	//
		unsigned char	LC4_RED_LED:	1;	//7-й бит.
	//----------------------------------------
	//1-й байт.
		unsigned char	LC5_GREEN_LED:	1;	//
		unsigned char	LC5_RED_LED:	1;	//
		unsigned char	not_relevant2:	6;	//
	//----------------------------------------	
	//2-й байт.
		unsigned char	LC6_GREEN_LED:	1;	//
		unsigned char	LC6_RED_LED:	1;	//
		unsigned char	LC7_GREEN_LED:	1;	//
		unsigned char	LC7_RED_LED:	1;	//
		unsigned char	LC8_GREEN_LED:	1;	//
		unsigned char	LC8_RED_LED:	1;	//
		
		unsigned char	SP1_GREEN_LED:	1;	//
		unsigned char	SP1_RED_LED:	1;	//
	//----------------------------------------
	//3-й байт.
		unsigned char	SP2_GREEN_LED:	1;	//
		unsigned char	SP2_RED_LED:	1;	//
		unsigned char	not_relevant4:	6;	//
	//----------------------------------------
	//4-й байт.
		unsigned char	SP3_GREEN_LED:	1;	//
		unsigned char	SP3_RED_LED:	1;	//
		unsigned char	SP4_GREEN_LED:	1;	//
		unsigned char	SP4_RED_LED:	1;	//
		unsigned char	SP5_GREEN_LED:	1;	//
		unsigned char	SP5_RED_LED:	1;	//
		unsigned char	SP6_GREEN_LED:	1;	//
		unsigned char	SP6_RED_LED:	1;	//	
	//----------------------------------------
	//5-й байт.
		unsigned char	SP7_GREEN_LED:	1;	//
		unsigned char	SP7_RED_LED:	1;	//
		unsigned char	not_relevant6:	6;	//
	//----------------------------------------
	//6-й байт.		
		unsigned char	LEFT_LED_PANEL_SEG_a:	1;	//
		unsigned char	LEFT_LED_PANEL_SEG_b:	1;	//
		unsigned char	LEFT_LED_PANEL_SEG_c:	1;	//
		unsigned char	LEFT_LED_PANEL_SEG_d:	1;	//
		unsigned char	LEFT_LED_PANEL_SEG_e:	1;	//
		unsigned char	LEFT_LED_PANEL_SEG_f:	1;	//	
		unsigned char	LEFT_LED_PANEL_SEG_g:	1;	//
		unsigned char	SP8_GREEN_LED:	1;	//	
	//----------------------------------------
	//7-й байт.
		unsigned char	SP8_RED_LED:	1;	//
		unsigned char	ALL_LED:		1;	//
		unsigned char	not_relevant8:	6;	//
	//----------------------------------------
	//8-й байт.
		unsigned char	RIGHT_LED_PANEL_SEG_a:	1;	//
		unsigned char	RIGHT_LED_PANEL_SEG_b:	1;	//
		unsigned char	RIGHT_LED_PANEL_SEG_c:	1;	//
		unsigned char	RIGHT_LED_PANEL_SEG_d:	1;	//
		unsigned char	RIGHT_LED_PANEL_SEG_e:	1;	//
		unsigned char	RIGHT_LED_PANEL_SEG_f:	1;	//
		unsigned char	RIGHT_LED_PANEL_SEG_g:	1;	//
		unsigned char	RS485_LED:	1;	//
	//----------------------------------------
	//9-й байт.
		unsigned char	STBAY_LED:	1;	//
		unsigned char	POWER_LED:	1;	//
		unsigned char	not_relevant10:	6;	//
	//----------------------------------------
	//10-й байт.
		unsigned char	PROT_LED:	1;	//
		unsigned char	not_relevant11:	7;	//
	//----------------------------------------
			}bit;
	unsigned char BLK[QUANTITY_FACE_PANEL_BYTE];//volatile unsigned char BLK[QUANTITY_FACE_PANEL_BYTE];
}__IndicatorFacePanel_TypeDef;
//-----------------------------------------------------------------------------

extern	__ButtonSost_TypeDef	ButtonSostStr;
extern uint8_t DisplayBright;
extern uint8_t TempDisplayBright;

//-----------------------------------------------------------------------------
//Прототипы функций.
void pt6964_init(void);
//--------------------------------------------
//Работа с индикацией.
void write_data_to_face_penel(void);
uint8_t bin_to_dec(uint8_t byte);
void display_set(uint16_t byte);//Вывод семимсегментный индикатор.
void display_off(void);

void power_led_green(void);
void power_led_blink_green(void);

void set_line_checker_led_green(uint8_t led);
void set_line_checker_led_red(uint8_t led);
void set_speaker_led_green(uint8_t led);
void set_speaker_led_red(uint8_t led);
//--------------------------------------------
//Работа с кнопками.
void read_button_from_face_penel(void);
uint8_t get_button(uint8_t buttons);
uint8_t toggle_button(uint8_t button);

uint8_t config_parameter_selection (uint8_t mode);
void modification_config_parameter(uint8_t *parameter, uint8_t min, uint8_t max);
//-----------------------------------------------------------------------------
#endif	// Закрывающий #endif к блокировке повторного включения
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
