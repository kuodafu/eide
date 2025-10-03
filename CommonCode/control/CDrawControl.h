#pragma once
#include "WndBase.h"
#include <CThemeColor.h>

class CDrawControl
{
    static constexpr LPCWSTR pszProp = L"{FA4D46F4-C74F-4F2B-9F90-32AE160A5AF9}";
protected:
    struct DRAWCONTROL_STRUCT
    {
        CDrawControl* pObj;
        WNDPROC oldProc;
    };
    template<typename T, typename R, typename _OBJ = std::unordered_map<T, R>>
    class UNMAP : public _OBJ
    {
    public:
        UNMAP() : _OBJ()
        {

        }
        ~UNMAP()
        {
            _OBJ& map = *this;
            for (auto& item : map)
            {
                R hbr = item.second;
                if (hbr)
                    DeleteObject(hbr);
            }
        }
    };

    PTHEMECOLOR_STRUCT m_pClr;
    bool m_isRef;
    HWND m_hWnd;
public:
    CDrawControl() : m_pClr(0), m_isRef(false), m_hWnd(0)
    {

    }
    virtual ~CDrawControl()
    {
        detach();
        if (!m_isRef && m_pClr)
            delete m_pClr;
    }

private:
    //inline static std::unordered_map<HWND, DRAWCONTROL_STRUCT>& GetMap()
    //{
    //    static std::unordered_map<HWND, DRAWCONTROL_STRUCT> map;
    //    return map;
    //}
    // 子类化窗口, 如果已经子类化, 那就返回原来的窗口过程
    static DRAWCONTROL_STRUCT& subclass(HWND hWnd, CDrawControl* pObj = 0)
    {
        DRAWCONTROL_STRUCT* pDrawControl = (DRAWCONTROL_STRUCT*)GetPropW(hWnd, pszProp);
        if (!pDrawControl)
        {
            pDrawControl = new DRAWCONTROL_STRUCT;
            memset(pDrawControl, 0, sizeof(DRAWCONTROL_STRUCT));
            SetPropW(hWnd, pszProp, pDrawControl);
        }

        DRAWCONTROL_STRUCT& data = *pDrawControl;
        if (!data.oldProc && data.oldProc != WindowProc)
        {
            data.oldProc = (WNDPROC)SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)WindowProc);
            data.pObj = pObj;
        }
        return data;
    }
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        DRAWCONTROL_STRUCT* pDrawControl = (DRAWCONTROL_STRUCT*)GetPropW(hWnd, pszProp);
        if (!pDrawControl)
            return DefWindowProcW(hWnd, message, wParam, lParam);

        DRAWCONTROL_STRUCT& wnd_data = *pDrawControl;
        WNDPROC oldProc = wnd_data.oldProc;
        CDrawControl* pObj = wnd_data.pObj;
        if (message == WM_DESTROY && pObj)
        {
            SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)oldProc);
            pObj->WndProc(hWnd, message, wParam, lParam, oldProc);
            RemovePropW(hWnd, pszProp);
            pObj = 0;
        }
        if (pObj)
            return pObj->WndProc(hWnd, message, wParam, lParam, oldProc);
        return 0;
    }

public:
    // 解除绑定
    virtual bool detach()
    {
        if (m_isRef)
            m_pClr = 0;
        DRAWCONTROL_STRUCT* pDrawControl = (DRAWCONTROL_STRUCT*)GetPropW(m_hWnd, pszProp);
        if (pDrawControl)
            pDrawControl->pObj = 0;
        return true;
    }
    // 绑定组件, 绑定后会子类化组件, 然后对组件进行重画操作
    // hWnd = 绑定的窗口, 如果为0则不子类化窗口, 只记录配色信息, 可以在外部调用指定方法处理
    // pClr = 窗口使用的配色结构, 如果为0会从当前进程的map里获取, 获取失败则使用默认色绘画
    // isRef = 参数2是否一直有效, 参数为真的时候内部不会分配配色结构, 会一直使用参数2, 所以为真时请确保参数2指针的存活
    virtual bool bind(HWND hWnd = 0, PTHEMECOLOR_STRUCT pClr = 0, bool isRef = false)
    {
        ChengeColor(pClr, isRef);
        if (IsWindow(hWnd))
        {
            m_hWnd = hWnd;
            DRAWCONTROL_STRUCT& wnd_data = subclass(hWnd, this);
            return wnd_data.oldProc != 0;
        }
        return true;
    }

    // 改变颜色
    virtual void ChengeColor(PTHEMECOLOR_STRUCT pClr, bool isRef = false)
    {
        if (isRef)
        {
            // 当前传递进来的是引用方式, 指针由外部负责, 这里需要判断是否需要删除原来的指针
            if (!m_isRef && m_pClr)
                delete m_pClr;
            m_pClr = pClr;
            m_isRef = isRef;
            return;
        }

        if (!m_pClr)
            m_pClr = new THEMECOLOR_STRUCT{};
        if (pClr)
        {
            memcpy(m_pClr, pClr, sizeof(THEMECOLOR_STRUCT));
        }
        else
        {
            THEMECOLOR_STRUCT clr = { 0 };
            bool isClr = CThemeColor::GetThemeColorFromMap(&clr);
            if (isClr)
                memcpy(m_pClr, &clr, sizeof(THEMECOLOR_STRUCT));
        }
    }
