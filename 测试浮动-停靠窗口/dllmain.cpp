// 测试浮动-停靠窗口.cpp : 定义应用程序的入口点。
//

#include <control/WndControl6_0.h>
#include "pch.h"
#include <assist/CFileRW.h>
#include <d2d/d2d.h>

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

CTinyXml2 xml;
std::map<int, MAP_INFO> m_map;
HANDLE m_hMapFile;
// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void OnPaint(HWND hWnd, PAINTSTRUCT& ps, int cxClient, int cyClient);
void __set_theme();
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MY, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    static IDRAW_INFO a = { 0 };
    s_info = &a;
    s_info->isDxRender = 1;
    s_info->isGpRender = !s_info->isDxRender;
    _canvas_init(s_info->isDxRender);
    m_hMapFile = OpenFileMappingW(FILE_MAP_ALL_ACCESS, 0, L"{AF437352-4509-44EA-8B1A-4FA7762A13FD}");
    if (!m_hMapFile)
        m_hMapFile = CreateFileMappingW(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, 4000, L"{AF437352-4509-44EA-8B1A-4FA7762A13FD}");

    CFileRW file;
    file.open(R"(D:\cpp\SVN工作目录\易语言界面\测试浮动-停靠窗口\测试停靠.xml)", 1, -1);
    int size = file.size();
    _str str(size);
    file.read((LPBYTE)str.data(), size);

    if (!xml.Parse(str.c_str()))
        return 0;
    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }


    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    if (m_hMapFile)
        CloseHandle(m_hMapFile);
    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 16;
    wcex.cbWndExtra     = 16;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = 0;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst  = hInstance; // 将实例句柄存储在全局变量中
    if (GetAttrValue("@1", "maximize"))
        nCmdShow = SW_MAXIMIZE;
    HWND hWnd = CreateWindowExW(0, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, 100, 100, 300, 200, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}


