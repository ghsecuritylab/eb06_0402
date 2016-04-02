#ifndef _MOTORCONFIG_H_
#define _MOTORCONFIG_H_

#include "api_type.h"

#define	PHOTOSWITCH_PROTECTING			1	   //若为1，则将使用GPIO去控制光电开关的工作状态，切换开/关

#define	ZOOM_STEP_DELAY		(delay_X1ms(5))//delay_half_ms(1)
#define	FOCUS_STEP_DELAY	(delay_X1ms(3))//	delay_half_ms(1)

#define	MOTOR_TILT_CHECK_MODE	1//马达垂直自检模式
#define	PAN_CHECK_MODE	1	//为1时，水平自检方式为来回二	次检查


#define	AUTO_FLIP_MODE	2	//为1时，表示自动翻转功能为垂直达到90度后，水平自动翻转180，然后垂直向下转动
							//10度，即垂直方向回到80度位置;
							//为2时，表示当垂直达到90度后，在不松开摇杆的情况下，马达水平转动180度后
							//继续向下转动，直到松开摇杆为止。

#define MOTOR_PAN_REVERSE       1

#define ENABLE_MOTOR  GPIOA->BRR = GPIO_Pin_8;
#define	DIS_MOTOR		GPIOA->BSRR = GPIO_Pin_8;



#if APP_MOTOR_CHANGE_SPEED_MACRO_ENABLE
#define	APP_Motor_ChangePanSpeed(time) \
	TIM5->ARR = time ;	\
	TIM5->EGR = ((u16)0x0001);	

#define	APP_Motor_ChangePanSpeed_NoUpdate(time) \
	TIM5->ARR = time ;	

#define	APP_Motor_ChangeTiltSpeed(time)	\
	TIM3->ARR = time ;	\
	TIM3->EGR = ((u16)0x0001);

#define	APP_Motor_ChangeTiltSpeed_NoUpdate(time)	\
	TIM3->ARR = time ;

#endif

#define TILT_ZERO_POINT_PORT  GPIOD             //Used PORTB msb bit
#define TILT_ZERO_POINT_PIN  GPIO_Pin_6             //Used PORTB msb bit
#define TILT_ZERO_POINT_POS  TILT_ZERO_POINT_PIN             //Used PORTB msb bit
#define TILT_ZERO_POINT_BUFFER  ( ((u16)GPIOD->IDR)& TILT_ZERO_POINT_POS)            //Used PORTB msb bit


#define	PAN_TOTAL_DEGREES	360
#define	TILT_TOTAL_DEGREES	90


#define	ZOOM_LEN_PER_MOTOR_STEP				(double)  0.08//0.01//0.4  //电机走一个整步所走的长度 ，单位是mm
#define	ZOOM_LEN_PER_STEP		(double)((double)ZOOM_LEN_PER_MOTOR_STEP/8)
#define	ZOOM_HIGH_LEN			(double)15.052//15.478//
#define	ZOOM_LOW_LEN			(double)5.288//5.681//
#define	ZOOM_TOTAL_LEN			(double)(ZOOM_HIGH_LEN+ZOOM_LOW_LEN)
#define	ZOOM_TOTAL_STEPS		(double)(ZOOM_TOTAL_LEN/ZOOM_LEN_PER_STEP)
#define	ZOOM_SENSOR_POS_HIGH_TO_LOW_STEPS		(double)(ZOOM_HIGH_LEN/ZOOM_LEN_PER_STEP)
#define	ZOOM_SENSOR_POS_LOW_TO_HIGH_STEPS		(double)(ZOOM_LOW_LEN/ZOOM_LEN_PER_STEP)

#define	ZOOM_SENSOR_POS_SENSOR_TO_WIDE_STEPS		ZOOM_SENSOR_POS_HIGH_TO_LOW_STEPS
#define	ZOOM_SENSOR_POS_SENSOR_TO_TELE_STEPS		ZOOM_SENSOR_POS_LOW_TO_HIGH_STEPS


