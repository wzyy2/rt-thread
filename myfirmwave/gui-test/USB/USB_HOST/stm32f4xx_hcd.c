/*
 * File      : stm32f4xx_hcd.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2011, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2012-05-16     Yi Qiu      first version
 */

#include <rtthread.h>
#include <rthw.h>
#include <rtdevice.h>
#include "usb_bsp.h"
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_hid_core.h"

#ifdef RT_USING_USB_HOST

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE           USB_OTG_dev __ALIGN_END ;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USBH_HOST                     USB_Host __ALIGN_END ;


void rt_hw_susb_init(void)
{

    USBH_Init(&USB_OTG_dev,//hardware reg&info
              USB_OTG_FS_CORE_ID,       //use FS not HS(enum)
              &USB_Host,//USB state
              &HID_cb,//Class callback structure
              &USR_Callbacks);//User callback structure

    while(1)
    {
        USBH_Process(&USB_OTG_dev , &USB_Host);
//        USB_OTG_BSP_mDelay (10);
        rt_thread_delay(1);
    }
}

#endif

