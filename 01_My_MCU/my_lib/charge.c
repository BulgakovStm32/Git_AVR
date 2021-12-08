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
//��������� ���������� �� ������� INPUT 24V.
static uint16_t measure_input24 (void){
	
	uint8_t i;
	uint16_t meas = 0;
	uint16_t meas_temp = 0;
	//-------------------------
	msDelay(1000);
	//get_adc_measure() ���������� 12 ��� ���� ���.
	//������� ����������������� ��� ��� ��������� ����������� ����������� ���.
	//���������� �� 8 ���������.
	for (i = 0; i < 8; i++)
		{
			meas += get_adc_measure (INPUT24V_ADC_CHANEL);
		}
	meas = (meas >> 3);	//����� �� 8.
	//-------------------------
	//��������� �� ����������� ������� �������� K_res_dev.
	meas = (meas * K_res_dev);
	//��������� ���� ��� �� �������� ��� � �������� ��. ��� = 5�. 5/4096 = 0,00122� = 1,22��.
	meas_temp = meas;
	meas_temp += (meas >> 2);//1 + 0.25 = 1.25
	meas_temp -= (meas >> 5);//1.25 - 0.03125 = 1,21875.
	//-------------------------
	return meas_temp;//����������� �������� � ��.
	//-------------------------
}
//-----------------------------------------------------------------------------
void charge_init(void){
	
	//���������� ������� ���.����� �� ����� � � +5.
	CHARGE_DDR  |=  CHARGE_LEG;
	CHARGE_PORT |=  CHARGE_LEG;//���������� ��.
	
	POWER220_DDR  &= ~POWER220_LEG; //����� �� ����.
	POWER220_PORT &= ~POWER220_LEG; //Z-���������.
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
//������� ���������, ������������ ������ �������� ���.
void charge_monitoring_on(void){
	
	static uint16_t measure = 0;
	//-------------------------
	if ( !((Timer0FlagReg & Timer1_Charge15secFlag) | (Timer0FlagReg & Timer1_ChargeChekrFlag)) )
		{
			//���������� ����� ������.
			CHARGE_OFF;
			//��������� ���������� �� ������� INPUT 24V.
			measure = measure_input24();
		}
	else
		{
			return;
		}
	//-------------------------
	//����������� ��������� ������������� ���.
	//�� �� �������.
	if (measure <= INPUT24V_SHORT)
		{
			//��������� ��������� "��" �� �������.
			ChargeIndicate = INPUT24V_INDICATE_ALARM;//INPUT24V_INDICATE_SHORT;
			//���������� ����� ������.
			CHARGE_OFF;
			//����� 15 ������ �������� ��������� ���.
			Timer0FlagReg |= Timer1_Charge15secFlag;
			return;
		}
	//-------------------------
	//����� �������������� �������� ��� �� ����� ������ �� ����������. 
	if (measure <= INPUT24V_BREAK)
		{
			//��������� ��������� �� ������� "��".
			ChargeIndicate = INPUT24V_INDICATE_ALARM;// INPUT24V_INDICATE_BREAK;
			//���������� ����� ������.
			CHARGE_OFF;
			//����� 15 ������ �������� ��������� ���.
			Timer0FlagReg |= Timer1_Charge15secFlag;
			return;
		}
	//-------------------------
	//�������� ������ ���. 19,5� � ����.
	if (measure <= INPUT24V_ALARM)
		{
			//��������� ��������� �� ������� "��".
			ChargeIndicate = INPUT24V_INDICATE_ALARM;
			//-------------------------
			//���� ���������� ������ ������������ �� �� �� ��������.
			if (measure <= INPUT24V_MIN)
				{
					CHARGE_OFF;
				}
			else
				{
					CHARGE_ON;	
				}
			//-------------------------
			//����� 15 ������ �������� ��������� ���.
			Timer0FlagReg |= Timer1_Charge15secFlag;		
			return;
		}
	//-------------------------
	//����������� ����������� ����� ���������� ������� � ����������� �������.
	//������� ���������� ��� ���������� �� ��� 20,5 �.
	if (measure >= INPUT24V_ALARM_OFF)
		{
			//���������� ��������� ��������� ��.
			clear_charge_indication();
			//-------------------------
			//���� ���������� �� ��� ���� ������������� �� ���������� ��.
			if (measure >= INPUT24V_MAX)
				{
					CHARGE_OFF;
				}
			else
				{
					CHARGE_ON;
				}
			//-------------------------
			//����� 5 ��� �������� ��������� ���.
			 clear_charge_hour_counter();
			Timer0FlagReg |= Timer1_ChargeChekrFlag;
		}
	else
		{
			//��������� ����� ������.
			CHARGE_ON;
			//����� 15 ������ �������� ��������� ���.
			Timer0FlagReg |= Timer1_Charge15secFlag;
		}
	//-------------------------
}
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------






