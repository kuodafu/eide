#pragma once
#include "CDrawEdit.h"
#include <GetDpiForWindow.h>

class CDrawComboBox : public CDrawControl
{
    CDrawEdit m_edit;
protected:

public:
    CDrawComboBox() : CDrawControl()
    {

    }
    virtual ~CDrawComboBox()
    {

    }

    inline static LPCWSTR paopName()
    {
        return L"{005E7DF6-C920-4146-B4C6-AD15417DE176}";
    }
    inline static LPCWSTR paopNameColor()
    {
        return L"{005E7DF6-C920-4146-B4C6-AD1541245176}";
    }
public:

    static void InitEdit(HWND hEdit, PTHEMECOLOR_STRUCT pClr)
    {
        return;
        static WNDPROC pfn_edit_proc = [](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) -> LRESULT
        {
            WNDPROC oldProc = (WNDPROC)GetPropW(hWnd, paopName());
            switch (message)
            {
            case WM_NCCALCSIZE:
                return CDrawEdit::OnNcCalcSize(hWnd, message, wParam, lParam, oldProc);
            case WM_NCPAINT:
            {
                PTHEMECOLOR_STRUCT clr = (PTHEMECOLOR_STRUCT)GetPropW(hWnd, paopNameColor());
                return CDrawEdit::OnNcPaint(hWnd, message, wParam, lParam, oldProc, clr);
            }
            case WM_WINDOWPOSCHANGING:
            {
                //LRESULT ret = CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
                LPWINDOWPOS pos = (LPWINDOWPOS)lParam;
                if (((pos->flags & SWP_NOSIZE) == 0) || ((pos->flags & SWP_NOMOVE) == 0)) // 尺寸/位置被改变了
                {
                    RECT rc;
                    HWND hParent = GetParent(hWnd);
                    GetWindowRect(hParent, &rc);
                    const int cxClient = rc.right - rc.left;
                    const int cyClient = rc.bottom - rc.top;
                    UINT dpi = GetDpiForWindow_d(hWnd);
                    const int offset_cx = MulDiv(18, dpi, USER_DEFAULT_SCREEN_DPI);
                    pos->cx = cxClient - offset_cx;
                    pos->cy = cyClient - 2;
                    pos->x = 1;
                    pos->y = 1;
                }
                return 0;
            }
            case WM_DESTROY:
            {
                SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)oldProc);
                PTHEMECOLOR_STRUCT clr = (PTHEMECOLOR_STRUCT)GetPropW(hWnd, paopNameColor());
                if (clr)
                    delete clr;
                RemovePropW(hWnd, paopNameColor());
                RemovePropW(hWnd, paopName());
                return 0;
            }
            default:
                return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
            }
            return 0;
        };

        // 这里需要对编辑框进行纵向居中处理, 看是要子类化自己弄非客户区还是怎样
        DWORD style = (DWORD)GetWindowLongPtrW(hEdit, GWL_STYLE);
        const DWORD addStyle = WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_AUTOHSCROLL | ES_NOHIDESEL;
        DWORD newStyle = style | addStyle;
        newStyle &= ~(ES_AUTOVSCROLL | ES_WANTRETURN);
        if (newStyle != style)
            SetWindowLongPtrW(hEdit, GWL_STYLE, style | addStyle);

        if (pClr)
        {
            PTHEMECOLOR_STRUCT clr = new THEMECOLOR_STRUCT;
            memcpy(clr, pClr, sizeof(THEMECOLOR_STRUCT));
            SetPropW(hEdit, paopNameColor(), clr);
        }
        WNDPROC oldProc = (WNDPROC)SetWindowLongPtrW(hEdit, GWLP_WNDPROC, (LONG_PTR)pfn_edit_proc);
        SetPropW(hEdit, paopName(), oldProc);

    }

    static void MoveComboboxEdit(HWND hWnd, PTHEMECOLOR_STRUCT pClr)
    {
        RECT rc;
        GetWindowRect(hWnd, &rc);
        const int cxClient = rc.right - rc.left;
        const int cyClient = rc.bottom - rc.top;

        HWND hChild = GetWindow(hWnd, GW_CHILD);
        wchar_t cls[260] = { 0 };
        while (hChild)
        {
            GetClassNameW(hChild, cls, 260);
            if (_wcsicmp(cls, L"edit") == 0)
                break;
            hChild = GetWindow(hChild, GW_HWNDNEXT);
        }
        if (!hChild)
            return;

        InitEdit(hChild, pClr);
        SetWindowPos(hChild, 0, 1, 1, cxClient - 18, cyClient - 2, SWP_NOZORDER | SWP_NOACTIVATE | SWP_DRAWFRAME);

        return;




        ////SendMessageW(hChild, EM_SETMARGINS, EC_LEFTMARGIN, )
        //RECT rc = { 3, 3, cxClient - 19, cyClient - 2 };
        //auto a1 = SendMessageW(hChild, EM_SETRECTNP, 0, (LPARAM)&rc);
        ////RECT rc1;
        ////auto a2 = SendMessageW(hChild, EM_GETRECT, 0, (LPARAM)&rc1);
        ////auto a3 = SendMessageW(hChild, EM_SETMARGINS, EC_LEFTMARGIN, 3);
        //MoveWindow(hChild, 1, 1, cxClient - 18, cyClient - 2, TRUE);
        //

    }

    // 初始化组合框, 就是把组合框里的编辑框设置成填充满整个组合框
    static void InitCombobox(HWND hWnd, PTHEMECOLOR_STRUCT pClr)
    {
        return;
        MoveComboboxEdit(hWnd, pClr);

        //static WNDPROC HexView_ComboboxProc = [](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) -> LRESULT
        //{
        //    WNDPROC oldProc = (WNDPROC)GetPropW(hWnd, paopName());
        //    switch (message)
        //    {
        //    case WM_SIZE:
        //        MoveComboboxEdit(hWnd, pClr);
        //        break;
        //    case WM_DESTROY:
        //        SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)oldProc);
        //        break;
        //    default:
        //        break;
        //    }
        //    return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        //};
        //WNDPROC oldProc = (WNDPROC)SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)HexView_ComboboxProc);
        //SetPropW(hWnd, paopName(), oldProc);
    }




};

