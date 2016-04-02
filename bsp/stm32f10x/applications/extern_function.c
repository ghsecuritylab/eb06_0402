#include "includes.h"
#include <rtdevice.h>

extern struct rt_serial_device serial1;
extern struct rt_serial_device serial2;


/* STM32 uart driver */
struct stm32_uart
{
    USART_TypeDef* uart_device;
    IRQn_Type irq;
};

static rt_err_t stm32_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    struct stm32_uart* uart;
    USART_InitTypeDef USART_InitStructure;

    RT_ASSERT(serial != RT_NULL);
    RT_ASSERT(cfg != RT_NULL);

    uart = (struct stm32_uart *)serial->parent.user_data;

    USART_InitStructure.USART_BaudRate = cfg->baud_rate;

    if (cfg->data_bits == DATA_BITS_8){
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    } else if (cfg->data_bits == DATA_BITS_9) {
        USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    }

    if (cfg->stop_bits == STOP_BITS_1){
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
    } else if (cfg->stop_bits == STOP_BITS_2){
        USART_InitStructure.USART_StopBits = USART_StopBits_2;
    }

    if (cfg->parity == PARITY_NONE){
        USART_InitStructure.USART_Parity = USART_Parity_No;
    } else if (cfg->parity == PARITY_ODD) {
        USART_InitStructure.USART_Parity = USART_Parity_Odd;
    } else if (cfg->parity == PARITY_EVEN) {
        USART_InitStructure.USART_Parity = USART_Parity_Even;
    }

    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(uart->uart_device, &USART_InitStructure);

    /* Enable USART */
    USART_Cmd(uart->uart_device, ENABLE);

    return RT_EOK;
}

void set_rs485_uart_baudrate(void)
{
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

	u16 bbt=2400;
	
	switch(Baud_rate)
	{
    case DOME_BAUDRATE_1200: 
				bbt = 1200;
			   break;    
	case DOME_BAUDRATE_2400: 
				bbt  = BAUD_RATE_2400;
			   break;
	case DOME_BAUDRATE_4800: 
				bbt  = BAUD_RATE_4800;
			   break;
	case DOME_BAUDRATE_9600: 
				bbt  = BAUD_RATE_9600;
			   break;
	case DOME_BAUDRATE_19200:
		bbt  = 19200;
	       break;
	case DOME_BAUDRATE_115200:
		bbt  = BAUD_RATE_115200;
	       break;	   
	default:   
		bbt  = BAUD_RATE_2400;
	    break;
	}

	config.baud_rate = bbt;
	stm32_configure(&serial1, &config);

}



void dome_func_control(uchar action,uchar prePoint)
{

}

