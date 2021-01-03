
#include <MAX31855.h>
#include <PID_v1.h>
#include "parameters.h"



void TC_Relay_Init()
{

  windowStartTime = millis();
  pinMode(TRIACPIN, OUTPUT);

  //initialize the variables we're linked to
  Setpoint = initialSetpoint;
  //x = temp.readMAX31855(&tempTC, &tempCJC, &faultOpen, &faultShortGND, &faultShortVCC);

  //tell the PID to range between 0 and 100 percent
  //myPID.SetOutputLimits(0.0, 50.0); //limit to 50% to avoid burning out the coils
  myPID.SetOutputLimits(0.0, 75.0); //The stovetop Burner should be able to handle 100%?
  myPID.SetSampleTime(SampleInterval); //1 second updates
  //turn the PID on
  myPID.SetMode(AUTOMATIC);

}


double GetSetpoint()
{
  return finalSetpoint;
}

void JumpSetpoint(double fSetPoint)
{
  initialTime = millis();

  windowStartTime = 0; //why reset window start time? TEST COMMENT OUT

  //initialSetpoint = tempTC; //This really messes things up when you just want to change the rate!
  initialSetpoint = fSetPoint;
  finalSetpoint = fSetPoint;
  heating_rate = 0; //if we're jumping setpoint we're not ramping
}

void SetSetpoint(double fSetPoint)
{
  initialTime = millis();
  windowStartTime = 0; //TEST COMMENT OUT
  //initialSetpoint = tempTC; //This really messes things up when you just want to change the rate!
  initialSetpoint = Setpoint;
  finalSetpoint = fSetPoint;
}

double GetRate()
{
  //What the hell is this crap below
  /*
    initialTime = millis();
    windowStartTime = 0;
    //initialSetpoint = tempTC;
    initialSetpoint = Setpoint;
  */
  return heating_rate;
}

void SetRate(double fRate)
{
  if (finalSetpoint > initialSetpoint)
    heating_rate = fRate;
  else
    heating_rate = -fRate;
}

/*
  void ProgramAdvance() //Absolute Time Version
  {
  unsigned long Schedule;
  if (programRunning)
  {
    Schedule = (millis() - programTime) / 1000;

    //if(Schedule % 30 == 0)
   // {
    //  Serial.print("DEBUG: ");
    //  Serial.print(14 * 3600); //WRONG
    //  Serial.print(", ");
    //  Serial.println(14.0 * 3600); //RIGHT
   // }

    switch (programID)
    {
      case 1:
        switch (programState)
        {
          case 0:
            Serial.println("Ramping to dry/dewax temp, 230C");
            process_string("M1 S230 R90", 11);
            //process_string("M1 S230 R0.4", 12);
            programState++;
            break;
          case 1:
            if (Schedule > 4.5 * 3600) //4.5 hours BE CAREFUL! Precompiler will NOT use the right type, append UL to be sure! (or add a decimal)
            //if (Schedule > 1 * 30)
              programState++; //If you reset programTime each state transition you would be able to skip states with an M9 SID Rstate command! thoughts?
            break;
          case 2:
            Serial.println("Ramping to burnout temp, 730C");
            process_string("M1 S730 R150", 12);
            //process_string("M1 S730 R0.4", 12);
            programState++;
            break;
          case 3:
            if (Schedule > 12.0 * 3600) //12.0 hours MUST BE FLOAT
            //if (Schedule > 2 * 30)
              programState++;
            break;
          case 4:
            Serial.println("Ramping to casting temp, 530C");
            process_string("M1 S530 R60", 11);
            //process_string("M1 S530 R0.4", 12);
            programState++;
            break;
          case 5:
            if (Schedule > 14.0 * 3600) //14.0 hours MUST BE FLOAT
            //if (Schedule > 3 * 30)
              programState++;
            break;
          case 6:
            programRunning = false;
            Serial.println("Program 1 finished, ready to cast");
            programState++;
            break;
        }
        break;
      case 2:
        break;
    }
  }
  }*/
//TODO: M10 jump to state maybe?
//TODO: Fix ramp rate code (DONE!)
//TODO: Fix issuing a ramp mid-ramp problem (either jumps or sits at ramp rate of 0)

