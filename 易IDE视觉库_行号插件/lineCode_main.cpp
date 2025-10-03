// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <unordered_set>
#include "edfs.h"
#include <elib/PublicIDEFunctions.h>

#define IDMENU_ENABLED      1
#define IDMENU_SETTING      1000
#define RGB2ARGB(_cr,a)     ((DWORD)(BYTE)(a) << 24) | ((_cr & 0x000000ff) << 16) | ((_cr & 0x0000ff00)) | ((_cr & 0x00ff0000) >> 16)
#define MAKEARGB(a,r,g,b)   ((COLORREF)( (((BYTE)(b) | ((WORD)((BYTE)(g))<<8)) | (((DWORD)(BYTE)(r))<<16)) | (((DWORD)(BYTE)(a))<<24) ))


LPIDRAW_INFO g_info;
IGetEWindow* g_eWnd;
ID2DDraw* g_dx;

CLineCodeConfig g_ini;      // 程序配置
HINSTANCE g_hInst;
HWND g_hEWnd;               // 易语言主界面窗口句柄
static LPCANVAS m_hCanvas;  // 画布句柄
static HWND m_hWnd;         // 显示行号线的窗口, 这个必须要有分层和穿透的样式

static RECT m_rcLine;       // 当前激活行号的矩形位置, 绘画时记录, 记录时已经转换成易语言主窗口的客户区坐标了
static LPEX_PEN m_hPen;     // 边框画笔
std::unordered_set<HWND> m_setMDIClient4;   // MDI下的第4层窗口, 代码切换选择夹切换的时候记录, mdi窗口销毁时删除
HWND m_hMDIClient4;                         // 当前MDI下显示的窗口, 切换代码选择夹的时候赋值, 需要判断是否为0
static LPMEMDCBITMAP m_md;  // 行号缓存
static CRITICAL_SECTION s_lockCaretChange;     // lock光标变动
static int* g_pLineSpacing;     // 单行间隔指针
static int* g_pLineHeight;      // 单行高度指针
static void* m_oldHook;
static int s_oldPos, s_oldLine, s_oldMaxPos, s_oldcxcy;
static POINT s_oldSel;
static int m_EVersion;
static int m_isDrawLine;        // 是否绘制行号, 593以下的版本需要绘画, 593或者以上的版本需要获取行号的矩形, 画个行号框


bool CALLBACK Scroll_DrawScroll(HDC hdcWindow);
// 更新分层窗口, 返回错误码, 0=成功
// hWnd = 要重画的窗口
// width = 窗口宽度
// height = 窗口高度
// srcDC = 从哪个dc拷贝到窗口上
// ppt = 窗口新的位置
// alpha = 窗口透明度
int UpdateLayered(HWND hWnd, int width, int height, HDC srcDC, PPOINT ppt = 0, int alpha = 255);

// 绘画行号线
void LineCode_DrawLine();

LRESULT CALLBACK EWindow_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);


