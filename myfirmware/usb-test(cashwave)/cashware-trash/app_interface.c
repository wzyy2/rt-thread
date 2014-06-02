#include "app_interface.h"
#include "string.h"
//buf[0]=0;
//buf[1]=pbuf[0];
//buf[2]=pbuf[1];
//buf[3]=pbuf[2];
//buf[4]=pbuf[3];
//buf[5]=pbuf[4];
//buf[6]=pbuf[5];
//buf[7]=pbuf[6];
//buf[8]=pbuf[7];
//rt_mb_send (mb_commu, (rt_uint32_t)buf);
//return true:let key



const u8  ascii2usb[]={
    0,0,0,0,  0,0,0,0,  0,0,0,0,  0,40,0,0, \
    0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0, \
    44,46,0,0,  0,0,0,0,  0,0,0,0,  54,0,55,0, \
    39,30,31,32,  33,34,35,36,  37,38,51,0,  0,0,0,56, \

    31,4,5,6,  7,8,9,10,  11,12,13,14,  15,16,17,18, \
    19,20,21,22,  23,24,25,26,  27,28,29,0,  0,0,0,0, \
    0,4,5,6,  7,8,9,10,  11,12,13,14,  15,16,17,18, \
    19,20,21,22,  23,24,25,26,  27,28,29,0,  50,0,53,0, \
};
const u8  *const p_ascii2usb=ascii2usb;
const u8 shift_table[]={
    0,0,0,0,    0,0,0,0,    0,0,0,0,    0,0,0,0,
    0,0,0,0,    0,0,0,0,    0,0,0,0,    0,0,0,0,
    0,0,0,0,    0,0,0,0,    0,0,0,0,    0,0,0,0,
    0,0,0,0,    0,0,0,0,    0,0,1,0,    0,0,0,1,

    1,1,1,1,    1,1,1,1,    1,1,1,1,    1,1,1,1,
    1,1,1,1,    1,1,1,1,    1,1,1,0,    0,0,0,0,
    0,0,0,0,    0,0,0,0,    0,0,0,0,    0,0,0,0,
    0,0,0,0,    0,0,0,0,    0,0,0,0,    1,0,1,0,

};
#include "ff.h"
static	rt_device_t OLED_dev;

#define min(a,b) (a>b?b:a)
void cmd_line(u8* content);
void cmd(u8* content)
{
    u32 content_char_cnt=0;
    u8 content_line_cnt=0;
    u8 i=0;
    while(content[content_char_cnt]!=0)
    {
        content_char_cnt++;
    }
    content_line_cnt=(content_char_cnt-1)/16+1;
    for(i=0;i<content_line_cnt;i++)
    {
        cmd_line(content+i*16);
    }
}
void cmd_line(u8* content)
{
    u32 i=0;
    u32 content_char_cnt=0;
    //	u32 content_line_cnt;
    static u32 line_cnt=0;
    static u8 OLED_buf[4][16];
    while(content[content_char_cnt]!=0)
    {
        content_char_cnt++;
        if(content_char_cnt==16)
        {
            break;
        }

    }
    if(line_cnt<4)
    {
        u8 j=0;
        for(j=0;j<min(16,content_char_cnt);j++)
        {
            OLED_buf[line_cnt][j]=content[j];
        }
    }
    else
    {
        u8 i=0;
        for(i=0;i<3;i++)
        {
            u8 j=0;
            for(j=0;j<16;j++)
            {
                OLED_buf[i][j]=OLED_buf[i+1][j];
            }
        }

        for(i=0;i<16;i++)
        {
            OLED_buf[3][i]=0;
        }
        for(i=0;i<min(16,content_char_cnt);i++)
        {
            OLED_buf[3][i]=content[i];
        }

    }


    OLED_dev->control(OLED_dev,0,0);
    for(i=0;i<4;i++)
    {
        //		u8 k=0;
        OLED_dev->write(OLED_dev,(i*2),OLED_buf[i],16);
        //		for(k=0;k<16;k++)
        //		DBG("OLED:%d",OLED_buf[i][k]);
    }


    line_cnt++;
}
#define key_string_num 8
#define key_stinrg_max_word 6

const u8 key_string[key_string_num][key_stinrg_max_word]={
    "lctrl",
    "lshift",
    "lalt",
    "lgui",
    "rctrl",
    "rshift",
    "ralt",
    "rgui"};
