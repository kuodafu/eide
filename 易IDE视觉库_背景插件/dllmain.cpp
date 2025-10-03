// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")



LPIDRAW_INFO g_info;
IGetEWindow* g_eWnd;
ID2DDraw* g_dx;
std::wstring m_imagePath;    // 插件读取图片的目录, 完整目录, 后面带'\'
HINSTANCE g_hInst;           // 当前插件的模块句柄

static LPEX_IMAGE g_showImage;      // 当前显示的图片
static RECT m_rc;                   // MDI第三层窗口位置, 基于易语言左上角
static bool m_isDraw;               // 是否绘画, 在启用/禁用菜单里赋值


#define MENU_ITEMHEIGHT     60  // 菜单项高度, 预览的图片根据这个高度绘画位置
#define INI_ENABLED         L"enabled"  // 是否启用绘画
#define INI_INDEX           L"index"    // 当前选中图片索引
#define INI_IMAGE_PATH      L"path"     // 背景图片的目录, 背景插件从这个目录读取图片



BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        g_hInst = hModule;
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


inline void GetMDIClient3() // 获取mdi下第三层窗口的位置
{
    int id = 0;
    HWND hChild[3] = { 0 };
    hChild[0] = GetWindow(g_eWnd->GetMDIClient(), GW_CHILD);


    while (hChild[0])
    {
        id = GetDlgCtrlID(hChild[0]);
        if (id >= 65280 && id < 66000)  // 第一层窗口是 65280 开始
            break;
        hChild[0] = GetWindow(hChild[0], GW_HWNDNEXT);
    }
    if (!hChild[0])
        return;

    hChild[1] = GetWindow(hChild[0], GW_CHILD);    // mdi下的第二层窗口
    while (hChild[1])
    {
        id = GetDlgCtrlID(hChild[1]);
        if (id == 59648)
            break;
        hChild[1] = GetWindow(hChild[1], GW_HWNDNEXT);
    }

    if (!hChild[1])
        return;


    hChild[2] = GetWindow(hChild[1], GW_CHILD);    // MDI下的第三层窗口, 根据这个窗口判断是写代码还是起始页或者是窗口设计器


    while (hChild[2])  // 1=设计器或者起始页, 59648=代码区, 只要不是这两种情况都继续往后取
    {
        // 这里的做法是排除滚动条
        id = GetDlgCtrlID(hChild[2]);
        if (id == 1 || id == 59648) break;
        hChild[2] = GetWindow(hChild[2], GW_HWNDNEXT);
    }
    if (!hChild[2]) return;

    GetWindowRect(hChild[2], &m_rc);
    const int cx = m_rc.right - m_rc.left;
    const int cy = m_rc.bottom - m_rc.top;
    ScreenToClient(g_eWnd->GetEWindow(), (LPPOINT)&m_rc);
    m_rc.right = m_rc.left + cx;
    m_rc.bottom = m_rc.top + cy;
}
// 背景即将绘画, 需要在这里自己处理绘画
DWORD IDRAW_CALL Event_BeforeDraw(UINT nEvent, LONG_PTR param, LPVOID pArg)
{
    if (!m_isDraw) return 0;    // 不重画, 直接返回, 让支持库处理
    int type = LOWORD(iDraw_GetInfo(g_info, IGI_MDICLIENTTYPE, 0));  // 低位是窗口类型, 高位显示代码类型
    // 1是设计器窗口, 2是起始页, 这两个直接不绘画返回, 也就是这两个页面不显示图片
    if (type == 1 || type == 2 || m_rc.right - m_rc.left <= 0 || !g_showImage)
        return 0;   // 不处理, 返回给支持库处理

    UINT imgWidth, imgHeight;
    LPIRE_STRUCT_BACKGROUND_DRAW evt = (LPIRE_STRUCT_BACKGROUND_DRAW)pArg;

    // 初始化图片
    g_showImage->GetSize(&imgWidth, &imgHeight);

    // 从图片哪个位置画, 这里就不去除底下20像素的水印了
    RECT_F rcImg;
    rcImg.left      = 0;
    rcImg.top       = 0;
    rcImg.right     = (float)imgWidth;
    rcImg.bottom    = (float)imgHeight;

    imgWidth = scale(imgWidth);     // 画出的位置跟随窗口缩放进行缩放
    imgHeight = scale(imgHeight);

    if (m_rc.right - m_rc.left <= 0 || m_rc.bottom - m_rc.top <= 0)
        GetMDIClient3();
    // 画到窗口上的哪个位置
    RECT_F rcDraw;
    rcDraw.right    = (float)(m_rc.right    - 20);          // 画出右边是mdi窗口的底边
    rcDraw.bottom   = (float)(m_rc.bottom   - 20);          // 画出底边是mdi窗口的底边
    rcDraw.left     = (float)(rcDraw.right  - imgWidth);
    rcDraw.top      = (float)(rcDraw.bottom - imgHeight);




    if (rcDraw.top < m_rc.top)    // 画出的位置超过了mdi窗口的顶边, 需要缩放
    {
        // 计算缩放比例, 这里计算得不精准, 数学比较好的可以自己计算一下, 或者使用缩放函数也行
        float scale = ((float)(m_rc.bottom - m_rc.top)) / (float)imgHeight;
        imgWidth    = (UINT)((float)imgWidth  * scale);
        imgHeight   = (UINT)((float)imgHeight * scale);

        rcDraw.left = rcDraw.right  - (float)imgWidth;
        rcDraw.top  = rcDraw.bottom - (float)imgHeight;
    }
    RECT rcMDI4;
    iDraw_IDCall(IDC_IDRAW_GETCODEVISIBLERECT, 0, (LPARAM)&rcMDI4);
    g_dx->_canvas_setClip(evt->hCanvas, (float)rcMDI4.left, (float)rcMDI4.top, (float)rcMDI4.right, (float)rcMDI4.bottom);

    const int alpha = 100;
    // 用法和gdi+差不多
    g_dx->_canvas_drawimagerectrect(evt->hCanvas, g_showImage,
        rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom,
        rcImg.left, rcImg.top, rcImg.right, rcImg.bottom, alpha);
    g_dx->_canvas_resetClip(evt->hCanvas);

    return 1;
}

