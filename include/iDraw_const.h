#pragma once

// 当前MDIClient窗口下显示的窗口是什么类型
enum MDICLIENT_TYPE : int
{
    MDICLIENT_TYPE_NONE     = -1,   // 没有窗口
    MDICLIENT_TYPE_CODE     = 0,    // 代码窗口
    MDICLIENT_TYPE_IDE      = 1,    // ide设计器, 窗口设计器
    MDICLIENT_TYPE_HONE     = 2,    // 起始页
};

// 当前代码编辑窗口是什么类型
enum MDICLIENT_CODETYPE : int
{
    MDICLIENT_CODETYPE_UNDEFINE                 = 0,   // 未定义
    MDICLIENT_CODETYPE_WINDOW                   = 1,   // 窗口设计器
    MDICLIENT_CODETYPE_ASSEMBLY                 = 2,   // 普通程序集
    MDICLIENT_CODETYPE_ASSEMBLY_WINDOW          = 3,   // 窗口程序集
    MDICLIENT_CODETYPE_CLASS                    = 4,   // 类
    MDICLIENT_CODETYPE_GLOBALVAR                = 5,   // 全局变量
    MDICLIENT_CODETYPE_STRUCT                   = 6,   // 数据类型
    MDICLIENT_CODETYPE_DLL                      = 7,   // DLL命令定义表
    MDICLIENT_CODETYPE_CONST                    = 8,   // 常量
    MDICLIENT_CODETYPE_IMAGE                    = 9,   // 图片资源
    MDICLIENT_CODETYPE_SOUND                    = 10,  // 声音资源
};

enum SCROLL_DRAWTYPE : int
{
    SCROLL_DRAWTYPE_BACK    = 0x00,     // 绘画背景, 这里接收到的dc是内存dc
    SCROLL_DRAWTYPE_BTN1    = 0x01,     // 绘画滚动条的按钮1, 这里接收到的dc是内存dc
    SCROLL_DRAWTYPE_BTN2    = 0x02,     // 绘画滚动条的按2, 这里接收到的dc是内存dc
    SCROLL_DRAWTYPE_THUMB   = 0x03,     // 绘画滚动条的滑块, 这里接收到的dc是内存dc
    SCROLL_DRAWTYPE_ALERT   = 0x05,     // 绘画结束, 这里接收到的HDC是窗口的hdc, 接收到的位置是计算出来有可能需要绘画的位置, 触发这个事件的时候滚动条已经绘画到这个dc上了
};

//////////////////////////////////////////////////////////////////////////

struct IRE_STRUCT_MENU_CALCSIZE
{
    int width;      // 菜单宽度
    int height;     // 菜单高度
    LPARAM param;   // 添加菜单时的菜单数值, 弹出式菜单这个值为0
};


// 背景被绘画事件结构
typedef struct IRE_STRUCT_MENU_DRAW
{
    LPCANVAS hCanvas;   // dx使用的绘画句柄, 需要gdi/gdi+的话请使用 iDraw_canvas_getdc() 获取绘画DC
    int width;          // 宽度
    int height;         // 高度
    int id;             // 菜单ID
    UINT type;          // 菜单类型
    LPCWSTR text;       // 菜单标题
    int state;          // 菜单状态, 一个或多个值的组合
}*LPIRE_STRUCT_MENU_DRAW;

typedef struct IRE_STRUCT_GETICON
{
    HWND hWnd;          // 要获取图标的窗口句柄, 如果是菜单这个值为0
    LPVOID hResource;   // 绑定窗口时的资源句柄
    LPCWSTR pszID;      // 项目文本
    LPVOID pArg;        // 如果是菜单, 这个值为菜单ID, 根据组件类型来传递不同数据, 详情请查看帮助文档(未写....)
    RECT_F* prc;        // isDx为true则指向 IDRAW_RECT_F 结构指针, 否则指向 IDRAW_RECT 结构指针, 需要把图标位置写入这个地址
    int state;          // 当前要获取的状态图标, 目前只有 STATE::NORMAL, STATE::HOT, STATE::DOWN, STATE::BAN 这几个状态
    int* pIconType;     // 图标来源, 0=id指向的图标, 1=默认图标, 目前暂时不使用这个参数

}*LPIRE_STRUCT_GETICON;


// 选择夹尺寸改变事件结构
typedef struct IRE_STRUCT_TAB_SIZE
{
    int index;      // 当前选中项
    int direction;  // 当前选择夹方向
    RECT rc;        // 选择夹内的组件建议的位置, 这个位置已经把选择夹的头部空了出来, 内部根据这个结构调整位置, 可以被修改
    HWND hWnd;      // 选择夹的句柄
}*LPIRE_STRUCT_TAB_CHANGE;

typedef struct IRE_STRUCT_MDICLIENT_MOVE_EVENT
{
    int windowType;     // 当前MDI下窗口的类型, 低位 -1=没有窗口, 0=代码区, 1=设计器窗口, 2=起始页, 高位 当前MDIClient 窗口下是显示什么窗口类型, 0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源
    HWND hChild1;       // MDI窗口下的第一层窗口
    HWND hChild2;       // MDI窗口下的第二层窗口
    HWND hChild3;       // MDI窗口下的第三层窗口
    HWND hChild4;       // MDI下的第四层窗口, 如果是设计器窗口, 那这个窗口就是设计器窗口, 起始页则为最近打开源码超列, 代码区就是代码窗口
    RECT rcChild1;      // MDI下第一层窗口的移动位置, 基于MDIClient这个窗口的位置修改
    RECT rcChild2;      // MDI下第二层窗口的移动位置, 基于MDIClient下的第一层窗口的位置修改
    RECT rcChild3;      // MDI下第三层窗口的移动位置, 基于MDIClient下的第二层窗口的位置修改
    RECT rcChild4;      // MDI下第四层窗口的移动位置, 基于MDIClient下的第三层窗口的位置修改
}*LPIRE_STRUCT_MDICLIENT_MOVE_EVENT;

// 代码切换选择夹选中被改变事件结构
typedef struct IRE_STRUCT_TAB_SELCHANGE
{
    int index;      // 切换后的索引, 如果为-1要么是代码修改要么就是选择夹空了
    int indexOld;   // 切换前的索引
    HWND hWnd;      // 切换后选择夹里的窗口句柄, 工作夹/状态夹暂时没有使用这个成员
    HWND hWndOld;   // 切换前选择夹里的窗口句柄, 工作夹/状态夹暂时没有使用这个成员
    HWND hTab;      // 触发事件的选择夹
}*LPIRE_STRUCT_TAB_SELCHANGE;


typedef struct IRE_STRUCT_HOOKARG
{
    int code;           // 详情请参考 SetWindowsHookEx 回调函数说明
    WPARAM wParam;      // 详情请参考 SetWindowsHookEx 回调函数说明
    LPARAM lParam;      // 详情请参考 SetWindowsHookEx 回调函数说明
    LRESULT ret;        // 拦截事件的返回值
    HHOOK hhk;          // 钩子句柄
}*LPIRE_STRUCT_HOOKARG;


// 背景被绘画事件结构
typedef struct IRE_STRUCT_BACKGROUND_DRAW
{
    LPCANVAS hCanvas;   // dx使用的绘画句柄, 需要gdi/gdi+的话请使用 iDraw_canvas_getdc() 获取绘画DC
    int cxClient;       // 宽度
    int cyClient;       // 高度
}*LPIRE_STRUCT_BACKGROUND_DRAW;

// 消息事件的消息参数
typedef struct IRE_STRUCT_MSG
{
    HWND hWnd;
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
}*LPIRE_STRUCT_MSG;

// 工具条状态改变事件结构
typedef struct IRE_STRUCT_TOOLSTATE_CHANGE
{
    int id;     // 触发改变状态的按钮ID
    int state;  // 状态
}*LPIRE_STRUCT_TOOLSTATE_CHANGE;

// MDI被创建事件参数
typedef struct IRE_STRUCT_MDICREATE
{
    HWND hWnd;                  // MDI窗口句柄
    LPMDICREATESTRUCTA mcs;     // MDI创建窗口结构, 易语言窗口都是A版, 这里接收的就是A版的结构
}*LPIRE_STRUCT_MDICREATE;

// 代码行号被改变事件参数
typedef struct IRE_STRUCT_EIDE_SELCHANGEDINFO
{
    IEIDE_CodeManage* pCodeManage;  // 代码管理类指针
    int line;                       // 当前选中行号
    int oldLine;                    // 选中当前行之前的行号
    int oldLineCol;                 // 选中当前行之前的列号
}*LPIRE_STRUCT_EIDE_SELCHANGEDINFO;

// 自动隐藏的窗口被调整尺寸
typedef struct IRE_STRUCT_AUTOHIDE_MOVE
{
    HWND hWnd;      // 被移动的窗口
    RECT rc;        // 移动的位置
}*LPIRE_STRUCT_AUTOHIDE_MOVE;

//////////////////////////////////////////////////////////////////////////

// 加入菜单到窗口顶部的菜单夹中
typedef struct IDC_STRUCT_MENU_INSERT
{
    int id;
    LPCWSTR lpszMenuText;
    int index;
    HMENU hPopupMenu;
    bool isPopup;
}*LPIDC_STRUCT_MENU_INSERT;

// 加入菜单到扩展菜单里, 各个成员可以参考MSDN的 InsertMenu 这个API
typedef struct IDC_STRUCT_MENUEXT_INSERT
{
    LPCSTR guid;            // 加入到的插件guid里
    UINT uPosition;         // 位置或ID
    UINT uFlags;            // 菜单标识
    UINT_PTR uIDNewItem;    // 菜单ID
    LPCWSTR lpNewItem;      // 菜单标题
    LPARAM param;           // 菜单数值
}*LPIDC_STRUCT_MENUEXT_INSERT;