u8  key_changetable[]={
    0,1,2,3,    4,5,6,7
};
s8 control_key_index(const char buf[])
{
    u8 i=0;
    for(i=0;i<8;i++)
    {
        if(!strcmp((const char *)buf,(const char *)key_string[i]))
            return i;
    }
    return -1;
}
u8 key_process(u8 control_key)
{
    u8 i=0;
    u8 return_key=0;
    for(i=0;i<8;i++)
    {
        if(control_key&(1<<i))
        {
            return_key|=(1<<key_changetable[i]);
        }
    }
    //key_changetable[]
    return return_key;
}
void table_line_process(u8* read_buf,u32* token_ptr)
{
    u32 i=0,j=0;
    u8 index_left;
    u8 index_right;
    for(i=0;i<key_string_num;i++)
    {
        for(j=token_ptr[0];j<token_ptr[1];j++)
        {
            if(read_buf[j]!=key_string[i][j-token_ptr[0]])
            {
                goto fail;
            }

        }
        index_left=i;
        DBG("find_index=%d\r\n",index_left);
        break;
fail:;
    }

    for(i=0;i<key_string_num;i++)
    {
        for(j=token_ptr[1]+1;j<token_ptr[2];j++)
        {
            if(read_buf[j]!=key_string[i][j-token_ptr[1]-1])
            {
                //DBG("alhpa%c!=%c!",read_buf[j],key_string[i][j]);
                goto fail2;
            }

        }
        index_right=i;
        printf("find_index2=%d\r\n",index_right);
        break;
fail2:;
    }
    key_changetable[index_left]=index_right;


}
u32 line_cnt;

u8 key_table_process(u8* read_buf,u32 size)
{
    u32 i=0;
    u32 write_cnt=0;
    u8 read_state=0;
    u32 line_ptr[3];
    for(i=0;i<size;i++)
    {
        char char_this=read_buf[i];
        if(((char_this>='a')&&(char_this<='z'))||((char_this>='A')&&(char_this<='Z'))||char_this=='='||char_this==0x0a)
        {
            if((char_this>='A')&&(char_this<='Z'))
            {
                char_this+=('a'-'A');
            }
            read_buf[write_cnt]=char_this;
            write_cnt++;
        }
        else
        {

        }
    }
    for(i=0;i<write_cnt;i++)
    {
        putchar(read_buf[i]);
    }

    for(i=0;i<=write_cnt;i++)
    {
        //		u8 char_this=read_buf[i];
        //printf("read_state=%d,%c\r\n",read_state,read_buf[i]);
        if(i==write_cnt)
        {
            if(read_state!=2&&read_state!=0)
            {
                cmd("read_state error!");
            }
            else
            {
                if(read_state==2)
                {
                    line_ptr[2]=i;
                    read_state=0;
                    table_line_process(read_buf,line_ptr);
                }
                break;

            }
        }
        switch(read_state)
        {
        case 0:
        {

            if(read_buf[i]!=0x0a)
            {
                line_ptr[0]=i;
                read_state=1;
            }
            //			if(read_buf[i]=='l'||read_buf[i]=='r')
            //			{
            //				cmd("Table file is OK");
            //				read_state=1;
            //			}
            //			else
            //			{
            //				u8 send_buf[20];
            //				cmd("Table file is wrong!");
            //				sprintf(send_buf,"find %c!!",read_buf[write_cnt]);
            //				cmd(send_buf);
            //				goto end;
            //			}
            break;
        }
        case 1:
        {

            if(read_buf[i]=='=')
            {
                read_state=2;
                line_ptr[1]=i;
            }
            break;
        }
        case 2://wait for end
        {

            if(read_buf[i]==0x0a)
            {
                line_ptr[2]=i;
                read_state=0;
                table_line_process(read_buf,line_ptr);
            }
            break;
        }
        }
    }
    //	end:;
    return 0;
}
#define isCtrlKey(a) (a=='#'?true:a=='!'?true:a=='^'?true:a=='+'?true:false)
//void filter_Init(u8* filter,u8 lenth)
//{
//	u8 i=0;
//	for(i=0;i<lenth;i++)
//	{
//		filter[i]=0xff;
//	}
//}
struct control_key_filter;
u8 control_key_decode(u8 key)
{
    switch(key)
    {
    case '#':
        return 1<<3;
    case '!':
        return 1<<2;
    case '^':
        return 1<<0;
    case '+':
        return 1<<1;
    }
    return 0xff;

}
struct control_key_filter
{
    u8 control_filter[8];
    u8 control_filter_cnt;
    u8 key;
};
typedef struct control_key_filter ctrl_filter;
u8 filter_add(struct control_key_filter* filter,u8* filter_info)
{
    u8 key_value=control_key_decode(filter_info[1]);
    switch(filter_info[0])
    {
    case 0:
        filter->control_filter[filter->control_filter_cnt]=key_value|(key_value<<4);
        break;
    case 1:
        filter->control_filter[filter->control_filter_cnt]=key_value;
        break;
    case 2:
        filter->control_filter[filter->control_filter_cnt]=key_value<<4;
        break;

    }
    if(filter->control_filter_cnt<7)
    {
        filter->control_filter_cnt++;
    }
    else
    {
        return 1;
    }
    return 0;
}

