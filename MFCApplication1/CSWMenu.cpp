#include "pch.h"
#include "CSWMenu.h"
#include <afxpriv.h>

#define CSWMenu_GAP 1

#ifndef OBM_CHECK
#define OBM_CHECK 32760
#endif

using namespace sw;

UINT CSWMenu::m_nDrawMode = CSWMenu_DRAWMODE_XP;
BOOL CSWMenu::m_bSelectDisabled = FALSE;
BOOL CSWMenu::m_bDraw3DBitmap = TRUE;
BOOL CSWMenu::m_bHighColorBitmap = FALSE;
BOOL CSWMenu::m_bSpaceAccelerators = TRUE;
BOOL CSWMenu::m_bOrgSpaceAccelerators = TRUE;

int CSWMenu::m_iconX = 16;
int CSWMenu::m_iconY = 16;
int CSWMenu::m_iSeparatorHeight = -1;

COLORREF CSWMenu::m_clrBitmapBar = GetSysColor(COLOR_3DFACE);
COLORREF CSWMenu::m_clrBackground = GetSysColor(COLOR_MENU);
COLORREF CSWMenu::m_clrBackgroundSel = GetSysColor(COLOR_MENU);
COLORREF CSWMenu::m_clrEdge = GetSysColor(COLOR_WINDOW);
COLORREF CSWMenu::m_clrText = GetSysColor(COLOR_MENUTEXT);
COLORREF CSWMenu::m_clrTextSel = GetSysColor(COLOR_HIGHLIGHT);

CImageList CSWMenu::m_AllImages;
CArray<int, int&> CSWMenu::m_AllImagesID;

CString CSWMenuData::GetString()
{
    CString strText;
    if (m_szMenuText)
    {
#ifdef UNICODE
        strText = m_szMenuText;
#else
        USES_CONVERSION;
        strText = W2A(m_szMenuText);
#endif    
    }
    return strText;
}

CTypedPtrArray<CPtrArray, HMENU> CSWMenu::m_AllSubMenus;

IMPLEMENT_DYNAMIC(CSWMenu, CMenu)
CSWMenu::CSWMenu()
{
    m_bDynIcons = FALSE;
    m_bDisableOldStyle = FALSE;
    m_nSelectCheck = -1;
    m_bUnSelectCheck = -1;
    m_imagelistCheckBitmap = NULL;
    m_bCheckBitmapShare = FALSE;
    m_clrBackgroundBitmatMask = RGB(192, 192, 192);
    m_bBitmapBackgroundFlag = FALSE;
    m_loadmenu = FALSE;
    InstallHook();
}

CSWMenu::~CSWMenu()
{
    UnInstallHook();
    DestroyMenu();
}

#define	MN_SETHMENU                 0x01E0
#define	MN_GETHMENU                 0x01E1
#define	MN_SIZEWINDOW               0x01E2
#define	MN_OPENHIERARCHY            0x01E3
#define	MN_CLOSEHIERARCHY           0x01E4
#define	MN_SELECTITEM               0x01E5
#define	MN_CANCELMENUS              0x01E6
#define	MN_SELECTFIRSTVALIDITEM     0x01E7

WNDPROC oldWndProc = NULL; // 用来保存被替换的窗口过程

// 处理菜单的窗口过程
LRESULT WINAPI MenuWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult;
    switch (message)
    {
    case WM_CREATE://是否是菜单的创建
    {
        // 首先要去掉菜单窗口的一些扩展风格
        // 包括：WS_BORDER、WS_EX_DLGMODALFRAME、WS_EX_WINDOWEDGE
        lResult = CallWindowProc(oldWndProc, hWnd, message, wParam, lParam);
        DWORD dwStyle = ::GetWindowLong(hWnd, GWL_STYLE);
        DWORD dwNewStyle = (dwStyle & ~WS_BORDER);
        ::SetWindowLong(hWnd, GWL_STYLE, dwNewStyle);
        DWORD dwExStyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);
        DWORD dwNewExStyle = (dwExStyle & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE));
        ::SetWindowLong(hWnd, GWL_EXSTYLE, dwNewExStyle);
        return lResult;
    }
    case WM_ERASEBKGND:
    {
        HDC hDC = GetWindowDC(hWnd);
        CRect rcClient;
        GetClientRect(hWnd, &rcClient);

        CBrush brush(sw::CSWMenu::m_clrEdge);
        FillRect(hDC, rcClient, brush);
        return FALSE;
    }
    case WM_NCPAINT:
    {
        HDC hDC = GetWindowDC(hWnd);

        CRect rcWindow;
        GetWindowRect(hWnd, &rcWindow);
        rcWindow.OffsetRect(-rcWindow.left, -rcWindow.top);

        CRect rcClient;
        GetClientRect(hWnd, &rcClient);

        //剪除掉客户区
        ExcludeClipRect(hDC, rcClient.left, rcClient.top,
            rcClient.right, rcClient.bottom);

        CBrush brush(sw::CSWMenu::m_clrEdge);
        FillRect(hDC, rcWindow, brush);
        return 0;
    }
    case WM_NCCALCSIZE:
    {
        NCCALCSIZE_PARAMS* lpncsp = (LPNCCALCSIZE_PARAMS)lParam;
        lpncsp->rgrc[0].top += 2;
        lpncsp->rgrc[0].left += 3;
        lpncsp->rgrc[0].right -= 2;
        lpncsp->rgrc[0].bottom -= 2;
        lpncsp->rgrc[1] = lpncsp->rgrc[0];
        lResult = CallWindowProc(oldWndProc, hWnd, message, wParam, lParam);
        return 0;
    }
    default:
        return CallWindowProc(oldWndProc, hWnd, message, wParam, lParam);
    }
}

// 如果需要去除菜单的外部边框，需要通过安装钩子，设置外框属性并改变菜单大小
LRESULT WINAPI CallWndProc(int code, WPARAM wParam, LPARAM lParam)
{
    CWPSTRUCT* pStruct = (CWPSTRUCT*)lParam;
    while (code == HC_ACTION)//HC_ACTION 为必须处理
    {
        HWND hWnd = pStruct->hwnd;
        // 捕捉创建消息WM_CREATE，后面筛选为是否是菜单的创建
        if (pStruct->message != WM_CREATE && pStruct->message != MN_SIZEWINDOW)
            break;
        TCHAR sClassName[10];
        int Count = ::GetClassName(hWnd, sClassName, sizeof(sClassName) / sizeof(sClassName[0]));
        // 检查是否菜单窗口，#32768为菜单类名
        if (Count != 6 || _tcscmp(sClassName, _T("#32768")) != 0)
            break;

        WNDPROC lastWndProc = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC); //获得指定窗口的信息 GWL_WNDPROC得到窗口回调函数的地址或句柄
        //获取传入进程中窗口的窗口过程，这个窗口过程用于接收和处理系统向窗口发送的消息
        if (lastWndProc != MenuWndProc)
        {
            // 替换菜单窗口过程 
            SetWindowLong(hWnd, GWL_WNDPROC, (long)MenuWndProc);
            //SetProp(hWnd, _T(""), NULL);
            // 保留原有的窗口过程 
            oldWndProc = lastWndProc;
        }
        break;
    }


    //每一个钩子函数在进行处理时都要考虑是否需要把事件传递给下一个钩子处理函数。如果需要传递，就要调用函数CallNestHookEx（）。
    //在实际使用时还是强烈建议无论是否需要进行事件传递都要在过程的最后调用一次CallNextHookEx( )，否则将会引起一些无法预知的系统行为或是系统锁定。
    return CallNextHookEx((HHOOK)WH_CALLWNDPROC, code, wParam, lParam);
}

void CSWMenu::InstallHook()
{
    // 需要移除边框时，要安装钩子
    if (m_hook == NULL)
    {
        DWORD id = ::GetCurrentThreadId(); // 获取当前线程的ID        
        m_hook = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProc, AfxGetInstanceHandle(), id);//负责将回调函数放置于钩子链表的开始位置。

        //HHOOK SetWindowsHookEx(int idHook;HOOKPROC lpfn;HINSTANCE hMod;DWORD dwThreadId);
        //idHook 指定了钩子的类型，WH_CALLWNDPROC 系统将消息发送到指定窗口之前的“钩子” 
        //参数lpfn为指向钩子函数的指针，也即回调函数的首地址；
        //参数hMod标识了钩子处理函数所处模块的句柄；
        //参数dwThreadId 指定被监视的线程，如果明确指定了某个线程的ID就只监视该线程，此时的钩子即为线程钩子；
        //如果该参数被设置为0，则表示此钩子为监视系统所有线程的全局钩子。
        //此函数在执行完后将返回一个钩子句柄。
    }
}

void CSWMenu::UnInstallHook()
{
    if (m_hook != NULL)
    {
        UnhookWindowsHookEx(m_hook);
        m_hook = NULL;
    }
}

BOOL CSWMenu::IsLunaMenuStyle()
{
    if (m_nDrawMode == CSWMenu_DRAWMODE_XP)
        return TRUE;

    return FALSE;
}

CSWMenuData::~CSWMenuData()
{
    if (bitmap)
        delete bitmap;

    delete[] m_szMenuText;
}

void CSWMenuData::SetString(LPCTSTR szString)
{
    delete[] m_szMenuText;

    if (szString)
    {
#ifdef UNICODE
        CStringW sString = szString;
#else
        USES_CONVERSION;
        CStringW sString = A2W(szString);
#endif

        m_szMenuText = new wchar_t[sizeof(wchar_t) * (wcslen(sString.GetBuffer(0)) + 1)];
        if (m_szMenuText)
            wcscpy_s(m_szMenuText, sizeof(wchar_t) * (wcslen(sString.GetBuffer(0)) + 1), sString.GetBuffer(0));
    }
    else
        m_szMenuText = NULL;
}

BOOL CSWMenu::IsMenu(CMenu* submenu)
{
    int m;
    int numSubMenus = m_AllSubMenus.GetUpperBound();
    for (m = 0; m <= numSubMenus; ++m)
    {
        if (submenu->m_hMenu == m_AllSubMenus[m])
            return TRUE;
    }

    return FALSE;
}

BOOL CSWMenu::IsMenu(HMENU submenu)
{
    int m;
    int numSubMenus = m_AllSubMenus.GetUpperBound();
    for (m = 0; m <= numSubMenus; ++m)
    {
        if (submenu == m_AllSubMenus[m])
            return TRUE;
    }

    return FALSE;
}

BOOL CSWMenu::DestroyMenu()
{
    int m, n;
    int numAllSubMenus = m_AllSubMenus.GetUpperBound();
    for (n = numAllSubMenus; n >= 0; n--)
    {
        if (m_AllSubMenus[n] == m_hMenu)
            m_AllSubMenus.RemoveAt(n);
    }

    int numSubMenus = m_SubMenus.GetUpperBound();
    for (m = numSubMenus; m >= 0; m--)
    {
        numAllSubMenus = m_AllSubMenus.GetUpperBound();
        for (n = numAllSubMenus; n >= 0; n--)
        {
            if (m_AllSubMenus[n] == m_SubMenus[m])
                m_AllSubMenus.RemoveAt(n);
        }

        CMenu* ptr = FromHandle(m_SubMenus[m]);
        if (ptr)
        {
            BOOL flag = ptr->IsKindOf(RUNTIME_CLASS(CSWMenu));
            if (flag)
                delete (CSWMenu*)ptr;
        }
    }

    m_SubMenus.RemoveAll();
    int numItems = m_MenuList.GetUpperBound();
    for (m = 0; m <= numItems; m++)
        delete m_MenuList[m];
    m_MenuList.RemoveAll();
    if (m_imagelistCheckBitmap && !m_bCheckBitmapShare)
    {
        delete m_imagelistCheckBitmap;
        m_imagelistCheckBitmap = NULL;
    }

    return CMenu::DestroyMenu();
}

int CSWMenu::GetMenuDrawMode()
{
    return m_nDrawMode;
}

BOOL CSWMenu::GetSelectDisableMode()
{
    return m_bSelectDisabled;
}

void CSWMenu::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
    CDC* pDC = CDC::FromHandle(lpDIS->hDC);
    if (pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
        DrawItem_Win9xNT2000(lpDIS);
    else
    {
        if (m_nDrawMode == CSWMenu_DRAWMODE_XP)
            DrawItem_WinXP(lpDIS);
        else
            DrawItem_Win9xNT2000(lpDIS);
    }
}


