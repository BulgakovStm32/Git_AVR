/*
 *
 *
 *  Created on:
 *      Author:
 */
#include "my_lib/i2c.h"
//*******************************************************************************************
//*******************************************************************************************
void DoNothing(void);

uint8_t i2c_Do;								// ���������� ��������� ����������� IIC
uint8_t i2c_InBuff[I2C_MASTER_BYTE_RX];		// ����� ����� ��� ������ ��� Slave
uint8_t i2c_OutBuff[I2C_MASTER_BYTE_TX];	// ����� �������� ��� ������ ��� Slave
uint8_t i2c_SlaveIndex;						// ������ ������ Slave

uint8_t i2c_Buffer[I2C_MAX_BUF_SIZE];		// ����� ��� ������ ������ � ������ Master
uint8_t i2c_index;							// ������ ����� ������
uint8_t i2c_ByteCount;						// ����� ���� ������������

uint8_t i2c_SlaveAddress;					// ����� ������������

uint8_t i2c_PageAddress[I2C_MAX_PAGE_ADDR_LENGTH];// ����� ������ ������� (��� ������ � sawsarp)
uint8_t i2c_PageAddrIndex;						  // ������ ������ ������ �������
uint8_t i2c_PageAddrCount;						  // ����� ���� � ������ �������� ��� �������� Slave

											// ��������� ������ �� ��������:
IIC_F MasterOutFunc = &DoNothing;			//  � Master ������
IIC_F SlaveOutFunc 	= &DoNothing;			//  � ������ Slave
IIC_F ErrorOutFunc 	= &DoNothing;			//  � ���������� ������ � ������ Master
//**********************************************************

