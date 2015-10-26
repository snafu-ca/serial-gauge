ASSEMBLY INSTRUCTIONS
=====================
D1-9	1N4148 diode
C1		22uF capacitor
C2		<optional, see below>
C3		0.1uF (104) capacitor
R1		470R or 2k2 (see below)
U1		ATTINY85-20PU (pre-programmed)
U2		78L05 (only for >5v supply, otherwise use jumper-see below)
U3		FOD817B  opto isolator (aka PC817)
M1		Switec/Juken X27.168 gauge stepper motor

Be careful about the polarized components the diodes, semiconductors and the 
22uF capacitor. For the diodes, the stripes are marked on the PCB. For the 
capacitor, the negative side has a square pad which lines up with the white 
stripe on the capacitor. For U1 and U3 the dot on the IC goes in the top
left, again marked with a square pad.

Note the motor can mount on either side of the circuit board; it will just
spin in the opposite direction.

CONNECTIONS
===========
J1: POWER AND DATA
S- & S+ are the serial input lines.  -ve & +ve are the power supply lines. 
The power and the serial data are completely isolated from each other, so
it's possible drive the serial with data that's not referenced to the 
logic ground of the drivers.

(Note some early boards have "?ve" and "S?" instead of "-ve" and "S-" 
respectively on the PCB silkscreen.)

J2: OPTIONAL ISP PROGRAMMING PORT
This is only required if you intend to reprogram the ATTINY in circuit.

OPTIONAL/MULTIPLE PART VALUES
=============================
U2: Voltage Regulator:
For a 12 volt supply, assemble using provided regulator U2. For 5 volt 
supply, insert a wire vertically between pins 1 & 3 of U2 (shown with a small 
white dash on the PCB); to bypass the regulator.
 
R1: Serial Port voltage
For 3.3v drive, use 470R for R1. For 5V or RS-232, use 2k2

C2: High frequency noise reducing capacitor
Only required if your source is electrically noisy, somewhere between 220pF 
and 10nF should help cut down on high-frequency noise into the serial port. 
I've never needed it, but space is provided for the off chance you might
need it.


