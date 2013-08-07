#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "definitions.h"
#include "indicator_debug.h"

#define EVENT_MAG 0x1
#define EVENT_BTN 0x2
#define EVENT_REDRAW 0x4

volatile u8 events = 0;
volatile u32 time_ticks = 0; // by 1/128 of second

volatile u32 speed_measurement_time = 0;
volatile u32 last_speed_measurement_time = 0;
volatile u32 metric_speed = 0;

/* returns in [(1/10 of kilometer) per hour] */
inline u32 speed_to_metric() {
	/* Length of wheel * WHEELS_PER_TICK_MEASUREMENT * TICKS_PER_SECOND * 3600 / 1000 = 38246 */
	return 38246 / speed_measurement_time;
}

volatile u8 view_mode = 0;
#define VIEW_MODES_COUNT 2
#define VIEW_MODE_LEN 0
#define VIEW_MODE_SPD 1
#define next_view_mode() (view_mode = (view_mode + 1) % VIEW_MODES_COUNT)

#define BUTTON_INTERRUPT_HANDLER(last_rising_time, noise_ticks, mask_happened, bit_no) { \
			u32 current_time = time_ticks; \
			if (!(PIND & 1 << (bit_no)) && current_time - last_rising_time >= noise_ticks) \
				events |= mask_happened; \
			else \
				last_rising_time = current_time; \
		}

volatile u32 wheel_ticks = 0;
volatile u32 time_int0 = 0; // time of last rising edge on INT0
/* magnet */
ISR(INT0_vect) {
	BUTTON_INTERRUPT_HANDLER(time_int0, NOISE_TICKS_MAG, EVENT_MAG, PD2);	
	if (events & EVENT_MAG)
		++wheel_ticks;
}

volatile u32 time_int1 = 0; // time of last rising edge on INT1
/* 'show' button */
ISR(INT1_vect) {
	BUTTON_INTERRUPT_HANDLER(time_int1, NOISE_TICKS_BTN, EVENT_BTN, PD3);
}

ISR(TIMER2_OVF_vect) {
	time_ticks++;
	TCNT2 = 0;
}

void init_MCU(void) {
	/* set up asynchronous timer */
	int i;
	for (i = 0; i < 2; ++i) /* wait for oscillator start */ 
		_delay_ms(30);
		
	TCCR2A = 0;
	TCCR2B |= (1 << CS20);	/* overflow time: 1/128s (no prescaler) */
	TCNT2 = 0;				/* reset counter value */
	ASSR |= (1 << AS2);		/* enable async mode */
	TIMSK2 |= (1 << TOIE2); /* enable interrupt on overflow */
	
	/* set up I/O ports */
	DDRD = 0x0;	 /* all inputs */
	DDRB = 0xff; /* all outputs */
	
	/* set up interrupts */
	EICRA |= (1 << ISC10) | (1 << ISC00); /* int0 & int1 requested on any level change */
	EIMSK |= (1 << INT0) | (1 << INT1); /* enable int0 & int1 */
	sei();
}

volatile u32 metric_len = 0;
/* returns in [(1/10 of kilometer)] */
inline u32 wheel_ticks_to_metric() {
	/*  (26 inch * PI) is 2075 millimeters.
	  26 inch - diameter of wheel */
	return (wheel_ticks * 2075) / 100000;
}

volatile u32 time_woke_up = 0;
#define LED_IS_ON (time_woke_up != -1)
#define LED_IS_OFF (time_woke_up == -1)
#define LED_OFF() (time_woke_up = -1)
#define LED_ON() (time_woke_up = time_ticks)

volatile u32 mshow_time = -1;
#define SHOWING_MODE (mshow_time != -1)
#define STOP_SHOWING_MODE() (mshow_time = -1)

void visualize_model() {
	if (LED_IS_OFF || SHOWING_MODE)
		return;
		
	switch (view_mode) {
		case VIEW_MODE_LEN:
			//add_num_draw_task(metric_len & 0xffff, NEXT_TO, TIME_INF);
			push_number(metric_len & 0xffff);
			break;
		default:
			//add_num_draw_task(metric_speed & 0xffff, NEXT_TO, TIME_INF);
			push_number(metric_speed & 0xffff);
			break;
	}
}

void show_mode() {
	switch (view_mode) {
		case VIEW_MODE_LEN:
			//add_str_draw_task(str_mode_len, NEXT_TO, time_ms(300));
			push_three_chars('l', 'e', 'n');
			break;
		default:
			//add_str_draw_task(str_mode_spd, NEXT_TO, time_ms(300));
			push_three_chars('s', 'p', 'd');
			break;
	}
	mshow_time = time_ticks;
}

int main(void) {	
	init_MCU();
	init_indicator();

	u32 old_metric_speed = metric_speed,
		old_metric_len	 = metric_len;
	//PORTB = MYCONST;
	push_three_chars('h', 'e', 'l');
	int i;
	for (i = 0; i < 30; ++i) {
		_delay_ms(3);
	}
	show_mode();
	mshow_time = time_ticks;	
	while (1) {	
		/*if (time_ticks - mshow_time == 1000)
			PORTB |= 1;
		if (time_ticks - time_woke_up == 1000)
			PORTB |= 2;*/
		
		if (SHOWING_MODE) {
			//PORTB ^= 0xff;
			if (time_ticks - mshow_time >= TIME_SHOWING_MODE) {
				STOP_SHOWING_MODE();
				//PORTB ^= 1;
				events |= EVENT_REDRAW;
			}
		}
		
		if (LED_IS_ON) { // if indicator is on
			if (time_ticks - time_woke_up >= TIME_AWAKE) {
				LED_OFF();
				indicator_off();
			}
		}
	 
		/* queue processing */
		if (events & EVENT_MAG) {
			events &= ~EVENT_MAG;
			
			old_metric_len = metric_len;
			metric_len = wheel_ticks_to_metric();
			
			if (metric_len != old_metric_len && view_mode == VIEW_MODE_LEN) 
				events |= EVENT_REDRAW;		
				
			if (wheel_ticks % WHEEL_TICKS_PER_SPEED_MEASUREMENT == 0) {
				speed_measurement_time = time_ticks - last_speed_measurement_time;				
				last_speed_measurement_time = time_ticks;				
				metric_speed = speed_to_metric();
				//PORTB = metric_speed;
				if (view_mode == VIEW_MODE_SPD)
					events |= EVENT_REDRAW;
			}
		}

		if (events & EVENT_BTN) {
			events &= ~EVENT_BTN;		
						
			if (LED_IS_OFF)
				indicator_on();
			else {
				next_view_mode();
			}			
			show_mode();
			LED_ON();
		}

		if (events & EVENT_REDRAW) {	
			//PORTB ^= 1;	
			events &= ~EVENT_REDRAW;
			visualize_model();			
		}
		/* End of VIEW part */
	}
	
	return 0;
}