#include <stdint.h>
#include "platform.h"
#include "platform_spi.h"

extern SPI_HandleTypeDef hspi1;

uint8_t SpiInOut( uint8_t outData )
{
    /* Send SPIy data */
	HAL_SPI_TransmitReceive( SPI_INTERFACE, &outData, &outData, sizeof(outData), SPI_TIMEOUT );
	return outData;
}
