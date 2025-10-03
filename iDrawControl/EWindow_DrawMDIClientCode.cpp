#include "EWindow_Fne_Header.h"
#include <EIDE_Interface.h>
#include <hook_detours/ApiHook.h>

struct CODE_DRAW_INFO
{
    PAINTSTRUCT* ps;        // BeginPaint 传递的值, 这里只记录他的指针
    LPOBJSTRUCT pData;      // 当前窗口
    LPCSTR codeText;        // 代码文本
    int textLen;            // 代码文本长度, 需要根据长度计算是否绘画到最后一个文本
    RECT rcLine;            // 行号绘画位置, 根据最后一个文本绘画的顶边+文本高度计算
    int lineTop;            // 当前行号左边小图标的顶边
    int lineCode;           // 当前行号
    LPMEMDCBITMAP md;       // 内存DC, 自己创建的, 需要自己销毁
    HDC hDC;                // ps指向的hdc, 这个不需要自己销毁

};
static std::unordered_map<HWND, CODE_DRAW_INFO*> s_map_hwnd;    // 指向绘画数据, 和 s_map_hdc指向的数据完全一样, 只不过这里是按窗口获取
static std::unordered_map<HDC, CODE_DRAW_INFO*> s_map_hdc;      // 指向绘画数据, 按hdc获取绘画数据
static std::vector<RECT> m_rcLines;                             // 当前绘画页的行号位置 TextOutA 里计算出的位置保存起来
bool s_isHook = false;

static apiHook hook_TextOutA;
static apiHook hook_TextOutW;
static apiHook hook_DrawTextA;
static apiHook hook_DrawTextW;
static apiHook hook_BeginPaint;
static apiHook hook_EndPaint;
static apiHook hook_BitBlt;
static apiHook hook_CreateCompatibleDC;

static int(WINAPI* TextOutA_hook)(HDC hdc, int x, int y, LPCSTR  lpString, int c) = TextOutA;
static int(WINAPI* TextOutW_hook)(HDC hdc, int x, int y, LPCWSTR lpString, int c) = TextOutW;
static int(WINAPI* DrawTextA_hook)(HDC hdc, LPCSTR  lpchText, int cchText, LPRECT lprc, UINT format) = DrawTextA;
static int(WINAPI* DrawTextW_hook)(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format) = DrawTextW;
static HDC(WINAPI* BeginPaint_hook)(HWND hWnd, PAINTSTRUCT* ps) = BeginPaint;
static BOOL(WINAPI* EndPaint_hook)(HWND hWnd, const PAINTSTRUCT* ps) = EndPaint;
static BOOL(WINAPI* BitBlt_hook)(HDC hdc, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop) = BitBlt;
static HDC(WINAPI* CreateCompatibleDC_hook)(HDC hdc) = CreateCompatibleDC;