enum ctrl_dir{ctrl_left=1,ctrl_right,ctrl_both,ctrl_raw};
struct control_info
{
    enum ctrl_dir dir;
    u8 value;
    u8 state;//0:nothing 1:newest only dir 2:complete
};
typedef struct control_info ctrl_info;
enum block_state{block_raw,contrl_key_gotton,full_quick_key_gotton,colon_gotton};
struct block_information
{
    ctrl_filter filter;
    u8 state;//0:nothing(not init) 1:inited 2:find ctrl key 3:in '::'
};
typedef struct block_information block_info;
enum token_class{token_raw,
                 token_both_dir_ctrl,
                 token_dir_ctrl,
                 token_colon,
                 token_key,
                 token_long_key
                };
enum token_state{ts_raw,//wait for anything
                 ts_wait_ctrl_key,//wait for ctrl key
                 ts_wait_2nd_colon,
                 ts_wait_key_value,
                 ts_finish};
struct token_struct
{
    enum token_class token_class;
    u32 content;
    enum token_state state;
};
typedef struct token_struct token;
void token_Init(token* token_this)
{
    token_this->content=0;
    token_this->token_class=token_raw;
    token_this->state=ts_raw;
}

struct token_queue_struct
{
    token queue[1024];//change when needed
    u8 state;
    u8 lenth;

};
typedef struct token_queue_struct token_q;
void token_q_Init(token_q* q)
{
    u16 i=0;
    for(i=0;i<1024;i++)
    {
        token_Init(&q->queue[i]);
    }
    q->state=0;
    q->lenth=0;
}
void ctrl_info_Init(ctrl_info* info)
{
    info->dir=ctrl_raw;
    info->value=0;
    info->state=0;
}
void filter_Init(ctrl_filter* filter)
{
    u8 i=0;
    for(i=0;i<8;i++)
    {
        filter->control_filter[i]=0;
    }
    filter->control_filter_cnt=0;
    filter->key=0;

}
void block_Init(block_info* block)
{
    block->state=block_raw;
    filter_Init(&(block->filter));
}
enum char_class{
    angle,//<>
    ctrl_key,
    colon,
    general_key,
    brace
};
enum char_class char_ana(u8 char_this)
{
    if(char_this=='<'||char_this=='>')
    {
        return angle;
    }
    else if(char_this=='#'||char_this=='!'||char_this=='^'||char_this=='+')
    {
        return ctrl_key;
    }
    else if(char_this==':')
    {
        return colon;
    }
    else if(char_this=='{'||char_this=='}')
    {
        return brace;
    }
    else
        return general_key;

}
typedef enum token_state ts;
const u8 general_key_value[]={0,
                              40,41,42,43,
                              44,57,58,59,
                              60,61,62,63,
                              64,65,66,67,

                              68,69,70,71,
                              72,73,74,75,
                              76,77,78,79,
                              80,81,82,83
                             };
const u8* general_key_map[]={"",
                             "enter",            "esc",      "delete",       "tab",
                             "space",            "caps lock","f1",           "f2",
                             "f3",               "f4",       "f5",           "f6",
                             "f7",               "f8",       "f9",           "f10",

                             "f11",              "f12",      "printscreen",  "scroll lock",
                             "pause",            "intert",   "home",         "pageup",
                             "deleteforward",   "end",      "pagedown",     "right",
                             "left",             "down",     "up",           "num lock"
                            };