void ProgramAdvance() //Relative time version
{
  unsigned long Schedule;
  if (programRunning)
  {
    Schedule = (millis() - programTime) / 1000;

    switch (programID) //Need to implement gcode command for easy jumping between program states!
    {
      case 1:
        switch (programState)
        {
          case 0: //ramp to 230C
            Serial.println(F("Starting Full Program for complex flasks"));
            Serial.println(F("0-Ramp to dry/dewax temp, 230C over 90min"));
            process_string("M1 S230 R90", 11); //ramp to 230C over 90 minutes
            programState++;
            break;

          case 1: //wait state
            if (Schedule > 90 * (60.0)) //4.5 hours
            {
              programState++; //advance
              programTime = millis(); //reset timer
            }
            break;

          case 2: //message only
            Serial.println(F("2-Hold at 230C for 180min"));
            programState++; //advance
            break;

          case 3: //wait state
            if (Schedule > 180 * (60.0))
            {
              programState++; //advance
              programTime = millis(); //reset timer
            }
            break;

          case 4: //ramp to 730C
            Serial.println(F("4-Ramping to burnout temp, 730C over 150min"));
            process_string("M1 S730 R150", 12); //ramp to 730C over 150 minutes
            programState++;
            break;

          case 5: //wait state
            if (Schedule > 150 * (60.0))
            {
              programState++;
              programTime = millis(); //reset timer
            }
            break;

          case 6: //message only
            Serial.println(F("6-Hold at 730C for 300min"));
            programState++; //advance
            break;

          case 7: //wait state
            if (Schedule > 300 * (60.0))
            {
              programState++; //advance
              programTime = millis(); //reset timer
            }
            break;

          case 8: //ramp to 530C
            Serial.println(F("8-Ramping to casting temp, 530C"));
            process_string("M1 S530 R60", 11); //Ramp to 530C over 60 minutes
            programState++;
            break;

          case 9: //wait state
            if (Schedule > 60 * (60.0))
            {
              programState++;
              programTime = millis(); //reset timer
            }
            break;

          case 10: //message only
            Serial.println(F("10-Hold at 530C for 60min"));
            programState++; //advance
            break;

          case 11: //wait state
            if (Schedule > 60 * (60.0))
            {
              programState++;
              programTime = millis(); //reset timer
            }
            break;

          case 12:
            programRunning = false;
            Serial.println("12-Program 1 finished, ready to cast");
            programState++;
            break;
        }
        break;
      case 2:
        switch (programState)
        {
          case 0: //ramp to 150C
            Serial.println(F("Starting Fast Program for 2in flasks"));
            Serial.println(F("0-Ramp to dry/dewax temp, 150C over 60min"));
            process_string("M1 S150 R60", 11); //ramp to 150C over 60 minutes
            programState++;
            break;

          case 1: //wait state
            if (Schedule > 60 * (60.0)) //4.5 hours
            {
              programState++; //advance
              programTime = millis(); //reset timer
            }
            break;

          case 2: //message only
            //Serial.println(F("2-Hold at 150C for 60min")); //experiment showed mold was not yet dry at 60 min, needed another 40 min.
            Serial.println(F("2-Hold at 150C for 120min"));
            programState++; //advance
            break;

          case 3: //wait state
            if (Schedule > 120 * (60.0))
            {
              programState++; //advance
              programTime = millis(); //reset timer
            }
            break;
          case 4: //ramp to 370C
            Serial.println(F("4-Ramping to low burnout temp, 370C")); //70C difference with measured data
            process_string("M1 S300 R60", 12); //ramp to 370C over 60 minutes
            programState++;
            break;

          case 5: //wait state
            if (Schedule > 60 * (60.0))
            {
              programState++;
              programTime = millis(); //reset timer
            }
            break;

          case 6: //message only
            Serial.println(F("6-Hold at 370C for 60min"));
            programState++; //advance
            break;

          case 7: //wait state
            if (Schedule > 60 * (60.0))
            {
              programState++;
              programTime = millis(); //reset timer
            }
            break;

          case 8: //ramp to 730C
            Serial.println(F("8-Ramping to full burnout temp, 730C over 120min")); //measured 50C delta
            process_string("M1 S680 R120", 12); //ramp to 730C over 120 minutes
            programState++;
            break;

          case 9: //wait state
            if (Schedule > 120 * (60.0))
            {
              programState++;
              programTime = millis(); //reset timer
            }
            break;

          case 10: //message only
            Serial.println(F("10-Hold at 730C for 120min"));
            programState++; //advance
            break;

          case 11: //wait state
            if (Schedule > 120 * (60.0))
            {
              programState++; //advance
              programTime = millis(); //reset timer
            }
            break;

          case 12: //ramp to 530C
            Serial.println("12-Ramping to casting temp, 530C"); //measured 60 degree delta
            process_string("M1 S470 R45", 11); //Ramp to 530C over 45 minutes
            programState++;
            break;

          case 13: //wait state
            if (Schedule > 45 * (60.0))
            {
              programState++;
              programTime = millis(); //reset timer
            }
            break;

          case 14: //message only
            Serial.println(F("14-Hold at 530C for 60min"));
            programState++; //advance
            break;

          case 15: //wait state
            if (Schedule > 60 * (60.0))
            {
              programState++;
              programTime = millis(); //reset timer
            }
            break;

          case 16:
            programRunning = false;
            Serial.println("16-Program 2 finished, ready to cast");
            programState++;
            break;
        }
        break;
      case 3:
        switch (programState)
        {
          case 0: //ramp to 230C
            Serial.println(F("Starting formlabs reccomended 14 hour burnout"));
            Serial.println(F("0-Ramp to dry/dewax temp, 177C over 150min"));
            process_string("M1 S177 R150", 11); //ramp to 177C over 150 minutes
            programState++;
            break;

          case 1: //wait state
            if (Schedule > 150 * (60.0)) //4.5 hours
            {
              programState++; //advance
              programTime = millis(); //reset timer
            }
            break;

          case 2: //message only
            Serial.println(F("2-Hold at 177C for 30min"));
            programState++; //advance
            break;

          case 3: //wait state
            if (Schedule > 30 * (60.0))
            {
              programState++; //advance
              programTime = millis(); //reset timer
            }
            break;

          case 4: //ramp to 730C
            Serial.println(F("4-Ramping to burnout temp, 730C over 270min"));
            process_string("M1 S730 R270", 12); //ramp to 730C over 270 minutes
            programState++;
            break;

          case 5: //wait state
            if (Schedule > 270 * (60.0))
            {
              programState++;
              programTime = millis(); //reset timer
            }
            break;

          case 6: //message only
            Serial.println(F("6-Hold at 730C for 180min"));
            programState++; //advance
            break;

          case 7: //wait state
            if (Schedule > 180 * (60.0))
            {
              programState++; //advance
              programTime = millis(); //reset timer
            }
            break;

          case 8: //ramp to 530C
            Serial.println(F("8-Ramping to casting temp, 500C over 150min"));
            process_string("M1 S530 R150", 11); //Ramp to 500C over 150 minutes
            programState++;
            break;

          case 9: //wait state
            if (Schedule > 150 * (60.0))
            {
              programState++;
              programTime = millis(); //reset timer
            }
            break;

          case 10: //message only
            Serial.println(F("10-Hold at 530C for 60min"));
            programState++; //advance
            break;

          case 11: //wait state
            if (Schedule > 60 * (60.0))
            {
              programState++;
              programTime = millis(); //reset timer
            }
            break;

          case 12:
            programRunning = false;
            Serial.println("12-Program 3 finished, ready to cast");
            programState++;
            break;
        }
        break;
      case 4:
        switch (programState)
        {
          case 0: //jump to 177C
            Serial.println(F("Starting Plasticast <3in PREHEATED!"));
            Serial.println(F("0-Verify dry/dewax temp, 150C"));
            process_string("M2 S132", 7); //jump to 132C just to make sure
            programState++;
            break;

          case 1: //wait state
            if (Schedule > 60 * (60.0)) //60 minutes
            {
              programState++; //advance
              programTime = millis(); //reset timer
            }
            break;

          case 2: //ramp to 730C
            Serial.println(F("2-Ramping to burnout1 temp, 370C over 60min"));
            process_string("M1 S328 R60", 11); //ramp to 370C over 60 minutes
            programState++;
            break;

          case 3: //wait state
            if (Schedule > 60 * (60.0))
            {
              programState++;
              programTime = millis(); //reset timer
            }
            break;

          case 4: //message only
            Serial.println(F("4-Hold at 370C for 60min"));
            programState++; //advance
            break;

          case 5: //wait state
            if (Schedule > 60 * (60.0))
            {
              programState++; //advance
              programTime = millis(); //reset timer
            }
            break;

          case 6: //ramp to 730C
            Serial.println(F("6-Ramping to burnout2 temp, 730C over 120min"));
            process_string("M1 S701 R120", 12); //Ramp to 730C over 120 minutes
            programState++;
            break;

          case 7: //wait state
            if (Schedule > 120 * (60.0))
            {
              programState++;
              programTime = millis(); //reset timer
            }
            break;

          case 8: //message only
            Serial.println(F("8-Hold at 730C for 120min"));
            programState++; //advance
            break;

          case 9: //wait state
            if (Schedule > 120 * (60.0))
            {
              programState++;
              programTime = millis(); //reset timer
            }
            break;

          case 10: //message only
            Serial.println(F("10-Ramp to cast 500C for 45min"));
            process_string("M1 S470 R45", 11); //Ramp to 500C over 45 minutes
            programState++; //advance
            break;

          case 11: //wait state
            if (Schedule > 45 * (60.0))
            {
              programState++;
              programTime = millis(); //reset timer
            }
            break;

          case 12: //message only
            Serial.println(F("12-Hold at 500C for 60min"));
            programState++; //advance
            break;

          case 13: //wait state
            if (Schedule > 60 * (60.0))
            {
              programState++;
              programTime = millis(); //reset timer
            }
            break;

          case 14:
            programRunning = false;
            Serial.println(F("14-Program 4 finished, ready to cast"));
            programState++;
            break;
        }
        break;
      default: programRunning = false;
    }
  }
}

