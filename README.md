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