inline int scale(int n)
{
    int dpi = iDraw_GetInfo(g_info, IGI_DPI, 0);
    float scale = (float)dpi / 96.f;
    return (int)((float)n * scale);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        g_hInst = hModule;
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
// 调试输出
inline static void debug(LPCSTR format, ...)
{
    if (!format)return;
    va_list aptr;
    va_start(aptr, format);

    int bufferSize = (int)(strlen(format) * 2);
    if (bufferSize < 260) bufferSize = 260;
    char* buffer = new char[bufferSize];

    //pointer buffer = realloc(bufferSize, true);
    int r = _vsnprintf_s(buffer, bufferSize, bufferSize - 1, format, aptr);
    while (r == -1)
    {
        bufferSize *= 2;
        delete[] buffer;
        buffer = new char[bufferSize];
        r = _vsnprintf_s(buffer, bufferSize, bufferSize - 1, format, aptr);
    }

    va_end(aptr);

    OutputDebugStringA(buffer);
    delete[] buffer;
    return;

}

inline void _create_pen()
{
    if (m_hPen)
        g_dx->_pen_destroy(m_hPen);
    m_hPen = g_dx->_pen_create(RGB2ARGB(g_ini.crLine(), 255), 1.f, g_ini.m_penStyle, g_ini.m_dot, g_ini.m_dotCount);
}

inline bool _dot_isChange()
{
    int dotCount = 0, style = 0;
    float* dot = g_ini.ReadDot(dotCount, style);
    if (!dot)
    {
        // 当前读取到的值为0
        if (!g_ini.m_dot)
        {
            if (style != g_ini.m_penStyle)
            {
                g_ini.m_penStyle = style;
                return true;
            }
        }
        g_ini.FreeDot(g_ini.m_dot);
        g_ini.m_dot = 0;
        g_ini.m_dotCount = 0;
        return true;
    }

    if (!g_ini.m_dot)
    {
        // 当前读取的有值, 配置里没有值
        g_ini.m_dot = dot;
        g_ini.m_dotCount = dotCount;
        return true;
    }
    if (memcmp(g_ini.m_dot, dot, sizeof(float) * dotCount) != 0)
    {
        // 读取的有值, 且配置里有值
        g_ini.FreeDot(g_ini.m_dot);
        g_ini.m_dot = dot;
        g_ini.m_dotCount = dotCount;
        return true;
    }
    g_ini.FreeDot(dot);
    return style != g_ini.m_penStyle;   // 能走到这就是有值, 且值相等
}

// 与支持库交互的回调函数
int CALLBACK IDraw_NotifyCallback(UINT nCode, WPARAM wParam, LPARAM lParam)
{
    if (!m_isDrawLine)
        return 0;
    switch (nCode)
    {
    case INE_CONFIG_CHANGED:    // 配置被改变
    {
        CPrivateProfile& ini = g_ini.ini;
        bool isChange = false;
#define _config_null
#define _config_change(_name, _type, _cmp) \
        _type m_##_name = ini.read_int(LINECODE_KEYNAME, L###_name, 1) _cmp ;\
        if (m_##_name != g_ini.m_##_name){\
            g_ini.m_##_name = m_##_name;\
            isChange = true;\
        }
#define _config_change_color(_name) \
        COLORREF m_##_name = g_ini.GetNodeColor(L###_name, 0);\
        if (m_##_name != g_ini.m_##_name){\
            g_ini.m_##_name = m_##_name;\
            isChange = true;\
        }

        _config_change(isEnabled        , bool, != 0);
        _config_change(isDrawLine       , bool, != 0);
        _config_change(isDrawLineNumber , bool, != 0);
        _config_change(isBoldLine       , bool, != 0);
        _config_change(isAutoBack       , bool, != 0);
        _config_change(isAutoWidth      , bool, != 0);
        _config_change(lineWidth        , int , _config_null);

        _config_change_color(crHot);
        _config_change_color(crBack);
        _config_change_color(crNormal);
        _config_change_color(crExpand);

        COLORREF crLine = g_ini.GetNodeColor(L"crLine", 0);
        if (crLine != g_ini.m_crLine || _dot_isChange())
        {
            g_ini.m_crLine = crLine;
            isChange = true;
            _create_pen();
        }

        if (isChange)
        {

        }
        if (m_hMDIClient4)
        {
            s_oldLine++;    // 修改一下原来记录的选中项, 让重画行号继续重画
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
    switch (id)
    {
    case IDMENU_ENABLED:    // 启用禁用
    {
        bool isEnabled = !g_ini.isEnabled();
        g_ini.isEnabled(isEnabled);

        MENUITEMINFOW item = { 0 };
        item.cbSize = sizeof(item);
        item.fMask = MIIM_STATE;
        item.fState = isEnabled ? MFS_CHECKED : MFS_UNCHECKED;   // 改变选中状态
        SetMenuItemInfoW((HMENU)DLL_GUID, IDMENU_ENABLED, false, &item);
        break;
    }
    case IDMENU_SETTING:    // 设置
    {
        LoadSettingWindow();
        break;
    }
    default:
        break;
    }
    return 0;
}

// 判断当前窗口是否为代码窗口, 全局变量, 常量, 资源, dll, 自定义数据类型暂时没有处理, 返回false
// 参考返回当前代码框的类型
bool isCodeType(MDICLIENT_CODETYPE& codeType)
{
    MDICLIENT_TYPE type = (MDICLIENT_TYPE)iDraw_GetInfo(g_info, IGI_MDICLIENTTYPE, 0);
    codeType = (MDICLIENT_CODETYPE)HIWORD(type);
    type = (MDICLIENT_TYPE)LOWORD(type);
    if (type != MDICLIENT_TYPE_CODE)    // 行号只有代码区才绘画
        return false;


    switch (codeType)
    {
    case MDICLIENT_CODETYPE_ASSEMBLY:
    case MDICLIENT_CODETYPE_ASSEMBLY_WINDOW:
    case MDICLIENT_CODETYPE_CLASS:
    {

        return true;
    }
    case MDICLIENT_CODETYPE_GLOBALVAR:
        break;
    case MDICLIENT_CODETYPE_STRUCT:
        break;
    case MDICLIENT_CODETYPE_DLL:
        break;
    case MDICLIENT_CODETYPE_CONST:
        break;
    case MDICLIENT_CODETYPE_IMAGE:
        break;
    case MDICLIENT_CODETYPE_SOUND:
        break;
    default:
        break;
    }

    return false;
}

// 获取mdi下第4层窗口数据, 返回第4层窗口的窗口句柄
inline HWND GetMDIClient4(HWND hMDIClient1, MDICLIENT_TYPE& type, HWND* pVScroll = 0)
{
    int id = 0;
    HWND hChild = hMDIClient1;

    hChild = GetWindow(hChild, GW_CHILD);    // mdi下的第二层窗口
    while (hChild)
    {
        id = GetDlgCtrlID(hChild);
        if (id == 59648)
            break;
        hChild = GetWindow(hChild, GW_HWNDNEXT);
    }

    if (!hChild) return 0;


    hChild = GetWindow(hChild, GW_CHILD);    // MDI下的第三层窗口, 根据这个窗口判断是写代码还是起始页或者是窗口设计器


    while (hChild)  // 1=设计器或者起始页, 59648=代码区, 只要不是这两种情况都继续往后取
    {
        // 这里的做法是排除滚动条
        id = GetDlgCtrlID(hChild);
        if (id == 1)
        {
            // 需要判断是起始页还是设计器窗口
            hChild = GetWindow(hChild, GW_CHILD);
            // 117是窗口设计器的窗口, 等于117就返回1, 否则返回2, 1=窗口设计器, 2=起始页, 0=代码区
            type = (GetDlgCtrlID(hChild) == 117) ? MDICLIENT_TYPE::MDICLIENT_TYPE_IDE : MDICLIENT_TYPE::MDICLIENT_TYPE_HONE;
            break;
        }
        else if (id == 59648)
        {
            type = MDICLIENT_TYPE::MDICLIENT_TYPE_CODE;
            break;
        }
        else if (id == 59920)
        {
            if (pVScroll)*pVScroll = hChild;
        }
        hChild = GetWindow(hChild, GW_HWNDNEXT);
    }

    if (!hChild)
    {
        type = MDICLIENT_TYPE::MDICLIENT_TYPE_NONE;
        return 0;
    }
    else
    {
        HWND hMDIClient4 = GetWindow(hChild, GW_CHILD);
        while (hMDIClient4)
        {
            int id = GetDlgCtrlID(hMDIClient4);
            if (id == 1)
                break;
            hMDIClient4 = GetWindow(hMDIClient4, GW_HWNDNEXT);
        }
        return hMDIClient4;
    }
    return 0;
}


// 计算当前行代码显示需要多少行
int _calc_linecount(LPCSTR text, int codeWidth, int chWidth, int flagWidth)
{
    if (!text) return 1;
    int textLen = (int)strlen(text);
    if (!textLen) return 1;

    int n = 0, arrCount = 0;
    LPBYTE arr = new BYTE[textLen + 1];
    memset(arr, 0, textLen + 1);
    // 循环一遍计算每个字是两个字符宽度还是1个字符宽度
    while ((n + 1) <= textLen)
    {
        LPBYTE pch = (LPBYTE)text;
        //if (n + 1 >= c || pch[n] != 13)
        //    z = 1;
        const int z = (pch[n] > 128 || pch[n + 1] == 10) ? 2 : 1; // 中文占用2个字符宽度, 非中文占用1个字符宽度
        arr[arrCount++] = z;
        n += z;
    }
    const int minWidth = (7 * chWidth) + flagWidth;     // 最小宽度, 左边标识宽度+7个中文宽度
    if (codeWidth < minWidth) codeWidth = minWidth;
    int width = codeWidth - flagWidth;
    int line = 1;
    chWidth /= 2;
    const int minCharWidth = chWidth * 2 + chWidth;     // 最小字符宽度, 小于这个值就是需要加一行
    for (int i = 0; i < arrCount; i++)
    {
        const int _item = arr[i];
        width -= (_item * chWidth);     // 每次减去字符的宽度, 如果宽度小于最小字符宽度, 那就是需要换行
        if ((i + 1) < arrCount)
        {
            if (width < minCharWidth)
            {
                width = codeWidth - flagWidth;
                line++;
            }
        }
    }
    delete[] arr;
    return line;
}
union EIDE_BASE
{
    eVariable*  pVariable;
    eResource*  pResource;
    eArray*     pArray;
    eStream*    pStream;
    eList*      pList;
    eBase*      pBase;
    eStruct*    pStruct;
    eMethod*    pMethod;
    eAssembly*  pAssembly;
    eDll*       pDll;
    eECInfo*    pECInfo;
    eLibInfo*   pLibInfo;
};

// 计算最后一个成员的高度, 最后一个成员需要-1
inline void _calc_last_height(int type1, int type2, int offset, long& bottom, EIDE_BASE data)
{
    if (!data.pArray || !type1 || !type2) return;
    offset++;
    switch (type2)
    {
    case 206:   // DLL参数
    {
        if (offset == data.pDll->Member.Count)
            bottom--;
        break;
    }
    case 313:   // 数据类型成员
    {
        if (offset == data.pStruct->Member.Count)
            bottom--;
        break;
    }
    case 367:   // 全局变量
    {
        if (offset == data.pVariable->Attribute)
            bottom--;
        break;
    }
    case 632:   // 程序集成员
    {
        if (offset == data.pAssembly->Method.Length)
            bottom--;
        break;
    }
    case 740:   // 子程序参数
    {
        if (offset == data.pMethod->Parameter.Count)
            bottom--;
        break;
    }
    case 796:   // 子程序变量
    {
        if (offset == data.pMethod->Variable.Count)
            bottom--;
        break;
    }
    case 421:   // 常量
    {
        //if (offset == data.pStream)
        //    bottom--;
        break;
    }
    case 474:   // 图片资源
    {
        if (offset == data.pResource->Attribute)
            bottom--;
        break;
    }
    case 527:   // 声音资源
    {
        if (offset == data.pResource->Attribute)
            bottom--;
        break;
    }
    default:
        break;
    }

}
struct LINE_WIDTH_DATA
{
    HFONT hFont;
    EWINDOW_MDICHILD wnd;
    SCROLLINFO si;
};
inline int LineCode_GetWidth(LINE_WIDTH_DATA* pData = 0)
{
    int defWidth = g_ini.lineWidth();
    if (g_ini.isAutoWidth())
    {
        defWidth = 200;
        LINE_WIDTH_DATA info = { 0 };
        pData = pData ? pData : &info;

        g_eWnd->GetMDIClientChildWindow(0, pData->wnd);
        if (!pData->wnd.hMDIClient4 || !pData->wnd.hVScroll)
            return defWidth;
        pData->si.cbSize = sizeof(pData->si);
        pData->si.fMask = SIF_ALL;
        GetScrollInfo(pData->wnd.hVScroll, SB_CTL, &pData->si);
        int max = pData->si.nPos + 50;


        int maxPos   = (int)(LONG_PTR)GetPropW(pData->wnd.hMDIClient4, L"maxPos");
        int maxWidth = (int)(LONG_PTR)GetPropW(pData->wnd.hMDIClient4, L"maxWidth");
        if (maxPos > max && maxWidth > 14)
            return maxWidth;
        
        if (max == 0)
            max = 999;
        HFONT hFont = (HFONT)SendMessageA(g_eWnd->GetStateTabOutput(), WM_GETFONT, 0, 0);
        if (!hFont)
            hFont = (HFONT)iDraw_GetInfo(g_info, IGI_CONFIG_DEFGDIFONT, 0);
        
        wchar_t text[50], text2[50];
        swprintf_s(text, 50, L"%d", max);
        swprintf_s(text2, 50, L"%d", maxPos);

        maxPos = max;
        HDC hdc = CreateCompatibleDC(0);
        SelectObject(hdc, hFont);
        RECT rcText = { 0, 0, 100, 30 };
        DrawTextW(hdc, text, -1, &rcText, DT_CALCRECT | DT_VCENTER | DT_RIGHT | DT_SINGLELINE);
        DeleteDC(hdc);
        defWidth = rcText.right - rcText.left + 5;
        SetPropW(pData->wnd.hMDIClient4, L"maxPos"  , (HANDLE)maxPos);
        SetPropW(pData->wnd.hMDIClient4, L"maxWidth", (HANDLE)defWidth);
        if (wcslen(text) > wcslen(text2))   // 如果计算出的最大值 长度大于 记录的最大值, 那就需要重新计算非客户区, 进位了
        {
            SetWindowPos(pData->wnd.hMDIClient4, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
        }
    }
    return defWidth;
}
inline bool CALLBACK callback_fnCalcScroll(void* pScroll, LPOBJSTRUCT pData, RECT* prc)
{
    if (!g_ini.isEnabled() || !g_ini.isDrawLineNumber()) return 0;
    prc->left += LineCode_GetWidth();
    return 0;
}

inline bool CALLBACK callback_fnGetScrollRect(void* pScroll, LPOBJSTRUCT pData, bool isVScroll, RECT* prc)
{
    if (!g_ini.isEnabled() || !g_ini.isDrawLineNumber()) return 0;
    prc->left -= LineCode_GetWidth();
    return 0;
}
// MDI下第三层窗口绘画回调
inline bool CALLBACK callback_fnDrawScroll(void* pScroll, LPOBJSTRUCT pData, bool isVScroll, HDC hdcWindow, int type, RECT_F* rcDraw)
{
    if (type != SCROLL_DRAWTYPE_ALERT)
        return false;
    return Scroll_DrawScroll(hdcWindow);
}
template<typename T, typename R>
inline int _nes_run_fun(int nCode, T wParam, R lParam)
{
    struct RUNFUN_ARG
    {
        int wParam;
        int lParam;
    };
    typedef int(__stdcall* pfn_ide_call)(int nCode, int, RUNFUN_ARG*);

    RUNFUN_ARG arg = { 0 };
    arg.wParam = (int)wParam;
    arg.lParam = (int)lParam;
    pfn_ide_call pfn = (pfn_ide_call)iDraw_GetInfo(g_info, IGI_IDENOTIFY, 0);
    return pfn(NES_RUN_FUNC, nCode, &arg);
}



bool CALLBACK Scroll_DrawScroll(HDC hdcWindow)
{
    if (!g_ini.isEnabled()) return false;
    IEIDE_CodeManage* ide = (IEIDE_CodeManage*)iDraw_IDCall(IDC_EIDE_GETCODEMANAGE, 0, 0);  // 没有代码管理类要么是bug, 要么就不是代码窗口
    if (!ide) return false;

    //EIDE_LineInit();
    //if (!g_pLineSpacing || !g_pLineHeight) return 0;
    // 不绘画行号的话需要绘画行号线, 行号线需要在这里计算线的位置
    const bool isDrawLineNumber = g_ini.isDrawLineNumber(); // 是否绘画行号, 为false的话就不进行任何绘画操作
    if (!isDrawLineNumber && !g_ini.isDrawLine()) return 0; // 不绘画行号 且不绘画行号线, 直接返回, 不做任何处理

    LINE_WIDTH_DATA info = { 0 };
    const int lineWidth = LineCode_GetWidth(&info);
    if (!info.wnd.hMDIClient4)
        return 0;
    const int nPos = (int)info.si.nPos; // 当前滚动条的位置, 也是第一行的行号
    if (nPos < 0) return 0;

    int nNowLine = 0;   // 鼠标选中行
    POINT ptSel = { 0 };
    ide->GetSelInfo(0, 0, &nNowLine, &ptSel);

    RECT rcMDIClient4;
    GetClientRect(info.wnd.hMDIClient4, &rcMDIClient4);
    RECT rcLine = { 0, 0, lineWidth, rcMDIClient4.bottom - rcMDIClient4.top };

    if (nNowLine == s_oldLine && s_oldPos == nPos && s_oldMaxPos == info.si.nMax
        && s_oldSel.x == ptSel.x && s_oldSel.y == ptSel.y
        && s_oldcxcy == (MAKELONG(rcMDIClient4.right - rcMDIClient4.left, rcMDIClient4.bottom - rcMDIClient4.top))
        )
    {
        // 行号, 选中, 当前滚动位置, 最大滚动位置, 宽度高度 都没有变化的情况下使用缓存dc的内容拷贝到窗口
        if (isDrawLineNumber)
            BitBlt(hdcWindow, 0, 0, rcLine.right, rcLine.bottom, m_md->hdc, 0, 0, SRCCOPY);
        return 0;
    }
    s_oldLine = nNowLine;
    s_oldPos = nPos;
    s_oldSel = ptSel;
    s_oldMaxPos = info.si.nMax;
    s_oldcxcy = (MAKELONG(rcMDIClient4.right - rcMDIClient4.left, rcMDIClient4.bottom - rcMDIClient4.top));



    //const int nLineSpacing = *g_pLineSpacing;   // 行间隔
    //const int nLineHeight  = *g_pLineHeight;    // 行高度
    const int nLineSpacing = iDraw_IDCall(IDC_EIDE_GETLINESPACING, 0, 0);   // 行间隔
    const int nLineHeight = iDraw_IDCall(IDC_EIDE_GETLINEHEIGHT, 0, 0);     // 行高度



    int type1 = 0, type2 = 0;
    int y1 = 0, y2 = 0, lineHeight = nLineHeight;
    RECT oldRcLine = m_rcLine;
    m_rcLine = { 0 };


    LPMEMDCBITMAP md = md_create(rcLine.right, rcLine.bottom);
    HDC hdc = md->hdc;
    COLORREF crBack = g_ini.crBack();
    if (g_ini.isAutoBack())
    {
        int size = iDraw_IDCall(IDC_EIDE_GETECOLOR, 0, 0);
        if (size > 100)
        {
            LPBYTE pColor = new BYTE[size];
            iDraw_IDCall(IDC_EIDE_GETECOLOR, size, (LPARAM)pColor);
            crBack = ((int*)pColor)[40];    // 第40个成员是标志列背景
            delete[] pColor;
        }
    }

    SetBkMode(hdc, TRANSPARENT);
    SetBkColor(hdc, crBack);
    HFONT hFont = (HFONT)SendMessageA(g_eWnd->GetStateTabOutput(), WM_GETFONT, 0, 0);
    if (!hFont)
        hFont = (HFONT)iDraw_GetInfo(g_info, IGI_CONFIG_DEFGDIFONT, 0);
    SelectObject(hdc, hFont);
    if (isDrawLineNumber)
    {
        HBRUSH hbr = iDraw_CreateBrush(crBack);
        FillRect(hdc, &rcLine, hbr);
        SelectObject(hdc, GetStockObject(NULL_BRUSH));  // 往后都不需要使用画刷了
    }

    int nLine = 0;  // 当前绘画行
    bool isReadValue = false;
    int lineFlag = 0;

    for (int i = 0; i < 10000; i++)
    {
        lineHeight = nLineHeight;
        nLine = nPos + i;
        isReadValue = true;
        lineFlag = 1;

#define _type_if_else(_i, _t1, _t2) \
    case _i:\
        if (type2 == 0)\
            type2 = _t2, isReadValue = false;\
        else if (type2 == _t2)\
            type1 = _t1, type2 = 0, isReadValue = false;\
        break

#define _type_if(_i, _t2) \
    case _i:\
        if (type2 == 0)\
            type2 = _t2, isReadValue = false;\
        break


        switch (type1)
        {
            _type_if_else(1, 2, 51);
            _type_if_else(2, 3, 104);
            _type_if_else(3, 4, 155);
            _type_if_else(5, 6, 261);

            _type_if(7, 367);
            _type_if(8, 580);
            _type_if(9, 632);
            _type_if(10, 686);
            _type_if(11, 740);
            _type_if(12, 796);
            _type_if(14, 421);
            _type_if(15, 474);
            _type_if(16, 527);
        default:
            break;
        }


        int flags = 0, data = 0, offset = 0, data2 = 0;
        if (isReadValue)
        {
            type1 = ide->GetLineInfo(nLine, &flags, &data);
            ide->GetCodeDataOffset(nLine, 0, &type2, &offset, &data2);
        }
        if (!type1 && !type2)
            break;


// case索引, 计算绘画顶边和绘画高度
// __case = 响应的索引
// __y1 = y1 要增加的高度
// __t2 = 判断type2的值
#define _case_type2_elseif(__case, __y1, __t2, __height1, __height2) \
        case __case:\
            if(type2 == 0){\
                y1 += __y1;\
                lineHeight += __height1;\
            }\
            else if( type2 == __t2){\
                y1++;\
                lineHeight += __height2;\
            }\
            break


        const int f1 = (nLineSpacing * 2 + (i == 0 ? 1 : 2));   // 边距 * 2 + 第一次+1, 其他+2
        const int s21 = (nLineSpacing * 2 + 1);   // 边距 * 2 + 1
        const int s1 = (nLineSpacing + 1);   // 边距 + 1
        const int s2 = (nLineSpacing + 2);   // 边距 + 2
        int lineCount = 1;
        switch (type1)
        {
            _case_type2_elseif(1,   f1,  51,  7, 7);    // DLL名
            _case_type2_elseif(2,   1,   104, 7, 7);    // DLL库
            _case_type2_elseif(3,   1,   155, 7, 7);    // DLL命令
            _case_type2_elseif(4,   1,   206, 7, 7);    // DLL参数

            _case_type2_elseif(5,   f1,  261, 7, 7);    // DLL数据类型名
            _case_type2_elseif(6,   1,   313, 7, 7);    // 数据类型成员

            _case_type2_elseif(7,   s21, 367, 7, 7);    // 全局变量
            _case_type2_elseif(8,   s1,  580, 7, 7);    // 程序集
            _case_type2_elseif(9,   1,   632, 7, 7);    // 程序集成员
            _case_type2_elseif(10,  f1,  686, 7, 6);    // 子程序
            _case_type2_elseif(11,  1,   740, 7, 7);    // 子程序参数
            _case_type2_elseif(12,  s2,  796, 7, 7);    // 子程序变量
            _case_type2_elseif(14,  s21, 421, 7, 7);    // 常量
            _case_type2_elseif(15,  s21, 474, 7, 7);    // 图片资源
            _case_type2_elseif(16,  s21, 527, 7, 7);    // 声音资源

        case 13: // 子程序代码
        {
            if (type2 == 851)
            {

                GET_PRG_TEXT_PARAM arg = { 0 };
                arg.m_nRowIndex = nLine;
                arg.m_nColIndex = 0;
                int r = _nes_run_fun(FN_GET_PRG_TEXT, &arg, 0);
                LPSTR lineCodeText = (LPSTR)ide->alloc(arg.m_nBufSize + 1);
                arg.m_pBuf = lineCodeText;
                r = _nes_run_fun(FN_GET_PRG_TEXT, &arg, 0);

                //LPSTR lineCodeText = ide->GetCodeText(nLine, 0);

                lineCount = _calc_linecount(lineCodeText, rcMDIClient4.right - rcMDIClient4.left, nLineHeight, 45);
                const char x[] = "※";
                if (i == 0)
                {
                    y1++;
                }
                else if (lineCodeText && lineCodeText[0] == x[0] && lineCodeText[1] == x[1])    // 前面是 ※ 这个符号表示是展开的参数
                {
                    lineFlag = 2;
                    y1 += 3;
                }
                else
                {
                    y1 = y1 + nLineSpacing + 1;
                }
                if (lineFlag != 2 && lineCodeText && lineCodeText[0] == x[0] && lineCodeText[1] == x[1])
                    lineFlag = 2;
                
                lineHeight = lineCount * nLineHeight + (lineCount - 1) * 1;
                if (lineCodeText)
                    ide->free(lineCodeText);
            }
            break;
        }
        default:
            break;
        }
        
        POINT ptCaret = { 0 };
        EIDE_BASE eideBase;
        eideBase.pVariable = (eVariable*)data;
        if (nLine == nNowLine)
        {
            GetCaretPos(&ptCaret);
            //debug("行(%d), type1 = %d, type2 = %d, flags = %d, data = %d, offset = %d, data2 = %d\n", nLine, type1, type2, flags, data, offset, data2);
        }
        y2 = y1 + lineHeight;
        RECT rcNowLine = { 0, y1, rcLine.right, y2 };
        _calc_last_height(type1, type2, offset, rcNowLine.bottom, eideBase);

        if (isDrawLineNumber)  // 不绘画行号, 绘画相关操作都不执行
        {
            COLORREF crText = g_ini.crNormal();
            if (lineFlag == 1)  // 普通
            {
                if (nNowLine == nLine || (nLine >= ptSel.x && nLine <= ptSel.y))  // 热点
                {
                    lineFlag = 3;
                    crText = g_ini.crHot();
                }
            }
            else if (lineFlag == 2) // 展开
            {
                crText = g_ini.crExpand();
            }

            SetTextColor(hdc, crText);
            char text[50];
            _itoa_s(nLine + 1, text, 50, 10);
            DrawTextA(hdc, text, -1, &rcNowLine, DT_VCENTER | DT_SINGLELINE | DT_RIGHT);
            int maxPos   = (int)(LONG_PTR)GetPropW(info.wnd.hMDIClient4, L"maxPos");
            int maxWidth = (int)(LONG_PTR)GetPropW(info.wnd.hMDIClient4, L"maxWidth");
            int defWidth = rcNowLine.right - rcNowLine.left;

            if (defWidth > maxWidth)
                SetPropW(info.wnd.hMDIClient4, L"maxWidth", (HANDLE)defWidth);
            
            if (nLine + 1 > maxPos)
                SetPropW(info.wnd.hMDIClient4, L"maxPos", (HANDLE)(nLine + 1));

        }



        if (ptSel.y - ptSel.x > 0)      // 有选中多行, 不绘画行号线
        {
            m_rcLine = { 0 };
        }
        else if (nNowLine == nLine)     // 当前选中行, 需要根据这个位置绘画行线
        {
            bool isClear = false;   // 为true就需要清空m_rcLine这个变量, 不绘画行号线
            //TODO 需要做一个开关, 用来判断是否在不可编辑的行上需要显示行号线
            // 都显示行号线的话就需要把isClear设置为false
            const int _t[][2] =
            {
                {8, 0},     // 程序集名称/保留/保留/备注 || 类名/基类/公开/备注
                {9, 0},     // 程序集变量名/类型/数组/备注
                {10, 0},    // 子程序名/返回值类型/公开/易包/备注
                {11, 0},    // 子程序参数名/类型/参考/可空/数组/备注
                {12, 0},    // 子程序变量名/类型/静态/数组/备注
                {14, 0},    // 常量名称/常量值/公开/备注
                {1, 0},     // dll命令名/返回值类型/公开/备注
                {2, 0},     // dll库文件名
                {3, 0},     // 在库中对应的命令名
                {4, 0},     // 参数名/类型/传址/数组/备注
                {5, 0},     // 数据类型名/公开/备注
                {6, 0},     // 成员名/公开/备注
                {7, 0},     // 全局变量名/类型/数组/公开/备注

            };
            for (int _i = 0; _i < sizeof(_t) / sizeof(_t[0]); ++_i)
            {
                if (_t[_i][0] == type1 && _t[_i][1] == type2)
                {
                    isClear = true;
                    break;
                }
            }

            isClear = false;
            if (isClear)
            {
                m_rcLine = { 0 };
            }
            else
            {
                m_rcLine = rcNowLine;
                if (lineCount == 1 && abs(ptCaret.y - rcNowLine.top) < nLineHeight)  // 计算出的位置和光标相差字符高度就按计算的结果为准
                {
                    m_rcLine.top = ptCaret.y;
                    m_rcLine.bottom = ptCaret.y + lineHeight;
                }
                ClientToScreen(info.wnd.hMDIClient4, (LPPOINT)&m_rcLine);
                ScreenToClient(g_hEWnd, (LPPOINT)&m_rcLine);

                m_rcLine.right = m_rcLine.left + (rcMDIClient4.right - rcMDIClient4.left) - 1;
                if (g_ini.isBoldLine())
                    m_rcLine.top--;
                m_rcLine.bottom = m_rcLine.top + (rcNowLine.bottom - rcNowLine.top);
                m_rcLine.top -= 2;
                if (type1 == 13 && type2 == 851)
                    m_rcLine.bottom += 4;

            }
        }


        y1 = rcNowLine.bottom + 1;
        if (y1 > rcLine.bottom)   // 顶边已经超过了绘画的底边了, 底边超过的还需要绘画, 顶边超过就不绘画了
            break;
    }

    if (isDrawLineNumber)
    {
        BitBlt(m_md->hdc, 0, 0, rcLine.right, rcLine.bottom, hdc, 0, 0, SRCCOPY);
        BitBlt(hdcWindow, 0, 0, rcLine.right, rcLine.bottom, hdc, 0, 0, SRCCOPY);
    }
    md_destroy(md);
    if (memcmp(&oldRcLine, &m_rcLine, sizeof(RECT)) != 0)
        LineCode_DrawLine();

    return 0;
}
LRESULT CALLBACK EWindow_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
    case WM_WINDOWPOSCHANGED:
    {
        LPWINDOWPOS pos = (LPWINDOWPOS)lParam;
        // 没有这两个标志, 要么改变了尺寸, 要么移动了窗口
        if ((pos->flags & SWP_NOMOVE) == 0 || (pos->flags & SWP_NOSIZE) == 0)
        {
            LineCode_DrawLine();
        }
        break;
    }
    default:
        break;
    }
    return 0;   // 不设置拦截的话, 这个返回值无意义
}

// 绘画背景, 需要把行号线绘画出来
void LineCode_DrawLine()
{
    struct _tmp_showwindow
    {
        bool isShow;
        _tmp_showwindow(bool isShow) : isShow(isShow)
        {

        }
        ~_tmp_showwindow()
        {
            if (m_hWnd && IsWindowVisible(m_hWnd) != isShow)
                ShowWindow(m_hWnd, isShow ? SW_SHOWNA : SW_HIDE);
        }
    };

    _tmp_showwindow _showWindow(false); // 返回时自动显示或隐藏窗口
    if (!g_ini.isEnabled() || !g_ini.isDrawLine() || !m_hMDIClient4)
        return;    // 不启用插件或者不绘画行号线就不往下执行
    
    if (m_rcLine.right - m_rcLine.left <= 0 || m_rcLine.bottom - m_rcLine.top <= 0)
        return;
    
    MDICLIENT_TYPE type;
    HWND hSCroll = 0;
    HWND hMDIClient4 = GetMDIClient4(g_eWnd->GetMDIClient1(), type, &hSCroll);
    if (type != MDICLIENT_TYPE_CODE)
        return;   // 不是代码框不绘画
    
    RECT rcMDI4;
    iDraw_IDCall(IDC_IDRAW_GETCODEVISIBLERECT, 0, (LPARAM)&rcMDI4);

    if (m_rcLine.top >= rcMDI4.bottom)  // 绘画的线顶边大于等于代码框的底边, 那就不需要绘画了
        return;

    _showWindow.isShow = true;  // 设置为显示, 当退出作用域的时候窗口会被显示

    const int cxClient = m_rcLine.right - m_rcLine.left - 20;
    int cyClient = m_rcLine.bottom - m_rcLine.top + 1;

    RECT_F rc;
    rc.left     = (float)1;
    rc.top      = (float)1;
    rc.right    = (float)cxClient - 1;
    rc.bottom   = (float)cyClient - 1;

    if (!IsWindow(m_hWnd))
    {
        const LPCWSTR lpszClassName = L"line_window_shadow";
        WNDCLASSEXW wcex = { 0 };
        wcex.cbSize     = sizeof(WNDCLASSEX);

        wcex.style          = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc    = DefWindowProcW;
        wcex.cbClsExtra     = 0;
        wcex.cbWndExtra     = 0;
        wcex.hInstance      = g_hInst;
        wcex.hIcon          = 0;
        wcex.hCursor        = 0;
        wcex.hbrBackground  = 0;
        wcex.lpszMenuName   = NULL;
        wcex.lpszClassName  = lpszClassName;
        wcex.hIconSm = 0;

        ATOM atom = RegisterClassExW(&wcex);

        m_hWnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT,
            lpszClassName, L"20752843", WS_POPUP | WS_VISIBLE, 0, 0, cxClient, cyClient, g_hEWnd, nullptr, wcex.hInstance, 0);

    }


    if (!m_hPen)
    {
        _create_pen();
    }
    if (!m_hCanvas)
        m_hCanvas = g_dx->_canvas_create(0, cxClient, cyClient);
    if (m_hCanvas->GetWidth() != (UINT)cxClient || m_hCanvas->GetHeight() != (UINT)cyClient)
        m_hCanvas->Resize(cxClient, cyClient);

    m_hCanvas->BeginDraw();
    m_hCanvas->clear(0);

    bool isBold = g_ini.isBoldLine();
    int oldMode = 0;
    if (!isBold)    // 不加粗的话就需要设置不抗锯齿, 默认是抗锯齿, 抗锯齿会加粗
        g_dx->_canvas_SetSmoothingMode(m_hCanvas, 0, &oldMode);
    
    g_dx->_canvas_drawRectangle(m_hCanvas, m_hPen, &rc);

    if (!isBold)
        g_dx->_canvas_SetSmoothingMode(m_hCanvas, oldMode);

    if (m_rcLine.top + cyClient > rcMDI4.bottom)
    {
        // 后面的一半不显示, 设置窗口为可显示的高度, 超出的就不显示
        cyClient = rcMDI4.bottom - m_rcLine.top;
    }

    HDC hdc = m_hCanvas->GetDC();
    RECT rcEWindow;
    GetWindowRect(g_hEWnd, &rcEWindow);
    // 行号线显示的位置
    POINT pt = { m_rcLine.left + rcEWindow.left + 21, m_rcLine.top + rcEWindow.top + 1 };

    UpdateLayered(m_hWnd, cxClient, cyClient, hdc, (LPPOINT)&pt);
    
    m_hCanvas->ReleaseDC();
    m_hCanvas->EndDraw();

    return;
}
DWORD IDRAW_CALL Event_MDIClient(UINT nEvent, LONG_PTR param, LPVOID pArg)
{
    MDICLIENT_CODETYPE codeType;
    switch (nEvent)
    {
    case IRE_TABSELCHANGED_CODE:  // 代码窗口被切换, 需要在这里子类化mdi下第三层窗口绘画行号
    {
        //if (!g_ini.isEnabled() || !g_ini.isDrawLineNumber()) return 0;
        LPIRE_STRUCT_TAB_SELCHANGE itsc = (LPIRE_STRUCT_TAB_SELCHANGE)pArg;
        MDICLIENT_TYPE type;
        HWND hSCroll = 0;
        m_hMDIClient4 = GetMDIClient4(itsc->hWnd, type, &hSCroll);
        auto pfn_timer = [](HWND hWnd, UINT message, UINT_PTR id, DWORD t)
        {
            KillTimer(hWnd, id);
            SetWindowPos(m_hMDIClient4, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME);
        };
        if (m_hMDIClient4)
            SetTimer(m_hMDIClient4, (UINT_PTR)&pfn_timer, 30, pfn_timer);
        LineCode_DrawLine();
        if (type != MDICLIENT_TYPE_CODE)
            break;
        if (m_setMDIClient4.find(m_hMDIClient4) != m_setMDIClient4.end())
            break;  // 有值就表示已经注册过消息了
        

        LPOBJSTRUCT pData = (LPOBJSTRUCT)iDraw_IDCall(IDC_IDRAW_GETWINDOWDATA, 0, (LPARAM)m_hMDIClient4);
        iDraw_IDCall(IDC_SCROLL_GETRECT , (WPARAM)callback_fnGetScrollRect  , (LPARAM)pData);
        iDraw_IDCall(IDC_SCROLL_CALCRECT, (WPARAM)callback_fnCalcScroll     , (LPARAM)pData);
        iDraw_IDCall(IDC_SCROLL_DRAW    , (WPARAM)callback_fnDrawScroll     , (LPARAM)pData);


        m_setMDIClient4.insert(m_hMDIClient4);
        break;
    }
    case IRE_MDICLIENT_DESTROY: // MDI窗口被销毁, 需要释放一些数据
    {
        if (!isCodeType(codeType))  // 只处理代码窗口
            break;
        HWND hWnd = (HWND)pArg;
        MDICLIENT_TYPE type;
        m_hMDIClient4 = GetMDIClient4(hWnd, type);
        if (type != MDICLIENT_TYPE_CODE)
            break;
        m_setMDIClient4.erase(m_hMDIClient4);    // 删除记录的窗口
        m_hMDIClient4 = 0;
        break;
    }
    default:
        break;
    }
    return 0;
}
DWORD CALLBACK EIDE_AutoHide(UINT nEvent, LONG_PTR param, LPVOID pArg)
{
    if (!g_ini.isEnabled() || !g_ini.isDrawLine()) return 0;
    bool bAsync = true;
    if (param == 2)         // 尺寸被改变了
    {
        bAsync = false;
    }
    else if (param == 1)    // 被显示了
    {

    }
    LineCode_DrawLine();
    return 0;
}

// 易语言行选中被改变回调
DWORD CALLBACK EIDE_SelChangeCallback(UINT nEvent, LONG_PTR param, LPVOID pArg)
{
    IRE_STRUCT_EIDE_SELCHANGEDINFO* info = (IRE_STRUCT_EIDE_SELCHANGEDINFO*)pArg;
    if (!info->pCodeManage)
        return 0;
    IEIDE_CodeManage& ide = *info->pCodeManage;
    //if (info->oldLine != info->line)
    //{
    //    int pStartCol = 0, pEndCol = 0, pNowLine = 0;
    //    POINT pSel = { 0 };
    //    ide.GetSelInfo(&pStartCol, &pEndCol, &pNowLine, &pSel);
    //    char __[2] = { 0 };
    //    int pFlags = 0, pData = 0;
    //    int lintInfo = ide.GetLineInfo(info->line, &pFlags, &pData);
    //    POINT pt1 = ide.GetCursorTablePos();
    //    LPSTR text = ide.GetCodeText(pt1.x, pt1.y);
    //    if (!text)text = __;
    //    debug(
    //        "hWnd = %d, 当前行 = %d, 旧行列 = {%d,%d}, 选中信息, 起始行列 = {%d,%d},"
    //        "pNowLine = %d, 选中范围 = {%d,%d}, IsSelCode = %d"
    //        "lintInfo = %d, pFlags=%d, pData=%d, GetCodeType = %d, 光标表位置 = {%d,%d}, %s\n"
    //        , ide.GetWindow(), info->line, info->oldLine, info->oldLineCol, pStartCol, pEndCol,
    //        pNowLine, pSel, ide.IsSelCode(),
    //        lintInfo, pFlags, pData, ide.GetCodeType(), pt1, text);
    //    if (text != __)
    //        ide.free(text);
    //}

    //POINT pt = ide.GetCursorPos();
    //if (pt.x == info->line && pt.y == info->oldLineCol) // 两次点击都是一个位置, 不处理
    //    return 0;

    HWND hMDIClient4 = ide.GetWindow();
    if (hMDIClient4)
    {
        HDC hdc = GetWindowDC(hMDIClient4);
        Scroll_DrawScroll(hdc);
        ReleaseDC(hMDIClient4, hdc);
        //LineCode_DrawLine();
        //POINT pt = ide.GetCursorPos();
        //if (!(pt.x == info->line && pt.y == info->oldLineCol))
        //{
        //    _str str = "123456789asd";
        //    _nes_run_fun(FN_INSERT_TEXT, str.data(), false);
        //}
    }
    return 0;
}

#define __FINDARG(_arr) _arr, sizeof(_arr) / sizeof(_arr[0])
// 查找易语言内存数据, 返回找到的内存地址, 如果没找到返回0
// findData = 要查找的数据
// findDataSize = 要查找的数据的长度
inline LPBYTE EIDE_FindAddress(const void* findData, int findDataSize)
{
    static LPBYTE pBegin = (LPBYTE)GetModuleHandleW(0);
    static LPBYTE pEnd = pBegin + 2097152;
    for (LPBYTE p = (LPBYTE)pBegin; p != pEnd; p++)
    {
        if (memcmp(p, findData, findDataSize) == 0)
            return p;
    }
    return 0;
}



extern "C" _declspec(dllexport) void* WINAPI IDraw_Interface(LPIDRAW_INFO pInfo, IGetEWindow * eWnd, int reserved1, int reserved2)
{
    int major = 0, minor = 0, build = 0;
    iDraw_GetVersion(&major, &minor, &build);
    if (minor == 1 && minor < 5)
        return 0;   // 1.5以下版本不支持

    m_EVersion = iDraw_GetEVersion();
    m_isDrawLine = m_EVersion < 593 || m_EVersion == 5922;
    if (!m_isDrawLine)
        return 0;   // 暂时还不支持5.93或以上版本, 等后续能获取到代码行的矩形再说


    g_info = pInfo;
    g_eWnd = eWnd;
    g_dx = iDraw_GetDxObject();
    g_ini.init();

    static PLUGIN_INFO pluginInfo = { 0 };
    g_hEWnd = (HWND)iDraw_GetInfo(g_info, IGI_EWINDOW, 0);
    m_md = md_create(300, 3000);

    pluginInfo.cbSize = sizeof(pluginInfo);
    pluginInfo.name = "易代码行号插件";
    pluginInfo.remarks = "给易语言代码框增加行号显示";
    pluginInfo.version = "1.0";
    pluginInfo.author = "梦寻简单 - 由项目部006改写";
    pluginInfo.Versionlimit = 0;
    pluginInfo.Dependence = 0;


    pluginInfo.guid = DLL_GUID;
    pluginInfo.pfnMenuExt_Command = IDraw_MenuCommand;
    pluginInfo.pfnCallback = IDraw_NotifyCallback;
    iDraw_InitPlugin(pluginInfo.guid, g_hInst, &pluginInfo, IDraw_MenuCommand, IDraw_NotifyCallback); // 初始化插件, 这里需要加入扩展菜单


    UINT uFlags = MF_BYPOSITION | (g_ini.isEnabled() ? MF_CHECKED : MF_UNCHECKED) | (m_isDrawLine ? MF_ENABLED : MF_DISABLED);
    const LPCWSTR menuName = m_isDrawLine ? L"启用/禁用" : L"启用/禁用\t暂不支持5.93";
    iDraw_MenuExt_Insert(DLL_GUID, -1, uFlags, IDMENU_ENABLED, menuName, 0);
    //iDraw_MenuExt_Insert(DLL_GUID, -1, MF_BYPOSITION | MF_SEPARATOR, -1, 0, 0);
    //iDraw_MenuExt_Insert(DLL_GUID, -1, MF_BYPOSITION, IDMENU_SETTING, L"设置", 0);

    if (!m_isDrawLine)
    {
        g_ini.m_isEnabled = false;  // 设置为不启用
    }
    else
    {
        iDraw_Event_RegisterEvent(DLL_GUID, IRE_TABSELCHANGED_CODE, 0, 0, 0, Event_MDIClient);      // 代码窗口被切换, 需要在这里子类化mdi下第三层窗口绘画行号
        iDraw_Event_RegisterEvent(DLL_GUID, IRE_MDICLIENT_DESTROY, 0, 0, 0, Event_MDIClient);       // MDI被销毁调用后注册, 调用前没必要注册
        //iDraw_Event_RegisterEvent(DLL_GUID, IRE_DRAW_BCAKGROUND, 0, 0, 0, LineCode_DrawLine);    // 绘画背景事件, 在调用后处理
        iDraw_Event_RegisterEvent(DLL_GUID, IRE_ECODESELCHANGED, 0, EIDE_SelChangeCallback, 0, 0);  // 行号选择被改变

        iDraw_Event_RegisterEvent(DLL_GUID, IRE_DRAW_AUTOHIDE_HIDE, 0, 0, 0, EIDE_AutoHide);        // 自动隐藏的窗口被隐藏了
        iDraw_Event_RegisterEvent(DLL_GUID, IRE_DRAW_AUTOHIDE_SHOW, 0, 0, 1, EIDE_AutoHide);        // 自动隐藏的窗口被显示了
        iDraw_Event_RegisterEvent(DLL_GUID, IRE_DRAW_AUTOHIDE_MOVE, 0, 0, 2, EIDE_AutoHide);        // 自动隐藏的窗口被移动
        iDraw_Event_RegisterMessage(DLL_GUID, g_hEWnd, WM_WINDOWPOSCHANGED, EWindow_Proc, 0);       // 窗口尺寸改变事件
    }

    // 需要注册MDI下第二层窗口的绘画消息, 需要获取到MDI插件即将被创建
    return &pluginInfo;
}

extern "C" __declspec(dllexport) void CALLBACK IDraw_UnInterface()
{
    md_destroy(m_md);
    m_md = 0;
    g_dx->_canvas_destroy(m_hCanvas);
    g_dx->_pen_destroy(m_hPen);
    if (IsWindow(m_hWnd))
        DestroyWindow(m_hWnd);
    m_hWnd = 0;
}

// 更新分层窗口, 返回错误码, 0=成功
// hWnd = 要重画的窗口
// width = 窗口宽度
// height = 窗口高度
// srcDC = 从哪个dc拷贝到窗口上
// ppt = 窗口新的位置
// alpha = 窗口透明度
int UpdateLayered(HWND hWnd, int width, int height, HDC srcDC, PPOINT ppt, int alpha)
{
    int ret = 0;
    HDC hdc = ::GetDC(hWnd);
    SIZE size = { width, height };
    POINT pt = { 0 };
    BLENDFUNCTION blend = { 0 };
    blend.BlendOp = AC_SRC_OVER;
    blend.BlendFlags = 0;
    blend.AlphaFormat = AC_SRC_ALPHA;
    blend.SourceConstantAlpha = (BYTE)alpha;

    typedef BOOL(WINAPI* pfn_UpdateLayeredWindow)(
        HWND hWnd,
        HDC hdcDst,
        POINT* pptDst,
        SIZE* psize,
        HDC hdcSrc,
        POINT* pptSrc,
        COLORREF crKey,
        BLENDFUNCTION* pblend,
        DWORD dwFlags);
    static pfn_UpdateLayeredWindow fun;
    if (!fun)
    {
        HMODULE hModule = GetModuleHandleW(L"user32.dll");
        if (!hModule)throw;
        fun = (pfn_UpdateLayeredWindow)GetProcAddress(hModule, "UpdateLayeredWindow");
    }
    if (!fun)
        throw;
    BOOL b = fun(hWnd, hdc, ppt, &size, srcDC, &pt, 0, &blend, ULW_ALPHA);
    if (!b)
        ret = GetLastError();
    ReleaseDC(hWnd, hdc);
    return ret;
}