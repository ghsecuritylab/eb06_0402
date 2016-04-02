
#include "includes.h"
#include "api.h"
#include "Motor_Config.h"
#include "Protocol.h"
#include "Queue.h"

#define uchar unsigned char

#define MOTOR_PAN_TILT_PRESET_REDUCE     1

#define PHOTOSWITCH_ENABLE (GPIOC->BSRR = GPIO_Pin_3)
#define PHOTOSWITCH_DISABLE (GPIOC->BRR = GPIO_Pin_3)


const uchar pt_code2motor[] = {2,0, 4,0, 0,3, 0,5, 2,3, 2,5, 4,3, 4,5, 0,0};

//----------------------------------------------------------------马达速度配置-----------------------

#define	PAN_MOTOR_SPEED_MAX		    0x3f
#define	TILT_MOTOR_SPEED_MAX		62

#if SYSTEM_CPU_CLOCK_FREQUENCE == SYSTEM_CPU_CLOCK_FREQUENCE_72M
#define	ONE_SECOND_COUNTER	(1150*3*1000)
#elif SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_48M
#define	ONE_SECOND_COUNTER	(1150*2*1000)

#elif SYSTEM_CPU_CLOCK_FREQUENCE==SYSTEM_CPU_CLOCK_FREQUENCE_16M
#define	ONE_SECOND_COUNTER	(1150*1*1000)

#endif

#define	TEN_SECOND_COUNTER  (ONE_SECOND_COUNTER*10)


//---------------------------------------------------------------------------------------------------


#define Pan_MOTOR_STOP 		TIM5->CR1 &= ((u16)0x03FE);               
#define Pan_MOTOR_START 	TIM5->CR1 |= ((u16)0x0001);
#define Tilt_MOTOR_STOP 	TIM3->CR1 &= ((u16)0x03FE);               
#define Tilt_MOTOR_START 	TIM3->CR1 |= ((u16)0x0001);

#define TIMER2_START  TIM4->CR1 |= ((u16)0x0001);
#define TIMER2_STOP   TIM4->CR1 &= ((u16)0x03FE);   
                   

#define DIRT_D 	GPIOC->BSRR = GPIO_Pin_8;
#define DIRT_U  GPIOC->BRR = GPIO_Pin_8;

#define DIRP_L 	GPIOC->BSRR = GPIO_Pin_6;
#define DIRP_R	GPIOC->BRR = GPIO_Pin_6;

#define STEP_TILT_H_NOR_L   ( ((u16)GPIOC->IDR) ^ GPIO_Pin_9)
#define Step_Tilt_L	(GPIOC->ODR = STEP_TILT_H_NOR_L)
#define Step_Tilt_H (GPIOC->ODR = STEP_TILT_H_NOR_L)


#define Step_Pan_H GPIOC->BSRR = GPIO_Pin_7;
#define Step_Pan_L GPIOC->BRR  = GPIO_Pin_7;

#define	DAC_NOT_INTEGER		0

#define XTAL 8

#define UART0_INT_ON 	USART1->CR1 |= ((u16)0x2000);
#define UART0_INT_OFF 	USART1->CR1 &= ((u16)0xDFFF);               
#define PANLEFT   0x01
#define PANRIGHT  0x02

#define TILTUP    0x08 //04 
#define TILTDOWN  0x04 //08

#define MOTOR_TILT_DELAY_MAX 30
#define MOTOR_PAN_DELAY_MAX 10

///////////////// ------- dometype -------------- ///////////////////////

//////////////////////////////////////////////////

u8 focus_run_state = 0,zoom_run_state=0;
u8 zoom_run_state_pre = 0;
u8 zoom_pos_limit_state = 0;


volatile MOTOR_PAN_STEPS_TYPE 	panpos;
volatile MOTOR_TILT_STEPS_TYPE 	tiltpos;

volatile MOTOR_PAN_STEPS_TYPE	pstepcount;
volatile MOTOR_TILT_STEPS_TYPE 	tstepcount;

u16	MotorTimeOut_2ms_Counter=0;
                     
static uchar Pan_motstate,Tilt_motstate;      

//extern MOTOR_PAN_STEPS_TYPE pstepcount;
//extern MOTOR_TILT_STEPS_TYPE tstepcount; 

extern uint zoomRate;   
extern uchar Rec_byte_com;

extern uchar PanTiltStopFlag1;
extern uchar command_byte;
extern uchar speed_limit;

extern uchar cameraType;

extern uchar cam_command_permit;

//////////////////////////////////
extern uint delay_3s;   // 在定时器0中计数 

static uint Pan_moto_speed;	  		//水平马达速度定时值
static uint Tilt_moto_speed;		//垂直马达速度定时值

static uint TPSC_Pan_Count;         //用于临时保存TOUR,Preset等在做加减速处理时的水平计数值
static uint TPSC_Tilt_Count;        //用于临时保存TOUR,Preset等在做加减速处理时的垂直计数值

static uchar Pan_move_or_posmove;
static uchar Tilt_move_or_posmove;

static uchar Pan_Tour_speed_class;
static uchar Tilt_Tour_speed_class;

static volatile  uchar SwitchFlag;           //有开关为1，无为0
static uchar TestFinish;
static uint MotorPanInitCom;

static volatile uchar PanSpeed;                 	// 输入水平马达速度等级， 共64级
static volatile uchar TiltSpeed;                	// 输入垂直马达速度等级， 共64级
static volatile uchar CurrentTiltSpeed;		   	// 当前垂直马达速度等级，共64级
static volatile uchar CurrentPanSpeed;         	// 当前水平马达速度等级，共64级

static uchar MotordirectionBufferForTwoOpintScan;  //
static uchar MotordirectionBufferForTwoOpintScan_tilt;  //

static uchar RockerComFlag;							//遥杆命令标志	

static uchar RockerComFlag_tilt;							//遥杆命令标志	


static uchar Panspeedbuff;              //水平速度暂存
uchar Uart0_rev_ena_for_flip;

///////////////////////////
static uchar CommandFlag;   

static uchar MotordirectionBuffer;    			//马达方向缓冲
static uchar MotordirectionBuffer_tilt;    			//马达方向缓冲


static uchar ZoomBuffer[12];         			//放大倍数的高字节

static uchar Motordirection;      				//马达方向
static uchar Motordirection_tilt;      				//马达方向

static uint DelayTimerBuffer;

static uchar Pan_motor_pos_flag;
static uchar Tilt_motor_pos_flag;
static uchar MotoMoveCount;
static uint MotoMoveSpeed;
//////////////////////////////////////////////////////////////////////////////////
void delay_1us(void);
void delay_1ms(void);
void delay_X1ms(uint m);

uchar MotorSpeedClass (uchar MotorSpeedKeyboardData);
void init_devices(void);
void MOTO_move(void); 
void MOTO_move_tilt(void);

void MotorPanInit(void);
void PositionMotorSpeedChange(void);


uchar RockerSpeed(uchar TiltPanSpeed,uchar CurrentTiltPanSpeed);
uchar RockerSpeed_Tilt(uchar TiltPanSpeed,uchar CurrentTiltPanSpeed);


extern void MotorDataInit (void);
extern void MotorControll (void);
extern void MotorStop (void);

extern void Keyboard_camera_porotocol(void);
extern uchar com_buffer[6];    //

void timer1_ovf_isr(void);
void timer2_ovf_isr(void);
void timer3_ovf_isr(void);
void Dir_Pos_focus(s16 Y);

/////////////////////////////////  0x6005,  ////////////////////////////////////////				   
                            /*    0xFF6C        */
