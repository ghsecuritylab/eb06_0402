#include "includes.h"
#include "rtthread.h"
#include <rthw.h>

#include <rtdevice.h>

volatile u8 camera_id  = 0x00;

u8 cam_response_check(void);

/*
功能说明： CRC16 校验码生成函数
入口参数： DataBuf，待校验数据缓冲区
            num，待校验数据长度

返回：      返回两字节校验码

*/
unsigned short CRC16(unsigned char *DataBuf,int num)
{ 
	int i,j;
	unsigned short c,crc=0xFFFF;
	for (i=0;i<num;i++ )
	{ 	
		c=DataBuf[i] & 0x00FF;
		crc^=c;
		for(j=0;j<8;j++ )
		{ 
			if (crc & 0x0001)
			{
				crc>>=1;
				crc^=0xA001;
			}
			else crc>>=1;
		}
	}
	
	return(crc);
}

unsigned short GenCRC(unsigned char * lpBuf, unsigned char sizeOfBuf)
{
    unsigned char I, j;
    unsigned char flg_carry;
    unsigned char tmp_rotate;
    unsigned char buf;
    unsigned short crc16;
    
    crc16 = 0;
    for (I = 0; I < sizeOfBuf; I++) 
    {
        buf = lpBuf[I];
        for (j = 0; j < 8; j++) 
        {
            flg_carry = 0;
            if(buf & 0x80)
            {
                flg_carry = 1;
            }
            buf <<= 1;
            tmp_rotate = (crc16 >> 15) & 0x01;
            crc16 <<= 1;
            if (tmp_rotate) 
            {
                crc16 ^= 0x1021;
            }
            crc16 ^= (unsigned short)flg_carry;
        }
    }
    return crc16;
}

extern struct rt_serial_device serial2;

#if 1
void txd1_buffer_send(void)
{
	txd1_buff_cFlag=0;		//clear camera command send complete flag
	rxd1_buff_cFlag=0;	    //clear camera command receive complete flag
	
	rt_device_write(&serial2.parent, 0, txd1_buffer, cam_cmd_lenth);
}

#else
void txd1_buffer_send(void)
{
	txd1_buff_cFlag=0;		//clear camera command send complete flag
	rxd1_buff_cFlag=0;	    //clear camera command receive complete flag

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE); 	 //
}
#endif


void camera_buffer_send(u8 cmd,u8 len)
{
    u16 temp;
    static u8 cbs_state = 0;

	if(!cbs_state)
	{//防止重入
		cbs_state = 1;
	    txd1_buffer[0]=camera_id;
	    txd1_buffer[1]=0x00;
	    txd1_buffer[2]=cmd;
	    txd1_buffer[3]=len;

	    temp = GenCRC(txd1_buffer,4+len);
	    txd1_buffer[4+len] = (u8)temp&0xFF;
	    txd1_buffer[5+len] = (u8)(temp>>8)&0xFF;

	    cam_cmd_lenth = 6+len;
	    txd1_buffer_send();
	    delayms(50);

		cbs_state = 0;
	}
	
}



u8 cam_get_parameters_oneByte(u8 blockNO,u16 offset,u8*get_para)
{
    u16 temp;
    
    txd1_buffer[0]=camera_id;
    txd1_buffer[1]=0x00;
    txd1_buffer[2]=0x20;
    txd1_buffer[3]=0x04;

    txd1_buffer[4] = blockNO;
    txd1_buffer[5] = offset&0xff;
    txd1_buffer[6] = (offset>>8)&0xff;
    txd1_buffer[7] = 1;
   
    temp = GenCRC(txd1_buffer,8);
    txd1_buffer[8] = (u8)temp&0xFF;
    txd1_buffer[9] = (u8)(temp>>8)&0xFF;

    cam_cmd_lenth = 10;
        
    txd1_buffer_send();

    if(cam_response_check()	)
    {
        *get_para = rxd1_buffer[9];
        return 1;
    }
    

    return 0;
}

const u8 cam_block_length_arr[]={
43,15,63,15,63,31,131,55,84, //第8区不读取第0偏移处数据
};

