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

#include "my_lib\line_chacker.h"

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//Струтура хранения кодов АЦП, соответствующих импедансам линий.
//typedef volatile union {
typedef union {
	struct{
			//----------------------------------------
			uint16_t LineCheckerResistenceCH1;	//
			uint16_t LineCheckerResistenceCH2;	//
			uint16_t LineCheckerResistenceCH3;	//
			uint16_t LineCheckerResistenceCH4;	//
			uint16_t LineCheckerResistenceCH5;	//
			uint16_t LineCheckerResistenceCH6;	//
			uint16_t LineCheckerResistenceCH7;	//
			uint16_t LineCheckerResistenceCH8;	//
			//----------------------------------------
		  }STR;
	uint16_t BLK[8];
}__ResistenceLine_TypeDef;
//-----------------------------------------------------------------------------
//Струтура хранения значений велечины отклонений для регистрации неисправности линии.
//Велечина отклонений задается при настройке блока и может составлять 5%, 10%, 25%, 40% и 65% от измеренной велиничи импеданса линии.
//При 65% отклонение может составлять более 1021 LSB АЦП (при 100 Омах отклонение +/- 1021). Поэтому тип данных uint_16.
//Значения в кодах АЦП. 
//typedef volatile union {
typedef union {
	struct{
			//----------------------------------------
			uint16_t CoefficientCH1;	//
			uint16_t CoefficientCH2;	//
			uint16_t CoefficientCH3;	//
			uint16_t CoefficientCH4;	//
			uint16_t CoefficientCH5;	//
			uint16_t CoefficientCH6;	//
			uint16_t CoefficientCH7;	//
			uint16_t CoefficientCH8;	//
			//----------------------------------------
		  }STR;
	uint16_t BLK[8];
}__ResistenceLine_Coefficient_TypeDef;
//-----------------------------------------------------------------------------
//Струтура хранения состояния линии.
typedef union {
	struct{
		//----------------------------------------
		uint8_t LineBreak;			//Обрыв линии.
		uint8_t LineAbove;			//Линия выше нормы.
		uint8_t LineNorm;			//Линия в норме.
		uint8_t LineBelow;			//Линия ниже нормы.
		uint8_t LineShort;			//КЗ на линии. Маска блокировки выбора зон
		uint8_t LinesOnTheControl;	//Регистр-маска для задания линий установленных на контроль.
		uint8_t Line;				//Линия для которой индикация.
		uint8_t Indicate;			//Код сотояния линии выводимы на дисплей.
		//----------------------------------------
	}STR;
	uint8_t BLK[8];
}__StateLine_TypeDef;
//-----------------------------------------------------------------------------
__ResistenceLine_TypeDef				ReferenceMeasureResisStr;	//Структура для хранения значений импедансов линий при запуске приборов.
__ResistenceLine_Coefficient_TypeDef	CoefficientStr;				//
__StateLine_TypeDef						StateLineStr; 
//-------------------------------------------
//Переменные для работы с измерением сопротивления линии.
static uint16_t CalibrCoeff = 0;


//-------------------------------------------
static volatile uint8_t LineChackerFlagReg  = 0;

