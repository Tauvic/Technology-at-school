#include "LineArray.h"
LineArray::LineArray(){
}

void LineArray::setPort(int pin) {
	DataPin = pin;
}

uint8_t LineArray::getRawValue() {
  return raw;
}

int8_t LineArray::getPosition() {
  return weighted;
}

char * LineArray::getBINValue() {
  return bin;
}

boolean LineArray::checkBIN(String val) {
	return (val.substring(1) == getBINValue());
}

boolean LineArray::isValidLine(uint8_t val) {
  
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


LineArray::direction LineArray::getDirection(){

      //Get sensor on/off reading
      //B111111 = 63 can go left and right
      //B111100 = 60 can go left
      //B001111 = 15 can go right

      switch (raw) {
         case 60: return can_left; 
         case 15: return can_right;
         case 63: return can_left_right;
         default: return can_forward;
      };
}

bool LineArray::readSensor(){

	long time_out_flag = 0;

  //Wakeup sensor
	pinMode(DataPin, OUTPUT);
 
  //Send 1 mSec low bit
	digitalWrite(DataPin, LOW);
	delayMicroseconds(980);

  //Send 50 uSec high bit 
	digitalWrite(DataPin, HIGH);
	delayMicroseconds(40);

  //Read sensor response
	pinMode(DataPin, INPUT_PULLUP);

  //Wait for 100 uSec low bit
	delayMicroseconds(50);
	time_out_flag = millis();
	while((digitalRead(DataPin) == 0)&&((millis() - time_out_flag) < 6));

  //Wait for 50 uSec high bit
	time_out_flag = millis();
	while((digitalRead(DataPin) == 1)&&((millis() - time_out_flag) < 6));

  //Now read sensor data 3*bit MSB first
  uint8_t Sensor_Data[3];
	for(uint8_t k=0; k<3; k++)
	{
			Sensor_Data[k] = 0x00;

      //Read 8 bits
			for(uint8_t i=0;i<8;i++)
			{
          //Low bit 50 uSec
					time_out_flag = millis();
					while(digitalRead(DataPin) == 0&&((millis() - time_out_flag) < 6));

          //High bit pulse width 27 uSec=0, 70 uSec=1
					uint32_t HIGH_level_read_time = micros();
					time_out_flag = millis();
					while(digitalRead(DataPin) == 1&&((millis() - time_out_flag) < 6));
         
					HIGH_level_read_time = micros() - HIGH_level_read_time;
				  if ( ! (HIGH_level_read_time > 50 && HIGH_level_read_time < 100) )
					{
							Sensor_Data[k] |= (0x80 >> i); // Set bit high
					}           
			}
	}

  //Init results
  raw = 0;
  weighted = 0;
  bool valid=false;
 
	if (Sensor_Data[1] == (uint8_t)(~(uint8_t)Sensor_Data[0]))
	{
    //The sensor reading was valid, now whe check the data
    raw = Sensor_Data[0] & B00111111; // Mask lower 6 bits

    //count number of high bits
    int cnt = 0;
    for (uint8_t i=0;i < 6;i++) {
      if ( bitRead(raw,i) ) {
        bin[i]='1'; cnt++;
      } else {
        bin[i]='0';       
      };
    };
    bin[6]='\0';

    if ( cnt > 0 && isValidLine(raw) ) {
      //Calulate position on line by weighted average
      //Method: http://theultimatelinefollower.blogspot.nl/2015/12/interpolation.html
      //float norm[8] = {-3.0,-1.8,-0.6,0.6,1.8,3.0};
      // -3000 -1800 -600
      weighted = (-30 * bitRead(raw,0)) + (-18 * bitRead(raw,1)) + (-6 * bitRead(raw,2)) + (6 * bitRead(raw,3)) + (18 * bitRead(raw,4)) + (30 * bitRead(raw,5)); // / (2.0 );
      weighted = weighted / cnt;
      valid = true;
    };


	}

  //return status 
  return valid;
}