u8 general_key_ana(u8* read_buf,u32 general_key_pos,u8 general_key_cnt)
{
    u8 string[21];
    u8 i=0;
    for(i=0;i<general_key_cnt;i++)
    {
        string[i]=read_buf[general_key_pos+i];
    }
    string[general_key_cnt]='\0';
    //printf("----%s----",string);
    for(i=0;i<sizeof(general_key_map);i++)
    {
        if(strcmp((const char*)string,(const char*)general_key_map[i])==0)
        {
            return general_key_value[i];
        }
    }
    return 0;

}
u8 token_ana(token_q* token_queue,u8* read_buf,u32 ptr[2])
{
    //  u8* s;
    u32 i=0;
    u8 general_key_cnt=0;
    u32 general_key_pos=0;

    // enum token_state ts=ts_raw;

    token_q_Init(token_queue);
    for(i=ptr[0];i<ptr[1];i++)
    {
        u8 char_this=read_buf[i];
        token* token_this=&(token_queue->queue[token_queue->lenth]);
#ifdef ANA_DBG
        DBG("(%c->%d)\r\n",char_this,token_this->state);
#endif
        switch(token_this->state)
        {
        case ts_raw:
            switch(char_ana(char_this))
            {
            case angle:
                token_this->state=ts_wait_ctrl_key;
                if(char_this=='<')
                    token_this->content|=(1<<7);
                break;
            case ctrl_key:
                token_queue->lenth++;
                token_this->state=ts_finish;
                token_this->content=char_this;
                token_this->token_class=token_both_dir_ctrl;
                break;
            case colon:
                token_this->state=ts_wait_2nd_colon;
                break;
            case general_key:
                token_queue->lenth++;
                token_this->state=ts_finish;
                token_this->content=char_this;
                token_this->token_class=token_key;
                break;
            case brace:
                token_this->state=ts_wait_key_value;
                break;
            default:
                DBG("error:line%d---unknown char type found!\r\n",line_cnt);
                return 1;

            }
            break;
        case ts_wait_ctrl_key:
            if(char_ana(char_this)==ctrl_key)
            {
                token_queue->lenth++;
                token_this->state=ts_finish;
                token_this->content|=char_this;
                token_this->token_class=token_dir_ctrl;
            }
            else
            {
                DBG("error:line%d---wait for ctrl_key but found%c\r\n",line_cnt,char_this);
                return 2;
            }
            break;
        case ts_wait_2nd_colon:
            if(char_ana(char_this)==colon)
            {
                token_queue->lenth++;
                token_this->state=ts_finish;
                token_this->content=char_this;
                token_this->token_class=token_colon;
            }
            else
            {
                DBG("error:line%d---wait for colon but found%c\r\n",line_cnt,char_this);
                return 3;
            }
            break;
        case ts_wait_key_value:
            if(char_ana(char_this)==general_key)
            {
                if(general_key_cnt==0)
                {
                    general_key_pos=i;
                }
                if(general_key_cnt<20)
                {
                    general_key_cnt++;
                }
                else
                {
                    DBG("error:line%d---unknown general key(too long)%c\r\n",line_cnt,char_this);
                    return 4;
                }



            }
            else if(char_ana(char_this)==brace)
            {
                u8 general_key_index;

                general_key_index=general_key_ana(read_buf,general_key_pos,general_key_cnt);
                general_key_cnt=0;
                general_key_pos=0;
                if(general_key_index==0)
                {
                    DBG("error:line%d---unknown general key%c\r\n",line_cnt,char_this);
                    return 5;
                }
                else
                {
                    token_queue->lenth++;
                    token_this->state=ts_finish;
                    token_this->content=general_key_index;
                    token_this->token_class=token_long_key;
                }

            }
            else
            {
                DBG("error:line%d---unknown word found when wait for general key%c\r\n",line_cnt,char_this);
                return 6;
            }
            break;

        }

        // printf("ana=%c(%d)",char_this,char_ana(char_this));
    }
    if(token_queue->queue[token_queue->lenth].state==token_raw)
        return 0;
    else
    {
        DBG("error:line%d---unexpected end",line_cnt);
        return 7;
    }
}
#define compile_DBG(a) do{DBG("error:line%d---",line_cnt);DBG(a);DBG("\r\n");}while(0)
u16 token2usb(token* token_get)
{
    if(token_get->token_class==token_key)
    {
        return (shift_table[token_get->content]<<8)|ascii2usb[token_get->content];
    }
    else //if(token_get->token_class==token_long_key)
    {
        return token_get->content;
    }
}
struct st_key_cap{
    ctrl_filter filter;
    void (*key_exe)(struct st_key_cap*);
    //  st_key_cap* pre;
    struct st_key_cap* next;
    u16 *string;
    u16 string_lenth;
};
typedef struct st_key_cap cap;
void press_string(cap * cap_this);
void  key_cap_add(cap* cap_this);
u8 mode_line_process(u8* read_buf,u32 ptr[2])
{
    u32 i=0;
//    static u8 ana_state=0;// 0:find quick key 1:find"::" 2:?
    //static u8 control_info[2];//[0]:1:< 2:> 0:both dir  [1]:whitch key
//    static u8 control_key_state=0;//1:dir word found,wait for key  0:wait for dir or key
//    ctrl_info info_temp;
    static block_info block;
    static token_q token_queue;
    cap cap_this;

//                   0,
//                   press_string,
//                   RT_NULL,
//                   my_id,
//                   sizeof(my_id)};
//    u8 filter_cnt=0;

    //DBG("line_cnt=%d~\r\n",line_cnt);
    if(read_buf[ptr[0]]==';')
    {
        line_cnt++;
        return 0;
    }

    if(token_ana(&token_queue,read_buf,ptr))
        return 1;
  //  DBG("ANASUCCESS");
  #undef ANA_DEBUG
#ifdef ANA_DEBUG
    for(i=0;i<token_queue.lenth;i++)
    {
        printf("queue[%d],class=",i);
        switch(token_queue.queue[i].token_class)
        {
        case token_raw:
            printf("token_raw");
            break;
        case token_both_dir_ctrl:
            printf("token_both_dir_ctrl");
            break;
        case token_dir_ctrl:
            printf("token_dir_ctrl");
            break;
        case token_colon:
            printf("token_colon");
            break;
        case token_key:
            printf("token_key");
            break;
        }
        printf(",content=%d\r\n",token_queue.queue[i].content);
    }
#endif
    //    enum token_class{token_raw,
    //                     token_both_dir_ctrl,
    //                     token_dir_ctrl,
    //                     token_colon,
    //                     token_key};
    // enum block_state{raw,contrl_key_gotton,full_quick_key_gotton,colon_gotton};
    for(i=0;i<token_queue.lenth;i++)
    {
        token* token_this=&(token_queue.queue[i]);
        enum token_class class_this=token_this->token_class;
        switch(block.state)
        {
        case block_raw:
            block_Init(&block);
        case contrl_key_gotton:
            if(class_this==token_both_dir_ctrl)
            {
                block.filter.control_filter[block.filter.control_filter_cnt++]=
                        control_key_decode(token_this->content)|
                        (control_key_decode(token_this->content)<<4);
                DBG("block_raw\r\n");
                block.state=contrl_key_gotton;
            }
            else if(class_this==token_dir_ctrl)
            {
                if(token_this->content&0x80)
                {
                    block.filter.control_filter[block.filter.control_filter_cnt++]=
                            control_key_decode(token_this->content&(0x7f));
                }
                else
                {
                    block.filter.control_filter[block.filter.control_filter_cnt++]=
                            control_key_decode(token_this->content&(0x7f))<<4;
                }
                //DBG("token_dir_ctrl\r\n");
                block.state=contrl_key_gotton;
            }
            else if(block.state==block_raw)
            {
                compile_DBG("Control key not found!");
            }
            else if(class_this==token_key||class_this==token_long_key)
            {
                block.filter.key=token2usb(token_this);
                block.state=full_quick_key_gotton;
            }
            else
            {
                compile_DBG("Unexpected token_class found when make control key!");
            }
            break;
        case full_quick_key_gotton:
            if(class_this!=token_colon)
            {
                compile_DBG("Didn't find colon!");
            }
            else
            {
                block.state=colon_gotton;
            }
            break;
        case colon_gotton:
        {
            u32 k=0;
            u32 key_cnt=0;
            u16* key_array;
            for(k=i;k<token_queue.lenth;k++)
            {
                token* token_this=&(token_queue.queue[k]);
                enum token_class class_this=token_this->token_class;
                if(class_this==token_key||class_this==token_long_key)
                {
                    key_cnt++;
                }
                else
                {
                    compile_DBG("Unexpected token_class found when get general key!");
                    return 1;
                }
            }
            //DBG("line%d,%d general key gotton!\r\n",line_cnt,key_cnt);
            key_array=rt_malloc(key_cnt<<1);
            for(k=i;k<token_queue.lenth;k++)
            {
                key_array[k-i]=token2usb(&token_queue.queue[k]);
            }

            cap_this.filter=block.filter;
            cap_this.key_exe=press_string;
            cap_this.string=key_array;
            cap_this.string_lenth=key_cnt;
            key_cap_add(&cap_this);

            block.state=block_raw;
            goto end;
         
        }



        }
    }
end:;


    line_cnt++;

 
//begin:

//    ctrl_info_Init(&info_temp);

//    {
//        switch(block.state)
//        {
//        case 0:
//            block_Init(&block);
//            block.state=1;

//            break;
//        case 1:
//		break;

//        }
//    }


    //	for(i=ptr[0];i<ptr[1];i++)
    //	{
    //		u8 char_this=read_buf[i];
    //		putchar(read_buf[i]);
    //		switch(ana_state)
    //		{
    //		case 0://find quick key
    //		{
    //			switch(control_key_state)
    //			{
    //			case 0:
    //			{
    //				if(char_this=='>')
    //				{
    //					control_info[0]=2;
    //					control_key_state=1;
    //				}
    //				else if(char_this=='<')
    //				{
    //					control_info[0]=1;
    //					control_key_state=1;
    //				}
    //				else if(isCtrlKey(char_this))
    //				{
    //					control_info[0]=0;
    //					control_key_state=0;
    //					control_info[1]=char_this;
    //					if(filter_add(&filter_temp,control_info))
    //					{
    //						printf("error:too lot control key");
    //					}
    //				}
    //				else
    //				{

    //					DBG("error:no control key");
    //					break;
    //				}
    //				break;
    //			}
    //			case 1:
    //			{

    //				break;
    //			}
    //			case 2:
    //			{
    //				break;
    //			}
    //			}


    //			printf("control_info[0]=%d\r\n",control_info[0]);
    //			break;
    //		}
    //		case 1://find which control key
    //		{
    //			break;
    //		}
    //		case 2:
    //		{
    //			break;
    //		}
    //		}
    //	}
    //if(ana_state!=?)


    //printf("\r\n");
	return 0;
}
u8 key_mode_process(u8* read_buf,u32 size)
{
    u32 i=0;
    u32 write_cnt=0;
    u8 read_state=0;
    u32 line_ptr[2];

    for(i=0;i<size;i++)
    {
        char char_this=read_buf[i];
        if(char_this!=0x0d&&char_this!=' '&&char_this!='\t')
        {
            if((char_this>='A')&&(char_this<='Z'))
            {
                char_this+=('a'-'A');
            }
            read_buf[write_cnt]=char_this;
            write_cnt++;
        }
        else
        {

        }
    }


    //	for(i=0;i<write_cnt;i++)
    //	{
    //		putchar(read_buf[i]);
    //	}

    for(i=0;i<=write_cnt;i++)
    {
        //		u8 char_this=read_buf[i];
        //printf("read_state=%d,%c\r\n",read_state,read_buf[i]);
        if(i==write_cnt)
        {
            if(read_state!=2&&read_state!=0)
            {
                cmd("read_state error!");
                break;
            }
            else
            {
                if(read_state==2)
                {
                    line_ptr[1]=i;
                    mode_line_process(read_buf,line_ptr);
                }
                break;
            }
        }
        switch(read_state)
        {
        case 0:
        {

            if(read_buf[i]!=0x0a)
            {
                line_ptr[0]=i;
                read_state=1;
            }
            //			if(read_buf[i]=='l'||read_buf[i]=='r')
            //			{
            //				cmd("Table file is OK");
            //				read_state=1;
            //			}
            //			else
            //			{
            //				u8 send_buf[20];
            //				cmd("Table file is wrong!");
            //				sprintf(send_buf,"find %c!!",read_buf[write_cnt]);
            //				cmd(send_buf);
            //				goto end;
            //			}
            break;
        }
        case 1:
        {

            if(read_buf[i]==0x0a)
            {
                read_state=0;
                line_ptr[1]=i;
                mode_line_process(read_buf,line_ptr);
            }

            break;
        }
        }
    }
    return 0;
}
extern rt_mq_t mq_commu;
ALIGN(RT_ALIGN_SIZE)
char thread_app_stack[4096];
struct rt_thread thread_app;
u8 read_buf[9000];
extern rt_sem_t sem_flash;
extern u8 blue_choose;
void blue_set(cap * cap_this)
{
    u8 buf[9];
    u8 i=0;
    buf[0]=10;
    for(i=1;i<9;i++)
    {
        buf[i]=0;
    }
    rt_mq_send(mq_commu,(void *)buf,9);
    blue_choose=1-blue_choose;
    if(blue_choose)
        cmd("Blue remote");
    else
    {
        cmd("Blue closed");
    }
}
void rt_thread_entry_app(void* parameter)
{
    static FATFS fs;  		//逻辑磁盘工作区.
    static FIL file;	  		//文件1
    u8 key_temp[2];
    cap  cap_this2;
    ctrl_filter filter;
    block_info block;
    //	static FIL ftemp;	  		//文件2.
    //	static UINT br,bw;			//读写变量
    //	static FILINFO fileinfo;	//文件信息
    //	static DIR dir;

    u32 cnt=0;
    u32 i=0;


    key_cap_Init();
    OLED_dev=rt_device_find("OLED");

    if(OLED_dev!=RT_NULL)
    {
        rt_kprintf("OLED dev found\n\r");
        //    rt_device_write(OLED_dev,0,&led_value,1);
        //  led_value++;
    }
    cmd("Initializing~");
	rt_device_open(OLED_dev,RT_DEVICE_FLAG_ACTIVATED);

    rt_sem_take(sem_flash,RT_WAITING_FOREVER);
    f_mount(&fs,"/",1);
    f_mkdir("/sks");
    //	f_opendir(&dir,"/");

    printf("open:%d",f_open(&file,"/key_t",FA_OPEN_EXISTING|FA_WRITE|FA_READ|FA_OPEN_ALWAYS|FA__WRITTEN));
    printf("read:%d",f_read(&file,read_buf,file.fsize,&cnt));
    printf("size=%d\r\n",file.fsize);
    for(i=0;i<file.fsize;i++)
    {
        putchar(read_buf[i]);
    }
    f_close(&file);
    
    if(key_table_process(read_buf,file.fsize))
    {
        cmd("Key table error");
    }
    else
    {
        cmd("Key table done");
    }

    printf("open:%d",f_open(&file,"/mode_1",FA_OPEN_EXISTING|FA_WRITE|FA_READ|FA_OPEN_ALWAYS|FA__WRITTEN));
    printf("read:%d",f_read(&file,read_buf,file.fsize,&cnt));
    printf("size=%d\r\n",file.fsize);
    rt_sem_release(sem_flash);
    for(i=0;i<file.fsize;i++)
    {
        putchar(read_buf[i]);
    }
    f_close(&file);

    if(key_mode_process(read_buf,file.fsize))
    {
        cmd("Key mode error");
    }
    else
    {
        cmd("Key mode done");
    }
   // rt_kprintf("cap cnt:%d\r\n",);
    cmd("Done");
    
    key_temp[0]=0;
    key_temp[1]='^';
    filter_Init(&filter);
    filter_add(&filter,key_temp);
    filter.key=53;
    block.filter=filter;
    cap_this2.filter=block.filter;
    cap_this2.key_exe=blue_set;
    key_cap_add(&cap_this2);
    
    key_temp[0]=0;
    key_temp[1]='!';
    filter_Init(&filter);
    filter_add(&filter,key_temp);
    filter.key=53;
    block.filter=filter;
    cap_this2.filter=block.filter;
    cap_this2.key_exe=blue_set;
    key_cap_add(&cap_this2);
}


















