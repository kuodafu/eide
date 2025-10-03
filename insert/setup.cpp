// insert.cpp : 定义应用程序的入口点。
//
// // 包含 SDKDDKVer.h 可定义可用的最高版本的 Windows 平台。
// 如果希望为之前的 Windows 平台构建应用程序，在包含 SDKDDKVer.h 之前请先包含 WinSDKVer.h 并
// 将 _WIN32_WINNT 宏设置为想要支持的平台。
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容

#include <tstr.h>
// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <control/CButton.h>
#include <control/CheckBox.h>
#include <control/CStatic.h>
#include <control/CEdit.h>
#include <CFileDialogCom.h>
#include <shlwapi.h>
#include <vector>
#include <assist/CRegList.h>
#include <assist/CFileRW.h>
#include <assist/assist.h>
#pragma comment(lib, "Shlwapi.lib")
#include <elib/lib2.h>
#include <control/WndControl6_0.h>
#include <elib/fne_decrypt.h>

#define ID_INSERT   1000
#define ID_SELPATH  1001
#define ID_ADDFNE   1002

// 全局变量:
HINSTANCE g_hInst;                                // 当前实例
HWND      g_hWnd;
wchar_t   g_runPath[260];
wchar_t   g_runFileName[260];
wchar_t   g_runName[260];
wstr g_output;
COLORREF g_crText;
int g_fmt;
wstr g_title;

CMyEdit g_edit;
CButton g_insert;
CButton g_selPath;
CMyStatic g_static;
CMyStatic g_static1;
CCheckBox g_check;
std::vector<wstr> g_src;
std::vector<wstr> g_dest;
const int g_sleep_timer = 30;
bool g_isInsert;    // 是否允许安装, 只要有一个文件的MD5不匹配就设置为真

// 此代码模块中包含的函数的前向声明:
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void btn_insert_click();    // 安装按钮被点击
void btn_selpath_click();   // 选择目录按钮被点击


// 递归枚举当前exe程序目录下的所有文件和文件夹, 把枚举到的文件和新路径记录到数组
// lpszSrcPath = 源路径, 当前exe程序的目录
// lpszDestPath = 目标路径, 会根据枚举到的文件名拼接成完整的路径
// src = 枚举到的所有文件路径
// dest = 应该移动到的路径, 点击安装后需要把枚举到的文件复制到这里
// prc = 重画的区域
void _enum_files(LPCWSTR lpszSrcPath, LPCWSTR lpszDestPath, std::vector<wstr>& src, std::vector<wstr>& dest, RECT* prc);

bool isEPath(LPCWSTR path, wstr* err);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    GetModuleFileNameW(g_hInst, g_runFileName, 260);
#ifdef _DEBUG
    wcscpy_s(g_runFileName, 260, LR"(D:\cpp\SVN工作目录\易语言界面\易IDE视觉库\setup.exe)");
#endif
    wchar_t* pos = wcsrchr(g_runFileName, '\\');
    if (!pos) return 0;
    pos++;
    wcscpy_s(g_runName, 260, pos);
    wcsncpy_s(g_runPath, 260, g_runFileName, pos - g_runFileName);

    wstr path(260);
    path.assign(g_runPath).append(L"lib\\iDraw.fne");
    HMODULE hDraw = LoadLibraryW(path.c_str());
    if (!hDraw)
    {
        wstr text;
        text.Format(L"文件 \"%s\" 不是视觉库的支持库文件\n调用LoadLibraryW加载失败, 错误码 = %d\n\n"
            "请将解压缩安装包后再运行setup.exe, 运行后程序会自动校验支持库版本", path.c_str(), GetLastError());
        MessageBoxW(0, text.c_str(), L"错误", MB_ICONERROR);
        return 1;
    }

    PFN_GET_LIB_INFO pfn = (PFN_GET_LIB_INFO)GetProcAddress(hDraw, "GetNewInf");
    if (!pfn)
    {
        wstr text;
        text.Format(L"文件 \"%s\" 不是视觉库的支持库文件\n文件没有 GetNewInf 接口", path.c_str());
        MessageBoxW(0, text.c_str(), L"错误", MB_ICONERROR);
        return 1;
    }

    LIB_INFO* info = pfn();
    g_title.Format(L"易视觉库工具 视觉库版本: v%d.%d.%d", info->m_nMajorVersion, info->m_nMinorVersion, info->m_nBuildNumber);
    FreeLibrary(hDraw);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }


    MSG msg;

    // 主消息循环:
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return (int) msg.wParam;
}

