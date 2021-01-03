/*
  Control_TC_Serial

  Sets temperature ramp based on serial inputs.  At the very
  least, it should take 2 arguments (setpoint and slope), and
  change the temperature.

  Based on PID Relay example and TC Shield examples.
  PID Relay example is weakly commented.  Analog example
  is much better.  Minimum time is ok?

  // How long before integral is flushed?
  // I guess it's the "Window Size" parameter -- 5000 millis.

  This work is licensed under a Creative Commons Attribution-ShareAlike 3.0 Unported License.
  http://creativecommons.org/licenses/by-sa/3.0/
*/

#include "parameters.h"

//Use an interrupt to detect the pulses from the zero crossing detector. The width of the pulse will straddle the actual crossing.
//When using zero-crossing triggered SSRs, we can pick which pulse to trigger, but we must trigger with the zero crossing pulse
ISR (PCINT0_vect) // handle pin change interrupt for D8 to D13 here
{
  if ((*portInputRegister(zeroCrossPort) & zeroCrossBit) == zeroCrossBit && zeroCrossStatus == 0) //zeroCrossPin went low to high
  {
    //digitalWrite(13,!digitalRead(13));
    *portOutputRegister(LEDPort) ^= LEDBit; //toggle LEDBit with XOR
    if (cycleOnCount > cycleCount) //overflow detection, pulses on can't be greater than total pulses!
    {
      cycleCount = 1;
      cycleOnCount = 0; //this will pretty much force an ON the next cycle. But seems smoother than forcing an OFF. 
    }

    if (((100.0 * cycleOnCount) / cycleCount) < round(OutputP)) //power level is too low, trigger this time to increase
    {
      *portOutputRegister(TRIACPort) |= TRIACBit; //set TRIAC pin high to trigger
      cycleOnCount++;
    }
    else
      *portOutputRegister(TRIACPort) &= ~TRIACBit; //Don't trigger triac (this isn't really needed)
      
    zeroCrossStatus = zeroCrossBit; //Mark status for next go around
    cycleCount++; //keep track of total number of zero crossings
  }
  else if ((*portInputRegister(zeroCrossPort) & zeroCrossBit) == 0 && zeroCrossStatus == zeroCrossBit) //zeroCrossPin went high to low
  {
    *portOutputRegister(TRIACPort) &= ~TRIACBit; //Turn off TRIAC trigger
    zeroCrossStatus = 0; //update status for next time
    cycleInt = millis() - cycleTime; //calculate the interval. This ought to be 100 or 120Hz!
    if(cycleInt > 14) //we must have missed something, bad connection? This will NOT detect a complete break as this will never get called if there are no edges!
      cycleFlag = 1; //this will get cleared once it gets read out to the user
    cycleTime = millis();
  }
  else //State of pin 10 didn't change and pin didn't change. Just update the state and bail.
    zeroCrossStatus = zeroCrossPort & zeroCrossBit; //Update pin state
}

void printHelp()
{   
  Serial.println(F("M0 Sx.x (DegC) Rx.x (DegC/min) to ramp-rate-to-hold"));
  Serial.println(F("M1 Sx.x (DegC) Rx.x (min) to ramp-time-to-hold"));
  Serial.println(F("M2 Sx.x (DegC) to jump"));
  Serial.println(F("M3 Sx.x (DegC) Rx.x ((DegC/min) to ramp-to-kill"));
  Serial.println(F("M4 Pause PID and Hold power"));
  Serial.println(F("M5 Resume PID"));
  Serial.println(F("M6 Sx.x (% Power) Set power level (manual)"));
  Serial.println(F("M7 Print PID settings"));
  Serial.println(F("M8 PrintHelp"));
  Serial.println(F("M9 SX Start Program X (M9 to stop)"));
  Serial.println(F("M10 Print program summary"));
  //Serial.println("M7 Sx.x Set P VALUE");
  //Serial.println("M8 Sx.x Set I VALUE");
  //Serial.println("M9 Sx.x Set D VALUE");
  }
  

void setup() {
  Serial.begin(38400);
  printHelp();
  TC_Relay_Init();

  //Set up Zero Crossing Pin as input, pullup, and interrupt
  pinMode(13, OUTPUT);  //LED is an output
  pinMode(ZEROCROSSPIN, INPUT);
  digitalWrite(ZEROCROSSPIN, HIGH); // enable pullup resistor

  zeroCrossBit = digitalPinToBitMask(ZEROCROSSPIN); //Set the bit for future use
  zeroCrossPort = digitalPinToPort(ZEROCROSSPIN);  //Set the port for future use
  TRIACBit = digitalPinToBitMask(TRIACPIN); //Set the bit for future use
  TRIACPort = digitalPinToPort(TRIACPIN);  //Set the port for future use

  LEDBit = digitalPinToBitMask(13); //Set the bit for future use
  LEDPort = digitalPinToPort(13);  //Set the port for future use
  
  noInterrupts();           // disable all interrupts
  //Setup Pin Interrupts
  *digitalPinToPCMSK(ZEROCROSSPIN) |= bit (digitalPinToPCMSKbit(ZEROCROSSPIN));  // enable pin
  PCIFR  |= bit (digitalPinToPCICRbit(ZEROCROSSPIN)); // clear any outstanding interrupt
  PCICR  |= bit (digitalPinToPCICRbit(ZEROCROSSPIN)); // enable interrupt for the group
  interrupts();             // enable all interrupts
}

void loop() {
  //unsigned long tic=micros(),toc;
  get_and_do_command();
  TC_Relay_Loop();
  //toc = micros();
  //Serial.println(toc-tic);
}
