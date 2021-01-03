/********************************************************
   process_string

   Just like in RepRap, I want functions to parse and use
   the serial commands.  Serial commands are delivered byte-
   by-byte, so I'd like to steal my well-tested "arduino-gcode"
   CNC control parser.  CNC Controller has been working well
   for almost 2 years now.
 *******************************************************/

#include "parameters.h"



/* gcode line parse results */
struct GcodeParser
{
  unsigned int Mseen;
  unsigned int Sseen;
  unsigned int Rseen;
  int M;
  float S;
  float R;
};

//init our string processing
void init_process_string()
{
  serial_count = 0;
  comment = false;
}

// Get a command and process it
void get_and_do_command()
{
  //read in characters if we got them.
  if (Serial.available())
  {
    c = Serial.read();
    if (c == '\r')
      c = '\n';
    // Throw away control chars except \n
    if (c >= ' ' || c == '\n')
    {

      //newlines are ends of commands.
      if (c != '\n')
      {
        // Start of comment - ignore any bytes received from now on
        if (c == ';')
          comment = true;

        // If we're not in comment mode, add it to our array.
        if (!comment)
          cmdbuffer[serial_count++] = c;
      }

    }
  }

  // Data runaway?
  if (serial_count >= COMMAND_SIZE)
    init_process_string();

  //if we've got a real command, do it
  if (serial_count && c == '\n')
  {
    Serial.println(cmdbuffer);
    // Terminate string
    cmdbuffer[serial_count] = 0;

    //process our command!
    process_string(cmdbuffer, serial_count);

    //clear command.
    init_process_string();

    // Say we're ready for the next one

    Serial.println("ok");
  }
}

/*
  #define PARSE_INT(ch, str, len, val) \
  case ch: \
  len = scan_int(str, &val); \
  break;

  #define PARSE_FLOAT(ch, str, len, val) \
  case ch: \
  len = scan_float(str, &val); \
  break;
*/
/*
  #define PARSE_INT(ch, str, len, val, seen, flag) \
  case ch: \
  len = scan_int(str, &val, &seen, flag); \
  break;

  #define PARSE_FLOAT(ch, str, len, val, seen, flag) \
  case ch: \
  len = scan_float(str, &val, &seen, flag); \
  break;
*/
//int scan_float(char *str, float *valp, unsigned int *seen, unsigned int flag)
int scan_float(char *str, float *valp, unsigned int *seen)
{
  float res;
  int len;
  char *end;

  res = (float)strtod(str, &end);

  len = end - str;

  if (len > 0)
  {
    *valp = res;
    *seen = 1;
  }
  else
    *valp = 0;

  return len;  /* length of number */
}

//int scan_int(char *str, int *valp, unsigned int *seen, unsigned int flag)
int scan_int(char *str, int *valp, unsigned int *seen)
{
  int res;
  int len;
  char *end;

  res = (int)strtol(str, &end, 10);
  len = end - str;

  if (len > 0)
  {
    *valp = res;
    *seen  = 1;
  }
  else
    *valp = 0;

  return len; /* length of number */
}