void CSWMenu::DrawItem_Win9xNT2000(LPDRAWITEMSTRUCT lpDIS)
{
    CDC* pDC = CDC::FromHandle(lpDIS->hDC);
    CRect rect;
    UINT state = (((CSWMenuData*)(lpDIS->itemData))->nFlags);
    CBrush m_brBackground;
    COLORREF m_clrBack;
    m_clrBack = GetSysColor(COLOR_3DFACE);
    m_brBackground.CreateSolidBrush(m_clrBack);
    if (lpDIS->itemState & ODS_GRAYED && !m_bSelectDisabled)
    {
        if (lpDIS->itemState & ODS_SELECTED)
            lpDIS->itemState = lpDIS->itemState & ~ODS_SELECTED;
    }

    if (state & MF_SEPARATOR)
    {
        rect.CopyRect(&lpDIS->rcItem);
        pDC->FillRect(rect, &m_brBackground);
        rect.top += (rect.Height() >> 1);
        //pDC->DrawEdge(&rect, EDGE_ETCHED, BF_TOP);
        pDC->Draw3dRect(&rect, m_clrEdge, m_clrEdge);
    }
    else
    {
        CRect rect2;
        BOOL standardflag = FALSE, bSelectedflag = FALSE, disableflag = FALSE;
        BOOL checkflag = FALSE;
        COLORREF crText = m_clrText;
        CBrush m_brSelect;
        CPen m_penBack;
        int x0, y0, dy;
        int nIconNormal = -1, xoffset = -1, global_offset = -1;
        CImageList* bitmap = NULL;
        m_penBack.CreatePen(PS_SOLID, 0, m_clrBack);
        m_brSelect.CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
        rect.CopyRect(&lpDIS->rcItem);
        rect2 = rect;
        UINT state = lpDIS->itemState;
        CString strText;

        if (lpDIS->itemData != NULL)
        {
            nIconNormal = (((CSWMenuData*)(lpDIS->itemData))->menuIconNormal);
            xoffset = (((CSWMenuData*)(lpDIS->itemData))->xoffset);
            global_offset = (((CSWMenuData*)(lpDIS->itemData))->global_offset);
            bitmap = (((CSWMenuData*)(lpDIS->itemData))->bitmap);
            strText = ((CSWMenuData*)(lpDIS->itemData))->GetString();
            if (nIconNormal < 0 && global_offset >= 0)
            {
                xoffset = global_offset;
                nIconNormal = 0;
                bitmap = &m_AllImages;
            }

            if (state & ODS_CHECKED && nIconNormal < 0)
            {
                if (state & ODS_SELECTED && m_nSelectCheck > 0)checkflag = TRUE;
                else if (m_bUnSelectCheck > 0) checkflag = TRUE;
            }
            else if (nIconNormal != -1)
            {
                standardflag = TRUE;
                if (state & ODS_SELECTED && !(state & ODS_GRAYED))bSelectedflag = TRUE;
                else if (state & ODS_GRAYED) disableflag = TRUE;
            }
        }
        else
        {
            strText.Empty();
        }

        if (state & ODS_SELECTED)
        {
            CPen* pOldPen = pDC->SelectObject(&m_penBack);
            if (checkflag || standardflag || bSelectedflag || disableflag || state & ODS_CHECKED)
                rect2.SetRect(rect.left + m_iconX + 4 + CSWMenu_GAP, rect.top, rect.right, rect.bottom);
            pDC->FillRect(rect2, &m_brSelect);
            pDC->SelectObject(pOldPen);
            crText = m_clrTextSel;
        }
        else
        {
            CPen* pOldPen = pDC->SelectObject(&m_penBack);
            pDC->FillRect(rect, &m_brBackground);
            pDC->SelectObject(pOldPen);
            pDC->Draw3dRect(rect, m_clrBack, m_clrBack);
        }

        dy = (rect.Height() - 4 - m_iconY) / 2;
        dy = dy < 0 ? 0 : dy;
        if (checkflag || standardflag || bSelectedflag || disableflag)
        {
            rect2.SetRect(rect.left + 1,
                rect.top + 1 + dy,
                rect.left + m_iconX + 3,
                rect.top + m_iconY + 3 + dy);
            pDC->Draw3dRect(rect2, m_clrBack, m_clrBack);
            if (checkflag && m_imagelistCheckBitmap)
            {
                pDC->FillRect(rect2, &m_brBackground);
                rect2.SetRect(rect.left, rect.top + dy, rect.left + m_iconX + 4,
                    rect.top + m_iconY + 4 + dy);
                pDC->Draw3dRect(rect2, m_clrBack, m_clrBack);
                CPoint ptImage(rect.left + 2, rect.top + 2 + dy);
                if (state & ODS_SELECTED)
                    m_imagelistCheckBitmap->Draw(pDC, 1, ptImage, ILD_TRANSPARENT);
                else
                    m_imagelistCheckBitmap->Draw(pDC, 0, ptImage, ILD_TRANSPARENT);
            }
            else if (disableflag)
            {
                if (!bSelectedflag)
                {
                    CBitmap bitmapstandard;
                    GetBitmapFromImageList(pDC, bitmap, xoffset, bitmapstandard);
                    rect2.SetRect(rect.left,
                        rect.top + dy,
                        rect.left + m_iconX + 4,
                        rect.top + m_iconY + 4 + dy);
                    pDC->Draw3dRect(rect2, m_clrBack, m_clrBack);
                    if (m_bDisableOldStyle)
                        DitherBlt(lpDIS->hDC,
                            rect.left + 2,
                            rect.top + 2 + dy,
                            m_iconX, m_iconY,
                            (HBITMAP)(bitmapstandard), 0, 0, m_clrBack);
                    else
                    {
                        if (m_bHighColorBitmap)
                            DitherBlt3(pDC, rect.left + 2,
                                rect.top + 2 + dy,
                                m_iconX, m_iconY,
                                bitmapstandard, m_clrBack);
                        else
                            DitherBlt(pDC, rect.left + 2,
                                rect.top + 2 + dy,
                                m_iconX, m_iconY,
                                bitmapstandard, 0, 0, m_clrBack);
                    }

                    bitmapstandard.DeleteObject();
                }
            }
            else if (bSelectedflag)
            {
                pDC->FillRect(rect2, &m_brBackground);
                rect2.SetRect(rect.left,
                    rect.top + dy,
                    rect.left + m_iconX + 4,
                    rect.top + m_iconY + 4 + dy);
                if (state & ODS_CHECKED)
                    pDC->Draw3dRect(rect2, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
                else
                    pDC->Draw3dRect(rect2, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));
                CPoint ptImage(rect.left + 2, rect.top + 2 + dy);
                if (bitmap)
                    bitmap->Draw(pDC, xoffset, ptImage, ILD_TRANSPARENT);
            }
            else
            {
                if (state & ODS_CHECKED)
                {
                    CBrush brush;
                    COLORREF col = m_clrBack;
                    col = LightenColor(col, 0.6);
                    brush.CreateSolidBrush(col);
                    pDC->FillRect(rect2, &brush);
                    brush.DeleteObject();
                    rect2.SetRect(rect.left,
                        rect.top + dy,
                        rect.left + m_iconX + 4,
                        rect.top + m_iconY + 4 + dy);
                    pDC->Draw3dRect(rect2, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
                }
                else
                {
                    pDC->FillRect(rect2, &m_brBackground);
                    rect2.SetRect(rect.left,
                        rect.top + dy,
                        rect.left + m_iconX + 4,
                        rect.top + m_iconY + 4 + dy);
                    pDC->Draw3dRect(rect2, m_clrBack, m_clrBack);
                }

                CPoint ptImage(rect.left + 2, rect.top + 2 + dy);
                if (bitmap)
                    bitmap->Draw(pDC, xoffset, ptImage, ILD_TRANSPARENT);
            }
        }

        if (nIconNormal < 0 && state & ODS_CHECKED && !checkflag)
        {
            rect2.SetRect(rect.left + 1,
                rect.top + 2 + dy,
                rect.left + m_iconX + 1,
                rect.top + m_iconY + 2 + dy);
            CMenuItemInfo info;
            info.fMask = MIIM_CHECKMARKS;
            ::GetMenuItemInfo((HMENU)lpDIS->hwndItem, lpDIS->itemID, MF_BYCOMMAND, &info);
            if (state & ODS_CHECKED || info.hbmpUnchecked) {
                Draw3DCheckmark(pDC,
                    rect2,
                    state & ODS_SELECTED,
                    state & ODS_CHECKED ? info.hbmpChecked : info.hbmpUnchecked);
            }
        }

        x0 = rect.left; y0 = rect.top;
        rect.left = rect.left + m_iconX + 8 + CSWMenu_GAP;
        if (!strText.IsEmpty())
        {
            CRect rectt(rect.left, rect.top - 1, rect.right, rect.bottom - 1);
            CString leftStr, rightStr;
            leftStr.Empty(); rightStr.Empty();
            int tablocr = strText.ReverseFind(TCHAR('\t'));
            if (tablocr != -1)
            {
                rightStr = strText.Mid(tablocr + 1);
                leftStr = strText.Left(strText.Find(TCHAR('\t')));
                rectt.right -= m_iconX;
            }
            else
            {
                leftStr = strText;
            }

            int iOldMode = pDC->GetBkMode();
            pDC->SetBkMode(TRANSPARENT);
            UINT nFormat = DT_LEFT | DT_SINGLELINE | DT_VCENTER;
            UINT nFormatr = DT_RIGHT | DT_SINGLELINE | DT_VCENTER;
            if (!(lpDIS->itemState & ODS_GRAYED))
            {
                pDC->SetTextColor(crText);
                pDC->DrawText(leftStr, rectt, nFormat);
                if (tablocr != -1)
                    pDC->DrawText(rightStr, rectt, nFormatr);
            }
            else
            {
                if (!(state & ODS_SELECTED))
                {
                    RECT offset = *rectt;
                    offset.left += 1;
                    offset.right += 1;
                    offset.top += 1;
                    offset.bottom += 1;
                    pDC->SetTextColor(GetSysColor(COLOR_BTNHILIGHT));
                    pDC->DrawText(leftStr, &offset, nFormat);
                    if (tablocr != -1)
                        pDC->DrawText(rightStr, &offset, nFormatr);
                    pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
                    pDC->DrawText(leftStr, rectt, nFormat);
                    if (tablocr != -1)
                        pDC->DrawText(rightStr, rectt, nFormatr);
                }
                else
                {
                    pDC->SetTextColor(m_clrBack);
                    pDC->DrawText(leftStr, rectt, nFormat);
                    if (tablocr != -1)
                        pDC->DrawText(rightStr, rectt, nFormatr);
                }
            }

            pDC->SetBkMode(iOldMode);
        }

        m_penBack.DeleteObject();
        m_brSelect.DeleteObject();
    }

    m_brBackground.DeleteObject();
}

COLORREF CSWMenu::LightenColor(COLORREF col, double factor)
{
    // 	if (factor > 0.0 && factor <= 1.0)
    // 	{
    // 		BYTE red, green, blue, lightred, lightgreen, lightblue;
    // 		red = GetRValue(col);
    // 		green = GetGValue(col);
    // 		blue = GetBValue(col);
    // 		lightred = (BYTE)((factor * (255 - red)) + red);
    // 		lightgreen = (BYTE)((factor * (255 - green)) + green);
    // 		lightblue = (BYTE)((factor * (255 - blue)) + blue);
    // 		col = RGB(lightred, lightgreen, lightblue);
    // 	}

    return col;
}

COLORREF CSWMenu::DarkenColor(COLORREF col, double factor)
{
    // 	if (factor > 0.0 && factor <= 1.0)
    // 	{
    // 		BYTE red, green, blue, lightred, lightgreen, lightblue;
    // 		red = GetRValue(col);
    // 		green = GetGValue(col);
    // 		blue = GetBValue(col);
    // 		lightred = (BYTE)(red - (factor * red));
    // 		lightgreen = (BYTE)(green - (factor * green));
    // 		lightblue = (BYTE)(blue - (factor * blue));
    // 		col = RGB(lightred, lightgreen, lightblue);
    // 	}

    return col;
}

void CSWMenu::DrawItem_WinXP(LPDRAWITEMSTRUCT lpDIS)
{
    ASSERT(lpDIS != NULL);
    CDC* pDC = CDC::FromHandle(lpDIS->hDC);
    CDC* pMemDC = NULL;
    CRect rect, rect2;
    UINT state = (((CSWMenuData*)(lpDIS->itemData))->nFlags);
    COLORREF m_newclrBack = m_clrBitmapBar;
    COLORREF m_clrBack = DarkenColor(m_clrBackground, 0.02);
    CFont m_fontMenu, * pFont = NULL;
    LOGFONT m_lf;
    m_newclrBack = LightenColor(m_newclrBack, 0.25);
    CBrush m_newbrBackground, m_brBackground;
    m_brBackground.CreateSolidBrush(m_clrBack);
    m_newbrBackground.CreateSolidBrush(m_newclrBack);
    int CSWMenu_PAD = 4;
    if (m_bDraw3DBitmap)
        CSWMenu_PAD = 7;
    int barwidth = m_iconX + CSWMenu_PAD;

    if (lpDIS->itemState & ODS_GRAYED && !m_bSelectDisabled)
    {
        if (lpDIS->itemState & ODS_SELECTED)lpDIS->itemState = lpDIS->itemState & ~ODS_SELECTED;
        pMemDC = new CDC;// (*pDC, &lpDIS->rcItem);
        pDC = pMemDC;
        ZeroMemory((PVOID)&m_lf, sizeof(LOGFONT));
        NONCLIENTMETRICS nm;
        nm.cbSize = sizeof(NONCLIENTMETRICS);
        VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, nm.cbSize, &nm, 0));
        m_lf = nm.lfMenuFont;
        m_fontMenu.CreateFontIndirect(&m_lf);
        pFont = pDC->SelectObject(&m_fontMenu);
    }

    if (state & MF_SEPARATOR)
    {
        rect.CopyRect(&lpDIS->rcItem);
        pDC->FillRect(rect, &m_brBackground);
        rect2.SetRect(rect.left, rect.top, rect.left + barwidth, rect.bottom);
        rect.top += rect.Height() >> 1;
        //rect.bottom = rect.top + 1;
        rect.left = rect2.left = 0;// rect2.right + CSWMenu_PAD;
        //pDC->DrawEdge(&rect, EDGE_ETCHED, BF_TOP);// 无法指定边框颜色，抛弃		
        pDC->FillRect(rect2, &m_newbrBackground);
        pDC->Draw3dRect(rect2, m_newclrBack, m_newclrBack);
        pDC->Draw3dRect(&rect, m_clrEdge, m_clrEdge);
    }
    else
    {
        BOOL standardflag = FALSE, bSelectedflag = FALSE, disableflag = FALSE;
        BOOL checkflag = FALSE;
        COLORREF crText = m_clrText;
        COLORREF crTextSel = m_clrTextSel;
        COLORREF crSelect = m_clrBackgroundSel;
        COLORREF crSelectFill = LightenColor(crSelect, 0.7);
        CBrush m_brSelect;
        CPen m_penBack;
        int x0, y0, dx, dy;
        int nIconNormal = -1, xoffset = -1, global_offset = -1;
        int faded_offset = 1, shadow_offset = 2, disabled_offset = 3;
        CImageList* bitmap = NULL;
        BOOL CanDraw3D = FALSE;

        m_penBack.CreatePen(PS_SOLID, 0, m_clrBack);
        m_brSelect.CreateSolidBrush(crSelectFill);
        rect.CopyRect(&lpDIS->rcItem);
        rect2 = rect;
        UINT state = lpDIS->itemState;
        CString strText;

        if (lpDIS->itemData != NULL)
        {
            nIconNormal = (((CSWMenuData*)(lpDIS->itemData))->menuIconNormal);
            xoffset = (((CSWMenuData*)(lpDIS->itemData))->xoffset);
            bitmap = (((CSWMenuData*)(lpDIS->itemData))->bitmap);
            strText = ((CSWMenuData*)(lpDIS->itemData))->GetString();
            global_offset = (((CSWMenuData*)(lpDIS->itemData))->global_offset);
            if (xoffset == 0 && m_bDraw3DBitmap && bitmap && bitmap->GetImageCount() > 2)
                CanDraw3D = TRUE;
            if (nIconNormal < 0 && xoffset < 0 && global_offset >= 0)
            {
                xoffset = global_offset;
                nIconNormal = 0;
                bitmap = &m_AllImages;
                if (m_bDraw3DBitmap && CanDraw3DImageList(global_offset))
                {
                    CanDraw3D = TRUE;
                    faded_offset = global_offset + 1;
                    shadow_offset = global_offset + 2;
                    disabled_offset = global_offset + 3;
                }
            }

            if (state & ODS_CHECKED && nIconNormal < 0)
            {
                if (state & ODS_SELECTED && m_nSelectCheck > 0)
                    checkflag = TRUE;
                else if (m_bUnSelectCheck > 0)
                    checkflag = TRUE;
            }
            else if (nIconNormal != -1)
            {
                standardflag = TRUE;
                if (state & ODS_SELECTED && !(state & ODS_GRAYED))
                    bSelectedflag = TRUE;
                else if (state & ODS_GRAYED)
                    disableflag = TRUE;
            }
        }
        else
        {
            strText.Empty();
        }

        if (state & ODS_SELECTED)
        {
            CPen* pOldPen = pDC->SelectObject(&m_penBack);
            pDC->FillRect(rect, &m_brSelect);
            pDC->Draw3dRect(rect, crSelect, crSelect);
            pDC->SelectObject(pOldPen);
        }
        else
        {
            rect2.SetRect(rect.left, rect.top, rect.left + barwidth, rect.bottom);
            CPen* pOldPen = pDC->SelectObject(&m_penBack);
            pDC->FillRect(rect, &m_brBackground);
            pDC->FillRect(rect2, &m_newbrBackground);
            pDC->SelectObject(pOldPen);
            pDC->Draw3dRect(rect, m_clrBack, m_clrBack);
            pDC->Draw3dRect(rect2, m_newclrBack, m_newclrBack);
        }

        dy = (int)(0.5 + (rect.Height() - m_iconY) / 2.0);
        dy = dy < 0 ? 0 : dy;
        dx = (int)(0.5 + (barwidth - m_iconX) / 2.0);
        dx = dx < 0 ? 0 : dx;
        rect2.SetRect(rect.left + 1, rect.top + 1, rect.left + barwidth - 2, rect.bottom - 1);

        if (checkflag || standardflag || bSelectedflag || disableflag)
        {
            if (checkflag && m_imagelistCheckBitmap)
            {
                pDC->FillRect(rect2, &m_newbrBackground);
                CPoint ptImage(rect.left + dx, rect.top + dy);
                if (state & ODS_SELECTED)
                    m_imagelistCheckBitmap->Draw(pDC, 1, ptImage, ILD_TRANSPARENT);
                else
                    m_imagelistCheckBitmap->Draw(pDC, 0, ptImage, ILD_TRANSPARENT);
            }
            else if (disableflag)
            {
                if (!bSelectedflag)
                {
                    if (CanDraw3D)
                    {
                        CPoint ptImage(rect.left + dx, rect.top + dy);
                        bitmap->Draw(pDC, disabled_offset, ptImage, ILD_TRANSPARENT);
                    }
                    else
                    {
                        CBitmap bitmapstandard;
                        GetBitmapFromImageList(pDC, bitmap, xoffset, bitmapstandard);
                        COLORREF transparentcol = m_newclrBack;
                        if (state & ODS_SELECTED)
                            transparentcol = crSelectFill;
                        if (m_bDisableOldStyle)
                            DitherBlt(lpDIS->hDC,
                                rect.left + dx,
                                rect.top + dy,
                                m_iconX, m_iconY,
                                (HBITMAP)(bitmapstandard), 0, 0, transparentcol);
                        else
                            DitherBlt(pDC,
                                rect.left + dx,
                                rect.top + dy,
                                m_iconX, m_iconY,
                                bitmapstandard, 0, 0, transparentcol);
                        if (state & ODS_SELECTED)
                            pDC->Draw3dRect(rect, crSelect, crSelect);
                        bitmapstandard.DeleteObject();
                    }
                }
            }
            else if (bSelectedflag)
            {
                CPoint ptImage(rect.left + dx, rect.top + dy);
                if (state & ODS_CHECKED)
                {
                    CBrush brIn;
                    brIn.CreateSolidBrush(LightenColor(crSelect, 0.55));
                    pDC->FillRect(rect2, &brIn);
                    brIn.DeleteObject();
                    pDC->Draw3dRect(rect2, crSelect, crSelect);
                    ptImage.x -= 1; ptImage.y -= 1;
                }
                else
                {
                    pDC->FillRect(rect2, &m_brSelect);
                }

                if (bitmap)
                {
                    if (CanDraw3D && !(state & ODS_CHECKED))
                    {
                        CPoint ptImage1(ptImage.x + 1, ptImage.y + 1);
                        CPoint ptImage2(ptImage.x - 1, ptImage.y - 1);
                        bitmap->Draw(pDC, shadow_offset, ptImage1, ILD_TRANSPARENT);
                        bitmap->Draw(pDC, xoffset, ptImage2, ILD_TRANSPARENT);
                    }
                    else
                    {
                        bitmap->Draw(pDC, xoffset, ptImage, ILD_TRANSPARENT);
                    }
                }
            }
            else
            {
                if (state & ODS_CHECKED)
                {
                    CBrush brIn;
                    brIn.CreateSolidBrush(LightenColor(crSelect, 0.85));
                    pDC->FillRect(rect2, &brIn);
                    brIn.DeleteObject();
                    pDC->Draw3dRect(rect2, crSelect, crSelect);
                    CPoint ptImage(rect.left + dx - 1, rect.top + dy - 1);
                    if (bitmap)
                        bitmap->Draw(pDC, xoffset, ptImage, ILD_TRANSPARENT);
                }
                else
                {
                    pDC->FillRect(rect2, &m_newbrBackground);
                    pDC->Draw3dRect(rect2, m_newclrBack, m_newclrBack);
                    CPoint ptImage(rect.left + dx, rect.top + dy);
                    if (bitmap)
                    {
                        if (CanDraw3D)
                            bitmap->Draw(pDC, faded_offset, ptImage, ILD_TRANSPARENT);
                        else
                            bitmap->Draw(pDC, xoffset, ptImage, ILD_TRANSPARENT);
                    }
                }
            }
        }

        if (nIconNormal < 0 && state & ODS_CHECKED && !checkflag)
        {
            CMenuItemInfo info;
            info.fMask = MIIM_CHECKMARKS;
            ::GetMenuItemInfo((HMENU)lpDIS->hwndItem, lpDIS->itemID, MF_BYCOMMAND, &info);
            if (state & ODS_CHECKED || info.hbmpUnchecked)
            {
                DrawXPCheckmark(pDC, rect2, state & ODS_CHECKED ? info.hbmpChecked : info.hbmpUnchecked, crSelect, state & ODS_SELECTED);
            }
        }

        x0 = rect.left; y0 = rect.top;
        rect.left = rect.left + barwidth + 8;

        if (!strText.IsEmpty())
        {
            CRect rectt(rect.left, rect.top - 1, rect.right, rect.bottom - 1);
            CString leftStr, rightStr;
            leftStr.Empty(); rightStr.Empty();
            int tablocr = strText.ReverseFind(TCHAR('\t'));
            if (tablocr != -1)
            {
                rightStr = strText.Mid(tablocr + 1);
                leftStr = strText.Left(strText.Find(TCHAR('\t')));
                rectt.right -= m_iconX;
            }
            else
            {
                leftStr = strText;
            }

            int iOldMode = pDC->GetBkMode();
            pDC->SetBkMode(TRANSPARENT);
            UINT nFormat = DT_LEFT | DT_SINGLELINE | DT_VCENTER;
            UINT nFormatr = DT_RIGHT | DT_SINGLELINE | DT_VCENTER;
            if (!(lpDIS->itemState & ODS_GRAYED))
            {
                if (state & ODS_SELECTED)
                    pDC->SetTextColor(crTextSel);
                else
                    pDC->SetTextColor(crText);
                pDC->DrawText(leftStr, rectt, nFormat);
                if (tablocr != -1)
                    pDC->DrawText(rightStr, rectt, nFormatr);
            }
            else
            {
                RECT offset = *rectt;
                offset.left += 1;
                offset.right += 1;
                offset.top += 1;
                offset.bottom += 1;
                pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
                pDC->DrawText(leftStr, rectt, nFormat);
                if (tablocr != -1)
                    pDC->DrawText(rightStr, rectt, nFormatr);
            }
            pDC->SetBkMode(iOldMode);
        }

        m_penBack.DeleteObject();
        m_brSelect.DeleteObject();
    }

    m_brBackground.DeleteObject();
    m_newbrBackground.DeleteObject();
    if (pFont)
        pDC->SelectObject(pFont);
    m_fontMenu.DeleteObject();
    if (pMemDC)
        delete pMemDC;
}

