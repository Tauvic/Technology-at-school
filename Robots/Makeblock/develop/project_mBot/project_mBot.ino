/*************************************************************************
* File Name          : mbot_firmware.ino
* Author             : Ander, Mark Yan
* Updated            : Ander, Mark Yan
* Version            : V06.01.107
* Date               : 01/03/2017
* Description        : Firmware for Makeblock Electronic modules with Scratch.  
* License            : CC-BY-SA 3.0
* Copyright (C) 2013 - 2016 Maker Works Technology Co., Ltd. All right reserved.
* http://www.makeblock.cc/
**************************************************************************/
#include <Wire.h>
#include <MeMCore.h>

//#define TEST_MODE 1

//Scheduler provided by https://github.com/arkhipenko/TaskScheduler
#include "TaskScheduler.h"

// Callback methods prototypes
void t1Callback();
void t2Callback();
void t3Callback();

//Tasks

//t1=Slow sensors
Task t1(20, TASK_FOREVER, &t1Callback);
//t2=Communication
Task t2(TASK_IMMEDIATE, TASK_FOREVER, &t2Callback);
//t3=Actuators
Task t3(20, TASK_FOREVER, &t3Callback);


#define VERSION                0
#define ULTRASONIC_SENSOR      1
//#define TEMPERATURE_SENSOR     2
#define LIGHT_SENSOR           3
#define POTENTIONMETER         4
#define JOYSTICK               5
//#define GYRO                   6
#define SOUND_SENSOR           7
#define RGBLED                 8
//#define SEVSEG                 9
#define MOTOR                  10
//#define SERVO                  11
#define ENCODER                12
#define IR                     13
#define IRREMOTE               14
//#define PIRMOTION              15
#define INFRARED               16
#define LINEFOLLOWER           17
#define IRREMOTECODE           18
#define SHUTTER                20
//#define LIMITSWITCH            21
#define BUTTON                 22
//#define HUMITURE               23
//#define FLAMESENSOR            24
//#define GASSENSOR              25
//#define COMPASS                26
#define TEMPERATURE_SENSOR_1   27
#define ROBOT_STATUS           28
#define DIGITAL                30
#define ANALOG                 31
#define PWM                    32
#define SERVO_PIN              33
#define TONE                   34
#define BUTTON_INNER           35
#define ULTRASONIC_ARDUINO     36
#define PULSEIN                37
//MeLineFollowerArray
#define LINEFOLLOW_ARRAY       38
#define LINEFOLLOW_DRIVER      39

//#define STEPPER                40
//#define LEDMATRIX              41
#define TIMER                  50
//#define TOUCH_SENSOR           51
#define JOYSTICK_MOVE          52
#define COMMON_COMMONCMD       60
  //Secondary command
  #define SET_STARTER_MODE     0x10
  #define SET_AURIGA_MODE      0x11
  #define SET_MEGAPI_MODE      0x12
  #define GET_BATTERY_POWER    0x70
  #define GET_AURIGA_MODE      0x71
  #define GET_MEGAPI_MODE      0x72
#define ENCODER_BOARD          61
  //Read type
  #define ENCODER_BOARD_POS    0x01
  #define ENCODER_BOARD_SPEED  0x02

#define ENCODER_PID_MOTION     62
  //Secondary command
  #define ENCODER_BOARD_POS_MOTION         0x01
  #define ENCODER_BOARD_SPEED_MOTION       0x02
  #define ENCODER_BOARD_PWM_MOTION         0x03
  #define ENCODER_BOARD_SET_CUR_POS_ZERO   0x04
  #define ENCODER_BOARD_CAR_POS_MOTION     0x05
  
#define PM25SENSOR     63
  //Secondary command
  #define GET_PM1_0         0x01
  #define GET_PM2_5         0x02
  #define GET_PM10          0x03

#ifdef SERVO
Servo servos[8];  
#endif

MeDCMotor dc;

#ifdef LINEFOLLOW_ARRAY
#include "MeLineFollowerArray.h"
MeLineFollowerArray lineFollowerArray;
#endif

#ifdef LINEFOLLOW_DRIVER
#include "LineDriver.h"
LineDriver lineDriver(lineFollowerArray);
#endif

