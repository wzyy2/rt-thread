#include "stm32f4xx.h"
#include <board.h>
#include <rtthread.h>
#include "inifile.h"

#define BUF_SIZE 256\

void rt_app_entry(void *parameter)
{
    const char *file ="profile.ini";//这里也可以改成“myconfig,txt”
    const char *section = "Service";
    const char *key = "Name";
    char rec[BUF_SIZE]={0};

//    while (USART_GetFlagStatus(USART6, USART_FLAG_RXNE) == RESET);
//        rt_kprintf("fuck\n");  
//    while(1)
//    {
//        rt_thread_delay(2);
//    }
    
#ifdef USE_INI //make sure you have profile.ini
//    if(!write_profile_string(section,key,"Tony",file))
//    {
//        rt_kprintf("write name pair to ini file fail\n");
//    }
    if(!read_profile_string(section, key, rec, BUF_SIZE,"",file))
    {
        rt_kprintf("read ini file fail\n");
    }
    else
    {
        rt_kprintf("%s=%s\n",key,rec);
    }
#endif
    
}


void Jacob_appinit()
{
    rt_thread_t tid;

    tid = rt_thread_create("jacob",
                           rt_app_entry, RT_NULL,
                           8048, 20, 20);
    
    
    if (tid != RT_NULL)
        rt_thread_startup(tid);
}
