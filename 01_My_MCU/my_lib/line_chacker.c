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
//�������� �������� ����� ���, ��������������� ���������� �����.
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
//�������� �������� �������� �������� ���������� ��� ����������� ������������� �����.
//�������� ���������� �������� ��� ��������� ����� � ����� ���������� 5%, 10%, 25%, 40% � 65% �� ���������� �������� ��������� �����.
//��� 65% ���������� ����� ���������� ����� 1021 LSB ��� (��� 100 ���� ���������� +/- 1021). ������� ��� ������ uint_16.
//�������� � ����� ���. 
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
//�������� �������� ��������� �����.
typedef union {
	struct{
		//----------------------------------------
		uint8_t LineBreak;			//����� �����.
		uint8_t LineAbove;			//����� ���� �����.
		uint8_t LineNorm;			//����� � �����.
		uint8_t LineBelow;			//����� ���� �����.
		uint8_t LineShort;			//�� �� �����. ����� ���������� ������ ���
		uint8_t LinesOnTheControl;	//�������-����� ��� ������� ����� ������������� �� ��������.
		uint8_t Line;				//����� ��� ������� ���������.
		uint8_t Indicate;			//��� �������� ����� �������� �� �������.
		//----------------------------------------
	}STR;
	uint8_t BLK[8];
}__StateLine_TypeDef;
//-----------------------------------------------------------------------------
__ResistenceLine_TypeDef				ReferenceMeasureResisStr;	//��������� ��� �������� �������� ���������� ����� ��� ������� ��������.
__ResistenceLine_Coefficient_TypeDef	CoefficientStr;				//
__StateLine_TypeDef						StateLineStr; 
//-------------------------------------------
//���������� ��� ������ � ���������� ������������� �����.
static uint16_t CalibrCoeff = 0;


//-------------------------------------------
static volatile uint8_t LineChackerFlagReg  = 0;