#define HRESOURCE_MENU                      ((HWND)(10000 - 4))     // 获取菜单资源句柄
#define HRESOURCE_TREEEXPAND                ((HWND)(10000 - 8))     // 获取树型框加减号资源句柄


// 通知消息, 低位保留, 使用的都是高位
#define INE_MENU_CALCMENU                   0x01000000  // 计算扩展菜单尺寸, wParam接收菜单ID, lParam接收 IRE_STRUCT_MENU_CALCSIZE 结构指针, 通过修改结构的值来处理菜单尺寸, 忽略返回值
#define INE_MENU_DRAWMENU_BEFORE            0x01010000  // 菜单即将绘画, wParam接收菜单ID, lParam接收 IRE_STRUCT_MENU_DRAW结构指针, 返回是否拦截
#define INE_MENU_DRAWMENU_AFTER             0x01020000  // 菜单绘画完毕, wParam接收菜单ID, lParam接收 IRE_STRUCT_MENU_DRAW结构指针, 忽略返回值
#define INE_MENU_GETICON                    0x01030000  // 获取菜单图标, wParam接收菜单ID, lParam接收 IRE_STRUCT_GETICON结构指针, 返回图片指针(LPWINDOW_ICON_DATA), 如果有返回值, 但是 prc没有值则忽略返回值

#define INE_CONFIG_CHANGED                  0x02000000  // 配置被改变, wParam 保留, lParam 保留
#define INE_STATE_CLICK                     0x02010000  // 内部使用, 顶部状态条被单击, wParam, 低位=点击的id, 高位=按钮状态, lParam = 触发消息的窗口句柄
#define INE_STATE_UPDATEHWND                0x02020000  // 内部使用, 顶部状态条当前激活的窗口被改变, wParam = 之前激活的窗口句柄, lParam = 新激活的窗口句柄

#define INE_TOOL_BUTTONCLICK                0x03000000  // 通知接口工具条的按钮被单击, wParam = id, lParam = 加入按钮时填写的项目数值


#define IGL_HWND                            0x0000      // 窗口句柄, 只读
#define IGL_WNDPROC                         0x0001      // 接收子类化的函数地址, iDraw_SubclassWindow() 函数的第二个参数记录在这
#define IGL_CALLBACK_NOTIFY                 0x0002      // 通知消息函数, 由父窗口调用, 函数原型 LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
#define IGL_CALLBACK_DRAWITEM               0x0003      // 绘画消息函数, 由父窗口调用, 函数原型 LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
#define IGL_CALLBACK_MEASUREITEM            0x0004      // 计算尺寸消息函数, 由父窗口调用, 函数原型 LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
#define IGL_CALLBACK_COMMAND                0x0005      // 事件消息函数, 由父窗口调用, 函数原型 LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
#define IGL_CALLBACK_CTLCOLOR               0x0006      // 颜色消息函数, 由父窗口调用, 函数原型 LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
#define IGL_CALLBACK_CALLWINDOWPROC         0x0007      // CallWindowProc 函数地址, 调用原函数时使用这个函数调用, 在调用子类化时会根据窗口是否为unicode版记录
#define IGL_INTERCEPT                       0x0008      // 是否拦截此消息, 挂接消息时才会使用这个值
#define IGL_OLDPROC                         0x0009      // 窗口的原回调函数, 把不处理的消息转到这个函数去处理
#define IGL_CALLPROC_RETURN                 0x000A      // 只读, 调用原回调函数的返回值, 注册处理后消息事件下可以通过这个索引取到返回值
#define IGL_CANVAS                          0x000B      // 与窗口关联的dx绘画句柄, 如果窗口不是使用dx渲染, 这个值为0
#define IGL_USERDATA                        0x000C      // 与窗口关联的用户数据


#define IGL_DRAW_NONCPAINT                  0x0100      // 不重画非客户区, 为true时接收子类化消息的函数不会收到 WM_NCPAINT 消息
#define IGL_DRAW_NOCALCSIZE                 0x0101      // 不计算非客户区, 为true时接收子类化消息的函数不会收到 WM_NCCALCSIZE 消息, 窗口不会有边框
#define IGL_DRAW_ISTRANSPARENT              0x0102      // 是否透明, 如果为true, 在内置的 CTLCOLOR 中会调用 SetMode() 设置为透明
#define IGL_DRAW_ISHOT                      0x0103      // 是否允许热点, 一般是列表相关的组件使用
#define IGL_DRAW_ISCLIENT                   0x0104      // 是否把所有鼠标消息都视为客户区, 为true则不会收到 WM_NCHITTEST消息, 内部全部返回 HTCLIENT
#define IGL_DRAW_ISERASEBKG                 0x0105      // 是否处理重画背景消息, 如果为true, 则窗口会收到 WM_ERASEBACKGROUND 消息, 否则内部返回1
#define IGL_DRAW_STATE                      0x0106      // 当前组件的状态
#define IGL_DRAW_FONT_DX                    0x0110      // 获取DX字体, 只读
#define IGL_DRAW_FONT                       0x0111      // 获取GDI字体, 只读
#define IGL_DRAW_TEXTFORMAT                 0x0112      // 绘画文本对齐格式

#define IGL_DRAW_CRBACKGROUND               0x0150      // 背景颜色, 每个组件都会使用这个颜色来绘画背景
#define IGL_DRAW_CRTEXT                     0x0151      // 文本颜色, 每个组件的正常文本颜色都是使用这个颜色绘画
#define IGL_DRAW_CRTEXTHOT                  0x0152      // 文本热点颜色, 哪个组件用到请参考资料
#define IGL_DRAW_CRTEXTDOWN                 0x0153      // 文本按下颜色, 哪个组件用到请参考资料
#define IGL_DRAW_CRTEXTBAN                  0x0154      // 文本禁止颜色, 哪个组件用到请参考资料
#define IGL_DRAW_CRNORMAL                   0x0155      // 组件正常颜色, 哪个组件用到请参考资料
#define IGL_DRAW_CRHOVER                    0x0156      // 组件进入颜色, 哪个组件用到请参考资料
#define IGL_DRAW_CRDOWN                     0x0157      // 组件按下颜色, 哪个组件用到请参考资料
#define IGL_DRAW_CRFOCUS                    0x0158      // 组件焦点颜色, 哪个组件用到请参考资料
#define IGL_DRAW_CRCHECKED                  0x0159      // 组件选中颜色, 哪个组件用到请参考资料
#define IGL_DRAW_CRSELECTED                 0x015A      // 组件选择颜色, 哪个组件用到请参考资料
#define IGL_DRAW_CRHOT                      0x015B      // 组件热点颜色, 哪个组件用到请参考资料
#define IGL_DRAW_CRBAN                      0x015C      // 组件禁止颜色, 哪个组件用到请参考资料
#define IGL_DRAW_CRVISTED                   0x015D      // 
#define IGL_DRAW_CRSHADOW                   0x015E      // 组件引用颜色, 哪个组件用到请参考资料
#define IGL_DRAW_CRBORDER                   0x015F      // 组件边框颜色, 所有绘制边框使用的颜色

#define IGL_DRAW_HBRBACKGROUND              0x0160      // 背景GDI画刷, 每个组件都会使用这个画刷来填充背景
#define IGL_DRAW_HBRTEXT                    0x0161      // 文本GDI画刷
#define IGL_DRAW_HBRTEXTHOT                 0x0162      // 文本热点GDI画刷, 哪个组件用到请参考资料
#define IGL_DRAW_HBRTEXTDOWN                0x0163      // 文本按下GDI画刷, 哪个组件用到请参考资料
#define IGL_DRAW_HBRTEXTBAN                 0x0164      // 文本禁止GDI画刷, 哪个组件用到请参考资料
#define IGL_DRAW_HBRNORMAL                  0x0165      // 组件正常GDI画刷, 哪个组件用到请参考资料
#define IGL_DRAW_HBRHOVER                   0x0166      // 组件进入GDI画刷, 哪个组件用到请参考资料
#define IGL_DRAW_HBRDOWN                    0x0167      // 组件按下GDI画刷, 哪个组件用到请参考资料
#define IGL_DRAW_HBRFOCUS                   0x0168      // 组件焦点GDI画刷, 哪个组件用到请参考资料
#define IGL_DRAW_HBRCHECKED                 0x0169      // 组件选中GDI画刷, 哪个组件用到请参考资料
#define IGL_DRAW_HBRSELECTED                0x016A      // 组件选择GDI画刷, 哪个组件用到请参考资料
#define IGL_DRAW_HBRHOT                     0x016B      // 组件热点GDI画刷, 哪个组件用到请参考资料
#define IGL_DRAW_HBRBAN                     0x016C      // 组件禁止GDI画刷, 哪个组件用到请参考资料
#define IGL_DRAW_HBRVISTED                  0x016D      // 
#define IGL_DRAW_HBRSHADOW                  0x016E      // 组件阴影GDI画刷, 哪个组件用到请参考资料
#define IGL_DRAW_HBRBORDER                  0x016F      // 组件边框GDI画笔, 所有绘制边框使用的画笔

#define IGL_POS_RECTSCREEN                  0x0200      // 窗口在屏幕中的位置, 返回RECT结构指针, 不允许修改这个指针, 只读
#define IGL_POS_RECTCLIENT                  0x0201      // 窗口在父窗口中的位置, 返回RECT结构指针, 不允许修改这个指针, 只读
#define IGL_POS_X                           0x0202      // 窗口在父窗口中的左边位置, 只读
#define IGL_POS_Y                           0x0203      // 窗口在父窗口中的顶边位置, 只读
#define IGL_POS_WIDTH                       0x0204      // 窗口的宽度 只读
#define IGL_POS_HEIGHT                      0x0205      // 窗口的高度, 只读