u8 buf_out[9];
extern rt_mq_t mq_commu;
void buf_clear(void);
extern rt_mailbox_t mb_commu;
#define press  rt_mq_send(mq_commu,buf_out,9) 

#define key_cap_cnt_all 200
cap key_cap_free[key_cap_cnt_all];
//st_key_cap key_cap_use[200];
cap* cap_use_head=RT_NULL;
cap* cap_free_head=key_cap_free;
rt_uint16_t key_cap_cnt=0;
void key_cap_Init()
{
    u16 i=0;
    for(i=0;i<key_cap_cnt_all-1;i++)
    {
        key_cap_free[i].next=&key_cap_free[i+1];
    }
    key_cap_free[key_cap_cnt-1].next=RT_NULL;
}
u8 my_id[]="qk333333";
u8 my_mail[]="qk333student@sogou.com";
u8 my_nickname[]="blackmiaool";


//void my_id_Init()
//{
//    cap cap_my_id={LShift,
//                   0,
//                   press_string,
//                   RT_NULL,
//                   my_id,
//                   sizeof(my_id)};
//    cap cap_my_id2={LShift,
//                    0,
//                    press_string,
//                    RT_NULL,
//                    my_mail,
//                    sizeof(my_mail)};
//    cap cap_my_id3={LShift,
//                    0,
//                    press_string,
//                    RT_NULL,
//                    my_nickname,
//                    sizeof(my_nickname)};

