#pragma once
#include "CDrawControl.h"

class CDrawTab : public CDrawControl
{

    int m_indexHot;
public:
    CDrawTab() : CDrawControl(), m_indexHot(-1)
    {

    }
    virtual ~CDrawTab()
    {

    }

public:
    
    virtual LRESULT OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc)
    {
        RECT rc;
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &rc);
        const int cxClient = rc.right - rc.left;
        const int cyClient = rc.bottom - rc.top;

        LPMEMDCBITMAP md = md_create(cxClient, cyClient);
        HDC hdc = md->hdc;
        COLORREF clrHot = RGB(216, 234, 249);
        COLORREF clrBack = RGB(255, 255, 255);
        COLORREF clrTab = RGB(247, 247, 247);
        COLORREF clrText = 0;
        COLORREF clrBorder = RGB(217, 217, 217);

        if (m_pClr)
        {
            clrHot    = m_pClr->crHot;
            clrText   = m_pClr->crText;
            clrBorder = m_pClr->crBorder;
            if (m_pClr->isDarkMode)
            {
                clrBack = RGB(45, 45, 48);
                clrTab = RGB(53, 53, 56);
            }
            else
            {
                clrTab = RGB(238, 238, 242);
                clrBack = RGB(245, 245, 245);
            }
        }

        HBRUSH hbrHot = GetBrush(clrHot);
        HBRUSH hbrBack = GetBrush(clrBack);
        HBRUSH hbrTab = GetBrush(clrTab);
        HPEN hPen = GetPen(PS_SOLID, 1, clrBorder);
        HPEN hPenBack = GetPen(PS_SOLID, 1, clrBack);

        HFONT hFont = (HFONT)SendMessageW(hWnd, WM_GETFONT, 0, 0);
        bool isFreeFont = false;
        if (hFont == 0)
        {
            isFreeFont = true;
            hFont = CWndBase::CreateFontW();
        }

        SelectObject(hdc, hFont);
        SelectObject(hdc, hPen);
        SelectObject(hdc, hbrBack);
        SetBkMode(hdc, TRANSPARENT);
        FillRect(hdc, &rc, hbrTab);
        SetTextColor(hdc, clrText);

        RECT rcSel, rcItem;
        TabCtrl_GetItemRect(hWnd, 0, &rcItem);
        const int tabHeight = rcItem.bottom - rcItem.top;

        Rectangle(hdc, 0, tabHeight - 1, cxClient, cyClient);

        const int index = TabCtrl_GetCurSel(hWnd);          // 当前选择夹选中项
        const int ItemCount = TabCtrl_GetItemCount(hWnd);   // 选择夹项目数
        const DWORD dwStyle = (DWORD)GetWindowLongPtrW(hWnd, GWL_STYLE);
        const bool isBottom = (dwStyle & TCS_BOTTOM) == TCS_BOTTOM; // 是在底边, 左边右边没处理   
        int direction = (dwStyle & TCS_VERTICAL) == TCS_VERTICAL;   // 选项卡方向, 0=左边, 1=顶边, 2=右边, 3=底边
        if (direction)  direction = isBottom ? 2 : 0;
        else            direction = isBottom ? 3 : 1;

        const int bufSize = 0x1000;
        wchar_t text[bufSize] = { 0 };
        TCITEMW item = { 0 };
        item.mask = TCIF_TEXT | TCIF_PARAM | TCIF_IMAGE;
        item.pszText = text;
        item.cchTextMax = bufSize;


        HIMAGELIST hList = TabCtrl_GetImageList(hWnd);
        TabCtrl_GetItemRect(hWnd, index, &rcSel);
        /*
        for (int i = 0; i < ItemCount; i++)
        {
            TabCtrl_GetItemRect(hWnd, i, &rcItem);
            TabCtrl_GetItem(hWnd, i, &item);
            RECT_F rcItemF(rcItem);
            //rcItem.right -= 2;
            if (i == 0)
            {
                // 这里会把子夹整个宽度/高度给填充成 hbrNormal 画刷颜色
                _tab_draw_line_first(hCanvas, direction, rcItemF, ps.rc, pData);
            }
            LPEX_BRUSH hbr = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crNormal, 255));

            COLORREF crText = pData->draw.color.crText;
            if (i == index) // 当前绘制的项目是选中项
            {
                hCanvas->FillRectangle(hbrBack, &rcItemF);
                hbr = hbrBack;
                if (direction == 0)
                    rcItemF.left++;
                _tab_draw_line_sel(hCanvas, direction, rcItemF, hPen);
                crText = pData->draw.color.crTextDown;
                if (direction == 0)
                    rcItemF.left--;
            }
            else if (i == pData->draw.indexHot)
            {
                RECT_F rcTmp = rcItem;
                if (direction == 1 || direction == 3)   // 上下
                {
                    if (i + 1 == index)
                        rcTmp.right = (float)rcSel.left;
                    else if (i - 1 == index)
                        rcTmp.left = (float)rcSel.right;
                }
                else
                {
                    if (i + 1 == index)
                        rcTmp.bottom = (float)rcSel.top;
                    else if (i - 1 == index)
                        rcTmp.top = (float)rcSel.bottom;
                }
                hCanvas->FillRectangle(hbrHot, &rcTmp);
                hbr = hbrHot;

                //_tab_draw_line_sel(hCanvas, direction, rcItem);
                crText = pData->draw.color.crTextHot;
            }

            LPEX_BRUSH hbrText = s_info->pfnCreateBrushDx(RGB2ARGB(crText, 255));
            DWORD argb = hbrText->GetColor();

            _tab_draw_icon(hCanvas, hList, item, rcItemF, direction);

            if (!text[0]) continue;

            LPEX_FONT hFont = g_scale.GetFont(pData->draw.hFont);
            const int fmt = DT_VCENTER | DT_CENTER | DT_SINGLELINE;
            if (direction != 1 && direction != 3)
            {
                // 需要旋转
                LPCANVAS& hCanvasRotate = _Tab_GetCanvas(pData);
                if (hCanvasRotate == 0)
                    hCanvasRotate = _canvas_create(hWnd, 1, 1);

                RECT_F rcRotate(0, 0, rcItemF.height(), rcItemF.width());
                const float nRotate = rcRotate.bottom;
                int maxWidth = (int)max(rcRotate.right, rcRotate.bottom);
                if ((int)hCanvasRotate->width < maxWidth || (int)hCanvasRotate->height < maxWidth)
                    hCanvasRotate->Resize(maxWidth + 20, maxWidth + 20);

                hCanvasRotate->BeginDraw();
                hCanvasRotate->clear(0);

                hCanvasRotate->SetTextRenderingHint(1);
                hCanvasRotate->SetAntialiasMode(0);
                hCanvasRotate->rotate(nRotate, nRotate, 90);
                argb = hbr->GetColor();
                hCanvasRotate->FillRectangle(hbr, &rcRotate);
                hCanvasRotate->drawtext(hFont, hbrText, text, -1, fmt, &rcRotate);
                hCanvasRotate->rotate(0, 0, 0);
                hCanvasRotate->EndDraw();

                if (0)
                {
                    _canvas_alphablend(hCanvas,
                                       rcItemF.left, rcItemF.top, rcItemF.width(), rcItemF.height(),
                                       hCanvasRotate, rcRotate.bottom, 0, rcItemF.width(), rcItemF.height(), 255);
                }
                else
                {
                    //bug 这个透明背景会变黑色.....
                    if (direction == 0)
                    {
                        _canvas_bitblt(hCanvas,
                                       (int)rcItemF.left + 1, (int)rcItemF.top + 1, (int)rcItemF.width() - 3, (int)rcItemF.height() - 2,
                                       hCanvasRotate, (int)rcRotate.bottom + 1, 1, SRCCOPY);
                    }
                    else
                    {
                        _canvas_bitblt(hCanvas,
                                       (int)rcItemF.left + 1, (int)rcItemF.top + 1, (int)rcItemF.width() - 2, (int)rcItemF.height() - 2,
                                       hCanvasRotate, (int)rcRotate.bottom + 1, 1, SRCCOPY);
                    }
                }
            }
            else
            {
                hCanvas->drawtext(hFont, hbrText, text, -1, fmt, &rcItemF);
            }
        }
        */

        // 


        for (int i = 0; i < ItemCount; i++)
        {
            TabCtrl_GetItemRect(hWnd, i, &rcItem);
            TabCtrl_GetItem(hWnd, i, &item);
            rcItem.top = 0;
            rcItem.bottom = tabHeight;
            rcItem.left--;
            int offsetTop = 0;
            if (i == index)
            {
                SetTextColor(hdc, RGB(0, 151, 251));
                SelectObject(hdc, hbrBack);
            }
            else if (i == m_indexHot)
            {
                offsetTop = 2;
                SelectObject(hdc, hbrHot);      // 热点使用热点色, 绘画项目顶边需要往下移动两个像素
            }
            else
            {
                offsetTop = 2;
                SelectObject(hdc, hbrTab);     // 其他使用默认色, 绘画项目顶边需要往下移动两个像素
            }

            if (i == 0)
                rcItem.left = 0;

            Rectangle(hdc, rcItem.left, rcItem.top + offsetTop, rcItem.right, rcItem.bottom);
            if (offsetTop == 0)
                OffsetRect(&rcItem, 0, -2);
            //rcItem.top += offsetTop;
            rcItem.top += 2;
            DrawTextW(hdc, text, -1, &rcItem, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
            if (i == index)
                SetTextColor(hdc, clrText);

            if (i == index)
            {
                //rcItem.left++;
                //rcItem.top = rcItem.bottom - 2;
                //FillRect(hdc, &rcItem, hbrWhite);

                SelectObject(hdc, hPenBack);
                //SelectObject(hdc, GetStockPen(BLACK_PEN));

                RECT rcLine = { rcItem.left + 1, rcItem.top + tabHeight - 1, rcItem.right - (i == ItemCount - 1), rcItem.top + tabHeight + 1 };
                MoveToEx(hdc, rcItem.left + 1, tabHeight - 1, 0);
                LineTo(hdc, rcItem.right - (i == ItemCount - 1), tabHeight - 1);
                SelectObject(hdc, hPen);
            }
        }

        if (isFreeFont)
            DeleteObject(hFont);

        BitBlt(ps.hdc, 0, 0, cxClient, cyClient, hdc, 0, 0, SRCCOPY);
        EndPaint(hWnd, &ps);
        md_destroy(md);
        return 0;
    }

