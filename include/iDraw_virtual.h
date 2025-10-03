#pragma once
#include <control/WndBaseTypedef.h>
#include <control/WndBaseObjStruct.h>
#include <assist/CPrivateProfile.h>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "EIDE_Virtual.h"
#include <control/ReadColor.h>

//#include <windows.h>
//#include <d2d/_canvas_fundef.h>
//#include <assist_draw.h>
//#include <control/WndBaseState.h>
//#include <control/WndBaseObjStruct.h>

// 各个组件开始和结束绘画使用的结构
struct BEGIN_END_PAINT_STRUCT : PAINTSTRUCT
{
    RECT rc;
    int cxClient;
    int cyClient;
    LPEX_FONT pFont;
    HFONT hFont;
    LPMEMDCBITMAP md;
    BOOL isDxRender;
    LPOBJSTRUCT pData;
};


// 顶部状态信息
typedef struct TOPSTATE_INFO
{
    RECT rc[5];         // 0=关闭按钮, 1=图钉按钮, 2=三角按钮, 3=组件切换按钮, 4=最大化/还原按钮 存放的都是客户区坐标
    int index;          // 当前绘画的是哪个按钮
    bool isDown;        // 是否是按下状态, 如果已经按下, 会捕获鼠标, 可以在鼠标离开的时钟里判断这个值, 为false时才重画
    int state;          // 按钮状态, 状态请使用 STATE:: 里的状态
    int count;          // 按钮数量
    int flags;          // 记录位置, 是否最大化, 是否浮动, 是否停靠等信息 TOP_STATE_BUTTON 枚举常量里的值, 调用 __query_flags 判断值
    bool isSelect;      // 图钉按钮是否选中, 选中的话使用 横向的图标, 未选中使用纵向图标, 没选中就是停靠, 选中就是浮动, 选中会自动隐藏
    bool isRestore;     // 是否显示还原图标, 如果为true, 那就绘画还原图标, 否则绘画最大化图标
    bool isDownTitle;   // 是否点击在标题的位置, 在这个位置点击一般就是要移动窗口了
    wchar_t name[20];   // 最大支持20个字符
    RECT rcOffset;      // 偏移量
    RECT rcWindow;      // 窗口位置, 计算坐标时记录, 绘画顶边的状态条的时候填充这个矩形
    HWND hWnd;          // 触发点击事件的窗口句柄
}*LPTOPSTATE_INFO;

// 查询顶部状态条是否存在指定值
// info = LPTOPSTATE_INFO 结构指针
// state = 查询的值, TOP_STATE_BUTTON 枚举常量的值
#define __query_flags(_info, _state)    (((_info->flags) & (_state)) == (_state))

// 添加指定的标识到结构里
// info = LPTOPSTATE_INFO 结构指针
// state = 查询的值, TOP_STATE_BUTTON 枚举常量的值
#define __add_flags(_info, _state)      ((_info->flags) |= (_state))

// 从结构里删除指定标识
// info = LPTOPSTATE_INFO 结构指针
// state = 查询的值, TOP_STATE_BUTTON 枚举常量的值
#define __del_flags(_info, _state)      ((_info->flags) &= ~(_state))

enum TOP_STATE_BUTTON : int
{
    TOP_STATE_BUTTON_UNKNOWN            = 0x00000000,   // 没有按钮
    TOP_STATE_BUTTON_CLOSE              = 0x00000001,   // 是否存在关闭按钮
    TOP_STATE_BUTTON_PUSHPIN            = 0x00000002,   // 是否存在图钉按钮, 
    TOP_STATE_BUTTON_PUSHPIN_SEL        = 0x00000004,   // 是否存在图钉选中按钮, 选中就是横向, 会自动隐藏
    TOP_STATE_BUTTON_TRIANGLE           = 0x00000008,   // 是否存在三角按钮
    TOP_STATE_BUTTON_SWITCH             = 0x00000010,   // 是否存在组件箱切换按钮
    TOP_STATE_BUTTON_MAXBOX             = 0x00000020,   // 是否存在最大化按钮
    TOP_STATE_BUTTON_RESTORE            = 0x00000040,   // 是否存在还原按钮

    TOP_STATE_BUTTON_ISFLOAT            = 0x00001000,   // 是否浮动, 有这个值表示存在最大化按钮, 要么是最大化, 要么是还原, 浮动菜单需要禁止
    TOP_STATE_BUTTON_ISSTOP             = 0x00002000,   // 是否停靠, 有这个值表示没有最大化按钮, 停靠菜单需要禁止
    TOP_STATE_BUTTON_ISAOTUHIDE         = 0x00004000,   // 是否自动隐藏, 有这个值表示图钉按钮是竖着的, 不会自动隐藏, 自动隐藏菜单需要禁止

    TOP_STATE_RESERVE_LEFT              = 0x00010000,   // 窗口预留位置, 左边会预留一部分出来允许调整尺寸
    TOP_STATE_RESERVE_TOP               = 0x00020000,   // 窗口预留位置, 顶边会预留一部分出来允许调整尺寸
    TOP_STATE_RESERVE_RIGHT             = 0x00040000,   // 窗口预留位置, 右边会预留一部分出来允许调整尺寸
    TOP_STATE_RESERVE_BOTTOM            = 0x00080000,   // 窗口预留位置, 底边会预留一部分出来允许调整尺寸

    TOP_STATE_DIRECTION_LEFT            = 0x00100000,   // 窗口位置, 当前窗口在左边
    TOP_STATE_DIRECTION_TOP             = 0x00200000,   // 窗口位置, 当前窗口在顶边
    TOP_STATE_DIRECTION_RIGHT           = 0x00400000,   // 窗口位置, 当前窗口在右边
    TOP_STATE_DIRECTION_BOTTOM          = 0x00800000,   // 窗口位置, 当前窗口在底边

    TOP_STATE_BUTTON_DEF                = (TOP_STATE_BUTTON_CLOSE | TOP_STATE_BUTTON_PUSHPIN | TOP_STATE_BUTTON_TRIANGLE),  // 默认按钮
    TOP_STATE_BUTTON_DEF_MAXBOX         = (TOP_STATE_BUTTON_DEF | TOP_STATE_BUTTON_MAXBOX),     // 浮动窗口时的默认按钮

    TOP_STATE_BUTTON_CONTROL            = (TOP_STATE_BUTTON_DEF | TOP_STATE_BUTTON_SWITCH),     // 组件箱的切换套餐...
    TOP_STATE_BUTTON_CONTROL_MAXBOX     = (TOP_STATE_BUTTON_CONTROL | TOP_STATE_BUTTON_MAXBOX), // 组件箱浮动窗口时的切换套餐...

    TOP_STATE_ISSHOW                    = 0x01000000,   // 窗口是否显示
    TOP_STATE_ISCALCLAYOUT              = 0x02000000,   // 调整窗口时是否计算位置, 没有这个标识的时候调整窗口就是固定位置
    TOP_STATE_ISTYPE_WIDTH              = 0x04000000,   // 记录的尺寸是否为宽度, 如果没有这个标识那就是高度
    TOP_STATE_ISGROUP                   = 0x08000000,   // 当前窗口是否为分组, 有这个标识就需要绘画选择夹
};

struct MDICLIENT_ALL_WINDOW
{
    HWND hChild1;       // 最外层窗口, MDIClient下的第一个子窗口
    HWND hChild2;       // 第二层子窗口, 里面包含3个滚动条
    HWND hChild3;       // 第三层子窗口, 代码框的父窗口, 里面包含代码框
    HWND hCode;         // 代码窗口
    HWND hvScroll;      // 纵向滚动条窗口, id = 59904
    HWND hhScroll;      // 横向滚动条窗口, id = 59920
};


