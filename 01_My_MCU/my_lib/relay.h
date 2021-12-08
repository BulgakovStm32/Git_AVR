//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
/*
 * 
 *
 * Created: 02.04.2018 
 *  Author: 
 */ 
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
#ifndef _RELAY_H        // Блокировка повторного включение модуля.
#define _RELAY_H 
//-----------------------------------------------------------------------------
#define F_CPU 16000000UL //Частота кварца.

#include <avr/io.h>
#include <util/delay.h>

#include "my_lib/timers.h"
#include "my_lib/hef4052.h"
#include "my_lib/line_chacker.h"
#include "my_lib/spi.h"
//-----------------------------------------------------------------------------
//Защелкивание данных в сдвиговом регистре MC74HC165A(DD7).
#define WRITE_PORT	PORTB
#define WRITE_DDR	DDRB
#define WRITE_PIN	PINB
#define WRITE_LEG	(1<<0)
//Дискретный вход ALL.
#define ALL_PORT	PORTB
#define ALL_DDR		DDRB
#define ALL_PIN		PINB
#define ALL_LEG		(1<<1)
//Управление реле RELE_3.
#define RELE_3_PORT	PORTB
#define RELE_3_DDR	DDRB
#define RELE_3_PIN	PINB
#define RELE_3_LEG	(1<<2)
//Управление реле УМ.
#define POWER_AMPLIFIER_PORT	PORTD
#define POWER_AMPLIFIER_DDR		DDRD
#define POWER_AMPLIFIER_PIN		PIND
#define POWER_AMPLIFIER_LEG	   (1<<6)

#define POWER_ON				0x05
#define POWER_OFF				0x4E

#define POWER_ON_DELAY			1500U //Задержка перед включением усилителя. В мСек.
#define POWER_OFF_DELAY			500U
//Выключатель POWER/STBY.
#define POWER_SWITCH_PORT	PORTD
#define POWER_SWITCH_DDR	DDRD
#define	POWER_SWITCH_PIN	PIND
#define POWER_SWITCH_LEG	(1<<7)
//Фиксация защиты УМ "PROT".
#define PROT_PORT	PORTA
#define PROT_DDR	DDRA
#define	PROT_PIN	PINA
#define PROT_LEG	(1<<3)


//D0-D4 - PB0-PB4; D5-D7 - PC5-PC7
//#define D0D4_PORT	PORTB
//#define D0D4_DDR	DDRB
//#define D0D4_PIN	PINB
//
//#define D5D7_PORT	PORTC
//#define D5D7_DDR	DDRC
//#define D5D7_PIN	PINC
//
#define D0	(1<<0)
#define D1	(1<<1)
#define D2	(1<<2)
#define D3	(1<<3)
#define D4	(1<<4)
#define D5	(1<<5)
#define D6	(1<<6)
#define D7	(1<<7)
//С0-С3 - PD7-PD5.
//#define C0C3_PORT	PORTD
//#define C0C3_DDR	DDRD
//#define C0C3_PIN	PIND
//
//#define C0	(1<<7)
//#define C1	(1<<6)
//#define C3	(1<<5)
//
//#define STROB_DELAY		1
//Управление реле УМ.
// #define POWER_AMPLIFIER_PORT	PORTC
// #define POWER_AMPLIFIER_DDR		DDRC
// #define POWER_AMPLIFIER_PIN		PINC
// #define POWER_AMPLIFIER_LEG	   (1<<3)
// 
// #define POWER_ON				0x05
// #define POWER_OFF				0x4E
// 
// #define POWER_ON_DELAY			1500U //Задержка перед включением усилителя. В мСек.
// #define POWER_OFF_DELAY			500U
// Выключатель POWER.
// #define POWER_SWITCH_PORT	PORTC
// #define POWER_SWITCH_DDR	DDRC
// #define	POWER_SWITCH_PIN	PINC
// #define POWER_SWITCH_LEG	(1<<4)
//Сухой контакт EM1+EM1.
// #define EM1Out_PORT PORTC
// #define EM1Out_DDR	DDRC
// #define EM1Out_PIN	PINC
// #define EM1Out_LEG  (1<<1)
//Сухой контакт EM2+EM2.
// #define EM2Out_PORT PORTC
// #define EM2Out_DDR	DDRC
// #define EM2Out_PIN	PINC
// #define EM2Out_LEG  (1<<2)
//Выходной контакт "485".
//#define _485_Out	D6
//Выходной контакт "LC"
//#define LC_Out		D7
//Выходная клемма "24V".
// #define _24VOut_PORT PORTC
// #define _24VOut_DDR	 DDRC
// #define _24VOut_PIN	 PINC
// #define _24VOut_LEG	 (1<<0)
//Дискретные входы "EM". Разрешение передачи информации К555АП5.
// #define EN_K555AP5_PORT	PORTA
// #define EN_K555AP5_DDR	DDRA
// #define EN_K555AP5_PIN	PINA
// #define EN_K555AP5_LEG  (1<<3)
//---------------------------------------
#define SPEAKER_RELAY		1
#define LINE_CHECKER_RELAY	2
#define ChipDD4				3
//---------------------------------------
//Выходные клеммы.
#define EM1_OUTPUT		1
#define EM2_OUTPUT		2
#define _485_OUTPUT		3
#define LC_OUTPUT		4
#define _24V_OUTPUT		5
#define ZUMMER			6	
#define ALL_OUTPUT_OFF	7
//---------------------------------------
//Соответствие выходов DD4.
#define EM1_DD4			(1<<0)
#define EM2_DD4			(1<<1) 
#define _485_DD4		(1<<2)
#define LC_DD4			(1<<3)
#define _24V_DD4		(1<<4)
#define ZUMMER_DD4		(1<<5)
#define RELE1_DD4		(1<<6)
#define RELE2_DD4		(1<<7)
//---------------------------------------
#define ACTIVATE		1
#define DEACTIVATE		2
//---------------------------------------
#define RELAY_1		(1<<0)
#define RELAY_2		(1<<1)
#define RELAY_3		(1<<2)
#define RELAY_4		(1<<3)
#define RELAY_5		(1<<4)
#define RELAY_6		(1<<5)
#define RELAY_7		(1<<6)
#define RELAY_8		(1<<7)

#define ALL_RELAY_ON	(RELAY_1 | RELAY_2 | RELAY_3 |RELAY_4 | RELAY_5 | RELAY_6 | RELAY_7 | RELAY_8)			
#define ALL_RELAY_OFF	0x00U
//-----------------------------------------------------------------------------
//Прототипы функций.
void relay_em_init(void);
//Упарвление реле.
void set_relay(uint8_t byte, uint8_t what);
uint8_t get_relay_state(uint8_t sp_or_lc);
void power_amplifier_control(uint8_t state);

//Работа с дискретными входами.
//uint8_t	get_state(volatile uint8_t *port, uint8_t data);
//uint8_t get_power_switch(void);
uint8_t get_emergency_input(void);
void set_discret_output(uint8_t output, uint8_t state);
void select_audio_source(uint8_t source);
void consol_select_audio_channel(uint8_t channel);
//-----------------------------------------------------------------------------
#endif   // Закрывающий #endif к блокировке повторного включения
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------

