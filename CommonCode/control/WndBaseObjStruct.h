#pragma once
#include <d2d/d2d_old.h>
#include <CommCtrl.h>
#include "WndBaseState.h"

typedef struct COLORINFO
{
    DWORD   crBackGround;           // 背景颜色
    DWORD   crText;                 // 文本颜色
    DWORD   crTextHot;              // 文本颜色
    DWORD   crTextDown;             // 文本颜色
    DWORD   crTextBan;              // 文本颜色
    DWORD   crNormal;               // 控件正常颜色
    DWORD   crHover;                // 控件进入颜色
    DWORD   crDown;                 // 控件按下颜色
    DWORD   crFocus;                // 控件焦点颜色
    DWORD   crChecked;              // 控件选中颜色
    DWORD   crSelected;             // 控件选择颜色
    DWORD   crHot;                  // 控件热点颜色
    DWORD   crBan;                  // 控件禁止颜色
    DWORD   crVisted;               // 
    DWORD   crShadow;               // 阴影颜色
    DWORD   crBorder;               // 边框颜色

    HBRUSH  hbrBackGround;          // 背景颜色
    HBRUSH  hbrText;                // 文本颜色
    HBRUSH  hbrTextHot;             // 文本颜色
    HBRUSH  hbrTextDown;            // 文本颜色
    HBRUSH  hbrTextBan;             // 文本颜色
    HBRUSH  hbrNormal;              // 控件正常颜色
    HBRUSH  hbrHover;               // 控件进入颜色
    HBRUSH  hbrDown;                // 控件按下颜色
    HBRUSH  hbrFocus;               // 控件焦点颜色
    HBRUSH  hbrChecked;             // 控件选中颜色
    HBRUSH  hbrSelected;            // 控件选择颜色
    HBRUSH  hbrHot;                 // 控件热点颜色
    HBRUSH  hbrBan;                 // 控件禁止颜色
    HBRUSH  hbrVisted;              // 
    HBRUSH  hbrShadow;              // 阴影颜色
    HPEN    hpenBorder;             // 边框画笔颜色


}*LPCOLORINFO;
typedef struct DRAWINFO
{
    LPCANVAS    hCanvas;            // d2d绘制句柄
    LPEX_FONT   hFont;              // 组件绘画字体
    LPEX_FONT   hFontBold;          // 组件绘画加粗字体
    DWORD       textFormat;         // 绘画文本对齐格式
    STATE       state;              // 组件当前状态
    COLORINFO   color;              // 组件颜色

    bool        isNoNcPaint;        // 不重画非客户区
    bool        isNcCalcSize;       // 重新处理非客户区, 把非客户区设置为0
    bool        isTransparent;      // 是否透明
    bool        isHot;              // 是否支持热点
    bool        isNoSize;           // 不处理Size消息
    bool        isClient;           // 所有鼠标消息都转换成客户区消息
    bool        isErasebkg;         // 处理背景事件, 如果这个为false, 则底层不处理WM_ERASEBACKGROUND 消息
    bool        isCheckBox;         // 是否有选择框

    int         fmt;                // 文本对齐模式
    int         nItemHeight;        // 项目高度
    RECT        offsetRect;         // 窗口位置偏移, 会在 WM_WINDOWPOSCHANGING 修改 lParam指针的数据

    HTREEITEM   hItemSel;           // 树型框专用, 当前选中表项句柄
    HTREEITEM   hItemHot;           // 树型框专用, 当前热点表项句柄
    HTREEITEM   hItemDown;          // 树型框专用, 当前按下表项句柄

    int         indexSel;           // 列表类组件专用, 当前选中表项索引
    int         indexHot;           // 列表类组件专用, 当前选中表项索引
    int         indexDown;          // 列表类组件专用, 当前选中表项索引

}*LPDRAWINFO;
typedef struct POSINFO
{
    RECT        rc;                 // 对象屏幕位置, 左顶右底
    RECT        rcIng;              // windowchange消息时的位置
    RECT        rcClient;           // 对象客户区位置, 左顶右底
    int         x;                  // 对象左边
    int         y;                  // 对象顶边
    int         width;              // 对象宽度
    int         height;             // 对象高度
}*LPPOSINFO;

