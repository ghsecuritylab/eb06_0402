#include <board.h>
#include <rtthread.h>

#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif  /* RT_USING_COMPONENTS_INIT */

#include "includes.h"

void iris_auto_manual_set(u8 mode);
void pelcod_call_pre_packet_send(u8 val);

void led_pin_init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIOD_InitStructure);	

	GPIO_ResetBits(GPIOB,GPIO_Pin_0);	
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);	
	GPIO_ResetBits(GPIOB,GPIO_Pin_2);	
	GPIO_ResetBits(GPIOB,GPIO_Pin_3);	
	GPIO_ResetBits(GPIOB,GPIO_Pin_4);	
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);	
	GPIO_ResetBits(GPIOB,GPIO_Pin_6);	
	GPIO_ResetBits(GPIOB,GPIO_Pin_7);	


    GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
    GPIO_Init(GPIOC, &GPIOD_InitStructure);	
    
	GPIO_ResetBits(GPIOC,GPIO_Pin_6);	
	GPIO_ResetBits(GPIOC,GPIO_Pin_7);		
	GPIO_ResetBits(GPIOC,GPIO_Pin_8);	
	GPIO_ResetBits(GPIOC,GPIO_Pin_9);	

	#if 0
    GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
    GPIO_Init(GPIOC, &GPIOD_InitStructure);	
    
	GPIO_ResetBits(GPIOC,GPIO_Pin_10);	
	GPIO_ResetBits(GPIOC,GPIO_Pin_11);		
 
	#endif
	
	 iris_auto_manual_set(0);// auto iris mode

}



#define	KEY_PORT1		GPIOA
#define	KEY_PORT2		GPIOB

void key_pin_init(void)
{

	GPIO_InitTypeDef GPIOD_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIOD_InitStructure);	

	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIOD_InitStructure);	


	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
	GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIOD_InitStructure); 



	
}

u16 key_pre = 0;

u8 video_sd_hd_mode = 1;
u8 record_state = 0;
u8 ssd_state = 0;


u8 filter_mode_value = 0;

#define	RECORDER_KEY_STATE_RECORD_PIN	GPIO_Pin_8
#define	RECORDER_KEY_STATE_PLAYER_PIN	GPIO_Pin_7
#define	RECORDER_KEY_STATE_NEXT_PIN	GPIO_Pin_6
#define	RECORDER_KEY_STATE_PREV_PIN	GPIO_Pin_9
#define	RECORDER_KEY_STATE_PAUSE_PIN	GPIO_Pin_8

void recorder_key_state_pin_init(void)
{

	GPIO_InitTypeDef GPIOD_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = RECORDER_KEY_STATE_PLAYER_PIN|RECORDER_KEY_STATE_NEXT_PIN|RECORDER_KEY_STATE_PREV_PIN|RECORDER_KEY_STATE_PAUSE_PIN;
	GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIOD_InitStructure); 

	GPIOD_InitStructure.GPIO_Pin = RECORDER_KEY_STATE_RECORD_PIN;
	GPIO_Init(GPIOA, &GPIOD_InitStructure); 

	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIOD_InitStructure); 


}




u8 recorder_key_state_check(void)
{
	u16 tmp;
	
	tmp = (GPIO_ReadInputData(GPIOC)>>6)&0x000f;


	u8 i;

	for(i=0;i<4;i++)
	{
		if(( (tmp>>i)&0x01) == 0)
		{
			record_state = i+1;
			break;
		}


	}
	

	if(i==4)
	{

		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8) == 0)
		{
			i = 0xff;
			record_state = 1;
		}
		else
			record_state = 0;
		
	}

	
	
}


static u8 pb11_mode_check(void)
{
	static u8 key_state_tmp = 0;
	
	if(GPIO_ReadInputDataBit(KEY_PORT2,GPIO_Pin_11) == 0)
	{
		rt_thread_delay(RT_TICK_PER_SECOND/50);
		if(GPIO_ReadInputDataBit(KEY_PORT2,GPIO_Pin_11) == 0)
		{

			if(key_state_tmp == 0)
			{

				key_state_tmp = 1;
				filter_mode_value++;
				if(filter_mode_value>3)
					filter_mode_value = 0;

				
				pelcod_call_pre_packet_send(filter_mode_value+201);
				return 1;

			}
		}

	}
	else
	{
		key_state_tmp = 0;

	}

	return 0;
}


