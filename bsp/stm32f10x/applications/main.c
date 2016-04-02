#include "includes.h"
#include "stdlib.h"
#include    <math.h>

#include "Protocol.h"
#include "Queue.h"
#include "motorconfig.h"
#include "an41908a.h"


void timer0_int(void);
void timer0_initial(void);
void timer3_init(void);
u8 zf_key_handle(void);

u8 af_action_cnt = 0;
u8 af_action_onepush_cnt = 0;

#define CAMERA_POWER_PIN    GPIO_Pin_3
#define CAMERA_SET_KEY_PIN    GPIO_Pin_2


void camera_power_key_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = CAMERA_SET_KEY_PIN|CAMERA_POWER_PIN;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIOD_InitStructure);


	GPIO_SetBits(GPIOC,CAMERA_POWER_PIN);	
	//GPIO_SetBits(GPIOC,CAMERA_POWER_PIN);	
    
	GPIO_SetBits(GPIOC,CAMERA_SET_KEY_PIN);	

}

void camera_power_off_on(u8 mode)
{
    if(mode)//on
        GPIO_SetBits(GPIOC,CAMERA_POWER_PIN);	
    else
        GPIO_ResetBits(GPIOC,CAMERA_POWER_PIN);	
}

void camera_sensor_format_set_with_long_key(void)
{
    camera_power_off_on(0);
    delay_X1ms(3000);
    GPIO_ResetBits(GPIOC,CAMERA_SET_KEY_PIN);	
    camera_power_off_on(1);
    delay_X1ms(9000);
    GPIO_SetBits(GPIOC,CAMERA_SET_KEY_PIN);

}

void zoom_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = ZOOM_PIN_A1|ZOOM_PIN_A2;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ZOOM_PORT, &GPIOD_InitStructure);	


    GPIO_ResetBits(ZOOM_PORT,ZOOM_PIN_A1);	
    GPIO_ResetBits(ZOOM_PORT,ZOOM_PIN_A2);	

}

u8 iii = 2;

void focus_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = FOCUS_PIN_A1|FOCUS_PIN_A2;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(FOCUS_PORT, &GPIOD_InitStructure);

	GPIO_ResetBits(FOCUS_PORT,FOCUS_PIN_A1);	
	GPIO_ResetBits(FOCUS_PORT,FOCUS_PIN_A2);	


}


void iris_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = IRIS_PIN_PLUS|IRIS_PIN_SUB;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IRIS_PORT, &GPIOD_InitStructure);

	GPIO_ResetBits(IRIS_PORT,IRIS_PIN_PLUS);	
	GPIO_ResetBits(IRIS_PORT,IRIS_PIN_SUB);	


}


void iris_auto_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = IRIS_AUTO_PIN_A1|IRIS_AUTO_PIN_A2|IRIS_AUTO_PIN_B1|IRIS_AUTO_PIN_B2;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IRIS_AUTO_PORT, &GPIOD_InitStructure);

	GPIO_ResetBits(IRIS_AUTO_PORT,IRIS_AUTO_PIN_A1);	
	GPIO_ResetBits(IRIS_AUTO_PORT,IRIS_AUTO_PIN_A2);	
	GPIO_ResetBits(IRIS_AUTO_PORT,IRIS_AUTO_PIN_B1);	
	GPIO_ResetBits(IRIS_AUTO_PORT,IRIS_AUTO_PIN_B2);	

        md127_keepmode();
}



//设置镜头电机的驱动电流，电压
void motor_voltage_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = VOLTAGE_VL3|VOLTAGE_VL2|VOLTAGE_VL1;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIOD_InitStructure);	

	GPIO_SetBits(GPIOB,VOLTAGE_VL3);	
	GPIO_ResetBits(GPIOB,VOLTAGE_VL2);	
	GPIO_ResetBits(GPIOB,VOLTAGE_VL1);	
}

void camera_power_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIOD_InitStructure);	

	GPIO_SetBits(GPIOC,GPIO_Pin_2);	

}

void camera_power_on_off(u8 mode)
{
    if(mode)
        GPIO_SetBits(GPIOC,GPIO_Pin_2);	
    else
        GPIO_ResetBits(GPIOC,GPIO_Pin_2);	   
}


