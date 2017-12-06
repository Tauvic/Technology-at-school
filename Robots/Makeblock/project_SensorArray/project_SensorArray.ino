#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#include "LineArray.h"
#include "LineDriver.h"
#include <MeMCore.h>

LineArray lineArray;
LineDriver lineDriver(lineArray);

MeSerial se;
MeRGBLed rgbled_7(7, 7==7?2:4);
MeDCMotor motor_9(9);
MeDCMotor motor_10(10);    

void setup(){
    
    Serial.begin(115200); // 115200 57600

    //color Leds blue
    rgbled_7.setColor(0,0,0,255);
    rgbled_7.show();
    
    //set line sensor pin
    lineArray.setPort(12);

    lineDriver.setParams(100,0.7,0.1,500);

/*
    //Wait until button pressed
    pinMode(A7,INPUT);
    while(!((0^(analogRead(A7)>10?0:1))))
    {
        _loop();
    }
*/

}

const uint8_t command_mode = 0;
const uint8_t line_mode = 1;

uint8_t mode = command_mode;

void loop(){

    long startTime = millis();

    //Handle communication
    //switch between command and line mode
    
    //Do other things

    if (mode == line_mode) {
      
      lineDriver.drive();  
  
      switch ( lineDriver.getCurrentAction() ) {
        case LineDriver::do_stop: 
          rgbled_7.setColor(0,255,0,0); // Both red
          rgbled_7.show();          
          break;
        case LineDriver::do_forward:
          rgbled_7.setColor(0,0,255,0); // Both green
          rgbled_7.show();          
          break;
        case LineDriver::do_left:
          rgbled_7.setColor(2,0,255,0); // Left green
          rgbled_7.show();          
          break;
        case LineDriver::do_right: 
          rgbled_7.setColor(1,0,255,0); // Right green
          rgbled_7.show();          
          break;
      }
  
      //Control the motor
      uint8_t left = lineDriver.getLeftPower();
      uint8_t right = lineDriver.getRightPower();
      motor_9.run((9)==M1?-(left):(left));
      motor_10.run((10)==M1?-(right):(right));       
    }

    //Keep the loop repeat time constant
    //the value here must be determined experimentaly
    _delay(startTime,0.020);
}

void _delay(float startTime, float seconds){
    long endTime = startTime + seconds * 1000;
    while(millis() < endTime)_loop();
}

void _loop(){
    
}