// 代码切换选择夹选择即将被改变, 这里每次都重画阴影窗口, 背景插件是基于阴影窗口实现的
// 这个插件要实现的效果是设计器窗口和起始页不显示图片, 其他都显示
DWORD IDRAW_CALL Event_CodeTabChang(UINT nEvent, LONG_PTR param, LPVOID pArg)
{
    if (!m_isDraw) return 0;
    iDraw_UpdateShadow(true);   // 重画阴影窗口, 会触发阴影窗口重画事件
    return 0;
}

// mdi窗口尺寸已经被改变, 这里要做的就是取得第三层窗口的位置
DWORD IDRAW_CALL Event_MDIClientSize(UINT nEvent, LONG_PTR param, LPVOID pArg)
{
    LPIRE_STRUCT_MDICLIENT_MOVE_EVENT evt = (LPIRE_STRUCT_MDICLIENT_MOVE_EVENT)pArg;

    RECT* rcMDIClient = (RECT*)iDraw_GetInfo(g_info, IGI_RCMDICLIENT, 0);
    RECT& rc    = m_rc;
    rc.left     = rcMDIClient->left;  // 因为这个位置是相对父窗口的位置, 需要加上MDIClient窗口的左边顶边
    rc.top      = rcMDIClient->top;
    rc.right    = rc.left + (evt->rcChild3.right - evt->rcChild3.left);
    rc.bottom   = rc.top  + (evt->rcChild3.bottom - evt->rcChild3.top);

    iDraw_UpdateShadow(false);   // 重画阴影窗口, 会触发阴影窗口重画事件
    return 0;
}

// 与支持库交互的回调函数
int CALLBACK IDraw_NotifyCallback(UINT nCode, WPARAM wParam, LPARAM lParam)
{
    switch (nCode)
    {
    case 0:
    default:
        break;
    }
    return 0;
}
#include <Commdlg.h>
void Dialog_Open()
{
    LPCWSTR title = L"打开文件";
    LPCWSTR filter = L"所有文件(*.*)\0*.*\0\0";
    WCHAR file[MAX_PATH] = { 0 };
    WCHAR FileTitle[MAX_PATH] = { 0 };
    OPENFILENAMEW ofn = { 0 };
    ofn.lStructSize         = sizeof(ofn);
    ofn.hwndOwner           = GetActiveWindow();
    ofn.hInstance           = 0;
    ofn.lpstrFilter         = filter;
    ofn.lpstrCustomFilter   = 0;
    ofn.nMaxCustFilter      = 0;
    ofn.nFilterIndex        = 0;
    ofn.lpstrFile           = file;
    ofn.nMaxFile            = MAX_PATH;
    ofn.lpstrFileTitle      = FileTitle;
    ofn.nMaxFileTitle       = MAX_PATH;
    ofn.lpstrInitialDir     = 0;
    ofn.lpstrTitle          = title;
    ofn.Flags               = OFN_EXPLORER;    // OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    GetOpenFileNameW(&ofn);
}
// 扩展菜单被选择, 一个是菜单ID, 一个是添加到菜单时的项目数值
int CALLBACK IDraw_MenuCommand(int id, LPARAM param)
{
    switch (id)
    {
    case 1000:      // 设置
    {
        Setting_Open();
        break;
    }
    case 1001:      // 启用/禁用按钮
    {
        m_isDraw = !m_isDraw;
        MENUITEMINFOW item = { 0 };
        item.cbSize = sizeof(item);
        item.fMask = MIIM_STATE;
        item.fState = m_isDraw ? MFS_CHECKED : MFS_UNCHECKED;   // 改变选中状态
        SetMenuItemInfoW((HMENU)DLL_GUID, 1001, false, &item);
        iDraw_UpdateShadow(false);   // 重画阴影窗口
        ini_write(INI_ENABLED, m_isDraw);
        break;
    }
    case 1002:
        Dialog_Open();
        break;
    default:
        break;
    }

    return 0;
}