// 图标结构, 存放dx/gdi+图片指针, 图标的位置, ID, 说明, 备注, 文本颜色, 背景颜色....
// 一个ID指向的数据都保存在这个结构里
typedef struct WINDOW_ICON_DATA
{
    Gdiplus::Bitmap* hBitmapGdip;   // gdi+ 位图
    LPEX_IMAGE hBitmapDx;           // dx 位图
    RECT rc[20];                    // 从图片哪个位置绘画, 4个状态, 0=支持,1=热点,2=按下,3=禁止, 记录5种DPI缩放位置
    LPCWSTR id;                     // 这个结构的id, ires的每个图标都是这个结构, 需要通过这个id找到这个结构
    LPCWSTR text;                   // 绘画显示的文本, 如果为0则使用原来的文本
    LPCWSTR remarks;                // 备注文本, 目前暂时未使用
    DWORD crText[4];                // ARGB颜色, 项目文本颜色
    DWORD crBack[4];                // ARGB颜色, 项目背景颜色
    DWORD by[5];                    // 备用
    float cxIcon;                   // 图标宽度
    float cyIcon;                   // 图标高度
    int nIconType;                  // 图标从哪个资源里获取的, 0=用户图标, 1=用户默认, 2=默认皮肤里图标, 3=默认里的默认图标
    int nDpi;                       // 获取到的图标的DPI, 如果获取150%缩放的失败了, 会取获取100%的图标, 如果100%的也获取失败, 那就返回0
    //TODO 获取图标的参数应该需要减少, 把部分返回的数据写入这个结构
    // 返回图标来源, 返回图标dpi缩放, 返回图标尺寸, 都应该用这个结构返回
}*LPWINDOW_ICON_DATA;

DECLARE_HANDLE(IDRAW_HRESOURCE);
typedef LPWINDOW_ICON_DATA(WINAPI* pfn_GetWindowIcon)(HWND hWnd, IDRAW_HRESOURCE hResource, LPCWSTR pszID, void* pArg, RECT_F* prc, STATE state, UINT dpi);
// 按钮被重画时的回调函数
typedef int(WINAPI* pfn_ToolDrawButton)(LPCANVAS hCanvas, int id, LPARAM param, const RECT_F* prc, STATE state, UINT dpi);


// 添加按钮到接口工具条中使用的结构
typedef struct IDRAW_STRUCT_TOOL_INSERT
{
    int idCommand;              // 按钮的ID, 触发事件时会把这个ID传递过去
    LPCWSTR lpszTitle;          // 按钮标题, 可以为0, 为0则没有标题, 鼠标移动上去时弹出的提示说明标题, 加粗显示, 这个标题不会显示在工具条上
    LPCWSTR lpszTips;           // 按钮说明, 可以为0, 为0则没有说明, 鼠标移动上去时弹出的提示说明
    LPARAM param;               // 项目数值
    BYTE fsState;               // 按钮状态, 请参考msdn的TBBUTTON 结构说明
    BYTE fsStyle;               // 按钮样式, 请参考msdn的TBBUTTON 结构说明
    BYTE bReserved[6];          // 保留参数
    pfn_ToolDrawButton pfnDraw; // 绘画回调
    const void* pImageData;     // 图片数据指针
    int nImageSize;             // 图片数据大小
    void* pPlugin;              // 触发事件调用的插件回调
}*LPIDRAW_STRUCT_TOOL_INSERT;



struct BINDWINDOW_INFO
{
    IDRAW_HRESOURCE hResource;
    pfn_GetWindowIcon callback;
    LPCSTR guid;
};

// 绘画接口类
class IiDraw
{
public:
    virtual int Release() = 0;
    virtual int Addref() = 0;

public:



    // 子类化窗口, 内部使用固定的配色, 需要单独修改颜色的可以返回值的指针进行替换
    // hWnd = 被子类化的窗口句柄
    // proc = 接收子类化的回调函数
    // reMoveBorder = 是否删除边框, 会删除 WS_BORDER 和 扩展样式几个会添加边框的样式
    // isSetBack = 是否设置背景颜色, 如果设置, 那么相同类名绘画背景时都会使用这个颜色绘画, 包括设计器窗口也一样
    // reMoveClassStyle = 要删除的类样式, 删除这个样式后所有这个类名的窗口都会被删除这个样式
    virtual LPOBJSTRUCT _subclass(HWND hWnd, pfn_SubClassWndProc proc, bool reMoveBorder = true, bool isSetBack = true, DWORD reMoveClassStyle = CS_HREDRAW | CS_VREDRAW) = 0;


    virtual LPOBJSTRUCT SubButton(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle) = 0;
    virtual LPOBJSTRUCT SubEdit(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, bool isScroll = true) = 0;
    virtual LPOBJSTRUCT SubStatic(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle) = 0;
    virtual LPOBJSTRUCT SubStatusBar(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle) = 0;

    // 重画树型框
    // hWnd = 树型框窗口句柄
    // iItemHeight = 项目高度
    // isScroll = 是否重画滚动条
    // isHot = 是否有热点效果
    virtual LPOBJSTRUCT SubTreeView(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int ItemHeight, bool isScroll = true, bool isHot = false) = 0;

    virtual LPOBJSTRUCT SubListBox(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int nItemHeight, bool isScroll = true, bool isHot = false) = 0;
    virtual LPOBJSTRUCT SubListView(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int ItemHeight, bool isScroll = true, bool isHot = false, int fmt = DT_VCENTER | DT_SINGLELINE) = 0;

    virtual LPOBJSTRUCT SubTool(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle) = 0;

    virtual LPOBJSTRUCT Submsctls_updown32(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle) = 0;

    virtual LPOBJSTRUCT SubTab(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle) = 0;
    virtual LPOBJSTRUCT SubComboBox(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int nItemHeight) = 0;
    virtual LPVOID DrawScroll(HWND hWnd, int nListViewItemHeight, bool isTreeView, bool isCallSetScrollInfo) = 0;


    // 子类化菜单句柄所在的窗口, 接收WM_DRAWITEM消息, 如果已经子类化, 则不处理
    virtual LPOBJSTRUCT _subclass_hMenu_Window(HWND hWnd, pfn_SubClassWndProc proc) = 0;








    virtual LPOBJSTRUCT _subclass_EWindow(HWND hWnd, HINSTANCE hInst, LPSYSBUTTONIMAGE pNskn) = 0;


    // 子类化易语言代码中间的那个自绘选择夹
    virtual LPOBJSTRUCT _subclass_CustomTab(HWND hWnd) = 0;
    // 初始化工作夹的选择夹, 修改选择夹位置, 设置选择框索引
    // hTab = 被初始化的选择夹, hTabOld = 易语言自带的选择夹, isMove = 为真则只移动窗口, 忽略第二个参数
    virtual void _Tab_Work_init(HWND hTab, bool isMove) = 0;
    // 初始化状态夹的选择夹, 修改选择夹位置, 设置选择框索引
    // hTab = 被初始化的选择夹, hTabOld = 易语言自带的选择夹, isMove = 为真则只移动窗口, 忽略第二个参数
    virtual void _Tab_State_init(HWND hTab, bool isMove) = 0;

    // hook消息即将到达窗口
    virtual void EWindow_Hook_SetWindowsHook() = 0;




    // 创建画刷, 返回的画刷不能释放, 要释放只能在支持库被卸载或者关闭易语言时调用本对象的 Release() 释放
    // 相同颜色不会创建多次, 内部使用 map 记录所有创建的画刷
    virtual HBRUSH Brush_Create(COLORREF cr) = 0;



    virtual LPOBJSTRUCT _subclass_tool(HWND hWnd, pfn_SubClassWndProc proc, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle) = 0;


    // 更新当前激活的窗口, 会重画激活的这个窗口和之前激活的窗口
    virtual void TopState_UpdateStateWindow(HWND hWnd) = 0;
    // 获取当前激活的窗口
    virtual HWND TopState_GetActivateWindow() = 0;

    virtual void TopState_SetWindowText(HWND hWnd, LPCWSTR lpszText, bool isUpdate) = 0;



