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
#include "pt6964.h"
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//������� ��������, ����������� �� flash.
const uint8_t sumbolTable[] PROGMEM = {	
	(Seg_a | Seg_b | Seg_c | Seg_d | Seg_e | Seg_f),		//����� 00 - ����� 0. 
	(Seg_b | Seg_c),										//����� 01 - ����� 1.
	(Seg_a | Seg_b | Seg_d | Seg_e | Seg_g),				//����� 02 - ����� 2.
	(Seg_a | Seg_b | Seg_c | Seg_d | Seg_g),				//����� 03 - ����� 3.
	(Seg_b | Seg_c | Seg_f | Seg_g),						//����� 04 - ����� 4.
	(Seg_a | Seg_c | Seg_d | Seg_f | Seg_g),				//����� 05 - ����� 5.
	(Seg_a | Seg_c | Seg_d | Seg_e | Seg_f | Seg_g),		//����� 06 - ����� 6.
	(Seg_a | Seg_b | Seg_c ),								//����� 07 - ����� 7.
	(Seg_a | Seg_b | Seg_c | Seg_d | Seg_e | Seg_f | Seg_g),//����� 08 - ����� 8.
	(Seg_a | Seg_b | Seg_c | Seg_d | Seg_f | Seg_g), 		//����� 09 - ����� 9.	
	
	(Seg_b | Seg_c | Seg_e | Seg_f | Seg_g ),				//����� 0A - ����� H.
	(Seg_a | Seg_d | Seg_e | Seg_f ),						//����� 0B - ����� C.
	(Seg_a | Seg_e | Seg_f ),								//����� 0C - ����� �.
	(Seg_b | Seg_c | Seg_d | Seg_f | Seg_g),				//����� 0D - ����� �.
	(Seg_a | Seg_b | Seg_c | Seg_e | Seg_f ),				//����� 0E - ����� �.
	(Seg_a | Seg_b | Seg_c | Seg_e | Seg_f | Seg_g), 		//����� 0F - ����� A.
	(Seg_a | Seg_b | Seg_e | Seg_f | Seg_g ),				//����� 10 - ����� P.
	(Seg_a | Seg_d | Seg_e | Seg_f | Seg_g )				//����� 11 - ����� E.
	
//(Seg_a | Seg_d | Seg_e | Seg_f ),						//����� D - ����� C.	
//	(Seg_a | Seg_e | Seg_f )								//����� �.
//	(Seg_c | Seg_e | Seg_g),								//����� B - ����� n.	
//	(Seg_a | Seg_e | Seg_f | Seg_g ),						//����� C - ����� F.	
	
	
	//(Seg_a | Seg_b | Seg_c | Seg_e | Seg_f | Seg_g), 		//����� A.
	//(Seg_c | Seg_d | Seg_e | Seg_f | Seg_g ),		 		//����� B.
	//(Seg_a | Seg_d | Seg_e | Seg_f ),						//����� C.
	//(Seg_b | Seg_c | Seg_d | Seg_e | Seg_g ),		 		//����� D.
	//(Seg_a | Seg_d | Seg_e | Seg_f | Seg_g ),				//����� E.
	//(Seg_a | Seg_e | Seg_f | Seg_g )						//����� F.
	
};
//---------------------------------------------------------------------------
__IndicatorFacePanel_TypeDef	IndicatorsFacePanelStr;
__ButtonFacePanel_TypeDef		ButtonFacePanelStr;
__ButtonSost_TypeDef			ButtonSostStr;

//static volatile	uint8_t EMButtonSost  = 0;
//static volatile uint8_t LCPressedButtons = 0; //������� ������ LC. 