//////////////////////////////////////////////////////////////////////////
// 0x0000 - 0x00FF 这些常量是只读常量, 是从视觉库里获取一些数据
#define IGI_EWINDOW                         0x0000  // 只读, 易语言主窗口
#define IGI_CUSTOMCODETAB                   0x0001  // 只读, 代码选择夹的窗口句柄
#define IGI_CODE_HWND                       0x0002  // 只读, 根据索引获取 MDIClient 下的第一层窗口句柄, argument 需要传递索引
#define IGI_SHADOWWINDOW                    0x0003  // 只读, 阴影窗口, 背景插件是基于这个窗口绘画的, 这个窗口是层窗口
#define IGI_MDICLIENTTYPE                   0x0004  // 只读, 当前MDIClient 窗口下是显示什么窗口, 低位 MDICLIENT_TYPE 枚举常量, 高位 MDICLIENT_CODETYPE 枚举常量, -1=没有窗口, 0=代码区, 1=设计器窗口, 2=起始页, 高位是窗口类型, 0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源
#define IGI_MDICLIENTCODETYPE               0x0005  // 只读, 获取指定MDIClient 窗口下是显示什么窗口类型, 第三个参数传递代码选择夹索引, 传递-1则返回当前索引, MDICLIENT_CODETYPE 枚举常量, 0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源
#define IGI_MDICLIENTTYPE_FROMINDEX         0x0006  // 只读, 获取MDIClient 窗口下是显示什么窗口, 低位 MDICLIENT_TYPE 枚举常量, 高位 MDICLIENT_CODETYPE 枚举常量, -1=没有窗口, 0=代码区, 1=设计器窗口, 2=起始页, 高位是窗口类型, 0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源
#define IGI_DPI                             0x0007  // 只读, 获取当前窗口DPI

#define IGI_RCCUSTOMCODETAB                 0x0020  // 只读, 代码选择夹的位置, 基于父窗口的位置, 返回的是 RECT 结构的指针, 不允许修改这个结构的数据
#define IGI_RCSTATE                         0x0021  // 只读, 下面状态夹的位置, 基于父窗口的位置, 返回的是 RECT 结构的指针, 不允许修改这个结构的数据
#define IGI_RCSTATUSBAR                     0x0022  // 只读, 下面状态条的位置, 基于父窗口的位置, 返回的是 RECT 结构的指针, 不允许修改这个结构的数据
#define IGI_RCMDICLIENT                     0x0023  // 只读, 中间代码区的位置, 基于父窗口的位置, 返回的是 RECT 结构的指针, 不允许修改这个结构的数据
#define IGI_RCCONTROL                       0x0024  // 只读, 组件箱的位置, 基于父窗口的位置, 返回的是 RECT 结构的指针, 不允许修改这个结构的数据
#define IGI_RCWORK                          0x0025  // 只读, 工作夹的位置, 基于父窗口的位置, 返回的是 RECT 结构的指针, 不允许修改这个结构的数据
#define IGI_RCMENUWND                       0x0026  // 只读, 菜单栏容器的位置, 基于父窗口的位置, 返回的是 RECT 结构的指针, 不允许修改这个结构的数据
#define IGI_DEFDXFONT                       0x0027  // 只读, 程序默认使用的DX字体, 会根据当前窗口DPI返回对应的字体
#define IGI_CONFIG_DEFDXFONT                0x0027  // 只读, 程序默认使用的DX字体, 会根据当前窗口DPI返回对应的字体
#define IGI_CONFIG_DEFGDIFONT               0x0028  // 只读, 程序默认使用的GDI字体, 会根据当前窗口DPI返回对应的字体

#define IGI_IGETEWINDOW                     0x0030  // 只读, 易语言获取各个窗口的类指针
#define IGI_IDENOTIFY                       0x0031  // 只读, 易语言交互函数, 调用这个函数就相当于调用特殊库的 调用易系统功能() 命令

#define IGI_LIBPATH                         0x0035  // 只读, 易语言支持库目录, 后面带反斜杠, 返回unicode文本指针
#define IGI_DATAPATH                        0x0036  // 只读, 易IDE数据存储目录, 后面带反斜杠, 一般是 易语言支持库目录/iDraw/, 返回unicode文本指针
#define IGI_IDRAW                           0x0037  // 只读, 自绘类接口, 目前暂时未对外公开
#define IGI_D2D                             0x0038  // 只读, D2D类接口, 目前暂时未对外公开

#define IGI_STATE_TOOL_GET                  0x0040  // 只读, 获取指定工具条ID的状态, 参数3需要传递按钮ID
#define IGI_STATE_TOOL_RUN                  0x0041  // 只读, 当前易语言运行按钮的状态
#define IGI_STATE_TOOL_STOP                 0x0042  // 只读, 当前易语言停止按钮的状态

#define IGI_PATH_LIB                        0x0060  // 只读, 易语言支持库目录, 后面带反斜杠, 返回unicode文本指针
#define IGI_PATH_IDRAW                      0x0061  // 只读, 易IDE主要使用的目录, 后面带反斜杠, 一般是 易语言支持库目录/iDraw/, 返回unicode文本指针
#define IGI_PATH_DATA                       0x0062  // 只读, 易IDE数据存储目录, 后面带反斜杠, 一般是 易语言支持库目录/iDraw/data/, 返回unicode文本指针
#define IGI_PATH_PLUGIN                     0x0063  // 只读, 易IDE插件路径
#define IGI_PATH_CONFIG                     0x0064  // 只读, 易IDE数据配置文件完整路径




// 0x0100 开始就是主配置相关常量, 0x0200 - 0x03FF 是颜色配置相关常量
#define IGI_CONFIG_ISDXRENDER               0x0100  // 是否使用DX渲染, 设置需要重启生效
#define IGI_CONFIG_ISRUNSAVE                0x0101  // 运行/编译后是否保存源码
#define IGI_CONFIG_WHEELPOS                 0x0102  // 鼠标滚动一次的距离
#define IGI_CONFIG_NOCLIPCHILDREN           0x0103  // 父窗口绘画时不排除子窗口区域
#define IGI_CONFIG_ISDPIAWARE               0x0104  // 是否开启DPI感知
#define IGI_CONFIG_ISDPISCALEWND            0x0105  // DPI改变时是否缩放窗口
#define IGI_CONFIG_SHOWCODETABCLOSE         0x0106  // 显示代码选择夹的关闭按钮

#define IGI_CONFIG_MENUITEMHEIGHT           0x0110  // 菜单项高度
#define IGI_CONFIG_MENUSEPARATORHEIGHT      0x0111  // 分隔条菜单项高度
#define IGI_CONFIG_ISLOCATIONTOOLBAR        0x0112  // 是否显示定位工具条
#define IGI_CONFIG_PROGRAMDBLCLKTYPE        0x0113  // 程序树型框双击模式, 0=只切换, 1=只展开, 2=展开+切换
#define IGI_CONFIG_NOCHANGETAB_WORK         0x0114  // 不自动切换工作夹
#define IGI_CONFIG_NOCHANGETAB_STATE        0x0115  // 不自动切换状态夹
#define IGI_CONFIG_SCROLL_WIDTH             0x0116  // 滚动条的宽度, 默认是16, 横向滚动条就是高度
#define IGI_CONFIG_SCROLL_BTN_SIZE          0x0117  // 滚动条按钮的尺寸, 默认是12, 纵向滚动条宽度就是滚动条的宽度, 这个尺寸是纵向的高度, 横向相反

#define IGI_CONFIG_WORK_DIRECTION           0x0120  // 工作夹的子夹方向, 0=左边, 1=顶边, 2=右边, 3=底边
#define IGI_CONFIG_WORK_ISICON              0x0121  // 左边的工作夹是否显示图标
//#define IGI_CONFIG_WORK_ISSHOW              0x0122  // 工作夹是否显示, 如果为false, 则失去焦点就隐藏起来
//#define IGI_CONFIG_WORK_POS                 0x0123  // 工作夹位置, 0=左边, 1=顶边, 2=右边, 3=底边
#define IGI_CONFIG_WORK_FLAGS               0x0124  // 工作夹标识, 记录位置, 是否最大化, 是否浮动, 是否停靠等信息 TOP_STATE_BUTTON 枚举常量里的值, 调用 __query_flags 判断值

#define IGI_CONFIG_STATE_DIRECTION          0x0130  // 状态夹的子夹方向, 0=左边, 1=顶边, 2=右边, 3=底边
#define IGI_CONFIG_STATE_ISICON             0x0131  // 底下的状态选择夹是否显示图标
//#define IGI_CONFIG_STATE_ISSHOW             0x0132  // 状态夹是否显示, 如果为false, 则失去焦点就隐藏起来
//#define IGI_CONFIG_STATE_POS                0x0133  // 状态夹位置, 0=左边, 1=顶边, 2=右边, 3=底边
#define IGI_CONFIG_STATE_FLAGS              0x0134  // 状态夹标识, 记录位置, 是否最大化, 是否浮动, 是否停靠等信息 TOP_STATE_BUTTON 枚举常量里的值, 调用 __query_flags 判断值

#define IGI_CONFIG_CODE_DIRECTION           0x0140  // 代码选择夹的子夹方向, 3=底边, 其他 = 顶边
#define IGI_CONFIG_CODE_ISICON              0x0141  // 代码切换选择夹是否显示图标

