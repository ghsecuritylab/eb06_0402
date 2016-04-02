#include <board.h>
#include <rtthread.h>

#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif  /* RT_USING_COMPONENTS_INIT */

#include "includes.h"




vu16 Photoreg_ADC1_ConvertedValue[16];
volatile u8 ADC_Ok=RT_FALSE;

static void Photoreg_ADC2_GPIO_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //ADC1-light
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

}

DMA_InitTypeDef DMA_InitStructure;

//DMA的配置
static void Photoreg_DMA_Configuration(void)
{
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
	/* open the DMA1 intterrupt service */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;//ENABLE;
	NVIC_Init(&NVIC_InitStructure); 

	/* 允许 DMA1 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	/* DMA通道1*/
	DMA_DeInit(DMA1_Channel1);
	//指定DMA外设基地址
	DMA_InitStructure.DMA_PeripheralBaseAddr =(u32)( &(ADC1->DR));		//ADC1数据寄存器
	//设定DMA内存基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Photoreg_ADC1_ConvertedValue;					//获取ADC的数组
	//外设作为数据传输的来源
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;					//片内外设作源头
	//指定DMA通道的DMA缓存大小
	DMA_InitStructure.DMA_BufferSize = 16;								//每次DMA16个数据
	//外设地址不递增（不变）
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//外设地址不增加
	//内存地址不递增（不变）
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				//内存地址增加
	//设定外设数据宽度为16位
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//半字
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			//半字
	//设定DMA的工作模式普通模式，还有一种是循环模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;								//普通模式
	//设定DMA通道的软件优先级
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;							//高优先级
	//使能DMA内存到内存的传输，此处没有内存到内存的传输
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;								//非内存到内存
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	//DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);								//DMA通道1传输完成中断
	
	/* Enable DMA1 channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

static void Photoreg_ADC_Configuration(void)
{
	ADC_InitTypeDef ADC_InitStructure;

	/* 允许ADC */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	/* ADC1 */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;						//独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;							//单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;						//连续扫描
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; 	//软件启动转换
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;					//数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1; 								//1个通道
	ADC_Init(ADC1, &ADC_InitStructure);

	/* 配置通道1的采样速度,*/ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);


	/* 允许ADC1的DMA模式 */
	ADC_DMACmd(ADC1, ENABLE);

	/* 允许ADC1*/
	ADC_Cmd(ADC1, ENABLE);

	/*重置校准寄存器 */   
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));

	/*开始校准状态*/
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	   
	/* 人工打开ADC转换.*/ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);


}

static void DMAReConfig(void)
{
	DMA_DeInit(DMA1_Channel1);
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
	DMA_Cmd(DMA1_Channel1, ENABLE);
}



static void ADC1_Init(void)
{

	Photoreg_ADC2_GPIO_INIT();
	Photoreg_DMA_Configuration();
	Photoreg_ADC_Configuration();

   
}

extern void delayms(int n);


u16 ADC_2_Filter(void)
{
	u32 result=0;
	u8 i;
	for(i=16;i>0;i--)
	{
	       delayms(1);
	       result += (u32)Photoreg_ADC1_ConvertedValue;
	}
	return (u16)(result/16);
}


/*光敏电阻值采样配置*/
#define	ADC_SAMPLE_NUM	16	//ADC值抽样数	
#define	ADC_MIDDLE_START_NUM	(ADC_SAMPLE_NUM/5)
#define	ADC_MIDDLE_END_NUM		(ADC_SAMPLE_NUM - ADC_MIDDLE_START_NUM)
#define	ADC_SAMPLE_VALID_SIZE	(ADC_SAMPLE_NUM - ADC_MIDDLE_START_NUM * 2)	




/**************************************************************
** 函数名:DigitFilter
** 功能:软件滤波
** 注意事项:取NO 的2/5 作为头尾忽略值,注意N 要大于5,否则不会去头尾
***************************************************************/

u16 DigitFilter(u16* buf,u8 no)
{
	u8 i,j;
	u32 tmp;
	u16 Pravite_ADC_buf[16];
	
	for(i=0;i<no;i++)
	{
		Pravite_ADC_buf[i] = buf[i];
	 	buf[i] = 0;
	}	
	//排序，将buf[0]到buf[no-1]从大到小排列
	for(i=0;i<no;i++)
	{
		for(j=0;j<no-i-1;j++)
		{
			if(Pravite_ADC_buf[j]>Pravite_ADC_buf[j+1])
			{
				tmp=Pravite_ADC_buf[j];
				Pravite_ADC_buf[j]=Pravite_ADC_buf[j+1];
				Pravite_ADC_buf[j+1]=tmp;
			}
		}
	}


	//平均
	tmp=0;
	//for(i=cut_no;i<no-cut_no;i++) //只取中间n-2*cut_no 个求平均
	for(i=ADC_MIDDLE_START_NUM;i<ADC_MIDDLE_END_NUM;i++) //只取中间n-2*cut_no 个求平均
		tmp+=Pravite_ADC_buf[i];
	return(tmp/ADC_SAMPLE_VALID_SIZE);
}

