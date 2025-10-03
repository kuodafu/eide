// dllmain.cpp : 定义 DLL 应用程序的入口点。
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
#include "../iDraw.h"
#include "resource.h"


#if defined(DEBUG) || defined(_DEBUG)
#include <assert.h>
#else
  #ifndef assert
    #define assert(_s) 
  #endif
#endif
#define DLL_GUID "{408DB622-122B-4D5D-BDFA-4BBC0483498C}"

LPIDRAW_INFO g_info;
IGetEWindow* g_eWnd;
ID2DDraw* g_dx;
LPEX_IMAGE g_image[8];      // 内置8张背景
LPEX_IMAGE g_showImage;     // 当前显示的图片
LPEX_IMAGE g_hImgMenuSel;   // 菜单打勾的图片
HINSTANCE hInst;
RECT m_rc;
IDRAW_HRESOURCE g_hResource;    // 工具条资源句柄
bool m_isDraw;                  // 是否绘画, 在启用/禁用菜单里赋值
int m_index;                    // 当前是选中哪个图片
static bool m_isCheck_Setting;


#define MENU_ITEMHEIGHT     60  // 菜单项高度, 预览的图片根据这个高度绘画位置
#define INI_ENABLED         L"enabled"  // 是否启用绘画
#define INI_INDEX           L"index"    // 当前选中图片索引

#define INI_APPNAME         L"BackgroundImage"  // 配置节名

template<typename T>
inline T scale(T n)
{
    int dpi = iDraw_GetInfo(g_info, IGI_DPI, 0);
    float scale = (float)dpi / 96.f;
    return (T)((float)n * scale);
}

int ini_read(LPCWSTR lpszKeyName, int nDefValue = 0)
{
    LPCWSTR szConfigFileName = (LPCWSTR)iDraw_GetInfo(g_info, IGI_PATH_CONFIG, 0);

    wchar_t szDefValue[50];
    _itow_s(nDefValue, szDefValue, 50, 10);

    DWORD bufferSize = (DWORD)260;
    LPWSTR buffer = new wchar_t[bufferSize];

    DWORD len = GetPrivateProfileStringW(INI_APPNAME, lpszKeyName, szDefValue, buffer, bufferSize, szConfigFileName);
    if (GetLastError() == ERROR_FILE_NOT_FOUND) // 没有创建这个配置项, 需要写出
    {
        WritePrivateProfileStringW(INI_APPNAME, lpszKeyName, szDefValue, szConfigFileName);
    }
    else
    {
        // 缓冲区不够大, 继续分配更大的内存
        while (len == bufferSize - 1 || len == bufferSize - 2)
        {
            bufferSize *= 2;
            delete[] buffer;
            buffer = new wchar_t[bufferSize];
            len = GetPrivateProfileStringW(INI_APPNAME, lpszKeyName, szDefValue, buffer, bufferSize, szConfigFileName);
        }
    }
    int nReadValue = _wtol(buffer);
    delete[] buffer;
    return nReadValue;
}
int ini_write(LPCWSTR lpszKeyName, int value)
{
    LPCWSTR szConfigFileName = (LPCWSTR)iDraw_GetInfo(g_info, IGI_PATH_CONFIG, 0);
    wchar_t str[50];
    _itow_s(value, str, 50, 10);
    return WritePrivateProfileStringW(INI_APPNAME, lpszKeyName, str, szConfigFileName);
}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        hInst = hModule;
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}



