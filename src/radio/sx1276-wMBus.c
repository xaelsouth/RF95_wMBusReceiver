#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "../platform/platform.h"
#include "radio.h"
#include "sx1276-Hal.h"
#include "sx1276.h"
#include "sx1276-FskMisc.h"
#include "sx1276-wMBus.h"

#define BITRATE					100000
#define FREQUENCY_DEVIATION		50000
#define BANDWIDTH				(BITRATE + 2*FREQUENCY_DEVIATION)

// Default settings
tFskSettings FskSettings = 
{
    868950000,      // RFFrequency
	BITRATE,         // Bitrate
	FREQUENCY_DEVIATION, // Fdev
    14,             // Power
	BANDWIDTH,      // RxBw
    200000,         // RxBwAfc
    false,          // AfcOn. Don't use that!
};

/*!
 * SX1276 FSK registers variable
 */
tSX1276* SX1276;

void SX1276FskInit( void )
{
    SX1276FskSetDefaults( );
    
    SX1276ReadBuffer( REG_OPMODE, SX1276Regs + 1, 0x70 - 1 );

    // Set the device in FSK mode and Sleep Mode
    SX1276->RegOpMode = RF_OPMODE_MODULATIONTYPE_FSK | RF_OPMODE_SLEEP;
    SX1276Write( REG_OPMODE, SX1276->RegOpMode );

    SX1276->RegPaRamp = RF_PARAMP_MODULATIONSHAPING_01;
    SX1276Write( REG_PARAMP, SX1276->RegPaRamp );

    SX1276->RegLna = RF_LNA_GAIN_G1;
    SX1276Write( REG_LNA, SX1276->RegLna );

    if( FskSettings.AfcOn == true )
    {
        SX1276->RegRxConfig = RF_RXCONFIG_RESTARTRXONCOLLISION_OFF | RF_RXCONFIG_AFCAUTO_ON |
                              RF_RXCONFIG_AGCAUTO_ON;
    }
    else
    {
        SX1276->RegRxConfig = RF_RXCONFIG_RESTARTRXONCOLLISION_OFF | RF_RXCONFIG_AFCAUTO_OFF |
                              RF_RXCONFIG_AGCAUTO_ON;
    }

    SX1276->RegSyncConfig = RF_SYNCCONFIG_AUTORESTARTRXMODE_WAITPLL_OFF | RF_SYNCCONFIG_PREAMBLEPOLARITY_AA |
                            RF_SYNCCONFIG_SYNC_ON |
                            RF_SYNCCONFIG_SYNCSIZE_3;


    SX1276->RegPacketConfig1 = RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE | RF_PACKETCONFIG1_DCFREE_OFF |
                               ( 0 << 4 ) | RF_PACKETCONFIG1_CRCAUTOCLEAR_ON |
                               RF_PACKETCONFIG1_ADDRSFILTERING_OFF | RF_PACKETCONFIG1_CRCWHITENINGTYPE_CCITT;

    SX1276->RegPacketConfig2 = RF_PACKETCONFIG2_DATAMODE_CONTINUOUS;

    // DIO mapping setup
    //                           CrcOk,                   DCLK,                     DATA,                   FifoEmpty
    SX1276->RegDioMapping1 = RF_DIOMAPPING1_DIO0_11 | RF_DIOMAPPING1_DIO1_00 | RF_DIOMAPPING1_DIO2_00 | RF_DIOMAPPING1_DIO3_11;

    //                          Preamble,                   Data
    SX1276->RegDioMapping2 = RF_DIOMAPPING2_DIO4_11 | RF_DIOMAPPING2_DIO5_11;

    // we can now update the registers with our configuration
    SX1276WriteBuffer( REG_OPMODE, SX1276Regs + 1, 0x70 - 1 );

    // then we need to set the RF settings 
    SX1276FskSetRFFrequency( FskSettings.RFFrequency );
    SX1276FskSetBitrate( FskSettings.Bitrate );
    SX1276FskSetFdev( FskSettings.Fdev );

    SX1276FskSetDccBw( &SX1276->RegRxBw, 0, FskSettings.RxBw );
    SX1276FskSetDccBw( &SX1276->RegAfcBw, 0, FskSettings.RxBwAfc );
    SX1276FskSetRssiOffset( 0 );

#if( MODULE_RF95_DRAGINO_SHIELD_1v4 == 1 )
    if( FskSettings.RFFrequency > 860000000 )
    {
        SX1276FskSetPAOutput( RF_PACONFIG_PASELECT_PABOOST );
        SX1276FskSetPa20dBm( true );
        FskSettings.Power = 20;
        SX1276FskSetRFPower( FskSettings.Power );
    }
    else
    {
        SX1276FskSetPAOutput( RF_PACONFIG_PASELECT_RFO );
        SX1276FskSetPa20dBm( false );
        FskSettings.Power = 14;
        SX1276FskSetRFPower( FskSettings.Power );
    }
#else
  #error "No module defined?!"
#endif

    SX1276FskSetOpMode( RF_OPMODE_STANDBY );

    // Calibrate the HF
    SX1276FskRxCalibrate( );
}

void SX1276FskSetDefaults( void )
{
    // REMARK: See SX1276 datasheet for modified default values.

    SX1276Read( REG_VERSION, &SX1276->RegVersion );
}

void SX1276FskSetOpMode( uint8_t opMode )
{
    static uint8_t opModePrev = RF_OPMODE_STANDBY;

    opModePrev = SX1276->RegOpMode & ~RF_OPMODE_MASK;

    if( opMode != opModePrev )
    {
        SX1276->RegOpMode = ( SX1276->RegOpMode & RF_OPMODE_MASK ) | opMode;
        SX1276Write( REG_OPMODE, SX1276->RegOpMode );        
    }
}

uint8_t SX1276FskGetOpMode( void )
{
    SX1276Read( REG_OPMODE, &SX1276->RegOpMode );
    
    return SX1276->RegOpMode & ~RF_OPMODE_MASK;
}

int32_t SX1276FskReadFei( void )
{
    SX1276ReadBuffer( REG_FEIMSB, &SX1276->RegFeiMsb, 2 );                          // Reads the FEI value

    return ( int32_t )( double )( ( ( uint16_t )SX1276->RegFeiMsb << 8 ) | ( uint16_t )SX1276->RegFeiLsb ) * ( double )FREQ_STEP;
}

int32_t SX1276FskReadAfc( void )
{
    SX1276ReadBuffer( REG_AFCMSB, &SX1276->RegAfcMsb, 2 );                            // Reads the AFC value
    return ( int32_t )( double )( ( ( uint16_t )SX1276->RegAfcMsb << 8 ) | ( uint16_t )SX1276->RegAfcLsb ) * ( double )FREQ_STEP;
}

uint8_t SX1276FskReadRxGain( void )
{
    SX1276Read( REG_LNA, &SX1276->RegLna );
    return( SX1276->RegLna >> 5 ) & 0x07;
}

double SX1276FskReadRssi( void )
{
    SX1276Read( REG_RSSIVALUE, &SX1276->RegRssiValue );                               // Reads the RSSI value

    return -( double )( ( double )SX1276->RegRssiValue / 2.0 );
}


