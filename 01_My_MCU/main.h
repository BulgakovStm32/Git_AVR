//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
/*
 * 
 *
 * Created: 05.04.2018 
 *  Author: 
 */ 
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
#ifndef _MAIN_H        // Блокировка повторного включение модуля.
#define _MAIN_H 
//-----------------------------------------------------------------------------
#define F_CPU 16000000UL //Частота кварца.

#include <avr/io.h>	   //
#include <avr/wdt.h>   //подключаем библиотеку для работы со сторожевым 
#include <avr/sleep.h> //

#include <util/delay.h>

//#include "my_lib/adc.h"
//#include "my_lib/relay.h"
//#include "my_lib/pt6964.h"
#include "my_lib/timers.h"
#include "my_lib/usart.h"
//#include "my_lib/eep.h"
//#include "my_lib/hef4052.h"
//#include "my_lib/line_chacker.h"
//#include "my_lib/charge.h"

#include "my_lib/i2c.h"

#include "my_lib/Lcd_i2c_1602.h"
//-----------------------------------------------------------------------------
#define PRIORITY_5	5
#define PRIORITY_4	4
#define PRIORITY_3	3
#define PRIORITY_2	2
#define PRIORITY_1	1
//--------------------------------
#define MAX_ADDRESS	64U

#define	On	1
#define	Of	0

#define Reserv_OFF		0
#define Reserv_MASTER	1
#define Reserv_SLAVE	2
//--------------------------------
#define ADDRESS_DEFAULT			1
#define GROUP_DEFAULT			1
#define CHARGE_CHECK_DEFAULT	Of
#define RS485_CHECK_DEFAULT		On
#define TIMEOUT_CHECK_DEFAULT	12
#define BUZZER_OFF				Of
#define LEVEL_LC_DEFAULT		LEVEL_25_PERCENT
#define RESERVATION_DEFAULT		Reserv_OFF
//--------------------------------
#define TIMEOUT_CONFIG_MODE		0
//--------------------------------
//Пороги срабатывания LC.
// #define LEVEL_5_PERCENT		1
// #define LEVEL_10_PERCENT	2
// #define LEVEL_25_PERCENT	3
// #define LEVEL_40_PERCENT	4
// #define LEVEL_65_PERCENT	5
//--------------------------------
#define SW_VERSION							(0xFF)
#define CONFIG_PARAMETER_ADDRES				LC1_BUTTON
#define CONFIG_PARAMETER_GROUP				LC2_BUTTON
#define CONFIG_PARAMETER_CHARGE_CHECK		LC3_BUTTON
#define CONFIG_PARAMETER_RS485_CHECK		LC4_BUTTON
#define CONFIG_PARAMETER_TIME_AUTO_CHECK	LC5_BUTTON
#define CONFIG_PARAMETER_BUZZER				LC6_BUTTON 
#define CONFIG_PARAMETER_LEVEL_LC			LC7_BUTTON 
#define CONFIG_PARAMETER_RESERVATION		LC8_BUTTON 
//-----------------------------------------------------------------------------
#endif   // Закрывающий #endif к блокировке повторного включения
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------