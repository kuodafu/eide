#include "../EWindow_Fne_Header.h"
#include <control/CCombobox.h>
#include <hook_detours/apiHook.h>



struct COMBOBOX_LIST_DATA
{
    LPOBJSTRUCT pData;  // 这个是组合框的数据
    bool isCaptrue;     // 是否允许捕获鼠标, 如果为假, 则 SetCapture 不执行
};
std::map<HWND, COMBOBOX_LIST_DATA> m_map_combobox_list;    // 存放组合框列表相关数据

LRESULT CALLBACK WndProc_ComboBox(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_ComboBox_List(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc_ComboBox_CtlColor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_ComboBox_Command(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_ComboBox_OnDrawItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_ComboBox_OnMeasureItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
void CALLBACK WndProc_ComboBox_OnPaint(HWND hWnd, HDC hdc, LPOBJSTRUCT pData, RECT& rc, const int cxClient, const int cyClient);
LRESULT CALLBACK WndProc_ComboBox_OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

// 重画指定项目
void InvalidateItem(LPOBJSTRUCT pData);


static WNDPROC s_oldProcList;   // 框列表默认消息处理函数
static apiHook hook_SetCapture;
static apiHook hook_ReleaseCapture;
static HWND(WINAPI* SetCapture_hook)(HWND hWnd) = ::SetCapture;
static BOOL(WINAPI* ReleaseCapture_hook)() = ::ReleaseCapture;
static ULONGLONG m_time_down;   // 弹出列表的时间, 鼠标放开和弹出的时间间隔低于300毫秒不处理
static BOOL m_isNoReleaseCapture;   // 是否不让执行释放操作
static HWND WINAPI MySetCapture_hook(HWND hWnd)
{
    auto it = m_map_combobox_list.find(hWnd);
    if (it != m_map_combobox_list.end())
    {
        //COMBOBOX_LIST_DATA& data = it->second;
        //if (!data.isCaptrue)    // 不允许捕获
        //    return hWnd;
    }
    return SetCapture_hook(hWnd);
}
static BOOL WINAPI MyReleaseCapture_hook()
{
    //if( m_isNoReleaseCapture )
}

// 即将弹出列表
// hComboBox = 组合框句柄
// pData = 组合框的数据
void Combobox_DropDown(HWND hComboBox, LPOBJSTRUCT pData);
inline void WINAPI _cbx_crChange(LPOBJSTRUCT pData)
{
    pData->draw.color.crBackGround = s_info->themeColor->crBack();
    pData->draw.color.hbrBackGround = s_info->pfnCreateBrush(pData->draw.color.crBackGround);
}
LPOBJSTRUCT _subclass_combobox(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int nItemHeight)
{
    if (nItemHeight <= 10)
        nItemHeight = 24;


    static LPEX_FONT hFont = 0;
    if (!hFont)
    {
        hFont = s_info->pfnCreateFontDx(L"宋体", -12, FONTSTYLE::FontStyleRegular);
        //hook_SetCapture.hook(&(PVOID&)SetCapture_hook, MySetCapture_hook);

    }
    
    LPOBJSTRUCT pData = _subclass(hWnd, WndProc_ComboBox, true, false, 0);
    if (!pData)return 0;

    //Marlett
    LOGFONTW lf = { 0 };
    int lfHeight = s_info->dpiX == 96 ? -18 : s_info->scale(-18);
    lf.lfHeight              = lfHeight	; // 字符高度
    lf.lfWidth               = lfHeight; // 字符平均宽度
    lf.lfEscapement          = 0	; // 转义向量与设备的 x 轴之间的角度
    lf.lfOrientation         = 0	; // 每个字符的基线和设备 x 轴之间的角度
    lf.lfWeight              = 500	; // 字体的粗细, 400=正常, 700=加粗
    lf.lfItalic              = 0	; // 是否是斜体
    lf.lfUnderline           = 0	; // 是否有下划线
    lf.lfStrikeOut           = 0	; // 是否有删除线
    lf.lfCharSet             = 2	; // 字符集
    lf.lfOutPrecision        = 3	; // 输出精度
    lf.lfClipPrecision       = 2	; // 剪裁精度
    lf.lfQuality             = 1	; // 输出质量
    lf.lfPitchAndFamily      = 2	; // 字体的间距和系列
    wcscpy_s(lf.lfFaceName, L"Marlett");    // 字体名

    pData->draw.hFontBold = s_info->pfnCreateFontLfDx(&lf);

    HWND hEdit = GetWindow(hWnd, GW_CHILD);
    if (hEdit)
        _subclass_Edit(hEdit, false, false, 0, false);
    
    InsertStyle(pData->hWnd, CBS_OWNERDRAWFIXED | WS_CLIPSIBLINGS);
    pData->draw.fmt         = DT_VCENTER | DT_SINGLELINE;
    pData->fnDrawItem       = WndProc_ComboBox_OnDrawItem;
    pData->fnMeasureItem    = WndProc_ComboBox_OnMeasureItem;
    pData->fnCommand        = WndProc_ComboBox_Command;
    pData->fnNotify         = WndProc_ComboBox_OnNotify;
    pData->fnCtlcolor       = WndProc_ComboBox_CtlColor;
    pData->draw.isErasebkg  = true;
    pData->draw.nItemHeight = nItemHeight;

    pData->fnCrChange = _cbx_crChange;
    _cbx_crChange(pData);
    //pData->draw.hFont = hFont;



    pData->SetClsLong(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)pData->draw.color.hbrBackGround);
    pData->SetWndLong(pData->hWnd, GWL_STYLE, WS_TABSTOP | WS_VISIBLE | WS_CHILDWINDOW | WS_CHILD | CBS_HASSTRINGS | CBS_DROPDOWNLIST);

    COMBOBOXINFO cbi = { 0 };
    cbi.cbSize = sizeof(cbi);
    GetComboBoxInfo(pData->hWnd, &cbi);
    assert(cbi.hwndList && "获取组合框列表框句柄失败");
    if (!cbi.hwndList) return pData;
    pData->param = cbi.hwndList;
    WNDPROC oldProc = (WNDPROC)pData->GetWndLong(cbi.hwndList, GWLP_WNDPROC);
    //_subclass_ListBox(cbi.hwndList, false, true, 0, nItemHeight, true, true);

    if (oldProc != WndProc_ComboBox_List)
    {
        if (1)
        {
            pData->drawScroll = ScrollCreate(cbi.hwndList, pData->draw.color.crBorder,
                                             0,          // m_isSysListView, 如果是超级列表框的话这个值就是超级列表框的表项高度
                                             false,      // m_isSysTreeView, 是否为树型框, 如果为真, 则先设置滚动条位置, 后发送滚动消息, 为假则先发送滚动消息后设置位置
                                             true,       // m_isCallSetScroll, 是否调用原来的设置滚动条位置函数
                                             false,      // isSubClass, 是否子类化, 如果为true, 内部会子类化这个窗口, 如果为假, 则必须外部把消息传递到 OnWndProc 方法里
                                             true        // isReCalcSize, 是否重新计算尺寸, 为true则内部会调用 SetWindowPos 进行重新计算非客户区
            );
        }

        if (!s_oldProcList)
        {
            WNDCLASSEXA wcex;
            wcex.cbSize = sizeof(wcex);
            GetClassInfoExA(CWndBase::GetInst(), "ListBox", &wcex);
            s_oldProcList = wcex.lpfnWndProc;
        }
        
        SendMessageW(pData->hWnd, CB_SETITEMHEIGHT, 0, s_info->scale(pData->draw.nItemHeight));
        oldProc = (WNDPROC)pData->SetWndLong(cbi.hwndList, GWLP_WNDPROC, (LONG_PTR)WndProc_ComboBox_List);
        DWORD style = WS_CLIPSIBLINGS | WS_CHILD | LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | LBS_NODATA | WS_VSCROLL | LBS_COMBOBOX;
        style = WS_CLIPSIBLINGS | WS_CHILD | LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | LBS_NODATA;
        style = pData->GetWndLong(cbi.hwndList, GWL_STYLE);
        DWORD styleEx = pData->GetWndLong(cbi.hwndList, GWL_EXSTYLE);
        //SetWindowLongPtrW(cbi.hwndList, GWL_STYLE, style);

        styleEx &= ~WS_EX_CLIENTEDGE;
        styleEx &= ~WS_EX_WINDOWEDGE;
        styleEx &= ~WS_EX_STATICEDGE;
        styleEx &= ~WS_EX_DLGMODALFRAME;

        style &= ~WS_BORDER;
        style &= ~WS_SIZEBOX;

        pData->SetWndLong(cbi.hwndList, GWL_EXSTYLE, styleEx);
        pData->SetWndLong(cbi.hwndList, GWL_STYLE, style);

        pData->param1 = oldProc;
        SetPropW(cbi.hwndList, L"oldProc", pData);
    }


    return pData;
}

// 释放组合框数据数组, 如果没有创建数组, 内部会创建
// pArr = 数组指针, 如果为0, 则内部会创建一个
// isDelete = 是否删除pArr, 为假则只释放数组里的数据
inline void __free_arr(std::vector<LPWSTR>*& pArr, bool isDelete)
{
    if (!pArr) pArr = new std::vector<LPWSTR>;
    for (auto it = pArr->begin(); it != pArr->end(); ++it)
    {
        if (*it)delete[] * it;
    }
    pArr->clear();
    if (isDelete)delete pArr;
}

inline Gdiplus::Bitmap* _src_get_combobox_image_gp()
{
    static Gdiplus::Bitmap* hImage;
    if (!hImage)
    {
        const unsigned char pData[] =
        {
            0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x14,
            0x00, 0x00, 0x00, 0x14, 0x08, 0x06, 0x00, 0x00, 0x00, 0x8D, 0x89, 0x1D, 0x0D, 0x00, 0x00, 0x00, 0x96, 0x49, 0x44, 0x41,
            0x54, 0x38, 0x4F, 0x63, 0x64, 0xA0, 0x32, 0x60, 0xA4, 0xB2, 0x79, 0x0C, 0xA3, 0x06, 0x52, 0x1E, 0xA2, 0xA3, 0x61, 0x48,
            0xE5, 0x30, 0x94, 0xE9, 0x7B, 0xCC, 0xF9, 0x8D, 0xE3, 0xFB, 0x4E, 0x46, 0x06, 0xC6, 0x0F, 0xC4, 0x18, 0xFD, 0x9F, 0xE1,
            0xBF, 0x00, 0xD7, 0x0F, 0x4E, 0xF7, 0x27, 0x45, 0xB2, 0xDF, 0x61, 0xEA, 0x31, 0x22, 0x45, 0x64, 0xEE, 0x0D, 0xDE, 0x7F,
            0x3F, 0x99, 0xB6, 0x32, 0x30, 0x30, 0xDA, 0xE2, 0x37, 0xF4, 0xFF, 0x61, 0x26, 0xF6, 0x7F, 0xDE, 0x6F, 0x92, 0x35, 0x3E,
            0x23, 0xAB, 0xC3, 0x1A, 0xCB, 0x84, 0x0D, 0xC5, 0x6E, 0x18, 0xC8, 0x60, 0x9C, 0xC9, 0x06, 0xB7, 0xA1, 0xB8, 0x0D, 0xC3,
            0x6B, 0x20, 0x48, 0x12, 0xD3, 0x50, 0xFC, 0x86, 0x11, 0x34, 0x10, 0xD5, 0x50, 0x06, 0x06, 0x6C, 0x61, 0x86, 0x1E, 0xCE,
            0x44, 0xE5, 0x14, 0x90, 0x4B, 0x41, 0x1A, 0xD1, 0x23, 0x00, 0x5B, 0xA4, 0x11, 0x65, 0x20, 0x31, 0x49, 0x08, 0x67, 0xB2,
            0x21, 0x45, 0x33, 0x5D, 0x5C, 0x08, 0x00, 0x07, 0x6D, 0x3F, 0x15, 0xA8, 0xF4, 0x57, 0xA2, 0x00, 0x00, 0x00, 0x00, 0x49,
            0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82,
        };
        hImage = _img_create_from_memory(pData, sizeof(pData) / sizeof(pData[0]));
    }
    return hImage;
}

inline void __draw_combobox(HWND hWnd, HDC hdcSrc, LPOBJSTRUCT pData)
{
    bool isRelease = false;
    if (hWnd == (HWND)hdcSrc)
    {
        hdcSrc = GetDC(hWnd);
        isRelease = true;
    }
    RECT rc;
    GetWindowRect(hWnd, &rc);
    const int cxClient = rc.right - rc.left;
    const int cyClient = rc.bottom - rc.top;
    if (cyClient == 20 && s_info->dpiX != 96)
    {
        SendMessageA(hWnd, CB_SETITEMHEIGHT, -1, s_info->scale(cyClient) - 6);
        return;
    }
    rc.left = 0, rc.top = 0, rc.right = cxClient, rc.bottom = cyClient;
    //const int cyClient = (int)SendMessageA(hWnd, CB_GETITEMHEIGHT, -1, 0) + 6;
    const LPEX_FONT pFont = g_scale.GetFont(pData->draw.hFont ? pData->draw.hFont : s_info->defData->draw.hFont);    // 绘画字体
    const HFONT hFont = pFont->GetFont();    // 绘画字体

    LPMEMDCBITMAP md = md_create(cxClient, cyClient);
    assert(md);
    HDC hdc = md->hdc;  // 如果md=空, 就让他崩溃


    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, pData->draw.color.crText);
    SelectObject(hdc, hFont);
    SelectObject(hdc, pData->draw.color.hbrBackGround);

    FillRect(hdc, &rc, pData->draw.color.hbrBackGround);

    WndProc_ComboBox_OnPaint(hWnd, hdc, pData, rc, cxClient, cyClient);

    BitBlt(hdcSrc, 0, 0, cxClient, cyClient, hdc, 0, 0, SRCCOPY);
    md_destroy(md);
    if (isRelease)
        ReleaseDC(hWnd, hdcSrc);
}

LRESULT CALLBACK WndProc_ComboBox(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    if (message == 641)return 0;
    //if (!(message >= CB_GETEDITSEL && message <= CB_MSGMAX || message == 512 || message == 32 || message == 132 || message == 49958))
    //if (!(message == 512 || message == 32 || message == 132 || message == 49958))
    //    wstr::dbg(L"WndProc_ComboBox message = %d\n", message);
    //wstr::dbg(L"WndProc_ComboBox message = %d\n", message);
    switch (message)
    {
    case WM_SIZE:
    {
        int cy = HIWORD(lParam);
        int cx = LOWORD(lParam);
        //if ((hWnd == s_info->eWnd->GetMenuComboBox1() || hWnd == s_info->eWnd->GetMenuComboBox2()) && cy < 25 && s_info->dpiX != 96)
        //{
        //    SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, cx, 600, SWP_DRAWFRAME | SWP_NOMOVE);
        //    SendMessageA(hWnd, CB_SETITEMHEIGHT, -1, s_info->scale(cy) - 6);
        //}
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        __draw_combobox(hWnd, ps.hdc, pData);
        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_PRINT:
        break;
        return DefWindowProcA(hWnd, message, wParam, lParam);
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);

    case WM_PRINTCLIENT:
        __draw_combobox(hWnd, (HDC)wParam, pData);
        break;
    case WM_NCPAINT:
        break;
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    case WM_NCACTIVATE:
        break;
    case WM_COMPAREITEM:
    {
        LRESULT ret = CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
        const int id = (int)wParam;
        LPCOMPAREITEMSTRUCT p = (LPCOMPAREITEMSTRUCT)lParam;


        // 返回值 -1=项目1按排序顺序位于项目2之前
        // 0=项目1和2在排序顺序上是等效的
        // 1=项目1按排序顺序紧随项目2之后
        return ret;
    }
    case WM_CTLCOLORLISTBOX:
    {
        //LRESULT ret = CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
        __draw_combobox(hWnd, (HDC)hWnd, pData);
        if (pData->param)InvalidateRect((HWND)pData->param, 0, 0);
        //return ret;
        return (LRESULT)pData->draw.color.hbrBackGround;
    }
    case WM_ERASEBKGND:
    {
        //__draw_combobox(hWnd, (HDC)wParam, pData);
        return 1;
    }
    case WM_DESTROY:
    {
        std::vector<LPWSTR>* pArr = (std::vector<LPWSTR>*)pData->param2;
        __free_arr(pArr, true);
        pData->param2 = 0;
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case CB_SETITEMHEIGHT:
    {
        if (lParam < 10)lParam = 24;
        //pData->draw.nItemHeight = (int)lParam;
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_STYLECHANGING:
    {
        LPSTYLESTRUCT ss = (LPSTYLESTRUCT)lParam;
        ss->styleNew |= CBS_OWNERDRAWFIXED;
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_LBUTTONDOWN:
        __draw_combobox(hWnd, (HDC)hWnd, pData);
        //return DefWindowProcA(hWnd, message, wParam, lParam);
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    case WM_LBUTTONUP:
        __draw_combobox(hWnd, (HDC)hWnd, pData);
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
        //ReleaseCapture();
        break;
    case WM_NCCALCSIZE:
    {
        if (wParam)
        {
            LPNCCALCSIZE_PARAMS cs = (LPNCCALCSIZE_PARAMS)lParam;
            memcpy(&cs->rgrc[1], &cs->rgrc[0], sizeof(cs->rgrc[0]));
            memcpy(&cs->rgrc[2], &cs->rgrc[0], sizeof(cs->rgrc[0]));
            return 0;
            return WVR_ALIGNTOP | WVR_ALIGNLEFT | WVR_ALIGNBOTTOM | WVR_ALIGNRIGHT;

            return WVR_REDRAW;
        }
        LPRECT prc = (LPRECT)lParam;
        return 0;
    }
    case WM_NCHITTEST:
        return HTCLIENT;
    case CB_SHOWDROPDOWN:
    {
        __draw_combobox(hWnd, (HDC)hWnd, pData);
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_SETFOCUS:
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);

    case WM_KILLFOCUS:
        //__draw_combobox(hWnd, (HDC)hWnd, pData);
        //return DefWindowProcA(hWnd, message, wParam, lParam);
        //break;
    case CB_SETCURSEL:
    {
        LRESULT ret = CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
        InvalidateRect(hWnd, 0, 0);
        __draw_combobox(hWnd, (HDC)hWnd, pData);
        return ret;
    }
    default:
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK WndProc_ComboBox_OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    return 0;
}
LRESULT CALLBACK WndProc_ComboBox_CtlColor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    const HDC hdc = (HDC)wParam;
    const HWND hControl = (HWND)lParam;
    LPOBJSTRUCT pCtlData = GetDataFromHwnd(hControl);
    if (!pCtlData)
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);

    InvalidateRect(hControl, 0, 0);
    return (LRESULT)pCtlData->draw.color.hbrBackGround;
}

LRESULT CALLBACK WndProc_ComboBox_OnMeasureItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    LPDRAWITEMSTRUCT dw = (LPDRAWITEMSTRUCT)lParam;
    HDC hdc = dw->hDC;
    RECT& rc = dw->rcItem;
    FillRect(hdc, &rc, pData->draw.color.hbrBackGround);


    return 0;
}

LRESULT CALLBACK WndProc_ComboBox_OnDrawItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    LPMEASUREITEMSTRUCT item = (LPMEASUREITEMSTRUCT)lParam;

    return 0;
}

class _LOAD_FONT
{
public:
    HANDLE handle;
    _LOAD_FONT(LPBYTE pData, int size)
    {
        DWORD nFontIndex = 0;
        handle = AddFontMemResourceEx(pData, size, NULL, &nFontIndex);
    }
    ~_LOAD_FONT()
    {
        RemoveFontMemResourceEx(handle);
    }
    static HFONT _create_font(LPCWSTR name, int size)
    {
        LOGFONTW lf = { 0 };
        lf.lfHeight = size;
        lf.lfWidth = size;
        //lf.lfEscapement          = 0	;
        //lf.lfOrientation         = 0	;
        //lf.lfWeight              = 400;
        //lf.lfItalic              = 0	;
        //lf.lfUnderline           = 0	;
        //lf.lfStrikeOut           = 0	;
        //lf.lfCharSet             = 0	;
        //lf.lfOutPrecision        = 3	;
        //lf.lfClipPrecision       = 2	;
        //lf.lfQuality             = 1	;
        //lf.lfPitchAndFamily      = 49	;
        wcscpy_s(lf.lfFaceName, name);
        return CreateFontIndirectW(&lf);
    }
    static HFONT get(LPCWSTR name, int size)
    {
        static std::unordered_map<int, HFONT> map;
        HFONT& hFont = map[size];
        if (!hFont)
            hFont = _create_font(name, size);
        return hFont;
    }

};



static HFONT GetMarlettFont(int size)
{
    int nSize = 0;
    LPBYTE pData = s_info->pResource->GetMarlettData(&nSize);
    static _LOAD_FONT _font(pData, nSize);
    return _LOAD_FONT::get(L"Marlett", size);
}

void CALLBACK WndProc_ComboBox_OnPaint(HWND hWnd, HDC hdc, LPOBJSTRUCT pData, RECT& rc, const int cxClient, const int cyClient)
{
    const int len = (int)(LONG_PTR)SendMessageW(hWnd, CB_GETCOUNT, 0, 0);
    const int index = (int)(LONG_PTR)SendMessageW(hWnd, CB_GETCURSEL, 0, 0);
    wstr text(0x1000);
    
    HPEN hOldPen = (HPEN)SelectObject(hdc, pData->draw.color.hpenBorder);
    Rectangle(hdc, 0, 0, cxClient, cyClient);
    SelectObject(hdc, hOldPen);

    Gdiplus::Bitmap* hImage = _src_get_combobox_image_gp();
    const UINT cxIcon = hImage->GetWidth();
    const UINT cyIcon = hImage->GetHeight();
    const int offset4 = s_info->scale(4);

    Gdiplus::Graphics gp(hdc);
    Gdiplus::Rect rcDraw;
    const int cyIconScale = s_info->scale(cyIcon);
    const int offset = (cyClient - cyIconScale) / 2;
    rcDraw.X        = rc.right - cyClient  + offset;
    rcDraw.Y        = rc.top   + offset - offset4;
    rcDraw.Width    = cyIconScale;
    rcDraw.Height   = cyIconScale;
    const int scale_height = s_info->scale(cyClient);
    RECT rcDrawImage = { cxClient - offset - scale_height, 0, cxClient, scale_height };
    HWND hWndList = (HWND)pData->param;


    //HFONT hFontImage = s_info->pResource->GetMarlettFont(s_info->scale(-12));
    HFONT hFontImage = GetMarlettFont(s_info->scale(-12));

    LPCWSTR pszDrawImage = L"5"; // 5=向上,展开状态, 6=向下三角, 收缩状态
    if ( GetPropW(hWnd, L"DropDown") == (HANDLE)1 || IsWindowVisible(hWndList) )
    {
        pszDrawImage = L"5";
        if (!hFontImage)
        {
            gp.TranslateTransform((float)cxClient, (float)s_info->scale(21));
            //gp.TranslateTransform((float)0, (float)0);
            gp.RotateTransform(180);
            rcDraw.X = 0;
            rcDraw.Y = 0;
            gp.DrawImage(hImage, rcDraw, 0, 0, cxIcon, cyIcon, Gdiplus::Unit::UnitPixel);
            gp.ResetTransform();
        }
    }
    else
    {
        pszDrawImage = L"6";
        if (!hFontImage)
        {
            gp.DrawImage(hImage, rcDraw, 0, 0, cxIcon, cyIcon, Gdiplus::Unit::UnitPixel);
        }
    }
    
    if (hFontImage)
    {
        COLORREF clrTextOld = SetTextColor(hdc, s_info->themeColor->crBorder());
        HGDIOBJ hOldFont = SelectObject(hdc, hFontImage);
        DrawTextW(hdc, pszDrawImage, 1, &rcDrawImage, DT_VCENTER | DT_SINGLELINE | DT_CENTER);
        SelectObject(hdc, hOldFont);
        SetTextColor(hdc, clrTextOld);
    }

    const int textLen = (int)SendMessageW(hWnd, CB_GETLBTEXTLEN, (WPARAM)(int)(index), 0) + 1;
    if (textLen == 1)
        return;
    text.resize(textLen);
    SendMessageW(hWnd, CB_GETLBTEXT, (WPARAM)(int)(index), (LPARAM)text.data());
    rc.left += offset4;
    if (hWnd == s_info->eWnd->GetMenuComboBox1() || hWnd == s_info->eWnd->GetMenuComboBox2())
    {
        LPCWSTR pszID = s_info->eWnd->GetMenuComboBox1() == hWnd ? L"1" : L"2";
        RECT_F rcIconDraw;

        LPWINDOW_ICON_DATA iconData = s_info->pResource->Combobox_GetIcon(pszID, &rcIconDraw, STATE::NORMAL, s_info->dpiX);
        if (iconData)
        {
            int cxIcon = (int)iconData->cxIcon;
            int cyIcon = (int)iconData->cyIcon;

            const int cyIconScale = s_info->scale(cyIcon);
            const int offset = (cyClient - cyIconScale) / 2;
            Gdiplus::RectF rcDraw;
            rcDraw.X        = (float)(rc.left + offset);
            rcDraw.Y        = (float)(rc.top  + offset);
            rcDraw.Width    = (float)(cyIconScale);
            rcDraw.Height   = (float)(cyIconScale);

            gp.DrawImage(iconData->hBitmapGdip, rcDraw, rcIconDraw.left, rcIconDraw.top,
                rcIconDraw.width(), rcIconDraw.height(), Gdiplus::Unit::UnitPixel);
            rc.left += cyIconScale + offset4;
        }
    }
    rc.right -= cyIconScale;
    DrawTextW(hdc, text.c_str(), -1, &rc, pData->draw.fmt);
    //67, 67, 70

    return;
}


LRESULT CALLBACK WndProc_ComboBox_Command(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    WNDPROC oldProc = pData->oldProc;
    const int id = LOWORD(wParam);
    const int code = HIWORD(wParam);
    HWND hComboBox = (HWND)lParam;
    pData = GetDataFromHwnd(hComboBox);
    if (!pData) return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);

    switch (code)
    {
    case CBN_SELCHANGE: // 选中被改变
    {
        //const int index = SendMessageW(hComboBox, CB_GETCURSEL, 0, 0);
        //wstr::dbg(L"组合框选中被改变, 句柄 = 0x%p(%d), 选中项 = %d\n", hComboBox, hComboBox, index);
        break;
    }
    case CBN_SELENDCANCEL:  // 当用户选择的项目，但随后选择另一个控制或关闭对话框通知代码被发送。它指示将忽略用户的初始选择。组合框的父窗口通过WM_COMMAND消息接收此通知代码
    {
        //const int index = SendMessageW(hComboBox, CB_GETCURSEL, 0, 0);
        //wstr::dbg(L"CBN_SELENDCANCEL, 句柄 = 0x%p(%d), 选中项 = %d\n", hComboBox, hComboBox, index);
        break;
    }
    case CBN_DROPDOWN:      // 即将弹出列表
    {
        LRESULT ret = CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
        Combobox_DropDown(hComboBox, pData);
        return ret;
    }
    default:
        return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
    }
    return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
}