void extern_function_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIOD_InitStructure);	

	GPIO_ResetBits(GPIOB,GPIO_Pin_12);	
	GPIO_ResetBits(GPIOB,GPIO_Pin_13);	

    system_para.system_para.para_ex_io_1_mode = 1;   

    GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIOD_InitStructure);	
    
	GPIO_SetBits(GPIOA,GPIO_Pin_6);	
	GPIO_SetBits(GPIOA,GPIO_Pin_7);	
    system_para.system_para.para_ex_io_2_mode = 1;   
    system_para.system_para.para_ex_io_3_mode = 1;   

}

void key_long_osd_emu(void)
{
    GPIO_SetBits(GPIOB,GPIO_Pin_8);	
    GPIO_SetBits(GPIOB,GPIO_Pin_9);	
    delay_X1ms(4000);   
    GPIO_ResetBits(GPIOB,GPIO_Pin_8);	
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);	
}

void key_init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = KEY_LEFT_PIN|KEY_RIGHT_PIN|KEY_UP_PIN|KEY_DOWN_PIN;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY_PORT, &GPIOD_InitStructure);	
}


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

void serial_int1_send(void)	   //send data to USAR1		   
{
    if (cam_counter_send<cam_cmd_lenth)
    {
    	USART_SendData(USART2, txd1_buffer[cam_counter_send-1]);
    	USART_ClearITPendingBit(USART2, USART_IT_TXE);
    	cam_counter_send++; 
    }
    else
    {
    	USART_SendData(USART2, txd1_buffer[cam_counter_send-1]);
    	USART_ClearITPendingBit(USART2, USART_IT_TXE);
    	cam_counter_send=1;
    	txd1_buff_cFlag=1;
    	USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
    }
	if (cam_counter_send>=200)
		cam_counter_send=1; 
}

u8 cam_cmd_recieve_len = 0;
u8 cam_recieve_cnt_public=0;
void serial_int1_receive(uchar udr1)//receive data from USAR1
{
     
    rxd1_buffer[counter_receive] = udr1;

    if(counter_receive == 3)
    {
        cam_recieve_cnt_public=0;
        if(rxd1_buffer[2]==0x20)
        {
            cam_cmd_recieve_len = rxd1_buffer[counter_receive]+6;
        }
        else if(rxd1_buffer[2]==0x28)
        {
            cam_cmd_recieve_len = 7;
        }
        else
            cam_cmd_recieve_len = rxd1_buffer[counter_receive]+6;

		if(rxd1_buffer[2] == 0x10)
			cam_cmd_recieve_len--;
    }
    
   	counter_receive++;

   
    if (counter_receive >= cam_cmd_recieve_len && counter_receive>3)
   //if (counter_receive >= 6 && counter_receive>3)
   {
        cam_recieve_cnt_public = cam_cmd_recieve_len;
    	rxd1_buff_cFlag=1;
    	counter_receive=0;
        cam_cmd_recieve_len=0;
    }

    if (counter_receive >= 200)
    	counter_receive = 0;
}

// control the camera
void uart_to_camera_init(void)
{
    BSP_USART2_Init();
    APP_USART2_Baudrate(19200);
    
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);// Enable USART2 receive intterrupt 
    USART_Cmd(USART2, ENABLE); // Enable USART2

}

//相机泪光片设置
void cam_filt_cutter_int(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIOD_InitStructure);	


    GPIO_ResetBits(GPIOB,GPIO_Pin_12);	
    GPIO_ResetBits(GPIOB,GPIO_Pin_13);	

}

void cam_filt_cutter_set(u8 mode)
{
    if(mode)
    {// fog
        GPIO_SetBits(GPIOB,GPIO_Pin_13);	

    }
    else
    {// ir

        GPIO_ResetBits(GPIOB,GPIO_Pin_13);	

    }

}

