#include "LineDriver.h"

LineDriver::LineDriver(MeLineFollowerArray* _sensor){
  sensor = _sensor;
  motor_power=0;
  motor_kP =0;
  motor_left =0;
  motor_right=0;
  current_action = action::do_nothing;
  current_status = status::idle;
  action_timer=0;
}

void LineDriver::setParams(uint8_t power,float kP) {
  motor_power = power;
  motor_kP = kP;
};


LineDriver::action LineDriver::getCurrentAction() {
  return current_action;
}

LineDriver::status LineDriver::getCurrentStatus() {
  return current_status;
}

void LineDriver::doNothing() {
  current_action = action::do_nothing;
  current_status = status::idle;
}

void LineDriver::doFollowLine() {
  current_action = action::do_followline;
  current_status = status::driving;
}

void LineDriver::doTurnLeft() {
  current_action = action::do_turnLeftOnNextCrossing;
  current_status = status::driving;
}

void LineDriver::doTurnRight() {
  current_action = action::do_turnRightOnNextCrossing;
  current_status = status::driving;
}

void LineDriver::doStop() {
  current_action = do_stop;
  current_status = status::idle;
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


  switch (current_action) {

	case do_stop: {
		current_action = action::do_stop;
        current_status = status::idle;
		motor_left = 0;
		motor_right = 0;
		break;
	}

    case do_nothing:
		current_action = action::do_nothing;
        current_status = status::idle;
        break;

    case do_left:
    case do_right:
        current_status =  status::turning;
      //check for timeout
      //to-do calibrate robot to determine turn rate ==> timeout
      //to-do do a line calibration to determine average line width
      if ( millis() - action_timer < 2500 ) {
        //If we see the line then follow else keep on turning
        uint8_t raw =sensor->getRawValue();
        if ( raw==B011000 || raw==B001100 || raw==B000110 ) {
        	//line detected
        	current_action = do_followline;
        }
      }
      else {
    	//timeout situation: stop the robot
        current_action = action::do_stop;
        current_status =  status::aborted;
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
