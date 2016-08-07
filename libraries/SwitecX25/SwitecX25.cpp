/*
 *  SwitecX25 Arduino Library
 *  Guy Carpenter, Clearwater Software - 2012
 *  Scott Brynen, snafu.ca - 2015-2016
 *
 *  Licensed under the BSD2 license, see license.txt for details.
 *
 *  All text above must be included in any redistribution.
 */

#include <Arduino.h>
#include "SwitecX25.h"

SwitecX25::SwitecX25(unsigned int steps, unsigned char pin1, unsigned char pin2, unsigned char pin3, unsigned char pin4)
{
	this->steps = steps;
	pins[0] = pin1;
	pins[1] = pin2;
	pins[2] = pin3;
	pins[3] = pin4;
	// figure out if we can use direct port manipulation; which is faster, 
	// but all the IO pins have to be on the same output port
	directPort = digitalPinToPort(pins[0]);
	if (directPort != NOT_A_PIN && directPort == digitalPinToPort(pins[1]) && 
		directPort == digitalPinToPort(pins[2]) && directPort == digitalPinToPort(pins[3])) 
	{
		directMask = digitalPinToBitMask(pins[0]) | digitalPinToBitMask(pins[1]) | digitalPinToBitMask(pins[2]) | digitalPinToBitMask(pins[3]);
		for (int i=0; i < stateCount; i++) 
		{
			directStateMap[i] = 0;
			byte mask = 1;
			for (int j=0; j < pinCount; j++) 
			{
				if ((stateMap[i] & mask) != 0)
					directStateMap[i] |= digitalPinToBitMask(pins[j]);
				mask <<= 1;
			}
		}
		*portModeRegister(directPort) |= directMask; // set output for our bits
		directMask = ~directMask; // and we'll want 0s for the bits we care about later
	}
	else {
		for (int i=0; i<pinCount; i++)
			pinMode(pins[i], OUTPUT);
		directPort = 0;
	}
	stopped = true;
	currentStep = 0;
	targetStep = 0;
	vel = 0;
	memcpy(accelTable, defaultAccelTable, sizeof(accelTable)); // our own copy
}

void SwitecX25::writeIO()
{
	if (directPort != 0)
	{
		volatile uint8_t *out;
		out = portOutputRegister(directPort);
		*out = (*out & directMask) | directStateMap[currentStep % stateCount];
	} else {
		byte mask = stateMap[currentStep % stateCount];
		byte iostate[4];
		iostate[0] = mask & 0x1; 
		iostate[1] = (mask & 0x2) >> 1; 
		iostate[2] = (mask & 0x4) >> 2; 
		iostate[3] = (mask & 0x8) >> 3; 
		noInterrupts();
		digitalWrite(pins[0], iostate[0]);
		digitalWrite(pins[1], iostate[1]);
		digitalWrite(pins[2], iostate[2]);
		digitalWrite(pins[3], iostate[3]);
		interrupts();
	}

}

void SwitecX25::stepUp()
{
  if (currentStep < steps) {
    currentStep++;
    writeIO();
  }
}

void SwitecX25::stepDown()
{ 
  if (currentStep > 0) {
    currentStep--;
    writeIO();
  }
}

void SwitecX25::zero()
{
  currentStep = steps - 1;
  for (unsigned int i=0; i<steps; i++) {
    stepDown();
    delayMicroseconds(RESET_STEP_MICROSEC);
  }
  currentStep = 0;
  targetStep = 0;
  vel = 0;
}


// set the current position (alternately to Zeroing above)
void SwitecX25::setZero(unsigned int pos)
{
	currentStep = pos;
	targetStep = pos;
	vel = 0;
}


// This function determines the speed and accel
// characteristics of the motor.  Ultimately it 
// steps the motor once (up or down) and computes
// the delay until the next step.  Because it gets
// called once per step per motor, the calcuations
// here need to be as light-weight as possible, so
// we are avoiding floating-point arithmetic.
//
// To model acceleration we maintain vel, which indirectly represents
// velocity as the number of motor steps travelled under acceleration
// since starting.  This value is used to look up the corresponding
// delay in accelTable.  So from a standing start, vel is incremented
// once each step until it reaches maxVel.  Under deceleration 
// vel is decremented once each step until it reaches zero.
void SwitecX25::advance()
{
  time0 = micros();
  
  // detect stopped state, but only when we're really slow
  if ((targetStep == currentStep) && (abs(vel) <= 10))
  {
    vel = 0;
	stopped = true;
    return;
  }
  if (targetStep > currentStep) 
  { // going up
    if (vel < 0)
      vel += 6;  //slam on the brakes we were going the wrong way
    else if ((targetStep - currentStep) >= vel)
      vel++;
    else
      vel--;
  }
  else if (targetStep < currentStep) 
  { // going down
    if (vel > 0)
      vel -= 6;  //slam on the brakes we were going the wrong way
    else if ((targetStep - currentStep) <= vel)
      vel--;
    else
      vel++;
  }
  if (vel > 0)
    stepUp();
  else if (vel < 0)
    stepDown();
  int i;
  for (i = DEFAULT_ACCEL_TABLE_SIZE - 1; i >= 0; i--)
    // will always match[0] if nothing else    
	if (abs(vel) >= accelTable[i][0]) 
    {
      microDelay = accelTable[i][1];
      break;
    } 
}

// set the accellation of the needle; 1.0 = normal speed, 
// range from approx 5% (0.05) to 200% (2.0)
void SwitecX25::setAccel(float pct)
{
  if (pct > 2.0)
    pct = 2.0;
  else if (pct < ACCEL_MIN)
    pct = ACCEL_MIN;
  for (int i = 0; i < DEFAULT_ACCEL_TABLE_SIZE; i++) 
    accelTable[i][1] = defaultAccelTable[i][1] / pct; // increase the delay = slower
}

void SwitecX25::setPosition(unsigned int pos)
{
  // pos is unsigned so don't need to check for <0
  if (pos >= steps) 
	pos = steps - 1;
  targetStep = pos;
  if (stopped) {
    // reset the timer to avoid possible time overflow giving spurious deltas
    stopped = false;
    time0 = micros();
    microDelay = 0;
  }
}

void SwitecX25::update()
{
  if (!stopped) {
    unsigned long delta = micros() - time0;
    if (delta >= microDelay) {
      advance();
    }
  }
}


//This updateMethod is blocking, it will give you smoother movements, but your application will wait for it to finish
void SwitecX25::updateBlocking()
{
  while (!stopped) {
    unsigned long delta = micros() - time0;
    if (delta >= microDelay) {
      advance();
    }
  }
}

