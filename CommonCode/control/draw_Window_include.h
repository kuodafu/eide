#pragma once
#include "WndBase.h"
#include <control/CShadow.h>
#include <control/WndBaseObjStruct.h>


#define SC_MENU         MAKELONG(0, 0x1)
#define SC_SETTING      MAKELONG(0, 0x2)
#define SC_SKIN         MAKELONG(0, 0x4)
#define SC_HELP         MAKELONG(0, 0x8)

typedef struct DRAW_WINDOW_COLOR
{
    DWORD               cbSize;         // 当前结构尺寸, 有升级的话根据这个尺寸判断
    ARGB                crNormal;       // 正常颜色, ARGB颜色, 为0则不使用
    ARGB                crHot;          // 点燃颜色, ARGB颜色, 为0则不使用
    ARGB                crDown;         // 按下颜色, ARGB颜色, 为0则不使用
    ARGB                crBack;         // 背景颜色, ARGB颜色, 为0则不使用
    ARGB                crBorder;       // 阴影边框颜色, 为0则没有边框, ARGB颜色
    ARGB                crBorder_nf;    // 阴影边框未激活时的颜色, 为0则没有边框, ARGB颜色
    ARGB                crShadow;       // 阴影颜色, ARGB颜色, 为0则不使用阴影
    ARGB                crShadow_nf;    // 阴影未激活时的颜色, ARGB颜色, 为0则不使用阴影

    ARGB                crTitle;        // 标题已激活颜色, ARGB颜色
    ARGB                crTitle_nf;     // 标题未激活颜色, ARGB颜色
}*LPDRAW_WINDOW_COLOR;

typedef struct DRAW_WINDOW_ARGUMENT : DRAW_WINDOW_COLOR
{
    HWND                hWnd;           // 需要处理的窗口
    int                 sysbtnWidth;    // 系统按钮宽度
    int                 sysbtnHeight;   // 系统按钮高度
    BOOL                isSubClass;     // 是否内部子类化该窗口, 为false则外部调用 iDraw_DrawWindowProc 把需要处理的消息传递进来
    BOOL                isUnicode;      // 窗口是否为Unicode窗口, 为true则使用W版函数
    BOOL                isCasualMove;   // 窗口随意移动, 为false则只有在标题栏中才能移动
    BOOL                isDrawIcon;     // 是否绘画图标, 如果不绘画图标, 鼠标在图标位置点击不会出现菜单, 标题也会占用图标的位置
    BOOL                isDrawDefBtn;   // 是否绘画默认的按钮, 如果为true, 则根据窗口风格, 加入系统按钮, 按钮的ID是 SC_ 开头的常量

    int                 nMinWidth;      // 最小窗口宽度
    int                 nMinHeight;     // 最小窗口高度

    LPCANVAS            hCanvas;        // 绘画句柄, 外部提供一个绘画句柄, 如果不提供, 则内部会创建一个
    LPEX_FONT           hFont;          // 绘画标题使用的字体, 如果外部不提供, 内部会创建一个
    LPEX_BRUSH          hbrTextTitle;   // 绘画标题使用的字体, 如果外部不提供, 内部会创建一个
    LPEX_BRUSH          hbrTextTitle_nf; // 绘画标题使用的字体, 如果外部不提供, 内部会创建一个

}*LPDRAW_WINDOW_ARGUMENT;


template<typename T>
struct DRAW_EVENT_STRUCT
{
    T       pfn;    // 事件的回调函数
    LPARAM  param;  // 事件的附加参数
    inline operator bool()
    {
        return pfn != 0;
    }
};

struct SYSBUTTON_INFO
{
    RECT                rc;             // 系统按钮位置
    DWORD               ID;             // 按钮ID, 点击事件是根据ID来处理的
    LPARAM              param;          // 按钮的项目数值
};

// 绘画的阶段
enum DRAW_STAGE
{
    DRAW_STAGE_BUTTON_BEGIN,    // 绘画系统按钮开始, 即将绘画系统按钮, 返回true则不使用内部的绘制
    DRAW_STAGE_BUTTON_END,      // 绘画系统按钮结束, 按钮已经绘画完毕
    DRAW_STAGE_ICON_BEGIN,      // 绘画系统图标开始, 即将绘画系统图标, 返回true则不使用内部的绘制
    DRAW_STAGE_ICON_END,        // 绘画系统图标结束, 系统图标已经绘画完毕
    DRAW_STAGE_TITLE_BEGIN,     // 绘画标题开始, 即将绘画标题, 返回true则不使用内部的绘制
    DRAW_STAGE_TITLE_END,       // 绘画标题结束, 标题已经绘画完毕
};