int ReloadListData(HWND hComboBox, HWND hListCom, LPOBJSTRUCT pData)
{
    if (!hComboBox || !pData)
    {
        COMBOBOX_LIST_DATA& data = m_map_combobox_list[hListCom];
        pData = data.pData;
        hComboBox = pData->hWnd;
    }
    if (!pData || !hComboBox)
        return 0;

    std::vector<LPWSTR>* pArr = (std::vector<LPWSTR>*)pData->param2;
    __free_arr(pArr, false);
    pData->param2 = pArr;

    const int count = (int)SendMessageW(hComboBox, CB_GETCOUNT, 0, 0);
    pArr->resize(count);
    for (int i = 0; i < count; i++)
    {
        const int textLen = (int)SendMessageW(hComboBox, CB_GETLBTEXTLEN, i, 0);
        if (textLen != CB_ERR && textLen > 0)
        {
            LPWSTR pStr = new wchar_t[textLen + 1];
            SendMessageW(hComboBox, CB_GETLBTEXT, i, (LPARAM)pStr);
            //pStr[textLen] = 0;
            (*pArr)[i] = pStr;
        }
    }
    return count;
}


// 即将弹出列表
// hComboBox = 组合框句柄
// pData = 组合框的数据
void Combobox_DropDown(HWND hComboBox, LPOBJSTRUCT pData)
{
    const int index = SendMessageW(hComboBox, CB_GETCURSEL, 0, 0);
    wstr::dbg(L"即将弹出列表, 句柄 = 0x%p(%d), 选中项 = %d\n", hComboBox, hComboBox, index);
    SetPropW(hComboBox, L"DropDown", (HANDLE)1);
    
    pData->draw.indexHot = -1;

    HWND hListCom = (HWND)pData->param;
    COMBOBOX_LIST_DATA& data = m_map_combobox_list[hListCom];
    data.pData = pData;
    data.isCaptrue = false;


    RECT rcList, rcCombobox;
    GetWindowRect(hListCom, &rcList);
    GetWindowRect(hComboBox, &rcCombobox);
    const int heightList = rcList.bottom - rcList.top;
    const int widthList  = rcList.right  - rcList.left;
    rcList.top = rcCombobox.top - heightList;
    //SetWindowPos(hListCom, 0, rcList.left, rcList.top, widthList, heightList, SWP_NOSIZE | SWP_NOZORDER);
    //SetWindowPos(hListCom, 0, rcList.left, 0, widthList, heightList, SWP_NOSIZE | SWP_NOZORDER);

    if ((int)SendMessageW(pData->hWnd, CB_GETITEMHEIGHT, 0, 0) != s_info->scale(pData->draw.nItemHeight))
        SendMessageW(pData->hWnd, CB_SETITEMHEIGHT, 0, s_info->scale(pData->draw.nItemHeight));

    ReloadListData(hComboBox, hListCom, pData);
    m_time_down = GetTickCount64();

    //ReleaseCapture();

    //RECT rc;
    //GetWindowRect(hListCom, &rc);
    //MoveWindow(hListCom, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top + 20, true);
    //MoveWindow(hListCom, rcList.left, 0, widthList, heightList, 1);
    InvalidateRect(hListCom, 0, 0);
   
    
}
static bool isBreak;
void CALLBACK __sdjkas(HWND hWnd, UINT message, UINT_PTR id, DWORD timer)
{
    wstr::dbg(L"触发了时钟, 重画已过3秒\n");
    isBreak = true;
    KillTimer(hWnd, id);
}
// pData = 列表框所在的组合框数据, pData->param 存放着列表框的句柄
static inline void _draw_list(LPOBJSTRUCT pData, HDC hdcSrc, int offset = 0)
{
    if (!pData->param2)
        return;
    //if (isBreak)
    //    __debugbreak();
    //else
    //    SetTimer(pData->hWnd, 10055, 3000, __sdjkas);
    IDrawScroll* pObj = (IDrawScroll*)pData->drawScroll;

    HWND hWndList = (HWND)pData->param;
    HWND hComboBox = pData->hWnd;
    RECT& rc = pData->draw.offsetRect;
    if (pObj)
    {
        GetWindowRect(hWndList, &rc);
        rc.right -= rc.left;
        rc.bottom -= rc.top;
        rc.left = 0;
        rc.top = 0;
    }
    else
    {
        GetClientRect(hWndList, &rc);
    }
    if (pObj && pObj->m_isVScroll)
        rc.right -= (int)(pObj->m_vInfo.btnThumbSize - 0);
    const int cxClient = rc.right - rc.left;
    const int cyClient = rc.bottom - rc.top;
    const LPEX_FONT pFont = g_scale.GetFont(pData->draw.hFont ? pData->draw.hFont : s_info->defData->draw.hFont);    // 绘画字体
    const HFONT hFont = pFont->GetFont();    // 绘画字体

    LPMEMDCBITMAP md = md_create(cxClient, cyClient);
    HDC hdc = md->hdc;
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, pData->draw.color.crText);
    SelectObject(hdc, hFont);
    SelectObject(hdc, pData->draw.color.hbrBackGround);
    FillRect(hdc, &rc, pData->draw.color.hbrBackGround);


    std::vector<LPWSTR>* pArr = (std::vector<LPWSTR>*)pData->param2;
    if (pArr->size() == 0)
        ReloadListData(hComboBox, hWndList, pData);
    const int pos = GetScrollPos(hWndList, SB_VERT);
    const int index = (int)SendMessageW(hComboBox, CB_GETCURSEL, 0, 0);
    const int count = (int)pArr->size();
    const int firstIndex = pos;

    SCROLLINFO si = { 0 };
    si.cbSize = sizeof(si);
    si.fMask = SIF_ALL;
    GetScrollInfo(hWndList, SB_VERT, &si);

    const int itemHeight = ComboBox_GetItemHeight(pData->hWnd);
    if ( itemHeight != s_info->scale(pData->draw.nItemHeight) )
    {
        SendMessageW(pData->hWnd, CB_SETITEMHEIGHT, 0, s_info->scale(pData->draw.nItemHeight));
        wstr::dbg(L"重画, 重新拉伸了表项高度, 高度 = %d\n", s_info->scale(pData->draw.nItemHeight));

    }
    RECT rcItem = rc; rcItem.top = 0;
    rcItem.bottom = rcItem.top + itemHeight;
    int maxCount = cyClient / itemHeight;
    if ( cyClient % itemHeight != 0 )
        maxCount++;

    //if ( count - maxCount != si.nMax )
    //{
    //    si.nPage = count - maxCount;
    //    si.fMask = SIF_PAGE;
    //    SetScrollInfo(hWndList, SB_VERT, &si, true);
    //}

    pData->count = count;
    pData->draw.hItemSel = (HTREEITEM)pos;


    wstr::dbg(L"滚动条, min = %d, max = %d, pos = %d(%d), page = %d, itemHeight = %d, maxCount = %d, count = %d\n",
              si.nMin, si.nMax, si.nPos, pos, si.nPage, itemHeight, maxCount, count);
    for ( int i = firstIndex; i < count; i++ )
    {
        if (i == pData->draw.indexHot)
            FillRect(hdc, &rcItem, pData->draw.color.hbrHot);
        if (i == index)
            FillRect(hdc, &rcItem, pData->draw.color.hbrSelected);

        LPWSTR pStr = (*pArr)[i];
        if (pStr)
        {
            rcItem.left += 5;
            DrawTextW(hdc, pStr, -1, &rcItem, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            rcItem.left -= 5;
        }

        rcItem.top += itemHeight;
        rcItem.bottom = rcItem.top + itemHeight;
        if (rcItem.top >= cyClient)
            break;    //超出了可视范围就退出绘画
    }

    HPEN hOldPen = (HPEN)SelectObject(hdc, pData->draw.color.hpenBorder);
    MoveToEx(hdc, cxClient, 0, 0);
    LineTo(hdc, 0, 0);
    LineTo(hdc, 0, cyClient - 1);
    LineTo(hdc, cxClient, cyClient - 1);
    SelectObject(hdc, hOldPen);

    BitBlt(hdcSrc, offset, 0, cxClient, cyClient, hdc, 0, 0, SRCCOPY);
    md_destroy(md);
}