u8 cam_get_parameters_by_block(u8 blockNO,u8*get_para)
{
    u16 temp;
    u8 len;
    u8 i;
    
    if(blockNO > 8)

        blockNO = 8;

    len = cam_block_length_arr[blockNO];
    
    txd1_buffer[0]=camera_id;
    txd1_buffer[1]=0x00;
    txd1_buffer[2]=0x20;
    txd1_buffer[3]=0x04;

    txd1_buffer[4] = blockNO;
    txd1_buffer[5] = 0;
    txd1_buffer[6] = 0;
    txd1_buffer[7] = len;
   
    temp = GenCRC(txd1_buffer,8);
    txd1_buffer[8] = (u8)temp&0xFF;
    txd1_buffer[9] = (u8)(temp>>8)&0xFF;

    cam_cmd_lenth = 10;
        
    txd1_buffer_send();

    if(cam_response_check()	)
    {
        if(cam_recieve_cnt_public<=11)
            return 0;
        len = cam_recieve_cnt_public-11;
        for(i=0;i<len;i++)
        {
            if(i>cam_block_length_arr[blockNO])
                break;
            *(get_para+i) = rxd1_buffer[9+i];

        }
        return 1;
    }
    

    return 0;
}


u8 cam_get_parameters_by_block_8(u8*get_para)
{
    u16 temp;
    u8 len;
    u8 i;
    
    
    len = cam_block_length_arr[8];
    
    txd1_buffer[0]=camera_id;
    txd1_buffer[1]=0x00;
    txd1_buffer[2]=0x20;
    txd1_buffer[3]=0x04;

    txd1_buffer[4] = 8;
    txd1_buffer[5] = 1;
    txd1_buffer[6] = 0;
    txd1_buffer[7] = len;
   
    temp = GenCRC(txd1_buffer,8);
    txd1_buffer[8] = (u8)temp&0xFF;
    txd1_buffer[9] = (u8)(temp>>8)&0xFF;

    cam_cmd_lenth = 10;
        
    txd1_buffer_send();

    if(cam_response_check()	)
    {
        if(cam_recieve_cnt_public<=11)
            return 0;
        len = cam_recieve_cnt_public-11;
        for(i=0;i<len;i++)
        {
            if(i>cam_block_length_arr[8])
                break;
            *(get_para+i) = rxd1_buffer[9+i];

        }
        return 1;
    }
    

    return 0;
}


#if 1
u8 cam_set_parameters(u8 blockNO,u16 offset,u8 para_len,u8 *para_buff)
{
    u8 i;
    
    txd1_buffer[4] = blockNO;
    txd1_buffer[5] = offset&0xff;
    txd1_buffer[6] = (offset>>8)&0xff;
    txd1_buffer[7] = para_len;
    
    for(i=0;i<para_len;i++)
        txd1_buffer[8+i] = *(para_buff+i);
    
    
    camera_buffer_send(0x28,4+para_len);
	return 0;
}
#else
u8 cam_set_parameters(u8 blockNO,u16 offset,u8 para_len,u8 *para_buff)
{
    u8 i;
    
    txd1_buffer[4] = blockNO;
    txd1_buffer[5] = offset&0xff;
    txd1_buffer[6] = (offset>>8)&0xff;
    txd1_buffer[7] = para_len;
    
    for(i=0;i<para_len;i++)
        txd1_buffer[8+i] = *(para_buff+i);
    
    
    camera_buffer_send(0x28,4+para_len);
	return 0;
}
#endif

u8 cam_set_parameters_by_block(u8 blockNO,u8 *para_buff)
{
    u8 i;
    u8 para_len;

    if(blockNO > 8)

        blockNO = 8;

    para_len = cam_block_length_arr[blockNO];

    txd1_buffer[4] = blockNO;
    txd1_buffer[5] = 0;
    txd1_buffer[6] = 0;
    txd1_buffer[7] = para_len;
    
    for(i=0;i<para_len;i++)
        txd1_buffer[8+i] = *(para_buff+i);
    
    
    camera_buffer_send(0x28,4+para_len);
	return 0;
}

