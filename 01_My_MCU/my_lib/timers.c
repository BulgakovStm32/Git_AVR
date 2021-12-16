//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
/*
 * timer0.c
 *
 * Created: 
 *  Author: 
 */ 
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------

#include "my_lib\timers.h"

//-----------------------------------------------------------------------------

extern __IndicatorFacePanel_TypeDef	IndicatorsFacePanelStr;
extern __ButtonFacePanel_TypeDef		ButtonFacePanelStr;

//-----------------------------------------------------------------------------
//�������� �������� ��������� �������0.
typedef volatile struct{
		//----------------------------------------
// 		uint8_t  Timer0UpdateIndicatorsCounter; //������� ��������� ���������� ��������� �� ������� ������.
//		uint8_t  Timer0ReadButtonCounter;		//������� ��������� ���������� ������ �� ������� ������.
		uint8_t  Timer0Clock_ms_Counter;		//������� ��� ������������ 15-����������� ����������.
		uint8_t  Timer0SecondCounter;			//������� 15-����������� ���������� ��� ������� ����.
		uint8_t  Timer0ChargeSecondCounter;		//������� 15-����������� ���������� ��� ������� �������� ��������� �������� ���.
		uint8_t  Timer0HourCounter;				//������� �����.
		uint8_t  Timer03secIndicatCounter;		//������� ��� ����������� 3-���������� ���������.
		uint16_t Timer0LongPressCounter;		//������� ��� �������� ���������� ������� �� ������.
		uint8_t  Timer0LongPressSPButtomCounter;//������� ��� ��������� ��������� ������� SP1 � SP2.
		uint8_t  Timer0LongPressSPButtomTimeout;//
		uint8_t  Timer0LongPressSPButtomEMSPCounter;//������� ��� �������� ����������� ������� ������ SP ��� �������� ���.
//		uint8_t  Timer0ProtocolCounter;			//������� ��� ����������� ��������� ������.
		uint8_t  Timer0Blink_ms_Counter;		//������� ��� ����������� �������� ���������.
		uint8_t	 Timer0FastBlink_ms_Counter;	//������� ��� ����������� �������� �������.
		uint8_t  Timer0_RX_LED_Counter;			//������� ��� ����������� ��������� �� ������� ���� �� ������.
		uint8_t  Timer0_TX_LED_Counter;			//������� ��� ����������� ��������� �� ������� ���� �� ��������.
		uint16_t Timer0Delay;					//������� ��� ������ ��������.
		//----------------------------------------
}__Timer0_Counters_TypeDef;
//-----------------------------------------------------------------------------
volatile uint8_t Timer0FlagReg  = 0;
volatile uint8_t Timer0FlagReg2 = 0;
volatile uint8_t Timer1BlinkReg = 0;
volatile uint8_t Timer1FastBlinkReg = 0;
volatile uint8_t Timer1EMSelectZone = 0xFF;	//�������� ���� ��� ������� �� ������ ���. �� ��������� ��� ���� �������.
//----------------------------------------
__Timer0_Counters_TypeDef	Timer0CountersStr;
//-----------------------------------------------------------------------------
void Timers_Init(void){
	
	//TCCR0 =0x00;
	//TCNT0 =0x00;
	//OCR0  =0x00;
	//-------------------------
	Timer0CountersStr.Timer0LongPressSPButtomTimeout = 100;
	//-------------------------
	 //����� 63035 = 0xF63b ��� ���������� ������ 10 ����.
	 //����� 60535 = 0xEC77 ��� ���������� ������ 20 ����.
	TCNT1H = 0xF6;
	TCNT1L = 0x3B;
	TCCR1B |= (1<<CS11)|(1<<CS10); //������������ 64.
	//TIMSK  |= (1<<TOIE1);		 //��������� ���������� �� ������������ �������1
	//-------------------------
	TCNT0  = TIMER0_1ms_CONST;	   //��������� �������� �������
	TCCR0 |= (1<<CS01)|(1<<CS00);  //������������ 64.
	//-------------------------
	TIMSK |= (1<<TOIE0 | 1<<TOIE1);//��������� ���������� �� ������������ �������0 � �������1.
}
//-----------------------------------------------------------------------------
//��������� �������� �����.
uint8_t get_hour_counter(void){
	
	return Timer0CountersStr.Timer0HourCounter;
}
//-----------------------------------------------------------------------------
void clear_hour_counter(void){
	
	cli(); //������ ����������.	
	Timer0CountersStr.Timer0HourCounter = 0;
	sei(); //���������� ����������.
}
//-----------------------------------------------------------------------------
void clear_charge_hour_counter(void){
	
	cli(); //������ ����������.	
	Timer0CountersStr.Timer0ChargeSecondCounter = 0;
	sei(); //���������� ����������.
}
//-----------------------------------------------------------------------------
void msDelay(volatile uint16_t delay){
	
	if(delay == 0) return;
	
	cli(); //������ ����������.		
	Timer0CountersStr.Timer0Delay = 0;
	sei(); //���������� ����������.
	
	while(Timer0CountersStr.Timer0Delay != delay){} //��������� �����.
}
//-----------------------------------------------------------------------------
//���������� ������ 10 ����.
ISR(TIMER1_OVF_vect){
	
	//----------------------------------------------------
	//��������� �������� �������. 63035 = 0xF63b ��� ���������� ������ 10 ����.
	TCNT1H = 0xF6;
	TCNT1L = 0x3B;
	//----------------------------------------------------
	//����� ����������� �������
	//wdt_reset(); 
	//----------------------------------------------------
	//������� ������� ��� ��������� LC.
	if (++Timer0CountersStr.Timer0FastBlink_ms_Counter >= 12)
		{
			Timer0CountersStr.Timer0FastBlink_ms_Counter = 0;
			Timer1FastBlinkReg ^= 0xFF;
		}
	//----------------------------------------------------
	//������� 500 ����.
	if (++Timer0CountersStr.Timer0Blink_ms_Counter >= 50)
		{
			Timer0CountersStr.Timer0Blink_ms_Counter = 0;
			Timer1BlinkReg ^= 0xFF;
			//----------------------------------
			//���� ��� ������� �������� ��������������� �������� �����.
			if (++Timer0CountersStr.Timer0Clock_ms_Counter >= TIMER1_15s_CONST)
				{
					Timer0CountersStr.Timer0Clock_ms_Counter = 0;
					Timer0FlagReg |= Timer1_15secFlag;
					Timer0FlagReg &= ~Timer1_Charge15secFlag;
					//-------------------------		
					//�������� ����� ��� �������������� �������� �����.
					if (++Timer0CountersStr.Timer0SecondCounter >= TIMER1_1h_CONST)
						{
							Timer0CountersStr.Timer0SecondCounter = 0;
							Timer0CountersStr.Timer0HourCounter++;
							//�������� �� ��������.
							if (Timer0CountersStr.Timer0HourCounter > 24)
								{
									Timer0CountersStr.Timer0HourCounter = 24;
								}
						}
					//----------------------------------
					//���� ��� ������� �������� �������� �������� ���.
					if (++Timer0CountersStr.Timer0ChargeSecondCounter >= TIMER1_CHARGE_CHEK_CONST)
						{
							Timer0CountersStr.Timer0ChargeSecondCounter = 0;
							Timer0FlagReg &= ~Timer1_ChargeChekrFlag;
						}
					//----------------------------------
				}
			//-------------------------------------
			//��������� � ������� 3 ������.
			if (Timer0FlagReg & Timer1_ShortIndicationFlag)
				{
					if (++Timer0CountersStr.Timer03secIndicatCounter >= 6)
						{
							Timer0CountersStr.Timer03secIndicatCounter = 0;
							Timer0FlagReg &= ~Timer1_ShortIndicationFlag;
						}
				}
			//-------------------------------------
			//��������� � �������� ������. �������� ������� ��������� POWER.
			if (Timer0FlagReg & Timer1_StendbyFlag)
				{
					IndicatorsFacePanelStr.bit.STBAY_LED = Timer1BlinkReg;
					if (get_charge_indication()) 
						{
							IndicatorsFacePanelStr.bit.POWER_LED = Timer1BlinkReg;
						}
				}
			//-------------------------------------
		}
	//----------------------------------------------------
 	//���� � ������ ������������ �� ��������� ��������� LINE CHECKERa ���������.
 	if((Timer0FlagReg & Timer1_ConfigModeFlag) == 0)
 		{
 			//-------------------------
 			//��������� LINE CHACKER�
 			resistence_all_line_indicate();
 			//-------------------------
 			//�������� ��������� ��� ������������� ��������� ����������. �� ����� ����������� ��������� ����� �������� ��������
			//�� ������� ������������ ��� ��������� �����. 
 			if  ( (Timer1BlinkReg & get_charge_indication()) && !(Timer0FlagReg & Timer1_ShortIndicationFlag) )
 				{
					 //�������� ��������� ��� ������������� ����� �� ��� ������������� ������� 220.
 					display_set(get_charge_indication());
 				}
 			//-------------------------
 		}
 	//------------------------- 
	sei(); //���������� ���������� ����������, �.�. ����� ����������� ����� ������ ��������� �� ����� ������ � ������� �������. 
	//������ ������ � ������� ������. ������ 65 uS.
 	write_data_to_face_penel();
	//----------------------------------------------------
	//������� ��������� ������. ������ 120 uS.
	read_button_from_face_penel();
	cli(); //������ ����������.	
	//-------------------------
	//��������� ����������� ������� (�� ����� 3 ���). 
 	if ( ButtonFacePanelStr.bit.EM_BUT || get_button(LINE_CHECKER_BUTTON))
  		{
  			if (++Timer0CountersStr.Timer0LongPressCounter >= 300)//���� ��������� �� 300 ������ ������ 3 �������.
  				{
  					//Timer0CountersStr.Timer0LongPressCounter = 0;
  					//-------------------------
  					//��������� ������ ������ EM. ����� 5 ������.
  					if (ButtonFacePanelStr.bit.EM_BUT)
  						{
							if (Timer0CountersStr.Timer0LongPressCounter >= 500)
								{
									Timer0CountersStr.Timer0LongPressCounter = 0;
									Timer0FlagReg ^= Timer1_ConfigModeFlag; //��������� ����� ������� ������� �� ������ EM - ������� � ����� ����������������.
								}
  						}
  					//-------------------------
  					//��������� ������ ���� �� ������ LC.
  					else
  						{
							Timer0CountersStr.Timer0LongPressCounter = 0;
  							Timer0FlagReg |= Timer1_LCLongPressFlag;	 //��������� ����� ����������� ������� �� ������ LC.
  							Timer0FlagReg &= ~(Timer1_15secFlag);		 //����� ����� 15 ���.
  							Timer0CountersStr.Timer0Clock_ms_Counter = 0;//����� �������� 15 ���.
  							Timer0CountersStr.Timer0SecondCounter = 0;	 //����� �������� ������� �����.
  						}
  					//-------------------------
  				}
  		}
  	else
  		{
  			Timer0CountersStr.Timer0LongPressCounter = 0;
  		}
	//----------------------------------------------------
	//���������� ������� ������ SP ��� �������� ���.
	if(toggle_button(ALL_EM_BUTTON) & EM_BUTTON)
		{
			//-------------------------
			if (get_button(SPEAKER_BUTTON))
				{
					Timer0CountersStr.Timer0LongPressSPButtomEMSPCounter++;
					if (Timer0CountersStr.Timer0LongPressSPButtomEMSPCounter >= 100)
						{
							Timer0CountersStr.Timer0LongPressSPButtomEMSPCounter = 0;
							Timer1EMSelectZone ^= get_button(SPEAKER_BUTTON);
						}
				}
			//-------------------------
			else
				{
					Timer0CountersStr.Timer0LongPressSPButtomEMSPCounter = 0;
				}
			//-------------------------
		}
	//----------------------------------------------------
	//���������� ������� ������ ��� ��������� �����.
	if(Timer0FlagReg & Timer1_ConfigModeFlag)
		{
			//-------------------------
			//���������� ������� ������ SP1 ��� SP2 ��� ��������� �����.
			if (get_button(SPEAKER_BUTTON) || ButtonFacePanelStr.bit.ALL_BUT)
				{
					Timer0CountersStr.Timer0LongPressSPButtomCounter++;
					//���������� ������� ������ SP1 ��� SP2 ��� ��������� �����.
					if ( (Timer0CountersStr.Timer0LongPressSPButtomCounter >= Timer0CountersStr.Timer0LongPressSPButtomTimeout) && (get_button(SPEAKER_BUTTON)) )
						{
							Timer0CountersStr.Timer0LongPressSPButtomCounter = 0;
							Timer0CountersStr.Timer0LongPressSPButtomTimeout = 9;
							Timer0FlagReg |= Timer1_LongPressSPButtomFlag;
						}
					//���������� ������� �� ������ ALL ��� �������� ����� - ������� � ����� ������������ ��������� (�������� � ��������).
					if ( (Timer0CountersStr.Timer0LongPressSPButtomCounter >= 255) && (ButtonFacePanelStr.bit.ALL_BUT & !ButtonFacePanelStr.bit.EM_BUT) )
						{
							Timer0FlagReg2 ^= Timer0_StartTestAllIndicatorsFlag;
							//Timer0CountersStr.Timer0LongPressSPButtomCounter = 0;
							//Timer0CountersStr.Timer0LongPressSPButtomTimeout = 100;
						}
					//-------------------------
				}
			else
				{
					Timer0CountersStr.Timer0LongPressSPButtomCounter = 0;
					Timer0CountersStr.Timer0LongPressSPButtomTimeout = 100;
				}
		}
	//----------------------------------------------------
	//��������� ������ �� "PROT".
	if (PROT_PIN & PROT_LEG)
		{
			IndicatorsFacePanelStr.bit.PROT_LED = 0;
		}
	else
		{
			IndicatorsFacePanelStr.bit.PROT_LED = 1;
		}
	//----------------------------------------------------
	//����� �� ��������� ����� ������ 2.
	if (Timer0FlagReg2 & Timer0_StartPtotocolForRA2Flag)
		{
			//usart_cycle_count_modify();
		}	
	//----------------------------------------------------
	//����� �� ��������� ����� ������ 1.
	if (Timer0FlagReg2 & Timer0_StartPtotocolForRA1Flag)
		{
			//rs485_protocol();
		}
	//----------------------------------------------------
}
//-----------------------------------------------------------------------------
//���������� ���������� �������0. ������ 1 ����.
ISR(TIMER0_OVF_vect){
	
	//----------------------------------------------------
	//��������� �������� �������.
	TCNT0 = TIMER0_1ms_CONST;
	//----------------------------------------------------
	//������������� ��������.
	Timer0CountersStr.Timer0Delay++;
	//----------------------------------------------------
	//������ �������������� ���.
	//ADCSRA |= (1<<ADSC);
	//----------------------------------------------------
//  	if (Timer0FlagReg2 & Timer0_StartRequestFlag)
//  		{
//	  		Timer0FlagReg2 &= ~Timer0_StartRequestFlag;
//	  		send_request_from_RA2();
//  		}
	//----------------------------------------------------
// 	if (Timer0FlagReg2 & Timer0_StartPtotocolForRA2Flag)
// 		{
// 			if (DE_RE_PIN & DE_RE)
// 				{
// 					usart_start_recive();
// 				}
// 		}
	//----------------------------------------------------
	if(++Timer0CountersStr.Timer0_RX_LED_Counter == TIMER0_TX_RX_LED_CONST)
	{
		//RX_LED_PORT ^= RX_LED;
		Timer0CountersStr.Timer0_RX_LED_Counter = 0;
	}
	//----------------------------------------------------
	if(++Timer0CountersStr.Timer0_TX_LED_Counter == TIMER0_TX_RX_LED_CONST)
	{
		//TX_LED_PORT ^= TX_LED;
		Timer0CountersStr.Timer0_TX_LED_Counter = 0;
	}
	//----------------------------------------------------
}
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------