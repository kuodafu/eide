// ImageListView.cpp : 定义应用程序的入口点。
//
#include <control/WndControl6_0.h>
#include <control/CListView.h>
#include <control/WndObjStruct.h>
#include "ImageListView.h"
#include "ExtView.h"
#include <control/CLayered.h>
//#include "../include/iDraw.h"



WINDOW_DRAW_COLOR g_color;


HWND g_hWnd;
CMenu s_menu;
CMenu s_menuImage;
CMenu s_menuFile;
CButton g_radioDPI[5];
int g_dpiIndex;
CCustomList s_listProject;      // 左边工程列表框
CCustomList s_listItem;         // 中间项目列表框
CCustomList s_listImage;        // 显示图片资源的列表
wstr s_path;                    // 文件列表的路径
std::vector<PROJECT_DATA>  s_data;          // 工程列表, 记录当前新建的所有工程
std::vector<SHOWIMAGEDATA> s_dataImage;     // 显示素材的图片

HBRUSH s_hbrBack;
CFileDialog_Com s_fileDlg;

CTinyXml2W s_xml_info;       // 程序配置信息
wstr m_pathname;

PROJECT_DATA* m_data;

CTinyXml2W g_xmlProject;     // 工程xml, 增删工程都从这里修改



#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE g_hInst;                              // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
WCHAR g_runPath[MAX_PATH];                      // 程序运行目录, 后面带 "\"
WCHAR g_runFile[MAX_PATH];                      // 程序完整路径