const uint MotorSpeed[64]={0x0000,0xFF6C,0XAE9E,0xD74F,0xE8BF,0xEFB9,0xF45F,0xF6F5,   //这些是计算值
                           0xF89A,0xF9BD,0xFA93,0xFB59,0xFBEE,0xFC61,0xFCBE,0xFD0A,    
                           0xFD49,0xFD7E,0xFDD3,0xFDF6,0xFE16,0xFE31,0xFE60,0xFE85,    
						   0xFEA5,0xFEC0,0xFED7,0xFEEB,0xFEFC,0xFF0B,0xFF19,0xFF30,    
						   0xFF43,0xFF53,0xFF60,0xFF6C,0xFF76,0xFF7E,0xFF86,0xFF8D,   
						   0xFF93,0xFF9D,0xFFA6,0xFFAD,0xFFB3,0xFFB9,0xFFBD,0xFFC1,   
                           0xFFC5,0xFFC8,0xFFCB,0xFFD0,0xFFD4,0xFFD8,0xFFDC,0xFFE0,   
						   0xFFE3,0xFFE7,0xFFEA,0xFFED,0xFFEE,0xFFEF,0xFFF0,0xFFF1};  //fff1=140度

const uint MotorSpeed_Tilt[64]= /*  0xFE31    */
							{0x0000,0xFE31,0xFF20,0xD74F,0xE8BF,0xEFB9,0xF45F,0xF6F5,   //这些是计算值
							0xF89A,0xF9BD,0xFA93,0xFB59,0xFBEE,0xFC61,0xFCBE,0xFD0A,    
							0xFD49,0xFD7E,0xFDD3,0xFDF6,0xFE16,0xFE31,0xFE60,0xFE85,    
							0xFEA5,0xFEC0,0xFED7,0xFEEB,0xFEFC,0xFF0B,0xFF19,0xFF20,    
							0xFF43,0xFF53,0xFF60,0xFF6C,0xFF76,0xFF7E,0xFF86,0xFF8D,   
							0xFF93,0xFF9D,0xFFA6,0xFFAD,0xFFB3,0xFFB9,0xFFBD,0xFFC1,   
							0xFFC5,0xFFC8,0xFFCB,0xFFD0,0xFFD4,0xFFD8,0xFFDB,0xFFDE,   
							0xFFE0,0xFFE3,0xFFE5,0xFFE6,0xFFE9,0xFFEC,0xFFEE,0xFFF1};  //fff5=360度

/////////////////////////////////////////////////////////////////////////////////////

void TestDelayTime(void)
{
	while(1) {
		Step_Pan_H;
		delay_X1ms(1);
		Step_Pan_L;
		delay_X1ms(1);	
		}
}

#define	TILT_DOWN	0X05
#define	TILT_UP		0X03


#define	MOTOR_START_INIT	0xFF50

void focus_goto_pos_relative(u32 steps,enum ZF_TYPE mode)
{
	u32 temp;

#if 0
	if(mode == FOCUS_STOP)
	{
		focus_action(mode);
		return;
	}

	temp = tiltpos;
	
	if(mode == ZOOM_TELE)
	{
		tiltpos += steps;
		if(tiltpos > Pan_ground_count)
			tiltpos = Pan_ground_count;

		temp = tiltpos - temp;
	}
	else if(mode == ZOOM_WIDE)
		{
		tiltpos -= steps;
		if(tiltpos < 0)
			tiltpos = 0;

		temp = temp-tiltpos;
	
	}
#else

	temp = steps;
#endif

	while(temp--)
	{

		focus_action(mode);
		FOCUS_STEP_DELAY;
	}

	focus_action(FOCUS_STOP);
	
}


void MotorTilt_test(void)
{
    u16 focus_sensor_state = 0xff,focus_sensor_state_pre = 0xff;
	u8 i;


	TestFinish = 0x00;

	Tilt_moto_speed = 0;
	Pan_moto_speed = 0;


	while(1)
	{
    	focus_sensor_state = GET_FOCUS_POS_SENSOR_STATE;

        if (focus_sensor_state)
        {
            if (focus_sensor_state!= focus_sensor_state_pre && focus_sensor_state_pre!=0xff) 
		    {
		        tiltpos = FOCUS_SENSOR_POS_HIGH_TO_LOW_STEPS;
                break;
            }
            focus_action(FOCUS_NEAR);
        }
        else
        {
            if (focus_sensor_state!= focus_sensor_state_pre && focus_sensor_state_pre!=0xff) 
		    {
		        tiltpos = FOCUS_SENSOR_POS_HIGH_TO_LOW_STEPS;
                break;
            }
            focus_action(FOCUS_FAR);
        }

    	focus_sensor_state_pre= focus_sensor_state;

        FOCUS_STEP_DELAY;// adjust the focus motor speed
	}

    focus_action(FOCUS_STOP);
    delayms(100);
    
    tiltpos = FOCUS_SENSOR_POS_HIGH_TO_LOW_STEPS;

    focus_goto_pos_relative(FOCUS_SENSOR_POS_SENSOR_TO_FAR_STEPS,FOCUS_FAR);
    tiltpos = 0;  // 

    while(1)
	{
    	focus_sensor_state = GET_FOCUS_POS_SENSOR_STATE;

        if (focus_sensor_state)
        {
            tiltpos++;
            focus_action(FOCUS_NEAR);
        }
        else
        {
            focus_action(FOCUS_STOP);
            break;
        }

    	focus_sensor_state_pre= focus_sensor_state;

        FOCUS_STEP_DELAY;// adjust the focus motor speed
	}

    delayms(50);
    focus_sensor_state_pre = focus_sensor_state;
	while(1)
	{
    	focus_sensor_state = GET_FOCUS_POS_SENSOR_STATE;

        if (focus_sensor_state)
        {
            if (focus_sensor_state!= focus_sensor_state_pre && focus_sensor_state_pre!=0xff) 
		    {
		        tiltpos = FOCUS_SENSOR_POS_HIGH_TO_LOW_STEPS;
                break;
            }
            focus_action(FOCUS_NEAR);
        }
        else
        {
            if (focus_sensor_state!= focus_sensor_state_pre && focus_sensor_state_pre!=0xff) 
		    {
		        tiltpos = FOCUS_SENSOR_POS_HIGH_TO_LOW_STEPS;
                break;
            }
            focus_action(FOCUS_FAR);
        }

    	focus_sensor_state_pre= focus_sensor_state;

        FOCUS_STEP_DELAY;// adjust the focus motor speed
	}

    focus_goto_pos_relative(FOCUS_SENSOR_POS_SENSOR_TO_NEAR_STEPS,FOCUS_NEAR);
    tiltpos = 0;  // 

	while(1)
	{
    	focus_sensor_state = GET_FOCUS_POS_SENSOR_STATE;

        if (!focus_sensor_state)
        {
            tiltpos++;
            focus_action(FOCUS_FAR);
        }
        else
        {
            focus_action(FOCUS_STOP);
            break;
        }

    	focus_sensor_state_pre= focus_sensor_state;

        FOCUS_STEP_DELAY;// adjust the focus motor speed
	}



}