void TC_Relay_Loop()
{
  unsigned long time_elapsed = millis() - initialTime;

  //if(time_elapsed < windowStartTime) //this is a weird siutation and I have no idea how it happens
  //    windowStartTime = time_elapsed - SampleInterval;

  //Sample time up, update all variables
  if ((time_elapsed > windowStartTime) && (time_elapsed - windowStartTime) >= SampleInterval)
  {
    //Serial.println(SampleInterval);
    //Serial.println(initialTime);
    //Serial.println(windowStartTime);
    //Serial.println(time_elapsed);


    //tic = micros();
    //Calc ramp
    Setpoint = initialSetpoint + double(time_elapsed) * heating_rate / oneminute;
    //we've reached our goal, so stop ramping
    if ((Setpoint > finalSetpoint && heating_rate > 0) || (Setpoint < finalSetpoint && heating_rate < 0))
    {
      initialSetpoint = finalSetpoint;
      heating_rate = 0;
    }

    windowStartTime += SampleInterval;
    //windowStartTime = time_elapsed - windowStartTime;

    //reportResult(Setpoint,tempTC,tempCJC,faultOpen,faultShortGND,faultShortVCC,Output,SampleInterval);
    double tempTCOld = tempTC, Temp1, Temp2, Temp3;


    do {

      x = temp.readMAX31855(&tempTC, &tempCJC, &faultOpen, &faultShortGND, &faultShortVCC);
      while (tempTC == 9999) x = temp.readMAX31855(&tempTC, &tempCJC, &faultOpen, &faultShortGND, &faultShortVCC);
      Temp1 = tempTC;

      x = temp.readMAX31855(&tempTC, &tempCJC, &faultOpen, &faultShortGND, &faultShortVCC);
      Temp2 = tempTC;

      x = temp.readMAX31855(&tempTC, &tempCJC, &faultOpen, &faultShortGND, &faultShortVCC);
      Temp3 = tempTC;

    } while (abs(Temp1 - Temp2) > 1.0 || abs(Temp2 - Temp3) > 1.0); //repeat until the differences between the readings are below 1 degree

    tempTC = (Temp1 + Temp2 + Temp3) / 3.0; //average three readings together

    if (millis() > 5000) //exponential filter
      tempTC = ALPHA_TC * tempTC + (1.0 - ALPHA_TC) * tempTCOld ;

    /*Serial.print(Temp1);
      Serial.print(' ');
      Serial.print(Temp2);
      Serial.print(' ');
      Serial.println(Temp3);*/
    //if(faultOpen | faultShortGND | faultShortVCC)
    //  tempTC = tempTCOld; //discard result if we have a blip, else the offscale value will mess up our PID (above filter should work)
    OutputPPrev = OutputP;
    myPID.Compute(); //this will update OutputP (0-100 output percent)
    OutputP = ALPHA_OUTP * OutputP + (1.0 - ALPHA_OUTP) * OutputPPrev ;

    if (round(OutputP) != round(OutputPPrev)) // Only restart algorithm on a power change
      cycleCount = 0; //this will force a restart of the algorithm on a power level change. This is handled in the interrupt!

    ProgramAdvance();
    reportResult(Setpoint, tempTC, tempCJC, faultOpen, faultShortGND, faultShortVCC, OutputP, SampleInterval);

    if (killFlag == true)
    {
      if (tempTC >= finalSetpoint)
        killFlagCounter++;
      else
        killFlagCounter = 0;

      if (killFlagCounter > 10) //10 consecutive seconds of temperature required to kill power
      {
        finalSetpoint = 0;
        initialSetpoint = finalSetpoint;
        heating_rate = 0;
        killFlagCounter = 0;
        killFlag = false;
        Serial.println("Reached Temp, killing heater");
      }
    }
    else
      killFlagCounter = 0;
  }
}