/*
  int scan_float(char *str, float *valp)
  {
  float res;
  int len;
  char *end;

  res = (float)strtod(str, &end);

  len = end - str;

  if (len > 0)
  {
     valp = res;
  }
  else
     valp = 0;

  return len;	// length of number
  }

  int scan_int(char *str, int *valp)
  {
  int res;
  int len;
  char *end;

  res = (int)strtol(str, &end, 10);
  len = end - str;

  if (len > 0)
  {
     valp = res;
  }
  else
     valp = 0;

  return len;	// length of number
  }
*/
void process_string(char instruction[], int size)
{

  GcodeParser gc;	/* string parse result */
  //double newSetpoint;
  //double newRate;
  int ind;
  int len;	/* length of parameter argument */

  gc.Mseen = 0;
  gc.Rseen = 0;
  gc.Sseen = 0;

  len = 0;
  /* scan the string for commands and parameters, recording the arguments for each,
     and setting the seen flag for each that is seen
  */

  for (ind = 0; ind < size; ind += (1 + len))
  {
    len = 0;
    switch (instruction[ind])
    {

      //PARSE_INT('M', &instruction[ind + 1], len, gc.M,gc.Seen,0x1);
      //PARSE_FLOAT('S', &instruction[ind + 1], len, gc.S,gc.Seen,0x2);
      //PARSE_FLOAT('R', &instruction[ind + 1], len, gc.R,gc.Seen,0x4);

      case 'M':
        len = scan_int(&instruction[ind + 1], &gc.M, &gc.Mseen);
        break;
      case 'S':
        len = scan_float(&instruction[ind + 1], &gc.S, &gc.Sseen);
        break;
      case 'R':
        len = scan_float(&instruction[ind + 1], &gc.R, &gc.Rseen);
        break;

        /*
          #define PARSE_INT(ch, str, len, val, seen, flag) \
          case ch: \
          len = scan_int(str, &val, &seen, flag); \
          break;

          #define PARSE_FLOAT(ch, str, len, val, seen, flag) \
          case ch: \
          len = scan_float(str, &val, &seen, flag); \
          break;
              default:
                break;*/
    }
  }

  if (gc.Mseen  == 0) //M was not seen!
  {

    Serial.println(", Invalid command");
    Serial.print("Seen = ");
    Serial.print(gc.Mseen);
    Serial.print(", M = ");
    Serial.print(gc.M);
    Serial.print(", S = ");
    Serial.print(gc.S);
    Serial.print(", R = ");
    Serial.print(gc.R);
    Serial.print(", Kill at temp = ");
    Serial.println(killFlag);
    return;
  }
  Serial.print(", MSeen = ");
  Serial.print(gc.Mseen);
  Serial.print(", M = ");
  Serial.print(gc.M);

  Serial.print(", SSeen = ");
  Serial.print(gc.Sseen);
  Serial.print(", S = ");
  Serial.print(gc.S);

  Serial.print(", RSeen = ");
  Serial.print(gc.Rseen);
  Serial.print(", R = ");
  Serial.print(gc.R);
  Serial.print(", Kill at temp = ");
  Serial.println(killFlag);

  if (gc.M == 0) //Ramp at deg c/min
  {
    if (gc.Sseen)
      if (gc.S >= TMIN && gc.S <= TMAX)
        SetSetpoint(gc.S);

    if (gc.Rseen)
      if (gc.R > RMIN && gc.R < RMAX)
        SetRate(gc.R);

    killFlag = false;
  }
  else if (gc.M == 1) //ramp over time
  {
    if (gc.Rseen)
      SetRate((GetSetpoint() - gc.S) / float(gc.R));

    if (gc.Sseen)
      if (gc.S >= TMIN && gc.S <= TMAX)
        SetSetpoint(gc.S);

    Serial.print("Ramp to ");
    Serial.print(GetSetpoint());
    Serial.print(" DegC at ");
    Serial.print(GetRate());
    Serial.print(" DegC/Min taking ");
    Serial.print(gc.R);
    Serial.println(" minutes");
    killFlag = false;
  }
  else if (gc.M == 2) //Jump to
  {
    if (gc.Sseen)
      if (gc.S >= TMIN && gc.S <= TMAX)
        //SetSetpoint(gc.S);
        JumpSetpoint(gc.S);

    SetRate(0);
    killFlag = false;
  }
  else if (gc.M == 3) //heat until "ramp to kill"
  {
    if (gc.Sseen)
      if (gc.S >= TMIN && gc.S <= TMAX)
        SetSetpoint(gc.S);

    if (gc.Rseen)
      if (gc.R > RMIN && gc.R < RMAX)
        SetRate(gc.R);

    killFlag = true;
  }
  else if (gc.M == 4) //pause and HOLD
  {
    myPID.SetMode(MANUAL);
    hold = true;
    //pause setpoint at current
    //gc.S = Setpoint;
    //gc.R = 0;
    //JumpSetpoint(gc.S);

  }
  else if (gc.M == 5)//resume
  {
    myPID.SetMode(AUTOMATIC);
    hold = false;
    //pause setpoint at current
    //gc.S = Setpoint;
    //gc.R = 0;
    //JumpSetpoint(gc-  >S);

  }
  else if (gc.M == 6) //pause and set power
  {
    //myPID.SetMode(MANUAL);
    //hold = true;
    OutputP = gc.S;
    //pause setpoint at current
    //gc.S = Setpoint;
    //gc.R = 0;
    //JumpSetpoint(gc.S);

  }
  else if (gc.M == 7) //Print PID values
  {

    Serial.print("P = ");
    Serial.print(aggKp);
    Serial.print(", I = ");
    Serial.print(aggKi);
    Serial.print(", D = ");
    Serial.println(aggKd);

  }

  else if (gc.M == 8) //Print Help
  {
    printHelp();
  }

  else if (gc.M == 9) //Run Program S
  {
    if (gc.Sseen)
    {
      programID = gc.S;
      programRunning = true;
      programTime = millis();
      programState = 0;
      Serial.print("Running program ID:");
      Serial.println(programID);
    }
    else
    {
      programRunning = false;
      Serial.println("Program Stopped");
    }
  }
  else if (gc.M == 10) //Run Program S
  {
    if (!gc.Sseen)
    {
      gc.S = 0;
    }
    Serial.println("Program Summary:");
    if (gc.S == 1 || gc.S == 0)
      Serial.println("1: SRS Classic Full");
    if (gc.S == 1)
    {
      Serial.println("1.1: Ramp 230C, 90min, hold 180 min");
      Serial.println("1.2: Ramp 730C, 150min, hold 300 min");
      Serial.println("1.3: Ramp cast, 60 min, hold 60 min");
    }

    if (gc.S == 2 || gc.S == 0)
      Serial.println("2: Modified SRS Classic for 2in flasks");
    if (gc.S == 2)
    {
      Serial.println("2.1: Ramp 150C, 60min, hold 120 min");
      Serial.println("2.2: Ramp 370C, 60 min, hold 60 min");
      Serial.println("2.3: Ramp 730C, 120 min, hold 120 min");
      Serial.println("2.4: Ramp cast, 45 min, hold 60 min");
    }

    if (gc.S == 3 || gc.S == 0)
      Serial.println("3: Formlabs reccomended 14 hour burnout");
    if (gc.S == 3)
    {
      Serial.println("3.1: Ramp 177C, 150min, hold 30 min");
      Serial.println("3.2: Ramp 730C, 270 min, hold 180 min");
      Serial.println("3.3: Ramp cast, 150 min, hold 60 min");
    }

    if (gc.S == 4 || gc.S == 0)
      Serial.println("4: Plasticast PREHEAT TO 150C!!");
    if (gc.S == 4)
    {

      Serial.println("4.1: Hold at 150C for 60 min");
      Serial.println("4.2: Ramp to 370C, 60 min, hold 60 min");
      Serial.println("4.3: Ramp to 730C, 120 min, hold 120 min");
      Serial.println("4.4: Ramp to cast, 60 min, hold 60 min");
    }
  }
  /* else if (gc.M == 8) //Print PID values
    {

    Serial.print("P = ");
    Serial.print(aggKp);
    Serial.print(", I = ");
    Serial.print(aggKi);
    Serial.print(", D = ");
    Serial.println(aggKd);

    }
    else if (gc.M == 9) //Print PID values
    {

    Serial.print("P = ");
    Serial.print(aggKp);
    Serial.print(", I = ");
    Serial.print(aggKi);
    Serial.print(", D = ");
    Serial.println(aggKd);

    }*/
  else
    printHelp();

}

