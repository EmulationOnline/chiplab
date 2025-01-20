# FPGA HDL

This directory houses HDL (hardware-description language) firmware
meant to be flashed onto the FPGA used on Chiplab boards.

Each directory contains more information about the purposes of the firmware.

## Building

Each HDL was designed to be synthesized and flashed using open source tools.
Each project contains a makefile which assumes the [Yosys OSS-CAD
suite](https://github.com/YosysHQ/oss-cad-suite-build).

Projects may also use [Verilator](https://github.com/verilator/verilator) for automated tests against models.