//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        HWND* p = (HWND*)MapViewOfFile(m_hMapFile, FILE_MAP_WRITE, 0, 0, 0);
        if (p)
        {
            p[1] = hWnd;
            UnmapViewOfFile(p);
        }
        break;

        int cxBorder = GetSystemMetrics(SM_CXBORDER);
        int cxBorder1 = GetSystemMetrics(SM_CXFOCUSBORDER);
        int cxBorder2 = GetSystemMetrics(SM_CXPADDEDBORDER);
        int cxBorder3 = GetSystemMetrics(SM_CYBORDER);
        int cxBorder4 = GetSystemMetrics(SM_CYFOCUSBORDER);
        int x = GetAttrValue("@1", "x");
        int y = GetAttrValue("@1", "y");
        int cx = GetAttrValue("@1", "width");
        int cy = GetAttrValue("@1", "height");
        RECT rcBorder;
        CWndBase::GetRect(hWnd, &rcBorder);
        MoveWindow(hWnd, x, y, cx + rcBorder.left + rcBorder.right, cy + rcBorder.top + rcBorder.bottom, false);
        const LPCWSTR name[] =
        {
            L"易支持库", L"项目结构", L"组件属性",
            L"模块管理", L"资源视图", L"系统组件",
            L"帮助提示", L"调试输出", L"变量监视",
            L"断点堆栈", L"搜寻1", L"搜寻2", L"反汇编", L"代码区"
        };
        srand((UINT)time(0));
        for (int i = 0; i < sizeof(name) / sizeof(name[0]); i++)
        {

            DWORD style = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
            int left = i * 100 + (i + 1) * 10;
            int id = 10001 + i;
            HWND hWndChild = CreateWindowExW(WS_EX_TRANSPARENT, L"button", name[i], style, left, 50, 100, 50, hWnd, (HMENU)(id), 0, 0);
            MAP_INFO& data = m_map[id];
            data.hWnd = hWndChild;
            data.name = name[i];
            data.crBack = RGB(rand(0, 255), rand(0, 255), rand(0, 255));
            data.hbrBack = CreateSolidBrush(data.crBack);
            SetWindowLongPtrW(hWndChild, GWLP_USERDATA, (LONG_PTR)&data);
            SendMessageW(hWndChild, WM_SETFONT, (WPARAM)CWndBase::DefaultFont(), 0);
        }

        wnd_create(hWnd);
        __set_theme();
        break;
    }
    case WM_SIZE:
        //wnd_create(hWnd);
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 分析菜单选择:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBoxParamW(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About, 0);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProcW(hWnd, message, wParam, lParam);
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc;
        static HBRUSH hbr = CreateSolidBrush(RGB(238,238,242));
        GetClientRect(hWnd, &rc);
        FillRect(hdc, &rc, hbr);
        OnPaint(hWnd, ps, rc.right - rc.left, rc.bottom - rc.top);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_CTLCOLORSTATIC:
    {
        MAP_INFO* map = (MAP_INFO*)GetWindowLongPtrW((HWND)lParam, GWLP_USERDATA);
        return (LRESULT)map->hbrBack;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_NOTIFY:
    {
        LPNMHDR hdr = (LPNMHDR)lParam;
        switch (hdr->code)
        {
        //case TCN_SELCHANGING:
        case TCN_SELCHANGE:
        {
            LPTABCHANGE_INFO info = (LPTABCHANGE_INFO)lParam;
            
            TCITEMW item = { 0 };
            item.mask = TCIF_PARAM;
            //if (info->pItem->lParam > 0)
            //{
            //    MAP_INFO& map = m_map[info->pItem->lParam];
            //    ShowWindow(map.hWnd, SW_SHOW);
            //}
            //if (info->index != info->indexOld && info->pItemOld->lParam > 0)
            //{
            //    MAP_INFO& map = m_map[info->pItemOld->lParam];
            //    ShowWindow(map.hWnd, SW_HIDE);
            //}
            break;
        }
        default:
            break;
        }
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    case WM_ERASEBKGND: return 1;
    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


void OnPaint(HWND hWnd, PAINTSTRUCT& ps, int cxClient, int cyClient)
{
    HWND hDbg = 0;
    if (m_hMapFile)
    {
        HWND* p = (HWND*)MapViewOfFile(m_hMapFile, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
        if (p)
        {
            hDbg = p[0];
            p[1] = hWnd;
            UnmapViewOfFile(p);
        }
    }
    float x = 0;
    float y = 0;
    float z = 0;
    bool isDxRender = false;
    if (hDbg)
    {
        x = (float)SendMessageW(hDbg, 32885, 1, 0);
        y = (float)SendMessageW(hDbg, 32885, 2, 0);
        z = (float)SendMessageW(hDbg, 32885, 3, 0);
        isDxRender = SendMessageW(hDbg, 32885, 4, 0);
    }
    if (!isDxRender)
    {
        Gdiplus::Font hFont(ps.hdc, CWndBase::DefaultFont());
        Gdiplus::SolidBrush hbrBack(RGB2ARGB(255, 255));
        Gdiplus::SolidBrush hbrBack1(MAKEARGB(255, 128, 255, 128));
        Gdiplus::SolidBrush hbrBack2(MAKEARGB(255, 0, 0, 255));
        Gdiplus::SolidBrush hbrText(RGB2ARGB(0, 255));
        Gdiplus::Pen hPen(MAKEARGB(255, 0, 0, 255));
        Gdiplus::Graphics gp(ps.hdc);
        gp.TranslateTransform(x, y);
        gp.RotateTransform(z);

        Gdiplus::RectF rcText(0, 0, 80, 20);
        gp.FillRectangle(&hbrBack, rcText);
        gp.DrawString(L"Gdiplus", 7, &hFont, rcText, 0, &hbrText);
        gp.DrawLine(&hPen, 40, 0, 40, 20);
        gp.DrawRectangle(&hPen, rcText);

        gp.DrawLine(&hPen, 0, 0, (int)x, (int)y);

        gp.ResetTransform();

        //gp.FillRectangle(&hbrBack1, rcText);
        //gp.DrawString(L"测试文本", 4, &hFont, rcText, 0, &hbrText);
        return;
    }
    static LPCANVAS hCanvas = 0;
    static LPEX_FONT hFont = 0;
    static LPEX_BRUSH hbrBack = 0;
    static LPEX_BRUSH hbrBack1 = 0;
    static LPEX_BRUSH hbrBack2 = 0;
    static LPEX_BRUSH hbrText = 0;
    static LPEX_PEN hPen = 0;

    if (!hCanvas)
    {
        hCanvas = _canvas_create(0, 0, 0);
        LOGFONTW lf;
        GetObjectW(CWndBase::DefaultFont(), sizeof(lf), &lf);
        hFont = _font_create_logfont(&lf, 0);
        hbrBack = _brush_create(RGB2ARGB(255, 255));
        hbrBack1 = _brush_create(MAKEARGB(255, 128, 255, 128));
        hbrBack2 = _brush_create(MAKEARGB(255, 0, 0, 255));
        hbrText = _brush_create(RGB2ARGB(0, 255));
        hPen = _pen_create(MAKEARGB(255, 0, 0, 255));
    }
    if (hCanvas->width < (UINT)cxClient || hCanvas->height < (UINT)cyClient)
        hCanvas->Resize(cxClient, cyClient);
    hCanvas->BeginDraw();
    hCanvas->clear(MAKEARGB(255, 233, 233, 233));
    hCanvas->SetAntialiasMode(0, 0);

    ID2D1DeviceContext* dx = hCanvas->pRenderTarget;


    D2D1::Matrix3x2F cls;

    D2D1_POINT_2F ptCenter = { x, y };
    D2D1::Matrix3x2F asd = D2D1::Matrix3x2F::Rotation(z, ptCenter);

    dx->SetTransform(asd);

    //hCanvas->translate(x, y);
    ////_canvas_rotate(hCanvas, 40, 10, 90);
    //hCanvas->rotate(0, 0, z);
    //hCanvas->translate(0, 0);

    const int fmt = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
    RECT_F rcText(0, 0, 80, 20);
    hCanvas->FillRectangle(hbrBack, &rcText);
    hCanvas->drawtext(hFont, hbrText, L"Direct2D", 8, fmt, &rcText);
    LINEPOINT_F pt(40, 0, 40, 20);
    hCanvas->DrawLine(hPen, &pt);
    hCanvas->DrawRectangle(hPen, &rcText);

    pt(0, 0, x, y);
    hCanvas->DrawLine(hPen, &pt);

    //hCanvas->rotate(0, 0, 0);
    //hCanvas->FillRectangle(hbrBack1, &rcText);
    //hCanvas->drawtext(hFont, hbrText, L"测试文本", 4, fmt, &rcText);

    RECT_F rcLine(0, 0, 80, 10);
    hCanvas->FillRectangle(hbrBack1, &rcLine);

    HDC hdc = hCanvas->GetDC();
    BitBlt(ps.hdc, 0, 0, cxClient, cyClient, hdc, 0, 0, SRCCOPY);
    hCanvas->ReleaseDC();
    hCanvas->EndDraw();
}



void __set_theme()
{
    static CPrivateProfile s_ini;
    static CPrivateProfile s_iniColor;
    s_info->ini = &s_ini;
    s_info->iniColor = &s_iniColor;

    s_info->path_e = LR"(E:\易语言正式版\)";
    s_info->path_run = LR"(E:\易语言正式版\lib\)";
    s_info->path_lib = LR"(E:\易语言正式版\lib\)";
    s_info->path_file = LR"(E:\易语言正式版\lib\)";
    s_info->path_data = LR"(E:\易语言正式版\lib\iDraw\data\)";
    s_info->path_iDraw = LR"(E:\易语言正式版\lib\iDraw\)";
    s_info->path_plugin = LR"(E:\易语言正式版\lib\iDraw\plugin\)";
    s_info->path_config = LR"(E:\易语言正式版\lib\iDraw\data\config.ini)";

    typedef void* (__stdcall* pfn_init)(IDRAW_INFO * info, IGetEWindow* eWnd, int reserved1, int reserved2);
    HMODULE h = LoadLibraryW(LR"(E:\易语言正式版\lib\iDraw\iResource.dll)"); if (!h)throw;
    pfn_init pfn = (pfn_init)GetProcAddress(h, "IDraw_Interface"); if (!pfn)throw;
    pfn(s_info, 0, 0, 0);

    h = LoadLibraryW(LR"(E:\易语言正式版\lib\iDraw\iConfig.dll)"); if (!h)throw;
    pfn = (pfn_init)GetProcAddress(h, "IDraw_Interface"); if (!pfn)throw;
    pfn(s_info, 0, 0, 0);

}