void ports_initial(void)
{
	BSP_Init();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	// 改变指定管脚的映射 GPIO_Remap_SWJ_Disable SWJ 完全禁用（JTAG+SW-DP）
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
	// 改变指定管脚的映射 GPIO_Remap_SWJ_JTAGDisable ，JTAG-DP 禁用 + SW-DP 使能


    //DC Motor driver ic: BA6208
	zoom_pin_Init();
	focus_pin_Init();
    iris_pin_Init();
    
	extern_function_pin_Init();
    //camera_power_pin_Init();
    motor_voltage_pin_Init();
    //iris_auto_pin_Init();
    
	camera_power_key_pin_Init();
    
	timer0_initial();


//调试UART4 PC10,PC11，暂时屏蔽按键功能 
	//key_init();


	
    uart_to_camera_init();

	timer3_init();
	timer1_init();

    I2C_AT24C512_Init();



    an41908a_spi_pin_init();
	
}

/*******************************************************************************
* Function Name  : InterruptConfig
* Description    : Configures the used IRQ Channels and sets their priority.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InterruptConfig(void)
{ 
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;

  /* Deinitializes the NVIC */
  NVIC_DeInit();
	#ifdef  VECT_TAB_RAM  
  		/* Set the Vector Table base location at 0x20000000 */
  		NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else  /* VECT_TAB_FLASH */
  		/* Set the Vector Table base location at 0x08000000 */
  		NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
	#endif

  /* Configure the Priority Group to 2 bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  /* Configure the SysTick handler priority */
  NVIC_SystemHandlerPriorityConfig(SystemHandler_SysTick, 0, 0);
  
  /* Enable the EXTI9_5 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the EXTI15_10 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the TIM2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the USB_LP_CAN_RX0 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN_RX0_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the USB_HP_CAN_TX Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN_TX_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the TIM1 UP Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the RTC Alarm Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the EXTI Line17 Interrupt */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
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

u16 key_check_period_cnt = 0;

void timer0_int(void)////2 ms timer
{
	if(key_check_period_cnt>0)
		key_check_period_cnt--;

    if(af_action_cnt < 0xff)
        af_action_cnt++;

    if(af_action_onepush_cnt < 0xff)
        af_action_onepush_cnt++;
}

void read_protect(void)
{
	if(FLASH_GetReadOutProtectionStatus() != SET)
	{
	       FLASH_Unlock();//不解锁FALSH可能会导致无法正常设置读保护
	       FLASH_ReadOutProtection(ENABLE);
	}
}



u8 key_get(u16 *data)
{
	u16 temp = 0;
	u8 ret = 0;
	temp = (GPIO_ReadInputData(KEY_PORT)>>4&0x000f);
	if(temp)
	{
		delayms(3);
		if(temp == (GPIO_ReadInputData(KEY_PORT)>>4)&0x000f)
		{
			*data = temp;
			ret = 1;
		}
	}
	return ret;
}

u16 KeyPre = 0;//  上次按键的状态
u16 KeyCurrent = 0;//  本次按键的状态
u16 KeyStbLast = 0;// 经滤波后的上次按键的状态
u16 KeyStb = 0;//  经滤波后的本次按键的状态
u16 KeyAvlUp = 0;// 按键抬起有效结果寄存器
u16 KeyAvlDown = 0;//  按键按下有效结果寄存器

#define	KEY_DOWN_STATE		0

u16 key_val_table[] = {KEY_LEFT_PIN,KEY_RIGHT_PIN,KEY_UP_PIN,KEY_DOWN_PIN,KEY_SET_PIN};


// 程序的用法：每隔10MS 或者 20MS 调用一下这个程序即可
#if 1
u8 CheckKey(void)
{
	u16 KTmp1 = 0,KTmp2 = 0 ;
	u16 KeyStatu = 0;
	u16 val = 0;
	u8 i =0;
		
	//if(!key_check_period_cnt)	
	if(1)
	{
		val = GPIO_ReadInputData(KEY_PORT);

		//val = val>>8;
		val &= 0x1f00;

		KeyStatu = val ;	

		KeyPre = KeyCurrent ;  //保存上次按键状态
		KeyCurrent = KeyStatu ;   //保存当前按键状态

		delayms(10);

		val = GPIO_ReadInputData(KEY_PORT);

		//val = val>>8;
		val &= 0x1f00;	

		KeyCurrent = val ;
		

		key_check_period_cnt = 8;
		return 1;
	}

	return 0;
}


