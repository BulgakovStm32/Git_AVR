//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
/*
 * 
 *
 * Created: 28.02.2018 13:07:38
 *  Author: 
 */ 
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
#ifndef _TIMER0_H        // ���������� ���������� ��������� ������.
#define _TIMER0_H
//-----------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/interrupt.h>

#include "my_lib\usart.h"
#include "my_lib\pt6964.h"
#include "my_lib\relay.h"
#include "my_lib\line_chacker.h"
#include "my_lib\charge.h"
//-----------------------------------------------------------------------------

//extern volatile unsigned char LedSost;

//-----------------------------------------------------------------------------
#define TIMER0_1ms_CONST				6		//��������� ��� ��������� ���������� ������ ������������ ��� ������ 16 ���.
//-------------------------------------
#define TIMER1_UPDATE_INDICATORS_CONST	5	    // ���� - ������ ���������� ����������� �� ������� ������.
#define TIMER1_15s_CONST				30		//��������� ��� ������� 15 ������.
#define TIMER1_1h_CONST					240		//���������� ���������� �� 15 ������ ��� ������� ������ ����.
#define TIMER1_CHARGE_CHEK_CONST		10		//�������� �� � ��� ����� 5 �����.


#define TIMER0_TX_RX_LED_CONST			250U		//
//-------------------------------------
//����� Timer0FlagReg
#define Timer1_15secFlag					(1<<0) //1 - ������ 15 ������.
#define Timer1_ConfigModeFlag				(1<<1) //1 - ���� �������� � ����� ����������������.	
#define Timer1_LCLongPressFlag				(1<<2) //1 - ��������� ������ ������� �� ���� �� ����� LC.
#define Timer1_ShortIndicationFlag			(1<<3) //1 - ������ ��������� ��������� �� 3 �������.
#define Timer1_StendbyFlag					(1<<4) //1 - �������� �����.
#define Timer1_Charge15secFlag				(1<<5) //1 - ������ 15 ������. ��� ������� ���.
#define Timer1_ChargeChekrFlag				(1<<6) //1 - ������ ���. ����� ��������� ��������� ���.
#define Timer1_LongPressSPButtomFlag		(1<<7) //1 - ��������� ������ ������� �� ������ SP1 ��� SP2.
//����� Timer0FlagReg2
#define Timer0_StartPtotocolForRA1Flag		(1<<0) //1 - ������ ��������� ���� ���� ����� ����� 1.
#define Timer0_StartPtotocolForRA2Flag		(1<<1) //1 - ������ ��������� ���� ���� ����� ����� 2 � �����.
#define Timer0_StartRequestFlag				(1<<2) //

#define Timer0_StartTestAllIndicatorsFlag	(1<<3) //1 - ������ ������������ ���������.
//-------------------------------------
extern volatile uint8_t Timer0FlagReg;
extern volatile uint8_t Timer0FlagReg2;
extern volatile uint8_t Timer1BlinkReg;
extern volatile uint8_t Timer1FastBlinkReg;

extern volatile uint8_t Timer1EMSelectZone;	//�������� ���� ��� ������� �� ������ ���.
//-----------------------------------------------------------------------------
//��������� �������.
void Timers_Init(void);

uint8_t get_hour_counter(void);
void clear_hour_counter(void);
void clear_charge_hour_counter(void);
void msDelay(volatile uint16_t delay);
//-----------------------------------------------------------------------------
#endif // ����������� #endif � ���������� ���������� ���������
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------