//#define IGI_CONFIG_CONTROL_ISSHOW           0x0150  // 组件箱是否显示, 如果为false, 则失去焦点就隐藏起来
//#define IGI_CONFIG_CONTROL_POS              0x0151  // 组件箱位置, 0=左边, 1=顶边, 2=右边, 3=底边
#define IGI_CONFIG_CONTROL_FLAGS            0x0152  // 组件箱标识, 记录位置, 是否最大化, 是否浮动, 是否停靠等信息 TOP_STATE_BUTTON 枚举常量里的值, 调用 __query_flags 判断值

#define IGI_CONFIG_TREE_FNE_ISHOT           0x0161  // 支持库树型框是否支持热点
#define IGI_CONFIG_TREE_PROGRAM_ISHOT       0x0162  // 程序树型框是否支持热点
#define IGI_CONFIG_TREE_CONTROL_ISHOT       0x0163  // 组件箱树型框是否支持热点
#define IGI_CONFIG_TREE_VAR_ISHOT           0x0164  // 变量表树型框是否支持热点
#define IGI_CONFIG_TREE_CALL_ISHOT          0x0165  // 调用表树型框是否支持热点
#define IGI_CONFIG_LIST_FIND1_ISHOT         0x0166  // 搜寻1列表框是否支持热点
#define IGI_CONFIG_LIST_FIND2_ISHOT         0x0167  // 搜寻1列表框树型框是否支持热点
#define IGI_CONFIG_TREE_CONTROL_ORGICON     0x0168  // 组件箱树型框显示原图标

#define IGI_CONFIG_TREE_FNE_HEIGHT          0x0171  // 支持库树型框项目高度
#define IGI_CONFIG_TREE_PROGRAM_HEIGHT      0x0172  // 程序树型框项目高度
#define IGI_CONFIG_TREE_CONTROL_HEIGHT      0x0173  // 组件箱树型框项目高度
#define IGI_CONFIG_TREE_VAR_HEIGHT          0x0174  // 变量表树型框项目高度
#define IGI_CONFIG_TREE_CALL_HEIGHT         0x0175  // 调用表树型框项目高度
#define IGI_CONFIG_LIST_FIND1_HEIGHT        0x0176  // 搜寻1列表框项目高度
#define IGI_CONFIG_LIST_FIND2_HEIGHT        0x0177  // 搜寻1列表框项目高度



// 这里开始是颜色配置相关常量
// 所有颜色配色参数3都可以传递 IGICR_ 开头的常量获取相应值, 默认是返回颜色, 如果不是以下这些值之一, 则返回颜色值
#define IGICR_COLOR         0x0000      // 返回颜色, 这是默认值
#define IGICR_BRUSH         0x0001      // 返回这个颜色对应的gdi画刷
#define IGICR_PEN           0x0002      // 返回这个颜色对应的gdi画笔
#define IGICR_BRUSHDX       0x0003      // 返回这个颜色对应的dx画刷, 这个画刷只能使用视觉库提供的绘画函数使用, 这个不是D2D的画刷句柄
#define IGICR_PENDX         0x0004      // 返回这个颜色对应的dx画笔, 这个画笔只能使用视觉库提供的绘画函数使用, 这个不是D2D的画笔句柄

//! 默认配色 0x0200 - 0x021F
#define IGI_CONFIG_CRDEF_BACK               0x0200  // [DefaultColor], 默认配色, 背景颜色
#define IGI_CONFIG_CRDEF_NORMAL             0x0201  // [DefaultColor], 默认配色, 正常颜色
#define IGI_CONFIG_CRDEF_HOT                0x0202  // [DefaultColor], 默认配色, 热点颜色
#define IGI_CONFIG_CRDEF_DOWN               0x0203  // [DefaultColor], 默认配色, 按下颜色
#define IGI_CONFIG_CRDEF_BAN                0x0204  // [DefaultColor], 默认配色, 禁止颜色
#define IGI_CONFIG_CRDEF_HOVER              0x0205  // [DefaultColor], 默认配色, 进入颜色
#define IGI_CONFIG_CRDEF_FOCUS              0x0206  // [DefaultColor], 默认配色, 焦点颜色
#define IGI_CONFIG_CRDEF_CHECKED            0x0207  // [DefaultColor], 默认配色, 选中颜色
#define IGI_CONFIG_CRDEF_SELECTED           0x0208  // [DefaultColor], 默认配色, 选择颜色
#define IGI_CONFIG_CRDEF_VISTED             0x0209  // [DefaultColor], 默认配色, 
#define IGI_CONFIG_CRDEF_SHADOW             0x020A  // [DefaultColor], 默认配色, 阴影颜色
#define IGI_CONFIG_CRDEF_BORDER             0x020B  // [DefaultColor], 默认配色, 边框颜色
#define IGI_CONFIG_CRDEF_TEXT               0x020C  // [DefaultColor], 默认配色, 文本颜色
#define IGI_CONFIG_CRDEF_TEXTHOT            0x020D  // [DefaultColor], 默认配色, 文本热点颜色
#define IGI_CONFIG_CRDEF_TEXTDOWN           0x020E  // [DefaultColor], 默认配色, 文本按下颜色
#define IGI_CONFIG_CRDEF_TEXTBAN            0x020F  // [DefaultColor], 默认配色, 文本禁止颜色



// 基础颜色 0x0220 - 0x022F
#define IGI_CONFIG_CRBACK                   0x0220  // [Basics], 背景颜色
#define IGI_CONFIG_CRBACKEXTERN             0x0221  // [Basics], 外部的背景颜色, 比如选择夹的子夹
#define IGI_CONFIG_CRSTATEBAR               0x0222  // [Basics], 组件箱/工作夹/状态夹顶部激活的颜色
#define IGI_CONFIG_CRTEXT                   0x0223  // [Basics], 一般文本颜色

// 边框/阴影配色 0x0230 - 0x023F
#define IGI_CONFIG_CRBORDER                 0x0230  // [Basics], 窗口边框颜色
#define IGI_CONFIG_CRBORDER_RUN             0x0231  // [Basics], 窗口边框运行时的颜色
#define IGI_CONFIG_CRBORDER_NF              0x0232  // [Basics], 窗口边框失去焦点时的颜色
#define IGI_CONFIG_CRSHADOW                 0x0238  // [Basics], 窗口阴影颜色
#define IGI_CONFIG_CRSHADOW_RUN             0x0239  // [Basics], 窗口阴影运行时的颜色
#define IGI_CONFIG_CRSHADOW_NF              0x023A  // [Basics], 窗口阴影失去焦点时的颜色

// 菜单项 0x0240 - 0x024F
#define IGI_CONFIG_CRMENUBORDER             0x0240  // [Basics], 菜单项的边框颜色
#define IGI_CONFIG_CRMENUBACK               0x0241  // [Basics], 菜单项的背景颜色, 没有热点, 不是禁止时使用这个颜色绘画
#define IGI_CONFIG_CRMENUHOT                0x0242  // [Basics], 菜单项的热点颜色
#define IGI_CONFIG_CRMENUBAN                0x0243  // [Basics], 菜单项的禁止颜色
#define IGI_CONFIG_CRMENUTEXT               0x0244  // [Basics], 菜单项正常文本颜色
#define IGI_CONFIG_CRMENUTEXTHOT            0x0245  // [Basics], 菜单项热点文本颜色
#define IGI_CONFIG_CRMENUTEXTBAN            0x0246  // [Basics], 菜单项禁止文本颜色

// 显示在标题栏那里的菜单夹 0x0250 - 0x025F
#define IGI_CONFIG_CRMENUBARHOT             0x0250  // [Basics], 顶部菜单夹热点颜色
#define IGI_CONFIG_CRMENUBARTEXT            0x0251  // [Basics], 顶部菜单夹助手菜单的文本颜色
#define IGI_CONFIG_CRMENUBARTEXTHOT         0x0252  // [Basics], 顶部菜单夹助手菜单的文本热点颜色

// 显示在标题栏里黑色块的配色 0x0260 - 0x026F
#define IGI_CONFIG_CRTITLEBACK              0x0260  // [Basics], 顶边黑块部分背景颜色
#define IGI_CONFIG_CRTITLELEFT_ACT          0x0261  // [Basics], 顶部黑块部分左边激活时文本颜色, 这个是源码名或者项目名
#define IGI_CONFIG_CRTITLERIGHT_ACT         0x0262  // [Basics], 顶部黑块部分右边激活时文本颜色, 这个是程序类型, 或者未保存时的星号使用的颜色
#define IGI_CONFIG_CRTITLELEFT              0x0263  // [Basics], 顶部黑块部分左边文本颜色, 这个是源码名或者项目名
#define IGI_CONFIG_CRTITLERIGHT             0x0264  // [Basics], 顶部黑块部分右边文本颜色, 这个是程序类型, 或者未保存时的星号使用的颜色

// 底部状态条的配色 0x0270 - 0x027F
#define IGI_CONFIG_CRTEXTSTATUSBAR          0x0270  // [Basics], 底部状态条的文本颜色
#define IGI_CONFIG_CRTEXTSTATUSBAR_RUN      0x0271  // [Basics], 底部状态条运行时的文本颜色
#define IGI_CONFIG_CRTEXTSTATUSBAR_NF       0x0272  // [Basics], 底部状态条不激活的文本颜色

