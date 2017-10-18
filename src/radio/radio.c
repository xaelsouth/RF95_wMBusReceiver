#include "../platform/platform.h"
#include "radio.h"
#include "sx1276.h"

tRadioDriver RadioDriver;

tRadioDriver* RadioDriverInit( void )
{
    RadioDriver.Init = SX1276Init;
    RadioDriver.Reset = SX1276Reset;
    RadioDriver.StartRx = SX1276StartRx;

    return &RadioDriver;
}
