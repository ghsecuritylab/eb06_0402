#ifndef _BSP_H_
#define _BSP_H_

/*Includes------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"
#include "api_type.h"


/* 调用外部变量*/
extern uchar man;
extern uchar cameraType;


//#define uchar unsigned char
//#define uint unsigned short

/*Defines**************************************************************/


#define	SYSTEM_CPU_CLOCK_FREQUENCE_8M	1
#define	SYSTEM_CPU_CLOCK_FREQUENCE_16M	2
#define	SYSTEM_CPU_CLOCK_FREQUENCE_24M	3
#define	SYSTEM_CPU_CLOCK_FREQUENCE_32M	4
#define	SYSTEM_CPU_CLOCK_FREQUENCE_40M	5
#define	SYSTEM_CPU_CLOCK_FREQUENCE_48M	6
#define	SYSTEM_CPU_CLOCK_FREQUENCE_56M	7
#define	SYSTEM_CPU_CLOCK_FREQUENCE_64M	8
#define	SYSTEM_CPU_CLOCK_FREQUENCE_72M	9

#define	SYSTEM_CPU_CLOCK_FREQUENCE	SYSTEM_CPU_CLOCK_FREQUENCE_72M

//8M 外部晶振
#define	EXTERNAL_OSC_FREQUENCE		(double)8000000	

//单位为HZ 表示定时器的时钟频率,定时器时钟总线频率为系统时钟频率的1/2
#define	TIMER_CLOCK_FREQUENCE	(double)(EXTERNAL_OSC_FREQUENCE/2*SYSTEM_CPU_CLOCK_FREQUENCE) 


//--定时器6，用于处理红外
#define	TIMER6_PRESCALER_VALUE	(double)10000
#define	TIMER6_PRESCALER_DATA	(TIMER6_PRESCALER_VALUE - 1) //此值用于赋给定时器的寄存器，必须减1
#define	TIMER6_INT_FREQUENCE	(double)10	//定时6中断频率为10HZ，即每100ms中断一次
#define	TIMER6_PERIOD_VALUE		(double)(TIMER_CLOCK_FREQUENCE/TIMER6_PRESCALER_VALUE/TIMER6_INT_FREQUENCE)

//---定时器2，用于球机定时处理
#define	TIMER2_PRESCALER_VALUE	(double)2000	//分频值
#define	TIMER2_PRESCALER_DATA	(TIMER2_PRESCALER_VALUE - 1) //此值用于赋给定时器的寄存器，必须减1
#define	TIMER2_INT_FREQUENCE	(double)500	//定时2中断频率为500HZ，即每2ms中断一次
#define	TIMER2_PERIOD_VALUE		(double)(TIMER_CLOCK_FREQUENCE/TIMER2_PRESCALER_VALUE/TIMER2_INT_FREQUENCE)

/*水平马达定时器*/
#define	TIM5_PRESCALER_VALUE	(u16)(TIMER_CLOCK_FREQUENCE/250000-1)	//


/*垂直马达定时器*/
#define	TIM3_PRESCALER_VALUE	(u16)(TIMER_CLOCK_FREQUENCE/250000-1)	//


/*自学习定时器*/
#define	TIM4_PRESCALER_VALUE	(u16)(TIMER_CLOCK_FREQUENCE/EXTERNAL_OSC_FREQUENCE*64-1)	//


/*--------------------------------------------------------------------*/
/* Protocol */
#define ProtocolIN1 GPIO_Pin_8	   	/* PE8 */
#define ProtocolIN2 GPIO_Pin_9		/* PE9 */
#define ProtocolIN3 GPIO_Pin_10		/* PE10 */
#define ProtocolIN4 GPIO_Pin_11		/* PE11 */
/*--------------------------------------------------------------------*/
/* Baudrate */
#define BaudrateIN1 GPIO_Pin_12		/* PE12 */
#define BaudrateIN2 GPIO_Pin_13		/* PE13 */
/*--------------------------------------------------------------------*/
/* Dome ID number */
#define DomeIDIN8 GPIO_Pin_8		/* PD8 */
#define DomeIDIN7 GPIO_Pin_9        /* PD9 */
#define DomeIDIN6 GPIO_Pin_10       /* PD10 */
#define DomeIDIN5 GPIO_Pin_11		/* PD11 */
#define DomeIDIN4 GPIO_Pin_12       /* PD12 */
#define DomeIDIN3 GPIO_Pin_13       /* PD13 */
#define DomeIDIN2 GPIO_Pin_14       /* PD14 */
#define DomeIDIN1 GPIO_Pin_15		/* PD15 */

/*--------------------------------------------------------------------*/
/* USART1 for rs485 */
#define RS485RXD1 GPIO_Pin_10       /* PA10 */
#define RS485TXD1 GPIO_Pin_9		/* PA9 */


#define RS485Control GPIO_Pin_9		/* PB9 */

#define	RS485_RX_TX_CTL_PORT	GPIOB	//rs485发送，接收使能引脚控制为PE0
#define	RS485_RX_TX_CTL_PIN		GPIO_Pin_9

#define	RS485_RX_ENABLE		(RS485_RX_TX_CTL_PORT->BRR = RS485_RX_TX_CTL_PIN)

#define	RS485_TX_ENABLE		(RS485_RX_TX_CTL_PORT->BSRR = RS485_RX_TX_CTL_PIN)


/*--------------------------------------------------------------------*/ 
/* USART2 for TTL of the Camera */
#define TTLRXD2 GPIO_Pin_3		/* PA3 */
#define TTLTXD2 GPIO_Pin_2		/* PA2 */
/*--------------------------------------------------------------------*/ 
/* Motor */
#define MotorPanDirection		GPIO_Pin_6		/* PC6 */
#define MotorPanStep			GPIO_Pin_7		/* PC7 */
#define MotorTiltDirection		GPIO_Pin_8		/* PC8 */
#define MotorTiltStep			GPIO_Pin_9		/* PC9 */
#define MotorActionEnable		GPIO_Pin_8		/* PA8 */

