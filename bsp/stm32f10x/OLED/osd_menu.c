#include <board.h>
#include <rtthread.h>

#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif  /* RT_USING_COMPONENTS_INIT */

#include "includes.h"
#include "oled.h"
#include "bmp.h"
#include "osd_menu.h"

#include "key_ctl.h"



void osd_clear_x_y(u8 x_start,u8 x_end,u8 y,u8 sizef)
{
	u8 ii;

	for(ii=x_start;ii<x_end;)
	{
		OLED_ShowString(ii,y," ",sizef);
		ii+=sizef;
	}
}





#define	OSD_LINE1_X_POS		0
#define	OSD_LINE4_Y_POS		6



void osd_val_disp(u8 *data,u8 cnt)
{
	//u8 i;

	//u8 buff[12];

	//if(cnt > 10)
	//	return;
	//
	//for(i=0;i<cnt;i++)
	//{
	//	buff[i]=data[i];
	//}
	//buff[i] = '\0';
	OLED_ShowString(OSD_VAL_START_ADDR_X,OSD_VAL_START_ADDR_Y,data,16);  

}

void osd_line2_val_disp_clear(void)
{

	//osd_clear_x_y(OSD_VAL_START_ADDR_X,OSD_VAL_START_ADDR_X+8*7,OSD_VAL_START_ADDR_Y,16);
	OLED_Clear_line(0,OSD_LINE2_Y_POS,8);
	OLED_Clear_line(0,OSD_LINE2_Y_POS+1,8);
}

void osd_line1_val_disp_clear(void)
{

	//osd_clear_x_y(OSD_VAL_START_ADDR_X,OSD_VAL_START_ADDR_X+8*7,OSD_VAL_START_ADDR_Y,16);
	OLED_Clear_line(0,0,16);
	OLED_Clear_line(0,0+1,16);
}


extern u8 battery_power;
extern u8 video_sd_hd_mode;

void osd_line2_disp(u8 x)
{
static u8 battery_power_pre=0xff;



	if(battery_power_pre==0xff || battery_power_pre!=battery_power )
	{
		osd_line2_val_disp_clear();


		osd_battery_mode_disp(battery_power,OSD_LINE2_X_16_POS,OSD_LINE2_Y_16_POS);

		osd_sd_hd_mode_disp(video_sd_hd_mode,OSD_LINE2_X_16_POS+88,OSD_LINE2_Y_16_POS);

		battery_power_pre = battery_power;

	}
}


void osd_line_2_disp_item_clear(u8 select)
{
	if(select>3)
		return;

	u8 xstart;//,xend;
	
	//osd_line2_val_disp_clear();	

	switch(select)
	{
	case 1:

		break;
	case 2:
		break;
	case 3:// filter
		//osd_para_mode_disp(cam_para_mode);
		//osd_iris_mode_disp(iris_mode);
		break;
	default:break;

	}
	
}


extern u8 osd_mid_buff[30];
extern u8 key_val_buffer_cnt;
extern u8 osd_mid_str_buff[10];

void osd_set_para_disp_line_1(u8 item)
{

}


const u8*osd_line1_string={"Mspectrum Imager"};


void osd_line1_disp(u8 x)
{
	//osd_val_disp(osd_mid_buff,key_val_buffer_cnt);

	OLED_ShowString(0,OSD_VAL_START_ADDR_Y,(u8*)osd_line1_string,16);  

	
}


void OLED_Clear_line(u8 x,u8 y,u8 sizef)  
{  
	u8 i,n;		

	i = y;
	
 
	OLED_WR_Byte (0xb0+i,OLED_CMD);    //¨¦¨¨??¨°3¦Ì??¡¤¡ê¡§0~7¡ê?
	OLED_WR_Byte (0x00,OLED_CMD);      //¨¦¨¨????¨º??????a¨¢D¦Ì¨ª¦Ì??¡¤
	OLED_WR_Byte (0x10,OLED_CMD);      //¨¦¨¨????¨º??????a¨¢D??¦Ì??¡¤   
	for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
}



void osd_para_mode_disp(u8 mode)
{
	u8 *mstr="Mode";
	u8 pp;

	if(mode > 8)
		return;
	OLED_ShowString(OSD_PARA_MODE_X_START,OSD_PARA_MODE_Y,mstr,16); 
	
	pp = mode+0x31;
	OLED_ShowString(OSD_PARA_MODE_X_START+32,OSD_PARA_MODE_Y,&pp,16); 
}