// 绘画的结构
typedef struct DRAW_PAINTSTRUCT
{
    HWND            hWnd;       // 窗口句柄
    LPCANVAS        hCanvas;    // 画布句柄
    DRAW_STAGE      stage;      // 绘画阶段
    const RECT_F*   rc;         // 绘画的矩形位置
    int             id;         // 绘画系统按钮时使用, 这个表示当前绘画的按钮ID
    STATE           state;      // 状态, 系统按钮使用
    LPARAM          param;      // 附加参数, 绘画事件绑定的项目数值
    LPCWSTR         lpszTitle;  // 绘画标题时使用的标题文本
    LPEX_IMAGE      hIcon;      // 绘画系统图标使用的图标
    LPEX_BRUSH      hbrText;    // 绘画标题使用的画刷

}*LPDRAW_PAINTSTRUCT;




// 系统按钮被点击事件
typedef bool(CALLBACK* PFN_SysBtnClick)(int id, LPARAM btnParam, BOOL isRButton, LPARAM param);
// 系统按钮被绘画事件
typedef bool(CALLBACK* PFN_SysDraw)(LPDRAW_PAINTSTRUCT dps);

typedef struct DRAW_WINDOW_DATA : DRAW_WINDOW_ARGUMENT
{
    WNDPROC             oldProc;
    pfn_CallWindowProc  fnCallProc;     // 调用原函数使用的API, 如果没有设置这个值, 默认会获取 CallWindowProcW
    pfn_DefWindowProc   fnDefProc;      // 默认调用使用的API, 如果没有设置这个值, 默认会获取 DefWindowProcW
    pfn_ColorChange     fnCrChange;     // 颜色改变函数, 有些组件使用的不是默认配色, 配色改变后需要调用这个来更改配色

    pfn_SetLong         SetWndLong;     // 设置窗口属性, 已经区分了W版和A版
    pfn_GetLong         GetWndLong;     // 获取窗口属性, 已经区分了W版和A版
    pfn_SetLong         SetClsLong;     // 设置类属性, 已经区分了W版和A版
    pfn_GetLong         GetClsLong;     // 获取类属性, 已经区分了W版和A版

    SYSBUTTON_INFO      btn[100];       // 系统按钮组, 最多允许100个, 静态变量, 会清0
    DWORD               count;          // 当前程序使用的系统按钮数量
    DWORD               index;          // 当前绘画的是哪个按钮
    STATE               state;          // 系统按钮当前状态, 只能有一个按钮有状态
    DWORD               downIndex;      // 系统按钮按下索引
    DWORD               rdownIndex;     // 系统按钮右键按下索引
    RECT                rcBtn;          // 整个系统按钮位置
    RECT                rcTitle;        // 标题栏位置
    RECT                rcIcon;         // 图标位置
    RECT                rcCaption;      // 标题栏, 系统图标, 系统按钮整个占用的位置

    DWORD               style;          // 窗口样式
    DWORD               styleEx;        // 窗口扩展样式

    bool                isZoomed;       // 是否最大化
    bool                isActivation;   // 是否是激活状态
    bool                isThickFrame;   // 是否有可调边框
    bool                reserve;        // 备用
    DWORD               freeFlags;      // 释放的标识, 根据标识决定释放的内容, 画布, 画刷, 字体
    BOOL                showImage;      // 交替显示使用, 窗口激活时跟随改变, 窗口交替显示的时候需要用时钟改变
    int                 showCount;      // 交替显示次数, 为0的时候就显示当前激活或者不激活的样子
    DRAW_EVENT_STRUCT<PFN_SysBtnClick>  pfnSysBtnClick;     // 系统按钮点击事件
    DRAW_EVENT_STRUCT<PFN_SysDraw>      pfnDraw;            // 绘画事件, 图标, 标题, 系统按钮


    RECT                rcWindow;       // GetWindowRect 的值, 在WINDOWPOSCHANGE的事件下记录
    RECT                rcClient;       // GetClientRect 的值, 在WINDOWPOSCHANGE的事件下记录
    CShadow*            shadow;         // 阴影窗口

    LPEX_IMAGE          hImageShdow;    // 阴影窗口激活时的图片
    LPEX_IMAGE          hImageShdow_nf; // 阴影窗口取消激活时的图片



}*LPDRAW_WINDOW_DATA;