// 从资源ID加载图片
inline LPEX_IMAGE _img_createfromresource(int id)
{
    HRSRC hRsrc = ::FindResourceW(hInst, MAKEINTRESOURCEW(id), L"PNG");
    assert(hRsrc && "获取资源出错, 调试版中断下来查看");
    if (!hRsrc) return 0;

    DWORD size = SizeofResource(hInst, hRsrc);  // 获取资源长度
    HGLOBAL hMem = LoadResource(hInst, hRsrc);  // 将指定的资源加载到变量
    LPVOID pImageData = 0;
    if (hMem)
        pImageData = LockResource(hMem);
    LPEX_IMAGE hImg = g_dx->_img_create_frommemory(pImageData, size);
    assert(hImg && "创建图片出错, 调试版中断下来查看");
    return hImg;
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
    case INE_MENU_CALCMENU:
    {
        IRE_STRUCT_MENU_CALCSIZE* arg = (IRE_STRUCT_MENU_CALCSIZE*)lParam;
        // 这里可以修改菜单的宽高
        if (wParam >= 0 && wParam < 8)
        {
            arg->height = scale(MENU_ITEMHEIGHT);  // 菜单高度
            arg->width += scale(MENU_ITEMHEIGHT);
        }
        break;
    }
    case INE_MENU_DRAWMENU_BEFORE:
    {
        // 不是几个预览的菜单项就让视觉库绘制
        if (wParam < 0 || wParam > 7)
            return 0;
        

        IRE_STRUCT_MENU_DRAW* draw = (IRE_STRUCT_MENU_DRAW*)lParam;
        LPEX_BRUSH hbrBack, hbrText;
        // 根据状态从配置里获取背景画刷/文本画刷
        
        if ((draw->state & STATE::HOT))
        {
            hbrBack = (LPEX_BRUSH)iDraw_GetInfo(g_info, IGI_CONFIG_CRMENUHOT, IGICR_BRUSHDX);
            hbrText = (LPEX_BRUSH)iDraw_GetInfo(g_info, IGI_CONFIG_CRMENUTEXTHOT, IGICR_BRUSHDX);
        }
        else if ((draw->state & STATE::BAN))
        {
            hbrBack = (LPEX_BRUSH)iDraw_GetInfo(g_info, IGI_CONFIG_CRMENUBAN, IGICR_BRUSHDX);
            hbrText = (LPEX_BRUSH)iDraw_GetInfo(g_info, IGI_CONFIG_CRMENUTEXTBAN, IGICR_BRUSHDX);
        }
        else
        {
            hbrBack = (LPEX_BRUSH)iDraw_GetInfo(g_info, IGI_CONFIG_CRMENUBACK, IGICR_BRUSHDX);
            hbrText = (LPEX_BRUSH)iDraw_GetInfo(g_info, IGI_CONFIG_CRMENUTEXT, IGICR_BRUSHDX);
        }

        RECT_F rcItem;
        rcItem.left    = 0;
        rcItem.top     = 0;
        rcItem.right   = (float)draw->width;
        rcItem.bottom  = (float)draw->height;
        g_dx->_canvas_fillRectangle(draw->hCanvas, hbrBack, &rcItem);

        RECT_F rcImage = { 0, 5, rcItem.bottom, rcItem.bottom - 5 };


        LPEX_IMAGE hImg = g_image[wParam];
        UINT cxIcon, cyIcon;
        g_dx->_img_getsize(hImg, &cxIcon, &cyIcon);

        float width = (((rcImage.right - rcImage.left) / (float)cyIcon) * (float)cxIcon);   // 按比例缩放图片
        if (0)
        {
            // 这里是靠左画出图片
            rcImage.right = rcImage.left + width;
        }
        else
        {
            // 这里是居中画出图片
            rcImage.left = ((float)draw->width - width) / 2.f;
            rcImage.right = rcImage.left + width;
        }
        
        // 菜单项做个预览效果, 按比例缩放
        g_dx->_canvas_drawimagerect(draw->hCanvas, hImg, rcImage.left, rcImage.top, rcImage.right, rcImage.bottom, m_isDraw ? 255 : 128);

        if (m_index == wParam)
        {
            // 当前是选中项
            RECT_F rcMenu;
            const float height = scale(16.f);
            rcMenu.left     = 5;
            rcMenu.top      = ((float)draw->height - height) / 2;
            rcMenu.right    = rcMenu.left + height;
            rcMenu.bottom   = rcMenu.top + height;
            draw->hCanvas->DrawImage(g_hImgMenuSel, rcMenu.left, rcMenu.top, rcMenu.right, rcMenu.bottom, 255);
        }

        return 1;   // 不要文本, 把图片居中看起来更舒服
        LPEX_FONT hFont = (LPEX_FONT)iDraw_GetInfo(g_info, IGI_DEFDXFONT, 0);

        size_t len = wcslen(draw->text);
        const wchar_t* pos = wcschr(draw->text, '\t');
        if (pos) len = pos++ - draw->text;   // 有快捷键
        
        RECT_F rcText   = rcItem;
        rcText.left     = rcImage.right + 10;   // 文本画出位置是图片的后面, 左边位置不固定, 右边快捷键固定
        rcText.left     = 100;   // 感觉画出位置固定会更好看一些
        g_dx->_canvas_drawtext(draw->hCanvas, hFont, hbrText, draw->text, len, DT_VCENTER | DT_SINGLELINE, &rcText);

        if (pos)    // tab键后面有文本, 向右对齐画出
            g_dx->_canvas_drawtext(draw->hCanvas, hFont, hbrText, pos, -1, DT_VCENTER | DT_SINGLELINE | DT_RIGHT, &rcText);
        
        return 1;   // 拦截, 不继续往后绘画
    }
    case INE_MENU_GETICON:
    {
        LPIRE_STRUCT_GETICON icon = (LPIRE_STRUCT_GETICON)lParam;
        if (wParam == 1000) // 设置按钮
        {
            // 资源里没有什么好的图标, 就使用运行和停止按钮吧, 启用状态就使用运行按钮, 停止状态就使用停止按钮
            const LPCWSTR id = m_isCheck_Setting ? L"32854" : L"32860";
            return (int)iDraw_GetIcon(g_hResource, id, icon->prc, icon->state, 96);
        }
        break;
    }
    default:
        break;
    }
    return 0;
}
// 扩展菜单被选择, 一个是菜单ID, 一个是添加到菜单时的项目数值
int CALLBACK IDraw_MenuCommand(int id, LPARAM param)
{
    if (id >= 0 && id < 8)  // 0-8 是几个切换图片的菜单
    {
        MENUITEMINFOW item = { 0 };
        item.cbSize = sizeof(item);
        item.fMask = MIIM_STATE;
        item.fState = MFS_UNCHECKED;        // 把上一次选中的菜单取消
        SetMenuItemInfoW((HMENU)DLL_GUID, m_index, false, &item);

        g_showImage = (LPEX_IMAGE)param;    // 项目数值里存放的是图片指针, 直接强转过来就行
        iDraw_UpdateShadow(false);           // 重画阴影窗口
        m_index = id;
        ini_write(INI_INDEX, m_index);

        // 其实不设置也没关系, 因为已经拦截了绘画事件, 选中的打勾也是由自己画
        item.fState = MFS_CHECKED;          // 设置当前选中的图片
        SetMenuItemInfoW((HMENU)DLL_GUID, m_index, false, &item);

        return 0;
    }
    switch (id)
    {
    case 1000:      // 设置
    {
        wchar_t text[100];
        swprintf_s(text, 100, L"背景插件设置菜单被选中, 菜单id = %d, 菜单数值 = %d\n\n设置功能没做, 就做个切换显示图标的功能吧", id, param);
        MessageBoxW(0, text, L"背景插件菜单被选中", 0);
        m_isCheck_Setting = !m_isCheck_Setting;
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
    default:
    {
        wchar_t text[100];
        swprintf_s(text, 100, L"背景插件设置菜单被选中, 菜单id = %d, 菜单数值 = %d\n", id, param);
        MessageBoxW(0, text, L"背景插件菜单被选中", 0);
        break;
    }
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
    m_index  = ini_read(INI_INDEX);
    m_isDraw = ini_read(INI_ENABLED) != 0;
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
    g_hResource = iDraw_GetDefControlIcon(eWnd->GetMenuStandard());     // 工具条按钮比较丰富, 就选工具条了

    g_image[0] = _img_createfromresource(IDPNG_BACK0);
    g_image[1] = _img_createfromresource(IDPNG_BACK1);
    g_image[2] = _img_createfromresource(IDPNG_BACK2);
    g_image[3] = _img_createfromresource(IDPNG_BACK3);
    g_image[4] = _img_createfromresource(IDPNG_BACK4);
    g_image[5] = _img_createfromresource(IDPNG_BACK5);
    g_image[6] = _img_createfromresource(IDPNG_BACK6);
    g_image[7] = _img_createfromresource(IDPNG_BACK7);
    g_hImgMenuSel = _img_createfromresource(IDPNG_MENU);
    g_showImage = g_image[m_index];
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
    iDraw_InitPlugin(pluginInfo.guid, hInst, &pluginInfo, IDraw_MenuCommand, IDraw_NotifyCallback); // 初始化插件, 这里需要加入扩展菜单

    UINT uFlags = MF_STRING | MF_BYPOSITION | MF_OWNERDRAW;
    iDraw_MenuExt_Insert(DLL_GUID, -1, uFlags, 0, 0, (LPARAM)g_image[0]);
    iDraw_MenuExt_Insert(DLL_GUID, -1, uFlags, 1, 0, (LPARAM)g_image[1]);
    iDraw_MenuExt_Insert(DLL_GUID, -1, uFlags, 2, 0, (LPARAM)g_image[2]);
    iDraw_MenuExt_Insert(DLL_GUID, -1, uFlags, 3, 0, (LPARAM)g_image[3]);
    iDraw_MenuExt_Insert(DLL_GUID, -1, uFlags, 4, 0, (LPARAM)g_image[4]);
    iDraw_MenuExt_Insert(DLL_GUID, -1, uFlags, 5, 0, (LPARAM)g_image[5]);
    iDraw_MenuExt_Insert(DLL_GUID, -1, uFlags, 6, 0, (LPARAM)g_image[6]);
    //iDraw_MenuExt_Insert(DLL_GUID, -1, uFlags, 7, 0, (LPARAM)g_image[7]);


    iDraw_MenuExt_Insert(DLL_GUID, -1, uFlags | MF_SEPARATOR, -1, 0, 0);
    iDraw_MenuExt_Insert(DLL_GUID, -1, uFlags, 1000, L"设置", 0);
    


    MENUITEMINFOW item = { 0 };
    item.cbSize = sizeof(item);

    item.fMask = MIIM_ID | MIIM_STATE;
    item.fState = m_isDraw ? MFS_CHECKED : MFS_UNCHECKED;
    item.wID = 1001;
    item.dwTypeData = (LPWSTR)L"启用/禁用";
    // 插入一个启用/禁用的菜单项, 默认是选中状态
    InsertMenuItemW((HMENU)DLL_GUID, -1, true, &item);   // 插入到设置按钮的前面


    item.fMask = MIIM_STATE;
    item.fState = MFS_CHECKED;  // 设置当前选中的图片
    SetMenuItemInfoW((HMENU)DLL_GUID, m_index, false, &item);

    iDraw_UpdateShadow(true);
    return &pluginInfo;
}

extern "C" __declspec(dllexport) void CALLBACK IDraw_UnInterface()
{

    iDraw_UnRegister_Message(DLL_GUID); // 这个是取消插件的所有已注册的消息回调
    iDraw_UnRegister_Event(DLL_GUID);   // 这个是取消插件所有已经注册的事件回调

    for (int i = 0; i < sizeof(g_image) / sizeof(g_image[0]); i++)
        g_dx->_img_destroy(g_image[i]); // 自己创建自己释放
    g_dx->_img_destroy(g_hImgMenuSel); // 自己创建自己释放
    g_showImage = 0;

}
