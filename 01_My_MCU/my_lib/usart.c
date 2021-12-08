//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
/*
 * 
 *
 * Created: 
 *  Author: 
 */ 
#include "my_lib\usart.h"
//*******************************************************************************************
//*******************************************************************************************
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
//*******************************************************************************************
//*******************************************************************************************
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



UsartWorkReg_t UsartWorkReg;
//*******************************************************************************************
//*******************************************************************************************
//������ CRC8.
static uint8_t crc8_calculation(volatile uint8_t *block, uint8_t len){
	
	uint8_t crc = 0xff;
	//-------------------------
	while (len--)
	{
		crc ^= *block;
		block++;
		for(uint8_t i = 0; i < 8; i++)
		{
			crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
		}
	}
	//-------------------------
	return (crc & 0x7F);
}
//*******************************************************************************************
//*******************************************************************************************
void USART_Init(void){
	
	//uint8_t i = 0;
	//-------------------------
	DE_RE_DDR  |=  (DE_RE | RX_LED | TX_LED);	//��������� ����� ~�����/�������� RS-485, ���������� ������ � ���������� ��������.
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
	//offset_control_zone8();
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
			//volatile uint8_t temp = UDR;					 //������� ��������� ��������.
			(void)UDR;										 //������� ��������� ��������
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
	WorkRegStr.UsartFlagReg |= USART_RECIVE_START_FLAG;
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
							//usart_recive_RetStat_RM_D(byte);
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
//*******************************************************************************************
//*******************************************************************************************
//������ �������� ������ �� ����������.
//void Uart1StarTx(uint8_t *TxBuf, uint8_t size){
//
	//U1WorkReg.CounterByte = 0;	  //C���� �������� ����.
	//U1WorkReg.BufSize	  = size; //���������� ���� �� ��������.
	//U1WorkReg.BufPtr	  = TxBuf;//��������� �� ���������� �����.
	////----------------
	//USART1->CR1 &= ~USART_CR1_RE; //��������� RX USART1.
	//USART1->DR   = *TxBuf;        //�������� ������� �����.
	//USART1->CR1 |= USART_CR1_TE  |//��������� TX USART1.
	//USART_CR1_TCIE;//��������� ���������� �� TX USART1.
//}

//������ �������� ������ �� ����������.
void USART_StartTX(uint8_t *txBuf, uint8_t size){

	UsartWorkReg.TxCounterByte = 0;
	UsartWorkReg.TxBufSize     = size;
	UsartWorkReg.TxBufPtr	   = txBuf;
	//----------------
	//����� �������� ������ �� ������.
	//DE_RE_PORT |= DE_RE;		//������� ���������� RS485 �� ��������.
	UDR    = *(txBuf+0); //���������� ������ ����.
	UCSRB |= (1<<TXCIE); //��������� ���������� �� �������� �����.
	//-------------------------
}
//*******************************************************************************************
//*******************************************************************************************
//���������� �� ������ �����.
ISR (USART_RXC_vect){
	
	//������ � ����� ������������������ ���������� ���� �� UDR
	uint8_t UCSRA_temp = UCSRA;
	uint8_t data       = UDR;
	//-------------------------
	//���� ���� �� ����� �� ����������� ���. ���� ����� �� �����c����.
	if((UCSRA_temp & ((1 << FE) | (1 << DOR) | (1 << UPE))) == 0)
	{
		WorkRegStr.ReciveCounterByte++;
		//-------------------------
		//������ � ������� ��� ������ 1.
		//if (RA_ConfigStr.STR.Address == 1) usart_recive_RetStat_RM_D(data);
		//������ � ������� ��� ������� 2 - 64.
		//else usart_recive_for_RA2(data);
		usart_recive_for_RA2(data);
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
//*******************************************************************************************
//*******************************************************************************************
//���������� �� ���������� �������� ������.
ISR (USART_TXC_vect){
	
	//uint8_t  txCounter = UsartWorkReg.TxCounterByte;//������� ���������� ����.
	//uint8_t  txSize    = UsartWorkReg.TxBufSize;    //���������� ������������ ����.
	//uint8_t *TxBufPtr;     //��������� �� ���������� �����.	
	//-------------------------
	//������� ���� ������?
	if(++UsartWorkReg.TxCounterByte >= UsartWorkReg.TxBufSize)
	{
		UCSRB &= ~(1<<TXCIE); //������ ���������� �� ���������� �������� �����.
	}
	//-------------------------
	//�������� ����� �� ������.
	else UDR = *(UsartWorkReg.TxBufPtr + UsartWorkReg.TxCounterByte);


	//if(++WorkRegStr.TransmitCounterByte >= WorkRegStr.TransmitBufSize) 
	//{
	 	//UCSRB &= ~(1<<TXCIE);			   //������ ���������� �� ���������� �������� �����.
	 	//WorkRegStr.TransmitCounterByte = 0;//C���� �������� ����.
		//WorkRegStr.TransmitBufSize     = 0; 
	 	////-------------------------
		////��������� ���������� RS485 �� ����.
		//DE_RE_PORT &= ~DE_RE;
		////-------------------------
		////������ ������ ���� ��������� �����.
		//if (WorkRegStr.UsartFlagReg & USART_RECIVE_START_FLAG)
		//{
			//WorkRegStr.UsartFlagReg &= ~USART_RECIVE_START_FLAG;
			////-------------------------
			////������ ������ ������ �� �������.
			//WorkRegStr.ReciveCounterByte = 0;
			//RxBuffer1[0] = 0;
			//RxBuffer1[1] = 0;
			//UCSRB |= (1<<RXEN | 1<<RXCIE); 
			////-------------------------
		//}
		////-------------------------
	//}
	//-------------------------
	//�������� ����� �� ������.
	//else UDR = TxBuffer[WorkRegStr.TransmitCounterByte];
	//-------------------------
}
//*******************************************************************************************
//*******************************************************************************************




