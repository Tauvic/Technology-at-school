#include "MeLineFollowArray.h"

MeLineFollowArray::MeLineFollowArray(){
}

uint8_t MeLineFollowArray::getRawValue() {
  return raw;
}

int8_t MeLineFollowArray::getPosition() {
  return weighted;
}


boolean MeLineFollowArray::isValidLine(uint8_t val) {
  
  if (val == B00111111) return true; 
  if (val == B00111110) return true;
  if (val == B00011111) return true;
  if (val == B00111100) return true;
  if (val == B00011110) return true;
  if (val == B00001111) return true;
  if (val == B00111000) return true;
  if (val == B00011100) return true;
  if (val == B00001110) return true;
  if (val == B00000111) return true;
  if (val == B00110000) return true;
  if (val == B00011000) return true;
  if (val == B00001100) return true;
  if (val == B00000110) return true;
  if (val == B00000011) return true;
  if (val == B00100000) return true;
  if (val == B00010000) return true;
  if (val == B00001000) return true;
  if (val == B00000100) return true;
  if (val == B00000010) return true;
  if (val == B00000001) return true;
  if (val == B00000000) return true;
  return false;
}


MeLineFollowArray::direction MeLineFollowArray::getDirection(){

      //Get sensor on/off reading
      //B111111 = 63 can go left and right
      //B111100 = 60 can go right
      //B001111 = 15 can go left

      switch (raw) {
         case 60: return can_right; 
         case 15: return can_left;
         case 63: return can_left_right;
         default: return can_forward;
      };
}

bool MeLineFollowArray::readSensor(){

  long time_out_flag = 0;
  uint8_t DataPin = pin2();
  
  //Wakeup sensor
  pinMode(DataPin, OUTPUT);
 
  //Send 1 mSec low bit
  digitalWrite(DataPin, LOW);
  delayMicroseconds(980);

  // ===================================== ||
  // Begin of time critical section        ||
  // Disable interrupts to garantee timing ||
  //noInterrupts();
  
  //Send 50 uSec high bit 
  digitalWrite(DataPin, HIGH);
  delayMicroseconds(40); //40

  //Read sensor response
  pinMode(DataPin, INPUT_PULLUP);

  //Wait for 100 uSec low bit
  delayMicroseconds(50);
  time_out_flag = millis();
  while((digitalRead(DataPin) == 0)&&((millis() - time_out_flag) < 1));

  //Wait for 50 uSec high bit
  time_out_flag = millis();
  while((digitalRead(DataPin) == 1)&&((millis() - time_out_flag) < 1));

  //Now read sensor data 3*bit MSB first
  uint8_t Sensor_Data[3];
  for(uint8_t k=0; k<3; k++)
  {
      Sensor_Data[k] = 0x00;

      //Read 8 bits
      for(uint8_t i=0;i<8;i++)
      {

          /*
          //Low bit 50 uSec
          time_out_flag = millis();
          while(digitalRead(DataPin) == 0&&((millis() - time_out_flag) < 6));

          //High bit pulse width 27 uSec=0, 70 uSec=1
          uint32_t HIGH_level_read_time = micros();
          time_out_flag = millis();
          while(digitalRead(DataPin) == 1&&((millis() - time_out_flag) < 6));

          HIGH_level_read_time = micros() - HIGH_level_read_time;
          */
          uint32_t HIGH_level_read_time = pulseIn(DataPin,HIGH,250);
         
          if ( ! (HIGH_level_read_time > 50 && HIGH_level_read_time < 100) )
          {
              Sensor_Data[k] |= (0x80 >> i); // Set bit high
          }           
      }
  }

  //interrupts();
  // Enable interrupt processing   ||
  // End of time critical section  ||
  // ============================= ||  

  //Final low - high
  //while(digitalRead(DataPin) == 0&&((millis() - time_out_flag) < 6));
  //while(digitalRead(DataPin) == 1&&((millis() - time_out_flag) < 6));

  //Init results
  //raw = 0;
  //weighted = 0;
  bool valid=false;
 
  if (Sensor_Data[1] == (uint8_t)(~(uint8_t)Sensor_Data[0]))
  {
    //The sensor reading was valid, now whe check the data
    raw = Sensor_Data[0] & B00111111; // Mask lower 6 bits

    //count number of high bits
    int cnt = 0;
    for (uint8_t i=0;i < 6;i++) {
      if ( bitRead(raw,i) ) cnt++;
    }

    if ( cnt > 0 && isValidLine(raw) ) {
      //Calulate position on line by weighted average
      //Method: http://theultimatelinefollower.blogspot.nl/2015/12/interpolation.html
      //float norm[8] = {-3.0,-1.8,-0.6,0.6,1.8,3.0};
      // -3000 -1800 -600
      weighted = (-30 * bitRead(raw,0)) + (-18 * bitRead(raw,1)) + (-6 * bitRead(raw,2)) + (6 * bitRead(raw,3)) + (18 * bitRead(raw,4)) + (30 * bitRead(raw,5));
      weighted = weighted / cnt;
      valid = true;
    };

  }

  //return status 
  return valid;
}
