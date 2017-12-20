#include "MeLineFollowerArray.h"

MeLineFollowerArray::MeLineFollowerArray(){
}

uint8_t MeLineFollowerArray::getRawValue() {
  return raw & B00111111;
}

int8_t MeLineFollowerArray::getPosition() {
  return weighted;
}


boolean MeLineFollowerArray::isValidLine(uint8_t val) {

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


MeLineFollowerArray::direction MeLineFollowerArray::getDirection(){

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

unsigned long * MeLineFollowerArray::getHighTime(){
  return high_time;
}

bool MeLineFollowerArray::readSensor(){

  unsigned long time_out_start = 0;
  bool timeout = false;
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
  time_out_start = micros();
  while((digitalRead(DataPin) == 0) && ((micros() - time_out_start) < 6000));

  //Wait for 50 uSec high bit
  time_out_start = micros();
  while((digitalRead(DataPin) == 1)&&((micros() - time_out_start) < 6000));

  timeout = (micros() - time_out_start) > 6000 ;

  //Now read sensor data 3*bit MSB first
  uint8_t Sensor_Data[3];
  
  for(uint8_t k=0; k<3 && timeout==false; k++)
  {
      Sensor_Data[k] = 0;

      //Read 8 bits
      for(uint8_t i=0;i<8;i++)
      {

          //Low bit 50 uSec
          time_out_start = micros();
          while(digitalRead(DataPin) == 0 && ((micros() - time_out_start) < 1000));

          //High bit pulse width 27 uSec => 0, 70 uSec => 1        
          time_out_start = micros();
          while(digitalRead(DataPin) == 1 && ((micros() - time_out_start) < 1000));

          unsigned long HIGH_level_read_time = micros() - time_out_start;

          if (k==0)  {
            high_time[i] = HIGH_level_read_time; //(HIGH_level_read_time < 255) ? HIGH_level_read_time : 255;
          }
          
          if (HIGH_level_read_time > 1000) {
            timeout = true;
            break;
          }          
          
          //uint32_t HIGH_level_read_time = pulseIn(DataPin,HIGH,250);          

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
  //while(digitalRead(DataPin) == 0&&((millis() - time_out_start) < 6));
  //while(digitalRead(DataPin) == 1&&((millis() - time_out_start) < 6));

  if (timeout==false && Sensor_Data[1] == (uint8_t)(~(uint8_t)Sensor_Data[0])) {

    //Current sensor reading is a valid reading
    raw = Sensor_Data[0] & B00111111; // Mask lower 6 bits
    validReading = true; // set valid reading flag

    //count number of high bits
    int cnt = 0;
    for (uint8_t i=0;i < 6;i++) {
      if ( bitRead(raw,i) ) cnt++;
    }

    if ( cnt > 0 && isValidLine(raw) ) {
      //Calulate position on line by weighted average
      //Method: http://theultimatelinefollower.blogspot.nl/2015/12/interpolation.html
      //the sensors are spaced 6 mm this makes {-30,-18, -6, 6, 18, 30};
      weighted = (-30 * bitRead(raw,0)) + (-18 * bitRead(raw,1)) + (-6 * bitRead(raw,2)) + (6 * bitRead(raw,3)) + (18 * bitRead(raw,4)) + (30 * bitRead(raw,5));
      weighted = weighted / cnt;
    } else {
      //No bits set, we are not on a line
      weighted = MeLineFollowerArray::invalid;
    }
  }
  else {
    //Current sensor reading is not valid
    //now check the previous sensor reading

    if (validReading == true) {

      //Previous reading was valid
      //we can fallback to the previous reading
      //but we reset the valid reading flag to prevent continous reuse of this repaired data
      validReading = false;
      //raw and weighted are kept the same

    } else {

      //Previous reading was invalid
      //We have two invalid readings in a row
      raw = 64; //MeLineFollowerArray::invalid;
      weighted = MeLineFollowerArray::invalid;
    }
  }

  //return status
  return validReading;
}
