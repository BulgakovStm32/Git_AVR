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
#ifndef _USART_H        // ���������� ���������� ��������� ������.
#define _USART_H 
//-----------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h> //���������� ���������� ��� ������ �� ����������


#include "my_lib/eep.h"
#include "my_lib/line_chacker.h"
#include "my_lib/charge.h"
#include "my_lib/relay.h"
#include "my_lib/timers.h"
//-----------------------------------------------------------------------------
#define FOSC		16000000UL
#define BAUDRATE	256000UL//115200UL				//������� ������ USART
//#define UBRR_CONST	FOSC/(BAUDRATE*16) - 1	//������ �������� ������������.
#define UBRR_CONST	(((FOSC + BAUDRATE*8)/(BAUDRATE*16)) - 1) //������ �������� ������������.
//-----------------------------------------------------------------------------
#define DE_RE_PORT  PORTD	// ���������� ���������� ~�����/�������� ���������� RS-485
#define DE_RE_DDR	DDRD	//
#define DE_RE_PIN	PIND	//
#define DE_RE		(1<<2)	// ����� ���� ����� ���������� ���������� ~�����/�������� ���������� RS-485.

#define RX_LED_PORT	PORTD	// ��������� ������ RS-485 �� �������.
#define RX_LED_DDR	DDRD	//
#define RX_LED_PIN	PIND	//
#define RX_LED		(1<<PIND6)	//

#define TX_LED_PORT	PORTD	// ��������� �������� RS-485 �� �������.
#define TX_LED_DDR	DDRD	//
#define TX_LED_PIN	PIND	//
#define TX_LED		(1<<PIND5)	//

#define TRANSMIT_ON			1	
#define TRANSMIT_OFF		0
//-----------------------------------------------------------------------------
#define	RequestQuantity		5	//8		//���������� �������� ����� ������� ���������� ��������� ������� ����������.
#define	CycleQuantityForRA2 150	//300	//���� �� ���� ������ � ������� CycleQuantityForRA2, �� ������ ��� �������� � ���������� ���������� 5 ����������.
//������ ��������.
#define Consol_9			73U		// 0�49
#define Consol_8			72U		//
#define Consol_7			71U		//
#define Consol_6			70U		//
#define Consol_5			69U		//
#define Consol_4			68U		//
#define Consol_3			67U		//
#define Consol_2			66U		//
#define Consol_1			65U		// 0�41
//���� ������.
#define GetStat				0x81	//������ ������� RA2 �RA64, RM1 � RM8, PS, IBM
#define GetStatD			0x82	//������ ������� ���������� RMx, PS, IBM
#define DriveRA				0x83	//������� ��������� ����������, ��� ������ RA2�RA64
#define StartA				0x84	//������ ������� ��������� ������� ��� RMx, PS, IBM
#define RetStat_RA			0x85	//������ ������� �� ����� RA�	
#define RetStat_RM			0x86	//������ ������� �� ������� RM�
#define RetStat_PS			0x87	//������ ������� �� ��������� PS
#define RetStat_RM_D		0x88	//������ ������ ���������� �� ������� RM�
#define RetStat_PS_D		0x89	//������ ������ ���������� �� ������� PS
#define RetActivControlBlok	0x8a	//������ ������� ������ RA, ������� �������������� ���������� PS.
//����� ������. � ������.
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
//������� � ������� �� ������.
#define BroadCast			(1<<4)
//----------------------------------------------------------------
//�����.
#define USART_TRANSMIT_START_FLAG (1<<0)	//1-���� ��������, 0 - �������� ���������.
#define USART_RECIVE_START_FLAG   (1<<1)	//1-���� ����, 0 - ���� ��������.
#define UsartConsolFlag			(1<<2)  //1-��� ������ ����� �� �������.
#define UsartStartRequestFlag	(1<<3)	//1 - ����� �������� ����� �� ������.
//----------------------------------------------------------------
//������� ��������.
typedef struct{
	//--------------------
	volatile uint8_t  TxCounterByte;//������� ���������� ����.
	volatile uint8_t  TxBufSize;    //���������� ������������ ����.
	volatile uint8_t *TxBufPtr;     //��������� �� ���������� �����.
	//--------------------
}UsartWorkReg_t;


//-----------------------------------------------------------------------------
//��������� �������.
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
#endif   // ����������� #endif � ���������� ���������� ���������
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------

