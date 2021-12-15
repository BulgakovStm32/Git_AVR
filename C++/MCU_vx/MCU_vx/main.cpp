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
AtmegaConfig			atmegaConfig;

//TMC2130::InterfaceImpl 	tmc2130; /** Включает в себя и tmc4361 */
//mcu::Mcu 				mcu5(tmc2130.tmc4361_interface(), &tmc2130);
//I2cDispatcher 			commandHandler(mcu5);
//CommandReceiver_I2C 	commandReceiverI2C(commandHandler); // тут используются методы handler как коллбэки

//*******************************************************************************************
//*******************************************************************************************
//void initVariant()
//{
	//// Выключаем Watch Dog
	//cli();
	//wdt_reset();
	//MCUSR  &= ~(1<<WDRF);
	//WDTCSR |=  (1<<WDCE) | (1<<WDE);
	//WDTCSR  =   0x0;
	//sei();
//}


//*******************************************************************************************
//*******************************************************************************************
void Init(void){
	
	atmegaConfig.i2c_baudrate = 1000000;
	atmegaConfig.i2c_address  = 0x06;
	atmegaConfig.com_speed    = 57600;
	
	
	
	//PowerManager::instance().setup();
	
	//Ожидание нажатия кнопки включения питания.
	//PowerManager::instance().waitPowerButton();
	
	//PowerManager::instance().turnOn();
	//TempSensorManager::instance().setup();

	//COM_INFO("turning on command handler...");
	//clockOnForTMC4361();
	//
	//COM_INFO("starting command receiver");
	//commandReceiverI2C.start(atmegaConfig.i2c_address, atmegaConfig.i2c_baudrate);
	//
	//COM_INFO("starting tmc2130 interface");
	//tmc2130.start(atmegaConfig.config4361.clockFrequency);
	//
	//COM_INFO("setup mcu");
	//mcu5.setup(atmegaConfig.config4361, atmegaConfig.config2130);
	//COM_INFO("command handler turned on");
	//
	//commandHandler.setup();
	//COM_INFO("setup finished");
//
	//PowerManager::instance().enableFan(true); // Сразу включаем вентиляторы
}
//*************************************************************



//*******************************************************************************************
//*******************************************************************************************
int main(void){
	
	//------------------------------
	Init();
	
	//----------------------------------------------------------------------
	while (1)
	{
		
	}
	//----------------------------------------------------------------------
	
	//TimerManager::instance().update();
	//PowerManager::instance().check();
	//TempSensorManager::instance().update();
	//commandHandler.executeCtrlRequestTime();
}
//*******************************************************************************************
//*******************************************************************************************
