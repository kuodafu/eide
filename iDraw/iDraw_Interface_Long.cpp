#if 1
#include "EWindow_Fne_Main.h"


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取子类化返回的指针的数据, 或者是子类化接收消息回调函数里的第五个参数
// 
// 参数:
//      参数1: 子类化返回的指针, 或者是子类化接收消息回调函数里的第五个参数
//      参数2: 数据索引, 具体请参考 IGL_ 开头常量
// 
// 返回值:
//      获取到的值
EXTERN_C _declspec(dllexport) LONG_PTR WINAPI iDraw_GetLong(LPOBJSTRUCT pData, int index)
{
    if (!pData)return 0;
    switch (index)
    {
    case IGL_HWND:       // 窗口句柄
        return (LONG_PTR)pData->hWnd;
    case IGL_WNDPROC:       // 接收子类化的函数地址, iDraw_SubclassWindow() 函数的第二个参数记录在这
        return (LONG_PTR)pData->fnWndproc;
    case IGL_CALLBACK_NOTIFY:       // 通知消息函数, 由父窗口调用, 函数原型 LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
        return (LONG_PTR)pData->fnNotify;
    case IGL_CALLBACK_DRAWITEM:       // 绘画消息函数, 由父窗口调用, 函数原型 LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
        return (LONG_PTR)pData->fnDrawItem;
    case IGL_CALLBACK_MEASUREITEM:       // 计算尺寸消息函数, 由父窗口调用, 函数原型 LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
        return (LONG_PTR)pData->fnMeasureItem;
    case IGL_CALLBACK_COMMAND:       // 事件消息函数, 由父窗口调用, 函数原型 LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
        return (LONG_PTR)pData->fnCommand;
    case IGL_CALLBACK_CTLCOLOR:       // 颜色消息函数, 由父窗口调用, 函数原型 LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
        return (LONG_PTR)pData->fnCtlcolor;
    case IGL_CALLBACK_CALLWINDOWPROC:       // CallWindowProc 使用W版还是A版, 在调用子类化时会根据窗口是否为unicode版记录
        return (LONG_PTR)pData->fnCallProc;
    case IGL_INTERCEPT:       // 是否拦截此消息, 挂接消息时才会使用这个值
        return (LONG_PTR)pData->isReturn;
    case IGL_OLDPROC:       // 窗口的原回调函数
        return (LONG_PTR)pData->oldProc;
    case IGL_CALLPROC_RETURN:       // 调用原回调函数的返回值, 注册处理后消息事件下可以通过这个索引取到返回值
        return (LONG_PTR)pData->lResult;
    case IGL_CANVAS:            // 与窗口关联的dx绘画句柄, 如果窗口不是使用dx渲染, 这个值为0
        return (LONG_PTR)pData->draw.hCanvas;
    case IGL_USERDATA:          // 与窗口关联的用户数据
        return (LONG_PTR)pData->userdata;

    case IGL_DRAW_NONCPAINT:       // 不重画非客户区, 为true时接收子类化消息的函数不会收到 WM_NCPAINT 消息
        return (LONG_PTR)pData->draw.isNoNcPaint;
    case IGL_DRAW_NOCALCSIZE:       // 不计算非客户区, 为true时接收子类化消息的函数不会收到 WM_NCCALCSIZE 消息, 窗口不会有边框
        return (LONG_PTR)pData->draw.isNcCalcSize;
    case IGL_DRAW_ISTRANSPARENT:       // 是否透明, 如果为true, 在内置的 CTLCOLOR 中会调用 SetMode() 设置为透明
        return (LONG_PTR)pData->draw.isTransparent;
    case IGL_DRAW_ISHOT:       // 是否允许热点, 一般是列表相关的组件使用
        return (LONG_PTR)pData->draw.isHot;
    case IGL_DRAW_ISCLIENT:       // 是否把所有鼠标消息都视为客户区, 为true则不会收到 WM_NCHITTEST消息, 内部全部返回 HTCLIENT
        return (LONG_PTR)pData->draw.isClient;
    case IGL_DRAW_ISERASEBKG:       // 是否处理重画背景消息, 如果为true, 则窗口会收到 WM_ERASEBACKGROUND 消息, 否则内部返回1
        return (LONG_PTR)pData->draw.isErasebkg;
    case IGL_DRAW_STATE:       // 当前组件的状态
        return (LONG_PTR)pData->draw.state;
    case IGL_DRAW_FONT_DX:       // 获取DX字体
        return (LONG_PTR)pData->draw.hFont;
    case IGL_DRAW_FONT:       // 获取GDI字体
        return (LONG_PTR)(pData->draw.hFont ? pData->draw.hFont->GetFont() : 0);
    case IGL_DRAW_TEXTFORMAT:       // 绘画文本对齐格式
        return (LONG_PTR)pData->draw.textFormat;

    case IGL_DRAW_CRBACKGROUND:       // 背景颜色, 每个组件都会使用这个颜色来绘画背景
        return (LONG_PTR)pData->draw.color.crBackGround;
    case IGL_DRAW_CRTEXT:       // 文本颜色, 每个组件的正常文本颜色都是使用这个颜色绘画
        return (LONG_PTR)pData->draw.color.crText;
    case IGL_DRAW_CRTEXTHOT:       // 文本热点颜色, 哪个组件用到请参考资料
        return (LONG_PTR)pData->draw.color.crTextHot;
    case IGL_DRAW_CRTEXTDOWN:       // 文本按下颜色, 哪个组件用到请参考资料
        return (LONG_PTR)pData->draw.color.crTextDown;
    case IGL_DRAW_CRTEXTBAN:       // 文本禁止颜色, 哪个组件用到请参考资料
        return (LONG_PTR)pData->draw.color.crTextBan;
    case IGL_DRAW_CRNORMAL:       // 组件正常颜色, 哪个组件用到请参考资料
        return (LONG_PTR)pData->draw.color.crNormal;
    case IGL_DRAW_CRHOVER:       // 组件进入颜色, 哪个组件用到请参考资料
        return (LONG_PTR)pData->draw.color.crHover;
    case IGL_DRAW_CRDOWN:       // 组件按下颜色, 哪个组件用到请参考资料
        return (LONG_PTR)pData->draw.color.crDown;
    case IGL_DRAW_CRFOCUS:       // 组件焦点颜色, 哪个组件用到请参考资料
        return (LONG_PTR)pData->draw.color.crFocus;
    case IGL_DRAW_CRCHECKED:       // 组件选中颜色, 哪个组件用到请参考资料
        return (LONG_PTR)pData->draw.color.crChecked;
    case IGL_DRAW_CRSELECTED:       // 组件选择颜色, 哪个组件用到请参考资料
        return (LONG_PTR)pData->draw.color.crSelected;
    case IGL_DRAW_CRHOT:       // 组件热点颜色, 哪个组件用到请参考资料
        return (LONG_PTR)pData->draw.color.crHot;
    case IGL_DRAW_CRBAN:       // 组件禁止颜色, 哪个组件用到请参考资料
        return (LONG_PTR)pData->draw.color.crBan;
    case IGL_DRAW_CRVISTED:       // 
        return (LONG_PTR)pData->draw.color.crVisted;
    case IGL_DRAW_CRSHADOW:       // 组件引用颜色, 哪个组件用到请参考资料
        return (LONG_PTR)pData->draw.color.crShadow;
    case IGL_DRAW_CRBORDER:       // 组件边框颜色, 所有绘制边框使用的颜色
        return (LONG_PTR)pData->draw.color.crBorder;

    case IGL_DRAW_HBRBACKGROUND:       // 背景GDI画刷, 每个组件都会使用这个画刷来填充背景
        return (LONG_PTR)pData->draw.color.hbrBackGround;
    case IGL_DRAW_HBRTEXT:       // 文本GDI画刷
        return (LONG_PTR)pData->draw.color.hbrText;
    case IGL_DRAW_HBRTEXTHOT:       // 文本热点GDI画刷, 哪个组件用到请参考资料
        return (LONG_PTR)pData->draw.color.hbrTextHot;
    case IGL_DRAW_HBRTEXTDOWN:       // 文本按下GDI画刷, 哪个组件用到请参考资料
        return (LONG_PTR)pData->draw.color.hbrTextDown;
    case IGL_DRAW_HBRTEXTBAN:       // 文本禁止GDI画刷, 哪个组件用到请参考资料
        return (LONG_PTR)pData->draw.color.hbrTextBan;
    case IGL_DRAW_HBRNORMAL:       // 组件正常GDI画刷, 哪个组件用到请参考资料
        return (LONG_PTR)pData->draw.color.hbrNormal;
    case IGL_DRAW_HBRHOVER:       // 组件进入GDI画刷, 哪个组件用到请参考资料
        return (LONG_PTR)pData->draw.color.hbrHover;
    case IGL_DRAW_HBRDOWN:       // 组件按下GDI画刷, 哪个组件用到请参考资料
        return (LONG_PTR)pData->draw.color.hbrDown;
    case IGL_DRAW_HBRFOCUS:       // 组件焦点GDI画刷, 哪个组件用到请参考资料
        return (LONG_PTR)pData->draw.color.hbrFocus;
    case IGL_DRAW_HBRCHECKED:       // 组件选中GDI画刷, 哪个组件用到请参考资料
        return (LONG_PTR)pData->draw.color.hbrChecked;
    case IGL_DRAW_HBRSELECTED:       // 组件选择GDI画刷, 哪个组件用到请参考资料
        return (LONG_PTR)pData->draw.color.hbrSelected;
    case IGL_DRAW_HBRHOT:       // 组件热点GDI画刷, 哪个组件用到请参考资料
        return (LONG_PTR)pData->draw.color.hbrHot;
    case IGL_DRAW_HBRBAN:       // 组件禁止GDI画刷, 哪个组件用到请参考资料
        return (LONG_PTR)pData->draw.color.hbrBan;
    case IGL_DRAW_HBRVISTED:       // 
        return (LONG_PTR)pData->draw.color.hbrVisted;
    case IGL_DRAW_HBRSHADOW:       // 组件阴影GDI画刷, 哪个组件用到请参考资料
        return (LONG_PTR)pData->draw.color.hbrShadow;
    case IGL_DRAW_HBRBORDER:       // 组件边框GDI画笔, 所有绘制边框使用的画笔
        return (LONG_PTR)pData->draw.color.hpenBorder;

    case IGL_POS_RECTSCREEN:       // 窗口在屏幕中的位置, 返回RECT结构指针, 不允许修改这个指针, 只能获取不能设置
        return (LONG_PTR)&pData->pos.rc;
    case IGL_POS_RECTCLIENT:       // 窗口在父窗口中的位置, 返回RECT结构指针, 不允许修改这个指针, 只能获取不能设置
        return (LONG_PTR)&pData->pos.rcClient;
    case IGL_POS_X:       // 窗口在父窗口中的左边位置, 只能获取不能设置
        return (LONG_PTR)pData->pos.x;
    case IGL_POS_Y:       // 窗口在父窗口中的顶边位置, 只能获取不能设置
        return (LONG_PTR)pData->pos.y;
    case IGL_POS_WIDTH:       // 窗口的宽度 只能获取不能设置
        return (LONG_PTR)pData->pos.width;
    case IGL_POS_HEIGHT:       // 窗口的高度, 只能获取不能设置
        return (LONG_PTR)pData->pos.height;
    default:
        break;
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      设置子类化返回的指针的数据, 或者是子类化接收消息回调函数里的第五个参数, 返回设置前的数据
// 
// 参数:
//      参数1: 子类化返回的指针, 或者是子类化接收消息回调函数里的第五个参数
//      参数2: 数据索引, 具体请参考 IGL_ 开头常量
//      参数3: 要设置的数据
// 
// 返回值:
//      返回设置前的数据
EXTERN_C _declspec(dllexport) LONG_PTR WINAPI iDraw_SetLong(LPOBJSTRUCT pData, int index, LONG_PTR value)
{
    if (!pData)return 0;
    LONG_PTR oldData = 0;
    switch (index)
    {
    case IGL_HWND:       // 窗口句柄
        return 0;
    case IGL_WNDPROC:       // 接收子类化的函数地址, iDraw_SubclassWindow() 函数的第二个参数记录在这
        oldData = (LONG_PTR)pData->fnWndproc;
        pData->fnWndproc = (pfn_SubClassWndProc)value;
        break;
    case IGL_CALLBACK_NOTIFY:       // 通知消息函数, 由父窗口调用, 函数原型 LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
        oldData = (LONG_PTR)pData->fnNotify;
        pData->fnNotify = (pfn_SubClassWndProc)value;
        break;
    case IGL_CALLBACK_DRAWITEM:       // 绘画消息函数, 由父窗口调用, 函数原型 LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
        oldData = (LONG_PTR)pData->fnDrawItem;
        pData->fnDrawItem = (pfn_SubClassWndProc)value;
        break;
    case IGL_CALLBACK_MEASUREITEM:       // 计算尺寸消息函数, 由父窗口调用, 函数原型 LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
        oldData = (LONG_PTR)pData->fnMeasureItem;
        pData->fnMeasureItem = (pfn_SubClassWndProc)value;
        break;
    case IGL_CALLBACK_COMMAND:       // 事件消息函数, 由父窗口调用, 函数原型 LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
        oldData = (LONG_PTR)pData->fnCommand;
        pData->fnCommand = (pfn_SubClassWndProc)value;
        break;
    case IGL_CALLBACK_CTLCOLOR:       // 颜色消息函数, 由父窗口调用, 函数原型 LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
        oldData = (LONG_PTR)pData->fnCtlcolor;
        pData->fnCtlcolor = (pfn_SubClassWndProc)value;
        break;
    case IGL_CALLBACK_CALLWINDOWPROC:       // CallWindowProc 使用W版还是A版, 在调用子类化时会根据窗口是否为unicode版记录
        oldData = (LONG_PTR)pData->fnCallProc;
        pData->fnCallProc = (pfn_CallWindowProc)value;
        break;
    case IGL_INTERCEPT:       // 是否拦截此消息, 挂接消息时才会使用这个值
        oldData = (LONG_PTR)pData->isReturn;
        pData->isReturn = value;
        break;
    case IGL_OLDPROC:       // 窗口的原回调函数
        oldData = (LONG_PTR)pData->oldProc;
        pData->oldProc = (WNDPROC)value;
        break;
    case IGL_CANVAS:            // 与窗口关联的dx绘画句柄, 如果窗口不是使用dx渲染, 这个值为0
        oldData = (LONG_PTR)pData->draw.hCanvas;
        pData->draw.hCanvas = (LPCANVAS)value;
        break;
    case IGL_USERDATA:          // 与窗口关联的用户数据
        oldData = (LONG_PTR)pData->userdata;
        pData->userdata = (LPVOID)value;
        break;

    case IGL_DRAW_NONCPAINT:       // 不重画非客户区, 为true时接收子类化消息的函数不会收到 WM_NCPAINT 消息
        oldData = (LONG_PTR)pData->draw.isNoNcPaint;
        pData->draw.isNoNcPaint = value;
        break;
    case IGL_DRAW_NOCALCSIZE:       // 不计算非客户区, 为true时接收子类化消息的函数不会收到 WM_NCCALCSIZE 消息, 窗口不会有边框
        oldData = (LONG_PTR)pData->draw.isNcCalcSize;
        pData->draw.isNcCalcSize = value;
        break;
    case IGL_DRAW_ISTRANSPARENT:       // 是否透明, 如果为true, 在内置的 CTLCOLOR 中会调用 SetMode() 设置为透明
        oldData = (LONG_PTR)pData->draw.isTransparent;
        pData->draw.isTransparent = value;
        break;
    case IGL_DRAW_ISHOT:       // 是否允许热点, 一般是列表相关的组件使用
        oldData = (LONG_PTR)pData->draw.isHot;
        pData->draw.isHot = value;
        break;
    case IGL_DRAW_ISCLIENT:       // 是否把所有鼠标消息都视为客户区, 为true则不会收到 WM_NCHITTEST消息, 内部全部返回 HTCLIENT
        oldData = (LONG_PTR)pData->draw.isClient;
        pData->draw.isClient = value;
        break;
    case IGL_DRAW_ISERASEBKG:       // 是否处理重画背景消息, 如果为true, 则窗口会收到 WM_ERASEBACKGROUND 消息, 否则内部返回1
        oldData = (LONG_PTR)pData->draw.isErasebkg;
        pData->draw.isErasebkg = value;
        break;
    case IGL_DRAW_STATE:       // 当前组件的状态
        oldData = (LONG_PTR)pData->draw.state;
        pData->draw.state = (STATE)value;
        break;
    case IGL_DRAW_FONT_DX:       // 获取DX字体
    case IGL_DRAW_FONT:       // 获取GDI字体
        break;
    case IGL_DRAW_TEXTFORMAT:       // 绘画文本对齐格式
        oldData = (LONG_PTR)pData->draw.textFormat;
        pData->draw.textFormat = (DWORD)value;
        break;

    case IGL_DRAW_CRBACKGROUND:       // 背景颜色, 每个组件都会使用这个颜色来绘画背景
        oldData = (LONG_PTR)pData->draw.color.crBackGround;
        pData->draw.color.crBackGround = (COLORREF)value;
        break;
    case IGL_DRAW_CRTEXT:       // 文本颜色, 每个组件的正常文本颜色都是使用这个颜色绘画
        oldData = (LONG_PTR)pData->draw.color.crText;
        pData->draw.color.crText = (COLORREF)value;
        break;
    case IGL_DRAW_CRTEXTHOT:       // 文本热点颜色, 哪个组件用到请参考资料
        oldData = (LONG_PTR)pData->draw.color.crTextHot;
        pData->draw.color.crTextHot = (COLORREF)value;
        break;
    case IGL_DRAW_CRTEXTDOWN:       // 文本按下颜色, 哪个组件用到请参考资料
        oldData = (LONG_PTR)pData->draw.color.crTextDown;
        pData->draw.color.crTextDown = (COLORREF)value;
        break;
    case IGL_DRAW_CRTEXTBAN:       // 文本禁止颜色, 哪个组件用到请参考资料
        oldData = (LONG_PTR)pData->draw.color.crTextBan;
        pData->draw.color.crTextBan = (COLORREF)value;
        break;
    case IGL_DRAW_CRNORMAL:       // 组件正常颜色, 哪个组件用到请参考资料
        oldData = (LONG_PTR)pData->draw.color.crNormal;
        pData->draw.color.crNormal = (COLORREF)value;
        break;
    case IGL_DRAW_CRHOVER:       // 组件进入颜色, 哪个组件用到请参考资料
        oldData = (LONG_PTR)pData->draw.color.crHover;
        pData->draw.color.crHover = (COLORREF)value;
        break;
    case IGL_DRAW_CRDOWN:       // 组件按下颜色, 哪个组件用到请参考资料
        oldData = (LONG_PTR)pData->draw.color.crDown;
        pData->draw.color.crDown = (COLORREF)value;
        break;
    case IGL_DRAW_CRFOCUS:       // 组件焦点颜色, 哪个组件用到请参考资料
        oldData = (LONG_PTR)pData->draw.color.crFocus;
        pData->draw.color.crFocus = (COLORREF)value;
        break;
    case IGL_DRAW_CRCHECKED:       // 组件选中颜色, 哪个组件用到请参考资料
        oldData = (LONG_PTR)pData->draw.color.crChecked;
        pData->draw.color.crChecked = (COLORREF)value;
        break;
    case IGL_DRAW_CRSELECTED:       // 组件选择颜色, 哪个组件用到请参考资料
        oldData = (LONG_PTR)pData->draw.color.crSelected;
        pData->draw.color.crSelected = (COLORREF)value;
        break;
    case IGL_DRAW_CRHOT:       // 组件热点颜色, 哪个组件用到请参考资料
        oldData = (LONG_PTR)pData->draw.color.crHot;
        pData->draw.color.crHot = (COLORREF)value;
        break;
    case IGL_DRAW_CRBAN:       // 组件禁止颜色, 哪个组件用到请参考资料
        oldData = (LONG_PTR)pData->draw.color.crBan;
        pData->draw.color.crBan = (COLORREF)value;
        break;
    case IGL_DRAW_CRVISTED:       // 
        oldData = (LONG_PTR)pData->draw.color.crVisted;
        pData->draw.color.crVisted = (COLORREF)value;
        break;
    case IGL_DRAW_CRSHADOW:       // 组件引用颜色, 哪个组件用到请参考资料
        oldData = (LONG_PTR)pData->draw.color.crShadow;
        pData->draw.color.crShadow = (COLORREF)value;
        break;
    case IGL_DRAW_CRBORDER:       // 组件边框颜色, 所有绘制边框使用的颜色
        oldData = (LONG_PTR)pData->draw.color.crBorder;
        pData->draw.color.crBorder = (COLORREF)value;
        break;

    case IGL_DRAW_HBRBACKGROUND:       // 背景GDI画刷, 每个组件都会使用这个画刷来填充背景
        oldData = (LONG_PTR)pData->draw.color.hbrBackGround;
        pData->draw.color.hbrBackGround = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRTEXT:       // 文本GDI画刷
        oldData = (LONG_PTR)pData->draw.color.hbrText;
        pData->draw.color.hbrText = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRTEXTHOT:       // 文本热点GDI画刷, 哪个组件用到请参考资料
        oldData = (LONG_PTR)pData->draw.color.hbrTextHot;
        pData->draw.color.hbrTextHot = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRTEXTDOWN:       // 文本按下GDI画刷, 哪个组件用到请参考资料
        oldData = (LONG_PTR)pData->draw.color.hbrTextDown;
        pData->draw.color.hbrTextDown = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRTEXTBAN:       // 文本禁止GDI画刷, 哪个组件用到请参考资料
        oldData = (LONG_PTR)pData->draw.color.hbrTextBan;
        pData->draw.color.hbrTextBan = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRNORMAL:       // 组件正常GDI画刷, 哪个组件用到请参考资料
        oldData = (LONG_PTR)pData->draw.color.hbrNormal;
        pData->draw.color.hbrNormal = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRHOVER:       // 组件进入GDI画刷, 哪个组件用到请参考资料
        oldData = (LONG_PTR)pData->draw.color.hbrHover;
        pData->draw.color.hbrHover = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRDOWN:       // 组件按下GDI画刷, 哪个组件用到请参考资料
        oldData = (LONG_PTR)pData->draw.color.hbrDown;
        pData->draw.color.hbrDown = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRFOCUS:       // 组件焦点GDI画刷, 哪个组件用到请参考资料
        oldData = (LONG_PTR)pData->draw.color.hbrFocus;
        pData->draw.color.hbrFocus = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRCHECKED:       // 组件选中GDI画刷, 哪个组件用到请参考资料
        oldData = (LONG_PTR)pData->draw.color.hbrChecked;
        pData->draw.color.hbrChecked = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRSELECTED:       // 组件选择GDI画刷, 哪个组件用到请参考资料
        oldData = (LONG_PTR)pData->draw.color.hbrSelected;
        pData->draw.color.hbrSelected = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRHOT:       // 组件热点GDI画刷, 哪个组件用到请参考资料
        oldData = (LONG_PTR)pData->draw.color.hbrHot;
        pData->draw.color.hbrHot = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRBAN:       // 组件禁止GDI画刷, 哪个组件用到请参考资料
        oldData = (LONG_PTR)pData->draw.color.hbrBan;
        pData->draw.color.hbrBan = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRVISTED:       // 
        oldData = (LONG_PTR)pData->draw.color.hbrVisted;
        pData->draw.color.hbrVisted = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRSHADOW:       // 组件阴影GDI画刷, 哪个组件用到请参考资料
        oldData = (LONG_PTR)pData->draw.color.hbrShadow;
        pData->draw.color.hbrShadow = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRBORDER:       // 组件边框GDI画笔, 所有绘制边框使用的画笔
        oldData = (LONG_PTR)pData->draw.color.hpenBorder;
        pData->draw.color.hpenBorder = (HPEN)value;
        break;

    case IGL_POS_RECTSCREEN:    // 窗口在屏幕中的位置, 返回RECT结构指针, 不允许修改这个指针, 只能获取不能设置
    case IGL_POS_RECTCLIENT:    // 窗口在父窗口中的位置, 返回RECT结构指针, 不允许修改这个指针, 只能获取不能设置
    case IGL_POS_X:             // 窗口在父窗口中的左边位置, 只能获取不能设置
    case IGL_POS_Y:             // 窗口在父窗口中的顶边位置, 只能获取不能设置
    case IGL_POS_WIDTH:         // 窗口的宽度 只能获取不能设置
    case IGL_POS_HEIGHT:        // 窗口的高度, 只能获取不能设置
        break;
    default:
        break;
    }
    return oldData;
}

#endif