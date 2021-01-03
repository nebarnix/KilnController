# KilnController
Arduino Kiln Controller using a 9" 220V stove burner, a solid state relay, a zero crossing detector, and a type K thermocouple (MAX31855.h)

Serial Commands (or use the QT GUI)

* M0 Sx.x (DegC) Rx.x (DegC/min) to ramp-rate-to-hold
* M1 Sx.x (DegC) Rx.x (min) to ramp-time-to-hold
* M2 Sx.x (DegC) to jump
* M3 Sx.x (DegC) Rx.x ((DegC/min) to ramp-to-kill
* M4 Pause PID and Hold power
* M5 Resume PID
* M6 Sx.x (% Power) Set power level (manual)
* M7 Print PID settings
* M8 PrintHelp
* M9 SX Start Program X (M9 to stop)
* M10 Print program summary

Program Summary:

#: SRS Classic Full<br/>

##: Ramp 230C, 90min, hold 180 min<br/>
##: Ramp 730C, 150min, hold 300 min<br/>
##: Ramp cast, 60 min, hold 60 min<br/>

#: Modified SRS Classic for 2in flasks<br/>

##: Ramp 150C, 60min, hold 120 min<br/>
##: Ramp 370C, 60 min, hold 60 min<br/>
##: Ramp 730C, 120 min, hold 120 min<br/>
##: Ramp cast, 45 min, hold 60 min<br/>

#: Formlabs reccomended 14 hour burnout<br/>

##: Ramp 177C, 150min, hold 30 min<br/>
##: Ramp 730C, 270 min, hold 180 min<br/>
##: Ramp cast, 150 min, hold 60 min<br/>

#: Plasticast PREHEAT TO 150C!!<br/>
##: Hold at 150C for 60 min<br/>
##: Ramp to 370C, 60 min, hold 60 min<br/>
##: Ramp to 730C, 120 min, hold 120 min<br/>
##: Ramp to cast, 60 min, hold 60 min<br/>