// 滚动条配色 0x0280 - 0x028F
#define IGI_CONFIG_CRSCROLLBTN1NORRMAL      0x0280  // [Basics], 滚动条按钮1正常颜色, 左边或者顶边的小按钮
#define IGI_CONFIG_CRSCROLLBTN1HOT          0x0281  // [Basics], 滚动条按钮1热点颜色, 左边或者顶边的小按钮
#define IGI_CONFIG_CRSCROLLBTN1DOWN         0x0282  // [Basics], 滚动条按钮1按下颜色, 左边或者顶边的小按钮
#define IGI_CONFIG_CRSCROLLBTN2NORRMAL      0x0283  // [Basics], 滚动条按钮2正常颜色, 右边或者底边的小按钮
#define IGI_CONFIG_CRSCROLLBTN2HOT          0x0284  // [Basics], 滚动条按钮2热点颜色, 右边或者底边的小按钮
#define IGI_CONFIG_CRSCROLLBTN2DOWN         0x0285  // [Basics], 滚动条按钮2按下颜色, 右边或者底边的小按钮
#define IGI_CONFIG_CRSCROLLTHUMBNORRMAL     0x0286  // [Basics], 滚动条滑块颜色
#define IGI_CONFIG_CRSCROLLTHUMBHOT         0x0287  // [Basics], 滚动条滑块热点颜色
#define IGI_CONFIG_CRSCROLLTHUMBDOWN        0x0288  // [Basics], 滚动条滑块按下颜色
#define IGI_CONFIG_CRSCROLLBACKGROUND       0x0289  // [Basics], 滚动条背景颜色

// 分割线颜色 0x0290 - 0x029F
#define IGI_CONFIG_CRLINELEFT               0x0290  // [Basics], 工具条, 超列表头, 等组件的分割线左边颜色, 分割线都是两个像素
#define IGI_CONFIG_CRLINERIGHT              0x0291  // [Basics], 工具条, 超列表头, 等组件的分割线右边颜色, 分割线都是两个像素

// 自绘代码切换选择夹关闭按钮颜色, 0x02A0 - 0x02AF
#define IGI_CONFIG_CODETAB_CRCLOSEHOT               0x02A0  // [CustomCodeTab], 代码切换选择夹关闭按钮热点颜色
#define IGI_CONFIG_CODETAB_CRCLOSEDOWN              0x02A1  // [CustomCodeTab], 代码切换选择夹关闭按钮按下颜色

// 自绘代码切换选择夹文本颜色, 0x02B0 - 0x02BF
#define IGI_CONFIG_CODETAB_CRTEXT_WINDOW            0x02B0  // [CustomCodeTab], 代码切换选择夹, 窗口文本颜色
#define IGI_CONFIG_CODETAB_CRTEXT_ASSEMBLY          0x02B1  // [CustomCodeTab], 代码切换选择夹, 程序集文本颜色
#define IGI_CONFIG_CODETAB_CRTEXT_ASSEMBLY_WINDOW   0x02B2  // [CustomCodeTab], 代码切换选择夹, 窗口程序集文本颜色
#define IGI_CONFIG_CODETAB_CRTEXT_CLASS             0x02B3  // [CustomCodeTab], 代码切换选择夹, 类文本颜色
#define IGI_CONFIG_CODETAB_CRTEXT_GLOBALVAR         0x02B4  // [CustomCodeTab], 代码切换选择夹, 全局变量文本颜色
#define IGI_CONFIG_CODETAB_CRTEXT_STRUCT            0x02B5  // [CustomCodeTab], 代码切换选择夹, 自定义数据类型文本颜色
#define IGI_CONFIG_CODETAB_CRTEXT_DLL               0x02B6  // [CustomCodeTab], 代码切换选择夹, DLL文本颜色
#define IGI_CONFIG_CODETAB_CRTEXT_CONST             0x02B7  // [CustomCodeTab], 代码切换选择夹, 常量文本颜色
#define IGI_CONFIG_CODETAB_CRTEXT_IMAGE             0x02B8  // [CustomCodeTab], 代码切换选择夹, 图片文本颜色
#define IGI_CONFIG_CODETAB_CRTEXT_SOUND             0x02B9  // [CustomCodeTab], 代码切换选择夹, 声音文本颜色

// 自绘代码切换选择夹底下线条颜色, 0x02C0 - 0x02CF
#define IGI_CONFIG_CODETAB_CRLINE_WINDOW            0x02C0  // [CustomCodeTab], 代码切换选择夹, 窗口下边线颜色
#define IGI_CONFIG_CODETAB_CRLINE_ASSEMBLY          0x02C1  // [CustomCodeTab], 代码切换选择夹, 程序集下边线颜色
#define IGI_CONFIG_CODETAB_CRLINE_ASSEMBLY_WINDOW   0x02C2  // [CustomCodeTab], 代码切换选择夹, 窗口程序集下边线颜色
#define IGI_CONFIG_CODETAB_CRLINE_CLASS             0x02C3  // [CustomCodeTab], 代码切换选择夹, 类下边线颜色
#define IGI_CONFIG_CODETAB_CRLINE_GLOBALVAR         0x02C4  // [CustomCodeTab], 代码切换选择夹, 全局变量下边线颜色
#define IGI_CONFIG_CODETAB_CRLINE_STRUCT            0x02C5  // [CustomCodeTab], 代码切换选择夹, 自定义数据类型下边线颜色
#define IGI_CONFIG_CODETAB_CRLINE_DLL               0x02C6  // [CustomCodeTab], 代码切换选择夹, DLL下边线颜色
#define IGI_CONFIG_CODETAB_CRLINE_CONST             0x02C7  // [CustomCodeTab], 代码切换选择夹, 常量下边线颜色
#define IGI_CONFIG_CODETAB_CRLINE_IMAGE             0x02C8  // [CustomCodeTab], 代码切换选择夹, 图片下边线颜色
#define IGI_CONFIG_CODETAB_CRLINE_SOUND             0x02C9  // [CustomCodeTab], 代码切换选择夹, 声音下边线颜色

// 颜色配色需要到 0x03FF


#define IRE_SIZE_TABWORK                    0x0001  // 左边工作夹尺寸被改变, 事件回调的第三个参数收到 IRE_STRUCT_TAB_SIZE 结构指针
#define IRE_SIZE_TABSTATE                   0x0002  // 下边状态夹尺寸被改变, 事件回调的第三个参数收到 IRE_STRUCT_TAB_SIZE 结构指针
#define IRE_SIZE_TABMDICLIENT               0x0003  // MDIClient尺寸被改变, 事件回调的第三个参数收到 IRE_STRUCT_MDICLIENT_MOVE_EVENT 结构指针
#define IRE_TAB_WORK_SIZE                   IRE_SIZE_TABWORK
#define IRE_TAB_STATE_SIZE                  IRE_SIZE_TABSTATE
#define IRE_TAB_MDICLIENT_SIZE              IRE_SIZE_TABMDICLIENT
//#define IRE_SCROLL_GETRECT                  0x0010  // 获取滚动条位置, 可以通过这个事件修改边框达到指定绘画滚动条位置, 事件回调第三个参数收到 IRE_STRUCT_SCROLL_GETRECT 结构指针, 此事件不会触发调用后
//#define IRE_SCROLL_CALCRECT                 0x0011  // 计算滚动条位置, 可以通过这个事件修改窗口非客户区位置, 事件回调第三个参数收到 IRE_STRUCT_SCROLL_CALCRECT 结构指针, 此事件不会触发调用后 
//#define IRE_SCROLL_DRAW                     0x0012  // 绘画滚动条, 事件回调第三个参数收到 IRE_STRUCT_SCROLL_DRAW 结构指针, 此事件不会触发调用后



//#define IRE_E_RUNSTATE_CHANGE               0x0010  // 弃用, 不会触发, 易语言运行按钮状态被改变, 事件回调的第三个参数接收是否启用, 1=运行按钮已启用, 0=运行按钮已禁用
//#define IRE_E_STOPSTATE_CHANGE              0x0011  // 弃用, 不会触发, 易语言停止按钮状态被改变, 事件回调的第三个参数接收是否启用, 1=停止按钮已启用, 0=停止按钮已禁用
//#define IRE_TOOLSTATE_CHANGE_RUN            IRE_E_RUNSTATE_CHANGE
//#define IRE_TOOLSTATE_CHANGE_STOP           IRE_E_STOPSTATE_CHANGE
#define IRE_TOOLSTATE_CHANGE                0x0012  // 易语言标准工具条按钮状态被改变, 事件回调的第三个参数接收 IRE_STRUCT_TOOLSTATE_CHANGE 结构


#define IRE_TABSELCHANGED_WORK              0x0020  // 左边工作夹选择被改变, 事件回调的第三个参数收到 IRE_STRUCT_TAB_SELCHANGE 结构指针, 调用前拦截的话不会改变子夹
#define IRE_TABSELCHANGED_STATE             0x0021  // 下边状态夹选择被改变, 事件回调的第三个参数收到 IRE_STRUCT_TAB_SELCHANGE 结构指针, 调用前拦截的话不会改变子夹
#define IRE_TABSELCHANGED_CODE              0x0022  // 代码切换选择夹选择被改变, 事件回调的第三个参数收到 IRE_STRUCT_TAB_SELCHANGE 结构指针, 调用前拦截的话不会触发尺寸改变事件
#define IRE_TAB_WORK_SELCHANGED             IRE_TABSELCHANGED_WORK
#define IRE_TAB_STATE_SELCHANGED            IRE_TABSELCHANGED_STATE
#define IRE_TAB_CODE_SELCHANGED             IRE_TABSELCHANGED_CODE

