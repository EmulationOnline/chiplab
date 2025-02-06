# RAM Echo

This project is meant to test the ram blocks within the FPGA.

After reset, the FPGA will read 16k bytes via spi. It will then
echo back those same 16K bytes.

## Pin assignments

These are applied in the pcf file. They are documented here for visibility.

| Function | FPGA Pin | FPGA Pin # | PCB Label / Position |
| -- | -- | -- | -- |
| Clk | FPGA_IOT46B | 35 | n/a |

TODO:
- Reset
- SPI