uint8_t DisplayBright	  = DEFAULT_BRIGHT_DISPLAY; //�������� ������� �������� ���������.
uint8_t TempDisplayBright = DEFAULT_BRIGHT_DISPLAY;	//������� ��� ����������� �������� �������� ������� �������� ���������.
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
void pt6964_init(void){
	
	CLKDDR |= (1<<CLKPIN);	//������ �� �����.
	STBDDR |= (1<<STBPIN);	//
	DIODDR |= (1<<DIOPIN);	//
	
	HIGH(STBPORT, STBPIN);
	HIGH(CLKPORT, CLKPIN);
	LOW(DIOPORT, DIOPIN);
}
//-----------------------------------------------------------------------------
static void write_byte_to_pt6964(uint8_t byte){
	
	uint8_t i = 0;
	//------------------------------
	for (i = 0; i < 8; i++)
		{
			LOW(CLKPORT, CLKPIN);
			//_delay_us(CLKSPEED);
			if (byte & (1 << i))
				{
					HIGH(DIOPORT, DIOPIN);
				}
			else
				{
					LOW(DIOPORT, DIOPIN);
				}
			HIGH(CLKPORT, CLKPIN);
			//_delay_us(CLKSPEED);
		}
}
//-----------------------------------------------------------------------------
static void write_command_to_pt6964(uint8_t command){
	
	LOW(STBPORT, STBPIN);
	//_delay_us(CLKSPEED);
	write_byte_to_pt6964(command);
	//_delay_us(CLKSPEED);
	LOW(DIOPORT, DIOPIN);
	HIGH(STBPORT, STBPIN);
}
//-----------------------------------------------------------------------------
void write_data_to_face_penel(void){
	
	volatile uint8_t i = 0;
	//------------------------------	
 	write_command_to_pt6964(COMMAND2_WRITE);
 	LOW(STBPORT, STBPIN);
 	//pt6964_pausa();//_delay_us(CLKSPEED);
 	write_byte_to_pt6964(COMMAND3_ADDRESS_00);
 	//pt6964_pausa();//_delay_us(CLKSPEED);
 	//--------------------
 	//�������� ������� ���� � ������� ������.
 	for (i = 0; i < QUANTITY_FACE_PANEL_BYTE; i++)
 		{
 			write_byte_to_pt6964(IndicatorsFacePanelStr.BLK[i]);
 		}
 	//--------------------
 	//pt6964_pausa();//_delay_us(CLKSPEED);
 	HIGH(STBPORT, STBPIN);
 	//pt6964_pausa();//_delay_us(CLKSPEED);
 
 	write_command_to_pt6964(COMMAND1_7DIGITS_10SEG);	
	//write_command_to_pt6964(COMMAND4_DISPLAY_ON);
	write_command_to_pt6964(COMMAND4_DISPLAY_ON | DisplayBright);
}
//-----------------------------------------------------------------------------
uint8_t bin_to_dec (uint8_t byte){
	
	uint8_t des = 0;
	//------------------------------
	while(byte >= 10)
		{
			byte -= 10;
			des++;
		}
	return (((des << 4) & 0xF0) | (byte & 0x0F));
}
//-----------------------------------------------------------------------------
//����� ��������������� ���������.
//������������ �������� byte �� �������. 
//������� 4 ���� ������������ �� ������ ����������.
//������� 4 ���� ������������ �� ����� ����������.
void display_set(uint16_t byte){
	
	uint8_t temp = 0;
	//------------------------------	
	cli(); //������ ����������.		
	//------------------------------
	if (byte <= 0x00FF)
		{
			//������� ������ ������ �������.
			temp = 0x80 & IndicatorsFacePanelStr.BLK[8];
			temp |= pgm_read_byte_near (sumbolTable + (byte & 0x0F));
			IndicatorsFacePanelStr.BLK[8] = temp;
			//------------------------------
			//������� ����� ������ �������.
			temp = 0x80 & IndicatorsFacePanelStr.BLK[6];
			temp |= pgm_read_byte_near (sumbolTable + ((byte & 0xF0)>>4));
			IndicatorsFacePanelStr.BLK[6] = temp;
		} 
	else
		{
			//������� ������ ������ �������.
			temp = 0x80 & IndicatorsFacePanelStr.BLK[8];
			temp |= pgm_read_byte_near (sumbolTable + (byte & 0x00FF));
			IndicatorsFacePanelStr.BLK[8] = temp;
			//------------------------------
			//������� ����� ������ �������.
			temp = 0x80 & IndicatorsFacePanelStr.BLK[6];
			temp |= pgm_read_byte_near (sumbolTable + ((byte & 0x7F00)>>8));
			IndicatorsFacePanelStr.BLK[6] = temp;
		}
	//------------------------------
	sei(); //���������� ����������.	
	//------------------------------	
}
//-----------------------------------------------------------------------------
void display_off(void){
	
	IndicatorsFacePanelStr.BLK[8] &= 0x80;
	IndicatorsFacePanelStr.BLK[6] &= 0x80;
}
//-----------------------------------------------------------------------------
void power_led_green(void){
	
	cli(); //������ ����������.
	Timer0FlagReg &= ~Timer1_StendbyFlag;	 //���� ���� ����� ��� ����������� ��������� � �������� ������.
	IndicatorsFacePanelStr.bit.STBAY_LED = 1;//
	if (get_charge_indication())
		{
			IndicatorsFacePanelStr.bit.POWER_LED = 1;//
		}
	else
		{
			IndicatorsFacePanelStr.bit.POWER_LED = 0;//
		}
	sei(); //���������� ����������.
}
//-----------------------------------------------------------------------------
void power_led_blink_green(void){
	
	cli(); //������ ����������.
	Timer0FlagReg |= Timer1_StendbyFlag;//���� ���� ����� ��� ����������� ��������� � �������� ������.
	sei(); //���������� ����������.
}
//-----------------------------------------------------------------------------
void set_line_checker_led_green(uint8_t led){
	
	//------------------------------
	//cli(); //������ ����������.
	//------------------------------
		if (led & Led_1)
			{
				IndicatorsFacePanelStr.bit.LC1_GREEN_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.LC1_GREEN_LED = 0;
			}
	//------------------------------
		if (led & Led_2)
			{
				IndicatorsFacePanelStr.bit.LC2_GREEN_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.LC2_GREEN_LED = 0;
			}
	//------------------------------
		if (led & Led_3)
			{
				IndicatorsFacePanelStr.bit.LC3_GREEN_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.LC3_GREEN_LED = 0;
			}
	//------------------------------
		if (led & Led_4)
			{
				IndicatorsFacePanelStr.bit.LC4_GREEN_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.LC4_GREEN_LED = 0;
			}
	//------------------------------
		if (led & Led_5)
			{
				IndicatorsFacePanelStr.bit.LC5_GREEN_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.LC5_GREEN_LED = 0;
			}
	//------------------------------
		if (led & Led_6)
			{
				IndicatorsFacePanelStr.bit.LC6_GREEN_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.LC6_GREEN_LED = 0;
			}	
	//------------------------------
		if (led & Led_7)
			{
				IndicatorsFacePanelStr.bit.LC7_GREEN_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.LC7_GREEN_LED = 0;
			}
	//------------------------------
		if (led & Led_8)
			{
				IndicatorsFacePanelStr.bit.LC8_GREEN_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.LC8_GREEN_LED = 0;
			}
	//------------------------------
	//sei(); //���������� ����������.		
}
//-----------------------------------------------------------------------------
void set_line_checker_led_red(uint8_t led){
	
	//cli(); //������ ����������.
	//------------------------------
		if (led & Led_1)
			{
				IndicatorsFacePanelStr.bit.LC1_RED_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.LC1_RED_LED = 0;
			}
	//------------------------------
		if (led & Led_2)
			{
				IndicatorsFacePanelStr.bit.LC2_RED_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.LC2_RED_LED = 0;
			}
	//------------------------------
		if (led & Led_3)
			{
				IndicatorsFacePanelStr.bit.LC3_RED_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.LC3_RED_LED = 0;
			}
	//------------------------------
		if (led & Led_4)
			{
				IndicatorsFacePanelStr.bit.LC4_RED_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.LC4_RED_LED = 0;
			}
	//------------------------------
		if (led & Led_5)
			{
				IndicatorsFacePanelStr.bit.LC5_RED_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.LC5_RED_LED = 0;
			}
	//------------------------------
		if (led & Led_6)
			{
				IndicatorsFacePanelStr.bit.LC6_RED_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.LC6_RED_LED = 0;
			}
	//------------------------------
		if (led & Led_7)
			{
				IndicatorsFacePanelStr.bit.LC7_RED_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.LC7_RED_LED = 0;
			}
	//------------------------------
		if (led & Led_8)
			{
				IndicatorsFacePanelStr.bit.LC8_RED_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.LC8_RED_LED = 0;
			}
	//------------------------------
	//sei(); //���������� ����������.
}
//-----------------------------------------------------------------------------
void set_speaker_led_green(uint8_t led){
	
		//cli(); //������ ����������.
		//------------------------------
		if (led & Led_1)
			{
				IndicatorsFacePanelStr.bit.SP1_GREEN_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.SP1_GREEN_LED = 0;
			}
		//------------------------------
		if (led & Led_2)
			{
				IndicatorsFacePanelStr.bit.SP2_GREEN_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.SP2_GREEN_LED = 0;
			}
		//------------------------------
		if (led & Led_3)
			{
				IndicatorsFacePanelStr.bit.SP3_GREEN_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.SP3_GREEN_LED = 0;
			}
		//------------------------------
		if (led & Led_4)
			{
				IndicatorsFacePanelStr.bit.SP4_GREEN_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.SP4_GREEN_LED = 0;
			}
		//------------------------------
		if (led & Led_5)
			{
				IndicatorsFacePanelStr.bit.SP5_GREEN_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.SP5_GREEN_LED = 0;
			}
		//------------------------------
		if (led & Led_6)
			{
				IndicatorsFacePanelStr.bit.SP6_GREEN_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.SP6_GREEN_LED = 0;
			}
		//------------------------------
		if (led & Led_7)
			{
				IndicatorsFacePanelStr.bit.SP7_GREEN_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.SP7_GREEN_LED = 0;
			}
		//------------------------------
		if (led & Led_8)
			{
				IndicatorsFacePanelStr.bit.SP8_GREEN_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.SP8_GREEN_LED = 0;
			}
		//------------------------------
		//sei(); //���������� ����������.
}
//-----------------------------------------------------------------------------
void set_speaker_led_red(unsigned char led){
	
		//cli(); //������ ����������.
		//------------------------------
		if (led & Led_1)
			{
				IndicatorsFacePanelStr.bit.SP1_RED_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.SP1_RED_LED = 0;
			}
		//------------------------------
		if (led & Led_2)
			{
				IndicatorsFacePanelStr.bit.SP2_RED_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.SP2_RED_LED = 0;
			}
		//------------------------------
		if (led & Led_3)
			{
				IndicatorsFacePanelStr.bit.SP3_RED_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.SP3_RED_LED = 0;
			}
		//------------------------------
		if (led & Led_4)
			{
				IndicatorsFacePanelStr.bit.SP4_RED_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.SP4_RED_LED = 0;
			}
		//------------------------------
		if (led & Led_5)
			{
				IndicatorsFacePanelStr.bit.SP5_RED_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.SP5_RED_LED = 0;
			}
		//------------------------------
		if (led & Led_6)
			{
				IndicatorsFacePanelStr.bit.SP6_RED_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.SP6_RED_LED = 0;
			}
		//------------------------------
		if (led & Led_7)
			{
				IndicatorsFacePanelStr.bit.SP7_RED_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.SP7_RED_LED = 0;
			}
		//------------------------------
		if (led & Led_8)
			{
				IndicatorsFacePanelStr.bit.SP8_RED_LED = 1;
			}
		else
			{
				IndicatorsFacePanelStr.bit.SP8_RED_LED = 0;
			}
		//------------------------------
		//sei(); //���������� ����������.
}
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
static uint8_t read_byte_from_pt6964(void){
	
	uint8_t i = 0;
	uint8_t byte = 0;
	//------------------------------
	for (i = 0; i < 8; i++)
		{
			__asm__ volatile ("nop");
			LOW(CLKPORT, CLKPIN);
			__asm__ volatile ("nop");
			__asm__ volatile ("nop");
			__asm__ volatile ("nop");
			__asm__ volatile ("nop");
			//-------------------------------
			if (DIOPININ & (1<<DIOPIN))
				{
					byte |= (1<<i);
					__asm__ volatile ("nop");
				}
			else
				{
					byte &= ~(1<<i);
				}
			//-------------------------------
			__asm__ volatile ("nop");
			HIGH(CLKPORT, CLKPIN);
			__asm__ volatile ("nop");
			__asm__ volatile ("nop");
			__asm__ volatile ("nop");
			__asm__ volatile ("nop");
			__asm__ volatile ("nop");
			__asm__ volatile ("nop");
			__asm__ volatile ("nop");
			
		}
	//-------------------------------
	return byte;
}
//-----------------------------------------------------------------------------
static void read_all_keys_from_pt6964(__ButtonFacePanel_TypeDef *sostBut){

	uint8_t i = 0;
	//-------------------------------
	//���������� �������� ������ �� PT6964.
	LOW(STBPORT, STBPIN);
	//_delay_us(CLKSPEED);
	write_byte_to_pt6964(COMMAND2_READ);
	//_delay_us(CLKSPEED);
	DIODDR &= ~(1<<DIOPIN);//����� �� ����.
	//_delay_us(CLKSPEED);

	for (i = 0; i < QUANTITY_READ_BYTE; i++)
		{
			sostBut->BLK[i] = read_byte_from_pt6964();
		}

	LOW(DIOPORT, DIOPIN);
	DIODDR |= (1<<DIOPIN);//����� �� �����.
	HIGH(STBPORT, STBPIN);
	LOW(CLKPORT, CLKPIN);
}
//-----------------------------------------------------------------------------
//����������� ��������� ������ � ������������ ����������� ��������� ������.
//���������� � ���������� �� �������1 ������ 10 ����. 
void read_button_from_face_penel(void){
 
 	uint8_t i = 0;
 	static uint8_t cycle = 0;
 	static __ButtonFacePanel_TypeDef SostBut[3];
 	//-------------------------------
 	if (cycle < 3)
 		{
 			read_all_keys_from_pt6964(&SostBut[cycle]);//����������� ��������� ������
			//��������� ����������� ���. 
			//���� ���� �� ����������� ��� ����� ����� 1 ������ ����������� �������� ������ �� pt6964.
			if ((SostBut[cycle].BLK[0] & (CHECKBIT1 | CHECKBIT2)) ||
				(SostBut[cycle].BLK[1] & (CHECKBIT1 | CHECKBIT2)) ||
				(SostBut[cycle].BLK[2] & (CHECKBIT1 | CHECKBIT2)) ||
				(SostBut[cycle].BLK[3] & (CHECKBIT1 | CHECKBIT2)) ||
				(SostBut[cycle].BLK[4] & (CHECKBIT1 | CHECKBIT2)))
				{
					cycle = 0; 
				}
			else
				{
					cycle++;	
				}
 			return;
 		} 
 	//-------------------------------
 	else
 		{
 			cycle = 0;
 			//������������ ����������� ��������� ������.
  			for (i = 0; i < 5; i++)
  				{
  					ButtonFacePanelStr.BLK[i] = ((SostBut[0].BLK[i] & SostBut[1].BLK[i]) |
  												 (SostBut[1].BLK[i] & SostBut[2].BLK[i]) |
  												 (SostBut[0].BLK[i] & SostBut[2].BLK[i]));
  				}
 		}
 	//-------------------------------
 }
