#include "LineDriver.h"

LineDriver::LineDriver(MeLineFollowerArray _sensor){
  sensor = _sensor;
}

void LineDriver::setParams(uint8_t power,float kP) {
  motor_power = power;
  motor_kP = kP;
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

LineDriver::action LineDriver::getCurrentAction() {
  return current_action;
}

void LineDriver::doNothing() {
  current_action = do_nothing;
}

void LineDriver::doForward() {
  current_action = do_forward;
}


void LineDriver::doStop() {
  current_action = do_stop;
}

uint8_t LineDriver::getLeftPower() {
  return motor_left;
}

uint8_t LineDriver::getRightPower() {
  return motor_right;
}

LineDriver::action LineDriver::drive() {

   if (current_action == do_nothing) return do_nothing;

   int8_t delta = sensor.getPosition(); //Delta is difference in mm             

   if (delta != MeLineFollowerArray::invalid) {
      int8_t correction = delta * motor_kP;
      motor_left  = motor_power - correction;
      motor_right = motor_power + correction;
      current_action = do_forward;     
    } else {
      current_action = do_stop;
      motor_left = 0;
      motor_right = 0;
    }

/*
    sendDebug(",L=");
    sendDebug(motor_left);
    sendDebug(",R=");
    sendDebug(motor_right);         
    sendDebug(",action=");
    sendDebug(current_action);    
    sendDebug(";\n");  
*/
    return current_action;
}
