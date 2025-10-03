#pragma once
#include <windows.h>
#include "CControlDraw.h"
#include "src/scroll.h"
#include <commctrl.h>


// 初始化自绘滚动条
bool IDrawScroll_Init();
// 取消初始化自绘滚动条
bool IDrawScroll_UnInit();
class IDrawScroll;

enum ISCROLL_TYPE
{
    ISCROLL_DRAWTYPE_BACK       = 0x00, // 绘画背景, 这里接收到的dc是内存dc
    ISCROLL_DRAWTYPE_BTN1       = 0x01, // 绘画滚动条的按钮1, 这里接收到的dc是内存dc
    ISCROLL_DRAWTYPE_BTN2       = 0x02, // 绘画滚动条的按2, 这里接收到的dc是内存dc
    ISCROLL_DRAWTYPE_THUMB      = 0x03, // 绘画滚动条的滑块, 这里接收到的dc是内存dc
    ISCROLL_DRAWTYPE_BEFOR      = 0x04, // 绘画开始, 暂时没有处理这个事件
    ISCROLL_DRAWTYPE_ALERT      = 0x05, // 绘画结束, 这里接收到的HDC是窗口的hdc, 接收到的位置是计算出来有可能需要绘画的位置, 触发这个事件的时候滚动条已经绘画到这个dc上了
    ISCROLL_DRAWTYPE_BORDER     = 0x06, // 绘画边框, 这里接收到的HDC是窗口的hdc, 接收到的位置是窗口边框矩形, 触发这个事件的时候滚动条已经绘画到这个dc上了

    ISCROLL_GETRECT_SCREEN      = 0x10, // 计算滚动条在屏幕中的位置
    ISCROLL_GETRECT_CLIENT      = 0x11, // 计算滚动条在窗口中的位置
    ISCROLL_GETRECT_BORDER      = 0x12, // 计算窗口非客户区位置, 内部是根据这个位置计算滚动条绘画位置的, 可以增减这个结构让绘画位置进行改变
};
typedef bool (CALLBACK* pfn_IScroll_CalcScroll)(IDrawScroll* pObj, RECT* prc);
typedef bool (CALLBACK* pfn_IScroll_DrawScroll)(IDrawScroll* pObj, bool isVScroll, HDC hdc, Gdiplus::Graphics* gp, ISCROLL_TYPE type, RECT_F* rcDraw);
typedef bool (CALLBACK* pfn_IScroll_GetScrollRect)(IDrawScroll* pObj, bool isVScroll, ISCROLL_TYPE type, RECT* prc);

// 把窗口非客户区空出一块地方绘制滚动条
class IDrawScroll
{
    // 按钮位置, 在重画时记录
    struct DRAWSCROLL_RECT
    {
        RECT thumb; // 滑块的位置
        RECT btn1;  // 按钮1的位置
        RECT btn2;  // 按钮2的位置
        RECT rc;    // 滑块条的位置
        float maxBlockHeight;   // 滑块的最大像素, 滚动条尺寸 - 两个按钮的尺寸
        float blockHeight;      // 滑块的高度
        float maxPos;           // 最大位置 - 页数量 + 1
    };


public:
    void clear();
    MEMDCBITMAP m_md;   // 整个窗口dc
    MEMDCBITMAP m_vmd;  // 纵向滚动条dc
    MEMDCBITMAP m_hmd;  // 横向滚动条dc

    UINT m_dpi;         // DPI缩放, 根据这个值进行缩放

    pfn_IScroll_CalcScroll m_pfn_CalcScroll;
    pfn_IScroll_DrawScroll m_pfn_DrawScroll;
    pfn_IScroll_GetScrollRect m_pfn_GetScrollRect;

    DRAWSCROLL_RECT m_vrc;      // 纵向滚动条位置, 在重画的时候记录
    DRAWSCROLL_RECT m_hrc;      // 横向滚动条位置, 在重画的时候记录
    bool m_lDownIsVscroll;      // 左键按下的是否为纵向滚动条
    int  m_isCapture;           // 是否捕获了鼠标, 1=纵向滚动条, 2=横向滚动条, 0=未捕获

    int m_VState;               // 纵向滚动条当前状态, 低位是状态, 高位是哪个按钮, 1=按钮1, 2=按钮2, 3=滑块, 其他=没有
    int m_HState;               // 横向滚动条当前状态, 低位是状态, 高位是哪个按钮, 1=按钮1, 2=按钮2, 3=滑块, 其他=没有
    POINT m_downOffset;         // 按下鼠标时鼠标在滑块里的位置, 横向使用x, 纵向使用y
    int m_downPos;              // 按下时滚动条的位置
    POINT m_downPoint;          // 鼠标按下时的位置

    HPEN m_hPen;
    int m_nBorderWidth;         // 边框宽度, 有的就为1, 没有就为0


public:

    HWND m_hWnd;                // 绑定的窗口句柄
    HWND m_hWndVBind;           // 绑定的纵向滚动条句柄
    HWND m_hWndHBind;           // 绑定的横向滚动条句柄

    bool m_isVScroll;           // 是否有纵向滚动条
    bool m_isHScroll;           // 是否有横向滚动条
    bool m_isCallSetScroll;     // 是否调用原函数
    bool m_isAbsScroll;         // 绝对画出滚动条
    bool m_isControl;           // 是否为滚动条控件
    bool m_isBind;              // 是否为绑定的组件
    bool m_isSleep;             // 是否延时发送

    SCROLLINFO m_vScroll;       // 纵向滚动条信息
    SCROLLINFO m_hScroll;       // 横向滚动条信息