#ifdef TEMPERATURE_SENSOR
MeTemperature ts;
#endif

#ifdef RGBLED
MeRGBLed led(0,30);
#endif

MeUltrasonicSensor us;

#ifdef SEVSEG
Me7SegmentDisplay seg;
#endif
MePort generalDevice;

#ifdef LEDMATRIX
MeLEDMatrix ledMx;
#endif

MeBuzzer buzzer;

MeIR ir;

#ifdef GYRO
MeGyro gyro;
#endif

#ifdef JOYSTICK
MeJoystick joystick;
#endif

#ifdef COMPASS
MeCompass Compass;
#endif

#ifdef HUMITURE
MeHumiture humiture;
#endif

#ifdef FLAMESENSOR
MeFlameSensor FlameSensor;
#endif

#ifdef GASSENSOR
MeGasSensor GasSensor;
#endif

#ifdef TOUCH_SENSOR
MeTouchSensor touchSensor;
#endif

Me4Button buttonSensor;

typedef struct MeModule
{
    int device;
    int port;
    int slot;
    int pin;
    int index;
    float values[3];
} MeModule;

union{
    byte byteVal[4];
    float floatVal;
    long longVal;
}val;

union{
  byte byteVal[8];
  double doubleVal;
}valDouble;

union{
  byte byteVal[2];
  short shortVal;
}valShort;

#if defined(__AVR_ATmega32U4__) 
const int analogs[12] PROGMEM = {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11};
#else
const int analogs[8] PROGMEM = {A0,A1,A2,A3,A4,A5,A6,A7};
#endif
String mVersion = "06.01.107";
boolean isAvailable = false;

int len = 52;
char buffer[52];
byte index = 0;
byte dataLen;
byte modulesLen=0;
boolean isStart = false;
char serialRead;
uint8_t command_index = 0;
int irDelay = 0;
int irIndex = 0;
char irRead = 0;
boolean irReady = false;
String irBuffer = "";
double lastTime = 0.0;
double currentTime = 0.0;
double lastIRTime = 0.0;

#define GET 1
#define RUN 2
#define RESET 4
#define START 5
float angleServo = 90.0;
int servo_pins[8]={0,0,0,0,0,0,0,0};
unsigned char prevc=0;
boolean buttonPressed = false;
uint8_t keyPressed = KEY_NULL;


/*
 * function list
 */
 void readButtonInner(uint8_t pin, int8_t s);
 void buzzerOn();
 void buzzerOff();
 unsigned char readBuffer(int index);
 void writeBuffer(int index,unsigned char c);
 void writeHead();
 void writeEnd();
 void writeSerial(unsigned char c);
 void readSerial();
 void parseData();
 void callOK();
 void sendByte(char c);
 void sendString(String s);
 void sendFloat(float value);
 void sendShort(double value);
 void sendDouble(double value);
 short readShort(int idx);
 float readFloat(int idx);
 char* readString(int idx,int len);
 uint8_t* readUint8(int idx,int len);
 void runModule(int device);
 int searchServoPin(int pin);
 void readSensor(int device);

#include "Robot.h"
Robot robot;
 
void readButtonInner(uint8_t pin, int8_t s)
{
  pin = pgm_read_byte(&analogs[pin]);
  pinMode(pin,INPUT);
  boolean currentPressed = !(analogRead(pin)>10);
      
  if(buttonPressed == currentPressed){
    return;
  }
  buttonPressed = currentPressed;
  writeHead();
  writeSerial(0x80);
  sendByte(currentPressed);
  writeEnd();
}