//**********************************************************
// ������� ��������, �������� �������������� ������
void DoNothing(void){

}																
//**********************************************************
// ��������� ������ �������
void I2C_Init(void){						

	I2C_PORT |=  (I2C_SCL | I2C_SDA); //������� �������� �� ����.
	I2C_DDR  &= ~(I2C_SCL | I2C_SDA);

	//�������� �������
	TWBR = 0xFF;         						
	TWSR = 0x03;
}
//**********************************************************
// ��������� ������ ������ (���� �����)
void I2C_Init_Slave(IIC_F Addr){

	TWAR = I2C_MASTER_ADDR;	//������ � ������� ���� �����, �� ������� ����� ����������. 
							//1 � ������� ���� ��������, ��� �� ���������� �� ����������������� ������
	SlaveOutFunc = Addr;	//�������� ��������� ������ �� ������ ������� ������

	TWCR = 0<<TWSTA|0<<TWSTO|0<<TWINT|1<<TWEA|1<<TWEN|1<<TWIE;// �������� ������� � �������� ������� ����.
}
//*******************************************************************************************
//*******************************************************************************************
// ���������� TWI. 
ISR (TWI_vect){
/*
PORTB ^= 0x01;								// ������� ����� �����, ��� ������������� ����������� ����������� � ������� ������ TWI


// ���������� �����. ����� ���� ������ ��������� �������� � ����� ������, � �����. �� ��������� ������ ����� UART �� ����
if (WorkIndex <99)							// ���� ��� �� ����������
{
	if (TWSR)								// ������ �������?
		{
		WorkLog[WorkIndex]= TWSR;			// ����� ������ � ���
		WorkIndex++;						
		}
	else
		{
		WorkLog[WorkIndex]= 0xFF;			// ���� ������ ������� �� ��������� FF
		WorkIndex++;
		}
}
*/
	//����������� ������� ����������.
	switch(TWSR & 0xF8)	{ // �������� ���� ����������
		//--------------------
		//Bus Fail - ���������� ������ ����. ��������, ��������� ����� ������� �������� ����.
		case (0x00):	
			i2c_Do |= i2c_ERR_BF;
			TWCR = 0<<TWSTA|1<<TWSTO|1<<TWINT|I2C_I_AM_SLAVE<<TWEA|1<<TWEN|1<<TWIE;// Go!
			MACRO_i2c_WhatDo_ErrorOut
		break;
		//--------------------
		//����� ���, � ����� �� � ����������� �� ������
		case (0x08):	
			if((i2c_Do & i2c_type_msk) == i2c_sarp) i2c_SlaveAddress |= 0x01;//���� Addr+R	
			else									i2c_SlaveAddress &= 0xFE;//���� Addr+W
			
			TWDR = i2c_SlaveAddress;													// ����� ������
			TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|I2C_I_AM_SLAVE<<TWEA|1<<TWEN|1<<TWIE;  	// Go!
		break;
		//--------------------	
		//��� ��������� ��������� �����. ����� ������������� � ������ �� ������ ��� ��������. �� ������ �������.
		case (0x10):
			if( (i2c_Do & i2c_type_msk) == i2c_sawsarp) i2c_SlaveAddress |= 0x01;//���� Addr+R
			else										i2c_SlaveAddress &= 0xFE;//���� Addr+W
		
			// To Do: �������� ���� ��������� ������ 

			TWDR = i2c_SlaveAddress;													// ����� ������
			TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|I2C_I_AM_SLAVE<<TWEA|1<<TWEN|1<<TWIE;  	// Go!
		break;
		//--------------------
		//��� ������ SLA+W �������� ACK, � ����� � ����������� �� ������
		case (0x18):	
			if((i2c_Do & i2c_type_msk) == i2c_sawp)						
			{
				TWDR = i2c_Buffer[i2c_index];//���� ���� ������
				i2c_index++;				 //����������� ��������� ������
				TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|I2C_I_AM_SLAVE<<TWEA|1<<TWEN|1<<TWIE;  // Go! 
			}

			if((i2c_Do & i2c_type_msk) == i2c_sawsarp)
			{
				TWDR = i2c_PageAddress[i2c_PageAddrIndex];//��� ���� ����� ������� (�� ���� ���� ���� ������)
				i2c_PageAddrIndex++;					  //����������� ��������� ������ ��������
				TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|I2C_I_AM_SLAVE<<TWEA|1<<TWEN|1<<TWIE;	// Go!
			}	 
		break;
		//--------------------
		//��� ������ SLA+W �������� NACK - ����� ���� �����, ���� ��� ��� ����.
		case (0x20):	
			i2c_Do |= i2c_ERR_NA;												   //��� ������
			TWCR = 0<<TWSTA|1<<TWSTO|1<<TWINT|I2C_I_AM_SLAVE<<TWEA|1<<TWEN|1<<TWIE;//���� ���� Stop

			MACRO_i2c_WhatDo_ErrorOut 											   // ������������ ������� ������;
		break;
		//--------------------
		//���� ������ �������, �������� ACK!  (���� sawp - ��� ��� ���� ������. ���� sawsarp - ���� ������ ��������)// � ������: 
		case (0x28): 	
			if((i2c_Do & i2c_type_msk) == i2c_sawp)							// � ����������� �� ������
			{
				if (i2c_index == i2c_ByteCount)												// ���� ��� ���� ������ ���������
				{																		
					TWCR = 0<<TWSTA|1<<TWSTO|1<<TWINT|I2C_I_AM_SLAVE<<TWEA|1<<TWEN|1<<TWIE;	// ���� Stop
					MACRO_i2c_WhatDo_MasterOut												// � ������� � ��������� �����
				}
				else
				{
					TWDR = i2c_Buffer[i2c_index];												// ���� ���� ��� ���� ����
					i2c_index++;
					TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|I2C_I_AM_SLAVE<<TWEA|1<<TWEN|1<<TWIE;  	// Go!
				}
			}

			if((i2c_Do & i2c_type_msk) == i2c_sawsarp)						// � ������ ������ ��
			{
				if(i2c_PageAddrIndex == i2c_PageAddrCount)					// ���� ��������� ���� ������ ��������
				{
					TWCR = 1<<TWSTA|0<<TWSTO|1<<TWINT|I2C_I_AM_SLAVE<<TWEA|1<<TWEN|1<<TWIE;		// ��������� ��������� �����!
				}
				else
				{															// ����� 
					TWDR = i2c_PageAddress[i2c_PageAddrIndex];				// ���� ��� ���� ����� ��������
					i2c_PageAddrIndex++;									// ����������� ������ �������� ������ �������
					TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|I2C_I_AM_SLAVE<<TWEA|1<<TWEN|1<<TWIE;		// Go!
				}
			}	 
		break;
		//--------------------
		//���� ����, �� �������� NACK ������ ���. 1� �������� �������� ������� � ��� ����. 2� ����� �������.
		case (0x30):	
			i2c_Do |= i2c_ERR_NK;				// ������� ������ ������. ���� ��� �� ����, ��� ������. 

			TWCR = 0<<TWSTA|1<<TWSTO|1<<TWINT|I2C_I_AM_SLAVE<<TWEA|1<<TWEN|1<<TWIE;		// ���� Stop

			MACRO_i2c_WhatDo_MasterOut													// ������������ ������� ������
		break;
		//--------------------
		//�������� �� ����. ������� ��� �� ���������
		case (0x38):	
			i2c_Do |= i2c_ERR_LP;			// ������ ������ ������ ����������

			// ����������� ������� ������. 
			i2c_index = 0;
			i2c_PageAddrIndex = 0;

			TWCR = 1<<TWSTA|0<<TWSTO|1<<TWINT|I2C_I_AM_SLAVE<<TWEA|1<<TWEN|1<<TWIE;// ��� ������ ���� ����� �������� 
		break;																	   // ��������� �������� �����.
		//--------------------
		//������� SLA+R �������� ���. � ������ ����� �������� �����
		case (0x40): 
			if(i2c_index+1 == i2c_ByteCount)								// ���� ����� �������� �� ���� �����, �� 
			{
				TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|0<<TWEA|1<<TWEN|1<<TWIE;	// ������� ����, � � ����� ����� ������ NACK(Disconnect)
			}															// ��� ���� ������ ������, ��� ��� ������ �����. � �� �������� ����
			else
			{
				TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|1<<TWEA|1<<TWEN|1<<TWIE;	// ��� ������ ������ ���� � ������ ����� ACK
			}
		break;
		//--------------------
		//������� SLA+R, �� �������� NACK. ������ slave ����� ��� ��� ��� ����.
		case (0x48):  
			i2c_Do |= i2c_ERR_NA;															// ��� ������ No Answer
			TWCR = 0<<TWSTA|1<<TWSTO|1<<TWINT|I2C_I_AM_SLAVE<<TWEA|1<<TWEN|1<<TWIE;			// ���� Stop

			MACRO_i2c_WhatDo_ErrorOut														// ������������ �������� �������� ������
		break;
		//--------------------
		//������� ����.
		case (0x50):
			i2c_Buffer[i2c_index] = TWDR;// ������� ��� �� ������
			i2c_index++;

			// To Do: �������� �������� ������������ ������. � �� ���� �� ��� ���� ���������

			//���� ������� ��� ���� ���� �� ���, ��� �� ������ �������
			if(i2c_index+1 == i2c_ByteCount) TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|0<<TWEA|1<<TWEN|1<<TWIE;//����������� ��� � ����� ������ NACK (Disconnect)
			else						     TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|1<<TWEA|1<<TWEN|1<<TWIE;//���� ���, �� ����������� ��������� ����, � � ����� ������ ���
		break;
		//--------------------
		//��� �� ����� ��������� ����, ������� NACK ����� �������� � �����. 
		case (0x58):	
			i2c_Buffer[i2c_index] = TWDR;										   //����� ���� � �����
			TWCR = 0<<TWSTA|1<<TWSTO|1<<TWINT|I2C_I_AM_SLAVE<<TWEA|1<<TWEN|1<<TWIE;//�������� Stop
			
			MACRO_i2c_WhatDo_MasterOut												// ���������� ����� �����
		break;

	//IIC_Slave ============================================================================
		case (0x68):	// RCV SLA+W Low Priority							// ������� ���� ����� �� ����� �������� ��������
		//--------------------
		case (0x78):	// RCV SLA+W Low Priority (Broadcast)				// ��� ��� ��� ����������������� �����. �� �����
			i2c_Do |= i2c_ERR_LP | i2c_Interrupted;//������ ���� ������ Low Priority, � ����� ���� ����, ��� ������� ��������
			// Restore Trans after.
			i2c_index = 0;						   //����������� ��������� �������� ������
			i2c_PageAddrIndex = 0;                 //� ����� ������. ��������!!! break ��� ���, � ������ ���� � "case 60"
		//--------------------
		case (0x60): // RCV SLA+W  Incoming?								// ��� ������ �������� ���� �����
		//--------------------
		//��� ����������������� �����
		case (0x70): // RCV SLA+W  Incoming? (Broascast)					
			i2c_Do |= i2c_Busy;										// �������� ����. ����� ������ �� ��������
			i2c_SlaveIndex = 0;										// ��������� �� ������ ������ ������, ������� ����� �����. �� ��������

			//���� ��� ������� ������� ����� ���� ����, �� ��������� �������  ���
			if (I2C_MASTER_BYTE_RX == 1) TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|0<<TWEA|1<<TWEN|1<<TWIE;//������� � ������� ����� ��� �... NACK!					
			else						 TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|1<<TWEA|1<<TWEN|1<<TWIE;//� ���� ���� ���� ��� ���� ����, �� ������ � ��������� ��� ACK!
		break;
		//--------------------
		case (0x80):	// RCV Data Byte									// � ��� �� ������� ���� ����. ��� ��� �����������������. �� �����
		//--------------------
		case (0x90):	// RCV Data Byte (Broadcast
			i2c_InBuff[i2c_SlaveIndex] = TWDR;						// ������� ��� � �����.
			i2c_SlaveIndex++;										// �������� ���������

			//�������� ����� ����� ��� ���� ����? 
			if(i2c_SlaveIndex == I2C_MASTER_BYTE_RX-1) TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|0<<TWEA|1<<TWEN|1<<TWIE;//������� ��� � ������� NACK!
			else									   TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|1<<TWEA|1<<TWEN|1<<TWIE;//����� ��� ������? ������� � ACK!
		break;
		//--------------------
		case (0x88): // RCV Last Byte										// ������� ��������� ����
		//--------------------
		case (0x98): // RCV Last Byte (Broadcast)
			i2c_InBuff[i2c_SlaveIndex] = TWDR;// ������� ��� � �����
			
			//���� � ��� ��� ���������� ����� �� ����� �������
			if (i2c_Do & i2c_Interrupted) TWCR = 1<<TWSTA|0<<TWSTO|1<<TWINT|1<<TWEA|1<<TWEN|1<<TWIE;//������ � ���� ���� Start �������� � ������� ��� ���� �������						
			else						  TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|1<<TWEA|1<<TWEN|1<<TWIE;//���� �� ���� ������ �����, �� ������ ��������� � ����� �����
		
			MACRO_i2c_WhatDo_SlaveOut //� ������ ���������� ��� �������� ���� ��� ������
		break;
		//--------------------
		//��, �� �������� ��������� �����. �� �� ��� � ��� ������? 
		case (0xA0): 
			// �����, �������, ������� ��������������� �������, ����� ������������ ��� � ������ ���������� �������, ������� ��������. 
			// �� � �� ���� ��������������. � ���� ������ �������� ��� ���.

			TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|1<<TWEA|1<<TWEN|1<<TWIE;			// ������ �������������, �������������� ���� �����	
		break;
		//--------------------
		//������� ���� ����� �� ������ �� ����� �������� ��������			
		case (0xB0):  
			i2c_Do |= i2c_ERR_LP | i2c_Interrupted;//�� ��, ���� ������ � ���� ��������� ��������.
			
			// ��������������� �������
			i2c_index = 0;
			i2c_PageAddrIndex = 0;// Break ���! ���� ������						
		//--------------------
		//���� ������ ������� ���� ����� �� ������
		case (0xA8):	
			i2c_SlaveIndex = 0;								// ������� ��������� �������� �� 0

			TWDR = i2c_OutBuff[i2c_SlaveIndex];				// ����, ������� ���� �� ��� ��� ����.
			
			if(I2C_MASTER_BYTE_TX == 1) TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|0<<TWEA|1<<TWEN|1<<TWIE;//���� �� ���������, �� ��� �� NACK � ����� �������� 
			else					    TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|1<<TWEA|1<<TWEN|1<<TWIE;//� ���� ���, ��  ACK ����
		break;
		//--------------------			
		//������� ����, �������� ACK
		case (0xB8): 
			i2c_SlaveIndex++;								// ������ ���������� ���������. ����� ��������� ����
			TWDR = i2c_OutBuff[i2c_SlaveIndex];				// ���� ��� �������

			//���� �� ��������� ���, ��
			if(i2c_SlaveIndex == I2C_MASTER_BYTE_TX-1) TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|0<<TWEA|1<<TWEN|1<<TWIE;//���� ��� � ���� NACK	
			else									   TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|1<<TWEA|0<<TWEN|1<<TWIE;//���� ���, �� ���� � ���� ACK
		break;
		//--------------------	
		case (0xC0): // �� ������� ��������� ����, ������ � ��� ���, �������� NACK
		//--------------------	
		// ��� ACK. � ������ ������ ��� ���. �.�. ������ ������ � ��� ���.	
		case (0xC8): 
			if(i2c_Do & i2c_Interrupted)								   //���� ��� ���� ��������� �������� �������
			{															   //�� �� ��� �� ������
				i2c_Do &= i2c_NoInterrupted;							   //������ ���� �����������
				TWCR = 1<<TWSTA|0<<TWSTO|1<<TWINT|1<<TWEA|1<<TWEN|1<<TWIE; //�������� ����� ����� �� ��� ������� ����.
			}
			else TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|1<<TWEA|1<<TWEN|1<<TWIE;//���� �� ��� ����, �� ������ ������� ����
		
			MACRO_i2c_WhatDo_SlaveOut									   //� ���������� ����� ������. �������, �� ���
																		   //�� ����� �� �����. ����� ��� ��� ������, ��� ������
		break;															   //��� ������ ����� �������.
		//--------------------	
		default:	break;
		//--------------------	
		}
}
//*******************************************************************************************
//*******************************************************************************************