BOOL CSWMenu::GetBitmapFromImageList(CDC* pDC, CImageList* imglist, int nIndex, CBitmap& bmp)
{
    HICON hIcon = imglist->ExtractIcon(nIndex);
    CDC dc;
    dc.CreateCompatibleDC(pDC);
    bmp.CreateCompatibleBitmap(pDC, m_iconX, m_iconY);
    CBitmap* pOldBmp = dc.SelectObject(&bmp);
    CBrush brush;
    COLORREF m_newclrBack;
    m_newclrBack = GetSysColor(COLOR_3DFACE);
    brush.CreateSolidBrush(m_newclrBack);
    ::DrawIconEx(
        dc.GetSafeHdc(),
        0,
        0,
        hIcon,
        m_iconX,
        m_iconY,
        0,
        (HBRUSH)brush,
        DI_NORMAL
    );
    dc.SelectObject(pOldBmp);
    dc.DeleteDC();
    ::DestroyIcon(hIcon);
    return(TRUE);
}

void CSWMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
    UINT state = (((CSWMenuData*)(lpMIS->itemData))->nFlags);
    int CSWMenu_PAD = 4;
    if (IsLunaMenuStyle() && m_bDraw3DBitmap)
        CSWMenu_PAD = 7;
    if (state & MF_SEPARATOR)
    {
        if (m_iSeparatorHeight != -1)
        {
            lpMIS->itemWidth = m_iSeparatorHeight;
        }
        else
        {
            lpMIS->itemWidth = 0;
            int temp = GetSystemMetrics(SM_CYMENU) >> 1;
            if (IsLunaMenuStyle())
                lpMIS->itemHeight = 1;
            else
                lpMIS->itemHeight = temp > (m_iconY + CSWMenu_PAD) / 2 ? temp : (m_iconY + CSWMenu_PAD) / 2;
        }
    }
    else
    {
        CFont m_fontMenu;
        LOGFONT m_lf;
        ZeroMemory((PVOID)&m_lf, sizeof(LOGFONT));
        NONCLIENTMETRICS nm;
        nm.cbSize = sizeof(NONCLIENTMETRICS);
        VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, nm.cbSize, &nm, 0));
        m_lf = nm.lfMenuFont;
        m_fontMenu.CreateFontIndirect(&m_lf);
        CWnd* pWnd = AfxGetMainWnd();
        if (pWnd == NULL)
            pWnd = CWnd::GetDesktopWindow();
        CDC* pDC = pWnd->GetDC();
        CFont* pFont = NULL;
        pFont = pDC->SelectObject(&m_fontMenu);
        CString sText = ((CSWMenuData*)(lpMIS->itemData))->GetString();
#ifdef UNICODE
        CStringW swText = sText;
#else
        USES_CONVERSION;
        CStringW swText = A2W(sText);
#endif // UNICODE

        SIZE size;
        size.cx = size.cy = 0;

        CSize sz = pDC->GetTextExtent(sText);
        size.cx = sz.cx;
        size.cy = sz.cy;

        CSize t = CSize(size);
        pDC->SelectObject(pFont);
        pWnd->ReleaseDC(pDC);
        if (IsLunaMenuStyle())
            lpMIS->itemWidth = m_iconX + CSWMenu_PAD + 8 + t.cx;
        else
            lpMIS->itemWidth = m_iconX + t.cx + m_iconX + CSWMenu_GAP;
        int temp = GetSystemMetrics(SM_CYMENU);
        lpMIS->itemHeight = temp > m_iconY + CSWMenu_PAD ? temp : m_iconY + CSWMenu_PAD;
        m_fontMenu.DeleteObject();
    }
}


void CSWMenu::SetIconSize(int width, int height)
{
    m_iconX = width;
    m_iconY = height;
}

BOOL CSWMenu::AppendODMenu(TCHAR* lpstrText, UINT nFlags, UINT nID,
    int nIconNormal)
{
    if (!nID)
    {
        if (nFlags & MF_BYPOSITION)
            nFlags = MF_SEPARATOR | MF_OWNERDRAW | MF_BYPOSITION;
        else
            nFlags = MF_SEPARATOR | MF_OWNERDRAW;
    }
    else if (!(nFlags & MF_OWNERDRAW))
        nFlags |= MF_OWNERDRAW;

    if (nFlags & MF_POPUP)
    {
        m_AllSubMenus.Add((HMENU)nID);
        m_SubMenus.Add((HMENU)nID);
    }

    CSWMenuData* mdata = new CSWMenuData;
    m_MenuList.Add(mdata);
    mdata->SetString(lpstrText);
    mdata->menuIconNormal = -1;
    mdata->xoffset = -1;

    if (nIconNormal >= 0) {
        CImageList bitmap;
        int xoffset = 0;
        LoadFromToolBar(nID, nIconNormal, xoffset);
        if (mdata->bitmap) {
            mdata->bitmap->DeleteImageList();
            mdata->bitmap = NULL;
        }
        bitmap.Create(m_iconX, m_iconY, ILC_COLORDDB | ILC_MASK, 1, 1);
        if (AddBitmapToImageList(&bitmap, nIconNormal)) {
            mdata->global_offset = AddToGlobalImageList(&bitmap, xoffset, nID);
        }
    }
    else mdata->global_offset = GlobalImageListOffset(nID);
    mdata->nFlags = nFlags;
    mdata->nID = nID;
    BOOL returnflag = CMenu::AppendMenu(nFlags, nID, (LPCTSTR)mdata);
    if (m_loadmenu)RemoveTopLevelOwnerDraw();
    return(returnflag);
}

BOOL CSWMenu::AppendODMenu(TCHAR* lpstrText, UINT nFlags, UINT nID,
    CImageList* il, int xoffset)
{
    if (!nID)
    {
        if (nFlags & MF_BYPOSITION)
            nFlags = MF_SEPARATOR | MF_OWNERDRAW | MF_BYPOSITION;
        else
            nFlags = MF_SEPARATOR | MF_OWNERDRAW;
    }
    else if (!(nFlags & MF_OWNERDRAW))
        nFlags |= MF_OWNERDRAW;

    if (nFlags & MF_POPUP)
    {
        m_AllSubMenus.Add((HMENU)nID);
        m_SubMenus.Add((HMENU)nID);
    }

    CSWMenuData* mdata = new CSWMenuData;
    m_MenuList.Add(mdata);
    mdata->SetString(lpstrText);

    if (il)
    {
        mdata->menuIconNormal = 0;
        mdata->xoffset = 0;
        if (mdata->bitmap)mdata->bitmap->DeleteImageList();
        else mdata->bitmap = new(CImageList);
        ImageListDuplicate(il, xoffset, mdata->bitmap);
    }
    else
    {
        mdata->menuIconNormal = -1;
        mdata->xoffset = -1;
    }
    mdata->nFlags = nFlags;
    mdata->nID = nID;
    return CMenu::AppendMenu(nFlags, nID, (LPCTSTR)mdata);
}


BOOL CSWMenu::InsertODMenu(UINT nPosition, TCHAR* lpstrText, UINT nFlags, UINT nID,
    int nIconNormal)
{
    if (!(nFlags & MF_BYPOSITION))
    {
        int iPosition = 0;
        CSWMenu* pMenu = FindMenuOption(nPosition, iPosition);
        if (pMenu)
        {
            return(pMenu->InsertODMenu(iPosition, lpstrText, nFlags | MF_BYPOSITION, nID, nIconNormal));
        }
        else
        {
            return FALSE;
        }
    }

    if (!nID)
        nFlags = MF_SEPARATOR | MF_OWNERDRAW | MF_BYPOSITION;
    else if (!(nFlags & MF_OWNERDRAW))
        nFlags |= MF_OWNERDRAW;
    int menustart = 0;
    if (nFlags & MF_POPUP)
    {
        if (m_loadmenu)
        {
            menustart = GetMenuStart();
            if (nPosition < (UINT)menustart)
                menustart = 0;
        }
        m_AllSubMenus.Add((HMENU)nID);
        m_SubMenus.Add((HMENU)nID);
    }

    if (nPosition == (UINT)-1)
        nPosition = GetMenuItemCount();
    CSWMenuData* mdata = new CSWMenuData;
    nPosition++;
    m_MenuList.InsertAt(nPosition - menustart, mdata);
    mdata->SetString(lpstrText);
    mdata->menuIconNormal = nIconNormal;
    mdata->xoffset = -1;
    if (nIconNormal >= 0)
    {
        CImageList bitmap;
        int xoffset = 0;
        LoadFromToolBar(nID, nIconNormal, xoffset);
        if (mdata->bitmap)
        {
            mdata->bitmap->DeleteImageList();
            mdata->bitmap = NULL;
        }

        bitmap.Create(m_iconX, m_iconY, ILC_COLORDDB | ILC_MASK, 1, 1);
        if (AddBitmapToImageList(&bitmap, nIconNormal))
        {
            mdata->global_offset = AddToGlobalImageList(&bitmap, xoffset, nID);
        }
    }
    else
    {
        mdata->global_offset = GlobalImageListOffset(nID);
    }

    mdata->nFlags = nFlags;
    mdata->nID = nID;
    BOOL returnflag = CMenu::InsertMenu(nPosition, nFlags, nID, (LPCTSTR)mdata);
    if (m_loadmenu)
        RemoveTopLevelOwnerDraw();
    return returnflag;
}


