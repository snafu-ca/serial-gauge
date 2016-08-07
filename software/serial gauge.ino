#include <SwitecX25.h>

/*
* Copyright (c) 2015, Scott Brynen - snafu.ca
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without modification, are permitted 
* provided that the following conditions are met:
* 
* 1. Redistributions of source code must retain the above copyright notice, this list of 
* conditions and the following disclaimer.
* 
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of 
* conditions and the following disclaimer in the documentation and/or other materials provided 
* with the distribution.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS 
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
* COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
* GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
* POSSIBILITY OF SUCH DAMAGE.
*/

/* 
* Serial Meter Firmware
*  1.0 - Oct 2015 SB - Initial Release; 3 speeds w/ meter select
*/

#include <SwitecX25.h>
#include <SoftwareSerial.h>

#define MAXSTEP 315*3  // 315 degrees of range = 315x3 steps = 945 steps
#define RXPIN  2       // Serial IO is pin2 of the ATTiny
#define THISMETER 'A'  // so multiple meters in parallel can be controlled separately


SwitecX25 meter(MAXSTEP, 0,1,3,4);
SoftwareSerial rcvSerial(RXPIN, -1, true);  // uses negative serial logic
int inData = -1;
char meterid = THISMETER;

/***
  setup() - set the IO pins, and sweep the meter to reset
***/
void setup(void) {
	pinMode(RXPIN, INPUT_PULLUP); // SoftwareSerial sets it to plain input
  rcvSerial.begin(2400);
	//meter.zero();
}

/***
  loop() - main code here
***/
void loop(void) {
	meter.update(); // stepping is done in the background
	if (rcvSerial.available()) 
	{
		char c = rcvSerial.read();
		if (c >= 'A' && c <= 'Z' || (c >= 'a' && c <= 'z')) // select meter
			meterid = c & 0xDF; // convert to upper
		else if (c == '*') // select all meters
			meterid = THISMETER;
		if (meterid == THISMETER)
		{
			if (c >= '0' && c <= '9') 
			{
				if (inData < 0) // -1 = no previous data
					inData = 0;
				inData = inData*10 + (int(c) - '0');
				if (inData >= MAXSTEP) 
					inData = MAXSTEP - 1;
			} 
			else if ((c == '\n' || c == '\r') && inData >= 0) 
			{
				meter.setPosition(inData);
				inData = -1; // -1 means no data
			} 
      else if (c == '@')
      {
        meter.setZero(inData);
        inData = -1;
      }
      else if (c == '!') 
        meter.zero();
			else if (c == '+') 
				meter.setAccel(1.0);
			else if (c == '-') 
				meter.setAccel(0.75);
			else if (c == '=') 
				meter.setAccel(0.5);
		}
	}	  
}