#define IRE_MDICLIENT_CREATE                0x0023  // MDIClient下的mid窗口被创建事件, 事件回调的第三个参数收到 IRE_STRUCT_MDICREATE 结构指针, 调用前拦截且不正确处理的话插件不会被创建, 调用前结构中的 hWnd成员为0, 调用后成员为新创建的mdi窗口句柄
#define IRE_MDICLIENT_DESTROY               0x0024  // MDIClient下的mid窗口被销毁事件, 事件回调的第三个参数收到 被销毁的MDI窗口句柄
#define IRE_MDICLIENT_ACTIVATE              0x0025  // MDIClient下的mid窗口被激活事件, 事件回调的第三个参数收到 要激活的 MDI 子窗口的句柄
#define IRE_ECODESELCHANGED                 0x0026  // 易语言代码窗口光标改变事件, 事件回调的第三个参数收到 IRE_STRUCT_EIDE_SELCHANGEDINFO 结构指针, 此事件不会触发调用后事件


#define IRE_DRAW_BCAKGROUND                 0x0030  // 背景被绘画事件, 返回值低位为是否拦截, 高位1=gdi,0=dx, 事件回调的第三个参数收到 IRE_STRUCT_BACKGROUND_DRAW 结构指针
#define IRE_BCAKGROUND_DRAW                 IRE_DRAW_BCAKGROUND
#define IRE_DRAW_AUTOHIDE_SHOW              0x0031  // 自动隐藏的窗口被显示, 调用前返回是否拦截, 拦截后不会被显示, 事件回调的第三个参数收到 要显示的窗口句柄
#define IRE_DRAW_AUTOHIDE_HIDE              0x0032  // 自动隐藏的窗口被隐藏, 调用前返回是否拦截, 拦截后不会被隐藏, 事件回调的第三个参数收到 要隐藏的窗口句柄
#define IRE_DRAW_AUTOHIDE_MOVE              0x0033  // 自动隐藏的窗口被移动, 前和后都忽略返回值, 事件回调的第三个参数收到 IRE_STRUCT_AUTOHIDE_MOVE 指针


#define IRE_HOOKPROC_CBT                    0x0050  // cbt钩子事件(WH_CBT), 事件回调第三个参数收到 IRE_STRUCT_HOOKARG 结构指针, 调用前返回是否拦截, 调用后忽略返回值
//#define IRE_HOOKPROC_GETMESSAGE             0x0051  // 获取消息钩子事件(WH_GETMESSAGE), 事件回调第三个参数收到 IRE_STRUCT_HOOKARG 结构指针, 调用前返回是否拦截, 调用后忽略返回值

#define IRE_ICON_GET                        0x0060  // 获取图标事件, 

#define IRE_MENUEXT_MANAGE                  0x0500  // 管理扩展菜单, 不会触发调用后事件, 调用前返回是否拦截, 拦截的话其他插件不会收到这个事件, 第三个参数为0
#define IRE_MENUEXT_CUSTOP                  0x0501  // 自定义扩展菜单, 不会触发调用后事件, 调用前返回是否拦截, 拦截的话其他插件不会收到这个事件, 第三个参数为0








// 调用视觉库功能, 根据功能号调用不同功能, 参数传递请参考对应功能号
// IDC id call, 调用视觉库功能, ID=功能号 C=call , IDC = 功能号调用

#define IDC_ICONFIG_BEGIN               0x01D00000  // 配置插件功能号开始
#define IDC_ICONFIG_END                 0x01D0FFFF  // 配置插件功能号结束
#define IDC_ITHEME_BEGIN                0x02D00000  // 主题插件功能号开始
#define IDC_ITHEME_END                  0x02D0FFFF  // 主题插件功能号结束
#define IDC_IEVENT_BEGIN                0x03D00000  // 事件插件功能号开始
#define IDC_IEVENT_END                  0x03D0FFFF  // 事件插件功能号结束
#define IDC_ICONTROL_BEGIN              0x04D00000  // 绘画插件功能号开始
#define IDC_ICONTROL_END                0x04D0FFFF  // 绘画插件功能号结束
#define IDC_IRESOURCE_BEGIN             0x05D00000  // 资源插件功能号开始
#define IDC_IRESOURCE_END               0x05D0FFFF  // 资源插件功能号结束
#define IDC_IMOVE_BEGIN                 0x06D00000  // 移动插件功能号开始
#define IDC_IMOVE_END                   0x06D0FFFF  // 移动插件功能号结束
#define IDC_IETOOLS_BEGIN               0x07D00000  // 助手插件功能号开始
#define IDC_IETOOLS_END                 0x07D0FFFF  // 助手插件功能号结束
#define IDC_IDRAW_BEGIN                 0x08D00000  // 主支持库功能号开始
#define IDC_IDRAW_END                   0x08D0FFFF  // 主支持库功能号结束

// 临时内部使用, 传递参数使用
struct IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT
{
    HWND                hWnd;           // 绑定事件/添加删除系统按钮的窗口句柄
    union
    {
        int             id;             // 添加,删除,插入 系统按钮的ID
        LPVOID          pfn;            // 事件地址
    };
    int                 nPosition;      // 插入按钮的索引或ID
    LPARAM              param;          // 附加数值
    int                 flags;          // 标识

};
// 临时内部使用, 传递参数使用
struct IDRAW_SYSBUTTON_ICON_TITLE_WINDOWPROC_STRUCT
{
    HWND                hWnd;
    UINT                message;
    WPARAM              wParam;
    LPARAM              lParam;
    WNDPROC             oldProc;
    LRESULT*            pRet;
};
//+ 以下通知常量是公开的
#define IDC_IDRAW_STR2RGB               (IDC_IDRAW_BEGIN + 0x0100)  // 将一个文本颜色转换成RGB颜色, wParam = 默认颜色, 调用失败返回这个颜色, lParam指向颜色文本指针
#define IDC_IDRAW_STR2ARGB              (IDC_IDRAW_BEGIN + 0x0101)  // 将一个文本颜色转换成ARGB颜色, wParam = 默认颜色, 调用失败返回这个颜色, lParam指向颜色文本指针
#define IDC_IDRAW_RGB2STR               (IDC_IDRAW_BEGIN + 0x0102)  // 将RGB颜色转换成文本 RGB(1,2,3), wParam=要转换的rgb, lParam = 至少20个WCHAR缓冲区指针, 返回写入字符数
#define IDC_IDRAW_ARGB2STR              (IDC_IDRAW_BEGIN + 0x0103)  // 将ARGB颜色转换成文本 ARGB(255,1,2,3), wParam=要转换的argb, lParam = 至少26个WCHAR缓冲区指针, 返回写入字符数
#define IDC_IDRAW_GETWINDOWDATA         (IDC_IDRAW_BEGIN + 0x0104)  // 获取窗口数据, wParam保留, lParam = 要获取的窗口句柄, 成功返回数据指针, 失败返回0
#define IDC_IDRAW_GETCODEVISIBLERECT    (IDC_IDRAW_BEGIN + 0x0105)  // 获取代码窗口可视的矩形, wParam保留, lParam = 接收矩形的指针(基于易语言主窗口位置)
#define IDC_IDRAW_SUBWINDOWEX           (IDC_IDRAW_BEGIN + 0x0106)  // 子类化窗口, 把这个窗口重画成视觉库窗口一样的风格, 只限顶层窗口, wParam保留, lParam = DRAW_WINDOW_ARGUMENT 结构指针, 返回窗口句柄
#define IDC_IDRAW_CREATEWINDOW          (IDC_IDRAW_BEGIN + 0x0107)  // 创建一个和视觉库窗口一样风格的顶层窗口, wParam = CREATESTRUCTW 结构指针, lParam = DRAW_WINDOW_ARGUMENT 结构指针, 返回窗口句柄
#define IDC_IDRAW_SYSDRAW               (IDC_IDRAW_BEGIN + 0x0108)  // 窗口重画事件, wParam保留, lParam = IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT 结构指针, 无返回值
#define IDC_IDRAW_SYSBTNCLICK           (IDC_IDRAW_BEGIN + 0x0109)  // 系统按钮点击事件, wParam保留, lParam = IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT 结构指针, 无返回值
#define IDC_IDRAW_ADDSYSBUTTON          (IDC_IDRAW_BEGIN + 0x010A)  // 添加按钮, wParam保留, lParam = IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT 结构指针, 无返回值
#define IDC_IDRAW_DELSYSBUTTON          (IDC_IDRAW_BEGIN + 0x010B)  // 删除按钮, wParam保留, lParam = IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT 结构指针, 无返回值
#define IDC_IDRAW_INSSYSBUTTON          (IDC_IDRAW_BEGIN + 0x010C)  // 插入按钮, wParam保留, lParam = IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT 结构指针, 无返回值
#define IDC_IDRAW_DRAW_WINDOWPROC       (IDC_IDRAW_BEGIN + 0x010D)  // 消息处理函数, 把消息传递到绘画系统按钮的消息过程里处理, wParam保留, lParam = IDRAW_SYSBUTTON_ICON_TITLE_WINDOWPROC_STRUCT结构地址
#define IDC_IDRAW_DRAW_DEFARG           (IDC_IDRAW_BEGIN + 0x010E)  // 获取默认参数, wParam = 窗口句柄, lParam = DRAW_WINDOW_ARGUMENT结构地址, 返回是否成功
#define IDC_IDRAW_SETBTNPARAM           (IDC_IDRAW_BEGIN + 0x010F)  // 设置系统按钮数值, wParam保留, lParam = DRAW_WINDOW_ARGUMENT结构地址, 返回是否成功
#define IDC_IDRAW_GETBTNPARAM           (IDC_IDRAW_BEGIN + 0x0110)  // 获取系统按钮数值, wParam保留, lParam = DRAW_WINDOW_ARGUMENT结构地址, 返回是否成功
#define IDC_IDRAW_GETBTNPARAM           (IDC_IDRAW_BEGIN + 0x0110)  // 获取系统按钮数值, wParam保留, lParam = DRAW_WINDOW_ARGUMENT结构地址, 返回是否成功
#define IDC_IDRAW_GETBTNID              (IDC_IDRAW_BEGIN + 0x0111)  // 获取系统按钮ID, wParam保留, lParam = DRAW_WINDOW_ARGUMENT结构地址, 返回是否成功
#define IDC_IDRAW_GETBTNPOS             (IDC_IDRAW_BEGIN + 0x0112)  // 获取系统按钮索引, wParam保留, lParam = DRAW_WINDOW_ARGUMENT结构地址, 返回是否成功