BOOL CSWMenu::InsertODMenu(UINT nPosition, TCHAR* lpstrText, UINT nFlags, UINT nID,
    CImageList* il, int xoffset)
{
    if (!(nFlags & MF_BYPOSITION))
    {
        int iPosition = 0;
        CSWMenu* pMenu = FindMenuOption(nPosition, iPosition);
        if (pMenu)
        {
            return(pMenu->InsertODMenu(iPosition, lpstrText, nFlags | MF_BYPOSITION, nID, il, xoffset));
        }
        else
        {
            return FALSE;
        }
    }

    if (!nID)
        nFlags = MF_SEPARATOR | MF_OWNERDRAW | MF_BYPOSITION;
    else if (!(nFlags & MF_OWNERDRAW))
        nFlags |= MF_OWNERDRAW;

    if (nFlags & MF_POPUP)
    {
        m_AllSubMenus.Add((HMENU)nID);
        m_SubMenus.Add((HMENU)nID);
    }

    if (nPosition == (UINT)-1)
        nPosition = GetMenuItemCount();

    CSWMenuData* mdata = new CSWMenuData;
    m_MenuList.InsertAt(nPosition, mdata);
    mdata->SetString(lpstrText);
    mdata->menuIconNormal = -1;
    mdata->xoffset = -1;
    if (il)
    {
        if (mdata->bitmap)
        {
            mdata->bitmap->DeleteImageList();
            mdata->bitmap = NULL;
        }

        mdata->global_offset = AddToGlobalImageList(il, xoffset, nID);
    }

    mdata->nFlags = nFlags;
    mdata->nID = nID;
    return CMenu::InsertMenu(nPosition, nFlags, nID, (LPCTSTR)mdata);
}


BOOL CSWMenu::ModifyMenu(TCHAR* lpstrText, UINT nID, int nIconNormal)
{
    int nLoc;
    CSWMenuData* mdata;
    CArray<CSWMenu*, CSWMenu*>bcsubs;
    CArray<int, int&>bclocs;
    CSWMenu* psubmenu = FindMenuOption(nID, nLoc);
    do {
        if (psubmenu && nLoc >= 0)
            mdata = psubmenu->m_MenuList[nLoc];
        else
        {
            mdata = new CSWMenuData;
            m_MenuList.Add(mdata);
        }

        ASSERT(mdata);
        if (lpstrText)
            mdata->SetString(lpstrText);
        mdata->menuIconNormal = -1;
        mdata->xoffset = -1;
        if (nIconNormal >= 0)
        {
            CImageList bitmap;
            int xoffset = 0;
            LoadFromToolBar(nID, nIconNormal, xoffset);
            if (mdata->bitmap)
            {
                mdata->bitmap->DeleteImageList();
                mdata->bitmap = NULL;
            }

            bitmap.Create(m_iconX, m_iconY, ILC_COLORDDB | ILC_MASK, 1, 1);
            if (AddBitmapToImageList(&bitmap, nIconNormal))
            {
                mdata->global_offset = AddToGlobalImageList(&bitmap, xoffset, nID);
            }
        }
        else
        {
            mdata->global_offset = GlobalImageListOffset(nID);
        }

        mdata->nFlags &= ~(MF_BYPOSITION);
        mdata->nFlags |= MF_OWNERDRAW;
        mdata->nID = nID;
        bcsubs.Add(psubmenu);
        bclocs.Add(nLoc);
        if (psubmenu && nLoc >= 0)
            psubmenu = FindAnotherMenuOption(nID, nLoc, bcsubs, bclocs);
        else
            psubmenu = NULL;
    } while (psubmenu);

    return CMenu::ModifyMenu(nID, mdata->nFlags, nID, (LPCTSTR)mdata);
}

BOOL CSWMenu::ModifyMenu(TCHAR* lpstrText, UINT nID, CImageList* il, int xoffset)
{
    int nLoc;
    CSWMenuData* mdata;
    CArray<CSWMenu*, CSWMenu*>bcsubs;
    CArray<int, int&>bclocs;
    CSWMenu* psubmenu = FindMenuOption(nID, nLoc);
    do {
        if (psubmenu && nLoc >= 0)
            mdata = psubmenu->m_MenuList[nLoc];
        else
        {
            mdata = new CSWMenuData;
            m_MenuList.Add(mdata);
        }

        if (lpstrText)
            mdata->SetString(lpstrText);
        mdata->menuIconNormal = -1;
        mdata->xoffset = -1;
        if (il)
        {
            if (mdata->bitmap)
            {
                mdata->bitmap->DeleteImageList();
                mdata->bitmap = NULL;
            }

            mdata->global_offset = AddToGlobalImageList(il, xoffset, nID);
        }

        mdata->nFlags &= ~(MF_BYPOSITION);
        mdata->nFlags |= MF_OWNERDRAW;
        mdata->nID = nID;
        bcsubs.Add(psubmenu);
        bclocs.Add(nLoc);
        if (psubmenu && nLoc >= 0)
            psubmenu = FindAnotherMenuOption(nID, nLoc, bcsubs, bclocs);
        else psubmenu = NULL;
    } while (psubmenu);

    return CMenu::ModifyMenu(nID, mdata->nFlags, nID, (LPCTSTR)mdata);
}


BOOL CSWMenu::ModifyMenu(TCHAR* lpstrText, UINT nID, CBitmap* bmp)
{
    if (bmp)
    {
        CImageList temp;
        temp.Create(m_iconX, m_iconY, ILC_COLORDDB | ILC_MASK, 1, 1);
        if (m_bBitmapBackgroundFlag)
            temp.Add(bmp, m_clrBackgroundBitmatMask);
        else
            temp.Add(bmp, GetSysColor(COLOR_3DFACE));
        return ModifyMenu(lpstrText, nID, &temp, 0);
    }

    return ModifyMenu(lpstrText, nID, NULL, 0);
}

BOOL CSWMenu::ModifyMenu(TCHAR* lpstrText, UINT nID, COLORREF fill, COLORREF border, int hatchstyle, CSize* pSize)
{
    CWnd* pWnd = AfxGetMainWnd();
    CDC* pDC = pWnd->GetDC();
    SIZE sz;
    if (!pSize)
    {
        sz.cx = m_iconX;
        sz.cy = m_iconY;
    }
    else
    {
        sz.cx = pSize->cx;
        sz.cy = pSize->cy;
    }

    CSize bitmap_size(sz);
    CSize icon_size(m_iconX, m_iconY);
    CBitmap bmp;
    ColorBitmap(pDC, bmp, bitmap_size, icon_size, fill, border, hatchstyle);
    pWnd->ReleaseDC(pDC);
    return ModifyMenu(lpstrText, nID, &bmp);
}

BOOL CSWMenu::ModifyMenu(TCHAR* lpstrText, TCHAR* OptionText,
    int nIconNormal)
{
    CSWMenuData* mdata;
    mdata = FindMenuOption(OptionText);
    if (mdata) {
        if (lpstrText)
            mdata->SetString(lpstrText);
        mdata->menuIconNormal = nIconNormal;
        mdata->xoffset = -1;
        if (nIconNormal >= 0)
        {
            mdata->xoffset = 0;
            if (mdata->bitmap)
                mdata->bitmap->DeleteImageList();
            else
                mdata->bitmap = new CImageList;
            mdata->bitmap->Create(m_iconX, m_iconY, ILC_COLORDDB | ILC_MASK, 1, 1);
            if (!AddBitmapToImageList(mdata->bitmap, nIconNormal))
            {
                mdata->bitmap->DeleteImageList();
                delete mdata->bitmap;
                mdata->bitmap = NULL;
                mdata->menuIconNormal = nIconNormal = -1;
                mdata->xoffset = -1;
            }
        }

        return TRUE;
    }

    return FALSE;
}

BOOL CSWMenu::SetImageForPopupFromToolbar(TCHAR* strPopUpText, UINT toolbarID, UINT nIDFind)
{
    CWnd* pWnd = AfxGetMainWnd();
    if (pWnd == NULL)pWnd = CWnd::GetDesktopWindow();
    CToolBar bar;
    bar.Create(pWnd);
    if (bar.LoadToolBar(toolbarID))
    {
        CSWMenuData* mdata = FindMenuOption(strPopUpText);
        if (mdata != NULL)
        {
            if (mdata->bitmap != NULL)
            {
                mdata->bitmap->DeleteImageList();
                delete mdata->bitmap;
                mdata->bitmap = NULL;
            }

            CImageList imglist;
            imglist.Create(m_iconX, m_iconY, ILC_COLORDDB | ILC_MASK, 1, 1);
            if (AddBitmapToImageList(&imglist, toolbarID))
            {
                int ind = bar.CommandToIndex(nIDFind);
                if (ind < 0)
                    return FALSE;

                UINT dummyID, dummyStyle;
                int image_index;
                bar.GetButtonInfo(ind, dummyID, dummyStyle, image_index);
                ASSERT(dummyID == nIDFind);

                mdata->bitmap = new CImageList;
                mdata->bitmap->Create(m_iconX, m_iconY, ILC_COLORDDB | ILC_MASK, 0, 1);
                mdata->bitmap->Add(imglist.ExtractIcon(image_index));
                mdata->menuIconNormal = toolbarID;
                mdata->xoffset = 0;

                return TRUE;
            }
            else
            {
                mdata->menuIconNormal = -1;
                mdata->xoffset = -1;
            }
        }
    }

    return FALSE;
}

CSWMenuData* CSWMenu::NewMenu(UINT pos, UINT nFlags, UINT nID, CString string)
{
    CSWMenuData* mdata;

    mdata = new CSWMenuData;
    mdata->menuIconNormal = -1;
    mdata->xoffset = -1;
    mdata->SetString(string);
    mdata->nFlags = nFlags;
    mdata->nID = nID;

    // if(nFlags & MF_POPUP)m_AllSubMenus.Add((HMENU)nID);

    if (nFlags & MF_OWNERDRAW)
    {
        ASSERT(!(nFlags & MF_STRING));
        CMenu::ModifyMenu(pos, nFlags, nID, (LPCTSTR)mdata);
    }
    else if (nFlags & MF_STRING)
    {
        ASSERT(!(nFlags & MF_OWNERDRAW));
        CMenu::ModifyMenu(pos, nFlags, nID, mdata->GetString());
    }
    else
    {
        ASSERT(nFlags & MF_SEPARATOR);
        CMenu::ModifyMenu(pos, nFlags, nID);
    }

    return mdata;
}

BOOL CSWMenu::LoadToolbars(const UINT* arID, int n)
{
    ASSERT(arID);
    BOOL returnflag = TRUE;
    for (int i = 0; i < n; ++i)
    {
        if (!LoadToolbar(arID[i]))
            returnflag = FALSE;
    }

    return returnflag;
}

BOOL CSWMenu::LoadToolbar(UINT nToolBar)
{
    UINT nID, nStyle;
    BOOL returnflag = FALSE;
    CToolBar bar;
    int xoffset = -1, xset;

    CWnd* pWnd = AfxGetMainWnd();
    if (pWnd == NULL)pWnd = CWnd::GetDesktopWindow();
    bar.Create(pWnd);
    if (bar.LoadToolBar(nToolBar))
    {
        CImageList imglist;
        imglist.Create(m_iconX, m_iconY, ILC_COLORDDB | ILC_MASK, 1, 1);
        if (AddBitmapToImageList(&imglist, nToolBar))
        {
            returnflag = TRUE;
            for (int i = 0; i < bar.GetCount(); ++i)
            {
                nID = bar.GetItemID(i);
                if (nID && GetMenuState(nID, MF_BYCOMMAND) != 0xFFFFFFFF)
                {
                    xoffset = bar.CommandToIndex(nID);
                    if (xoffset >= 0)
                    {
                        bar.GetButtonInfo(xoffset, nID, nStyle, xset);
                        if (xset > 0)
                            xoffset = xset;
                    }

                    ModifyMenu(NULL, nID, &imglist, xoffset);
                }
            }
        }
    }

    return returnflag;
}

BOOL CSWMenu::LoadFromToolBar(UINT nID, UINT nToolBar, int& xoffset)
{
    int xset, offset;
    UINT nStyle;
    BOOL returnflag = FALSE;
    CToolBar bar;

    CWnd* pWnd = AfxGetMainWnd();
    if (pWnd == NULL)pWnd = CWnd::GetDesktopWindow();
    bar.Create(pWnd);
    if (bar.LoadToolBar(nToolBar))
    {
        offset = bar.CommandToIndex(nID);
        if (offset >= 0)
        {
            bar.GetButtonInfo(offset, nID, nStyle, xset);
            if (xset > 0)
                xoffset = xset;
            returnflag = TRUE;
        }
    }

    return returnflag;
}

CSWMenuData* CSWMenu::FindMenuItem(UINT nID)
{
    CSWMenuData* pData = NULL;
    int i;

    for (i = 0; i <= m_MenuList.GetUpperBound(); i++)
    {
        if (m_MenuList[i]->nID == nID)
        {
            pData = m_MenuList[i];
            break;
        }
    }

    if (!pData)
    {
        int loc;
        CSWMenu* pMenu = FindMenuOption(nID, loc);
        ASSERT(pMenu != this);
        if (loc >= 0)
        {
            return pMenu->FindMenuItem(nID);
        }
    }

    return pData;
}

CSWMenu* CSWMenu::FindAnotherMenuOption(int nId, int& nLoc, CArray<CSWMenu*, CSWMenu*>& bcsubs,
    CArray<int, int&>& bclocs)
{
    int i, numsubs, j;
    CSWMenu* psubmenu, * pgoodmenu;
    BOOL foundflag;

    for (i = 0; i < (int)(GetMenuItemCount()); ++i)
    {
#ifdef _CPPRTTI 
        psubmenu = dynamic_cast<CSWMenu*>(GetSubMenu(i));
#else
        psubmenu = (CSWMenu*)GetSubMenu(i);
#endif
        if (psubmenu)
        {
            pgoodmenu = psubmenu->FindAnotherMenuOption(nId, nLoc, bcsubs, bclocs);
            if (pgoodmenu)
                return pgoodmenu;
        }
        else if (nId == (int)GetMenuItemID(i))
        {
            numsubs = bcsubs.GetSize();
            foundflag = TRUE;
            for (j = 0; j < numsubs; ++j)
            {
                if (bcsubs[j] == this && bclocs[j] == i)
                {
                    foundflag = FALSE;
                    break;
                }
            }

            if (foundflag)
            {
                nLoc = i;
                return this;
            }
        }
    }

    nLoc = -1;
    return NULL;
}

CSWMenu* CSWMenu::FindMenuOption(int nId, int& nLoc)
{
    int i;
    CSWMenu* psubmenu, * pgoodmenu;

    for (i = 0; i < (int)(GetMenuItemCount()); ++i)
    {
#ifdef _CPPRTTI 
        psubmenu = dynamic_cast<CSWMenu*>(GetSubMenu(i));
#else
        psubmenu = (CSWMenu*)GetSubMenu(i);
#endif
        if (psubmenu)
        {
            pgoodmenu = psubmenu->FindMenuOption(nId, nLoc);
            if (pgoodmenu)return(pgoodmenu);
        }
        else if (nId == (int)GetMenuItemID(i))
        {
            nLoc = i;
            return this;
        }
    }

    nLoc = -1;
    return NULL;
}

