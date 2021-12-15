/*
 * GccApplication1.cpp
 *
 * Created: 10.12.2021 13:25:38
 * Author : belyaev
 */ 

#include <avr/io.h>

class Reg{
	
	private:
		static uint8_t reg1;
		
	public:
		static uint8_t reg2;

};

uint8_t Reg::reg1 = 0;
uint8_t Reg::reg2 = 0;

int main(void)
{
	Reg r1;
	
	//r1.reg1 = 0;
	//r1.reg2 = 0;
	
	
    /* Replace with your application code */
    while (1) 
    {
    }
}