protected:

    virtual LRESULT OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc)
    {
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    virtual LRESULT OnMouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc)
    {
        switch (message)
        {
        case WM_MOUSEMOVE:
        {
            TCHITTESTINFO tti = { 0 };
            tti.pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            int index = TabCtrl_HitTest(hWnd, &tti);
            if (index != -1)
            {
                if (index != m_indexHot)
                {
                    m_indexHot = index;
                    InvalidateRect(hWnd, 0, 0);
                    SetTimer(hWnd, (UINT_PTR)this, 50, [](HWND hWnd, UINT message, UINT_PTR id, DWORD timer)
                    {
                        CDrawTab* pObj = (CDrawTab*)id;
                        if (pObj->m_indexHot == -1)
                        {
                            KillTimer(hWnd, id);
                            return;
                        }
                        POINT pt;
                        RECT rcItem;
                        GetCursorPos(&pt);
                        ScreenToClient(hWnd, &pt);
                        TabCtrl_GetItemRect(hWnd, pObj->m_indexHot, &rcItem);   // 取热点项的位置
                        // 如果鼠标不在热点项中就需要重画热点项
                        if (!PtInRect(&rcItem, pt))
                        {
                            KillTimer(hWnd, id);
                            pObj->m_indexHot = -1;
                            InvalidateRect(hWnd, 0, 0);
                        }
                    });

                }
            }
            else
            {
                // 鼠标不在选择夹中
                if (m_indexHot != -1)
                {
                    m_indexHot = -1;
                    InvalidateRect(hWnd, 0, 0);
                }
            }
            break;
        }
        default:
            break;
        }
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
};