void buzzerOn(){
  buzzer.tone(500,1000); 
}
void buzzerOff(){
  buzzer.noTone(); 
}
unsigned char readBuffer(int index){
 return buffer[index]; 
}
void writeBuffer(int index,unsigned char c){
  buffer[index]=c;
}
void writeHead(){
  writeSerial(0xff);
  writeSerial(0x55);
}
void writeEnd(){
 Serial.println();
 t1.enable();
}
void writeSerial(unsigned char c){
 Serial.write(c);
}
void readSerial(){
  isAvailable = false;
  if(Serial.available()>0){
    isAvailable = true;
    serialRead = Serial.read();
  }
}
/*
ff 55 len idx action device port slot data a
0  1  2   3   4      5      6    7    8
*/
void parseData(){
  isStart = false;
  int idx = readBuffer(3);
  command_index = (uint8_t)idx;
  int action = readBuffer(4);
  int device = readBuffer(5);
  switch(action){
    case GET:{
        if(device != ULTRASONIC_SENSOR){
          writeHead();
          writeSerial(idx);
        }
        readSensor(device);
        writeEnd();
     }
     break;
     case RUN:{
       runModule(device);
       callOK();
     }
      break;
      case RESET:{
        //reset
        dc.reset(M1);
        dc.run(0);
        dc.reset(M2);
        dc.run(0);
        #ifdef LINEFOLLOW_DRIVER
        lineDriver.doNothing();
        #endif        
        buzzerOff();
        callOK();
      }
     break;
     case START:{
        //start
        callOK();
      }
     break;
  }
}
void callOK(){
    writeSerial(0xff);
    writeSerial(0x55);
    writeEnd();
}
void sendByte(char c){
  writeSerial(1);
  writeSerial(c);
}
void sendString(String s){
  int l = s.length();
  writeSerial(4);
  writeSerial(l);
  for(int i=0;i<l;i++){
    writeSerial(s.charAt(i));
  }
}
//1 byte 2 float 3 short 4 len+string 5 double
void sendFloat(float value){ 
     writeSerial(2);
     val.floatVal = value;
     writeSerial(val.byteVal[0]);
     writeSerial(val.byteVal[1]);
     writeSerial(val.byteVal[2]);
     writeSerial(val.byteVal[3]);
}
void sendShort(double value){
     writeSerial(3);
     valShort.shortVal = value;
     writeSerial(valShort.byteVal[0]);
     writeSerial(valShort.byteVal[1]);
     writeSerial(valShort.byteVal[2]);
     writeSerial(valShort.byteVal[3]);
}
void sendDouble(double value){
     writeSerial(5);
     valDouble.doubleVal = value;
     writeSerial(valDouble.byteVal[0]);
     writeSerial(valDouble.byteVal[1]);
     writeSerial(valDouble.byteVal[2]);
     writeSerial(valDouble.byteVal[3]);
     writeSerial(valDouble.byteVal[4]);
     writeSerial(valDouble.byteVal[5]);
     writeSerial(valDouble.byteVal[6]);
     writeSerial(valDouble.byteVal[7]);
}
short readShort(int idx){
  valShort.byteVal[0] = readBuffer(idx);
  valShort.byteVal[1] = readBuffer(idx+1);
  return valShort.shortVal; 
}
float readFloat(int idx){
  val.byteVal[0] = readBuffer(idx);
  val.byteVal[1] = readBuffer(idx+1);
  val.byteVal[2] = readBuffer(idx+2);
  val.byteVal[3] = readBuffer(idx+3);
  return val.floatVal;
}
char _receiveStr[20] = {};
uint8_t _receiveUint8[16] = {};
char* readString(int idx,int len){
  for(int i=0;i<len;i++){
    _receiveStr[i]=readBuffer(idx+i);
  }
  _receiveStr[len] = '\0';
  return _receiveStr;
}
uint8_t* readUint8(int idx,int len){
  for(int i=0;i<len;i++){
    if(i>15){
      break;
    }
    _receiveUint8[i] = readBuffer(idx+i);
  }
  return _receiveUint8;
}