static uint8_t LevelLC = 0;		 // ����� ������������ LC.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static uint16_t calculate_procent(uint16_t value){
	
	uint16_t temp = 0;
	//---------------------------
	switch(LevelLC)
		{
			//---------------------------
			//5.078% �� ����������� ������������� �����.
// 			case(LEVEL_5_PERCENT):
// 				temp  = (value >> 4); //6.25%
// 				temp -= (value >> 6); //1.562%
// 				temp += (value >> 8); //0.39%
// 				return temp;
// 			break; 
			//---------------------------
			//9.745% �� ����������� ������������� �����. 
			case(LEVEL_10_PERCENT):
				temp  = (value >> 4); //6.25%
				temp += (value >> 5); //3.125%
				temp += (value >> 8); //0.39%
				return temp;
			break;
			//---------------------------
			//25% �� ����������� ������������� �����.
			case(LEVEL_25_PERCENT):
				return (value >> 2);
			break;
			//---------------------------
			//40.625% �� ����������� ������������� �����.
			case(LEVEL_40_PERCENT):
				temp  = (value >> 2); //25%
				temp += (value >> 3); //12.5%
				temp += (value >> 5); //3.125%
				return temp;
			break;
			//---------------------------
			//65.625% �� ����������� ������������� �����.
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
//��������� ��������� ����� �����. ������������ �������� Rline * 10.
static uint16_t measure_resistence_one_line(uint8_t line, uint16_t delay){
	
	uint8_t i = 0;
	uint16_t meas = 0;
	
	uint16_t CalibrCoeff_temp = 0;
	uint32_t meas_temp = 0;
	uint32_t meas_temp1 = 0;
	//-------------------------
	//calibration_adc_for_line_checker();			//�������������� ����� �� �����.
	set_relay((1 << line), LINE_CHECKER_RELAY);	//����������� ���������� ����� � ���
	set_line_checker_led_green(1 << line);		//��������� ������������ �����.
	ms_delay(delay);							//����� ��� ���������� ���������� ��������� � �����.
	//-------------------------
	//get_adc_measure() ���������� 12 ��� ���� ��� ����� 6.7 ����. AVCC with external capacitor at AREF pin.
	//������� ����������������� ��� ��� ��������� ����������� ����������� ���.
	//���������� �� 8 ��������� ������ 6.7 * 8 = 53.6 ����.
	for (i = 0; i < 8; i++)
		{
			meas += get_adc_measure(LINE_CHECKER_ADC_CHANEL);
		}
	meas = (meas >> 3);
	//-------------------------
	//����������� ������������� ���������� ������������� ��� �������� ������.
	if (meas >= 2023U)//���� ������ 4000 ��.
		{
			meas = 2023U;
		}
	//-------------------------
	set_relay(ALL_RELAY_OFF, LINE_CHECKER_RELAY);//���������� ���� ���� LINE_CHECKER �� �����.
	//set_line_checker_led_green(ALL_LED_OFF);	 //������� ������� ����������� LINE_CHECKER.
	//-------------------------
	//��������� �������� ������������� ����� �� ���������������� ��������.
	//������� ���������� ������������� ����� ��� ���������� �������� R1 = R2 = 100��: Rline = (50 * Uadc) / (Ucc/2 - Uadc).
	//��� ��� ��� - ���������� ������� �����������, ��� �� ���������� ������������ ��� ������� ��������. ��� ���� Ucc = 4095 (��� 12-��� ���).
	//Uadc - ��� ���. ����������� 50 ����� ������ � ������ ������� ��������� R1 � R2 ��������. 
	//-------------------------
	//������ ������������.
	if (meas < 1862U)//������ 500 ��.
		{
			CalibrCoeff_temp = CalibrCoeff - (CalibrCoeff / 32);// + (CalibrCoeff / 64);
		}
	else if ( (meas >= 1862U) && (meas < 1950U) )//���� Rline ������ 500 � ������ 1000 ��.
		{
			CalibrCoeff_temp = CalibrCoeff - (CalibrCoeff / 8) + (CalibrCoeff / 128);
		}
	else if ( (meas >= 1950U) && (meas < 1982U) )//���� Rline ������ 1000 � ������ 1500 ��.
		{
			CalibrCoeff_temp = CalibrCoeff - (CalibrCoeff / 8);
		}
	else if (meas >= 1982U) //���� Rline ������ 1500 ��.
		{
			CalibrCoeff_temp = CalibrCoeff - (CalibrCoeff / 8) -  (CalibrCoeff / 32);
		}
	//-------------------------
	//������ ������������� �����.
	meas_temp = (2048U - meas);										
	meas_temp1 = ( ((uint32_t)meas * CalibrCoeff_temp + 5) / 10 );
	meas_temp = ( (meas_temp1 + (meas_temp / 2)) / meas_temp );
	//-------------------------
	return (uint16_t)meas_temp;
	//-------------------------
}
//-----------------------------------------------------------------------------
//���������� ��� ���������.
//���� ���������� � ���������� ��������� ���������� �� ������� ��������� 
//� ���������� �� ������ ��������� ������������ ��������. � ������ ��������� = 1.
//������������� ����� ��� ���������� �������� 100�� ������������ �� �������: Rline = (50 * Uadc) / (Ucc/2 - Uadc). 
void calibration_adc_for_line_checker(void){
	
	uint8_t  i = 0;
	uint32_t meas = 0;
	uint32_t meas_temp = 0;
	//-------------------------
	//���������� �� 8 ��������� ������ 6.7 * 8 = 53.6 ����.
	for (i = 0; i < 8; i++)
		{
			meas += get_adc_measure(LINE_CHECKER_ADC_CHANEL);
		}
	meas = (meas >> 3);
	//-------------------------	
//	meas = get_adc_measure(LINE_CHECKER_ADC_CHANEL);//���������� 12��� ���� ���. ���: AVCC with external capacitor at AREF pin.
	meas_temp = (4095 - meas);						//���������� �� ������� ��������� ��������.
	meas *= COEFF_SCALE;//10000U;					//���������� �� ������ ��������� �������� ���������� �� 10000.
	meas = ( (meas + (meas_temp / 2)) / meas_temp );//������ ��������� ����������.
	CalibrCoeff = (uint16_t)(50000000U / meas);		//������ �������������� ������������. (50 * 1000 000)/��������� ����������.
}
//-----------------------------------------------------------------------------
//����������� ��������� �����.
static void line_status_check(uint16_t meas, uint8_t number){	
	
	//----------------------------------------
	//cli(); //������ ����������.
	//----------------------------------------
	//�������� ���� ����������� ����� ��������.
	if ( (meas < (ReferenceMeasureResisStr.BLK[number] - CoefficientStr.BLK[number])) || (meas < RESISTENCE_MIN) )
		{
			//---------------------------
			//�������� �� ��.
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
			//sei(); //���������� ����������.
			return;	
		}
	//----------------------------------------
	//�������� ���� ����������� ����� ��������.
	if ( (meas > (ReferenceMeasureResisStr.BLK[number] + CoefficientStr.BLK[number])) || (meas > RESISTENCE_MAX) )
		{
			//---------------------------
			//�������� �� ����� �����.
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
			//sei(); //���������� ����������.
			return;
		}
	//----------------------------------------
	//�������� � �����.
	StateLineStr.STR.LineNorm |= (1 << number);
	//----------------------------------------
	//sei(); //���������� ����������.
	//----------------------------------------
}
//-----------------------------------------------------------------------------
//����� ��������� �����.
static void clear_line_status(uint8_t clear){
	
	StateLineStr.STR.LineBreak &= ~(clear);
	StateLineStr.STR.LineAbove &= ~(clear);
	StateLineStr.STR.LineNorm  &= ~(clear);
	StateLineStr.STR.LineBelow &= ~(clear);
	StateLineStr.STR.LineShort &= ~(clear);
}
//-----------------------------------------------------------------------------
//��������� ��� ������ ����� �� �������� �� ����������� �������.
void set_lines_to_control(uint8_t line){
	
	//	cli();
	StateLineStr.STR.LinesOnTheControl ^= line;
	//	sei();
}
//-----------------------------------------------------------------------------
//��������� ����� ����� �� ��������.
uint8_t get_lines_to_control(void){
	
	return StateLineStr.STR.LinesOnTheControl;
}
//-----------------------------------------------------------------------------
//��������� ����� ���������� ������ ���.
uint8_t get_blocked_speaker_line(void){
	
	return (~StateLineStr.STR.LineShort);
}
//-----------------------------------------------------------------------------
//��������� ��������� ������ LC.
uint8_t get_alarms_lines(void){
	
	return (StateLineStr.STR.LineBreak |
			StateLineStr.STR.LineBelow |
			StateLineStr.STR.LineAbove |
			StateLineStr.STR.LineShort);
}
//-----------------------------------------------------------------------------
//�������� ������� �����. 
static void line_check(uint8_t line, uint16_t meas){
	
	//------------------------------
	//��� ��������� ��� ����� ����� ��� ��.
	if (ReferenceMeasureResisStr.BLK[line] == 0)
		{
			//------------------------------
			//���� ����� � �����.
			if ((meas > RESISTENCE_MIN) && (meas < RESISTENCE_MAX))
				{
					ReferenceMeasureResisStr.BLK[line] = meas;		   //���������� ���������� ��������.
					CoefficientStr.BLK[line] = calculate_procent(meas);//����������� ������������ ��� ����������� ��������� ������������� ����� �����������������.
					StateLineStr.STR.LineNorm |= (1 << line);
				}
			//���� �� ��� �����, �� �� ��������� ���������� �������� � ���������� ��������������� ����.
			else
				{
					//------------------------------
					//�������� �� ��.
					if (meas <= RESISTENCE_MIN)
						{
							StateLineStr.STR.LineShort |= (1 << line);
						}
					//����� �����
					else
						{
							StateLineStr.STR.LineBreak |= (1 << line);
						}
					//------------------------------
				}
			//------------------------------
		}
	//----------------------------------------
	//��� ��������� ���� �������������� ������� ��������.
	else
		{
			//����������� ��������� ���������� �����.
			line_status_check(meas, line);
		}
	//------------------------------
}
//-----------------------------------------------------------------------------
//��������� ��������� ����� ����� ��������� �������.
void measure_resistence_all_line_after_power_on(uint8_t level){
	
	uint8_t i = 0;
	uint16_t meas_temp = 0;
	//------------------------------
	LineChackerFlagReg |= LineChecker_MeasResistFlag;
	//------------------------------
	//����� ������������ LC.
	LevelLC = level;
	//------------------------------
	//����� ��������� �����.
	clear_line_status(ALL_LINES);
	//------------------------------
	//��������� ��������� 8-�� �����.
	for (i = 0; i < 8; i++)
		{
			ReferenceMeasureResisStr.BLK[i] = 0;
			//------------------------------
			//��������� ��������� ����� �����.
			meas_temp = measure_resistence_one_line(i, LC_SMALL_TIMEOUT);
			//------------------------------
			//���� �� ����� ���� ����� �� �������� � ��� � �����.
			if ((meas_temp > RESISTENCE_MIN) && (meas_temp < RESISTENCE_MAX))
				{
					ReferenceMeasureResisStr.BLK[i] = meas_temp;		 //���������� ���������� ��������.
					CoefficientStr.BLK[i] = calculate_procent(meas_temp);//����������� ������������ ��� ����������� ��������� ������������� ����� �����������������.
					StateLineStr.STR.LineNorm |= (1 << i);				 //��������� ���� ����� � �����.
				}
			 //------------------------------
			 //���� �� ��� �����, �� �� ��������� ���������� �������� � ���������� ��������������� ����.
			 else
				 {
					 //------------------------------
					 //�������� �� ��.
					 if (meas_temp <= RESISTENCE_MIN)
						{
							StateLineStr.STR.LineShort |= (1 << i);
						}
					//------------------------------
					//����� �����
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
	set_line_checker_led_green(ALL_LED_OFF);	 //������� ������� ����������� LINE_CHECKER.
	//------------------------------
}
//----------------------------------------------------------------------------
//��������� ��������� ���� ����� �� ��������� ��������.
void measure_resistence_line_after_timeout(void){
	
	uint8_t i = 0;
	uint16_t meas_temp = 0;
	//------------------------------
	LineChackerFlagReg |= LineChecker_MeasResistFlag;
	//------------------------------
	//����� ��������� ����� ������������� �� ��������.
	clear_line_status(get_lines_to_control());
	//------------------------------
	set_line_checker_led_green(ALL_LED_OFF);
	set_line_checker_led_red(ALL_LED_OFF);
	//------------------------------
	//��������� 8�� �����.
	for (i = 0; i < 8; i++)
		{
			//------------------------------
			//���� ����� ����� �� ��������, �� ��������� �� ���������.
			if (get_lines_to_control() & (1 << i))
				{
					//��������� ��������� ����� �����.
					meas_temp = measure_resistence_one_line(i, LC_LONG_TIMEOUT);
					//����������� ��������� �����.
					line_check(i, meas_temp);
				}
			//------------------------------
		}
	//------------------------------
	LineChackerFlagReg &= ~(LineChecker_MeasResistFlag);
	//------------------------------
}
//----------------------------------------------------------------------------
//����������� ��������� ����� �����.
void one_line_check(uint8_t line){
	
	 uint8_t i = 0;	
	 uint16_t meas_temp = 0;
	//------------------------------
	if (line == 0)	{return;}
	//------------------------------
	LineChackerFlagReg |= LineChecker_MeasResistFlag;
	//------------------------------
	//����������� ������ ������� ������. �� 0 �� 7.
	for(i = 0; i < 8; i++)
		{
			if (line & (1 << i)) { break; }
		}
	//------------------------------
	//����� ��������� �����.
	clear_line_status(1 << i);
	//----------------------------------------
	set_line_checker_led_green (ALL_LED_OFF);
	set_line_checker_led_red   (ALL_LED_OFF);
	//----------------------------------------	
	meas_temp = measure_resistence_one_line(i, LC_LONG_TIMEOUT);		//��������� ��������� ����� �����.
	line_check(i, meas_temp);											//����������� ��������� �����.
	//----------------------------------------
	//cli(); //������ ����������
	ButtonSostStr.STR.LineChekerButtonSost &= ~(1 << 3);				 //����� ��������� ��������������� ������ Line Checkera.
	ButtonSostStr.STR.SpeakerButtonSost &= ~(StateLineStr.STR.LineShort);//���������� ������ ��� ���� �������������� �� �� �����.
	//------------------------------
	LineChackerFlagReg &= ~LineChecker_MeasResistFlag;
	//------------------------------
} 
//-----------------------------------------------------------------------------
void start_short_indication(uint8_t line){

	cli(); //������ ����������.
	//-------------------------------
	//��������� �������� ��������.
	Timer0FlagReg |= Timer1_ShortIndicationFlag;
	//-------------------------------
	//����� ��� ������� ����� ���������.
	StateLineStr.STR.Line = line;
	//-------------------------------
	//��������� ����� �����.
	if (StateLineStr.STR.LineBreak & line)
		{
			StateLineStr.STR.Indicate = 0xA1;
			sei(); //���������� ����������.
			return;
		}
	//-------------------------------
	//��������� �������� ���� �����.
	if (StateLineStr.STR.LineAbove & line)
		{
			StateLineStr.STR.Indicate = 0xA2;
			sei(); //���������� ����������.
			return;
		}
	//-------------------------------
	//��������� �������� ���� �����.
	if (StateLineStr.STR.LineBelow & line)
		{
			StateLineStr.STR.Indicate = 0xA3;
			sei(); //���������� ����������.
			return;
		}
	//-------------------------------
	//��.
	if (StateLineStr.STR.LineShort & line)
		{
			StateLineStr.STR.Indicate = 0xA4;
			sei(); //���������� ����������.
			return;
		}
	//-------------------------------
	//sei(); //���������� ����������.
}
//-----------------------------------------------------------------------------
//��������� ��������� ����� �����������������.
void resistence_all_line_indicate(void){
	
	uint8_t not_the_norm = 0;
	uint8_t norm = 0;
	//------------------------------
	//���� ���� ��������� �� ������ �� ����������.
	if ((LineChackerFlagReg & LineChecker_MeasResistFlag) == 0)
		{
			//��� ��� ������� ��������� ��� ��������� LC.
			Timer1FastBlinkReg = 0;
			//------------------------------
			//�������� ���������. 3 ���.
			if (Timer0FlagReg & Timer1_ShortIndicationFlag)
				{
					//-----------------------------------------------	
					//��� ��������� �� �� ������� ������ �� �������.
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
	//�������� ��������� ����������� LC ��� ���������.
	else
		{
			set_line_checker_led_green(get_relay_state(LINE_CHECKER_RELAY & Timer1FastBlinkReg));
		}
	//------------------------------
}
//-----------------------------------------------------------------------------

	

	

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------