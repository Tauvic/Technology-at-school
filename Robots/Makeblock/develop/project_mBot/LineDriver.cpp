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

  switch (current_action) {
    case do_nothing:
       break;

    case do_right:
      //check for timeout
      if (true || millis() - action_timer < 5000) {
        //If we see the line then follow else keep on turning
        if ( abs(sensor->getPosition()) <= 32 ) current_action = do_followline;
      }
      else {
        //current_action = do_stop;
        //motor_left = 0;
        //motor_right = 0;        
      }
      

    case do_followline:

      #ifdef TEST_MODE
        if (true || millis() % 1000 == 0) {
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
          Serial.print(",D=");
          Serial.print(sensor->getDirection());
        }   
      #endif
    
      //{can_nowhere,can_forward,can_left,can_right,can_left_right,can_left_right_forward}
      switch ( sensor->getDirection() ) {
    
          case MeLineFollowerArray::can_left_right: 
          case MeLineFollowerArray::can_right: 
            {
              current_action = do_right;
              action_timer = millis(); // Start timer
              int correction = 50;
              motor_left  = motor_power + correction;
              motor_right = motor_power - correction;
              break;
            }
          
          case MeLineFollowerArray::can_left: 
            {
              current_action = do_stop;
              motor_left = 0;
              motor_right = 0;          
              break;
            }
        
          case MeLineFollowerArray::can_forward: 
            {
              int8_t delta = sensor->getPosition(); //Delta is difference in mm             
              int correction = delta * motor_kP;
              motor_left  = motor_power + correction;
              motor_right = motor_power - correction;
              current_action = do_followline;     
            
              #ifdef TEST_MODE
                    if (true || millis() % 1000 == 0) {
                      Serial.print(",d=");
                      Serial.print(delta);
                      Serial.print(",k=");
                      Serial.print(motor_kP);
                      Serial.print(",c=");
                      Serial.print(correction);
                    } 
              #endif
              
              break;
            }
    
          default: 
            current_action = do_stop;
            motor_left = 0;
            motor_right = 0;
      }

  }


#ifdef TEST_MODE
  if (true || millis() % 1000 == 0) {
    Serial.print(",L=");
    Serial.print(motor_left);
    Serial.print(",R=");
    Serial.print(motor_right);         
    Serial.print(",action=");
    Serial.print(current_action);  
    Serial.print(";\n");  
  }
#endif    

  //Store event in blackbox
  event ev = events.last();
  uint8_t raw = sensor->getRawValue();
  if (raw != ev.raw || ev.action != current_action) events.push(event{millis(),raw,current_action});
    
  return current_action;
}