void runModule(int device){
  //0xff 0x55 0x6 0x0 0x2 0x22 0x9 0x0 0x0 0xa 
  int port = readBuffer(6);
  int pin = port;
  switch(device){
   case MOTOR:{
     int speed = readShort(7);
     if(dc.getPort()!=port){
       dc.reset(port);
     }
     dc.run(speed);
     #ifdef LINEFOLLOW_DRIVER
     lineDriver.doNothing();
     #endif
   } 
    break;
    case JOYSTICK:{
     int leftSpeed = readShort(6);
     dc.reset(M1);
     dc.run(leftSpeed);
     int rightSpeed = readShort(8);
     dc.reset(M2);
     dc.run(rightSpeed);
     #ifdef LINEFOLLOW_DRIVER
     lineDriver.doNothing();
     #endif     
    }
    break;
   #ifdef RGBLED
   case RGBLED:{
     int slot = readBuffer(7);
     int idx = readBuffer(8);
     int r = readBuffer(9);
     int g = readBuffer(10);
     int b = readBuffer(11);
     if((led.getPort() != port) || led.getSlot() != slot)
     {
       led.reset(port,slot);
     }
     if(idx>0)
     {
       led.setColorAt(idx-1,r,g,b); 
     }
     else
     {
       led.setColor(r,g,b); 
     }
     led.show();
   }
   break;
   #endif
   #ifdef SERVO
   case SERVO:{
     int slot = readBuffer(7);
     pin = slot==1?mePort[port].s1:mePort[port].s2;
     int v = readBuffer(8);
     Servo sv = servos[searchServoPin(pin)];
     if(v >= 0 && v <= 180)
     {
       if(!sv.attached())
       {
         sv.attach(pin);
       }
       sv.write(v);
     }
   }
   break;
   #endif
   #ifdef SEVSEG
   case SEVSEG:{
     if(seg.getPort()!=port){
       seg.reset(port);
     }
     float v = readFloat(7);
     seg.display(v);
   }
   break;
   #endif
   #ifdef LEDMATRIX
   case LEDMATRIX:{
     if(ledMx.getPort()!=port){
       ledMx.reset(port);
     }
     int action = readBuffer(7);
     if(action==1){
            int px = buffer[8];
            int py = buffer[9];
            int len = readBuffer(10);
            char *s = readString(11,len);
            ledMx.drawStr(px,py,s);
      }else if(action==2){
            int px = readBuffer(8);
            int py = readBuffer(9);
            uint8_t *ss = readUint8(10,16);
            ledMx.drawBitmap(px,py,16,ss);
      }else if(action==3){
            int point = readBuffer(8);
            int hours = readBuffer(9);
            int minutes = readBuffer(10);
            ledMx.showClock(hours,minutes,point);
     }else if(action == 4){
            ledMx.showNum(readFloat(8),3);
     }
   }
   break;
   #endif
   case LIGHT_SENSOR:{
     if(generalDevice.getPort()!=port){
       generalDevice.reset(port);
     }
     int v = readBuffer(7);
     generalDevice.dWrite1(v);
   }
   break;
   case IR:{
     String Str_data;
     int len = readBuffer(2)-3;
     for(int i=0;i<len;i++){
       Str_data+=(char)readBuffer(6+i);
     }
     ir.sendString(Str_data);
     Str_data = "";
   }
   break;
   case SHUTTER:{
     if(generalDevice.getPort()!=port){
       generalDevice.reset(port);
     }
     int v = readBuffer(7);
     if(v<2){
       generalDevice.dWrite1(v);
     }else{
       generalDevice.dWrite2(v-2);
     }
   }
   break;
   case DIGITAL:{
     pinMode(pin,OUTPUT);
     int v = readBuffer(7);
     digitalWrite(pin,v);
   }
   break;
   case PWM:{
     pinMode(pin,OUTPUT);
     int v = readBuffer(7);
     analogWrite(pin,v);
   }
   break;
   case TONE:{
     int hz = readShort(6);
     int tone_time = readShort(8);
     if(hz>0){
       buzzer.tone(hz,tone_time);
     }else{
       buzzer.noTone(); 
     }
   }
   break;
   #ifdef SERVO
   case SERVO_PIN:{
     int v = readBuffer(7);
     Servo sv = servos[searchServoPin(pin)]; 
     if(v >= 0 && v <= 180)
     {
       if(!sv.attached())
       {
         sv.attach(pin);
       }
       sv.write(v);
     }
   }
   break;
   #endif
   case TIMER:{
    lastTime = millis()/1000.0; 
   }
   break;
   #ifdef LINEFOLLOW_DRIVER
   case LINEFOLLOW_DRIVER:
   {
    lineDriver.setParams(readShort(6),readFloat(8));
    lineDriver.doForward();
   }
   #endif
  }
}

