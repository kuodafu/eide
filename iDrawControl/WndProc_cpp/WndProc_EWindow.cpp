#include "../EMenu.h"
#include "../EWindow.h"
#include <assist/assist_Resource.h>
#include <control/Ctips.h>

#include <control/CMenu.h>
#include <iDraw_Version.h>

#include <iDraw_const.h>
#include <assist/assist.h>
#include <control/draw_Window.h>
#include <CDarkMode.h>
#include <CThemeColor.h>
#include <shlobj_core.h>
#include <shlwapi.h>
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Shlwapi.lib")


#define _DEBUG_ISVERSION        1           // 是否显示版本号, 正式版得考虑去掉


static int m_startID;       // 标题菜单, 用户起始ID

wstr m_eCodeFileName;       // 从标题中获取到的源码路径
wstr m_eCodeType;           // 从标题中获取到的源码类型
wstr m_eCodeShowName;       // 显示标题, 通常是 源码文件名 - [程序类型], 未保存的就 * 程序类型
wstr m_eCodeTipsTitle;      // 提示里的标题, 一般是 易语言-加密狗版, 或者是设置的程序名

SYSBTN sysBtn;                  // 系统按钮信息
HINSTANCE s_hInst;
RECT m_rcTitle;                 // 显示标题的地方, 一小块黑色, 鼠标上去应该弹出提示

CLayered m_tips;
int m_isHover;                  // 鼠标是否进入了项目标题中
RECT m_tips_rect;               // 提示信息当前的矩形位置, 在调用时钟前设置




#if _DEBUG_ISVERSION
static RECT m_rcVersion;        // 显示版本号和助手兼容情况的地方, 鼠标移动上去需要显示手型指针, 点击需要跳转到助手兼容帖子链接
static LPEX_BRUSH m_hbrVersion; // 版本号文本画刷
#endif


static LONG(WINAPI* SetWindowLongA_hook)(HWND hWnd, int nIndex, LONG dwNewLong) = SetWindowLongA;
static LONG(WINAPI* SetWindowLongW_hook)(HWND hWnd, int nIndex, LONG dwNewLong) = SetWindowLongW;




