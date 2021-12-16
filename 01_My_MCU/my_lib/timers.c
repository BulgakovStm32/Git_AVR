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
//Струтура хранения счетчиков таймера0.
typedef volatile struct{
		//----------------------------------------
// 		uint8_t  Timer0UpdateIndicatorsCounter; //счетчик интервала обновление индикации на лицевой панели.
//		uint8_t  Timer0ReadButtonCounter;		//счетчик интервала считывания кнопок из лицевой панели.
		uint8_t  Timer0Clock_ms_Counter;		//счетчик для отсчитывания 15-тисекундных интервалов.
		uint8_t  Timer0SecondCounter;			//счетчик 15-тисекундных интервалов для отсчета часа.
		uint8_t  Timer0ChargeSecondCounter;		//счетчик 15-тисекундных интервалов для отсчета часового интервала проверки АКБ.
		uint8_t  Timer0HourCounter;				//счетчик часов.
		uint8_t  Timer03secIndicatCounter;		//счетчик для организации 3-хсекундной индикации.
		uint16_t Timer0LongPressCounter;		//счетчик для фиксации длительных нажатий на кнопки.
		uint8_t  Timer0LongPressSPButtomCounter;//счетчик для обработки длителных нажатий SP1 и SP2.
		uint8_t  Timer0LongPressSPButtomTimeout;//
		uint8_t  Timer0LongPressSPButtomEMSPCounter;//Счетчик для фиксации длительного нажатия кнопок SP при активном МИК.
//		uint8_t  Timer0ProtocolCounter;			//счетчик для организации протокола обмена.
		uint8_t  Timer0Blink_ms_Counter;		//счетчик для организации мигающей индикации.
		uint8_t	 Timer0FastBlink_ms_Counter;	//счетчик для организации быстрого мигания.
		uint8_t  Timer0_RX_LED_Counter;			//счетчик для организации индикации на разъеме сети по приему.
		uint8_t  Timer0_TX_LED_Counter;			//счетчик для организации индикации на разъеме сети по передаче.
		uint16_t Timer0Delay;					//счетчик для фунции задержки.
		//----------------------------------------
}__Timer0_Counters_TypeDef;
//-----------------------------------------------------------------------------
volatile uint8_t Timer0FlagReg  = 0;
volatile uint8_t Timer0FlagReg2 = 0;
volatile uint8_t Timer1BlinkReg = 0;
volatile uint8_t Timer1FastBlinkReg = 0;
volatile uint8_t Timer1EMSelectZone = 0xFF;	//Выбраные зоны при нажатии на кнопку МИК. По умолчанию все зоны активны.
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
	 //Число 63035 = 0xF63b для прерывания каждые 10 мСек.
	 //Число 60535 = 0xEC77 для прерывания каждые 20 мСек.
	TCNT1H = 0xF6;
	TCNT1L = 0x3B;
	TCCR1B |= (1<<CS11)|(1<<CS10); //Предделитель 64.
	//TIMSK  |= (1<<TOIE1);		 //Разрешить прерывание по переполнению таймера1
	//-------------------------
	TCNT0  = TIMER0_1ms_CONST;	   //Начальное значение таймера
	TCCR0 |= (1<<CS01)|(1<<CS00);  //Предделитель 64.
	//-------------------------
	TIMSK |= (1<<TOIE0 | 1<<TOIE1);//Разрешить прерывание по переполнению таймера0 и таймера1.
}
//-----------------------------------------------------------------------------
//Получение значения часов.
uint8_t get_hour_counter(void){
	
	return Timer0CountersStr.Timer0HourCounter;
}
//-----------------------------------------------------------------------------
void clear_hour_counter(void){
	
	cli(); //Запрет прерываний.	
	Timer0CountersStr.Timer0HourCounter = 0;
	sei(); //Разрешение прерываний.
}
//-----------------------------------------------------------------------------
void clear_charge_hour_counter(void){
	
	cli(); //Запрет прерываний.	
	Timer0CountersStr.Timer0ChargeSecondCounter = 0;
	sei(); //Разрешение прерываний.
}
//-----------------------------------------------------------------------------
void msDelay(volatile uint16_t delay){
	
	if(delay == 0) return;
	
	cli(); //Запрет прерываний.		
	Timer0CountersStr.Timer0Delay = 0;
	sei(); //Разрешение прерываний.
	
	while(Timer0CountersStr.Timer0Delay != delay){} //Ожиданиае паузы.
}
//-----------------------------------------------------------------------------
//Прерывание каждяе 10 мСек.
ISR(TIMER1_OVF_vect){
	
	//----------------------------------------------------
	//Начальное значение таймера. 63035 = 0xF63b для прерывания каждые 10 мСек.
	TCNT1H = 0xF6;
	TCNT1L = 0x3B;
	//----------------------------------------------------
	//сброс сторожевого таймера
	//wdt_reset(); 
	//----------------------------------------------------
	//Быстрое мигание при измерении LC.
	if (++Timer0CountersStr.Timer0FastBlink_ms_Counter >= 12)
		{
			Timer0CountersStr.Timer0FastBlink_ms_Counter = 0;
			Timer1FastBlinkReg ^= 0xFF;
		}
	//----------------------------------------------------
	//Счетчик 500 мСек.
	if (++Timer0CountersStr.Timer0Blink_ms_Counter >= 50)
		{
			Timer0CountersStr.Timer0Blink_ms_Counter = 0;
			Timer1BlinkReg ^= 0xFF;
			//----------------------------------
			//Часы для отсчета таймаута автоматического контроля линий.
			if (++Timer0CountersStr.Timer0Clock_ms_Counter >= TIMER1_15s_CONST)
				{
					Timer0CountersStr.Timer0Clock_ms_Counter = 0;
					Timer0FlagReg |= Timer1_15secFlag;
					Timer0FlagReg &= ~Timer1_Charge15secFlag;
					//-------------------------		
					//Счетчики часов для автоматической проверки линий.
					if (++Timer0CountersStr.Timer0SecondCounter >= TIMER1_1h_CONST)
						{
							Timer0CountersStr.Timer0SecondCounter = 0;
							Timer0CountersStr.Timer0HourCounter++;
							//Проверка на максимум.
							if (Timer0CountersStr.Timer0HourCounter > 24)
								{
									Timer0CountersStr.Timer0HourCounter = 24;
								}
						}
					//----------------------------------
					//Часы для отсчета таймаута проверки сотояния АКБ.
					if (++Timer0CountersStr.Timer0ChargeSecondCounter >= TIMER1_CHARGE_CHEK_CONST)
						{
							Timer0CountersStr.Timer0ChargeSecondCounter = 0;
							Timer0FlagReg &= ~Timer1_ChargeChekrFlag;
						}
					//----------------------------------
				}
			//-------------------------------------
			//Индикация в течении 3 секунд.
			if (Timer0FlagReg & Timer1_ShortIndicationFlag)
				{
					if (++Timer0CountersStr.Timer03secIndicatCounter >= 6)
						{
							Timer0CountersStr.Timer03secIndicatCounter = 0;
							Timer0FlagReg &= ~Timer1_ShortIndicationFlag;
						}
				}
			//-------------------------------------
			//Индикация в дежурном режиме. Мигающий зеленый светодтод POWER.
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
 	//Если в режиме конфигурации то индикация состояний LINE CHECKERa запрешена.
 	if((Timer0FlagReg & Timer1_ConfigModeFlag) == 0)
 		{
 			//-------------------------
 			//Индикация LINE CHACKERа
 			resistence_all_line_indicate();
 			//-------------------------
 			//Мигающая индикация при неисправности зарядного устройства. Во время определения состояния линии коротким нажатием
			//на дисплее отображается код состоянии линии. 
 			if  ( (Timer1BlinkReg & get_charge_indication()) && !(Timer0FlagReg & Timer1_ShortIndicationFlag) )
 				{
					 //Мигающая индикация при неисправности блока ЗУ или неиспарвности питания 220.
 					display_set(get_charge_indication());
 				}
 			//-------------------------
 		}
 	//------------------------- 
	sei(); //Разрешение прерываний необходимо, т.к. может происходить прием байтов протокола во время работы с лицевой панелью. 
	//Запись данных в лицевую панель. Длится 65 uS.
 	write_data_to_face_penel();
	//----------------------------------------------------
	//Считали состояние кнопок. Длится 120 uS.
	read_button_from_face_penel();
	cli(); //Запрет прерываний.	
	//-------------------------
	//Обработка длительного нажатия (не менее 3 сек). 
 	if ( ButtonFacePanelStr.bit.EM_BUT || get_button(LINE_CHECKER_BUTTON))
  		{
  			if (++Timer0CountersStr.Timer0LongPressCounter >= 300)//Если досчитали до 300 значит прошло 3 секунды.
  				{
  					//Timer0CountersStr.Timer0LongPressCounter = 0;
  					//-------------------------
  					//Длительно нажата кнопок EM. Более 5 секунд.
  					if (ButtonFacePanelStr.bit.EM_BUT)
  						{
							if (Timer0CountersStr.Timer0LongPressCounter >= 500)
								{
									Timer0CountersStr.Timer0LongPressCounter = 0;
									Timer0FlagReg ^= Timer1_ConfigModeFlag; //Установка флага долгово нажатия на кнопку EM - переход в режим конфигурирования.
								}
  						}
  					//-------------------------
  					//Длительно нажата одна из кнопок LC.
  					else
  						{
							Timer0CountersStr.Timer0LongPressCounter = 0;
  							Timer0FlagReg |= Timer1_LCLongPressFlag;	 //Установка флага длительного нажатия на кнопку LC.
  							Timer0FlagReg &= ~(Timer1_15secFlag);		 //Сброс флага 15 сек.
  							Timer0CountersStr.Timer0Clock_ms_Counter = 0;//Сброс счетчика 15 сек.
  							Timer0CountersStr.Timer0SecondCounter = 0;	 //Сброс счетчика отсчета часов.
  						}
  					//-------------------------
  				}
  		}
  	else
  		{
  			Timer0CountersStr.Timer0LongPressCounter = 0;
  		}
	//----------------------------------------------------
	//Длительное нажатие кнопок SP при активном МИК.
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
	//Длительное нажатие кнопок при настройке блока.
	if(Timer0FlagReg & Timer1_ConfigModeFlag)
		{
			//-------------------------
			//Длительное нажатие кнопок SP1 или SP2 при настройке блока.
			if (get_button(SPEAKER_BUTTON) || ButtonFacePanelStr.bit.ALL_BUT)
				{
					Timer0CountersStr.Timer0LongPressSPButtomCounter++;
					//Длительное нажатие кнопок SP1 или SP2 при настройке блока.
					if ( (Timer0CountersStr.Timer0LongPressSPButtomCounter >= Timer0CountersStr.Timer0LongPressSPButtomTimeout) && (get_button(SPEAKER_BUTTON)) )
						{
							Timer0CountersStr.Timer0LongPressSPButtomCounter = 0;
							Timer0CountersStr.Timer0LongPressSPButtomTimeout = 9;
							Timer0FlagReg |= Timer1_LongPressSPButtomFlag;
						}
					//Длительное нажание на кнопку ALL при настроке блока - переход в режим тестирования индикации (световой и звуковой).
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
	//Индикация защиты УМ "PROT".
	if (PROT_PIN & PROT_LEG)
		{
			IndicatorsFacePanelStr.bit.PROT_LED = 0;
		}
	else
		{
			IndicatorsFacePanelStr.bit.PROT_LED = 1;
		}
	//----------------------------------------------------
	//Обмен по протоколу когда адресс 2.
	if (Timer0FlagReg2 & Timer0_StartPtotocolForRA2Flag)
		{
			//usart_cycle_count_modify();
		}	
	//----------------------------------------------------
	//Обмен по протоколу когда адресс 1.
	if (Timer0FlagReg2 & Timer0_StartPtotocolForRA1Flag)
		{
			//rs485_protocol();
		}
	//----------------------------------------------------
}
//-----------------------------------------------------------------------------
//Обработчик прерывания такмера0. Каждую 1 мСек.
ISR(TIMER0_OVF_vect){
	
	//----------------------------------------------------
	//Начальное значение таймера.
	TCNT0 = TIMER0_1ms_CONST;
	//----------------------------------------------------
	//Формаирование задержки.
	Timer0CountersStr.Timer0Delay++;
	//----------------------------------------------------
	//Запуск преобразования АЦП.
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