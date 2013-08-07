#ifndef DRAW_QUEUE_C
#define DRAW_QUEUE_C

#include "settings.h"
#include "definitions.h"
#include "indicator_debug.h"

struct task_s {
	union {
		u16 number;
		char[3] chars;
	} action;
	u8 action_type;
	u32 delay;
};
typedef struct task_s task;

void add_drawing_task(task t) {

}

void draw() {

}

#endif