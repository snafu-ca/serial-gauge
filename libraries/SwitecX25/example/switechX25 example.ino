#include "SwitecX25.h"

// 315 degrees of range = 315x3 steps = 945 steps
// declare motor1 with 945 steps on pins 4-7
SwitecX25 motor1(315*3, 4,5,6,7);

// declare motor2 with 945 steps on pins 8-11
SwitecX25 motor2(315*3, 8,9,10,11);

void setup(void) {
  Serial.begin(9600);
  Serial.println("Go!");

  // run both motors against stops to re-zero
  motor1.zero();   // this is a slow, blocking operation
  motor2.zero();  
  motor1.setPosition(427);
  motor2.setPosition(427);
}

void loop(void) {
  // update motors frequently to allow them to step
  motor1.update();
  motor2.update();

  // do stuff, call motorX.setPosition(step) to 
  // direct needle to new position.
}