void InvalidateList(LPOBJSTRUCT pData)
{
    HWND hWnd = (HWND)pData->param;
    HDC hdc = GetWindowDC(hWnd);
    _draw_list(pData, hdc);
    ReleaseDC(hWnd, hdc);
}

LRESULT CALLBACK WndProc_ComboBox_List(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // pData = 列表框所在的组合框数据, pData->param 存放着列表框的句柄, pData->param1 = 旧的回调函数
    LPOBJSTRUCT pData = (LPOBJSTRUCT)GetPropW(hWnd, L"oldProc");
    if (!pData)return DefWindowProcA(hWnd, message, wParam, lParam);
    WNDPROC oldProc = (WNDPROC)pData->param1;
    IDrawScroll* pObj = (IDrawScroll*)pData->drawScroll;
    LRESULT ret = 0;
    if (pObj)
    {
        bool isReturn = false;
        if (message == WM_NCLBUTTONDOWN)
        {
            COMBOBOX_LIST_DATA& data = m_map_combobox_list[hWnd];
            data.isCaptrue = true;
            isReturn = pObj->OnWndProc(oldProc, hWnd, message, wParam, lParam, &ret);
            data.isCaptrue = false;
        }
        else
        {
            if ( message == WM_LBUTTONUP && pObj->m_isCapture )
            {
                int oldVl = pObj->m_isCapture;
                pObj->m_isCapture = 0;
                pObj->OnWndProc(oldProc, hWnd, message, wParam, lParam, &ret);
                pObj->m_isCapture = oldVl;
                pObj->OnWndProc(oldProc, hWnd, WM_CAPTURECHANGED, wParam, lParam, &ret);
                isReturn = true;
            }
            else
            {
                isReturn = pObj->OnWndProc(oldProc, hWnd, message, wParam, lParam, &ret);
            }
        }

        if (isReturn)
        {
            switch (message)
            {
            case WM_VSCROLL:
            case WM_HSCROLL:
            case WM_MOUSEHWHEEL:
            case WM_MOUSEWHEEL:
            {
                InvalidateItem(pData);
                return ret;
            }
            default:
                break;
            }
            return ret;
        }
    }

    switch (message)
    {
    case WM_WINDOWPOSCHANGING:
    {
        LPWINDOWPOS pos = (LPWINDOWPOS)lParam;
        // 有改变尺寸才处理
        if ((pos->flags & SWP_NOSIZE) == 0 || (pos->flags & SWP_NOMOVE) == 0 && GetPropW(pData->hWnd, L"DropDown"))
        {
            SetPropW(pData->hWnd, L"DropDown", 0);
            RemovePropW(pData->hWnd, L"DropDown");
            RECT rcCbb;
            GetWindowRect(pData->hWnd, &rcCbb);
            if ( pos->y < rcCbb.top && (pos->flags & SWP_NOMOVE) == 0 )
                pos->y += 2;
            
        }
        return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
        return 0;
    }
    case WM_TIMER:
    {
        KillTimer(hWnd, wParam);
        //ReleaseCapture();
        InvalidateList(pData);

        break;
    }
    case WM_NCPAINT:
    {
        LRESULT ret = CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
        InvalidateList(pData);
        return ret;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        _draw_list(pData, ps.hdc, 0);
        EndPaint(hWnd, &ps);
        InvalidateList(pData);
        break;
    }
    case WM_PRINT:
    {
        //wstr::dbg(L"WM_PRINT 重画, lParam = 0x%08X\n", lParam);
        //InvalidateList(pData);
        int offset = -1;
        if ((lParam & PRF_NONCLIENT) == PRF_NONCLIENT)
            offset = 0;

        _draw_list(pData, (HDC)wParam, offset);
        return 0;
    }
    case WM_PRINTCLIENT:
        //CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
        //wstr::dbg(L"WM_PRINTCLIENT 重画\n");
        _draw_list(pData, (HDC)wParam);
        break;
    case WM_ERASEBKGND:
        //wstr::dbg(L"WM_ERASEBKGND 重画\n");
        _draw_list(pData, (HDC)wParam);
        return 1;
    case WM_DESTROY:
    {
        RemovePropW(hWnd, L"oldProc");
        pData->SetWndLong(hWnd, GWLP_WNDPROC, (LONG_PTR)oldProc);
        //return DefWindowProcA(hWnd, message, wParam, lParam);
        return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_MOUSEMOVE:
    {
        const int x = GET_X_LPARAM(lParam);
        const int y = GET_Y_LPARAM(lParam);
        const int itemHeight = ComboBox_GetItemHeight(pData->hWnd);
        const int scrollPos = (int)pData->draw.hItemSel * itemHeight;      // 这个成员记录当前滚动条的位置, 在绘画表项的时候获取
        const int maxHeight = pData->count * itemHeight;
        RECT& rcList = pData->draw.offsetRect;
        const int cxClient = rcList.right - rcList.left;
        const int cyClient = rcList.bottom - rcList.top;

        TRACKMOUSEEVENT evt;
        evt.cbSize      = sizeof(evt);
        evt.dwHoverTime = HOVER_DEFAULT;
        evt.hwndTrack   = hWnd;
        evt.dwFlags     = TME_LEAVE;
        TrackMouseEvent(&evt);

        int right = cxClient;    // 客户区宽度 + 滑块宽度 = 右边距离
        if (pObj && pObj->m_isVScroll)
             right += (long)(pObj->m_vInfo.btnThumbSize);
        
        const bool isGreaterLeft    = x > cxClient;     // 大于左边
        const bool isLessRight      = x <= right;       // 小于等于右边
        const bool isGreaterTop     = y >= 0;           // 大于顶边
        const bool isLessBottom     = y <= cyClient;    // 大于顶边

        //wstr::dbg(L"鼠标移动x,y{%d,%d}, {%d,%d,%d,%d}, 大于左边, 小于右边 = %d, %d,  大于顶边, 小于底边 = %d,%d, 进入判断 = %d\n",
        //    x, y, 0, 0, cxClient, right,
        //    isGreaterLeft, isLessRight, isGreaterTop, isLessBottom,
        //    pObj && isGreaterLeft && isLessRight && isGreaterTop && isLessBottom);
        if (pObj && isGreaterLeft && isLessRight && isGreaterTop && isLessBottom)   // 鼠标在滚动条内
        {
            POINT pt = { x, y };
            ClientToScreen(hWnd, &pt);
            if(pObj->m_vrc.rc.right == 0)
                pObj->OnWndProc(oldProc, hWnd, WM_NCHITTEST, 0, MAKELONG(pt.x, pt.y), &ret);
            pObj->OnWndProc(oldProc, hWnd, WM_NCMOUSEMOVE, HTVSCROLL, MAKELONG(pt.x, pt.y), &ret);
            pData->draw.indexHot = -1;
            InvalidateItem(pData);
            return ret;
        }

        if (y > maxHeight || x < 0 || x > cxClient )
        {
            if (pData->draw.indexHot != -1)
            {
                InvalidateItem(pData);
                pData->draw.indexHot = -1;
                InvalidateItem(pData);
            }
            //return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
            return DefWindowProcA(hWnd, message, wParam, lParam);
        }


        int indexHot = -1;
        if ( x >= 0 && y >= 0 && x <= cxClient && y < cyClient )
            indexHot = ( y + scrollPos ) / itemHeight;
        if ( pData->draw.indexHot != indexHot )
        {
            InvalidateItem(pData);
            pData->draw.indexHot = indexHot;
            InvalidateItem(pData);
        }

        //return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
        return DefWindowProcA(hWnd, message, wParam, lParam);
        break;
    }
    case WM_MOUSELEAVE:
        pData->draw.indexHot = -1;
        InvalidateItem(pData);
        break;
    case WM_LBUTTONUP:
    case WM_LBUTTONDOWN:
    {
        if ( message == WM_LBUTTONUP && GetTickCount64() - m_time_down < 300 )
            return 0;   // 小于300毫秒不处理放开操作, 有可能是点击弹出列表鼠标还没放开时鼠标在列表上, 这种情况不能处理
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        RECT& rcList = pData->draw.offsetRect;
        const int cxClient = rcList.right - rcList.left;
        const int cyClient = rcList.bottom - rcList.top;

        int right = cxClient;    // 客户区宽度 + 滑块宽度 = 右边距离
        if (pObj && pObj->m_isVScroll)
            right += (long)(pObj->m_vInfo.btnThumbSize);

        const bool isGreaterLeft = pt.x > cxClient;     // 大于左边
        const bool isLessRight   = pt.x <= right;       // 小于等于右边
        const bool isGreaterTop  = pt.y >= 0;           // 大于顶边
        const bool isLessBottom  = pt.y <= cyClient;    // 大于顶边

        wstr::dbg(L"鼠标点击x,y{%d,%d}, {%d,%d,%d,%d}, 大于左边, 小于右边 = %d, %d,  大于顶边, 小于底边 = %d,%d, 进入判断 = %d\n",
            pt.x, pt.y, 0, 0, cxClient, right,
            isGreaterLeft, isLessRight, isGreaterTop, isLessBottom,
            pObj && isGreaterLeft && isLessRight && isGreaterTop && isLessBottom);
        if (pObj && isGreaterLeft && isLessRight && isGreaterTop && isLessBottom)   // 鼠标在滚动条内
        {
            ClientToScreen(hWnd, &pt);
            message = (message == WM_LBUTTONDOWN ? WM_NCLBUTTONDOWN : WM_NCLBUTTONUP);
            pObj->OnWndProc(oldProc, hWnd, message, HTVSCROLL, MAKELONG(pt.x, pt.y), &ret);
            return DefWindowProcA(hWnd, message, wParam, lParam);
            return 0;
        }
        const LPCWSTR propName = L"{EA55833E-A097-4FA6-9136-4109BB4164B3}";
        if (message == WM_LBUTTONUP)
        {
            // 鼠标放开, 需要触发点击事件, 并隐藏列表
            RECT rcList; POINT ptList;
            GetCursorPos(&ptList);
            GetWindowRect(hWnd, &rcList);
            if ( PtInRect(&rcList, ptList) )
            {
                SendMessageW(pData->hWnd, CB_SETCURSEL, pData->draw.indexHot, 0);
                SendMessageW(pData->hWnd, WM_COMMAND, MAKELONG(pData->id, CBN_SELENDOK	), (LPARAM)pData->hWnd);
            }
            else
            {
                //SendMessageW(pData->hWnd, CB_SETCURSEL, -1, 0);
                SendMessageW(pData->hParent, WM_COMMAND, MAKELONG(pData->id, CBN_SELENDCANCEL	), (LPARAM)pData->hWnd);
            }
            ReleaseCapture();
            //ShowWindowEx(hWnd, SW_HIDE);
            //SendMessageW(pData->hParent, WM_COMMAND, MAKELONG(pData->id, CBN_SELCHANGE), (LPARAM)pData->hWnd);
            //
        }
        return DefWindowProcA(hWnd, message, wParam, lParam);
        return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
    }
    //case WM_NCCALCSIZE:
    //{
    //    if (wParam)
    //    {
    //        LPNCCALCSIZE_PARAMS cs = (LPNCCALCSIZE_PARAMS)lParam;
    //        cs->rgrc[0].right -= 200;
    //        return 0;
    //        memcpy(&cs->rgrc[1], &cs->rgrc[0], sizeof(cs->rgrc[0]));
    //        memcpy(&cs->rgrc[2], &cs->rgrc[0], sizeof(cs->rgrc[0]));
    //        return 0;
    //        return WVR_ALIGNTOP | WVR_ALIGNLEFT | WVR_ALIGNBOTTOM | WVR_ALIGNRIGHT;

    //        return WVR_REDRAW;
    //    }
    //    LPRECT prc = (LPRECT)lParam;
    //    return 0;
    //}
    case LB_ADDSTRING:
    case LB_INSERTSTRING:
    case LB_DELETESTRING:
    case LB_SELITEMRANGEEX:
    case LB_RESETCONTENT:
    case LB_SETSEL:
    case LB_SETCURSEL:
    case LB_GETSEL:
    case LB_GETCURSEL:
    case LB_GETTEXT:
    case LB_GETTEXTLEN:
    case LB_GETCOUNT:
    case LB_SELECTSTRING:
    case LB_DIR:
    case LB_GETTOPINDEX:
    case LB_FINDSTRING:
    case LB_GETSELCOUNT:
    case LB_GETSELITEMS:
    case LB_SETTABSTOPS:
    case LB_GETHORIZONTALEXTENT:
    case LB_SETHORIZONTALEXTENT:
    case LB_SETCOLUMNWIDTH:
    case LB_ADDFILE:
    case LB_SETTOPINDEX:
    case LB_GETITEMRECT:
    case LB_GETITEMDATA:
    case LB_SETITEMDATA:
    case LB_SELITEMRANGE:
    case LB_SETANCHORINDEX:
    case LB_GETANCHORINDEX:
    case LB_SETCARETINDEX:
    case LB_GETCARETINDEX:
    case LB_SETITEMHEIGHT:
    case LB_GETITEMHEIGHT:
    case LB_FINDSTRINGEXACT:
    case LB_SETLOCALE:
    case LB_GETLOCALE:
    case LB_SETCOUNT:
    case LB_INITSTORAGE:
    case LB_ITEMFROMPOINT:
    case LB_GETLISTBOXINFO:
    case LB_MSGMAX:
    case WM_SETFOCUS:
    case WM_KILLFOCUS:
    case WM_CAPTURECHANGED:
        return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
    case WM_VSCROLL:
    case WM_HSCROLL:
    case WM_MOUSEHWHEEL:
    case WM_MOUSEWHEEL:
    {
        LRESULT ret = 0; CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
        InvalidateItem(pData);
        return ret;
    }
    default:
        //wstr::dbg(L"xiaox = %d\n", message);
        //return DefWindowProcA(hWnd, message, wParam, lParam);
        return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
        return CallWindowProcA(s_oldProcList, hWnd, message, wParam, lParam);
    }
    return 0;
}

// 重画指定项目
void InvalidateItem(LPOBJSTRUCT pData)
{
    
    RECT& rcList = pData->draw.offsetRect;
    HWND hList = (HWND)pData->param;
    InvalidateList(pData);


    //::InvalidateRect(hList, &rcList, 0);
    return;

    int cxClient = rcList.right - rcList.left;
    int cyClient = rcList.bottom - rcList.top;

    const int itemHeight = ComboBox_GetItemHeight(pData->hWnd);
    int top = pData->draw.indexHot * itemHeight;
    top -= (int)pData->draw.hItemSel; // 这个成员记录当前滚动条的位置, 在绘画表项的时候获取
    if (top + itemHeight <= 0) return;   // 顶边+高度 小于0, 不需要重画

    RECT rc;
    if (cxClient == 0)
    {
        ::GetClientRect(hList, &rc);
        cxClient = rc.right - rc.left;
        cyClient = rc.bottom - rc.top;
    }
    rc = { 0, 0, cxClient, cyClient };
    if (top > rc.bottom - rc.top)
        return; // 顶边大于列表底边, 不需要重画
    rc.left = 0;
    rc.top = top;
    rc.bottom = rc.top + itemHeight;
    ::InvalidateRect(hList, &rc, 0);
}