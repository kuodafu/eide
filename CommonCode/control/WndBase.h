#pragma once
#include <tstr.h>
#include <windowsx.h>
#include <commctrl.h>
#include "WndBaseState.h"
#include "WndClassName.h"
#include "WndStyleDef.h"
#include "CControlDraw.h"



#define PROP_WNDPROC L"{1DE85EBD-ECDD-4091-93D6-D609A63D9DC6}"
class CWndBase;


//typedef tagSUBCLASSSTRUCT_YEMPLATE<OBJSTRUCT>* LPOBJSUBCLASS;
typedef LRESULT(CALLBACK* WND_SUBPROC)( WNDPROC, HWND, UINT, WPARAM, LPARAM );



typedef struct PROPSTRUCT
{
    PROPSTRUCT() :name(0), value(0) { }
    ~PROPSTRUCT()
    {
        if ( name )
            wstr::free(name);
        name = 0;
        value = 0;
    }

    PROPSTRUCT(const PROPSTRUCT& obj) :name(0), value(0)
    {
        operator=(obj);
    }

    inline PROPSTRUCT& operator=(const PROPSTRUCT& obj)
    {
        if ( !obj.name ) return *this;
        operator=(obj.name);
        value = obj.value;
        return *this;
    }

    inline PROPSTRUCT& operator=(LPCWSTR text)
    {
        if ( !text ) return *this;
        if ( name )
            wstr::free(name);
        name = wstr::copy(text);
        return *this;
    }

    inline PROPSTRUCT& move(PROPSTRUCT& obj)
    {
        if ( !obj.name ) return *this;
        if ( name )
            wstr::free(name);
        name        = obj.name;
        value       = obj.value;
        obj.name    = 0;
        obj.value   = 0;
        return *this;
    }

#if _MSC_VER > 1200
    PROPSTRUCT(PROPSTRUCT&& obj) noexcept :name(0), value(0)
    {
        operator=(std::move(obj));
    }

    inline PROPSTRUCT& operator=(PROPSTRUCT&& obj) noexcept
    {
        return move(obj);
    }
#endif

    LPWSTR name;
    HANDLE value;
}* PPROPSTRUCT, * LPPROPSTRUCT;



typedef struct tagPROP_SUBWNDDATA
{
    CWndBase* pThis;        // 本窗口的基类指针
    LPVOID param;           // 创建窗口附加参数
    LONG_PTR id;            // 窗口ID
    WNDPROC oldProc;        // 旧过程地址
    WND_SUBPROC subFun;     // 用户定义的子类化回调函数
    BOOL isSubClass;        // 是否子类化
}PROP_SUBWNDDATA, * LPPROP_SUBWNDDATA;


// 弹出内存对话框, 返回对话框回调里设置的返回值, 失败返回0
inline INT_PTR DialogBoxPop(DWORD dwExStyle, LPCWSTR lpszName, LONG style, 
    int x, int y, int nWidth, int nHeight, HWND hWndParent, HINSTANCE hInstance, DLGPROC dlgProc, LPVOID lpParam = 0)
{
    struct __DLGTEMPLATE : DLGTEMPLATE
    {
        WORD menuid;
        WORD cls;
        wchar_t caption[1024];
    };
    __DLGTEMPLATE dlg;
    memset(&dlg, 0, sizeof(dlg));
    dlg.style = style;
    dlg.dwExtendedStyle = dwExStyle;
    dlg.cdit = 0;
    dlg.x = x;
    dlg.y = y;
    dlg.cx = nWidth;
    dlg.cy = nHeight;
    if (!lpszName)lpszName = L"";
    size_t len = wcslen(lpszName) + 1;
    if (len > 1024) len = 1024;
    memcpy(dlg.caption, lpszName, len * sizeof(wchar_t));

    INT_PTR b = DialogBoxIndirectParamW(hInstance, &dlg, hWndParent, dlgProc, (LPARAM)lpParam);
    return b;
}
// 创建内存对话框, 返回对话框句柄
inline HWND CreateDialogBoxPop(DWORD dwExStyle, LPCWSTR lpszName, LONG style, 
    int x, int y, int nWidth, int nHeight, HWND hWndParent, HINSTANCE hInstance, DLGPROC dlgProc, LPVOID lpParam = 0)
{
    struct __DLGTEMPLATE : DLGTEMPLATE
    {
        WORD menuid;
        WORD cls;
        wchar_t caption[1024];
    };
    __DLGTEMPLATE dlg;
    memset(&dlg, 0, sizeof(dlg));
    dlg.style = style;
    dlg.dwExtendedStyle = dwExStyle;
    dlg.cdit = 0;

    dlg.x = x;
    dlg.y = y;
    dlg.cx = nWidth;
    dlg.cy = nHeight;
    if (!lpszName)lpszName = L"";
    size_t len = wcslen(lpszName) + 1;
    if (len > 1024) len = 1024;
    memcpy(dlg.caption, lpszName, len * sizeof(wchar_t));

    return CreateDialogIndirectParamW(hInstance, &dlg, hWndParent, dlgProc, (LPARAM)lpParam);
}