//在 FAR 尽头 tiltpos = 0
void MotorTiltInit(void)
{
    u16 focus_sensor_state = 0xff,focus_sensor_state_pre = 0xff;
	u8 i;



    //MotorTilt_test();


	TestFinish = 0x00;
	Tilt_moto_speed = 0;
	Pan_moto_speed = 0;


	while(1)
	{
    	focus_sensor_state = GET_FOCUS_POS_SENSOR_STATE;

        if (focus_sensor_state)
        {
            if (focus_sensor_state!= focus_sensor_state_pre && focus_sensor_state_pre!=0xff) 
		    {
		        tiltpos = FOCUS_SENSOR_POS_HIGH_TO_LOW_STEPS;
                break;
            }
            focus_action(FOCUS_NEAR);
        }
        else
        {
            if (focus_sensor_state!= focus_sensor_state_pre && focus_sensor_state_pre!=0xff) 
		    {
		        tiltpos = FOCUS_SENSOR_POS_HIGH_TO_LOW_STEPS;
                break;
            }
            focus_action(FOCUS_FAR);
        }

    	focus_sensor_state_pre= focus_sensor_state;

        FOCUS_STEP_DELAY;// adjust the focus motor speed
	}

    focus_action(FOCUS_STOP);
    delayms(100);
    
    tiltpos = FOCUS_SENSOR_POS_HIGH_TO_LOW_STEPS;

    focus_goto_pos_relative(FOCUS_SENSOR_POS_SENSOR_TO_FAR_STEPS,FOCUS_FAR);
    tiltpos = 0;  // 从FAR方向往NEAR方向，走到0点位置

   // focus_goto_pos_absolutely(FOCUS_SENSOR_POS_SENSOR_TO_FAR_STEPS,0,0);
    
    #if 0
#if 1
	focus_goto_pos_relative(MOTOR_TILT_STEP,FOCUS_FAR);

	for(i=0;i<200;i++)
	{
		focus_action(FOCUS_FAR);
		
		FOCUS_STEP_DELAY;

	}
	
	focus_sensor_state_pre = 0;
	while(1)
	{
		focus_sensor_state = GET_FOCUS_POS_SENSOR_STATE;

        if (focus_sensor_state)
        {
			focus_action(FOCUS_NEAR);
			FOCUS_STEP_DELAY;
			focus_sensor_state_pre++;
        }
		else
		{
			Tilt_moto_speed = 0;
			
		}

	}

#else
	focus_goto_pos_relative(MOTOR_TILT_STEP,FOCUS_NEAR);

	for(i=0;i<200;i++)
	{
		focus_action(FOCUS_NEAR);
		
		FOCUS_STEP_DELAY;

	}

	focus_sensor_state_pre = 0;
	while(1)
	{
		focus_sensor_state = GET_FOCUS_POS_SENSOR_STATE;

		if (!focus_sensor_state)
		{
			focus_action(FOCUS_FAR);
			FOCUS_STEP_DELAY;
			focus_sensor_state_pre++;
		}
		else
		{
			Tilt_moto_speed = 0;
			
		}

	}
#endif
#endif
}


void zoom_goto_pos_relative(u32 steps,enum ZF_TYPE mode)
{
	s32 temp;

	if(mode == ZOOM_STOP)
	{
		zoom_action(mode);
		return;
	}

	temp = panpos;
	
	if(mode == ZOOM_TELE)
	{
		panpos += steps;
		if(panpos > Pan_ground_count)
			panpos = Pan_ground_count;

		temp = panpos - temp;
	}
	else if(mode == ZOOM_WIDE)
		{
		panpos -= steps;
		if(panpos < 0)
			panpos = 0;

		temp = temp-panpos;
	
	}

	while(temp--)
	{

		zoom_action(mode);
		ZOOM_STEP_DELAY;
	}

	zoom_action(ZOOM_STOP);
	
}

//在WIDE , panpos=0
void MotorPanInit(void)
{
    u16 focus_sensor_state = 0xff,focus_sensor_state_pre = 0xff;

	u8 i = 0;


	TestFinish = 0x00;

	Tilt_moto_speed = 0;
	Pan_moto_speed = 0;

	while(1)
	{
    	focus_sensor_state = GET_ZOOM_POS_SENSOR_STATE;

        if (focus_sensor_state)
        {
            if (focus_sensor_state!= focus_sensor_state_pre && focus_sensor_state_pre!= 0xff) 
		    {
                break;
            }
            zoom_action(ZOOM_TELE);
        }
        else
        {
            if (focus_sensor_state!= focus_sensor_state_pre && focus_sensor_state_pre!= 0xff) 
		    {
                break;
            }
            zoom_action(ZOOM_WIDE);
        }

		
    	focus_sensor_state_pre= focus_sensor_state;

        ZOOM_STEP_DELAY;// adjust the focus motor speed
	}

    zoom_action(ZOOM_STOP);
    panpos = ZOOM_SENSOR_POS_HIGH_TO_LOW_STEPS;

	
	zoom_goto_pos_relative(ZOOM_SENSOR_POS_SENSOR_TO_WIDE_STEPS,ZOOM_WIDE);
    panpos = 0;
    
	
}

void MotorDataInit(void)
{
	u16 i;
	BSP_Motor_Init();

	UART0_INT_OFF;

	SwitchFlag = 0;
	TestFinish  = 0;
	Pan_motor_pos_flag = 0x00;

	TestFinish = 0;
	for (i=4;i>0;i--)
		delay_X1ms(500);
#if 1
	MotorTiltInit();
	delay_X1ms(500);
	MotorPanInit(); 
#endif

    Tilt_moto_speed = 0;
	com_buffer[5] = 0x00;
	com_buffer[3] = 0x00;
	com_buffer[4] = 0X00;  //自检速度
	panpos = 0;           //0点

    TestFinish = 1;
	delay_X1ms(200);	
	UART0_INT_ON;
#if PHOTOSWITCH_PROTECTING
	PHOTOSWITCH_DISABLE;
#endif
  
}


///////////////////////////////////////////////////////////////////////////////
void MotorControll(void)
{ 

	Pan_move_or_posmove = 0x55;
	
	if ((com_buffer[0] == 0x55)  && (com_buffer[1] != 0xA5)) 
    {       //摇杆输入   

        RockerComFlag = 1;

        Motordirection = 0x00;

        if ((com_buffer[1] == 0x02) && (com_buffer[3] == 0x03)&& (com_buffer[2] != 0x00) && (com_buffer[4] != 0x00)) 			//LeftUp
            Motordirection = 1;
        else if ((com_buffer[1] == 0x02) && (com_buffer[3] == 0x05)&& (com_buffer[2] != 0x00) && (com_buffer[4] != 0x00)) 	//LeftDown	
            Motordirection = 2;
        else if ((com_buffer[1] == 0x04) && (com_buffer[3] == 0x03)&& (com_buffer[2] != 0x00) && (com_buffer[4] != 0x00))  	//RightUp
            Motordirection = 3;
        else if ((com_buffer[1] == 0x04) && (com_buffer[3] == 0x05)&& (com_buffer[2] != 0x00) && (com_buffer[4] != 0x00)) 	//RightDown
            Motordirection = 4;
        else if (Motordirection == 0x00) 
        {			
            switch (com_buffer[1]) 
            {
            case 0x02: 
            	  			//Left
            if((com_buffer[2] != 0x00))
            	Motordirection = 5;
               break;
               
            case 0x04: 
            	if((com_buffer[2] != 0x00))
               Motordirection = 6;	 						//Right
               break;
            default:   break;
            }
        }

        if (Motordirection == 0x00) 
        {	
            switch (com_buffer[3]) 
            {
            case 0x03://Up
            if((com_buffer[4] != 0x00))
            Motordirection = 7;
            break; 

            case 0x05: 											//Down
            if((com_buffer[4] != 0x00))
            	Motordirection = 8;
            break;

            default:   
            	break;
            }
        }  

        if (MotordirectionBuffer != Motordirection)
		{

			RockerComFlag = 1;
			if (0x00 == MotordirectionBuffer)
				MotordirectionBuffer = MotordirectionBufferForTwoOpintScan;
			MotorTimeOut_2ms_Counter = 0x00;

			CurrentTiltSpeed = 0;
			MotordirectionBuffer = Motordirection;

			if ((Motordirection > 0) && (Motordirection < 5) && 		//LeftUp,LeftDown,RightUp,RightDown
			(com_buffer[1] != 0x00) && (com_buffer[3] != 0x00) && (com_buffer[2] != 0x00) && (com_buffer[4] != 0x00))
			{
				APP_Motor_ChangePanSpeed(0);     				//startup speed
				Pan_MOTOR_START;

			}
			else if ((Motordirection < 7) && (com_buffer[1] != 0x00) && (com_buffer[2] != 0x00))	   //Left, Right
			 {
				APP_Motor_ChangePanSpeed(0);  		//startup speed
				Pan_MOTOR_START;
			 } 
			else if ((Motordirection < 9) && (com_buffer[3] != 0x00) && (com_buffer[4] != 0x00))	   //Up, Down
			 {
			 }

		} 

        if ((Motordirection > 0) && (Motordirection < 5) && 
            (com_buffer[1] != 0x00) && (com_buffer[3] != 0x00))
        {//水平和垂直同时转动时的速度限制
            if (com_buffer[2] > 0x30)
            com_buffer[2] = 0x30;

            if (com_buffer[4] > 38)
            	com_buffer[4] = 38;
        }
        
        com_buffer[2] = MotorSpeedClass(com_buffer[2]);
        com_buffer[4] = MotorSpeedClass(com_buffer[4]);

        if (com_buffer[3] != 0x00)                                 //垂直马达动作
            TiltSpeed = com_buffer[4];    	

        if (com_buffer[1] != 0x00)                            //水平马达动作
            PanSpeed = com_buffer[2]; 

    }
    else if (com_buffer[0] == 0xAA) 
    {   //AA                // PRESET
        com_buffer[0]  = 0;
        RockerComFlag = 0;
        Pan_move_or_posmove = 0x00;
        Tilt_move_or_posmove = 0x00;
        Call_presetpoint_flag = 0x01;
        Dir_Pos_focus (tstepcount); 
    }
    else if (!(com_buffer[5] == 0x80))
    {
        Pan_MOTOR_STOP;
        Tilt_MOTOR_STOP;
    }
}