static void pa4_hdsd_set(void)
{
	static u8 key_state_tmp = 0;

	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	rt_thread_delay(RT_TICK_PER_SECOND/2);

	GPIO_ResetBits(GPIOA,GPIO_Pin_4);

}

static void pa5_dpon_set(void)
{
	static u8 key_state_tmp = 0;

	GPIO_SetBits(GPIOA,GPIO_Pin_5);
	rt_thread_delay(RT_TICK_PER_SECOND/2);

	GPIO_ResetBits(GPIOA,GPIO_Pin_5);

}

static u8 pc2_hdon_check(void)
{
	static u8 key_state_tmp = 0;
	
	static u16 key_state_tmp_cnt = 0;

	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2) == 0)
	{
		rt_thread_delay(RT_TICK_PER_SECOND/50);
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2) == 0)
		{
			if(key_state_tmp_cnt < 0xffff)
				key_state_tmp_cnt++;

			if(key_state_tmp_cnt> 3*50)
			{//long push
				key_state_tmp_cnt = 0;

				
			}

			if(key_state_tmp == 0)
			{
	
				key_state_tmp = 1;


				return 1;

			}
		}

	}
	else
	{
		if(key_state_tmp == 1)
		{// short push
			pa4_hdsd_set();
			key_state_tmp = 0;

		}
	}

	return 0;
}




//返回0为无按键，返回非0值，则为对应的按键号
static u16 key_ctl_check(void)
{
	u16 data,data2;
	u16 i;
	
	static u16 key_bak=0;
	
	data = GPIO_ReadInputData(KEY_PORT1);
	data2 = GPIO_ReadInputData(KEY_PORT2);	
	data = (data>>1)&0x00ff;
	data2 = (data2>>4)&0x0f00;
	data 	+= data2;

	
	
	for(i=0;i<12;i++)
	{
		if(((data>>i)&0x0001)==0)
		{
			rt_thread_delay(3);
			data = GPIO_ReadInputData(KEY_PORT1);
			data2 = GPIO_ReadInputData(KEY_PORT2);	
			data = (data>>1)&0x00ff;
			data2 = (data2>>4)&0x0f00;
			data 	+= data2;

			if(((data>>i)&0x0001)==0)
			{
				//
				if(i==6)
				{
					if(((data>>7)&0x0001)==0)
					{
						while(1)
						{
							data = GPIO_ReadInputData(KEY_PORT1);
							data2 = GPIO_ReadInputData(KEY_PORT2);	
							data = (data>>1)&0x00ff;
							data2 = (data2>>4)&0x0f00;
							data 	+= data2;
							
							if((((data>>6)&0x0001)!=0)&&(((data>>7)&0x0001)!=0))
								break;
							rt_thread_delay(3);
						}
						if(key_pre == 0x0708)
							return 0;
						
						key_pre = 0x0708;
						return (key_pre);
					}
				}
				
				if(i==7)
				{
					if(((data>>6)&0x0001)==0)
					{
						if(key_pre == 0x0708)
							return 0;
						
						while(1)
						{
							data = GPIO_ReadInputData(KEY_PORT1);
							data2 = GPIO_ReadInputData(KEY_PORT2);	
							data = (data>>1)&0x00ff;
							data2 = (data2>>4)&0x0f00;
							data 	+= data2;
							
							if((((data>>6)&0x0001)!=0)&&(((data>>7)&0x0001)!=0))
								break;
							rt_thread_delay(3);
						}
						
						key_pre = 0x0708;
						return (key_pre);
					}
				}

				if(key_pre == (i+1))
					return 0;
				
				key_pre = i+1;
				return (i+1);
				
			}
		}
	}

	if(key_pre)
	{
		i = key_pre|0x8000;
		key_pre = 0;
		return i;

	}
	return 0;
}