int searchServoPin(int pin){
    for(int i=0;i<8;i++){
      if(servo_pins[i] == pin){
        return i;
      }
      if(servo_pins[i]==0){
        servo_pins[i] = pin;
        return i;
      }
    }
    return 0;
}
void readSensor(int device){
  /**************************************************
      ff    55      len idx action device port slot data a
      0     1       2   3   4      5      6    7    8
      0xff  0x55   0x4 0x3 0x1    0x1    0x1  0xa 
  ***************************************************/
  float value=0.0;
  int port,slot,pin;
  port = readBuffer(6);
  pin = port;
  switch(device){
   case  ULTRASONIC_SENSOR:{
     if(us.getPort()!=port){
       us.reset(port);
     }
     value = (float)us.distanceCm();
     writeHead();
     writeSerial(command_index);
     sendFloat(value);
   }
   break;
   #ifdef TEMPERATURE_SENSOR
   case  TEMPERATURE_SENSOR:{
     slot = readBuffer(7);
     if(ts.getPort()!=port||ts.getSlot()!=slot){
       ts.reset(port,slot);
     }
     value = ts.temperature();
     sendFloat(value);
   }
   break;
   #endif
   case  LIGHT_SENSOR:
   case  SOUND_SENSOR:
   case  POTENTIONMETER:{
     if(generalDevice.getPort()!=port){
       generalDevice.reset(port);
       pinMode(generalDevice.pin2(),INPUT);
     }
     value = generalDevice.aRead2();
     sendFloat(value);
   }
   break;
   case  JOYSTICK:{
     slot = readBuffer(7);
     if(joystick.getPort() != port){
       joystick.reset(port);
     }
     value = joystick.read(slot);
     sendFloat(value);
   }
   break;
   case  IR:{
      if(irReady){
         sendString(irBuffer);
         irReady = false;
         irBuffer = "";
      }
   }
   break;
   case IRREMOTE:{
     unsigned char r = readBuffer(7);
     if(millis()/1000.0-lastIRTime>0.2){
       sendByte(0);
     }else{
       sendByte(irRead==r);
     }
     //irRead = 0;
     irIndex = 0;
   }
   break;
   case IRREMOTECODE:{
     if(irRead<0xff){
       sendByte(irRead);
     }
     irRead = 0;
     irIndex = 0;
   }
   break;
   #ifdef PIRMOTION
   case PIRMOTION:{
     if(generalDevice.getPort()!=port){
       generalDevice.reset(port);
       pinMode(generalDevice.pin2(),INPUT);
     }
     value = generalDevice.dRead2();
     sendFloat(value);
   }
   break;
   #endif   
   case  LINEFOLLOWER:{
     if(generalDevice.getPort()!=port){
       generalDevice.reset(port);
         pinMode(generalDevice.pin1(),INPUT);
         pinMode(generalDevice.pin2(),INPUT);
     }
     value = generalDevice.dRead1()*2+generalDevice.dRead2();
     sendFloat(value);
   }
   break;
   #ifdef LIMITSWITCH
   case LIMITSWITCH:{
     slot = readBuffer(7);
     if(generalDevice.getPort()!=port||generalDevice.getSlot()!=slot){
       generalDevice.reset(port,slot);
     }
     if(slot==1){
       pinMode(generalDevice.pin1(),INPUT_PULLUP);
       value = !generalDevice.dRead1();
     }else{
       pinMode(generalDevice.pin2(),INPUT_PULLUP);
       value = !generalDevice.dRead2();
     }
     sendFloat(value);  
   }
   break;
   #endif
   case BUTTON_INNER:{
     //pin = analogs[pin];
     pin = pgm_read_byte(&analogs[pin]);
     char s = readBuffer(7);
     pinMode(pin,INPUT);
     boolean currentPressed = !(analogRead(pin)>10);
     sendByte(s^(currentPressed?1:0));
     buttonPressed = currentPressed;
   }
   break;
   #ifdef COMPASS
   case COMPASS:{
     if(Compass.getPort()!=port){
       Compass.reset(port);
       Compass.setpin(Compass.pin1(),Compass.pin2());
     }
     sendFloat(Compass.getAngle());
   }
   break;
   #endif
   #ifdef HUMITURE
   case HUMITURE:{
     uint8_t index = readBuffer(7);
     if(humiture.getPort()!=port){
       humiture.reset(port);
     }
     uint8_t HumitureData;
     humiture.update();
     HumitureData = humiture.getValue(index);
     sendByte(HumitureData);
   }
   break;
   #endif
   #ifdef FLAMESENSOR 
   case FLAMESENSOR:{
     if(FlameSensor.getPort()!=port){
       FlameSensor.reset(port);
       FlameSensor.setpin(FlameSensor.pin2(),FlameSensor.pin1());
     }
     int16_t FlameData; 
     FlameData = FlameSensor.readAnalog();
     sendShort(FlameData);
   }
   break;
   #endif
   #ifdef GASSENSOR
   case GASSENSOR:{
     if(GasSensor.getPort()!=port){
       GasSensor.reset(port);
       GasSensor.setpin(GasSensor.pin2(),GasSensor.pin1());
     }
     int16_t GasData; 
     GasData = GasSensor.readAnalog();
     sendShort(GasData);
   }
   break;
   #endif
   #ifdef GYRO
   case  GYRO:{
       int axis = readBuffer(7);
       gyro.update();
       value = gyro.getAngle(axis);
       sendFloat(value);
   }
   break;
   #endif
   case  VERSION:{
     sendString(mVersion);
   }
   break;
   case  DIGITAL:{
     pinMode(pin,INPUT);
     sendFloat(digitalRead(pin));
   }
   break;
   case  ANALOG:{
     //pin = analogs[pin];
     pin = pgm_read_byte(&analogs[pin]);
     pinMode(pin,INPUT);
     sendFloat(analogRead(pin));
   }
   break;
   case TIMER:{
     sendFloat(currentTime);
   }
   break;
   #ifdef TOUCH_SENSOR
   case TOUCH_SENSOR:
   {
     if(touchSensor.getPort() != port){
       touchSensor.reset(port);
     }
     sendByte(touchSensor.touched());
   }
   break;
   #endif
   case BUTTON:
   {
     if(buttonSensor.getPort() != port){
       buttonSensor.reset(port);
     }
     sendByte(keyPressed == readBuffer(7));
   }
   break;
   #ifdef ROBOT_STATUS
   case ROBOT_STATUS:
   {
    //Return statistics such as loop time, motor usage etc.
    //sendShort(robot.getLoopTime());
    sendShort(robot.getFreeMem());
   }
   break;
   #endif
   #ifdef LINEFOLLOW_ARRAY
   case LINEFOLLOW_ARRAY:
   {
     if(lineFollowerArray.getPort()!=port) {
       lineFollowerArray.reset(port);
     };


     //lineFollowerArray.readSensor();
     delayMicroseconds(250);
     for (int i = 0; !lineFollowerArray.readSensor() && i< 10; i++) {
      delayMicroseconds(i*250);
     }
     
     //get mode: 1=position, 2=raw, 3=hightime
     switch (readBuffer(7)) {
      case 1: //Position
        sendByte(lineFollowerArray.getPosition()+30);
        break;
      case 2: //Bits
        sendByte(lineFollowerArray.getRawValue());
        break;
      case 3: //Raw
        sendByte(lineFollowerArray.getRawValue());
        break;
      case 4: //Raw + debug info
        writeSerial(4); //Send string
        writeSerial(9); //Send 7 byte string
        //unsigned char buf[7] = {0,1,2,3,4,5,6};
        writeSerial(lineFollowerArray.getRawValue());
        //writeSerial(32);
        uint8_t *db = lineFollowerArray.getDebugInfo();
        writeSerial(db[0]);
        writeSerial(db[1]);
        writeSerial(db[2]);
        writeSerial(db[3]);
        writeSerial(db[4]);
        writeSerial(db[5]);
        writeSerial(db[6]);
        writeSerial(db[7]);

        break;
     }

   } 
  
   #endif
  }
}




