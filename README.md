# Chiplab by EmulationOnline

The Chiplab is a research platform for studying the behavior of (initially just digital)
integrated circuits. 

More information about motivation & history can be found at:

https://emulationonline.com/systems/chiplab

## Structure

The long term vision is that there will be significant reuse or at least
similarity across chips, so this project is structured as a monorepo.

For a given chip supported in the lab, there are several relevant pieces:
- target. Programs meant to run on the microcontroller.
- firmware. This code runs on a microcontroller and interfaces with the 
target IC. Its core functionality is to set and read the pins
on the target IC.
- host. This is code that runs on a more powerful machine, and communicates
with the firmware program over serial.
- board. This is the electronics design for a board containing the
microcontroller, the target IC, and a serial interface for communicating
with the host.

For the 6502 chip, for example, code lives in the following folders:
- 6502/target
- 6502/firmware
- 6502/host
- 6502/board

## Contributing

If you see an issue, feel free to either open a Github issue. Please open an
issue before sending a pull request, so we can discuss. This helps make sure
we're on the same page and reduce time to implement fixes.

If you would like to see a new chip covered, feel free to open an issue. Since
supporting a chip requires several pieces, a good way to achieve this is to
build a prototype yourself by forking the 6502 code, and sending a pull request
once things are working.

## Consider donating

Donations are a great way of contributing. Funds are used both to fund
development and to acquire the hardware to add to the lab.

https://ko-fi.com/emulationonline