#define IDC_IDRAW_INLINEHOOK            (IDC_IDRAW_BEGIN + 0x0200)  // 设置inlineHook, wParam = 原函数, lParam = 新函数, 返回原函数地址, 可以直接调用原函数
#define IDC_IDRAW_UNINLINEHOOK          (IDC_IDRAW_BEGIN + 0x0201)  // 取消inlineHook, wParam = 原函数, lParam = 保留, 返回是否成功
#define IDC_IDRAW_UNINLINEHOOK_PLUGIN   (IDC_IDRAW_BEGIN + 0x0202)  // 取消被释放插件的inlineHook, wParam = 模块起始地址, lParam = 模块结束地址
#define IDC_IDRAW_CREATEPOPUPMENU       (IDC_IDRAW_BEGIN + 0x0203)  // 创建有自绘样式的弹出式菜单, 所有加入到这个弹出式菜单里的子菜单都会被自绘

#define IDC_EIDE_GETCODEMANAGE          (IDC_IDRAW_BEGIN + 0x0401)  // 获取IDE代码管理(IEIDE_CodeManage)类指针, 忽略wParam参数, lParma = 代码窗口窗口句柄, 为0则获取当前窗口
#define IDC_EIDE_GETLINESPACING         (IDC_IDRAW_BEGIN + 0x0402)  // 获取单行间隔, 忽略wParam和 lParam参数
#define IDC_EIDE_SETLINESPACING         (IDC_IDRAW_BEGIN + 0x0403)  // 暂未实现, 设置单行间隔, 忽略wParam和 lParam参数
#define IDC_EIDE_GETLINEHEIGHT          (IDC_IDRAW_BEGIN + 0x0404)  // 获取单行高度, 忽略wParam和 lParam参数
#define IDC_EIDE_SETLINEHEIGHT          (IDC_IDRAW_BEGIN + 0x0405)  // 暂未实现, 设置单行高度, 忽略wParam和 lParam参数
#define IDC_EIDE_GETCODEWINDOW          (IDC_IDRAW_BEGIN + 0x0406)  // 获取当前代码框句柄, 如果当前是起始页或者设计器窗口则返回0, 忽略wParam和 lParam参数
#define IDC_EIDE_GETECOLOR              (IDC_IDRAW_BEGIN + 0x0407)  // 获取易语言当前使用的配色, wParam = 缓冲区尺寸, lParam = 缓冲区地址, 返回写入缓冲区字节数, wParam/lParam为0则返回缓冲区需要的尺寸
#define IDC_EIDE_SETECOLOR              (IDC_IDRAW_BEGIN + 0x0408)  // 设置易语言当前使用的配色, wParam为0则lParam是完整的配色文件路径Unicode版, 不为0则lParam指向配色数据, wParam为数据尺寸, lParam = 根据wParam决定是什么值

#define IDC_SCROLL_GETRECT              (IDC_IDRAW_BEGIN + 0x0500)  // 获取滚动条位置, 可以通过这个事件修改边框达到指定绘画滚动条位置, wParam = pfn_GetScrollRect 回调函数, lParam = 窗口数据指针, 成功返回 wParam, 失败返回0
#define IDC_SCROLL_CALCRECT             (IDC_IDRAW_BEGIN + 0x0501)  // 计算滚动条位置, 可以通过这个事件修改窗口非客户区位置, wParam = pfn_CalcScroll 回调函数, lParam = 窗口数据指针, 成功返回 wParam, 失败返回0
#define IDC_SCROLL_DRAW                 (IDC_IDRAW_BEGIN + 0x0502)  // 绘画滚动条, wParam = pfn_DrawScroll 回调函数, lParam = 窗口数据指针, 成功返回 wParam, 失败返回0




#define IDC_IDRAW_SET_ETOOLSFUN         (IDC_IDRAW_BEGIN + 0x0001)  // 设置助手功能函数, wParam忽略, lParam 传递助手的功能函数, int __stdcall callback(int, int, int)
#define IDC_INLINEHOOK                  (IDC_IDRAW_BEGIN + 0x0002)  // 设置hook, wParam=hook方式, 0=detoursHook, 1=MinHook, lParam=[被hook地址, 接收地址], 返回原地址
#define IDC_UNINLINEHOOK                (IDC_IDRAW_BEGIN + 0x0003)  // 取消hook, wParam=保留, lParam=hook时的返回值
#define IDC_GETSCROLLDATA               (IDC_IDRAW_BEGIN + 0x0004)  // 获取滚动条数据, wParam = 滚动条对象指针, lParam = IDRAW_STRUCT_SCROLL_DATA 结构指针



#define BUILD_STATE_BEGIN               0   // 编译开始
#define BUILD_STATE_END                 1   // 编译结束
#define BUILD_STATE_OK                  2   // 编译成功
#define BUILD_STATE_FAIL                3   // 编译失败

#define IDC_ICONTROL_MENU_BEGIN         (IDC_ICONTROL_BEGIN + 0x1000)       // 绘画插件菜单相关命令开始
#define IDC_ICONTROL_MENU_END           (IDC_ICONTROL_BEGIN + 0x1FFF)       // 绘画插件菜单相关命令结束
#define IDC_ICONTROL_INSERTMENUBAR      (IDC_ICONTROL_MENU_BEGIN + 0x0001)  // 加入菜单到窗口顶部的菜单夹中, wParam忽略, lParam 接收 IDC_STRUCT_MENU_INSERT结构指针
#define IDC_ICONTROL_REMOVEMENUBAR      (IDC_ICONTROL_MENU_BEGIN + 0x0002)  // 从顶部菜单夹里删除菜单, wParam 是菜单ID, lParam 表示 wParam是否为索引
#define IDC_ICONTROL_INSERT_MENUEXT     (IDC_ICONTROL_MENU_BEGIN + 0x0003)  // 加入菜单到扩展菜单里, wParam忽略, lParam 接收 IDC_STRUCT_MENUEXT_INSERT结构指针
#define IDC_ICONTROL_ADD_MENUBAR        (IDC_ICONTROL_MENU_BEGIN + 0x0004)  // 助手调用, wParam传递成员数, lParam 传递数组数据, 数组存放需要加入顶部菜单的句柄, 支持库只允许这些菜单加入顶部
#define IDC_ICONTROL_ISADDMENUBAR       (IDC_ICONTROL_MENU_BEGIN + 0x0005)  // 助手专用功能, 加入顶部菜单时判断是否允许加入, wParam = 0, lParam = 要加入的菜单句柄, 因为助手加入顶部的都是弹出式菜单, 所以都是菜单句柄
#define IDC_ICONTROL_BUILD              (IDC_ICONTROL_MENU_BEGIN + 0x0006)  // 助手通知视觉库编译开始, wParam = 编译状态, BUILD_STATE_ 开头常量, lParam保留
#define IDC_ICONTROL_UNLOADLIB          (IDC_ICONTROL_MENU_BEGIN + 0x0007)  // 通知支持库卸载支持库, wParam = 1=启用,0=停用,-1=获取是否开启支持库, lParam = 保留

#define IDC_ICONTROL_TOOL_BEGIN         (IDC_ICONTROL_BEGIN + 0x2000)       // 绘画插件菜单相关命令开始
#define IDC_ICONTROL_TOOL_END           (IDC_ICONTROL_BEGIN + 0x2FFF)       // 绘画插件菜单相关命令结束
#define IDC_ICONTROL_TOOL_INSERT        (IDC_ICONTROL_TOOL_BEGIN + 0x0008)  // 往接口工具条中添加一个按钮, wParam = 保留, lParam = IDRAW_STRUCT_TOOL_INSERT指针
#define IDC_ICONTROL_TOOL_DELETE        (IDC_ICONTROL_TOOL_BEGIN + 0x0009)  // 从接口工具条中删除一个按钮, wParam = 按钮id, lParam = LPPLUGIN_INFO 指针
#define IDC_ICONTROL_TOOL_GETBBUTTON    (IDC_ICONTROL_TOOL_BEGIN + 0x000A)  // 从接口工具条中获取一个按钮的数据, wParam = 按钮id, lParam = LPPLUGIN_INFO 指针, 返回 IDRAW_STRUCT_TOOL_INSERTEX 结构指针, 这个指向按钮的数据
#define IDC_ICONTROL_TOOL_SETBBUTTON    (IDC_ICONTROL_TOOL_BEGIN + 0x000B)  // 从接口工具条中设置一个按钮的数据, wParam = 按钮id, lParam = IDRAW_STRUCT_TOOL_INSERT 指针, lParam的插件指针和按钮ID必须记录, 否则返回0

#define IDC_IMOVE_GETWINDOWTYPE         (IDC_IMOVE_BEGIN + 0X0001)   // 获取当前MDIClient 窗口下是显示什么窗口, -1=没有窗口, 0=代码区, 1=设计器窗口, 2=起始页, 高位是窗口类型, 0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源
#define IDC_IMOVE_GETCODETYPE           (IDC_IMOVE_BEGIN + 0X0002)   // 获取当前MDIClient 窗口下是显示什么窗口类型, 0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源, 不需要传递参数