#else
void CheckKey(void)
{
	u16 KTmp1 = 0,KTmp2 = 0 ;
	u16 KeyStatu = 0;
	u16 val = 0;

	if(!key_check_period_cnt)	
	{
		val = GPIO_ReadInputData(KEY_PORT);

		//val = val>>8;
		val &= 0x1f00;

		KeyStatu = val ;	

		KeyPre = KeyCurrent ;  //保存上次按键状态
		KeyCurrent = KeyStatu ;   //保存当前按键状态
			
		KTmp1 = (KeyCurrent | KeyPre) ; //00 -> 0 取滤波值0
		KTmp2 = (KeyCurrent & KeyPre) ; //11 -> 1 取滤波值1
		
		KeyStbLast = KeyStb ;	//保存上次经滤波后的值	
		KeyStb &= KTmp1 ; //赋值当前经滤波后的值
		KeyStb |= KTmp2 ;

		//去抖后的当前按键状态 1有效
		KeyAvlUp = (KeyStb ^ KeyStbLast) & KeyStbLast; //上升沿按下标志 获取1至0的状态	
		KeyAvlDown = (KeyStb ^ KeyStbLast) & KeyStb ;	//下降沿按下标志 获取0至1的状态		

		key_check_period_cnt = 8;
	}
	
}

#endif


//按键处理程序 处理第一个按键 如果你想按键按下有效
u8 CmdK1DOWN(u16 key)
{
	if((KeyAvlDown & key) == 0) 
		return 0;
	KeyAvlDown &= ~key ;
	return 1;
	//处理按键内容
}

//按键处理程序 处理第一个按键 如果你想按键抬起来有效
u8 CmdK1UP(u16 key)
{
	if((KeyAvlUp & key) == 0) 
		return 0;
	KeyAvlUp &= ~key ;

	return 1;
	//处理按键内容
}

volatile enum ZF_TYPE zoom_state_cur = ALL_STOP;
volatile enum ZF_TYPE focus_state_cur = ALL_STOP;
volatile enum ZF_TYPE zf_state_cur = ALL_STOP;

u8 key_zf_convert(void)
{
	if(CmdK1DOWN(FOCUS_PLUS_PIN))
		focus_state_cur = FOCUS_FAR;

	if(CmdK1DOWN(FOCUS_SUB_PIN))
		focus_state_cur = FOCUS_NEAR;

	if(CmdK1DOWN(ZOOM_PLUS_PIN))
		zoom_state_cur = ZOOM_TELE;

	if(CmdK1DOWN(ZOOM_SUB_PIN))
		zoom_state_cur = ZOOM_WIDE;

//-----------------------------------------
	if(CmdK1UP(FOCUS_PLUS_PIN))
		focus_state_cur = FOCUS_STOP;

	if(CmdK1UP(FOCUS_SUB_PIN))
		focus_state_cur = FOCUS_STOP;

	if(CmdK1UP(ZOOM_PLUS_PIN))
		zoom_state_cur = ZOOM_STOP;

	if(CmdK1UP(ZOOM_SUB_PIN))
		zoom_state_cur = ZOOM_STOP;

	return 1;
}

u8 key_monitor(void)
{
	u16 key_tmp;
	
	if(CheckKey())
	{

		key_tmp = KeyCurrent;
		
		return 1;

	}

	else
		return 0;
}


#define		KEY_DEFAULT		0X1F00
void keyscan(void)// 键盘扫描函数
{ 
uchar xx;//局部变量
u16 val;

val = GPIO_ReadInputData(KEY_PORT);
val &= 0x1f00;

	if(val!=KEY_DEFAULT) 
	{ 
		delayms(10); 
		if(val!=KEY_DEFAULT) 
		{ 
			
			val = GPIO_ReadInputData(KEY_PORT);
			val &= KEY_DEFAULT;
			
			switch(val) //根据按键状态，调用不同的子函数
			{ 
			case 0xfe:

			break; 

			case 0xfd: 
			break; 
			default:break; 
			 
			} 
		} 


	} 

}