//template<typename TData, typename TStringA, typename TStringW>
class CWndBase
{
private:
    static inline void __init_data(HFONT* hFont = 0, HINSTANCE* hInst = 0)
    {
        static HFONT s_hFont;
        static HINSTANCE s_hInst;
        if (!s_hInst)
        {
            s_hFont = CWndBase::CreateFontW();
            s_hInst = GetModuleHandleW(0);
        }
        if (hFont)*hFont = s_hFont;
        if (hInst)*hInst = s_hInst;
        return;
    }

public:
    static HINSTANCE GetInst() {
        static HINSTANCE hInst;
        if (!hInst)
            __init_data(0, &hInst);
        return hInst;
    }
    static HFONT GetStaticFont() { return DefaultFont(); }
    static HFONT GetDefaultFont() { return DefaultFont(); }
    static HFONT DefaultFont() {
        static HFONT hFont;
        if (!hFont)
            __init_data(&hFont, 0);
        return hFont;
    }


    CWndBase() :m_hWnd(0), m_data(0)
    {
    }
    ~CWndBase()
    {
        LPPROP_SUBWNDDATA param = (LPPROP_SUBWNDDATA)GetProp(PROP_WNDPROC);
        if (param)
            param->pThis = 0;
        if (m_data)delete m_data;
        m_data = 0;
        //clear();
    }
    HWND m_hWnd;
    LPPROP_SUBWNDDATA m_data;   // 子类化窗口用的数据

    static WNDPROC CWndBase_GetSubWndData(HWND hWnd)
    {
        WNDPROC oldProc = 0;

        // 走到这的一定是超类化, 子类化的话是先设置 PROP_WNDPROC 这个值后子类化
        int size = (int)GetClassLongPtrW(hWnd, GCL_CBCLSEXTRA);
        if (size < 8)    // 超类化的额外类尺寸一定不小于8字节
            return 0;
        const int GCLP_OLDPROC = size - sizeof(WNDPROC);
        oldProc = (WNDPROC)GetClassLongPtrW(hWnd, GCLP_OLDPROC);    // 这个类的旧过程地址, 如果不为0, 这个就是旧过程地址, 可以直接使用, 这个是相同类型通用的

        if (!oldProc)       // 旧回调过程为0则表示这个类还没有把旧回调地址写入额外类信息里, 需要写入
        {
            LPBYTE pMem = (LPBYTE)GetClassLongPtrW(hWnd, GCLP_MENUNAME);
            if (!pMem) return 0;

            const size_t len = wcslen((LPCWSTR)pMem) * 2;
            for (size_t i = 0; i < len; i++)
                pMem[i] ^= 121;

            memcpy(&oldProc, pMem, sizeof(oldProc));
            pMem += sizeof(oldProc);
            if (pMem[0] == 0) pMem = 0;    // 前面存放旧过程, 后面存放原菜单, 如果原菜单为0, 则菜单清零

            SetClassLongPtrW(hWnd, GCLP_MENUNAME, (LONG_PTR)pMem);      // 注册类的时候这里临时存放旧过程
            SetClassLongPtrW(hWnd, GCLP_OLDPROC, (LONG_PTR)oldProc);    // 把这个类的旧过程地址存放到类额外尺寸的最后位置
        }

        return oldProc;
    }
    

protected:

    static LRESULT CALLBACK CWndBase_DefWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        LPPROP_SUBWNDDATA data = (LPPROP_SUBWNDDATA)::GetPropW(hWnd, PROP_WNDPROC);
        WNDPROC oldProc = 0;
        if ( !data )
            oldProc = CWndBase_GetSubWndData(hWnd);
        