struct OBJSTRUCT;
typedef LRESULT(CALLBACK* pfn_SubClassWndProc)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, OBJSTRUCT* pData);
typedef LRESULT(CALLBACK* pfn_CallWindowProc)(WNDPROC lpPrevWndFunc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
typedef LRESULT(CALLBACK* pfn_DefWindowProc)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
typedef LONG_PTR(WINAPI* pfn_SetLong)(HWND hWnd, int nIndex, LONG_PTR dwNewLong);
typedef LONG_PTR(WINAPI* pfn_GetLong)(HWND hWnd, int nIndex);
typedef void (WINAPI* pfn_ColorChange)(OBJSTRUCT* pData);
typedef bool (CALLBACK* pfn_CalcScroll)(void* pScroll, OBJSTRUCT* pData, RECT* prc);
typedef bool (CALLBACK* pfn_DrawScroll)(void* pScroll, OBJSTRUCT* pData, bool isVScroll, HDC hdc, int type, RECT_F* rcDraw);
typedef bool (CALLBACK* pfn_GetScrollRect)(void* pScroll, OBJSTRUCT* pData, bool isVScroll, RECT* prc);

typedef struct OBJSTRUCT
{
    union
    {
        HWND        hObj;               // 对象句柄或者叫窗口句柄
        HWND        hWnd;               // 窗口句柄或者叫对象句柄
    };

    pfn_SubClassWndProc fnWndproc;      // 组件的处理函数, 子类化回调函数会调用这个函数
    pfn_SubClassWndProc fnNotify;       // 组件的通知处理函数, 每一个窗口在收到这个消息时都会获取触发这个消息的窗口数据, 5个参数都是父窗口的参数
    pfn_SubClassWndProc fnDrawItem;     // 组件的绘画处理函数, 每一个窗口在收到这个消息时都会获取触发这个消息的窗口数据, 5个参数都是父窗口的参数
    pfn_SubClassWndProc fnMeasureItem;  // 组件的菜单重新计算函数, 每一个窗口在收到这个消息时都会获取触发这个消息的窗口数据, 5个参数都是父窗口的参数
    pfn_SubClassWndProc fnCommand;      // 组件的WM_COMMAND消息, 5个参数都是父窗口的参数, pData参数也是父窗口的数据
    pfn_SubClassWndProc fnCtlcolor;     // 组件的WM_ERASEBKGND消息, 5个参数都是父窗口的参数, pData参数也是父窗口的数据
    pfn_CallWindowProc  fnCallProc;     // 调用原函数使用的API, 如果没有设置这个值, 默认会获取 CallWindowProcW
    pfn_DefWindowProc   fnDefProc;      // 默认调用使用的API, 如果没有设置这个值, 默认会获取 DefWindowProcW
    pfn_ColorChange     fnCrChange;     // 颜色改变函数, 有些组件使用的不是默认配色, 配色改变后需要调用这个来更改配色
    //pfn_CalcScroll      fnCalcScroll;   // 计算滚动条位置, 可以通过这个事件修改窗口非客户区位置
    //pfn_DrawScroll      fnDrawScroll;   // 绘画滚动条
    //pfn_GetScrollRect   fnGetScrollRect;// 获取滚动条位置, 可以通过这个事件修改边框达到指定绘画滚动条位置


    pfn_SetLong         SetWndLong;     // 设置窗口属性, 已经区分了W版和A版
    pfn_GetLong         GetWndLong;     // 获取窗口属性, 已经区分了W版和A版
    pfn_SetLong         SetClsLong;     // 设置类属性, 已经区分了W版和A版
    pfn_GetLong         GetClsLong;     // 获取类属性, 已经区分了W版和A版

    LRESULT             lResult;        // 调用后消息事件, 调用原过程的返回值
    bool                isReturn;       // 拦截消息, 使用回调的消息返回
    void*               drawScroll;     // 绘制滚动条的类, 需要强转成 IDrawScroll*

    WNDPROC oldProc;                    // 组件原来的消息回调处理函数

    DRAWINFO    draw;                   // 绘画信息
    POSINFO     pos;                    // 位置信息
    LONG_PTR    style;                  // 对象样式
    LONG_PTR    styleEx;                // 对象扩展样式
    HCURSOR     hCursor;                // 鼠标指针
    LONG_PTR    id;                     // 对象ID
    LPVOID      param;                  // 对象附加数据
    LPVOID      param1;                 // 对象附加数据1
    LPVOID      param2;                 // 对象附加数据2
    LPVOID      userdata;               // 用户数据, 内部不能占用这个值
    LPWSTR      title;                  // 对象标题
    LPWSTR      tips;                   // 对象提示信息
    ATOM        atom;                   // 类原子
    size_t      index;                  // 列表/工具条等有数量的控件使用, 当前索引
    size_t      count;                  // 列表/工具条等有数量的控件使用, 数量
    DWORD       flags;                  // 窗口标志
    LONG_PTR    winState;               // 窗口当前状态, 最大化, 最小化, 正常
    LPEX_RGN    hrgn_client;            // 窗口客户区区域
    LPEX_RGN    hrgn_sizebox;           // 窗口边框区域
    FLOAT       rectroundX;             // 窗口圆角x
    FLOAT       rectroundY;             // 窗口圆角y
    HWND        hParent;                // 组件父窗口句柄
    struct OBJSTRUCT* pParent;          // 组件父对象信息
#ifdef __cplusplus
    OBJSTRUCT() { memset(this, 0, sizeof(*this)); }
#endif
}*POBJSTRUCT, *LPOBJSTRUCT;

