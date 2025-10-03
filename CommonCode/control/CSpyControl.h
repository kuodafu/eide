#pragma once
#include "WndBase.h"
typedef int(CALLBACK* PFN_SpyControlDown)(HWND hWnd, BOOL isRDown, LPARAM param);
typedef int(CALLBACK* PFN_SpyControlUp)(HWND hWnd, BOOL isRUp, LPARAM param);
typedef int(CALLBACK* PFN_SpyControlMove)(HWND hWnd, WPARAM wParam, LPARAM lParam, LPARAM param);
typedef int(CALLBACK* PFN_SpyControlDraw)(HWND hWnd, HDC hdc, int cxClient, int cyClient, STATE state, LPARAM param);
typedef int(CALLBACK* PFN_SpyControlLeave)(HWND hWnd, LPARAM param);
typedef int(CALLBACK* PFN_SpyControlHover)(HWND hWnd, LPARAM param);

class CSpyControl : public CControlBase
{
private:
// 查询 _flags 是否包含 _flag
// _flags = 被查询的值
// _flag = 查询的值
#define __query(_flags, _flag)    (((_flags) & (_flag)) == (_flag))

// 把 _flag 添加到 _flags 里
// _flags = 添加后保存的值
// _flag = 要添加的值
#define __add_state(_flags, _flag)      _flags = (STATE)((_flags) | (_flag))

// 把 _flag 从 _flags 里删除
// _flags = 删除后保存的值
// _flag = 要删除的值
#define __del_state(_flags, _flag)      _flags = (STATE)(_flags & ~(_flag))

    typedef struct SPYCONTROL_DATA 
    {
        STATE       state;          // 当前组件的状态
        HCURSOR     hCursor;        // 使用的鼠标指针, 正常状态, 默认手型
        HCURSOR     hCursorDown;    // 使用的鼠标指针, 按下状态, 按下的时候组件隐藏图标, 鼠标指针变成这个图标, 默认是手型


        PFN_SpyControlDown  pfnDown;        // 按下事件
        LPARAM              nDownParam;     // 按下事件参数
        PFN_SpyControlMove  pfnMove;        // 移动事件
        LPARAM              nMoveParam;     // 移动事件参数
        PFN_SpyControlUp    pfnUp;          // 放开事件
        LPARAM              nUpParam;       // 放开事件参数
        PFN_SpyControlDraw  pfnDraw;        // 绘画事件, 返回是否拦截
        LPARAM              nDrawParam;     // 绘画事件参数
        PFN_SpyControlLeave pfnLeave;       // 鼠标离开事件, 返回是否拦截
        LPARAM              nLeaveParam;    // 鼠标离开事件参数
        PFN_SpyControlHover pfnHover;       // 鼠标进入事件, 返回是否拦截
        LPARAM              nHoverParam;    // 鼠标进入事件参数

    }*LPSPYCONTROL_DATA;

    LPSPYCONTROL_DATA m_data;
protected:
    virtual inline LPCWSTR SuperClassName() { return L"static"; }

public:
    CSpyControl() : m_data(0), CControlBase() { ; }
    virtual ~CSpyControl() { ; }
public:
    virtual inline LPCWSTR className() { return L"spyWindowControl"; }
    virtual HWND create(int x, int y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR id = 0, LPVOID lpParam = 0)
    {
        DWORD dwStyle = WS_CLIPSIBLINGS | WS_VISIBLE | WS_CHILD | SS_NOTIFY;
        DWORD dwExStyle = 0;
        return CWndBase::create(dwExStyle, 0, dwStyle, x, y, nWidth, nHeight, hWndParent, id, lpParam);
    }
    virtual LONG_PTR SetCursor(HCURSOR hCursor)
    {
        LPSPYCONTROL_DATA pData = GetData();
        LONG_PTR ret = 0;
        if (pData)
        {
            ret = (LONG_PTR)pData->hCursor;
            pData->hCursor = hCursor;
        }
        return ret;
    }
    virtual LONG_PTR SetCursorDown(HCURSOR hCursor)
    {
        LPSPYCONTROL_DATA pData = GetData();
        LONG_PTR ret = 0;
        if (pData)
        {
            ret = (LONG_PTR)pData->hCursorDown;
            pData->hCursorDown = hCursor;
        }
        return ret;
    }

#define _FUN_DEF(_d)    \
    inline void Event_##_d(PFN_SpyControl##_d pfn, LPARAM param = 0){\
        LPSPYCONTROL_DATA pData = GetData();\
        if (pData){\
            pData->pfn##_d = pfn;\
            pData->n##_d##Param = param;\
        }\
    }
    _FUN_DEF(Down)
    _FUN_DEF(Up)
    _FUN_DEF(Move)
    _FUN_DEF(Draw)
    _FUN_DEF(Leave)
    _FUN_DEF(Hover)


