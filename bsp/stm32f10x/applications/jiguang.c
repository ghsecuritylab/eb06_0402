#include "includes.h"
#include "Protocol.h"
#include "Queue.h"
#include "malloc.h"

#include "rtthread.h"
#include <rtdevice.h>

s16 jiguang_zoom_steps = 0;
u8 jiguang_zoom_state = 0;


extern struct rt_serial_device serial3;




void uart3_rx_isr(uchar udr0)
{

		
       
}



/* Initialize the USART3做激光通信

----------------------------------------------*/
void BSP_USART3_Init(void)
{
	GPIO_InitTypeDef GPIOA_InitStructure;
//	USART_InitTypeDef USART_2_InitStructure;
//	USART_ClockInitTypeDef USART_2_ClockInitStructure;
	NVIC_InitTypeDef NVIC_USART2_InitStructure;
//	INT16U nCount;

	/* open the USART2 RCC */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, DISABLE);
	BSP_DelayMS(10);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

	/* open the USART2 intterrupt service */
	NVIC_USART2_InitStructure.NVIC_IRQChannel = UART4_IRQn;
					
	NVIC_USART2_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_USART2_InitStructure.NVIC_IRQChannelSubPriority = 7;//8;
	NVIC_USART2_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_USART2_InitStructure);

	/* Configure USART2 Tx (PA2) as alternate function push-pull */ 
	GPIOA_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIOA_InitStructure);
	
	/* Configure USART2 Rx (PA3) as input floating */ 
	GPIOA_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIOA_InitStructure);

}


void APP_USART3_Baudrate(INT16U baudrate)
{
	USART_InitTypeDef USART_2_InitStructure;
	INT16U BaudrateTemp;
//	USART_ClockInitTypeDef USART_2_ClockInitStructure;
	/* USART2 configuration */
  	/*USART2 configured as follow:
        - BaudRate = 9600 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the middle 
        - USART LastBit: The clock pulse of the last data bit is not output to 
                         the SCLK pin
  	*/
	BaudrateTemp = baudrate;
  	USART_2_InitStructure.USART_BaudRate = BaudrateTemp;
  	USART_2_InitStructure.USART_WordLength = USART_WordLength_8b;
  	USART_2_InitStructure.USART_StopBits = USART_StopBits_1;
  	USART_2_InitStructure.USART_Parity = USART_Parity_No;
  	USART_2_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  	USART_2_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART4, &USART_2_InitStructure);
		
}


void jiguang_io_init(void)
{
	BSP_USART3_Init();
	
	APP_USART3_Baudrate(9600);
	USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);
	USART_Cmd(UART4, ENABLE);

}


void jiguang_comm_init(void)
{

	jiguang_io_init();
	
	jiguang_motor_reset();


}
#if 0

u8 send_bytes_to_jiguang(u8 len,u8*databuff)
{

USART_SendData(UART4,(u8) i); //回发给PC   
 while(USART_GetFlagStatus(UART4, USART_FLAG_TXE)==RESET)
  {
  }
}
#else
u8 send_bytes_to_jiguang(u8 len,u8*databuff)
{
	
	rt_device_write(&serial3.parent, 0, databuff, len);


	return 1;

}
#endif

static u8 jiguang_com2[15];//return_com2[9];//	数据返回：P-P协议，返回的数据少一位。

void jiguang_cmd_packet_send(u8 data1,u8 data2,u8 data3,u8 data4)
{
	
	u8 i;
	u16 k=0;
	
	jiguang_com2[0] = 0xFF;
	jiguang_com2[1] = 0x01;
	jiguang_com2[2] = data1;
	jiguang_com2[3] = data2;
	jiguang_com2[4] = data3;	
	jiguang_com2[5] = data4;	

	for(i=1;i<6;i++)
	{
		k = k+jiguang_com2[i];
	}
	jiguang_com2[6] = (u8)k;	

	send_bytes_to_jiguang(7,jiguang_com2);
	delay_X1ms(30);
}


void jiguang_on_off_set(enum ON_OFF_TYPE mode)
{
	u8 i;

	if(mode==ON_MODE)
	{
		i=1;
	}
	else
		i=0;
	jiguang_cmd_packet_send(1,1,i,0);
	

}

//mode,1,increase the current,otherwise,decrease the current
void jiguang_current_set(u8 mode)
{
	u8 i;

	if(mode)//增加电流
	{
		i=0;
	}
	else
		i=1;
	jiguang_cmd_packet_send(1,2,i,0);
	

}

void jiguang_motor_reset(void)
{

	jiguang_cmd_packet_send(1,6,0,0);
	

}

void jiguang_zoom_set(u8 mode,u8 val)
{
	u8 i;

	if(mode)//tele
	{
		i=0;
	}
	else
		i=1;
	jiguang_cmd_packet_send(1,4,i,val);
	

}

#define	JIGUANG_ZOOM_STEPS  30;

void jiguang_zoom_handle(u8 mode)
{
	if(mode==1)//tele
	{

#if 1
		jiguang_zoom_steps = JIGUANG_ZOOM_STEPS;
#else
		jiguang_zoom_steps++;
		if(jiguang_zoom_steps > 255 )
			jiguang_zoom_steps = 255;
#endif
		jiguang_zoom_set(1,jiguang_zoom_steps);

	}
	else if(mode==2)//wide
	{
#if 1
				jiguang_zoom_steps = JIGUANG_ZOOM_STEPS;
#else
	
		jiguang_zoom_steps--;
		if(jiguang_zoom_steps < 0 )
			jiguang_zoom_steps = 0;
#endif		
		jiguang_zoom_set(0,jiguang_zoom_steps);

	}
	else if(mode==0)
	{

		
	}
}


