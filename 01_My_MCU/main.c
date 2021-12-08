//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
/*
 * roxton_ra_rp_hw6_sw003.c
 *
 * Created: 10.10.2018 15:09
 * Author : anton
 */ 
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------

//����������� ���������!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define SW_Version	61	//������ �� �����.������ ����� - ��� ������ ������, ������ ����� - ������ ��!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#include "my_lib/main.h"

//-----------------------------------------------------------------------------
extern __IndicatorFacePanel_TypeDef		IndicatorsFacePanelStr;
extern __ButtonSost_TypeDef				ButtonSostStr;
//------------------------------
__Configuration_TypeDef					ConfigurationStr;//��������� ��� �������� ������������.

//uint8_t EMSelectZone = 0xFF;	//�������� ���� ��� ������� �� ������ ���.
//******************************************************************************************************************
//******************************************************************************************************************
static void led_ALL_off(void){
	
	//--------------------------------
	 	cli(); //������ ����������.
	IndicatorsFacePanelStr.bit.ALL_LED = 0;
	ButtonSostStr.STR.AllMicButtonSost &= ~(ALL_BUTTON);
	 	sei(); //���������� ����������.
	//--------------------------------
}
//-----------------------------------------------------------------------------------------------------------------
//�������� ���������.
static void zummer_indication (uint8_t number_of_times, uint16_t interval){

	do 
		{
			set_discret_output(ZUMMER, ACTIVATE);
			ms_delay(interval);
			set_discret_output(ZUMMER, DEACTIVATE);
			ms_delay(interval);
		} 
	while (--number_of_times);
}
//-----------------------------------------------------------------------------------------------------------------
//������������ ���������.
static void test_indicators(uint8_t byte){
	
// 	set_speaker_led_red(byte);		
// 	set_speaker_led_green(byte);		
// 	
// 	set_line_checker_led_red(byte);	
// 	set_line_checker_led_green(byte);	
// 	
// 	IndicatorsFacePanelStr.bit.ALL_LED   = byte;
// 	IndicatorsFacePanelStr.bit.RS485_LED = byte;
// 	IndicatorsFacePanelStr.bit.PROT_LED  = byte;
// 	IndicatorsFacePanelStr.bit.POWER_LED = byte;
// 	IndicatorsFacePanelStr.bit.STBAY_LED = byte;
// 	
// 	if (byte)
// 		{
// 			display_set(0x88);//��������� "88"
// 			set_discret_output(ZUMMER, ACTIVATE);
// 		}
// 	else
// 		{
// 			display_off();
// 			set_discret_output(ZUMMER, DEACTIVATE);
// 		}
	//--------------------------------
	//��� �������� �� 44 ����� ������.
	//���� �������.	
	if (byte)
	 	{
	 		set_discret_output(ZUMMER, ACTIVATE);
	 	}
	else
	 	{
	 		set_discret_output(ZUMMER, DEACTIVATE);
	 	}
	//���� ����������� ������� ������.
	IndicatorsFacePanelStr.BLK[0] = byte;
	IndicatorsFacePanelStr.BLK[1] = byte;
	IndicatorsFacePanelStr.BLK[2] = byte;
	IndicatorsFacePanelStr.BLK[3] = byte;
	IndicatorsFacePanelStr.BLK[4] = byte;
	IndicatorsFacePanelStr.BLK[5] = byte;
	IndicatorsFacePanelStr.BLK[6] = byte;
	IndicatorsFacePanelStr.BLK[7] = byte;
	IndicatorsFacePanelStr.BLK[8] = byte;
	IndicatorsFacePanelStr.BLK[9] = byte;
	IndicatorsFacePanelStr.BLK[10] = byte;
}
//-----------------------------------------------------------------------------------------------------------------
//����� ����������������.
static void configuration_mode (void){
	
	//--------------------------------
	CHARGE_OFF;									 //���������� ����� ��
	clear_charge_indication();					 //� ������� ��������� "��".
	set_discret_output(ZUMMER, DEACTIVATE);		 //���������� �������� ���������.
	set_discret_output(_485_OUTPUT, DEACTIVATE); //���������� �������� RS485.
	set_discret_output(LC_OUTPUT, DEACTIVATE);	 //���������� �������� LC.
	IndicatorsFacePanelStr.bit.RS485_LED = 0;	 //���������� �������� ��������� RS485.
	set_relay(ALL_RELAY_OFF, SPEAKER_RELAY);	 //���������� ����� ����������������� �� ���������.
	set_relay(ALL_RELAY_OFF, LINE_CHECKER_RELAY);//���������� ����� �������� ������ �� �����.
	select_audio_source(OFF_SOURCE);			 //����� ��������� ����� - ���� ��������� �������� � ������.
	power_amplifier_control(POWER_OFF);			 //���������� ��.
	set_speaker_led_green(ALL_LED_OFF);			 //������� ������� �����������.
	set_line_checker_led_green(ALL_LED_OFF);	 //������� ������� �����������.
	set_line_checker_led_red(ALL_LED_OFF);		 //������� �������� �����������.
	display_off();								 //������� �������. 
	led_ALL_off();								 //������� ���������� ALL.
	//--------------------------------
	while (Timer0FlagReg & Timer1_ConfigModeFlag)
		{
			//--------------------------------------
			set_speaker_led_red(Timer1BlinkReg);  //������� ������������ SPEACKER.
			//--------------------------------------
			//����������� ��������� ���������
			switch(config_parameter_selection(CONFIG_START))
				{	//----------------------------
					//����� ������ �� � ������ ��������� �� ������� ��� ������� ������ SP8.
					case(SW_VERSION):
						display_set(bin_to_dec(SW_Version) );//����� �� ������� ������ ��.
					break;
					//----------------------------
					//��������� ������.
					case(CONFIG_PARAMETER_ADDRES):
						set_line_checker_led_green(LC1_LED);
						display_set( bin_to_dec(ConfigurationStr.STR.Address) );//����� �������������� ������ �� �������.
						modification_config_parameter(&ConfigurationStr.STR.Address, CONFIGURATION_ADDRESS_MIN,CONFIGURATION_ADDRESS_MAX);
					break;
					//----------------------------
					//��������� ������.
					case(CONFIG_PARAMETER_GROUP):
						set_line_checker_led_green(LC2_LED);
						display_set(drawH | ConfigurationStr.STR.Group);
						modification_config_parameter(&ConfigurationStr.STR.Group,CONFIGURATION_GROUP_MIN, CONFIGURATION_GROUP_MAX);
					break;
					//----------------------------
					//���-���� �������� ���.
					case(CONFIG_PARAMETER_CHARGE_CHECK):
						set_line_checker_led_green(LC3_LED);
						//display_set(drawOF - ConfigurationStr.STR.ChargeCheck);
						if (ConfigurationStr.STR.ChargeCheck)
							{
								display_set(0x3F);
							}
						else
							{
								display_set(0x0B);
							}						
						modification_config_parameter(&ConfigurationStr.STR.ChargeCheck,CONFIGURATION_CHARGE_CHECK_MIN, CONFIGURATION_CHARGE_CHECK_MAX);
					break;
					//----------------------------
					//���-���� ������� �������� RS-485.
					case(CONFIG_PARAMETER_RS485_CHECK):
						set_line_checker_led_green(LC4_LED);
						//display_set(drawOF - ConfigurationStr.STR.RS485Check);
						if (ConfigurationStr.STR.RS485Check)
							{
								display_set(0x3F);
							} 
						else
							{
								display_set(0x0B);
							}
						modification_config_parameter(&ConfigurationStr.STR.RS485Check,CONFIGURATION_RS485_CHECK_MIN, CONFIGURATION_RS485_CHECK_MAX);
					break;
					//----------------------------
					//��������� ������� ��������������� �������� �����.
					case(CONFIG_PARAMETER_TIME_AUTO_CHECK):
						set_line_checker_led_green(LC5_LED);
						display_set( bin_to_dec(ConfigurationStr.STR.TimeAutoCheck) );
						modification_config_parameter(&ConfigurationStr.STR.TimeAutoCheck,CONFIGURATION_TIME_AUTO_CHECK_MIN, CONFIGURATION_TIME_AUTO_CHECK_MAX);
					break;
					//----------------------------
					//��������� �������.
					case(CONFIG_PARAMETER_BUZZER):
						set_line_checker_led_green(LC6_LED);
						if (ConfigurationStr.STR.Buzzer)
							{
								display_set(0x3F);
							}
						else
							{
								display_set(0x0B);
							}
						modification_config_parameter(&ConfigurationStr.STR.Buzzer,CONFIGURATION_BUZZER_MIN, CONFIGURATION_BUZZER_MAX);
					break;
					//----------------------------
					//��������� ������ ������������ LC.
					case(CONFIG_PARAMETER_LEVEL_LC):
						set_line_checker_led_green(LC7_LED);
						//��������� �������������� ������. ����� ����������� �������� ������ �����.
// 						if (ConfigurationStr.STR.LevelLC == LEVEL_5_PERCENT)
// 							{
// 								display_set( bin_to_dec(5) );
// 							} 
// 						else 
						if (ConfigurationStr.STR.LevelLC == LEVEL_10_PERCENT)
							{
								display_set( bin_to_dec(10) );
							}
						else if (ConfigurationStr.STR.LevelLC == LEVEL_25_PERCENT)
							{
								display_set( bin_to_dec(25) );
							}
						else if (ConfigurationStr.STR.LevelLC == LEVEL_40_PERCENT)
							{
								display_set( bin_to_dec(40) );
							}
						else if (ConfigurationStr.STR.LevelLC == LEVEL_65_PERCENT)
							{
								display_set( bin_to_dec(65) );
							}
						//----------------------------
						modification_config_parameter(&ConfigurationStr.STR.LevelLC,CONFIGURATION_LEVEL_LC_MIN, CONFIGURATION_LEVEL_LC_MAX);
					break;
					//----------------------------
					//��������� ������ �������������� �����.
// 					case(CONFIG_PARAMETER_RESERVATION):
// 						set_line_checker_led_green(LC8_LED);
// 						//----------------------------
// 						if (ConfigurationStr.STR.Reservation == Reserv_OFF)
// 							{
// 								display_set( 0x1000 );//��������� "��" - ������ ��������.
// 							}
// 						else if (ConfigurationStr.STR.Reservation == Reserv_MASTER)
// 							{
// 								display_set( 0x000B );//��������� "OC" - ��������.
// 							}
// 						else if (ConfigurationStr.STR.Reservation == Reserv_SLAVE)
// 							{
// 								display_set( 0x1011 );//��������� "��" - ���������.
// 							}
// 						//----------------------------
// 						modification_config_parameter(&ConfigurationStr.STR.Reservation, CONFIG_PARAMETER_RESERVATION_MIN, CONFIG_PARAMETER_RESERVATION_MAX);
// 					break;
					//----------------------------
				}
			//--------------------------------------
			//����� ������������ ��������� (�������� � ��������).
			if (Timer0FlagReg2 & Timer0_StartTestAllIndicatorsFlag)
				{
					while(1)
						{
							//----------------------------
							//������� ������������.
							test_indicators(Timer1BlinkReg);
							//����������� ������� �������� ��������� ������� ������.
							modification_config_parameter(&DisplayBright, CONFIG_PARAMETER_DISPLAY_BRIGHT_MIN, CONFIG_PARAMETER_DISPLAY_BRIGHT_MAX);
							TempDisplayBright = DisplayBright;
							//----------------------------
							//����� �� ������ ������������ ���������.
							if ( (Timer0FlagReg2 & Timer0_StartTestAllIndicatorsFlag)==0 )
								{
									//���������� ���� �����������.
									test_indicators(0x00);
									//�������� ���������� ������.
									while(get_button(ALL_EM_BUTTON) & ALL_BUTTON){ }	
									break;
								}
							//----------------------------
						}			
				}
			//--------------------------------------
		}
	//--------------------------------------
	display_set(bin_to_dec(ConfigurationStr.STR.Address));//����� �������������� ������ �� �������.
	config_parameter_selection(CONFIG_END);		//��� ����� ��� �� ��������� ���������������� ���������� � ������� ���������. 
	eep_write_configuration(&ConfigurationStr);	//������ ������������������ ������������.
	set_speaker_led_red(ALL_LED_OFF);			//������� ����������� LINE CHECKER.
	set_line_checker_led_red(ALL_LED_OFF);		//������� �������� �����������.
	set_line_checker_led_green(ALL_LED_OFF);	//������� ����������� SPEAKER.
	//--------------------------------------
	measure_resistence_all_line_after_power_on(ConfigurationStr.STR.LevelLC);//��������� � ���������� ���������� �����. 
	//--------------------------------------
	Timer0FlagReg &= ~Timer1_15secFlag;	  //����� ����� 15 ��� �
	clear_hour_counter();				  //�������� �����.
	//����� �������� � ������ ��.
	clear_charge_hour_counter();		  //
	Timer0FlagReg &= (Timer1_Charge15secFlag | Timer1_ChargeChekrFlag);
	//����� ��������� ������ ��� � ALL, ��� �� ��� ������ �� ���������������� �� ���������� � ��������� ���
	//� �� �������������� ��� ���� �� ������ ��������� ������ ALL.
	ButtonSostStr.STR.AllMicButtonSost &= ~(EM_BUTTON | ALL_BUTTON);
	//--------------------------------------
}
//-----------------------------------------------------------------------------------------------------------------
//������ ���������� ���������� ���.
static void manual_control_select_zone(void){

	//-----------------------------------------------------------------------------------
	//������ ������ ALL.	
	if (toggle_button(ALL_EM_BUTTON) & ALL_BUTTON)
		{
			if (toggle_button(SPEAKER_BUTTON) == 0xFF)//���� ��� ���� ��������....
				{
					set_relay(ALL_RELAY_OFF, SPEAKER_RELAY);
					set_speaker_led_green(ALL_LED_ON & get_blocked_speaker_line());
					IndicatorsFacePanelStr.bit.ALL_LED = 0;
					ButtonSostStr.STR.SpeakerButtonSost = 0;
					ButtonSostStr.STR.AllMicButtonSost &= ~(ALL_BUTTON);
				}
			else
				{
					//���� ���� �������� ��������, �� �������� ��� ����.
					set_relay(ALL_RELAY_ON, SPEAKER_RELAY);
					set_speaker_led_green(ALL_LED_ON & get_blocked_speaker_line());
					IndicatorsFacePanelStr.bit.ALL_LED = 1;
					ButtonSostStr.STR.SpeakerButtonSost = 0xFF;
					ButtonSostStr.STR.AllMicButtonSost &= ~(ALL_BUTTON);
				}
		}
	//-----------------------------------------------------------------------------------
	else
		{
			//--------------------------------
			//���� �������� ��� ���� �� ������ ��������� ALL.
			if (toggle_button(SPEAKER_BUTTON) == 0xFF)
					{IndicatorsFacePanelStr.bit.ALL_LED = 1;}
			else    {IndicatorsFacePanelStr.bit.ALL_LED = 0;}
			//--------------------------------
			set_relay(toggle_button(SPEAKER_BUTTON), SPEAKER_RELAY);
			set_speaker_led_green( get_relay_state(SPEAKER_RELAY) );
		}
	//-----------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------------------------------------
//������� �����.
static void work_mode(void){
	
	//--------------------------------
	power_led_green();									  //��������� �������� ���������� POWER.
	set_speaker_led_red(ALL_LED_OFF);					  //������� ������� ����������� ��� �������� �� ������� ����������.
	display_set(bin_to_dec(ConfigurationStr.STR.Address));//����� �������������� ������ �� �������.
	//--------------------------------
	power_amplifier_control(POWER_ON);					  //��������� ��.
	//--------------------------------
	//����� ��������� �����. ���� ���� �������� ����, �� ���� �� ������������ � AUX.
	if (get_relay_state(SPEAKER_RELAY))
		{
			select_audio_source(AUX);		
		}
	//���� ��� �������� ���, �� ���� �� �� �����.
	else
		{
			select_audio_source(OFF_SOURCE);
		}							  
	//--------------------------------
	manual_control_select_zone();						  //������ ����� ���.
	//--------------------------------
}
//-----------------------------------------------------------------------------------------------------------------
//�������� �����.
static void standby_mode(void){
	
	//--------------------------------
	power_led_blink_green();			//�������� ������� ��������� POWER
	set_speaker_led_green(ALL_LED_OFF);	//����� ��� ������� ����������.
	set_speaker_led_red(ALL_LED_OFF);	//����� ��� ������� ����������.
	display_off();						//���������� �������.
	//--------------------------------
	set_relay(ALL_RELAY_OFF,SPEAKER_RELAY);//���������� ����� ����������������� �� ��.
	select_audio_source(OFF_SOURCE);	//����� ��������� ����� - ���� ��������� �������� � ������.
	power_amplifier_control(POWER_OFF);	//���������� ��.
	//--------------------------------
	led_ALL_off();
// 	cli(); //������ ����������.
//   	IndicatorsFacePanelStr.bit.ALL_LED = 0;
//   	ButtonSostStr.STR.AllMicButtonSost &= ~(ALL_BUTTON);
// 	sei(); //���������� ����������.
	//--------------------------------
}
//-----------------------------------------------------------------------------------------------------------------
//���������� � ������� ��������� �������� RM-8064.
static void consol_control(void){
	
	uint8_t temp = get_consol_control();					//��������� ���������� �� �������.
	//--------------------------------	
	//��� ��������� ��������� �� �������� � 5 �� 9 ����� ������������ ������� +24�.
	if ( (get_consol_number() >= 5) && (get_consol_number() <= 9) )
		{
			set_discret_output(_24V_OUTPUT, ACTIVATE);		//��������� �������� ������ +24.
		}
	else
		{
			set_discret_output(ALL_OUTPUT_OFF, ACTIVATE);	//����� �������� ����� EM1, EM2 � +24.
		}
	//--------------------------------	
	power_led_green();										//��������� �������� ���������� POWER.	
	display_set(0xD0 | get_consol_number());				//����� ������ ������� �� �������.
	//--------------------------------
	power_amplifier_control(POWER_ON);						//��������� ��.	
	select_audio_source(CONSOL);							//����� ��������� ����� ��� ��.
	consol_select_audio_channel(get_consol_audio_chanel());	//����� ����������� �������.
	//--------------------------------
	set_relay(temp, SPEAKER_RELAY);							//����������� ����� ����������������� � ��.
 	set_speaker_led_green(ALL_LED_OFF);						//����� ��� ������� ����������.
 	set_speaker_led_red(temp & get_blocked_speaker_line());	//��������� ��������������� ������� �����������.
	//--------------------------------
	led_ALL_off();
// 	cli(); //������ ����������.
//   	IndicatorsFacePanelStr.bit.ALL_LED = 0;
//   	ButtonSostStr.STR.AllMicButtonSost &= ~(ALL_BUTTON);
// 	sei(); //���������� ����������.
	//--------------------------------
}
//-----------------------------------------------------------------------------------------------------------------
//���������� � ������� ������ EMERGENCY INPUT ALL.
static void emergency_input_all_control(void){
	
	//--------------------------------
	//set_relay(ALL_RELAY_OFF, LINE_CHECKER_RELAY);//���������� ���� LINE_CHECKER.
	power_led_green();											 //��������� �������� ���������� POWER.
	set_speaker_led_green(ALL_LED_OFF);							 //����� ��� ������� ����������.	
	set_speaker_led_red(ALL_LED_ON & get_blocked_speaker_line());//��������� ������� �����������.
	display_set(0xE3);											 //����� �� ������� �3.
	//--------------------------------
	power_amplifier_control(POWER_ON);							 //��������� ��.
	select_audio_source(EM2);									 //����� ��������� �����
	set_relay(ALL_RELAY_ON, SPEAKER_RELAY);						 //����������� ����� ����������������� � ��.
	//--------------------------------
	set_discret_output(EM2_OUTPUT, ACTIVATE);					 //��������� ����� EM2 � 24V.
	//--------------------------------
	led_ALL_off();
// 	cli(); //������ ����������.
//   	IndicatorsFacePanelStr.bit.ALL_LED = 0;
//   	ButtonSostStr.STR.AllMicButtonSost &= ~(ALL_BUTTON);
//	sei(); //���������� ����������.
	//--------------------------------
}
//-----------------------------------------------------------------------------------------------------------------
//���������� � ������� ����� EMERGENCY INPUT1 - INPUT8.
static void emergency_input_1_8_control(void){
	
	uint8_t temp = get_emergency_input();					//��������� ��������� ����� EM1-EM8.
	//--------------------------------
	//set_relay(ALL_RELAY_OFF, LINE_CHECKER_RELAY);	//���������� ���� LINE_CHECKER.
	power_led_green();										//��������� �������� ���������� POWER.
	set_speaker_led_green(ALL_LED_OFF);						//����� ��� ������� ����������.	
	set_speaker_led_red(temp & get_blocked_speaker_line());	//��������� ��������������� ������� �����������.
	display_set(0xE2);										//����� �� ������� �2.
	//--------------------------------	
	power_amplifier_control(POWER_ON);						//��������� ��.
	select_audio_source(EM1);								//����� ��������� �����
	set_relay(temp, SPEAKER_RELAY);							//����������� ����� ����������������� � ��.
	//--------------------------------
	set_discret_output(EM1_OUTPUT, ACTIVATE);				//��������� ����� EM1 � 24V.
	//--------------------------------
	led_ALL_off();
// 	cli(); //������ ����������.
//   	IndicatorsFacePanelStr.bit.ALL_LED = 0;
//   	ButtonSostStr.STR.AllMicButtonSost &= ~(ALL_BUTTON);
// 	sei(); //���������� ����������.
	//--------------------------------
}
//-----------------------------------------------------------------------------------------------------------------
//���������� � ������� ������ EM(���).
//�� ����������� ������� (����� 1 ���) ������ SP ��������������� ���� ��������������/������������.
static void emergency_button_control(void){
	
	//--------------------------------
	//set_relay(ALL_RELAY_OFF, LINE_CHECKER_RELAY);//���������� ���� LINE_CHECKER.
	power_led_green();											 //��������� �������� ���������� POWER.
	set_speaker_led_green(ALL_LED_OFF);							 //����� ��� ������� ����������.
	set_speaker_led_red(Timer1EMSelectZone & get_blocked_speaker_line());//��������� ������� �����������. �� ����������� ������� (����� 1 ���) ������ SP ��������������� ���� ��������������/������������.
	display_set(0xE1);											 //����� �� ������� �1.
	//--------------------------------
	power_amplifier_control(POWER_ON);							 //��������� ��.
	select_audio_source(MIC);									 //����� ��������� �����
	set_relay(Timer1EMSelectZone, SPEAKER_RELAY);				 //����������� ����� ����������������� � ��. �� ����������� ������� (����� 1 ���) ������ SP ��������������� ���� ��������������/������������.
	//--------------------------------	
	set_discret_output(_24V_OUTPUT, ACTIVATE);				     //��������� �������� ������ +24.
	//--------------------------------
	led_ALL_off();
// 	cli(); //������ ����������.
//   	IndicatorsFacePanelStr.bit.ALL_LED = 0;
//   	ButtonSostStr.STR.AllMicButtonSost &= ~(ALL_BUTTON);
// 	sei(); //���������� ����������.
	//--------------------------------
}
//-----------------------------------------------------------------------------------------------------------------
//����������� ��������� ����������.
static uint8_t get_priority(void){
	
	//------------------------------------------
	//����� ������ EM.
	if(toggle_button(ALL_EM_BUTTON) & EM_BUTTON)
		{
			//���� ���� �������� ������.
			while(get_button(ALL_EM_BUTTON) & EM_BUTTON)
				{
					//------------------------------------------
					//���� ���� ���������� ������� �� ������� � ����� � ��������� � ����� ����������������.
					if (Timer0FlagReg & Timer1_ConfigModeFlag)
						{
							//����� ��������� ������ ��� �� ����� ������
							//�� ������ ������������ �� �������� � ������ ���������.
							ButtonSostStr.STR.AllMicButtonSost &= ~(EM_BUTTON);
							return 0;
						}
					//------------------------------------------
				}
			return PRIORITY_1;
		}
	//------------------------------------------
	//����� ����� 1 - 8.
	if (get_emergency_input())
		{
			return PRIORITY_2;
		}
	//------------------------------------------
	//����� ����� ALL.
	if ((ALL_PIN & ALL_LEG) == 0)
		{
			return PRIORITY_3;
		}
	//------------------------------------------
	//���������� �� �������.
	if (get_consol_number())
		{
			return PRIORITY_4;
		}
	//------------------------------------------
	//������ ����������.	
	return PRIORITY_5;
}
//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
//�������� ���� ���������.
int main(void){
	
	uint8_t temp  = 0;
	//------------------------------
	charge_init();	//���������� ��.
	//------------------------------
	_delay_ms(1000);//����� ��� ������������ ���������� ���������.
	//------------------------------
	relay_em_init();//���������� ��.
	timers_init();	//
	adc_init();		//
	pt6964_init();	//
	usart_init();	//
	hef4052_init(); //
	//----------------------------------------------------------------------
	select_audio_source(OFF_SOURCE);			 //���� ��������� �������� � ������.
	set_relay(ALL_RELAY_OFF, SPEAKER_RELAY);	 //���������� ���� ����� �� ��.
	set_relay(ALL_RELAY_OFF, LINE_CHECKER_RELAY);//���������� ���� LC �� �����������������.	
	//----------------------------------------------------------------------
	//��������� ����������� ������� �� ������� ����� 2 �������. ����� ����������� ������� � ���������� �������1 ������ 10 ����.
	wdt_enable(WDTO_2S);
	//----------------------------------------------------------------------	
	//���������� ���������� ����������.
	sei();
	//----------------------------------------------------------------------
	_delay_ms(1000);							 //����� ��� ������������ ���������� ���������.
	calibration_adc_for_line_checker();			 //�������������� �������������� ������ ��� LineCheckera ��� ���������.
	//----------------------------------------------------------------------
	//�������� ���������.
	set_relay(ALL_RELAY_OFF, SPEAKER_RELAY);//���������� ���� ����� �� ��.
	for (temp = 0; temp < 9; temp++)
		{
			set_speaker_led_green((1 << (8 - temp)));
			ms_delay(60);
		}

	for (temp = 0; temp < 8; temp++)
		{
			set_speaker_led_green(1 << temp);
			ms_delay(60);
		}
	set_speaker_led_green(ALL_LED_OFF);//����� ��� ������� ����������. 	
	//----------------------------------------------------------------------
	//���������� ������������ �� ������.
	eep_read_configuration(&ConfigurationStr);
	//���� � ������ ����������� ������������ �� ������� ������������ �� ���������. 
	if (ConfigurationStr.STR.Address > MAX_ADDRESS)//���� � EEPROM 0�FF, ������ ������������ ��� �� ������������. 
		{
			//������������ �� ���������.
			ConfigurationStr.STR.Address		= ADDRESS_DEFAULT;
			ConfigurationStr.STR.Group			= GROUP_DEFAULT;
			ConfigurationStr.STR.ChargeCheck	= CHARGE_CHECK_DEFAULT;
			ConfigurationStr.STR.RS485Check		= RS485_CHECK_DEFAULT;
			ConfigurationStr.STR.TimeAutoCheck	= TIMEOUT_CHECK_DEFAULT;
			ConfigurationStr.STR.Buzzer			= BUZZER_OFF; 
			ConfigurationStr.STR.LevelLC		= LEVEL_LC_DEFAULT; 
//			ConfigurationStr.STR.Reservation	= RESERVATION_DEFAULT; 
			eep_write_configuration(&ConfigurationStr);
			eep_write_set_lines_to_control(0xFF);//��� ������ ��������� ������� ��� ����� ������������ �� ��������.
		}
	//----------------------------------------------------------------------
	//���������� ����������� POWER.
	if((POWER_SWITCH_PIN & POWER_SWITCH_LEG) == 0)
		{
			//������� � ��������� ON.
			power_led_green();									   //������� ��������� POWER.
			display_set( bin_to_dec(ConfigurationStr.STR.Address));//����� �������������� ������ �� �������.
		}			
	else
		{
			//������� � ��������� OFF.
			power_led_blink_green();								//�������� ������� ��������� POWER.
		}	
	//----------------------------------------------------------------------
	//��������� ��������� 8-�� �����. ���� �������� ����� ������� �� ����� ��������� ���
	//��������, �� ��� �������� �� �����������. ���� ��, �� ��������� �������� ������ � ��������� �������� ���������.
	measure_resistence_all_line_after_power_on(ConfigurationStr.STR.LevelLC);
	//----------------------------------------------------------------------
	//���������� �� EEPROM ������������� �� �������� �����. 
	set_lines_to_control(eep_read_set_lines_to_control());  
	//----------------------------------------------------------------------
	//������������� ������������� ���� SP � LC.
	//set_relay(ALL_RELAY_OFF, SPEAKER_RELAY);	 //���������� ���� ����� �� ��.
	//set_relay(ALL_RELAY_OFF, LINE_CHECKER_RELAY);//���������� ���� LC �� �����������������.
	//----------------------------------------------------------------------
	//������������ ����� ��� ������ ��������� � ������ ���������.
	usart_config_for_protocol(&ConfigurationStr);
	//---------------------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------------------
    while (1) 
		{
			//----------------------------------------------------------------------
			//���� ������� Timer0_EMLongPressFlag �� �������� � ����� ���������.
  			if (Timer0FlagReg & Timer1_ConfigModeFlag)
  				{
					UCSRB &= ~(1<<RXEN | 1<<RXCIE);															//���������� ������ �������� �� RA1.	
 					Timer0FlagReg2 &= ~(Timer0_StartPtotocolForRA1Flag | Timer0_StartPtotocolForRA2Flag);	//������� ���������.
					zummer_indication(3, 100);																//�������� ��������� �������� � ����� ����������������.��� ���� ������.
 	 				configuration_mode();
 					usart_config_for_protocol(&ConfigurationStr);											//������������ ��� ���������.
  				}
  			//----------------------------------------------------------------------
 			//����������� ���������� ����������.
 			temp = get_priority();
  			//----------------------------------------------------------------------
 			//��������� ���������� ������ ��� ���������.
 			priority_for_protocol(temp);
 			//----------------------------------------------------------------------
  			//���������� �� �����������.		
 			switch(temp) 
 				{	
 					//----------------------------
 					//�������� 5 (������).������ �����.
 					case(PRIORITY_5):
 						//����� �������� �����.
 						set_discret_output(ALL_OUTPUT_OFF, ACTIVATE);
 						//--------------
 						//������� �����.
 						if ((POWER_SWITCH_PIN & POWER_SWITCH_LEG) == 0)
 							{
 								work_mode();											
 							} 
 						//--------------	
 						//�������� �����.	
 						else
 							{
 								standby_mode();
 							}
 						//--------------											
 					break;
 					//----------------------------
 					//��������� 4. ���������� � ������� ��������� �������� RM-8064.
 					case(PRIORITY_4):
 						consol_control();						
 					break;
 					//----------------------------
 					//��������� 3. ���������� � ������� ������ EMERGENCY INPUT ALL.
 					case(PRIORITY_3):
 						emergency_input_all_control();			
 					break;
 					//----------------------------
 					//��������� 2. ���������� � ������� ����� EMERGENCY INPUT1 - INPUT8.
 					case(PRIORITY_2):
 						emergency_input_1_8_control();
 					break;
 					//----------------------------
 					//��������� 1 (������). ���������� � ������� ������ EM.
 					case(PRIORITY_1):
 						emergency_button_control();
 					break;
 					//----------------------------
 					default:
 					break;
 					//----------------------------
 				}
 			//----------------------------------------------------------------------
 			//��������� ��������� ������LC.
 			temp = get_button(LINE_CHECKER_BUTTON);
 			//��������� ��������� � ����������� ������� �� ������ LC.
 			if (temp | (Timer0FlagReg & Timer1_LCLongPressFlag))
 				{	
 					//-----------------------------------------
 					//�������� ���������� ������ ��� ��������� ����� ����������� �������.
 					while (get_button(LINE_CHECKER_BUTTON))
 						{
 							//���� ���������� ������� �� �������.
 							if (Timer0FlagReg & Timer1_LCLongPressFlag)
 								{
 									break;
 								}
 						}
 					//-----------------------------------------
 					//���������� �������.
 					if (Timer0FlagReg & Timer1_LCLongPressFlag)
 						{	
 							//-----------------------------------------
 							//���� ������ �� �������� �� ����������� ��������� �����.
 							//���� ������ ����� � ��������, �� �� ���������� ��������� �����.
 							if ( !(get_lines_to_control() & temp) )
 								{
 									one_line_check(temp);//����������� ��������� �����.
 								}
 							//-----------------------------------------
 							//��������� ��� ������ ���� �� ��������. 
 							set_lines_to_control(temp); // & get_blocked_speaker_line());
 							//-----------------------------------------
 							//�������� ���������� ������.
 							while (get_button(LINE_CHECKER_BUTTON))	{}
 							//-----------------------------------------
							//���������� ������ �� �������� ����� � EEPROM.
							eep_write_set_lines_to_control(get_lines_to_control());
							//-----------------------------------------
 							//���������� ��������� ������ �������� �������.
 							Timer0FlagReg &= ~(Timer1_LCLongPressFlag);
 							//-----------------------------------------
 						}
 					//-----------------------------------------
 					//�������� �������. ������ ����������� ��������� �����.
 					else
 						{
 							//����������� ��������� �����.
 							one_line_check (temp);
 							//������ ��������� �� ������� � ������� 3 ���.
 							//��������� ������ � ������ ����������� ���������� �� ����� �� �����.
 							if (temp & get_alarms_lines())
 								{
 									start_short_indication(temp);
 								}
 						}
 					//-----------------------------------------
 				}			
 			//----------------------------------------------------------------------
 			//�������� ����� ������������������ �� ��������.
 			//����� ���������. �������� ������ 15 ������.
 			if (ConfigurationStr.STR.TimeAutoCheck == TIMEOUT_CONFIG_MODE)
 				{
 					if (Timer0FlagReg & Timer1_15secFlag)
 						{
 							measure_resistence_line_after_timeout();
 							Timer0FlagReg &= ~(Timer1_15secFlag);
 						}
 				}
 			//�������������� �������.
 			else
 				{
 					if (get_hour_counter() >= ConfigurationStr.STR.TimeAutoCheck)
 						{
 							measure_resistence_line_after_timeout();
 							clear_hour_counter();
 						}
 				}
 			//----------------------------------------------------------------------
 			//������� ������ � �������� ��������� ���.
 			if (ConfigurationStr.STR.ChargeCheck == On)
 				{
 					charge_monitoring_on();
 				}	
 			//������� ������ � �������� ��������� ��� ���������.
 			else
 				{
 					CHARGE_OFF;
					clear_charge_indication();
 				}
 			//----------------------------------------------------------------------
			//���������� ����� ��������� RS485.
			if (IndicatorsFacePanelStr.bit.RS485_LED != 0)
				{
					set_discret_output(_485_OUTPUT, ACTIVATE);
				}
			else
				{
					set_discret_output(_485_OUTPUT, DEACTIVATE);
				}
			//----------------------------------------------------------------------
			//���������� ����� ��������� LC.
			if ( (get_blocked_speaker_line()!=0xFF)					|| //
				 (get_alarms_lines() & get_lines_to_control())		|| //
				 (get_charge_indication() != 0)	)					   // ������������� ������� ������ � �������� ��������� ��� ���������.	
				
				{
					set_discret_output(LC_OUTPUT, ACTIVATE);
				}
			else
				{
					set_discret_output(LC_OUTPUT, DEACTIVATE);
				}
			//----------------------------------------------------------------------
 			//���������� �������� �������������.
  			if  ( (~(get_blocked_speaker_line()) & get_lines_to_control())							  ||
				   ((get_charge_indication() && ConfigurationStr.STR.Buzzer)!=0)					  ||
				  (((get_alarms_lines() & get_lines_to_control()) && ConfigurationStr.STR.Buzzer)!= 0)|| 	
				   ((IndicatorsFacePanelStr.bit.RS485_LED && ConfigurationStr.STR.Buzzer)!= 0) )
  				{
  					set_discret_output(ZUMMER, ACTIVATE);
  				}
  			else
  				{
  					set_discret_output(ZUMMER, DEACTIVATE);
  				}	
			//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------	
 		}
	//---------------------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------------------	
}
//******************************************************************************************************************
//******************************************************************************************************************



