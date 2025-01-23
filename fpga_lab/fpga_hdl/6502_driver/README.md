# 6502_driver

## Function

The 6502_driver HDL is meant as a compatible replacement for the [atmega32u4 based Chiplab board](https://chiplab.emulationonline.com/6502/). It is used to capture cycle-by-cycle recordings of all pins from a 6502 as it executes a program of choice.

After receiving
a 6502 memory dump, it will single-step the 6502 for a configurable number of cycles. At each step it will record(buffer) outputs from the 6502. Once execution is done, it will
send this buffered output to the microcontroller to forward to the host.

## System Architecture

```
TODO: NICER SYSTEM DIAGRAM

Host machine
 
 ^
via usb
 V
 
Microcontroller (atmega32u4)

 ^
via SPI (programming + communication)
 V
  
FPGA (ICE40)
 
 ^
via GPIO, assert & inspect pins
 V
  
6502 
 
<= usb => 

```

## Execution Details
IO pins are to be connected to a CMOS 6502 chip. The FPGA simulates a memory bus for the 6502, and records all signals out of the 6502.

Once programmed, the FPGA repeatedly waits for execution requests via SPI. These consist of:
1. 6502 memory dump (65k bytes)
2. Number of cycles (NUM_CYCLES) for which to run (2 bytes little endian)

The FPGA will then reset the 6502, and allow the 6502 to execute. On each cycle, the FPGA will record the outputs from the 6502 into internal block ram.

After allowing the 6502 to execute for NUM_CYCLES, the FPGA will upload execution details to the Microcontroller. (Protocol TBD)

## Contributing / Development

This project contains Makefiles which allow synthesis of bitstreams with FOSS tools (Yosys OSS-CAD suite).

```
make bits
```

Verilator is also used to generate c++ models from the HDL. Unit tests are written in C++ and use the GoogleTest framework.

```
make tests # run unit tests
```