/*
 * File      : board.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009 RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      first implementation
 * 2013-07-12     aozima       update for auto initial.
 */

#include <rthw.h>
#include <rtthread.h>

#include "stm32f10x.h"
#include "stm32f10x_fsmc.h"
#include "board.h"
#include "usart.h"

#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif  /* RT_USING_COMPONENTS_INIT */


#include "includes.h"
#include "stdlib.h"
#include    <math.h>

#include "Protocol.h"



/*@{*/

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM
    /* Set the Vector Table base location at 0x20000000 */
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif
}

#if STM32_EXT_SRAM
void EXT_SRAM_Configuration(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  p;

    /* FSMC GPIO configure */
    {
        GPIO_InitTypeDef GPIO_InitStructure;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF
                               | RCC_APB2Periph_GPIOG, ENABLE);
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

        /*
        FSMC_D0 ~ FSMC_D3
        PD14 FSMC_D0   PD15 FSMC_D1   PD0  FSMC_D2   PD1  FSMC_D3
        */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_14 | GPIO_Pin_15;
        GPIO_Init(GPIOD,&GPIO_InitStructure);

        /*
        FSMC_D4 ~ FSMC_D12
        PE7 ~ PE15  FSMC_D4 ~ FSMC_D12
        */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10
                                      | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
        GPIO_Init(GPIOE,&GPIO_InitStructure);

        /* FSMC_D13 ~ FSMC_D15   PD8 ~ PD10 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
        GPIO_Init(GPIOD,&GPIO_InitStructure);

        /*
        FSMC_A0 ~ FSMC_A5   FSMC_A6 ~ FSMC_A9
        PF0     ~ PF5       PF12    ~ PF15
        */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3
                                      | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
        GPIO_Init(GPIOF,&GPIO_InitStructure);

        /* FSMC_A10 ~ FSMC_A15  PG0 ~ PG5 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
        GPIO_Init(GPIOG,&GPIO_InitStructure);

        /* FSMC_A16 ~ FSMC_A18  PD11 ~ PD13 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
        GPIO_Init(GPIOD,&GPIO_InitStructure);

        /* RD-PD4 WR-PD5 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
        GPIO_Init(GPIOD,&GPIO_InitStructure);

        /* NBL0-PE0 NBL1-PE1 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
        GPIO_Init(GPIOE,&GPIO_InitStructure);

        /* NE1/NCE2 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_Init(GPIOD,&GPIO_InitStructure);
        /* NE2 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
        GPIO_Init(GPIOG,&GPIO_InitStructure);
        /* NE3 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_Init(GPIOG,&GPIO_InitStructure);
        /* NE4 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
        GPIO_Init(GPIOG,&GPIO_InitStructure);
    }
    /* FSMC GPIO configure */

    /*-- FSMC Configuration ------------------------------------------------------*/
    p.FSMC_AddressSetupTime = 0;
    p.FSMC_AddressHoldTime = 0;
    p.FSMC_DataSetupTime = 2;
    p.FSMC_BusTurnAroundDuration = 0;
    p.FSMC_CLKDivision = 0;
    p.FSMC_DataLatency = 0;
    p.FSMC_AccessMode = FSMC_AccessMode_A;

    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

    /* Enable FSMC Bank1_SRAM Bank */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);
}
#endif

/**
 * This is the timer interrupt service routine.
 *
 */
void SysTick_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}


/**
 * @addtogroup STM32
 */

/*@{*/

void timer0_int(void);
void timer0_initial(void);
void timer3_init(void);



// recieve datas from keyboard
void uart0_init(void)
{
// UCSR0B = 0x00; //disable while setting baud rate
// UCSR0A = 0x00;
// UCSR0C = 0x06;
//-----------先关闭串口功能
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
	USART_Cmd(USART1, DISABLE);


	BSP_USART1_Init();

	switch(Baud_rate)
	{
    case DOME_BAUDRATE_1200: 
				APP_USART1_Baudrate(1200);
			   break;    
	case DOME_BAUDRATE_2400: 
				APP_USART1_Baudrate(2400);
			   break;
	case DOME_BAUDRATE_4800: 
				APP_USART1_Baudrate(4800);
			   break;
	case DOME_BAUDRATE_9600: 
				APP_USART1_Baudrate(9600);
			   break;
	case DOME_BAUDRATE_14400:
		APP_USART1_Baudrate(14400);
	       break;
	case DOME_BAUDRATE_19200:
		APP_USART1_Baudrate(19200);
	       break;
	case DOME_BAUDRATE_115200:
		APP_USART1_Baudrate(115200);
	       break;	   
	default:   
		APP_USART1_Baudrate(2400);
	    break;
	}

	/* Enable USART1 receive intterrupt */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
  	/* Enable USART1 */
  	USART_Cmd(USART1, ENABLE);
	
	/* set the control pin at receive status */
	//GPIO_ResetBits(GPIOA, RS485Control);

	RS485_RX_ENABLE;
	//RS485_TX_ENABLE;

}


