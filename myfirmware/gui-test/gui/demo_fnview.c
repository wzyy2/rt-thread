/*
 * �����嵥���ļ��б���ͼ��ʾ
 *
 * ������ӻ��ȴ�����һ����ʾ�õ�view�����������İ�ťʱ�ᰴ��ģʽ��ʾ����ʽ��ʾ
 * �µ��ļ��б���ͼ��
 */
#include "demo_view.h"
#include <rtgui/widgets/label.h>
#include <rtgui/widgets/button.h>
#include <rtgui/widgets/filelist_view.h>

#if defined(RTGUI_USING_DFS_FILERW) || defined(RTGUI_USING_STDIO_FILERW)
/* ����������ʾ�ļ��б���ͼ����ͼ */
rtgui_container_t *demo_fn_view(struct rtgui_application *app)
{
    rtgui_rect_t rect;
    rtgui_container_t *view;
    rtgui_button_t *open_btn;
    rtgui_font_t *font;
    rtgui_filelist_view_t *flist;

    /* Ĭ�ϲ���12�������ʾ */
    font = rtgui_font_refer("asc", 12);

    /* ������ʾ�õ���ͼ */
    view = demo_view("FileList View");
    /* �����ʾ��ͼ��λ����Ϣ */
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
