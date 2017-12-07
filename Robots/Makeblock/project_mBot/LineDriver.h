#ifndef LineDriver_h
#define LineDriver_h

#include <Arduino.h>
#include "LineArray.h"

class LineDriver
{
    
 public:
    enum actions {do_stop,do_forward,do_left,do_right}; 
    LineDriver(LineArray sensor);
    void setParams(uint8_t power,float kP,float t_ratio, int t_time);
    void drive();
    void doForward();
    void doStop();
    actions getCurrentAction();
    uint8_t getLeftPower();
    uint8_t getRightPower();    
 private:
    LineArray sensor;
    uint8_t motor_power = 0;
    float   motor_kP = 0.0;
    uint8_t motor_left = 0;
    uint8_t motor_right = 0;    
    actions current_action = do_stop;
    long    action_timer;
    float   turn_ratio;
    int     turn_time;
};

#endif
