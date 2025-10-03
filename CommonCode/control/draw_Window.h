#pragma once
#include "draw_Window_include.h"

#define DRAW_WND_BYCOMMAND      0x00000000L // nPosition 指的是ID
#define DRAW_WND_BYPOSITION     0x00000400L // nPosition 指的是位置

// 自绘窗口, 绘画系统按钮, 标题, 图标, 边框阴影等
// 
class CDrawWindow
{
    LPDRAW_WINDOW_DATA m_data;
public:
    // 绑定窗口, 绑定后内部会为这个窗口进行自绘处理
    // arg = 窗口的一些基础信息
    virtual bool BindWindow(const DRAW_WINDOW_ARGUMENT* arg);
    static LPDRAW_WINDOW_DATA BindWindow_s(const DRAW_WINDOW_ARGUMENT* arg);
    static LPDRAW_WINDOW_DATA GetObjFromHwnd(HWND hWnd);

    // 添加系统按钮, 返回系统按钮数量
    // hWnd = 需要添加系统按钮的窗口
    // id = 系统按钮ID
    // param = 按钮数值
    virtual int AddSysButton(int id, LPARAM param);
    static int AddSysButton(HWND hWnd, int id, LPARAM param);
    static int AddSysButton(LPDRAW_WINDOW_DATA data, int id, LPARAM param);

    // 插入系统按钮, 返回插入删除成功
    // hWnd = 需要添加系统按钮的窗口
    // nPosition = 要插入的位置, 插入后这个按钮的索引就是这个值
    // id = 系统按钮ID
    // param = 按钮数值
    // flags = DRAW_WND_ 开头常量, 表示 nPosition 是位置还是ID
    virtual int InsSysButton(int nPosition, int id, LPARAM param, int flags);
    static  int InsSysButton(HWND hWnd, int nPosition, int id, LPARAM param, int flags);
    static  int InsSysButton(LPDRAW_WINDOW_DATA data, int nPosition, int id, LPARAM param, int flags);

    // 删除系统按钮, 返回是否删除成功
    // hWnd = 需要删除系统按钮的窗口
    // id = 系统按钮ID
    // flags = DRAW_WND_ 开头常量, 表示 nPosition 是位置还是ID
    virtual int DelSysButton(int nPosition, int flags);
    static  int DelSysButton(HWND hWnd, int nPosition, int flags);
    static  int DelSysButton(LPDRAW_WINDOW_DATA data, int nPosition, int flags);

    // 设置系统按钮项目数值, 返回设置前的项目数值
    // hWnd = 需要获设置统按钮项目数值的窗口
    // nPosition = 要设置的按钮ID或位置
    // param = 要设置的数值
    // flags = DRAW_WND_ 开头常量, 表示 nPosition 是位置还是ID
    virtual LPARAM SetButtonParam(int nPosition, LPARAM param, int flags);
    static  LPARAM SetButtonParam(HWND hWnd, int nPosition, LPARAM param, int flags);
    static  LPARAM SetButtonParam(LPDRAW_WINDOW_DATA data, int nPosition, LPARAM param, int flags);

    // 获取系统按钮项目数值
    // hWnd = 需要获取系统按钮项目数值的窗口
    // nPosition = 要获取的按钮ID或位置
    // flags = DRAW_WND_ 开头常量, 表示 nPosition 是位置还是ID
    virtual LPARAM GetButtonParam(int nPosition, int flags);
    static  LPARAM GetButtonParam(HWND hWnd, int nPosition, int flags);
    static  LPARAM GetButtonParam(LPDRAW_WINDOW_DATA data, int nPosition, int flags);

    // 从按钮索引获取系统按钮ID
    // hWnd = 系统按钮所在窗口
    // nPosition = 要获取的按钮位置
    virtual int GetButtonIdFromPos(int nPosition);
    static  int GetButtonIdFromPos(HWND hWnd, int nPosition);
    static  int GetButtonIdFromPos(LPDRAW_WINDOW_DATA data, int nPosition);

    // 从按钮ID获取系统按钮的索引
    // hWnd = 系统按钮所在窗口
    // id = 按钮ID
    virtual int GetButtonPosFromId(int id);
    static  int GetButtonPosFromId(HWND hWnd, int id);
    static  int GetButtonPosFromId(LPDRAW_WINDOW_DATA data, int id);

    // 设置绘画句柄, 返回原来的画布句柄
    virtual LPCANVAS SetCanvas(LPCANVAS hCanvas);