u8 cam_set_parameters_by_block_8(u8 *para_buff)
{
    u8 i;
    u8 para_len;

    para_len = cam_block_length_arr[8];

    txd1_buffer[4] = 8;
    txd1_buffer[5] = 1;
    txd1_buffer[6] = 0;
    txd1_buffer[7] = para_len;
    
    for(i=0;i<para_len;i++)
        txd1_buffer[8+i] = *(para_buff+i);
    
    
    camera_buffer_send(0x28,4+para_len);
	return 0;
}



u8 cam_request_response(void)
{
    
    camera_buffer_send(0x10,0);
	return 0;
}



void camera_osd_key_emulate_mode(enum ON_OFF_TYPE mode)
{
    
    txd1_buffer[4] = mode;
    camera_buffer_send(0x58,1);
}

void camera_osd_key_emulate(enum osd_key_type type)
{
    
    txd1_buffer[4] = type;
    camera_buffer_send(0x59,1);
}

void camera_osd_switch(u8 mode)
{
    if(mode)
    {
       ; //camera_osd_key_emulate(OSD_ENTER_LONG);
        //camera_osd_key_emulate_mode(OFF_MODE);
        
    }
    else
    {
        camera_osd_key_emulate_mode(ON_MODE); 
        delay_X1ms(80);

        camera_osd_key_emulate(OSD_ENTER_LONG);
        delay_X1ms(300);
        //delay_X1ms(3000);
        //camera_osd_key_emulate(0x01);
        
    }
}

void cam_zoom_mode_set(enum ON_OFF_TYPE mode)
{
    u8 data = 0;
    
    if(mode == ON_MODE)
    {
        data = 1;
    }
    else
    {
        data = 0;
    }
    cam_set_parameters(5,0,1,&data);
}


void cam_zoom_menu_set(enum ON_OFF_TYPE mode)
{
    u8 data = 0;
    
    if(mode == ON_MODE)
    {
        data = 4;
    }
    else
    {
        data = 0;
    }
    cam_set_parameters(8,40,1,&data);
}

void cam_zoom_disp_set(enum ON_OFF_TYPE mode)
{
    u8 data = 0;
    
    if(mode == ON_MODE)
    {
        data = 2;
    }
    else
    {
        data = 0;
    }
    cam_set_parameters(8,2,1,&data);
}


void cam_zoom_data_set(u16 val)
{
    
#if 0
    u8 data[2];
	data[0] = val&0xff;

	data[1] = (val>>8)&0xff;
	cam_set_parameters(5,1,2,data);

#else
    u8 data;

#if 0
    data = val&0xff;
    cam_set_parameters(5,2,1,&data);
    
    data = (val>>8)&0xff;
    cam_set_parameters(5,1,1,&data);

#else
    data = val&0xff;
    cam_set_parameters(5,1,1,&data);
    
    data = (val>>8)&0xff;
    cam_set_parameters(5,2,1,&data);
#endif
#endif
}

u8 cam_zoom_data_get(u16 *val)
{
    u8 data;

    if(cam_get_parameters_oneByte(5,1,&data))
    {
        *val = data;
        
    }
    
    if(cam_get_parameters_oneByte(5,2,&data))
    {
        *val += data<<8;
        return 1;
    }

    return 0;
}


/*
0 : Off (Default)
1 : Low
2 : Middle
3 : High

*/
void cam_digital_wdr_set(u8 val)
{
    u8 data;

    data = val;
    cam_set_parameters(3,10,1,&data);
    
}

void cam_freeze_set(enum ON_OFF_TYPE val)
{
    u8 data;

    data = val;
    cam_set_parameters(8,38,1,&data);
    
}

u8 cam_freeze_get(u8 *buf)
{
    u8 data=0;
    
    if(cam_get_parameters_oneByte(8,38,buf))
    {
        data=1;
    }
    return data;
}


void cam_atw_level_set(u8 val)
{
    u8 data;

    data = val;
    cam_set_parameters(4,2,1,&data);
    
}

u8 cam_atw_level_get(u8 val)
{
    u8 data;

    cam_get_parameters_oneByte(4,2,&data);
    return data;
}

