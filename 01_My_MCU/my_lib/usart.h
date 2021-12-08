//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
/*
 * 
 *
 * Created: 24.02.2018 13:00:03
 *  Author: 
 */ 
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
#ifndef _USART_H        // Блокировка повторного включение модуля.
#define _USART_H 
//-----------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h> //подключаем библиотеку для работы со сторожевым


#include "my_lib/eep.h"
#include "my_lib/line_chacker.h"
#include "my_lib/charge.h"
#include "my_lib/relay.h"
#include "my_lib/timers.h"
//-----------------------------------------------------------------------------
#define FOSC		16000000UL
#define BAUDRATE	256000UL//115200UL				//Скрость работы USART
//#define UBRR_CONST	FOSC/(BAUDRATE*16) - 1	//Расчет значения предделителя.
#define UBRR_CONST	(((FOSC + BAUDRATE*8)/(BAUDRATE*16)) - 1) //Расчет значения предделителя.
//-----------------------------------------------------------------------------
#define DE_RE_PORT  PORTD	// Управление состоянием ~ПРИЕМ/ПЕРЕДАЧА интерфейса RS-485
#define DE_RE_DDR	DDRD	//
#define DE_RE_PIN	PIND	//
#define DE_RE		(1<<2)	// номер бита порта управления состоянием ~ПРИЕМ/ПЕРЕДАЧА интерфейса RS-485.

#define RX_LED_PORT	PORTD	// Светодиод приема RS-485 на разъеме.
#define RX_LED_DDR	DDRD	//
#define RX_LED_PIN	PIND	//
#define RX_LED		(1<<PIND6)	//

#define TX_LED_PORT	PORTD	// Светодиод передачи RS-485 на разъеме.
#define TX_LED_DDR	DDRD	//
#define TX_LED_PIN	PIND	//
#define TX_LED		(1<<PIND5)	//

#define TRANSMIT_ON			1	
#define TRANSMIT_OFF		0
//-----------------------------------------------------------------------------
#define	RequestQuantity		5	//8		//Количество запросов через которое управление пердастся другому приоритету.
#define	CycleQuantityForRA2 150	//300	//Если не было ответа в течении CycleQuantityForRA2, то значит нет консолей и управление передается 5 приоритету.
//Адреса консолей.
#define Consol_9			73U		// 0х49
#define Consol_8			72U		//
#define Consol_7			71U		//
#define Consol_6			70U		//
#define Consol_5			69U		//
#define Consol_4			68U		//
#define Consol_3			67U		//
#define Consol_2			66U		//
#define Consol_1			65U		// 0х41
//Коды команд.
#define GetStat				0x81	//Запрос статуса RA2 –RA64, RM1 – RM8, PS, IBM
#define GetStatD			0x82	//Запрос статуса управления RMx, PS, IBM
#define DriveRA				0x83	//Команда применить управление, для блоков RA2–RA64
#define StartA				0x84	//Запуск анализа состояния системы для RMx, PS, IBM
#define RetStat_RA			0x85	//Выдача статуса от блока RAх	
#define RetStat_RM			0x86	//Выдача статуса от консоли RMх
#define RetStat_PS			0x87	//Выдача статуса от селектора PS
#define RetStat_RM_D		0x88	//Выдача команд управления от консоли RMх
#define RetStat_PS_D		0x89	//Выдача команд управления от консоли PS
#define RetActivControlBlok	0x8a	//Выдача номеров блоков RA, которые контролируются селектором PS.
//Длина команд. В байтах.
#define GetStat_Length				3	
#define GetStatD_Length				3	
#define DriveRA_Length				2	
#define StartA_Length				2	
#define RetStat_RA_Length			10	
#define RetStat_RM_Length			6	
#define RetStat_PS_Length			6	
#define RetStat_RM_D_Length			79U	
#define RetStat_PS_D_Length			79U	
#define RetActivControlBlok_Length	12	
//Команды в ответах на запрос.
#define BroadCast			(1<<4)
//----------------------------------------------------------------
//Флаги.
#define USART_TRANSMIT_START_FLAG (1<<0)	//1-идет передача, 0 - передача закончена.
#define USART_RECIVE_START_FLAG   (1<<1)	//1-идет приём, 0 - приём закончен.
#define UsartConsolFlag			(1<<2)  //1-был принят ответ от консоли.
#define UsartStartRequestFlag	(1<<3)	//1 - нужно передать ответ на запрос.
//----------------------------------------------------------------
//Рабочие регистры.
typedef struct{
	//--------------------
	volatile uint8_t  TxCounterByte;//Счетчик переданных байт.
	volatile uint8_t  TxBufSize;    //Количество передаваемых байт.
	volatile uint8_t *TxBufPtr;     //Указатель на передающий буфер.
	//--------------------
}UsartWorkReg_t;


//-----------------------------------------------------------------------------
//Прототипы функций.
void USART_Init(void);
void USART_StartTX(uint8_t *txBuf, uint8_t size);

void usart_config_for_protocol(__Configuration_TypeDef *config);
void priority_for_protocol (uint8_t priority);

//void start_send_to_usart(void);
//void parsing_control_command_from_consolRM(void);
void usart_cycle_count_modify(void);
uint8_t get_consol_number(void);
uint8_t get_consol_audio_chanel(void);
uint8_t get_consol_control(void);
void send_request_from_RA2(void);

//void usart_strt_recive(void);
//void rs485_protocol(__Configuration_TypeDef *config, uint8_t priority);

void rs485_protocol(void);
//void usart_start_recive(void);
//-----------------------------------------------------------------------------
#endif   // Закрывающий #endif к блокировке повторного включения
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------