#define CWnd_m_hWnd                             28      // 
#define CFrameWnd_m_hMenuDefault                68      // 
#define CFrameWnd_m_listControlBars             108     // 非指针
#define CMDIFrameWnd_m_hWndMDIClient            188     // 
#define CWinThread_m_pMainWnd                   28      // 
#define CWinApp_m_pDocManager                   128     // 
#define CDocManager_m_templateList              4       // 非指针
#define CMultiDocTemplate_m_hMenuShared         100     // 
#define CPtrList_m_pNodeHead                    4       // 
#define CPtrList_m_pNodeTail                    8       // 
#define CPtrList_m_nCount                       12      // 
#define CNode_pNext                             0       // 
#define CNode_pPrev                             4       // 
#define CNode_data                              8       // 
#define CControlBar_m_dwStyle                   100     // 
#define CControlBar_m_pDockSite                 108     // 
#define CControlBar_m_pDockBar                  112     // 
#define CControlBar_m_pDockContext              116     // 
#define ECodeEdit_nCodeType                     60      // 
#define ECodeEdit_nCursorRowStart               16      // 
#define ECodeEdit_nCursorTabColStart            120     // 
#define ECodeEdit_nCursorColumnStart            124     // 
#define ECodeEdit_nCursorColumnEnd              128     // 
#define ECodeEdit_nCursorRowEnd                 132     // 
#define ECodeEdit_nCursorTabColEnd              136     // 
#define ECodeEdit_pSelectRowInfo                192     // 
#define ECodeEdit_nSelectRowInfoMaxLen          196     // 
#define ECodeEdit_nSelectRowInfoLen             200     // 

























// > `IGL_HWND`                           `0x0000`       // 窗口句柄, 只读
// > `IGL_WNDPROC`                        `0x0001`       // 接收子类化的函数地址, iDraw_SubclassWindow() 函数的第二个参数记录在这
// > `IGL_CALLBACK_NOTIFY`                `0x0002`       // 通知消息函数, 由父窗口调用, 函数原型 LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
// > `IGL_CALLBACK_DRAWITEM`              `0x0003`       // 绘画消息函数, 由父窗口调用, 函数原型 LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
// > `IGL_CALLBACK_MEASUREITEM`           `0x0004`       // 计算尺寸消息函数, 由父窗口调用, 函数原型 LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
// > `IGL_CALLBACK_COMMAND`               `0x0005`       // 事件消息函数, 由父窗口调用, 函数原型 LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
// > `IGL_CALLBACK_CTLCOLOR`              `0x0006`       // 颜色消息函数, 由父窗口调用, 函数原型 LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
// > `IGL_CALLBACK_CALLWINDOWPROC`        `0x0007`       // CallWindowProc 函数地址, 调用原函数时使用这个函数调用, 在调用子类化时会根据窗口是否为unicode版记录
// > `IGL_INTERCEPT`                      `0x0008`       // 是否拦截此消息, 挂接消息时才会使用这个值
// > `IGL_OLDPROC`                        `0x0009`       // 窗口的原回调函数, 把不处理的消息转到这个函数去处理
// > `IGL_CALLPROC_RETURN`                `0x000A`       // 只读, 调用原回调函数的返回值, 注册处理后消息事件下可以通过这个索引取到返回值
// > `IGL_DRAW_NONCPAINT`                 `0x0100`       // 不重画非客户区, 为true时接收子类化消息的函数不会收到 WM_NCPAINT 消息
// > `IGL_DRAW_NOCALCSIZE`                `0x0101`       // 不计算非客户区, 为true时接收子类化消息的函数不会收到 WM_NCCALCSIZE 消息, 窗口不会有边框
// > `IGL_DRAW_ISTRANSPARENT`             `0x0102`       // 是否透明, 如果为true, 在内置的 CTLCOLOR 中会调用 SetMode() 设置为透明
// > `IGL_DRAW_ISHOT`                     `0x0103`       // 是否允许热点, 一般是列表相关的组件使用
// > `IGL_DRAW_ISCLIENT`                  `0x0104`       // 是否把所有鼠标消息都视为客户区, 为true则不会收到 WM_NCHITTEST消息, 内部全部返回 HTCLIENT
// > `IGL_DRAW_ISERASEBKG`                `0x0105`       // 是否处理重画背景消息, 如果为true, 则窗口会收到 WM_ERASEBACKGROUND 消息, 否则内部返回1
// > `IGL_DRAW_STATE`                     `0x0106`       // 当前组件的状态
// > `IGL_DRAW_FONT_DX`                   `0x0110`       // 获取DX字体, 只读
// > `IGL_DRAW_FONT`                      `0x0111`       // 获取GDI字体, 只读
// > `IGL_DRAW_TEXTFORMAT`                `0x0112`       // 绘画文本对齐格式
// > `IGL_DRAW_CRBACKGROUND`              `0x0150`       // 背景颜色, 每个组件都会使用这个颜色来绘画背景
// > `IGL_DRAW_CRTEXT`                    `0x0151`       // 文本颜色, 每个组件的正常文本颜色都是使用这个颜色绘画
// > `IGL_DRAW_CRTEXTHOT`                 `0x0152`       // 文本热点颜色, 哪个组件用到请参考资料
// > `IGL_DRAW_CRTEXTDOWN`                `0x0153`       // 文本按下颜色, 哪个组件用到请参考资料
// > `IGL_DRAW_CRTEXTBAN`                 `0x0154`       // 文本禁止颜色, 哪个组件用到请参考资料
// > `IGL_DRAW_CRNORMAL`                  `0x0155`       // 组件正常颜色, 哪个组件用到请参考资料
// > `IGL_DRAW_CRHOVER`                   `0x0156`       // 组件进入颜色, 哪个组件用到请参考资料
// > `IGL_DRAW_CRDOWN`                    `0x0157`       // 组件按下颜色, 哪个组件用到请参考资料
// > `IGL_DRAW_CRFOCUS`                   `0x0158`       // 组件焦点颜色, 哪个组件用到请参考资料
// > `IGL_DRAW_CRCHECKED`                 `0x0159`       // 组件选中颜色, 哪个组件用到请参考资料
// > `IGL_DRAW_CRSELECTED`                `0x015A`       // 组件选择颜色, 哪个组件用到请参考资料
// > `IGL_DRAW_CRHOT`                     `0x015B`       // 组件热点颜色, 哪个组件用到请参考资料
// > `IGL_DRAW_CRBAN`                     `0x015C`       // 组件禁止颜色, 哪个组件用到请参考资料
// > `IGL_DRAW_CRVISTED`                  `0x015D`       // 
// > `IGL_DRAW_CRSHADOW`                  `0x015E`       // 组件引用颜色, 哪个组件用到请参考资料
// > `IGL_DRAW_CRBORDER`                  `0x015F`       // 组件边框颜色, 所有绘制边框使用的颜色
// > `IGL_DRAW_HBRBACKGROUND`             `0x0160`       // 背景GDI画刷, 每个组件都会使用这个画刷来填充背景
// > `IGL_DRAW_HBRTEXT`                   `0x0161`       // 文本GDI画刷
// > `IGL_DRAW_HBRTEXTHOT`                `0x0162`       // 文本热点GDI画刷, 哪个组件用到请参考资料
// > `IGL_DRAW_HBRTEXTDOWN`               `0x0163`       // 文本按下GDI画刷, 哪个组件用到请参考资料
// > `IGL_DRAW_HBRTEXTBAN`                `0x0164`       // 文本禁止GDI画刷, 哪个组件用到请参考资料
// > `IGL_DRAW_HBRNORMAL`                 `0x0165`       // 组件正常GDI画刷, 哪个组件用到请参考资料
// > `IGL_DRAW_HBRHOVER`                  `0x0166`       // 组件进入GDI画刷, 哪个组件用到请参考资料
// > `IGL_DRAW_HBRDOWN`                   `0x0167`       // 组件按下GDI画刷, 哪个组件用到请参考资料
// > `IGL_DRAW_HBRFOCUS`                  `0x0168`       // 组件焦点GDI画刷, 哪个组件用到请参考资料
// > `IGL_DRAW_HBRCHECKED`                `0x0169`       // 组件选中GDI画刷, 哪个组件用到请参考资料
// > `IGL_DRAW_HBRSELECTED`               `0x016A`       // 组件选择GDI画刷, 哪个组件用到请参考资料
// > `IGL_DRAW_HBRHOT`                    `0x016B`       // 组件热点GDI画刷, 哪个组件用到请参考资料
// > `IGL_DRAW_HBRBAN`                    `0x016C`       // 组件禁止GDI画刷, 哪个组件用到请参考资料
// > `IGL_DRAW_HBRVISTED`                 `0x016D`       // 
// > `IGL_DRAW_HBRSHADOW`                 `0x016E`       // 组件阴影GDI画刷, 哪个组件用到请参考资料
// > `IGL_DRAW_HBRBORDER`                 `0x016F`       // 组件边框GDI画笔, 所有绘制边框使用的画笔
// > `IGL_POS_RECTSCREEN`                 `0x0200`       // 窗口在屏幕中的位置, 返回RECT结构指针, 不允许修改这个指针, 只读
// > `IGL_POS_RECTCLIENT`                 `0x0201`       // 窗口在父窗口中的位置, 返回RECT结构指针, 不允许修改这个指针, 只读
// > `IGL_POS_X`                          `0x0202`       // 窗口在父窗口中的左边位置, 只读
// > `IGL_POS_Y`                          `0x0203`       // 窗口在父窗口中的顶边位置, 只读
// > `IGL_POS_WIDTH`                      `0x0204`       // 窗口的宽度 只读
// > `IGL_POS_HEIGHT`                     `0x0205`       // 窗口的高度, 只读