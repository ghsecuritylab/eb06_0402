#include "includes.h"

/*************************************************************************************/
/*************************************************************************************/

#define ADDRESS_LPS25H 0x5D
#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define PRESS_OUT_XL 0x28


#ifndef false
#define false   0
#endif

#ifndef true
#define true   0
#endif

typedef enum {FALSE = 0, TRUE = !FALSE} bool;



/*---------------------------------------------------------------------*/
/* I2C EEPROM */
#define countof(a) (sizeof(a) / sizeof(*(a)))

#define SCL_H         GPIOA->BSRR = GPIO_Pin_0		
#define SCL_L         GPIOA->BRR  = GPIO_Pin_0 		
   
#define SDA_H         GPIOA->BSRR = GPIO_Pin_1 		
#define SDA_L         GPIOA->BRR  = GPIO_Pin_1 			

#define SCL_read      GPIOA->IDR  & GPIO_Pin_0		
#define SDA_read      GPIOA->IDR  & GPIO_Pin_1

/*************************************************************************************/

void lps25_delay(int val)
{
    delay_X1ms(6);

}

void I2C_LPS25HB_Init(void)
{
	GPIO_InitTypeDef  GPIOB_InitStructure;
	
	/* GPIOB Periph clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/* Configure I2C2 pins: SCL and SDA */
	GPIOB_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1;
	GPIOB_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIOB_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOA, &GPIOB_InitStructure);		
}


void I2C_delay(void)
{	
   u8 i=40; 		//this value can be optimized
   while(i--);
}

bool I2C_Start(void)
{
	SDA_H;
	SCL_H;
	I2C_delay();
	if(!SDA_read)return FALSE;	
	SDA_L;
	I2C_delay();
	if(SDA_read) return FALSE;	
	SDA_L;
	I2C_delay();
	return TRUE;
}

void I2C_Stop(void)
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;
	I2C_delay();
}

void I2C_Ack(void)
{	
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

void I2C_NoAck(void)
{	
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

/* return: TRUE:Ack; FALSE: No Ack */
bool I2C_WaitAck(void) 	 
{
	SCL_L;
	I2C_delay();
	SDA_H;			
	I2C_delay();
	SCL_H;
	I2C_delay();
	if(SDA_read) {
      	SCL_L;
      	return FALSE;
	}
	SCL_L;
	return TRUE;
}

/* send one byte data from high bit to low bit */
void I2C_SendByte(u8 SendByte) 
{
    u8 i=8;
    while(i--) {
        SCL_L;
        I2C_delay();
      	if(SendByte&0x80)
        	SDA_H;  
      	else 
        	SDA_L;   
        SendByte<<=1;
        I2C_delay();
		SCL_H;
        I2C_delay();
    }
    SCL_L;
}

/* receive one byte data from high bit to low bit */
u8 I2C_ReceiveByte(void)  
{ 
    u8 i=8;
    u8 ReceiveByte=0;

    SDA_H;				
    while(i--) {
      	ReceiveByte<<=1;      
      	SCL_L;
      	I2C_delay();
	  	SCL_H;
      	I2C_delay();	
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


bool lps25hb_ReadBuffer(u8* pBuffer, u8 length, u8 ReadAddress)
{
    if(!I2C_Start())
        return FALSE;
	I2C_SendByte(ADDRESS_LPS25H);
    if(!I2C_WaitAck())
    {
        I2C_Stop(); 
        return FALSE;
    }
	I2C_SendByte(ReadAddress);   
	I2C_WaitAck();
    
    I2C_Start();
	I2C_SendByte(ADDRESS_LPS25H+1);
    I2C_WaitAck();
    while(length) 
    {
      	*pBuffer = I2C_ReceiveByte();
      	if(length == 1) I2C_NoAck();
      	else I2C_Ack(); 
      	pBuffer++;
      	length--;
	}
    I2C_Stop();
    return TRUE;
}

void lps25hb_setup(void)
{
    //power down the device (clean start)
    I2C_Start();
	I2C_SendByte(ADDRESS_LPS25H);
    I2C_WaitAck();
    
	I2C_SendByte(CTRL_REG1);   
	I2C_WaitAck();
    
    I2C_SendByte(0x00);      
    I2C_WaitAck();	
    I2C_Stop(); 
    
    //turn on the sensor, set the one-shot mode, and set the BDU bit
    I2C_Start();
	I2C_SendByte(ADDRESS_LPS25H);
    I2C_WaitAck();
    
	I2C_SendByte(CTRL_REG1);   
	I2C_WaitAck();
    
    I2C_SendByte(0x84);      
    I2C_WaitAck();	
    I2C_Stop(); 

}


bool lps25hb_WriteByte(u8 SendByte, u8 WriteAddress)
{

    if(!I2C_Start())
        return FALSE;
	I2C_SendByte(ADDRESS_LPS25H);
    if(!I2C_WaitAck())
    {
        I2C_Stop(); 
        return FALSE;
    }
	I2C_SendByte(WriteAddress);   
	I2C_WaitAck();
    I2C_SendByte(SendByte);
    I2C_WaitAck();   
    I2C_Stop(); 
    return TRUE;
}

u8 lps25hb_ReadByte(u8 ReadAddress)
{
	u8 ReceiveData=0;

    if(!I2C_Start())
    return FALSE;
	I2C_SendByte(ADDRESS_LPS25H);
    if(!I2C_WaitAck())
    {
        I2C_Stop(); 
        return FALSE;
    }

    I2C_SendByte(ReadAddress);   
    I2C_WaitAck();
    I2C_Start();
	I2C_SendByte(ADDRESS_LPS25H + 1);
    I2C_WaitAck();
	ReceiveData = I2C_ReceiveByte();
	I2C_NoAck();
    I2C_Stop();
	return ReceiveData;			
}


float tempVal_pub;
float presVal_pub;

void lps25hb_pressure_temperature_read(void)
{
    u8 ready = false;
    u8 buffer[6];
    int tempOut;
    long presOut;
    float tempVal;
    float presVal;

    //run one-shot measurement
    lps25hb_WriteByte(0x01,CTRL_REG2);
    
    //wait until the measurement is completed
    while (ready == false)
    {
        lps25_delay(5); //conversion time: ~37ms
 
        //lps25hb_ReadByte(1);
        if (lps25hb_ReadByte(1) == 0x00)
        {
            ready = true;
        }
    }

    //read the result

    lps25hb_ReadBuffer(buffer,5,PRESS_OUT_XL);
        
    //calculation
    presOut = (long)(buffer[2]<< 16)|(long)(buffer[1] << 8)|(long)(buffer[0]);
    presOut = (presOut << 8) >> 8; //PRESS_OUT_H/_L/_XL and is represented as 2's complement
    presVal = presOut/4096.0;

    tempOut = (buffer[4] << 8) | buffer[3];
    tempVal = 42.5 + tempOut/480.0;

    tempVal_pub = tempVal;
    presVal_pub = presVal;
}