CSWMenuData* CSWMenu::FindMenuOption(TCHAR* lpstrText)
{
    int i, j;
    CSWMenu* psubmenu;
    CSWMenuData* pmenulist;

    for (i = 0; i < (int)(GetMenuItemCount()); ++i)
    {
#ifdef _CPPRTTI 
        psubmenu = dynamic_cast<CSWMenu*>(GetSubMenu(i));
#else
        psubmenu = (CSWMenu*)GetSubMenu(i);
#endif
        if (psubmenu)
        {
            pmenulist = psubmenu->FindMenuOption(lpstrText);
            if (pmenulist)
                return pmenulist;
        }
        else
        {
            const TCHAR* szWide;
            for (j = 0; j <= m_MenuList.GetUpperBound(); ++j)
            {
                szWide = m_MenuList[j]->GetString();
                if (szWide && !_tcscmp(lpstrText, szWide))
                    return m_MenuList[j];
            }
        }
    }

    return NULL;
}

BOOL CSWMenu::LoadMenu(int nResource)
{
    return CSWMenu::LoadMenu(MAKEINTRESOURCE(nResource));
}

BOOL CSWMenu::LoadMenu(LPCTSTR lpszResourceName)
{
    HINSTANCE hInst = AfxFindResourceHandle(lpszResourceName, RT_MENU);
    HRSRC hRsrc = ::FindResource(hInst, lpszResourceName, RT_MENU);
    if (hRsrc == NULL)
    {
        hInst = NULL;
        hRsrc = ::FindResource(hInst, lpszResourceName, RT_MENU);
    }

    if (hRsrc == NULL)
        return FALSE;

    HGLOBAL hGlobal = LoadResource(hInst, hRsrc);
    if (hGlobal == NULL)
        return FALSE;

    DestroyMenu();
    if (!CMenu::CreateMenu())
        return FALSE;

    MENUITEMTEMPLATEHEADER* pTpHdr = (MENUITEMTEMPLATEHEADER*)LockResource(hGlobal);
    BYTE* pTp = (BYTE*)pTpHdr + (sizeof(MENUITEMTEMPLATEHEADER) + pTpHdr->offset);

    int j = 0;
    WORD    dwFlags = 0;
    WORD    dwID = 0;
    UINT    uFlags;
    wchar_t* szCaption = NULL;
    int      nLen = 0;
    CTypedPtrArray<CPtrArray, CSWMenu*>  m_Stack;
    CArray<BOOL, BOOL>  m_StackEnd;
    m_Stack.Add(this);
    m_StackEnd.Add(FALSE);

    do
    {
        memcpy(&dwFlags, pTp, sizeof(WORD));
        pTp += sizeof(WORD);
        if (!(dwFlags & MF_POPUP))
        {
            memcpy(&dwID, pTp, sizeof(WORD));
            pTp += sizeof(WORD);
        }
        else dwID = 0;

        uFlags = (UINT)dwFlags;
        if (uFlags & MF_END)
            uFlags -= MF_END;

        nLen = 0;
        szCaption = new wchar_t[wcslen((wchar_t*)pTp) + 1];
        wcscpy_s(szCaption, wcslen((wchar_t*)pTp) + 1, (wchar_t*)pTp);
        pTp = &pTp[(wcslen((wchar_t*)pTp) + 1) * sizeof(wchar_t)];

        if (dwFlags & MF_POPUP)
        {
            if (dwFlags & MF_END)
                m_StackEnd.SetAt(m_Stack.GetUpperBound(), TRUE);
            CSWMenu* pSubMenu = new CSWMenu;
            pSubMenu->m_bUnSelectCheck = m_bUnSelectCheck;
            pSubMenu->m_nSelectCheck = m_nSelectCheck;
            pSubMenu->m_imagelistCheckBitmap = m_imagelistCheckBitmap;
            pSubMenu->m_bCheckBitmapShare = TRUE;
            pSubMenu->CreatePopupMenu();
#ifdef UNICODE
            m_Stack[m_Stack.GetUpperBound()]->AppendODMenu(szCaption, uFlags, (UINT)pSubMenu->m_hMenu, -1);
#else
            USES_CONVERSION;
            m_Stack[m_Stack.GetUpperBound()]->AppendODMenu(W2A(szCaption), uFlags, (UINT)pSubMenu->m_hMenu, -1);
#endif // UNICODE

            m_Stack.Add(pSubMenu);
            m_StackEnd.Add(FALSE);
        }
        else
        {
#ifdef UNICODE
            m_Stack[m_Stack.GetUpperBound()]->AppendODMenu(szCaption, uFlags, dwID, -1);
#else
            USES_CONVERSION;;
            m_Stack[m_Stack.GetUpperBound()]->AppendODMenu(W2A(szCaption), uFlags, dwID, -1);
#endif // UNICODE

            if (dwFlags & MF_END)
                m_StackEnd.SetAt(m_Stack.GetUpperBound(), TRUE);
            j = m_Stack.GetUpperBound();
            while (j >= 0 && m_StackEnd.GetAt(j))
            {
                m_Stack[m_Stack.GetUpperBound()]->InsertSpaces();
                m_Stack.RemoveAt(j);
                m_StackEnd.RemoveAt(j);
                --j;
            }
        }

        delete[] szCaption;
    } while (m_Stack.GetUpperBound() != -1);

    for (int i = 0; i < GetMenuItemCount(); ++i)
    {
        CString str = m_MenuList[i]->GetString();
        if (GetSubMenu(i))
        {
            m_MenuList[i]->nFlags = MF_POPUP | MF_BYPOSITION;
            CMenu::ModifyMenu(i, MF_POPUP | MF_BYPOSITION, (UINT)GetSubMenu(i)->m_hMenu, str);
        }
        else
        {
            m_MenuList[i]->nFlags = MF_STRING | MF_BYPOSITION;
            CMenu::ModifyMenu(i, MF_STRING | MF_BYPOSITION, m_MenuList[i]->nID, str);
        }
    }

    m_loadmenu = TRUE;
    return TRUE;
}

int CSWMenu::GetMenuStart()
{
    if (!m_loadmenu)
        return 0;
    CString name, str;
    int menuloc = -1, listloc = -1, menustart = 0, i = 0, j = 0;
    int nummenulist = m_MenuList.GetSize();
    int nummenu = (int)GetMenuItemCount();
    while (i < nummenu && menuloc == -1)
    {
        GetMenuString(i, name, MF_BYPOSITION);
        if (name.GetLength() > 0)
        {
            for (j = 0; j < nummenulist; ++j)
            {
                str = m_MenuList[j]->GetString();
                if (name == str)
                {
                    menuloc = i;
                    listloc = j;
                    break;
                }
            }
        }

        ++i;
    }

    if (menuloc >= 0 && listloc >= 0 && menuloc >= listloc)
        menustart = menuloc - listloc;

    return menustart;
}

void CSWMenu::RemoveTopLevelOwnerDraw()
{
    CString str;
    int i = 0, j = 0;
    int nummenulist = m_MenuList.GetSize(), menustart;
    menustart = GetMenuStart();
    for (i = menustart, j = 0; i < (int)GetMenuItemCount(); ++i, ++j)
    {
        if (j < nummenulist)
        {
            str = m_MenuList[j]->GetString();
            if (GetSubMenu(i))
            {
                m_MenuList[j]->nFlags = MF_POPUP | MF_BYPOSITION;
                CMenu::ModifyMenu(i, MF_POPUP | MF_BYPOSITION, (UINT)GetSubMenu(i)->m_hMenu, str);
            }
        }
    }
}

void CSWMenu::InsertSpaces()
{
    if (IsLunaMenuStyle())
        if (!m_bSpaceAccelerators)
            return;
        else if (!m_bOrgSpaceAccelerators)
            return;

    int i, j, numitems, maxlength;
    CString string, newstring;
    CSize t;
    CFont m_fontMenu;
    LOGFONT m_lf;

    ZeroMemory((PVOID)&m_lf, sizeof(LOGFONT));
    NONCLIENTMETRICS nm;
    nm.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, nm.cbSize, &nm, 0);
    m_lf = nm.lfMenuFont;
    m_fontMenu.CreateFontIndirect(&m_lf);

    CWnd* pWnd = AfxGetMainWnd();
    if (pWnd == NULL)pWnd = CWnd::GetDesktopWindow();
    CDC* pDC = pWnd->GetDC();
    CFont* pFont = pDC->SelectObject(&m_fontMenu);

    numitems = GetMenuItemCount();
    maxlength = -1;
    for (i = 0; i < numitems; ++i)
    {
        string = m_MenuList[i]->GetString();
        j = string.Find(TCHAR('\t'));
        newstring.Empty();
        if (j != -1)
            newstring = string.Left(j);
        else
            newstring = string;
        newstring += _T(" ");
        LPCTSTR lpstrText = (LPCTSTR)newstring;
        t = pDC->GetTextExtent(lpstrText, strlen(lpstrText));
        if (t.cx > maxlength)
            maxlength = t.cx;
    }

    for (i = 0; i < numitems; ++i)
    {
        string = m_MenuList[i]->GetString();
        j = string.Find(TCHAR('\t'));
        if (j != -1)
        {
            newstring.Empty();
            newstring = string.Left(j);
            LPCTSTR lpstrText = (LPCTSTR)(newstring);
            t = pDC->GetTextExtent(lpstrText, _tcslen(lpstrText));
            while (t.cx < maxlength)
            {
                newstring += _T(' ');
                LPCTSTR lpstrText = (LPCTSTR)(newstring);
                t = pDC->GetTextExtent(lpstrText, strlen(lpstrText));
            }
            newstring += string.Mid(j);
            m_MenuList[i]->SetString(newstring);
        }
    }

    pDC->SelectObject(pFont);
    pWnd->ReleaseDC(pDC);
    m_fontMenu.DeleteObject();
}

void CSWMenu::LoadCheckmarkBitmap(int unselect, int select)
{
    if (unselect > 0 && select > 0)
    {
        m_nSelectCheck = select;
        m_bUnSelectCheck = unselect;
        if (m_imagelistCheckBitmap)m_imagelistCheckBitmap->DeleteImageList();
        else m_imagelistCheckBitmap = new(CImageList);
        m_imagelistCheckBitmap->Create(m_iconX, m_iconY, ILC_MASK, 2, 1);
        BOOL flag1 = AddBitmapToImageList(m_imagelistCheckBitmap, unselect);
        BOOL flag2 = AddBitmapToImageList(m_imagelistCheckBitmap, select);
        if (!flag1 || !flag2)
        {
            m_imagelistCheckBitmap->DeleteImageList();
            delete m_imagelistCheckBitmap;
            m_imagelistCheckBitmap = NULL;
        }
    }
}

BOOL CSWMenu::GetMenuText(UINT id, CString& string, UINT nFlags/*= MF_BYPOSITION*/)
{
    BOOL returnflag = FALSE;

    if (MF_BYPOSITION & nFlags)
    {
        UINT numMenuItems = m_MenuList.GetUpperBound();
        if (id <= numMenuItems)
        {
            string = m_MenuList[id]->GetString();
            returnflag = TRUE;
        }
    }
    else
    {
        int uiLoc;
        CSWMenu* pMenu = FindMenuOption(id, uiLoc);
        if (NULL != pMenu)
            returnflag = pMenu->GetMenuText(uiLoc, string);
    }

    return returnflag;
}

void CSWMenu::DrawRadioDot(CDC* pDC, int x, int y, COLORREF color)
{
    CRect rcDot(x, y, x + 6, y + 6);
    CBrush brush;
    CPen pen;
    brush.CreateSolidBrush(color);
    pen.CreatePen(PS_SOLID, 0, color);
    CBrush* pOldBrush = pDC->SelectObject(&brush);
    CPen* pOldPen = pDC->SelectObject(&pen);
    pDC->Ellipse(&rcDot);
    pDC->SelectObject(pOldBrush);
    pDC->SelectObject(pOldPen);
    pen.DeleteObject();
    brush.DeleteObject();
}

void CSWMenu::DrawCheckMark(CDC* pDC, int x, int y, COLORREF color, BOOL narrowflag)
{
    int dp = 0;
    CPen m_penBack;
    m_penBack.CreatePen(PS_SOLID, 0, color);
    CPen* pOldPen = pDC->SelectObject(&m_penBack);
    if (narrowflag)dp = 1;
    pDC->MoveTo(x, y + 2);
    pDC->LineTo(x, y + 5 - dp);

    pDC->MoveTo(x + 1, y + 3);
    pDC->LineTo(x + 1, y + 6 - dp);

    pDC->MoveTo(x + 2, y + 4);
    pDC->LineTo(x + 2, y + 7 - dp);

    pDC->MoveTo(x + 3, y + 3);
    pDC->LineTo(x + 3, y + 6 - dp);

    pDC->MoveTo(x + 4, y + 2);
    pDC->LineTo(x + 4, y + 5 - dp);

    pDC->MoveTo(x + 5, y + 1);
    pDC->LineTo(x + 5, y + 4 - dp);

    pDC->MoveTo(x + 6, y);
    pDC->LineTo(x + 6, y + 3 - dp);

    pDC->SelectObject(pOldPen);
    m_penBack.DeleteObject();
}

CSWMenuData* CSWMenu::FindMenuList(UINT nID)
{
    for (int i = 0; i <= m_MenuList.GetUpperBound(); ++i)
    {
        if (m_MenuList[i]->nID == nID && !m_MenuList[i]->syncflag)
        {
            m_MenuList[i]->syncflag = 1;
            return(m_MenuList[i]);
        }
    }

    return NULL;
}

void CSWMenu::InitializeMenuList(int value)
{
    for (int i = 0; i <= m_MenuList.GetUpperBound(); ++i)
        m_MenuList[i]->syncflag = value;
}

void CSWMenu::DeleteMenuList()
{
    for (int i = 0; i <= m_MenuList.GetUpperBound(); ++i)
    {
        if (!m_MenuList[i]->syncflag)
        {
            delete m_MenuList[i];
        }
    }
}


void CSWMenu::SynchronizeMenu()
{
    CTypedPtrArray<CPtrArray, CSWMenuData*> temp;
    CSWMenuData* mdata;
    CString string;
    UINT submenu, nID = 0, state;

    InitializeMenuList(0);
    for (int j = 0; j < GetMenuItemCount(); ++j)
    {
        mdata = NULL;
        state = GetMenuState(j, MF_BYPOSITION);
        if (state & MF_POPUP)
        {
            submenu = (UINT)GetSubMenu(j)->m_hMenu;
            mdata = FindMenuList(submenu);
            GetMenuString(j, string, MF_BYPOSITION);
            if (!mdata)mdata = NewMenu(j, (state & 0xFF) | MF_BYPOSITION | MF_POPUP | MF_OWNERDRAW, submenu, string);
            else if (string.GetLength() > 0)
                mdata->SetString(string);
        }
        else if (state & MF_SEPARATOR)
        {
            mdata = FindMenuList(0);
            if (!mdata)
                mdata = NewMenu(j, state | MF_BYPOSITION | MF_SEPARATOR | MF_OWNERDRAW, 0, _T(""));
            else
                CMenu::ModifyMenu(j, mdata->nFlags, nID, (LPCTSTR)mdata);
        }
        else
        {
            nID = GetMenuItemID(j);
            mdata = FindMenuList(nID);
            GetMenuString(j, string, MF_BYPOSITION);
            if (!mdata)mdata = NewMenu(j, state | MF_BYPOSITION | MF_OWNERDRAW, nID, string);
            else
            {
                mdata->nFlags = state | MF_BYPOSITION | MF_OWNERDRAW;
                if (string.GetLength() > 0)
                    mdata->SetString(string);
                CMenu::ModifyMenu(j, mdata->nFlags, nID, (LPCTSTR)mdata);
            }
        }

        if (mdata)
            temp.Add(mdata);
    }

    DeleteMenuList();
    m_MenuList.RemoveAll();
    m_MenuList.Append(temp);
    temp.RemoveAll();
}

