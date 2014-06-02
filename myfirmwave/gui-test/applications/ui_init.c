#include <rtthread.h>
#include <rtgui/rtgui_server.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/calibration.h>
#include <rtgui/driver.h>

#include <rtgui/rtgui.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/window.h>
#include <rtgui/widgets/notebook.h>
#include <rtgui/widgets/container.h>
#include <rtgui/widgets/window.h>
#include <rtgui/widgets/button.h>
#include <rtgui/widgets/label.h>
#include <rtgui/dc.h>
#include <rtgui/widgets/box.h>

#include <rtthread.h>
#include <board.h>

struct rtgui_win *main_wind;




static int dc_draw(struct rtgui_widget *object)
{
    struct rtgui_rect rect, size;
    struct rtgui_dc * dc;
    rtgui_graphic_driver_get_rect(rtgui_graphic_driver_get_default(), &rect);
    dc = rtgui_dc_begin_drawing(object);
    /* draw English word */
//    RTGUI_DC_FC(dc) = RTGUI_RGB(0xFF, 0, 0);
//    size.x1 = rect.x2/4;
//    size.y1 = rect.y2/4;
//    size.x2 = rect.x2;
//    size.y2 = rect.y2/2;
//    rtgui_dc_draw_text(dc, "hello, world", &size);
    /* draw Chinese word */
    RTGUI_DC_FC(dc) = RTGUI_RGB(0, 0, 0xFF);
    RTGUI_WIDGET_FONT(object) = rtgui_font_refer("hz", 24);
    size.x1 = rect.x2/4;
    size.y1 = rect.y2/2;
    size.x2 = rect.x2;
    size.y2 = rect.y2/4 * 3;
    rtgui_dc_draw_text(dc, "I am Chinese.", &size);
    rtgui_dc_end_drawing(dc);
    return 0;
}

struct rtgui_label* label;
void draw(const char *Data)
{
    rtgui_label_set_text(label,Data); 
}
static char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }
    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }
    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';
        /* Make the value positive. */
        value *= -1;
    }
    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;
        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }
    /* Null terminate the string. */
    *ptr = 0;
    return string;
} /* NCL_Itoa */
void draw_num(int num)
{
    char *temp;
    char buf[24];
    temp = itoa(num, buf, 10); 
    rtgui_label_set_text(label,temp);
}
static rt_bool_t event_handle(struct rtgui_object * object,struct rtgui_event * event)
{
    switch(event->type)
    {
    case RTGUI_EVENT_PAINT:
        rt_kprintf("RTGUI_EVENT_PAINT\n");
        rtgui_win_event_handler((struct rtgui_object*)object, event);
        dc_draw(RTGUI_WIDGET(object));

        return 0;
    default:
        rt_kprintf("other type <%d>\n", object->type);
    }

    return rtgui_win_event_handler(object,event);
}



static void application_entry(void *parameter)
{
    struct rtgui_app *app;
    struct rtgui_rect rect;
    struct rtgui_button* button;
    struct rtgui_widget *widget;
    rtgui_rect_t button_rect = {100, 100, 100, 100};   
    rtgui_rect_t label_rect = {100, 100, 200, 200};      
    
    app = rtgui_app_create("gui_demo");
    if (app == RT_NULL) {
        rt_kprintf("rtgui_app_create failed! \n");
        return;
    }
     //注：(1)
     /* create a full screen window */
     rtgui_graphic_driver_get_rect(rtgui_graphic_driver_get_default(), &rect);

     main_wind = rtgui_win_create(RT_NULL, "demo_win", &rect,
                                 RTGUI_WIN_STYLE_NO_BORDER | RTGUI_WIN_STYLE_NO_TITLE);
    if (main_wind == RT_NULL)
    {
        rt_kprintf("rtgui_win_create failed! \n");
        rtgui_app_destroy(app);
        return;
    } 
    
//    rtgui_image_t* pressed_image;
//    rtgui_image_t* unpressed_image;
//    pressed_image = rtgui_image_create("/programs/button/pressed.bmp",RT_TRUE);
//    unpressed_image = rtgui_image_create("/programs/button/unpressed.bmp",RT_TRUE);
//    rtgui_button_set_pressed_image(button, pressed_image);
//    rtgui_button_set_unpressed_image(button, unpressed_image); 
//    button = rtgui_button_create("ch");
//    rtgui_widget_set_rect(RTGUI_WIDGET(button), &button_rect);
//    //rtgui_button_set_onbutton(button, on_clicked_func); //设置按键回调函数
//    rtgui_container_add_child(RTGUI_CONTAINER(main_wind), RTGUI_WIDGET(button));
//    
    widget = RTGUI_WIDGET(rtgui_label_create("fuck you"));
    label = RTGUI_LABEL(widget);
    rtgui_widget_set_rect(RTGUI_WIDGET(label), &label_rect);
    rtgui_container_add_child(RTGUI_CONTAINER(main_wind), widget);     

    /* 绑定消息处理函数 */
//    rtgui_object_set_event_handler(RTGUI_OBJECT(main_wind), event_handle);

    /* 显示主窗口 */
    rtgui_win_show(main_wind, RT_FALSE);

    /* 循环 */
    rtgui_app_run(app);

}

void app_init()
{
    static rt_bool_t inited = RT_FALSE;

    if (inited == RT_FALSE) /* 避免重复初始化而做的保护 */
    {
        rt_thread_t tid;

        tid = rt_thread_create("wb",
                               application_entry, RT_NULL,
                               2048 * 2, 20, 10);

        if (tid != RT_NULL)
            rt_thread_startup(tid);

        inited = RT_TRUE;
    }
}


int ui_init(void)
{
    rt_device_t device;    

    device = rt_device_find("lcd");    
    if (device == RT_NULL)
    {
		rt_kprintf("no graphic device in the system.\n");
		return -1;
    }    

    /* re-set graphic device */    
    rtgui_graphic_set_device(device); 

		{
//			extern void application_init();
//            application_init();
            app_init();
		}	
	return 0;
}





/*
 * 函数名：USART2_printf
 * 描述  ：格式化输出，类似于C库中的printf，但这里没有用到C库
 * 输入  ：-USARTx 串口通道，这里只用到了串口2，即USART2
 *		     -Data   要发送到串口的内容的指针
 *			   -...    其他参数
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用
 *         典型应用USART2_printf( USART2, "\r\n this is a demo \r\n" );
 *            		 USART2_printf( USART2, "\r\n %d \r\n", i );
 *            		 USART2_printf( USART2, "\r\n %s \r\n", j );
 */
void USART_printf(USART_TypeDef* USARTx, uint8_t *Data,...)
{
	const char *s;
  int d;   
  char buf[16];

  va_list ap;
  va_start(ap, Data);

	while ( *Data != 0)     // 判断是否到达字符串结束符
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //回车符
					USART_SendData(USARTx, 0x0d);
					Data ++;
					break;

				case 'n':							          //换行符
					USART_SendData(USARTx, 0x0a);	
					Data ++;
					break;
				
				default:
					Data ++;
				    break;
			}			 
		}
		else if ( *Data == '%')
		{									  //
			switch ( *++Data )
			{				
				case 's':										  //字符串
					s = va_arg(ap, const char *);
          for ( ; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
          }
					Data++;
          break;

        case 'd':										//十进制
          d = va_arg(ap, int);
          itoa(d, buf, 10);
          for (s = buf; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
          }
					Data++;
          break;
				 default:
						Data++;
				    break;
			}		 
		} /* end of else if */
		else USART_SendData(USARTx, *Data++);
		while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	}
}
/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/
