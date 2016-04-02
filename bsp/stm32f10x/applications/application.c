/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2013-07-12     aozima       update for auto initial.
 */

/**
 * @addtogroup STM32
 */
/*@{*/

#include <board.h>
#include <rtthread.h>

#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif  /* RT_USING_COMPONENTS_INIT */

#ifdef RT_USING_DFS
/* dfs filesystem:ELM filesystem init */
#include <dfs_elm.h>
/* dfs Filesystem APIs */
#include <dfs_fs.h>
#endif

#ifdef RT_USING_RTGUI
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_server.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/driver.h>
#include <rtgui/calibration.h>
#endif

#include "includes.h"
#include "rs485_decode.h"



#ifdef RT_USING_RTGUI
rt_bool_t cali_setup(void)
{
    rt_kprintf("cali setup entered\n");
    return RT_FALSE;
}

void cali_store(struct calibration_data *data)
{
    rt_kprintf("cali finished (%d, %d), (%d, %d)\n",
               data->min_x,
               data->max_x,
               data->min_y,
               data->max_y);
}
#endif /* RT_USING_RTGUI */

void rt_init_thread_entry(void* parameter)
{
#ifdef RT_USING_COMPONENTS_INIT
    /* initialization RT-Thread Components */
    rt_components_init();
#endif

#ifdef  RT_USING_FINSH
    finsh_set_device(RT_CONSOLE_DEVICE_NAME);
#endif  /* RT_USING_FINSH */

    /* Filesystem Initialization */
#if defined(RT_USING_DFS) && defined(RT_USING_DFS_ELMFAT)
    /* mount sd card fat partition 1 as root directory */
    if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("File System initialized!\n");
    }
    else
        rt_kprintf("File System initialzation failed!\n");
#endif  /* RT_USING_DFS */

}




void pt_action(uchar action,uchar pan_speed,uchar tilt_speed) //action:0~7 ->left,right....; action=8 means stop command
{ 

        
}

void pt_action_tilt(uchar action,uchar tilt_speed) //action:0~7 ->left,right....; action=8 means stop command
{ 


        
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

    break;
    
    case 0x03://focus
    case 0x04:	
        temp = command_byte - 1; 
        pt_action_tilt(temp,1);

    break;
    case 0x05:  

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


void rt_myt1_thread_entry(void* parameter)
{

	

	while(1)
	{


		rt_thread_delay(RT_TICK_PER_SECOND/20);
		//if(i<160)
		//{
		//	LenDrvZoomMove(1,1);
		//	i++;
		//}
		//else
		//{
		//	LenDrvZoomMove(0,1);
		//	i--;
		//}
	}
	
}

extern rt_sem_t	uart1_sem;
u16 ttlen;
void rs485_recieve_handle(void)
{
	u16 k;

	while(1)
	{
		if(rt_sem_take(uart1_sem, RT_WAITING_FOREVER) == RT_EOK)
        {
			if (command_analysis()) 
			{
				if(command_byte != 0x0e && command_byte!=0x0f)
				{
;

				}
				ptz_execute();
	            switch(command_byte)
			    {
				 	case 0x0E://open iris
														 

						break;
				 	case 0x0F://close iris
			 			
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


	                	//cam_osd_control(1);
	        			//camera_osd_switch(0);

	                    //camera_osd_key_emulate(OSD_ENTER);
	                    delay_half_ms(20);
	                    break;
	             	default:
					break;
		   	    }


				
			}
        rt_thread_delay(RT_TICK_PER_SECOND/50);
		}
    }

}

extern int rt_adc_ctl_init(void);
extern int rt_key_ctl_init(void);
extern void OLED_Init(void);
extern void osd_test(void);
extern void osd_init(void);



void rt_main_thread_entry(void* parameter)
{
	rt_thread_delay(1000);

	rs485_system_init();
	osd_init();
	rt_thread_delay(600);

	rt_key_ctl_init();
	rt_adc_ctl_init();

    
}



int rt_application_init(void)
{
    rt_thread_t init_thread;


	
    init_thread = rt_thread_create("mY",
                                   rt_main_thread_entry, RT_NULL,
                                   8192, 6, 5);
    if (init_thread != RT_NULL)
        rt_thread_startup(init_thread);


#if (RT_THREAD_PRIORITY_MAX == 32)
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 8, 20);
#else
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 80, 20);
#endif

    if (init_thread != RT_NULL)
        rt_thread_startup(init_thread);

    return 0;
}


/*@}*/
