#pragma once

//#include "Peripherals/tmc4361config.h"
//#include "Peripherals/tmc2130config.h"
#include "main.h"


//extern const char FIRMWARE_VERSION[];

struct AtmegaConfig{
	
	uint32_t i2c_baudrate;// = 100000;///скорость работы i2c
	uint8_t  i2c_address; // = 0x06;  ///адрес устройства на шине
	uint32_t com_speed;   // = 57600; ///скорость работы ком порта(9600 38400 57600 115200)
//    TMC4361::Config 	config4361;///конфиг чипа TMC4361
//    TMC2130::Config 	config2130;///конфиг чипа TMC2130
};