        switch ( message )
        {
        case WM_NCCREATE:   // 非客户区窗口创建,消息循环的第一个消息
        {
            //! 能走到这个消息里的一定是超类化, 或者新注册的类, 肯定是调用 CWndBase::create() 来创建的
            // 需要在窗口创建的第一个消息里改掉这个值
            LPCREATESTRUCT ct = (LPCREATESTRUCT)lParam;
            LPPROP_SUBWNDDATA ptr = (LPPROP_SUBWNDDATA)ct->lpCreateParams;
            if ( ptr )
            {
                ct->lpCreateParams = ptr->param;    // 用户传递的lParam, 改回用户传递的值
                ptr->pThis->m_hWnd = hWnd;
                ::SetPropW(hWnd, PROP_WNDPROC, ptr);
                data = ptr;
                data->oldProc = oldProc;
            }
            break;
        }
        case WM_NCDESTROY:
        {
            ::RemovePropW(hWnd, PROP_WNDPROC);
            if ( oldProc )
                return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
            return DefWindowProcW(hWnd, message, wParam, lParam);
        }
        case WM_DESTROY:
        {
            if ( data)
            {
                CWndBase* pThis = data->pThis;
                if ( data->isSubClass )
                {
                    oldProc = data->oldProc;
                    SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)oldProc);
                }
                
                pThis->m_data = 0;
                pThis->m_hWnd = 0;
                delete data;
                data = 0;
            }
            ::SetPropW(hWnd, PROP_WNDPROC, 0);
            ::RemovePropW(hWnd, PROP_WNDPROC);
            if ( oldProc )
                return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
            return DefWindowProcW(hWnd, message, wParam, lParam);
        }
        default:
            break;
        }

        if ( !data )
        {
            if ( !oldProc )
                return DefWindowProcW(hWnd, message, wParam, lParam);
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        }

        CWndBase* pThis = data->pThis;
        oldProc = data->oldProc;

        if (pThis) // 在外部调用类里的方法,所有操作都由类里完成
        {
            if (data->subFun)
                return data->subFun(oldProc, hWnd, message, wParam, lParam);
            return pThis->WndProc(oldProc, hWnd, message, wParam, lParam);
        }

        if (!oldProc)
            return DefWindowProcW(hWnd, message, wParam, lParam);
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    // 窗口基类的注册类名, 如果需要超类化, 请继承 SuperClassName() 返回需要超类化的类名
    // 执行流程
    // 1. 如果className()已经注册, 则返回className();
    // 2. 取出SuperClassName()的默认回调过程, 如果SuperClassName()为空则使用 DefWindowProcW;
    // 3. 注册className(), 回调过程是窗口类内的默认处理回调函数
    LPCWSTR WndBaseRegisterClass(WNDPROC proc = 0, UINT style = 0, HBRUSH hbrBackground = 0, HCURSOR hCursor = 0, HICON hIcon = 0, HICON hIconSm = 0, int cbClsExtra = 0, int cbWndExtra = 0, WNDPROC* pOldProc = 0)
    {
        LPCWSTR newClassName = className();
        if (!newClassName)return newClassName;
        WNDCLASSEXW wcex = { 0 };
        wcex.cbSize = sizeof(WNDCLASSEXW);
        if (GetClassInfoExW(GetInst(), newClassName, &wcex))
            return newClassName;

        LPCWSTR oldClassName = SuperClassName();
        if (oldClassName)    // 旧类名不为空才获取类信息
        {
            if (GetClassInfoExW(GetInst(), oldClassName, &wcex))
            {
                // 获取类信息成功表示已经注册, 并且新旧类名相等, 直接返回新类名
                if (wcsicmp_api(oldClassName, newClassName) == 0)
                    return newClassName;
            }
            else
            {
                // 类信息获取失败, 并且两个类名不一样就返回, 一样的话走下面注册
                if (wcsicmp_api(oldClassName, newClassName) != 0)
                    return newClassName;
            }
        }

        WNDPROC oldProc = wcex.lpfnWndProc;
        if (!oldProc)
            oldProc = DefWindowProcW;    // 如果是直接注册, 那原回调地址就是默认回调

        if (!proc)proc = CWndBase::CWndBase_DefWndProc;
        if (!style)style = CS_VREDRAW | CS_HREDRAW;
        if (!hCursor)hCursor = LoadCursor(0, IDC_ARROW);
        if (!hbrBackground)hbrBackground = wcex.hbrBackground;
        if (wcex.style) style |= wcex.style;
        if (pOldProc)*pOldProc = oldProc;

        wcex.lpfnWndProc = proc;
        wcex.lpszClassName = newClassName;
        wcex.cbWndExtra += (8 + cbWndExtra);
        wcex.cbClsExtra += (8 + cbClsExtra);
        wcex.style = style;
        wcex.hCursor = hCursor;
        wcex.hbrBackground = hbrBackground;

        size_t len = 0;
        const size_t cbSize = sizeof(oldProc);
        if (wcex.lpszMenuName)
            len = wcslen(wcex.lpszMenuName);

        len = len * 2 + 2;
        BYTE pMem[280] = { 0 };
        memcpy(pMem, &oldProc, cbSize);
        if (wcex.lpszMenuName && len > 2)
            memcpy(pMem + cbSize, wcex.lpszMenuName, len);
        len += cbSize;    // 总字节长度 = 原字符串长度 + 旧过程地址长度

        for (size_t i = 0; i < len; i++)
            pMem[i] ^= 121;

        wcex.lpszMenuName = (LPCWSTR)pMem;
        ATOM atom = RegisterClassExW(&wcex);
        assert(atom != 0);
        atom = GlobalAddAtomW(wcex.lpszClassName);
        assert(atom != 0);
        return newClassName;
    }

    // 超类化的类名, 比如我要超类化, static, 则继承这个函数返回static, className() 函数就是超类化后的新类名
    // 返回0则不进行超类化
    virtual inline LPCWSTR SuperClassName()
    {
        return 0;
    }