u8 key_check(u16 key)
{
	u16 key_tmp;
	u8 i=0;
	
	if(CheckKey())
	{
		for(i=0;i<5;)
		{

			if(key_val_table[i] == key)
				break;

			i++;
		}
		
		if(KeyCurrent & key)
		{

			if(i < 2)
			{	
				zf_state_cur = FOCUS_STOP;
				focus_state_cur = FOCUS_STOP;
			}
			else
			{
				zf_state_cur = ZOOM_STOP;
				zoom_state_cur = ZOOM_STOP;
				}

		}
		else
		{
			
			switch(i)
				{
			case 0:

				zf_state_cur = FOCUS_FAR;
				focus_state_cur = zf_state_cur;
				break;
			case 1:
				zf_state_cur = FOCUS_NEAR;
				focus_state_cur = zf_state_cur;
				break;
			case 2:
				zf_state_cur = ZOOM_TELE;
				zoom_state_cur = zf_state_cur;
				break;
			case 3:
				zf_state_cur = ZOOM_WIDE;
				zoom_state_cur = zf_state_cur;
				break;
			default:
				zf_state_cur = ALL_STOP;
				zoom_state_cur = zf_state_cur;
				focus_state_cur = zf_state_cur;
				break;
			}

			

		}
		
		return 1;

	}

	else
		return 1;


}

void zoom_focus_handle(void)
{
u8 i;

	for(i=0;i<5;i++)
	{
		if( !key_check(key_val_table[i]))
			continue;
		
		if(i<2 )
		{
			
			if(zf_state_cur == FOCUS_NEAR || zf_state_cur == FOCUS_FAR)
				break;
		}
		else
		{
			if(focus_state_cur == FOCUS_STOP && zoom_state_cur == ZOOM_STOP)
				zf_state_cur = ALL_STOP;
				
			if(zf_state_cur == ZOOM_WIDE || zf_state_cur == ZOOM_TELE)
			{

				zoom_run_state = 1;

				break;

			}

			
		}
	}

	if(zoom_state_cur == ZOOM_STOP)
	{

		zoom_run_state = 0;
	}

	
	focus_action(zf_state_cur);
	zoom_action(zf_state_cur);
	
}


u8 zf_key_handle(void)
{
	u8 i;

	static enum ZF_TYPE zf_state_cur_pre = ALL_STOP;

	
	zf_state_cur_pre = zf_state_cur;
	
	for(i=0;i<5;i++)
	{
		if( !key_check(key_val_table[i]))
			continue;
		
		if(i<2 )
		{
			
			if(zf_state_cur == FOCUS_NEAR || zf_state_cur == FOCUS_FAR)
				break;
		}
		else
		{
			if(focus_state_cur == FOCUS_STOP && zoom_state_cur == ZOOM_STOP)
				zf_state_cur = ALL_STOP;
				
			if(zf_state_cur == ZOOM_WIDE || zf_state_cur == ZOOM_TELE)
			{

				zoom_run_state = 1;

				break;

			}

			
		}
	}

	i = 0x00;
	
	if(zf_state_cur_pre != zf_state_cur)
	{	
		i = 0x01;

		if(zf_state_cur == FOCUS_STOP || zf_state_cur == ZOOM_STOP || zf_state_cur == ALL_STOP)
			{
			command_byte = 5;
			}
		else if(zf_state_cur == ZOOM_TELE)
			{
				command_byte = 1;

		}
		else if(zf_state_cur == ZOOM_WIDE)
		{
			command_byte = 2;

		}
		else if(zf_state_cur == FOCUS_FAR)
		{
			command_byte = 3;

		}
		else if(zf_state_cur == FOCUS_NEAR)
		{
			command_byte = 4;

		}
	}

	return i;

}


