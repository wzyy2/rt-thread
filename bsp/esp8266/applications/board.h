#ifndef __BOARD_H__
#define __BOARD_H__

#include "esp8266.h"

extern char   _heap_start;

#define HEAP_BEGIN &_heap_start
#define HEAP_END 0x3FFFBFFF

void rt_application_init();
void rt_hw_board_init(void);

#endif
