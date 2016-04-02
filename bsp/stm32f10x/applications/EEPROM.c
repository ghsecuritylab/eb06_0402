
#include "includes.h"
#include "rtthread.h"
#include "eeprom.h"
/*************************************************************************************/
/*************************************************************************************/


typedef enum {FALSE = 0, TRUE = !FALSE} bool;


#define	EEPROM_DEVICE_ADDRESS	0xA4//0xA4

/*---------------------------------------------------------------------*/
/* I2C EEPROM */
#define countof(a) (sizeof(a) / sizeof(*(a)))

#define SCL_H         GPIOA->BSRR = GPIO_Pin_4		
#define SCL_L         GPIOA->BRR  = GPIO_Pin_4 		
   
#define SDA_H         GPIOA->BSRR = GPIO_Pin_5 		
#define SDA_L         GPIOA->BRR  = GPIO_Pin_5 			

#define SCL_read      GPIOA->IDR  & GPIO_Pin_4		
#define SDA_read      GPIOA->IDR  & GPIO_Pin_5

/*************************************************************************************/


void I2C_AT24C512_Init(void)
{
	GPIO_InitTypeDef  GPIOB_InitStructure;
	
	/* GPIOB Periph clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/* Configure I2C2 pins: SCL and SDA */
	GPIOB_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5;
	GPIOB_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIOB_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOA, &GPIOB_InitStructure);		
}


#if SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_16M

void I2C_delay(void)
{	
   u8 i=10; 		//this value can be optimized
   while(i--);
}
#elif SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_72M
void EEP_I2C_delay(void)
{	
   u8 i=40; 		//this value can be optimized
   while(i--);
}

#elif SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_48M
void I2C_delay(void)
{	
   u8 i=25; 		//this value can be optimized
   while(i--);
}

#endif

bool EEP_I2C_Start(void)
{
	SDA_H;
	SCL_H;
	EEP_I2C_delay();
	if(!SDA_read)return FALSE;	
	SDA_L;
	EEP_I2C_delay();
	if(SDA_read) return FALSE;	
	SDA_L;
	EEP_I2C_delay();
	return TRUE;
}

void EEP_I2C_Stop(void)
{
	SCL_L;
	EEP_I2C_delay();
	SDA_L;
	EEP_I2C_delay();
	SCL_H;
	EEP_I2C_delay();
	SDA_H;
	EEP_I2C_delay();
}

void EEP_I2C_Ack(void)
{	
	SCL_L;
	EEP_I2C_delay();
	SDA_L;
	EEP_I2C_delay();
	SCL_H;
	EEP_I2C_delay();
	SCL_L;
	EEP_I2C_delay();
}

void EEP_I2C_NoAck(void)
{	
	SCL_L;
	EEP_I2C_delay();
	SDA_H;
	EEP_I2C_delay();
	SCL_H;
	EEP_I2C_delay();
	SCL_L;
	EEP_I2C_delay();
}

/* return: TRUE:Ack; FALSE: No Ack */
bool EEP_I2C_WaitAck(void) 	 
{
	SCL_L;
	EEP_I2C_delay();
	SDA_H;			
	EEP_I2C_delay();
	SCL_H;
	EEP_I2C_delay();
	if(SDA_read) {
      	SCL_L;
      	return FALSE;
	}
	SCL_L;
	return TRUE;
}

/* send one byte data from high bit to low bit */
void EEP_I2C_SendByte(u8 SendByte) 
{
    u8 i=8;
    while(i--) {
        SCL_L;
        EEP_I2C_delay();
      	if(SendByte&0x80)
        	SDA_H;  
      	else 
        	SDA_L;   
        SendByte<<=1;
        EEP_I2C_delay();
		SCL_H;
        EEP_I2C_delay();
    }
    SCL_L;
}

/* receive one byte data from high bit to low bit */
u8 EEP_I2C_ReceiveByte(void)  
{ 
    u8 i=8;
    u8 ReceiveByte=0;

    SDA_H;				
    while(i--) {
      	ReceiveByte<<=1;      
      	SCL_L;
      	EEP_I2C_delay();
	  	SCL_H;
      	EEP_I2C_delay();	
      	if(SDA_read) {
        	ReceiveByte|=0x01;
      		}
    	}
    SCL_L;
    return ReceiveByte;
}

/**********************************************************************************************
										AT24Cxxxx
***********************************************************************************************/