u8 zf_key_osd_handle(void)
{
	u8 i;

	static enum ZF_TYPE zf_state_cur_pre = ALL_STOP;

	
	zf_state_cur_pre = zf_state_cur;
	
	for(i=0;i<5;i++)
	{
		if( !key_check(key_val_table[i]))
			continue;
		
		if(i<2 )
		{
			
			if(zf_state_cur == FOCUS_NEAR || zf_state_cur == FOCUS_FAR)
				break;
		}
		else
		{
			if(focus_state_cur == FOCUS_STOP && zoom_state_cur == ZOOM_STOP)
				zf_state_cur = ALL_STOP;
				
			if(zf_state_cur == ZOOM_WIDE || zf_state_cur == ZOOM_TELE)
			{

				zoom_run_state = 1;

				break;

			}

			
		}
	}

	i = 0x00;
	
	if(zf_state_cur_pre != zf_state_cur)
	{	
		i = 0x01;

		if(zf_state_cur == FOCUS_STOP || zf_state_cur == ZOOM_STOP || zf_state_cur == ALL_STOP)
			{
			command_byte = 5;
			}
		else if(zf_state_cur == ZOOM_TELE)
			{
				command_byte = 6;

		}
		else if(zf_state_cur == ZOOM_WIDE)
		{
			command_byte = 7;

		}
		else if(zf_state_cur == FOCUS_FAR)
		{
			command_byte = 8;

		}
		else if(zf_state_cur == FOCUS_NEAR)
		{
			command_byte = 9;

		}
	}

	return i;

}



void pt_action(uchar action,uchar pan_speed,uchar tilt_speed) //action:0~7 ->left,right....; action=8 means stop command
{ 
    com_buffer[1] = pt_code2motor[action<<1];
    com_buffer[3] = pt_code2motor[(action<<1)+1];
	com_buffer[0] = 0x55; 								//自学习功能
	com_buffer[2] = pan_speed;
	com_buffer[4] = tilt_speed;
    com_buffer[5] = 0;

    if(action<=1)
    	MotorControll(); 
    else if(action <= 3)
        focus_MotorControll();
        
}

void pt_action_tilt(uchar action,uchar tilt_speed) //action:0~7 ->left,right....; action=8 means stop command
{ 
    com_buffer_tilt[1] = pt_code2motor[action<<1];
    com_buffer_tilt[3] = pt_code2motor[(action<<1)+1];
	com_buffer_tilt[0] = 0x55; 								//自学习功能
	com_buffer_tilt[4] = tilt_speed;
    com_buffer_tilt[5] = 0;

    focus_MotorControll();
        
}


void ptz_execute(void)
{
    uchar temp;

    switch(command_byte)
	{
    case 0x01://zoom
    case 0x02:	
        temp = command_byte - 1; 
        pt_action(temp,1,1);
        zoom_run_state = 1;
    break;
    
    case 0x03://focus
    case 0x04:	
        temp = command_byte - 1; 
        pt_action_tilt(temp,1);
        focus_run_state = 1;
    break;
    case 0x05:  
        MotorStop();
		jiguang_zoom_state = 0;
    break;

    default:break;
    }
}

uchar prePoint_num_Function(uchar point)//let preset point be in 1~128 continually, or it returns a value 0
{
    #if 0
	if(point==0)
		return 0;

	if ((point >= 79) && (point <= 101) ) 
    	return 0;				

	if ((point >= 40) && (point <= 70) ) 
    	return 0;				

	if ((point >= 10) && (point <= 20) ) 
    	return 0;	
	

    return 1;
    #else
    return 0;
    #endif
}

#define FOCUS_LEVEL_1       580
#define FOCUS_LEVEL_2       770
#define FOCUS_LEVEL_3       900
#define FOCUS_LEVEL_4       960
#define FOCUS_LEVEL_5       1000
#define FOCUS_LEVEL_6       1030
#define FOCUS_LEVEL_7       1000


#define		AUTO_ZF_ZERO_POS_STEPS		(double)((double)16.24/ZOOM_LEN_PER_STEP)

void auto_focus_onepush(void)
{
    s32 temp = 0;
    double focus_temp = 0;
    
    	
	temp = panpos;

	if(temp >= 2044)
		temp = 2043;
	
	if(temp < 0)
		temp = 0;

	focus_temp = focus_pos_table[temp];
    focus_temp = focus_temp/FOCUS_LEN_PER_MOTOR_STEP;
    
#if 0

	if(focus_temp<=0)
		temp = fabs(focus_temp) + FOCUS_SENSOR_POS_SENSOR_TO_FAR_STEPS;
	else
		temp = FOCUS_SENSOR_POS_SENSOR_TO_FAR_STEPS - focus_temp;

    
    
#else
	if(focus_temp<=0)
		temp = fabs(focus_temp) + FOCUS_ZERO_TO_FAR_END_STEPS;
	else
		temp = FOCUS_ZERO_TO_FAR_END_STEPS - focus_temp;
#endif	
    focus_goto_pos_absolutely(temp,0,0);    
}

