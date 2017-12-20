#ifndef LineDriver_h
#define LineDriver_h

#include <Arduino.h>
#include "MeLineFollowerArray.h"

class LineDriver
{
    
 public:
    enum action {do_nothing,do_stop,do_forward,do_left,do_right}; 
    LineDriver(MeLineFollowerArray sensor);
    void setParams(uint8_t power,float kP);
    action drive();
    void doNothing();    
    void doForward();
    void doStop();
    action getCurrentAction();
    uint8_t getLeftPower();
    uint8_t getRightPower();    
 private:
    MeLineFollowerArray sensor;
    uint8_t motor_power = 0;
    float   motor_kP = 0.0;
    uint8_t motor_left = 0;
    uint8_t motor_right = 0;    
    action current_action = do_nothing;
    long    action_timer;
    float   turn_ratio;
    int     turn_time;
};

#endif