void cam_picture_reverse_set(enum picture_reverse_type val)
{
    u8 data;

	switch(val)
	{
	case PIC_MIRROR:
		data = 1;
		break;	
	case PIC_VFLIP:
		data = 0x02;
	break;	
	case PIC_ALL_FLIP:
		data = 0x03;
	break;	

	case PIC_NORMAL:
	default:
		data = 0;
	break;

	}
	
    cam_set_parameters(8,1,1,&data);
    
}

//val 1,60,30; 0,50,25
void cam_sensor_format_set(u8 val)
{
    u8 data,data1;

    if(cam_get_parameters_oneByte(8,0,&data1))
    {
            if(val)//30fps default
            {
                data = 0x10|data1;
            }
            else
            {
                data = (~0x10) & data1;
            }
            
    }
            
    cam_set_parameters(8,0,1,&data);
    
}

//val 1,pal; 0,ntsc
void cam_cvbs_mode_set(u8 val)
{
    u8 data,data1;

    if(cam_get_parameters_oneByte(8,0,&data1))
    {
            if(val)//pal
            {
                data = 0x01|data1;
            }
            else
            {
                data = (~0x01) & data1;
            }
            
    }
    {
        data = val;
    }
            
    cam_set_parameters(8,0,1,&data);
    
}

void cam_comm_menu_set(u8 val)// 关闭或者开启 通讯设置在菜单中的显示
{
    u8 data,data1;

    if(cam_get_parameters_oneByte(8,44,&data1))
    {
            if(val==ON_MODE)//pal
            {
                data = 0x20|data1;
            }
            else
            {
                data = (~0x20) & data1;
            }
            
    }
            
   // cam_set_parameters(8,44,1,&data);
    cam_set_parameters(8,44,1,0);
}


void cam_framerate_set(enum framerate_type val)
{
    u8 data;

    if((val != CAM_CVBS_PAL)&&(val != CAM_CVBS_NTSC) )
    {
    	switch(val)
    	{
    	case CAM_FRAMERATE_720P30:
            cam_sensor_format_set(1);
    		data = 2;
    		data = data<<4;	
    		break;
    	case CAM_FRAMERATE_720P60:
            cam_sensor_format_set(1);
    		data =0;
    		data = data<<4; 
    		break;
    	
    	case CAM_FRAMERATE_1080P30:
            cam_sensor_format_set(1);
    		data = 1;
    		data = data<<4;	
    		break;
    		
    	case CAM_FRAMERATE_1080I60:
            cam_sensor_format_set(1);
            data = 5;
    		data = data<<4;
            break;
            
        case CAM_FRAMERATE_720P25:
            cam_sensor_format_set(0);
            data = 2;
    		data = data<<4;
            break;

        case CAM_FRAMERATE_720P50:
            cam_sensor_format_set(0);
            data = 0;
    		data = data<<4;
            break;

        case CAM_FRAMERATE_1080P25:
            cam_sensor_format_set(0);
            data = 1;
    		data = data<<4;
            break;

        case CAM_FRAMERATE_1080I50:
            cam_sensor_format_set(0);
            data = 5;
    		data = data<<4;
            break;
            
    	default:

    	break;

    	}
	
        cam_set_parameters(8,35,1,&data);
    }
    else
    {
        if((val == CAM_CVBS_PAL))
        {
            cam_cvbs_mode_set(1);
        }
        else if(val == CAM_CVBS_NTSC)
        {

            cam_cvbs_mode_set(0);
        }
    }
    
}

void cam_frog_set(u8 val)
{
    u8 data;

	switch(val)
	{
	case 0:
		data = 0;
		break;
	case 1:
		data =1;
		break;
	
	case 2:
		data = 2;
		break;
		
	case 3:
	default:
		data = 3;
	break;

	}
	
    cam_set_parameters(3,1,1,&data);
    
}



//@breif brightness,luminance 设置，针对sensor TC-729,luminance 控制模式默认为ALC控制，以下控制协议采用ALC控制
//@para 0-8
void cam_brightness_set(u8 val,u8 mode)
{
    u8 data;

    if(val > 8)
        val = 8;
    data = val;

    if(mode==1)//alc
        cam_set_parameters(2,1,1,&data);
    else if(mode==2)
        cam_set_parameters(2,2,1,&data);   
        
}

