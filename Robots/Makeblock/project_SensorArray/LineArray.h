#ifndef LineArray_h
#define LineArray_h

#include <Arduino.h>

class LineArray
{

    
	public:
    //direction capabilities
    enum direction {can_forward,can_left,can_right,can_left_right};

		LineArray();
		void setPort(int pin);
    bool readSensor();
    int8_t getPosition();
    direction getDirection();
		uint8_t getRawValue();
		char * getBINValue();
		boolean checkBIN(String val);
 private:
		int DataPin=12;
		uint8_t raw=0;
    char bin[7];
    int8_t weighted=0;
    bool isValidLine(uint8_t raw);
		String fillZero(String bin);
};

#endif
