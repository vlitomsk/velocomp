#ifndef INDICATOR_DEBUG_C
#define INDICATOR_DEBUG_C

#include <avr/io.h>
#include <util/delay.h>
#include "indicator_debug.h"

#define bit_on(bit_no) (CTLPORT |= (1 << bit_no))
#define bit_off(bit_no) (CTLPORT &= ~(1 << bit_no))

/*#define indicator_on() (bit_on(ON))
#define indicator_off() (bit_off(ON))*/

u8 digit1[30] = {
	0b01110111, // 0
	0b01000001, // 1
	0b00111011, // 2
	0b01101011, // 3
	0b01001101, // 4
	0b01101110, // 5
	0b01111110, // 6
	0b01000011, // 7
	0b01111111, // 8
	0b01101111, // 9
	0b10000000, // .
	0b01011111, // a
	0b01111100, // b
	0b00111000, // c
	0b01111001, // d
	0b00111111, // e
	0b00011110, // f
	0b01111110, // g
	0b01011100, // h
	0b01000000, // i
	0b01100001, // j
	0b01011100, // k
	0b00110100, // l
	0b01010111, // m
	0b01011000, // n
	0b01111000, // o
	0b00011111, // p
	0b01001111, // q
	0b00011000, // r
	0b01101110  // s
};

u8 digit2[30] = {
	0b01111011, // 0
	0b01000001, // 1
	0b00110111, // 2
	0b01100111, // 3
	0b01001101, // 4
	0b01101110, // 5
	0b01111110, // 6
	0b01000011, // 7
	0b01111111, // 8
	0b01101111, // 9
	0b10000000, // .
	0b01011111, // a
	0b01111100, // b
	0b00110100, // c
	0b01110101, // d
	0b00111111, // e
	0b00011110, // f
	0b01111110, // g
	0b01011100, // h
	0b01000000, // i
	0b01100001, // j
	0b01011100, // k
	0b00111000, // l
	0b01011011, // m
	0b01010100, // n
	0b01110100, // o
	0b00011111, // p
	0b01001111, // q
	0b00010100, // r
	0b01101110  // s
};

u8 digit3[30] = {
	0b10110111, // 0
	0b10000001, // 1
	0b01110011, // 2
	0b11100011, // 3
	0b11000101, // 4
	0b11100110, // 5
	0b11110110, // 6
	0b10000011, // 7
	0b11110111, // 8
	0b11100111, // 9
	0b11010111, // a
	0b11110100, // b
	0b01110000, // c
	0b11110001, // d
	0b01110111, // e
	0b01010110, // f
	0b11110110, // g
	0b11010100, // h
	0b10000000, // i
	0b10100001, // j
	0b11010100, // k
	0b00110100, // l
	0b10010111, // m
	0b11010000, // n
	0b11110000, // o
	0b01010111, // p
	0b11000111, // q
	0b01010000, // r
	0b11100110 // s
};

u16 last_indicator_state = 0;

void push_bit(u8 bit) {
	if (bit) {
		bit_on(DATA);
	} else {
		bit_off(DATA);
	}

	bit_off(CLK);
	_delay_us(2);
	bit_on(CLK);
}

/* do not use - use push_triplet or push_number */
void push_byte(u8 byte) {
	push_bit(byte & 0x80);
	push_bit(byte & 0x40);
	push_bit(byte & 0x20);
	push_bit(byte & 0x10);
	push_bit(byte & 0x8);
	push_bit(byte & 0x4);
	push_bit(byte & 0x2);
	push_bit(byte & 0x1);
}

void reset_sreg() {
	bit_off(RST);
	_delay_us(3);
	bit_on(RST);
}

void push_triplet(u8 first, u8 second, u8 third) {
	reset_sreg();
	push_byte(third);
	push_byte(second);
	push_byte(first);
}

void push_number(u16 number) {
	reset_sreg();
	push_byte(digit3[number % 10]);
	number /= 10;
	push_byte(digit2[number % 10]);
	number /= 10;
	push_byte(digit1[number % 10]);
	last_indicator_state = number;
}

void push_single_char(char ch) {
	reset_sreg();
	push_byte(0);
	push_byte(0);
	if (ch == 's' || ch == 'S') {
		push_byte(digit1[11]);
	} else if (ch == 'l' || ch == 'L') {
		push_byte(digit1[12]);
	}	
}

// c1 - left, c2 - middle, c3 - right position
void push_three_chars(char c1, char c2, char c3) {
	push_triplet(digit1[c1 - 86], digit2[c2 - 86], digit3[c3 - 87]);
}

void init_indicator() {
	CTLPORTDDR |= (1 << CLK) | (1 << DATA) | (1 << RST ) | (1 << ON);
	//CTLPORT &= (~(1 << CLK)) & (~(1 << DATA)) & (~(1 << RST)) & (~(1 << ON));
	CTLPORT = 0;
}

void indicator_on() {
	push_number(last_indicator_state);
}

void indicator_off() {
	push_byte(0);
	push_byte(0);
	push_byte(0);
}

#endif