#define MotorLightSwitch		GPIO_Pin_7		/* PD7 */
#define MotorLightSwitch_TILT		GPIO_Pin_6		/* PD6 */

/* DAC1 */
#define MotorPanReference_DAC_OUT1		GPIO_Pin_4		/* PA4 */

#define DAC_DHR8R1_Address      0x40007410
/* DAC2 */
#define MotorTileReference_DAC_OUT2		GPIO_Pin_5		/* PA5 */
/*--------------------------------------------------------------------*/
/* I2C2 bus storage */
#define Storage_I2C2_SCL   GPIO_Pin_10       /* PB10 */
#define Storage_I2C2_SDA   GPIO_Pin_11       /* PB11 */

#define I2C1_SLAVE_ADDRESS7   0x30
#define I2C2_SLAVE_ADDRESS7   0x30
//#define BufferSize            4
#define ClockSpeed            200000
/*--------------------------------------------------------------------*/ 
/* OSD */
#define OSD_CS			GPIO_Pin_5			/* PB5 */  
#define OSD_NTSC_OR_PAL GPIO_Pin_8			/* PB8 */
#define OSD_GD			GPIO_Pin_7			/* PD7 */

/* I2C1 bus for communication */
#define OSD_I2C1_SCL	GPIO_Pin_6			/* PB6 */
#define OSD_I2C1_SDA 	GPIO_Pin_7			/* PB7 */

#define OSD_CS_H        GPIOB->BSRR = GPIO_Pin_5		
#define OSD_CS_L        GPIOB->BRR  = GPIO_Pin_5 		
   
#define OSD_SIN_H       GPIOB->BSRR = GPIO_Pin_7 		
#define OSD_SIN_L       GPIOB->BRR  = GPIO_Pin_7 			

#define OSD_SLK_H      	GPIOB->BSRR = GPIO_Pin_6		
#define OSD_SLK_L      	GPIOB->BRR  = GPIO_Pin_6




/*Functions************************************************************/
/* initialize system clocks */
void BSP_RCC_Init(void);

/* initialize the NVIC */
void BSP_NVIC_Init(void);

/* Configure a SysTick Base time to 10 ms. */
void BSP_SysTick_Init(void);

/* all of the initializations of the dome------------------------------*/
void BSP_Init(void);

/* Initialize the protocol -------------------------------------------*/
void BSP_Protocol_Init(void);

/* Initialize the baudrate--------------------------------------------*/
void BSP_Baudrate_Init(void);

/* Initialize the dome ID---------------------------------------------*/
void BSP_DomeID_Init(void);

//初始化报警处理相关引脚，包括报警输入和报警输出
void BSP_Alarm_Init(void);

/* Initialize the USART1----------------------------------------------*/
void BSP_USART1_Init(void);

/* Initialize the USART2----------------------------------------------*/
void BSP_USART2_Init(void);

/* Initialize the Motor-----------------------------------------------*/
void BSP_Motor_Init(void);
void BSP_PhotoSwitch_Init(void);
void BSP_DAC1_Init(void);
void BSP_DAC2_Init(void);

/* Initialize the storage------------------------------------*/
void BSP_Storage_Init(void);
void BSP_I2C2_Init(void);

/* Initialize the OSD to control the display on the camera------------*/
void BSP_OSD_Init(void);
void BSP_I2C1_Init(void);

/* Initialzie the Timer1~Timer4----------------------------------------*/
void BSP_Timer2_Init(void);

void BSP_Timer3_Init(void);

void BSP_Timer4_Init(void);

void BSP_Timer5_Init(void);

void BSP_Test_Init(void);

/************************************************************************/
/* Retargets the C library printf function to the USART. */
 int fputc(int ch, FILE *f);

/* Inserts a delay time. (time base 10 ms) */
 void Delay(u32 nCount);

/* Decrements the TimingDelay variable. */
 void Decrement_TimingDelay(void);
/************************************************************************/
void BSP_DelayTime(INT32U nCount);
void BSP_DelayMS(INT32U nCount);



/************************************************************************/
/* APP IRQHandle */
extern void uart0_rx_isr(uchar udr0);
extern void serial_int1_receive(uchar udr1);
extern void serial_int1_send(void);
extern void timer0_int(void);
extern void timer1_ovf_isr(void);
extern void timer2_ovf_isr(void);
extern void timer3_ovf_isr(void);
extern void BSP_Timer6_Init(void);


void APP_Init(void);

void APP_DelayTime(INT32U nCount);

void APP_USART1_Baudrate(u32 baudrate);

void APP_USART1_IRQHandler(void);

void APP_USART2_Baudrate(INT16U baudrate);

void APP_USART2_IRQHandler(void);

void APP_TIM2_IRQHandler(void);

void APP_TIM3_IRQHandler(void);

void APP_TIM4_IRQHandler(void);

void APP_TIM5_IRQHandler(void);
void APP_TIM6_IRQHandler(void);



/************************************************************************/

#endif

void ports_initial(void);
void BSP_RtcIC_Init(void);
void delay_half_us(void);
void delay_2us(uchar time);
void delay_half_ms(uchar time);//0.5 ms delay 
void delay_half_second(uchar time);

extern void delayms(int n);
void BSP_USART1_Init(void);
void BSP_USART2_Init(void);
void APP_USART1_Baudrate(u32 baudrate);
void APP_USART2_Baudrate(INT16U baudrate);