#define     FOCUS_TABLE_SIZE     sizeof(focus_pos_table)/sizeof(double)
#if 1
void auto_focus_track_zoom(void)
{
    s32 temp = 0;
    double focus_temp = 0;
    
   
    
    if(af_action_cnt > 20)
    {        
        
        if(!focus_run_state && zoom_run_state)
        {
    			
    			temp = panpos;

    			if(temp >= FOCUS_TABLE_SIZE)
    				temp = FOCUS_TABLE_SIZE-1;
    			
    			if(temp < 0)
    				temp = 0;

    			focus_temp = focus_pos_table[temp];
                
                focus_temp = focus_temp/FOCUS_LEN_PER_MOTOR_STEP; 
    			//focus_temp *= 8;
                
    			if(focus_temp<=0)
    				temp = fabs(focus_temp) + FOCUS_ZERO_TO_FAR_END_STEPS;
            	else
    				temp = FOCUS_ZERO_TO_FAR_END_STEPS - focus_temp;

               
            focus_goto_pos_absolutely(temp,0,0);    

            
        }

        af_action_cnt = 0;
    }

     if(af_action_onepush_cnt > 50)
    {
        if((zoom_run_state_pre == 1 && !zoom_run_state) || zoom_pos_limit_state)
        {
            zoom_pos_limit_state = 0;
            
            temp = panpos;
			if(temp >= FOCUS_TABLE_SIZE)
				temp = FOCUS_TABLE_SIZE-1;			
			if(temp < 0)
				temp = 0;
			focus_temp = focus_pos_table[temp];
            //focus_temp *= 8;

            focus_temp = focus_temp/FOCUS_LEN_PER_MOTOR_STEP;
            //focus_temp = fabs(focus_temp);

			if(focus_temp<=0)
				temp = fabs(focus_temp) + FOCUS_ZERO_TO_FAR_END_STEPS;
        	else
				temp = FOCUS_ZERO_TO_FAR_END_STEPS - focus_temp;   	

            
            
            focus_goto_pos_absolutely(temp,0,0);   

            zoom_run_state_pre = zoom_run_state;

            return;
        }
        af_action_onepush_cnt = 0;
    }    
    
}


#else
void auto_focus_track_zoom(void)
{
    s32 temp = 0;
    
    if(!focus_run_state)
    {
        if(panpos<FOCUS_LEVEL_1)
            temp = panpos;
        else if(panpos < FOCUS_LEVEL_2)
        {
            temp = FOCUS_LEVEL_1 + (panpos - FOCUS_LEVEL_1)/2;
        }
        else if(panpos < FOCUS_LEVEL_3)
        {
            temp = FOCUS_LEVEL_1 + (FOCUS_LEVEL_2 - FOCUS_LEVEL_1)/2 + (panpos - FOCUS_LEVEL_2)/3;
        }
        else if(panpos < FOCUS_LEVEL_4)
        {
            temp = FOCUS_LEVEL_1 + (FOCUS_LEVEL_2 - FOCUS_LEVEL_1)/2 + (FOCUS_LEVEL_3 - FOCUS_LEVEL_2)/3 + (panpos- FOCUS_LEVEL_3)/4;
        }  
        else if(panpos < FOCUS_LEVEL_5)
        {
            temp = FOCUS_LEVEL_1 + (FOCUS_LEVEL_2 - FOCUS_LEVEL_1)/2 + (FOCUS_LEVEL_3 - FOCUS_LEVEL_2)/3 + (FOCUS_LEVEL_3 - FOCUS_LEVEL_2)/4 + (panpos- FOCUS_LEVEL_4)/8;
        } 
        else if(panpos < FOCUS_LEVEL_6)
        {
            temp = FOCUS_LEVEL_1 + (FOCUS_LEVEL_2 - FOCUS_LEVEL_1)/2 + (FOCUS_LEVEL_3 - FOCUS_LEVEL_2)/3 + (FOCUS_LEVEL_3 - FOCUS_LEVEL_2)/4 - (panpos- FOCUS_LEVEL_4)/8;
        }  
        
        focus_goto_pos_absolutely(temp,0,0);    
    }

}

