/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */

#include <stdio.h>
#include <rtthread.h>
#include <board.h>
#include <components.h>
#include <dfs_posix.h>

#ifdef RT_USING_USB_DEVICE
extern void rt_hw_usbd_init(void);
#endif
#ifdef RT_USING_USB_HOST
extern void rt_hw_susb_init(void);
#endif
extern void draw();
extern void draw_num(int num);
extern void rt_platform_init();


void rt_init_thread_entry(void *parameter)
{
    rt_platform_init();
    /* initialization RT-Thread Components */
    rt_components_init();		
    rt_kprintf("thread1 \r\n");
//    rt_thread_delay(RT_TICK_PER_SECOND);

#ifdef RT_USING_RTGUI
	{
		extern int ui_init(void);
		ui_init();
	}
#endif /* RT_USING_RTGUI */	
    
#ifdef RT_USING_USB_DEVICE
    /* usb device controller driver initilize */
    rt_hw_usbd_init();
#endif 
    
#ifdef RT_USING_DFS_ELMFAT    
    if (dfs_mount("flash0", "/", "elm", 0, 0) == 0)  //mkfs("elm","flash0")
    {  
        rt_kprintf("flash0 mount to /.\n");  
    }  
    else  
    rt_kprintf("flash0 mount to / failed.\n");      
#endif    
    
}


void rt_usb_entry(void *parameter)
{
#ifdef RT_USING_USB_HOST
    /* usb device controller driver initilize */
    rt_hw_susb_init();
#endif

}

rt_mq_t mq_commu;
void rt_app_entry(void *parameter)
{
    u8 mail[9];
    while(1)
    {
        rt_mq_recv (mq_commu, mail,9, RT_WAITING_FOREVER);
        draw_num(mail[2]);
        rt_thread_delay(2);
    }
}
int rt_application_init(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("init",
                           rt_init_thread_entry, RT_NULL,
                           4048, 0, 20);
    if (tid != RT_NULL)
        rt_thread_startup(tid);
    
     tid = rt_thread_create("usb",
                           rt_usb_entry, RT_NULL,
                           4048, 10, 20);
    if (tid != RT_NULL)
        rt_thread_startup(tid);
    
    tid = rt_thread_create("app",
                           rt_app_entry, RT_NULL,
                           4048, 30, 20);
    if (tid != RT_NULL)
        rt_thread_startup(tid);
    
    mq_commu=rt_mq_create ("mq_commu", 9, 500, RT_IPC_FLAG_FIFO);

    return 0;
}


void debug()
{
//    /* mount sd card fat partition 1 as root directory */
//	if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
//	{
//	rt_kprintf("File System initialized!\n");//这里可以通过串口显示正常
//    }
//    else
//	rt_kprintf("File System initialzation failed!\n");
//    if (dfs_mount("flash0", "/", "elm", 0, 0) == 0)  //mkfs("elm","flash0")
//    {  
//        rt_kprintf("flash0 mount to /.\n");  
//    }  
//    else  
//    rt_kprintf("flash0 mount to / failed.\n"); 
//    {
//        //文件系统测试代码
//        int fd=0;
//        fd =open("/myfile.txt",DFS_O_CREAT|DFS_O_RDWR,0);
//        if(fd <0)
//        {
//            rt_kprintf("open file failed!\r\n");
//        }
//        else
//        {
//            int count =write(fd,"123456",7);
//            char buf[10];

//            close(fd);
//            fd =0;


//            rt_thread_delay(50);
//            rt_memset(buf,0,10);
//            fd =open("/myfile.txt",DFS_O_RDONLY,0);
//            if(read(fd,buf,7))
//            {
//                rt_kprintf("read=%s\r\n",buf);
//            }
//            else
//            {
//                rt_kprintf("read file err!\r\n");
//            }

//        }
//    }  
}