    // 加载易语言的菜单, id必须是易语言的菜单id
    //127,    // 顶部菜单栏的菜单, 一大堆顶级菜单+子级菜单
    //235,    // 其他地方右键菜单, 各种对齐工具条, 顶边菜单  查看->工具条
    //236,    // 代码区 右键菜单
    //237,    // 支持库 -> 树型框 -> 右键菜单
    //238,    // 程序 -> 树型框 -> 模块引用表 右键菜单, 除了模块/外部文件记录表, 其他地方右键菜单也是238, 不过父菜单项不同
    //239,    // 程序 -> 树型框 -> 程序数据 右键菜单
    //240,    // 窗口设计器右键菜单
    //276,    // 运行时 代码区 右键菜单
    //293,    // 左边易语言属性框 -> 属性栏右键菜单
    //587,    // 底边状态夹 -> 监视表右键
    //606,    // 代码切换选择夹右键菜单
    //609,    // 书签右键菜单
    //610,    // 常量右键菜单
    //611,    // 底边状态夹 -> 剪辑历史
    //626,    // 程序 -> 分类文件夹右键菜单
    //627,    // 程序 -> 树型框 -> 外部文件记录表 右键菜单
    //1,      // 编辑框右键菜单
    //12,     // 编辑框右键菜单 多了个打开/关闭输入法
    virtual HMENU EMenu_LoadMenu(int id) = 0;

    // 弹出菜单
    virtual BOOL EMenu_PopUpMenu(HWND hWnd, HMENU hMenu, int x = 0, int y = 0) = 0;

public:
    // 处理顶部菜单栏里的控件, 去除组件边框, 子类化各个组件
    virtual void _subclass_menu() = 0;
    virtual void _subclass_work() = 0;
    // 处理状态夹里的控件, 去除组件边框, 子类化各个组件
    virtual void _subclass_state() = 0;
    // 处理底部状态条, 去除组件边框, 子类化各个组件
    virtual void _subclass_statusBar() = 0;
    // 处理中间代码区的控件, 去除组件边框, 子类化各个组件
    virtual void _subclass_MDIControl() = 0;
    // 处理组件箱, 去除组件边框, 子类化各个组件
    virtual void _subclass_control() = 0;



public:
    
    // 初始化绘画信息, 会调用 BeginPaint, 会创建内存DC, 返回内存DC, DX渲染返回0
    // 把组件设置的字体颜色, 背景画刷, 边框画笔都选入DC
    // 如果 isFillBack 为true, 则内部会使用背景画刷填充内存DC
    virtual HDC BeginPaint(LPOBJSTRUCT pData, BEGIN_END_PAINT_STRUCT& data, bool isDxRender = false, bool isFillBack = true) = 0;


    // 结束绘画, 会调用 EndPaint, 会根据dx渲染调用dx函数
    virtual bool EndPaint(BEGIN_END_PAINT_STRUCT& data) = 0;

public:




};




// 资源模块
class IEWindowResource
{
public:
    // 获取窗口主题数据
    virtual LPBYTE GetELogo(DWORD* size) = 0;

    // 获取默认主题配色和clr文件, 返回文件数据, 长度+数据, 目前一共有6个文件
    virtual LPBYTE GetDefThemeData(DWORD* size);

    // 获取窗口主题数据
    virtual LPSYSBUTTONIMAGE GetNskn() = 0;

    // 获取工具条图标, pIconType = 是否是从默认值里得到的图标, 仅在调用成功时赋值
    // pIconType = 接收获取到的图标来源, 0=用户图标, 1=用户默认, 2=默认皮肤里图标, 3=默认里的默认图标
    virtual LPWINDOW_ICON_DATA Tool_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96) = 0;

    // 获取支持库树型框图标, pIconType = 是否是从默认值里得到的图标, 仅在调用成功时赋值
    // pIconType = 接收获取到的图标来源, 0=用户图标, 1=用户默认, 2=默认皮肤里图标, 3=默认里的默认图标
    virtual LPWINDOW_ICON_DATA TreeFne_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96) = 0;

    // 获取程序树型框图标, 返回图标数据, pIconType = 是否是从默认值里得到的图标, 仅在调用成功时赋值
    // pIconType = 接收获取到的图标来源, 0=用户图标, 1=用户默认, 2=默认皮肤里图标, 3=默认里的默认图标
    virtual LPWINDOW_ICON_DATA TreeProgram_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96) = 0;

    // 获取组件箱图标, pIconType = 是否是从默认值里得到的图标, 仅在调用成功时赋值
    // pIconType = 接收获取到的图标来源, 0=用户图标, 1=用户默认, 2=默认皮肤里图标, 3=默认里的默认图标
    virtual LPWINDOW_ICON_DATA TreeControl_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96) = 0;

    // 获取树型框加减号图标, pIconType = 是否是从默认值里得到的图标, 仅在调用成功时赋值
    // pIconType = 接收获取到的图标来源, 0=用户图标, 1=用户默认, 2=默认皮肤里图标, 3=默认里的默认图标
    virtual LPWINDOW_ICON_DATA TreeExpand_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96) = 0;

    // 获取组合框图标, pIconType = 是否是从默认值里得到的图标, 仅在调用成功时赋值
    // pIconType = 接收获取到的图标来源, 0=用户图标, 1=用户默认, 2=默认皮肤里图标, 3=默认里的默认图标
    virtual LPWINDOW_ICON_DATA Combobox_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96) = 0;

    // 获取菜单项的图标, pIconType = 是否是从默认值里得到的图标, 仅在调用成功时赋值
    // pIconType = 接收获取到的图标来源, 0=用户图标, 1=用户默认, 2=默认皮肤里图标, 3=默认里的默认图标
    virtual LPWINDOW_ICON_DATA Menu_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96) = 0;

    // 获取左右调节器的图标, pIconType = 是否是从默认值里得到的图标, 仅在调用成功时赋值
    // pIconType = 接收获取到的图标来源, 0=用户图标, 1=用户默认, 2=默认皮肤里图标, 3=默认里的默认图标
    virtual LPWINDOW_ICON_DATA MsctlsUpdowm32_GetIcon_lr(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96) = 0;

    // 获取上下调节器的图标, pIconType = 是否是从默认值里得到的图标, 仅在调用成功时赋值
    // pIconType = 接收获取到的图标来源, 0=用户图标, 1=用户默认, 2=默认皮肤里图标, 3=默认里的默认图标
    virtual LPWINDOW_ICON_DATA MsctlsUpdowm32_GetIcon_tb(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96) = 0;

    // 获取顶部状态条的按钮图标, pIconType = 是否是从默认值里得到的图标, 仅在调用成功时赋值
    // pIconType = 接收获取到的图标来源, 0=用户图标, 1=用户默认, 2=默认皮肤里图标, 3=默认里的默认图标
    virtual LPWINDOW_ICON_DATA TopState_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96) = 0;
    
    // 从常用图标里获取图标, 返回图标数据, 没有这个图标返回0
    virtual LPWINDOW_ICON_DATA GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96) = 0;


    // 释放资源
    virtual void EWindow_Resource_Free() = 0;

    // 从文件加载资源, 返回资源句柄, 加载会影响效率, 一个ext文件加载一次就行, 销毁时会自动销毁
    virtual IDRAW_HRESOURCE LoadFromFile(LPCWSTR szFile) = 0;
    // 从内存加载资源, 返回资源句柄, 加载会影响效率, 一个ext文件加载一次就行, 销毁时会自动销毁
    virtual IDRAW_HRESOURCE LoadFromMemory(const void* pData, int size) = 0;

    // 从用户加载的资源里获取指定ID的图标, 返回id指向的数据
    virtual LPWINDOW_ICON_DATA User_GetIcon(IDRAW_HRESOURCE hResource, LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96) = 0;


    // 取消插件所有绑定窗口资源的回调
    virtual bool UnBindWindow(LPCSTR guid) = 0;

    // 获取默认组件的资源句柄, 传递窗口句柄获取
    // hWnd = 要获取资源的窗口句柄, 获取菜单使用 HRESOURCE_MENU, 获取树型框展开收缩按钮使用 HRESOURCE_TREEEXPAND
    virtual IDRAW_HRESOURCE GetDefControlIcon(HWND hWnd) = 0;

    // 将资源绑定到窗口上, 下次重画这个窗口时会从这个资源获取图标
    // hWnd = 要绑定的窗口句柄, 绑定菜单使用 HRESOURCE_MENU, 绑定树型框展开收缩按钮使用 HRESOURCE_TREEEXPAND
    // hResource = 加载的资源句柄, 如果这个参数为0, 且最后一个参数为0, 则不会加载设定的图标
    // callback = 回调函数, 当这个窗口需要加载图标时调用, 为0则使用默认的资源加载, 如果没有默认的资源则不加载
    //              回调函数里需要根据是否为dx渲染来写入 prc 和返回值
    virtual bool BindWindow(LPCSTR guid, HWND hWnd, IDRAW_HRESOURCE hResource, pfn_GetWindowIcon callback) = 0;

    // 从窗口句柄获取回调函数, 可以调用回调函数来获取图标, 返回写入缓冲区的数量, bufCallback 为0则返回需要的缓冲区成员数
    // hWnd = 从哪个窗口上获取回调信息
    // bufCallback = 接收绑定时传递的回调函数数组, 如果为0则返回绑定的数量
    // bufCount = 表示 bufCallback 这个缓冲区有多少个成员
    virtual int GetResourceCallbackFromHwnd(HWND hWnd, BINDWINDOW_INFO* bufCallback = 0, int bufCount = 0) = 0;

    // 从窗口句柄里获取图标, 如果在绑定的时候给了回调函数, 则会调用回调函数处理
    // 内部调用 GetResourceFromHwnd() + User_GetIcon()/User_GetIconDx() 来实现
    // hWnd = 绑定的窗口句柄, 如果没有绑定则返回0
    // pszID = 要取的图标ID, 这里一般是项目文本
    // arg = 这个根据不同组件传递不同结构, 树型框传递 TVITEMEXW结构指针, 
    // prc = 接收图标在图片里的位置
    // state = 图片状态, STATE 枚举常量里的值, 目前只支持 STATE::HOT, STATE::DOWN, STATE::BAN, STATE::NORMAL 这4个
    // pIconType = 接收图标来源, 0=从ID中获取, 1=从默认图标里获取
    virtual LPWINDOW_ICON_DATA GetIconFromHwnd(HWND hWnd, LPCWSTR pszID, void* arg, RECT_F* prc, STATE state, UINT dpi = 96) = 0;

    // 从gdi+位图或者dx位图上生成资源返回值, 所有指针必须保证存活, 内部不会申请内存来保存传递进来的文本和位图
    // 返回值不使用时必须调用 FreeResourceReturn 来释放
    // pImg = 图片数据地址
    // pImgSize = 图片数据大小
    // text = 绘制当前项目显示的文本, 不会修改原组件文本, 只是绘画时显示这个文本, 可以为0
    // remarks = 备注文本, 可以为0
    // crText = 文本ARGB颜色, 可以为0, 不为0则必须是4个成员, 分别是 正常, 热点, 按下, 禁止
    // crBack = 背景ARGB颜色, 可以为0, 不为0则必须是4个成员, 分别是 正常, 热点, 按下, 禁止
    virtual LPWINDOW_ICON_DATA MakeResourceReturn(const void* pImg, int pImgSize, LPCWSTR text, LPCWSTR remarks, DWORD* crText, DWORD* crBack) = 0;

    // 释放 MakeResourceReturn() 生成的返回值
    virtual bool FreeResourceReturn(LPWINDOW_ICON_DATA pData) = 0;

    // 从内存加载gdi+图片
    // pData = 图片数据
    // size = 图片数据大小
    virtual Gdiplus::Bitmap* LoadGpImageFromMemory(const void* pData, int size) = 0;

    // 释放从 LoadGpImageFromMemory() 返回的值
    virtual bool FreeGpImage(Gdiplus::Bitmap*) = 0;

    // 获取marlett字体, 图标字体, 绘画三角用
    virtual HFONT GetMarlettFont(int size) = 0;

    // 获取字体数据
    virtual LPBYTE GetMarlettData(int* size) = 0;
};



