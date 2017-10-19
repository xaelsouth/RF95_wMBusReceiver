# RF95_wMBusReceiver
wMbus receiver based on STM32L476 Nucleo and Hoperf RFM95

wMBus mode T1 and C1 are done. Receiving mode C1 datagrams is not good
now because of missing channel coding: RFM95 seems having problems with
clock long 0- or 1-sequences. Playing with bandwidth (and frequency deviation)
may help.

Project has been compiled on Windows 7 with toolchain installed by following instructions
on https://eewiki.net/display/microcontroller/Getting+Started+with+the+STM32L432+Nucleo-32+using+Eclipse+and+the+GNU+ARM+Toolchain
closely.

I've taken the liberty of copying documents for hardware parts (MCU and radio transceiver)
into the "doc" directory.

RFM95 is a clone of Semtech's SX1276. Both are widely compatible to each other
with exception of few registers. So that the driver written by Semtech could be adopted to
receive wmBus datagrams (s. src/radio and src/platform). For the original
Semtech's driver see doc/sx12xxDrivers-V2.1.0.zip and doc/sx1276_mbed.zip.

Doing first steps with STM32 microcontroller the use of STM32CubeMX for
code generation was enormously helpful.

The state machine from earlier rtl-wmbus-Projekt which works with synchronized bit stream has been recycled too.

I will be quite happy to know if you have been using that stuff in your own project.

TODO:
	* Hardware description
	* Software description
	* Formatting
