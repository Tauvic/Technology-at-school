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
    //{can_nowhere,can_forward,can_left,can_right,can_left_right,can_left_right_forward}
	enum goal {do_RandomDrive,do_turnLeftOnNextCrossing,do_turnRightOnNextCrossing}
    enum action {do_nothing,do_stop,do_followline,do_turnLeftOnNextCrossing,do_turnRightOnNextCrossing,do_left,do_right};
    enum status {idle,driving,turning,done,aborted};


    //## Steer robot to next crossing
    //Command can get aborted by:
    // Manual override: the user takes control of the robot by sending explicit motor commands
    // Object collision: the robot detects an object in its path and stops
    // Loss of line: the robot does not see the line and stops
    // Wrong way: The robots reaches the intersection but cannot go left/right/forward
    //
    //goal {turnLeftOnNextCrossing, turnRightOnNextCrossing, goForwardOnNextCrossing}
    //lineSensor {online, offline, intersection}
    //collisionSensor {free, collision}
    //situation {beforeCrossing, onCrossing, afterCrossing}
    //internal action {followline, goLeft, goRight, goForward, stop}
    //result {busy, success, failed}
    //reason {lineLoss, noWay, collision, aborted, error, noPower}

    //Event record
    typedef struct {
      uint32_t time;
      uint8_t raw;
      uint8_t action;
    } event;
    
    LineDriver(MeLineFollowerArray* sensor);
    void setParams(uint8_t power,float kP);

    //semi-autonomous commands
    void doFollowLine(); //follow line and take random course
    void doTurnLeft();   //follow line and go left on next crossing
    void doTurnRight();  //follow line and go right on next crossing
    void doStop();       //stop all driving motors

    void doNothing();    //disable autonomous mode

    action drive();      //execute on each cycle

    action getCurrentAction();
    status getCurrentStatus();
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
    action  current_action;
    status  current_status;
    long    action_timer;
    CircularBuffer<event, 10> events;
};

#endif
