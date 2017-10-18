#include <stdbool.h>
#include "../platform/platform.h"
#include "radio.h"
#include "sx1276.h"
#include "sx1276-Hal.h"
#include "sx1276-wmBus.h"

/*!
 * SX1276 registers variable
 */
uint8_t SX1276Regs[0x70];

void SX1276Init( void )
{
    // Initialize FSK and LoRa registers structure
    SX1276 = ( tSX1276* )SX1276Regs;

    SX1276InitIo( );
    
    SX1276Reset( );

    // Initialize FSK modem
    SX1276FskInit( );
}

void SX1276Reset( void )
{
    SX1276SetReset( RADIO_RESET_ON );
    
    // Wait 1ms
    uint32_t startTick = GET_TICK_COUNT( );
    while( ( GET_TICK_COUNT( ) - startTick ) < TICK_RATE_MS( 1 ) );    

    SX1276SetReset( RADIO_RESET_OFF );
    
    // Wait 6ms
    startTick = GET_TICK_COUNT( );
    while( ( GET_TICK_COUNT( ) - startTick ) < TICK_RATE_MS( 6 ) );    
}

void SX1276SetOpMode( uint8_t opMode )
{

        SX1276FskSetOpMode( opMode );
}

uint8_t SX1276GetOpMode( void )
{
	return SX1276FskGetOpMode( );
}

double SX1276ReadRssi( void )
{
    return SX1276FskReadRssi( );
}

uint8_t SX1276ReadRxGain( void )
{
    return SX1276FskReadRxGain( );
}

void SX1276StartRx( void )
{
	SX1276FskSetOpMode( RF_OPMODE_RECEIVER );
}