void focus_MotorControll(void)
{ 

	Tilt_move_or_posmove = 0x55;
	if ((com_buffer_tilt[0] == 0x55)  && (com_buffer_tilt[1] != 0xA5)) 
    {       //摇杆输入   

        RockerComFlag_tilt = 1;

        Motordirection_tilt = 0x00;

        if ((com_buffer_tilt[1] == 0x02) && (com_buffer_tilt[3] == 0x03)&& (com_buffer_tilt[2] != 0x00) && (com_buffer_tilt[4] != 0x00)) 			//LeftUp
            Motordirection_tilt = 1;
        else if ((com_buffer_tilt[1] == 0x02) && (com_buffer_tilt[3] == 0x05)&& (com_buffer_tilt[2] != 0x00) && (com_buffer_tilt[4] != 0x00)) 	//LeftDown	
            Motordirection_tilt = 2;
        else if ((com_buffer_tilt[1] == 0x04) && (com_buffer_tilt[3] == 0x03)&& (com_buffer_tilt[2] != 0x00) && (com_buffer_tilt[4] != 0x00))  	//RightUp
            Motordirection_tilt = 3;
        else if ((com_buffer_tilt[1] == 0x04) && (com_buffer_tilt[3] == 0x05)&& (com_buffer_tilt[2] != 0x00) && (com_buffer_tilt[4] != 0x00)) 	//RightDown
            Motordirection_tilt = 4;
        else if (Motordirection_tilt == 0x00) 
        {			
            switch (com_buffer_tilt[1]) 
            {
            case 0x02: 
            	  			//Left
            if((com_buffer_tilt[2] != 0x00))
            	Motordirection_tilt = 5;
               break;
               
            case 0x04: 
            	if((com_buffer_tilt[2] != 0x00))
               Motordirection_tilt = 6;	 						//Right
               break;
            default:   break;
            }
        }

        if (Motordirection_tilt == 0x00) 
        {	
            switch (com_buffer_tilt[3]) 
            {
            case 0x03://Up
            if((com_buffer_tilt[4] != 0x00))
            Motordirection_tilt = 7;
            break; 

            case 0x05: 											//Down
            if((com_buffer_tilt[4] != 0x00))
            	Motordirection_tilt = 8;
            break;

            default:   
            	break;
            }
        }  

        if (MotordirectionBuffer_tilt != Motordirection_tilt)
        {
        	
			RockerComFlag_tilt = 1;
			if (0x00 == MotordirectionBuffer_tilt)
				MotordirectionBuffer_tilt = MotordirectionBufferForTwoOpintScan_tilt;
			MotorTimeOut_2ms_Counter = 0x00;

			CurrentTiltSpeed = 0;
			MotordirectionBuffer_tilt = Motordirection_tilt;
			

           if ((Motordirection_tilt < 9) && (com_buffer_tilt[3] != 0x00) && (com_buffer_tilt[4] != 0x00))	   //Up, Down
            {
                APP_Motor_ChangeTiltSpeed(0);		//startup speed
                Tilt_MOTOR_START;
            }
        } 

      //  com_buffer[2] = MotorSpeedClass(com_buffer[2]);
        com_buffer_tilt[4] = MotorSpeedClass(com_buffer_tilt[4]);

        if (com_buffer_tilt[3] != 0x00)                                 //垂直马达动作
            TiltSpeed = com_buffer_tilt[4];    	

    }
    else if (com_buffer_tilt[0] == 0xAA) 
    {   //AA                // PRESET
        com_buffer_tilt[0]  = 0;
        RockerComFlag_tilt = 0;
        Tilt_move_or_posmove = 0x00;
        Call_presetpoint_flag = 0x01;
        Dir_Pos_focus (tstepcount); 
    }
    else if (!(com_buffer_tilt[5] == 0x80))
    {
        Tilt_MOTOR_STOP;
    }
}
/////////////////////////////////////////////////////////////////////
//TIMER1 initialisation - prescale:64
// desired value: 1uSec
// actual value: Out of range
void timer1_init(void)
{
	BSP_Timer3_Init();      
}



//由函数 void APP_TIM3_IRQHandler(void)调用
//执行时间大约为 11us
__inline void timer1_ovf_isr(void)	   
{
	static u8 focus_sensor_state = 0xff,focus_sensor_state_pre = 0xff;

	if (RockerComFlag_tilt) 
    {
		CurrentTiltSpeed = 1;
		MOTO_move_tilt();
    }

	APP_Motor_ChangeTiltSpeed(0xFFFF-Tilt_moto_speed-1);

	focus_sensor_state_pre= focus_sensor_state;
	focus_sensor_state = GET_FOCUS_POS_SENSOR_STATE;
	
	if (focus_sensor_state != focus_sensor_state_pre && focus_sensor_state_pre!=0xff) 
	{
		SwitchFlag = 1;
		tiltpos = FOCUS_SENSOR_POS_HIGH_TO_LOW_STEPS;
	}
	else 
	{

	} 
	
 /*****************************************************************************************/ 
 /*遥杆命令*/
	if (Tilt_move_or_posmove == 0x55) 
	{
		if ((Tilt_motstate == TILTUP) && (tiltpos > 0)) 
		{
			focus_action(FOCUS_FAR);
			tiltpos--;
		}
		else if ((Tilt_motstate == TILTDOWN) && (tiltpos < Tilt_quarter_ground_count)) 
        {
            focus_action(FOCUS_NEAR);
            tiltpos++; 
        }
		else
			{
			focus_action(FOCUS_STOP);
		}
    }
    else if (Tilt_move_or_posmove == 0xAA) 
    {
        if (tstepcount) 
        {
            tstepcount--;

            if ((Tilt_motstate == TILTUP) && (tiltpos > 0)) 
            {
                tiltpos--;
                focus_action(FOCUS_FAR);
            }
            else if ((Tilt_motstate == TILTDOWN) && (tiltpos < Tilt_quarter_ground_count)) 
            {
                focus_action(FOCUS_NEAR);
                tiltpos++; 
            }
        }
        else 
        {
            Tilt_MOTOR_STOP;
            Tilt_moto_speed = 0;
            TPSC_Tilt_Count = 0;
            Tilt_moto_speed = 0x00;
            MotordirectionBuffer = 0x00;
            Tilt_Tour_speed_class = 0x00;
            Tilt_move_or_posmove = 0;
            APP_Motor_ChangeTiltSpeed(0xffff); 
            Tilt_motor_pos_flag = 0x01;
            focus_action(FOCUS_STOP);
        }
    }
   /*****************************************************************************************/
}