extern u8 cam_filter_mode;




void osd_battery_mode_disp(u8 data,u8 x,u8 y)
{
	static u8 tmp = 1;

	if(tmp)
		{
		tmp = 0;
	if(data>100)
		data = 100;
	
	OLED_ShowString(x,y,"Batt:   %",16); 
	OLED_ShowNum(x+40,y,data,3,16); 
		tmp = 1;
		}
		return;

}

void osd_battery_val_disp(u8 data,u8 x,u8 y)
{
	if(data>100)
		data = 100;
	
	OLED_ShowNum(x+40,y,data,3,16); 
	
		return;

}


void osd_sd_hd_mode_disp_clear(u8 data,u8 x,u8 y)
{

		OLED_ShowString(x,y,"     ",16); 

	return;
}


void osd_sd_hd_mode_disp(u8 data,u8 x,u8 y)
{
	osd_sd_hd_mode_disp_clear(data,x,y);

	if(data)
	{
		OLED_ShowString(x,y,"HD/SD",16); 

		}
	else
		{
		OLED_ShowString(x,y+1,"HD/SD",8); 

		}
	return;
}





#define	PIXCEL_NUM_PER_FONT_16		8

const u8 *filter_mode_string[]={
	//{"    "},
		{" UV "},
		{" BL "},
		{" GR "},
		{" IR "},

};

void filter_mode_disp_clear(u8 x,u8 y,u8 mode)
{
	if(mode>3)
		mode=3;
		
	OLED_ShowString(x+mode*16,y,(u8*)"  ",16);

}

void filter_mode_disp(u8 x,u8 y,u8 mode)
{
	if(mode>3)
		mode=3;
	u8 i = 16;


	for(u8 k=0;k<4;k++)
	{
		if(k == mode)
			i = 16;
		else
			i = 8;

		if(i==16)
			OLED_ShowString(x+k*32,y,(u8*)filter_mode_string[k],i);
		else
			OLED_ShowString(x+k*32,y+1,(u8*)filter_mode_string[k],i);
			
	}
}




extern u8 filter_mode_value;

void osd_line3_disp(u8 select)
{
	
	OLED_Clear_line(0,OSD_LINE3_Y_POS,16);
	OLED_Clear_line(0,OSD_LINE3_Y_POS+1,16);


	filter_mode_disp(0,OSD_LINE3_Y_POS,filter_mode_value);
}


void osd_line3_disp_clear_line(void)
{
	
	OLED_Clear_line(0,OSD_LINE3_Y_POS,16);
	OLED_Clear_line(0,OSD_LINE3_Y_POS+1,16);

}


#define	OSD_LINE_4_LITTLE_START_X	0
#define	OSD_LINE_4_LITTLE_START_Y	(6)

#define	OSD_LINE_4_FONT_PIXEL_X		8


u8 line4_buff[16]={0};





const u8 recorder_state_msg[]={

	"Record"};

const u8 ssd_state_msg[]={

	"SSD"};



extern enum KEY_STATE_TYPE recorder_work_state;

extern u8 record_state;
extern u8 ssd_state;

void osd_line4_disp(u8 chn)
{

	static u8 tmp = 1;

	if(tmp)
		{
		tmp = 0;
		

	OLED_Clear_line(0,6,16);
	OLED_Clear_line(0,7,16);

	memset(line4_buff,0,16);

	if(record_state)
		strcpy((char *)line4_buff,(const char*)recorder_state_msg);
	if(ssd_state)
		strcpy((char *)line4_buff+10,(const char*)ssd_state_msg);

	
	OLED_ShowString(0,6,line4_buff,16);

	tmp = 1;
		}
}



void osd_line_little_4_disp(u8 chn)
{
	OLED_Clear_line(0,6,16);
	OLED_Clear_line(0,7,16);
	

}

void osd_line_little_4_disp_item_clear(u8 select)
{

}


void osd_line_1to4_all_disp(void)
{
	static u8 tmp = 1;

	if(tmp)
		{
		tmp = 0;

	osd_line1_disp(1);	
	osd_line2_disp(1);
	osd_line3_disp(2);	
	osd_line4_disp(0);
tmp = 1;
		}
}


