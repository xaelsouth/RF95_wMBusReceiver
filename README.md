# RF95_wMBusReceiver
wMbus receiver based on STM32L476 Nucleo and Hoperf RFM95

wMBus mode T1 and C1 have been finished. Receiving C1 datagrams is not good enough at the moment because this mode does not have channel coding: RFM95 seems to have some issues with clock reconstruction on long 0- or 1-sequences. Adjustment of the bandwidth (and frequency deviation) should help.

Project has been compiled on Windows 7 with toolchain. For the instructions how to install the toolchain see this source: https://eewiki.net/display/microcontroller/Getting+Started+with+the+STM32L432+Nucleo-32+using+Eclipse+and+the+GNU+ARM+Toolchain. 

I've taken the liberty of copying documents for hardware parts (MCU and radio transceiver) into the "doc" directory.

RFM95 is a clone of Semtech's SX1276. Both chips are widely compatible to each other with exception of few registers. So that the driver (FSK part only) written by Semtech has been adopted (s. src/radio and src/platform). For the original Semtech's driver see doc/sx12xxDrivers-V2.1.0.zip and doc/sx1276_mbed.zip.

Doing first steps with STM32 microcontroller with the use of STM32CubeMX for code generation was enormously helpful.

The state machine from an earlier rtl-wmbus project, which works with synchronized bit stream, has been recycled, too.

I will be quite happy to know if you will apply that stuff in your own project and will inform me.

# Software description
RFM95 demodulates the FSK signal which is present on the DIO2 pin as 0 or 1. Reconstructed clock signal is present in DIO1. DI2 and DIO1 are connected with the MCU to SPI interface (SPI3) pins. The SPI interface is configured for so called Slave Receive Only mode. DMA controller reads data from SPI in double buffering manner without any CPU interention. The CPU will only drive the state machine to process received data. Each received datagrams will be sent to UART (UART2) (115000 - 8N1) out.

# Hardware description
TODO
