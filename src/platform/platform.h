#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#define MODULE_RF95_DRAGINO_SHIELD_1v4			1

#include "stm32l476xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_gpio.h"
#include "stm32l4xx_hal_rcc.h"
#include "stm32l4xx_hal_dma.h"
#include "stm32l4xx_hal_spi.h"

#define RESET_IOPORT                                GPIOC
#define RESET_PIN                                   GPIO_PIN_7

#define DIO0_IOPORT                                 GPIOA
#define DIO0_PIN                                    GPIO_PIN_10

#define DIO1_IOPORT                                 GPIOB
#define DIO1_PIN                                    GPIO_PIN_10

#define DIO2_IOPORT                                 GPIOA
#define DIO2_PIN                                    GPIO_PIN_8

#define DIO3_IOPORT									/* not connected */
#define DIO3_PIN									/* not connected */

#define DIO4_IOPORT									/* not connected */
#define DIO4_PIN									/* not connected */

#define DIO5_IOPORT									GPIOA
#define DIO5_PIN									GPIO_PIN_9

#endif /* __PLATFORM_H__ */