//////////////////////////////////////////////////////////////////////////////
//函数功能:        ZOOM motor     初始化时的定位
//入口参数:              无
//出口参数:              无
//模块调用:              delay_X1ms()
//全局变量调用:           
//////////////////////////


void timer3_ovf_isr(void)	 
{
//	uint timer3_com;
//    u16 temp;
    
//	static uchar PanLeftFlag;
//	static uchar PanRightFlag;
//	static uchar OnePushEmendationFinish;

	static u8 zoom_sensor_state = 0xff,zoom_sensor_state_pre = 0xff;
	
	
	if (RockerComFlag)
	{                             // 
		if ((CurrentPanSpeed != PanSpeed))
		{                              
			CurrentPanSpeed = 1;//RockerSpeed( PanSpeed,CurrentPanSpeed); 	 	//加减速控制
			Pan_motstate = 0;                      
		} 
		MOTO_move();                               //实现方向

	}

	APP_Motor_ChangePanSpeed(0xFFFF-Pan_moto_speed);
 
	MotorPanInitCom++;
    
	zoom_sensor_state_pre = zoom_sensor_state;
	zoom_sensor_state = GET_ZOOM_POS_SENSOR_STATE;
	
	if (zoom_sensor_state != zoom_sensor_state_pre && zoom_sensor_state_pre!=0xff) 
	{
		SwitchFlag = 1;
		panpos = ZOOM_SENSOR_POS_HIGH_TO_LOW_STEPS;//ZOOM_SENSOR_POS_SENSOR_TO_WIDE_STEPS
	}
	else 
	{

	} 
/*****************************************************************************************/ 	                          
    if (Pan_move_or_posmove == 0xAA) 
    {
        if (pstepcount)
        {
            if (Pan_motstate == PANLEFT && panpos<Pan_ground_count)
    		{            
                panpos++;
    			zoom_action(ZOOM_TELE);
            }
            else
            {
                panpos--;
    			zoom_action(ZOOM_WIDE);
            }

            pstepcount--;
        }
        else
        {
            MotordirectionBuffer = 0x00;
            Pan_MOTOR_STOP;
            Pan_moto_speed = 0x00;
            TPSC_Pan_Count = 0;
            Pan_motstate = 0;
            APP_Motor_ChangePanSpeed(0xFFFF); 
            Pan_motor_pos_flag = 0x01;
            Pan_move_or_posmove = 0;
            
            zoom_action(ZOOM_STOP);
        } 
    }
    else if (Pan_move_or_posmove == 0x55)  	//遥杆动作
	{
		if (Pan_motstate == PANLEFT && panpos<Pan_ground_count)
		{
			panpos++;
			#if 1
			zoom_action(ZOOM_TELE);
			#else
			zoom_action(ZOOM_WIDE);
			#endif
		}
		else if (Pan_motstate == PANRIGHT && panpos > 0)
		{
			panpos--;
			#if 1
			zoom_action(ZOOM_WIDE);
			#else
			zoom_action(ZOOM_TELE);
			#endif
			
		}
		else
		{
            zoom_pos_limit_state = 1;
			zoom_action(ZOOM_STOP);
		}
	}
         
/*****************************************************************************************/
}


void APP_TIM3_IRQHandler(void)
{
	 if (((TIM3->SR & TIM_IT_Update) != (u16)RESET) && ((TIM3->DIER & TIM_IT_Update) != (u16)RESET))
	{
		TIM3->DIER &= (u16)~TIM_IT_Update;
	   	timer1_ovf_isr();
		TIM3->DIER |= TIM_IT_Update;
		TIM3->SR = (u16)~TIM_IT_Update;
	}	
}




