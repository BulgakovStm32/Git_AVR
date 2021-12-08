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

#include "my_lib\usart.h"

//-----------------------------------------------------------------------------
typedef volatile struct{
			//----------------------------------------
			uint8_t ConsolAddress;
			uint8_t ConsolControl;
			uint8_t ConsolAudioCh;					
			uint8_t Control;			
			//----------------------------------------
}__ConsolData_TypeDef;
//-----------------------------------------------------------------------------
//������� �������� ��� ����������� ������ �������.
typedef volatile struct{
	//----------------------------------------
	uint8_t UsartFlagReg;			//������� ������.
	uint8_t ReciveCounterByte;		//������� �������� ����.
	uint8_t CommandLengt;			//����� ����������� �������.
	uint8_t TransmitCounterByte;	//������� ���� �� ��������.
	uint8_t TransmitBufSize;		//���������� ���� �� ��������.
	uint8_t Offset;
	uint8_t Bit;
	//uint8_t *TransmitBufPointer;	//��������� ��� �������� ������.
	//----------------------------------------		
} __UsartWorkReg_TypeDef;
//-----------------------------------------------------------------------------
//������ �� ���������� ���� ������. ������ ������ ������������� ������ � ����� ������������������.
//const uint8_t CommandLengthArray[] PROGMEM = {	GetStat_Length,				
												//GetStatD_Length,				
												//DriveRA_Length,			
												//StartA_Length,		
												//RetStat_RAx_Length,		
												//RetStat_RMx_Length,		
												//RetStat_PS_Length,		
												//RetStat_RMx_D_Length,	
												//RetStat_PS_D_Length,	
												//RetActivControlBlok_Length
//};
//------------------------------------------------------------------------------
static volatile uint16_t CycleCount			= 0;
static volatile uint8_t  RequestCount		= 0;
static volatile uint8_t  Priority			= 0;

static volatile uint8_t  ConsolAddress		= 0;
static volatile uint8_t  ConsolControlSP	= 0;
static volatile uint8_t  ConsolAudoiChannel = 0;
static volatile uint8_t  AdressPointer		= 0;

//static volatile uint8_t uart_crc = 0xFF;
//static volatile uint8_t address = 2;
//static volatile uint8_t GroupControlChanel = 0;
//static volatile uint8_t SPControlChanel = 0;

	
static volatile uint8_t TxBuffer[10];
static volatile uint8_t RxBuffer1[80];//����� �� �����.

__UsartWorkReg_TypeDef	WorkRegStr;	//������� ��������.		
__Configuration_TypeDef RA_ConfigStr;	

