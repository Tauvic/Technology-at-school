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

void LineDriver::doFollowLine() {
  current_action = do_followline;
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

int LineDriver::getEventCount() {
  return events.size();
}

LineDriver::event LineDriver::popEvent() {
  return events.pop();
}


LineDriver::action LineDriver::drive() {
  
#if defined (LOG_SENSOR) && LOG_LEVEL >= LOG_INFO
  Serial.print("@A,");
  Serial.print(millis());
  uint8_t raw = sensor->getRawValue();
  Serial.print(",v");
  Serial.print(raw,DEC);
  Serial.print(",d");
  Serial.print(sensor->getDirection());  
  Serial.print(",a");
  Serial.print(current_action);  
#endif 

  switch (current_action) {
    case do_nothing:
       break;

    case do_left:
    case do_right:
      //check for timeout
      //to-do calibrate robot to determine turn rate ==> timeout
      //to-do do a line calibration to determine average line width
      if ( millis() - action_timer < 2500 ) {
        //If we see the line then follow else keep on turning
        uint8_t raw =sensor->getRawValue();
        if ( raw==B011000 || raw==B001100 || raw==B000110 ) current_action = do_followline;
      }
      else {
        current_action = do_stop;
        motor_left = 0;
        motor_right = 0;        
      }
      break;
      

    case do_followline:
    
      //{can_nowhere,can_forward,can_left,can_right,can_left_right,can_left_right_forward}
      switch ( sensor->getDirection() ) {

          case MeLineFollowerArray::can_left_right: 
          {
            int correction = 0;

            int dice = random(0,100);
            if (dice < 33) {
                current_action = do_left;
                correction = -50;
            } else if (dice < 66) {
                current_action = do_followline;
                correction = 0;
            } else {
                current_action = do_right;
                correction = 50;
            }

            action_timer = millis(); // Start timer
            motor_left  = motor_power + correction;
            motor_right = motor_power - correction;
            
            break;
          }
         

          case MeLineFollowerArray::can_left: 
            {
              current_action = do_left;
              int correction = -50;
              action_timer = millis(); // Start timer
              motor_left  = motor_power + correction;
              motor_right = motor_power - correction;
              break;
            }
          
          case MeLineFollowerArray::can_right: 
            {
              current_action = do_right;
              int correction = 50;
              action_timer = millis(); // Start timer
              motor_left  = motor_power + correction;
              motor_right = motor_power - correction;
              break;
            }
                  
          case MeLineFollowerArray::can_forward: 
            {
              int8_t delta = sensor->getPosition(); //Delta is difference in mm             
              int correction = delta * motor_kP;
              motor_left  = motor_power + correction;
              motor_right = motor_power - correction;
              current_action = do_followline;     
            
              #if defined (LOG_ACTUATOR) && LOG_LEVEL>=LOG_DEBUG
                      Serial.print(",e");
                      Serial.print(delta);
                      Serial.print(",k");
                      Serial.print(motor_kP);
                      Serial.print(",c");
                      Serial.print(correction);
              #endif
              
              break;
            }
    
          default: {
            current_action = do_stop;
            motor_left = 0;
            motor_right = 0;
          }
      }
      break;

  }

#if defined (LOG_ACTUATOR) && LOG_LEVEL>=LOG_INFO
  Serial.print(",l");
  Serial.print(motor_left);
  Serial.print(",r");
  Serial.print(motor_right);         
  Serial.print(",a");
  Serial.print(current_action);
#endif

#if ( defined (LOG_SENSOR) || defined (LOG_ACTUATOR) ) && LOG_LEVEL >= LOG_INFO 
  Serial.print(";\n");
#endif    

  //Store event in blackbox
  //event ev = events.last();
  //if (raw != ev.raw || ev.action != current_action) events.push(event{millis(),raw,current_action});
    
  return current_action;
}