//    cap_my_id.char_key=ascii2usb['a'];
//    key_cap_add(&cap_my_id);


//    cap_my_id2.char_key=ascii2usb['b'];
//    key_cap_add(&cap_my_id2);

//    cap_my_id3.char_key=ascii2usb['c'];
//    key_cap_add(&cap_my_id3);
//}
//buf 0~7:USBcode 8:if press LShift 9:control_key add 10:control_key del
void press_string_pure(u16 *buf,u32 lenth)
{
    u32 i=0;
    u8 control_key=0;
    buf_clear();
    buf_out[1]=0;
    for(i=0;i<lenth;i+=1)
    {
        
        if(buf[i]&(1<<9))
        {
            control_key|=buf[i];
            buf_out[1]=control_key;
            press;
        }
        else if(buf[i]&(1<<10))
        {
            control_key&=(~buf[i]);
            buf_out[1]=control_key;
            press;
        }
        else if(buf[i]&(1<<8))
        {
            buf_out[1]=LShift;
            buf_out[3]=buf[i];
            press;
            buf_out[1]=0;
            
        }
        else
        {
            buf_out[3]=buf[i];
            buf_out[1]=control_key;
            press;
        }
        
        
        //printf("press%d\r\n",buf_out[3]);
        if(i!=lenth-1)
        buf_clear();
    }
}
void press_string(cap * cap_this)
{
    press_string_pure(cap_this->string,cap_this->string_lenth); 
}

