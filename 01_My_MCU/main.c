//*******************************************************************************************
//*******************************************************************************************
/*
 * 
 *
 * Created: 
 * Author : 
 */ 
//*******************************************************************************************
//*******************************************************************************************

#include "main.h"

//*******************************************************************************************
//*******************************************************************************************


//*******************************************************************************************
//*******************************************************************************************
//�������� ���� ���������.
int main(void){
	
	//uint8_t temp  = 0;
	//------------------------------
//	charge_init();	//���������� ��.
	//------------------------------
	_delay_ms(1000);//����� ��� ������������ ���������� ���������.
	//------------------------------
//	relay_em_init();//���������� ��.
	Timers_Init();	
	USART_Init();
	I2C_Init();

	lcd_init();
	//----------------------------------------------------------------------
	//��������� ����������� ������� �� ������� ����� 2 �������. 
	//wdt_enable(WDTO_2S);

	//����� ����������� �������
	//wdt_reset();
	//----------------------------------------------------------------------
//	select_audio_source(OFF_SOURCE);			 //���� ��������� �������� � ������.
//	set_relay(ALL_RELAY_OFF, SPEAKER_RELAY);	 //���������� ���� ����� �� ��.
//	set_relay(ALL_RELAY_OFF, LINE_CHECKER_RELAY);//���������� ���� LC �� �����������������.	
	//----------------------------------------------------------------------
	//��������� ����������� ������� �� ������� ����� 2 �������. ����� ����������� ������� � ���������� �������1 ������ 10 ����.
//	wdt_enable(WDTO_2S);
	//----------------------------------------------------------------------	
	//���������� ���������� ����������.
	sei();
	//----------------------------------------------------------------------
//	_delay_ms(1000);							 //����� ��� ������������ ���������� ���������.
//	calibration_adc_for_line_checker();			 //�������������� �������������� ������ ��� LineCheckera ��� ���������.
	//----------------------------------------------------------------------
	//�������� ���������.
	//set_relay(ALL_RELAY_OFF, SPEAKER_RELAY);//���������� ���� ����� �� ��.
	//for (temp = 0; temp < 9; temp++)
		//{
			//set_speaker_led_green((1 << (8 - temp)));
			//ms_delay(60);
		//}
//
	//for (temp = 0; temp < 8; temp++)
		//{
			//set_speaker_led_green(1 << temp);
			//ms_delay(60);
		//}
	//set_speaker_led_green(ALL_LED_OFF);//����� ��� ������� ����������. 	
	//----------------------------------------------------------------------
	//���������� ������������ �� ������.
	//eep_read_configuration(&ConfigurationStr);
	////���� � ������ ����������� ������������ �� ������� ������������ �� ���������. 
	//if (ConfigurationStr.STR.Address > MAX_ADDRESS)//���� � EEPROM 0�FF, ������ ������������ ��� �� ������������. 
		//{
			////������������ �� ���������.
			//ConfigurationStr.STR.Address		= ADDRESS_DEFAULT;
			//ConfigurationStr.STR.Group			= GROUP_DEFAULT;
			//ConfigurationStr.STR.ChargeCheck	= CHARGE_CHECK_DEFAULT;
			//ConfigurationStr.STR.RS485Check		= RS485_CHECK_DEFAULT;
			//ConfigurationStr.STR.TimeAutoCheck	= TIMEOUT_CHECK_DEFAULT;
			//ConfigurationStr.STR.Buzzer			= BUZZER_OFF; 
			//ConfigurationStr.STR.LevelLC		= LEVEL_LC_DEFAULT; 
////			ConfigurationStr.STR.Reservation	= RESERVATION_DEFAULT; 
			//eep_write_configuration(&ConfigurationStr);
			//eep_write_set_lines_to_control(0xFF);//��� ������ ��������� ������� ��� ����� ������������ �� ��������.
		//}
	//----------------------------------------------------------------------
	//���������� ����������� POWER.
	//if((POWER_SWITCH_PIN & POWER_SWITCH_LEG) == 0)
		//{
			////������� � ��������� ON.
			//power_led_green();									   //������� ��������� POWER.
			//display_set( bin_to_dec(ConfigurationStr.STR.Address));//����� �������������� ������ �� �������.
		//}			
	//else
		//{
			////������� � ��������� OFF.
			//power_led_blink_green();								//�������� ������� ��������� POWER.
		//}	
	//----------------------------------------------------------------------
	//��������� ��������� 8-�� �����. ���� �������� ����� ������� �� ����� ��������� ���
	//��������, �� ��� �������� �� �����������. ���� ��, �� ��������� �������� ������ � ��������� �������� ���������.
//	measure_resistence_all_line_after_power_on(ConfigurationStr.STR.LevelLC);
	//----------------------------------------------------------------------
	//���������� �� EEPROM ������������� �� �������� �����. 
//	set_lines_to_control(eep_read_set_lines_to_control());  
	//----------------------------------------------------------------------
	//������������� ������������� ���� SP � LC.
	//set_relay(ALL_RELAY_OFF, SPEAKER_RELAY);	 //���������� ���� ����� �� ��.
	//set_relay(ALL_RELAY_OFF, LINE_CHECKER_RELAY);//���������� ���� LC �� �����������������.
	//----------------------------------------------------------------------
	//������������ ����� ��� ������ ��������� � ������ ���������.
//	usart_config_for_protocol(&ConfigurationStr);
	//---------------------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------------------
	//uint8_t TextBuf[] = "123456789\n";
	//uint8_t	i2c_TxBuf[] = "1234567890";

	while (1)
	{
		//RX_LED_PORT ^= RX_LED;
		//USART_StartTX(TextBuf, sizeof(TextBuf)-1);

		//I2C_SetState(I2C_FREE);	// ����������� ����	
		//I2C_StartWrite(0x4F, 0x0000, i2c_TxBuf, sizeof(i2c_TxBuf)-1);
		
		//lcd_set_cursor(LCD_STRING_1, 1); 
		//lcd_print_string("123456");

		lcd_BacklightOFF();
		msDelay(1000);

		lcd_BacklightON();
		msDelay(1000);
		
		I2C_SetState(I2C_FREE);	// ����������� ����	
	}
	//---------------------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------------------	
}
//*******************************************************************************************
//*******************************************************************************************