const u16 led_pin[]=
{
GPIO_Pin_0,
GPIO_Pin_1,
GPIO_Pin_2,
GPIO_Pin_3,
GPIO_Pin_4,
GPIO_Pin_5,
GPIO_Pin_6,
GPIO_Pin_7,

};


extern rt_sem_t	uart1_sem;

rt_err_t rs485_recieve_check(u8 val)
{

	
	if(rt_sem_take(uart1_sem, 30) == RT_EOK)
    {
		if (command_analysis()) 
		{
            switch(command_byte)
		    {
			 	case 0x11://call preset point

					if(Rocket_fir_data == val)
						return RT_EOK;
					break;

             	default:
				break;
	   	    }

		}
	}
	return RT_ERROR;

}


u8 cmd_buff[7];

rt_sem_t rs485_return_sem;

extern rt_err_t rs485_send_data(u8* data,u16 len);

void pelcod_call_pre_packet_send(u8 val)
{
	u8 cnt;
	cmd_buff[0] = 0xff;
	cmd_buff[1] = 0xff;
	cmd_buff[2] = 0;
	cmd_buff[3] = 0x07;
	cmd_buff[4] = 0;
	cmd_buff[5] = val;
	
	cmd_buff[6] = cmd_buff[1] + cmd_buff[2] + cmd_buff[3] + cmd_buff[4] + cmd_buff[5];
	rs485_send_data(cmd_buff,7);

//	cnt=3;
//	while(cnt--)
//	{
//		if(RT_EOK == rs485_recieve_check(val))
//			break;
//		else
//			rs485_send_data(cmd_buff,7);
//	}
}


void pelcod_set_pre_packet_send(u8 val)
{
	u8 cnt;
	cmd_buff[0] = 0xff;
	cmd_buff[1] = 0xff;
	cmd_buff[2] = 0;
	cmd_buff[3] = 0x03;
	cmd_buff[4] = 0;
	cmd_buff[5] = val;
	
	cmd_buff[6] = cmd_buff[1] + cmd_buff[2] + cmd_buff[3] + cmd_buff[4] + cmd_buff[5];
	rs485_send_data(cmd_buff,7);

//	cnt=3;
//	while(cnt--)
//	{
//		if(RT_EOK == rs485_recieve_check(val))
//			break;
//		else
//			rs485_send_data(cmd_buff,7);
//	}
}

//val: 0,open; 1,close
void pelcod_open_close_packet_send(u8 val)
{
	u8 cnt;
	cmd_buff[0] = 0xff;
	cmd_buff[1] = 0xff;
	if(val)//close
		cmd_buff[2] = 0x04;
	else
		cmd_buff[2] = 0x02;
	cmd_buff[3] = 0;
	cmd_buff[4] = 0;
	cmd_buff[5] = 0;
	
	cmd_buff[6] = cmd_buff[1] + cmd_buff[2] + cmd_buff[3] + cmd_buff[4] + cmd_buff[5];
	rs485_send_data(cmd_buff,7);

//	cnt=3;
//	while(cnt--)
//	{
//		if(RT_EOK == rs485_recieve_check(val))
//			break;
//		else
//			rs485_send_data(cmd_buff,7);
//	}
}


//val 7,7on 8off;8,7 off,8on
void led_7_8_onoff_set(u8 val)
{
	if(val>8)
		val = 8;
	if(val==7)
	{
		GPIO_WriteBit(GPIOB, led_pin[val-1], Bit_SET);
		GPIO_WriteBit(GPIOB, led_pin[val], Bit_RESET);

	}
	else
	{
		GPIO_WriteBit(GPIOB, led_pin[val-2], Bit_RESET);
		GPIO_WriteBit(GPIOB, led_pin[val-1], Bit_SET);
	}
	//pelcod_call_pre_packet_send(val+200);
}


u8 iris_auto_manual_state = 0;// 默认0 为自动模式
void iris_auto_manual_switch(void)
{
	if(!iris_auto_manual_state)//manual iris
	{
		iris_auto_manual_state = 1;//manual
		led_7_8_onoff_set(8);

		pelcod_call_pre_packet_send(128);//manual
	}
	else
	{
		iris_auto_manual_state = 0;
		
		led_7_8_onoff_set(7);
		pelcod_set_pre_packet_send(128);

	}

}