#define	FOCUS_LEN_PER_MOTOR_STEP				(double)0.00625 //0.25  //电机走一个整步所走的长度
#define	FOCUS_LEN_PER_STEP		(double)((double)FOCUS_LEN_PER_MOTOR_STEP/1)
#define	FOCUS_HIGH_LEN			(double)0.9375//1.468
#define	FOCUS_LOW_LEN			(double)1.174//1.9
#define	FOCUS_TOTAL_LEN			(double)(FOCUS_HIGH_LEN+FOCUS_LOW_LEN)
#define	FOCUS_TOTAL_STEPS		(double)(FOCUS_TOTAL_LEN/FOCUS_LEN_PER_STEP)
#define	FOCUS_SENSOR_POS_HIGH_TO_LOW_STEPS		(double)(FOCUS_HIGH_LEN/FOCUS_LEN_PER_STEP)
#define	FOCUS_SENSOR_POS_LOW_TO_HIGH_STEPS		(double)(FOCUS_LOW_LEN/FOCUS_LEN_PER_STEP)
#define	FOCUS_SENSOR_POS_SENSOR_TO_FAR_STEPS		FOCUS_SENSOR_POS_HIGH_TO_LOW_STEPS
#define	FOCUS_SENSOR_POS_SENSOR_TO_NEAR_STEPS		FOCUS_SENSOR_POS_LOW_TO_HIGH_STEPS

#define	FOCUS_SENSOR_POS_SENSOR_TO_ZERO_POS_STEPS		(double)((double)0.225/(double)FOCUS_LEN_PER_STEP)
#define	FOCUS_ZERO_TO_FAR_END_LENGTH		(double)1.1625//1.693
#define	FOCUS_ZERO_TO_FAR_END_STEPS		(double)(FOCUS_ZERO_TO_FAR_END_LENGTH/FOCUS_LEN_PER_STEP)

#define	FOCUS_ZERO_TO_NEAR_END_LENGTH		(double)0.949//1.45
#define	FOCUS_ZERO_TO_NEAR_END_STEPS		(double)(FOCUS_ZERO_TO_NEAR_END_LENGTH/FOCUS_LEN_PER_STEP)



#define MOTOR_PAN_STEP           ((unsigned short)(ZOOM_TOTAL_STEPS)) //18400     //16000   //360度
//                                                          360       /           1.8        *    16     *    5.75               
#define MOTOR_TILT_STEP          ((unsigned short)(FOCUS_TOTAL_STEPS))//3600	//90度
//                                                          90       /           1.8        *    16     *    4.5                    
#define PAN_STEPS_OF_45DEGREE    ((unsigned short)(0))
#define PAN_STEPS_OF_90DEGREE    ((unsigned short)(0)
#define PAN_STEPS_OF_180DEGREE    ((unsigned short)(0)

#define Pan_ground_count    MOTOR_PAN_STEP//16000//20800   
#define Tilt_quarter_ground_count MOTOR_TILT_STEP//90度


//--------------------------------马达齿轮，皮带等配置  --结束(邓强添加)-----------------------

#define PANLEFT   0x01
#define PANRIGHT  0x02

//---------------------------------------------------类型定义---------------
typedef	s32 MOTOR_PAN_STEPS_TYPE;	
typedef s16	MOTOR_TILT_STEPS_TYPE;

//////////////////////////////////////////////////////////////////
enum ZF_TYPE
{
	ALL_STOP,
	ZOOM_STOP,
	FOCUS_STOP,
	ZOOM_TELE,
	ZOOM_WIDE,
	FOCUS_FAR,
	FOCUS_NEAR
};


enum IRIS_TYPE
{
	IRIS_STOP,
	IRIS_PLUS,
	IRIS_PLUS_ONE_STEP,
	
	IRIS_SUB,
	IRIS_SUB_ONE_STEP,

};


