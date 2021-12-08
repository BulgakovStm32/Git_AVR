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
//Рабочие регистры для организации обмена данными.
typedef volatile struct{
	//----------------------------------------
	uint8_t UsartFlagReg;			//Регистр флагов.
	uint8_t ReciveCounterByte;		//Счетчик принятых байт.
	uint8_t CommandLengt;			//Длина принимаемой команды.
	uint8_t TransmitCounterByte;	//Счетчик байт на передачу.
	uint8_t TransmitBufSize;		//Количество байт на передачу.
	uint8_t Offset;
	uint8_t Bit;
	//uint8_t *TransmitBufPointer;	//Указатель при передаче данных.
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
static volatile uint8_t RxBuffer1[80];//Буфер на прием.

__UsartWorkReg_TypeDef	WorkRegStr;	//Рабочие регистры.		
__Configuration_TypeDef RA_ConfigStr;	

extern __IndicatorFacePanel_TypeDef	IndicatorsFacePanelStr;



UsartWorkReg_t UsartWorkReg;
//*******************************************************************************************
//*******************************************************************************************
//Расчет CRC8.
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
	DE_RE_DDR  |=  (DE_RE | RX_LED | TX_LED);	//Настройка пинов ~ПРИЕМ/ПЕРЕДАЧА RS-485, светодиода приема и светодиода передачи.
	DE_RE_PORT &= ~(DE_RE | RX_LED | TX_LED);	//Выводы к ЗЕМЛЕ.
	//-------------------------
	UBRRL = UBRR_CONST;							//Установка скорости.
	UBRRH = UBRR_CONST >> 8;					//
	UCSRA = 0;									//
	UCSRB = (1<<TXEN|0<<RXEN|0<<RXCIE|0<<TXCIE);//Влючаем передачу, прерывания по приему байта. 
	UCSRC = (1<<URSEL | 3<<UCSZ0);				//8 бит данных, 1 стоп-бит. 
}
//-----------------------------------------------------------------------------
//Применение настроек блока для работы протокола обмена.
void usart_config_for_protocol(__Configuration_TypeDef *config){
	
	//-------------------------
	cli(); //Запрет прерываний.
	//-------------------------
	RA_ConfigStr.STR.Address    = config->STR.Address;
	RA_ConfigStr.STR.Group      = config->STR.Group;
	RA_ConfigStr.STR.RS485Check = config->STR.RS485Check;
	//-------------------------
	//Отключение мониторинга и тревог RS485.
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
	//Расчет смещения для упраления 8-й зоной блока.
	//offset_control_zone8();
	//-------------------------
	//Запуск протокола для адреса 1.
	if (config->STR.Address == 1)
		{
// 			UCSRB &= ~(1<<TXCIE);			   //Запрет прерывание по завершению передачи байта.
// 			WorkRegStr.TransmitCounterByte = 0;//Cброс счетчика байт.
			Timer0FlagReg2 |= Timer0_StartPtotocolForRA1Flag;//Запуск протокола для адреса 1.
		}
	//-------------------------
	//Запуск протокола для адреса 2 и более.
	else
		{
			UCSRB &= ~(1<<TXCIE);							 //Запрет прерывание по завершению передачи байта.
			WorkRegStr.TransmitCounterByte = 0;				 //Cброс счетчика байт.
			DE_RE_PORT &= ~DE_RE;							 //перевод микросхемы RS485 на приём.
			//volatile uint8_t temp = UDR;					 //Очистка приемного регистра.
			(void)UDR;										 //Очистка приемного регистра
			WorkRegStr.ReciveCounterByte = 0;				 //Подготовка регистров для работы протокола
			RxBuffer1[0] = 0;								 //
			RxBuffer1[1] = 0;								 //
			UCSRB |= (1<<RXEN | 1<<RXCIE);					 //Запуск приема ответа от консоли.
			Timer0FlagReg2 |= Timer0_StartPtotocolForRA2Flag;//Запуск протокола для адреса 2 и более.
			//-------------------------
//			//Запуск приема по оканчанию передачи статуса.
//			WorkRegStr.UsartFlagReg |= UsartStartReciveFlag;
//			//Запуск приема ответа	
//			Timer0FlagReg2 |= Timer0_StartPtotocolForRA2Flag;	
// 			WorkRegStr.ReciveCounterByte = 0;
// 			RxBuffer1[0] = 0;
// 			RxBuffer1[1] = 0;
// 			DE_RE_PORT &= ~DE_RE;		  //перевод микросхемы RS485 на приём.
// 			UCSRB |= (1<<RXEN | 1<<RXCIE);//Запуск приема ответа от консоли.
			//-------------------------
		}
	//-------------------------
	sei(); //Разрешение прерываний.
	//-------------------------
}
//-----------------------------------------------------------------------------
void send_request_from_RA2(void){
	
	uint8_t temp = 0;
	//-------------------------
	//Формирование ответа на запрос.
	TxBuffer[0] = RetStat_RA;						//Код команды.
	TxBuffer[1] = (0x7F & RA_ConfigStr.STR.Address);//адрес блока.
	//-------------------------
	//Сборка 2-го байта.
	temp  = (0x0F & RA_ConfigStr.STR.Group);//номер группы блока.
	temp |= (0x20 & ((get_lines_to_control() & (get_alarms_lines() | ~get_blocked_speaker_line())) >> 2));//статус неисправности налана LC.
	temp |=	(0x10 & (get_relay_state(SPEAKER_RELAY) >> 3));//статус канала SP8.
	TxBuffer[2] = temp;
	//-------------------------
	//Сборка 3-го байта.
	temp = (0x0f & (ConsolAddress - 64));//статус 2-го подприоритета управлениея - это адрес консоли, под управлением которой находится система.
	if (get_charge_indication())
		{
			temp |= (1<<4);//ErroACC. Неисправность питания.
		}
	TxBuffer[3] = temp;
	//-------------------------
	TxBuffer[4] = (0x0F & Priority);					  //Статус приоритета управления.
	TxBuffer[5] = (0x7F & get_relay_state(SPEAKER_RELAY));//Статус каналов SP1 - 7.
	TxBuffer[6] = (0x7F & (get_lines_to_control() & (get_alarms_lines() | ~get_blocked_speaker_line()) ));//статус неисправности каналов LC1 - 7.
	TxBuffer[7] = 0;
	TxBuffer[8] = 0;
	TxBuffer[9] = crc8_calculation(&TxBuffer[0], 9);//CRC
	//-------------------------
	//Запуск приема по оканчанию передачи статуса.
	WorkRegStr.UsartFlagReg |= USART_RECIVE_START_FLAG;
	//-------------------------
	//Индикация передачи
	TX_LED_PORT |= TX_LED;
	//-------------------------
	//Запус передачи ответа на запрос.
	WorkRegStr.TransmitBufSize = RetStat_RA_Length;//Количество байт на передачу.
	DE_RE_PORT |= DE_RE;						   //перевод микросхемы RS485 на передачу.
	UDR = TxBuffer[0];							   //Отправляем первый байт.
	UCSRB |= (1<<TXCIE);						   //Включение прерывания по передаче байта.
	//-------------------------
}
//-----------------------------------------------------------------------------
//Обработка принятых байт когда блок имеет адрес 2 и более.
static void usart_recive_for_RA2(volatile uint8_t byte){
	
	//-------------------------
	if ( (RxBuffer1[0] == GetStat) || (RxBuffer1[0] == RetStat_RM_D) )//Если ранее был принят первый байт запроса
		{
			//-------------------------
			//Запрос статуса блока.
			if (RxBuffer1[0] == GetStat)
				{
					//-------------------------
					if (RxBuffer1[1] == RA_ConfigStr.STR.Address)//Если ранее был принят байт адреса.
						{
							//-------------------------
							//Проверка CRC принятого запроса.
							if (byte == crc8_calculation(&RxBuffer1[0], 2))
								{
									//-------------------------
									WorkRegStr.ReciveCounterByte = 0;
									RxBuffer1[0] = 0;
									RxBuffer1[1] = 0;
									//-------------------------
									//Индикация принятого пакета.
	 								//RX_LED_PORT |= RX_LED;
									//-------------------------
									//Отключение приема запросов от RA1.//Не нужно отключать прием. ПРОВЕРЕНО.??????
									//??????????????????????????????????????????????????????????????????????????????
									//UCSRB &= ~(1<<RXEN | 1<<RXCIE);///////////////////////
									//-------------------------
									//Если наивысший приоритет перестал вдруг отвечать когда управление было у него (например пропало питаниа или пропала связь)
									//то через RequestQuantity запросов управление передастся другому приоритету.
 									if ( ++RequestCount >= RequestQuantity )
 										{
 									 		RequestCount = 0;
 									 		ConsolAddress = 0;//Здесь нужно сбрасывать этот регистр по любому.
											//ConsolAudoiChannel = 0;//////////////////
 									 		AdressPointer = 0;
 										}
									//-------------------------
									//AdressPointer = 0;//Тут не нужно сбрасывать этот регистр.
									CycleCount = 0;
									IndicatorsFacePanelStr.bit.RS485_LED = 0;
									//-------------------------
									//Запуск передачи ответа на запрос.
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
					//Адреса раньше не было.
					else
						{
							if (byte == RA_ConfigStr.STR.Address)//Сохраним адрес если совпадает.
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
					//Прием управляющей команды от консоли.
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
	//Ранее не было принято первого байта запроса.
	else
		{
			//-------------------------
			//Сохранение первого байта запроса.
			if ( (byte == GetStat) || (byte == RetStat_RM_D) )
				{
					RxBuffer1[0] = byte;
				} 
			//-------------------------
			//сброс счетчика принятых байт.
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
//Запуск передачи буфера по прерыванию.
//void Uart1StarTx(uint8_t *TxBuf, uint8_t size){
//
	//U1WorkReg.CounterByte = 0;	  //Cброс счетчика байт.
	//U1WorkReg.BufSize	  = size; //Количество байт на передачу.
	//U1WorkReg.BufPtr	  = TxBuf;//Указатель на передающий буфер.
	////----------------
	//USART1->CR1 &= ~USART_CR1_RE; //Отлючение RX USART1.
	//USART1->DR   = *TxBuf;        //Передача первого байта.
	//USART1->CR1 |= USART_CR1_TE  |//Включение TX USART1.
	//USART_CR1_TCIE;//Включение прерывания от TX USART1.
//}

//Запуск передачи буфера по прерыванию.
void USART_StartTX(uint8_t *txBuf, uint8_t size){

	UsartWorkReg.TxCounterByte = 0;
	UsartWorkReg.TxBufSize     = size;
	UsartWorkReg.TxBufPtr	   = txBuf;
	//----------------
	//Запус передачи ответа на запрос.
	//DE_RE_PORT |= DE_RE;		//перевод микросхемы RS485 на передачу.
	UDR    = *(txBuf+0); //Отправляем первый байт.
	UCSRB |= (1<<TXCIE); //Включение прерывания по передаче байта.
	//-------------------------
}
//*******************************************************************************************
//*******************************************************************************************
//Прерывание по приему байта.
ISR (USART_RXC_vect){
	
	//Именно в такой последовательности вычитывать байт из UDR
	uint8_t UCSRA_temp = UCSRA;
	uint8_t data       = UDR;
	//-------------------------
	//Если байт не битый то обрабатыаем его. Если битый то пропуcкаем.
	if((UCSRA_temp & ((1 << FE) | (1 << DOR) | (1 << UPE))) == 0)
	{
		WorkRegStr.ReciveCounterByte++;
		//-------------------------
		//Работа с байтами при адресе 1.
		//if (RA_ConfigStr.STR.Address == 1) usart_recive_RetStat_RM_D(data);
		//Работа с байтами при адресах 2 - 64.
		//else usart_recive_for_RA2(data);
		usart_recive_for_RA2(data);
		//-------------------------
	}
	//-------------------------
	//Если байт битый.
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
//Прерывание по завершению передачи буфера.
ISR (USART_TXC_vect){
	
	//uint8_t  txCounter = UsartWorkReg.TxCounterByte;//Счетчик переданных байт.
	//uint8_t  txSize    = UsartWorkReg.TxBufSize;    //Количество передаваемых байт.
	//uint8_t *TxBufPtr;     //Указатель на передающий буфер.	
	//-------------------------
	//Передан весь буффер?
	if(++UsartWorkReg.TxCounterByte >= UsartWorkReg.TxBufSize)
	{
		UCSRB &= ~(1<<TXCIE); //Запрет прерывание по завершению передачи байта.
	}
	//-------------------------
	//Передача байта из буфера.
	else UDR = *(UsartWorkReg.TxBufPtr + UsartWorkReg.TxCounterByte);


	//if(++WorkRegStr.TransmitCounterByte >= WorkRegStr.TransmitBufSize) 
	//{
	 	//UCSRB &= ~(1<<TXCIE);			   //Запрет прерывание по завершению передачи байта.
	 	//WorkRegStr.TransmitCounterByte = 0;//Cброс счетчика байт.
		//WorkRegStr.TransmitBufSize     = 0; 
	 	////-------------------------
		////Включение микросхемы RS485 на приём.
		//DE_RE_PORT &= ~DE_RE;
		////-------------------------
		////Запуск приема если ожидается ответ.
		//if (WorkRegStr.UsartFlagReg & USART_RECIVE_START_FLAG)
		//{
			//WorkRegStr.UsartFlagReg &= ~USART_RECIVE_START_FLAG;
			////-------------------------
			////Запуск приема ответа от консоли.
			//WorkRegStr.ReciveCounterByte = 0;
			//RxBuffer1[0] = 0;
			//RxBuffer1[1] = 0;
			//UCSRB |= (1<<RXEN | 1<<RXCIE); 
			////-------------------------
		//}
		////-------------------------
	//}
	//-------------------------
	//Передача байта из буфера.
	//else UDR = TxBuffer[WorkRegStr.TransmitCounterByte];
	//-------------------------
}
//*******************************************************************************************
//*******************************************************************************************