void CSWMenu::UpdateMenu(CMenu* pmenu)
{
#ifdef _CPPRTTI 
    CSWMenu* psubmenu = dynamic_cast<CSWMenu*>(pmenu);
#else
    CSWMenu* psubmenu = (CSWMenu*)pmenu;
#endif
    if (psubmenu)psubmenu->SynchronizeMenu();
}

LRESULT CSWMenu::FindKeyboardShortcut(UINT nChar, UINT nFlags, CMenu* pMenu)
{
#ifdef _CPPRTTI 
    CSWMenu* pCSWMenu = dynamic_cast<CSWMenu*>(pMenu);
#else
    CSWMenu* pCSWMenu = (CSWMenu*)pMenu;
#endif
    if (pCSWMenu && nFlags & MF_POPUP)
    {
        CString key(_T('&'), 2);
        key.SetAt(1, (TCHAR)nChar);
        key.MakeLower();
        CString menutext;
        int menusize = pCSWMenu->GetMenuItemCount();
        if (menusize != (pCSWMenu->m_MenuList.GetUpperBound() + 1))
            pCSWMenu->SynchronizeMenu();
        for (int i = 0; i < menusize; ++i)
        {
            if (pCSWMenu->GetMenuText(i, menutext))
            {
                menutext.MakeLower();
                if (menutext.Find(key) >= 0)
                    return (MAKELRESULT(i, 2));
            }
        }
    }

    return 0;
}

void CSWMenu::SetMenuDrawMode(UINT mode)
{
    CSWMenu::m_nDrawMode = mode;
}

void CSWMenu::SetSelectDisableMode(BOOL mode)
{
    CSWMenu::m_bSelectDisabled = mode;
}

void CSWMenu::SetXPBitmap3D(BOOL val)
{
    CSWMenu::m_bDraw3DBitmap = val;
}

BOOL CSWMenu::GetXPBitmap3D()
{
    return CSWMenu::m_bDraw3DBitmap;
}

void CSWMenu::DitherBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth,
    int nHeight, HBITMAP hbm, int nXSrc, int nYSrc,
    COLORREF bgcolor)
{
    ASSERT(hdcDest && hbm);
    ASSERT(nWidth > 0 && nHeight > 0);
    HDC hDC = CreateCompatibleDC(hdcDest);
    ASSERT(hDC);

    if (hDC)
    {
        HDC bwDC = CreateCompatibleDC(hDC);
        ASSERT(bwDC);

        if (bwDC)
        {
            struct {
                BITMAPINFOHEADER bmiHeader;
                RGBQUAD      bmiColors[2];
            } RGBBWBITMAPINFO = {

                {    // a BITMAPINFOHEADER
                    sizeof(BITMAPINFOHEADER),  // biSize 
                        nWidth,         // biWidth; 
                        nHeight,        // biHeight; 
                        1,            // biPlanes; 
                        1,            // biBitCount 
                        BI_RGB,         // biCompression; 
                        0,            // biSizeImage; 
                        0,            // biXPelsPerMeter; 
                        0,            // biYPelsPerMeter; 
                        0,            // biClrUsed; 
                        0            // biClrImportant; 
                },
                {
                    { 0x00, 0x00, 0x00, 0x00 }, { 0xFF, 0xFF, 0xFF, 0x00 }
                }
            };

            VOID* pbitsBW;
            HBITMAP hbmBW = CreateDIBSection(bwDC, (LPBITMAPINFO)&RGBBWBITMAPINFO, DIB_RGB_COLORS, &pbitsBW, NULL, 0);
            ASSERT(hbmBW);

            if (hbmBW)
            {
                HBITMAP olddib = (HBITMAP)SelectObject(bwDC, hbmBW);
                HBITMAP hdcolddib = (HBITMAP)SelectObject(hDC, hbm);
                BitBlt(bwDC, 0, 0, nWidth, nHeight, hDC, nXSrc, nYSrc, SRCCOPY);
                FillRect(hdcDest, CRect(nXDest, nYDest, nXDest + nWidth, nYDest +
                    nHeight), GetSysColorBrush(bgcolor));
                HBRUSH hb = CreateSolidBrush(GetSysColor(COLOR_3DHILIGHT));
                HBRUSH oldBrush = (HBRUSH)SelectObject(hdcDest, hb);
                BitBlt(hdcDest, nXDest + 1, nYDest + 1, nWidth, nHeight, bwDC, 0, 0, 0xB8074A);
                hb = CreateSolidBrush(GetSysColor(COLOR_3DSHADOW));
                DeleteObject(SelectObject(hdcDest, hb));
                BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, bwDC, 0, 0, 0xB8074A);
                DeleteObject(SelectObject(hdcDest, oldBrush));
                VERIFY(DeleteObject(SelectObject(bwDC, olddib)));
                SelectObject(hDC, hdcolddib);
            }

            VERIFY(DeleteDC(bwDC));
        }

        VERIFY(DeleteDC(hDC));
    }
}

void CSWMenu::GetFadedBitmap(CBitmap& bmp)
{
    CDC dcMem;
    COLORREF bgcol, col;
    BITMAP BitMap;
    bmp.GetBitmap(&BitMap);
    dcMem.CreateCompatibleDC(NULL);
    CBitmap* pMemOldBmp = dcMem.SelectObject(&bmp);
    CDC dcMem2;
    CBrush brush;
    CBitmap bmp2;
    dcMem2.CreateCompatibleDC(NULL);
    bmp2.CreateCompatibleBitmap(&dcMem, BitMap.bmWidth, BitMap.bmHeight);
    col = GetSysColor(COLOR_3DFACE);
    brush.CreateSolidBrush(col);
    CBitmap* pMemOldBmp2 = dcMem2.SelectObject(&bmp2);
    CRect rect(0, 0, BitMap.bmWidth, BitMap.bmHeight);
    dcMem2.FillRect(rect, &brush);
    bgcol = dcMem2.GetPixel(1, 1);
    brush.DeleteObject();
    dcMem2.SelectObject(pMemOldBmp2);
    for (int i = 0; i < BitMap.bmWidth; ++i)
    {
        for (int j = 0; j < BitMap.bmHeight; ++j)
        {
            col = dcMem.GetPixel(i, j);
            if (col != bgcol)dcMem.SetPixel(i, j, LightenColor(col, 0.3));
        }
    }

    dcMem.SelectObject(pMemOldBmp);
}

void CSWMenu::GetTransparentBitmap(CBitmap& bmp)
{
    CDC dcMem;
    COLORREF bgcol, col, newcol;
    BITMAP BitMap;
    bmp.GetBitmap(&BitMap);
    dcMem.CreateCompatibleDC(NULL);
    CBitmap* pMemOldBmp = dcMem.SelectObject(&bmp);
    CDC dcMem2;
    CBrush brush;
    CBitmap bmp2;
    dcMem2.CreateCompatibleDC(NULL);
    bmp2.CreateCompatibleBitmap(&dcMem, BitMap.bmWidth, BitMap.bmHeight);
    col = RGB(192, 192, 192);
    brush.CreateSolidBrush(col);
    CBitmap* pMemOldBmp2 = dcMem2.SelectObject(&bmp2);
    CRect rect(0, 0, BitMap.bmWidth, BitMap.bmHeight);
    dcMem2.FillRect(rect, &brush);
    bgcol = dcMem2.GetPixel(1, 1);
    brush.DeleteObject();
    dcMem2.SelectObject(pMemOldBmp2);
    newcol = GetSysColor(COLOR_3DFACE);
    for (int i = 0; i < BitMap.bmWidth; ++i)
    {
        for (int j = 0; j < BitMap.bmHeight; ++j)
        {
            col = dcMem.GetPixel(i, j);
            if (col == bgcol)dcMem.SetPixel(i, j, newcol);
        }
    }

    dcMem.SelectObject(pMemOldBmp);
}

void CSWMenu::GetDisabledBitmap(CBitmap& bmp, COLORREF background)
{
    CDC dcMem;
    COLORREF bgcol, col, discol;
    BITMAP BitMap;
    bmp.GetBitmap(&BitMap);
    dcMem.CreateCompatibleDC(NULL);
    CBitmap* pMemOldBmp = dcMem.SelectObject(&bmp);
    CDC dcMem2;
    CBrush brush;
    CBitmap bmp2;
    dcMem2.CreateCompatibleDC(NULL);
    bmp2.CreateCompatibleBitmap(&dcMem, BitMap.bmWidth, BitMap.bmHeight);
    col = GetSysColor(COLOR_3DFACE);
    brush.CreateSolidBrush(col);
    CBitmap* pMemOldBmp2 = dcMem2.SelectObject(&bmp2);
    CRect rect(0, 0, BitMap.bmWidth, BitMap.bmHeight);
    dcMem2.FillRect(rect, &brush);
    bgcol = dcMem2.GetPixel(1, 1);
    brush.DeleteObject();
    dcMem2.SelectObject(pMemOldBmp2);
    discol = GetSysColor(COLOR_BTNSHADOW);
    for (int i = 0; i < BitMap.bmWidth; ++i)
    {
        for (int j = 0; j < BitMap.bmHeight; ++j)
        {
            col = dcMem.GetPixel(i, j);
            if (col != bgcol)
            {
                BYTE r = GetRValue(col);
                BYTE g = GetGValue(col);
                BYTE b = GetBValue(col);
                int avgcol = (r + g + b) / 3;
                double factor = avgcol / 255.0;
                dcMem.SetPixel(i, j, LightenColor(discol, factor));
            }
            else
            {
                if (background)
                    dcMem.SetPixel(i, j, background);
            }
        }
    }

    dcMem.SelectObject(pMemOldBmp);
}

void CSWMenu::GetShadowBitmap(CBitmap& bmp)
{
    CDC dcMem;
    COLORREF bgcol, col, shadowcol = GetSysColor(COLOR_BTNSHADOW);
    BITMAP BitMap;
    bmp.GetBitmap(&BitMap);
    dcMem.CreateCompatibleDC(NULL);
    CBitmap* pMemOldBmp = dcMem.SelectObject(&bmp);
    CDC dcMem2;
    CBrush brush;
    CBitmap bmp2;
    dcMem2.CreateCompatibleDC(NULL);
    bmp2.CreateCompatibleBitmap(&dcMem, BitMap.bmWidth, BitMap.bmHeight);
    col = GetSysColor(COLOR_3DFACE);
    brush.CreateSolidBrush(col);
    CBitmap* pMemOldBmp2 = dcMem2.SelectObject(&bmp2);
    CRect rect(0, 0, BitMap.bmWidth, BitMap.bmHeight);
    dcMem2.FillRect(rect, &brush);
    bgcol = dcMem2.GetPixel(1, 1);
    brush.DeleteObject();
    dcMem2.SelectObject(pMemOldBmp2);
    for (int i = 0; i < BitMap.bmWidth; ++i)
    {
        for (int j = 0; j < BitMap.bmHeight; ++j)
        {
            col = dcMem.GetPixel(i, j);
            if (col != bgcol)dcMem.SetPixel(i, j, shadowcol);
        }
    }

    dcMem.SelectObject(pMemOldBmp);
}

BOOL CSWMenu::AddBitmapToImageList(CImageList* bmplist, UINT nResourceID)
{
    BOOL bReturn = FALSE;
    if (m_bDynIcons)
    {
        bmplist->Add((HICON)nResourceID);
        bReturn = TRUE;
    }
    else
    {
        HBITMAP hbmp = LoadSysColorBitmap(nResourceID);
        if (hbmp)
        {
            CBitmap bmp;
            bmp.Attach(hbmp);
            if (m_bBitmapBackgroundFlag)
            {
                if (bmplist->Add(&bmp, m_clrBackgroundBitmatMask) >= 0)
                    bReturn = TRUE;
            }
            else
            {
                if (bmplist->Add(&bmp, GetSysColor(COLOR_3DFACE)) >= 0)
                    bReturn = TRUE;
            }

            bmp.Detach();
            DeleteObject(hbmp);
        }
        else
        {
            CBitmap mybmp;
            if (mybmp.LoadBitmap(nResourceID))
            {
                m_bHighColorBitmap = TRUE;
                GetTransparentBitmap(mybmp);
                if (m_bBitmapBackgroundFlag)
                {
                    if (bmplist->Add(&mybmp, m_clrBackgroundBitmatMask) >= 0)
                        bReturn = TRUE;
                }
                else
                {
                    if (bmplist->Add(&mybmp, GetSysColor(COLOR_3DFACE)) >= 0)
                        bReturn = TRUE;
                }
            }
        }
    }

    if (bReturn && IsLunaMenuStyle() && m_bDraw3DBitmap)
    {
        CWnd* pWnd = AfxGetMainWnd();
        if (pWnd == NULL) pWnd = CWnd::GetDesktopWindow();
        CDC* pDC = pWnd->GetDC();
        CBitmap bmp, bmp2, bmp3;
        GetBitmapFromImageList(pDC, bmplist, 0, bmp);
        GetFadedBitmap(bmp);
        bmplist->Add(&bmp, GetSysColor(COLOR_3DFACE));
        GetBitmapFromImageList(pDC, bmplist, 0, bmp2);
        GetShadowBitmap(bmp2);
        bmplist->Add(&bmp2, GetSysColor(COLOR_3DFACE));
        GetBitmapFromImageList(pDC, bmplist, 0, bmp3);
        GetDisabledBitmap(bmp3);
        bmplist->Add(&bmp3, GetSysColor(COLOR_3DFACE));
        pWnd->ReleaseDC(pDC);
    }

    return bReturn;
}

void CSWMenu::SetBitmapBackground(COLORREF color)
{
    m_clrBackgroundBitmatMask = color;
    m_bBitmapBackgroundFlag = TRUE;
}

void CSWMenu::SetMenuBackColor(COLORREF clrBar, COLORREF clrBack, COLORREF clrBackSel, COLORREF clrEdge)
{
    m_clrBitmapBar = clrBar;
    m_clrBackground = clrBack;
    m_clrBackgroundSel = clrBackSel;
    m_clrEdge = clrEdge;
}

void CSWMenu::SetTextColor(COLORREF clr, COLORREF clrSel)
{
    m_clrText = clr;
    m_clrTextSel = clrSel;
}

void CSWMenu::SetSeparatorHeight(int height)
{
    m_iSeparatorHeight = height;
}

void CSWMenu::UnSetBitmapBackground()
{
    m_bBitmapBackgroundFlag = FALSE;
}

void CSWMenu::AddFromToolBar(CToolBar* pToolBar, int nResourceID)
{
    for (int i = 0; i < pToolBar->GetCount(); i++)
    {
        UINT nID = pToolBar->GetItemID(i);
        if (nID == 0 || GetMenuState(nID, MF_BYCOMMAND) == 0xFFFFFFFF)
            continue;
        UINT nStyle;
        int nImage;
        pToolBar->GetButtonInfo(i, nID, nStyle, nImage);
        int nLoc;
        CSWMenuData* pData;
        CSWMenu* pSubMenu = FindMenuOption(nID, nLoc);
        if (pSubMenu && nLoc >= 0)
            pData = pSubMenu->m_MenuList[nLoc];
        else
        {
            pData = new CSWMenuData;
            m_MenuList.Add(pData);
        }

        pData->menuIconNormal = nResourceID;
        pData->nID = nID;
        pData->nFlags = MF_BYCOMMAND | MF_OWNERDRAW;
        pData->xoffset = nImage;
        if (pData->bitmap)pData->bitmap->DeleteImageList();
        else pData->bitmap = new CImageList;
        pData->bitmap->Create(m_iconX, m_iconY, ILC_COLORDDB | ILC_MASK, 1, 1);

        if (!AddBitmapToImageList(pData->bitmap, nResourceID))
        {
            pData->bitmap->DeleteImageList();
            delete pData->bitmap;
            pData->bitmap = NULL;
            pData->menuIconNormal = -1;
            pData->xoffset = -1;
        }

        CMenu::ModifyMenu(nID, pData->nFlags, nID, (LPCTSTR)pData);
    }
}

