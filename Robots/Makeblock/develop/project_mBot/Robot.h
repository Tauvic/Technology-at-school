#ifndef Robot_h
#define Robot_h

#include <Arduino.h>

class Robot {
  public:
    int getFreeMem();
    void loopStart();
    void loopEnd();
    int getLoopTime();
  private:
    unsigned long loopStartTime;
    int loopTime;
};

#endif
