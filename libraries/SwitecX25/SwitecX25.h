/*
 *  SwitecX25 Arduino Library
 *  Guy Carpenter, Clearwater Software - 2012
 *  Scott Brynen, snafu.ca - 2015
 *
 *  Licensed under the BSD2 license, see license.txt for details.
 *
 *  All text above must be included in any redistribution.
 */

 
#ifndef SwitecX25_h
#define SwitecX25_h
#include <Arduino.h>

// This table defines the acceleration curve.
// 1st value is the speed step, 2nd value is delay in microseconds
// 1st value in each row must be > 1st value in subsequent row
// 1st value in last row should be == maxVel, must be <= maxVel
static unsigned short defaultAccelTable[][2] = {
  {  0, 3000},
  { 50, 1500},
  {100, 1000},
  {150,  800},
  {225,  600} 
};
#define DEFAULT_ACCEL_TABLE_SIZE (sizeof(defaultAccelTable)/sizeof(*defaultAccelTable))
// calculate the minimum rate multiplier that doesn't cause the above table to overflow
#define ACCEL_MIN ((float)defaultAccelTable[0][1] / (unsigned short)(0 - 1)) + 0.001
// During zeroing we will step the motor CCW 
// with a fixed step period defined by RESET_STEP_MICROSEC
#define RESET_STEP_MICROSEC 800
// experimentation suggests that 400uS is about the step limit 
// with my hand-made needles made by cutting up aluminium from
// floppy disk sliders.  A lighter needle will go faster.

// State  3 2 1 0   Value
// 0      1 0 0 1   0x9
// 1      0 0 0 1   0x1
// 2      0 1 1 1   0x7
// 3      0 1 1 0   0x6
// 4      1 1 1 0   0xE
// 5      1 0 0 0   0x8
static byte stateMap[] = {0x9, 0x1, 0x7, 0x6, 0xE, 0x8};

class SwitecX25
{
 public:
   static const unsigned char pinCount = 4;
   static const unsigned char stateCount = 6;
   unsigned char pins[pinCount];
   unsigned char currentState;    // 6 steps 
   unsigned int currentStep;      // step we are currently at
   unsigned int targetStep;       // target we are moving to
   unsigned int steps;            // total steps available
   unsigned long time0;           // time when we entered this state
   unsigned int microDelay;       // microsecs until next state
   int vel;                       // steps travelled under acceleration
   boolean stopped;               // true if stopped
   unsigned short accelTable[DEFAULT_ACCEL_TABLE_SIZE][2]; // accel table can be modified.
   
   SwitecX25(unsigned int steps, unsigned char pin1, unsigned char pin2, unsigned char pin3, unsigned char pin4);

   void stepUp();
   void stepDown();
   void zero();
   void update();
   void updateBlocking();
   void setAccel(float pct);
   void setPosition(unsigned int pos);
  
 private:
   void advance();
   void writeIO();
};


#endif