    // 内部创建一个窗口, 并重画这个窗口
    // dwExStyle = 窗口扩展样式
    // lpWindowName = 窗口标题
    // dwStyle = 窗口样式
    // x,y,cx,cy = 窗口左顶宽高
    // hWndParent = 父窗口, 可以为0
    // arg = 窗口的子类化参数, 为0则使用默认配置
    virtual HWND create(DWORD dwExStyle, LPCWSTR lpWindowName, DWORD dwStyle, int x, int y, int cx, int cy, HWND hWndParent = 0, const DRAW_WINDOW_ARGUMENT* arg = 0);
    static HWND create_s(DWORD dwExStyle, LPCWSTR lpWindowName, DWORD dwStyle, int x, int y, int cx, int cy, HWND hWndParent = 0, const DRAW_WINDOW_ARGUMENT* arg = 0);


    // 消息处理函数, 返回内部是否已处理这条消息, pRet = 接收处理完的返回值
    // message = 需要处理的消息
    // wParam, lParam = 消息的两个参数
    // oldProc = 原来处理消息的回调地址, 内部有可能会需要调用原函数
    // pRet = 接收处理完的消息返回值
    virtual bool WindowProc(UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc, LRESULT* pRet);
    static bool WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc, LRESULT* pRet);


    // 绘画系统按钮, 内部不会调用 BeginPaint 和 EndPaint, 内部直接使用 hCanvas, 把系统按钮, 图标, 标题画出来
    // 这个方法是调用 DrawSysIcon, DrawTitle, DrawSysButton 这几个方法实现的, 位置是内部记录的位置
    // 需要更灵活的方式请调用单独的绘画命令
    // hWnd = 窗口句柄
    // ps = BeginPaint 得到的结构数据, 内部会根据重画区域判断是否进行绘画
    // hCanvas = 画布句柄, 内部直接使用这个画布句柄进行绘画
    virtual bool OnPaint(const PAINTSTRUCT& ps, LPCANVAS hCanvas);

    // 绘画图标, 可以在指定的位置绘画系统图标
    // ps = BeginPaint 得到的结构数据, 内部会根据重画区域判断是否进行绘画
    // hCanvas = 画布句柄, 内部直接使用这个画布句柄进行绘画
    virtual bool DrawSysIcon(const PAINTSTRUCT& ps, LPCANVAS hCanvas, const RECT_F& rcIcon);
    static  bool DrawSysIcon(LPDRAW_WINDOW_DATA data, const PAINTSTRUCT& ps, LPCANVAS hCanvas, const RECT_F& rcIcon);

    // 绘画标题, 可以在指定的位置绘画标题
    // ps = BeginPaint 得到的结构数据, 内部会根据重画区域判断是否进行绘画
    // hCanvas = 画布句柄, 内部直接使用这个画布句柄进行绘画
    // rcTitle = 绘画标题的位置
    virtual bool DrawTitle(const PAINTSTRUCT& ps, LPCANVAS hCanvas, const RECT_F& rcTitle);
    static  bool DrawTitle(LPDRAW_WINDOW_DATA data, const PAINTSTRUCT& ps, LPCANVAS hCanvas, const RECT_F& rcTitle);

    // 绘画系统, 可以在指定的位置绘画系统按钮, 系统按钮从右到左逐个绘画
    // ps = BeginPaint 得到的结构数据, 内部会根据重画区域判断是否进行绘画
    // hCanvas = 画布句柄, 内部直接使用这个画布句柄进行绘画
    // rcIcon = 整个系统图标占用的位置, 内部从右到左逐个绘画图标
    virtual bool DrawSysButton(const PAINTSTRUCT& ps, LPCANVAS hCanvas, const RECT_F& rcBtn);
    static  bool DrawSysButton(LPDRAW_WINDOW_DATA data, const PAINTSTRUCT& ps, LPCANVAS hCanvas, const RECT_F& rcBtn);

    virtual bool EventDraw(PFN_SysDraw pfn, LPARAM param);
    static  bool EventDraw(HWND hWnd, PFN_SysDraw pfn, LPARAM param);
    static  bool EventDraw(LPDRAW_WINDOW_DATA data, PFN_SysDraw pfn, LPARAM param);
    virtual bool EventBtnClick(PFN_SysBtnClick pfn, LPARAM param);
    static  bool EventBtnClick(HWND hWnd, PFN_SysBtnClick pfn, LPARAM param);
    static  bool EventBtnClick(LPDRAW_WINDOW_DATA data, PFN_SysBtnClick pfn, LPARAM param);


    // 获取默认的参数结构
    static DRAW_WINDOW_ARGUMENT GetDefArg(HWND hWnd);

};
