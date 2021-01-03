#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <PID_v1.h>
#include <MAX31855.h>

#define ZEROCROSSPIN 10
#define TRIACPIN 11
#define minimumRelayTime 8 // one cycle is 8ms. Don't go below this?
#define oneminute 60000 // 60,000 millis per minute
#define COMMAND_SIZE 128 //our command string length

#define TMIN 0 // min temp (C)
#define TMAX 1000 // max temp (C)
#define RMIN 0 // min temp rate (C)
#define RMAX 20 // max temp rate (C)

#define ALPHA_TC 0.25 //smoothing factor for temperature measurements
#define ALPHA_OUTP 0.25 //smoothing factor for output power

//double aggKp=10, aggKi=0.05, aggKd=10;
//double aggKp=10, aggKi=.5, aggKd=10;
//double aggKp=1, aggKi=.05, aggKd=0;
//double aggKp=65, aggKi=0, aggKd=0;
//double aggKp=65.0, aggKi=0.1, aggKd=65.0/5.0;
//double aggKp=55.85, aggKi=0.240, aggKd=2152;
//double aggKp=5.585, aggKi=0.0240, aggKd=280; //D term way too noisey without filtering
//double aggKp=1.4561, aggKi=(aggKp/308.0), aggKd=(aggKp*20);
//double aggKp=1.4561, aggKi=(aggKp/308.0), aggKd=(aggKp/5.0); // Burner at 110V
//double aggKp=1.4561/5.0, aggKi=(aggKp/3000.0), aggKd=(aggKp/5.0);  //220V 2500 watt heater coils POE
//double aggKp=0.28, aggKi=(aggKp/3093.0), aggKd=0;  //220V 2500 watt heater coils POE -- tunings set by IMC with aggressive tuning
//double aggKp=0.28*4.0, aggKi=(aggKp/309.0), aggKd=0;  //110V 2500/4 watt heater coils POE -- tunings set by IMC with aggressive tuning, I*10
//double aggKp=0.28, aggKi=(aggKp/309.0), aggKd=0;  //220V 2500 watt heater coils POE -- tunings set by IMC with aggressive tuning, I*10
//double aggKp=0.25, aggKi=(aggKp/300.0), aggKd=0;  //220V 2500 watt heater coils POM
//double aggKp=0.23, aggKi=(aggKp/308.0), aggKd=0; // 9" Burner at 220V (DO NOT USE, need to tune this)
//double aggKp=0.6, aggKi=(aggKp/1000.0), aggKd=0; // 9" Burner at 220V (cohen coon plus matlab model fit) (orig is 0.6, 3000,0)

//double aggKp=0.4, aggKi=(aggKp/300.0), aggKd=0; // 9" Burner PonM at 220V (cohen coon plus matlab model fit) (orig is 0.6, 3000,0)
//double aggKp=0.5, aggKi=(aggKp/250.0), aggKd=0; // 9" Burner PonM at 220V (cohen coon plus matlab model fit) (orig is 0.6, 3000,0)
//double aggKp=0.6, aggKi=(aggKp/200.0), aggKd=0; // 9" Burner PonM at 220V (cohen coon plus matlab model fit) (orig is 0.6, 3000,0)
//double aggKp=0.5, aggKi=0.003/2.0, aggKd=0; // 9" Burner PonM at 220V worked really well but oscillated a bit around set point
double aggKp=0.5, aggKi=0.003/2.0, aggKd=0; // 9" Burner PonM at 220V 

bool killFlag = false;

//our command string
char cmdbuffer[COMMAND_SIZE];
char c = '?';
byte serial_count = 0;
boolean comment = false;

int scan_int(char *str, int *valp);
int scan_float(char *str, float *valp);

double initialSetpoint = 0.0; // initial
double finalSetpoint = 0.0; // goal
double heating_rate = 0; // degree per minute
unsigned long programTime=0, initialTime = 0;

//Define Variables we'll be connecting to
double Setpoint, OutputP, OutputPPrev;
unsigned int OutputUS; //output value in microseconds used by the timer ISR
double tempTC, tempCJC;
bool hold=false,faultOpen, faultShortGND, faultShortVCC, x,programRunning=false;

unsigned char programID,programState=0;


uint8_t volatile zeroCrossStatus=0; //this could get changed at any time by the interrupt, mark as volatile
uint8_t zeroCrossPort, zeroCrossBit; //these are just set once during initialization
uint8_t TRIACPort, TRIACBit; //these are just set once during initialization
uint8_t LEDPort, LEDBit; //these are just set once during initialization
unsigned int volatile cycleFlag=0,cycleCount=1,cycleOnCount=1; //store the 120hz cycles
unsigned long volatile cycleTime=0,cycleInt;

int killFlagCounter = 0, SampleInterval = 1000;
unsigned long windowStartTime; // each new increment

#define POLY1 -57920
#define POLY2 144800
#define POLY3 -138400
#define POLY4 62840
#define POLY5 -17870
#define POLY6 7455

//MAX31855 temp(3, 4, 5); // measure and fault pins
MAX31855 temp(5, 4, 3); // measure and fault pins PD3 PD4 PD5

//Specify the links and initial tuning parameters
//PID myPID(&tempTC, &OutputP, &Setpoint, aggKp, aggKi, aggKd, P_ON_E, DIRECT);
PID myPID(&tempTC, &OutputP, &Setpoint, aggKp, aggKi, aggKd, P_ON_M, DIRECT);

#endif
