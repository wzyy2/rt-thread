/*
 * 程序清单：文件列表视图演示
 *
 * 这个例子会先创建出一个演示用的view，当点击上面的按钮时会按照模式显示的形式显示
 * 新的文件列表视图。
 */
#include "demo_view.h"
#include <rtgui/widgets/label.h>
#include <rtgui/widgets/button.h>
#include <rtgui/widgets/filelist_view.h>

#if defined(RTGUI_USING_DFS_FILERW) || defined(RTGUI_USING_STDIO_FILERW)
/* 创建用于演示文件列表视图的视图 */
rtgui_container_t *demo_fn_view(struct rtgui_application *app)
{
    rtgui_rect_t rect;
    rtgui_container_t *view;
    rtgui_button_t *open_btn;
    rtgui_font_t *font;
    rtgui_filelist_view_t *flist;

    /* 默认采用12字体的显示 */
    font = rtgui_font_refer("asc", 12);

    /* 创建演示用的视图 */
    view = demo_view("FileList View");
    /* 获得演示视图的位置信息 */
    demo_view_get_rect(view, &rect);

    rect.x1 += 5;
    rect.x2 -= 5;
    rect.y1 += 5;
    rect.y2 -= 5;

    flist = rtgui_filelist_view_create("/", "*.*", &rect);
    rtgui_container_add_child(RTGUI_CONTAINER(view), RTGUI_WIDGET(flist));

    return view;
}
#endif