DWORD CALLBACK EIDE_AutoHide(UINT nEvent, LONG_PTR param, LPVOID pArg)
{
    if (!m_isDraw) return 0;
    bool bAsync = true;
    if (param == 2)         // 尺寸被改变了
    {
        bAsync = false;
    }
    else if (param == 1)    // 被显示了
    {

    }

    // 被隐藏了
    iDraw_UpdateShadow(bAsync);

    return 0;
}

// 初始化程序配置
inline void InitConfig()
{
    m_isDraw = ini_read_int(INI_ENABLED) != 0;
    LPCWSTR dataPath = (LPCWSTR)iDraw_GetInfo(g_info, IGI_PATH_IDRAW, 0);
    m_imagePath = dataPath;
    m_imagePath += L"plugin\\";

    std::wstring path = ini_read(INI_IMAGE_PATH);
    if (path == L"")
        path = L"BackGroundImagePath";

    m_imagePath += path;
    if (m_imagePath.back() != '\\')
        m_imagePath.push_back('\\');

    if (!PathIsDirectoryW(m_imagePath.c_str()))
    {
        CreateDirectoryW(m_imagePath.c_str(), 0);
    }

}

extern "C" _declspec(dllexport) void* WINAPI IDraw_Interface(LPIDRAW_INFO pInfo, IGetEWindow* eWnd, int reserved1, int reserved2)
{
    // 需要在这里初始化自己的程序
    g_info = pInfo;
    g_eWnd = eWnd;
    InitConfig();
    iDraw_Event_RegisterEvent(DLL_GUID, IRE_BCAKGROUND_DRAW, 0, Event_BeforeDraw, 0, 0);        // 注册背景重画事件
    iDraw_Event_RegisterEvent(DLL_GUID, IRE_TAB_CODE_SELCHANGED, 0, 0, 0, Event_CodeTabChang);  // 注册代码切换选择夹选中被改变
    iDraw_Event_RegisterEvent(DLL_GUID, IRE_TAB_MDICLIENT_SIZE, 0, 0, 0, Event_MDIClientSize);  // mdi尺寸被改变, 这个是注册处理后

    iDraw_Event_RegisterEvent(DLL_GUID, IRE_DRAW_AUTOHIDE_HIDE, 0, 0, 0, EIDE_AutoHide);        // 自动隐藏的窗口被隐藏了
    iDraw_Event_RegisterEvent(DLL_GUID, IRE_DRAW_AUTOHIDE_SHOW, 0, 0, 1, EIDE_AutoHide);        // 自动隐藏的窗口被显示了
    iDraw_Event_RegisterEvent(DLL_GUID, IRE_DRAW_AUTOHIDE_MOVE, 0, 0, 2, EIDE_AutoHide);        // 自动隐藏的窗口被移动

    g_dx = iDraw_GetDxObject();
    static PLUGIN_INFO pluginInfo = { 0 };
    
    pluginInfo.cbSize           = sizeof(pluginInfo);
    pluginInfo.name             = "视觉库背景插件";
    pluginInfo.remarks          = "可选插件, 这个插件主要负责绘画背景";
    pluginInfo.version          = "1.0";
    pluginInfo.author           = "项目部006";
    pluginInfo.Versionlimit     = 0;
    pluginInfo.Dependence       = 0;


    pluginInfo.guid                 = DLL_GUID;
    pluginInfo.pfnMenuExt_Command   = IDraw_MenuCommand;
    pluginInfo.pfnCallback          = IDraw_NotifyCallback;
    iDraw_InitPlugin(pluginInfo.guid, g_hInst, &pluginInfo, IDraw_MenuCommand, IDraw_NotifyCallback); // 初始化插件, 这里需要加入扩展菜单

    UINT uFlags = MF_STRING | MF_BYPOSITION;
    iDraw_MenuExt_Insert(DLL_GUID, -1, uFlags, 1000, L"设置", (LPARAM)0);
    iDraw_MenuExt_Insert(DLL_GUID, -1, uFlags | MF_SEPARATOR, -1, 0, 0);
    uFlags |= (m_isDraw ? MFS_CHECKED : MFS_UNCHECKED);
    iDraw_MenuExt_Insert(DLL_GUID, -1, uFlags, 1001, L"启用/禁用", 0);
    iDraw_MenuExt_Insert(DLL_GUID, -1, uFlags, 1002, L"调用测试", 0);


    iDraw_UpdateShadow(true);
    return &pluginInfo;
}

extern "C" __declspec(dllexport) void CALLBACK IDraw_UnInterface()
{

    g_showImage = 0;

}


