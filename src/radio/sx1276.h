#ifndef __SX1276_H__
#define __SX1276_H__

/*!
 * \brief SX1276 registers array
 */
extern uint8_t SX1276Regs[0x70];

/*!
 * \brief Initializes the SX1276
 */
void SX1276Init( void );

/*!
 * \brief Resets the SX1276
 */
void SX1276Reset( void );

/*!
 * \brief Sets the SX1276 operating mode
 *
 * \param [IN] opMode New operating mode
 */
void SX1276SetOpMode( uint8_t opMode );

/*!
 * \brief Gets the SX1276 operating mode
 *
 * \retval opMode Current operating mode
 */
uint8_t SX1276GetOpMode( void );

/*!
 * \brief Reads the current Rx gain setting
 *
 * \retval rxGain Current gain setting
 */
uint8_t SX1276ReadRxGain( void );

/*!
 * \brief Trigs and reads the current RSSI value
 *
 * \retval rssiValue Current RSSI value in [dBm]
 */
double SX1276ReadRssi( void );

void SX1276StartRx( void );

#endif //__SX1276_H__
