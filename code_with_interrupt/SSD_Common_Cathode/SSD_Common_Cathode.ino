// These define's must be placed at the beginning before #include "TimerInterrupt.h"
// _TIMERINTERRUPT_LOGLEVEL_ from 0 to 4
// Don't define _TIMERINTERRUPT_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

#if ( defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)  || \
        defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_MINI) ||    defined(ARDUINO_AVR_ETHERNET) || \
        defined(ARDUINO_AVR_FIO) || defined(ARDUINO_AVR_BT)   || defined(ARDUINO_AVR_LILYPAD) || defined(ARDUINO_AVR_PRO)      || \
        defined(ARDUINO_AVR_NG) || defined(ARDUINO_AVR_UNO_WIFI_DEV_ED) || defined(ARDUINO_AVR_DUEMILANOVE) || defined(ARDUINO_AVR_FEATHER328P) || \
        defined(ARDUINO_AVR_METRO) || defined(ARDUINO_AVR_PROTRINKET5) || defined(ARDUINO_AVR_PROTRINKET3) || defined(ARDUINO_AVR_PROTRINKET5FTDI) || \
        defined(ARDUINO_AVR_PROTRINKET3FTDI) )
  #define USE_TIMER_1     true
  #warning Using Timer1
#else          
  #define USE_TIMER_3     true
  #warning Using Timer3
#endif

#include "TimerInterrupt.h"

#define TIMER_INTERVAL_MS         10
#define DEBOUNCING_INTERVAL_MS    250
#define LONG_PRESS_INTERVAL_MS    5000

unsigned char ssd_code[10]=  {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x07, 0x7F, 0x67};

const int dispNumber=4;
int dipIndex=0;

unsigned int numdigit[dispNumber] = { 0 };
unsigned int time_ms=0;
unsigned int time_sn=0;

char state = 'r'; // s=stop, c=count, r=reset

const byte buttonPin = 2;
const byte serData = 8;
const byte serClk = 9;
const byte latchPin = 0;
const byte digit1 = 1;
const byte digit2 = 12;
const byte digit3 = 11;
const byte digit4 = 10;
int bouncing = 0;


void setup() {
  // common pins of ssd output
  pinMode(digit4,OUTPUT);
  pinMode(digit3,OUTPUT);    
  pinMode(digit2,OUTPUT);
  pinMode(digit1,OUTPUT);
  
  pinMode(serData,OUTPUT);
  pinMode(serClk,OUTPUT);
  
  pinMode(latchPin,OUTPUT);
  
  // turn off displays
  digitalWrite(digit4, LOW);
  digitalWrite(digit3,LOW);
  digitalWrite(digit2,LOW);
  digitalWrite(digit1,LOW);
  digitalWrite(serData,LOW);
  digitalWrite(serClk,LOW);
  digitalWrite(latchPin,HIGH);
  
  ITimer1.init();
  ITimer1.attachInterruptInterval(TIMER_INTERVAL_MS, Timer1Handler);

  //  control button
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonHandler, RISING);

}

void Timer1Handler()
{
  // state
  switch(state){
    case 'r' : 
      time_sn=0;
      time_ms=0;
      break;
    case 's' : 
      //do noting
      break;
    case 'c' :
      count();
      break;
  }
  
  // convert and display time
  divideTimeToDigits(time_sn, time_ms);
  showDigit();
}

void buttonHandler()
{
  // check if the button is pressed
  // change state when relased
    // change status

    if (bouncing==0)
    {
      // set state
      switch(state){
      case 'r' : 
        state='c';
        break;
      case 's' : 
        state='r';
        break;
      case 'c' :
        state='s';
        break;
      }
    } else {
      bouncing+=1;
      if (bouncing>DEBOUNCING_INTERVAL_MS/TIMER_INTERVAL_MS) bouncing=0;    
    }
}


void divideTimeToDigits(int t_s, int t_ms)
{
  numdigit[3] = t_s/100;
  numdigit[2] =(t_s/10)%10;
  numdigit[1] = t_s%10;
  numdigit[0] = t_ms/100;  
}

void  showDigit( )
{
    digitalWrite(digit4, HIGH);
    digitalWrite(digit3, HIGH);
    digitalWrite(digit2, HIGH);
    digitalWrite(digit1, HIGH);

    digitalWrite(latchPin,LOW);
    if (dipIndex==1) 
    {
      shiftOut(serData, serClk, MSBFIRST, ssd_code[numdigit[dipIndex]]|0x80);
    } else {
      shiftOut(serData, serClk, MSBFIRST, ssd_code[numdigit[dipIndex]]);
    }
    digitalWrite(latchPin,HIGH);
    
    
    // turn on nth display
    switch(dipIndex){
      case 0 : 
        digitalWrite(digit1,LOW);
        break;
      case 1 : 
        digitalWrite(digit2,LOW);
        break;
      case 2 :
        digitalWrite(digit3,LOW);
        break;
      case 3 :
        digitalWrite(digit4,LOW);
        break;
    }

  // increase display index
  dipIndex = (dipIndex==dispNumber-1) ? 0 : ++dipIndex;
}

void count()
{
  time_ms=time_ms+10;
  if (time_ms==1000)
  {
    time_sn=time_sn+1;
    time_ms=0;
  }
  
  if (time_sn==1000)
  {
    time_sn=0;
    time_ms=0;
  }

}


void loop() 
{
  // put your main  code here, to run repeatedly:


  
   
}
