#ifndef __PLATFORM_SPI_H__
#define __PLATFORM_SPI_H__

#define SPI_INTERFACE								(&hspi1)

#define SPI_TIMEOUT									UINT32_MAX /* milliseconds */

#define SPI_PIN_SCK_PORT                            GPIOA
#define SPI_PIN_SCK                                 GPIO_PIN_5

#define SPI_PIN_MISO_PORT                           GPIOA
#define SPI_PIN_MISO                                GPIO_PIN_6

#define SPI_PIN_MOSI_PORT                           GPIOA
#define SPI_PIN_MOSI                                GPIO_PIN_7

#define NSS_IOPORT                                  GPIOB
#define NSS_PIN                                     GPIO_PIN_6

uint8_t SpiInOut( uint8_t outData );

#endif //__PLATFORM_SPI_H__
