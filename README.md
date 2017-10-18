# RF95_wMBusReceiver
wMbus receiver based on STM32L476 Nucleo and Hoperf RFM95

wMBus mode T1 and C1 are done. Receiving mode C1 datagrams is not good
now because of missing channel coding: RF95 seems having problems with
long 0- or 1-sequences. Playing with bandwidth (and frequency deviaion)
may help.

Projects compiled on Windows 7 with toolchain installed closely by following instructions
on https://eewiki.net/display/microcontroller/Getting+Started+with+the+STM32L432+Nucleo-32+using+Eclipse+and+the+GNU+ARM+Toolchain

I've taken the liberty of copying documents for hardware parts (MCU and radio transceiver)
into the "doc" directory.

RFM95 is a clone of Semtech's SX1276. Both are widely compatible to each other
but few registers. So the driver written by Semtech could be adopted to
receive wmBus datagrams. Some code from rtl-wmbus-Projekt has been recycled. ;-)

TODO:
	Hardware description
	Software description
