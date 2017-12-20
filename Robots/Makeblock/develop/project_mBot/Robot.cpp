#include "Robot.h"

//Calculate amount of free memory
int Robot::getFreeMem() {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void Robot::loopStart() {
  loopStartTime = micros();
}


void Robot::loopEnd() {
  loopTime = micros() - loopStartTime;
}


int Robot::getLoopTime() {
  return loopTime;
}