protected:
    virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc)
    {
        switch (message)
        {
        case WM_COMMAND:
            return OnCommand(hWnd, message, wParam, lParam, oldProc);
        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK:
        case WM_MOUSEWHEEL:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
        case WM_XBUTTONDBLCLK:
        case WM_MOUSEHWHEEL:
        case WM_MOUSELEAVE:
            return OnMouse(hWnd, message, wParam, lParam, oldProc);

        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLORSCROLLBAR:
        case WM_CTLCOLORSTATIC:
            return OnCtlColor(hWnd, message, wParam, lParam, oldProc);
        case WM_CLOSE:
            return OnClose(hWnd, message, wParam, lParam, oldProc);
        case WM_PAINT:
            return OnPaint(hWnd, message, wParam, lParam, oldProc);
        case WM_THEMECOLORCHANGED:  // 主题配色被改变, wParam = 是否是深色模式, lParam = THEMECOLOR_STRUCT
            return OnThemeColorChanged(hWnd, message, wParam, lParam, oldProc);
        case WM_NOTIFY:
            return OnNotify(hWnd, message, wParam, lParam, oldProc);
        case WM_DRAWITEM:
            return OnDrawItem(hWnd, message, wParam, lParam, oldProc);

        default:
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        }
        return 0;
    }
    virtual LRESULT OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc)
    {
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    virtual LRESULT OnMouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc)
    {
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    virtual LRESULT OnClose(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc)
    {
        EndDialog(hWnd, 0);
        DestroyWindow(hWnd);
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    virtual LRESULT OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc)
    {
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        PAINTSTRUCT ps = { 0 };
        BeginPaint(hWnd, &ps);
        RECT rc = { 0 };
        GetClientRect(hWnd, &rc);

        HBRUSH hbrBack = m_pClr ? GetBrush(m_pClr->crBack) : (HBRUSH)GetStockObject(WHITE_BRUSH);

        FillRect(ps.hdc, &rc, hbrBack);
        EndPaint(hWnd, &ps);
        return 0;
    }

    virtual LRESULT OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc)
    {
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    virtual LRESULT OnDrawItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc)
    {
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }

public:
    virtual LRESULT OnCtlColor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc)
    {
        if (!m_pClr)
            return (LRESULT)GetStockObject(WHITE_BRUSH);

        HWND hControl = (HWND)lParam;
        HDC hdc = (HDC)wParam;
        THEMECOLOR_STRUCT& clr = *m_pClr;

        HBRUSH hbrBack = GetBrush(clr.crBack);
        //SetTextColor(hdc, clr.crText);

        COLORREF crText = 0;
        COLORREF crBack = 0;
        if (!IsWindowEnabled(hControl))
        {
            hbrBack = GetBrush(clr.crBan);
            crText = clr.crTextBan;
            crBack = clr.crBan;
        }
        else
        {
            hbrBack = GetBrush(clr.crBack);
            crText = clr.crText;
            crBack = clr.crBack;
        }
        //SetBkMode(hdc, TRANSPARENT);
        SetBkColor(hdc, crBack);
        SetTextColor(hdc, crText);
        return (INT_PTR)hbrBack;
    }
    virtual LRESULT OnThemeColorChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc)
    {
        if (m_pClr)
        {
            PTHEMECOLOR_STRUCT pClr = (PTHEMECOLOR_STRUCT)lParam;
            memcpy(m_pClr, pClr, sizeof(THEMECOLOR_STRUCT));
            UpdateAllWindow(hWnd, true);
        }
        return 0;
    }

public:
    // 检测两个矩形是否有相交
    inline static bool Rect_isOverlap(const RECT& rc1, const RECT& rc2)
    {
        return (rc1.right > rc2.left && rc2.right > rc1.left
                && rc1.bottom > rc2.top && rc2.bottom > rc1.top);
    }
public:
    static HBRUSH GetBrush(COLORREF clr)
    {
        static UNMAP<COLORREF, HBRUSH> map;
        auto it = map.find(clr);
        if (it != map.end())
            return it->second;
        HBRUSH hbr = CreateSolidBrush(clr);
        map[clr] = hbr;
        return hbr;
    }

    static HPEN GetPen(int iStyle, int cWidth, COLORREF clr)
    {
        DWORD hash = iStyle + cWidth + clr;
        static UNMAP<COLORREF, HPEN> map;
        auto it = map.find(hash);
        if (it != map.end())
            return it->second;
        HPEN hPen = CreatePen(iStyle, cWidth, clr);
        map[hash] = hPen;
        return hPen;
    }

    // 重画这个窗口以及下面的所有子窗口
    // hWnd = 被重画的窗口, 会递归枚举这个窗口以下的子窗口
    // isSetWindowPos = 是否调用 SetWindowPos 改变非客户区
    static void UpdateAllWindow(HWND hWnd, bool isSetWindowPos)
    {
        if (!hWnd)
            return;

        HWND hChild = GetWindow(hWnd, GW_CHILD);
        while (hChild)
        {
            UpdateAllWindow(hChild, isSetWindowPos);
            hChild = GetWindow(hChild, GW_HWNDNEXT);
        }

        if (isSetWindowPos)
            SetWindowPos(hWnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME);
        InvalidateRect(hWnd, 0, 0);
    }
};