inline void _jiaoyan_gengxin()
{
    RECT rc;
    GetClientRect(g_hWnd, &rc);
    rc.left = 8, rc.top = 90, rc.right -= 8;
    g_fmt = DT_LEFT | DT_EXTERNALLEADING | DT_NOPREFIX | DT_WORDBREAK;
    g_crText = RGB(64, 0, 128);

    wstr path = g_edit.GetText();
    if (path.back() != '\\') path.push_back('\\');
    g_src.reserve(500);
    g_dest.reserve(500);
    _enum_files(g_runPath, path.c_str(), g_src, g_dest, &rc);

    const LPCWSTR title = g_insert ? L"安装" : L"无需更新";
    g_insert.SetVisible(g_isInsert);
    g_insert.SetEnabled(g_isInsert);
    g_insert.SetWindowTextW(title);

    g_check.SetVisible(!g_isInsert);
    g_check.SetEnabled(!g_isInsert);

    const LPCWSTR text = g_isInsert ? L"处理完成, 请点击安装进行安装视觉库" : L"当前版本与易语言使用的版本完全一致, 不需要安装\r\n可以启用或者关闭视觉库";
    g_output.assign(text);
    g_fmt = DT_CENTER | DT_EXTERNALLEADING | DT_NOPREFIX | DT_WORDBREAK;
    g_crText = g_isInsert ? RGB(0, 128, 0) : RGB(64, 0, 255);
    if (g_isInsert) g_fmt |= DT_VCENTER | DT_SINGLELINE;
    InvalidateRect(g_hWnd, &rc, 0);
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
    g_hInst = hInstance; // 将实例句柄存储在全局变量中

    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIconW(0, IDI_APPLICATION);
    wcex.hCursor        = LoadCursorW(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = 0;
    wcex.lpszClassName  = L"insert";
    wcex.hIconSm        = LoadIconW(0, IDI_WINLOGO);
    RegisterClassExW(&wcex);
    DWORD style = WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER | WS_DLGFRAME | WS_CLIPSIBLINGS;
    HWND hWnd = CreateWindowExW(0, wcex.lpszClassName, g_title.c_str(), style,
        CW_USEDEFAULT, 0, 433, 180, nullptr, nullptr, hInstance, nullptr);

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
        g_hWnd = hWnd;
        wstr path(260);
        RECT rc;
        GetClientRect(hWnd, &rc);
        const int cxClient = rc.right - rc.left;
        const int cyClient = rc.bottom - rc.top;

        DWORD styleEx = 0, style = 0;
        CRegList reg(HKEY_CURRENT_USER, L"Software\\FlySky\\E\\Install");
        reg.read(L"Path", path.data());
        path.resize(wcslen(path.c_str()));
        if (!path.empty() && path.back() != '\\') path.push_back('\\');
        if (path.right(4) == L"lib\\")
            path.resize(path.size() - 4);



        styleEx = WS_EX_TRANSPARENT;
        style = WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | WS_CLIPSIBLINGS;
        g_static.create(styleEx, L"安装路径", style, 8, 8, 60, 24, hWnd);

        styleEx = WS_EX_CLIENTEDGE;
        style = WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_CLIPSIBLINGS;
        g_edit.create(styleEx, path.c_str(), style, 70, 8, cxClient - 70 - 88, 24, hWnd);

        styleEx = 0;
        style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;
        g_selPath.create(styleEx, L"选择路径", style, cxClient - 88, 8, 80, 24, hWnd, ID_SELPATH);

        g_insert.create(styleEx, L"校验更新", style, (cxClient - 130) / 2, 40, 130, 42, hWnd, ID_INSERT);

        style = WS_CHILD | WS_CLIPSIBLINGS | WS_DISABLED;
        g_check.create(styleEx, L"启用 易IDE视觉库", style, (cxClient - 130) / 2, 40, 130, 42, hWnd, ID_ADDFNE);

        style = SS_NOPREFIX;
        g_static1.create(styleEx, L"", style, 8, 90, cxClient - 16, 40, hWnd);
        break;
    }
    case WM_COMMAND:
    {
        const int id = LOWORD(wParam);
        // 分析菜单选择:
        switch (id)
        {
        case ID_INSERT:
            btn_insert_click();
            break;
        case ID_SELPATH:
            btn_selpath_click();
            break;
        case ID_ADDFNE:
        {
            if(g_check.IsCheck())
                fne::deleteFne("iDraw");
            else
                fne::addFne("iDraw");

            g_check.SetCheck(fne::isFne("iDraw"));
            break;
        }
        default:
            return DefWindowProcW(hWnd, message, wParam, lParam);
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 在此处添加使用 hdc 的任何绘图代码...
        RECT rc;
        static HFONT hFont = CWndBase::DefaultFont();
        SelectObject(hdc, hFont);
        GetClientRect(hWnd, &rc);
        SetTextColor(hdc, g_crText);

        FillRect(hdc, &rc, GetStockBrush(WHITE_BRUSH));
        rc.left += 8, rc.right -= 8, rc.top = 90;
        DrawTextW(hdc, g_output.c_str(), -1, &rc, g_fmt);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_CTLCOLORSTATIC:
        return (LRESULT)GetStockBrush(WHITE_BRUSH);
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}

inline static void __createPath(LPWSTR path)
{
    if (PathIsDirectoryW(path)) return;
    LPWSTR find = wcschr(path, '\\');
    wchar_t old = 0;
    while (find)
    {
        old = *find;
        *find = 0;
        if (!PathIsDirectoryW(path))
            CreateDirectoryW(path, 0);

        *find = old;
        find = wcschr(find + 1, '\\');

    }
    if (!PathIsDirectoryW(path))
        CreateDirectoryW(path, 0);
}

void CALLBACK TimerProc(HWND hWnd, UINT message, UINT_PTR id, DWORD timer)
{
    RECT rc;
    GetWindowRect(g_hWnd, &rc);
    int cxClient = rc.right - rc.left;
    int cyClient = rc.bottom - rc.top;
    const int offset = 5;
    if (id == 10)
    {
        if (cyClient < 200)
        {
            cyClient += offset;
            if (cyClient > 200)
                cyClient = 200;
            MoveWindow(g_hWnd, rc.left, rc.top, cxClient, cyClient, true);
            return;
        }
        KillTimer(g_hWnd, id);
        return;
    }

    if (id == 11)
    {
        if (cyClient > 130)
        {
            cyClient -= offset;
            if (cyClient < 130)
                cyClient = 130;
            MoveWindow(g_hWnd, rc.left, rc.top, cxClient, cyClient, true);
            return;
        }
        KillTimer(g_hWnd, id);
        return;
    }
}
void btn_insert_click()
{
    wstr text = g_insert.GetWindowTextW();
    if (text == L"校验更新")
    {
        g_insert.SetEnabled(false);
        _jiaoyan_gengxin();
        return;
    }
    //KillTimer(g_hWnd, 11);
    //SetTimer(g_hWnd, 10, 10, TimerProc);
    g_fmt = DT_LEFT | DT_EXTERNALLEADING | DT_NOPREFIX | DT_WORDBREAK;
    RECT rc;
    GetWindowRect(g_hWnd, &rc);
    const int cxScreen = rc.right - rc.left;
    //SetWindowPos(g_hWnd, 0, 0, 0, cxScreen, 200, SWP_NOMOVE | SWP_NOZORDER);
    g_insert.SetEnabled(false);

    GetClientRect(g_hWnd, &rc);
    rc.left = 8, rc.top = 90, rc.right -= 8;

    wstr err;
    CFileRW wl, wr;
    for (size_t i = 0; i < g_src.size(); i++)
    {
        wstr& l = g_src[i];
        wstr& r = g_dest[i];
        if (l.back() == '\\')   // 加入的时候已经做了处理, 目录最后是反斜杠
        {
            __createPath(r.data());
            continue;
        }

        bool isOpenl = wl.open(l.c_str(), 1);
        bool isOpenR = wr.open(r.c_str(), 4, 0);

        if (isOpenl && isOpenR)
        {
            int size = wl.size();
            if (size > 0)
            {
                LPBYTE data = new BYTE[size];
                isOpenl = wl.read(data, size);
                if (isOpenl)
                    isOpenR = wr.write(data, size);
                delete[] data;
            }
            else
            {
                isOpenR = false;
            }
        }
        g_output.Format(L"将 \"%s\" 复制到 \"%s\"", l.c_str(), r.c_str());
        g_crText = (isOpenl && isOpenR) ? RGB(0, 128, 0) : RGB(255, 0, 0);
        InvalidateRect(g_hWnd, &rc, 0);
        UpdateWindow(g_hWnd);
        sleep(g_sleep_timer);
        if (!isOpenl || !isOpenR)
        {
            wstr text;
            text.Format(L"读入 \"%s\" %s, 写出 \"%s\" %s\n", l.c_str(), (isOpenl ? L"成功" : L"失败"),
                r.c_str(), (isOpenl && isOpenR ? L"成功" : L"失败"), GetLastError());
            err.append(text);
            wl.reset();
            wr.reset();
            DeleteFileW(r.c_str());
        }

        //bool isOk = CopyFileW(l.c_str(), r.c_str(), false);
        wstr::dbg(L"原始文件 = %s, 移动到 = %s\n", l.c_str(), r.c_str());
    }

    if (!err.empty())
    {
        g_check.SetEnabled(false);
        g_check.hide();
        g_insert.show();
        g_output.assign(L"安装出错, 请关闭易语言后再重新安装");
        g_fmt = DT_CENTER | DT_EXTERNALLEADING | DT_NOPREFIX | DT_WORDBREAK;
        g_crText = RGB(128, 0, 0);
        g_fmt |= DT_VCENTER | DT_SINGLELINE;
        InvalidateRect(g_hWnd, &rc, 0);
        MessageBoxW(g_hWnd, err.c_str(), L"安装出错", MB_ICONERROR);
    }
    else
    {
        //int ret = MessageBoxW(g_hWnd, L"安装完成, 是否启用视觉库支持库?", L"安装成功", MB_ICONINFORMATION | MB_YESNO);
        //if (ret == IDYES)
        //{
        //    bool isOk = fne::addFne("iDraw");
        //    if (isOk)
        //    {
        //        MessageBoxW(g_hWnd, L"启用视觉库成功, 请打开易语言查看效果", L"启用成功", MB_ICONINFORMATION);
        //    }
        //    else
        //    {
        //        MessageBoxW(g_hWnd, L"启用视觉库失败, 尝试重新启用", L"启用失败", MB_ICONERROR);
        //    }
        //}

        bool isFne = fne::isFne("iDraw");
        g_check.SetCheck(isFne);
        g_check.SetEnabled(true);
        g_check.show();
        g_insert.hide();

        g_output.assign(L"安装成功, 启用后可以打开查看效果");
        g_fmt = DT_CENTER | DT_EXTERNALLEADING | DT_NOPREFIX | DT_WORDBREAK;
        g_crText = RGB(0, 128, 0);
        g_fmt |= DT_VCENTER | DT_SINGLELINE;
        InvalidateRect(g_hWnd, &rc, 0);
    }
    g_insert.SetEnabled(true);
    //KillTimer(g_hWnd, 10);
    //SetTimer(g_hWnd, 11, 10, TimerProc);
    //SetWindowPos(g_hWnd, 0, 0, 0, cxScreen, 130, SWP_NOMOVE | SWP_NOZORDER);
    return ;
}
void btn_selpath_click()
{
    wchar_t path[260] = { 0 };
    CFileDialog_Com f;
    wstr file = f.openFolder(0, 0, 0, L"请选择安装目录");
    if (file.empty())return;
    if (file.back() != '\\')
        file.append(L"\\");

    g_edit.SetWindowTextW(file.c_str());
}

inline wstr GetMd5FromFile(LPCWSTR file, int* pSize = 0)
{
    CFileRW f;
    f.open(file, 1, -1);
    int size = f.size();
    if (size == -1)return L"";
    LPBYTE ptr = new BYTE[size];
    f.read(ptr, size);
    if (pSize) *pSize = size;
    wstr md5;
    GetMd5(ptr, size, md5);
    delete[] ptr;
    return md5;
}

// 对比两个文件的MD5是否一样
inline bool cmpmd5(LPCWSTR file1, LPCWSTR file2, int* pSize = 0)
{
    return GetMd5FromFile(file1, pSize) == GetMd5FromFile(file2);
}

// 递归枚举当前exe程序目录下的所有文件和文件夹, 把枚举到的文件和新路径记录到数组
// lpszSrcPath = 源路径, 当前exe程序的目录
// lpszDestPath = 目标路径, 会根据枚举到的文件名拼接成完整的路径
// src = 枚举到的所有文件路径
// dest = 应该移动到的路径, 点击安装后需要把枚举到的文件复制到这里
// prc = 重画的区域
void _enum_files(LPCWSTR lpszSrcPath, LPCWSTR lpszDestPath, std::vector<wstr>& src, std::vector<wstr>& dest, RECT* prc)
{
    wstr tmpPath(260);
    wstr path(260);
    path.assign(lpszSrcPath);
    if (path.back() != '\\') path.push_back('\\');

    tmpPath.append(path).append(L"*.*");

    WIN32_FIND_DATA fd = { 0 };
    DWORD dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY
        | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_COMPRESSED | FILE_ATTRIBUTE_DIRECTORY
        | FILE_ATTRIBUTE_TEMPORARY | FILE_ATTRIBUTE_SPARSE_FILE;

    HANDLE hFind = FindFirstFileW(tmpPath.c_str(), &fd);
    if (INVALID_HANDLE_VALUE == hFind) return;
    wstr srcName(260);
    wstr dstName(260);
    do
    {
        if ((dwFileAttributes & fd.dwFileAttributes) != fd.dwFileAttributes)   // 匹配类型
            continue;

        srcName.assign(lpszSrcPath).append(fd.cFileName);
        dstName.assign(lpszDestPath).append(fd.cFileName);
        if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
        {
            if (fd.cFileName[0] == '.')
                continue;


            if (srcName.back() != '\\') srcName.push_back('\\');
            if (dstName.back() != '\\') dstName.push_back('\\');
            if (srcName.find(L"\\lib\\") != wstr::npos || srcName.find(L"\\clr\\") != wstr::npos || srcName.find(L"\\Tools\\") != wstr::npos)
            {
                src.push_back(srcName);
                dest.push_back(dstName);
                _enum_files(srcName.c_str(), dstName.c_str(), src, dest, prc);
            }
            continue;
        }
        if (srcName.find(L"\\lib\\") != wstr::npos || srcName.find(L"\\clr\\") != wstr::npos || srcName.find(L"\\Tools\\") != wstr::npos)
        {
            g_output.Format(L"正在校验: \"%s\"", srcName.c_str());
            InvalidateRect(g_hWnd, prc, false);
            UpdateWindow(g_hWnd);

            int size = 0;
            bool isCmp = cmpmd5(srcName.c_str(), dstName.c_str(), &size);
            if (!isCmp)
                g_isInsert = true;

            const float x = 1024 * 1024 * 1;
            const int b = (size > (int)x) ? rand(2, 5): 1;
            sleep(g_sleep_timer * (int)b);

            src.push_back(srcName);
            dest.push_back(dstName);
        }

    } while (FindNextFileW(hFind, &fd));

    FindClose(hFind);
    return;
}

// 判断目录是否为易语言根目录, 从目录下寻找lib\\krnln.fne 文件
bool isEPath(LPCWSTR path, wstr* err)
{
    wstr file(260);
    file.assign(path);
    if (file.back() != '\\') file.append(L"\\");
    file.append(L"krnln.fne");
    if (!PathFileExistsW(file.c_str()))
    {
        if (err) err->Format(L"\"%s\" 文件不存在, 请选择易语言主程序", file.c_str());
        return false;
    }

    HMODULE hModule = LoadLibraryExW(file.c_str(), 0, DONT_RESOLVE_DLL_REFERENCES);
    if (!hModule)
    {
        if (err) err->Format(L"\"%s\" 文件不是易语言核心库或已损坏, 请重新选择", file.c_str());
        return false;
    }
    FreeLibrary(hModule);
    if (err) err->clear();
    return true;
}