    SCROLL_RECT_INFO m_vInfo;   // 纵向滚动条位置
    SCROLL_RECT_INFO m_hInfo;   // 横向滚动条位置
    Gdiplus::Bitmap* m_VBmp;    // 纵向滚动条位图, 位置信息是由 m_vInfo 记录
    Gdiplus::Bitmap* m_HBmp;    // 横向滚动条位图, 位置信息是由 m_hInfo 记录
    int m_PageDown;             // 时钟连续滚动页面标识, SendMessage的wParam参数
    int m_isSysListView;        // 是否为超级列表框, 超列需要特殊处理, 为0表示不是超列, 否则为表项高度
    int m_isSysTreeView;        // 是否为树型框, 树型框需要先设置位置在发送消息

    POINT m_thumbPoint;         // 按照移动时候的坐标

    //int m_vThumbSize;           // 纵向滚动条背景滑块宽度
    //int m_hThumbSize;           // 横向滚动条背景滑块高度

public:
    // 构造对象时必须初始化的数据
    // 边框颜色, 窗口有边框时绘画, 内部会创建一个画笔
    IDrawScroll(COLORREF crBorder, SCROLL_RECT_INFO& info);
    ~IDrawScroll();

    inline void OnCalcScroll(pfn_IScroll_CalcScroll callback = 0) { m_pfn_CalcScroll = callback; }
    inline void OnDrawScroll(pfn_IScroll_DrawScroll callback = 0) { m_pfn_DrawScroll = callback; }
    inline void OnGetScrollRect(pfn_IScroll_GetScrollRect callback = 0) { m_pfn_GetScrollRect = callback; }
    void SetBorderColor(COLORREF clrBorder);
public:

    static bool IsBindControl(HWND hWnd, bool* isVScroll);
    static void SetBindControl(HWND hWnd, bool isBind, bool isVScroll);
    static IDrawScroll* GetobjFromHwnd(HWND hWnd);
    static IDrawScroll* SetobjFromHwnd(HWND hWnd, IDrawScroll* pScroll);
    // 绑定窗口, 绑定后内部会处理滚动条信息
    // hWnd = 要绑定的窗口
    // isSubClass = 是否子类化, 如果为true, 内部会子类化这个窗口, 如果为假, 则必须外部把消息传递到 OnWndProc 方法里
    // isReCalcSize = 是否重新计算尺寸, 为true则内部会调用 SetWindowPos 进行重新计算非客户区
    bool bind(HWND hWnd, bool isSubClass = false, bool isReCalcSize = false);

    
    // 可以将窗口所有消息都投递进来, 内部处理需要处理的消息, 返回是否已处理, 返回值是pRet
    bool OnWndProc(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    void InvalidateRect() {
        if (m_isControl)
            ::InvalidateRect(m_hWnd, 0, 0);
        else
            OnNcPaint(0, m_hWnd, WM_NCPAINT, 0, 0, 0);
    }

    // 取纵向滚动条位置, 返回屏幕坐标
    bool GetVScrollRect(RECT* rcScreen = 0, RECT* rcClient = 0, RECT* rcBtn1 = 0, RECT* rcBtn2 = 0, RECT* rcThumb = 0, RECT* rcWindow = 0, RECT* rcWindowClient = 0, RECT* rcBorder = 0);

    // 取横向滚动条位置, 返回屏幕坐标
    bool GetHScrollRect(RECT* rcScreen = 0, RECT* rcClient = 0, RECT* rcBtn1 = 0, RECT* rcBtn2 = 0, RECT* rcThumb = 0, RECT* rcWindow = 0, RECT* rcWindowClient = 0, RECT* rcBorder = 0);


    inline bool IsState(int state, bool isVScroll) { return ((GetState(isVScroll)) & state) == state; }
    inline void SetState(int state, bool isVScroll) {
        int i = MAKELONG(state, HIWORD(isVScroll ? m_VState : m_HState));
        if (isVScroll) m_VState = i;
        else m_HState = i;
    }
    inline int GetState(bool isVScroll) { return LOWORD(isVScroll ? m_VState : m_HState); }
    inline void SetIndex(int index, bool isVScroll) {
        int i = MAKELONG(LOWORD(isVScroll ? m_VState : m_HState), index);
        if (isVScroll) m_VState = i;
        else m_HState = i;
    }
    inline int GetIndex(bool isVScroll) { return HIWORD(isVScroll ? m_VState : m_HState); }

    bool OnDrawScroll(HDC hdcWindow);
private:

    // 坐标点是哪个按钮的位置
    int GetVHittestPos(const POINT& pt);
    int GetHHittestPos(const POINT& pt);
    
    bool OnNcHittest(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnNcMouseMove(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnNcCalcSize(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnPaint(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnPrint(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnNcPaint(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnNcLbuttonDown(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnCaptureChanged(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnNcLbuttonUp(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    // 鼠标按下滑块后移动鼠标, 没有按下滑块的话不会触发
    bool OnThumbMove(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnNcLbuttonDblclk(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnNcRbuttonDown(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnNcRbuttonUp(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnNcRbuttonDblclk(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);

    bool OnHScroll(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnVScroll(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);



    // 绘制纵向滚动条, 传递一个hdc和绘制区域进来, 内部把结果绘画到这个dc上
    // cxClient = 纵向滚动条宽度
    // cyClient = 纵向滚动条高度
    void DrawVScroll(HDC hdc, int cxClient, int cyClient, const RECT& rcScroll);

    // 绘制横向滚动条, 传递一个hdc和绘制区域进来, 内部把结果绘画到这个dc上
    // cxClient = 横向滚动条宽度
    // cyClient = 横向滚动条高度
    void DrawHScroll(HDC hdc, int cxClient, int cyClient, const RECT& rcScroll);
};