inline CODE_DRAW_INFO* __GetData(HWND hWnd)
{
    auto it = s_map_hwnd.find(hWnd);
    if (it == s_map_hwnd.end())
        return 0;
    return it->second;
}
inline CODE_DRAW_INFO* __GetData(HDC hdc)
{
    auto it = s_map_hdc.find(hdc);
    if (it == s_map_hdc.end())
    {
        HWND hWnd = WindowFromDC(hdc);
        if (hWnd)
            return __GetData(hWnd);
        return 0;
    }
    return it->second;
}
static HDC WINAPI MyCreateCompatibleDC_hook(HDC hdc)
{
    HDC ret = CreateCompatibleDC_hook(hdc);
    if (!s_isHook)
        return ret;

    CODE_DRAW_INFO* data = __GetData(hdc);

    if (data)
        s_map_hdc[ret] = data;

    return ret;
}
static HDC WINAPI MyBeginPaint_hook(HWND hWnd, PAINTSTRUCT* ps)
{
    HDC hdc = BeginPaint_hook(hWnd, ps);
    if (!s_isHook)
        return hdc;

    CODE_DRAW_INFO* data = __GetData(hWnd);
    if (!data)
        return hdc;
    RECT rc;
    GetWindowRect(hWnd, &rc);
    data->md = md_create(rc.right - rc.left, rc.bottom - rc.top);
    wstr::dbg(L"BeginPaint, ps.hdc = 0x%p, hdc = 0x%p\n", ps->hdc, hdc);
    s_map_hdc[hdc] = data;
    data->ps = ps;
    ps->hdc = data->md->hdc;
    return ps->hdc;
}
static BOOL WINAPI MyEndPaint_hook(HWND hWnd, PAINTSTRUCT* ps)
{
    if (!s_isHook)
        return EndPaint_hook(hWnd, ps);
    CODE_DRAW_INFO* data = __GetData(hWnd);
    if (!data)
        return EndPaint_hook(hWnd, ps);

    wstr::dbg(L"EndPaint, ps.hdc = 0x%p\n", ps->hdc);

    ps->hdc = data->hDC;
    BitBlt_hook(ps->hdc, 0, 0, data->md->width, data->md->height, data->md->hdc, 0, 0, SRCCOPY);
    md_destroy(data->md);
    data->md = 0;
    return EndPaint_hook(hWnd, ps);
}
static BOOL WINAPI MyBitBlt_hook(HDC hdc, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop)
{
    // x==2, cx==16, cy==14, 有这个就需要在这一行绘画行号线
    // x1 == 16, y1 == 0, 绘画左边的向下箭头, 


    if (!s_isHook)
        return BitBlt_hook(hdc, x, y, cx, cy, hdcSrc, x1, y1, rop);
    CODE_DRAW_INFO* data = __GetData(hdc);

    if (!data)
        return BitBlt_hook(hdc, x, y, cx, cy, hdcSrc, x1, y1, rop);

    wstr::dbg(L"MyBitBlt_hook hdc = %d, x,y,cx,cy = %d,%d,%d,%d, hdcSrc = %d, x1,y1 = %d,%d\n",
        hdc, x, y, cx, cy, hdcSrc, x1, y1);
    //int size = 0;
    //LPBYTE p = GetBitmapDataFromHDC(hdc, &size);
    //wstr file(100);
    //static int i = 0;
    //file.assign(LR"(D:\cpp\SVN工作目录\易语言界面\src\hdc)").append(++i).append(L"_").append((void*)hdc, 0).append(L".png");
    //CFileRW f;
    //f.open(file.c_str(), 4, -1);
    //f.write(p, size);
    //delete[] p;

    //file.assign(LR"(D:\cpp\SVN工作目录\易语言界面\src\hdcSrc)").append(++i).append(L"_").append((void*)hdcSrc, 0).append(L".png");
    //f.open(file.c_str(), 4, -1);
    //p = GetBitmapDataFromHDC(hdcSrc, &size);
    //f.write(p, size);
    //delete[] p;

    return BitBlt_hook(hdc, x, y, cx, cy, hdcSrc, x1, y1, rop);
}
static int WINAPI MyDrawTextA_hook(HDC hdc, LPCSTR  lpchText, int cchText, LPRECT lprc, UINT format)
{
    if (__GetData(hdc))
        _str::dbg("MyDrawTextA_hook hdc = %d, lpchText = %s, cchText = %d, lprc = {%d,%d,%d,%d}, format = %d\n",
            hdc, lpchText, cchText, *lprc, format);
    return DrawTextA_hook(hdc, lpchText, cchText, lprc, format);
}
static int WINAPI MyDrawTextW_hook(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format)
{
    if (__GetData(hdc))
        wstr::dbg(L"MyDrawTextW_hook hdc = %d, lpchText = %s, cchText = %d, lprc = {%d,%d,%d,%d}, format = %d\n",
            hdc, lpchText, cchText, *lprc, format);
    return DrawTextW_hook(hdc, lpchText, cchText, lprc, format);
}
static int WINAPI MyTextOutW_hook(HDC hdc, int x, int y, LPCWSTR lpString, int c)
{
    if (__GetData(hdc))
        wstr::dbg(L"MyTextOutW_hook hdc = %d, x = %d, y = %d, lpString = %s, c = %d\n", hdc, x, y, lpString, c);
    return TextOutW_hook(hdc, x, y, lpString, c);
}
static int WINAPI MyTextOutA_hook(HDC hdc, int x, int y, LPCSTR  lpString, int c)
{
    //static HBRUSH hbr = CreateSolidBrush(255);
    //RECT rcFill = { x, y, x + 100, y + 21 };
    //FillRect(hdc, &rcFill, hbr);
    //SetBkColor(hdc, 255);
    //_str::dbg("error 绘画文本, x,y = {%d,%d}, 文本 = %s\n", x, y, lpString);
    // SetTextAlign(hdc, TA_UPDATECP); // 设置 TA_UPDATECP 这个标识后绘画文本可以获取到绘画后的位置
    int ret = TextOutA_hook(hdc, x, y, lpString, c);
    if (!s_isHook)
        return ret;
    CODE_DRAW_INFO* data = __GetData(hdc);
    if (!data)
        return ret;
    if (x < 25)
    {
        data->lineCode = atol(lpString);
        SIZE si;
        GetTextExtentPoint32A(hdc, lpString, c, &si);

        RECT& rc = data->rcLine;
        rc.left     = 0;
        rc.top      = y - 2;
        rc.right    = data->pData->pos.rcClient.right - 18;
        rc.bottom   = rc.top + si.cy + 2;
        m_rcLines.push_back(rc);

        int left = 22 + si.cx + 35;
        // 行号绘画的左边是22, 小于25的都按他是绘画行号
        return ret;
    }

    if (data->codeText == 0)    // 
    {
        data->codeText = lpString;
        data->textLen = (int)strlen(lpString);

    }
    LPCSTR codeTextEnd = data->codeText + data->textLen;
    if (lpString + c == codeTextEnd)
    {
        //SIZE si;
        //GetTextExtentPoint32A(hdc, lpString, c, &si);
        //RECT rc;     
        //rc.left     = 0;
        //rc.top      = data->rcLine.top - 2;     // 顶边由第一次绘画代码文本时记录
        //rc.bottom   = y + si.cy + 2;            // 底边是当前代码行最后一个文本的底边
        //rc.right    = data->pData->pos.rcClient.right - 19;

        //m_rcLines.push_back(rc);
    }

    return ret;
}