private:
    inline void FreeData()
    {
        if (m_data)
            delete m_data;
        m_data = 0;
    }
    inline LPSPYCONTROL_DATA GetData()
    {
        if (!m_data)
        {
            m_data = new SPYCONTROL_DATA;
            memset(m_data, 0, sizeof(SPYCONTROL_DATA));
            m_data->hCursor = LoadCursorW(0, IDC_HAND);
            m_data->hCursorDown = LoadCursorW(0, IDC_HAND);
        }
        return m_data;
    }
    virtual LRESULT WndProc(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        LPSPYCONTROL_DATA pData = GetData();
        switch (message)
        {
        case WM_ENABLE:
            InvalidateRect();
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        case WM_MOUSEMOVE:
            return _move_(oldProc, hWnd, message, wParam, lParam, pData);
        case WM_MOUSELEAVE:
        case WM_NCMOUSELEAVE:
            return _leave_(oldProc, hWnd, message, wParam, lParam, pData);
        case WM_MOUSEHOVER:
        case WM_NCMOUSEHOVER:
        {
            break;
        }
        case WM_RBUTTONDOWN:
        case WM_LBUTTONDOWN:
            return _down_(oldProc, hWnd, message, wParam, lParam, pData);

            // 左键放开和右键放开都只是释放鼠标, 让释放鼠标的事件去处理放开
        case WM_RBUTTONUP:
        case WM_LBUTTONUP:
        {
            ::ReleaseCapture();
            __del_state(pData->state, STATE::DOWN); // 删除按下状态
            ::InvalidateRect(hWnd, 0, 0);
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        }
        case WM_CAPTURECHANGED:
            return _capture_release_(oldProc, hWnd, message, wParam, lParam, pData);
        case WM_ERASEBKGND:
            return 1;
        case WM_DESTROY:
            FreeData();
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);

        case WM_PAINT:
        {
            RECT rc;
            PAINTSTRUCT ps;
            BeginPaint(hWnd, &ps);
            GetClientRect(hWnd, &rc);
            const int cxClient = rc.right - rc.left;
            const int cyClient = rc.bottom - rc.top;
            LPMEMDCBITMAP md = md_create(cxClient, cyClient);
            HDC hdc = md->hdc;

            if (!pData->pfnDraw || pData->pfnDraw(hWnd, hdc, cxClient, cyClient, pData->state, pData->nDrawParam) == 0)
            {
                HBRUSH hbrBack = GetStockBrush(WHITE_BRUSH);
                HGDIOBJ hbrOld = SelectObject(hdc, hbrBack);
                HCURSOR hCursor = __query(pData->state, STATE::DOWN) ? 0 : pData->hCursor;

                Rectangle(hdc, 0, 0, cxClient, cyClient);



                MoveToEx(hdc, 0, cyClient / 2, 0);
                LineTo(hdc, cxClient, cyClient / 2);
                MoveToEx(hdc, cxClient / 2, 0, 0);
                LineTo(hdc, cxClient / 2, cyClient);

                if (hCursor)    // 有鼠标指针就绘画, 没有就不处理
                    DrawIconEx(hdc, 2, 2, hCursor, 0, 0, 0, 0, DI_NORMAL | DI_DEFAULTSIZE);
                SelectObject(hdc, hbrOld);
            }

            BitBlt(ps.hdc, 0, 0, cxClient, cyClient, hdc, 0, 0, SRCCOPY);
            EndPaint(hWnd, &ps);
            return 0;
        }
        default:
            break;
        }
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }

    inline static LRESULT _move_(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPSPYCONTROL_DATA pData)
    {
        if (!pData->pfnMove || pData->pfnMove(hWnd, wParam, lParam, pData->nMoveParam) == 0)
        {
            // 内部处理的代码
        }

        if (!__query(pData->state, STATE::HOVER))  // 没有进入状态就加上一个进入状态
        {
            __add_state(pData->state, STATE::HOVER | STATE::HOT);    // 添加进入和热点状态

            TRACKMOUSEEVENT tme = { 0 };
            tme.cbSize      = sizeof(tme);
            tme.dwFlags     = TME_LEAVE | TME_HOVER;
            tme.dwHoverTime = 500;    // 鼠标进入多长时间后触发进入事件,单位为毫秒,HOVER_DEFAULT为系统默认
            tme.hwndTrack   = hWnd;
            TrackMouseEvent(&tme);
            ::InvalidateRect(hWnd, 0, 0);

            // 鼠标进入的时候就设置鼠标指针为正常, 鼠标按下的时候设置为按下
            SetClassLongPtrW(hWnd, GCLP_HCURSOR, (LONG_PTR)pData->hCursor);

            if (!pData->pfnHover || pData->pfnHover(hWnd, pData->nHoverParam) == 0)
            {

            }
            
        }
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }

    inline static LRESULT _leave_(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPSPYCONTROL_DATA pData)
    {
        __del_state(pData->state, STATE::HOVER | STATE::HOT);   // 鼠标离开, 删除进入和热点状态
        ::InvalidateRect(hWnd, 0, 0);
        SetClassLongPtrW(hWnd, GCLP_HCURSOR, (LONG_PTR)pData->hCursor); // 设置鼠标指针为正常

        if (!pData->pfnLeave || pData->pfnLeave(hWnd, pData->nLeaveParam) == 0)
        {

        }

        return 0;
    }

    // 左键, 右键执行的流程都一样
    inline static LRESULT _down_(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPSPYCONTROL_DATA pData)
    {
        __add_state(pData->state, STATE::DOWN); // 添加按下状态
        const int isRDown = (message == WM_RBUTTONDOWN ? 1 : 0);
        ::SetCursor(pData->hCursorDown);    // 设置鼠标指针为按下
        ::SetCapture(hWnd);                 // 捕获鼠标


        if (isRDown)
            __add_state(pData->state, BUSYING);
        else
            __del_state(pData->state, BUSYING);

        if (!pData->pfnDown || pData->pfnDown(hWnd, isRDown, pData->nDownParam) == 0)
        {

        }
        ::InvalidateRect(hWnd, 0, 0);       // 重画组件
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }

    // 是鼠标释放事件调用, 鼠标放开消息只负责释放鼠标, 鼠标放开事件有可能会接收不到, 释放消息一般不会漏
    inline static LRESULT _capture_release_(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPSPYCONTROL_DATA pData)
    {
        if (!pData->pfnUp || pData->pfnUp(hWnd, __query(pData->state, BUSYING), pData->nUpParam) == 0)
        {

        }
        if (__query(pData->state, STATE::DOWN))
            ::InvalidateRect(hWnd, 0, 0);       // 有按下状态就重画按钮, 没有就不重画
        __del_state(pData->state, STATE::DOWN); // 删除按下状态
        __del_state(pData->state, BUSYING);
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }

};