DECLARE_HANDLE(ICTinyXml2_XMLPrinter);
DECLARE_HANDLE(ICTinyXml2_XMLNode);
DECLARE_HANDLE(ICTinyXml2_XMLElement);

class ICTinyXml2
{
public:

    // 将unicode字符串转换成当前对象需要的文本格式
    virtual LPCWSTR ToPointer(const wchar_t* pstr) = 0;
    // 将ansi字符串转换成当前对象需要的文本格式
    virtual LPCWSTR ToPointer(const char* pstr) = 0;
public:
    // 清空当前xml, 析构时会自动触发
    virtual void Clear() = 0;
    virtual ICTinyXml2_XMLElement NewElement(LPCSTR name) = 0;
    virtual ICTinyXml2_XMLNode InsertEndChild(ICTinyXml2_XMLNode node, ICTinyXml2_XMLElement child) = 0;
    virtual ICTinyXml2_XMLNode FirstChildElement(ICTinyXml2_XMLNode node, LPCSTR name) = 0;
    virtual void SetText(ICTinyXml2_XMLElement node, LPCSTR text) = 0;
    virtual void SetAttribute(ICTinyXml2_XMLElement node, LPCSTR name, LPCSTR value) = 0;
    virtual ICTinyXml2_XMLElement ToElement(ICTinyXml2_XMLNode node) = 0;


    // 创建xml, 会清空当前已有xml数据
    // pszRootName = 根节点名, 为0则创建xml空对象
    virtual bool Create(LPCWSTR pszRootName) = 0;

    // 设置xml头
    virtual bool SetDeclaration(LPCWSTR pszText) = 0;

    // 解析xml数据, 会清空当前已有xml数据
    virtual bool Parse(LPCWSTR p) = 0;

    // 解析xml数据, 会清空当前已有xml数据
    virtual void Print(ICTinyXml2_XMLPrinter streamer = 0) = 0;

    // 从文件加载xml, 会清空当前已有xml数据
    virtual bool LoadFile(LPCWSTR filename) = 0;

    // 从已经打开的文件加载xml, 会清空当前已有xml数据
    virtual bool LoadFile(FILE* fp) = 0;

    // 保存当前xml到磁盘, 如果内部记录的文件为空时会保存当前文件路径
    // filename = 保存的路径, 如果为空, 则使用内部记录的路径, 如果内部没有记录路径则调用失败
    // compact = 不格式化文本, 默认为false, 默认会格式化文本
    virtual bool SaveFile(LPCWSTR filename = 0, bool compact = false) = 0;

    // 保存当前xml数据到打开的文件中
    virtual bool SaveFile(FILE* fp, bool compact = false) = 0;

    // 设置保存时使用的目录
    virtual void SetSaveFile(LPCWSTR file) = 0;

    // 获取内部记录的文件路径
    virtual LPCWSTR GetSaveFile() = 0;

public:

    // 取根节点名, 如果未创建则返回空
    virtual LPCWSTR GetRootName() const = 0;

    // 取子节点数, 取得参数节点的所有子节点的个数
    // node = 节点指针, 读取这个节点下的子节点数量
    virtual int GetChildNodeCount(ICTinyXml2_XMLNode node) const = 0;

    // 取子节点数, 取得参数节点的所有子节点的个数
    // pszPath = 路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    virtual int GetChildNodeCount(LPCWSTR pszPath = 0) const = 0;
    // 取节点名, 取得参数节点的名字, <节点名 属性名="属性值"/>
    // node = 节点指针, 读取这个节点的节点名
    virtual LPCWSTR GetNodeName(ICTinyXml2_XMLNode node) const = 0;
    // 取节点名, 取得参数节点的名字, <节点名 属性名="属性值"/>
    // pszPath = 路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    virtual LPCWSTR GetNodeName(LPCWSTR pszPath = 0) const = 0;
    // 取节点值, 取得参数节点的值, <节点名 属性名="属性值"/>
    // node = 节点指针, 读取这个节点的节点值
    virtual LPCWSTR GetNodeValue(ICTinyXml2_XMLNode node) const = 0;
    // 取节点值, 取得参数节点的值, <节点名 属性名="属性值"/>
    // pszPath = 路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    virtual LPCWSTR GetNodeValue(LPCWSTR pszPath = 0) const = 0;

