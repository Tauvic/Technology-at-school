#ifndef LineDriver_h
#define LineDriver_h

#include "Globals.h"
#include <Arduino.h>
#include "MeLineFollowerArray.h"

//https://github.com/rlogiacco/CircularBuffer
#include "CircularBuffer.h"


class LineDriver
{
    
 public:
    enum action {do_nothing,do_stop,do_followline,do_left,do_right}; 
    //Event record
    typedef struct {
      uint32_t time;
      uint8_t raw;
      uint8_t action;
    } event;
    
    LineDriver(MeLineFollowerArray* sensor);
    void setParams(uint8_t power,float kP);
    action drive();
    void doNothing();    
    void doFollowLine();
    void doStop();
    action getCurrentAction();
    int    getLeftPower();
    int    getRightPower();
    int    getEventCount();
    event  popEvent();
 private:
    MeLineFollowerArray* sensor;
    uint8_t motor_power = 0;
    float   motor_kP = 0.0;
    int     motor_left = 0;
    int     motor_right = 0;    
    action current_action = do_nothing;
    long    action_timer = 0;
    CircularBuffer<event, 10> events;
};

#endif