static RECT m_rcLine;
static CODE_DRAW_INFO* m_drawinfo;
LRESULT MDIClientCode_BeginPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData, IDrawScroll* pObj)
{
    if(0)
    {
        LRESULT ret = WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        HDC hdcWindow = GetWindowDC(hWnd);
        pObj->OnDrawScroll(hdcWindow);
        ReleaseDC(hWnd, hdcWindow);
        return ret;
    }

    s_isHook = true;
    CODE_DRAW_INFO* data = s_map_hwnd[pData->hWnd];
    if (!data)
        data = new CODE_DRAW_INFO;  // 不存在时才创建, 存在就不管, 每次在这里都会重新赋值, 不怕打架
    memset(data, 0, sizeof(CODE_DRAW_INFO));
    data->pData = pData;
    s_map_hwnd[pData->hWnd] = data;
    s_map_hdc.clear();
    m_rcLines.clear();
    LRESULT ret = WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    //POINT ptCare;
    //GetCaretPos(&ptCare);
    //for (auto it = m_rcLines.begin(); it != m_rcLines.end(); ++it)
    //{
    //    RECT& rc = *it;
    //    if (rc.top <= ptCare.y && rc.bottom > ptCare.y)   // 位置是在左边图标中间
    //    {
    //        m_rcLine = rc;
    //        ClientToScreen(hWnd, (LPPOINT)&m_rcLine);
    //        ScreenToClient(s_info->hEWnd, (LPPOINT)&m_rcLine);
    //        m_rcLine.right = m_rcLine.left + (rc.right - rc.left);
    //        m_rcLine.bottom = m_rcLine.top + (rc.bottom - rc.top);
    //        m_drawinfo = data;
    //        wstr::dbg(L"鼠标点击坐标 = {%d,%d,%d,%d}\n", m_rcLine);
    //        RECT rc;
    //        GetWindowRect(s_info->hEWnd, &rc);
    //        s_info->shadow->Update(rc, true);
    //        break;
    //    }
    //}

    if (pObj)
    {
        HDC hdcWindow = GetWindowDC(hWnd);
        pObj->OnDrawScroll(hdcWindow);
        ReleaseDC(hWnd, hdcWindow);
    }
    s_isHook = false;
    m_drawinfo = 0;
    return ret;
}
static LPEX_PEN m_hPen;
void MDIClientCode_DrawLine(IRE_STRUCT_BACKGROUND_DRAW* evt)
{
    return;
    if (!m_drawinfo) return;

    HWND hMDIClient4 = m_drawinfo->pData->hWnd;
    RECT rcMDI4;
    GetClientRect(hMDIClient4, &rcMDI4);
    const int cxClient = rcMDI4.right - rcMDI4.left;
    const int cyClient = rcMDI4.bottom - rcMDI4.top;
    ClientToScreen(hMDIClient4, (LPPOINT)&rcMDI4);
    ScreenToClient(s_info->hEWnd, (LPPOINT)&rcMDI4);
    rcMDI4.right = rcMDI4.left + cxClient;
    rcMDI4.bottom = rcMDI4.top + cyClient;
    if (m_rcLine.top >= rcMDI4.bottom)  // 绘画的线顶边大于等于代码框的底边, 那就不需要绘画了
        return;

    if (!m_hPen)
        m_hPen = _pen_create(MAKEARGB(255, 70, 70, 70));

    bool isBold = false;// g_ini.isBoldLine();
    int oldMode = 0;
    if (!isBold)    // 不加粗的话就需要设置不抗锯齿, 默认是抗锯齿, 抗锯齿会加粗
        _canvas_SetSmoothingMode(evt->hCanvas, 1, &oldMode);


    RECT_F rc;
    if (m_rcLine.bottom > rcMDI4.bottom)
    {
        // 越界了, 需要画线, 而不是画矩形
        LINEPOINT_F l;
        l.x = (float)m_rcLine.left;
        l.y = (float)m_rcLine.top;
        l.x1 = (float)l.x;
        l.y1 = (float)rcMDI4.bottom;
        _canvas_drawLine(evt->hCanvas, m_hPen, &l);   // 左边竖线
        l.x = (float)m_rcLine.left;
        l.y = (float)m_rcLine.top;
        l.x1 = (float)m_rcLine.right;
        l.y1 = (float)m_rcLine.top;
        _canvas_drawLine(evt->hCanvas, m_hPen, &l);   // 顶边横线

        l.x = (float)m_rcLine.right;
        l.y = (float)m_rcLine.top;
        l.x1 = (float)l.x;
        l.y1 = (float)rcMDI4.bottom;
        _canvas_drawLine(evt->hCanvas, m_hPen, &l);   // 右边竖线
    }
    else
    {
        rc.left = (float)m_rcLine.left;
        rc.top = (float)m_rcLine.top;
        rc.right = (float)m_rcLine.right;
        rc.bottom = (float)m_rcLine.bottom;
        _canvas_drawRectangle(evt->hCanvas, m_hPen, &rc);
    }

    if (!isBold)
        _canvas_SetSmoothingMode(evt->hCanvas, oldMode, 0);
}