void reportResult(double Setpoint, double tempTC, double tempCJC,
                  bool faultOpen, bool faultShortGND, bool faultShortVCC,
                  double OutputP, int SampleInterval)
{
  // Report during each new window
  Serial.print(millis() / 1000.0);
  Serial.print("\t");
  Serial.print(Setpoint);
  Serial.print("\t");
  Serial.print(tempTC);
  Serial.print("\t");
  Serial.print(Setpoint - tempTC);
  Serial.print("\t");
  Serial.print(faultOpen);
  Serial.print(faultShortGND);
  Serial.print(faultShortVCC);
  Serial.print("\t");
  //Serial.print(OutputP); //this is ideal power, not actual
  //Serial.print(((100.0 * cycleOnCount) / cycleCount)); //ACTUAL power not requested power plz
  Serial.print(round(OutputP)); //actual power has some bugs, but its rounded to the nearest percent.

  //Serial.print("\tP\t");
  //Serial.print(myPID.GetKp());
  //Serial.print("\tI\t");
  //Serial.print(myPID.GetKi());

  if (cycleFlag == 1 || ((millis() - cycleTime) > 20) )
  {
    //let the user know we have a problem with the zero crossing detector and reset the flag.
    cycleFlag = 0;
    cycleTime = millis(); //reset this just in case to avoid rollover errors if power is lost
    Serial.print("\t"); //temp
    Serial.print("ERR_ZERO");
  }

  if (hold == true)
  {
    Serial.print("\t");
    Serial.print("HOLD");
  }
  if (programRunning == true)
  {
    Serial.print("\t");
    Serial.print("RUN-");
    Serial.print(programState);
    Serial.print(":");
    Serial.print(programID);

  }
  Serial.print("\r\n");
  //Serial.print("\t");
  //Serial.println(SampleInterval);
}