//-----------------------------------------------------------------------------
static uint8_t sorting_button_bit (__ButtonFacePanel_TypeDef *button_array, uint8_t mask){
	
	uint8_t i = 0;
	uint8_t temp = 0;
	//-------------------------------------
	//cli(); //������ ����������.
	//-------------------------------------
	for (i = 0; i < 4; i++)
		{
			temp |= ((((button_array->BLK[i] >> mask) & 0b00000001) | (((button_array->BLK[i] >> mask) & 0b00001000)>>2)) << (i<<1));
		}
	//-------------------------------------
	//sei(); //���������� ����������.
	//-------------------------------------	
	return temp;
}
//-----------------------------------------------------------------------------
//��������� ����������� ��������� ������.
//��������� ������ ����������� � ���������� �� �������1 ������ 10 ����.
uint8_t get_button (uint8_t buttons){
		
	uint8_t temp = 0;
	//-------------------------------
	//���������� �������� �����.
	switch(buttons)
		{	
			//-------------------------------
			case LINE_CHECKER_BUTTON:
				temp = sorting_button_bit(&ButtonFacePanelStr, LINE_CHECKER_BUTTON_BIT_MASK);
			break;
			//-------------------------------
			case SPEAKER_BUTTON:		
				temp = sorting_button_bit(&ButtonFacePanelStr, SPEAKER_BUTTON_BIT_MASK);	
			break;
			//-------------------------------
			case ALL_EM_BUTTON:
				temp = (ButtonFacePanelStr.BLK[4] & (ALL_BUTTON | EM_BUTTON));	
			break;
			//-------------------------------
		}
	//-------------------------------------
	return temp;
}
//-----------------------------------------------------------------------------
uint8_t toggle_button(uint8_t button){
	
	uint8_t	temp = 0;
	//-------------------------------------------------------------
	//������ ��������� ������.
	temp = get_button(button);	
	switch (temp)
		{
			//-------------------------------
			case 0:
				ButtonSostStr.BLK[button-2] = 0;
				//--------------------------
				//���������� ������ ���.
				if (button == SPEAKER_BUTTON)
					{
						ButtonSostStr.BLK[button-1] &= get_blocked_speaker_line();
					}
				//--------------------------
			break;
			//-------------------------------
			default:		
				if(ButtonSostStr.BLK[button-2] != temp)
					{
						ButtonSostStr.BLK[button-2]  = temp;
						ButtonSostStr.BLK[button-1] ^= temp;
						//--------------------------
						//���������� ������ ���.
						if (button == SPEAKER_BUTTON)
							{
								ButtonSostStr.BLK[button-1] &= get_blocked_speaker_line();
							}
						//--------------------------
					}	
			break;
			//-------------------------------
		}
	//-------------------------------------------------------------
	return ButtonSostStr.BLK[button-1];
}
//-----------------------------------------------------------------------------
uint8_t config_parameter_selection(uint8_t mode){
	
	static uint8_t	ButtonTemporarySost;//��������� ��������� ������. ��� ������ �� ������� ������� ������.
	static uint8_t	ButtonSost;			//��������� ������.
	uint8_t	temp = 0;
	//-------------------------------
	//���� ��������� ��������������� �� ������� ��������� ��������� ������.
	if (mode == CONFIG_END)
		{
			ButtonTemporarySost = 0;
			ButtonSost = 0;
			return 0;
		}
	//-------------------------------------------------------------
	//����� ������ �� � ������ ��������� �� ������� ��� ������� ������ SP8.
	if (get_button(SPEAKER_BUTTON) & SP8_BUTTON)
		{
			return 0xFF;
		}
	//-------------------------------------------------------------
	//������ ��������� ������.
	temp = (get_button(LINE_CHECKER_BUTTON) & 0b01111111);//����� ��� ��������� ������ 8 LC ������.
	switch (temp)
		{
			//-------------------------------
			case 0:
				ButtonTemporarySost = 0;
			break;
			//-------------------------------
			default:
				if(ButtonTemporarySost != temp)
					{
						ButtonTemporarySost = temp;
						ButtonSost = temp;
					}
			break;
			//-------------------------------
		}
	//-------------------------------------------------------------
	return ButtonSost;
}
//-----------------------------------------------------------------------------
void modification_config_parameter(uint8_t *parameter, uint8_t min, uint8_t max){
	
	static uint8_t ButtonTemporarySost;//��������� ��������� ������. 
	//-------------------------------
	if ((Timer0FlagReg & Timer1_LongPressSPButtomFlag) != 0)
		{
			Timer0FlagReg &= ~Timer1_LongPressSPButtomFlag;
			ButtonTemporarySost = 0;
		}
	//-------------------------------
	switch(get_button(SPEAKER_BUTTON))
		{
			//---------------------------------------------
			case SP1_BUTTON:
				if((ButtonTemporarySost & SP1_BUTTON) == 0)
					{
						ButtonTemporarySost	|= SP1_BUTTON;
						//-------------------------
						if ((*parameter) > min)//�������� �� �������.
							{
								(*parameter)--;
							}
						//-------------------------
						//��������������� �������������� ���������.
						else
							{
								(*parameter) = max;
							}
						//-------------------------
					}
			break;
			//---------------------------------------------
			case SP2_BUTTON:
				if((ButtonTemporarySost & SP2_BUTTON) == 0)
					{
						ButtonTemporarySost	|= SP2_BUTTON;
						//-------------------------
						if ((*parameter) < max)//�������� �� ��������.
							{
								(*parameter)++;
							}
						//-------------------------
						//��������������� �������������� ���������.
						else
							{
								(*parameter) = min;
							}
						//-------------------------
					}
			break;
			//---------------------------------------------
			default:
				ButtonTemporarySost = 0;
			//---------------------------------------------	
		}	
}
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------