    // 枚举指定路径节点的所有属性, 返回属性数量, <节点名 属性名="属性值"/>
    // pszPath = 路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    // pNames = 接收属性名数组, 留空则不获取
    // pValues = 属性名对应的值, 留空则不获取
    virtual int EnumAttribute(LPCWSTR pszPath = 0, std::vector<LPCWSTR>* pNames = 0, std::vector<LPCWSTR>* pValues = 0) const = 0;
    // 枚举指定路径节点的所有属性, 返回属性数量, <节点名 属性名="属性值"/>
    // node = 节点指针, 属性值从这个节点获取
    // pNames = 接收属性名数组, 留空则不获取
    // pValues = 属性名对应的值, 留空则不获取
    virtual int EnumAttribute(ICTinyXml2_XMLNode node, std::vector<LPCWSTR>* pNames = 0, std::vector<LPCWSTR>* pValues = 0) const = 0;
    // 取所有属性名, 返回属性数量, <节点名 属性名="属性值"/>
    // pszPath = 路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    // pNames = 接收属性名数组, 留空则不获取
    // pValues = 接收属性值数组, 留空则不获取
    virtual int GetAttrName(LPCWSTR pszPath = 0, std::vector<LPCWSTR>* pNames = 0, std::vector<LPCWSTR>* pValues = 0) const = 0;

    // 取所有属性名, 返回属性数量, <节点名 属性名="属性值"/>
    // node = 节点指针, 属性值从这个节点获取
    // pNames = 接收属性名数组, 留空则不获取
    // pValues = 接收属性值数组, 留空则不获取
    virtual int GetAttrName(ICTinyXml2_XMLNode node, std::vector<LPCWSTR>* pNames = 0, std::vector<LPCWSTR>* pValues = 0) const = 0;

    // 取属性值, 取得参数节点的特定属性的值, <节点名 属性名="属性值"/>
    // node = 节点指针, 从这个节点获取属性值
    // pszAttrName = 属性名, 为0则调用失败  <节点名 属性名="属性值"/>
    // isAttribute = 要获取的这个属性是否存在, 可以为0, 如果属性不存在, 这个值为0, 存在则为1
    virtual LPCWSTR GetAttrValue(ICTinyXml2_XMLNode node, LPCWSTR pszAttrName, BOOL* isAttribute = 0) const = 0;
    // 取属性值, 取得参数节点的特定属性的值, <节点名 属性名="属性值"/>
    // pszPath = 路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    // pszAttrName = 属性名, 为0则调用失败  <节点名 属性名="属性值"/>
    // isAttribute = 要获取的这个属性是否存在, 可以为0, 如果属性不存在, 这个值为0, 存在则为1
    virtual LPCWSTR GetAttrValue(LPCWSTR pszPath, LPCWSTR pszAttrName, BOOL* isAttribute = 0) const = 0;
    //// 插入备注文本, 值为0则删除该备注文本
    //// pszPath = 路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    //// pszNodeValue = 节点值  <节点名>节点值</节点名>
    //virtual bool InsertComment(ICTinyXml2_XMLNode node, LPCWSTR pszCommentText = 0) = 0;

    //// 插入备注文本, 值为0则删除该备注文本
    //// pszPath = 路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    //// pszNodeValue = 节点值  <节点名>节点值</节点名>
    //virtual bool InsertComment(LPCWSTR pszPath, LPCWSTR pszCommentText = 0) = 0;

    // 插入子节点, 新建一个节点，并把该参数节点作为该新节点的父节点，然后插入到XML树中
    // pszParentPath = 父节点全路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点, 新加入的节点路径 = @1/@1/@2/@节点数量
    // pszNodeName = 新的节点名, 为0则调用失败  <节点名></节点名>
    // pszNodeValue = 节点值  <节点名>节点值</节点名>
    // isCDATA = 如果本参数为真，则节点值文本将被 "<![CDATA[" 和 "]]>" 包围，这意味着该文本可以包含任意字符(以不影响CDATA解析为限}
    virtual bool InsertChildNode(LPCWSTR pszParentPath, LPCWSTR pszNodeName, LPCWSTR pszNodeValue = 0, bool isCDATA = false, ICTinyXml2_XMLNode* ppNode = 0) = 0;
    // 插入子节点, 新建一个节点，并把该参数节点作为该新节点的父节点，然后插入到XML树中
    // parentNode = 父节点
    // pszNodeName = 新的节点名, 为0则调用失败  <节点名></节点名>
    // pszNodeValue = 节点值  <节点名>节点值</节点名>
    // isCDATA = 如果本参数为真，则节点值文本将被 "<![CDATA[" 和 "]]>" 包围，这意味着该文本可以包含任意字符(以不影响CDATA解析为限}
    virtual bool InsertChildNode(ICTinyXml2_XMLNode parentnode, LPCWSTR pszNodeName, LPCWSTR pszNodeValue = 0, bool isCDATA = false, ICTinyXml2_XMLNode* ppNode = 0) = 0;
    // 插入节点, 插入到路径节点后面
    // pszPath 最后的路径为@0 则加入到第一个节点, @-1 则加入到最后一个节点, 否则加入到 指定路径的下一个节点
    // pszNodeName = 新的节点名, 为0则调用失败  <节点名></节点名>
    // pszNodeValue = 节点值  <节点名>节点值</节点名>
    // isCDATA = 如果本参数为真，则节点值文本将被 "<![CDATA[" 和 "]]>" 包围，这意味着该文本可以包含任意字符(以不影响CDATA解析为限}
    virtual bool InsertNode(LPCWSTR pszPath, LPCWSTR pszNodeName, LPCWSTR pszNodeValue = 0, bool isCDATA = false, ICTinyXml2_XMLNode* ppNode = 0) = 0;

    // 插入属性, 在参数节点中插入一个新的属性，并给属性赋值
    // node = 节点指针, 从这个节点加入属性
    // pszAttrName = 新属性名, 为0则调用失败  <节点名 属性名="属性值"/>
    // pszAttrValue = 属性值  <节点名 属性名="属性值"/>
    // isAttribute = 要获取的这个属性是否存在, 可以为0, 如果属性不存在, 这个值为0, 存在则为1
    virtual bool InsertAttr(ICTinyXml2_XMLNode node, LPCWSTR pszAttrName, LPCWSTR pszAttrValue, BOOL* isAttribute = 0) = 0;
    // 插入属性, 在参数节点中插入一个新的属性，并给属性赋值
    // pszPath = 节点全路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    // pszAttrName = 新属性名, 为0则调用失败  <节点名 属性名="属性值"/>
    // pszAttrValue = 属性值  <节点名 属性名="属性值"/>
    // isAttribute = 要获取的这个属性是否存在, 可以为0, 如果属性不存在, 这个值为0, 存在则为1
    virtual bool InsertAttr(LPCWSTR pszPath, LPCWSTR pszAttrName, LPCWSTR pszAttrValue, BOOL* isAttribute = 0) = 0;

    // 删除节点, 删除给定节点和他的所有子节点
    // node = 节点指针, 删除这个节点
    virtual bool DelNode(ICTinyXml2_XMLNode node) = 0;
    // 删除节点, 删除给定节点和他的所有子节点
    // pszPath = 节点全路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    virtual bool DelNode(LPCWSTR pszPath) = 0;

    // 删除属性
    // node = 节点指针, 从这个节点删除属性
    // pszAttrName = 属性名, 为0则调用失败  <节点名 属性名="属性值"/>
    virtual bool DelAttr(ICTinyXml2_XMLNode node, LPCWSTR pszAttrName) = 0;
    // 删除属性
    // pszPath = 节点全路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    // pszAttrName = 属性名, 为0则调用失败  <节点名 属性名="属性值"/>
    virtual bool DelAttr(LPCWSTR pszPath, LPCWSTR pszAttrName) = 0;