void  key_cap_add(cap* cap_this)
{
    u16 i=0;
    cap * cap_new;
    //DBG("++++++++++");
    if(cap_free_head->next==RT_NULL||key_cap_cnt==key_cap_cnt_all)
    {
        return ;
    }
    else
    {
        cap_new=cap_free_head;
        cap_free_head=cap_free_head->next;
    }

    cap_new->filter=cap_this->filter;
    cap_new->key_exe=cap_this->key_exe;
    cap_new->string=cap_this->string;
    cap_new->string_lenth=cap_this->string_lenth;

    if(key_cap_cnt==0)
    {
        cap_use_head=cap_new;
    }
    else
    {
        cap** cap_current=&cap_use_head;
        for(i=0;i<key_cap_cnt;i++)
        {
            cap_current=&((*cap_current)->next);
        }
        *cap_current=cap_new;
    }
    
    key_cap_cnt++;

}
void key_cap_del(u16 index)
{

}



//const u8 start_slogan[]="Hello,my dear master~\r\n"  \
//    "What do you want to do?\r\n"                             \
//    "a:add new string \r\n" \
//    "b:change hotkey \r\n" \
//    ;

void buf_clear()
{
    u8 i=0;
    for(i=2;i<9;i++)
    {
        buf_out[i]=0;
    }
    buf_out[0]=6;
    press;

}
extern  const  int8_t HID_KEYBRD_Key[];
extern const  uint8_t  HID_KEYBRD_Codes[];


