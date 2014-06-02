#ifndef _COMMU_H_
#define _COMMU_H_
#include "stm32f4lib.h"
void commu_send(u8* buf,u32 lenth);
void commu_recv(u8* buf,u32 lenth);
void commu_Init(void);
u8 commu_send_byte(u8 info);
#endif//_COMMU_H_

