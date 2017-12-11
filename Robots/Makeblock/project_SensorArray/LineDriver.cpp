#include "LineDriver.h"
LineDriver::LineDriver(MeLineFollowArray _sensor){
  sensor = _sensor;
}

void LineDriver::setParams(uint8_t power,float kP,float t_ratio,int t_time) {
  motor_power = power;
  motor_kP = kP;
  turn_ratio = t_ratio;
  turn_time = t_time;
};

void sendDebug(char* buffer) {
  int bufSize = Serial.availableForWrite();
  if (bufSize > strlen(buffer)) {
    Serial.print(buffer);
  }
}

void sendDebug(uint8_t val) {
  int bufSize = Serial.availableForWrite();
  if (bufSize > 1) {
    Serial.print(val);
  }
}

LineDriver::actions LineDriver::getCurrentAction() {
  return current_action;
}

void LineDriver::doForward() {
  current_action = do_forward;
}


void LineDriver::doStop() {
  current_action = do_stop;
  motor_left = 0;
  motor_right = 0;  
}

uint8_t LineDriver::getLeftPower() {
  return motor_left;
}

uint8_t LineDriver::getRightPower() {
  return motor_right;
}

void LineDriver::drive() {
  
    //always set motor to fail safe speed
    motor_left = 0;
    motor_right = 0;

    if ( sensor.readSensor() ) {

      //Decide if we want to change direction or continue line following


      //if we do_forward then check line position
      //if we do_left (or right) keep turning until we are on the line

      //determine what we want, can and should do
      //do_left, do_right, do_stop, do_forward
            
      switch (sensor.getDirection()) {
      /*  
         case MeLineFollowArray::can_left:
               if ( current_action == do_left ) {
                 if ( millis() - action_timer > turn_time ) current_action = do_forward;                
                 break;
               };
               
               if ( current_action == do_forward ) {
                 action_timer == millis(); // reset start time
                 current_action = do_left;
                 uint8_t correction = motor_power * turn_ratio;
                 motor_left  = motor_power + correction;
                 motor_right = motor_power - correction;                                 
                 break;
               };
               
               break;
               
         case MeLineFollowArray::can_right:
               if ( current_action == do_right ) {
                 if ( millis() - action_timer > turn_time ) current_action = do_forward;                
                 break;
               };
               
               if ( current_action == do_forward ) {
                 action_timer == millis(); // reset start time
                 current_action = do_right;
                 uint8_t correction = -motor_power * turn_ratio;                 
                 motor_left  = motor_power + correction;
                 motor_right = motor_power - correction;                     
                 break;
               };
               
               break;
*/               
         default:
              current_action = do_forward;
              int8_t delta = sensor.getPosition(); //Delta is difference in mm             
              int8_t correction = delta * motor_kP;
              motor_left  = motor_power + correction;
              motor_right = motor_power - correction;         
              
              sendDebug("p=");
              sendDebug(delta);
              
      };
      
      
      //}
    } else {
      current_action = do_stop;
      motor_left = 0;
      motor_right = 0;
    }

    sendDebug(",L=");
    sendDebug(motor_left);
    sendDebug(",R=");
    sendDebug(motor_right);         
    sendDebug(",action=");
    sendDebug(current_action);    
    sendDebug(";\n");
  
}