/*
 * Function:       
 * Description:    获取光敏电阻的电压值，用来反馈当前环境光线强度
 * Calls:          
 * Called By:      
 * Table Accessed: 
 * Table Updated:  
 * Input:          
 * Output:         
 * Return:         返回电压值，单为mv
 * Others:         
 */
 
static rt_uint32_t  Get_agc_Value(void)
{
	rt_uint32_t i;
	i = DigitFilter((u16 *)Photoreg_ADC1_ConvertedValue,ADC_SAMPLE_NUM)*3300/0xFFF;
DMAReConfig();
	return i;
}



static void rt_hw_adc_init(void)
{
ADC1_Init();
	
	rt_kprintf("ACD init.....OK\n\r");
}

void adc_io_set(u16 val)
{
	switch(val)
	{
	case 1:
		GPIO_WriteBit(GPIOC, GPIO_Pin_0, Bit_SET);
		GPIO_WriteBit(GPIOC, GPIO_Pin_1, Bit_SET);
		GPIO_WriteBit(GPIOC, GPIO_Pin_2, Bit_SET);
		GPIO_WriteBit(GPIOC, GPIO_Pin_3, Bit_SET);
		break;
	case 2:
		GPIO_WriteBit(GPIOC, GPIO_Pin_0, Bit_SET);
		GPIO_WriteBit(GPIOC, GPIO_Pin_1, Bit_SET);
		GPIO_WriteBit(GPIOC, GPIO_Pin_2, Bit_SET);
		GPIO_WriteBit(GPIOC, GPIO_Pin_3, Bit_RESET);
		break;
	case 3:
		GPIO_WriteBit(GPIOC, GPIO_Pin_0, Bit_SET);
		GPIO_WriteBit(GPIOC, GPIO_Pin_1, Bit_SET);
		GPIO_WriteBit(GPIOC, GPIO_Pin_2, Bit_RESET);
		GPIO_WriteBit(GPIOC, GPIO_Pin_3, Bit_RESET);
		break;
	case 4:
		GPIO_WriteBit(GPIOC, GPIO_Pin_0, Bit_SET);
		GPIO_WriteBit(GPIOC, GPIO_Pin_1, Bit_RESET);
		GPIO_WriteBit(GPIOC, GPIO_Pin_2, Bit_RESET);
		GPIO_WriteBit(GPIOC, GPIO_Pin_3, Bit_RESET);
		break;
	case 5:
		GPIO_WriteBit(GPIOC, GPIO_Pin_0, Bit_RESET);
		GPIO_WriteBit(GPIOC, GPIO_Pin_1, Bit_RESET);
		GPIO_WriteBit(GPIOC, GPIO_Pin_2, Bit_RESET);
		GPIO_WriteBit(GPIOC, GPIO_Pin_3, Bit_RESET);
		break;
	default:break;
	
	}

}

//adc>1470 pc0-h, <1470 pc0-l
// >1570 pc1 h,< pc1 l
// >1670 pc2 h,< pc2 l
// >1770 pc3 h,< pc3 l

#define	PC3_LED_VAL		1770

u8 battery_power = 100;

static void adc_check(void)
{
	rt_uint32_t adc_this;
  rt_kprintf("hello here is a ADC test\n\r");

	adc_this = Get_agc_Value();

	if(adc_this >= 1130)
	{
		battery_power = 100;
	}
	else if(adc_this >= 1090)
	{
		battery_power = 90;
	}
	else if(adc_this >= 1040)
	{
		battery_power = 80;
	}
	else if(adc_this >= 990)
	{
		battery_power = 70;
	}
	else if(adc_this >= 950)
	{
		battery_power = 60;
	}
	else if(adc_this >= 910)
	{
		battery_power = 50;
	}

	else if(adc_this >= 870)
	{
		battery_power = 40;
	}

	else if(adc_this >= 820)
	{
		battery_power = 30;
	}

	else if(adc_this >= 770)
	{
		battery_power = 20;
	}

	else if(adc_this >= 720)
	{
		battery_power = 10;
	}
	else
		battery_power= 10;
	

	
}


static void adc_io_pin_init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
    GPIO_Init(GPIOC, &GPIOD_InitStructure);	
    
	GPIO_SetBits(GPIOC,GPIO_Pin_0);	
	GPIO_SetBits(GPIOC,GPIO_Pin_1);		
	GPIO_SetBits(GPIOC,GPIO_Pin_2);	
	GPIO_SetBits(GPIOC,GPIO_Pin_3);	
 

}

void rt_adc_thread_entry(void* parameter)
{
    rt_thread_t init_thread;
	u16 k;

	rt_hw_adc_init();
	//adc_io_pin_init();
	
    while(1)
	{
		adc_check();
		rt_thread_delay(RT_TICK_PER_SECOND/2);
    }
}




int rt_adc_ctl_init(void)
{

	
    rt_thread_t init_thread;

    init_thread = rt_thread_create("adc",
                                   rt_adc_thread_entry, RT_NULL,
                                   4089, 10, 30);
    if (init_thread != RT_NULL)
        rt_thread_startup(init_thread);

    return 0;
}