#endif

//u8 ttest[2035];
//double myd = 0.00078125;
u16 ttlen;
//#define VECT_TAB_RAM
extern void delay1Ms(int ms);
int main(void)
{	
    u16 k;
	
	InterruptConfig();
	ports_initial();
    delay_X1ms(60);
 system_para.system_para.para_system_para_flag1 = 0x12; 
 system_para.system_para.para_system_para_flag2 = 0x34;
 
     //for(k=0;k<2030;k++)
     //    eeprom_byte_write(k+2000, 0x58);
     ////
     //for(k=0;k<2030;k++)
     //    ttest[k] = eeprom_byte_read(k+2000);

    uart_to_camera_init();  
//    cam_get_ID(0);
////cam_zoom_mode_set(ON_MODE);

//    get_domeID();	
//    get_baudrate();

//    //load_system_para();
//    cam_get_ID(0);
    
	jiguang_comm_init();


	Protocol_No = 0;

	uart0_init();
	//cam_init();

//delay_X1ms(9000);
	
	System_data_init();
    auto_focus_onepush();

    LensDrvInit();

    an41908_check_work_state();
    
an41908_brake_set(1);
        delay_X1ms(500);

an41908_brake_set(0);

    ttlen = LensDrvSPIReadUint( 0x25 );
    ttlen = LensDrvSPIReadUint( 0x20 );

    while(1)
    {
        LenDrvZoomMove(1,200);
LensDrvFocusMove(1,100);
        delay_X1ms(2000);
        //LenDrvZoomMove(0,200);
        //LensDrvFocusMove(0,100);

        //delay_X1ms(2000);
    }

	while(1)
	{
		//if (command_analysis() || zf_key_handle()) 
		//if (zf_key_handle()) 

        //return_cmd_to_camlink(0xcc,8);
        //delay_half_second(1);
        
		if (command_analysis()) 
		{
			if(command_byte != 0x0e && command_byte!=0x0f)
			{
				jiguang_zoom_state = 0;


			}
			ptz_execute();
            switch(command_byte)
		    {
			 	case 0x0E://open iris
					switch(iris_ctl_mode)
					{
					case IRIS_JIGUANG_ZOOM:
						
						jiguang_zoom_state = 1;
						
						break;
					
					case IRIS_JIGUANG_CURRENT:
						jiguang_current_set(1);
						break;

					case IRIS_CAM_IRIS:
						iris_motor_plus();
						break;

					default:
						break;
					}								 

					break;
			 	case 0x0F://close iris
			 		switch(iris_ctl_mode)
		 			{
					case IRIS_JIGUANG_ZOOM:
						jiguang_zoom_state = 2;
						break;
					
					case IRIS_JIGUANG_CURRENT:
						jiguang_current_set(0);
						break;

					case IRIS_CAM_IRIS:
						iris_motor_sub();
						break;

					default:
						break;
					}
			 	    break;
			 
			 	case 0x10://set preset point
					k = prePoint_num_Function(Rocket_fir_data); //return 0 or 1~128 preset point
					if (k)
					{

					}
					else           //function preset point
						dome_func_control(command_byte,Rocket_fir_data);
					break;
			 
			 	case 0x11://call preset point
					k = prePoint_num_Function(Rocket_fir_data); //return 0 or 1~128 preset point
					if (k)
                    {
                    
                    }               
					else           //function preset point
						dome_func_control(command_byte,Rocket_fir_data);
					break;
			 	case 0x90:
                    iris_ctl_mode = IRIS_CAM_OSD_ENTER;
                	osdSwitchFlag = 0;
                	//cam_osd_control(1);
        			//camera_osd_switch(0);
                    key_long_osd_emu();
                    //camera_osd_key_emulate(OSD_ENTER);
                    delay_half_ms(20);
                    break;
             	default:
				break;
	   	    }

			cam_osd_control(1);
			
		}

		//zoom_focus_handle();

		jiguang_zoom_handle(jiguang_zoom_state);

		//auto_focus_track_zoom();
			
        //key_monitor();
	}
}


