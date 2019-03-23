# The Timestamper

This project lets you get get exact timestamps of 'events' (electric pulses on an input).

## Hardware
The hardware (schematic and PCB) are designed in KiCad 5, and can be found in the ./hardware/cpld/ directory. The 'heart'
of the design is a Xilinx XC9572XL CPLD. This CPLD implements a capture-counter which timestamps the arrival of events
with a resolution of 10ns. This CPLD is programmed in VHDL using Xilinx's ISE 14.7 (https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/design-tools/v2012_4---14_7.html).

The VHDL project resides in the ./hardware/cpld/ directory. The XC9572XL can be programmed using the xc3sprog (http://xc3sprog.sourceforge.net/) utility.
Both the 'ft232h' (with a serial FT232H UART chip) and the 'sysfsgpio' (sysfsgpio on the Raspberry Pi) have been
successfully used to program the chip.

## Firmware
The firmware runs on an Atmel Atmega328p, which makes it compatible with the arduino. This way, using the default bootloader for,
for example, the Arduino pro mini 3.3v @8Mhz, we can easily program the firmware using avrdude. The firmware simply reads the
captured values from the CPLD and passes them on to a CH340G serial to USB UART. The firmware can be found in ./firmware/atmega328/

## Software
The software just outputs the values from the device onto stdout. See ./software/

# Theory of operation
The board is fed 3 signals:
 - A (very stable) 10MHz reference clock (I.E. from an OCXO or atomic rubidium reference)
 - A precise PPS signal, indicating the start of a UTC second. (I.E. from a GPS timing receiver)
 - An event signal that needs to be precisely timestamped.
 
 The 10Mhz is fed into a PLL (A Texas Instruments CDCE925), and multiplied to 100MHz, which is then fed to the CPLD.
 Inside the CPLD, this clocks a 27-bit counter. This counter is (synchronously) reset on the rising edge of the PPS
 signal. The event signal is sampled at every clock pulse, and when a rising-edge is detected, the current count is stored
 inside into a register and the capture counter is 'disarmed'. The CPLD then notifies the Atmega328P through an interrupt
 which, in turn, reads the value from the CPLD, places it in a buffer, and instructs the CPLD te re-arm the capture logic.
 The capture value is then simply sent out of the USB UART to the PC. We now have the fractional part of the timestamp on
 the PC. This can be combined with the current time on the PC to form a complete timestamp.
 NOTE: This assumes that the time on the PC that is reading the timestamps is well within a 500ms tolerance.
 
 # Links and other stuff
 See https://www.febo.com/pages/soekris/ for details on making an accurate NTP server.
 In addition, a Motorola m12+ GPS timing receiver together with a saw-tooth correction circuit is used to provide both a
 UTC time-source for the NTP server, as well as a PPS pulse for the Timestamper board.
 The Timestamper board also contains an 'auxiliary' output, which could be programmed to output a 33MHz clock that could
 be fed into the Soekris' motherboard to provide an even more accurate NTP time source (untested).
