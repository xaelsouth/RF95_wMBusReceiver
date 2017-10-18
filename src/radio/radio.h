#ifndef __RADIO_H__
#define __RADIO_H__

/*!
 * Radio driver structure defining the different function pointers
 */
typedef struct sRadioDriver
{
    void ( *Init )( void );
    void ( *Reset )( void );
    void ( *StartRx )( void );

} tRadioDriver;

/*!
 * \brief Initializes the RadioDriver structure with specific radio
 *        functions.
 *
 * \retval radioDriver Pointer to the radio driver variable
 */
tRadioDriver* RadioDriverInit( void );

#endif // __RADIO_H__