extern __IndicatorFacePanel_TypeDef	IndicatorsFacePanelStr;
//-------------------------
//���������� ��� ������ ����� ����� �� 2 �� 64.
//static volatile uint8_t	crc8_for_Request = 0;
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//������ CRC8.
static uint8_t crc8_calculation(volatile uint8_t *block, uint8_t len){
	
	uint8_t i;
	uint8_t crc = 0xff;
	//-------------------------
	while (len--)
		{
			crc ^= *block;
			block++;
			for (i = 0; i < 8; i++)
				{
					crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
				}
		}
	//-------------------------
	return (crc & 0x7F);
}
//-----------------------------------------------------------------------------
//������ �������� ��� ��������� 8-� ����� �����.
static void offset_control_zone8(void){

	if (RA_ConfigStr.STR.Address <= 7)
		{
			WorkRegStr.Offset = 0;
			WorkRegStr.Bit = (1 << (RA_ConfigStr.STR.Address - 1));
			return;
		}
	else if ( (RA_ConfigStr.STR.Address >= 8) && (RA_ConfigStr.STR.Address <= 14) )
		{
			WorkRegStr.Offset = 1;
			WorkRegStr.Bit = (1 << (RA_ConfigStr.STR.Address - 8));
			return;
		}
	else if ( (RA_ConfigStr.STR.Address >= 15) && (RA_ConfigStr.STR.Address <= 21) )
		{
			WorkRegStr.Offset = 2;
			WorkRegStr.Bit = (1 << (RA_ConfigStr.STR.Address - 15));
			return;
		}
	else if ( (RA_ConfigStr.STR.Address >= 22) && (RA_ConfigStr.STR.Address <= 28) )
		{
			WorkRegStr.Offset = 3;
			WorkRegStr.Bit = (1 << (RA_ConfigStr.STR.Address - 22));
			return;
		}
	else if ( (RA_ConfigStr.STR.Address >= 29) && (RA_ConfigStr.STR.Address <= 35) )
		{
			WorkRegStr.Offset = 4;
			WorkRegStr.Bit = (1 << (RA_ConfigStr.STR.Address - 29));
			return;
		}
	else if ( (RA_ConfigStr.STR.Address >= 36) && (RA_ConfigStr.STR.Address <= 42) )
		{
			WorkRegStr.Offset = 5;
			WorkRegStr.Bit = (1 << (RA_ConfigStr.STR.Address - 36));
			return;
		}
	else if ( (RA_ConfigStr.STR.Address >= 43) && (RA_ConfigStr.STR.Address <= 49) )
		{
			WorkRegStr.Offset = 6;
			WorkRegStr.Bit = (1 << (RA_ConfigStr.STR.Address - 43));
			return;
		}
	else if ( (RA_ConfigStr.STR.Address >= 50) && (RA_ConfigStr.STR.Address <= 56) )
		{
			WorkRegStr.Offset = 7;
			WorkRegStr.Bit = (1 << (RA_ConfigStr.STR.Address - 50));
			return;
		}
	else if ( (RA_ConfigStr.STR.Address >= 57) && (RA_ConfigStr.STR.Address <= 63) )
		{
			WorkRegStr.Offset = 8;
			WorkRegStr.Bit = (1 << (RA_ConfigStr.STR.Address - 57));
			return;
		}
	else if (RA_ConfigStr.STR.Address == 64)
		{
			WorkRegStr.Offset = 9;
			WorkRegStr.Bit = (1 << 0);
		}
}
//-----------------------------------------------------------------------------
void usart_init(void){
	
	//uint8_t i = 0;
	//-------------------------
	DE_RE_DDR   |= (DE_RE | RX_LED | TX_LED);	//��������� ����� ~�����/�������� RS-485, ���������� ������ � ���������� ��������.
	DE_RE_PORT &= ~(DE_RE | RX_LED | TX_LED);	//������ � �����.
	//-------------------------
	UBRRL = UBRR_CONST;							//��������� ��������.
	UBRRH = UBRR_CONST >> 8;					//
	UCSRA = 0;									//
	UCSRB = (1<<TXEN|0<<RXEN|0<<RXCIE|0<<TXCIE);//������� ��������, ���������� �� ������ �����. 
	UCSRC = (1<<URSEL | 3<<UCSZ0);				//8 ��� ������, 1 ����-���. 
}
//-----------------------------------------------------------------------------
//���������� �������� ����� ��� ������ ��������� ������.
void usart_config_for_protocol(__Configuration_TypeDef *config){
	
	//-------------------------
	cli(); //������ ����������.
	//-------------------------
	RA_ConfigStr.STR.Address    = config->STR.Address;
	RA_ConfigStr.STR.Group      = config->STR.Group;
	RA_ConfigStr.STR.RS485Check = config->STR.RS485Check;
	//-------------------------
	//���������� ����������� � ������ RS485.
	if (RA_ConfigStr.STR.RS485Check == 0)
		{
			IndicatorsFacePanelStr.bit.RS485_LED = 0;
		}
	//-------------------------
	ConsolAddress = 0;
	ConsolAudoiChannel = 0;
	ConsolControlSP = 0;
	AdressPointer = 0;
	//-------------------------
	//������ �������� ��� ��������� 8-� ����� �����.
	offset_control_zone8();
	//-------------------------
	//������ ��������� ��� ������ 1.
	if (config->STR.Address == 1)
		{
// 			UCSRB &= ~(1<<TXCIE);			   //������ ���������� �� ���������� �������� �����.
// 			WorkRegStr.TransmitCounterByte = 0;//C���� �������� ����.
			Timer0FlagReg2 |= Timer0_StartPtotocolForRA1Flag;//������ ��������� ��� ������ 1.
		}
	//-------------------------
	//������ ��������� ��� ������ 2 � �����.
	else
		{
			UCSRB &= ~(1<<TXCIE);							 //������ ���������� �� ���������� �������� �����.
			WorkRegStr.TransmitCounterByte = 0;				 //C���� �������� ����.
			DE_RE_PORT &= ~DE_RE;							 //������� ���������� RS485 �� ����.
			volatile uint8_t temp = UDR;					 //������� ��������� ��������.
			WorkRegStr.ReciveCounterByte = 0;				 //���������� ��������� ��� ������ ���������
			RxBuffer1[0] = 0;								 //
			RxBuffer1[1] = 0;								 //
			UCSRB |= (1<<RXEN | 1<<RXCIE);					 //������ ������ ������ �� �������.
			Timer0FlagReg2 |= Timer0_StartPtotocolForRA2Flag;//������ ��������� ��� ������ 2 � �����.
			//-------------------------
//			//������ ������ �� ��������� �������� �������.
//			WorkRegStr.UsartFlagReg |= UsartStartReciveFlag;
//			//������ ������ ������	
//			Timer0FlagReg2 |= Timer0_StartPtotocolForRA2Flag;	
// 			WorkRegStr.ReciveCounterByte = 0;
// 			RxBuffer1[0] = 0;
// 			RxBuffer1[1] = 0;
// 			DE_RE_PORT &= ~DE_RE;		  //������� ���������� RS485 �� ����.
// 			UCSRB |= (1<<RXEN | 1<<RXCIE);//������ ������ ������ �� �������.
			//-------------------------
		}
	//-------------------------
	sei(); //���������� ����������.
	//-------------------------
}
//-----------------------------------------------------------------------------
//������ ���������� ���������� ��� ������ ����� ���������.
void priority_for_protocol(uint8_t priority){
	
	//cli(); //������ ����������.
	//-------------------------
	switch (priority)
		{	
			//-------------------------
			case (1):
				Priority = 5;
			break;
			//-------------------------
			case (2):
				Priority = 4;
			break;
			//-------------------------
			case (3):
				Priority = 3;		
			break;
			//-------------------------
			case (4):
				Priority = 2;		
			break;
			//-------------------------
			case (5):
				Priority = 1;		
			break;
			//-------------------------			
		}
	//-------------------------
	//sei(); //���������� ����������.
}
//-----------------------------------------------------------------------------
//�������� ������ ����������� �������.
uint8_t get_consol_number(void){
	
	return(ConsolAddress);
	//-------------------------
	//cli(); //������ ����������.
	//uint8_t temp = ConsolAddress;
	//sei(); //���������� ����������.
	//-------------------------
	//return(temp);
}
//-----------------------------------------------------------------------------
//��������� ������ ����� ������ ����������� �������.
uint8_t get_consol_audio_chanel(void){
	
	return(ConsolAudoiChannel);
	//-------------------------
	//cli(); //������ ����������.
	//uint8_t temp = ConsolAudoiChannel;
	//sei(); //���������� ����������.
	//-------------------------
	//return(temp);
}
//-----------------------------------------------------------------------------
//���������� �������� SP1 - SP8 ��������� ���.
uint8_t get_consol_control(void){
	
	return(ConsolControlSP);
	//-------------------------
	//cli(); //������ ����������.
	//uint8_t temp = ConsolControlSP;
	//sei(); //���������� ����������.
	//-------------------------
	//return(temp);
}
//-----------------------------------------------------------------------------
//������ ������ ���������� �� �������� RM. 	
static void parsing_control_command_from_consol(void){

	//---------------------------------------------------------	
	//���������� ��������.
	if (RxBuffer1[2] == 0)
		{
	 		//-------------------------
	 		//���� ������ ������������� ������������� �� �������.
            //���� ������ ������������� ������������� �� �������.
	 		if ( (((RxBuffer1[4] & 0x7F) & (1 << (RA_ConfigStr.STR.Group - 1))) != 0) || //�������� ����� � 1 �� 7.
				 ( (RA_ConfigStr.STR.Group == 8) && (RxBuffer1[5] != 0)) )				 //�������� ������ 8.
	 			{
	 				//-------------------------
	 				if (RxBuffer1[1] & BroadCast)
	 					{
	 						if ((RxBuffer1[1] & 0x0F) >= AdressPointer)
	 							{
									//-------------------------
 	 								AdressPointer = (RxBuffer1[1] & 0x0F);			  //���������� ������ ������������ ���������� � ��������� �����������.
	 								//-------------------------
	 								RequestCount = 0;
									//-------------------------
									ConsolAddress = AdressPointer;					  //����������� ������ ����������� ������� �� �������.
	 								ConsolAudoiChannel = (0x03 & (RxBuffer1[3] >> 4));//����� ����� ������.
	 								ConsolControlSP = 0xFF;							  //��������� ���� ���.
									//-------------------------
	 							}
	 					}
					else
						{
 							if ((RxBuffer1[1] & 0x0F) == ConsolAddress)
 								{
 									AdressPointer = 0;
// 									ConsolAddress = 0;//��� ����� ��� ������� �������� ���������� 5-�� ���������� ���� �� ������� ��������� ������.
 									//ConsolAudoiChannel = 0;
 								}
						}
	 				//-------------------------
	 			}
	 		//-------------------------
	 		//���� ������ �� �������������.
	 		else
	 			{
	 				if ((RxBuffer1[1] & 0x0F) == ConsolAddress)
	 					{
							AdressPointer = 0;
//							ConsolAddress = 0;//��� ����� ��� ������� �������� ���������� 5-�� ���������� ���� �� ������� ��������� ������.
							//ConsolAudoiChannel = 0;
	 					}
	 			}
	 		//-------------------------
		}
	//---------------------------------------------------------
	//���������� ������.
	else
		{
			//-------------------------
			//���� ���� ��� ��������.
			//������ WorkRegStr.Offset � WorkRegStr.Bit ������������ � �-� static void offset_control_zone8(void).
			if ( ((RxBuffer1[(3 + RA_ConfigStr.STR.Address)] & 0x7F) != 0) ||
			     ((RxBuffer1[(68 + WorkRegStr.Offset)] & WorkRegStr.Bit) != 0) )
				{
					//-------------------------
					if (RxBuffer1[1] & BroadCast)
						{
							if ( (RxBuffer1[1] & 0x0F) >= AdressPointer )
								{
									//-------------------------
 									AdressPointer = (RxBuffer1[1] & 0x0F);
									//-------------------------
									RequestCount = 0;
									//-------------------------
									ConsolAddress = AdressPointer;					  //����������� ������ ����������� ������� �� �������.
									ConsolAudoiChannel = (0x03 & (RxBuffer1[3] >> 4));//����� ����� ������.
									//-------------------------
									//���������� �������� SP1 - SP7.
									ConsolControlSP = ( RxBuffer1[(3 + RA_ConfigStr.STR.Address)] & 0x7F);
									//���������� ������� SP8.
									if ((RxBuffer1[(68 + WorkRegStr.Offset)] & WorkRegStr.Bit) != 0)
										{
											ConsolControlSP |= (1<<7);
										}
									//-------------------------
								}
						}
					else
						{
							if ((RxBuffer1[1] & 0x0F) == ConsolAddress)
								{
									AdressPointer = 0;
//									ConsolAddress = 0;//��� ����� ��� ������� �������� ���������� 5-�� ���������� ���� �� ������� ��������� ��� ������.
									//ConsolAudoiChannel = 0;
								}
						} 
					//-------------------------	
				}
			//-------------------------
			//������ ��������.
			else
				{
			 		if ((RxBuffer1[1] & 0x0F) == ConsolAddress)
			 			{
							AdressPointer = 0;
//							ConsolAddress = 0;//��� ����� ��� ������� �������� ���������� 5-�� ���������� ���� �� ������� ��������� ��� ������.
							//ConsolAudoiChannel = 0;
			 			}
				}
			//-------------------------
		}
	//---------------------------------------------------------
}
//-----------------------------------------------------------------------------
void usart_cycle_count_modify(void){
	
	//���� �� ���� ������ � ������� CycleQuantityForRA2, �� ������ ��� �������� � ���������� ���������� 5 ����������.
	if (++CycleCount >= CycleQuantityForRA2)
		{
			//-------------------------
			//���������� ��� �������� ��� �����.
			CycleCount = 0;
			ConsolAddress = 0;
			AdressPointer = 0;
			ConsolControlSP = 0;
			//-------------------------
			WorkRegStr.ReciveCounterByte = 0;
			RxBuffer1[0] = 0;
			RxBuffer1[1] = 0;
			UCSRB |= (1<<RXEN | 1<<RXCIE);
			//-------------------------
			//��������� ���������� ������������� ���� ��� �������� � ������������ �����.
			if (RA_ConfigStr.STR.RS485Check)
				{
					IndicatorsFacePanelStr.bit.RS485_LED = 1;
				}
			else
				{
					IndicatorsFacePanelStr.bit.RS485_LED = 0;
				}
			//-------------------------
		}
    //-------------------------
}
//-----------------------------------------------------------------------------
//������� ���������, ������������ ������� ������ ��������� � ����� ����� ���� ����� ����� 01.
void rs485_protocol(void){
	
	static volatile uint8_t step = 0;
	static volatile uint8_t address = 0;
	volatile uint8_t	temp = 0;
	//------------------------------------------------------------------
	//������� ���������, ������������ ������� ������ ��������� � ����� ����� ����� 1.	
	switch(step)
		{
			//---------------------------------------------------
			//������ ������� �� ����� RA1.
			case (0):
				//-------------------------
				TxBuffer[0] = RetStat_RA;//��� �������.
				TxBuffer[1] = 0x01;		 //����� �����.
				//-------------------------
				//������ 2-�� �����.
				temp  = (0x0F & RA_ConfigStr.STR.Group);//����� ������ �����.
				temp |= (0x20 & ((get_lines_to_control() & (get_alarms_lines() | ~get_blocked_speaker_line())) >> 2));//������ ������������� ������ LC.
				temp |=	(0x10 & (get_relay_state(SPEAKER_RELAY) >> 3));//������ ������ SP8.
				TxBuffer[2] = temp;
				//-------------------------
				//������ 3-�� �����.
				temp = (0x0f & (ConsolAddress - 64));//������ 2-�� ������������� ����������� - ��� ����� �������, ��� ����������� ������� ��������� �������.
				if (get_charge_indication())
					{
						temp |= (1<<4);//ErroACC. ������������� �������.
					}
				TxBuffer[3] = temp;
				//-------------------------
				TxBuffer[4] = (0x0F & Priority);					  //������ ���������� ����������.
				TxBuffer[5] = (0x7F & get_relay_state(SPEAKER_RELAY));//������ ������� SP1 - 7.
				TxBuffer[6] = (0x7F & (get_lines_to_control() & (get_alarms_lines() | ~get_blocked_speaker_line()) ));//������ ������������� ������� LC1 - 7.
				TxBuffer[7] = 0;
				TxBuffer[8] = 0;
				TxBuffer[9] = crc8_calculation(&TxBuffer[0], 9);//CRC			
				//-------------------------
				//���������� ���� �� ��������.
				WorkRegStr.TransmitBufSize = RetStat_RA_Length;
				//-------------------------
				//��������� �������� ��� ������ �����.
				TX_LED_PORT |= TX_LED;
				//-------------------------
				//������ ������ � ������ 2.
				address = 2;
				//-------------------------
				//������� �� ��������� ���.
				step = 1;
				//-------------------------
			break;
			//---------------------------------------------------
			//�������� �������� GetStat. ����� 73 �������.
			case (1):
				//-------------------------
				TxBuffer[0] = GetStat;		   //��� �������.
				TxBuffer[1] = (0x7F & address);//����� �����.
				TxBuffer[2] = crc8_calculation(&TxBuffer[0], 2);//CRC
				//-------------------------
				//���������� ���� �� ��������.
				WorkRegStr.TransmitBufSize = GetStat_Length;
				//-------------------------
				//��������� ����� �������.
				address++;
				//-------------------------
				step = 1;
				
				//���� ������� ��� ����� �� ��������� �� ���� ���.
				if (address == 74)
					{
						//-------------------------
						//������� �� ��������� ���.
						step = 2;
						//-------------------------
					}
				//-------------------------
			break;
			//---------------------------------------------------
			//������ �������. ������� StartA.		
			case (2):
				TxBuffer[0] = StartA;//��� �������.
				TxBuffer[1] = 0x12;	 //CRC
				//-------------------------
				//���������� ���� �� ��������.
				WorkRegStr.TransmitBufSize = StartA_Length;
				//-------------------------
				//����� ������� � ������� ����� ������ ������. 
				address = Consol_9;	//������ � ������� �9.
				AdressPointer = 0;	//
				//-------------------------
				//������� �� ��������� ���.
				step = 3;
				//-------------------------
			break;
			//---------------------------------------------------
			//������ ������� ���������� �� �������.
			case (3):
				TxBuffer[0] = GetStatD;			//��� �������.
				TxBuffer[1] = (0x7F & address);	//����� �������.
				TxBuffer[2] = crc8_calculation(&TxBuffer[0], 2);//CRC
				//-------------------------
				//���������� ���� �� ��������.
				WorkRegStr.TransmitBufSize = GetStatD_Length;
				//-------------------------
				//������ ����� ������ �� �������.
				WorkRegStr.UsartFlagReg |= UsartStartReciveFlag;
				//-------------------------
				//��������� ������ ��������.
				address--;
				//-------------------------				
				//���� �������� ��� 9 �������� �� ���� ������.
				if (address == (Consol_1 - 1))
					{
						//-------------------------
						//������� �� ��������� ���.
						step = 4;
						//-------------------------
					}
				//-------------------------
			break;
			//---------------------------------------------------
			//��������� ���������� ��� RA2 - RA64 �� ����� RA1.
			case (4):
				//-------------------------
				//���������� ������ ������ �� �������.
				UCSRB &= ~(1<<RXEN | 1<<RXCIE);//���������� ���������.
				//-------------------------
				//�������� ������� DriveRA.
				TxBuffer[0] = DriveRA;	    //��� �������.
				TxBuffer[1] = (0x85 & 0x7F);//CRC
				//-------------------------
				//���������� ���� �� ��������.
				WorkRegStr.TransmitBufSize = DriveRA_Length;
				//-------------------------
				//���� �� ���� ������ � ������� 6 ������ �� ������ ��� ��������(��������, ������� ������� ��� ������� �����) � ���������� ���������� 5 ����������.
				//�� ����� 6 ������ ����� ��� �� �� ���� ������ ������������ ���� ����� ����� ��������� ������ � �����.
				if ( ++CycleCount >= 4 )
					{
						CycleCount = 0;
						ConsolAddress = 0;
						AdressPointer = 0;
						//-------------------------
						//��������� ���������� �������������.
						if (RA_ConfigStr.STR.RS485Check)
							{
								IndicatorsFacePanelStr.bit.RS485_LED = 1;
							}
						else
							{
								IndicatorsFacePanelStr.bit.RS485_LED = 0;
							}
						//-------------------------
					}
				//------------------------- 
				//���� ��������� ��������� ������� BroadCast (��������, ��������� ��� ���� �� �������, ������� ������� �� ������� ��� ������� �����)
				//�� ����� 8 �������� ���������� ���������� ������� ����������.
				if ( ++RequestCount >= 4 )
					{
						RequestCount = 0;
						ConsolAddress = 0;
						AdressPointer = 0;
					}
				//-------------------------
				//������� ��������� � ��������� ���������.
				//address = 2;
				step = 0;				
				//-------------------------
			break;
			//---------------------------------------------------				
		}
	//------------------------------------------------------------------
	DE_RE_PORT |= DE_RE;	//������� ���������� RS485 �� ��������.
	UDR = TxBuffer[0];		//���������� ������ ����.
	UCSRB |= (1<<TXCIE);
	//------------------------------------------------------------------
}
//-----------------------------------------------------------------------------
static void usart_recive_RetStat_RM_D(volatile uint8_t byte){

	//-------------------------
	//���������� ��������� �����.
	RxBuffer1[WorkRegStr.ReciveCounterByte - 1] = byte;
	//-------------------------
	//������� ������ ���������� ����.
	if(WorkRegStr.ReciveCounterByte >= RetStat_RM_D_Length)
		{
	 		//-------------------------
	 		//���������� ������ ������ �� �������.
	 		//UCSRB &= ~(1<<RXEN | 1<<RXCIE);//�� ����� ��������� �����. ���������.
	 		//-------------------------
	 		//CR� �������?
	 		if ( (crc8_calculation(&RxBuffer1[0], (RetStat_RM_D_Length - 1))) == RxBuffer1[RetStat_RM_D_Length - 1] )
	 			{
	 				//-------------------------
	 				//��������� ��������� ������.
	 				RX_LED_PORT |= RX_LED;
	 				//-------------------------
					if (RA_ConfigStr.STR.Address == 1)
						{
							//-------------------------
							//UCSRB &= ~(1<<RXEN | 1<<RXCIE);//�� ����� ��������� �����. ���������.
							CycleCount = 0;
							IndicatorsFacePanelStr.bit.RS485_LED = 0;
							//-------------------------
						}
	 				//-------------------------
	 				//������ ����������.
	 				parsing_control_command_from_consol();
	 				//-------------------------
	 			}
			//-------------------------
			//���������� � ������ ��������.
			WorkRegStr.ReciveCounterByte = 0;
			RxBuffer1[0] = 0;
			RxBuffer1[1] = 0;
			//UCSRB |= (1<<RXEN | 1<<RXCIE);//�� ����� ��������� �����. ���������.
			//-------------------------
		}
	//-------------------------
}
//-----------------------------------------------------------------------------
void send_request_from_RA2(void){
	
	uint8_t temp = 0;
	//-------------------------
	//������������ ������ �� ������.
	TxBuffer[0] = RetStat_RA;						//��� �������.
	TxBuffer[1] = (0x7F & RA_ConfigStr.STR.Address);//����� �����.
	//-------------------------
	//������ 2-�� �����.
	temp  = (0x0F & RA_ConfigStr.STR.Group);//����� ������ �����.
	temp |= (0x20 & ((get_lines_to_control() & (get_alarms_lines() | ~get_blocked_speaker_line())) >> 2));//������ ������������� ������ LC.
	temp |=	(0x10 & (get_relay_state(SPEAKER_RELAY) >> 3));//������ ������ SP8.
	TxBuffer[2] = temp;
	//-------------------------
	//������ 3-�� �����.
	temp = (0x0f & (ConsolAddress - 64));//������ 2-�� ������������� ����������� - ��� ����� �������, ��� ����������� ������� ��������� �������.
	if (get_charge_indication())
		{
			temp |= (1<<4);//ErroACC. ������������� �������.
		}
	TxBuffer[3] = temp;
	//-------------------------
	TxBuffer[4] = (0x0F & Priority);					  //������ ���������� ����������.
	TxBuffer[5] = (0x7F & get_relay_state(SPEAKER_RELAY));//������ ������� SP1 - 7.
	TxBuffer[6] = (0x7F & (get_lines_to_control() & (get_alarms_lines() | ~get_blocked_speaker_line()) ));//������ ������������� ������� LC1 - 7.
	TxBuffer[7] = 0;
	TxBuffer[8] = 0;
	TxBuffer[9] = crc8_calculation(&TxBuffer[0], 9);//CRC
	//-------------------------
	//������ ������ �� ��������� �������� �������.
	WorkRegStr.UsartFlagReg |= UsartStartReciveFlag;
	//-------------------------
	//��������� ��������
	TX_LED_PORT |= TX_LED;
	//-------------------------
	//����� �������� ������ �� ������.
	WorkRegStr.TransmitBufSize = RetStat_RA_Length;//���������� ���� �� ��������.
	DE_RE_PORT |= DE_RE;						   //������� ���������� RS485 �� ��������.
	UDR = TxBuffer[0];							   //���������� ������ ����.
	UCSRB |= (1<<TXCIE);						   //��������� ���������� �� �������� �����.
	//-------------------------
}
//-----------------------------------------------------------------------------
//��������� �������� ���� ����� ���� ����� ����� 2 � �����.
static void usart_recive_for_RA2(volatile uint8_t byte){
	
	//-------------------------
	if ( (RxBuffer1[0] == GetStat) || (RxBuffer1[0] == RetStat_RM_D) )//���� ����� ��� ������ ������ ���� �������
		{
			//-------------------------
			//������ ������� �����.
			if (RxBuffer1[0] == GetStat)
				{
					//-------------------------
					if (RxBuffer1[1] == RA_ConfigStr.STR.Address)//���� ����� ��� ������ ���� ������.
						{
							//-------------------------
							//�������� CRC ��������� �������.
							if (byte == crc8_calculation(&RxBuffer1[0], 2))
								{
									//-------------------------
									WorkRegStr.ReciveCounterByte = 0;
									RxBuffer1[0] = 0;
									RxBuffer1[1] = 0;
									//-------------------------
									//��������� ��������� ������.
	 								//RX_LED_PORT |= RX_LED;
									//-------------------------
									//���������� ������ �������� �� RA1.//�� ����� ��������� �����. ���������.??????
									//??????????????????????????????????????????????????????????????????????????????
									//UCSRB &= ~(1<<RXEN | 1<<RXCIE);///////////////////////
									//-------------------------
									//���� ��������� ��������� �������� ����� �������� ����� ���������� ���� � ���� (�������� ������� ������� ��� ������� �����)
									//�� ����� RequestQuantity �������� ���������� ���������� ������� ����������.
 									if ( ++RequestCount >= RequestQuantity )
 										{
 									 		RequestCount = 0;
 									 		ConsolAddress = 0;//����� ����� ���������� ���� ������� �� ������.
											//ConsolAudoiChannel = 0;//////////////////
 									 		AdressPointer = 0;
 										}
									//-------------------------
									//AdressPointer = 0;//��� �� ����� ���������� ���� �������.
									CycleCount = 0;
									IndicatorsFacePanelStr.bit.RS485_LED = 0;
									//-------------------------
									//������ �������� ������ �� ������.
									//send_request_from_RA2();
									Timer0FlagReg2 |= Timer0_StartRequestFlag;
									//-------------------------
								}
							//-------------------------
							else
								{
									WorkRegStr.ReciveCounterByte = 0;
									RxBuffer1[0] = 0;
									RxBuffer1[1] = 0;
								}
							//-------------------------
						}
					//-------------------------
					//������ ������ �� ����.
					else
						{
							if (byte == RA_ConfigStr.STR.Address)//�������� ����� ���� ���������.
								{
									RxBuffer1[1] = byte;
								}
							else
								{
									WorkRegStr.ReciveCounterByte = 0;
									RxBuffer1[0] = 0;
									RxBuffer1[1] = 1;
								}
						}
					//-------------------------
				}
			//-------------------------
			else
				{
					//-------------------------
					//����� ����������� ������� �� �������.
					if (RxBuffer1[0] == RetStat_RM_D)
						{
							usart_recive_RetStat_RM_D(byte);
						}
					//-------------------------
					else
						{
							WorkRegStr.ReciveCounterByte = 0;
							RxBuffer1[0] = 0;
						}
					//-------------------------
				}
			//-------------------------
		} 
	//-------------------------
	//����� �� ���� ������� ������� ����� �������.
	else
		{
			//-------------------------
			//���������� ������� ����� �������.
			if ( (byte == GetStat) || (byte == RetStat_RM_D) )
				{
					RxBuffer1[0] = byte;
				} 
			//-------------------------
			//����� �������� �������� ����.
			else
				{
					WorkRegStr.ReciveCounterByte = 0;
				}
			//-------------------------
		}
	//-------------------------
}
//-----------------------------------------------------------------------------
//���������� �� ������ �����.
ISR (USART_RXC_vect){
	
	uint8_t data = 0;
	uint8_t UCSRA_temp = 0;
	//-------------------------
	UCSRA_temp = UCSRA;
	data = UDR;
	//-------------------------
	//���� ���� �� ����� �� ����������� ���. ���� ����� �� �����c����.
	if ((UCSRA_temp & ((1 << FE) | (1 << DOR) | (1 << UPE))) == 0)
	{
		WorkRegStr.ReciveCounterByte++;
		//-------------------------
		//������ � ������� ��� ������ 1.
		if (RA_ConfigStr.STR.Address == 1)
			{
				usart_recive_RetStat_RM_D(data);
			}
		//-------------------------
		//������ � ������� ��� ������� 2 - 64.
		else
			{
				usart_recive_for_RA2(data);
			}
		//-------------------------
	}
	//-------------------------
	//���� ���� �����.
	//     else
	// 		{
	// 			WorkRegStr.ReciveCounterByte = 0;
	// 			RxBuffer1[0] = 0;
	// 			RxBuffer1[1] = 0;
	// 		}
	//-------------------------	
}
//-----------------------------------------------------------------------------
// void usart_start_recive(void){
// 	//������ ������ ���� ��������� �����.
// 	if (WorkRegStr.UsartFlagReg & UsartStartReciveFlag)
// 		{
// 	 		WorkRegStr.UsartFlagReg &= ~UsartStartReciveFlag;
// 	 		//-------------------------
// 	 		//������ ������ ������ �� �������.
// 	 		WorkRegStr.ReciveCounterByte = 0;
// 	 		RxBuffer1[0] = 0;
// 	 		RxBuffer1[1] = 0;
// 	 		UCSRB |= (1<<RXEN | 1<<RXCIE);
// 	 		//-------------------------
// 		}
// }
//-----------------------------------------------------------------------------
//���������� �� ���������� �������� ������.
ISR (USART_TXC_vect){
	
	//-------------------------
	//������� ���� ������?
	if(++WorkRegStr.TransmitCounterByte >= WorkRegStr.TransmitBufSize) 
		{
	 		UCSRB &= ~(1<<TXCIE);			   //������ ���������� �� ���������� �������� �����.
	 		WorkRegStr.TransmitCounterByte = 0;//C���� �������� ����.
			WorkRegStr.TransmitBufSize = 0; 
	 		//-------------------------
			//��������� ���������� RS485 �� ����.
			DE_RE_PORT &= ~DE_RE;
			//-------------------------
			//������ ������ ���� ��������� �����.
			if (WorkRegStr.UsartFlagReg & UsartStartReciveFlag)
				{
					WorkRegStr.UsartFlagReg &= ~UsartStartReciveFlag;
					//-------------------------
					//������ ������ ������ �� �������.
					WorkRegStr.ReciveCounterByte = 0;
					RxBuffer1[0] = 0;
					RxBuffer1[1] = 0;
					UCSRB |= (1<<RXEN | 1<<RXCIE); 
					//-------------------------
				}
			//-------------------------
		}
	//-------------------------
	else
		{
			//�������� ����� �� ������.
	 		UDR = TxBuffer[WorkRegStr.TransmitCounterByte];
		}
	//-------------------------
}
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------

