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

//Обязательно заполнять!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define SW_Version	61	//Версия ПО блока.Первая цифра - это версия железа, Вторая цифра - версия ПО!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#include "my_lib/main.h"

//-----------------------------------------------------------------------------
extern __IndicatorFacePanel_TypeDef		IndicatorsFacePanelStr;
extern __ButtonSost_TypeDef				ButtonSostStr;
//------------------------------
__Configuration_TypeDef					ConfigurationStr;//Структура для хранения конфигурации.

//uint8_t EMSelectZone = 0xFF;	//Выбраные зоны при нажатии на кнопку МИК.
//******************************************************************************************************************
//******************************************************************************************************************
static void led_ALL_off(void){
	
	//--------------------------------
	 	cli(); //Запрет прерываний.
	IndicatorsFacePanelStr.bit.ALL_LED = 0;
	ButtonSostStr.STR.AllMicButtonSost &= ~(ALL_BUTTON);
	 	sei(); //Разрешение прерываний.
	//--------------------------------
}
//-----------------------------------------------------------------------------------------------------------------
//Звуковая индикация.
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
//Тестирование индикации.
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
// 			display_set(0x88);//Индикация "88"
// 			set_discret_output(ZUMMER, ACTIVATE);
// 		}
// 	else
// 		{
// 			display_off();
// 			set_discret_output(ZUMMER, DEACTIVATE);
// 		}
	//--------------------------------
	//Так занимает на 44 байта меньше.
	//Тест зуммера.	
	if (byte)
	 	{
	 		set_discret_output(ZUMMER, ACTIVATE);
	 	}
	else
	 	{
	 		set_discret_output(ZUMMER, DEACTIVATE);
	 	}
	//Тест светодиодов лицевой панели.
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
//Режим конфигурирования.
static void configuration_mode (void){
	
	//--------------------------------
	CHARGE_OFF;									 //Отключение блока ЗУ
	clear_charge_indication();					 //и гашение индикации "НА".
	set_discret_output(ZUMMER, DEACTIVATE);		 //Отключение звуковой индикации.
	set_discret_output(_485_OUTPUT, DEACTIVATE); //Отключение контакта RS485.
	set_discret_output(LC_OUTPUT, DEACTIVATE);	 //Отключение контакта LC.
	IndicatorsFacePanelStr.bit.RS485_LED = 0;	 //Отключение световой индикации RS485.
	set_relay(ALL_RELAY_OFF, SPEAKER_RELAY);	 //Отключение линий громкоговорителей от усилителя.
	set_relay(ALL_RELAY_OFF, LINE_CHECKER_RELAY);//Отключение блока проверки линиий от линий.
	select_audio_source(OFF_SOURCE);			 //Выбор источника звука - вход усилителя соединен с землей.
	power_amplifier_control(POWER_OFF);			 //Отключение УМ.
	set_speaker_led_green(ALL_LED_OFF);			 //Гашение зеленых светодиодов.
	set_line_checker_led_green(ALL_LED_OFF);	 //Гашение зеленых светодиодов.
	set_line_checker_led_red(ALL_LED_OFF);		 //Гашение крастных светодиодов.
	display_off();								 //Очистка дисплея. 
	led_ALL_off();								 //Гашение светодиода ALL.
	//--------------------------------
	while (Timer0FlagReg & Timer1_ConfigModeFlag)
		{
			//--------------------------------------
			set_speaker_led_red(Timer1BlinkReg);  //Мигание светодиодами SPEACKER.
			//--------------------------------------
			//Определение параметра настройки
			switch(config_parameter_selection(CONFIG_START))
				{	//----------------------------
					//Вывод версии ПО в режиме настройки на дисплей при нажатии кнопки SP8.
					case(SW_VERSION):
						display_set(bin_to_dec(SW_Version) );//Вывод на дисплей версии ПО.
					break;
					//----------------------------
					//Установка адреса.
					case(CONFIG_PARAMETER_ADDRES):
						set_line_checker_led_green(LC1_LED);
						display_set( bin_to_dec(ConfigurationStr.STR.Address) );//Вывод установленного адреса на дисплей.
						modification_config_parameter(&ConfigurationStr.STR.Address, CONFIGURATION_ADDRESS_MIN,CONFIGURATION_ADDRESS_MAX);
					break;
					//----------------------------
					//Установка группы.
					case(CONFIG_PARAMETER_GROUP):
						set_line_checker_led_green(LC2_LED);
						display_set(drawH | ConfigurationStr.STR.Group);
						modification_config_parameter(&ConfigurationStr.STR.Group,CONFIGURATION_GROUP_MIN, CONFIGURATION_GROUP_MAX);
					break;
					//----------------------------
					//Вкл-выкл контроля АКБ.
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
					//Вкл-выкл системы контроля RS-485.
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
					//Установка времени автоматического контроля линий.
					case(CONFIG_PARAMETER_TIME_AUTO_CHECK):
						set_line_checker_led_green(LC5_LED);
						display_set( bin_to_dec(ConfigurationStr.STR.TimeAutoCheck) );
						modification_config_parameter(&ConfigurationStr.STR.TimeAutoCheck,CONFIGURATION_TIME_AUTO_CHECK_MIN, CONFIGURATION_TIME_AUTO_CHECK_MAX);
					break;
					//----------------------------
					//Активация пищалки.
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
					//Установка порога срабатывания LC.
					case(CONFIG_PARAMETER_LEVEL_LC):
						set_line_checker_led_green(LC7_LED);
						//Индикация установленного порога. Такая конструкция занимает меньше места.
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
					//Установка режима резервирования блока.
// 					case(CONFIG_PARAMETER_RESERVATION):
// 						set_line_checker_led_green(LC8_LED);
// 						//----------------------------
// 						if (ConfigurationStr.STR.Reservation == Reserv_OFF)
// 							{
// 								display_set( 0x1000 );//Индикация "РО" - резерв отключен.
// 							}
// 						else if (ConfigurationStr.STR.Reservation == Reserv_MASTER)
// 							{
// 								display_set( 0x000B );//Индикация "OC" - основной.
// 							}
// 						else if (ConfigurationStr.STR.Reservation == Reserv_SLAVE)
// 							{
// 								display_set( 0x1011 );//Индикация "РЕ" - резервный.
// 							}
// 						//----------------------------
// 						modification_config_parameter(&ConfigurationStr.STR.Reservation, CONFIG_PARAMETER_RESERVATION_MIN, CONFIG_PARAMETER_RESERVATION_MAX);
// 					break;
					//----------------------------
				}
			//--------------------------------------
			//Режим тестирования индикации (световой и звуковой).
			if (Timer0FlagReg2 & Timer0_StartTestAllIndicatorsFlag)
				{
					while(1)
						{
							//----------------------------
							//Мигание индикаторами.
							test_indicators(Timer1BlinkReg);
							//Регулировка яркости световой индикации лицевой панели.
							modification_config_parameter(&DisplayBright, CONFIG_PARAMETER_DISPLAY_BRIGHT_MIN, CONFIG_PARAMETER_DISPLAY_BRIGHT_MAX);
							TempDisplayBright = DisplayBright;
							//----------------------------
							//Выход из режима тестирования индикации.
							if ( (Timer0FlagReg2 & Timer0_StartTestAllIndicatorsFlag)==0 )
								{
									//Отключение всех индикаторов.
									test_indicators(0x00);
									//Ожидание отпускания кнопки.
									while(get_button(ALL_EM_BUTTON) & ALL_BUTTON){ }	
									break;
								}
							//----------------------------
						}			
				}
			//--------------------------------------
		}
	//--------------------------------------
	display_set(bin_to_dec(ConfigurationStr.STR.Address));//Вывод установленного адреса на дисплей.
	config_parameter_selection(CONFIG_END);		//Это нужно что бы следующее конфигурирование начиналось с первого параметра. 
	eep_write_configuration(&ConfigurationStr);	//Запись отредактирнованной конфигурации.
	set_speaker_led_red(ALL_LED_OFF);			//Гашение индикаторов LINE CHECKER.
	set_line_checker_led_red(ALL_LED_OFF);		//Гашение крастных светодиодов.
	set_line_checker_led_green(ALL_LED_OFF);	//Гашение индикаторов SPEAKER.
	//--------------------------------------
	measure_resistence_all_line_after_power_on(ConfigurationStr.STR.LevelLC);//Измерение и сохранение импедансов линий. 
	//--------------------------------------
	Timer0FlagReg &= ~Timer1_15secFlag;	  //Сброс флага 15 сек и
	clear_hour_counter();				  //счетчика часов.
	//Сброс счетчика и флагов ЗУ.
	clear_charge_hour_counter();		  //
	Timer0FlagReg &= (Timer1_Charge15secFlag | Timer1_ChargeChekrFlag);
	//Сброс состояния кнопок МИК и ALL, что бы при выходе из конфигурирования не провалится в приоритет МИК
	//и не активировались все зоны от поздно отпущеной кнопки ALL.
	ButtonSostStr.STR.AllMicButtonSost &= ~(EM_BUTTON | ALL_BUTTON);
	//--------------------------------------
}
//-----------------------------------------------------------------------------------------------------------------
//Ручное управление селектором зон.
static void manual_control_select_zone(void){

	//-----------------------------------------------------------------------------------
	//Нажата кнопка ALL.	
	if (toggle_button(ALL_EM_BUTTON) & ALL_BUTTON)
		{
			if (toggle_button(SPEAKER_BUTTON) == 0xFF)//Если все зоны включены....
				{
					set_relay(ALL_RELAY_OFF, SPEAKER_RELAY);
					set_speaker_led_green(ALL_LED_ON & get_blocked_speaker_line());
					IndicatorsFacePanelStr.bit.ALL_LED = 0;
					ButtonSostStr.STR.SpeakerButtonSost = 0;
					ButtonSostStr.STR.AllMicButtonSost &= ~(ALL_BUTTON);
				}
			else
				{
					//Если зоны включены частично, то включаем все зоны.
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
			//Если включены все зоны то зажгем светодиод ALL.
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
//Рабочий режим.
static void work_mode(void){
	
	//--------------------------------
	power_led_green();									  //Включение зеленого светодиода POWER.
	set_speaker_led_red(ALL_LED_OFF);					  //Гашение красных светодиодов при переходе из высоких приритетов.
	display_set(bin_to_dec(ConfigurationStr.STR.Address));//Вывод установленного адреса на дисплей.
	//--------------------------------
	power_amplifier_control(POWER_ON);					  //Включение УМ.
	//--------------------------------
	//Выбор источника звука. Если есть активные зоны, то вход УМ подключается к AUX.
	if (get_relay_state(SPEAKER_RELAY))
		{
			select_audio_source(AUX);		
		}
	//Если нет активных зон, то вход УМ на землю.
	else
		{
			select_audio_source(OFF_SOURCE);
		}							  
	//--------------------------------
	manual_control_select_zone();						  //Ручной выбор зон.
	//--------------------------------
}
//-----------------------------------------------------------------------------------------------------------------
//Дежурный режим.
static void standby_mode(void){
	
	//--------------------------------
	power_led_blink_green();			//Мигающий зеленым светодиод POWER
	set_speaker_led_green(ALL_LED_OFF);	//Гасим все зеленые светодиоды.
	set_speaker_led_red(ALL_LED_OFF);	//Гасим все красные светодиоды.
	display_off();						//Отключение дисплея.
	//--------------------------------
	set_relay(ALL_RELAY_OFF,SPEAKER_RELAY);//Отключение линий громкоговорителей от УМ.
	select_audio_source(OFF_SOURCE);	//Выбор источника звука - вход усилителя соединен с землей.
	power_amplifier_control(POWER_OFF);	//Отключение УМ.
	//--------------------------------
	led_ALL_off();
// 	cli(); //Запрет прерываний.
//   	IndicatorsFacePanelStr.bit.ALL_LED = 0;
//   	ButtonSostStr.STR.AllMicButtonSost &= ~(ALL_BUTTON);
// 	sei(); //Разрешение прерываний.
	//--------------------------------
}
//-----------------------------------------------------------------------------------------------------------------
//Управление с помощью удаленных консолей RM-8064.
static void consol_control(void){
	
	uint8_t temp = get_consol_control();					//Получение управление от консоли.
	//--------------------------------	
	//При получении упраление от консолей с 5 по 9 адрес активируется контакт +24В.
	if ( (get_consol_number() >= 5) && (get_consol_number() <= 9) )
		{
			set_discret_output(_24V_OUTPUT, ACTIVATE);		//Включение выходной клеммы +24.
		}
	else
		{
			set_discret_output(ALL_OUTPUT_OFF, ACTIVATE);	//Сброс выходных клемм EM1, EM2 и +24.
		}
	//--------------------------------	
	power_led_green();										//Включение зеленого светодиода POWER.	
	display_set(0xD0 | get_consol_number());				//Вывод номера консоли на дисплей.
	//--------------------------------
	power_amplifier_control(POWER_ON);						//Включение УМ.	
	select_audio_source(CONSOL);							//Выбор источника звука для УМ.
	consol_select_audio_channel(get_consol_audio_chanel());	//Выбор аудиоканала консоли.
	//--------------------------------
	set_relay(temp, SPEAKER_RELAY);							//Подключение линий громкоговорителей к УМ.
 	set_speaker_led_green(ALL_LED_OFF);						//Гасим все зеленые светодиоды.
 	set_speaker_led_red(temp & get_blocked_speaker_line());	//Включение соответствующих красных светодтодов.
	//--------------------------------
	led_ALL_off();
// 	cli(); //Запрет прерываний.
//   	IndicatorsFacePanelStr.bit.ALL_LED = 0;
//   	ButtonSostStr.STR.AllMicButtonSost &= ~(ALL_BUTTON);
// 	sei(); //Разрешение прерываний.
	//--------------------------------
}
//-----------------------------------------------------------------------------------------------------------------
//Управление с помощью клеммы EMERGENCY INPUT ALL.
static void emergency_input_all_control(void){
	
	//--------------------------------
	//set_relay(ALL_RELAY_OFF, LINE_CHECKER_RELAY);//Отключение реле LINE_CHECKER.
	power_led_green();											 //Включение зеленого светодиода POWER.
	set_speaker_led_green(ALL_LED_OFF);							 //Гасим все зеленые светодиоды.	
	set_speaker_led_red(ALL_LED_ON & get_blocked_speaker_line());//Включение красных светодтодов.
	display_set(0xE3);											 //Вывод на дисплей П3.
	//--------------------------------
	power_amplifier_control(POWER_ON);							 //Включение УМ.
	select_audio_source(EM2);									 //Выбор источника звука
	set_relay(ALL_RELAY_ON, SPEAKER_RELAY);						 //Подключение линий громкоговорителей к УМ.
	//--------------------------------
	set_discret_output(EM2_OUTPUT, ACTIVATE);					 //Активация клемм EM2 и 24V.
	//--------------------------------
	led_ALL_off();
// 	cli(); //Запрет прерываний.
//   	IndicatorsFacePanelStr.bit.ALL_LED = 0;
//   	ButtonSostStr.STR.AllMicButtonSost &= ~(ALL_BUTTON);
//	sei(); //Разрешение прерываний.
	//--------------------------------
}
//-----------------------------------------------------------------------------------------------------------------
//Управление с помощью клемм EMERGENCY INPUT1 - INPUT8.
static void emergency_input_1_8_control(void){
	
	uint8_t temp = get_emergency_input();					//Получение состояний клемм EM1-EM8.
	//--------------------------------
	//set_relay(ALL_RELAY_OFF, LINE_CHECKER_RELAY);	//Отключение реле LINE_CHECKER.
	power_led_green();										//Включение зеленого светодиода POWER.
	set_speaker_led_green(ALL_LED_OFF);						//Гасим все зеленые светодиоды.	
	set_speaker_led_red(temp & get_blocked_speaker_line());	//Включение соответствующих красных светодтодов.
	display_set(0xE2);										//Вывод на дисплей П2.
	//--------------------------------	
	power_amplifier_control(POWER_ON);						//Включение УМ.
	select_audio_source(EM1);								//Выбор источника звука
	set_relay(temp, SPEAKER_RELAY);							//Подключение линий громкоговорителей к УМ.
	//--------------------------------
	set_discret_output(EM1_OUTPUT, ACTIVATE);				//Активация клемм EM1 и 24V.
	//--------------------------------
	led_ALL_off();
// 	cli(); //Запрет прерываний.
//   	IndicatorsFacePanelStr.bit.ALL_LED = 0;
//   	ButtonSostStr.STR.AllMicButtonSost &= ~(ALL_BUTTON);
// 	sei(); //Разрешение прерываний.
	//--------------------------------
}
//-----------------------------------------------------------------------------------------------------------------
//Управление с помощью кнопки EM(МИК).
//По длительному нажатию (более 1 сек) кнопки SP соответствующая зона деактивируется/активируется.
static void emergency_button_control(void){
	
	//--------------------------------
	//set_relay(ALL_RELAY_OFF, LINE_CHECKER_RELAY);//Отключение реле LINE_CHECKER.
	power_led_green();											 //Включение зеленого светодиода POWER.
	set_speaker_led_green(ALL_LED_OFF);							 //Гасим все зеленые светодиоды.
	set_speaker_led_red(Timer1EMSelectZone & get_blocked_speaker_line());//Включение красных светодтодов. По длительному нажатию (более 1 сек) кнопки SP соответствующая зона деактивируется/активируется.
	display_set(0xE1);											 //Вывод на дисплей П1.
	//--------------------------------
	power_amplifier_control(POWER_ON);							 //Включение УМ.
	select_audio_source(MIC);									 //Выбор источника звука
	set_relay(Timer1EMSelectZone, SPEAKER_RELAY);				 //Подключение линий громкоговорителей к УМ. По длительному нажатию (более 1 сек) кнопки SP соответствующая зона деактивируется/активируется.
	//--------------------------------	
	set_discret_output(_24V_OUTPUT, ACTIVATE);				     //Включение выходной клеммы +24.
	//--------------------------------
	led_ALL_off();
// 	cli(); //Запрет прерываний.
//   	IndicatorsFacePanelStr.bit.ALL_LED = 0;
//   	ButtonSostStr.STR.AllMicButtonSost &= ~(ALL_BUTTON);
// 	sei(); //Разрешение прерываний.
	//--------------------------------
}
//-----------------------------------------------------------------------------------------------------------------
//Определение активного приоритета.
static uint8_t get_priority(void){
	
	//------------------------------------------
	//Опрос кнопки EM.
	if(toggle_button(ALL_EM_BUTTON) & EM_BUTTON)
		{
			//Ждем пока отпустят кнопку.
			while(get_button(ALL_EM_BUTTON) & EM_BUTTON)
				{
					//------------------------------------------
					//Если было длительное нажатие то выходим с нулем и переходим в режим конфигурирования.
					if (Timer0FlagReg & Timer1_ConfigModeFlag)
						{
							//Сброс состояния кнопки что бы после выхода
							//из режима конфигурации не свалится в первый приоритет.
							ButtonSostStr.STR.AllMicButtonSost &= ~(EM_BUTTON);
							return 0;
						}
					//------------------------------------------
				}
			return PRIORITY_1;
		}
	//------------------------------------------
	//Опрос клемм 1 - 8.
	if (get_emergency_input())
		{
			return PRIORITY_2;
		}
	//------------------------------------------
	//Опрос клемм ALL.
	if ((ALL_PIN & ALL_LEG) == 0)
		{
			return PRIORITY_3;
		}
	//------------------------------------------
	//Управление от консоли.
	if (get_consol_number())
		{
			return PRIORITY_4;
		}
	//------------------------------------------
	//Ручное управление.	
	return PRIORITY_5;
}
//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
//Основной цикл программы.
int main(void){
	
	uint8_t temp  = 0;
	//------------------------------
	charge_init();	//Отключение ЗУ.
	//------------------------------
	_delay_ms(1000);//Пауза для стабилизации переходных процессов.
	//------------------------------
	relay_em_init();//Отключение УМ.
	timers_init();	//
	adc_init();		//
	pt6964_init();	//
	usart_init();	//
	hef4052_init(); //
	//----------------------------------------------------------------------
	select_audio_source(OFF_SOURCE);			 //Вход усилителя соединен с землей.
	set_relay(ALL_RELAY_OFF, SPEAKER_RELAY);	 //Отключение всех линий от УМ.
	set_relay(ALL_RELAY_OFF, LINE_CHECKER_RELAY);//Отключение реле LC от громкоговорителей.	
	//----------------------------------------------------------------------
	//Включение сторожевого таймера со сбросом через 2 секунды. Сброс сторожевого таймера в прерывании таймера1 каждые 10 мСек.
	wdt_enable(WDTO_2S);
	//----------------------------------------------------------------------	
	//Глобальное разрешение прерываний.
	sei();
	//----------------------------------------------------------------------
	_delay_ms(1000);							 //Пауза для стабилизации переходных процессов.
	calibration_adc_for_line_checker();			 //Автокалибровка измерительного канала АЦП LineCheckera при включении.
	//----------------------------------------------------------------------
	//Красивое включение.
	set_relay(ALL_RELAY_OFF, SPEAKER_RELAY);//Отключение всех линий от УМ.
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
	set_speaker_led_green(ALL_LED_OFF);//Гасим все зеленые светодиоды. 	
	//----------------------------------------------------------------------
	//Считывание конфигурации из ЕЕПРОМ.
	eep_read_configuration(&ConfigurationStr);
	//Если в ЕЕПРОМ отсутствует конфигурация то запишем конфигурацию по умолчанию. 
	if (ConfigurationStr.STR.Address > MAX_ADDRESS)//Если в EEPROM 0хFF, значит конфигурация еще не записывалась. 
		{
			//Конфигурации по умолчанию.
			ConfigurationStr.STR.Address		= ADDRESS_DEFAULT;
			ConfigurationStr.STR.Group			= GROUP_DEFAULT;
			ConfigurationStr.STR.ChargeCheck	= CHARGE_CHECK_DEFAULT;
			ConfigurationStr.STR.RS485Check		= RS485_CHECK_DEFAULT;
			ConfigurationStr.STR.TimeAutoCheck	= TIMEOUT_CHECK_DEFAULT;
			ConfigurationStr.STR.Buzzer			= BUZZER_OFF; 
			ConfigurationStr.STR.LevelLC		= LEVEL_LC_DEFAULT; 
//			ConfigurationStr.STR.Reservation	= RESERVATION_DEFAULT; 
			eep_write_configuration(&ConfigurationStr);
			eep_write_set_lines_to_control(0xFF);//При первом включении прибора все линии Установленны на контроль.
		}
	//----------------------------------------------------------------------
	//Управление светодиодом POWER.
	if((POWER_SWITCH_PIN & POWER_SWITCH_LEG) == 0)
		{
			//Клавиша в положении ON.
			power_led_green();									   //Зеленый светодиод POWER.
			display_set( bin_to_dec(ConfigurationStr.STR.Address));//Вывод установленного адреса на дисплей.
		}			
	else
		{
			//Клавиша в положении OFF.
			power_led_blink_green();								//Мигающий зеленый светодиод POWER.
		}	
	//----------------------------------------------------------------------
	//Измерение импеданса 8-ми линий. Если импеданс линии выходит за рамки максимума или
	//минимума, то это значение не сохраняется. Если КЗ, то индикация мигающий желтый и включение звуковой индикации.
	measure_resistence_all_line_after_power_on(ConfigurationStr.STR.LevelLC);
	//----------------------------------------------------------------------
	//Считывание из EEPROM установленных на контроль линий. 
	set_lines_to_control(eep_read_set_lines_to_control());  
	//----------------------------------------------------------------------
	//Принудительне обесточивание реле SP и LC.
	//set_relay(ALL_RELAY_OFF, SPEAKER_RELAY);	 //Отключение всех линий от УМ.
	//set_relay(ALL_RELAY_OFF, LINE_CHECKER_RELAY);//Отключение реле LC от громкоговорителей.
	//----------------------------------------------------------------------
	//Конфигурация блока для работы протокола и запуск протокола.
	usart_config_for_protocol(&ConfigurationStr);
	//---------------------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------------------
    while (1) 
		{
			//----------------------------------------------------------------------
			//Если взведен Timer0_EMLongPressFlag то попадаем в режим настройки.
  			if (Timer0FlagReg & Timer1_ConfigModeFlag)
  				{
					UCSRB &= ~(1<<RXEN | 1<<RXCIE);															//Отключение приема запросов от RA1.	
 					Timer0FlagReg2 &= ~(Timer0_StartPtotocolForRA1Flag | Timer0_StartPtotocolForRA2Flag);	//Останов протокола.
					zummer_indication(3, 100);																//Звуковая индикация перехода в режим конфигурирования.Три раза пикнет.
 	 				configuration_mode();
 					usart_config_for_protocol(&ConfigurationStr);											//Конфигурация для протокола.
  				}
  			//----------------------------------------------------------------------
 			//Определение приоритета управления.
 			temp = get_priority();
  			//----------------------------------------------------------------------
 			//Приоритет управления блоком для протокола.
 			priority_for_protocol(temp);
 			//----------------------------------------------------------------------
  			//Управление по приоритетам.		
 			switch(temp) 
 				{	
 					//----------------------------
 					//Приорите 5 (низший).Ручной режим.
 					case(PRIORITY_5):
 						//Сброс выходных клемм.
 						set_discret_output(ALL_OUTPUT_OFF, ACTIVATE);
 						//--------------
 						//Рабочий режим.
 						if ((POWER_SWITCH_PIN & POWER_SWITCH_LEG) == 0)
 							{
 								work_mode();											
 							} 
 						//--------------	
 						//Дежурный режим.	
 						else
 							{
 								standby_mode();
 							}
 						//--------------											
 					break;
 					//----------------------------
 					//Приоритет 4. Управление с помощью удаленных консолей RM-8064.
 					case(PRIORITY_4):
 						consol_control();						
 					break;
 					//----------------------------
 					//Приоритет 3. Управление с помощью клеммы EMERGENCY INPUT ALL.
 					case(PRIORITY_3):
 						emergency_input_all_control();			
 					break;
 					//----------------------------
 					//Приоритет 2. Управление с помощью клемм EMERGENCY INPUT1 - INPUT8.
 					case(PRIORITY_2):
 						emergency_input_1_8_control();
 					break;
 					//----------------------------
 					//Приоритет 1 (высший). Управление с помощью кнопки EM.
 					case(PRIORITY_1):
 						emergency_button_control();
 					break;
 					//----------------------------
 					default:
 					break;
 					//----------------------------
 				}
 			//----------------------------------------------------------------------
 			//Получение состояния кнопокLC.
 			temp = get_button(LINE_CHECKER_BUTTON);
 			//Обработка короткого и длительного нажатия на кнопки LC.
 			if (temp | (Timer0FlagReg & Timer1_LCLongPressFlag))
 				{	
 					//-----------------------------------------
 					//Ожидание отпускания кнопки или взведения флага длительного нажатия.
 					while (get_button(LINE_CHECKER_BUTTON))
 						{
 							//Если длительное нажатие то выходим.
 							if (Timer0FlagReg & Timer1_LCLongPressFlag)
 								{
 									break;
 								}
 						}
 					//-----------------------------------------
 					//Длительное нажатие.
 					if (Timer0FlagReg & Timer1_LCLongPressFlag)
 						{	
 							//-----------------------------------------
 							//Если взятие на контроль то определение состояния линии.
 							//Если снятие линии с контроля, то не определяем состояние линии.
 							if ( !(get_lines_to_control() & temp) )
 								{
 									one_line_check(temp);//Определение состояния линии.
 								}
 							//-----------------------------------------
 							//Установка или снятие лини на контроль. 
 							set_lines_to_control(temp); // & get_blocked_speaker_line());
 							//-----------------------------------------
 							//Ожидание отпускания кнопки.
 							while (get_button(LINE_CHECKER_BUTTON))	{}
 							//-----------------------------------------
							//Сохранение взятых на контроль линий в EEPROM.
							eep_write_set_lines_to_control(get_lines_to_control());
							//-----------------------------------------
 							//Разрешение обработки нового длинного нажатия.
 							Timer0FlagReg &= ~(Timer1_LCLongPressFlag);
 							//-----------------------------------------
 						}
 					//-----------------------------------------
 					//Короткое нажатие. Ручное определение состояния линии.
 					else
 						{
 							//Определение состояния линии.
 							one_line_check (temp);
 							//Запуск индикации на дисплее в течении 3 сек.
 							//Индикация только в случае обнаружения отклонения от нормы на линии.
 							if (temp & get_alarms_lines())
 								{
 									start_short_indication(temp);
 								}
 						}
 					//-----------------------------------------
 				}			
 			//----------------------------------------------------------------------
 			//Проверка линий громокоговорителей по таймауту.
 			//Режим настройки. Проверка каждые 15 секунд.
 			if (ConfigurationStr.STR.TimeAutoCheck == TIMEOUT_CONFIG_MODE)
 				{
 					if (Timer0FlagReg & Timer1_15secFlag)
 						{
 							measure_resistence_line_after_timeout();
 							Timer0FlagReg &= ~(Timer1_15secFlag);
 						}
 				}
 			//Автоматический конроль.
 			else
 				{
 					if (get_hour_counter() >= ConfigurationStr.STR.TimeAutoCheck)
 						{
 							measure_resistence_line_after_timeout();
 							clear_hour_counter();
 						}
 				}
 			//----------------------------------------------------------------------
 			//Система заряда и контроля состояния АКБ.
 			if (ConfigurationStr.STR.ChargeCheck == On)
 				{
 					charge_monitoring_on();
 				}	
 			//Система заряда и контроля состояния АКБ отключена.
 			else
 				{
 					CHARGE_OFF;
					clear_charge_indication();
 				}
 			//----------------------------------------------------------------------
			//Управление сухим контактом RS485.
			if (IndicatorsFacePanelStr.bit.RS485_LED != 0)
				{
					set_discret_output(_485_OUTPUT, ACTIVATE);
				}
			else
				{
					set_discret_output(_485_OUTPUT, DEACTIVATE);
				}
			//----------------------------------------------------------------------
			//Управление сухим контактом LC.
			if ( (get_blocked_speaker_line()!=0xFF)					|| //
				 (get_alarms_lines() & get_lines_to_control())		|| //
				 (get_charge_indication() != 0)	)					   // Неисправность система заряда и контроля состояния АКБ отключена.	
				
				{
					set_discret_output(LC_OUTPUT, ACTIVATE);
				}
			else
				{
					set_discret_output(LC_OUTPUT, DEACTIVATE);
				}
			//----------------------------------------------------------------------
 			//Управление звуковой сигнализацией.
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



