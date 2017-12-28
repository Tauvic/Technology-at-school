#include "MeLineFollowerArray.h"

MeLineFollowerArray::MeLineFollowerArray(){
}

uint8_t MeLineFollowerArray::getRawValue() {
  return raw;
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

  //{can_nowhere,can_forward,can_left,can_right,can_left_right,can_left_right_forward}

      if ( !validReading ) return can_nowhere;

      switch (raw) {
         case 0: return can_nowhere;
         case B111111: return can_left_right;
         case B111110: return can_right;
         case B111100: return can_right;
         case B011111: return can_left;
         case B001111: return can_left;
         default: 
            return can_forward;
      };
}

uint8_t * MeLineFollowerArray::getDebugInfo(){
  return debug_info;
}

boolean MeLineFollowerArray::isValid(){
  return validReading;
}

bool MeLineFollowerArray::readSensor(){

  unsigned long time_out_start = 0;
  bool timeout = false;
  uint8_t DataPin = pin2(); // 12

  //Wakeup sensor
  pinMode(DataPin, OUTPUT);

  //Send 1 mSec low bit
  digitalWrite(DataPin, LOW);
  delayMicroseconds(980);

  // ===================================== ||
  // Begin of time critical section        ||
  // Disable interrupts to garantee timing ||
  //noInterrupts();
  //note: this doesn't work we should use an interrupt procedure

  //Send 50 uSec high bit
  digitalWrite(DataPin, HIGH);
  delayMicroseconds(40); //40

  //Read sensor response
  pinMode(DataPin, INPUT_PULLUP);

  //Wait for 100 uSec low - high transition
  if ( pulseIn(DataPin,LOW,150) == 0 ) return false;
  
  //Now read sensor data 3*bit MSB first
  uint8_t Sensor_Data[3];
  
  for(uint8_t k=0; k<3; k++)
  {
      Sensor_Data[k] = 0;

      //Read 8 bits
      for(uint8_t i=0;i<8;i++)
      {          
          uint32_t high_level = pulseIn(DataPin,HIGH,350);
          uint8_t high = (high_level >0 && high_level < 255) ? high_level : 255;
          
          //if (high == 255) timeout = true;                    

          //Line detected when value between 50 and 100
          if ( ! (high > 50 && high < 100) )
          {
              Sensor_Data[k] |= (0x80 >> i); // Set bit high
          }

          if (k==0) debug_info[i] = high;
      }
  }

  //interrupts();
  // Enable interrupt processing   ||
  // End of time critical section  ||
  // ============================= ||
    
  //We only use lower 6 bits
  Sensor_Data[0] = Sensor_Data[0] & B00111111;
  Sensor_Data[1] = Sensor_Data[1] | B11000000;
  boolean valid_checksum = (Sensor_Data[1] == (uint8_t)(~(uint8_t)Sensor_Data[0]));

  //debug info  
  debug_info[0] = timeout ? 1 : 0;
  debug_info[1] = valid_checksum ? 1 : 0;


  if (timeout==false && valid_checksum==true) {

    //Current sensor reading is a valid reading
    validReading = true; // set valid reading flag
    raw = Sensor_Data[0];

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
      validReading = false;
    }
  } else {
      validReading = false; // set invalid reading flag
  }

  if (!validReading) {
      weighted = 64;
      raw = 64;
  }

#if defined (LOG_SENSOR) && LOG_LEVEL >= LOG_INFO 
  Serial.print("@S,");
  Serial.print(millis());
  Serial.print(",v");
  Serial.print(raw,DEC);
  Serial.print(",b");
  for (int i=0;i<6;i++) {
    if (bitRead(raw,i)) 
      Serial.print('1');
    else
      Serial.print('0');
  }  
  Serial.print(",d");
  Serial.print(getDirection());  
#if LOG_LEVEL >= LOG_DEBUG
  Serial.print(",t");  
  Serial.print(debug_info[0],DEC);  
  Serial.print(",s");  
  Serial.print(debug_info[1],DEC);  
  for (int i=2;i<8;i++) {
      Serial.print(',');
      Serial.print(debug_info[i],DEC);
  }
#endif
  Serial.print('\n');
#endif   

  return validReading;

}