    // 修改节点名, 修改指定节点的名字
    // node = 节点指针, 从这个节点设置节点名
    // pszNodeName = 新节点名, 为0则调用失败  <节点名 属性名="属性值"/>
    virtual bool SetNodeName(ICTinyXml2_XMLNode node, LPCWSTR pszNodeName) = 0;
    // 修改节点名, 修改指定节点的名字
    // pszPath = 节点全路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    // pszNodeName = 新节点名, 为0则调用失败  <节点名 属性名="属性值"/>
    virtual bool SetNodeName(LPCWSTR pszPath, LPCWSTR pszNodeName) = 0;

    // 修改节点值, 修改指定节点的值
    // node = 节点指针, 从这个节点设置节点值
    // pszNodeValue = 新节点值   <节点名 属性名="属性值">节点值</节点名>
    virtual bool SetNodeValue(ICTinyXml2_XMLNode node, LPCWSTR pszNodeValue) = 0;
    // 修改节点值, 修改指定节点的值
    // pszPath = 节点全路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    // pszNodeValue = 新节点值   <节点名 属性名="属性值">节点值</节点名>
    virtual bool SetNodeValue(LPCWSTR pszPath, LPCWSTR pszNodeValue) = 0;

    //// 修改属性名, 修改指定节点的属性名称
    //// pszPath = 节点全路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    //// pszOldAttrName = 旧属性名, 为0则调用失败  <节点名 属性名="属性值"/> 
    //// pszNewAttrName = 新属性名, 为0则调用失败  <节点名 属性名="属性值"/>
    //virtual bool SetAttrName(LPCWSTR pszPath, LPCWSTR pszOldAttrName, LPCWSTR pszNewAttrName)
    //{
    //    ICTinyXml2_XMLNode node;
    //    if (!GetNode(pszPath, node)) return false;
    //    const tinyxml2::XMLAttribute* attr;
    //    tinyxml2::XMLElement* ele;
    //    if (!GetAttr(node, ele, pszOldAttrName, attr))return false;
    //    //attr->SetName(pszNewAttrName);
    //    return true;
    //}


    // 修改属性值, 修改指定节点的属性值
    // node = 节点指针, 从这个节点设置属性值
    // pszAttrName = 旧属性值, 为0则调用失败  <节点名 属性名="属性值"/> 
    // pszAttrValue = 新属性值  <节点名 属性名="属性值"/>
    // isAttribute = 接收这个属性是否存在
    virtual bool SetAttrValue(ICTinyXml2_XMLNode node, LPCWSTR pszAttrName, LPCWSTR pszNewAttrValue, BOOL* isAttribute = 0) = 0;
    // 修改属性值, 修改指定节点的属性值
    // pszPath = 节点全路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    // pszAttrName = 旧属性值, 为0则调用失败  <节点名 属性名="属性值"/> 
    // pszAttrValue = 新属性值  <节点名 属性名="属性值"/>
    // isAttribute = 接收这个属性是否存在
    virtual bool SetAttrValue(LPCWSTR pszPath, LPCWSTR pszAttrName, LPCWSTR pszAttrValue, BOOL* isAttribute = 0) = 0;

    // 获取xml数据, 返回xml文本
    virtual LPCWSTR GetXmlData() = 0;

public:
    // 测试两个节点是否相同, 单不测试子节点, 2个节点不必位于同一Document中
    // 注意：如果在XMLDocument上调用，则将返回false。.
    virtual bool Compare(const ICTinyXml2_XMLNode compare) const = 0;
public:

    // pszPath = 节点全路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    virtual bool GetNode(LPCWSTR pszPath, ICTinyXml2_XMLNode& node) const = 0;

};

class IConfig  // 配置接口类
{
public:

    virtual BOOL IsRunSave            ( BOOL* pValue = 0 ) = 0;   // 运行/编译后是否保存源码, 取值需要传递一个指针接收结
    virtual BOOL IsDxRender           ( BOOL* pValue = 0 ) = 0;   // 是否DX渲染, 取值需要传递一个指针接收结果
    virtual BOOL IsDPIAware           ( BOOL* pValue = 0 ) = 0;   // 是否开启DPI感知
    virtual BOOL IsDPIScaleWnd        ( BOOL* pValue = 0 ) = 0;   // DPI改变时是否缩放窗口
    virtual BOOL IsShowCodeTabClose   ( BOOL* pValue = 0 ) = 0;   // 显示代码选择夹的关闭按钮
    virtual BOOL IsShowLocationTool   ( BOOL* pValue = 0 ) = 0;   // 是否显示定位工具条
    virtual int  programTreeDblclk    ( int * pValue = 0 ) = 0;   // 程序树型框双击模式, 0=只切换, 1=只展开, 2=展开+切换
    virtual int  wheelPos             ( int * pValue = 0 ) = 0;   // 鼠标滚动一次的距离
    virtual BOOL noClipChildren       ( BOOL* pValue = 0 ) = 0;   // 不排除子窗口区域
    virtual int  MenuItemHeight       ( int * pValue = 0 ) = 0;   // 菜单项高度
    virtual int  MenuSeparatorHeight  ( int * pValue = 0 ) = 0;   // 分隔菜单项高度
    virtual int  noChangeTab_work     ( int * pValue = 0 ) = 0;   // 不自动切换工作夹
    virtual int  noChangeTab_state    ( int * pValue = 0 ) = 0;   // 不自动切换状态夹
    virtual int  scroll_width         ( int * pValue = 0 ) = 0;   // 滚动条宽度
    virtual int  scroll_btn_size      ( int * pValue = 0 ) = 0;   // 滚动条按钮尺寸

    virtual BOOL tree_fne_ishot       ( BOOL* pValue = 0 ) = 0;   // 支持库树型框是否支持热点
    virtual BOOL tree_program_ishot   ( BOOL* pValue = 0 ) = 0;   // 程序树型框是否支持热点
    virtual BOOL tree_control_ishot   ( BOOL* pValue = 0 ) = 0;   // 组件箱树型框是否支持热点
    virtual BOOL tree_var_ishot       ( BOOL* pValue = 0 ) = 0;   // 变量表树型框是否支持热点
    virtual BOOL tree_call_ishot      ( BOOL* pValue = 0 ) = 0;   // 调用表树型框是否支持热点
    virtual BOOL list_find1_ishot     ( BOOL* pValue = 0 ) = 0;   // 搜寻1列表框是否支持热点
    virtual BOOL list_find2_ishot     ( BOOL* pValue = 0 ) = 0;   // 搜寻1列表框树型框是否支持热点

    virtual int  tree_fne_height      ( int * pValue = 0 ) = 0;   // 支持库树型框项目高度
    virtual int  tree_program_height  ( int * pValue = 0 ) = 0;   // 程序树型框项目高度
    virtual int  tree_control_height  ( int * pValue = 0 ) = 0;   // 组件箱树型框项目高度
    virtual int  tree_control_orgIcon ( int * pValue = 0 ) = 0;   // 组件箱树型框显示原图标
    virtual int  tree_var_height      ( int * pValue = 0 ) = 0;   // 变量表树型框项目高度
    virtual int  tree_call_height     ( int * pValue = 0 ) = 0;   // 调用表树型框项目高度
    virtual int  list_find1_height    ( int * pValue = 0 ) = 0;   // 搜寻1列表框项目高度
    virtual int  list_find2_height    ( int * pValue = 0 ) = 0;   // 搜寻2列表框项目高度

    virtual int  tab_work_direction   ( int * pValue = 0 ) = 0;   // 工作夹方向, 0=左边, 1=顶边, 2=右边, 3=底边
    virtual BOOL tab_work_isIcon      ( BOOL* pValue = 0 ) = 0;   // 工作夹是否显示图标
    virtual int  tab_work_flags       ( int * pValue = 0 ) = 0;   // 工作夹标识, 记录位置, 是否最大化, 是否浮动, 是否停靠等信息 TOP_STATE_BUTTON 枚举常量里的值, 调用 __query_flags 判断值

