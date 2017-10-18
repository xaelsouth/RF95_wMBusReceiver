#include "platform/platform.h"
#include "gpio-int.h"

static GpioIrqHandler *GpioIrq[16];

void GpioSetInterrupt(uint16_t gpioPin, IrqPriority irqPriority, GpioIrqHandler irqHandler)
{
	uint16_t gpioPinIndex;
    uint32_t priority;
    IRQn_Type IRQnb;

    switch( irqPriority )
    {
    case IRQ_VERY_LOW_PRIORITY:
    case IRQ_LOW_PRIORITY:
        priority = 3;
        break;
    case IRQ_MEDIUM_PRIORITY:
        priority = 2;
        break;
    case IRQ_HIGH_PRIORITY:
        priority = 1;
        break;
    case IRQ_VERY_HIGH_PRIORITY:
    default:
        priority = 0;
        break;
    }

    switch( gpioPin )
    {
    case GPIO_PIN_0:
    	gpioPinIndex = 0;
        IRQnb = EXTI0_IRQn;
        break;
    case GPIO_PIN_1:
    	gpioPinIndex = 1;
        IRQnb = EXTI1_IRQn;
        break;
    case GPIO_PIN_2:
    	gpioPinIndex = 2;
        IRQnb = EXTI2_IRQn;
        break;
    case GPIO_PIN_3:
    	gpioPinIndex = 3;
        IRQnb = EXTI3_IRQn;
        break;
    case GPIO_PIN_4:
    	gpioPinIndex = 4;
        IRQnb = EXTI4_IRQn;
        break;
    case GPIO_PIN_5:
    	gpioPinIndex = 5;
        IRQnb = EXTI9_5_IRQn;
        break;
    case GPIO_PIN_6:
    	gpioPinIndex = 6;
        IRQnb = EXTI9_5_IRQn;
        break;
    case GPIO_PIN_7:
    	gpioPinIndex = 7;
        IRQnb = EXTI9_5_IRQn;
        break;
    case GPIO_PIN_8:
    	gpioPinIndex = 8;
        IRQnb = EXTI9_5_IRQn;
        break;
    case GPIO_PIN_9:
    	gpioPinIndex = 9;
        IRQnb = EXTI9_5_IRQn;
        break;
    case GPIO_PIN_10:
    	gpioPinIndex = 10;
    	IRQnb = EXTI15_10_IRQn;
    	break;
    case GPIO_PIN_11:
    	gpioPinIndex = 11;
    	IRQnb = EXTI15_10_IRQn;
    	break;
    case GPIO_PIN_12:
    	gpioPinIndex = 12;
    	IRQnb = EXTI15_10_IRQn;
    	break;
    case GPIO_PIN_13:
    	gpioPinIndex = 13;
    	IRQnb = EXTI15_10_IRQn;
    	break;
    case GPIO_PIN_14:
    	gpioPinIndex = 14;
    	IRQnb = EXTI15_10_IRQn;
    	break;
    case GPIO_PIN_15:
    	gpioPinIndex = 15;
        IRQnb = EXTI15_10_IRQn;
        break;
    default:
        break;
    }

    GpioIrq[gpioPinIndex] = irqHandler;

    HAL_NVIC_SetPriority( IRQnb , priority, 0 );
    HAL_NVIC_EnableIRQ( IRQnb );
}

void EXTI0_IRQHandler( void )
{
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_0 );
}

void EXTI1_IRQHandler( void )
{
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_1 );
}

void EXTI2_IRQHandler( void )
{
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_2 );
}

void EXTI3_IRQHandler( void )
{
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_3 );
}

void EXTI4_IRQHandler( void )
{
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_4 );
}

void EXTI9_5_IRQHandler( void )
{
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_5 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_6 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_7 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_8 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_9 );
}

void EXTI15_10_IRQHandler( void )
{
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_10 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_11 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_12 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_13 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_14 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_15 );
}

void HAL_GPIO_EXTI_Callback( uint16_t gpioPin )
{
    uint8_t callbackIndex;

    if( gpioPin > 0 )
    {
    	callbackIndex = 0;

        while( gpioPin != 0x01 )
        {
        	gpioPin >>= 1;
            callbackIndex++;
        }

        if( GpioIrq[callbackIndex] )
        {
            GpioIrq[callbackIndex]();
        }
    }
}
