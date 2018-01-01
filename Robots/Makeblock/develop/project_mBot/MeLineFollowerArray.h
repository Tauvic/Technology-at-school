#ifndef MeLineFollowerArray_h
#define MeLineFollowerArray_h

/* Includes ------------------------------------------------------------------*/
#include "config.h"
#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>
#include "MeConfig.h"

#ifdef ME_PORT_DEFINED
#include "MePort.h"
#endif /* ME_PORT_DEFINED */

class MeLineFollowerArray : public MePort
{    
  public:
    //direction capabilities
    enum direction {can_nowhere,can_forward,can_left,can_right,can_left_right,can_left_right_forward};
    MeLineFollowerArray();
    bool readSensor();
    boolean isValid();
    int8_t getPosition();
    uint8_t getRawValue();
    uint8_t getDensity();
    direction getDirection();
    uint8_t *getDebugInfo();    
  private:
    uint8_t debug_info[8];
    uint8_t raw = 64;
    uint8_t density = 0;
    int8_t weighted = 64;
    bool validReading = false;
    bool isValidLine(uint8_t raw);
};

#endif