void t1Callback() {
  //Run slow sensors

  //Skip if there are bytes to read
  if(Serial.available()>0) return;

  //Check if we are initialised
  if (lineFollowerArray.getPort()==0) return;

  delay(1);

  lineFollowerArray.readSensor();

  #ifdef TEST_MODE  

  unsigned long startTimer = micros();

  uint8_t raw = lineFollowerArray.getRawValue();
  Serial.print("X");
  Serial.print(raw,DEC);
  Serial.print(',');
  for (int i=0;i<6;i++) {
    if (bitRead(raw,i)) 
      Serial.print('1');
    else
      Serial.print('0');
  }
  uint8_t *db =lineFollowerArray.getDebugInfo();
  Serial.print(",T");  
  Serial.print(db[0],DEC);  
  Serial.print(",S");  
  Serial.print(db[1],DEC);  
  Serial.print(',');
  for (int i=2;i<8;i++) {
      Serial.print(db[i],DEC);
      Serial.print(',');
  }  
  Serial.print("t=");
  Serial.print(micros()-startTimer);
  Serial.print('\n');
  
  Serial.flush();

  if (raw != 0 || (db[0]==0 && db[1]==0) ) t1.disable();

  #endif
  
}

//void t2Callback() {}

void t2Callback() {
  //Communication

  //Check on board button is pressed
  readButtonInner(7,0);
  keyPressed = buttonSensor.pressed();

  //Time sensor
  currentTime = millis()/1000.0-lastTime;


  //IR receiver
  if(ir.decode())
  {
    irRead = ((ir.value>>8)>>8)&0xff;
    lastIRTime = millis()/1000.0;
    if(irRead==0xa||irRead==0xd){
      irIndex = 0;
      irReady = true;
    }else{
      irBuffer+=irRead; 
      irIndex++;
      if(irIndex>64){
        irIndex = 0;
        irBuffer = "";
      }
    }
    irDelay = 0;
  }else{
    irDelay++;
    if(irRead>0){
     if(irDelay>5000){
      irRead = 0;
      irDelay = 0;
     }
   }
  }

  //Serial commands
  readSerial();
  if(isAvailable){
    unsigned char c = serialRead&0xff;
    if(c==0x55&&isStart==false){
     if(prevc==0xff){
      index=1;
      isStart = true;
      t1.disable(); // Disable sensor reading
     }
    }else{
      prevc = c;
      if(isStart){
        if(index==2){
         dataLen = c; 
        }else if(index>2){
          dataLen--;
        }
        writeBuffer(index,c);
      }
    }
     index++;
     if(index>51){
      index=0; 
      isStart=false;
     }
     if(isStart&&dataLen==0&&index>3){ 
        isStart = false;
        parseData(); 
        index=0;
     }
  }
  
}

