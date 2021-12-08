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
#include "my_lib\charge.h"
//-----------------------------------------------------------------------------
extern __IndicatorFacePanel_TypeDef	IndicatorsFacePanelStr;

static volatile uint8_t ChargeIndicate = 0;
//-----------------------------------------------------------------------------
//Измерение напряжение на клеммах INPUT 24V.
static uint16_t measure_input24 (void){
	
	uint8_t i;
	uint16_t meas = 0;
	uint16_t meas_temp = 0;
	//-------------------------
	msDelay(1000);
	//get_adc_measure() возвращает 12 бит кода АЦП.
	//Введена передескритезация АЦП для повышения разрешающей способности АЦП.
	//Усреднение по 8 выборокам.
	for (i = 0; i < 8; i++)
		{
			meas += get_adc_measure (INPUT24V_ADC_CHANEL);
		}
	meas = (meas >> 3);	//Делим на 8.
	//-------------------------
	//Умножение на коэффециент деления делителя K_res_dev.
	meas = (meas * K_res_dev);
	//Умножение кода АЦП на значение ИОН в единицах мВ. ИОН = 5В. 5/4096 = 0,00122В = 1,22мВ.
	meas_temp = meas;
	meas_temp += (meas >> 2);//1 + 0.25 = 1.25
	meas_temp -= (meas >> 5);//1.25 - 0.03125 = 1,21875.
	//-------------------------
	return meas_temp;//Возвращение значения в мВ.
	//-------------------------
}
//-----------------------------------------------------------------------------
void charge_init(void){
	
	//Управление зарядом АКБ.ножку на выход и к +5.
	CHARGE_DDR  |=  CHARGE_LEG;
	CHARGE_PORT |=  CHARGE_LEG;//Отключение ЗУ.
	
	POWER220_DDR  &= ~POWER220_LEG; //Ножку на вход.
	POWER220_PORT &= ~POWER220_LEG; //Z-состояние.
}
//-----------------------------------------------------------------------------
uint8_t get_charge_indication(void){
		
	if (POWER220_PIN & POWER220_LEG)
		{
			DisplayBright = 1;
			return POWER220_INDICATE_ALARM;
		} 
	else
		{
			DisplayBright = TempDisplayBright;
			return ChargeIndicate;
		}
}  
//-----------------------------------------------------------------------------
void set_charge_indication(uint8_t byte){
	
	ChargeIndicate = byte;
}
//-----------------------------------------------------------------------------
void clear_charge_indication(void){
	
	if ( (POWER220_PIN & POWER220_LEG) == 0 )
		{
			IndicatorsFacePanelStr.bit.POWER_LED = 0;
		}
	ChargeIndicate = 0;
	
	
	//ChargeIndicate = 0;
	//IndicatorsFacePanelStr.bit.POWER_LED = 0;
}
//-----------------------------------------------------------------------------
//Автомат состояний, определяющий работу контроля АКБ.
void charge_monitoring_on(void){
	
	static uint16_t measure = 0;
	//-------------------------
	if ( !((Timer0FlagReg & Timer1_Charge15secFlag) | (Timer0FlagReg & Timer1_ChargeChekrFlag)) )
		{
			//Отключение блока заряда.
			CHARGE_OFF;
			//Измерение напряжение на клеммах INPUT 24V.
			measure = measure_input24();
		}
	else
		{
			return;
		}
	//-------------------------
	//Определение состояния подключенного АКБ.
	//КЗ на клеммах.
	if (measure <= INPUT24V_SHORT)
		{
			//Включение индикации "НА" на дисплее.
			ChargeIndicate = INPUT24V_INDICATE_ALARM;//INPUT24V_INDICATE_SHORT;
			//Отключение блока заряда.
			CHARGE_OFF;
			//Через 15 секунд проверка состояния АКБ.
			Timer0FlagReg |= Timer1_Charge15secFlag;
			return;
		}
	//-------------------------
	//Обрыв соединительных проводов или ко входу ничего не подключено. 
	if (measure <= INPUT24V_BREAK)
		{
			//Включение индикации на дисплее "НА".
			ChargeIndicate = INPUT24V_INDICATE_ALARM;// INPUT24V_INDICATE_BREAK;
			//Отключение блока заряда.
			CHARGE_OFF;
			//Через 15 секунд проверка состояния АКБ.
			Timer0FlagReg |= Timer1_Charge15secFlag;
			return;
		}
	//-------------------------
	//Глубокий разряд АКБ. 19,5В и ниже.
	if (measure <= INPUT24V_ALARM)
		{
			//Включение индикации на дисплее "НА".
			ChargeIndicate = INPUT24V_INDICATE_ALARM;
			//-------------------------
			//Если напряжение меньше минимального то ЗУ не включаем.
			if (measure <= INPUT24V_MIN)
				{
					CHARGE_OFF;
				}
			else
				{
					CHARGE_ON;	
				}
			//-------------------------
			//Через 15 секунд проверка состояния АКБ.
			Timer0FlagReg |= Timer1_Charge15secFlag;		
			return;
		}
	//-------------------------
	//Обеспечение гестирезиса между включением тревоги и отключением тревоги.
	//Тревога отключится при напряжении на АКБ 20,5 В.
	if (measure >= INPUT24V_ALARM_OFF)
		{
			//Отключение аварийной индикации ЗУ.
			clear_charge_indication();
			//-------------------------
			//Если напряжение на АКБ выше максимального то отключение ЗУ.
			if (measure >= INPUT24V_MAX)
				{
					CHARGE_OFF;
				}
			else
				{
					CHARGE_ON;
				}
			//-------------------------
			//Через 5 мин проверка состояния АКБ.
			 clear_charge_hour_counter();
			Timer0FlagReg |= Timer1_ChargeChekrFlag;
		}
	else
		{
			//Включение блока заряда.
			CHARGE_ON;
			//Через 15 секунд проверка состояния АКБ.
			Timer0FlagReg |= Timer1_Charge15secFlag;
		}
	//-------------------------
}
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------






