#include "includes.h"


uchar osdSwitchFlag = 0;//bit1=1:means f-osd has been opened,  bit0=0:closed(sanyo/ANT didn't have this bit)

uint inquiry_time =0;



void cam_osd_switch(void)
{

    if(!osdSwitchFlag)
    {
        camera_osd_switch(0);
    }
    else
    {
            
        camera_osd_switch(1);

    }


}


void cam_osd_stateInq(void)
{

}

void wait_one_command(uchar command)
{
 while(1)
	{
	 if (command_analysis())
		{
		 if (command_byte == command)
			break;
		}
  }
}

void wait_not_this_command(uchar command)
{
    while(1)
	{
        if (command_analysis())
		{
            if (command_byte != command)
                break;
		}
    }
}


u8 osd_menu_flag=0;//1,open,0,close
#define OSD_HANDLE_DELAY    1000
#if 1
void cam_osd_control(uchar type)//1:call camera osd directly; 0:call camera osd in dome osd
{
    uchar efficient_command = 0;

    if(osdSwitchFlag)
    {
            if (8 == command_byte) //if Tilt
		    {
				efficient_command = 1;
                wait_one_command(0x05); //wait for stop command from controller

                camera_osd_key_emulate(OSD_UP);
			}
            else if (9 == command_byte)
            {
				efficient_command = 1;
                                wait_one_command(0x05); //wait for stop command from controller

                camera_osd_key_emulate(OSD_DOWN);
            }
            else if (6 == command_byte) //left
		    {
			 	efficient_command = 1;
                                wait_one_command(0x05); //wait for stop command from controller

				camera_osd_key_emulate(OSD_LEFT);
			}
            else if (7 == command_byte)//right
            {
                
                efficient_command = 1;
                                wait_one_command(0x05); //wait for stop command from controller

            	camera_osd_key_emulate(OSD_RIGHT);
            }
            else if (0x0E == command_byte || 0x0F == command_byte) //if iris open/close
		    {
                if (0x0E == command_byte)  //"iris close" will close the OSD of camera of CNB and GP
                {
                    
                    {
                        camera_osd_key_emulate(OSD_ENTER);
                        wait_not_this_command(0x0E);
                    }
					
				}
				else
				{
					
					
					;//break;
				}
            
			}
    }

}

#else
void cam_osd_control(uchar type)//1:call camera osd directly; 0:call camera osd in dome osd
{
    uchar efficient_command = 0;


    cam_osd_switch();
    osdSwitchFlag |= 0x01;
    cam_osd_stateInq();// to inquiry osd state
    if (0x11 != command_byte && 57 != Rocket_fir_data )
    wait_one_command(0x05); //wait stop command
    inquiry_time=0;
    
	while(1)
	{
	    
		if ((osdSwitchFlag ==1)&&(inquiry_time>50))  // 25s≤È—Ø“ª¥Œ
		{  
            inquiry_time=0;
			delay_half_ms(80);
			 
		}
        
		if (!(osdSwitchFlag & 0x01))
    		break;

        //if (command_analysis())
        if(command_analysis())
		{
            if (8 == command_byte) //if Tilt
		    {
				efficient_command = 1;
                camera_osd_key_emulate(OSD_UP);

			}
            else if (9 == command_byte)
            {
				efficient_command = 1;
                camera_osd_key_emulate(OSD_DOWN);

            }
            else if (6 == command_byte) //left
		    {
			 	efficient_command = 1;
				camera_osd_key_emulate(OSD_LEFT);
			}
            else if (7 == command_byte)//right
            {
                
                efficient_command = 1;
            	camera_osd_key_emulate(OSD_RIGHT);
            }
            else if (0x0E == command_byte || 0x0F == command_byte) //if iris open/close
		    {
                if (0x0E == command_byte)  //"iris close" will close the OSD of camera of CNB and GP
                {
                    camera_osd_key_emulate(OSD_ENTER);
					
				}
				else
				{
					
					
					;//break;
				}
            
			}
            else if ((0x10 == command_byte||0x11 == command_byte) && (90 == Rocket_fir_data||95 == Rocket_fir_data) && type)//preset point command
		    {

                break;
			}


			if (efficient_command)
		    {
			 efficient_command = 0;
			 delay_half_ms(200);
			 delay_half_ms(100);
			if ((0x10 != command_byte)&&((1==osdSwitchFlag)||(3==osdSwitchFlag)))
			    wait_one_command(0x05); //wait stop command
				
			// cam_osd_stateInq();// to inquiry osd state
			}
					 
		}
	}

	osdSwitchFlag = 1;
	cam_osd_switch();
}

#endif