u8 cam_brightness_get(u8 mode,u8 *reval)
{
    u8 data=0;

    if(mode==1)
    {
        if(cam_get_parameters_oneByte(2,1,reval))
        {
            data=1;
        }

    }
    else 
    {
        if(cam_get_parameters_oneByte(2,2,reval))
        {
            data = 1;

        }

    }
    return data;
}

//1,alc; 2,aes; 0,manual
u8 cam_brightness_mode_get(void)
{
    u8 data;

    cam_get_parameters_oneByte(2,0,&data);

    return data;
}

//1,alc; 2,aes; 0,manual
void cam_brightness_mode_set(u8 mode)
{
    u8 data;
    data = mode;
    cam_set_parameters(2,0,1,&data);   
}

u8 cam_agc_level_get(u16 *val)
{
	u8 recnt = 2;

	u8 i = 0;
	
	while(recnt--)
	{
		camera_buffer_send(0x33,0);
		//cam_request_response();
		delay_X1ms(50);
		if(rxd1_buff_cFlag)
		{
			*val = (rxd1_buffer[6]<<8)&0xff00;
			*val += (rxd1_buffer[5]&0x00ff);
			rxd1_buff_cFlag = 0;
			i = 1;
			break;
		}
	}

	return i;
}


void cam_cross_line_mode_set(enum ON_OFF_TYPE mode)
{
    u8 data = 0,data1;
    
    if(mode == ON_MODE)
    {
        data = 0x04;
    }
    else
    {
        data = 0;
    }

    if(cam_get_parameters_oneByte(8,2,&data1))
    {
            if(mode == ON_MODE)
            {
                data = 0x04|data1;
            }
            else
            {
                data = (~0x04) & data1;
            }
    }
    cam_set_parameters(8,2,1,&data);
}


void cam_color_bar_mode_set(enum ON_OFF_TYPE mode)
{
    u8 data = 0,data1;
    
    if(mode == ON_MODE)
    {
        data = 0x04;
    }
    else
    {
        data = 0;
    }

    if(cam_get_parameters_oneByte(8,2,&data1))
    {
            if(mode == ON_MODE)
            {
                data = 0x08|data1;
            }
            else
            {
                data = (~0x08) & data1;
            }
    }
    cam_set_parameters(8,2,1,&data);
}


void cam_nega_posi_mode_set(enum ON_OFF_TYPE mode)
{
    u8 data = 0,data1;
    
    if(cam_get_parameters_oneByte(8,1,&data1))
    {
            if(mode == ON_MODE)
            {
                data = 0x80|data1;
            }
            else
            {
                data = (~0x80) & data1;
            }
    }
    cam_set_parameters(8,1,1,&data);
}


u8 cam_cross_line_mode_get(u8 *buf)
{
    u8 data=0;
    
    if(cam_get_parameters_oneByte(8,2,buf))
    {
        data=1;
    }
    return data;
}



void cam_color_mode_set(enum ON_OFF_TYPE mode)
{
    u8 data = 0,data1;
    
    if(mode == OFF_MODE)
    {//color
        data = 0x00;
    }
    else
    {//b/w
        data = 0x0c;
    }

    if(cam_get_parameters_oneByte(4,0,&data1))
    {
        data = data|(data1&0x03);
  
    }
    cam_set_parameters(4,0,1,&data);
}


extern void delay_X1ms(uint m);

u8 cam_iris_value_get(void)
{
	u8 recnt = 2;

	u8 i = 0;
	
	while(recnt--)
	{
		camera_buffer_send(0x31,0);
		//cam_request_response();
		delay_X1ms(50);
		if(rxd1_buff_cFlag)
		{
			i = 1;
			break;
		}
	}

	return i;
}

u8 cam_iris_value_data_get(u16 *val)
{
	u8 recnt = 2;

	u8 i = 0;
	
	while(recnt--)
	{
		camera_buffer_send(0x31,0);
		//cam_request_response();
		delay_X1ms(50);
		if(rxd1_buff_cFlag)
		{
			*val = (rxd1_buffer[6]<<8)&0xff00;
			*val += (rxd1_buffer[5]&0x00ff);
			i = 1;
						rxd1_buff_cFlag = 0;

			break;
		}
	}

	return i;
}

