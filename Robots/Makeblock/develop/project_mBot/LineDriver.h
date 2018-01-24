#ifndef LineDriver_h
#define LineDriver_h

#include "config.h"
#include <Arduino.h>
#include "MeLineFollowerArray.h"

//https://github.com/rlogiacco/CircularBuffer
#include "CircularBuffer.h"


class LineDriver
{
    
 public:
    //          {           can_nowhere,can_forward,can_left,can_right,can_left_right,can_left_right_forward}
    enum action {do_nothing,do_stop    ,do_followline,do_left,do_right}; 
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
    uint8_t motor_power;
    float   motor_kP;
    int     motor_left;
    int     motor_right;
    action current_action;
    long    action_timer;
    CircularBuffer<event, 10> events;
};

#endif
