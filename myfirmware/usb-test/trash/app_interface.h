#ifndef _APP_INTERFACE_H_
#define _APP_INTERFACE_H_
#include "stm32f4lib.h"
#include "debug.h"
#include "rtthread.h"
bool key_capture(u8 *buf);
void key_cap_Init(void);
void press_string_pure(u16 *buf,u32 lenth);
s8 control_key_index(const char buf[]);
extern const u8  ascii2usb[128];
#define LCtrl       0x1
#define LShift     0x2
#define LAlt        0x4
#define LWin      0x8
#define RCtrl      0x10
#define RShift       0x20
#define RAlt        0x40
#define RWin    0x80

#endif//_APP_INTERFACE_H_


