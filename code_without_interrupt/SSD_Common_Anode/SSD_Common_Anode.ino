#define buttonDelay 200
#define timeDelay_ms 10

const int dLedA = 0;
const int dLedB = 1;
const int dLedC = 2;
const int dLedD = 3;
const int dLedE = 4;
const int dLedF = 5;
const int dLedG = 6;
const int dLedDP = 7;

const int dispNumber = 4;
int dispIndex=0;

const int disp1 = 8;
const int disp2 = 9;
const int disp3 = 10;
const int disp4 = 11;

const int button = 12;

const int dLedAddr[8] = {dLedA,dLedB,dLedC,dLedD,dLedE,dLedF,dLedG,dLedDP};
const int dispAddr[dispNumber] = {disp1,disp2,disp3,disp4};

unsigned const char ssd_code[10]=  {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x07, 0x7F, 0x67};
unsigned int digitNumber[dispNumber];
unsigned int time_ms=0;
unsigned int time_sn=0;

char state = 'r'; // s=stop, c=count, r=reset
   
void setup() {
  // set portd output
  DDRD=255;

  // display select pins
  for (int i=0; i<dispNumber; i++) {
    pinMode(dispAddr[i], OUTPUT);
  }
  
  // control button
  pinMode(button,INPUT_PULLUP);  
  
  // turn off displays
  turnOffDisplay();

}

void divideTimeToDigits(int t_s, int t_ms)
{
  digitNumber[3] = t_s/100;
  digitNumber[2] = (t_s/10)%10;
  digitNumber[1] = t_s%10;
  digitNumber[0] = t_ms/100;  
}

void  showDigit( )
{
    turnOffDisplay();

    if (dispIndex==1) 
    {
      PORTD=~(ssd_code[ digitNumber[dispIndex] ]|0x80);
    } else {
      PORTD=~ssd_code[ digitNumber[dispIndex] ];
    }
  
    // turn on nth display
    digitalWrite(dispAddr[dispIndex],HIGH);
    
    // increase display index
    dispIndex = (dispIndex==dispNumber-1) ? 0 : ++dispIndex;
    
}

void count()
{
  time_ms=time_ms+timeDelay_ms;
  if (time_ms>999)
  {
    time_sn=time_sn+1;
    time_ms=0;
  }
  
  if (time_sn==10*(dispNumber-1))
  {
    time_sn=0;
    time_ms=0;
  }

  delay( timeDelay_ms );   
}

void turnOffDisplay()
{
  for (int i=0; i<dispNumber; i++) {
    digitalWrite(dispAddr[i], LOW);
  }
}


void loop() 
{
  // put your main  code here, to run repeatedly:

  // check if the button is pressed
  // change state when relased
  if ( digitalRead(button) )
  {
    while (digitalRead(button));
    delay(buttonDelay);

    // change status
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
  }
   
  // call state function
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