u8 cam_shutter_value_get(u16 *val)
{
	u8 recnt = 2;

	u8 i = 0;
	
	while(recnt--)
	{
		camera_buffer_send(0x32,0);
		//cam_request_response();
		delay_X1ms(50);
		if(rxd1_buff_cFlag)
		{
			*val = (rxd1_buffer[6]<<8)&0xff00;
			*val += (rxd1_buffer[5]&0x00ff);
			i = 1;
			rxd1_buff_cFlag = 0;
			break;
		}
	}

	return i;
}




u8 cam_response_get(void)
{
	u8 recnt = 3;

	u8 i = 0;
	
	while(recnt--)
	{
		//camera_buffer_send(0x31,0);
		cam_request_response();
		delay_X1ms(50);
		if(rxd1_buff_cFlag)
		{
			i = 1;
			break;
		}
	}

	return i;
}

u8 cam_response_check(void)
{
	u16 recnt = 200;

	u8 i = 0;

    if(cam_recieve_cnt_public<300)
        recnt = cam_recieve_cnt_public*2 + recnt;
	while(recnt--)
	{
		delay_X1ms(20);
		if(rxd1_buff_cFlag)
		{
			i = 1;
			break;
		}
	}

	return i;
}

u8 cam_get_ID(u8 val)
{
	u16 i;

    if(cam_iris_value_get())
        return camera_id;

    camera_id=0;
	for(i=0;i<=0xFF;i++)
	{
		//camera_id = (u8)i;
		if(cam_iris_value_get())
		{
			
			camera_id = (u8)i;
			break;
			}

	}

	return camera_id;
}

void cam_init(void)
{
	cam_zoom_mode_set(OFF_MODE);
	delay_X1ms(80);
	cam_framerate_set(CAM_FRAMERATE_1080I60);
	delay_X1ms(80);
    cam_filt_cutter_set(0);//IR Filter cutter
    delay_X1ms(80);
    delay_half_second(1);
    cam_brightness_mode_set(2);
    delay_X1ms(80);
    cam_brightness_set(4,2);
}

#define CAM_DELAY_MAX       150
void get_cam_para_all(void)
{
            
    cam_get_parameters_by_block(0,&(system_para.system_para.para_cam_block0[0]));
    delay_X1ms(CAM_DELAY_MAX);
    cam_get_parameters_by_block(1,&(system_para.system_para.para_cam_block1[0]));
    delay_X1ms(CAM_DELAY_MAX);
    cam_get_parameters_by_block(2,&(system_para.system_para.para_cam_block2[0]));
    delay_X1ms(CAM_DELAY_MAX);
    cam_get_parameters_by_block(3,&(system_para.system_para.para_cam_block3[0]));
    delay_X1ms(CAM_DELAY_MAX);
    cam_get_parameters_by_block(4,&(system_para.system_para.para_cam_block4[0]));
    delay_X1ms(CAM_DELAY_MAX);
    cam_get_parameters_by_block(5,&(system_para.system_para.para_cam_block5[0]));
    delay_X1ms(CAM_DELAY_MAX);
    cam_get_parameters_by_block(6,&(system_para.system_para.para_cam_block6[0]));
    delay_X1ms(CAM_DELAY_MAX);
    cam_get_parameters_by_block(7,&(system_para.system_para.para_cam_block7[0]));
    delay_X1ms(CAM_DELAY_MAX);
    //cam_get_parameters_by_block(8,&(system_para.system_para.para_cam_block8[0]));
    cam_get_parameters_by_block_8(&(system_para.system_para.para_cam_block8[0]));
    delay_X1ms(CAM_DELAY_MAX);

}

