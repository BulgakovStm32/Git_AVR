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
#ifndef _LINE_CHECKER_H        // Блокировка повторного включение модуля.
#define _LINE_CHECKER_H
//-----------------------------------------------------------------------------

#include <avr/io.h>

#include "my_lib/relay.h"
#include "my_lib/adc.h"
#include "my_lib/timers.h"
#include "my_lib/pt6964.h"
//-----------------------------------------------------------------------------
#define COEFF_SCALE		10000U

#define RESISTENCE_MAX	30000U  //3000 Ом  - при обрыве линии.
#define RESISTENCE_MIN	20U	    //2    Ома - минимальное сопротивление.  0x00E2U	//Код АЦП при сопротивления линии 3 Ома. 
//-------------------------------------
//Пороги срабатывания LC.
// #define LEVEL_5_PERCENT	1
#define LEVEL_10_PERCENT	1
#define LEVEL_25_PERCENT	2
#define LEVEL_40_PERCENT	3
#define LEVEL_65_PERCENT	4
//-------------------------------------
#define LC_LONG_TIMEOUT		1000U
#define LC_SMALL_TIMEOUT	500U
//-------------------------------------
#define ALL_LINES			0xFF	//
//-----------------------------------------------------------------------------
//Флаги.
#define LineChecker_MeasResistFlag	(1<<0) //1 - идет измерение сопротивления линии.
//-----------------------------------------------------------------------------
//Прототипы функций.
void set_lines_to_control(uint8_t line);
uint8_t get_lines_to_control(void);
uint8_t get_blocked_speaker_line(void);

uint8_t get_alarms_lines(void);

void calibration_adc_for_line_checker(void);
void measure_resistence_all_line_after_power_on(uint8_t level);
void measure_resistence_line_after_timeout(void);
void one_line_check(uint8_t number);

void resistence_all_line_indicate(void);
void start_short_indication(uint8_t line);
//-----------------------------------------------------------------------------
#endif // Закрывающий #endif к блокировке повторного включения
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------