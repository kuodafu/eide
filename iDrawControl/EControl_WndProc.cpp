#include "EWindow_Fne_Header.h"
LRESULT CALLBACK WndProc_Edit_Prop(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc_OncePaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
    case WM_NCPAINT:
    case WM_NCCREATE:
        break;
    case WM_NCCALCSIZE:
    {
        if (wParam)
        {
            LPNCCALCSIZE_PARAMS cs = (LPNCCALCSIZE_PARAMS)lParam;
            memcpy(&cs->rgrc[1], &cs->rgrc[0], sizeof(cs->rgrc[0]));
            memcpy(&cs->rgrc[2], &cs->rgrc[0], sizeof(cs->rgrc[0]));
            return WVR_ALIGNTOP | WVR_ALIGNLEFT | WVR_ALIGNBOTTOM | WVR_ALIGNRIGHT;
        }
        break;
    }
    case WM_SIZE:
        return 0;
    case WM_PAINT:
        return WndProc_DefPaint(hWnd, message, wParam, lParam, pData);
    case WM_ERASEBKGND:
        return WndProc_EraseBkgnd(hWnd, message, wParam, lParam, pData);
    default:
        return WndProc_CtlColor(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}


LRESULT CALLBACK WndProc_MenuMenu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
    case WM_PAINT:
    {
        RECT rc;
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &rc);
        FillRect(hdc, &rc, pData->draw.color.hbrBackGround);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_NCPAINT:
    {
        HDC hdc = GetWindowDC(hWnd);
        RECT rc;
        GetWindowRect(hWnd, &rc);
        const int cxScreen = rc.right - rc.left;
        const int cyScreen = rc.bottom - rc.top;
        ScreenToClient(hWnd, (LPPOINT)&rc);
        rc.right = rc.left + cxScreen;
        rc.bottom = rc.top + cyScreen;
        FillRect(hdc, &rc, pData->draw.color.hbrBackGround);

        break;
    }
    case WM_PRINT:
    {
        HDC hdc = (HDC)wParam;
        RECT rc;
        GetWindowRect(hWnd, &rc);
        const int cxScreen = rc.right - rc.left;
        const int cyScreen = rc.bottom - rc.top;
        ScreenToClient(hWnd, (LPPOINT)&rc);
        rc.right = rc.left + cxScreen;
        rc.bottom = rc.top + cyScreen;

        DrawTextW(hdc, L"�����ı�......", -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        break;
    }
    case WM_NCHITTEST:
        return HTCLIENT;
    case WM_PRINTCLIENT:
    {
        break;
    }
    case WM_WINDOWPOSCHANGING:
    {
        LPWINDOWPOS pos = (LPWINDOWPOS)lParam;
        if ((pos->flags & SWP_NOSIZE) == 0 || (pos->flags & SWP_NOMOVE) == 0)    // �ı��˳ߴ���߱��ƶ���
        {
            pos->x = -1;
            pos->y = -1;
            pos->cx = 1;
            pos->cy = 1;
        }
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_DESTROY:
    {
        ShowWindowEx(hWnd, SW_SHOW);
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_ERASEBKGND:
        return WndProc_EraseBkgnd(hWnd, message, wParam, lParam, pData);
    default:
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}

static std::vector<HWND> s_btns;
void CALLBACK _timer_btn(HWND hWnd, UINT message, UINT_PTR id, DWORD t)
{
    if (1)
        return;
    for (auto it = s_btns.begin(); it != s_btns.end(); ++it)
    {
        HWND hChild = *it;
        wstr dbg(0x40000);
        for (int i = 0; i < 0x7FFFF; i++)
        {
            LRESULT r1 = SendMessageW(hChild, i, 0, 0);
            dbg.append(r1).append(L",");
            if (i % 50 == 0)
                dbg.append(L"\r\n");
        }
        OutputDebugStringW(dbg.c_str());
        return;
        BUTTON_IMAGELIST b = { 0 };
        BUTTON_SPLITINFO bs = { 0 };
        SIZE si = { 0 };
        RECT rc6 = { 0 };
        bs.mask = BCSIF_GLYPH | BCSIF_IMAGE | BCSIF_SIZE | BCSIF_STYLE;
        LRESULT r1 = SendMessageW(hChild, BCM_GETIMAGELIST, 0, (LPARAM)&b);
        LRESULT r2 = SendMessageW(hChild, BCM_GETSPLITINFO, 0, (LPARAM)&bs);
        LRESULT r3 = SendMessageW(hChild, BCM_GETIDEALSIZE, 0, (LPARAM)&si);
        LRESULT r4 = SendMessageW(hChild, BCM_GETNOTELENGTH, 0, 0) + 1;
        wstr s5(r4);
        LRESULT r5 = SendMessageW(hChild, BCM_GETNOTE, r4, (LPARAM)s5.data());
        LRESULT r6 = SendMessageW(hChild, BCM_GETTEXTMARGIN, 0, (LPARAM)&rc6);
        LRESULT r7 = SendMessageW(hChild, BM_GETCHECK, 0, 0);
        LRESULT r8 = SendMessageW(hChild, BM_GETIMAGE, IMAGE_BITMAP, 0);
        LRESULT r9 = SendMessageW(hChild, BM_GETIMAGE, IMAGE_ICON, 0);
        LRESULT r0 = SendMessageW(hChild, BM_GETSTATE, 0, 0);

        wstr::dbg(L"��ť��Ϣ, r1,r2={%d,%d}, ͼƬ�� = 0x%08x, margin = {%d,%d,%d,%d}, uAlign = %dn, BUTTON_SPLITINFO={%d, 0x%08x, %d, {%d, %d}}, ", r1, r2, b, bs);
        wstr::dbg(L"r3 = %d, size = {%d,%d}, r4,r5={%d,%d}, str = %s, ", r3, si, r4, r5, s5.c_str());
        wstr::dbg(L"r6 = %d, rc6={%d,%d,%d,%d}", r6, rc6);
        wstr::dbg(L"r7-r10 = {%d,%d,%d,%d}\n", r7, r8, r9, r0);
        //static HBRUSH hbr = Brush_Create(RGB(37, 37, 38));
        //SetClassLongPtrW(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)hbr);
    }

    {
        HWND hChild = hWnd;
        BUTTON_IMAGELIST b = { 0 };
        BUTTON_SPLITINFO bs = { 0 };
        SIZE si = { 0 };
        RECT rc6 = { 0 };
        bs.mask = BCSIF_GLYPH | BCSIF_IMAGE | BCSIF_SIZE | BCSIF_STYLE;
        LRESULT r1 = SendMessageW(hChild, BCM_GETIMAGELIST, 0, (LPARAM)&b);
        LRESULT r2 = SendMessageW(hChild, BCM_GETSPLITINFO, 0, (LPARAM)&bs);
        LRESULT r3 = SendMessageW(hChild, BCM_GETIDEALSIZE, 0, (LPARAM)&si);
        LRESULT r4 = SendMessageW(hChild, BCM_GETNOTELENGTH, 0, 0) + 1;
        wstr s5(r4);
        LRESULT r5 = SendMessageW(hChild, BCM_GETNOTE, r4, (LPARAM)s5.data());
        LRESULT r6 = SendMessageW(hChild, BCM_GETTEXTMARGIN, 0, (LPARAM)&rc6);
        LRESULT r7 = SendMessageW(hChild, BM_GETCHECK, 0, 0);
        LRESULT r8 = SendMessageW(hChild, BM_GETIMAGE, IMAGE_BITMAP, 0);
        LRESULT r9 = SendMessageW(hChild, BM_GETIMAGE, IMAGE_ICON, 0);
        LRESULT r0 = SendMessageW(hChild, BM_GETSTATE, 0, 0);
        wstr::dbg(L"��ť��Ϣ, r1,r2={%d,%d}, ͼƬ�� = 0x%08x, margin = {%d,%d,%d,%d}, uAlign = %dn, BUTTON_SPLITINFO={%d, 0x%08x, %d, {%d, %d}}, ", r1, r2, b, bs);
        wstr::dbg(L"r3 = %d, size = {%d,%d}, r4,r5={%d,%d}, str = %s, ", r3, si, r4, r5, s5.c_str());
        wstr::dbg(L"r6 = %d, rc6={%d,%d,%d,%d}", r6, rc6);
        wstr::dbg(L"r7-r10 = {%d,%d,%d,%d}\n", r7, r8, r9, r0);
    }
}

// �޸����ص�, �滻ָ����ɫ, �����ϵ���ɫ������
// crFixed = �µ���ɫ
// crBack = ���滻����ɫ
// lpBits = ͼƬ���ڴ��������
// nCount = ͼƬ����, �߶� * ���
void  PixelFix(DWORD crFixed, DWORD crBack, LPBYTE lpBits, int nCount)
{
    DWORD* cr = (DWORD*)lpBits;        // ת��4�ֽ�һ�����ص�������
    for (int i = 0; i < nCount; ++i)
    {
        if (cr[i] == 0) continue;    // ����0 ��ʾ��͸��, ͸���Ĳ�����

        if (cr[i] == crBack)        // FF FF FF ��ɫ, �ǰ�ɫ���ǻ��ϵ��ı���ɫ, ֱ���޸ĳɴ��ݽ�������ɫ
        {
            cr[i] = crFixed;
            continue;
        }
    }
}


inline LRESULT CALLBACK WndProc_Control_32770_DrawItem_Button(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    //TODO ������Ҫhook Bitblt, ������ͼƬ��DC�ϸ�������
    LRESULT ret = CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    return ret;
    LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lParam;
    wstr::dbg(L"�����İ�ť���ػ�, CtlID = %d, itemID = %d, itemAction = %d, itemState = %08X, hwndItem = %d, itemData = %08X\n",
        dis->CtlID, dis->itemAction, dis->itemState, dis->hwndItem, dis->itemData);
    RECT& rc = dis->rcItem;
    const int cx = rc.right - rc.left;
    const int cy = rc.bottom - rc.top;
    LPMEMDCBITMAP md = md_create(cx, cy);

    BitBlt(md->hdc, 0, 0, cx, cy, dis->hDC, 0, 0, SRCCOPY);

    PixelFix(RGB2ARGB(pData->draw.color.crBackGround, 255), RGB(240, 240, 240), md->lpBits, cx * cy);
    BitBlt(dis->hDC, 0, 0, cx, cy, md->hdc, 0, 0, SRCCOPY);

    //int size;
    //LPBYTE p = GetBitmapDataFromBitmap(md->hBitmap, &size, md->hdc);
    //CFileRW f;
    //f.open(LR"(D:\cpp\SVN����Ŀ¼\�����Խ���\��IDE�Ӿ���\aaaa.bmp)", 4, -1);
    //f.write(p, size);
    //f.reset();
    //delete[] p;
    md_destroy(md);
    return ret;
}

LRESULT CALLBACK WndProc_Control_32770(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
    case WM_PARENTNOTIFY:   // ��ť�½���Ҫ���໯
    {
        const int code = LOWORD(wParam);
        const int id = HIWORD(wParam);
        //if (code == WM_CREATE)
        //    wstr::dbg(L"����������������, id = %d, hWnd = %d\n", id, lParam);
        if (code == WM_CREATE && id >= 11225 && id < 16000)
        {
            s_info->iDraw->SubButton((HWND)lParam, true, false, 0);
        }

        if(code == WM_CREATE && id >= 16225)
        {
            s_btns.push_back((HWND)lParam);
            SetTimer(hWnd, 100, 1000, _timer_btn);
        }

        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lParam;
        switch (dis->CtlType)
        {
        case ODT_BUTTON:
            return WndProc_Control_32770_DrawItem_Button(hWnd, message, wParam, lParam, pData);
        default:
            break;
        }
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_NCHITTEST:
        return HTCLIENT;
    case WM_PAINT:
        return WndProc_DefPaint(hWnd, message, wParam, lParam, pData);
    default:
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK WndProc_Control_32770Parent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    // 32770�ĸ�������Ҫ��������滭��Ϣ
    switch (message)
    {
    case WM_NCHITTEST:
        return HTCLIENT;
    case WM_NCPAINT:
    case WM_NCCREATE:
        break;
    case WM_NCCALCSIZE:
    {
        if (wParam)
        {
            LPNCCALCSIZE_PARAMS cs = (LPNCCALCSIZE_PARAMS)lParam;
            memcpy(&cs->rgrc[1], &cs->rgrc[0], sizeof(cs->rgrc[0]));
            memcpy(&cs->rgrc[2], &cs->rgrc[0], sizeof(cs->rgrc[0]));
            return WVR_ALIGNTOP | WVR_ALIGNLEFT | WVR_ALIGNBOTTOM | WVR_ALIGNRIGHT;
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);

        EndPaint(hWnd, &ps);
        break;
    }
    case WM_SIZE:
        return 0;
    default:
        return WndProc_CtlColor(hWnd, message, wParam, lParam, pData);
        //return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}


LRESULT CALLBACK WndProc_PropProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    IDrawScroll* pObj = (IDrawScroll*)pData->drawScroll;
    if (pObj && s_info->MY_MF_OWNERDRAW)
    {
        LRESULT ret = 0;
        if (pObj->OnWndProc(pData->oldProc, hWnd, message, wParam, lParam, &ret))
        {
            return ret;
        }
    }
    switch (message)
    {
    case WM_ERASEBKGND:
    {
        RECT rc;
        GetClientRect(hWnd, &rc);
        FillRect((HDC)wParam, &rc, pData->draw.color.hbrBackGround);
        if (pData->draw.isTransparent)
            SetBkMode((HDC)wParam, TRANSPARENT);

        return 1;
    }
    case WM_PARENTNOTIFY:
    {
        const int code = LOWORD(wParam);
        const int id = HIWORD(wParam);

        if (code == WM_CREATE && id == 119) // 119�Ǳ༭���ID
        {
            HWND hChild = (HWND)lParam;
            WNDPROC oldChildProc = (WNDPROC)SetWindowLongPtrA(hChild, GWLP_WNDPROC, (LONG_PTR)WndProc_Edit_Prop);
            SetPropW(hChild, L"proc", oldChildProc);
        }
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    default:
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
}

LRESULT CALLBACK WndProc_Edit_Prop(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WNDPROC oldProc = (WNDPROC)GetPropW(hWnd, L"proc");
    if (!oldProc)
        return DefWindowProcA(hWnd, message, wParam, lParam);
    switch (message)
    {
    case WM_RBUTTONUP:
    {
        SendMessageW(s_info->hEWnd, WM_USER + 0x12, 0, 1);
        LRESULT ret = CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
        SendMessageW(s_info->hEWnd, WM_USER + 0x12, 0, 0);
        return ret;
    }
    case WM_DESTROY:
    {
        SetWindowLongPtrA(hWnd, GWLP_WNDPROC, (LONG_PTR)oldProc);
        RemovePropW(hWnd, L"proc");
        return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_DRAWITEM:
    {
        OBJSTRUCT data;
        data.oldProc = oldProc;
        LPDRAWITEMSTRUCT dw = (LPDRAWITEMSTRUCT)lParam;
        if (dw->CtlType == ODT_MENU)
            return EMenu_OnDrawItem(hWnd, message, wParam, lParam, &data);
        return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_MEASUREITEM:
    {
        OBJSTRUCT data;
        data.oldProc = oldProc;
        LPMEASUREITEMSTRUCT item = (LPMEASUREITEMSTRUCT)lParam;
        if (item->CtlType == ODT_MENU)
            return EMenu_OnMeasureItem(hWnd, message, wParam, lParam, &data);
        return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
    }
    default:
        return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}