inline void IDraw_MdiCode_Destroy()
{
    for (auto it = s_map_hwnd.begin(); it != s_map_hwnd.end(); ++it)
        delete it->second;
    s_map_hwnd.clear();

    hook_TextOutA           .unHook();
    hook_TextOutW           .unHook();
    hook_DrawTextA          .unHook();
    hook_DrawTextW          .unHook();
    hook_BeginPaint         .unHook();
    hook_EndPaint           .unHook();
    hook_BitBlt             .unHook();
    hook_CreateCompatibleDC .unHook();

}

void MDIClientCode_Hook()
{
    static bool isHook = false;
    if (!isHook)
    {
        IDraw_PushPluginFreeCallback(IDraw_MdiCode_Destroy);
        hook_TextOutA.hook(&(PVOID&)TextOutA_hook, MyTextOutA_hook);
        //hook_TextOutW.hook(&(PVOID&)TextOutW_hook, MyTextOutW_hook);
        //hook_DrawTextA.hook(&(PVOID&)DrawTextA_hook, MyDrawTextA_hook);
        //hook_DrawTextW.hook(&(PVOID&)DrawTextW_hook, MyDrawTextW_hook);
        //hook_BeginPaint.hook(&(PVOID&)BeginPaint_hook, MyBeginPaint_hook);
        //hook_EndPaint.hook(&(PVOID&)EndPaint_hook, MyEndPaint_hook);
        //hook_BitBlt.hook(&(PVOID&)BitBlt_hook, MyBitBlt_hook);
        //hook_CreateCompatibleDC.hook(&(PVOID&)CreateCompatibleDC_hook, MyCreateCompatibleDC_hook);
        m_rcLines.reserve(100);
    }
}