bool EEPROM_WriteByte(u8 SendByte, EEPROM_ADDR_TYPE WriteAddress, u8 DeviceAddress)
{

u8 SlaveAddress;
  SlaveAddress=(WriteAddress>>15)%256;
  SlaveAddress = SlaveAddress & 0x0e | DeviceAddress;

    if(!EEP_I2C_Start())return FALSE;
	EEP_I2C_SendByte(SlaveAddress);
    if(!EEP_I2C_WaitAck()){EEP_I2C_Stop(); return FALSE;}
	EEP_I2C_SendByte((u8)((WriteAddress>>8) & 0x00FF));   
	EEP_I2C_WaitAck();
    EEP_I2C_SendByte((u8)(WriteAddress & 0x00FF));      
    EEP_I2C_WaitAck();	
    EEP_I2C_SendByte(SendByte);
    EEP_I2C_WaitAck();   
    EEP_I2C_Stop(); 
    return TRUE;
}

bool EEPROM_WriteBuffer(u8* pBuffer, EEPROM_PAGE_BYTES_TYPE length, EEPROM_ADDR_TYPE WriteAddress, u8 DeviceAddress)
{

u8 SlaveAddress;
  SlaveAddress=(WriteAddress>>15)%256;
  SlaveAddress = SlaveAddress & 0x0e | DeviceAddress;

    if(!EEP_I2C_Start())return FALSE;
	EEP_I2C_SendByte(SlaveAddress);
    if(!EEP_I2C_WaitAck()){EEP_I2C_Stop(); return FALSE;}
	EEP_I2C_SendByte((u8)((WriteAddress>>8) & 0x00FF));   
	EEP_I2C_WaitAck();
    EEP_I2C_SendByte((u8)(WriteAddress & 0x00FF));    
	EEP_I2C_WaitAck();	
	  
	while(length--) {
		EEP_I2C_SendByte(* pBuffer);
		EEP_I2C_WaitAck();
		pBuffer++;
		}
	EEP_I2C_Stop();
	return TRUE;
}

void EEPROM_WritePage(u8* pBuffer, EEPROM_PAGE_BYTES_TYPE length, EEPROM_ADDR_TYPE WriteAddress, u8 DeviceAddress)
{
    u8 NumOfPage = 0; 
	u8 NumOfSingle = 0; 
	u8 Addr = 0; 
	u8 count = 0;
    Addr  = WriteAddress % EEPROM_PAGE_SIZE;      
    count = EEPROM_PAGE_SIZE - Addr;			
    NumOfPage   =  length / EEPROM_PAGE_SIZE;     
    NumOfSingle =  length % EEPROM_PAGE_SIZE;    

    if(Addr == 0) {
		if(NumOfPage == 0) {
			EEPROM_WriteBuffer(pBuffer,NumOfSingle,WriteAddress,DeviceAddress);   
			} else {
				while(NumOfPage) {
					EEPROM_WriteBuffer(pBuffer,EEPROM_PAGE_SIZE,WriteAddress,DeviceAddress);
					WriteAddress +=  EEPROM_PAGE_SIZE;		  
					pBuffer      +=  EEPROM_PAGE_SIZE;
					NumOfPage--;
					Delay(1);
					}
				if(NumOfSingle!=0) {
					EEPROM_WriteBuffer(pBuffer,NumOfSingle,WriteAddress,DeviceAddress); 
					Delay(1);
					}
			}
    	} else {
			if(NumOfPage== 0) {
				EEPROM_WriteBuffer(pBuffer,NumOfSingle,WriteAddress,DeviceAddress);   
				} else {
					length       -= count;
					NumOfPage     = length / EEPROM_PAGE_SIZE;  
					NumOfSingle   = length % EEPROM_PAGE_SIZE;  	
			
					if(count != 0) {  
						EEPROM_WriteBuffer(pBuffer,count,WriteAddress,DeviceAddress);     
						WriteAddress += count;
						pBuffer      += count;
						} 
			
					while(NumOfPage--) {
						EEPROM_WriteBuffer(pBuffer,EEPROM_PAGE_SIZE,WriteAddress,DeviceAddress);
						WriteAddress +=  EEPROM_PAGE_SIZE;
						pBuffer      +=  EEPROM_PAGE_SIZE; 
						}
					if(NumOfSingle != 0) {
						EEPROM_WriteBuffer(pBuffer,NumOfSingle,WriteAddress,DeviceAddress);  
						}
				}
    	} 
}

