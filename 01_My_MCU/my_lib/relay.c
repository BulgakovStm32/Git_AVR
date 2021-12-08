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

#include "my_lib\relay.h"

//-----------------------------------------------------------------------------
static volatile uint8_t EmergencyInputSost	 = 0;
static volatile uint8_t SpeakerRelaySost	 = 0;
static volatile uint8_t LineCheckerRelaySost = 0;
static volatile uint8_t SostDD4 			 = 0;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void  relay_em_init(void){
	
//  	ACSR |= (1 << ACD);//���������� ����������� �����������.
//  	SFIOR|= (1 << PUD);//���������� ���������� ��������.
//  	TCCR2 = 0x00;	   //������ �������� �� ������ O�2
 	//--------------------------
	//������������ ������ � ��������� �������� MC74HC165A(DD7).
	WRITE_DDR |= WRITE_LEG;		//����� �� �����
	WRITE_PORT |= WRITE_LEG;	//1 �� �����.
	//--------------------------
	//���������� ���� ALL.
	ALL_DDR	&= ~ALL_LEG;		//����� �� ����
	ALL_PORT &= ~ALL_LEG;		// Z-���������.
	//--------------------------
	//���������� ���� RELE_3.
	RELE_3_DDR |= RELE_3_LEG;	//����� �� �����
	RELE_3_PORT &= ~RELE_3_LEG; //0 �� �����.
	//--------------------------
	//���������� ���� ��.
	POWER_AMPLIFIER_DDR |= POWER_AMPLIFIER_LEG;	 //����� �� �����
	POWER_AMPLIFIER_PORT &= ~POWER_AMPLIFIER_LEG;//0 �� �����.
	//--------------------------
	//����������� POWER. 
	POWER_SWITCH_DDR  &= ~ POWER_SWITCH_LEG;//����� �� ����
	POWER_SWITCH_PORT &= ~ POWER_SWITCH_LEG;// Z-���������.
	//--------------------------
	//�������� ������ �� "PROT".
	PROT_DDR  &= ~ALL_LEG;		//����� �� ����
	PROT_PORT &= ~ALL_LEG;		// Z-���������.
	//--------------------------
	spi_init();
	//--------------------------
	
	
// 	ACSR |= (1 << ACD);//���������� ����������� �����������.
// 	SFIOR|= (1 << PUD);//���������� ���������� ��������.
// 	TCCR2 = 0x00;	   //������ �������� �� ������ O�2	
// 	//--------------------------	
// 	//����� � ���� ���� ��������-�������.
// 	D0D4_DDR |= (D4 | D3 | D2 | D1 | D0);	//������ D0-D7 �� �����.
// 	D5D7_DDR |= (D7 | D6 | D5);				//
// 	
// 	D0D4_PORT &= ~(D4 | D3 | D2 | D1 | D0); //����� ����� ������ � ����.
// 	D5D7_PORT &= ~(D7 | D6 | D5);			//
// 
// 	C0C3_DDR  |=  (C0 | C1 | C3);			//����� ������� �� �����.
// 	
// 	_delay_us(10);
// 	C0C3_PORT &= ~(C0 | C1 | C3);			//� � ����.
// 	_delay_us(10);
// 	C0C3_PORT |=  (C0 | C1 | C3);			//����� ����� �� ��� �������� �������.
// 	_delay_us(10);
// 	C0C3_PORT &= ~(C0 | C1 | C3);			//
// 	//--------------------------
// 	EN_K555AP5_PORT |= EN_K555AP5_LEG;		//������ �555��5 � Z-���������.
// 	EN_K555AP5_DDR	|= EN_K555AP5_LEG;		//
// 	//--------------------------
// 	//���������� ���� ALL. ����� �� ����.
// 	ALL_DDR  &= ~ALL_LEG;
// 	ALL_PORT &= ~ALL_LEG;
// 	//--------------------------
// 	//����������� POWER. ����� �� ����.
// 	POWER_SWITCH_DDR  &= ~ POWER_SWITCH_LEG;
// 	POWER_SWITCH_PORT &= ~ POWER_SWITCH_LEG;
// 	//--------------------------
// 	//���������� ���� ��.	 ����� �� ����� � � �����.
// 	//����� ������� EM1+EM1. ����� �� ����� � � �����.
// 	//����� ������� EM2+EM2. ����� �� ����� � � �����.
// 	//�������� ������ "24V". ����� �� ����� � � �����.
// 	DDRC  |=  (POWER_AMPLIFIER_LEG | EM1Out_LEG | EM2Out_LEG | _24VOut_LEG);
// 	PORTC &= ~(POWER_AMPLIFIER_LEG | EM1Out_LEG | EM2Out_LEG | _24VOut_LEG);
	//--------------------------
}
//-----------------------------------------------------------------------------
void set_relay(uint8_t byte, uint8_t what){
	
	static uint8_t buf[3] = {0, 0, 0};
	//-------------------------
	//���������� ��������� �����.
	if (what == SPEAKER_RELAY)
		{
			byte &= get_blocked_speaker_line();
		}
	//-------------------------
 	//������������ ������ ������ � �������-�������.
 	switch(what)
 		{
 			//-------------------------
 			case(SPEAKER_RELAY):
 				buf[2] = byte;
 				SpeakerRelaySost = byte;//�������� ��������� ����.
 			break;
 			//-------------------------
 			case(LINE_CHECKER_RELAY):
				buf[1] = byte;
 				LineCheckerRelaySost = byte;//�������� ��������� ����.
 			break;
 			//-------------------------
 			case(ChipDD4):	
				buf[0] = byte;
 				SostDD4 = byte;//�������� ���������.
 			break;
 			//-------------------------
 		}
 	//-------------------------
	EmergencyInputSost = spi_write_array(3, buf);
// 	//-------------------------
}
//-----------------------------------------------------------------------------
uint8_t get_relay_state(uint8_t sp_or_lc){
	
	//cli(); //������ ����������.	
		
	if (sp_or_lc == SPEAKER_RELAY)
		{
			return SpeakerRelaySost;
		}
	if (sp_or_lc == LINE_CHECKER_RELAY)
		{
			return LineCheckerRelaySost;
		}
		
	//sei(); //���������� ����������.
	
	return 0;
}
//-----------------------------------------------------------------------------
void power_amplifier_control(uint8_t state){
	
	//--------------------------------------------------		
	if (state == POWER_ON)
		{
			if (POWER_AMPLIFIER_PIN & POWER_AMPLIFIER_LEG)//��� �������� ������.
				{
					return;
				}
			//����� ��������� ������� ���������� ���� �� � �����. ��� �� �� ���� �������.
			select_audio_source(OFF_SOURCE);	
			//--------------------------------------------------	
			ms_delay(10);
			POWER_AMPLIFIER_PORT |= POWER_AMPLIFIER_LEG; //��������� ����.
			ms_delay(POWER_ON_DELAY);
			return;
		}
	//--------------------------------------------------
	if (state == POWER_OFF)
		{
			if ((POWER_AMPLIFIER_PIN & POWER_AMPLIFIER_LEG) == 0)//��� �������� ������.
				{
					return;
				}
			//ms_delay(POWER_OFF_DELAY);
			POWER_AMPLIFIER_PORT &= ~POWER_AMPLIFIER_LEG;
		}
	//--------------------------------------------------
}	
//-----------------------------------------------------------------------------
//uint8_t	get_state(volatile uint8_t *pin, uint8_t leg){
	//
	//uint8_t em_input1 = 0;
	//uint8_t em_input2 = 0;
	//uint8_t em_input3 = 0;
	//-------------------------------
	//���������� �������� ����� ��� ���� ��� ������������� ����������� ���������.
	//em_input1 = (*pin & leg);
	//ms_delay(5);//_delay_ms(5);
	//em_input2 = (*pin & leg);
	//ms_delay(5);//_delay_ms(5);
	//em_input3 = (*pin & leg);
	//-------------------------------
	//������������ ����������� ��������� ������.
	//return ( (~((em_input1 & em_input2) | (em_input2 & em_input3) | (em_input1 & em_input3))) & leg);
	//return (~(*pin & leg));