    virtual int  tab_state_direction  ( int * pValue = 0 ) = 0;   // 状态夹方向, 0=左边, 1=顶边, 2=右边, 3=底边
    virtual BOOL tab_state_isIcon     ( BOOL* pValue = 0 ) = 0;   // 状态夹是否显示图标
    virtual int  tab_state_flags      ( int * pValue = 0 ) = 0;   // 状态夹标识, 记录位置, 是否最大化, 是否浮动, 是否停靠等信息 TOP_STATE_BUTTON 枚举常量里的值, 调用 __query_flags 判断值

    virtual int  tab_control_flags    ( int * pValue = 0 ) = 0;   // 组件箱标识, 记录位置, 是否最大化, 是否浮动, 是否停靠等信息 TOP_STATE_BUTTON 枚举常量里的值, 调用 __query_flags 判断值

    virtual int  tab_code_direction   ( int * pValue = 0 ) = 0;   // 代码切换选择夹方向, 3=底边, 非3=顶边
    virtual BOOL tab_code_isIcon      ( BOOL* pValue = 0 ) = 0;   // 代码切换选择夹是否显示图标

    virtual RECT rcWork               ( RECT* pValue = 0 ) = 0;   // 工作夹窗口位置, 其他组件根据这个矩形计算位置
    virtual RECT rcState              ( RECT* pValue = 0 ) = 0;   // 状态夹窗口位置, 其他组件根据这个矩形计算位置
    virtual RECT rcControl            ( RECT* pValue = 0 ) = 0;   // 组件箱窗口位置, 其他组件根据这个矩形计算位置
    virtual RECT rcMenu               ( RECT* pValue = 0 ) = 0;   // 菜单夹窗口位置, 其他组件根据这个矩形计算位置
    virtual RECT rcMDIClient          ( RECT* pValue = 0 ) = 0;   // MDIClient窗口位置, 其他组件根据这个矩形计算位置

    virtual RECT rcWork1              ( RECT* pValue = 0 ) = 0;   // 工作夹窗口位置, 显示到屏幕的位置
    virtual RECT rcState1             ( RECT* pValue = 0 ) = 0;   // 状态夹窗口位置, 显示到屏幕的位置
    virtual RECT rcControl1           ( RECT* pValue = 0 ) = 0;   // 组件箱窗口位置, 显示到屏幕的位置


};

class IThemeColor
{

public:

    virtual COLORREF crBack                  ( COLORREF* pValue = 0 ) = 0;  // [Basics], 背景颜色
    virtual COLORREF crBackExtern            ( COLORREF* pValue = 0 ) = 0;  // [Basics], 外部的背景颜色, 比如选择夹的子夹
    virtual COLORREF crStateBar              ( COLORREF* pValue = 0 ) = 0;  // [Basics], 组件箱/工作夹/状态夹顶部激活的颜色

    virtual COLORREF crBorder                ( COLORREF* pValue = 0 ) = 0;  // [Basics], 边框颜色
    virtual COLORREF crBorder_run            ( COLORREF* pValue = 0 ) = 0;  // [Basics], 运行时边框颜色
    virtual COLORREF crBorder_nf             ( COLORREF* pValue = 0 ) = 0;  // [Basics], 取消激活时的边框颜色

    virtual COLORREF crShadow                ( COLORREF* pValue = 0 ) = 0;  // [Basics], 阴影颜色
    virtual COLORREF crShadow_run            ( COLORREF* pValue = 0 ) = 0;  // [Basics], 运行时阴影颜色
    virtual COLORREF crShadow_nf             ( COLORREF* pValue = 0 ) = 0;  // [Basics], 取消激活时的边框颜色

    virtual COLORREF crMenuBorder            ( COLORREF* pValue = 0 ) = 0;  // [Basics], 菜单边框颜色
    virtual COLORREF crMenuBack              ( COLORREF* pValue = 0 ) = 0;  // [Basics], 菜单背景颜色, 弹出菜单使用的背景色, 菜单条按下时使用的背景色
    virtual COLORREF crMenuHot               ( COLORREF* pValue = 0 ) = 0;  // [Basics], 菜单热点项颜色
    virtual COLORREF crMenuBan               ( COLORREF* pValue = 0 ) = 0;  // [Basics], 菜单禁止颜色
    virtual COLORREF crMenuText              ( COLORREF* pValue = 0 ) = 0;  // [Basics], 菜单文本颜色
    virtual COLORREF crMenuTextHot           ( COLORREF* pValue = 0 ) = 0;  // [Basics], 菜单文本热点颜色
    virtual COLORREF crMenuTextBan           ( COLORREF* pValue = 0 ) = 0;  // [Basics], 菜单文本禁止颜色

    virtual COLORREF crMenuBarHot            ( COLORREF* pValue = 0 ) = 0;  // [Basics], 顶部菜单条热点项颜色
    virtual COLORREF crMenuBarToolsText      ( COLORREF* pValue = 0 ) = 0;  // [Basics], 顶部菜单条助手的文本颜色
    virtual COLORREF crMenuBarToolsTextHot   ( COLORREF* pValue = 0 ) = 0;  // [Basics], 顶部菜单条助手的文本热点颜色

    virtual COLORREF crTitleback             ( COLORREF* pValue = 0 ) = 0;  // [Basics], 中间黑块标题的背景颜色
    virtual COLORREF crTitle_left_act        ( COLORREF* pValue = 0 ) = 0;  // [Basics], 中间黑块标题左边的颜色, 这个是源码名或者项目名, 激活时的文本颜色
    virtual COLORREF crTitle_right_act       ( COLORREF* pValue = 0 ) = 0;  // [Basics], 中间黑块标题右边的颜色, 这个是程序类型, 或者未保存时的星号使用的颜色, 激活时的文本颜色
    virtual COLORREF crTitle_left            ( COLORREF* pValue = 0 ) = 0;  // [Basics], 中间黑块标题左边的颜色, 这个是源码名或者项目名
    virtual COLORREF crTitle_right           ( COLORREF* pValue = 0 ) = 0;  // [Basics], 中间黑块标题右边的颜色, 这个是程序类型, 或者未保存时的星号使用的颜色

    virtual COLORREF crText                  ( COLORREF* pValue = 0 ) = 0;  // [Basics], 普通文本绘画用的颜色
    virtual COLORREF crTextStatusBar         ( COLORREF* pValue = 0 ) = 0;  // [Basics], 底部状态条的文本颜色
    virtual COLORREF crTextStatusBar_run     ( COLORREF* pValue = 0 ) = 0;  // [Basics], 底部状态条运行时的文本颜色
    virtual COLORREF crTextStatusBar_nf      ( COLORREF* pValue = 0 ) = 0;  // [Basics], 底部状态条不激活的文本颜色

    virtual COLORREF crScrollBtn1            ( COLORREF* pValue = 0 ) = 0;  // [Basics], 滚动条按钮1正常颜色
    virtual COLORREF crScrollBtnHot1         ( COLORREF* pValue = 0 ) = 0;  // [Basics], 滚动条按钮1热点颜色
    virtual COLORREF crScrollBtnDown1        ( COLORREF* pValue = 0 ) = 0;  // [Basics], 滚动条按钮1按下颜色
    virtual COLORREF crScrollBtn2            ( COLORREF* pValue = 0 ) = 0;  // [Basics], 滚动条按钮2正常颜色
    virtual COLORREF crScrollBtnHot2         ( COLORREF* pValue = 0 ) = 0;  // [Basics], 滚动条按钮2热点颜色
    virtual COLORREF crScrollBtnDown2        ( COLORREF* pValue = 0 ) = 0;  // [Basics], 滚动条按钮2按下颜色
    virtual COLORREF crScrollThumb           ( COLORREF* pValue = 0 ) = 0;  // [Basics], 滚动条滑块颜色
    virtual COLORREF crScrollThumbHot        ( COLORREF* pValue = 0 ) = 0;  // [Basics], 滚动条滑块热点颜色
    virtual COLORREF crScrollThumbDown       ( COLORREF* pValue = 0 ) = 0;  // [Basics], 滚动条滑块按下颜色
    virtual COLORREF crScrollBack            ( COLORREF* pValue = 0 ) = 0;  // [Basics], 滚动条背景颜色