bool EEPROM_ReadBuffer(u8* pBuffer, EEPROM_PAGE_BYTES_TYPE length, EEPROM_ADDR_TYPE ReadAddress, u8 DeviceAddress)
{
	u8 SlaveAddress;
  SlaveAddress=(ReadAddress>>15)%256;
  SlaveAddress = SlaveAddress & 0x0e | DeviceAddress;


    if(!EEP_I2C_Start())return FALSE;
	EEP_I2C_SendByte(SlaveAddress);
    if(!EEP_I2C_WaitAck()){EEP_I2C_Stop(); return FALSE;}
	EEP_I2C_SendByte((u8)((ReadAddress>>8) & 0x00FF));   
	EEP_I2C_WaitAck();
    EEP_I2C_SendByte((u8)(ReadAddress & 0x00FF));    
    EEP_I2C_WaitAck();
    EEP_I2C_Start();
	EEP_I2C_SendByte(SlaveAddress+1);
    EEP_I2C_WaitAck();
    while(length) {
      	*pBuffer = EEP_I2C_ReceiveByte();
      	if(length == 1) EEP_I2C_NoAck();
      	else EEP_I2C_Ack(); 
      	pBuffer++;
      	length--;
    	}
    EEP_I2C_Stop();
    return TRUE;
}

u8 EEPROM_ReadByte(EEPROM_ADDR_TYPE ReadAddress, u8 DeviceAddress)
{
	u8 ReceiveData=0;
		u8 SlaveAddress;
  SlaveAddress=(ReadAddress>>15)%256;
  SlaveAddress = SlaveAddress & 0x0e | DeviceAddress;

    if(!EEP_I2C_Start())return FALSE;
	EEP_I2C_SendByte(SlaveAddress);
    if(!EEP_I2C_WaitAck()){EEP_I2C_Stop(); return FALSE;}
	EEP_I2C_SendByte((u8)((ReadAddress>>8) & 0x00FF));   
	EEP_I2C_WaitAck();
    EEP_I2C_SendByte((u8)(ReadAddress & 0x00FF));   
    EEP_I2C_WaitAck();
    EEP_I2C_Start();
	EEP_I2C_SendByte(SlaveAddress + 1);
    EEP_I2C_WaitAck();
	ReceiveData = EEP_I2C_ReceiveByte();
	EEP_I2C_NoAck();
    EEP_I2C_Stop();
	return ReceiveData;			
}

void eeprom_byte_write(EEPROM_ADDR_TYPE address, uchar data)
{

	EEPROM_WriteByte(data, address, EEPROM_DEVICE_ADDRESS);
	//delay_half_ms(200);
    delay_X1ms(50);

}

uchar eeprom_byte_read(EEPROM_ADDR_TYPE address)
{
	uchar  ReadDataTemp = 0;


	ReadDataTemp = EEPROM_ReadByte(address, EEPROM_DEVICE_ADDRESS);
	//delay_half_ms(200);
    delay_X1ms(50);


	return ReadDataTemp;
}



//control=0:just write address for reading; or,write a byte to designated address of external EEPROM.
void exEEPROM_byte_write(EEPROM_ADDR_TYPE address,uchar data,uchar control)
{

	control = control;
	rt_interrupt_enter();
	EEPROM_WriteByte(data, address, EEPROM_DEVICE_ADDRESS);
	rt_interrupt_leave();

	delay_half_ms(3);
}


uchar exEEPROM_byte_read(EEPROM_ADDR_TYPE address)
{

 	uchar  ReadDataTemp = 0;
	rt_interrupt_enter();

	ReadDataTemp = EEPROM_ReadByte(address, EEPROM_DEVICE_ADDRESS);
	rt_interrupt_leave();
	
	delay_half_ms(2);
	return ReadDataTemp;
}


void exEEPROM_block_write(EEPROM_ADDR_TYPE start_addr, uchar *start_data)
{

 EEPROM_WriteBuffer(start_data, EEPROM_PAGE_SIZE, start_addr, EEPROM_DEVICE_ADDRESS);

 #if 1//__exEEPROM_block_Option_delay
 delay_half_ms(5);
 #endif
}


void exEEPROM_block_read(EEPROM_ADDR_TYPE start_addr, uchar *start_data)
{

 EEPROM_ReadBuffer(start_data, EEPROM_PAGE_SIZE, start_addr, EEPROM_DEVICE_ADDRESS);

 #if __exEEPROM_block_Option_delay
 delay_half_ms(5);
 #endif
}


