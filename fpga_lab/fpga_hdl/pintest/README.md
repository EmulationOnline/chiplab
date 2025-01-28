# Pintest

This project is meant to test the pins on the upduino breakout board.

The sketch asserts a 1 on each of the fpga outputs, one at a time.
The pattern:
- starts at 23 (upper left when usb is at the top), 
- down the left side to 28,
- up the right side, starting at 2 and ending with SS.