BOOL CSWMenu::Draw3DCheckmark(CDC* dc, const CRect& rc,
    BOOL bSelected, HBITMAP hBitmapCheck)
{
    CRect rcDest = rc;
    CBrush brush;
    COLORREF col = GetSysColor(COLOR_3DFACE);
    if (!bSelected)
        col = LightenColor(col, 0.6);
    brush.CreateSolidBrush(col);
    //dc->FillRect(rcDest, &brush);
    brush.DeleteObject();
    //dc->DrawEdge(&rcDest, BDR_SUNKENOUTER, BF_RECT);
    dc->Draw3dRect(&rcDest, m_clrEdge, m_clrEdge);
    if (!hBitmapCheck)
        DrawCheckMark(dc, rc.left + 4, rc.top + 4, GetSysColor(COLOR_MENUTEXT));
    else
        DrawRadioDot(dc, rc.left + 5, rc.top + 4, GetSysColor(COLOR_MENUTEXT));
    return TRUE;
}

BOOL CSWMenu::DrawXPCheckmark(CDC* dc, const CRect& rc, HBITMAP hBitmapCheck, COLORREF& clrOut, BOOL bSelected)
{
    CBrush brIn;
    COLORREF col;
    int dx, dy;
    dy = (rc.Height() >> 1) - 3;
    dy = dy < 0 ? 0 : dy;
    dx = (rc.Width() >> 1) - 3;
    dx = dx < 0 ? 0 : dx;
    if (bSelected)
        col = LightenColor(m_clrTextSel, 0.55);
    else
        col = LightenColor(m_clrText, 0.85);
    brIn.CreateSolidBrush(col);
    //dc->FillRect(rc, &brIn);
    brIn.DeleteObject();
    dc->Draw3dRect(rc, col, col);
    if (!hBitmapCheck)
        DrawCheckMark(dc, rc.left + dx, rc.top + dy, m_clrText /*GetSysColor(COLOR_MENUTEXT)*/);
    else
        DrawRadioDot(dc, rc.left + dx, rc.top + dy, m_clrText /*GetSysColor(COLOR_MENUTEXT)*/);

    return TRUE;
}

void CSWMenu::DitherBlt(CDC* pDC, int nXDest, int nYDest, int nWidth,
    int nHeight, CBitmap& bmp, int nXSrc, int nYSrc,
    COLORREF bgcolor)
{
    CDC dcMem;
    dcMem.CreateCompatibleDC(0);
    CBitmap bwbmp;
    bwbmp.CreateCompatibleBitmap(&dcMem, nWidth, nHeight);
    CBitmap* pMemOldBmp = dcMem.SelectObject(&bwbmp);

    CDC dc;
    dc.CreateCompatibleDC(0);
    CBitmap* pOldBmp = dc.SelectObject(&bmp);

    dcMem.PatBlt(0, 0, nWidth, nHeight, WHITENESS);
    dc.SetBkColor(GetSysColor(COLOR_BTNFACE));
    dcMem.BitBlt(0, 0, nWidth, nHeight, &dc, nXSrc, nYSrc, SRCCOPY);
    dc.SetBkColor(GetSysColor(COLOR_BTNHILIGHT));
    dcMem.BitBlt(0, 0, nWidth, nHeight, &dc, nXSrc, nYSrc, SRCPAINT);
    dc.FillSolidRect(0, 0, nWidth, nHeight, bgcolor);
    dc.SetBkColor(RGB(0, 0, 0));
    dc.SetTextColor(RGB(255, 255, 255));
    CBrush brShadow, brHilight;
    brHilight.CreateSolidBrush(GetSysColor(COLOR_BTNHILIGHT));
    brShadow.CreateSolidBrush(GetSysColor(COLOR_BTNSHADOW));
    CBrush* pOldBrush = dc.SelectObject(&brHilight);
    dc.BitBlt(0, 0, nWidth, nHeight, &dcMem, 0, 0, 0x00E20746L);
    pDC->BitBlt(nXDest + 1, nYDest + 1, nWidth, nHeight, &dc, 0, 0, SRCCOPY);
    dc.BitBlt(1, 1, nWidth, nHeight, &dcMem, 0, 0, 0x00E20746L);
    dc.SelectObject(&brShadow);
    dc.BitBlt(0, 0, nWidth, nHeight, &dcMem, 0, 0, 0x00E20746L);
    pDC->BitBlt(nXDest, nYDest, nWidth, nHeight, &dc, 0, 0, SRCCOPY);
    dcMem.SelectObject(pMemOldBmp);
    dcMem.DeleteDC();
    dc.SelectObject(pOldBrush);
    dc.SelectObject(pOldBmp);
    dc.DeleteDC();
    brShadow.DeleteObject();
    brHilight.DeleteObject();
    bwbmp.DeleteObject();
}

void CSWMenu::DitherBlt3(CDC* pDC, int nXDest, int nYDest, int nWidth,
    int nHeight, CBitmap& bmp, COLORREF bgcolor)
{
    GetDisabledBitmap(bmp, bgcolor);
    CDC dc;
    dc.CreateCompatibleDC(NULL);
    CBitmap* pOldBmp = dc.SelectObject(&bmp);
    pDC->BitBlt(nXDest, nYDest, nWidth, nHeight, &dc, 0, 0, SRCCOPY);
    dc.SelectObject(pOldBmp);
    dc.DeleteDC();
}

void CSWMenu::SetDisableOldStyle()
{
    m_bDisableOldStyle = TRUE;
}

void CSWMenu::UnSetDisableOldStyle()
{
    m_bDisableOldStyle = FALSE;
}

BOOL CSWMenu::GetDisableOldStyle()
{
    return(m_bDisableOldStyle);
}

WORD CSWMenu::NumBitmapColors(LPBITMAPINFOHEADER lpBitmap)
{
    WORD returnval = 0;
    if (lpBitmap->biClrUsed != 0)
    {
        returnval = (WORD)lpBitmap->biClrUsed;
    }
    else
    {
        switch (lpBitmap->biBitCount)
        {
        case 1:
            returnval = 2;
            break;
        case 4:
            returnval = 16;
            break;
        case 8:
            returnval = 256;
            break;
        default:
            returnval = 0;
            break;
        }
    }

    return returnval;
}

HBITMAP CSWMenu::LoadSysColorBitmap(int nResourceId)
{
    HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(nResourceId), RT_BITMAP);
    HRSRC hRsrc = ::FindResource(hInst, MAKEINTRESOURCE(nResourceId), RT_BITMAP);
    if (hRsrc == NULL)
    {
        hInst = NULL;
        hRsrc = ::FindResource(hInst, MAKEINTRESOURCE(nResourceId), RT_BITMAP);
    }

    if (hRsrc == NULL)
        return NULL;

    HGLOBAL hglb;
    if ((hglb = LoadResource(hInst, hRsrc)) == NULL)
        return NULL;

    LPBITMAPINFOHEADER lpBitmap = (LPBITMAPINFOHEADER)LockResource(hglb);
    if (lpBitmap == NULL)
        return NULL;

    WORD numcol = NumBitmapColors(lpBitmap);
    ::FreeResource(hglb);
    if (numcol != 16)
        return NULL;

    return AfxLoadSysColorBitmap(hInst, hRsrc, FALSE);
}

BOOL CSWMenu::RemoveMenu(UINT uiId, UINT nFlags)
{
    if (MF_BYPOSITION & nFlags)
    {
        UINT uint = GetMenuState(uiId, MF_BYPOSITION);
        if (uint & MF_SEPARATOR && !(uint & MF_POPUP))
        {
            delete m_MenuList.GetAt(uiId);
            m_MenuList.RemoveAt(uiId);
        }
        else
        {
            CSWMenu* pSubMenu = (CSWMenu*)GetSubMenu(uiId);
            if (NULL == pSubMenu)
            {
                UINT uiCommandId = GetMenuItemID(uiId);
                for (int i = 0; i < m_MenuList.GetSize(); i++)
                {
                    if (m_MenuList[i]->nID == uiCommandId)
                    {
                        delete m_MenuList.GetAt(i);
                        m_MenuList.RemoveAt(i);
                        break;
                    }
                }
            }
            else
            {
                int numSubMenus = m_SubMenus.GetUpperBound();
                for (int m = numSubMenus; m >= 0; m--)
                {
                    if (m_SubMenus[m] == pSubMenu->m_hMenu)
                    {
                        int numAllSubMenus = m_AllSubMenus.GetUpperBound();
                        for (int n = numAllSubMenus; n >= 0; n--)
                        {
                            if (m_AllSubMenus[n] == m_SubMenus[m])m_AllSubMenus.RemoveAt(n);
                        }

                        m_SubMenus.RemoveAt(m);
                    }
                }

                int num = pSubMenu->GetMenuItemCount();
                for (int i = num - 1; i >= 0; --i)
                    pSubMenu->RemoveMenu(i, MF_BYPOSITION);
                for (int i = m_MenuList.GetUpperBound(); i >= 0; i--)
                {
                    if (m_MenuList[i]->nID == (UINT)pSubMenu->m_hMenu)
                    {
                        delete m_MenuList.GetAt(i);
                        m_MenuList.RemoveAt(i);
                        break;
                    }
                }

                delete pSubMenu;
            }
        }
    }
    else
    {
        int iPosition = 0;
        CSWMenu* pMenu = FindMenuOption(uiId, iPosition);
        if (pMenu)
            return pMenu->RemoveMenu(iPosition, MF_BYPOSITION);
    }

    return CMenu::RemoveMenu(uiId, nFlags);
}

BOOL CSWMenu::DeleteMenu(UINT uiId, UINT nFlags)
{
    if (MF_BYPOSITION & nFlags)
    {
        UINT uint = GetMenuState(uiId, MF_BYPOSITION);
        if (uint & MF_SEPARATOR && !(uint & MF_POPUP))
        {
            int menulistsize = m_MenuList.GetSize();
            if (uiId < (UINT)menulistsize)
            {
                CString str = m_MenuList[uiId]->GetString();
                if (str == "")
                {
                    delete m_MenuList.GetAt(uiId);
                    m_MenuList.RemoveAt(uiId);
                }
            }
        }
        else
        {
            CSWMenu* pSubMenu = (CSWMenu*)GetSubMenu(uiId);
            if (NULL == pSubMenu)
            {
                UINT uiCommandId = GetMenuItemID(uiId);
                for (int i = 0; i < m_MenuList.GetSize(); i++)
                {
                    if (m_MenuList[i]->nID == uiCommandId)
                    {
                        delete m_MenuList.GetAt(i);
                        m_MenuList.RemoveAt(i);
                        break;
                    }
                }
            }
            else
            {
                int numSubMenus = m_SubMenus.GetUpperBound();
                for (int m = numSubMenus; m >= 0; m--)
                {
                    if (m_SubMenus[m] == pSubMenu->m_hMenu)
                    {
                        int numAllSubMenus = m_AllSubMenus.GetUpperBound();
                        for (int n = numAllSubMenus; n >= 0; n--)
                        {
                            if (m_AllSubMenus[n] == m_SubMenus[m])m_AllSubMenus.RemoveAt(n);
                        }

                        m_SubMenus.RemoveAt(m);
                    }
                }

                int num = pSubMenu->GetMenuItemCount();
                for (int i = num - 1; i >= 0; --i)
                    pSubMenu->DeleteMenu(i, MF_BYPOSITION);

                for (int i = m_MenuList.GetUpperBound(); i >= 0; i--)
                {
                    if (m_MenuList[i]->nID == (UINT)pSubMenu->m_hMenu)
                    {
                        delete m_MenuList.GetAt(i);
                        m_MenuList.RemoveAt(i);
                        break;
                    }
                }

                delete pSubMenu;
            }
        }
    }
    else
    {
        int iPosition = 0;
        CSWMenu* pMenu = FindMenuOption(uiId, iPosition);
        if (pMenu)
            return pMenu->DeleteMenu(iPosition, MF_BYPOSITION);
    }

    return CMenu::DeleteMenu(uiId, nFlags);
}

BOOL CSWMenu::AppendMenu(UINT nFlags, UINT nIDNewItem, TCHAR* lpszNewItem, int nIconNormal)
{
    return AppendODMenu(lpszNewItem, nFlags, nIDNewItem, nIconNormal);
}

BOOL CSWMenu::AppendMenu(UINT nFlags, UINT nIDNewItem, TCHAR* lpszNewItem, CImageList* il, int xoffset)
{
    return AppendODMenu(lpszNewItem, nFlags, nIDNewItem, il, xoffset);
}

BOOL CSWMenu::AppendMenu(UINT nFlags, UINT nIDNewItem, TCHAR* lpszNewItem, CBitmap* bmp)
{
    if (bmp)
    {
        CImageList temp;
        temp.Create(m_iconX, m_iconY, ILC_COLORDDB | ILC_MASK, 1, 1);
        if (m_bBitmapBackgroundFlag)
            temp.Add(bmp, m_clrBackgroundBitmatMask);
        else
            temp.Add(bmp, GetSysColor(COLOR_3DFACE));

        return AppendODMenu(lpszNewItem, nFlags, nIDNewItem, &temp, 0);
    }
    return AppendODMenu(lpszNewItem, nFlags, nIDNewItem, NULL, 0);
}

BOOL CSWMenu::InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem, TCHAR* lpszNewItem, int nIconNormal)
{
    return InsertODMenu(nPosition, lpszNewItem, nFlags, nIDNewItem, nIconNormal);
}

BOOL CSWMenu::InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem, TCHAR* lpszNewItem, CImageList* il, int xoffset)
{
    return InsertODMenu(nPosition, lpszNewItem, nFlags, nIDNewItem, il, xoffset);
}

BOOL CSWMenu::InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem, TCHAR* lpszNewItem, CBitmap* bmp)
{
    if (bmp)
    {
        CImageList temp;
        temp.Create(m_iconX, m_iconY, ILC_COLORDDB | ILC_MASK, 1, 1);
        if (m_bBitmapBackgroundFlag)
            temp.Add(bmp, m_clrBackgroundBitmatMask);
        else
            temp.Add(bmp, GetSysColor(COLOR_3DFACE));
        return InsertODMenu(nPosition, lpszNewItem, nFlags, nIDNewItem, &temp, 0);
    }

    return InsertODMenu(nPosition, lpszNewItem, nFlags, nIDNewItem, NULL, 0);
}

CSWMenu* CSWMenu::AppendODPopupMenu(TCHAR* lpstrText)
{
    CSWMenu* pSubMenu = new CSWMenu;
    pSubMenu->m_bUnSelectCheck = m_bUnSelectCheck;
    pSubMenu->m_nSelectCheck = m_nSelectCheck;
    pSubMenu->m_imagelistCheckBitmap = m_imagelistCheckBitmap;
    pSubMenu->m_bCheckBitmapShare = TRUE;
    pSubMenu->CreatePopupMenu();
    AppendODMenu(lpstrText, MF_POPUP, (UINT)pSubMenu->m_hMenu, -1);
    return pSubMenu;
}

