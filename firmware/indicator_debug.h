#ifndef INDICATOR_DEBUG_H
#define INDICATOR_DEBUG_H

#include "definitions.h"

extern void push_number(u16);
extern void push_single_char(char);
extern void push_three_chars(char left, char middle, char right);

extern void indicator_on(void);
extern void indicator_off(void);

#endif