void rt_osd_thread_entry(void* parameter)
{

	while(1)
	{
			
#if 0

		if(menu_normal_flag)
		{
			if(cursor_ver_pos == 0)
			{
				osd_sd_hd_mode_disp(video_sd_hd_mode,OSD_LINE2_X_16_POS+88,OSD_LINE2_Y_16_POS);
				rt_thread_delay(600);
				osd_sd_hd_mode_disp_clear(video_sd_hd_mode,OSD_LINE2_X_16_POS+88,OSD_LINE2_Y_16_POS);
			}
			else if(cursor_ver_pos == 1)
			{
				filter_mode_disp(0,OSD_LINE3_Y_POS,filter_mode_value);
				rt_thread_delay(600);
				
				filter_mode_disp_clear(0,OSD_LINE3_Y_POS,filter_mode_value);
			}
			rt_thread_delay(500);

		}
		else
		{

			osd_battery_val_disp(video_sd_hd_mode,OSD_LINE2_X_16_POS,OSD_LINE2_Y_16_POS);
			
		}
#endif		
		
		//osd_battery_mode_disp(battery_power,OSD_LINE2_X_16_POS,OSD_LINE2_Y_16_POS);
		//osd_line4_disp(1);

		//osd_line_1to4_all_disp();
		osd_line2_disp(1);
		osd_line3_disp(2);	
		osd_line4_disp(0);

		rt_thread_delay(500);
	}

}

void osd_init(void)
{
	OLED_Init();			//3?¨º??¡¥OLED  
	OLED_Clear()  	; 
	osd_line_1to4_all_disp();

	rt_thread_t init_thread;


	init_thread = rt_thread_create("osd",
								   rt_osd_thread_entry, RT_NULL,
								   512, 10, 5);
	if (init_thread != RT_NULL)
		rt_thread_startup(init_thread);


}

void osd_test(void)
{	u8 t;
		OLED_Init();			//3?¨º??¡¥OLED  
		OLED_Clear()  	; 
	
		t=' ';
		OLED_ShowCHinese(0,0,0);//?D
				OLED_ShowCHinese(18,0,1);//?¡ã
		OLED_ShowCHinese(36,0,2);//?¡ã
		OLED_ShowCHinese(54,0,3);//¦Ì?
		OLED_ShowCHinese(72,0,4);//¡Á¨®
		OLED_ShowCHinese(90,0,5);//??
		OLED_ShowCHinese(108,0,6);//??
	while(1) 
	{		
		OLED_Clear();
		OLED_ShowCHinese(0,0,0);//?D
		OLED_ShowCHinese(18,0,1);//?¡ã
		OLED_ShowCHinese(36,0,2);//?¡ã
		OLED_ShowCHinese(54,0,3);//¦Ì?
		OLED_ShowCHinese(72,0,4);//¡Á¨®
		OLED_ShowCHinese(90,0,5);//??
		OLED_ShowCHinese(108,0,6);//??
		OLED_ShowString(6,3,"0.96' OLED TEST",16);

				OLED_ShowString(6,4,"0.96' OLED TEST",16);
		OLED_ShowString(0,5,"0.96' OLED TEST",16);

		OLED_ShowString(0,6,"0.96' OLED TEST",16);
		OLED_ShowString(1,7,"0.96' OLED TEST",16);

		OLED_ShowString(6,3,"0.96' OLED TEST",16);

		//OLED_ShowString(8,2,"ZHONGJINGYUAN");  
	 //	OLED_ShowString(20,4,"2014/05/01");  
		OLED_ShowString(0,6,"ASCII:",16);  
		OLED_ShowString(63,6,"CODE:",16);  
		OLED_ShowChar(48,6,t,16);//??¨º?ASCII¡Á?¡¤?	   
		t++;
		if(t>'~')t=' ';
		OLED_ShowNum(103,6,t,3,16);//??¨º?ASCII¡Á?¡¤?¦Ì????¦Ì 	
		rt_thread_delay(1000*3);


		OLED_DrawBMP(0,0,128,8,BMP1);  //¨ª?????¨º?(¨ª?????¨º?¨¦¡Â¨®?¡ê?¨¦¨²3¨¦¦Ì?¡Á?¡À¨ª??¡ä¨®¡ê??¨¢??¨®????¨¤????¡ê?FLASH????8K¨°???¨¦¡Â¨®?)
		rt_thread_delay(1000*3);

		OLED_DrawBMP(0,0,128,8,BMP1);
		rt_thread_delay(1000*3);


	}	
	
}