public:
    // 默认为微软雅黑, 字体大小 -12, 没有样式
    // lfWeight = 粗体,  lfItalic = 斜体,  lfUnderline = 下划线, lfStrikeOut = 删除线
    static HFONT CreateFontW(const wchar_t* name = L"微软雅黑", int size = -12, BOOL lfWeight = false, BOOL lfItalic = false, BOOL lfUnderline = false, BOOL lfStrikeOut = false)
    {
        LOGFONTW lf = { 0 };
        SystemParametersInfoW(SPI_GETICONTITLELOGFONT, sizeof(lf), &lf, 0);
        lf.lfHeight = size;
        if (lfWeight)
            lf.lfWeight = FW_BOLD;
        lf.lfItalic = lfItalic;
        lf.lfUnderline = lfUnderline;
        lf.lfStrikeOut = lfStrikeOut;
        lf.lfCharSet = GB2312_CHARSET;
        if (!name) name = L"微软雅黑";
        size_t len = wcslen(name) + 1;
        memcpy(lf.lfFaceName, name, len * sizeof(wchar_t));
        return CreateFontIndirectW(&lf);
    }
    virtual LRESULT WndProc(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    virtual inline LPCWSTR className()
    {
        return 0;
    }

    // 通知消息
    virtual LRESULT OnNotify(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }

    virtual HWND create(DWORD dwExStyle, LPCWSTR lpszTitle, DWORD dwStyle,
        int x, int y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR id = 0, LPVOID lpParam = 0)
    {
        if (IsWindow()) return GetWindow();
        if (!m_data)
        {
            m_data = new PROP_SUBWNDDATA;
            memset(m_data, 0, sizeof(PROP_SUBWNDDATA));
        }
        m_data->id      = (LONG_PTR)id;
        m_data->pThis   = this;
        m_data->param   = lpParam;
        m_data->oldProc = 0;

        m_hWnd = CreateWindowExW(dwExStyle, WndBaseRegisterClass(), lpszTitle, dwStyle, x, y, nWidth, nHeight, hWndParent, (HMENU)(LONG_PTR)id, GetInst(), m_data);
        if (!m_hWnd)
        {
            delete m_data;
            m_data = 0;
            return 0;
        }
        SetProp(PROP_WNDPROC, m_data);

        if (m_data->oldProc == 0 && m_data->isSubClass)    // 如果旧窗口过程没有值, 并且需要子类化
        {
            m_data->oldProc = (WNDPROC)SetWindowLongPtrW(m_hWnd, GWLP_WNDPROC, (LONG_PTR)CWndBase::CWndBase_DefWndProc);
        }
        SetFont(DefaultFont());
        return m_hWnd;
    }

    inline static BOOL WINAPI GetClientRect(HWND hWnd, RECT* prc) {
        return ::GetClientRect(hWnd, prc);
    }
    inline static BOOL WINAPI GetWindowRect(HWND hWnd, RECT* prc) {
        return ::GetWindowRect(hWnd, prc);
    }
    // 取窗口客户区坐标尺寸, 左边顶边是客户区在屏幕中的位置
    inline static BOOL WINAPI GetClientSize(HWND hWnd, RECT* prc)
    {
        if (!hWnd || !prc)return false;
        BOOL ret = ::GetClientRect(hWnd, prc);
        if(!ret)return false;
        const int cxClient = prc->right - prc->left;  // 记录窗口客户区尺寸
        const int cyClient = prc->bottom - prc->top;
        ClientToScreen(hWnd, (LPPOINT)prc);
        prc->right = prc->left + cxClient;
        prc->bottom = prc->top + cyClient;
        return true;
    }

    // 取窗口矩形, 从父窗口里获取, 返回的是非客户区在父窗口中的位置, 左顶右底
    inline static BOOL WINAPI GetRectFromParent(HWND hWnd, RECT* prc) {
        if (!hWnd || !prc)return false;
        RECT* prcBorder = 0, * prcWindow = 0, * prcClient = 0, * prcClientScreen = prc;
        RECT rcWindow = { 0 };
        GetRect(hWnd, prcBorder, &rcWindow, prcClient, prc);
        //const int width = prc->right - prc->left;
        //const int height = prc->bottom - prc->top;
        const int width = rcWindow.right - rcWindow.left;
        const int height = rcWindow.bottom - rcWindow.top;
        HWND hParent = GetParent(hWnd);
        if (hParent)
            ScreenToClient(hParent, (LPPOINT)prc);
        
        prc->right = prc->left + width;
        prc->bottom = prc->top + height;
        return true;
    }

    // 取窗口非客户区左顶右底边框宽度
    // prcBorder = 接收4个边框的宽度, 为0则不获取, 4个边都为0表示没有边框
    // prcWindow = 接收该窗口在屏幕中的位置, GetWindowRect() 返回的值
    // prcClient = 接收该窗口的客户区位置, GetClientRect() 返回的值
    // prcClientScreen = 接收该窗口客户区在屏幕中的位置, 去除了边框
    inline static BOOL WINAPI GetRect(HWND hWnd, RECT* prcBorder = 0, RECT* prcWindow = 0, RECT* prcClient = 0, RECT* prcClientScreen = 0) {
        if (!hWnd)return false;
        if (!prcBorder && !prcWindow && !prcClientScreen)return false;
        RECT rc = { 0 }, rcClient = { 0 }, rcBorder = { 0 };
        ::GetWindowRect(hWnd, &rc);
        ::GetClientRect(hWnd, &rcClient);
        if (prcWindow)memcpy(prcWindow, &rc, sizeof(RECT));
        if (prcClient)memcpy(prcClient, &rcClient, sizeof(RECT));

        int cxClient = rcClient.right - rcClient.left;  // 记录窗口客户区尺寸
        int cyClient = rcClient.bottom - rcClient.top;

        ::ClientToScreen(hWnd, (LPPOINT)&rcClient);
        rcClient.right = rcClient.left + cxClient;
        rcClient.bottom = rcClient.top + cyClient;

        rcBorder.left = rcClient.left - rc.left;
        rcBorder.top = rcClient.top - rc.top;
        rcBorder.right = rc.right - rcClient.right;
        rcBorder.bottom = rc.bottom - rcClient.bottom;

        if (prcBorder)
            memcpy(prcBorder, &rcBorder, sizeof(RECT));
        if (!prcClientScreen)return true;

        rcClient.left = rcBorder.left + rc.left;
        rcClient.top = rcBorder.top + rc.top;
        rcClient.right = rcClient.left + cxClient;
        rcClient.bottom = rcClient.top + cyClient;
        memcpy(prcClientScreen, &rcClient, sizeof(RECT));
        return true;
    }
    

public:
    inline operator HWND()      const   { return m_hWnd; }
    inline BOOL     Update()    const   { return ::UpdateWindow(m_hWnd); }
    inline BOOL     IsWindow()  const   { return ::IsWindow(m_hWnd); }
    inline BOOL     Destroy()   { BOOL ret = ::DestroyWindow(m_hWnd); m_hWnd = 0; return ret; }
    inline HWND     GetWindow() const   { return m_hWnd; }
    inline BOOL     SetFocus()  const   { return ::SetFocus(m_hWnd) != NULL; }
    inline BOOL     IsFocus()   const   { return ::GetFocus() == m_hWnd; }
    inline int      GetID()     const   { return GetDlgCtrlID(m_hWnd); }
    // 取窗口的默认消息回调函数, 该函数不返回空
    inline WNDPROC  GetDefProc() const  {
        if (m_data && m_data->oldProc)
            return m_data->oldProc;
        return DefWindowProcW;
    }

    //// 调用类内的默认处理函数
    //inline LRESULT CallProc(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    //    if (!m_hWnd)return false;
    //    return WndProc(oldProc, hWnd, message, wParam, lParam);
    //}

    // 取窗口客户位置
    inline BOOL GetClientRect(RECT* prc) const {
        if (!m_hWnd)return false;
        return ::GetClientRect(m_hWnd, prc);
    }

    // 取窗口在屏幕中的位置
    inline BOOL GetWindowRect(RECT* prc) const {
        if (!m_hWnd)return false;
        return ::GetWindowRect(m_hWnd, prc);
    }

    // 取窗口非客户区左顶右底边框宽度
    // prcBorder = 接收4个边框的宽度, 为0则不获取
    // prcWindow = 接收该窗口在屏幕中的位置
    // prcClient = 接收该窗口的客户区位置, GetClientRect() 返回的值
    // prcClientScreen = 接收该窗口客户区在屏幕中的位置, 去除了边框
    inline BOOL GetRect(RECT* prcBorder = 0, RECT* prcWindow = 0, RECT* prcClient = 0, RECT* prcClientScreen = 0) const {
        return GetRect(m_hWnd, prcBorder, prcWindow, prcClient, prcClientScreen);
    }

    // 取窗口矩形, 从父窗口里获取, 返回的是窗口在父窗口中的位置, 左顶右底
    inline BOOL GetRectFromParent(RECT* prc) const {
        return GetRectFromParent(m_hWnd, prc);
    }

    // 取窗口宽度, 包含了非客户区
    inline int GetScreenWidth() const {
        if (!m_hWnd)return 0;
        RECT rc = { 0 };
        ::GetClientRect(m_hWnd, &rc);
        return rc.right - rc.left;
    }

    // 取窗口高度, 包含了非客户区
    inline int GetScreenHeight() const {
        if (!m_hWnd)return 0;
        RECT rc = { 0 };
        ::GetClientRect(m_hWnd, &rc);
        return rc.bottom - rc.top;
    }

    // 取窗口在父窗口中的坐标
    inline BOOL GetClientPos(LPPOINT ppt) const {
        if (!m_hWnd || !ppt)return false;
        RECT rc;
        if (!GetRectFromParent(m_hWnd, &rc)) return false;
        ppt->x = rc.left;
        ppt->y = rc.top;
        return true;
    }

    // 设置窗口在父窗口中的坐标
    inline BOOL SetClientPos(const LPPOINT ppt) const {
        if (!ppt) return false;
        return SetWindowPos(m_hWnd, 0, ppt->x, ppt->y, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE);
    }


    // 取窗口宽高, 包含非客户区
    inline BOOL GetSize(int* width, int* height) const {
        if (!m_hWnd)return false;
        if (!width && !height)return false;
        RECT rc = { 0 };
        BOOL ret = ::GetWindowRect(m_hWnd, &rc);
        if (!ret)return false;

        if (width)  *width  = rc.right - rc.left;
        if (height) *height = rc.bottom - rc.top;
        return true;
    }

    // 设置窗口宽高
    inline BOOL SetSize(int width, int height) const {
        if (!m_hWnd)return false;
        return SetWindowPos(m_hWnd, 0, 0, 0, width, height, SWP_NOZORDER | SWP_NOMOVE);
    }

    // 设置窗口左边
    inline BOOL SetLeft(int left) const {
        if (!m_hWnd)return false;
        RECT rc = { 0 };
        ::GetWindowRect(m_hWnd, &rc);
        return SetWindowPos(m_hWnd, 0, left, rc.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

    }

    // 设置窗口顶边
    inline BOOL SetTop(int top) const {
        if (!m_hWnd)return false;
        RECT rc = { 0 };
        ::GetWindowRect(m_hWnd, &rc);
        return SetWindowPos(m_hWnd, 0, rc.left, top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
    }

    // 设置窗口宽度
    inline BOOL SetWidth(int width) const {
        if (!m_hWnd)return false;
        RECT rc = { 0 };
        ::GetWindowRect(m_hWnd, &rc);
        return SetWindowPos(m_hWnd, 0, 0, 0, width, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOMOVE);

    }

    // 设置窗口高度
    inline BOOL SetHeight(int height) const {
        if (!m_hWnd)return false;
        RECT rc = { 0 };
        ::GetWindowRect(m_hWnd, &rc);
        return SetWindowPos(m_hWnd, 0, 0, 0, rc.right - rc.left, height, SWP_NOZORDER | SWP_NOMOVE);
    }

    // 取窗口左边
    inline int GetLeft() const {
        if (!m_hWnd)return 0;
        RECT rc;
        GetRectFromParent(m_hWnd, &rc);
        return rc.left;
    }

    // 取窗口顶边
    inline int GetTop() const {
        if (!m_hWnd)return 0;
        RECT rc;
        GetRectFromParent(m_hWnd, &rc);
        return rc.top;
    }
    // 取窗口高度, 返回的是客户区的宽度
    inline int GetWidth() const {
        if (!m_hWnd)return 0;
        RECT rc = { 0 };
        ::GetClientRect(m_hWnd, &rc);
        return rc.right - rc.left;
    }

    // 取窗口高度, 返回的是客户区的高度
    inline int GetHeight() const {
        if (!m_hWnd)return 0;
        RECT rc = { 0 };
        ::GetClientRect(m_hWnd, &rc);
        return rc.bottom - rc.top;
    }

    // 获取更新区域, 如果没有更新区域则返回空矩形, 所有坐标都为0
    // prc = 接收更新区域的RECT结构指针
    // bErase = 指定是否要删除更新区域中的背景。如果此参数为TRUE并且更新区域不为空，则GetUpdateRect将WM_ERASEBKGND消息发送到指定的窗口以擦除背景。
    inline BOOL GetUpdateRect(RECT* prc = 0, BOOL bErase = false) const {
        if (!m_hWnd)return false;
        return ::GetUpdateRect(m_hWnd, prc, bErase);
    }
    inline BOOL InvalidateRect(const RECT* prc = 0, BOOL bErase = false) const {
        if (!m_hWnd)return false;
        return ::InvalidateRect(m_hWnd, prc, bErase);
    }

    inline BOOL InvalidateRgn(HRGN hRgn, BOOL bErase = false) const {
        if (!m_hWnd)return false;
        return ::InvalidateRgn(m_hWnd, hRgn, bErase);
    }

    inline BOOL Move(int x, int y, int cx, int cy, BOOL bRepaint = true) const {
        if (!m_hWnd)return false;
        return MoveWindow(m_hWnd, x, y, cx, cy, bRepaint);
    }

    inline BOOL Move(const RECT& rc, BOOL bRepaint = true) const {
        if (!m_hWnd)return false;
        return Move(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, bRepaint);
    }

    inline BOOL Move(const RECT* rc, BOOL bRepaint = true) const {
        if (!m_hWnd)return false;
        return Move(rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, bRepaint);
    }

    

    // 通知边框改变, 不修改窗口位置, 强制触发 WM_NCCALCSIZE重新计算边框
    inline BOOL ChangedBorder() const {
        if (!m_hWnd)return false;
        return SetWindowPos(m_hWnd, 0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
    }

    // 调整层次
    inline BOOL After(HWND after, UINT uFlags = 0) const {
        if (!m_hWnd)return false;
        return SetWindowPos(m_hWnd, after, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | uFlags);
    }
    
    // 发送W版消息
    inline INT_PTR Send(UINT message, WPARAM wParam, LPARAM lParam) const {
        if (!m_hWnd)return 0;
        return (INT_PTR)SendMessageW(m_hWnd, message, wParam, lParam);
    }

    // 发送A版消息
    inline INT_PTR SendA(UINT message, WPARAM wParam, LPARAM lParam) const {
        if (!m_hWnd)return 0;
        return (INT_PTR)SendMessageA(m_hWnd, message, wParam, lParam);
    }
#ifndef SMTO_ERRORONEXIT
#define SMTO_ERRORONEXIT 0x20
#endif
    // 发送W版消息
    inline INT_PTR SendTimeOut(UINT message, WPARAM wParam, LPARAM lParam) const {
        if (!m_hWnd)return 0;
        DWORD_PTR ret = 0;
        SendMessageTimeoutW(m_hWnd, message, wParam, lParam,
            SMTO_ABORTIFHUNG | SMTO_NOTIMEOUTIFNOTHUNG | SMTO_ERRORONEXIT,
            1000, &ret);
        return (INT_PTR)ret;
    }

    // 发送A版消息
    inline INT_PTR SendTimeOutA(UINT message, WPARAM wParam, LPARAM lParam) const {
        if (!m_hWnd)return 0;
        DWORD_PTR ret = 0;
        SendMessageTimeoutA(m_hWnd, message, wParam, lParam,
            SMTO_ABORTIFHUNG | SMTO_NOTIMEOUTIFNOTHUNG | SMTO_ERRORONEXIT,
            1000, &ret);
        return (INT_PTR)ret;
    }

    // 投递W版消息
    inline BOOL Post(UINT message, WPARAM wParam, LPARAM lParam) const {
        if (!m_hWnd)return 0;
        return ::PostMessageW(m_hWnd, message, wParam, lParam);
    }

    // 投递A版消息
    inline BOOL PostA(UINT message, WPARAM wParam, LPARAM lParam) const {
        if (!m_hWnd)return 0;
        return ::PostMessageA(m_hWnd, message, wParam, lParam);
    }

    // 显示窗口
    inline BOOL show() const {
        if (!m_hWnd)return false;
        return SetVisible(true);
    }

    // 隐藏窗口
    inline BOOL hide() const {
        if (!m_hWnd)return false;
        return SetVisible(false);
    }

    // 置窗口可视状态
    inline BOOL SetVisible(BOOL isVisible) const {
        if (!m_hWnd)return false;
        return ::ShowWindow(m_hWnd, isVisible ? SW_SHOW : SW_HIDE);
    }

    // 取窗口可视状态
    inline BOOL GetVisible() const {
        if (!m_hWnd)return false;
        return ::IsWindowVisible(m_hWnd);
    }

    // 取窗口可视状态
    inline BOOL IsVisible() const {
        if (!m_hWnd)return false;
        return ::IsWindowVisible(m_hWnd);
    }

    // 置窗口启用状态, true = 启用, false = 禁止
    inline BOOL SetEnabled(BOOL isEnable) const {
        if (!m_hWnd)return false;
        return ::EnableWindow(m_hWnd, isEnable);
    }

    // 取窗口启用状态, true = 启用, false = 禁止
    inline BOOL GetEnabled() const {
        if (!m_hWnd)return false;
        return ::IsWindowEnabled(m_hWnd);
    }

    // 设置窗口样式
    inline DWORD SetStyle(DWORD style, BOOL isEx) const {
        if (!m_hWnd)return 0;
        return (DWORD)SetWindowLongPtrW(m_hWnd, isEx ? GWL_EXSTYLE : GWL_STYLE, style);
    }

    // 取窗口样式
    inline DWORD GetStyle(BOOL isEx) const {
        if (!m_hWnd)return 0;
        return (DWORD)::GetWindowLongPtrW(m_hWnd, isEx ? GWL_EXSTYLE : GWL_STYLE);
    }

    // 添加窗口样式, 在原有的样式上增加一个样式
    inline DWORD InsertStyle(DWORD style, BOOL isEx) const {
        if (!m_hWnd)return 0;
        return (DWORD)::SetWindowLongPtrW(m_hWnd, isEx ? GWL_EXSTYLE : GWL_STYLE, GetStyle(isEx) | style);
    }

    // 去除窗口样式, 在原有的样式上去除一个样式
    inline DWORD RemoveStyle(DWORD style, BOOL isEx) const {
        if (!m_hWnd)return 0;
        return (DWORD)::SetWindowLongPtrW(m_hWnd, isEx ? GWL_EXSTYLE : GWL_STYLE, (GetStyle(isEx) & ~style));
    }

    // 窗口是否存在指定样式
    inline BOOL IsStyle(DWORD style, BOOL isEx) const {
        if (!m_hWnd)return false;
        return (GetStyle(isEx) & style) == style;
    }

//#pragma push_macro("SetProp")
//#pragma push_macro("GetProp")
//#pragma push_macro("RemoveProp")
//#undef SetProp
//#undef GetProp
//#undef RemoveProp
    // 设置附加属性
    inline BOOL SetProp(LPCWSTR name, void* pData) const {
        if (!m_hWnd)return false;
        return ::SetPropW(m_hWnd, name, (HANDLE)pData);
    }

    // 获取附加属性
    inline void* GetProp(LPCWSTR name) const {
        if (!m_hWnd)return 0;
        return ::GetPropW(m_hWnd, name);
    }

    // 移除附加属性
    inline void* RemoveProp(LPCWSTR name) const {
        if (!m_hWnd)return 0;
        return ::RemovePropW(m_hWnd, name);
    }
//#pragma pop_macro("SetProp")
//#pragma pop_macro("GetProp")
//#pragma pop_macro("RemoveProp")
    
    // 取属性列表, 返回数组成员数, 参数传递指针接受数组, 不使用时需要调用 FreeProps() 释放参考回去的参数
    inline int GetProps(PPROPSTRUCT* propAry) const {
        return GetProps(m_hWnd, propAry);
    }
    inline static void FreeProps(PPROPSTRUCT propAry) {
        if (propAry)
            delete[] propAry;
    }
    // 取属性列表, 返回数组成员数, 参数传递指针接受数组, 不使用时需要调用 FreeProps() 释放参考回去的参数
    inline static int GetProps(HWND hWnd, PPROPSTRUCT* propAry) {
        if (!hWnd)return 0;
        ENUMPROPS_PROPDATA data = { 0 };
        if (propAry)
        {
            *propAry = 0;
            data.ary = new PROPSTRUCT[50];
            data.length = 0;
            data.bufLen = 50;
        }
        if (EnumPropsExW(hWnd, EnumProp_Proc, (LPARAM)&data) == -1)
        {
            if (data.ary) delete[] data.ary;
            return 0;
        }
        if (propAry)    // 不接收的时候不会申请这块内存
            *propAry = data.ary;
        return data.length;
    }
private:

    struct ENUMPROPS_PROPDATA
    {
        LPPROPSTRUCT ary;
        DWORD length;
        DWORD bufLen;
    };
    
    // 返回TRUE以继续属性列表枚举, 返回FALSE以停止属性列表枚举
    static BOOL CALLBACK EnumProp_Proc(HWND hWnd, LPWSTR name, HANDLE value, ULONG_PTR param)
    {
        ENUMPROPS_PROPDATA* ptr = (ENUMPROPS_PROPDATA*)param;
        if ( !ptr )return FALSE;
        if ( !ptr->ary )
        {
            ptr->length++;
            return TRUE;
        }
        const DWORD index = ptr->length;
        LPPROPSTRUCT pItem = 0;
        if ( index >= ptr->bufLen )
        {
            const DWORD newSize = index * 2;
            LPPROPSTRUCT newData = new PROPSTRUCT[newSize];
            for ( DWORD i = 0; i < ptr->length; i++ )
                newData[i].move(ptr->ary[i]);
            delete[] ptr->ary;
            ptr->ary = newData;
            ptr->bufLen = newSize;
        }

        ptr->length++;
        PROPSTRUCT& item = ptr->ary[index];
        if ( (LONG_PTR)name <= 0xffff )
        {
            // 是原子, 不是文本, 需要转成文本
            wstr text = wstr::GetFormat(L" ATOM:%d", (int)(LONG_PTR)name);
            item = text.c_str();
        }
        else
        {
            item = name;
        }
        item.value = value;
        return TRUE;
    }
public:

    // 设置鼠标指针
    inline LONG_PTR SetCursor(HCURSOR hCursor) const {
        if (!m_hWnd)return 0;
        return ::SetClassLongPtrW(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCursor);
    }

    // 取鼠标指针
    inline HCURSOR GetCursor() const {
        if (!m_hWnd)return 0;
        return (HCURSOR)::GetClassLongPtrW(m_hWnd, GCLP_HCURSOR);
    }

    // 捕获鼠标, 返回之前的窗口句柄
    inline HWND SetCapture() const {
        if (!m_hWnd)return 0;
        return ::SetCapture(m_hWnd);
    }

    // 释放捕获鼠标
    inline BOOL ReleaseCapture() const {
        return ::ReleaseCapture();
    }

    // 设置父窗口, 返回上一个父窗口的句柄
    inline HWND SetParent(HWND hWndParent) const {
        if (!m_hWnd)return 0;
        return ::SetParent(m_hWnd, hWndParent);
    }

    // 取W版窗口标题
    inline wstr GetWindowTextW() const {
        if (!m_hWnd)
            return wstr();
        int len = GetWindowTextLengthW(m_hWnd) + 1;
        wstr ret(len);
        ::GetWindowTextW(m_hWnd, ret.data(), len + 1);
        ret.resize(wcslen(ret.c_str()));
        return ret;
    }

    // 取A版窗口标题
    inline _str GetWindowTextA() const {
        if (!m_hWnd)return _str();
        int len = GetWindowTextLengthA(m_hWnd) + 1;
        _str ret(len);
        ::GetWindowTextA(m_hWnd, ret.data(), len + 1);
        ret.resize(strlen(ret.c_str()));
        return ret;
    }

    // 置W版窗口标题
    inline BOOL SetWindowTextW(LPCWSTR lpString) const {
        if (!m_hWnd)return false;
        return ::SetWindowTextW(m_hWnd, lpString);
    }

    // 置A版窗口标题
    inline BOOL SetWindowTextA(LPCSTR lpString) const {
        if (!m_hWnd)return false;
        return ::SetWindowTextA(m_hWnd, lpString);
    }

    // 设置字体
    inline void SetFont(HFONT hFont, BOOL isUpdate = false) const {
        if (!m_hWnd)return;
        ::SendMessageW(m_hWnd, WM_SETFONT, (WPARAM)hFont, isUpdate);
    }

    virtual void SetFont(const wchar_t* name = L"微软雅黑", int size = -12, BOOL isUpdate = false) const {
        if (!m_hWnd)return;
        Send(WM_SETFONT, (WPARAM)CreateFontW(name, size), isUpdate);
    }

    // 获取字体
    inline HFONT GetFont() const {
        if (!m_hWnd)return 0;
        return (HFONT)::SendMessageW(m_hWnd, WM_GETFONT, 0, 0);
    }

    //// 文件拖放
    //inline void drag(BOOL isDrag) {
    //    DragAcceptFiles(m_hWnd, isDrag);
    //}

    // 支持在创建窗口前调用, 子类化窗口, 接收消息, 为0则清空, 需要调用类内方法处理请调用类内方法 WndProc
    inline void SubClass(WND_SUBPROC proc = 0) {
        if (!m_data)
        {
            m_data = new PROP_SUBWNDDATA;
            memset(m_data, 0, sizeof(PROP_SUBWNDDATA));
        }

        m_data->subFun = proc;
        m_data->isSubClass = proc != 0;
        if ( !m_data->oldProc && IsWindow() )    // 窗口已创建, 但是没有子类化, 内部处理一下子类化
        {
            // 子类化全部都是由内部转发出去, 如果消息不经过内部, 有部分功能不好实现
            SetProp(PROP_WNDPROC, m_data);
            m_data->oldProc = (WNDPROC)SetWindowLongPtrW(m_hWnd, GWLP_WNDPROC, (LONG_PTR)CWndBase::CWndBase_DefWndProc);
        }
    }

};

// 控件基类, create() 方法多了一个子窗口样式
class CControlBase : public CWndBase
{
public:
    CControlBase() :CWndBase() { ; }
    virtual ~CControlBase() { ; }
public:
    virtual HWND create(DWORD dwExStyle, LPCWSTR lpszTitle, DWORD dwStyle,
        int x, int y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR id = 0, LPVOID lpParam = 0) {
        return CWndBase::create(dwExStyle, lpszTitle, dwStyle | WS_CHILD, x, y, nWidth, nHeight, hWndParent, id, lpParam);
    }
};

