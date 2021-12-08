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

#include "my_lib\hef4052.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void hef4052_init(void){
	
	HEF_DDR |= (HEF_MASTER_IN_A0 | HEF_MASTER_IN_A1 | HEF_UM_A0 | HEF_UM_A1);	//����� �� �����.
	HEF_PORT |= (HEF_MASTER_IN_A0 | HEF_MASTER_IN_A1 | HEF_UM_A0 | HEF_UM_A1);	//�� ������ 1.
}
//-----------------------------------------------------------------------------
void hef4052_master_in(uint8_t select){
	
	switch(select)
		{
			//--------------------------
			case(HEF4052_MASTER_IN_LIN_OUT):
				HEF_PORT &= ~HEF_MASTER_IN_A0;
				HEF_PORT &= ~HEF_MASTER_IN_A1;			
			break;
			//--------------------------
			case(HEF4052_MASTER_IN_MIC_OUT):
				HEF_PORT |=  HEF_MASTER_IN_A0;
				HEF_PORT &= ~HEF_MASTER_IN_A1;
			break;
			//--------------------------
			case(HEF4052_MASTER_IN_CONSOL):
				HEF_PORT &= ~HEF_MASTER_IN_A0;
				HEF_PORT |=  HEF_MASTER_IN_A1;
			break;
			//--------------------------
			case(HEF4052_MASTER_IN_ALL_OFF):
				HEF_PORT |=  HEF_MASTER_IN_A0;
				HEF_PORT |=  HEF_MASTER_IN_A1;
			break;
			//--------------------------
		}
}
//-----------------------------------------------------------------------------
void hef4052_um(uint8_t select){
	
	switch(select)
		{
			//--------------------------
			case (HEF4052_UM_MASTER_OUT):
				HEF_PORT &= ~HEF_UM_A0;
				HEF_PORT &= ~HEF_UM_A1;				
			break;
			//--------------------------
			case (HEF4052_UM_EM2):
				HEF_PORT |=  HEF_UM_A0;
				HEF_PORT &= ~HEF_UM_A1;		
			break;
			//--------------------------
			case (HEF4052_UM_EM1):
				HEF_PORT &= ~HEF_UM_A0;
				HEF_PORT |=  HEF_UM_A1;				
			break;
			//--------------------------
			case (HEF4052_UM_ALL_OFF):
				HEF_PORT |=  HEF_UM_A0;
				HEF_PORT |=  HEF_UM_A1;					
			break;
			//--------------------------
		}
}
//-----------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------