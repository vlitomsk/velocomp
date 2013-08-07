#ifndef SETTINGS_H_
#define SETTINGS_H_

/* * * * * * * * * * * * * * * * * * * * * *
   This part is for indicator configuration 
* * * * * * * * * * * * * * * * * * * * * * */

/* Control port - PORTC */
#define CTLPORT PORTD
#define CTLPORTDDR DDRD

/* CLK - PC0, RST - PC1, ... */
#define CLK PD5
#define RST PD6
#define DATA PD7
#define ON PD4

//-------------------------------------------

/* * * * * * * * * * * * * * * * * * * * * * *
   This part is for hardware&logic configuration
   (buttons, sensors, etc.)
* * * * * * * * * * * * * * * * * * * * * * * */

#define WHEEL_TICKS_PER_SPEED_MEASUREMENT 4

#define TICKS_PER_SECOND 128
#define ticks_s(time_s) (time_s * TICKS_PER_SECOND)
//#define ticks_ms(time_ms) ((time_ms * TICKS_PER_SECOND) / 1000) // wtf this doesn't work

#define TIME_AWAKE ticks_s(5)
#define TIME_SHOWING_MODE 102UL

/* For anti-noise wait */
#define NOISE_TICKS_MAG 10
#define NOISE_TICKS_BTN 10

#endif