u8 control_key_filt(u8 key,ctrl_filter* filter)
{
    u8 i=0;
    for(i=0;i<filter->control_filter_cnt;i++)
    {
       // printf("filter%d",filter->control_filter[i]);
        if(key&filter->control_filter[i])
        {
            key&=(~filter->control_filter[i]);
        }
        else
            return 1;
    }
    if(key==0)
        return 0;
    else
        return 1;
}
bool key_capture(u8 *buf)
{
    u8 i=0;
    static  u8 hotkey_flag=0;
    static  u8 hotkey_value=0;
    DBG("CNT=%d",key_cap_cnt);
    //    for(i=0;i<8;i++)
    //    {
    //        DBG("%d\r\n",buf[i]);
    //    }
    //    DBG("%c",HID_KEYBRD_Key[HID_KEYBRD_Codes[buf[2]]]);
    //    if(buf[0]&(1<<6))
    //    {
    //        buf[0]&=(0xff-(1<<6));W
    //        buf[0]|=(1<<4) ;
    //    }
    //printf("buf[0]_raw=%d\r\n",buf[0]);
    buf[0]=key_process(buf[0]);//TODO
    if(hotkey_flag)
    {

        if(buf[2]==hotkey_value)
        {
            DBG("I block it!\r\n");

            return false;
        }
        else
        {
            hotkey_flag=0;
        }
    }
    for(i=0;i<key_cap_cnt;i++)
    {
        cap* cap_this;

        if(i==0)
        {
            cap_this=cap_use_head;
        }
        else
            cap_this=cap_this->next;


        if(control_key_filt(buf[0],&(cap_this->filter))==0)
        {
            DBG("(%d----%d)",key_cap_cnt,cap_this->filter.key);
            if( buf[2]==cap_this->filter.key)
            {
                DBG("I got it!\r\n");
                (cap_this->key_exe)(cap_this);
                hotkey_flag=1;
                hotkey_value=buf[2];

                return false;
            }
        }
    }

    return true;
}
void Down_Up(u8 func_key,u8 ascii_key)
{

}
