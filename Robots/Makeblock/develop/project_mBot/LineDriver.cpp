#include "LineDriver.h"

LineDriver::LineDriver(MeLineFollowerArray* _sensor){
  sensor = _sensor;
}

void LineDriver::setParams(uint8_t power,float kP) {
  motor_power = power;
  motor_kP = kP;
};


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

int LineDriver::getLeftPower() {
  return -motor_left;
}

int LineDriver::getRightPower() {
  return motor_right;
}

LineDriver::action LineDriver::drive() {

   if (current_action == do_nothing || sensor == 0) return do_nothing;

/*
    uint8_t raw = sensor->getRawValue();
    Serial.print("R");
    Serial.print(raw,DEC);
    Serial.print(',');
    for (int i=0;i<6;i++) {
      if (bitRead(raw,i)) 
        Serial.print('1');
      else
        Serial.print('0');
    }
    uint8_t *db =sensor->getDebugInfo();
    Serial.print(",T");  
    Serial.print(db[0],DEC);  
    Serial.print(",S");  
    Serial.print(db[1],DEC);  
    Serial.print(',');
    for (int i=2;i<8;i++) {
        Serial.print(db[i],DEC);
        Serial.print(',');
    }   
*/

   if ( sensor->isValid() ) {
      int8_t delta = sensor->getPosition(); //Delta is difference in mm             
      int correction = delta * motor_kP;
      motor_left  = motor_power + correction;
      motor_right = motor_power - correction;
      current_action = do_forward;     

      /*if (millis() % 1000 == 0) {
        Serial.print(",d=");
        Serial.print(delta);
        Serial.print(",k=");
        Serial.print(motor_kP);
        Serial.print(",c=");
        Serial.print(correction);
      } */

      
    } else {
      current_action = do_stop;
      motor_left = 0;
      motor_right = 0;
    }

    /*if (millis() % 1000 == 0) {
      Serial.print(",L=");
      Serial.print(motor_left);
      Serial.print(",R=");
      Serial.print(motor_right);         
      Serial.print(",action=");
      Serial.print(current_action);  
      Serial.print(";\n");  
    }
    */

    return current_action;
}