void iris_auto_manual_set(u8 mode)
{
	if(mode)//manual iris
	{
		iris_auto_manual_state = 1;//manual
		led_7_8_onoff_set(8);

		pelcod_call_pre_packet_send(128);//manual
	}
	else
	{
		iris_auto_manual_state = 0;
		led_7_8_onoff_set(7);
		pelcod_set_pre_packet_send(128);

	}

}



void led_onoff_set(u16 val)
{
	u8 i;

	if(val > 7)
		return;
#if 0
GPIO_WriteBit(GPIOC, GPIO_Pin_10, Bit_RESET);
GPIO_WriteBit(GPIOC, GPIO_Pin_11, Bit_RESET);

#endif
	for(i=0;i<6;i++)//不处理 7号和8号灯
	{
		GPIO_WriteBit(GPIOB, led_pin[i], Bit_RESET);

	}
	GPIO_WriteBit(GPIOB, led_pin[val-1], Bit_SET);

#if 0
	if(val == 4)
		GPIO_WriteBit(GPIOC, GPIO_Pin_10, Bit_RESET);
	if
#endif
	pelcod_call_pre_packet_send(val+200);
}

void key_io_set(u16 val)
{
u16 tmp;

	switch(val)
	{
	case 9:
		GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_SET);
		GPIO_WriteBit(GPIOC, GPIO_Pin_9, Bit_RESET);
		break;
	case 10:
		GPIO_WriteBit(GPIOC, GPIO_Pin_9, Bit_SET);
		GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_RESET);
		break;
	case 11:
		GPIO_WriteBit(GPIOC, GPIO_Pin_6, Bit_SET);
		GPIO_WriteBit(GPIOC, GPIO_Pin_7, Bit_RESET);
		break;
	case 12:
		GPIO_WriteBit(GPIOC, GPIO_Pin_7, Bit_SET);
		GPIO_WriteBit(GPIOC, GPIO_Pin_6, Bit_RESET);
		break;

	
	}

	if(val & 0x8000)
	{
		tmp = val&0x8000;

		if(tmp<9)
			return;
		GPIO_WriteBit(GPIOC, GPIO_Pin_6, Bit_RESET);
		GPIO_WriteBit(GPIOC, GPIO_Pin_7, Bit_RESET);
		GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_RESET);
		GPIO_WriteBit(GPIOC, GPIO_Pin_9, Bit_RESET);
	}
}



void key_handle(u16 val)
{
	if(val == 0)
		return;

	
	
	if(val == 0x0708)
	{
		iris_auto_manual_switch();
	}
	else
	{
		
		if(val == 0x8708)
			return;

#if 0		
		if(val & 0x8000)
		{
			val = val&0x7fff;
		}
		else 
#endif	

		if((val < 0x8009)&&(val >= 0x8000))
			val = val&0x7fff;
		else
		{
			if(val >=9)
			{
				key_io_set(val);
				return;
			}
			else
				return;

		}
		
		if(val<9)
		{
			if(val < 7)// 在此处不处理按键7 8
				led_onoff_set(val);
			else
			{
				if(iris_auto_manual_state)//manual mode
				{
				if(val==7)
					pelcod_open_close_packet_send(1);
				else
					pelcod_open_close_packet_send(0);
				}
			}
		}
		else if(val != 0)
		{
			key_io_set(val);

		}
	}
}

void rt_key_thread_entry(void* parameter)
{

	u16 k;

	key_pin_init();
	led_pin_init();
	

    while(1)
	{



		pc2_hdon_check();

		pb11_mode_check();
		
		
		rt_thread_delay(4);
    }
}




int rt_key_ctl_init(void)
{

	
    rt_thread_t init_thread;

    init_thread = rt_thread_create("key",
                                   rt_key_thread_entry, RT_NULL,
                                   4096, 10, 5);
    if (init_thread != RT_NULL)
        rt_thread_startup(init_thread);

    return 0;
}