LRESULT __OnSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_EWindow(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT EWndProc_OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT WndProc_Enteridle(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

LRESULT __OnWindowPosChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT __OnWindowPosChanging(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT WndProc_OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT WndProc_OnSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT WndProc_OnNcMouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT WndProc_OnNcCalcsize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT WndProc_OnNcHittest(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT WndProc_OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT WndProc_OnMouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT WndProc_OnKey(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT WndProc_OnNcCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT WndProc_OnWindowPosChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT WndProc_OnWindowPosChanging(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT WndProc_OnStyleChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT WndProc_OnStyleChanging(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT WndProc_OnGetMinMaxInfo(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT WndProc_OnMeasureItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

void EWindow_StatusBar_drawItem(LPCANVAS hCanvas, int i, LPOBJSTRUCT pData, RECT* rcItems, LPEX_FONT hFont);
LPCWSTR _e_get_title(LPCSTR szTitle);
void CALLBACK TimeProc_SaveClr(HWND hWnd, UINT message, UINT_PTR id, DWORD timer);


static CMenu m_menuZhushou;






// 调用易语言指定菜单功能, 传递菜单ID
inline LRESULT _call_menu(int id)
{
    static LPOBJSTRUCT pData = 0;
    if (!pData)
    {
        pData = (LPOBJSTRUCT)id;
        return 0;
    }
    return WndProc_CallWindowProc(pData->hWnd, WM_COMMAND, id, 0, pData);

}
inline void WINAPI _ewindow_crChange(LPOBJSTRUCT pData)
{
    pData->draw.color.crBackGround = s_info->themeColor->crBackExtern();
    pData->draw.color.hbrBackGround = s_info->pfnCreateBrush(pData->draw.color.crBackGround);

    DWORD crBorder = 0;
    crBorder = s_info->isActivation
        ? (s_info->isStop ? s_info->themeColor->crBorder_run() : s_info->themeColor->crBorder())
        : s_info->themeColor->crBorder_nf();

    crBorder = RGB2ARGB(crBorder, 255);
    DWORD crShadow = s_info->isActivation
        ? (s_info->isStop ? s_info->themeColor->crShadow_run() : s_info->themeColor->crShadow())
        : s_info->themeColor->crShadow_nf();

    crShadow = RGB2ARGB(crShadow, 255);
    //wstr::dbg(L"颜色改变, 边框颜色 = %s, 阴影颜色 = %s\n", argb2str(crBorder).c_str(), rgb2str(crShadow).c_str());

    s_info->shadow->SetColor(crShadow, crBorder, true);

}

LPOBJSTRUCT _subclass_EWindow(HWND hWnd, HINSTANCE hInst, LPSYSBUTTONIMAGE pNskn)
{
    if (!s_info->pTips)
    {
        s_info->pTips = &m_tips;
        s_info->pTips->create(TTS_ALWAYSTIP, WS_EX_LAYERED | WS_EX_TRANSPARENT);
    }
    s_info->isActivation = true;
    s_info->pszTitle = &m_eCodeFileName;
    s_info->shadow = EWindow_GetShadow();
    s_hInst = hInst;
    LPOBJSTRUCT data = _subclass(hWnd, WndProc_EWindow, false, false, 0);
    s_info->pWnd = data;
    bool isTop = s_info->config->tab_code_direction() != 3;    // 代码选择夹的子夹方向, 3=底边, 其他 = 顶边

                                                               //CTab
    int offsetPadding = 20;
    if (s_info->config->tab_code_isIcon())    // 有图标的话就加大20个像素绘画图标
        offsetPadding += 20;
    TabCtrl_SetPadding(s_info->hEWnd, offsetPadding, 0);
    _subclass_MDIClient_CustomTab(s_info->hEWnd);      // 代码区切换窗口选择夹, 自己创建的选择夹

    s_info->shadow->OnPaint(OnShadowPaint);
    if (data->pos.rc.right - data->pos.rc.left <= 0 || data->pos.rc.bottom - data->pos.rc.top <= 0)
        GetWindowRect(data->hWnd, &data->pos.rc);
    s_info->hShadow = s_info->shadow->create(s_hInst, data->pos.rc, data->hWnd, 0);

    auto pfnTimer = [](HWND hWnd, UINT message, UINT_PTR id, DWORD timer) -> void
    {
        if (s_info->isReady)
            KillTimer(hWnd, id);
        RECT rc;
        GetWindowRect(s_info->hEWnd, &rc);
        s_info->shadow->Update(rc, true);
        //wstr::debug(L"时钟重画背景\n");
    };
    SetTimer(s_info->hShadow, (UINT_PTR)&pfnTimer, 100, pfnTimer);

    _call_menu((int)data);
    s_info->pfnCallMenu = _call_menu;
    EWindow_TabInit(data);
    data->draw.hCanvas = _canvas_create(hWnd, 0, 0);

    {
        DWORD style = data->GetClsLong(data->hWnd, GCL_STYLE);
        //style &= ~(CS_VREDRAW | CS_HREDRAW);
        style |= (CS_VREDRAW | CS_HREDRAW);
        //style |= (CS_PARENTDC | CS_SAVEBITS | CS_CLASSDC);
        data->SetClsLong(data->hWnd, GCL_STYLE, style);
    }
    {
        DWORD style = data->GetWndLong(data->hWnd, GWL_STYLE);
        style &= ~WS_BORDER;
        //style &= ~WS_CLIPCHILDREN;
        style |= WS_CLIPSIBLINGS;
        if (s_info->config->noClipChildren())
            style &= ~WS_CLIPCHILDREN;
        else
            style |= WS_CLIPCHILDREN;

        data->SetWndLong(data->hWnd, GWL_STYLE, style);
    }
    data->draw.isErasebkg = true;
    _ewindow_crChange(data);
    data->fnCrChange = _ewindow_crChange;

    data->draw.indexHot = -1;
    data->draw.indexDown = -1;
    data->draw.indexSel = -1;

    ewnd_createed(hWnd, data->style, data);
    LPBYTE pImage = ((LPBYTE)pNskn) + pNskn->offset;
    sysBtn.pImage = _img_create_frommemory(pImage, pNskn->imgSize);
    sysBtn.image = pNskn;
    sysBtn.downIndex = -1;
    sysBtn.rdownIndex = -1;
    sysBtn.index = -1;

    if (!sysBtn.pImage_light)
    {
        sysBtn.pImage_light = s_info->d2d->_img_copy(sysBtn.pImage);

        // 修改位图颜色
        // crNew = 新的颜色
        // lpBits = 位图数据
        // nCount = 像素数, 宽 * 高
        auto PixelFix = [](COLORREF crNew, LPBYTE lpBits, int nCount)
        {
            BYTE sR = ARGB_GETR(crNew);
            BYTE sG = ARGB_GETG(crNew);
            BYTE sB = ARGB_GETB(crNew);

            DWORD* cr = (DWORD*)lpBits;     // 转成4字节一个像素点来处理
            for (int i = 0; i < nCount; ++i)
            {
                if (cr[i] == 0) continue;   // 等于0 表示是透明, 透明的不处理
                LPBYTE p = (LPBYTE)(&cr[i]);
                p[2] = sR * p[3] / 255;
                p[1] = sG * p[3] / 255;
                p[0] = sB * p[3] / 255;
            }
        };

        EX_IMAGELOCK lockData;
        bool isOk = s_info->d2d->_img_lock(sysBtn.pImage_light, 0, 0, 0, 0, 3, &lockData);
        assert(isOk);
        DWORD color = MAKEARGB(255, 31, 31, 31);
        PixelFix(color, (LPBYTE)lockData.pScan0, lockData.width * lockData.height);
        s_info->d2d->_img_unlock(sysBtn.pImage_light, &lockData);
    }

    


    data->param = &sysBtn;

    data->draw.textFormat = DT_VCENTER | DT_SINGLELINE;
    _init_menu_(data);
    _apihook_menu_init_sleep();


    //sysBtn.rcCaption = { WINDOW_CAPTION_HEIGHT, 0, 100, WINDOW_CAPTION_HEIGHT };
    //sysBtn.rcIcon = { 0, 0, WINDOW_CAPTION_HEIGHT, WINDOW_CAPTION_HEIGHT };

    RECT rc;
    GetWindowRect(hWnd, &rc);
    HRGN hRgn = CreateRectRgn(0, 0, rc.right - rc.left, rc.bottom - rc.top);
    SetWindowRgn(hWnd, hRgn, false);
    DeleteObject(hRgn);

    if (0)
    {
        CREATESTRUCTW cs = { 0 };
        cs.x = 100, cs.y = 100, cs.cx = 800, cs.cy = 600;
        cs.lpszName = L"测试窗口.......";
        cs.style = WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN;
        s_info->pfnControls(IDC_IDRAW_CREATEWINDOW, (WPARAM)&cs, 0);

    }
    //SetWindowPos(hWnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);

    SetTimer(hWnd, (UINT_PTR)TimeProc_SaveClr, 10000, TimeProc_SaveClr);
    return data;
}

// 时钟检测配色是否被修改, 如果内存里的配色和文件的配色不同, 那就写出文件
void CALLBACK TimeProc_SaveClr(HWND hWnd, UINT message, UINT_PTR id, DWORD timer)
{
    int size = (int)s_info->pfnControls(IDC_EIDE_GETECOLOR, 0, 0);
    if ( !size )
        return;
    LPCWSTR clr = s_info->themeColor->clr();    // 重新设置易语言配色
    if (!clr || !clr[0]) return;

    // 有clr配置项, 需要把数据写入这个文件
    wstr file(260); // clr文件完整路径
    file.assign(s_info->path_e).append(L"clr\\").append(clr);
    if (!PathFileExistsW(file.c_str())) return;

    int len = (int)s_info->pfnControls(IDC_EIDE_GETECOLOR, 0, 0);
    _str buf(len);
    s_info->pfnControls(IDC_EIDE_GETECOLOR, len, (WPARAM)buf.data());

    // 读文件里的数据
    _str clrFileData;
    int clrFileSize = 0;
    {
        CFileRW clrFile;
        clrFile.open(file.c_str(), 1, -1);
        clrFileSize = clrFile.size();
        clrFileData.resize(clrFileSize);
        clrFile.read((LPBYTE)clrFileData.data(), clrFileSize);
        clrFile.reset();
    }

    if (clrFileSize!= len || memcmp(buf.c_str(), clrFileData.c_str(), len) != 0)
    {
        CFileRW clrFile;
        if (!clrFile.open(file.c_str(), 4, -1))
            return;

        clrFile.write((LPBYTE)buf.c_str(), len);
        wstr::dbg(L"配色被改变, 重新写入配色\n");
    }


}

inline void _window_activate_changed(bool isActivate, LPARAM lParam, LPOBJSTRUCT pData)
{
    s_info->isActivation = isActivate;

    _ewindow_crChange(pData);
    InvalidateRect(s_info->hEWnd, &m_rcTitle, 0);
    if (s_info->rcStatusBar.bottom - s_info->rcStatusBar.top > 0)   // 需要重画状态条
    {
        RECT rcStatus;
        rcStatus.left = 0;
        rcStatus.top = s_info->rcStatusBar.top;
        rcStatus.right = pData->pos.rcClient.right;
        rcStatus.bottom = pData->pos.rcClient.bottom;
        InvalidateRect(s_info->hEWnd, &rcStatus, 0);
    }
}

inline void OnNcDestroy()
{
    LPPLUGIN_INFO pInfo[EVENT_MAX_ITEM] = { 0 };
    int count = s_info->pfnPlugin_Enum(pInfo, EVENT_MAX_ITEM);
    for (int i = 0; i < count; i++)
    {
        if (i >= EVENT_MAX_ITEM) break;
        LPPLUGIN_INFO info = pInfo[i];
        if (!info)continue;
        _str::dbg("%d/%d 卸载插件开始, guid = %s\t", i + 1, count, info->guid);
        if (info->pfnUnInterface)
            info->pfnUnInterface();
        _str::dbg("卸载插件结束, guid = %s\n", info->guid);

        //if (info->hModule)
        //    FreeLibrary(info->hModule);
        info->pfnMenuExt_Command = 0;
        info->pfnCallback = 0;
        info->hModule = 0;
        info->pfnCallback = 0;
        info->pfnInterface = 0;
        info->pfnUnInterface = 0;
    }
}
inline DWORD CALLBACK Thread_EWindowDestroy(LPVOID pArg)
{
    static BOOLEAN isNcDestroy;
    if(!pArg)
        isNcDestroy = TRUE; // WM_NCDESTROY 被执行了
    
    if (pArg)   // 是线程走到这里, 延时一下
    {
        Sleep(50000);
        if (!isNcDestroy)
        {
            // 暂时不执行, 反正都要结束进程了, 就差插件没有通知卸载事件而已了
            //! 如果 WM_NCDESTROY 没被执行, 手动执行一下
            //!OnNcDestroy();
        }
        // 延时后还能执行, 那就是进程没有被销毁, 强制结束进程
        TerminateProcess(GetCurrentProcess(), 0);
    }
    return 0;
}
LRESULT CALLBACK WndProc_EWindow(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    ////if (pData->index == 10)
    //wstr::dbg(L"窗口消息, msg = %d, wParam = %d, H=%d,L=%d, lParam = %d, H=%d,L=%d\n",
    //    message, wParam, HIWORD(wParam), LOWORD(wParam), lParam, HIWORD(lParam), LOWORD(lParam));
    LPTAB_INFO tabInfo = &s_info->tab;
    switch (message)
    {
    case WM_ENTERIDLE:
        return WndProc_Enteridle(hWnd, message, wParam, lParam, pData);
    case WM_DPICHANGED: // DPI被改变
        WndProc_MDIClient_CustomTabCtrl(hWnd, message, wParam, lParam);
        return Scale_DPIChanged(LOWORD(wParam), HIWORD(wParam), (RECT*)lParam, pData);
    case WM_ERASEBKGND:
    {
        //FillRect((HDC)wParam, &pData->pos.rcClient, pData->draw.color.hbrBackGround);
        //LPCANVAS hCanvas = pData->draw.hCanvas;
        //const int cxClient = pData->pos.width;
        //const int cyClient = pData->pos.height;
        //_canvas_begindraw(hCanvas);
        //HDC hdcWindow = (HDC)wParam;
        //HDC hdc = _canvas_getdc(hCanvas);       assert(hdc != 0);
        //BitBlt(hdcWindow, 0, 0, cxClient, WINDOW_CAPTION_HEIGHT, hdc, 0, 0, SRCCOPY);   // 顶边
        //BitBlt(hdcWindow, 0, WINDOW_CAPTION_HEIGHT, WINDOW_BORDER_WIDTH, cyClient - WINDOW_CAPTION_HEIGHT,
        //    hdc, 0, WINDOW_CAPTION_HEIGHT, SRCCOPY); // 左边

        //BitBlt(hdcWindow, cxClient - WINDOW_BORDER_WIDTH, WINDOW_BORDER_WIDTH,
        //    WINDOW_BORDER_WIDTH, cyClient - WINDOW_CAPTION_HEIGHT,
        //    hdc, cxClient - WINDOW_BORDER_WIDTH, WINDOW_BORDER_WIDTH, SRCCOPY);   // 顶边


        //BitBlt(hdcWindow, WINDOW_BORDER_WIDTH, s_info->rcStatusBar.top,
        //    cxClient - WINDOW_BORDER_WIDTH * 2, cyClient - s_info->rcStatusBar.top,
        //    hdc, WINDOW_BORDER_WIDTH, s_info->rcStatusBar.top, SRCCOPY);   // 底边

        //_canvas_releasedc(hCanvas);
        //bool bdraw = _canvas_enddraw(hCanvas);  assert(bdraw);

        //if (!bdraw && ExGetLastError() == D2DERR_RECREATE_TARGET)
        //{
        //    // 需要清除原有资源并重新创建资源, DiscardDeviceResources();
        //}
        return 1;
    }
        //WM_ACTIVATEAPP = 28
        //WM_ACTIVATE 6
        //WM_NCACTIVATE 134
    case LVM_GETITEMW:  // 获取配置信息, wParam 低位 = 获取ID, 高位保留, lParam = 某些获取需要参数
    {
        return s_info->pfnGetInfo(s_info, LOWORD(wParam), lParam);
    }
    case LVM_SETITEMW:  // 设置配置信息, wParam 低位 = 设置ID, 高位 = 是否重画控件, lParam = 设置的值
    {
        return s_info->pfnSetInfo(s_info, LOWORD(wParam), lParam, HIWORD(wParam));
    }
    case LVM_UPDATE:    // 配置被改变, wParam, lParam 都为0
    {
        // 枚举所有插件, 给所有插件都通知配置被改变

        s_info->pfnConfig(INE_CONFIG_CHANGED, wParam, (LPARAM)s_info); // 通知绘画插件重新插件绘画对象, 内部使用
        COLORREF clr_back = s_info->themeColor->crBack();
        BYTE r = GetRValue(clr_back);
        BYTE g = GetGValue(clr_back);
        BYTE b = GetBValue(clr_back);
        BOOL isDark = CThemeColor::IsArgbLight(255, r, g, b) == false;
        _hook_ThemeChange(isDark);
        return 0;
    }
    case WM_ACTIVATE:
    {
        //_window_activate_changed(LOWORD(wParam) != WA_INACTIVE, lParam, pData);
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_ACTIVATEAPP:
    {
        // lParam = 线程标识符, 如果wParam参数为TRUE, 则lParam是拥有被停用窗口的线程的标识符, 如果wParam为FALSE, 则lParam是拥有正在激活的窗口的线程的标识符
        //static DWORD tid = 0;
        //if (!tid) tid =  GetCurrentThreadId();
        //if (tid == lParam)
            _window_activate_changed(wParam != 0, lParam, pData);
        return 0;
    }
    case WM_NCACTIVATE:
    {
        //LPCANVAS hCanvas = pData->draw.hCanvas;
        //const int cxClient = pData->pos.width;
        //const int cyClient = pData->pos.height;
        //_canvas_begindraw(hCanvas);
        //HDC hdcWindow = GetWindowDC(hWnd);
        //HDC hdc = _canvas_getdc(hCanvas);       assert(hdc != 0);
        //BitBlt(hdcWindow, 0, 0, cxClient, cyClient, hdc, 0, 0, SRCCOPY);
        //_canvas_releasedc(hCanvas);
        //bool bdraw = _canvas_enddraw(hCanvas);  assert(bdraw);

        //if (!bdraw && ExGetLastError() == D2DERR_RECREATE_TARGET)
        //{
        //    // 需要清除原有资源并重新创建资源, DiscardDeviceResources();
        //}
        //ReleaseDC(hWnd, hdcWindow);
        return 1;
    }
    //{
    //    _window_activate_changed(wParam != 0, lParam, pData);
    //    return TRUE;
    //}
    //case WM_ACTIVATE:
    //{
    //    bool isAct = wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE;
    //    if (!isAct)
    //    {
    //        HWND hParent = (HWND)lParam;
    //        while (hParent)
    //        {
    //            if (hParent == hWnd)
    //            {
    //                isAct = true;
    //                break;
    //            }
    //            hParent = (HWND)GetWindowLongPtrW(hParent, GWLP_HWNDPARENT);
    //        }
    //    }
    //    _window_activate_changed(isAct, lParam, pData);
    //    return 0;
    //}
    case WM_MEASUREITEM:
    {
        LPMEASUREITEMSTRUCT item = (LPMEASUREITEMSTRUCT)lParam;
        if (item->CtlType != ODT_MENU)
            return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        return EMenu_OnMeasureItem(hWnd, message, wParam, lParam, pData);
        return WndProc_OnMeasureItem(hWnd, message, wParam, lParam, pData);
    }
    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT dw = (LPDRAWITEMSTRUCT)lParam;
        switch (dw->CtlType)
        {
        case ODT_MENU:
            return EMenu_OnDrawItem(hWnd, message, wParam, lParam, pData);
        case ODT_LISTBOX:
        case ODT_LISTVIEW:
        case ODT_COMBOBOX:
            return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        default:
            return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        }
        return 0;
    }
    case WM_HOTKEY:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

    case WM_CREATE:
        break;
    case WM_GETMINMAXINFO:
        return WndProc_OnGetMinMaxInfo(hWnd, message, wParam, lParam, pData);
    case WM_STYLECHANGED:
        return WndProc_OnStyleChanged(hWnd, message, wParam, lParam, pData);
    case WM_STYLECHANGING:
        return WndProc_OnStyleChanging(hWnd, message, wParam, lParam, pData);
    case WM_SYSCOMMAND:
        return EWndProc_OnNcCommand(hWnd, message, wParam, lParam, pData);
    case WM_WINDOWPOSCHANGING:
        return __OnWindowPosChanging(hWnd, message, wParam, lParam, pData);
    case WM_WINDOWPOSCHANGED:
        return __OnWindowPosChanged(hWnd, message, wParam, lParam, pData);
    case WM_COMMAND:
    {
        if (WndProc_MDIClient_CustomTabCtrl(hWnd, message, wParam, lParam))
            return 0;
        return EWindow_OnCommand(hWnd, LOWORD(wParam), HIWORD(wParam), (HWND)lParam, pData);
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_NCRBUTTONDOWN:
    case WM_NCRBUTTONUP:
    {

//#if _DEBUG_ISVERSION
//        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
//        ScreenToClient(hWnd, &pt);
//        if (PtInRect(&m_rcVersion, pt)) // 在版本号中按下了右键
//        {
//            if (message == WM_NCRBUTTONUP)
//            {
//                //ShellExecuteW(0, L"open", L"http://bbs.125.la/forum.php?mod=redirect&goto=findpost&ptid=14672340&pid=20913172", 0, 0, 1);
//                ShellExecuteW(0, L"open", L"https://www.showdoc.com.cn/1503905757897784/7259668884727845", 0, 0, 1);
//            }
//            s_info->pTips->hide();
//            return 0;
//        }
//#endif
        return WndProc_OnNcMouse(hWnd, message, wParam, lParam, pData);
    }
    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONUP:
    case WM_NCLBUTTONDBLCLK:
    case WM_NCRBUTTONDBLCLK:
    case WM_NCMBUTTONDOWN:
    case WM_NCMBUTTONUP:
    case WM_NCMBUTTONDBLCLK:
        //case WM_NCMOUSEMOVE:
    case WM_NCMOUSEHOVER:
    case WM_NCMOUSELEAVE:
        return WndProc_OnNcMouse(hWnd, message, wParam, lParam, pData);
        //case WM_MOUSELEAVE:
        //case WM_NCMOUSELEAVE:
        //{
        //    m_isHover = 0;
        //    s_info->pTips->hide();
        //    break;
        //}
    case WM_NCMOUSEMOVE:
    {
        LPSYSBTN sysBtn = (LPSYSBTN)pData->param;
        if (sysBtn->index != -1)
        {
            sysBtn->index = -1;
            sysBtn->state = STATE::NORMAL;
            InvalidateRect(pData->hObj, &sysBtn->rcBtn, 0);
        }
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        ScreenToClient(hWnd, &pt);
        if (PtInRect(&m_rcTitle, pt))
        {
            if(!EqualRect(&m_rcTitle, &m_tips_rect))  // 鼠标在标题中, 并且当前提示的位置不是标题的位置
            {
                EWidnow_ShowTips(m_rcTitle, m_eCodeTipsTitle.c_str(), m_eCodeFileName.c_str(), pt.x, -1);
                InvalidateRect(hWnd, &m_rcTitle, 0);
            }

            break;
        }

#if _DEBUG_ISVERSION
        if (PtInRect(&m_rcVersion, pt))     // 鼠标在显示版本号的区域中
        {
            if(!EqualRect(&m_rcVersion, &m_tips_rect))  // 鼠标在版本号中, 并且当前提示的位置不是提示的位置
            {
                const LPCWSTR text =
                    L"\n"
                    L"    1. win10 1809, 内部版本17763起的系统将使用系统内置的深色模式\n"
                    L"    2. win10 1809之前的系统使用自绘的方式\n"
                    L"    3. 系统按钮根据当前设置的主题背景颜色来确认使用深色还是浅色图标\n"
                    L"    4. 修复行号插件使用会崩溃的问题\n"
                    L"    5. 顶部菜单栏不限制加入\n"
                    L"    6. 新增自定义菜单, 顶部菜单 扩展 -> 自定义菜单(C)...\n"
                    L"    7. 修复F3查找下一个崩溃的问题\n"
                    L"    8. 修复多屏最小化还原后尺寸不正确的问题\n"
                    L"  有建议或者bug可以在论坛跟帖反馈或者群里 @扩大福 反馈, 勿私聊\n"
                    L"  官方QQ群 788539398\n"
                    L"本提示消息最后更新日期为 2024-12-31\n\n"
                    L"出门工作去了, 下次更新不知道什么时候了"
                    ;
                EWidnow_ShowTips(m_rcVersion, L"1.5测试版, 更新内容如下:", text, pt.x, -1);
                InvalidateRect(hWnd, &m_rcVersion, 0);
            }
            break;
        }
#endif
        s_info->pTips->hide();
        m_isHover = 0;
        break;
    }
    case WM_MOUSEMOVE:
    {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        if (pt.x < sysBtn.rcBtn.left && PtInRect(&s_info->rcMenu, pt))  // 鼠标是在菜单栏中, 交给菜单处理
        {
            return WndProc_Custom_Menu(hWnd, message, wParam, lParam, pData);
        }
        //if (pData->draw.indexHot != -1)
        //    WndProc_Custom_Menu(hWnd, WM_MOUSELEAVE, wParam, lParam, pData);
        if (PtInRect(&s_info->rcCustomTab, pt))
            return WndProc_MDIClient_CustomTabCtrl(hWnd, message, wParam, lParam);
        LRESULT ret = EWindow_TabMouse(hWnd, message, wParam, lParam, pData);
        if (pData->isReturn)
            return ret;
        return WndProc_OnMouse(hWnd, message, wParam, lParam, pData);
    }
    case WM_CAPTURECHANGED:
    {
        LRESULT ret = EWindow_TabMouse(hWnd, message, wParam, lParam, pData);
        if (pData->isReturn)
            return ret;
        if (tabInfo->indexDown != -1)
            return WndProc_MDIClient_CustomTabCtrl(hWnd, message, wParam, lParam);
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_LBUTTONDOWN:
    {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        if (pt.x < sysBtn.rcBtn.left && PtInRect(&s_info->rcMenu, pt))  // 鼠标是在菜单栏中, 交给菜单处理
        {
            return WndProc_Custom_Menu(hWnd, message, wParam, lParam, pData);
        }
        LRESULT ret = EWindow_TabMouse(hWnd, message, wParam, lParam, pData);
        if (pData->isReturn)
            return ret;
        
        if (PtInRect(&s_info->rcCustomTab, pt))
            return WndProc_MDIClient_CustomTabCtrl(hWnd, message, wParam, lParam);
        return WndProc_OnMouse(hWnd, message, wParam, lParam, pData);
    }
    case WM_LBUTTONUP:
    {
        if (tabInfo->indexDown != -1)
            return WndProc_MDIClient_CustomTabCtrl(hWnd, message, wParam, lParam);
        LRESULT ret = EWindow_TabMouse(hWnd, message, wParam, lParam, pData);
        if (pData->isReturn)
            return ret;
        return WndProc_OnMouse(hWnd, message, wParam, lParam, pData);
    }
    case WM_RBUTTONUP:
    {
        if (tabInfo->indexDown != -1)
            return WndProc_MDIClient_CustomTabCtrl(hWnd, message, wParam, lParam);
        return WndProc_OnMouse(hWnd, message, wParam, lParam, pData);
    }

    case WM_RBUTTONDOWN:
    {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        if (PtInRect(&s_info->rcCustomTab, pt))
            return WndProc_MDIClient_CustomTabCtrl(hWnd, message, wParam, lParam);
        return WndProc_OnMouse(hWnd, message, wParam, lParam, pData);
    }
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MBUTTONDBLCLK:
    case WM_MOUSEWHEEL:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    case WM_XBUTTONDBLCLK:
    case WM_MOUSEHWHEEL:
    {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        if (PtInRect(&s_info->rcCustomTab, pt))
            return WndProc_MDIClient_CustomTabCtrl(hWnd, message, wParam, lParam);
        return WndProc_OnMouse(hWnd, message, wParam, lParam, pData);
    }
    case WM_USER + 0x12:
        return WndProc_Custom_Menu(hWnd, message, wParam, lParam, pData);
    case WM_NCCALCSIZE:
        return WndProc_OnNcCalcsize(hWnd, message, wParam, lParam, pData);
    case WM_NCPAINT:
        InvalidateRect(hWnd, 0, 0);
        return 0;
    case WM_SIZE:
        WndProc_MDIClient_CustomTabCtrl(hWnd, message, wParam, lParam);
        return __OnSize(hWnd, message, wParam, lParam, pData);
    case WM_MOVE:
        s_info->pTips->hide();
        m_isHover = 0;
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    case WM_PAINT:
        return EWndProc_OnPaint(hWnd, message, wParam, lParam, pData);
    case WM_SETTEXT:
    {
        if (wParam > 0)
            return WndProc_MDIClient_CustomTabCtrl(hWnd, message, wParam, lParam);
        LRESULT ret = CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
        _e_get_title(0);
        _window_recalc_title(pData, false);
        return ret;
    }
    case WM_NCHITTEST:
        return WndProc_OnNcHittest(hWnd, message, wParam, lParam, pData);
        //case WM_MDIGETACTIVE:
        //    return 0;
        //case WM_KILLFOCUS:
        //    m_isFocus = 0;
        //    InvalidateRect(hWnd, &m_rcTitle, 0);
        //    return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
        //case WM_SETFOCUS:
        //    m_isFocus = 1;
        //    InvalidateRect(hWnd, &m_rcTitle, 0);
        //    return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    case WM_DESTROY:
    {
        //UNDONE 关闭易语言窗口前先关闭所有选项然后保存, 下次打开的时候就没有选项, 打开速度会加快
        // 需要记录当前已经打开的选项, 然后在初始化的时候把这些选项加入到选择夹里
        // 点击的时候判断有没有窗口, 有的话就不处理, 没有的话就需要触发点击事件让他显示出来
        // 
        // 如果撤销按钮不是禁止状态, 那就是有改变过代码, 点击保存
        //if (s_info->pfnGetInfo(s_info, IGI_STATE_TOOL_GET, 57643))
        //{
        //    CArray<CUSTOMTAB_DATA>& arr = *s_info->hWnds;
        //    const int count = arr.size();
        //    for (int i = count - 1; i >= 0; --i)
        //    {
        //        MDIClient_CustomTabCtrl_ClickItem(s_info->hCustomTab, i, true, pData);
        //    }
        //    s_info->pfnCallMenu(57603); // 保存
        //}
        TimeProc_SaveClr(hWnd, WM_TIMER, (UINT_PTR)TimeProc_SaveClr, 0);
        KillTimer(hWnd, (UINT_PTR)TimeProc_SaveClr);

        static int aaa = 0;
        std::map<HWND, OBJSTRUCT>& map = _get_subclass_allWindow();
        for (auto it = map.begin(); it != map.end(); ++it)
        {
            OBJSTRUCT& data = it->second;
            s_info->pfnGetDefData(&data);
            // 滚动条绘画时每次都会创建画刷, 只要枚举出滚动条对象修改颜色就可以
            IDrawScroll* iScroll = (IDrawScroll*)data.drawScroll;
            if (!iScroll) continue;


            // 取消所有自绘滚动条的挂接
            iScroll->m_pfn_CalcScroll = 0;
            iScroll->m_pfn_CalcScroll = 0;
            iScroll->m_pfn_CalcScroll = 0;

            //wstr::debug(L"%d  0x%08x取消滚动条绘画挂接, m_pfn_CalcScroll = 0X%08x, m_pfn_CalcScroll = 0X%08x, m_pfn_CalcScroll = 0X%08x\n",
            //    ++aaa, iScroll, iScroll->m_pfn_CalcScroll, iScroll->m_pfn_CalcScroll, iScroll->m_pfn_CalcScroll);

        }
        ShowWindowEx(s_info->eWnd->GetMenuMenu(), SW_SHOW);
        _canvas_destroy(pData->draw.hCanvas);
        HANDLE hThread = CreateThread(0, 0, Thread_EWindowDestroy, pData, 0, 0);
        if (hThread)
            CloseHandle(hThread);
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_NCDESTROY:
    {
        //TODO 需要释放所有插件
        Thread_EWindowDestroy(0);   // 告诉这个函数, WM_NCDESTROY 被执行了
        OnNcDestroy();
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_DROPFILES:
        return EWndProc_OnDropFile(hWnd, message, wParam, lParam, pData);

    case WM_MOUSELEAVE:
    case WM_MDIDESTROY:
    case TCM_DELETEALLITEMS:     // wParam, lParam == 0;
    case TCM_DELETEITEM:     // wParam = 项目索引, lParam == 0;
    case TCM_GETCURFOCUS:   // wParam, lParam == 0;
    case TCM_GETCURSEL:     // wParam, lParam == 0;
    case TCM_GETITEMA:
    case TCM_GETITEMW:      // wParam = 索引, lParam = TCITEM 结构指针, 如果成功，则返回 TRUE ; 否则返回 FALSE
    case TCM_GETITEMCOUNT:
    case TCM_GETITEMRECT:   // wParam = 索引, lParam = RECT 结构指针, 返回真假
    case TCM_HITTEST:       // wParam = 0, lParam = TCHITTESTINFO 结构指针, 返回子夹索引, 鼠标没有在子夹上就返回-1
    case TCM_INSERTITEMA:
    case TCM_INSERTITEMW:   // wParam = 索引, lParam = TCITEM 结构指针, 如果成功，则返回新的选项卡的索引; 否则返回-1
    case TCM_SETCURFOCUS:   // wParam = 索引, lParam = 0, 没有返回值
    case TCM_SETCURSEL:     // wParam = 索引, lParam = 0, 没有返回值
    case TCM_SETITEMA:
    case TCM_SETITEMW:      // wParam = 索引, lParam = TCITEM 结构指针, 如果成功，则返回 TRUE ; 否则返回 FALSE
    case TCM_SETITEMSIZE:   // wParam = 0, lParam = LOWORD=宽度, HIWORD=高度, 返回旧宽度和高度。 宽度位于返回值的 LOWORD 中，高度在 HIWORD中
    case TCM_SETMINTABWIDTH:
    case TCM_SETPADDING:
        return WndProc_MDIClient_CustomTabCtrl(hWnd, message, wParam, lParam);



    default:
    {
        if (message == s_info->uMsgEWindowThreadCallback)
        {
            // 需要调用回调函数, wParam是函数地址, lParam是函数参数
            pfn_EWindowThreadCallback pfn = (pfn_EWindowThreadCallback)wParam;
            pfn((int)lParam);
            return 0;
        }
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    }
    return 0;
}

void _window_recalc_title(LPOBJSTRUCT pData, bool isDrawMenu)
{
    if (IsIconic(s_info->hEWnd))
        return;
    const int left = s_info->rcMenu.right + 10;
    const int right = sysBtn.rcBtn.left - 10;

    const int width = right - left;
    const int height = sysBtn.rcBtn.bottom - sysBtn.rcBtn.top;
    float textWidth = 0;
    float textHeight = 0;

    LPEX_FONT hFont = g_scale.GetFont(s_info->window_d2d_info.hFont);
    _canvas_calctextsize(pData->draw.hCanvas, hFont, m_eCodeShowName.c_str(), m_eCodeShowName.size(),
        0, (float)width, (float)height, &textWidth, &textHeight);

    m_rcTitle.left = left;
    m_rcTitle.right = m_rcTitle.left + (int)textWidth + 20;
    if (m_rcTitle.right - m_rcTitle.left > width)
        m_rcTitle.right = m_rcTitle.left + width;

    ModifySysButtonPos(pData);
    //SetWindowPos(s_info->hEWnd, 0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_DRAWFRAME);

    if (isDrawMenu)
    {
        RECT rcDraw = { s_info->rcMenu.left, 0, right, m_rcTitle.bottom };
        InvalidateRect(pData->hWnd, &rcDraw, 0);    // 重画从菜单到标题的位置
    }
    else
    {
        InvalidateRect(pData->hWnd, &m_rcTitle, 0); // 只重画标题部分
    }
}

LRESULT __OnSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    //RECT rcClient;
    //GetClientRect(hWnd, &rcClient);
    //lParam = MAKELONG(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
    //lParam = MAKELONG(pData->pos.rc.right - pData->pos.rc.left, pData->pos.rc.bottom - pData->pos.rc.top);

    LRESULT ret = 0;
    //ret = WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    int cxClient = GET_X_LPARAM(lParam);
    int cyClient = GET_Y_LPARAM(lParam);

    bool isZoomed = (wParam == SIZE_MAXIMIZED || wParam == SIZE_MAXSHOW);
    s_info->isZoomed = isZoomed;
    sysBtn.btn[SYSBTN_MAX].flag = isZoomed ? SC_RESTORE : SC_MAXIMIZE;
    if (isZoomed) s_info->shadow->SetAlpha(0);
    else s_info->shadow->SetAlpha(255);
    pData->winState = wParam;

    //InvalidateRect(s_info->hMDIClient, 0, 0);
    return ret;
}

static const LPCWSTR titleMid = L" - ";
LPCWSTR _e_get_title(LPCSTR szTitle)
{
    wstr file;
    if (szTitle)
    {
        file = _str::A2W(szTitle);
    }
    else
    {
        const int titleLen = GetWindowTextLengthW(s_info->hEWnd) + 1;
        file.reserve(titleLen);
        GetWindowTextW(s_info->hEWnd, file.data(), titleLen);
        file.resize(wcslen(file.c_str()));
    }
    LPWSTR pFile = file.data();
    LPWSTR pos = 0;
    std::vector<LPCWSTR> arr;
    arr.reserve(20);
    int i = 0;
    while (pFile)
    {
        arr.push_back(pFile);
        if (pFile[1] == ':' && pFile[2] == '\\')    // 路径中间有 - , 需要寻找点后面的 - 
        {
            size_t npos = wstr::find(pFile, L" - Windows", 0, false);
            if(npos == wstr::npos) npos = wstr::find(pFile, L" - Linux", 0, false);
            if (npos == wstr::npos)break;

            pFile += npos;  // 指向了路径 .e 后面的空格

        }
        else
        {
            pFile = wcsstr(pFile, L" - ");

        }
        if (!pFile)break;

        *pFile++ = 0; *pFile++ = 0; *pFile++ = 0;   // 把 - 替换成空
    }

    size_t len = arr.size();
    m_eCodeShowName.reserve(260);
    m_eCodeFileName.reserve(260);
    for (size_t i = 0; i < len; i++)
    {
        LPCWSTR str = arr[i];
        // 第一个成员, 要么是易语言-加密狗, 要么是配置里设置的项目名字, 这个作为标题
        // 如果
        if (i == 0) 
        {
            m_eCodeTipsTitle.assign(str);
            continue;
        }

        if (i == 1) // 有保存源码的话会查找到 :\ , 没有保存的话第二个成员就是程序类型
        {
            LPCWSTR pos = wcschr(str, '\\');
            if (pos)    // 有值就是路径
            {
                LPCWSTR dot = wcsrchr(str, '.');
                LPCWSTR posFile = wcsrchr(str, '\\');
                if (dot && posFile)
                {
                    m_eCodeFileName.assign(str, dot - str + 2);
                    m_eCodeShowName.assign(posFile + 1, dot - posFile - 1);
                }
                else
                {
                    //assert(false);
                    m_eCodeFileName.assign(str);
                    m_eCodeShowName.assign(str);
                }
            }
            else
            {
                // 没有值的话就是程序类型
                m_eCodeType = str;
                m_eCodeShowName.assign(L"* ").append(m_eCodeType);
                m_eCodeFileName.assign(L"未保存的 ").append(m_eCodeType);
            }
            continue;
        }

        if (i == 2)
        {
            if (wcsnicmp_api(str, L"windows", 7) == 0
                || wcsnicmp_api(str, L"linux", 5) == 0)
            {
                m_eCodeType = str;
            }
            continue;
        }

    }

    if (!m_eCodeShowName.empty() && m_eCodeShowName.front() != '*')
    {
        m_eCodeShowName.append(titleMid).append(L"[");
        m_eCodeShowName.append(m_eCodeType);
        m_eCodeShowName.append(L"]");
    }
    return 0;
}

// 将图片保存到磁盘
inline void asdas()
{
    ////"D:\cpp\SVN工作目录\易语言界面\iDraw\src\未标题-19.png"
    LPEX_IMAGE hImg[] =
    {
        _img_create_fromfile(LR"(D:\cpp\AIDE\res\系统按钮\关闭按钮.png)"),
        _img_create_fromfile(LR"(D:\cpp\AIDE\res\系统按钮\还原按钮.png)"),
        _img_create_fromfile(LR"(D:\cpp\AIDE\res\系统按钮\最大化按钮.png)"),
        _img_create_fromfile(LR"(D:\cpp\AIDE\res\系统按钮\最小化按钮.png)"),
        _img_create_fromfile(LR"(D:\cpp\AIDE\res\系统按钮\设置按钮.png)"),
        _img_create_fromfile(LR"(D:\cpp\AIDE\res\系统按钮\皮肤按钮.png)"),
        _img_create_fromfile(LR"(D:\cpp\AIDE\res\系统按钮\帮助按钮.png)"),
        _img_create_fromfile(LR"(D:\cpp\AIDE\res\系统按钮\向下按钮.png)"),

        _img_create_fromfile(LR"(D:\cpp\AIDE\res\系统按钮\黑色\关闭按钮.png)"),
        _img_create_fromfile(LR"(D:\cpp\AIDE\res\系统按钮\黑色\还原按钮.png)"),
        _img_create_fromfile(LR"(D:\cpp\AIDE\res\系统按钮\黑色\最大化按钮.png)"),
        _img_create_fromfile(LR"(D:\cpp\AIDE\res\系统按钮\黑色\最小化按钮.png)"),
        _img_create_fromfile(LR"(D:\cpp\AIDE\res\系统按钮\黑色\设置按钮.png)"),
        _img_create_fromfile(LR"(D:\cpp\AIDE\res\系统按钮\黑色\皮肤按钮.png)"),
        _img_create_fromfile(LR"(D:\cpp\AIDE\res\系统按钮\黑色\帮助按钮.png)"),
        _img_create_fromfile(LR"(D:\cpp\AIDE\res\系统按钮\黑色\向下按钮.png)"),
    };
    wstr name(260);
    LPCANVAS hCanvas = _canvas_create(0, 8*30, 60);
    _canvas_begindraw(hCanvas);

    for (int i = 0; i < 8; i++)
    {
        float left = (float)i * 30;
        int n = i + 8;
        _canvas_drawimagerect(hCanvas, hImg[i], left,  0, left + 30, 30, 255);
        _canvas_drawimagerect(hCanvas, hImg[n], left, 30, left + 30, 60, 255);
    }


    _canvas_enddraw(hCanvas);
    SIZE_T size = 0;
    LPBYTE ptr = _img_save_formcanvas(hCanvas, &size);
    CFileRW f;
    name.assign(LR"(D:\cpp\SVN工作目录\易语言界面\src\系统按钮.png)");
    f.open(name.c_str(), 4);
    f.write(ptr, size);
    free(ptr);
    _canvas_destroy(hCanvas);
}

LRESULT EWndProc_OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    const RECT& rc = pData->pos.rcClient;
    LPCANVAS& hCanvas = pData->draw.hCanvas;
    const int cxClient = pData->pos.width;
    const int cyClient = pData->pos.height;
    BEGIN_END_PAINT_STRUCT ps;
    s_info->iDraw->BeginPaint(pData, ps, true, false);
    COLORREF crBack = s_info->themeColor->crBackExtern();
    _canvas_clear(hCanvas, RGB2ARGB(crBack, 255));
    const int border = s_info->scale(WINDOW_BORDER_WIDTH);

    //TODO 未就绪就需要画易语言欢迎语, 可以修改这里显示图片之类的数据
    if (!s_info->isReady)
    {
        _draw_ready_title(hCanvas, RGB2ARGB(pData->draw.color.crText, 244), rc, 0);
        //RECT rc;
        //GetWindowRect(s_info->hEWnd, &rc);
        s_info->iDraw->EndPaint(ps);
        //InvalidateRect(hWnd, 0, 0);
        //UpdateWindow(hWnd);
        //auto pfnTimer = [](HWND hWnd, UINT message, UINT_PTR id, DWORD timer) -> void
        //{
        //    InvalidateRect(hWnd, 0, 0);
        //    KillTimer(hWnd, id);
        //};
        //SetTimer(hWnd, 123871, 100, pfnTimer);
        return 0;
    }

#if 0
    LPBRUSHPEN brush = _brush_create(ARGB_D2D(255, 255, 0, 0));
    // 整个客户区位置
    RECT_F rcTmp = pData->pos.rcClient;
    _canvas_drawRectangle(hCanvas, brush, &rcTmp, 2);

    // 程序图标画出位置
    rcTmp = sysBtn.rcIcon;
    _canvas_drawRectangle(hCanvas, brush, &rcTmp, 2);

    // 标题画出位置
    rcTmp = sysBtn.rcCaption;
    _canvas_drawRectangle(hCanvas, brush, &rcTmp, 2);
    rcTmp.offset(5, 0);
    _canvas_drawtext(hCanvas, pData->draw.hFont, brush, L"标题栏画出位置", 0, DT_VCENTER | DT_SINGLELINE, &rcTmp);

    // 系统按钮绘画位置
    LPBRUSHPEN brush1 = _brush_create(ARGB_D2D(255, 128, 128, 128));
    for (DWORD i = 0; i < sysBtn.count; i++)
    {
        RECT_F rcSysBtn = sysBtn.btn[i].rc;
        _canvas_fillRectangle(hCanvas, brush1, &rcSysBtn);
        _canvas_drawRectangle(hCanvas, brush, &rcSysBtn, 2);
    }
    _brush_destroy(brush1);
    _brush_destroy(brush);
#endif

    int oldMode = 0;
    _canvas_SetTextRenderingHint(hCanvas, 1, &oldMode);
    if (m_rcTitle.right - m_rcTitle.left > 0 && Rect_isOverlap(m_rcTitle, ps.rcPaint))
        EWindow_OnPaint_DrawTitle(hCanvas, pData, titleMid);

#if _DEBUG_ISVERSION
    m_rcVersion = m_rcTitle;
    m_rcVersion.left = m_rcVersion.right + s_info->scale(10);
    m_rcVersion.right = sysBtn.rcBtn.left - s_info->scale(10);
    if (m_rcTitle.right - m_rcTitle.left > 0 && m_rcVersion.left > m_rcTitle.right && m_rcVersion.right - m_rcVersion.left > 0)
    {
        RECT_F rcVersion(m_rcVersion);
        wchar_t szVession[100];
        int nVersionLen = swprintf_s(szVession, 100, L"V%s%s", IDRAW_VERSION, L" 测试版");
        if (!m_hbrVersion)
            //m_hbrVersion = _brush_create(MAKEARGB(255, 128, 128, 255));
            m_hbrVersion = _brush_create(MAKEARGB(255, 255, 0, 0));
        LPEX_FONT hFont = g_scale.GetFont(s_info->window_d2d_info.hFont);
        _canvas_drawtextex(hCanvas, hFont, m_hbrVersion, szVession, nVersionLen,
            DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS, &rcVersion, 0, 0, 0, 0, &rcVersion);

        m_rcVersion.right = m_rcVersion.left + (int)(rcVersion.right - rcVersion.left);
    }
#endif
    RECT rcEIcon = { 0, 0, WINDOW_CAPTION_HEIGHT, WINDOW_CAPTION_HEIGHT };
    if (Rect_isOverlap(rcEIcon, ps.rcPaint))
        EWindow_OnPaint_DrawEIcon(hCanvas, pData);

    if (Rect_isOverlap(s_info->rcMenu, ps.rcPaint)) // 需要重画菜单
    {
        if (s_info->rcMenu.right >= sysBtn.rcBtn.left)
            _canvas_setClip(hCanvas, (float)s_info->rcMenu.left, (float)sysBtn.rcBtn.top,
                (float)sysBtn.rcBtn.left, (float)sysBtn.rcBtn.bottom, 0);
        WndProc_Custom_Menu(hWnd, message, (WPARAM)hCanvas, lParam, pData);
        if (s_info->rcMenu.right >= sysBtn.rcBtn.left)
            _canvas_resetClip(hCanvas);
    }
    if (Rect_isOverlap(s_info->rcCustomTab, ps.rcPaint)) // 需要重画代码选择夹
    {
        WndProc_MDIClient_CustomTabCtrl(hWnd, message, (WPARAM)hCanvas, lParam);
    }
    // 绘画自动隐藏使用的选择夹
    // 内部会判断是否需要绘画选择夹
    EWindow_DrawTab(hCanvas, pData, ps);
    if (Rect_isOverlap(sysBtn.rcBtn, ps.rcPaint))   // 重画区域的顶边小于系统按钮的底边, 需要重画系统按钮
    {
        EWindow_DrawSystemButton(hCanvas, pData);
    }
    

    int statusHeight = s_info->rcStatusBar.bottom - s_info->rcStatusBar.top;
    if (statusHeight > 0)
    {
        LPEX_BRUSH hbrStatusBar = s_info->isStop ? s_info->window_d2d_info.hbrBorder_run : s_info->window_d2d_info.hbrBorder;
        RECT_F rcf(0.f, (float)s_info->rcStatusBar.top, (float)cxClient, (float)cyClient);
        _canvas_fillRectangle(hCanvas, hbrStatusBar, &rcf);
        const int len = SendMessageW(s_info->hStatusBar, SB_GETPARTS, 0, 0);
        RECT rcItems[5] = { 0 };
        const int widths[] = { 0, 18, 62, 38, 30 };
        int rightBar = cxClient - border;
        for (int i = 4; i >= 0; i--)
        {
            int scaleWidth = s_info->scale(widths[i]);
            rcItems[i].left     = rightBar - scaleWidth;
            rcItems[i].top      = s_info->rcStatusBar.top + s_info->scale(5);
            rcItems[i].right    = rightBar;
            rcItems[i].bottom   = s_info->rcStatusBar.bottom;
            rightBar -= (scaleWidth + border);   // 每个项目之间需要分隔 border 的距离
        }
        rcItems[0].left = border;
        
        LPEX_FONT hFont = g_scale.GetFont(pData->draw.hFont);
        for (int i = 0; i < len; i++)
        {
            EWindow_StatusBar_drawItem(hCanvas, i, pData, rcItems, hFont);
        }

    }

    ARGB _cr[] = {
        MAKEARGB(255, 255, 0, 0),
        MAKEARGB(255, 255, 255, 0),
        MAKEARGB(255, 255, 255, 255),
        MAKEARGB(255, 0, 255, 0),
        MAKEARGB(255, 0, 255, 255),
        MAKEARGB(255, 0, 0, 255),
    };

    hCanvas->SetTextRenderingHint(oldMode);

    bool bdraw = s_info->iDraw->EndPaint(ps);    assert(bdraw);

    if (!bdraw && ExGetLastError() == D2DERR_RECREATE_TARGET)
    {
        // 当程序在运行过程中，图形设备可能因为某些原因而失效
        // 例如，设备显示分辨率被改变，或者用户移除显示适配器（显示器）等等
        // 如果设备丢失，那么渲染目标也会变得无效，顺带着所有的设备相关的资源都会失效
        // TODO Direct2D 当接收到设备丢失的信号后，会在调用 EndDraw 函数后返回 D2DERR_RECREATE_TARGET 代码
        // 如果你收到这个错误码，你必须重新创建渲染目标以及设备相关资源
        // 
        // 需要清除原有资源并重新创建资源, DiscardDeviceResources();

        EWindow_ResetFont();


        // 重新创建设备, 需要把内部的设备上下文重新创建, 因为画笔画刷这些都是从这个上下文创建的
        s_info->d2d->recreate_Device();

        s_info->pfnReloadDxResource();  // 先重新创建所有画笔画刷字体, 然后再重新创建所有画布

        std::map<HWND, OBJSTRUCT>& map = _get_subclass_allWindow();
        for (auto it = map.begin(); it != map.end(); ++it)
        {
            OBJSTRUCT& data = it->second;
            s_info->pfnGetDefData(&data);
            if (data.fnCrChange)
                data.fnCrChange(&data);     // 从默认配色里重新设置配色, 如果有自定义配色函数, 就调用自定义配色函数

            if (data.draw.hCanvas)
            {
                UINT cxCanvas = data.draw.hCanvas->GetWidth();
                UINT cyCanvas = data.draw.hCanvas->GetHeight();
                HWND hWndCanvas = data.draw.hCanvas->GetWindow();
                s_info->d2d->_canvas_destroy(data.draw.hCanvas);
                data.draw.hCanvas = s_info->d2d->_canvas_create(hWndCanvas, cxCanvas, cyCanvas);
            }
        }


        THEME_D2D_INFO& d2d = s_info->window_d2d_info;
        IThemeColor& theme = *s_info->themeColor;
        IDraw_ReloadDrawObject(d2d, theme);     // 重新加载绘画菜单使用的资源

        static bool isShowMessageBox;
        if (!isShowMessageBox)
        {
            isShowMessageBox = true;
            const LPCWSTR msgText =
                L"D2D资源失效, 已重新创建\n"
                L"看见这个弹窗后出现绘画错误, 或者崩溃的问题, 请联系006\n\n"
                L"由于开发环境没有出现过这个问题\n"
                L"所以, 请把重现方法发送给006, 让006处理\n"
                L"以及, 出现这个弹窗后与没出现前有什么区别\n"
                L"最好能有个截图, 方便开发人员处理\n"
                ;
            MessageBoxW(hWnd, msgText, L"D2D资源失效", 0);
        }


    }
    return 0;
}



void EWindow_StatusBar_drawItem(LPCANVAS hCanvas, int i, LPOBJSTRUCT pData, RECT* rcItems, LPEX_FONT hFont)
{
    const int len = SendMessageW(s_info->hStatusBar, SB_GETTEXTLENGTHW, i, 0);
    const int hi = HIWORD(len);
    wstr text(LOWORD(len) + 1);
    SendMessageW(s_info->hStatusBar, SB_GETTEXT, i, (LPARAM)text.data());

    LPWSTR str = (LPWSTR)text.data();
    LPWSTR pos = wcschr(str, '\r');
    while (pos)
    {
        if (pos[1] == 0)
            break;
        if (pos[1] == '\r')
        {
            // 连续两个 \r, 那就是自己设置的换行, 修改成\r\n
            LPWSTR ptr = str;
            while (*ptr)    // 把\r\r换成\r\n
            {
                wchar_t& ch = *ptr++;
                if (ch == '\r' && *ptr == '\r')
                    *ptr++ = '\n';
            }
            break;
        }
        else
        {
            pos = wcschr(pos + 1, '\r');    // 不是两个\r的话, 继续寻找到没有找到为止
        }
    }

    RECT_F rcf(rcItems[i]);
    LPEX_BRUSH hbr;
    if (!s_info->isActivation)
        hbr = s_info->window_d2d_info.hbrTextStatusBar_nf;
    else if (s_info->isRun)
        hbr = s_info->window_d2d_info.hbrTextStatusBar_run;
    else
        hbr = s_info->window_d2d_info.hbrTextStatusBar;

    _canvas_drawtext(hCanvas, hFont, hbr, text.data(), -1, DT_VCENTER | DT_SINGLELINE | DT_LEFT | DT_WORD_ELLIPSIS, &rcf);
    //DrawTextW(hdc, text.data(), -1, &rcItem, DT_VCENTER | DT_SINGLELINE | DT_LEFT | DT_WORD_ELLIPSIS);

}

LRESULT __OnWindowPosChanging(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    LPWINDOWPOS pos = (LPWINDOWPOS)lParam;
    pData->pos.rcIng = { pos->x, pos->y, pos->x + pos->cx, pos->y + pos->cy };
    // 没有这两个标志, 要么改变了尺寸, 要么移动了窗口
    if (!((pos->flags & SWP_NOMOVE) && (pos->flags & SWP_NOSIZE)))
    {

    }
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}
LRESULT __OnWindowPosChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    LPWINDOWPOS pos = (LPWINDOWPOS)lParam;
    // 没有这两个标志, 要么改变了尺寸, 要么移动了窗口
    if ((pos->flags & SWP_NOMOVE) == 0 || (pos->flags & SWP_NOSIZE) == 0)
    {
        pData->pos.rc.left      = pos->x;
        pData->pos.rc.top       = pos->y;
        s_info->rcEWindow.left  = pos->x;
        s_info->rcEWindow.top   = pos->y;

        if ((pos->flags & SWP_NOSIZE) == 0) // 没有不调整尺寸标志, 那就是调整了尺寸, 需要重新计算位置
        {
            pData->pos.rc.right         = pos->x + pos->cx;
            pData->pos.rc.bottom        = pos->y + pos->cy;
            s_info->rcEWindow.right     = s_info->rcEWindow.left + pos->cx;
            s_info->rcEWindow.bottom    = s_info->rcEWindow.top + pos->cy;

            s_info->cxClient = pos->cx;
            s_info->cyClient = pos->cy;

            pData->pos.rcClient.left    = 0;
            pData->pos.rcClient.top     = 0;
            pData->pos.rcClient.right   = pos->cx;
            pData->pos.rcClient.bottom  = pos->cy;

            pData->pos.x = pos->x;
            pData->pos.y = pos->y;
            pData->pos.width = pos->cx;
            pData->pos.height = pos->cy;


            int width  = (int)pData->draw.hCanvas->GetWidth();
            int height = (int)pData->draw.hCanvas->GetHeight();
            if (width < pData->pos.width || height < pData->pos.height)
                _canvas_resize(pData->draw.hCanvas, pData->pos.width, pData->pos.height);

            sysBtn.rcBtn.right = pData->pos.width;
            sysBtn.rcBtn.left = sysBtn.rcBtn.right - sysBtn.count * SYSBTN_WIDTH;
            sysBtn.rcBtn.top = 0;
            sysBtn.rcBtn.bottom = SYSBTN_HEIGHT;

            ModifySysButtonPos(pData);

            HRGN hRgn = CreateRectRgn(0, 0, pos->cx, pos->cy);
            SetWindowRgn(hWnd, hRgn, false);
            DeleteObject(hRgn);
        }

        s_info->shadow->Update(pData->pos.rc);
    }
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}


void CALLBACK TimerProc_LeaveTitle(HWND hWnd, UINT message, UINT_PTR id, DWORD timer)
{
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);
    if (!PtInRect(&m_tips_rect, pt))
    {
        s_info->pTips->hide();
        KillTimer(hWnd, id);
        m_tips_rect = { 0 };
        m_isHover = 0;
    }
}

void EWidnow_ShowTips(const RECT& rc, LPCWSTR title, LPCWSTR tips, int x, int y)
{
    m_tips_rect = rc;
    SetTimer(s_info->hEWnd, (UINT_PTR)s_info->pTips, 300, TimerProc_LeaveTitle);
    m_isHover = 1;

    s_info->pTips->SetTipsTitle(title);
    POINT pt = { x, y};
    if (x == -1) pt.x = rc.left;
    if (y == -1) pt.y = rc.bottom;
    ClientToScreen(s_info->hEWnd, &pt);
    LPEX_FONT hFont = g_scale.GetFont(s_info->defData->draw.hFont);
    LPEX_FONT hFontBold = g_scale.GetFont(s_info->defData->draw.hFontBold);
    s_info->pTips->show(tips, hFontBold, hFont, pt.x, pt.y);
}


LRESULT WndProc_Enteridle(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    // WM_ENTERIDLE
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}