void APP_TIM5_IRQHandler(void)
{
	if (((TIM5->SR & TIM_IT_Update) != (u16)RESET) && ((TIM5->DIER & TIM_IT_Update) != (u16)RESET))
	{	
		//TIM_ITConfig(TIM5, TIM_IT_Update, DISABLE);
		TIM5->DIER &= (u16)~TIM_IT_Update;
	   	timer3_ovf_isr();
//		TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
		TIM5->DIER |= TIM_IT_Update;
//		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
		TIM5->SR = (u16)~TIM_IT_Update;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////
//摇杆输入命令处理

__inline void MOTO_move(void)
{
	static uchar MotoMovePanTilt;

	Pan_motstate = 0x00; 
	Tilt_motstate = 0x00;      
	switch (MotordirectionBuffer) 
	{
	case 0x01:  
		  //pan 
	  Pan_moto_speed = MotorSpeed[CurrentPanSpeed];
	  //tilt
	  Tilt_moto_speed = MotorSpeed_Tilt[CurrentTiltSpeed];    
	  Pan_motstate     = PANLEFT;
	  Tilt_motstate     = TILTUP;          
	  DIRP_L;                    
	  DIRT_U; 
	  break;                                   
	case 0x02:   
	  //pan 
	  Pan_moto_speed = MotorSpeed[CurrentPanSpeed];
	  //tilt
	  Tilt_moto_speed = MotorSpeed_Tilt[CurrentTiltSpeed];  
	  Pan_motstate     = PANLEFT;
	  Tilt_motstate     = TILTDOWN;  
	  DIRP_L;                    
	  DIRT_D; 
	  break;                                                     
	case 0x03: 
	  //pan 
	  Pan_moto_speed = MotorSpeed[CurrentPanSpeed];
	  //tilt
	  Tilt_moto_speed = MotorSpeed_Tilt[CurrentTiltSpeed]; 
	  Pan_motstate     = PANRIGHT;
	  Tilt_motstate     = TILTUP;
	  DIRP_R;                    
	  DIRT_U; 
	  break;
	case 0x04:                                                     
	  //pan   
	  Pan_moto_speed = MotorSpeed[CurrentPanSpeed];
	  //tilt
	  Tilt_moto_speed = MotorSpeed_Tilt[CurrentTiltSpeed];      
	  Pan_motstate     = PANRIGHT;
	  Tilt_motstate     = TILTDOWN;
	  DIRP_R;                    
	  DIRT_D;    
	  break;                	
	case 0x05:  
	  Pan_motstate     = PANLEFT; 
	  MotordirectionBufferForTwoOpintScan = MotordirectionBuffer;
	  Pan_moto_speed = MotorSpeed[CurrentPanSpeed];
	  DIRP_L;                           
	  break;
	case 0x06: 
	  Pan_motstate     = PANRIGHT; 
	  MotordirectionBufferForTwoOpintScan = MotordirectionBuffer;
	  Pan_moto_speed = MotorSpeed[CurrentPanSpeed];    
	  DIRP_R; 
	  break;		
	 case 0x07:   
	  Tilt_motstate = TILTUP;           
	  Tilt_moto_speed = MotorSpeed_Tilt[CurrentTiltSpeed];     
	  DIRT_U; 
	  break;
	case 0x08:   
	  Tilt_motstate=TILTDOWN; 
	  Tilt_moto_speed = MotorSpeed_Tilt[CurrentTiltSpeed];   
	  DIRT_D;                    
	  break;		
	default: break;  	 
	}	

    delay_1us();    //增加延时，防止改变马达转动方向未能与转动脉冲同步
  
	if (MotoMovePanTilt != com_buffer[1]) 
    {
	 MotoMoveCount = 0x00;
     MotoMovePanTilt = com_buffer[1];
     MotoMoveSpeed = com_buffer[2];            //MotoMoveCom;
     //delay_3s = 0; 
    }
	else if (MotoMoveSpeed != Panspeedbuff) 
	{
	//delay_3s = 0;
	MotoMoveCount = 0x00;
	MotoMoveSpeed = com_buffer[2];
	}

	delay_1us(); 
	delay_1us(); 
}



__inline void MOTO_move_tilt(void)
{
	static uchar MotoMovePanTilt;

	Tilt_motstate = 0x00;      
	switch (MotordirectionBuffer_tilt) 
	{
	case 0x01:  
	  Tilt_moto_speed = MotorSpeed_Tilt[CurrentTiltSpeed];    
	  Tilt_motstate     = TILTUP;          
	  DIRT_U; 
	  break;                                   
	case 0x02:   
	  //tilt
	  Tilt_moto_speed = MotorSpeed_Tilt[CurrentTiltSpeed];  
	  Tilt_motstate     = TILTDOWN;  
	  DIRT_D; 
	  break;                                                     
	case 0x03: 
	  //tilt
	  Tilt_moto_speed = MotorSpeed_Tilt[CurrentTiltSpeed]; 
	  Tilt_motstate     = TILTUP;
	  DIRT_U; 
	  break;
	case 0x04:                                                     
	  //tilt
	  Tilt_moto_speed = MotorSpeed_Tilt[CurrentTiltSpeed];      
	  Tilt_motstate     = TILTDOWN;
	  DIRT_D;    
	  break;                	
	case 0x05:  
	  break;
	case 0x06: 
	  break;		
	 case 0x07:   
	  Tilt_motstate = TILTUP;           
	  Tilt_moto_speed = MotorSpeed_Tilt[CurrentTiltSpeed];     
	  DIRT_U; 
	  break;
	case 0x08:   
	  Tilt_motstate=TILTDOWN; 
	  Tilt_moto_speed = MotorSpeed_Tilt[CurrentTiltSpeed];   
	  DIRT_D;                    
	  break;		
	default: break;  	 
	}	

    delay_1us();    //增加延时，防止改变马达转动方向未能与转动脉冲同步
  
	if (MotoMovePanTilt != com_buffer_tilt[1]) 
    {
	 MotoMoveCount = 0x00;
     MotoMovePanTilt = com_buffer_tilt[1];
     MotoMoveSpeed = com_buffer_tilt[2];            //MotoMoveCom;
     //delay_3s = 0; 
    }
	else if (MotoMoveSpeed != Panspeedbuff) 
	{
	//delay_3s = 0;
	MotoMoveCount = 0x00;
	MotoMoveSpeed = com_buffer_tilt[2];
	}

	delay_1us(); 
	delay_1us(); 
}



/////////////////////////////////////////////////////////////////////
__inline void StoredPanSpeedOrientation(void)
{
//  if (delay_3s > 1200)   //3S
//    {
// 	MotoMoveCount = 0x01;
// 	 if (MotoMoveSpeed > 60)
// 		MotoMoveSpeed = 60;
// 	rotation_speed = MotoMoveSpeed;
//     rotation_direc = com_buffer[1];
// 	delay_3s = 0;
//    } 
}


/////////////////////////////////////////////////////////////////////////////
void MotorStop (void)
{
   com_buffer[0] = 0;
   com_buffer[1] = 0;
   com_buffer[3] = 0;
   
   PanTiltStopFlag1 = 1;                //机没有收到

    RockerComFlag = 1;
    if (0x00 == MotordirectionBuffer)
        MotordirectionBuffer = MotordirectionBufferForTwoOpintScan;
    MotorTimeOut_2ms_Counter = 0x00;


    MotorTimeOut_2ms_Counter = 0;

    if(zoom_run_state)
    {
        Pan_MOTOR_STOP;
		zoom_action(ZOOM_STOP);
        zoom_run_state = 0;
    }

    if(focus_run_state)
    {
		focus_action(FOCUS_STOP);
	
        Tilt_MOTOR_STOP;
        focus_run_state = 0;
    }
   /*************************************************************************/

    delay_X1ms(100);
    RockerComFlag = 0;
    CurrentPanSpeed = 0;
    CurrentTiltSpeed = 0;
    PanSpeed = 0;
    TiltSpeed = 0;
    MotoMoveCount = 0x00;
    Pan_moto_speed = 0;
    Tilt_moto_speed = 0;
    Pan_move_or_posmove = 0;
    Tilt_move_or_posmove = 0;
    TPSC_Pan_Count = 0;
    TPSC_Tilt_Count =0;
    Pan_motstate = 0;
    Tilt_motstate = 0;
    MotordirectionBuffer = 0;
} 


////////////////////////////////////////////////////////////////
//

uchar MotorSpeedClass (uchar MotorSpeedKeyboardData)
{

  return 1;	
}





__inline uchar RockerSpeed(uchar TiltPanSpeed,uchar CurrentTiltPanSpeed)
{
static uint RocCom;
//static uchar roccom2;
 
  if (CurrentTiltPanSpeed < TiltPanSpeed)			//
    {
	if (CurrentTiltPanSpeed >= 61) 
	        {
		     RocCom++;  
	         if (RocCom >= CurrentTiltPanSpeed + 300)   //30
	           {                                    //300
		        CurrentTiltPanSpeed++;
		        RocCom = 0;
	           }
	         }
	  else if (CurrentTiltPanSpeed >= 60) 
	        {
		     RocCom++;  
	         if (RocCom >= CurrentTiltPanSpeed + 100)   //30
	           {                                    //300
		        CurrentTiltPanSpeed++;
		        RocCom = 0;
	           }
	         }       
	 else if (CurrentTiltPanSpeed >= 50) 
	        {
		     RocCom++;  
	         if (RocCom >= CurrentTiltPanSpeed + 10)   //30
//	         if (RocCom >= CurrentTiltPanSpeed + 5)   //30
	           {                                    //300
		        CurrentTiltPanSpeed++;
		        RocCom = 0;
	           }
	         }
	 else if (CurrentTiltPanSpeed >= 40) 
	        {
		     RocCom++;  
	         if (RocCom >= CurrentTiltPanSpeed+5  )   //30
//	         if (RocCom >= CurrentTiltPanSpeed-20)   //30
	           {                                    //300
		        CurrentTiltPanSpeed++;
		        RocCom = 0;
	           }
	        }
	 else if (CurrentTiltPanSpeed >= 30) 
	        {
		     RocCom++;  
	         //if (RocCom >= CurrentTiltPanSpeed)   //30
	         if (RocCom >= CurrentTiltPanSpeed-27)   //30
	           {                                    //300
		        CurrentTiltPanSpeed++;
		        RocCom = 0;
	           }
	        }
     else {
	       if (TiltPanSpeed > 30)
	          CurrentTiltPanSpeed = 30;
	       else CurrentTiltPanSpeed = TiltPanSpeed;  
		  }
    }
	////////////////////
  else if (CurrentTiltPanSpeed > TiltPanSpeed)	//
         {
		if (CurrentTiltPanSpeed >= 61) 
	            {
		         RocCom++;  
	             if (RocCom >= CurrentTiltPanSpeed +300)   //30
//	             if (RocCom >= CurrentTiltPanSpeed - 25)   //30
	               {                                    //300
		            CurrentTiltPanSpeed--;
		            RocCom = 0;
	               }
	            }
	      else if (CurrentTiltPanSpeed >= 60) 
	            {
		         RocCom++;  
	             if (RocCom >= CurrentTiltPanSpeed +20)   //30
//	             if (RocCom >= CurrentTiltPanSpeed - 25)   //30
	               {                                    //300
		            CurrentTiltPanSpeed--;
		            RocCom = 0;
	               }
	            }      
		 else if (CurrentTiltPanSpeed >= 50) 
	            {
		         RocCom++;  
	             if (RocCom >= CurrentTiltPanSpeed -20 )   //30
//	             if (RocCom >= CurrentTiltPanSpeed - 25)   //30
	               {                                    //300
		            CurrentTiltPanSpeed--;
		            RocCom = 0;
	               }
	            }
	     else if (CurrentTiltPanSpeed >= 40) 
	            {
		         RocCom++;  
	             if (RocCom >= CurrentTiltPanSpeed - 30)   //30
//	             if (RocCom >= CurrentTiltPanSpeed - 32)   //30
	               {                                    //300
		            CurrentTiltPanSpeed--;
		            RocCom = 0;
	               }
	            }
		 else if (CurrentTiltPanSpeed >= 30) 
	            {
		         RocCom++;  
	             if (RocCom >= CurrentTiltPanSpeed -27)   //30
	               {                                    //300
		            CurrentTiltPanSpeed--;
		            RocCom = 0;
	               }
	            }
        else CurrentTiltPanSpeed = TiltPanSpeed; 
	   }
  return CurrentTiltPanSpeed;	  
}


__inline uchar RockerSpeed_Tilt(uchar TiltPanSpeed,uchar CurrentTiltPanSpeed)
{
static uint RocCom;
//static uchar roccom2;
 
  if (CurrentTiltPanSpeed < TiltPanSpeed)			//当前的速度等级小于输入的速度等级(加速）
    {
	
	 if (CurrentTiltPanSpeed >= 60) 
	        {
		     RocCom++;  
	         if (RocCom >= CurrentTiltPanSpeed - 30 )   //30
	           {                                    //300
		        CurrentTiltPanSpeed++;
		        RocCom = 0;
	           }
	         }       
	 else if (CurrentTiltPanSpeed >= 50) 
	        {
		     RocCom++;  
	         if (RocCom >= CurrentTiltPanSpeed-20)   //30
//	         if (RocCom >= CurrentTiltPanSpeed + 5)   //30
	           {                                    //300
		        CurrentTiltPanSpeed++;
		        RocCom = 0;
	           }
	         }
	 else if (CurrentTiltPanSpeed >= 40) 
	        {
		     RocCom++;  
	         if (RocCom >= CurrentTiltPanSpeed-30  )   //30
//	         if (RocCom >= CurrentTiltPanSpeed-20)   //30
	           {                                    //300
		        CurrentTiltPanSpeed++;
		        RocCom = 0;
	           }
	        }
	 else if (CurrentTiltPanSpeed >= 30) 
	        {
		     RocCom++;  
	         if (RocCom >= CurrentTiltPanSpeed-27)   //30
//	         if (RocCom >= CurrentTiltPanSpeed-28)   //30
	           {                                    //300
		        CurrentTiltPanSpeed++;
		        RocCom = 0;
	           }
	        }
     else {
	       if (TiltPanSpeed > 30)
	          CurrentTiltPanSpeed = 30;
	       else CurrentTiltPanSpeed = TiltPanSpeed;  
		  }
    }
	////////////////////
  else if (CurrentTiltPanSpeed > TiltPanSpeed)	//（减速），当前的速度等级大于输入的速度等级
         {
		  
	     if (CurrentTiltPanSpeed >= 60) 
	            {
		         RocCom++;  
	             if (RocCom >= CurrentTiltPanSpeed-50 )   //30
//	             if (RocCom >= CurrentTiltPanSpeed - 25)   //30
	               {                                    //300
		            CurrentTiltPanSpeed--;
		            RocCom = 0;
	               }
	            }      
		 else if (CurrentTiltPanSpeed >= 50) 
	            {
		         RocCom++;  
	             if (RocCom >= CurrentTiltPanSpeed -40)   //30
//	             if (RocCom >= CurrentTiltPanSpeed - 25)   //30
	               {                                    //300
		            CurrentTiltPanSpeed--;
		            RocCom = 0;
	               }
	            }
	     else if (CurrentTiltPanSpeed >= 40) 
	            {
		         RocCom++;  
	             if (RocCom >= CurrentTiltPanSpeed - 35)   //30
//	             if (RocCom >= CurrentTiltPanSpeed - 32)   //30
	               {                                    //300
		            CurrentTiltPanSpeed--;
		            RocCom = 0;
	               }
	            }
		 else if (CurrentTiltPanSpeed >= 30) 
	            {
		         RocCom++;  
	             if (RocCom >= CurrentTiltPanSpeed -27 )   //30
	               {                                    //300
		            CurrentTiltPanSpeed--;
		            RocCom = 0;
	               }
	            }
        else CurrentTiltPanSpeed = TiltPanSpeed; 
	   }
  return CurrentTiltPanSpeed;	  
}

/////////////////////////////////////////////////////////////
//
//TIMER2 initialisation - prescale:1
// WGM: Normal
// desired value: 16mSec
// actual value: 16.000mSec (0.0%)
void timer2_init(void)
{
// TCCR2 = 0x00; //stop
// TCNT2 = 0x06; //setup
// OCR2  = 0xFA;
// TCCR2 = 0x01; //start   ///test
// TIMSK |= 0x40; //timer interrupt sources
	BSP_Timer4_Init();
}
///////////////////////////////////////////////////////////////////////////
//
//#pragma interrupt_handler timer2_ovf_isr:11
void timer2_ovf_isr(void)
{

}


////////////////////////////////////////////////////////////////////////////////

 __asm void nop(void)
{
    NOP

}






void delay_1us(void)
{
	int i = 8;//5

	while(i--);

}


//////////////////////////////////////////////////////////////////////////////
void delay_1ms(void)
{
 	uint i,k=4;
	while(k--)
		for(i=1150; i>0; i--);

}


////////////////////////////////////////////////////////////////////////////
void delay_X1ms(uint m)
{
unsigned int i;   
 for(i=m; i>0; i--)
  delay_1ms();
}

//返回的度数放大了10倍
//return value:msb=1 means minus degree value(only appears in tilt step)
uint motorStep2degree(uint motor_step, uchar type)//type=1:tilt step; =0:pan step //cost about 125us
{
	return type;
}


//////////////////////////////////////////////////////////////////////////////
//跑到指定坐标处
void Dir_Pos_focus(s16 Y)
{
    short int Dir_PosXY_com;
    uchar Dir_PosXY_com1;
    uchar dir_pos_com;
    static uchar Pan_motstat_Buff;

    Tilt_motstate = 0;

    if ((Y != tiltpos) && (Y <= Tilt_quarter_ground_count))
    {     //    
        if (Tilt_move_or_posmove == 0x00)
            Tilt_move_or_posmove = 0xAA;
        Tilt_motor_pos_flag = 0x00;
        Dir_PosXY_com  = Y;
        Dir_PosXY_com -= tiltpos;                                                 

        if (Dir_PosXY_com > 0)
        {
            Tilt_motstate  |= TILTDOWN;
            MotordirectionBuffer_tilt = 0x07;
            tstepcount = Dir_PosXY_com;
        }
        else 
        {
            Tilt_motstate  |= TILTUP;
            MotordirectionBuffer_tilt = 0x08;
            tstepcount = tiltpos-Y;
        }
    }
    else 
    {
        tstepcount = 0;
        Tilt_motor_pos_flag = 0x01;
    }
    
    Tilt_Tour_speed_class = 0;

    if (Tilt_move_or_posmove == 0xAA)		   //设置调用预置点的垂直运行速度
    {
        Tilt_moto_speed = MotorSpeed_Tilt[2];//MotorSpeed_Tilt[1];
        Tilt_Tour_speed_class = 5;		     
	}  


    if (0x00 == Tilt_motor_pos_flag)  //(tstepcount > 0)
    {
        //delay_X1ms(30);//delay_X1ms(20);
        Tilt_MOTOR_START;
    }                 	 		               	 		   
}

//speed: bit7=0:(bit0~6=0:callpreset,or is vector speed (1~9)); bit7=1:(bit6 for line scanning mode; bit0~5:scan speed 0~63)
void send_fixed_speed_pos_toMotor(uint h_pos, uint v_pos, uchar speed, uchar flag) //flag: bit0=1:dome scanning recovering from tilt action
{
	uchar k;

	pstepcount = h_pos;
	tstepcount = v_pos;
	for (k=0; k<6; k++)  //clear buffer
	    com_buffer[k] = 0;
	
	if (speed)
	{
		com_buffer[0] = 0xa5;
		if (speed & 0x80) //scan speed 0x80~0xBF(0~63)
		{
			com_buffer[5] = speed;
			com_buffer[3] = flag & 0x01; 
		}
		else             //vector speed 1~9
			com_buffer[5] = speed - 1;
	}
	else
	    com_buffer[0] = 0xaa;

 /////////////////////////////////////////////////////
	MotorControll(); 	
 ///////////////////////////////////////////////////////
}

void focus_goto_pos_absolutely(s16 target_pos, u8 speed, u8 flag) //flag: bit0=1:dome scanning recovering from tilt action
{
	uchar k;

	tstepcount = target_pos;
    
	for (k=0; k<6; k++)  //clear buffer
	    com_buffer_tilt[k] = 0;
	
	if (speed)
	{
		com_buffer_tilt[0] = 0xa5;
		if (speed & 0x80) //scan speed 0x80~0xBF(0~63)
		{
			com_buffer_tilt[5] = speed;
			com_buffer_tilt[3] = flag & 0x01; 
		}
		else             //vector speed 1~9
			com_buffer_tilt[5] = speed - 1;
	}
	else
	    com_buffer_tilt[0] = 0xaa;

 /////////////////////////////////////////////////////
	focus_MotorControll(); 	
 ///////////////////////////////////////////////////////
}




//u8 zoom_table[8] = {0x09,0x08,0x0a,0x02,0x06,0x04,0x05,0x01};
//u8 zoom_table[8] = {0x09,0x01,0x05,0x04,0x06,0x02,0x0A,0x08};
u8 zoom_table[8] = {0x06,0x02,0x0A,0x08,0x09,0x01,0x05,0x04};


#define BIT_0	Bit_RESET
#define	BIT_1   Bit_SET

BitAction pb0_table[8] = {BIT_1,BIT_1,BIT_1,BIT_0,BIT_0,BIT_0,BIT_0,BIT_0};
BitAction pb1_table[8] = {BIT_0,BIT_0,BIT_0,BIT_0,BIT_1,BIT_1,BIT_1,BIT_0};
BitAction pb2_table[8] = {BIT_0,BIT_0,BIT_1,BIT_1,BIT_1,BIT_0,BIT_0,BIT_0};
BitAction pb3_table[8] = {BIT_1,BIT_0,BIT_0,BIT_0,BIT_0,BIT_0,BIT_1,BIT_1};


//u8 focus_table[8] = {0x09,0x08,0x0a,0x02,0x06,0x04,0x05,0x01};
u8 focus_table[8] = {0x06,0x02,0x0A,0x08,0x09,0x01,0x05,0x04};


#define	ZOOM_STEPS_NUM		8
#define	FOCUS_STEPS_NUM		8

void zoom_motor_tele(void)
{
    GPIO_ResetBits(ZOOM_PORT,ZOOM_PIN_A1);	
    GPIO_SetBits(ZOOM_PORT,ZOOM_PIN_A2);	

}

void zoom_motor_wide(void)
{
    GPIO_ResetBits(ZOOM_PORT,ZOOM_PIN_A2);	
    GPIO_SetBits(ZOOM_PORT,ZOOM_PIN_A1);	

}

void zoom_motor_stop(void)
{
    GPIO_ResetBits(ZOOM_PORT,ZOOM_PIN_A1);	
    GPIO_ResetBits(ZOOM_PORT,ZOOM_PIN_A2);	

}

static void zoom_motor_delay(void)
{
    delay_X1ms(50);
}

void zoom_action(enum ZF_TYPE mode)
{
	static u8 i=0;

	u16 temp = 0;
	
	switch(mode)
	{
	case ZOOM_WIDE:
    	zoom_motor_wide();

		i++;
		if(i>=ZOOM_STEPS_NUM)
			i = 0;

        zoom_run_state_pre = zoom_run_state;
        zoom_run_state = 1;
		break;
	case ZOOM_TELE:

        zoom_motor_tele();
//i--;
		 if(i == 0)
			i = ZOOM_STEPS_NUM-1;
		else
			i--;

        zoom_run_state_pre = zoom_run_state;
        zoom_run_state = 1;
		
		break;
	case ZOOM_STOP:
	case ALL_STOP:
	
    	zoom_motor_stop();	

        zoom_run_state_pre = zoom_run_state;
        zoom_run_state = 0;
      
		break;
	default:
		return;
	}

}


void focus_motor_far(void)
{
    GPIO_ResetBits(FOCUS_PORT,FOCUS_PIN_A1);	
    GPIO_SetBits(FOCUS_PORT,FOCUS_PIN_A2);	

}

void focus_motor_near(void)
{
    GPIO_ResetBits(FOCUS_PORT,FOCUS_PIN_A2);	
    GPIO_SetBits(FOCUS_PORT,FOCUS_PIN_A1);	

}

void focus_motor_stop(void)
{
    GPIO_ResetBits(FOCUS_PORT,FOCUS_PIN_A1);	
    GPIO_ResetBits(FOCUS_PORT,FOCUS_PIN_A2);	

}

static void focus_motor_delay(void)
{
    delay_X1ms(50);
}


void focus_action(enum ZF_TYPE mode)
{
	static u8 i=0;

	u16 temp = 0;

	//temp = GPIO_ReadOutputData(FOCUS_PORT);
	
	switch(mode)
	{
	case FOCUS_FAR:
    	focus_motor_far();
		i++;
		if(i>=FOCUS_STEPS_NUM)
			i = 0;
		break;
	case FOCUS_NEAR:
    	focus_motor_near();
		 if(i == 0)
			i = FOCUS_STEPS_NUM-1;
		else
			i--;
		break;
	case FOCUS_STOP:
	case ALL_STOP:
    	focus_motor_stop();
		break;
	default:
		return;
	}

}



void iris_motor_plus(void)
{
    GPIO_ResetBits(IRIS_PORT,IRIS_PIN_PLUS);	
    GPIO_SetBits(IRIS_PORT,IRIS_PIN_SUB);	

}

void iris_motor_sub(void)
{
    GPIO_SetBits(IRIS_PORT,IRIS_PIN_PLUS);	
    GPIO_ResetBits(IRIS_PORT,IRIS_PIN_SUB);	

}

void iris_motor_stop(void)
{
    GPIO_ResetBits(IRIS_PORT,IRIS_PIN_PLUS);	
    GPIO_ResetBits(IRIS_PORT,IRIS_PIN_SUB);	

}

static void iris_motor_delay(void)
{
    delay_X1ms(50);
}

void iris_action(enum IRIS_TYPE mode)
{
	static u8 i=0;

	u16 temp = 0;

	//temp = GPIO_ReadOutputData(FOCUS_PORT);
	
	switch(mode)
	{
	case IRIS_PLUS:
    	
        iris_motor_plus();
		break;
	case IRIS_SUB:
    	iris_motor_sub();
		break;
 	case IRIS_PLUS_ONE_STEP:
    	
        iris_motor_plus();
        iris_motor_delay();
        iris_motor_stop();
		break;
  	case IRIS_SUB_ONE_STEP:
    	
        iris_motor_sub();
        iris_motor_delay();
        iris_motor_stop();
		break;
        
	case IRIS_STOP:
    	iris_motor_stop();
		break;

        
	default:
		return;
	}

}