void t3Callback() {
  //Actuators

  LineDriver::action action = lineDriver.drive();

  switch (action) {

    case LineDriver::do_nothing:
       return;
           
    case LineDriver::do_forward:
       led.setColor(0,255,0); //Color green 
       break;
       
    case LineDriver::do_stop:
       led.setColor(255,0,0); //Color red
       break;        
  }

  led.show();
  
  //dc.reset(M1);
  //dc.run(lineDriver.getLeftPower());
  //dc.reset(M2);
  //dc.run(lineDriver.getRightPower());       
  
}

Scheduler runner;

void setup(){


  //Flash on board led
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);  
  delay(300);
  digitalWrite(13,LOW);
  
  //Enable serial data transmission
  Serial.begin(115200);
  delay(500);

  //Short buzz
  buzzer.tone(500,50); 
  delay(50);
  buzzerOff();

  //Enable IR receiver
  //ir.begin();

  //Switch leds off
  led.setpin(13);
  led.setColor(0,0,0);
  led.show();
  
  #ifdef GYRO
  gyro.begin();
  #endif
  
  //Serial.print("Version: ");
  //Serial.println(mVersion);
  
  #ifdef LEDMATRIX
  ledMx.setBrightness(6);
  ledMx.setColorIndex(1);
  #endif


  //Start task scheduler
  runner.init();

  runner.addTask(t1);
  runner.addTask(t2);
  runner.addTask(t3);

  //t1 = enabled on demand
  //t1.enable();
  #ifndef TEST_MODE
  t2.enable();
  #endif
  //t3.enable();

  lineFollowerArray.reset(1);
  //lineFollowerArray.readSensor(); // first reading fails
  //delay(100);
}

void loop(){
  //first statement in loop
  robot.loopStart();

  //Execute scheduler
  runner.execute();
  
  //last statement before loop end
  robot.loopEnd();
}