BOOL CSWMenu::ImageListDuplicate(CImageList* il, int xoffset, CImageList* newlist)
{
    if (il == NULL || newlist == NULL || xoffset < 0)
        return FALSE;

    HICON hIcon = il->ExtractIcon(xoffset);
    int cx, cy;
    ImageList_GetIconSize(il->m_hImageList, &cx, &cy);
    newlist->Create(cx, cy, ILC_COLORDDB | ILC_MASK, 1, 1);
    newlist->Add(hIcon);
    ::DestroyIcon(hIcon);
    if (IsLunaMenuStyle() && m_bDraw3DBitmap)
    {
        CWnd* pWnd = AfxGetMainWnd();
        if (pWnd == NULL)
            pWnd = CWnd::GetDesktopWindow();
        CDC* pDC = pWnd->GetDC();
        CBitmap bmp, bmp2, bmp3;
        GetBitmapFromImageList(pDC, newlist, 0, bmp);
        GetFadedBitmap(bmp);
        newlist->Add(&bmp, GetSysColor(COLOR_3DFACE));
        GetBitmapFromImageList(pDC, newlist, 0, bmp2);
        GetShadowBitmap(bmp2);
        newlist->Add(&bmp2, GetSysColor(COLOR_3DFACE));
        GetBitmapFromImageList(pDC, newlist, 0, bmp3);
        GetDisabledBitmap(bmp3);
        newlist->Add(&bmp3, GetSysColor(COLOR_3DFACE));
        pWnd->ReleaseDC(pDC);
    }

    return TRUE;
}

CMenu* CSWMenu::GetSubMenu(int nPos)
{
    return CMenu::GetSubMenu(nPos);
}

CMenu* CSWMenu::GetSubMenu(LPCTSTR lpszSubMenuName)
{
    int num = GetMenuItemCount();
    CString name;

    for (int i = 0; i < num; i++)
    {
        GetMenuString(i, name, MF_BYPOSITION);
        if (name.Compare(lpszSubMenuName) == 0)
        {
            return CMenu::GetSubMenu(i);
        }
    }

    return NULL;
}

CSWMenu* CSWMenu::GetSubCSWMenu(TCHAR* lpszSubMenuName)
{
    CSWMenuData* mdata;
    mdata = FindMenuOption(lpszSubMenuName);
    if (mdata)
    {
        HMENU bchmenu = (HMENU)mdata->nID;
        CMenu* ptr = FromHandle(bchmenu);
        if (ptr)
        {
            BOOL flag = ptr->IsKindOf(RUNTIME_CLASS(CSWMenu));
            if (flag)
                return (CSWMenu*)ptr;
        }
    }

    return NULL;
}

int CSWMenu::GetMenuPosition(TCHAR* pText)
{
    int i, j;
    CSWMenu* psubmenu;
    for (i = 0; i < (int)(GetMenuItemCount()); ++i)
    {
        psubmenu = (CSWMenu*)GetSubMenu(i);
        if (!psubmenu)
        {
            const TCHAR* szText;
            for (j = 0; j <= m_MenuList.GetUpperBound(); ++j)
            {
                szText = m_MenuList[j]->GetString();
                if (szText && !_tcscmp(pText, szText))
                    return j;
            }
        }
    }

    return -1;
}

int CSWMenu::RemoveMenu(TCHAR* pText, BC_Seperator sPos)
{
    int nPos = GetMenuPosition(pText);
    if (nPos != -1)
    {
        RemoveMenu(nPos, MF_BYPOSITION);
    }
    if (sPos == CSWMenu_HEAD)
    {
        ASSERT(nPos - 1 >= 0);
        RemoveMenu(nPos - 1, MF_BYPOSITION);
    }
    else if (sPos == CSWMenu_TAIL)
    {
        RemoveMenu(nPos - 1, MF_BYPOSITION);
    }
    else if (sPos == CSWMenu_BOTH)
    {
        RemoveMenu(nPos - 1, MF_BYPOSITION);
        ASSERT(nPos - 1 >= 0);
        RemoveMenu(nPos - 1, MF_BYPOSITION);
    }

    return nPos;
}

int CSWMenu::DeleteMenu(TCHAR* pText, BC_Seperator sPos)
{
    int nPos = GetMenuPosition(pText);
    if (nPos != -1)
    {
        DeleteMenu(nPos, MF_BYPOSITION);
    }
    if (sPos == CSWMenu_HEAD)
    {
        ASSERT(nPos - 1 >= 0);
        DeleteMenu(nPos - 1, MF_BYPOSITION);
    }
    else if (sPos == CSWMenu_TAIL)
    {
        DeleteMenu(nPos - 1, MF_BYPOSITION);
    }
    else if (sPos == CSWMenu_BOTH)
    {
        DeleteMenu(nPos - 1, MF_BYPOSITION);
        ASSERT(nPos - 1 >= 0);
        DeleteMenu(nPos - 1, MF_BYPOSITION);
    }

    return nPos;
}

BOOL CSWMenu::SetMenuText(UINT id, CString string, UINT nFlags/*= MF_BYPOSITION*/)
{
    BOOL returnflag = FALSE;

    if (MF_BYPOSITION & nFlags)
    {
        UINT numMenuItems = m_MenuList.GetUpperBound();
        if (id <= numMenuItems)
        {
            m_MenuList[id]->SetString(string);
            returnflag = TRUE;
        }
    }
    else
    {
        int uiLoc;
        CSWMenu* pMenu = FindMenuOption(id, uiLoc);
        if (NULL != pMenu) returnflag = pMenu->SetMenuText(uiLoc, string);
    }

    return returnflag;
}

void CSWMenu::ColorBitmap(CDC* pDC, CBitmap& bmp, CSize bitmap_size, CSize icon_size, COLORREF fill, COLORREF border, int hatchstyle)
{
    CDC bmpdc;
    COLORREF m_newclrBack;
    int x1 = 0, y1 = 0, x2 = bitmap_size.cx, y2 = bitmap_size.cy;
    m_newclrBack = GetSysColor(COLOR_3DFACE);
    bmpdc.CreateCompatibleDC(pDC);
    bmp.CreateCompatibleBitmap(pDC, icon_size.cx, icon_size.cy);
    CBitmap* pOldBitmap = bmpdc.SelectObject(&bmp);
    if (bitmap_size != icon_size)
    {
        CBrush background_brush;
        background_brush.CreateSolidBrush(m_newclrBack);
        CRect rect(0, 0, icon_size.cx, icon_size.cy);
        bmpdc.FillRect(rect, &background_brush);
        x1 = (icon_size.cx - bitmap_size.cx) / 2;
        y1 = (icon_size.cy - bitmap_size.cy) / 2;
        x2 = x1 + bitmap_size.cx;
        y2 = y1 + bitmap_size.cy;
        background_brush.DeleteObject();
    }

    CPen border_pen(PS_SOLID, 1, border);
    CBrush fill_brush;
    if (hatchstyle != -1)
        fill_brush.CreateHatchBrush(hatchstyle, fill);
    else
        fill_brush.CreateSolidBrush(fill);

    CPen* pOldPen = bmpdc.SelectObject(&border_pen);
    CBrush* pOldBrush = bmpdc.SelectObject(&fill_brush);
    bmpdc.Rectangle(x1, y1, x2, y2);
    if (NULL != pOldBrush)
        bmpdc.SelectObject(pOldBrush);
    if (NULL != pOldPen)
        bmpdc.SelectObject(pOldPen);
    if (NULL != pOldBitmap)
        bmpdc.SelectObject(pOldBitmap);
}

BOOL CSWMenu::IsWindowsClassicTheme()
{
    TCHAR Buf[_MAX_PATH + 10];
    HKEY hKey;
    DWORD size, type;
    long lRetCode;
    static BOOL XPTheme_returnflag = FALSE;
    static BOOL XPTheme_checkflag = FALSE;

    if (XPTheme_checkflag)
        return XPTheme_returnflag;
    XPTheme_checkflag = TRUE;
    lRetCode = RegOpenKeyEx(HKEY_CURRENT_USER,
        _T("Software//Microsoft//Plus!//Themes//Current"),
        0, KEY_READ, &hKey);
    if (lRetCode == ERROR_SUCCESS)
    {
        size = _MAX_PATH; type = REG_SZ;
        lRetCode = ::RegQueryValueEx(hKey, NULL, NULL, &type,
            (unsigned char*)Buf, &size);
        if (lRetCode == ERROR_SUCCESS)
        {
            TCHAR szClassicTheme[] = _T("Windows Classic.theme");
            int len = lstrlen(Buf);
            if (len >= lstrlen(szClassicTheme))
            {
                if (!lstrcmpi(&Buf[len - lstrlen(szClassicTheme)], szClassicTheme))
                {
                    XPTheme_returnflag = TRUE;
                }
            }
        }
        RegCloseKey(hKey);
    }

    return XPTheme_returnflag;
}

int CSWMenu::GlobalImageListOffset(int nID)
{
    int numcurrent = m_AllImagesID.GetSize();
    int existsloc = -1;
    for (int i = 0; i < numcurrent; ++i) {
        if (m_AllImagesID[i] == nID) {
            existsloc = i;
            break;
        }
    }
    return existsloc;
}
BOOL CSWMenu::CanDraw3DImageList(int offset)
{
    BOOL retflag = FALSE;
    int numcurrent = m_AllImagesID.GetSize();
    if (offset + 1 < numcurrent && offset + 2 < numcurrent) {
        int nID = m_AllImagesID[offset];
        if (m_AllImagesID[offset + 1] == nID && m_AllImagesID[offset + 2] == nID)retflag = TRUE;
    }
    return(retflag);
}
int CSWMenu::AddToGlobalImageList(CImageList* il, int xoffset, int nID)
{
    int loc = -1;
    HIMAGELIST hImageList = m_AllImages.m_hImageList;
    if (!hImageList)
    {
        m_AllImages.Create(m_iconX, m_iconY, ILC_COLORDDB | ILC_MASK, 1, 1);
    }

    HICON hIcon = il->ExtractIcon(xoffset);
    if (hIcon)
    {
        CBitmap bmp, bmp2, bmp3;
        if (IsLunaMenuStyle() && m_bDraw3DBitmap)
        {
            CWnd* pWnd = AfxGetMainWnd();
            if (pWnd == NULL) pWnd = CWnd::GetDesktopWindow();
            CDC* pDC = pWnd->GetDC();
            GetBitmapFromImageList(pDC, il, xoffset, bmp);
            GetFadedBitmap(bmp);
            GetBitmapFromImageList(pDC, il, xoffset, bmp2);
            GetShadowBitmap(bmp2);
            GetBitmapFromImageList(pDC, il, xoffset, bmp3);
            GetDisabledBitmap(bmp3);
            pWnd->ReleaseDC(pDC);
        }

        int numcurrent = m_AllImagesID.GetSize();
        int existsloc = -1;
        for (int i = 0; i < numcurrent; ++i)
        {
            if (m_AllImagesID[i] == nID)
            {
                existsloc = i;
                break;
            }
        }

        if (existsloc >= 0)
        {
            m_AllImages.Replace(existsloc, hIcon);
            loc = existsloc;
            if (IsLunaMenuStyle() && m_bDraw3DBitmap)
            {
                if (existsloc + 1 < numcurrent && m_AllImagesID[existsloc + 1] == nID)
                {
                    if (existsloc + 2 < numcurrent && m_AllImagesID[existsloc + 2] == nID)
                    {
                        CImageList il2;
                        il2.Create(m_iconX, m_iconY, ILC_COLORDDB | ILC_MASK, 1, 1);
                        il2.Add(&bmp, GetSysColor(COLOR_3DFACE));
                        HICON hIcon2 = il2.ExtractIcon(0);
                        m_AllImages.Replace(existsloc + 1, hIcon2);
                        il2.Add(&bmp2, GetSysColor(COLOR_3DFACE));
                        HICON hIcon3 = il2.ExtractIcon(1);
                        m_AllImages.Replace(existsloc + 2, hIcon3);
                        il2.Add(&bmp3, GetSysColor(COLOR_3DFACE));
                        HICON hIcon4 = il2.ExtractIcon(2);
                        m_AllImages.Replace(existsloc + 3, hIcon4);
                        ::DestroyIcon(hIcon2);
                        ::DestroyIcon(hIcon3);
                        ::DestroyIcon(hIcon4);
                    }
                }
            }
        }
        else
        {
            m_AllImages.Add(hIcon);
            m_AllImagesID.Add(nID);
            loc = numcurrent;
            if (IsLunaMenuStyle() && m_bDraw3DBitmap)
            {
                m_AllImages.Add(&bmp, GetSysColor(COLOR_3DFACE));
                m_AllImages.Add(&bmp2, GetSysColor(COLOR_3DFACE));
                m_AllImages.Add(&bmp3, GetSysColor(COLOR_3DFACE));
                m_AllImagesID.Add(nID);
                m_AllImagesID.Add(nID);
                m_AllImagesID.Add(nID);
            }
        }

        ::DestroyIcon(hIcon);
    }

    return loc;
}

BOOL CSWMenu::AppendMenu(CSWMenu* pMenuToAdd, BOOL add_separator /*= TRUE*/, int num_items_to_remove_at_end /*= 0*/)
{
    if (pMenuToAdd == NULL || !pMenuToAdd->IsKindOf(RUNTIME_CLASS(CSWMenu)))
    {
        ASSERT(FALSE);
        return FALSE;
    }

    int iMenuAddItemCount = pMenuToAdd->GetMenuItemCount();
    if (iMenuAddItemCount == 0)
        return TRUE;

    if (num_items_to_remove_at_end > 0)
    {
        int original_num_menu_items = GetMenuItemCount();
        if (original_num_menu_items >= num_items_to_remove_at_end)
        {
            int first_delete_item = original_num_menu_items - 1;
            int last_delete_item = original_num_menu_items - num_items_to_remove_at_end;
            for (int i = first_delete_item; i >= last_delete_item; i--)
            {
                if (!DeleteMenu(i, MF_BYPOSITION))
                {
                    ASSERT(FALSE);
                    break;
                }
            }
        }
        else
        {
            ASSERT(FALSE);
        }
    }

    if (add_separator && GetMenuItemCount() > 0)
    {
        if (MF_SEPARATOR != (MF_SEPARATOR & GetMenuState(GetMenuItemCount() - 1, MF_BYPOSITION)))
        {
            AppendMenu(MF_SEPARATOR);
        }
    }

    for (int iLoop = 0; iLoop < iMenuAddItemCount; iLoop++)
    {
        CString sMenuAddString;
        pMenuToAdd->GetMenuText(iLoop, sMenuAddString, MF_BYPOSITION);
        CSWMenu* pSubMenu = (CSWMenu*)pMenuToAdd->GetSubMenu(iLoop);
        if (!pSubMenu)
        {
            UINT nState = pMenuToAdd->GetMenuState(iLoop, MF_BYPOSITION);
            UINT nItemID = pMenuToAdd->GetMenuItemID(iLoop);
            if (!AppendMenu(nState, nItemID, sMenuAddString.GetBuffer(0)))
            {
                ASSERT(FALSE);
                return FALSE;
            }
        }
        else
        {
            CSWMenu* pNewPopupMenu = AppendODPopupMenu(sMenuAddString.GetBuffer(0));
            if (pNewPopupMenu == NULL)
            {
                ASSERT(FALSE);
                return FALSE;
            }

            if (!pNewPopupMenu->AppendMenu(pSubMenu, FALSE, 0))
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}