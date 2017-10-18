#include <stdint.h>

#include "platform.h"
#include "platform_spi.h"
#include "../radio/sx1276-Hal.h"

void SX1276InitIo( void )
{
}

void SX1276SetReset( uint8_t state )
{
    if( state == RADIO_RESET_ON )
    {
        // Set RESET pin to 0
        HAL_GPIO_WritePin( RESET_IOPORT, RESET_PIN, GPIO_PIN_RESET );
    }
    else
    {
        // Configure RESET as input?
		// Set RESET pin to 1
        HAL_GPIO_WritePin( RESET_IOPORT, RESET_PIN, GPIO_PIN_SET );
    }
}

void SX1276Write( uint8_t addr, uint8_t data )
{
    SX1276WriteBuffer( addr, &data, 1 );
}

void SX1276Read( uint8_t addr, uint8_t *data )
{
    SX1276ReadBuffer( addr, data, 1 );
}

void SX1276WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;

    //NSS = 0;
    HAL_GPIO_WritePin( NSS_IOPORT, NSS_PIN, GPIO_PIN_RESET );

    SpiInOut( addr | 0x80 );
    for( i = 0; i < size; i++ )
    {
        SpiInOut( buffer[i] );
    }

    //NSS = 1;
    HAL_GPIO_WritePin( NSS_IOPORT, NSS_PIN, GPIO_PIN_SET );
}

void SX1276ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;

    //NSS = 0;
    HAL_GPIO_WritePin( NSS_IOPORT, NSS_PIN, GPIO_PIN_RESET );

    SpiInOut( addr & 0x7F );

    for( i = 0; i < size; i++ )
    {
        buffer[i] = SpiInOut( 0 );
    }

    //NSS = 1;
    HAL_GPIO_WritePin( NSS_IOPORT, NSS_PIN, GPIO_PIN_SET );
}

void SX1276WriteFifo( uint8_t *buffer, uint8_t size )
{
    SX1276WriteBuffer( 0, buffer, size );
}

void SX1276ReadFifo( uint8_t *buffer, uint8_t size )
{
    SX1276ReadBuffer( 0, buffer, size );
}

inline uint8_t SX1276ReadDio0( void )
{
    return HAL_GPIO_ReadPin( DIO0_IOPORT, DIO0_PIN );
}

inline uint8_t SX1276ReadDio1( void )
{
    return HAL_GPIO_ReadPin( DIO1_IOPORT, DIO1_PIN );
}

inline uint8_t SX1276ReadDio2( void )
{
    return HAL_GPIO_ReadPin( DIO2_IOPORT, DIO2_PIN );
}

inline uint8_t SX1276ReadDio3( void )
{
	return 0; /* Not connected at all. */
}

inline uint8_t SX1276ReadDio4( void )
{
	return 0; /* Not connected at all. */
}

inline uint8_t SX1276ReadDio5( void )
{
	return HAL_GPIO_ReadPin( DIO5_IOPORT, DIO5_PIN );
}
