/**
 * USB device controller driver for RT-Thread RTOS
 *
 */
#include <rtthread.h>
#include <stm32f4xx.h>
#include <rtdevice.h>
#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h"


#ifdef RT_USING_USB_DEVICE

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE     USB_OTG_dev __ALIGN_END ;


void rt_hw_usbd_init(void)
{
    rt_kprintf("stm32_dcd_init\n");

    USBD_Init(&USB_OTG_dev,       
            USB_OTG_FS_CORE_ID,       
            &USR_desc,
            &USBD_MSC_cb, 
            &USR_cb);

}

#endif