// control the camera
void uart_to_camera_init(void)
{


}



/* Initialize the USART1----------------------------------------------*/
void rs485_rx_enable(void)
{
  	GPIO_InitTypeDef GPIOA_InitStructure;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	/* configure the rs485 control pin of the receive or send */
	GPIOA_InitStructure.GPIO_Pin = RS485_RX_TX_CTL_PIN;
	GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RS485_RX_TX_CTL_PORT, &GPIOA_InitStructure);

	/*设置RS485为接收模式*/
	GPIO_ResetBits(RS485_RX_TX_CTL_PORT,RS485_RX_TX_CTL_PIN);
}

void ports_initial(void)
{

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	// 改变指定管脚的映射 GPIO_Remap_SWJ_Disable SWJ 完全禁用（JTAG+SW-DP）
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
	// 改变指定管脚的映射 GPIO_Remap_SWJ_JTAGDisable ，JTAG-DP 禁用 + SW-DP 使能
	
	rs485_rx_enable();      

	

	
}



//////////////////////////////////////////////////////////////////
#if 1//CAMERA_HITACHI_SC110

//return: 1,success;0,overtime
u8 RS485_SendBytes(u16 bytes_size,u8 *send_buff)
{
	u32 overtime=0;
	u16 i;
	RS485_TX_ENABLE;
	for(i=0;i<bytes_size;i++)
	{
		overtime=0;
		USART1->DR = send_buff[i];
		while((USART1->SR&0x40)==0)
		{
			if(overtime>UART_SEND_OVERTIME)
				return 0;
			overtime++;
		}
	}
    RS485_RX_ENABLE;
	return 1;
}

#endif


void SerialPutString(unsigned char *Sent_value,int lenghtbuf)
{
    int k ;
//	RS485TX_Enable();
	 for (k=0;k<lenghtbuf;k++)   // 发回上位机
	     {
	       USART_SendData(USART1, *(Sent_value+k));
		   while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		  // delay_half_ms(12);
		 }
//      delay_half_ms(20);
//	 RS485TX_DisEnable() ;
   
}

void timer0_initial(void)
{
// TCCR0 = 0x00; //stop timer0
// TCNT0 = 0x83; //set count
// TIMSK |= 0x01; //timer interrupt sources
// TCCR0 = 0x06; //start timer0
	BSP_Timer2_Init();
}


//////////////////////////////////////////////////////////////
//TIMER3 initialisation - prescale:64
// WGM: 0) Normal, TOP=0xFFFF
// desired value: 128uSec
// actual value: 128.000uSec (0.0%)
void timer3_init(void)
{	 
 /*TCCR3B = 0x00; //stop
 TCNT3H = 0xFF; //setup
 TCNT3L = 0xa0;
 TCCR3A = 0x00;          //这里不启动定时器
// TCCR3B = 0x02;*/
	BSP_Timer5_Init();
}



void timer0_int(void)////2 ms timer
{

}

void read_protect(void)
{
	if(FLASH_GetReadOutProtectionStatus() != SET)
	{
	       FLASH_Unlock();//不解锁FALSH可能会导致无法正常设置读保护
	       FLASH_ReadOutProtection(ENABLE);
	}
}









/**
 * This function will initial STM32 board.
 */
void rt_hw_board_init(void)
{
    /* NVIC Configuration */
    NVIC_Configuration();

    /* Configure the SysTick */
    SysTick_Config( SystemCoreClock / RT_TICK_PER_SECOND );

#if STM32_EXT_SRAM
    EXT_SRAM_Configuration();
#endif

    rt_hw_usart_init();
#ifdef RT_USING_CONSOLE
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

    ports_initial();

}

/*@}*/