// 此代码模块中包含的函数的前向声明:
BOOL                InitInstance(int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    g_hInst = hInstance;
    _canvas_init(true, false);

    
    GetModuleFileNameExW(GetCurrentProcess(), 0, g_runPath, MAX_PATH);
    memcpy(g_runFile, g_runPath, MAX_PATH * sizeof(wchar_t));
    wchar_t* p = wcsrchr(g_runPath, '\\');
    if (p)*(p + 1) = '\0';
    wstr iniPath(260);
    iniPath.assign(g_runPath).append(L"data\\config.xml");
    if (!s_xml_info.LoadFile(iniPath.c_str()))
    {
        s_xml_info.Create(L"Config");
    }

    // 初始化全局字符串
    LoadStringW(g_hInst, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(g_hInst, IDC_IMAGELISTVIEW, szWindowClass, MAX_LOADSTRING);



    // 执行应用程序初始化:
    if (!InitInstance(nCmdShow))
    {
        return FALSE;
    }


    HACCEL hAccelTable = LoadAcceleratorsW(g_hInst, MAKEINTRESOURCEW(IDC_IMAGELISTVIEW));

    MSG msg;

    // 主消息循环:
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        if (!TranslateAcceleratorW(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    DeleteObject(s_hbrBack);
    return (int)0;
}


BOOL InitInstance(int nCmdShow)
{
    WNDCLASSEXW wcex;
    wcex.cbSize         = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = g_hInst;
    wcex.hIcon          = LoadIconW(g_hInst, MAKEINTRESOURCEW(IDI_IMAGELISTVIEW));
    wcex.hCursor        = LoadCursorW(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = 0;// MAKEINTRESOURCEW(IDC_IMAGELISTVIEW);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIconW(wcex.hInstance, MAKEINTRESOURCEW(IDI_SMALL));


    const wchar_t* pszSize = s_xml_info.GetAttrValue(L"@1", XML_NAME_RECT);
    RECT rc = { 0 };
    if (pszSize && pszSize[0])
    {
        swscanf_s(pszSize, L"%d,%d,%d,%d", &rc.left, &rc.top, &rc.right, &rc.bottom);
        rc.right = rc.right - rc.left;
        rc.bottom = rc.bottom - rc.top;
    }

    if (rc.left < -50 || rc.top < 0 || rc.right == 0 || rc.bottom == 0)
        rc = { CW_USEDEFAULT, 0, 916, 575 };

    ATOM atom = RegisterClassExW(&wcex);
    HWND hWnd = CreateWindowExW(WS_EX_ACCEPTFILES, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        rc.left, rc.top, rc.right, rc.bottom, nullptr, nullptr, g_hInst, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    atom = 0;
    return TRUE;
}


void CALLBACK CustomList_ItemSelect(HWND hWnd, int index)
{
    if (hWnd == s_listItem) return;
    if (hWnd == s_listImage) return;

    // 左边工程列表被选中
    PROJECT_DATA& projectData = s_data[index];
    std::vector<XMLDATA_STRUCT>& arr = *projectData.arr;
    if (m_data == &projectData) return;
    m_data = &projectData;

    LRESULT ret = 0;
    WndProc_command(hWnd, WM_COMMAND, m_data->themeIndex == 0 ? ID_MENU_BLACK : ID_MENU_WHITE, 0, &ret);

    int len = (int)arr.size();
    s_listItem.SetIndex(-1);
    s_listItem.SetItemHeight(m_data->maxHeight + 10);
    s_listItem.SetItemCount(len, true);

}
INT_PTR CALLBACK FontDialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}

VOID DoPropertySheet(HWND hwndOwner)
{
    PROPSHEETPAGE psp[2];
    
    PROPSHEETHEADER psh;
    
    psp[0].dwSize      = sizeof(PROPSHEETPAGE);
    psp[0].dwFlags     = PSP_USEICONID | PSP_USETITLE;
    psp[0].hInstance   = g_hInst;
    psp[0].pszTemplate = 0;
    psp[0].pszIcon     = 0;
    psp[0].pfnDlgProc  = FontDialogProc;
    psp[0].pszTitle    = L"标题0";
    psp[0].lParam      = 0;
    psp[0].pfnCallback = NULL;
    psp[1].dwSize      = sizeof(PROPSHEETPAGE);
    psp[1].dwFlags     = PSP_USEICONID | PSP_USETITLE;
    psp[1].hInstance   = g_hInst;
    psp[1].pszTemplate = 0;
    psp[1].pszIcon     = 0;
    psp[1].pfnDlgProc  = FontDialogProc;
    psp[1].pszTitle    = L"标题1";
    psp[1].lParam      = 0;
    psp[1].pfnCallback = NULL;
    
    psh.dwSize      = sizeof(PROPSHEETHEADER);
    psh.dwFlags     = PSH_USEICONID | PSH_PROPSHEETPAGE;
    psh.hwndParent  = hwndOwner;
    psh.hInstance   = g_hInst;
    psh.pszIcon     = 0;
    psh.pszCaption  = (LPWSTR) L"Cell Properties";
    psh.nPages      = sizeof(psp) / sizeof(PROPSHEETPAGE);
    psh.nStartPage  = 0;
    psh.ppsp        = (LPCPROPSHEETPAGE) &psp;
    psh.pfnCallback = NULL;
    
    PropertySheet(&psh);
    
    return;
}

//LRESULT CALLBACK WndProc11111(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, void* pData)
//{
//    WNDPROC oldProc = (WNDPROC)iDraw_GetLong(pData, IGL_OLDPROC);
//    return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
//}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    switch (message)
    {
    case WM_ERASEBKGND:
        return 1;
    case WM_DESTROY:
    {
        RECT rc;
        GetWindowRect(hWnd, &rc);
        wchar_t pszSize[100];
        swprintf_s(pszSize, 100, L"%d,%d,%d,%d", rc.left, rc.top, rc.right, rc.bottom);
        s_xml_info.SetAttrValue(L"@1", XML_NAME_RECT, pszSize);
        s_xml_info.SaveFile();
        g_xmlProject.SaveFile();
        assist_destroy(true);
        PostQuitMessage(0);
        break;
    }
    case WM_CREATE:
    {
        g_hWnd = hWnd;
        //{
        //    wstr path(260);
        //    const int cx = 46 * 16;
        //    const int cy = 16;
        //
        //    LPCANVAS hCanvas = _canvas_create(hWnd, cx, cy);
        //    _canvas_begindraw(hCanvas);
        //    LPBRUSHPEN hbr = _brush_create(MAKEARGB(255, 22, 22, 22));
        //    RECT_F rc(0, 0, cx, cy);
        //    for (int i = 0; i < 46; i++)
        //    {
        //        path.assign(LR"(D:\cpp\SVN工作目录\易语言界面\Debug\data\标准工具条热点\新建文件夹\\)").append(i + 1).append(L".png");
        //        LPEX_IMAGE hImg = _img_create(path.c_str());
        //        const float left = (float)i * 16.f;
        //        _canvas_drawimage(hCanvas, hImg, left, 0.f, left + 16.f, 16.f, 0.f, 0.f, 16.f, 16.f, 255);
        //        _img_destroy(hImg);
        //    }
        //    //_canvas_fillRectangle(hCanvas, hbr, &rc);
        //    SIZE_T size = 0;
        //    _canvas_enddraw(hCanvas);
        //    LPBYTE ptr = _img_save(hCanvas, size);
        //    if (!ptr)return 0;
        //    CFileRW f;
        //    f.open(LR"(D:\cpp\SVN工作目录\易语言界面\Debug\data\标准工具条热点\新建文件夹\\tool.png)", 4, -1);
        //    f.write(ptr, size);
        //    free(ptr);
        //    _canvas_destroy(hCanvas);
        //}
            //DoPropertySheet(hWnd);

        if (WndProc_create(hWnd, message, wParam, lParam, &result))
            return result;
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    case WM_SIZE:
    {
        int cxClient = LOWORD(lParam);
        int cyClient = HIWORD(lParam);
        const int top = 20;
        int width = cxClient / 4;
        s_listProject.Move  (width * 0, top, width, cyClient - top);
        s_listItem.Move     (width * 1, top, width * 2, cyClient - top);
        s_listImage.Move    (width * 3, top, width, cyClient - top);
        break;
    }
    case WM_COMMAND:
        if (WndProc_command(hWnd, message, wParam, lParam, &result))
            return result;
        return DefWindowProcW(hWnd, message, wParam, lParam);
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // TODO: 在此处添加使用 hdc 的任何绘图代码...
        EndPaint(hWnd, &ps);
        break;
    }
    //case WM_CTLCOLORMSGBOX:
    //case WM_CTLCOLOREDIT:
    //case WM_CTLCOLORLISTBOX:
    //case WM_CTLCOLORBTN:
    //case WM_CTLCOLORDLG:
    //case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
    {
        const HDC hdc = (HDC)wParam;
        const HWND hControl = (HWND)lParam;
        //SetBkColor(hdc, WINDOW_BACKGROUND);
        //SetTextColor(hdc, RGB(0, 0, 0));
        return (LRESULT)GetStockBrush(WHITE_BRUSH);
    }
    case WM_DROPFILES:
    {
        RECT rc;
        GetWindowRect(hWnd, &rc);
        HWND hExtWindow = ExtView_LoadWindow(g_hInst, false, rc.left + 50, rc.top + 50, rc.right - rc.left, rc.bottom - rc.top);
        SendMessageW(hExtWindow, message, wParam, lParam);
        break;
    }
    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}

inline static void  PixelFix(DWORD crNew, LPBYTE lpBits, int nCount)
{
    BYTE sR = RGB_GETR(crNew);
    BYTE sG = RGB_GETG(crNew);
    BYTE sB = RGB_GETB(crNew);

    DWORD* cr = (DWORD*)lpBits;     // 转成4字节一个像素点来处理
    for (int i = 0; i < nCount; ++i)
    {
        if (cr[i] == 0) continue;   // 等于0 表示是透明, 透明的不处理
        LPBYTE p = (LPBYTE)(&cr[i]);
        p[2] = sR * p[3] / 255;
        p[1] = sG * p[3] / 255;
        p[0] = sB * p[3] / 255;

    }
}