static uint8_t LevelLC = 0;		 // Порог срабатывания LC.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static uint16_t calculate_procent(uint16_t value){
	
	uint16_t temp = 0;
	//---------------------------
	switch(LevelLC)
		{
			//---------------------------
			//5.078% от измеренного сопротивления линии.
// 			case(LEVEL_5_PERCENT):
// 				temp  = (value >> 4); //6.25%
// 				temp -= (value >> 6); //1.562%
// 				temp += (value >> 8); //0.39%
// 				return temp;
// 			break; 
			//---------------------------
			//9.745% от измеренного сопротивления линии. 
			case(LEVEL_10_PERCENT):
				temp  = (value >> 4); //6.25%
				temp += (value >> 5); //3.125%
				temp += (value >> 8); //0.39%
				return temp;
			break;
			//---------------------------
			//25% от измеренного сопротивления линии.
			case(LEVEL_25_PERCENT):
				return (value >> 2);
			break;
			//---------------------------
			//40.625% от измеренного сопротивления линии.
			case(LEVEL_40_PERCENT):
				temp  = (value >> 2); //25%
				temp += (value >> 3); //12.5%
				temp += (value >> 5); //3.125%
				return temp;
			break;
			//---------------------------
			//65.625% от измеренного сопротивления линии.
			case(LEVEL_65_PERCENT):
				temp  = (value >> 1); //50%
				temp += (value >> 3); //12.5%
				temp += (value >> 5); //3.125%
				return temp;
			break;
			//---------------------------		
		}
	return 0;
}
//-----------------------------------------------------------------------------
//Измерение импеданса одной линии. Возвращаемое значение Rline * 10.
static uint16_t measure_resistence_one_line(uint8_t line, uint16_t delay){
	
	uint8_t i = 0;
	uint16_t meas = 0;
	
	uint16_t CalibrCoeff_temp = 0;
	uint32_t meas_temp = 0;
	uint32_t meas_temp1 = 0;
	//-------------------------
	//calibration_adc_for_line_checker();			//Автокалибровка здесь не нужна.
	set_relay((1 << line), LINE_CHECKER_RELAY);	//Подключение измеряемой линии к АЦП
	set_line_checker_led_green(1 << line);		//Индикация подключенной линии.
	ms_delay(delay);							//Пауза для завершение переходных процессов в линии.
	//-------------------------
	//get_adc_measure() возвращает 12 бит кода АЦП через 6.7 мСек. AVCC with external capacitor at AREF pin.
	//Введена передескритезация АЦП для повышения разрешающей способности АЦП.
	//Усреднение по 8 выборокам займет 6.7 * 8 = 53.6 мСек.
	for (i = 0; i < 8; i++)
		{
			meas += get_adc_measure(LINE_CHECKER_ADC_CHANEL);
		}
	meas = (meas >> 3);
	//-------------------------
	//Ограничение максимального измеряемго сопротивления для фиксации обрыва.
	if (meas >= 2023U)//Если больше 4000 Ом.
		{
			meas = 2023U;
		}
	//-------------------------
	set_relay(ALL_RELAY_OFF, LINE_CHECKER_RELAY);//Отключение всех реле LINE_CHECKER от линий.
	//set_line_checker_led_green(ALL_LED_OFF);	 //Гашение зеленых светодиодов LINE_CHECKER.
	//-------------------------
	//Получение значения сопротивления линии по логометрическому принципу.
	//формула вычисления сопротивления линии при резисторах делителя R1 = R2 = 100Ом: Rline = (50 * Uadc) / (Ucc/2 - Uadc).
	//ИОН для АЦП - напряжение питание контроллера, это же напряжение используется для питание делителя. При этом Ucc = 4095 (для 12-бит АЦП).
	//Uadc - код АЦП. Коэффициент 50 будет только в случае точного равенства R1 и R2 делителя. 
	//-------------------------
	//Расчет коэффициента.
	if (meas < 1862U)//Меньше 500 Ом.
		{
			CalibrCoeff_temp = CalibrCoeff - (CalibrCoeff / 32);// + (CalibrCoeff / 64);
		}
	else if ( (meas >= 1862U) && (meas < 1950U) )//Если Rline больше 500 и меньше 1000 Ом.
		{
			CalibrCoeff_temp = CalibrCoeff - (CalibrCoeff / 8) + (CalibrCoeff / 128);
		}
	else if ( (meas >= 1950U) && (meas < 1982U) )//Если Rline больше 1000 и меньше 1500 Ом.
		{
			CalibrCoeff_temp = CalibrCoeff - (CalibrCoeff / 8);
		}
	else if (meas >= 1982U) //Если Rline больше 1500 Ом.
		{
			CalibrCoeff_temp = CalibrCoeff - (CalibrCoeff / 8) -  (CalibrCoeff / 32);
		}
	//-------------------------
	//Расчет сопротивления линии.
	meas_temp = (2048U - meas);										
	meas_temp1 = ( ((uint32_t)meas * CalibrCoeff_temp + 5) / 10 );
	meas_temp = ( (meas_temp1 + (meas_temp / 2)) / meas_temp );
	//-------------------------
	return (uint16_t)meas_temp;
	//-------------------------
}
//-----------------------------------------------------------------------------
//Калибровка при включении.
//Суть калибровки в определнии отношения напряжения на верхнем резисторе 
//к напряжению на нижнем резисторе резистивного делителя. В идеале отношение = 1.
//Сопротивление линии для резисторов делителя 100Ом расчитывется по формуле: Rline = (50 * Uadc) / (Ucc/2 - Uadc). 
void calibration_adc_for_line_checker(void){
	
	uint8_t  i = 0;
	uint32_t meas = 0;
	uint32_t meas_temp = 0;
	//-------------------------
	//Усреднение по 8 выборокам займет 6.7 * 8 = 53.6 мСек.
	for (i = 0; i < 8; i++)
		{
			meas += get_adc_measure(LINE_CHECKER_ADC_CHANEL);
		}
	meas = (meas >> 3);
	//-------------------------	
//	meas = get_adc_measure(LINE_CHECKER_ADC_CHANEL);//Возвращает 12бит кода АЦП. ИОН: AVCC with external capacitor at AREF pin.
	meas_temp = (4095 - meas);						//Напряжение на верхнем резисторе делителя.
	meas *= COEFF_SCALE;//10000U;					//Напряжение на нижнем резисторе делителя умноженное на 10000.
	meas = ( (meas + (meas_temp / 2)) / meas_temp );//Расчет отношения напряжений.
	CalibrCoeff = (uint16_t)(50000000U / meas);		//Расчет калибровочного коэффициента. (50 * 1000 000)/отношение напряжений.
}
//-----------------------------------------------------------------------------
//Определение состояния линии.
static void line_status_check(uint16_t meas, uint8_t number){	
	
	//----------------------------------------
	//cli(); //Запрет прерываний.
	//----------------------------------------
	//Импеданс ниже измеренного ранее значения.
	if ( (meas < (ReferenceMeasureResisStr.BLK[number] - CoefficientStr.BLK[number])) || (meas < RESISTENCE_MIN) )
		{
			//---------------------------
			//Проверка на КЗ.
			if (meas < RESISTENCE_MIN)
				{
					StateLineStr.STR.LineShort |= (1 << number);
				}
			//---------------------------
			else
				{
					StateLineStr.STR.LineBelow |= (1 << number);
				}
			//---------------------------	
			//sei(); //Разрешение прерываний.
			return;	
		}
	//----------------------------------------
	//Импеданс выше измеренного ранее значения.
	if ( (meas > (ReferenceMeasureResisStr.BLK[number] + CoefficientStr.BLK[number])) || (meas > RESISTENCE_MAX) )
		{
			//---------------------------
			//Проверка на обрыв линии.
			if (meas > RESISTENCE_MAX)
				{
					StateLineStr.STR.LineBreak |= (1 << number);
				}
			//----------------------------
			else
				{
					StateLineStr.STR.LineAbove |= (1 << number);
				}
			//----------------------------
			//sei(); //Разрешение прерываний.
			return;
		}
	//----------------------------------------
	//Импеданс в норме.
	StateLineStr.STR.LineNorm |= (1 << number);
	//----------------------------------------
	//sei(); //Разрешение прерываний.
	//----------------------------------------
}
//-----------------------------------------------------------------------------
//Сброс состояний линий.
static void clear_line_status(uint8_t clear){
	
	StateLineStr.STR.LineBreak &= ~(clear);
	StateLineStr.STR.LineAbove &= ~(clear);
	StateLineStr.STR.LineNorm  &= ~(clear);
	StateLineStr.STR.LineBelow &= ~(clear);
	StateLineStr.STR.LineShort &= ~(clear);
}
//-----------------------------------------------------------------------------
//Установка или снятие линии на контроль по длительному нажатию.
void set_lines_to_control(uint8_t line){
	
	//	cli();
	StateLineStr.STR.LinesOnTheControl ^= line;
	//	sei();
}
//-----------------------------------------------------------------------------
//Получение маски линий на контроле.
uint8_t get_lines_to_control(void){
	
	return StateLineStr.STR.LinesOnTheControl;
}
//-----------------------------------------------------------------------------
//Получение маски блокировки выбора зон.
uint8_t get_blocked_speaker_line(void){
	
	return (~StateLineStr.STR.LineShort);
}
//-----------------------------------------------------------------------------
//Получение состояния аварии LC.
uint8_t get_alarms_lines(void){
	
	return (StateLineStr.STR.LineBreak |
			StateLineStr.STR.LineBelow |
			StateLineStr.STR.LineAbove |
			StateLineStr.STR.LineShort);
}
//-----------------------------------------------------------------------------
//Проверка статуса линии. 
static void line_check(uint8_t line, uint16_t meas){
	
	//------------------------------
	//При включении был обрыв линии или КЗ.
	if (ReferenceMeasureResisStr.BLK[line] == 0)
		{
			//------------------------------
			//Если линия в норме.
			if ((meas > RESISTENCE_MIN) && (meas < RESISTENCE_MAX))
				{
					ReferenceMeasureResisStr.BLK[line] = meas;		   //запоминаем измеренное значение.
					CoefficientStr.BLK[line] = calculate_procent(meas);//расчитываем коэффициенты для определения изменения сопротивления линий громкоговорителей.
					StateLineStr.STR.LineNorm |= (1 << line);
				}
			//Если КЗ или обрыв, то не сохраняем измеренное значение и выставляем соответствующий флаг.
			else
				{
					//------------------------------
					//Проверка на КЗ.
					if (meas <= RESISTENCE_MIN)
						{
							StateLineStr.STR.LineShort |= (1 << line);
						}
					//Обрыв линии
					else
						{
							StateLineStr.STR.LineBreak |= (1 << line);
						}
					//------------------------------
				}
			//------------------------------
		}
	//----------------------------------------
	//При включении было зафиксированно наличие нагрузки.
	else
		{
			//Определение состояния измеренной линии.
			line_status_check(meas, line);
		}
	//------------------------------
}
//-----------------------------------------------------------------------------
//Измерение импеданса линий после включения прибора.
void measure_resistence_all_line_after_power_on(uint8_t level){
	
	uint8_t i = 0;
	uint16_t meas_temp = 0;
	//------------------------------
	LineChackerFlagReg |= LineChecker_MeasResistFlag;
	//------------------------------
	//Порог срабатывания LC.
	LevelLC = level;
	//------------------------------
	//Сброс состояний линий.
	clear_line_status(ALL_LINES);
	//------------------------------
	//Измерение импеданса 8-ми линий.
	for (i = 0; i < 8; i++)
		{
			ReferenceMeasureResisStr.BLK[i] = 0;
			//------------------------------
			//Измерение импеданса одной линии.
			meas_temp = measure_resistence_one_line(i, LC_SMALL_TIMEOUT);
			//------------------------------
			//Если на линии есть какая то нагрузка и она в норме.
			if ((meas_temp > RESISTENCE_MIN) && (meas_temp < RESISTENCE_MAX))
				{
					ReferenceMeasureResisStr.BLK[i] = meas_temp;		 //запоминаем измеренное значение.
					CoefficientStr.BLK[i] = calculate_procent(meas_temp);//расчитываем коэффициенты для определения изменения сопротивления линий громкоговорителей.
					StateLineStr.STR.LineNorm |= (1 << i);				 //Установка флга Линия в норме.
				}
			 //------------------------------
			 //Если КЗ или обрыв, то не сохраняем измеренное значение и выставляем соответствующий флаг.
			 else
				 {
					 //------------------------------
					 //Проверка на КЗ.
					 if (meas_temp <= RESISTENCE_MIN)
						{
							StateLineStr.STR.LineShort |= (1 << i);
						}
					//------------------------------
					//Обрыв линии
					else
						{
							StateLineStr.STR.LineBreak |= (1 << i);
						}
					//------------------------------
				 }
			//------------------------------
		}
	//------------------------------
	LineChackerFlagReg &= ~(LineChecker_MeasResistFlag);
	set_line_checker_led_green(ALL_LED_OFF);	 //Гашение зеленых светодиодов LINE_CHECKER.
	//------------------------------
}
//----------------------------------------------------------------------------
//Измерение импеданса всех линий по истечению таймаута.
void measure_resistence_line_after_timeout(void){
	
	uint8_t i = 0;
	uint16_t meas_temp = 0;
	//------------------------------
	LineChackerFlagReg |= LineChecker_MeasResistFlag;
	//------------------------------
	//Сброс состояний линий установленных на контроль.
	clear_line_status(get_lines_to_control());
	//------------------------------
	set_line_checker_led_green(ALL_LED_OFF);
	set_line_checker_led_red(ALL_LED_OFF);
	//------------------------------
	//Измерение 8ми линий.
	for (i = 0; i < 8; i++)
		{
			//------------------------------
			//Если линия взята на контроль, то проверяем ее состояние.
			if (get_lines_to_control() & (1 << i))
				{
					//Измерение импеданса одной линии.
					meas_temp = measure_resistence_one_line(i, LC_LONG_TIMEOUT);
					//Определение состояния линии.
					line_check(i, meas_temp);
				}
			//------------------------------
		}
	//------------------------------
	LineChackerFlagReg &= ~(LineChecker_MeasResistFlag);
	//------------------------------
}
//----------------------------------------------------------------------------
//Определение состояния одной линии.
void one_line_check(uint8_t line){
	
	 uint8_t i = 0;	
	 uint16_t meas_temp = 0;
	//------------------------------
	if (line == 0)	{return;}
	//------------------------------
	LineChackerFlagReg |= LineChecker_MeasResistFlag;
	//------------------------------
	//Определение номера нажатой кнопки. От 0 до 7.
	for(i = 0; i < 8; i++)
		{
			if (line & (1 << i)) { break; }
		}
	//------------------------------
	//Сброс состояния линии.
	clear_line_status(1 << i);
	//----------------------------------------
	set_line_checker_led_green (ALL_LED_OFF);
	set_line_checker_led_red   (ALL_LED_OFF);
	//----------------------------------------	
	meas_temp = measure_resistence_one_line(i, LC_LONG_TIMEOUT);		//Измерение импеданса одной линии.
	line_check(i, meas_temp);											//Определение состояния линии.
	//----------------------------------------
	//cli(); //Запрет прерываний
	ButtonSostStr.STR.LineChekerButtonSost &= ~(1 << 3);				 //Сброс состояния соответствующей кнопки Line Checkera.
	ButtonSostStr.STR.SpeakerButtonSost &= ~(StateLineStr.STR.LineShort);//Блокировка выбора зон если зафиксированно КЗ на линии.
	//------------------------------
	LineChackerFlagReg &= ~LineChecker_MeasResistFlag;
	//------------------------------
} 
//-----------------------------------------------------------------------------
void start_short_indication(uint8_t line){

	cli(); //Запрет прерываний.
	//-------------------------------
	//Запускаем короткую индикуци.
	Timer0FlagReg |= Timer1_ShortIndicationFlag;
	//-------------------------------
	//Линия для которой будет индикация.
	StateLineStr.STR.Line = line;
	//-------------------------------
	//индикация Обрыв линии.
	if (StateLineStr.STR.LineBreak & line)
		{
			StateLineStr.STR.Indicate = 0xA1;
			sei(); //Разрешение прерываний.
			return;
		}
	//-------------------------------
	//индикация Импеданс выше нормы.
	if (StateLineStr.STR.LineAbove & line)
		{
			StateLineStr.STR.Indicate = 0xA2;
			sei(); //Разрешение прерываний.
			return;
		}
	//-------------------------------
	//индикация Импеданс ниже нормы.
	if (StateLineStr.STR.LineBelow & line)
		{
			StateLineStr.STR.Indicate = 0xA3;
			sei(); //Разрешение прерываний.
			return;
		}
	//-------------------------------
	//КЗ.
	if (StateLineStr.STR.LineShort & line)
		{
			StateLineStr.STR.Indicate = 0xA4;
			sei(); //Разрешение прерываний.
			return;
		}
	//-------------------------------
	//sei(); //Разрешение прерываний.
}
//-----------------------------------------------------------------------------
//Индикация состояний линий громкоговорителей.
void resistence_all_line_indicate(void){
	
	uint8_t not_the_norm = 0;
	uint8_t norm = 0;
	//------------------------------
	//Если идет измерение то ничего не индицируем.
	if ((LineChackerFlagReg & LineChecker_MeasResistFlag) == 0)
		{
			//Это для быстрой индикации при измерении LC.
			Timer1FastBlinkReg = 0;
			//------------------------------
			//Короткая индикация. 3 сек.
			if (Timer0FlagReg & Timer1_ShortIndicationFlag)
				{
					//-----------------------------------------------	
					//При индикации КЗ на дисплей ничего не выводим.
					if (StateLineStr.STR.Indicate != 0xA4)
						{
							if (Timer1BlinkReg)
								{
									display_set(StateLineStr.STR.Indicate);
								}
							else
								{
									display_off();
								}
						}
					//-----------------------------------------------				
					not_the_norm = ( StateLineStr.STR.Line & (StateLineStr.STR.LineBreak | StateLineStr.STR.LineBelow |
									 StateLineStr.STR.LineAbove | (StateLineStr.STR.LineShort & Timer1BlinkReg)) );
					
					set_line_checker_led_red  (not_the_norm);
					set_line_checker_led_green(not_the_norm);
					return;
				}
			//------------------------------
			not_the_norm = ( ((StateLineStr.STR.LineBreak | StateLineStr.STR.LineBelow | StateLineStr.STR.LineAbove) & get_lines_to_control()) ^ ((StateLineStr.STR.LineShort & get_lines_to_control()) & Timer1BlinkReg) );
			norm =		   ( ((StateLineStr.STR.LineNorm & get_lines_to_control()) | ((StateLineStr.STR.LineBreak | StateLineStr.STR.LineBelow | StateLineStr.STR.LineAbove) & get_lines_to_control())) ^ ((StateLineStr.STR.LineShort & get_lines_to_control()) & Timer1BlinkReg)  );
			
			set_line_checker_led_red(not_the_norm);
			set_line_checker_led_green(norm);
			//------------------------------
		}
	//------------------------------
	//Мигающая индикация светодиодов LC при измерении.
	else
		{
			set_line_checker_led_green(get_relay_state(LINE_CHECKER_RELAY & Timer1FastBlinkReg));
		}
	//------------------------------
}
//-----------------------------------------------------------------------------

	

	

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------