//}
//-----------------------------------------------------------------------------
uint8_t get_emergency_input(void){
	
	return (EmergencyInputSost);
}
//-----------------------------------------------------------------------------
//���������� ��������� ��������.
void set_discret_output(uint8_t output, uint8_t state){
	
	//cli(); //������ ����������.
	//-------------------------------
 	switch(output)
 		{	
 			//---------------------------
 			case(EM1_OUTPUT):
			 	//----------------------
			 	if (state == ACTIVATE)
			 		{
				 		SostDD4 |= EM1_DD4;
			 		}
			 	//----------------------
			 	if (state == DEACTIVATE)
			 		{
				 		SostDD4 &= ~EM1_DD4;
			 		}
			 	//----------------------
				SostDD4 |= _24V_DD4;
 			break; 
 			//---------------------------
 			case(EM2_OUTPUT):
		 		//----------------------
		 		if (state == ACTIVATE)
		 			{
			 			SostDD4 |= EM2_DD4;
		 			}
		 		//----------------------
		 		if (state == DEACTIVATE)
		 			{
			 			SostDD4 &= ~EM2_DD4;
		 			}
		 		//----------------------
		 		SostDD4 |= _24V_DD4;
 			break;
 			//---------------------------
 			case(_485_OUTPUT):
 				//----------------------
 				if (state == ACTIVATE)
 					{
 						SostDD4 |= _485_DD4;
 					}
 				//----------------------
 				if (state == DEACTIVATE)
 					{
 						SostDD4 &= ~_485_DD4;
 					}
 				//----------------------
 			break;
 			//---------------------------
 			case(LC_OUTPUT):
 				//----------------------
 				if (state == ACTIVATE)
 					{
 						SostDD4 |= LC_DD4;
 					}
 				//----------------------
 				if (state == DEACTIVATE)
 					{
 						SostDD4 &= ~LC_DD4;
 					}
 				//----------------------			
 			break;
 			//---------------------------
  			case(_24V_OUTPUT):
				SostDD4 |= _24V_DD4;
  			break;
 			//---------------------------
			case(ZUMMER): 
				//----------------------
				if (state == ACTIVATE)
					{
						SostDD4 |= ZUMMER_DD4;
					}
				//----------------------
				if (state == DEACTIVATE)
					{
						SostDD4 &= ~ZUMMER_DD4;
					}
				//----------------------
			break; 
			//--------------------------- 
  			case(ALL_OUTPUT_OFF):
				SostDD4 &= ~(_24V_DD4 | EM2_DD4 | EM1_DD4);
				
  			break;
 			//---------------------------
 		}
 	//sei(); //���������� ����������.
 	//-------------------------------------------
 	//������ ������ � ������� �������.
 	set_relay(SostDD4, ChipDD4);
 	//-------------------------------------------
}
//-----------------------------------------------------------------------------
//����� ��������� ������� ��� ��.
void select_audio_source(uint8_t source){
	
	//cli(); //������ ����������.
	//--------------------------
	switch(source)
		{
			//--------------------------
			case (AUX):
				hef4052_master_in(HEF4052_MASTER_IN_LIN_OUT);
				hef4052_um(HEF4052_UM_MASTER_OUT);
				RELE_3_PORT &= ~RELE_3_LEG;//���������� �������������� �� �����.
			break;
			//--------------------------
			case (CONSOL):
				hef4052_master_in(HEF4052_MASTER_IN_CONSOL);
				hef4052_um(HEF4052_UM_MASTER_OUT);
				RELE_3_PORT |= RELE_3_LEG;//����������� �������������� � ������.
			break;
			//--------------------------
			case (EM1):
				hef4052_master_in(HEF4052_MASTER_IN_ALL_OFF);
				hef4052_um(HEF4052_UM_EM2);	
				RELE_3_PORT &= ~RELE_3_LEG;//���������� �������������� �� �����.
			break;
			//--------------------------
			case (EM2):
				hef4052_master_in(HEF4052_MASTER_IN_ALL_OFF);
				hef4052_um(HEF4052_UM_EM1);
				RELE_3_PORT &= ~RELE_3_LEG;//���������� �������������� �� �����.
			break;
			//--------------------------
			case (MIC):
				hef4052_master_in(HEF4052_MASTER_IN_MIC_OUT);
				hef4052_um(HEF4052_UM_MASTER_OUT);
				RELE_3_PORT &= ~RELE_3_LEG;//���������� �������������� �� �����.
			break;
			//--------------------------
			case (OFF_SOURCE):
				hef4052_master_in(HEF4052_MASTER_IN_ALL_OFF);
				hef4052_um(HEF4052_UM_ALL_OFF);
				RELE_3_PORT &= ~RELE_3_LEG;//���������� �������������� �� �����.
			break;
			//--------------------------
		}
	//sei(); //���������� ����������.
	//--------------------------
}
//-----------------------------------------------------------------------------
void consol_select_audio_channel(uint8_t channel){
	
 	switch(channel)
 		{
 			//---------------------------
//   			case(0):
//   				select_audio_source(OFF_SOURCE);//���� ��������� �������� � ������.
//   			break;
 			//---------------------------
 			case(1):
 				SostDD4 &= ~(RELE1_DD4 | RELE2_DD4);
 			break;
 			//---------------------------
 			case(2):
 				SostDD4 &= ~(RELE1_DD4);
 				SostDD4 |=  (RELE2_DD4);
 			break;
 			//---------------------------
 			case(3):
 				SostDD4 |= (RELE1_DD4 | RELE2_DD4);
 			break;
 			//---------------------------
 		}	
 	//---------------------------
 	//������ ������ � ������� �������.
 	set_relay(SostDD4, ChipDD4);
 	//---------------------------
}
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------


