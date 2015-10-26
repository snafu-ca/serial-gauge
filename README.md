# SERIAL GAUGE
### http://github.com/snafu-ca/serial-gauge

This is a micro-project to produce a completely standalone serial port
driven guage motor, for retrofitting into pretty much any analog display.
The range of motor is 315 degrees, in 1/3 degree increments, or 945 discrete
steps.

The X27.168 is a common stepper motor used in the automotive industry
to drive dashboard guages, and works well for driving most small pointers.
I've found that if you don't have a pointer, go down to a local hobby store
and find a nice clock hand that will suit your purpose. There are also 
online stores with thousands of clock hands to choose from.  The size might
not fit exactly, but a small dab of glue fixes that.

The sky is the limit for what it can be used for, and because the input
is opto isolated from the drive and drive supply voltage it will work
pretty much on any signal; positive or negative going.

## SERIAL COMMANDS

The software is set up to receive data at 2400 baud and supports the 
following commands down the serial port

* 0-944<newline>  set the gauge to this value 
* =               set very slow mode
* -               set slow mode
* +               set normal speed mode
* *               select all meters
* A-Z             select meter A, meter B... (see source code for details)
<newline> can be either a carriage return or line feed

## SERIAL INPUT

The serial input is completely [both signal and ground] opto-isolated from 
the power supply, so it will accept 3.3 - 15 volt serial signals. This means 
that it can be driven 3.3v drivers like a Raspberry Pi (3V3); standard TTL 
such as an USB FTDI serial; or even RS-232 at +/- 15v.  For 3.3V, use the 
470R resistor for 5V or more, use the 2k2 resistor.  Also, as the input 
impedance is relatively high, it's possible to drive a few gauges in parallel. 

The polarity for the serial port is positive = logic high. This is backwards
from RS-232, so if you're driving straight from an RS-232 source, you'll need
to swap the S+ / S- lines

## DRIVING IT FROM AN FTDI TYPE SERIAL PORT

(Check github for a picture of the wiring diagram)
J1: +ve		FTDI: +5v
J1: -ve		FTDI: GND
J1: S+		FTDI: TX
J1: S-		FTDI: GND
Start a serial program (the monitor in the Arduino dev environment, or puTTY set 
to serial), set the baud rate to 2400, and start typing. You wont see anything 
echoed back on the screen, but it should work when you type "500<enter>"

## DRIVING IT FROM A RASPBERRY PI

(check github for a picture of the wiring diagram)
J1: +ve		RPi: Pin 2 (or pin 4) +5v
J1: -ve		RPi: Pin 6 GND
J1: S+		RPi: Pin 8 UART0_TX0
J1: S-		RPi: Pin 6 GND
On the raspberry pi a few steps are required to stop the O/S from using the serial
port to allow you to use it (do these commands as root):
1. edit the /etc/inittab file, and comment out (add a # at the beginning of the line) 
the line reads T0:23:respawn:/sbin/getty -L ttyAMA0 115200 vt100
2. run "init q" (this will reload inittab and stop the above getty process)
3. set the baud rate to 2400 by running:  stty /dev/ttyAMA0 2400
4. send a test command like this: echo 500 >/dev/ttyAMA0

SOFTWARE
========
If you want to reprogram on onboard controller to change the baud rate, device ID
or completely change it to do something else (a tachometer comes to mind), the source 
code, and related (modified libraries) are available for download at 
http://github.com/snafu-ca
Note you require both the SwitecX25 (stepper motor) library and the included replacement 
library for softwareSerial (it's modified to allow a receive only setup). For information 
on how to install the libraries, see the Arduino documentation.

Also if you're reprogramming the device, remember it's an ATTINY85 @ 8MHz; internal clock;
remember to 'burn bootloader' which sets the fuses, before burning the software.