    virtual COLORREF crToolSeplineLeft       ( COLORREF* pValue = 0 ) = 0;  // [Basics], 工具条分割线左边颜色
    virtual COLORREF crToolSeplineRight      ( COLORREF* pValue = 0 ) = 0;  // [Basics], 工具条分割线右边颜色

    virtual COLORREF crStatelineNormal       ( COLORREF* pValue = 0 ) = 0;  // [Basics], 窗口顶部中间线的正常颜色
    virtual COLORREF crStatelineHot          ( COLORREF* pValue = 0 ) = 0;  // [Basics], 窗口顶部中间线的热点颜色

    virtual LPCWSTR  ires_Toolbar            ( LPCWSTR   pValue = 0 ) = 0;  // [iresFile], 工具条资源名
    virtual LPCWSTR  ires_TreeFne            ( LPCWSTR   pValue = 0 ) = 0;  // [iresFile], 支持库树型框资源名
    virtual LPCWSTR  ires_TreeProgram        ( LPCWSTR   pValue = 0 ) = 0;  // [iresFile], 程序树型框资源名
    virtual LPCWSTR  ires_TreeControl        ( LPCWSTR   pValue = 0 ) = 0;  // [iresFile], 组件箱树型框资源名
    virtual LPCWSTR  ires_TreeExpand         ( LPCWSTR   pValue = 0 ) = 0;  // [iresFile], 树型框展开收缩按钮资源名
    virtual LPCWSTR  ires_ComboboxLocation   ( LPCWSTR   pValue = 0 ) = 0;  // [iresFile], 组合框资源名
    virtual LPCWSTR  ires_MenuItem           ( LPCWSTR   pValue = 0 ) = 0;  // [iresFile], 菜单资源名
    virtual LPCWSTR  ires_msctls_updown32_lr ( LPCWSTR   pValue = 0 ) = 0;  // [iresFile], 调节器左右箭头资源名
    virtual LPCWSTR  ires_msctls_updown32_tb ( LPCWSTR   pValue = 0 ) = 0;  // [iresFile], 调节器上下箭头资源名
    virtual LPCWSTR  ires_TopState           ( LPCWSTR   pValue = 0 ) = 0;  // [iresFile], 顶部状态资源名
    virtual LPCWSTR  ires_icon               ( LPCWSTR   pValue = 0 ) = 0;  // [iresFile], 默认资源资源名
    virtual LPCWSTR  clr                     ( LPCWSTR   pValue = 0 ) = 0;  // [clr], 配色文件


    virtual COLORREF close_hot                  ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], 代码切换选择夹关闭按钮热点颜色
    virtual COLORREF close_down                 ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], 代码切换选择夹关闭按钮按下颜色
    virtual COLORREF crTab_text_window          ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], 代码切换选择夹, 窗口文本颜色
    virtual COLORREF crTab_text_assembly        ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], 代码切换选择夹, 程序集文本颜色
    virtual COLORREF crTab_text_assembly_window ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], 代码切换选择夹, 窗口程序集文本颜色
    virtual COLORREF crTab_text_class           ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], 代码切换选择夹, 类文本颜色
    virtual COLORREF crTab_text_globalvar       ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], 代码切换选择夹, 全局变量文本颜色
    virtual COLORREF crTab_text_struct          ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], 代码切换选择夹, 自定义数据类型文本颜色
    virtual COLORREF crTab_text_dll             ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], 代码切换选择夹, DLL文本颜色
    virtual COLORREF crTab_text_const           ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], 代码切换选择夹, 常量文本颜色
    virtual COLORREF crTab_text_image           ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], 代码切换选择夹, 图片文本颜色
    virtual COLORREF crTab_text_sound           ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], 代码切换选择夹, 声音文本颜色
    virtual COLORREF crTab_line_window          ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], 代码切换选择夹, 窗口下边线颜色
    virtual COLORREF crTab_line_assembly        ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], 代码切换选择夹, 程序集下边线颜色
    virtual COLORREF crTab_line_assembly_window ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], 代码切换选择夹, 窗口程序集下边线颜色
    virtual COLORREF crTab_line_class           ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], 代码切换选择夹, 类下边线颜色
    virtual COLORREF crTab_line_globalvar       ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], 代码切换选择夹, 全局变量下边线颜色
    virtual COLORREF crTab_line_struct          ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], 代码切换选择夹, 自定义数据类型下边线颜色
    virtual COLORREF crTab_line_dll             ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], 代码切换选择夹, DLL下边线颜色
    virtual COLORREF crTab_line_const           ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], 代码切换选择夹, 常量下边线颜色
    virtual COLORREF crTab_line_image           ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], 代码切换选择夹, 图片下边线颜色
    virtual COLORREF crTab_line_sound           ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], 代码切换选择夹, 声音下边线颜色

};


typedef bool (WINAPI* pfn_ShadowPaint)(LPCANVAS hCanvas, LPEX_IMAGE pImg, const RECT* prc, COLORREF crShadow, DWORD argbBorder, int alpha, LPVOID param);
class ICShadow
{

public:
    // 创建阴影窗口, 根据 rcWindow 参数调整阴影窗口的大小和位置, 阴影位置是 rcWindow.左边顶边-10, 右边底边+10
    // argbColorShadow = 阴影RGB颜色, 默认是黑色
    virtual HWND create(HINSTANCE hInst, const RECT& rcWindow, HWND hWnd, DWORD crShadow = 0) = 0;

    // 设置阴影颜色, 颜色值是ARGB
    virtual void SetColor(DWORD crShadow, DWORD argbBorder, bool isRedraw = false) = 0;

    // 更新阴影窗口位置, 根据 rcWindow 参数调整阴影窗口的大小和位置, 阴影位置是 rcWindow.左边顶边-10, 右边底边+10
    // bForce = 强制刷新, 为假时窗口尺寸未改变则不会重画, 为true时则必须重画
    virtual void Update(const RECT& rcWindow, bool bForce = false) = 0;

    // 挂接绘画回调函数, 0为取消回调
    virtual void OnPaint(pfn_ShadowPaint pfn, LPVOID param = 0) = 0;

    // 销毁当前类的阴影窗口
    virtual void destroy() = 0;
    virtual bool isCreate() = 0;
    virtual void SetParent(HWND hWnd) = 0;
    virtual void hide() = 0;
    virtual void show() = 0;
    virtual void SetAlpha(int alpha) = 0;
    virtual int GetAlpha() = 0;
    virtual bool move(const RECT& rc) = 0;
    virtual HWND GetWindow() = 0;

};


struct CUSTOMTAB_DATA
{
    HWND hWnd;          // 选项卡对应的窗口句柄
    int type;           // 当前选项卡的类型, CUSTOMTAB_INFO 里的枚举常量, 现在有10种类型
    LPWSTR text;        // 备注文本, 从数组种删除时需要调用 wstr::free() 释放
    COLORREF crText;    // 文本颜色

    LPWSTR  name;       // 当前子夹名字, 删除成员需要释放, 需要通过 s_info->copyStrW() 来申请空间存放字符串, s_info->freeStr() 释放
    int     width;      // 当前子夹宽度
    LPARAM  lParam;     // 项目数值
    int     iImage;     // 图标索引
    int     state;      // 当前子夹状态
    //bool    isClick;    // 是否已经点击过, 如果有点击过就设置为true, 没有的话就
};

// 自定义代码切换选择夹类
class ICustomCodeTab
{
public:
    virtual int size() = 0;
    virtual bool empty() = 0;
    virtual int erase(int index) = 0;
    virtual CUSTOMTAB_DATA& at(int index) = 0;
    virtual CUSTOMTAB_DATA& operator[](int index) = 0;
    virtual int push_back(const CUSTOMTAB_DATA& data) = 0;
    virtual int insert(const CUSTOMTAB_DATA& data, int index) = 0;
    virtual int resize(int count) = 0;
    virtual void clear() = 0;
};

//class iDrawScalc
//{
//public:
//    // 根据当前dpi返回需要使用的字体
//    virtual LPEX_FONT GetFont(LPEX_FONT hFont) = 0;
//    virtual void rescale_rect(RECT& rc) = 0;
//    virtual void _scale_rect(RECT& rc, UINT dpi) = 0;
//};