void set_cam_para_all(void)
{
    cam_set_parameters_by_block(0,&(system_para.system_para.para_cam_block0[0]));
    delay_X1ms(CAM_DELAY_MAX);
    cam_set_parameters_by_block(1,&(system_para.system_para.para_cam_block1[0]));
    delay_X1ms(CAM_DELAY_MAX);
    cam_set_parameters_by_block(2,&(system_para.system_para.para_cam_block2[0]));
    delay_X1ms(CAM_DELAY_MAX);
    cam_set_parameters_by_block(3,&(system_para.system_para.para_cam_block3[0]));
    delay_X1ms(CAM_DELAY_MAX);
    cam_set_parameters_by_block(4,&(system_para.system_para.para_cam_block4[0]));
    delay_X1ms(CAM_DELAY_MAX);
    cam_set_parameters_by_block(5,&(system_para.system_para.para_cam_block5[0]));
    delay_X1ms(CAM_DELAY_MAX);
    cam_set_parameters_by_block(6,&(system_para.system_para.para_cam_block6[0]));
    delay_X1ms(CAM_DELAY_MAX);
    cam_set_parameters_by_block(7,&(system_para.system_para.para_cam_block7[0]));
    delay_X1ms(CAM_DELAY_MAX);
    //cam_set_parameters_by_block(8,&(system_para.system_para.para_cam_block8[0]));
    cam_set_parameters_by_block_8(&(system_para.system_para.para_cam_block8[0]));
    delay_X1ms(CAM_DELAY_MAX);

}

void save_cam_para_all(void)
{
    get_cam_para_all();
    
}

//=========================================================================================================


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
   {
        cam_recieve_cnt_public = cam_cmd_recieve_len;
    	rxd1_buff_cFlag=1;
    	counter_receive=0;
        cam_cmd_recieve_len=0;
    }

    if (counter_receive >= 200)
    	counter_receive = 0;
}


rt_sem_t	uart2_sem;

struct _uart_dev_my
{
	struct rt_semaphore rx_sem;

	rt_device_t device;
};

struct _uart_dev_my* uart2_dev_my;
static rt_err_t uart2_cam_rx_ind(rt_device_t dev, rt_size_t size)
{
    RT_ASSERT(uart2_dev_my != RT_NULL);

    /* release semaphore to let  thread rx data */
    rt_sem_release(&uart2_dev_my->rx_sem);

    return RT_EOK;
}
void uart2_cam_set_device(void)
{
	    rt_device_t dev = RT_NULL;

    dev = rt_device_find("uart2");
	
	if (dev == RT_NULL)
	{
        rt_kprintf("finsh: can not find device: %s\n", "uart2");
        return;
    }

    /* check whether it's a same device */
    if (dev == uart2_dev_my->device) return;
		
    if (rt_device_open(dev, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX |\
                       RT_DEVICE_FLAG_STREAM) == RT_EOK)
    {
        if (uart2_dev_my->device != RT_NULL)
        {
            /* close old finsh device */
            rt_device_close(uart2_dev_my->device);
            rt_device_set_rx_indicate(uart2_dev_my->device, RT_NULL);
        }

        uart2_dev_my->device = dev;
        rt_device_set_rx_indicate(dev, uart2_cam_rx_ind);
    }
}

void rt_cam_thread_entry(void* parameter)
{
    char ch;

	while (1)
    {
        /* wait receive */
        if (rt_sem_take(&uart2_dev_my->rx_sem, RT_WAITING_FOREVER) != RT_EOK) continue;

        /* read one character from device */
        while (rt_device_read(uart2_dev_my->device, 0, &ch, 1) == 1)
        {
            serial_int1_receive(ch);
        } /* end of device read */
    }	
	
}

int cam_uart1_init(void)
{
    rt_err_t result;
    rt_thread_t init_thread;

    uart2_dev_my = (struct _uart_dev_my*)rt_malloc(sizeof(struct _uart_dev_my));
	
    memset(uart2_dev_my, 0, sizeof(struct _uart_dev_my));
    rt_sem_init(&(uart2_dev_my->rx_sem), "camrx", 0, 0);

	uart2_cam_set_device();

		
	uart2_sem = rt_sem_create("uart2_sem",0, RT_IPC_FLAG_FIFO);  


	init_thread = rt_thread_create("camctl",rt_cam_thread_entry, RT_NULL,
                                   2048, 8, 21);
	  if (init_thread != RT_NULL)
        rt_thread_startup(init_thread);

    return 0;
}


