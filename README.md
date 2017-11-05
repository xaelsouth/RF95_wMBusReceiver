# RF95_wMBusReceiver
wMbus receiver based on STM32L476 Nucleo and Hoperf RFM95

wMBus mode T1 and C1 have been finished. Receiving C1 datagrams is not very good  at the moment because this mode does not have channel coding: RFM95 seems to have some issues with clock reconstruction on long 0- or 1-sequences. Adjustment of the bandwidth (and frequency deviation) or playing with registers specific for GFSK filtering may help.

Project has been compiled on Windows 7 with toolchain. For the instructions how to install the toolchain see this source: https://eewiki.net/display/microcontroller/Getting+Started+with+the+STM32L432+Nucleo-32+using+Eclipse+and+the+GNU+ARM+Toolchain. 

I've taken the liberty of copying documents for hardware parts (MCU and radio transceiver) into the "doc" directory.

RFM95 is a clone of Semtech's SX1276. Both chips are widely compatible to each other with exception of few registers. So that the driver (FSK part only) written by Semtech has been adopted (s. src/radio and src/platform). For the original Semtech's driver see doc/sx12xxDrivers-V2.1.0.zip and doc/sx1276_mbed.zip.

Doing first steps with STM32 microcontroller with the use of STM32CubeMX for code generation was enormously helpful.

The state machine from an earlier rtl-wmbus project, which works with synchronized bit stream, has been recycled, too. Which is far from being optimal, because the state machine will be called once for _each_ bit! To get more speed on data processing you have to modify the state machine for working with at least six or eight bits at once (T1 or C1 mode).

I will be quite happy to know if you will apply that stuff in your own project and will inform me.

# Software description
SPI1 is used to configure the RFM95: to set its registers properly und to initiate receiving.

RFM95 then begins to demodulate the FSK signal which is present on the DIO2 pin in digital form as 0 or 1. Reconstructed clock signal is present in DIO1. DI2 and DIO1 are connected to the MCU via SPI3 (blue and yellow wires). SPI3 is configured for so called "Slave Receive Only" mode. DMA controller reads data from SPI3 in double buffering manner without any CPU intervention. The CPU will only drive the state machine to process received data. Each received datagrams will be sent on UART4 (115000 - 8N1) out.

# Hardware description
First control the jumpers on the Dragino Shield v1.4 which carries the RFM95:
* J_DIO5 is set, but don't care.
* J_DIO2 is set, but don't care.
* J_DIO1 is set, but don't care.
* SV2: pins 2 and 3 connected.
* SV4: pins 1 and 2 connected.
* SV3 : pins 2 and 3 connected.

SPI1:
* PA5  (D13, SPI1_SCLK)  <-> connected to SV2
* PA6  (D12, SPI1_MISO)  <-> connected to SV4
* PA7  (D11, SPI1_MOSI)  <-> connected to SV3
* PB6  (D10, CS)         <-> connected to RFM95 directly

SPI3:
* PC10 (SPI3_SCLK)  <-> DIO1 to Dragino Shield
* PC12 (SPI3_MOSI)  <-> DIO2 to Dragino Shield

UART4:
* PC15 (SERIAL4_TX) <-> RX wire (white) to RPI USB TTL adapter
* PH0  (SERIAL4_RX) <-> TX wire (green) to RPI USB TTL adapter
* GND wire (black) to RPI USB TTL adapter

OTHERS:
* PC7  (D9) <-> RESET to Dragino Shield
* PA10 (D2) <-> DIO0 to Dragino Shield
* PB10 (D6) <-> DIO1 to Dragino Shield
* PA8  (D7) <-> DIO2 to Dragino Shield
* NC        <-> DIO3 (not connected)
* NC        <-> DIO4 (not connected)
* PA9  (D8) <-> DIO5 to Dragino Shield