//PORT B
#define	ZOOM_PORT		GPIOC
#define	ZOOM_PIN_A1		GPIO_Pin_8
#define	ZOOM_PIN_A2		GPIO_Pin_9

#define	ZOOM_PIN_B1		GPIO_Pin_2
#define	ZOOM_PIN_B2		GPIO_Pin_3

//PORT B
#define	FOCUS_PORT		GPIOC
#define	FOCUS_PIN_A1	GPIO_Pin_6
#define	FOCUS_PIN_A2	GPIO_Pin_7

#define	FOCUS_PIN_B1	GPIO_Pin_6
#define	FOCUS_PIN_B2	GPIO_Pin_7


#define	IRIS_PORT		GPIOB
#define	IRIS_PIN_PLUS	GPIO_Pin_14
#define	IRIS_PIN_SUB	GPIO_Pin_15



#define	IRIS_AUTO_PORT		GPIOB
#define	IRIS_AUTO_PIN_A1	GPIO_Pin_0
#define	IRIS_AUTO_PIN_A2	GPIO_Pin_1
#define	IRIS_AUTO_PIN_B1	GPIO_Pin_2
#define	IRIS_AUTO_PIN_B2	GPIO_Pin_3




//PORT A
#define	KEY_PORT		GPIOC
#define	KEY_LEFT_PIN	GPIO_Pin_8
#define	KEY_RIGHT_PIN	GPIO_Pin_9
#define	KEY_UP_PIN		GPIO_Pin_10
#define	KEY_DOWN_PIN	GPIO_Pin_11
#define	KEY_SET_PIN		GPIO_Pin_12


#define	FOCUS_PLUS_PIN	KEY_LEFT_PIN
#define	FOCUS_SUB_PIN	KEY_RIGHT_PIN
#define	ZOOM_PLUS_PIN	KEY_UP_PIN
#define	ZOOM_SUB_PIN	KEY_DOWN_PIN
#define	ZOOM_POS_SENSOR_PIN		GPIO_Pin_0
#define	FOCUS_POS_SENSOR_PIN		GPIO_Pin_1

#define	GET_FOCUS_POS_SENSOR_STATE	(((u16)GPIOA->IDR) & FOCUS_POS_SENSOR_PIN)
//#define	GET_ZOOM_POS_SENSOR_STATE 			GPIO_ReadInputDataBit(GPIOA,ZOOM_POS_SENSOR_PIN)	   //(((u16)GPIOA->IDR) & 0x01)
#define	GET_ZOOM_POS_SENSOR_STATE 			(((u16)GPIOA->IDR) & ZOOM_POS_SENSOR_PIN)



//-----------------------------------全局函数声明-------------------------------------------------------------------

extern uint motorStep2degree(uint motor_step, uchar type);//type=1:tilt step; =0:pan step //cost about 125us

//speed: bit7=0:(bit0~6=0:callpreset,or is vector speed (1~9)); bit7=1:(bit6 for line scanning mode; bit0~5:scan speed 0~63)
extern void send_fixed_speed_pos_toMotor(uint h_pos, uint v_pos, uchar speed, uchar flag); //flag: bit0=1:dome scanning recovering from tilt action

extern void motorCommWhileScanRotat(uchar action,uchar pan_speed,uchar tilt_speed); //action:0~7 ->left,right....

extern void zoom_action(enum ZF_TYPE mode);
extern void focus_action(enum ZF_TYPE mode);


/*------------------------------------------------引用外部变量----------------------------------*/
extern const uchar pt_code2motor[];
extern uchar AutoFlipCount;
extern u16 MotorTimeOut_2ms_Counter;

extern volatile MOTOR_PAN_STEPS_TYPE	pstepcount;
extern volatile MOTOR_TILT_STEPS_TYPE	tstepcount;
extern volatile MOTOR_PAN_STEPS_TYPE panpos;
extern volatile MOTOR_TILT_STEPS_TYPE tiltpos;

extern u8 focus_run_state,zoom_run_state;



#endif  /* _MOTORCONFIG_H_ */


