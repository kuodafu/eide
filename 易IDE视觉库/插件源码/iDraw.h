#pragma once
#include <windows.h>
#include <comdef.h>


#define IDRAW_CALL __stdcall    // 视觉库接口的调用约定
DECLARE_HANDLE(LPOBJSTRUCT);
DECLARE_HANDLE(LPREGISTER_MESSAGE_INFO);
DECLARE_HANDLE(LPREGISTER_EVENT_INFO);
DECLARE_HANDLE(LPIDRAW_INFO);
DECLARE_HANDLE(ICTinyXml2_XMLNode);
DECLARE_HANDLE(IDRAW_HRESOURCE);
DECLARE_HANDLE(LPWINDOW_ICON_DATA);

#define __DEF_TYPE(_s) struct _s; typedef _s* P##_s, *LP##_s
__DEF_TYPE(CANVAS);
__DEF_TYPE(EX_BRUSH);
__DEF_TYPE(EX_PEN);
__DEF_TYPE(EX_IMAGE);
__DEF_TYPE(EX_FONT);
__DEF_TYPE(EX_PATH);
__DEF_TYPE(EX_RGN);

struct EX_IMAGELOCK;


// 接口_IDE代码编辑框管理
class IEIDE_CodeManage;
struct RECT_F;
typedef DWORD ARGB;
typedef void* ID2D1Image;
typedef LRESULT (IDRAW_CALL* pfn_SubClassWndProc)   (HWND, UINT, WPARAM, LPARAM, LPOBJSTRUCT);
typedef DWORD   (IDRAW_CALL* pfn_Register_Event)    (UINT, LONG_PTR, LPVOID);
typedef int     (CALLBACK* pfn_MenuExt_Command)     (int id, LPARAM param);
typedef int     (CALLBACK* pfn_IDraw_Callback)      (UINT, WPARAM, LPARAM);
typedef bool    (CALLBACK* pfn_CalcScroll)          (void* pScroll, LPOBJSTRUCT pData, RECT* prc);
typedef bool    (CALLBACK* pfn_DrawScroll)          (void* pScroll, LPOBJSTRUCT pData, bool isVScroll, HDC hdc, int type, RECT_F* rcDraw);
typedef bool    (CALLBACK* pfn_GetScrollRect)       (void* pScroll, LPOBJSTRUCT pData, bool isVScroll, RECT* prc);
typedef void    (CALLBACK* pfn_EWindowThreadCallback)(int);


// 枚举常量开始

enum STATE
{
    NORMAL                  = 0X00000000,    // 正常
    BAN                     = 0X00000001,    // 禁止
    SELECTED                = 0X00000002,    // 选择
    FOCUS                   = 0X00000004,    // 焦点
    DOWN                    = 0X00000008,    // 按下
    CHECKED                 = 0X00000010,    // 选中
    INDETERMINATE           = 0X00000020,    // 半选中
    READONLY                = 0X00000040,    // 只读
    HOT                     = 0X00000080,    // 点燃
    DEF                     = 0X00000100,    // 默认
    SUBSHOW                 = 0X00000200,    // 子项目可视
    SUBHIDE                 = 0X00000400,    // 子项目隐藏
    BUSYING                 = 0X00000800,    // 繁忙中
    HOVER                   = 0X00001000,    // 进入
    ROLLING                 = 0X00002000,    // 滚动中
    EFFECTING               = 0X00004000,    // 动画中
    HIDE                    = 0X00008000,    // 隐藏
    ZHANWEI1                = 0X00010000,    // 占位

    ALLOWMODIFYSIZE         = 0X00020000,    // 允许修改尺寸
    ALLOWDRAG               = 0X00040000,    // 允许拖动
    ZHANWEI2                = 0X00080000,    // 占位
    ALLOWFOCUS              = 0X00100000,    // 允许焦点
    ALLOWSELECT             = 0X00200000,    // 允许选择
    ALLOWHREFHOT            = 0X00400000,    // 允许超链接点燃
    ALLOWHREFEND            = 0X00800000,    // 允许超链接已访问
    ALLOWMULTI              = 0X01000000,    // 允许多选
    ZHANWEI3                = 0X02000000,    // 占位
    ZHANWEI4                = 0X04000000,    // 占位
    ZHANWEI5                = 0X08000000,    // 占位
    ZHANWEI6                = 0X10000000,    // 占位
    PASSWORD                = 0X20000000,    // 密码模式
};
typedef LPWINDOW_ICON_DATA(IDRAW_CALL* pfn_GetWindowIcon)(HWND hWnd, IDRAW_HRESOURCE hResource, LPCWSTR pszID, void* pArg, RECT_F* prc, STATE state, UINT dpi);
// 按钮被重画时的回调函数
typedef int(IDRAW_CALL* pfn_ToolDrawButton)(LPCANVAS hCanvas, int id, LPARAM param, const RECT_F* prc, STATE state, UINT dpi);

// 当前MDIClient窗口下显示的窗口是什么类型
enum MDICLIENT_TYPE : int
{
    MDICLIENT_TYPE_NONE     = -1,   // 没有窗口
    MDICLIENT_TYPE_CODE     = 0,    // 代码窗口
    MDICLIENT_TYPE_IDE      = 1,    // ide设计器, 窗口设计器
    MDICLIENT_TYPE_HONE     = 2,    // 起始页
};

// 当前代码编辑窗口是什么类型
enum MDICLIENT_CODETYPE : int
{
    MDICLIENT_CODETYPE_UNDEFINE                 = 0,   // 未定义
    MDICLIENT_CODETYPE_WINDOW                   = 1,   // 窗口设计器
    MDICLIENT_CODETYPE_ASSEMBLY                 = 2,   // 普通程序集
    MDICLIENT_CODETYPE_ASSEMBLY_WINDOW          = 3,   // 窗口程序集
    MDICLIENT_CODETYPE_CLASS                    = 4,   // 类
    MDICLIENT_CODETYPE_GLOBALVAR                = 5,   // 全局变量
    MDICLIENT_CODETYPE_STRUCT                   = 6,   // 数据类型
    MDICLIENT_CODETYPE_DLL                      = 7,   // DLL命令定义表
    MDICLIENT_CODETYPE_CONST                    = 8,   // 常量
    MDICLIENT_CODETYPE_IMAGE                    = 9,   // 图片资源
    MDICLIENT_CODETYPE_SOUND                    = 10,  // 声音资源
};


// 绘画的阶段
enum DRAW_STAGE
{
    DRAW_STAGE_BUTTON_BEGIN,    // 绘画系统按钮开始, 即将绘画系统按钮, 返回true则不使用内部的绘制
    DRAW_STAGE_BUTTON_END,      // 绘画系统按钮结束, 按钮已经绘画完毕
    DRAW_STAGE_ICON_BEGIN,      // 绘画系统图标开始, 即将绘画系统图标, 返回true则不使用内部的绘制
    DRAW_STAGE_ICON_END,        // 绘画系统图标结束, 系统图标已经绘画完毕
    DRAW_STAGE_TITLE_BEGIN,     // 绘画标题开始, 即将绘画标题, 返回true则不使用内部的绘制
    DRAW_STAGE_TITLE_END,       // 绘画标题结束, 标题已经绘画完毕
};

// 代码类型, 从代码管理类的方法中获取的代码类型
// CODELINETYPE = 代码行类型
// CODETYPE = 代码类型
// CODEOFFSET = 代码数据偏移类型
enum class ECODETYPE : int
{
    CODELINETYPE_STRUCT                     = 5,    // 数据类型
    CODELINETYPE_STRUCTMEMBER               = 6,    // 数据类型成员
    CODELINETYPE_GLOBALVAR                  = 7,    // 全局变量
    CODELINETYPE_ASSEMBLY                   = 8,    // 程序集
    CODELINETYPE_ASSEMBLYVAR                = 9,    // 程序集变量
    CODELINETYPE_FUN                        = 10,   // 子程序
    CODELINETYPE_FUNARG                     = 11,   // 子程序参数
    CODELINETYPE_FUNVAR                     = 12,   // 子程序变量
    CODELINETYPE_FUNCODE                    = 13,   // 子程序代码
    CODELINETYPE_CONST                      = 14,   // 常量
    CODELINETYPE_IMAGE                      = 15,   // 资源图片
    CODELINETYPE_SOUND                      = 16,   // 资源声音

    CODETYPE_UNDEFINE                       = 0,    // 未定义
    CODETYPE_ASSEMBLY                       = 1,    // 程序集
    CODETYPE_STRUCT                         = 2,    // 数据类型
    CODETYPE_GLOBALVAR                      = 3,    // 全局变量
    CODETYPE_DLL                            = 4,    // DLL命令定义表
    CODETYPE_WINDOW                         = 5,    // 窗口设计器
    CODETYPE_CONST                          = 6,    // 常量
    CODETYPE_IMAGE                          = 7,    // 图片资源
    CODETYPE_SOUND                          = 8,    // 声音资源

    CODEOFFSET_STRUCT_NAME                  = 261,  // 数据类型名称
    CODEOFFSET_STRUCT_PUBLIC                = 263,  // 数据类型公开
    CODEOFFSET_STRUCT_REMARKS               = 262,  // 数据类型备注
    CODEOFFSET_STRUCT_MEMBER_NAME           = 313,  // 数据类型 成员 名称
    CODEOFFSET_STRUCT_MEMBER_TYPE           = 314,  // 数据类型 成员 类型
    CODEOFFSET_STRUCT_MEMBER_REFERENCE      = 317,  // 数据类型 成员 传址
    CODEOFFSET_STRUCT_MEMBER_ARRAY          = 315,  // 数据类型 成员 数组
    CODEOFFSET_STRUCT_MEMBER_REMARKS        = 316,  // 数据类型 成员 备注

    CODEOFFSET_GLOBALVAR_NAME               = 367,  // 全局变量 名称
    CODEOFFSET_GLOBALVAR_TYPE               = 368,  // 全局变量 类型
    CODEOFFSET_GLOBALVAR_ARRAY              = 369,  // 全局变量 数组
    CODEOFFSET_GLOBALVAR_PUBLIC             = 371,  // 全局变量 公开
    CODEOFFSET_GLOBALVAR_REMARKS            = 370,  // 全局变量 备注


    CODEOFFSET_ASSEMBLY_NAME                = 580,  // 程序集变量 名称
    CODEOFFSET_ASSEMBLY_BASECLASS           = 582,  // 程序集变量 基类
    CODEOFFSET_ASSEMBLY_PUBLIC              = 573,  // 程序集变量 公开
    CODEOFFSET_ASSEMBLY_REMARKS             = 581,  // 程序集变量 备注

    CODEOFFSET_ASSEMBLYVAR_NAME             = 632,  // 程序集变量 名称
    CODEOFFSET_ASSEMBLYVAR_TYPE             = 633,  // 程序集变量 类型
    CODEOFFSET_ASSEMBLYVAR_ARRAY            = 634,  // 程序集变量 数组
    CODEOFFSET_ASSEMBLYVAR_REMARKS          = 635,  // 程序集变量 备注

    CODEOFFSET_FUN_NAME                     = 686,  // 子程序名称
    CODEOFFSET_FUN_RETURNTYPE               = 687,  // 子程序 返回类型
    CODEOFFSET_FUN_PUBLIC                   = 690,  // 子程序 公开
    CODEOFFSET_FUN_EPACKAGE                 = 688,  // 子程序 易包
    CODEOFFSET_FUN_REMARKS                  = 689,  // 子程序 备注

    CODEOFFSET_FUNARG_NAME                  = 740,  // 子程序参数 名称
    CODEOFFSET_FUNARG_TYPE                  = 741,  // 子程序参数 类型
    CODEOFFSET_FUNARG_REFERENCE             = 742,  // 子程序参数 参考
    CODEOFFSET_FUNARG_OPTIONAL              = 743,  // 子程序参数 可空
    CODEOFFSET_FUNARG_ARRAY                 = 744,  // 子程序参数 数组
    CODEOFFSET_FUNARG_REMARKS               = 745,  // 子程序参数 备注

    CODEOFFSET_FUNVAR_NAME                  = 796,  // 子程序变量 名称
    CODEOFFSET_FUNVAR_TYPE                  = 797,  // 子程序变量 类型
    CODEOFFSET_FUNVAR_STATIC                = 798,  // 子程序变量 静态
    CODEOFFSET_FUNVAR_ARRAY                 = 799,  // 子程序变量 数组
    CODEOFFSET_FUNVAR_REMARKS               = 800,  // 子程序变量 备注


    CODEOFFSET_CODE                         = 851,  // 子程序代码



};

enum SCROLL_DRAWTYPE : int
{
    SCROLL_DRAWTYPE_BACK    = 0x00,     // 绘画背景, 这里接收到的dc是内存dc
    SCROLL_DRAWTYPE_BTN1    = 0x01,     // 绘画滚动条的按钮1, 这里接收到的dc是内存dc
    SCROLL_DRAWTYPE_BTN2    = 0x02,     // 绘画滚动条的按2, 这里接收到的dc是内存dc
    SCROLL_DRAWTYPE_THUMB   = 0x03,     // 绘画滚动条的滑块, 这里接收到的dc是内存dc
    SCROLL_DRAWTYPE_ALERT   = 0x05,     // 绘画结束, 这里接收到的HDC是窗口的hdc, 接收到的位置是计算出来有可能需要绘画的位置, 触发这个事件的时候滚动条已经绘画到这个dc上了
};

// 字体样式
enum class FONTSTYLE
{
    FontStyleRegular    = 0,    // 正常, 默认
    FontStyleBold       = 1,    // 粗体
    FontStyleItalic     = 2,    // 斜体
    FontStyleBoldItalic = 3,    // 粗体和斜体
    FontStyleUnderline  = 4,    // 下划线
    FontStyleStrikeout  = 8     // 删除线
};

// 枚举常量结束



// 数据类型开始

// 助手插件信息结构
typedef struct PLUGIN_INFO
{
    DWORD cbSize;           // 结构尺寸
    LPCSTR name;            // 插件名字
    LPCSTR remarks;         // 描述
    LPCSTR version;         // 版本
    LPCSTR author;          // 作者
    DWORD  Versionlimit;    // 版本限制
    void* Dependence;       // 依赖插件

    LPCSTR  guid;                           // 插件唯一标识符
    pfn_MenuExt_Command pfnMenuExt_Command; // 菜单被调用事件
    pfn_IDraw_Callback  pfnCallback;        // 与视觉库通讯的回调函数
}*LPPLUGIN_INFO;

typedef struct IDRAW_STRUCT_SCROLL_DATA
{
    DWORD cbSize;       // 结构尺寸, 调用前需要先给这个成员赋值
    float btn1Size;     // 按钮1的宽高, 横向是左边按钮宽度, 纵向是顶边按钮高度
    float btn2Size;     // 按钮2的宽高, 横向是右边按钮宽度, 纵向是底边按钮高度
    float btnThumbSize; // 滑块宽度, 这个决定了整个滚动条的宽高, 横向是高度, 纵向是宽度
}*LPIDRAW_STRUCT_SCROLL_DATA;




typedef struct IDRAW_STRUCT_WINDOW_ARGUMENT
{
    DWORD               cbSize;         // 当前结构尺寸, 有升级的话根据这个尺寸判断
    ARGB                crNormal;       // 正常颜色, ARGB颜色, 为0则不使用
    ARGB                crHot;          // 点燃颜色, ARGB颜色, 为0则不使用
    ARGB                crDown;         // 按下颜色, ARGB颜色, 为0则不使用
    ARGB                crBack;         // 背景颜色, ARGB颜色, 为0则不使用
    ARGB                crBorder;       // 阴影边框颜色, 为0则没有边框, ARGB颜色
    ARGB                crBorder_nf;    // 阴影边框未激活时的颜色, 为0则没有边框, ARGB颜色
    ARGB                crShadow;       // 阴影颜色, ARGB颜色, 为0则不使用阴影
    ARGB                crShadow_nf;    // 阴影未激活时的颜色, ARGB颜色, 为0则不使用阴影

    ARGB                crTitle;        // 标题已激活颜色, ARGB颜色
    ARGB                crTitle_nf;     // 标题未激活颜色, ARGB颜色


    HWND                hWnd;           // 需要处理的窗口
    int                 sysbtnWidth;    // 系统按钮宽度
    int                 sysbtnHeight;   // 系统按钮高度
    BOOL                isSubClass;     // 是否内部子类化该窗口, 为false则外部调用 iDraw_DrawWindowProc 把需要处理的消息传递进来
    BOOL                isUnicode;      // 窗口是否为Unicode窗口, 为true则使用W版函数
    BOOL                isCasualMove;   // 窗口随意移动, 为false则只有在标题栏中才能移动
    BOOL                isDrawIcon;     // 是否绘画图标, 如果不绘画图标, 鼠标在图标位置点击不会出现菜单, 标题也会占用图标的位置
    BOOL                isDrawDefBtn;   // 是否绘画默认的按钮, 如果为true, 则根据窗口风格, 加入系统按钮, 按钮的ID是 SC_ 开头的常量

    int                 nMinWidth;      // 最小窗口宽度
    int                 nMinHeight;     // 最小窗口高度

    LPCANVAS            hCanvas;        // 绘画句柄, 外部提供一个绘画句柄, 如果不提供, 则内部会创建一个
    LPEX_FONT           hFont;          // 绘画标题使用的字体, 如果外部不提供, 内部会创建一个
    LPEX_BRUSH          hbrTextTitle;   // 绘画标题使用的字体, 如果外部不提供, 内部会创建一个
    LPEX_BRUSH          hbrTextTitle_nf; // 绘画标题使用的字体, 如果外部不提供, 内部会创建一个

}*LPIDRAW_STRUCT_WINDOW_ARGUMENT;


// 绘画的结构
typedef struct IDRAW_STRUCT_DRAWSTRUCT
{
    HWND            hWnd;       // 窗口句柄
    LPCANVAS        hCanvas;    // 画布句柄
    DRAW_STAGE      stage;      // 绘画阶段
    const RECT_F*   rc;         // 绘画的矩形位置
    int             id;         // 绘画系统按钮时使用, 这个表示当前绘画的按钮ID
    STATE           state;      // 状态, 系统按钮使用
    LPARAM          param;      // 项目数值, 绘画事件绑定的项目数值
    LPCWSTR         lpszTitle;  // 绘画标题时使用的标题文本
    LPEX_IMAGE      hIcon;      // 绘画系统图标使用的图标
    LPEX_BRUSH      hbrText;    // 绘画标题使用的画刷

}*LPIDRAW_STRUCT_DRAWSTRUCT;

// 系统按钮被点击事件
typedef bool(CALLBACK* PFN_SysBtnClick)(int id, LPARAM btnParam, BOOL isRButton, LPARAM param);
// 系统按钮被绘画事件
typedef bool(CALLBACK* PFN_SysDraw)(LPIDRAW_STRUCT_DRAWSTRUCT dps);

typedef struct RECT_F
{
    float left;     // 左边
    float top;      // 顶边
    float right;    // 右边
    float bottom;   // 底边
}*LPRECT_F;
typedef const RECT_F* LPCRECT_F;

typedef struct RECTROUNDED_F : RECT_F
{
    float radiusX;  // 圆角
    float radiusY;  // 圆角
}*LPRECTROUNDED_F;
typedef const RECTROUNDED_F* LPCRECTROUNDED_F;

typedef struct POINT_F
{
    float x;    // 坐标点x
    float y;    // 坐标点y
}*LPPOINT_F;
typedef const POINT_F* LPCPOINT_F;

typedef struct ELLIPSE_F : POINT_F
{
    float radiusX;  // 半径宽度
    float radiusY;  // 半径高度
}*LPELLIPSE_F;
typedef const ELLIPSE_F* LPCELLIPSE_F;

typedef struct LINEPOINT_F : POINT_F
{
    float x1;   // 第二个横向坐标
    float y1;   // 第二个纵向坐标
}*LPLINEPOINT_F;
typedef const LINEPOINT_F* LPCLINEPOINT_F;

// 图像锁定结构
typedef struct EX_IMAGELOCK
{
    UINT width;     // 锁定图像的宽度, 内部使用, 不允许修改
    UINT height;    // 锁定图像的高度, 内部使用, 不允许修改
    UINT stride;    // 内部使用, 不允许修改
    void* pScan0;   // 内部使用, 不允许修改
    void* pLock;    // 内部使用, 不允许修改
}*PEX_IMAGELOCK, * LPEX_IMAGELOCK;

// MDIClient 窗口句柄结构
struct EWINDOW_MDICHILD
{
    HWND hMDIClient1;   // MDI下的第一层窗口
    HWND hMDIClient2;   // MDI下的第二层窗口
    HWND hMDIClient3;   // MDI下的第三层窗口, 可以根据这个窗口判断是否为起始页或者设计器窗口或者代码区
    HWND hMDIClient4;   // MDI下的第四层窗口, 如果是代码区, 这个窗口就是代码窗口
    HWND hVScroll;      // 纵向滚动条
    HWND hHScroll;      // 横向滚动条
};

// 各个组件开始和结束绘画使用的结构
struct BEGIN_END_PAINT_STRUCT : PAINTSTRUCT
{
    RECT rc;            // 组件客户区矩形
    int cxClient;       // 客户区宽度
    int cyClient;       // 客户区高度
    LPVOID pFont;       // 内部使用, 不允许修改
    LPVOID hFont;       // 内部使用, 不允许修改
    LPVOID md;          // 内部使用, 不允许修改
    BOOL isDxRender;    // 内部使用, 不允许修改
    LPOBJSTRUCT pData;  // 窗口数据
};

// 计算菜单尺寸事件结构
typedef struct IRE_STRUCT_MENU_CALCSIZE
{
    int width;      // 菜单宽度
    int height;     // 菜单高度
    LPARAM param;   // 添加菜单时的菜单数值, 弹出式菜单这个值为0
}*LPIRE_STRUCT_MENU_CALCSIZE;


// 背景被绘画事件结构
typedef struct IRE_STRUCT_MENU_DRAW
{
    LPCANVAS hCanvas;   // dx使用的绘画句柄, 需要gdi/gdi+的话请使用 iDraw_canvas_getdc() 获取绘画DC
    int width;          // 宽度
    int height;         // 高度
    int id;             // 菜单ID
    UINT type;          // 菜单类型
    LPCWSTR text;       // 菜单标题
    int state;          // 菜单状态, 一个或多个值的组合
}*LPIRE_STRUCT_MENU_DRAW;


typedef struct IRE_STRUCT_GETICON
{
    HWND hWnd;          // 要获取图标的窗口句柄, 如果是菜单这个值为0
    LPVOID hResource;   // 绑定窗口时的资源句柄
    LPCWSTR pszID;      // 项目文本
    LPVOID pArg;        // 如果是菜单, 这个值为菜单ID, 根据组件类型来传递不同数据, 详情请查看帮助文档(未写....)
    RECT_F* prc;        // 指向 IDRAW_RECT_F 结构指针, 需要把图标位置写入这个地址
    int state;          // 当前要获取的状态图标, 目前只有 STATE::NORMAL, STATE::HOT, STATE::DOWN, STATE::BAN 这几个状态
    int* pIconType;     // 图标来源, 0=id指向的图标, 1=默认图标, 目前暂时不使用这个参数

}*LPIRE_STRUCT_GETICON;


// 选择夹尺寸改变事件结构
typedef struct IRE_STRUCT_TAB_SIZE
{
    int index;      // 当前选中项
    int direction;  // 当前选择夹方向
    RECT rc;        // 选择夹内的组件建议的位置, 这个位置已经把选择夹的头部空了出来, 内部根据这个结构调整位置, 可以被修改
    HWND hWnd;      // 选择夹的句柄
}*LPIRE_STRUCT_TAB_CHANGE;

typedef struct IRE_STRUCT_MDICLIENT_MOVE_EVENT
{
    int windowType;     // 当前MDI下窗口的类型, 低位 -1=没有窗口, 0=代码区, 1=设计器窗口, 2=起始页, 高位 当前MDIClient 窗口下是显示什么窗口类型, 0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源
    HWND hChild1;       // MDI窗口下的第一层窗口
    HWND hChild2;       // MDI窗口下的第二层窗口
    HWND hChild3;       // MDI窗口下的第三层窗口
    HWND hChild4;       // MDI下的第四层窗口, 如果是设计器窗口, 那这个窗口就是设计器窗口, 起始页则为最近打开源码超列, 代码区就是代码窗口
    RECT rcChild1;      // MDI下第一层窗口的移动位置, 基于MDIClient这个窗口的位置修改
    RECT rcChild2;      // MDI下第二层窗口的移动位置, 基于MDIClient下的第一层窗口的位置修改
    RECT rcChild3;      // MDI下第三层窗口的移动位置, 基于MDIClient下的第二层窗口的位置修改
    RECT rcChild4;      // MDI下第四层窗口的移动位置, 基于MDIClient下的第三层窗口的位置修改
}*LPIRE_STRUCT_MDICLIENT_MOVE_EVENT;

// 代码切换选择夹选中被改变事件结构
typedef struct IRE_STRUCT_TAB_SELCHANGE
{
    int index;      // 切换后的索引, 如果为-1要么是代码修改要么就是选择夹空了
    int indexOld;   // 切换前的索引
    HWND hWnd;      // 切换后选择夹里的窗口句柄, 工作夹/状态夹传递当前触发事件的选择夹句柄
    HWND hWndOld;   // 切换前选择夹里的窗口句柄, 工作夹/状态夹暂时没有使用这个成员
}*LPIRE_STRUCT_TAB_SELCHANGE;


typedef struct IRE_STRUCT_HOOKARG
{
    int code;           // 详情请参考 SetWindowsHookEx 回调函数说明
    WPARAM wParam;      // 详情请参考 SetWindowsHookEx 回调函数说明
    LPARAM lParam;      // 详情请参考 SetWindowsHookEx 回调函数说明
    LRESULT ret;        // 拦截事件的返回值
    HHOOK hhk;          // 钩子句柄
}*LPIRE_STRUCT_HOOKARG;


// 背景被绘画事件结构
typedef struct IRE_STRUCT_BACKGROUND_DRAW
{
    LPCANVAS hCanvas;   // dx使用的绘画句柄, 需要gdi/gdi+的话请使用 iDraw_canvas_getdc() 获取绘画DC
    int cxClient;       // 宽度
    int cyClient;       // 高度
}*LPIRE_STRUCT_BACKGROUND_DRAW;

// 消息事件的消息参数
typedef struct IRE_STRUCT_MSG
{
    HWND hWnd;          // 触发消息的窗口句柄
    UINT message;       // 消息值
    WPARAM wParam;      // 参数1
    LPARAM lParam;      // 参数2
}*LPIRE_STRUCT_MSG;

// 工具条状态改变事件结构
typedef struct IRE_STRUCT_TOOLSTATE_CHANGE
{
    HWND hWnd;  // 被改变的工具条句柄
    int id;     // 触发改变状态的按钮ID
    int state;  // 状态
}*LPIRE_STRUCT_TOOLSTATE_CHANGE;

// MDI被创建事件参数
typedef struct IRE_STRUCT_MDICREATE
{
    HWND hWnd;                  // MDI窗口句柄
    LPMDICREATESTRUCTA mcs;     // MDI创建窗口结构, 易语言窗口都是A版, 这里接收的就是A版的结构
}*LPIRE_STRUCT_MDICREATE;

// 代码行号被改变事件参数
typedef struct IRE_STRUCT_EIDE_SELCHANGEDINFO
{
    IEIDE_CodeManage* pCodeManage;  // 代码管理类指针
    int line;                       // 当前选中行号
    int oldLine;                    // 选中当前行之前的行号
    int oldLineCol;                 // 选中当前行之前的列号
}*LPIRE_STRUCT_EIDE_SELCHANGEDINFO;

// 自动隐藏的窗口被调整尺寸
typedef struct IRE_STRUCT_AUTOHIDE_MOVE
{
    HWND hWnd;      // 被移动的窗口
    RECT rc;        // 移动的位置
}*LPIRE_STRUCT_AUTOHIDE_MOVE;


// 加入菜单到扩展菜单里, 各个成员可以参考MSDN的 InsertMenu 这个API
typedef struct IDC_STRUCT_MENUEXT_INSERT
{
    LPCSTR guid;            // 加入到的插件guid里
    UINT uPosition;         // 位置或ID
    UINT uFlags;            // 菜单标识
    UINT_PTR uIDNewItem;    // 菜单ID
    LPCWSTR lpNewItem;      // 菜单标题
    LPARAM param;           // 菜单数值
}*LPIDC_STRUCT_MENUEXT_INSERT;

// 数据类型结束




// 图片数据
struct EX_IMAGE
{
public:

    // 释放当前的图像, 释放后不能再使用这个对象, 调用这个方法后会销毁类指针
    virtual int Release() = 0;

    // 获取图像宽度
    virtual UINT GetWidth() = 0;

    // 获取图像高度
    virtual UINT GetHeight() = 0;

    // 获取图像尺寸, 返回图像尺寸, 低位=宽度, 高位=高度
    virtual DWORD GetSize(UINT* cx = 0, UINT* cy = 0) = 0;

    // 从当前图片拷贝一份新的图片出来, 返回新的图片句柄
    // 4个参数分别为需要复制的左顶宽高, 如果宽度/高度等于0, 则使用图像的宽度/高度
    virtual LPEX_IMAGE copy(UINT x = 0, UINT y = 0, UINT width = 0, UINT height = 0) = 0;

    // 锁定位图, 成功返回true, 失败返回false, 不使用时必须调用 unlock 释放数据
    // x,y,width,height = 左顶宽高, 宽高为0时会自动获取图片大小
    // flags = 1=读, 2=写, 3=读写
    // lockData = 锁定图像数据, 解锁图像时需要使用
    virtual bool lock(UINT x, UINT y, UINT width, UINT height, DWORD flags, EX_IMAGELOCK* lockData) = 0;

    // 取消锁定图片
    // lockData = 锁定图像数据, 由 lock 获得
    virtual bool unlock(EX_IMAGELOCK* lockData) = 0;

    // 取图像帧数
    virtual UINT GetFrameCount() = 0;

    // 设置当前活动帧
    // index = 第几帧
    virtual bool SelectActiveFrame(UINT index) = 0;

    // 把当前图片对象保存成图片数据, 返回的图片数据不使用时需要调用本对象的 free 来释放
    // pSize = 接收返回图片数据的大小
    // 左顶右底, 要保存的位置, 默认为图片对象大小
    virtual LPBYTE save(SIZE_T* pSize, UINT left = 0, UINT top = 0, UINT right = 0, UINT bottom = 0) = 0;

    // 把绘画句柄保存成图片数据, 返回的图片数据不使用时需要调用本对象的 free 来释放
    // hCanvas = 绘画句柄
    // pSize = 接收返回图片数据的大小
    // 左顶右底, 要保存的位置, 宽度/高度为0则使用绘画句柄的宽度/高度
    virtual LPBYTE save(const LPCANVAS hCanvas, SIZE_T* pSize, UINT left = 0, UINT top = 0, UINT right = 0, UINT bottom = 0) = 0;

    // 把当前图像转成GDI位图, 不使用时需要调用win32API DeleteObject() 释放
    virtual HBITMAP ToGdiBitmap() = 0;

    // 释放 save 返回的地址
    virtual void free(LPBYTE p) = 0;

};


struct EX_RGN
{
    // 销毁区域, 释放后不能再使用这个对象, 调用这个方法后会销毁类指针
    virtual int Release() = 0;

    // 测试指定坐标是否在区域内
    virtual bool Hittest(float x, float y) = 0;
};

struct EX_FONT
{
    // 销毁字体, 释放后不能再使用这个对象, 调用这个方法后会销毁类指针
    virtual int Release() = 0;

    // 字符高度转字体大小, 字符高度一般是 LOGFONT 的 lfHeight, 字体大小一般是易语言的字体大小
    virtual int height2size(int height) = 0;

    // 字体大小转字符高度, 字符高度一般是 LOGFONT 的 lfHeight, 字体大小一般是易语言的字体大小
    virtual int size2height(int size) = 0;

    // 获取当前字体的LOGFONTW 结构数据
    // lplf = LOGFONTW 结构指针, 内部会把数据写入这个结构里
    virtual bool GetLogfont(LPLOGFONTW lplf) = 0;

    // 获取当前字体对象的GDI字体, 不允许调用 DeleteObject() 进行释放
    virtual HFONT GetFont() = 0;

};

struct EX_PATH
{
    // 销毁路径, 释放后不能再使用这个对象, 调用这个方法后会销毁类指针
    virtual int Release() = 0;

    // 在指定点开始一个新路径
    // ppt = 坐标点指针, 为0则不使用
    virtual bool BeginFigure(const LPCPOINT_F pt = 0) = 0;

    // 结束当前路径
    virtual bool EndFigure() = 0;

    // 重置为空路径
    virtual bool Reset() = 0;

    // 添加弧
    virtual bool AddArc(float x1, float y1, float x2, float y2, float radiusX, float radiusY, bool fClockwise) = 0;

    // 添加贝赛尔曲线
    // pt1, pt2, pt3, pt4 = 4个坐标点
    virtual bool AddBezier(const POINT_F& pt1, const POINT_F& pt2, const POINT_F& pt3, const POINT_F& pt4) = 0;

    // 添加直线
    // pt1, pt2 = 起始点坐标和结束点坐标
    virtual bool AddLine(const POINT_F& pt1, const POINT_F& pt2) = 0;

    // 添加矩形
    // rc = 矩形
    virtual bool AddRect(const RECT_F& rc) = 0;

    // 添加圆角矩形
    // rc = 矩形
    // radiusTopLeft = 左上角圆角度数
    // radiusTopRight = 右上角圆角度数
    // radiusBottomLeft = 左下角圆角度数
    // radiusBottomRight = 右下角圆角度数
    virtual bool AddRoundedRect(const RECT_F& rc, float radiusTopLeft, float radiusTopRight, float radiusBottomLeft, float radiusBottomRight) = 0;

    // 打开路径, 打开后才能往路径里添加
    virtual bool Open() = 0;

    // 关闭路径, 添加完毕后, 使用路径前请关闭路径
    virtual bool Close() = 0;

    // 取路径边界矩形
    // prc = 接收路径边界矩形
    virtual bool GetBounds(LPRECT_F rc) = 0;

    // 测试坐标是否在路径范围内
    // pt = 坐标点
    virtual bool Hittest(const POINT_F& pt) = 0;


};

// 画刷
struct EX_BRUSH
{
    // 销毁画刷, 释放后不能再使用这个对象, 调用这个方法后会销毁类指针
    virtual int Release() = 0;

    // 设置纯色画刷的画刷颜色, 非纯色画刷会设置失败
    // color = 新的画刷颜色
    virtual bool SetColor(ARGB argb) = 0;

    // 获取纯色画刷的颜色, 返回的是ARGB颜色
    virtual ARGB GetColor() = 0;

    // 设置渐变画刷起始位置, GDI+暂未支持
    // x, y = 新的起始位置
    virtual bool SetStartPoint(float x, float y) = 0;

    // 设置渐变画刷结束位置, GDI+暂未支持
    // x, y = 新的结束位置
    virtual bool SetEndPoint(float x, float y) = 0;

    // 获取渐变画刷起始位置, GDI+暂未支持
    // x, y = 接收新的起始位置指针, 为0则不获取
    virtual bool GetStartPoint(float* x = 0, float* y = 0) = 0;

    // 获取渐变画刷结束位置, GDI+暂未支持
    // x, y = 接收新的结束位置指针, 为0则不获取
    virtual bool GetEndPoint(float* x = 0, float* y = 0) = 0;

    // 设置放射渐变画刷中心坐标, GDI+暂未支持
    // x, y = 新的中心点位置
    virtual bool SetCenter(float x, float y) = 0;

    // 获取放射渐变画刷中心坐标, GDI+暂未支持
    // x, y = 接收新的中心点位置指针, 为0则不获取
    virtual bool GetCenter(float* x = 0, float* y = 0) = 0;

    // 设置放射渐变原点偏移, GDI+暂未支持
    // x, y = 新的偏移位置
    virtual bool SetGradientOriginOffset(float x, float y) = 0;

    // 获取放射渐变原点偏移, GDI+暂未支持
    // x, y = 接收新的偏移位置指针, 为0则不获取
    virtual bool GetGradientOriginOffset(float* x = 0, float* y = 0) = 0;

    // 设置放射渐变画刷横向半径, GDI+暂未支持
    // x = 中心点横向半径
    virtual bool SetRadiusX(float x) = 0;

    // 获取放射渐变画刷横向半径, GDI+暂未支持
    virtual float GetRadiusX() = 0;

    // 设置放射渐变画刷纵向半径, GDI+暂未支持
    // y = 中心点纵向半径
    virtual bool SetRadiusY(float y) = 0;

    // 获取放射渐变画刷纵向半径, GDI+暂未支持
    virtual float GetRadiusY() = 0;

};

// 画笔
struct EX_PEN
{
    // 销毁画笔, 释放后不能再使用这个对象, 调用这个方法后会销毁类指针
    virtual int Release() = 0;

    // 设置画笔的颜色
    // color = 新的画笔ARGB颜色
    virtual bool SetColor(ARGB argb) = 0;

    // 获取画笔的颜色
    virtual ARGB GetColor() = 0;

    // 设置画笔的宽度
    // width = 画笔宽度
    virtual bool SetWidth(float width) = 0;

    // 获取画笔的宽度
    virtual float GetWidth() = 0;

    // 设置画笔线条样式
    // style = 0=实线, 1=划线{2, 2}, 2=点线{0, 2}, 3=点划线{2, 2, 0, 2}, 4=点点划线{2, 2, 0, 2, 0, 2}, 5=自定义 Gdiplus::DashStyle 或者 D2D1_DASH_STYLE 这两个枚举常量的成员
    // dashes = 点线的数组, 划线={2,2}, 点线={0,2}, 点划线={2,2,0,2}, 点点划线={2,2,0,2,0,2}, 其他请自定义
    // dashesCount = 数组成员数
    virtual float SetDashStyle(int style = 0, float* dashes = 0, int dashesCount = 0) = 0;

};

// 绘画信息
struct CANVAS
{
public:

    virtual int Release() = 0;
    virtual HWND GetWindow() = 0;
    virtual UINT GetWidth() = 0;
    virtual UINT GetHeight() = 0;
    virtual bool GetTargetRender(LPVOID* ppObj) = 0;
    virtual HDC GetGdiDC() = 0;

    // 开始绘画, 绘画前必须调用, 调用后才可以进行绘画操作, 结束绘画时必须调用 EndDraw() 来提交绘画结果
    virtual bool BeginDraw() = 0;

    // 结束绘画, 返回绘画是否有错误, 如果有错误可以调用 ExGetLastError() 获取错误码, 如果错误码是 D2DERR_RECREATE_TARGET 则需要重新创建D2D资源了
    virtual bool EndDraw() = 0;

    // 重新调整画布尺寸
    // cx = 新的画布宽度
    // cy = 新的画布高度
    virtual bool Resize(int cx, int cy) = 0;

    // 获取画布的尺寸
    // pWidth = 接收画布宽度的指针, 为0则不接收
    // pHeight = 接收画布高度的指针, 为0则不接收
    virtual bool GetSize(UINT* width, UINT* height) = 0;

    // 获取画布对应的GDI HDC
    // 只能在 BeginDraw() 和 EndDraw() 之间调用
    // 在调用 EndDraw() 前必须调用 ReleaseDC() 进行释放, 否则 EndDraw() 将调用失败
    virtual HDC GetDC() = 0;

    // 释放 GetDC() 获取的HDC, 如果有调用 GetDC(), 则必须调用这个函数释放
    virtual void ReleaseDC() = 0;

    // 清除画布
    // argb = 清除画布时填充的ARGB颜色, 0=透明
    virtual bool clear(ARGB argb) = 0;

    // 执行所有挂起的绘图命令
    virtual bool flush() = 0;

    // 设置当前渲染的图像, 设置之后绘画操作会保存到 image 参数里, 类似与GDI的位图
    // image = 图像句柄
    virtual bool SetTarget(LPEX_IMAGE bitmap) = 0;

    // 还原当前渲染图像
    virtual bool ReTarget() = 0;

    // 获取当前渲染图像句柄, 返回的句柄不使用时需要调用 FreeTarget() 进行释放
    virtual LPEX_IMAGE GetTarget() = 0;

    // 释放 GetTarget() 返回的图像句柄
    // image = GetTarget() 返回的值
    virtual void FreeTarget(LPEX_IMAGE img) = 0;



    // 平移
    // x, y = 平移的横向纵向坐标
    virtual bool translate(float x, float y) = 0;
    // 缩放
    // x, y = 缩放的坐标点
    // scaleX, scaleY = 缩放的横向纵向比例
    virtual bool scale(float x, float y, float scaleX, float scaleY) = 0;
    // 旋转
    // x, y = 旋转的坐标点
    // rotate = 旋转的角度, 负数则反向旋转
    virtual bool rotate(float x, float y, float rotate) = 0;


    // 绘制简单图形

    // 绘画矩形
    // hPen = 边框画笔
    // prc = RECT_F 矩形指针
    virtual bool DrawRectangle(const LPEX_PEN hPen, const LPCRECT_F rcF) = 0;

    // 填充矩形
    // hBrush = 填充的画刷
    // prc = RECT_F 矩形指针
    virtual bool FillRectangle(const LPEX_BRUSH hBrush, const LPCRECT_F rcF) = 0;

    // 绘画椭圆
    // hPen = 边框画笔
    // prc = ELLIPSE_F 椭圆位置指针, 记录一个中心点和原的宽度和高度
    virtual bool DrawEllipse(const LPEX_PEN hPen, const LPCELLIPSE_F rcF) = 0;

    // 填充椭圆
    // hBrush = 填充画刷
    // prc = ELLIPSE_F 椭圆位置指针, 记录一个中心点和原的宽度和高度
    virtual bool FillEllipse(const LPEX_BRUSH hBrush, const LPCELLIPSE_F rcF) = 0;

    // 绘画圆角矩形
    // hPen = 边框画笔
    // prcRounded = RECTROUNDED_F 圆角矩形指针
    virtual bool DrawRoundedRectangle(const LPEX_PEN hPen, const LPCRECTROUNDED_F rcRounded) = 0;

    // 填充圆角矩形
    // hBrush = 填充画刷
    // prcRounded = RECTROUNDED_F 圆角矩形指针
    virtual bool FillRoundedRectangle(const LPEX_BRUSH hBrush, const LPCRECTROUNDED_F rcRounded) = 0;

    // 绘画直线
    // hPen = 线条颜色
    // ppt = 线条坐标
    virtual bool DrawLine(const LPEX_PEN hPen, const LPCLINEPOINT_F pt) = 0;

    // 绘画路径
    // hPen = 边框画笔
    // path = 路径指针
    virtual bool DrawPath(const LPEX_PEN hPen, const LPEX_PATH path) = 0;

    // 填充路径
    // hBrush = 填充画刷
    // path = 路径指针
    virtual bool FillPath(const LPEX_BRUSH hBrush, const LPEX_PATH path) = 0;


    // 绘画图像, 把整个图像绘画到画布指定的位置, 不会对图像进行缩放
    // image = 图像句柄
    // left = 绘画到画布上的左边位置
    // top = 绘画到画布上的顶边位置
    // alpha = 透明度, 0=完全透明, 255=完全不透明
    virtual bool DrawImage(LPEX_IMAGE image, float left, float top, BYTE alpha) = 0;

    // 绘画图像, 把整个图像绘画到画布指定的位置, 如果指定的大小和图像的不同, 则会缩放
    // image = 图像句柄
    // left = 绘画到画布上的左边位置
    // top = 绘画到画布上的顶边位置
    // right = 绘画到画布上的右边位置
    // bottom = 绘画到画布上的底边位置
    // alpha = 透明度, 0=完全透明, 255=完全不透明
    virtual bool DrawImage(LPEX_IMAGE image, float left, float top, float right, float bottom, BYTE alpha) = 0;

    // 绘画图像, 从图像的某个位置绘画到画布某个位置, 尺寸不同会缩放
    // image = 图像句柄
    // dstLeft = 绘画到画布上的左边位置
    // dstTop = 绘画到画布上的顶边位置
    // dstRight = 绘画到画布上的右边位置
    // dstBottom = 绘画到画布上的底边位置
    // srcLeft = 从图像的左边位置
    // srcTop = 从图像的顶边位置
    // srcRight = 从图像的右边位置
    // srcBottom = 从图像的底边位置
    // alpha = 透明度, 0=完全透明, 255=完全不透明
    virtual bool DrawImage(LPEX_IMAGE image, float dstLeft, float dstTop, float dstRight, float dstBottom,
                           float srcLeft, float srcTop, float srcRight, float srcBottom, BYTE alpha) = 0;

    // 绘画九宫图像, 尺寸不同会缩放
    // image = 图像句柄
    // dstLeft = 绘画到画布上的左边位置
    // dstTop = 绘画到画布上的顶边位置
    // dstRight = 绘画到画布上的右边位置
    // dstBottom = 绘画到画布上的底边位置
    // srcLeft = 从图像的左边位置
    // srcTop = 从图像的顶边位置
    // srcRight = 从图像的右边位置
    // srcBottom = 从图像的底边位置
    // gridPaddingLeft = 九宫区域左边
    // gridPaddingTop = 九宫区域顶边
    // gridPaddingRight = 九宫区域右边
    // gridPaddingBottom = 九宫区域底边
    // alpha = 透明度, 0=完全透明, 255=完全不透明
    virtual bool DrawImage(LPEX_IMAGE image, float dstLeft, float dstTop, float dstRight, float dstBottom,
                           float srcLeft, float srcTop, float srcRight, float srcBottom,
                           float gridPaddingLeft, float gridPaddingTop, float gridPaddingRight, float gridPaddingBottom,
                           DWORD dwFlags, BYTE alpha) = 0;

    // 计算文本尺寸
    // font = 字体句柄
    // text = 需要计算的文本
    // textLen = 文本长度, 如果为-1则text必须是\\0结尾
    // textFormat = 文本对齐格式, 参考 DT_ 开头常量, 与 win32API DrawText一样
    // layoutWidth = 布局宽度, 计算文本会根据这个宽度计算行数, 行数不同高度就会不同
    // layoutHeight = 布局高度
    // pWidth = 接收计算出的文本宽度
    // pHeight = 接收计算出的文本高度
    virtual bool calctextsize(LPEX_FONT font, LPCWSTR text, size_t textLen,
                              DWORD textFormat, float layoutWidth, float layoutHeight, float* pWidth, float* pHeight) = 0;

    // 绘画文本
    // font = 字体句柄
    // hbrText = 文本画刷
    // text = 需要绘画的文本
    // textLen = 文本长度, 如果为-1则text必须是\\0结尾
    // textFormat = 文本对齐格式, 参考 DT_ 开头常量, 与 win32API DrawText一样
    // prc = 文本绘画的位置
    virtual bool drawtext(LPEX_FONT font, LPEX_BRUSH hBrush, LPCWSTR text, size_t textLen, DWORD textFormat, const RECT_F* rc) = 0;

    // 绘画文本
    // hCanvas = 画布句柄
    // font = 字体句柄
    // hbrText = 文本画刷
    // text = 需要绘画的文本
    // textLen = 文本长度, 如果为-1则text必须是\\0结尾
    // textFormat = 文本对齐格式, 参考 DT_ 开头常量, 与 win32API DrawText一样
    // prc = 文本绘画的位置
    // iGlowsize = 暂未使用, 传递0
    // crShadow = 暂未使用, 传递0
    // lParam = 暂未使用, 传递0
    // prclayout = 暂未使用, 传递0
    // outrcText = 接收绘画文本的区域
    virtual bool drawtextex(LPEX_FONT font, LPEX_BRUSH hBrush,
                            LPCWSTR text, size_t textLen, DWORD textFormat, const RECT_F* rc,
                            DWORD iGlowsize, ARGB crShadow, LPDRAWTEXTPARAMS lParam, const RECT_F* prclayout, RECT_F* outrcText) = 0;

    // 混合拷贝, 拷贝数据到画布上
    // xoriginDest = 拷贝到画布的左边位置
    // yoriginDest = 拷贝到画布的左边位置
    // wDest = 拷贝到画布的宽度
    // hDest = 拷贝到画布的高度
    // sCanvas = 从哪个画布上拷贝
    // xoriginSrc = 从 sCanvas 画布上的哪个位置开始拷贝
    // yoriginSrc = 从 sCanvas 画布上的哪个位置开始拷贝
    // wSrc = 从 sCanvas 画布上的哪个位置开始拷贝
    // hSrc = 从 sCanvas 画布上的哪个位置开始拷贝
    // Alpha = 透明度
    virtual bool Alphablend(float xoriginDest, float yoriginDest, float wDest, float hDest, const LPCANVAS sCanvas,
                            float xoriginSrc, float yoriginSrc, float wSrc, float hSrc, BYTE Alpha) = 0;

    // 位拷贝, 拷贝数据到画布上
    // x = 拷贝到画布的左边位置
    // y = 拷贝到画布的左边位置
    // cx = 拷贝到画布的宽度
    // cy = 拷贝到画布的高度
    // sCanvas = 从哪个画布上拷贝
    // x1 = 从 sCanvas 画布上的哪个位置开始拷贝
    // y1 = 从 sCanvas 画布上的哪个位置开始拷贝
    // rop = 光栅操作代码, 一般是 SRCCOPY, D2D渲染忽略这个参数, D2D只支持 SRCCOPY
    virtual bool bitblt(int x, int y, int cx, int cy, const LPCANVAS sCanvas, int x1, int y1, DWORD rop) = 0;

    // 暂未实现, 请不要调用
    virtual bool SetRgn(float left, float top, float right, float bottom, int mode = 0) = 0;

    // 设置裁剪区, 设置后绘画的内容只会在设置的区域里显示
    // left = 裁剪区的左边
    // top = 裁剪区的顶边
    // right = 裁剪区的右边
    // bottom = 裁剪区的底边
    // mode = 裁剪区模式, 一般填0, 现在内部暂时还没有为D2D和GDI+做个统一的标准, D2D渲染请查看D2D相关的常量, GDI+请参阅GDI+的常量
    virtual bool SetClip(float left, float top, float right, float bottom, int mode = 0) = 0;
    virtual bool ResetClip() = 0;

    // 设置抗锯齿模式, 0=关闭抗锯齿, 1=开启
    // 此函数调与 SetSmoothingMode, 功能一模一样, 多加一个是为了和d2d名字一样
    virtual bool SetAntialiasMode(int mode, int* oldMode = 0) = 0;

    // 设置抗锯齿模式, 0=关闭抗锯齿, 1=开启
    // 此函数调用 SetAntialiasMode 实现, 功能一模一样, 多加一个是为了和gdi+名字一样
    virtual bool SetSmoothingMode(int mode, int* oldMode = 0) = 0;

    // 设置字体抗锯齿模式, 0=默认, 1=ClearType抗锯齿(最高质量), 2=灰度抗锯齿, 3=不使用抗锯齿
    // 此函数调与 SetTextRenderingHint, 功能一模一样, 多加一个是为了和d2d名字一样
    virtual bool SetTextAntialiasMode(int mode, int* oldMode = 0) = 0;
    // 设置字体抗锯齿模式, 0=默认, 1=ClearType抗锯齿(最高质量), 2=灰度抗锯齿, 3=不使用抗锯齿
    // 此函数调用 SetTextAntialiasMode 实现, 功能一模一样, 多加一个是为了和gdi+名字一样
    virtual bool SetTextRenderingHint(int mode, int* oldMode = 0) = 0;



    //inline operator HDC() const { return hdc; }
    //inline operator HBITMAP() const { return hBitmap; }
    //inline operator ID2D1Bitmap1* () const { return pBitmap; }
    //inline operator Gdiplus::Graphics* () const { return pGraihics; }
    //inline operator ID2D1DeviceContext* () const { return pRenderTarget; }

};



// 常量开始

#define HRESOURCE_MENU                      ((HWND)(10000 - 4))     // 获取菜单资源句柄
#define HRESOURCE_TREEEXPAND                ((HWND)(10000 - 8))     // 获取树型框加减号资源句柄


// 通知消息, 低位保留, 使用的都是高位
#define INE_MENU_CALCMENU                   0x01000000  // 计算扩展菜单尺寸, wParam接收菜单ID, lParam接收 IRE_STRUCT_MENU_CALCSIZE 结构指针, 通过修改结构的值来处理菜单尺寸, 忽略返回值
#define INE_MENU_DRAWMENU_BEFORE            0x01010000  // 菜单即将绘画, wParam接收菜单ID, lParam接收 IRE_STRUCT_MENU_DRAW结构指针, 返回是否拦截
#define INE_MENU_DRAWMENU_AFTER             0x01020000  // 菜单绘画完毕, wParam接收菜单ID, lParam接收 IRE_STRUCT_MENU_DRAW结构指针, 忽略返回值
#define INE_MENU_GETICON                    0x01030000  // 获取菜单图标, wParam接收菜单ID, lParam接收 IRE_STRUCT_GETICON结构指针, 返回图片指针(LPWINDOW_ICON_DATA), 如果有返回值, 但是 prc没有值则忽略返回值

#define INE_CONFIG_CHANGED                  0x02000000  // 配置被改变, wParam 保留, lParam 保留

#define INE_TOOL_BUTTONCLICK                0x03000000  // 通知接口工具条的按钮被单击, wParam = id, lParam = 加入按钮时填写的项目数值




#define IGL_HWND                            0x0000       // 窗口句柄, 只读
#define IGL_WNDPROC                         0x0001       // 接收子类化的函数地址, iDraw_SubclassWindow() 函数的第二个参数记录在这
#define IGL_CALLBACK_NOTIFY                 0x0002       // 通知消息函数, 由父窗口调用, 函数原型 LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
#define IGL_CALLBACK_DRAWITEM               0x0003       // 绘画消息函数, 由父窗口调用, 函数原型 LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
#define IGL_CALLBACK_MEASUREITEM            0x0004       // 计算尺寸消息函数, 由父窗口调用, 函数原型 LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
#define IGL_CALLBACK_COMMAND                0x0005       // 事件消息函数, 由父窗口调用, 函数原型 LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
#define IGL_CALLBACK_CTLCOLOR               0x0006       // 颜色消息函数, 由父窗口调用, 函数原型 LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
#define IGL_CALLBACK_CALLWINDOWPROC         0x0007       // CallWindowProc 函数地址, 调用原函数时使用这个函数调用, 在调用子类化时会根据窗口是否为unicode版记录
#define IGL_INTERCEPT                       0x0008       // 是否拦截此消息, 挂接消息时才会使用这个值
#define IGL_OLDPROC                         0x0009       // 窗口的原回调函数, 把不处理的消息转到这个函数去处理
#define IGL_CALLPROC_RETURN                 0x000A       // 只读, 调用原回调函数的返回值, 注册处理后消息事件下可以通过这个索引取到返回值
#define IGL_CANVAS                          0x000B      // 与窗口关联的dx绘画句柄, 如果窗口不是使用dx渲染, 这个值为0
#define IGL_USERDATA                        0x000C      // 与窗口关联的用户数据

#define IGL_DRAW_NONCPAINT                  0x0100       // 不重画非客户区, 为true时接收子类化消息的函数不会收到 WM_NCPAINT 消息
#define IGL_DRAW_NOCALCSIZE                 0x0101       // 不计算非客户区, 为true时接收子类化消息的函数不会收到 WM_NCCALCSIZE 消息, 窗口不会有边框
#define IGL_DRAW_ISTRANSPARENT              0x0102       // 是否透明, 如果为true, 在内置的 CTLCOLOR 中会调用 SetMode() 设置为透明
#define IGL_DRAW_ISHOT                      0x0103       // 是否允许热点, 一般是列表相关的组件使用
#define IGL_DRAW_ISCLIENT                   0x0104       // 是否把所有鼠标消息都视为客户区, 为true则不会收到 WM_NCHITTEST消息, 内部全部返回 HTCLIENT
#define IGL_DRAW_ISERASEBKG                 0x0105       // 是否处理重画背景消息, 如果为true, 则窗口会收到 WM_ERASEBACKGROUND 消息, 否则内部返回1
#define IGL_DRAW_STATE                      0x0106       // 当前组件的状态
#define IGL_DRAW_FONT_DX                    0x0110       // 获取DX字体, 只读
#define IGL_DRAW_FONT                       0x0111       // 获取GDI字体, 只读
#define IGL_DRAW_TEXTFORMAT                 0x0112       // 绘画文本对齐格式

#define IGL_DRAW_CRBACKGROUND               0x0150       // 背景颜色, 每个组件都会使用这个颜色来绘画背景
#define IGL_DRAW_CRTEXT                     0x0151       // 文本颜色, 每个组件的正常文本颜色都是使用这个颜色绘画
#define IGL_DRAW_CRTEXTHOT                  0x0152       // 文本热点颜色, 哪个组件用到请参考资料
#define IGL_DRAW_CRTEXTDOWN                 0x0153       // 文本按下颜色, 哪个组件用到请参考资料
#define IGL_DRAW_CRTEXTBAN                  0x0154       // 文本禁止颜色, 哪个组件用到请参考资料
#define IGL_DRAW_CRNORMAL                   0x0155       // 组件正常颜色, 哪个组件用到请参考资料
#define IGL_DRAW_CRHOVER                    0x0156       // 组件进入颜色, 哪个组件用到请参考资料
#define IGL_DRAW_CRDOWN                     0x0157       // 组件按下颜色, 哪个组件用到请参考资料
#define IGL_DRAW_CRFOCUS                    0x0158       // 组件焦点颜色, 哪个组件用到请参考资料
#define IGL_DRAW_CRCHECKED                  0x0159       // 组件选中颜色, 哪个组件用到请参考资料
#define IGL_DRAW_CRSELECTED                 0x015A       // 组件选择颜色, 哪个组件用到请参考资料
#define IGL_DRAW_CRHOT                      0x015B       // 组件热点颜色, 哪个组件用到请参考资料
#define IGL_DRAW_CRBAN                      0x015C       // 组件禁止颜色, 哪个组件用到请参考资料
#define IGL_DRAW_CRVISTED                   0x015D       // 
#define IGL_DRAW_CRSHADOW                   0x015E       // 组件引用颜色, 哪个组件用到请参考资料
#define IGL_DRAW_CRBORDER                   0x015F       // 组件边框颜色, 所有绘制边框使用的颜色

#define IGL_DRAW_HBRBACKGROUND              0x0160       // 背景GDI画刷, 每个组件都会使用这个画刷来填充背景
#define IGL_DRAW_HBRTEXT                    0x0161       // 文本GDI画刷
#define IGL_DRAW_HBRTEXTHOT                 0x0162       // 文本热点GDI画刷, 哪个组件用到请参考资料
#define IGL_DRAW_HBRTEXTDOWN                0x0163       // 文本按下GDI画刷, 哪个组件用到请参考资料
#define IGL_DRAW_HBRTEXTBAN                 0x0164       // 文本禁止GDI画刷, 哪个组件用到请参考资料
#define IGL_DRAW_HBRNORMAL                  0x0165       // 组件正常GDI画刷, 哪个组件用到请参考资料
#define IGL_DRAW_HBRHOVER                   0x0166       // 组件进入GDI画刷, 哪个组件用到请参考资料
#define IGL_DRAW_HBRDOWN                    0x0167       // 组件按下GDI画刷, 哪个组件用到请参考资料
#define IGL_DRAW_HBRFOCUS                   0x0168       // 组件焦点GDI画刷, 哪个组件用到请参考资料
#define IGL_DRAW_HBRCHECKED                 0x0169       // 组件选中GDI画刷, 哪个组件用到请参考资料
#define IGL_DRAW_HBRSELECTED                0x016A       // 组件选择GDI画刷, 哪个组件用到请参考资料
#define IGL_DRAW_HBRHOT                     0x016B       // 组件热点GDI画刷, 哪个组件用到请参考资料
#define IGL_DRAW_HBRBAN                     0x016C       // 组件禁止GDI画刷, 哪个组件用到请参考资料
#define IGL_DRAW_HBRVISTED                  0x016D       // 
#define IGL_DRAW_HBRSHADOW                  0x016E       // 组件阴影GDI画刷, 哪个组件用到请参考资料
#define IGL_DRAW_HBRBORDER                  0x016F       // 组件边框GDI画笔, 所有绘制边框使用的画笔

#define IGL_POS_RECTSCREEN                  0x0200       // 窗口在屏幕中的位置, 返回RECT结构指针, 不允许修改这个指针, 只读
#define IGL_POS_RECTCLIENT                  0x0201       // 窗口在父窗口中的位置, 返回RECT结构指针, 不允许修改这个指针, 只读
#define IGL_POS_X                           0x0202       // 窗口在父窗口中的左边位置, 只读
#define IGL_POS_Y                           0x0203       // 窗口在父窗口中的顶边位置, 只读
#define IGL_POS_WIDTH                       0x0204       // 窗口的宽度 只读
#define IGL_POS_HEIGHT                      0x0205       // 窗口的高度, 只读




//////////////////////////////////////////////////////////////////////////
// 0x0000 - 0x00FF 这些常量是只读常量, 是从视觉库里获取一些数据
#define IGI_EWINDOW                         0x0000  // 只读, 易语言主窗口
#define IGI_CUSTOMCODETAB                   0x0001  // 只读, 代码选择夹的窗口句柄
#define IGI_CODE_HWND                       0x0002  // 只读, 根据索引获取 MDIClient 下的第一层窗口句柄, argument 需要传递索引
#define IGI_SHADOWWINDOW                    0x0003  // 只读, 阴影窗口, 背景插件是基于这个窗口绘画的, 这个窗口是层窗口
#define IGI_MDICLIENTTYPE                   0x0004  // 只读, 当前MDIClient 窗口下是显示什么窗口, 低位 MDICLIENT_TYPE 枚举常量, 高位 MDICLIENT_CODETYPE 枚举常量, -1=没有窗口, 0=代码区, 1=设计器窗口, 2=起始页, 高位是窗口类型, 0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源
#define IGI_MDICLIENTCODETYPE               0x0005  // 只读, 获取指定MDIClient 窗口下是显示什么窗口类型, 第三个参数传递代码选择夹索引, 传递-1则返回当前索引, MDICLIENT_CODETYPE 枚举常量, 0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源
#define IGI_MDICLIENTTYPE_FROMINDEX         0x0006  // 只读, 获取MDIClient 窗口下是显示什么窗口, 低位 MDICLIENT_TYPE 枚举常量, 高位 MDICLIENT_CODETYPE 枚举常量, -1=没有窗口, 0=代码区, 1=设计器窗口, 2=起始页, 高位是窗口类型, 0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源
#define IGI_DPI                             0x0007  // 只读, 获取当前窗口DPI

#define IGI_RCCUSTOMCODETAB                 0x0020  // 只读, 代码选择夹的位置, 基于父窗口的位置, 返回的是 RECT 结构的指针, 不允许修改这个结构的数据
#define IGI_RCSTATE                         0x0021  // 只读, 下面状态夹的位置, 基于父窗口的位置, 返回的是 RECT 结构的指针, 不允许修改这个结构的数据
#define IGI_RCSTATUSBAR                     0x0022  // 只读, 下面状态条的位置, 基于父窗口的位置, 返回的是 RECT 结构的指针, 不允许修改这个结构的数据
#define IGI_RCMDICLIENT                     0x0023  // 只读, 中间代码区的位置, 基于父窗口的位置, 返回的是 RECT 结构的指针, 不允许修改这个结构的数据
#define IGI_RCCONTROL                       0x0024  // 只读, 组件箱的位置, 基于父窗口的位置, 返回的是 RECT 结构的指针, 不允许修改这个结构的数据
#define IGI_RCWORK                          0x0025  // 只读, 工作夹的位置, 基于父窗口的位置, 返回的是 RECT 结构的指针, 不允许修改这个结构的数据
#define IGI_RCMENUWND                       0x0026  // 只读, 菜单栏容器的位置, 基于父窗口的位置, 返回的是 RECT 结构的指针, 不允许修改这个结构的数据
#define IGI_DEFDXFONT                       0x0027  // 只读, 程序默认使用的DX字体, 会根据当前窗口DPI返回对应的字体
#define IGI_CONFIG_DEFDXFONT                0x0027  // 只读, 程序默认使用的DX字体, 会根据当前窗口DPI返回对应的字体
#define IGI_CONFIG_DEFGDIFONT               0x0028  // 只读, 程序默认使用的GDI字体, 会根据当前窗口DPI返回对应的字体

#define IGI_IGETEWINDOW                     0x0030  // 只读, 易语言获取各个窗口的类指针
#define IGI_IDENOTIFY                       0x0031  // 只读, 易语言交互函数, 调用这个函数就相当于调用特殊库的 调用易系统功能() 命令

#define IGI_LIBPATH                         0x0035  // 只读, 易语言支持库目录, 后面带反斜杠, 返回unicode文本指针
#define IGI_DATAPATH                        0x0036  // 只读, 易IDE数据存储目录, 后面带反斜杠, 一般是 易语言支持库目录/iDraw/, 返回unicode文本指针
#define IGI_IDRAW                           0x0037  // 只读, 自绘类接口, 目前暂时未对外公开
#define IGI_D2D                             0x0038  // 只读, D2D类接口, 目前暂时未对外公开

#define IGI_STATE_TOOL_GET                  0x0040  // 只读, 获取指定工具条ID的状态, 参数3需要传递按钮ID
#define IGI_STATE_TOOL_RUN                  0x0041  // 只读, 当前易语言运行按钮的状态
#define IGI_STATE_TOOL_STOP                 0x0042  // 只读, 当前易语言停止按钮的状态

#define IGI_PATH_LIB                        0x0060  // 只读, 易语言支持库目录, 后面带反斜杠, 返回unicode文本指针
#define IGI_PATH_IDRAW                      0x0061  // 只读, 易IDE主要使用的目录, 后面带反斜杠, 一般是 易语言支持库目录/iDraw/, 返回unicode文本指针
#define IGI_PATH_DATA                       0x0062  // 只读, 易IDE数据存储目录, 后面带反斜杠, 一般是 易语言支持库目录/iDraw/data/, 返回unicode文本指针
#define IGI_PATH_PLUGIN                     0x0063  // 只读, 易IDE插件路径
#define IGI_PATH_CONFIG                     0x0064  // 只读, 易IDE数据配置文件完整路径


// 0x0100 开始就是主配置相关常量, 0x0200 - 0x03FF 是颜色配置相关常量
#define IGI_CONFIG_ISDXRENDER               0x0100  // 是否使用DX渲染, 设置需要重启生效
#define IGI_CONFIG_ISRUNSAVE                0x0101  // 运行/编译后是否保存源码
#define IGI_CONFIG_WHEELPOS                 0x0102  // 鼠标滚动一次的距离
#define IGI_CONFIG_NOCLIPCHILDREN           0x0103  // 父窗口绘画时不排除子窗口区域
#define IGI_CONFIG_ISDPIAWARE               0x0104  // 是否开启DPI感知
#define IGI_CONFIG_ISDPISCALEWND            0x0105  // DPI改变时是否缩放窗口
#define IGI_CONFIG_SHOWCODETABCLOSE         0x0106  // 显示代码选择夹的关闭按钮

#define IGI_CONFIG_MENUITEMHEIGHT           0x0110  // 菜单项高度
#define IGI_CONFIG_MENUSEPARATORHEIGHT      0x0111  // 分隔条菜单项高度
#define IGI_CONFIG_ISLOCATIONTOOLBAR        0x0112  // 是否显示定位工具条
#define IGI_CONFIG_PROGRAMDBLCLKTYPE        0x0113  // 程序树型框双击模式, 0=只切换, 1=只展开, 2=展开+切换
#define IGI_CONFIG_NOCHANGETAB_WORK         0x0114  // 不自动切换工作夹
#define IGI_CONFIG_NOCHANGETAB_STATE        0x0115  // 不自动切换状态夹
#define IGI_CONFIG_SCROLL_WIDTH             0x0116  // 滚动条的宽度, 默认是16, 横向滚动条就是高度
#define IGI_CONFIG_SCROLL_BTN_SIZE          0x0117  // 滚动条按钮的尺寸, 默认是12, 纵向滚动条宽度就是滚动条的宽度, 这个尺寸是纵向的高度, 横向相反

#define IGI_CONFIG_WORK_DIRECTION           0x0120  // 工作夹的子夹方向, 0=左边, 1=顶边, 2=右边, 3=底边
#define IGI_CONFIG_WORK_ISICON              0x0121  // 左边的工作夹是否显示图标
//#define IGI_CONFIG_WORK_ISSHOW              0x0122  // 工作夹是否显示, 如果为false, 则失去焦点就隐藏起来
//#define IGI_CONFIG_WORK_POS                 0x0123  // 工作夹位置, 0=左边, 1=顶边, 2=右边, 3=底边
#define IGI_CONFIG_WORK_FLAGS               0x0124  // 工作夹标识, 记录位置, 是否最大化, 是否浮动, 是否停靠等信息 TOP_STATE_BUTTON 枚举常量里的值, 调用 __query_flags 判断值

#define IGI_CONFIG_STATE_DIRECTION          0x0130  // 状态夹的子夹方向, 0=左边, 1=顶边, 2=右边, 3=底边
#define IGI_CONFIG_STATE_ISICON             0x0131  // 底下的状态选择夹是否显示图标
//#define IGI_CONFIG_STATE_ISSHOW             0x0132  // 状态夹是否显示, 如果为false, 则失去焦点就隐藏起来
//#define IGI_CONFIG_STATE_POS                0x0133  // 状态夹位置, 0=左边, 1=顶边, 2=右边, 3=底边
#define IGI_CONFIG_STATE_FLAGS              0x0134  // 状态夹标识, 记录位置, 是否最大化, 是否浮动, 是否停靠等信息 TOP_STATE_BUTTON 枚举常量里的值, 调用 __query_flags 判断值

#define IGI_CONFIG_CODE_DIRECTION           0x0140  // 代码选择夹的子夹方向, 3=底边, 其他 = 顶边
#define IGI_CONFIG_CODE_ISICON              0x0141  // 代码切换选择夹是否显示图标

//#define IGI_CONFIG_CONTROL_ISSHOW           0x0150  // 组件箱是否显示, 如果为false, 则失去焦点就隐藏起来
//#define IGI_CONFIG_CONTROL_POS              0x0151  // 组件箱位置, 0=左边, 1=顶边, 2=右边, 3=底边
#define IGI_CONFIG_CONTROL_FLAGS            0x0152  // 组件箱标识, 记录位置, 是否最大化, 是否浮动, 是否停靠等信息 TOP_STATE_BUTTON 枚举常量里的值, 调用 __query_flags 判断值

#define IGI_CONFIG_TREE_FNE_ISHOT           0x0161  // 支持库树型框是否支持热点
#define IGI_CONFIG_TREE_PROGRAM_ISHOT       0x0162  // 程序树型框是否支持热点
#define IGI_CONFIG_TREE_CONTROL_ISHOT       0x0163  // 组件箱树型框是否支持热点
#define IGI_CONFIG_TREE_VAR_ISHOT           0x0164  // 变量表树型框是否支持热点
#define IGI_CONFIG_TREE_CALL_ISHOT          0x0165  // 调用表树型框是否支持热点
#define IGI_CONFIG_LIST_FIND1_ISHOT         0x0166  // 搜寻1列表框是否支持热点
#define IGI_CONFIG_LIST_FIND2_ISHOT         0x0167  // 搜寻1列表框树型框是否支持热点
#define IGI_CONFIG_TREE_CONTROL_ORGICON     0x0168  // 组件箱树型框显示原图标

#define IGI_CONFIG_TREE_FNE_HEIGHT          0x0171  // 支持库树型框项目高度
#define IGI_CONFIG_TREE_PROGRAM_HEIGHT      0x0172  // 程序树型框项目高度
#define IGI_CONFIG_TREE_CONTROL_HEIGHT      0x0173  // 组件箱树型框项目高度
#define IGI_CONFIG_TREE_VAR_HEIGHT          0x0174  // 变量表树型框项目高度
#define IGI_CONFIG_TREE_CALL_HEIGHT         0x0175  // 调用表树型框项目高度
#define IGI_CONFIG_LIST_FIND1_HEIGHT        0x0176  // 搜寻1列表框项目高度
#define IGI_CONFIG_LIST_FIND2_HEIGHT        0x0177  // 搜寻1列表框项目高度



// 这里开始是颜色配置相关常量
// 所有颜色配色参数3都可以传递 IGICR_ 开头的常量获取相应值, 默认是返回颜色, 如果不是以下这些值之一, 则返回颜色值
#define IGICR_COLOR         0x0000      // 返回颜色, 这是默认值
#define IGICR_BRUSH         0x0001      // 返回这个颜色对应的gdi画刷
#define IGICR_PEN           0x0002      // 返回这个颜色对应的gdi画笔
#define IGICR_BRUSHDX       0x0003      // 返回这个颜色对应的dx画刷, 这个画刷只能使用视觉库提供的绘画函数使用, 这个不是D2D的画刷句柄
#define IGICR_PENDX         0x0004      // 返回这个颜色对应的dx画笔, 这个画笔只能使用视觉库提供的绘画函数使用, 这个不是D2D的画笔句柄

//! 默认配色 0x0200 - 0x021F
#define IGI_CONFIG_CRDEF_BACK               0x0200  // [DefaultColor], 默认配色, 背景颜色
#define IGI_CONFIG_CRDEF_NORMAL             0x0201  // [DefaultColor], 默认配色, 正常颜色
#define IGI_CONFIG_CRDEF_HOT                0x0202  // [DefaultColor], 默认配色, 热点颜色
#define IGI_CONFIG_CRDEF_DOWN               0x0203  // [DefaultColor], 默认配色, 按下颜色
#define IGI_CONFIG_CRDEF_BAN                0x0204  // [DefaultColor], 默认配色, 禁止颜色
#define IGI_CONFIG_CRDEF_HOVER              0x0205  // [DefaultColor], 默认配色, 进入颜色
#define IGI_CONFIG_CRDEF_FOCUS              0x0206  // [DefaultColor], 默认配色, 焦点颜色
#define IGI_CONFIG_CRDEF_CHECKED            0x0207  // [DefaultColor], 默认配色, 选中颜色
#define IGI_CONFIG_CRDEF_SELECTED           0x0208  // [DefaultColor], 默认配色, 选择颜色
#define IGI_CONFIG_CRDEF_VISTED             0x0209  // [DefaultColor], 默认配色, 
#define IGI_CONFIG_CRDEF_SHADOW             0x020A  // [DefaultColor], 默认配色, 阴影颜色
#define IGI_CONFIG_CRDEF_BORDER             0x020B  // [DefaultColor], 默认配色, 边框颜色
#define IGI_CONFIG_CRDEF_TEXT               0x020C  // [DefaultColor], 默认配色, 文本颜色
#define IGI_CONFIG_CRDEF_TEXTHOT            0x020D  // [DefaultColor], 默认配色, 文本热点颜色
#define IGI_CONFIG_CRDEF_TEXTDOWN           0x020E  // [DefaultColor], 默认配色, 文本按下颜色
#define IGI_CONFIG_CRDEF_TEXTBAN            0x020F  // [DefaultColor], 默认配色, 文本禁止颜色



// 基础颜色 0x0220 - 0x022F
#define IGI_CONFIG_CRBACK                   0x0220  // [Basics], 背景颜色
#define IGI_CONFIG_CRBACKEXTERN             0x0221  // [Basics], 外部的背景颜色, 比如选择夹的子夹
#define IGI_CONFIG_CRSTATEBAR               0x0222  // [Basics], 组件箱/工作夹/状态夹顶部激活的颜色
#define IGI_CONFIG_CRTEXT                   0x0223  // [Basics], 一般文本颜色

// 边框/阴影配色 0x0230 - 0x023F
#define IGI_CONFIG_CRBORDER                 0x0230  // [Basics], 窗口边框颜色
#define IGI_CONFIG_CRBORDER_RUN             0x0231  // [Basics], 窗口边框运行时的颜色
#define IGI_CONFIG_CRBORDER_NF              0x0232  // [Basics], 窗口边框失去焦点时的颜色
#define IGI_CONFIG_CRSHADOW                 0x0238  // [Basics], 窗口阴影颜色
#define IGI_CONFIG_CRSHADOW_RUN             0x0239  // [Basics], 窗口阴影运行时的颜色
#define IGI_CONFIG_CRSHADOW_NF              0x023A  // [Basics], 窗口阴影失去焦点时的颜色

// 菜单项 0x0240 - 0x024F
#define IGI_CONFIG_CRMENUBORDER             0x0240  // [Basics], 菜单项的边框颜色
#define IGI_CONFIG_CRMENUBACK               0x0241  // [Basics], 菜单项的背景颜色, 没有热点, 不是禁止时使用这个颜色绘画
#define IGI_CONFIG_CRMENUHOT                0x0242  // [Basics], 菜单项的热点颜色
#define IGI_CONFIG_CRMENUBAN                0x0243  // [Basics], 菜单项的禁止颜色
#define IGI_CONFIG_CRMENUTEXT               0x0244  // [Basics], 菜单项正常文本颜色
#define IGI_CONFIG_CRMENUTEXTHOT            0x0245  // [Basics], 菜单项热点文本颜色
#define IGI_CONFIG_CRMENUTEXTBAN            0x0246  // [Basics], 菜单项禁止文本颜色

// 显示在标题栏那里的菜单夹 0x0250 - 0x025F
#define IGI_CONFIG_CRMENUBARHOT             0x0250  // [Basics], 顶部菜单夹热点颜色
#define IGI_CONFIG_CRMENUBARTEXT            0x0251  // [Basics], 顶部菜单夹助手菜单的文本颜色
#define IGI_CONFIG_CRMENUBARTEXTHOT         0x0252  // [Basics], 顶部菜单夹助手菜单的文本热点颜色

// 显示在标题栏里黑色块的配色 0x0260 - 0x026F
#define IGI_CONFIG_CRTITLEBACK              0x0260  // [Basics], 顶边黑块部分背景颜色
#define IGI_CONFIG_CRTITLELEFT_ACT          0x0261  // [Basics], 顶部黑块部分左边激活时文本颜色, 这个是源码名或者项目名
#define IGI_CONFIG_CRTITLERIGHT_ACT         0x0262  // [Basics], 顶部黑块部分右边激活时文本颜色, 这个是程序类型, 或者未保存时的星号使用的颜色
#define IGI_CONFIG_CRTITLELEFT              0x0263  // [Basics], 顶部黑块部分左边文本颜色, 这个是源码名或者项目名
#define IGI_CONFIG_CRTITLERIGHT             0x0264  // [Basics], 顶部黑块部分右边文本颜色, 这个是程序类型, 或者未保存时的星号使用的颜色

// 底部状态条的配色 0x0270 - 0x027F
#define IGI_CONFIG_CRTEXTSTATUSBAR          0x0270  // [Basics], 底部状态条的文本颜色
#define IGI_CONFIG_CRTEXTSTATUSBAR_RUN      0x0271  // [Basics], 底部状态条运行时的文本颜色
#define IGI_CONFIG_CRTEXTSTATUSBAR_NF       0x0272  // [Basics], 底部状态条不激活的文本颜色

// 滚动条配色 0x0280 - 0x028F
#define IGI_CONFIG_CRSCROLLBTN1NORRMAL      0x0280  // [Basics], 滚动条按钮1正常颜色, 左边或者顶边的小按钮
#define IGI_CONFIG_CRSCROLLBTN1HOT          0x0281  // [Basics], 滚动条按钮1热点颜色, 左边或者顶边的小按钮
#define IGI_CONFIG_CRSCROLLBTN1DOWN         0x0282  // [Basics], 滚动条按钮1按下颜色, 左边或者顶边的小按钮
#define IGI_CONFIG_CRSCROLLBTN2NORRMAL      0x0283  // [Basics], 滚动条按钮2正常颜色, 右边或者底边的小按钮
#define IGI_CONFIG_CRSCROLLBTN2HOT          0x0284  // [Basics], 滚动条按钮2热点颜色, 右边或者底边的小按钮
#define IGI_CONFIG_CRSCROLLBTN2DOWN         0x0285  // [Basics], 滚动条按钮2按下颜色, 右边或者底边的小按钮
#define IGI_CONFIG_CRSCROLLTHUMBNORRMAL     0x0286  // [Basics], 滚动条滑块颜色
#define IGI_CONFIG_CRSCROLLTHUMBHOT         0x0287  // [Basics], 滚动条滑块热点颜色
#define IGI_CONFIG_CRSCROLLTHUMBDOWN        0x0288  // [Basics], 滚动条滑块按下颜色
#define IGI_CONFIG_CRSCROLLBACKGROUND       0x0289  // [Basics], 滚动条背景颜色

// 分割线颜色 0x0290 - 0x029F
#define IGI_CONFIG_CRLINELEFT               0x0290  // [Basics], 工具条, 超列表头, 等组件的分割线左边颜色, 分割线都是两个像素
#define IGI_CONFIG_CRLINERIGHT              0x0291  // [Basics], 工具条, 超列表头, 等组件的分割线右边颜色, 分割线都是两个像素

// 自绘代码切换选择夹关闭按钮颜色, 0x02A0 - 0x02AF
#define IGI_CONFIG_CODETAB_CRCLOSEHOT               0x02A0  // [CustomCodeTab], 代码切换选择夹关闭按钮热点颜色
#define IGI_CONFIG_CODETAB_CRCLOSEDOWN              0x02A1  // [CustomCodeTab], 代码切换选择夹关闭按钮按下颜色

// 自绘代码切换选择夹文本颜色, 0x02B0 - 0x02BF
#define IGI_CONFIG_CODETAB_CRTEXT_WINDOW            0x02B0  // [CustomCodeTab], 代码切换选择夹, 窗口文本颜色
#define IGI_CONFIG_CODETAB_CRTEXT_ASSEMBLY          0x02B1  // [CustomCodeTab], 代码切换选择夹, 程序集文本颜色
#define IGI_CONFIG_CODETAB_CRTEXT_ASSEMBLY_WINDOW   0x02B2  // [CustomCodeTab], 代码切换选择夹, 窗口程序集文本颜色
#define IGI_CONFIG_CODETAB_CRTEXT_CLASS             0x02B3  // [CustomCodeTab], 代码切换选择夹, 类文本颜色
#define IGI_CONFIG_CODETAB_CRTEXT_GLOBALVAR         0x02B4  // [CustomCodeTab], 代码切换选择夹, 全局变量文本颜色
#define IGI_CONFIG_CODETAB_CRTEXT_STRUCT            0x02B5  // [CustomCodeTab], 代码切换选择夹, 自定义数据类型文本颜色
#define IGI_CONFIG_CODETAB_CRTEXT_DLL               0x02B6  // [CustomCodeTab], 代码切换选择夹, DLL文本颜色
#define IGI_CONFIG_CODETAB_CRTEXT_CONST             0x02B7  // [CustomCodeTab], 代码切换选择夹, 常量文本颜色
#define IGI_CONFIG_CODETAB_CRTEXT_IMAGE             0x02B8  // [CustomCodeTab], 代码切换选择夹, 图片文本颜色
#define IGI_CONFIG_CODETAB_CRTEXT_SOUND             0x02B9  // [CustomCodeTab], 代码切换选择夹, 声音文本颜色

// 自绘代码切换选择夹底下线条颜色, 0x02C0 - 0x02CF
#define IGI_CONFIG_CODETAB_CRLINE_WINDOW            0x02C0  // [CustomCodeTab], 代码切换选择夹, 窗口下边线颜色
#define IGI_CONFIG_CODETAB_CRLINE_ASSEMBLY          0x02C1  // [CustomCodeTab], 代码切换选择夹, 程序集下边线颜色
#define IGI_CONFIG_CODETAB_CRLINE_ASSEMBLY_WINDOW   0x02C2  // [CustomCodeTab], 代码切换选择夹, 窗口程序集下边线颜色
#define IGI_CONFIG_CODETAB_CRLINE_CLASS             0x02C3  // [CustomCodeTab], 代码切换选择夹, 类下边线颜色
#define IGI_CONFIG_CODETAB_CRLINE_GLOBALVAR         0x02C4  // [CustomCodeTab], 代码切换选择夹, 全局变量下边线颜色
#define IGI_CONFIG_CODETAB_CRLINE_STRUCT            0x02C5  // [CustomCodeTab], 代码切换选择夹, 自定义数据类型下边线颜色
#define IGI_CONFIG_CODETAB_CRLINE_DLL               0x02C6  // [CustomCodeTab], 代码切换选择夹, DLL下边线颜色
#define IGI_CONFIG_CODETAB_CRLINE_CONST             0x02C7  // [CustomCodeTab], 代码切换选择夹, 常量下边线颜色
#define IGI_CONFIG_CODETAB_CRLINE_IMAGE             0x02C8  // [CustomCodeTab], 代码切换选择夹, 图片下边线颜色
#define IGI_CONFIG_CODETAB_CRLINE_SOUND             0x02C9  // [CustomCodeTab], 代码切换选择夹, 声音下边线颜色

// 颜色配色需要到 0x03FF







#define IRE_SIZE_TABWORK                    0x0001  // 左边工作夹尺寸被改变, 事件回调的第三个参数收到 IRE_STRUCT_TAB_SIZE 结构指针, 返回是否拦截, 处理后返回值暂时无意义
#define IRE_SIZE_TABSTATE                   0x0002  // 下边状态夹尺寸被改变, 事件回调的第三个参数收到 IRE_STRUCT_TAB_SIZE 结构指针, 返回是否拦截, 处理后返回值暂时无意义
#define IRE_SIZE_TABMDICLIENT               0x0003  // MDIClient尺寸被改变, 事件回调的第三个参数收到 IRE_STRUCT_MDICLIENT_MOVE_EVENT 结构指针, 返回是否拦截, 处理后返回值暂时无意义
#define IRE_TAB_WORK_SIZE                   IRE_SIZE_TABWORK
#define IRE_TAB_STATE_SIZE                  IRE_SIZE_TABSTATE
#define IRE_TAB_MDICLIENT_SIZE              IRE_SIZE_TABMDICLIENT



//#define IRE_E_RUNSTATE_CHANGE               0x0010  // 弃用, 不会触发, 易语言运行按钮状态被改变, 事件回调的第三个参数接收是否启用, 1=运行按钮已启用, 0=运行按钮已禁用
//#define IRE_E_STOPSTATE_CHANGE              0x0011  // 弃用, 不会触发, 易语言停止按钮状态被改变, 事件回调的第三个参数接收是否启用, 1=停止按钮已启用, 0=停止按钮已禁用
//#define IRE_TOOLSTATE_CHANGE_RUN            IRE_E_RUNSTATE_CHANGE
//#define IRE_TOOLSTATE_CHANGE_STOP           IRE_E_STOPSTATE_CHANGE
#define IRE_TOOLSTATE_CHANGE                0x0012  // 易语言标准工具条按钮状态被改变, 事件回调的第三个参数接收 IRE_STRUCT_TOOLSTATE_CHANGE 结构, 返回是否拦截, 处理后返回值暂时无意义


#define IRE_TABSELCHANGED_WORK              0x0020  // 左边工作夹选择被改变, 事件回调的第三个参数收到 IRE_STRUCT_TAB_SELCHANGE 结构指针, 处理前拦截的话不会改变子夹, 处理后返回值暂时无意义
#define IRE_TABSELCHANGED_STATE             0x0021  // 下边状态夹选择被改变, 事件回调的第三个参数收到 IRE_STRUCT_TAB_SELCHANGE 结构指针, 处理前拦截的话不会改变子夹, 处理后返回值暂时无意义
#define IRE_TABSELCHANGED_CODE              0x0022  // 代码切换选择夹选择被改变, 事件回调的第三个参数收到 IRE_STRUCT_TAB_SELCHANGE 结构指针, 处理前拦截的话不会触发尺寸改变事件, 处理后返回值暂时无意义
#define IRE_TAB_WORK_SELCHANGED             IRE_TABSELCHANGED_WORK
#define IRE_TAB_STATE_SELCHANGED            IRE_TABSELCHANGED_STATE
#define IRE_TAB_CODE_SELCHANGED             IRE_TABSELCHANGED_CODE

#define IRE_MDICLIENT_CREATE                0x0023  // MDIClient下的mid窗口被创建事件, 事件回调的第三个参数收到 IRE_STRUCT_MDICREATE 结构指针, 调用前拦截且不正确处理的话窗口不会被创建, 调用前结构中的 hWnd成员为0, 调用后成员为新创建的mdi窗口句柄, 处理后返回值目前无意义
#define IRE_MDICLIENT_DESTROY               0x0024  // MDIClient下的mid窗口被销毁事件, 事件回调的第三个参数收到 被销毁的MDI窗口句柄, 调用前调用后都应该返回0
#define IRE_MDICLIENT_ACTIVATE              0x0025  // MDIClient下的mid窗口被激活事件, 事件回调的第三个参数收到 要激活的 MDI 子窗口的句柄, 处理前返回非0拦截, 处理后返回值无意义
#define IRE_ECODESELCHANGED                 0x0026  // 易语言代码窗口光标改变事件, 事件回调的第三个参数收到 IRE_STRUCT_EIDE_SELCHANGEDINFO 结构指针, 此事件不会触发调用后事件, 返回值无意义


#define IRE_DRAW_BCAKGROUND                 0x0030  // 背景被绘画事件, 返回值是否拦截, 事件回调的第三个参数收到 IRE_STRUCT_BACKGROUND_DRAW 结构指针
#define IRE_BCAKGROUND_DRAW                 IRE_DRAW_BCAKGROUND
#define IRE_DRAW_AUTOHIDE_SHOW              0x0031  // 自动隐藏的窗口被显示, 调用前返回是否拦截, 拦截后不会被显示, 事件回调的第三个参数收到 要显示的窗口句柄
#define IRE_DRAW_AUTOHIDE_HIDE              0x0032  // 自动隐藏的窗口被隐藏, 调用前返回是否拦截, 拦截后不会被隐藏, 事件回调的第三个参数收到 要隐藏的窗口句柄
#define IRE_DRAW_AUTOHIDE_MOVE              0x0033  // 自动隐藏的窗口被移动, 前和后都忽略返回值, 事件回调的第三个参数收到 IRE_STRUCT_AUTOHIDE_MOVE 指针

#define IRE_HOOKPROC_CBT                    0x0050  // cbt钩子事件(WH_CBT), 事件回调第三个参数收到 IRE_STRUCT_HOOKARG 结构指针, 调用前返回是否拦截, 调用后忽略返回值
//#define IRE_HOOKPROC_GETMESSAGE             0x0051  // 获取消息钩子事件(WH_GETMESSAGE), 事件回调第三个参数收到 IRE_STRUCT_HOOKARG 结构指针, 调用前返回是否拦截, 调用后忽略返回值

#define IRE_ICON_GET                        0x0060  // 获取图标事件, 

#define IRE_MENUEXT_MANAGE                  0x0500  // 管理扩展菜单, 不会触发调用后事件, 调用前返回是否拦截, 拦截的话其他插件不会收到这个事件, 第三个参数为0
#define IRE_MENUEXT_CUSTOP                  0x0501  // 自定义扩展菜单, 不会触发调用后事件, 调用前返回是否拦截, 拦截的话其他插件不会收到这个事件, 第三个参数为0








// 调用视觉库功能, 根据功能号调用不同功能, 参数传递请参考对应功能号
// IDC id call, 调用视觉库功能, ID=功能号 C=call , IDC = 功能号调用




//////////////////////////////////////////////////////////////////////////
#define IDC_ICONFIG_BEGIN               0x01D00000  // 配置插件功能号开始
#define IDC_ICONFIG_END                 0x01D0FFFF  // 配置插件功能号结束
#define IDC_ITHEME_BEGIN                0x02D00000  // 主题插件功能号开始
#define IDC_ITHEME_END                  0x02D0FFFF  // 主题插件功能号结束
#define IDC_IEVENT_BEGIN                0x03D00000  // 事件插件功能号开始
#define IDC_IEVENT_END                  0x03D0FFFF  // 事件插件功能号结束
#define IDC_ICONTROL_BEGIN              0x04D00000  // 绘画插件功能号开始
#define IDC_ICONTROL_END                0x04D0FFFF  // 绘画插件功能号结束
#define IDC_IRESOURCE_BEGIN             0x05D00000  // 资源插件功能号开始
#define IDC_IRESOURCE_END               0x05D0FFFF  // 资源插件功能号结束
#define IDC_IMOVE_BEGIN                 0x06D00000  // 移动插件功能号开始
#define IDC_IMOVE_END                   0x06D0FFFF  // 移动插件功能号结束
#define IDC_IETOOLS_BEGIN               0x07D00000  // 助手插件功能号开始
#define IDC_IETOOLS_END                 0x07D0FFFF  // 助手插件功能号结束
#define IDC_IDRAW_BEGIN                 0x08D00000  // 主支持库功能号开始
#define IDC_IDRAW_END                   0x08D0FFFF  // 主支持库功能号结束



#define IDC_IDRAW_STR2RGB               (IDC_IDRAW_BEGIN + 0x0100)  // 将一个文本颜色转换成RGB颜色, wParam = 默认颜色, 调用失败返回这个颜色, lParam指向颜色文本指针
#define IDC_IDRAW_STR2ARGB              (IDC_IDRAW_BEGIN + 0x0101)  // 将一个文本颜色转换成ARGB颜色, wParam = 默认颜色, 调用失败返回这个颜色, lParam指向颜色文本指针
#define IDC_IDRAW_RGB2STR               (IDC_IDRAW_BEGIN + 0x0102)  // 将RGB颜色转换成文本 RGB(1,2,3), wParam=要转换的rgb, lParam = 至少20个WCHAR缓冲区指针, 返回写入字符数
#define IDC_IDRAW_ARGB2STR              (IDC_IDRAW_BEGIN + 0x0103)  // 将ARGB颜色转换成文本 ARGB(255,1,2,3), wParam=要转换的argb, lParam = 至少26个WCHAR缓冲区指针, 返回写入字符数
#define IDC_IDRAW_GETWINDOWDATA         (IDC_IDRAW_BEGIN + 0x0104)  // 获取窗口数据, wParam保留, lParam = 要获取的窗口句柄, 成功返回数据指针, 失败返回0
#define IDC_IDRAW_GETCODEVISIBLERECT    (IDC_IDRAW_BEGIN + 0x0105)  // 获取代码窗口可视的矩形, wParam保留, lParam = 接收矩形的指针(基于易语言主窗口位置)

#define IDC_IDRAW_INLINEHOOK            (IDC_IDRAW_BEGIN + 0x0200)  // 设置inlineHook, wParam = 原函数, lParam = 新函数, 返回原函数地址, 可以直接调用原函数
#define IDC_IDRAW_UNINLINEHOOK          (IDC_IDRAW_BEGIN + 0x0201)  // 取消inlineHook, wParam = 原函数, lParam = 新函数, 返回是否成功
#define IDC_IDRAW_CREATEPOPUPMENU       (IDC_IDRAW_BEGIN + 0x0203)  // 创建有自绘样式的弹出式菜单, 所有加入到这个弹出式菜单里的子菜单都会被自绘

#define IDC_EIDE_GETCODEMANAGE          (IDC_IDRAW_BEGIN + 0x0401)  // 获取IDE代码管理(IEIDE_CodeManage)类指针, 忽略wParam参数, lParma = 代码窗口窗口句柄, 为0则获取当前窗口
#define IDC_EIDE_GETLINESPACING         (IDC_IDRAW_BEGIN + 0x0402)  // 获取单行间隔, 忽略wParam和 lParam参数
#define IDC_EIDE_SETLINESPACING         (IDC_IDRAW_BEGIN + 0x0403)  // 暂未实现, 设置单行间隔, 忽略wParam和 lParam参数
#define IDC_EIDE_GETLINEHEIGHT          (IDC_IDRAW_BEGIN + 0x0404)  // 获取单行高度, 忽略wParam和 lParam参数
#define IDC_EIDE_SETLINEHEIGHT          (IDC_IDRAW_BEGIN + 0x0405)  // 暂未实现, 设置单行高度, 忽略wParam和 lParam参数
#define IDC_EIDE_GETCODEWINDOW          (IDC_IDRAW_BEGIN + 0x0406)  // 获取当前代码框句柄, 如果当前是起始页或者设计器窗口则返回0, 忽略wParam和 lParam参数
#define IDC_EIDE_GETECOLOR              (IDC_IDRAW_BEGIN + 0x0407)  // 获取易语言当前使用的配色, wParam = 缓冲区尺寸, lParam = 缓冲区地址, 返回写入缓冲区字节数, wParam/lParam为0则返回缓冲区需要的尺寸
#define IDC_EIDE_SETECOLOR              (IDC_IDRAW_BEGIN + 0x0408)  // 设置易语言当前使用的配色, wParam为0则lParam是完整的配色文件路径Unicode版, 不为0则lParam指向配色数据, wParam为数据尺寸, lParam = 根据wParam决定是什么值

#define IDC_SCROLL_GETRECT              (IDC_IDRAW_BEGIN + 0x0500)  // 获取滚动条位置, 可以通过这个事件修改边框达到指定绘画滚动条位置, wParam = pfn_GetScrollRect 回调函数, lParam = 窗口数据指针, 成功返回 wParam, 失败返回0
#define IDC_SCROLL_CALCRECT             (IDC_IDRAW_BEGIN + 0x0501)  // 计算滚动条位置, 可以通过这个事件修改窗口非客户区位置, wParam = pfn_CalcScroll 回调函数, lParam = 窗口数据指针, 成功返回 wParam, 失败返回0
#define IDC_SCROLL_DRAW                 (IDC_IDRAW_BEGIN + 0x0502)  // 绘画滚动条, wParam = pfn_DrawScroll 回调函数, lParam = 窗口数据指针, 成功返回 wParam, 失败返回0


#define IDC_IMOVE_GETWINDOWTYPE         (IDC_IMOVE_BEGIN + 0X0001)  // 获取当前MDIClient 窗口下是显示什么窗口, -1=没有窗口, 0=代码区, 1=设计器窗口, 2=起始页, 高位是窗口类型, 0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源
#define IDC_IMOVE_GETCODETYPE           (IDC_IMOVE_BEGIN + 0X0002)  // 获取当前MDIClient 窗口下是显示什么窗口类型, 0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源, 不需要传递参数


#define DRAW_WND_BYCOMMAND      0x00000000L // nPosition 指的是ID
#define DRAW_WND_BYPOSITION     0x00000400L // nPosition 指的是位置

// 常量结束

// 设置dll的路径, 这个是易语言支持库目录下的 iDraw.fne 
inline const wchar_t* iDrawSetDllName(const wchar_t* dllFileName)
{
    static wchar_t fileName[260] = L"iDraw.fne";
    if (!dllFileName || !dllFileName[0]) return fileName;
    size_t len = wcslen(dllFileName);
    if (len >= 259)len = 259;
    memcpy(fileName, dllFileName, len * sizeof(wchar_t));
    fileName[len] = 0;
    return fileName;
}

// 获取dll路径
inline const wchar_t* iDrawGetDllName()
{
    return iDrawSetDllName(0);
}

inline void* iDrawGetAddress(const char* funName)
{
    static HMODULE hModule = 0;
    if (!hModule)
        hModule = LoadLibraryW(iDrawGetDllName());

    if (!hModule)throw; // 加载失败直接抛出异常
    return GetProcAddress(hModule, funName);
}


class ID2DDraw
{
public:
    // 初始化相关
    virtual bool _canvas_init(bool isDxRender) = 0;
    virtual bool _canvas_isDxRender() = 0;
    virtual bool _canvas_destroy(LPCANVAS& hCanvas) = 0;
    virtual LPCANVAS _canvas_create(HWND hWnd, UINT width, UINT height) = 0;
    virtual bool _canvas_uninit() = 0;


    virtual int ExSetLastError(int code) = 0;
    virtual int ExGetLastError() = 0;


    // 绘制相关
    virtual bool _canvas_clear(const LPCANVAS hCanvas, ARGB argb) = 0;
    virtual bool _canvas_SetTarget(const LPCANVAS hCanvas, LPEX_IMAGE bitmap) = 0;
    virtual bool _canvas_ReTarget(const LPCANVAS hCanvas) = 0;
    virtual LPEX_IMAGE _canvas_GetTarget(const LPCANVAS hCanvas) = 0;
    virtual void _canvas_FreeTarget(const LPCANVAS hCanvas, LPEX_IMAGE image) = 0;
    virtual bool _canvas_begindraw(const LPCANVAS hCanvas) = 0;
    // 平移
    virtual bool _canvas_translate(const LPCANVAS hCanvas, float x, float y) = 0;
    // 缩放
    virtual bool _canvas_scale(const LPCANVAS hCanvas, float x, float y, float scaleX, float scaleY) = 0;
    // 旋转
    virtual bool _canvas_rotate(const LPCANVAS hCanvas, float x, float y, float rotate) = 0;
    virtual bool _canvas_enddraw(const LPCANVAS hCanvas) = 0;
    virtual bool _canvas_resize(const LPCANVAS hCanvas, int cx, int cy) = 0;
    virtual HDC _canvas_getdc(const LPCANVAS hCanvas) = 0;
    virtual void _canvas_releasedc(const LPCANVAS hCanvas) = 0;
    virtual bool _canvas_calctextsize(const LPCANVAS hCanvas, LPEX_FONT font, LPCWSTR text, size_t textLen,
                                      DWORD textFormat, float layoutWidth, float layoutHeight, float* pWidth, float* pHeight) = 0;
    virtual bool _canvas_getsize(const LPCANVAS hCanvas, UINT* width, UINT* height) = 0;
    // 绘制简单图形
    virtual bool _canvas_drawRectangle(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPCRECT_F rcF) = 0;
    virtual bool _canvas_fillRectangle(const LPCANVAS hCanvas, const LPEX_BRUSH hBrush, const LPCRECT_F rcF) = 0;
    virtual bool _canvas_drawEllipse(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPCELLIPSE_F rcF) = 0;
    virtual bool _canvas_fillEllipse(const LPCANVAS hCanvas, const LPEX_BRUSH hBrush, const LPCELLIPSE_F rcF) = 0;
    virtual bool _canvas_drawRoundedRectangle(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPCRECTROUNDED_F rcRounded) = 0;
    virtual bool _canvas_fillRoundedRectangle(const LPCANVAS hCanvas, const LPEX_BRUSH hBrush, const LPCRECTROUNDED_F rcRounded) = 0;
    virtual bool _canvas_drawLine(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPCLINEPOINT_F pt) = 0;


    virtual bool _canvas_fillpath(const LPCANVAS hCanvas, const LPEX_BRUSH hBrush, const LPEX_PATH path) = 0;
    virtual bool _canvas_drawpath(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPEX_PATH path) = 0;



    virtual bool _canvas_drawimage(const LPCANVAS hCanvas, LPEX_IMAGE image, float left, float top, BYTE alpha) = 0;
    virtual bool _canvas_drawimagerect(const LPCANVAS hCanvas, LPEX_IMAGE image, float left, float top, float right, float bottom, BYTE alpha) = 0;
    virtual bool _canvas_drawimagerectrect(const LPCANVAS hCanvas, LPEX_IMAGE image,
                                           float dstLeft, float dstTop, float dstRight, float dstBottom,
                                           float srcLeft, float srcTop, float srcRight, float srcBottom, BYTE alpha) = 0;
    virtual bool _canvas_drawimagegridPadding(const LPCANVAS hCanvas, LPEX_IMAGE image,
                                              float dstLeft, float dstTop, float dstRight, float dstBottom,
                                              float srcLeft, float srcTop, float srcRight, float srcBottom,
                                              float gridPaddingLeft, float gridPaddingTop, float gridPaddingRight, float gridPaddingBottom,
                                              DWORD dwFlags, BYTE alpha) = 0;







    virtual bool _canvas_drawtext(const LPCANVAS hCanvas, LPEX_FONT font, LPEX_BRUSH hBrush, LPCWSTR text, size_t textLen, DWORD textFormat, const RECT_F* rc) = 0;

    virtual bool _canvas_drawtextex(const LPCANVAS hCanvas, LPEX_FONT font, LPEX_BRUSH hBrush,
                                    LPCWSTR text, size_t textLen, DWORD textFormat, const RECT_F* rc,
                                    DWORD iGlowsize, ARGB crShadow, LPDRAWTEXTPARAMS lParam, const RECT_F* prclayout, RECT_F* outrcText) = 0;

    virtual bool _canvas_alphablend(const LPCANVAS hCanvas,
                                    float xoriginDest, float yoriginDest, float wDest, float hDest, const LPCANVAS sCanvas,
                                    float xoriginSrc, float yoriginSrc, float wSrc, float hSrc, BYTE Alpha) = 0;
    virtual bool _canvas_bitblt(LPCANVAS hCanvas, int x, int y, int cx, int cy, const LPCANVAS sCanvas, int x1, int y1, DWORD rop) = 0;
    virtual bool _canvas_flush(const LPCANVAS hCanvas) = 0;
    virtual bool _canvas_setrgn(const LPCANVAS hCanvas, float left, float top, float right, float bottom, int mode = 0) = 0;
    virtual bool _canvas_setClip(const LPCANVAS hCanvas, float left, float top, float right, float bottom, int mode = 0) = 0;
    virtual bool _canvas_resetClip(const LPCANVAS hCanvas) = 0;

    // 设置抗锯齿模式, 0=关闭抗锯齿, 1=开启
    // 此函数调与 _canvas_SetSmoothingMode, 功能一模一样, 多加一个是为了和d2d名字一样
    virtual bool _canvas_SetAntialiasMode(const LPCANVAS hCanvas, int mode, int* oldMode = 0) = 0;

    // 设置抗锯齿模式, 0=关闭抗锯齿, 1=开启
    // 此函数调用 _canvas_SetAntialiasMode 实现, 功能一模一样, 多加一个是为了和gdi+名字一样
    virtual bool _canvas_SetSmoothingMode(const LPCANVAS hCanvas, int mode, int* oldMode = 0) = 0;

    // 设置字体抗锯齿模式, 0=默认, 1=ClearType抗锯齿(最高质量), 2=灰度抗锯齿, 3=不使用抗锯齿
    // 此函数调与 _canvas_SetTextRenderingHint, 功能一模一样, 多加一个是为了和d2d名字一样
    virtual bool _canvas_SetTextAntialiasMode(const LPCANVAS hCanvas, int mode, int* oldMode = 0) = 0;
    // 设置字体抗锯齿模式, 0=默认, 1=ClearType抗锯齿(最高质量), 2=灰度抗锯齿, 3=不使用抗锯齿
    // 此函数调用 _canvas_SetTextAntialiasMode 实现, 功能一模一样, 多加一个是为了和gdi+名字一样
    virtual bool _canvas_SetTextRenderingHint(const LPCANVAS hCanvas, int mode, int* oldMode = 0) = 0;



    // 画刷
    virtual LPEX_BRUSH _brush_create(ARGB color) = 0;
    // 创建线性渐变画刷
    // pt1,pt2 = 起点, 终点坐标
    // color,colorCount = 颜色数组, 颜色数组数量
    // fillMode = 填充模式
    // pRatios, ratiosCount = 渐变比例数组, 从0-1, 数组数量
    virtual LPEX_BRUSH _brush_create_LinearGradient(const POINT_F& pt1, const POINT_F& pt2, ARGB* color, DWORD colorCount, int fillMode = 0, const float* pRatios = 0, DWORD ratiosCount = 0) = 0;
    // 创建放射渐变画刷, 从中心点往外扩散渐变
    // pos = 包含中心点以及向外扩展的长度
    // color,colorCount = 颜色数组, 颜色数组数量
    // fillMode = 填充模式
    // pRatios, ratiosCount = 渐变比例数组, 从0-1, 数组数量
    virtual LPEX_BRUSH _brush_create_RadialGradient(const ELLIPSE_F* pos, ARGB* color, DWORD colorCount, int fillMode = 0, const float* pRatios = 0, DWORD ratiosCount = 0) = 0;
    virtual bool _brush_destroy(LPEX_BRUSH& pBrush) = 0;

    virtual LPEX_PEN _pen_create(ARGB color, float width = 1.0f, int style = 0, const float* dashes = 0, int dashesCount = 0) = 0;
    virtual bool _pen_destroy(LPEX_PEN& hPen) = 0;
    virtual bool _pen_setcolor(LPEX_PEN pen, ARGB color) = 0;
    virtual DWORD _pen_getcolor(LPEX_PEN pen) = 0;

    virtual bool _brush_setcolor(LPEX_BRUSH brush, ARGB color) = 0;
    virtual DWORD _brush_getcolor(LPEX_BRUSH brush) = 0;

#ifndef __USEXP // 不是xp, 使用D2D
    virtual bool _brush_setstartpoint(LPEX_BRUSH brush, float x, float y) = 0;
    virtual bool _brush_setendpoint(LPEX_BRUSH brush, float x, float y) = 0;
    virtual bool _brush_getstartpoint(LPEX_BRUSH brush, float* x, float* y) = 0;
    virtual bool _brush_getendpoint(LPEX_BRUSH brush, float* x, float* y) = 0;
    virtual bool _brush_setcenter(LPEX_BRUSH brush, float x, float y) = 0;
    virtual bool _brush_SetGradientOriginOffset(LPEX_BRUSH brush, float x, float y) = 0;
    virtual bool _brush_SetRadiusX(LPEX_BRUSH brush, float x) = 0;
    virtual bool _brush_SetRadiusY(LPEX_BRUSH brush, float x) = 0;
    virtual bool _brush_GetCenter(LPEX_BRUSH brush, float* x, float* y) = 0;
    virtual bool _brush_GetGradientOriginOffset(LPEX_BRUSH brush, float* x, float* y) = 0;
    virtual float _brush_GetRadiusY(LPEX_BRUSH brush) = 0;
#endif

    // 路径
    virtual LPEX_PATH _path_create(int fillMode) = 0;
    virtual LPEX_PATH _path_createroundedrectangle(const LPCRECTROUNDED_F rcRounded, int fillMode) = 0;
    virtual LPEX_PATH _path_createrectangle(const LPCRECT_F rcRounded, int fillMode) = 0;
    virtual bool _path_destroy(LPEX_PATH& path) = 0;
    virtual bool _path_beginfigure(LPEX_PATH path, const LPCPOINT_F pt) = 0;
    virtual bool _path_endfigure(LPEX_PATH path) = 0;
    virtual bool _path_reset(LPEX_PATH path) = 0;
    virtual bool _path_addarc(LPEX_PATH path, float x1, float y1, float x2, float y2, float radiusX, float radiusY, bool fClockwise) = 0;
    virtual bool _path_addbezier(LPEX_PATH path, const POINT_F& pt1, const POINT_F& pt2, const POINT_F& pt3, const POINT_F& pt4) = 0;
    virtual bool _path_addline(LPEX_PATH path, const POINT_F& pt1, const POINT_F& pt2) = 0;
    virtual bool _path_addrect(LPEX_PATH path, const RECT_F& rc) = 0;
    virtual bool _path_addroundedrect(LPEX_PATH path, const RECT_F& rc, float radiusTopLeft, float radiusTopRight, float radiusBottomLeft, float radiusBottomRight) = 0;
    virtual bool _path_open(LPEX_PATH path) = 0;
    virtual bool _path_close(LPEX_PATH path) = 0;
    // 取路径边界矩形
    virtual bool _path_getbounds(LPEX_PATH path, LPRECT_F rc) = 0;
    // 测试坐标是否在可见路径内
    virtual bool _path_hittest(LPEX_PATH path, const POINT_F& pt) = 0;


    // 图片
    virtual bool _img_destroy(LPEX_IMAGE& img) = 0;
    virtual LPEX_IMAGE _img_create(UINT width, UINT height) = 0;
    virtual LPEX_IMAGE _img_create_frommemory(const void* pData, DWORD len) = 0;
    virtual LPEX_IMAGE _img_create_fromfile(LPCWSTR file) = 0;
    virtual LPEX_IMAGE _img_create_frombitmap(HBITMAP hBitmap, HPALETTE hPalette, bool fPreAlpha) = 0;
    virtual LPEX_IMAGE _img_create_formicon(HICON hIcon) = 0;
    virtual LPEX_IMAGE _img_create_fromstream(IStream* stream) = 0;

    virtual bool _img_getsize(LPEX_IMAGE image, UINT* width, UINT* height) = 0;
    virtual UINT _img_width(LPEX_IMAGE image) = 0;
    virtual UINT _img_height(LPEX_IMAGE image) = 0;
    virtual LPEX_IMAGE _img_copyrect(LPEX_IMAGE image, UINT x, UINT y, UINT width, UINT height) = 0;
    virtual LPEX_IMAGE _img_copy(LPEX_IMAGE image) = 0;

    // 锁定位图, 成功返回true, 失败返回false, 不使用时必须调用 _img_unlock 释放数据
    // x,y,width,height = 左顶宽高, 宽高为0时会自动获取图片大小
    // flags = 1=读, 2=写, 3=读写
    virtual bool _img_lock(LPEX_IMAGE image, UINT x, UINT y, UINT width, UINT height, DWORD flags, EX_IMAGELOCK* lockData) = 0;
    // 
    virtual bool _img_unlock(LPEX_IMAGE image, EX_IMAGELOCK* lockData) = 0;

    // 取图像帧数
    virtual UINT _img_getframecount(LPEX_IMAGE image) = 0;
    // 设置当前活动帧
    virtual bool _img_selectactiveframe(LPEX_IMAGE image, UINT index) = 0;

#ifndef __USEXP
    // 从D2D位图指定左顶宽高获取图片或效果数据, 返回图片数据指针和尺寸, 需要调用 _img_free 释放
    virtual LPBYTE _img_save_frombitmap(ID2D1Image* pBitmap, SIZE_T* pSize, UINT left, UINT top, UINT right, UINT bottom) = 0;
#endif


    // 从绘画句柄里获取图片数据, 返回图片数据指针和尺寸, 需要调用 _img_free 释放
    virtual LPBYTE _img_save_formcanvas(const LPCANVAS hCanvas, SIZE_T* pSize) = 0;
    // 从绘画句柄指定左顶宽高获取图片数据, 返回图片数据指针和尺寸, 需要调用 _img_free 释放
    virtual LPBYTE _img_save_formcanvasex(const LPCANVAS hCanvas, SIZE_T* pSize, UINT left, UINT top, UINT right, UINT bottom) = 0;
    // 从图片句柄获取图片数据, 返回图片数据指针和尺寸, 需要调用 _img_free 释放
    virtual LPBYTE _img_save(LPEX_IMAGE image, SIZE_T* pSize) = 0;
    // 从图片句柄指定左顶宽高获取图片数据, 返回图片数据指针和尺寸, 需要调用 _img_free 释放
    virtual LPBYTE _img_save_rect(LPEX_IMAGE image, SIZE_T* pSize, UINT left, UINT top, UINT right, UINT bottom) = 0;

    virtual void _img_free(LPBYTE ptr) = 0;

    // 把图片转成gdi的位图
    virtual HBITMAP _img_ToGdiBitmap(LPEX_IMAGE image) = 0;



    // 位图
    virtual LPEX_IMAGE _bitmap_create(UINT width, UINT height, bool fGdi) = 0;


    // 字体
    // 从LOGFONTW 里创建字体
    virtual LPEX_FONT _font_create_logfont(const LPLOGFONTW logFont, DWORD flags) = 0;
    // 从字体名中创建字体, 如果参数都为0则创建默认字体
    virtual LPEX_FONT _font_create(LPCWSTR name = 0, LONG lfHeight = 0, FONTSTYLE fontStyle = FONTSTYLE::FontStyleRegular) = 0;
    virtual bool _font_destroy(LPEX_FONT& font) = 0;
    // 字符高度转字体大小, 字符高度一般是 LOGFONT 的 lfHeight, 字体大小一般是易语言的字体大小
    virtual int _font_height2size(int height) = 0;
    // 字体大小转字符高度, 字符高度一般是 LOGFONT 的 lfHeight, 字体大小一般是易语言的字体大小
    virtual int _font_size2height(int size) = 0;


    // 区域

    virtual bool _rgn_destroy(LPEX_RGN& hRgn) = 0;
    virtual LPEX_RGN _rgn_create(float left, float top, float right, float bottom) = 0;
    virtual LPEX_RGN _rgn_create_round(float left, float top, float right, float bottom, float radiusX, float radiusY) = 0;
    virtual LPEX_RGN _rgn_combine(LPEX_RGN hRgnSrc, LPEX_RGN hRgnDst, DWORD nCombineMode, float dstOffsetX, float dstOffsetY) = 0;
    virtual bool _rgn_hittest(LPEX_RGN hRgn, float x, float y) = 0;


    // 重新创建设备
    virtual bool recreate_Device() = 0;

};




class IGetEWindow   // 接口类
{
public:
    virtual HWND IDRAW_CALL GetEWindow() = 0;

    // 获取菜单条窗口句柄, 包含顶部的运行/对齐/菜单条
    virtual HWND IDRAW_CALL GetMenu() = 0;

    // 获取菜单条容器下的菜单条窗口句柄
    virtual HWND IDRAW_CALL GetMenuMenu() = 0;
    // 定位工具条
    virtual HWND IDRAW_CALL GetMenuLocation() = 0;
    // 对齐工具条
    virtual HWND IDRAW_CALL GetMenuAlign() = 0;
    // 标准工具条
    virtual HWND IDRAW_CALL GetMenuStandard() = 0;
    // 定位工具条左边的组合框
    virtual HWND IDRAW_CALL GetMenuComboBox1() = 0;
    // 定位工具条右边的组合框
    virtual HWND IDRAW_CALL GetMenuComboBox2() = 0;
    // 查询指定窗口是否为菜单下的窗口
    virtual bool IDRAW_CALL isMenuWindow(HWND hWnd) = 0;


    // 获取代码区下面的选择夹句柄
    virtual HWND IDRAW_CALL GetCodeTab() = 0;

    // 获取代码框的窗口句柄, 代码框里包含所有打开的标签
    virtual HWND IDRAW_CALL GetMDIClient() = 0;

    // 查询指定窗口是否为MDI下的窗口
    virtual bool IDRAW_CALL isMDIClientWindow(HWND hWnd) = 0;


    //! 选择夹相关命令

    // 获取状态夹的窗口句柄, 状态夹包含 输出/提示选择夹/ 输出框/提示框
    virtual HWND IDRAW_CALL GetState() = 0;

    // 获取状态夹的选择夹句柄, 选择夹夹包含 输出/提示选择夹/ 输出框/提示框
    virtual HWND IDRAW_CALL GetStateTab() = 0;

    // 选择夹的父窗口, 需要填充背景
    virtual HWND IDRAW_CALL GetState32770() = 0;

    // 选择夹的父窗口的父窗口, 需要填充背景
    virtual HWND IDRAW_CALL GetState32770Parent() = 0;

    // 获取状态夹里的输出编辑框句柄
    virtual HWND IDRAW_CALL GetStateTabOutput() = 0;

    // 获取状态夹里的提示编辑框句柄
    virtual HWND IDRAW_CALL GetStateTabTips() = 0;

    // 获取状态夹里的变量表输入编辑框句柄
    virtual HWND IDRAW_CALL GetStateTabVarTableEdit() = 0;

    // 获取调用表树型框
    virtual HWND IDRAW_CALL GetStateTabCallTable() = 0;

    // 变量表堆栈查看组合框
    virtual HWND IDRAW_CALL GetStateStackCombobox() = 0;

    // 变量数据类型, 长度, 比如 数据类型: 字节集 / 长度: 0字节
    virtual HWND IDRAW_CALL GetStateTypeSize() = 0;

    // 获取变量表树型框
    virtual HWND IDRAW_CALL GetStateVarTableTree() = 0;

    // 获取剪辑历史超级列表框
    virtual HWND IDRAW_CALL GetStateTabClip() = 0;

    // 获取搜寻1列表框句柄
    virtual HWND IDRAW_CALL GetStateTabFind1() = 0;

    // 获取搜寻2列表框句柄
    virtual HWND IDRAW_CALL GetStateTabFind2() = 0;

    // 获取监视表超级列表框
    virtual HWND IDRAW_CALL GetStateTabMonitorTable() = 0;

    // 获取变量表写入按钮
    virtual HWND IDRAW_CALL GetStateTabVarTableWriteBtn() = 0;

    // 获取变量表标签, 仅调试中断状态下有效
    virtual HWND IDRAW_CALL GetStateTabVarTableStatic() = 0;

    // 查询指定窗口是否为状态夹下的窗口
    virtual bool IDRAW_CALL isStateWindow(HWND hWnd) = 0;


    //! 工作夹相关命令


    // 获取工作夹的窗口句柄, 包含 支持库/程序/属性 选择夹 支持库/程序 树型框
    virtual HWND IDRAW_CALL GetWork() = 0;

    // 获取工作夹的选择夹窗口句柄, 选择夹的子窗口是 支持库/程序/属性 选择夹 支持库/程序 树型框
    virtual HWND IDRAW_CALL GetWorkTab() = 0;

    // 选择夹的父窗口, 需要填充背景
    virtual HWND IDRAW_CALL GetWork32770() = 0;

    // 选择夹的父窗口的父窗口, 需要填充背景
    virtual HWND IDRAW_CALL GetWork32770Parent() = 0;

    // 获取程序的树型框句柄, id=1019
    virtual HWND IDRAW_CALL GetWorkProgramTree() = 0;

    // 获取支持库的树型框句柄, id=1022
    virtual HWND IDRAW_CALL GetWorkFneTree() = 0;

    // 获取事件组合框, 属性框下面
    virtual HWND IDRAW_CALL GetWorkEventComboBox() = 0;

    // 获取组件组合框, 属性框上面
    virtual HWND IDRAW_CALL GetWorkControlComboBox() = 0;

    // 获取属性框
    virtual HWND IDRAW_CALL GetWorkProperty() = 0;

    // 获取属性框下面的编辑框
    virtual HWND IDRAW_CALL GetWorkEdit() = 0;

    // 查询指定窗口是否为工作夹下的窗口
    virtual bool IDRAW_CALL isWorkWindow(HWND hWnd) = 0;

    //! 组件箱相关命令


    // 获取组件箱的窗口句柄
    virtual HWND IDRAW_CALL GetControl() = 0;
    virtual HWND IDRAW_CALL GetControl32770() = 0;
    virtual HWND IDRAW_CALL GetControl32770Parent() = 0;
    virtual HWND IDRAW_CALL GetControlTree() = 0;

    // 查询指定窗口是否为组件箱下的窗口
    virtual bool IDRAW_CALL isControlWindow(HWND hWnd) = 0;

    // 获取底部状态条
    virtual HWND IDRAW_CALL GetBottomStatusBar() = 0;
    // 获取底部状态条里的向上箭头按钮
    virtual HWND IDRAW_CALL GetBottomStatusBarBtn() = 0;
    // 获取底部状态条里的进度条
    virtual HWND IDRAW_CALL GetBottomStatusBarProgress() = 0;

    // 获取MDI下第一个子窗口, 会排除其他不是易语言创建的窗口
    virtual HWND GetMDIClient1() = 0;
    // 从MDI下第一层窗口里获取所有易语言自带的子窗口
    // hMDIClient1 = MDI下的第一层窗口, 如果这个句柄为0, 则自动获取第一个MDI下的窗口
    virtual bool GetMDIClientChildWindow(HWND hMDIClient1, EWINDOW_MDICHILD& WndChild) = 0;
};


#define IDRAW_GET_PTR_ITERATOR(_name) \
    static __FN_##_name__ _PFN_CALLBACK_ = 0;\
    if(!_PFN_CALLBACK_) _PFN_CALLBACK_ = (__FN_##_name__)iDrawGetAddress(#_name);\
    if (!_PFN_CALLBACK_) {\
        MessageBoxA(((HWND)0), "未能找到\""## #_name ##"\"接口, 程序无法正常启动", #_name ## "未找到", 0);\
        exit(0);\
    }

#define IDRAW_FUNC0(_ret, _name) \
        inline _ret IDRAW_CALL _name(){\
        typedef _ret(IDRAW_CALL* __FN_##_name__)();\
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_();}

#define IDRAW_FUNC1(_ret, _name, _1, _2) \
        inline _ret IDRAW_CALL _name(_1 _2){\
        typedef _ret(IDRAW_CALL* __FN_##_name__)(_1);\
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2);}


#define IDRAW_FUNC2(_ret, _name, _1, _2, _3, _4) \
        inline _ret IDRAW_CALL _name(_1 _2, _3 _4){\
        typedef _ret(IDRAW_CALL * __FN_##_name__)(_1, _3); \
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4);}

#define IDRAW_FUNC3(_ret, _name, _1, _2, _3, _4, _5, _6) \
        inline _ret IDRAW_CALL _name(_1 _2, _3 _4, _5 _6){\
        typedef _ret(IDRAW_CALL * __FN_##_name__)(_1, _3, _5); \
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6);}

#define IDRAW_FUNC4(_ret, _name, _1, _2, _3, _4, _5, _6, _7, _8) \
        inline _ret IDRAW_CALL _name(_1 _2, _3 _4, _5 _6, _7 _8){\
        typedef _ret(IDRAW_CALL * __FN_##_name__)(_1, _3, _5, _7); \
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6, _8);}

#define IDRAW_FUNC5(_ret, _name, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) \
        inline _ret IDRAW_CALL _name(_1 _2, _3 _4, _5 _6, _7 _8, _9 _10){\
        typedef _ret(IDRAW_CALL * __FN_##_name__)(_1, _3, _5, _7, _9); \
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6, _8, _10);}

#define IDRAW_FUNC6(_ret, _name, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) \
        inline _ret IDRAW_CALL _name(_1 _2, _3 _4, _5 _6, _7 _8, _9 _10, _11 _12){\
        typedef _ret(IDRAW_CALL * __FN_##_name__)(_1, _3, _5, _7, _9, _11); \
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6, _8, _10, _12);}

#define IDRAW_FUNC7(_ret, _name, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) \
        inline _ret IDRAW_CALL _name(_1 _2, _3 _4, _5 _6, _7 _8, _9 _10, _11 _12, _13 _14){\
        typedef _ret(IDRAW_CALL * __FN_##_name__)(_1, _3, _5, _7, _9, _11, _13); \
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6, _8, _10, _12, _14);}

#define IDRAW_FUNC8(_ret, _name, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16) \
        inline _ret IDRAW_CALL _name(_1 _2, _3 _4, _5 _6, _7 _8, _9 _10, _11 _12, _13 _14, _15 _16){\
        typedef _ret(IDRAW_CALL * __FN_##_name__)(_1, _3, _5, _7, _9, _11, _13, _15); \
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6, _8, _10, _12, _14, _16);}

#define IDRAW_FUNC9(_ret, _name, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18) \
        inline _ret IDRAW_CALL _name(_1 _2, _3 _4, _5 _6, _7 _8, _9 _10, _11 _12, _13 _14, _15 _16, _17 _18){\
        typedef _ret(IDRAW_CALL * __FN_##_name__)(_1, _3, _5, _7, _9, _11, _13, _15, _17); \
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6, _8, _10, _12, _14, _16, _18);}

// API接口开始


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      调用视觉库提供的功能
// 
// 参数:
//      参数1: 功能编号, IDC_ 开头常量
//      参数2: 根据功能传递参数
//      参数3: 根据功能传递参数
// 
// 返回值:
//      参考 IDC_ 开头常量的说明
IDRAW_FUNC3(int, iDraw_IDCall, int, nCode, WPARAM, wParam, LPARAM, lParam)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      调用视觉库提供的功能
// 
// 参数:
//      参数1: 功能编号
//      参数2: 根据功能传递参数
//      参数3: 根据功能传递参数
// 
// 返回值:
//      获取到的值
IDRAW_FUNC3(int, iDraw_PluginFunction, int, nCode, WPARAM, wParam, LPARAM, lParam)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取当前支持库的版本号
// 
// 参数:
//      参数1: 接收主版本号指针, 可以为0
//      参数2: 接收次版本号指针, 可以为0
//      参数3: 接收编译版本号指针, 可以为0
// 
// 返回值:
//      返回版本号unicode文本指针
IDRAW_FUNC3(LPCWSTR, iDraw_GetVersion, int*, pMajor, int*, pMinor, int*, pBuild)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取当前支持库的版本号
// 
// 参数:
//      参数1: 接收主版本号指针, 可以为0
//      参数2: 接收次版本号指针, 可以为0
//      参数3: 接收编译版本号指针, 可以为0
// 
// 返回值:
//      返回版本号文本指针
IDRAW_FUNC3(LPCSTR, iDraw_GetVersionA, int*, pMajor, int*, pMinor, int*, pBuild)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取当前易语言的版本号
// 
// 参数:
//      无参数
// 
// 返回值:
//      获取到的值, 5.93 返回593, 5.11 返回511, 5.92有两个版本, 会返回592和5922
IDRAW_FUNC0(int, iDraw_GetEVersion)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取从支持库里申请内存, 内存是从 iDraw.fne 这个模块里申请的, 内部调用的是 malloc
// 
// 参数:
//      参数1: 申请的尺寸
// 
// 返回值:
//      申请的内存地址, 出错返回0, 申请失败不会抛出异常
IDRAW_FUNC1(void*, iDraw_malloc, int, size)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      释放 iDraw_malloc 申请的内存, 这里释放的是 iDraw.fne 这个模块的内存, 内部调用的是 free
//      如果由其他模块去释放 iDraw_malloc 申请的内存, 一般会有一个堆不相同的异常
// 
// 参数:
//      参数1: iDraw_malloc 返回的地址
// 
// 返回值:
//      无返回值
IDRAW_FUNC1(void, iDraw_free, void*, ptr)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取xml指定节点, 返回节点指针, 所有读写操作都需要节点指针
// 
// 参数:
//      参数1: xml节点名
//      参数2: 节点值文本, 可以为0
//      参数3: 节点不存在时是否创建该节点, 可以为0
//      参数4: 传递一个指针接收返回的节点是否是新创建的, 可以为0
// 
// 返回值:
//      获取到的值
IDRAW_FUNC4(ICTinyXml2_XMLNode, iDraw_Xml_GetNode, LPCWSTR, lpszNodeName, LPCWSTR, lpszNodeValue, BOOL, isCreate, BOOL*, lpIsNewNode)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取xml指定节点下的子节点, 返回节点指针, 所有读写操作都需要节点指针
// 
// 参数:
//      参数1: xml节点, 如果这个值为0, 则功能与 iDraw_Xml_GetNode() 一样
//      参数2: 要获取的子节点节点名, 如果为空则调用失败
//      参数3: 节点值文本, 可以为0
//      参数4: 节点不存在时是否创建该节点, 可以为0
//      参数5: 传递一个指针接收返回的节点是否是新创建的, 可以为0
// 
// 返回值:
//      获取到的值
IDRAW_FUNC5(ICTinyXml2_XMLNode, iDraw_Xml_GetChildNode, ICTinyXml2_XMLNode, nodeParent, LPCWSTR, lpszNodeName, LPCWSTR, lpszNodeValue, BOOL, isCreate, BOOL*, lpIsNewNode)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      添加子节点到某个节点下, 如果被添加的节点已经存在, 则不会添加, 返回节点指针
// 
// 参数:
//      参数1: xml节点指针
//      参数2: 要插入的xml节点名
//      参数3: 要插入的xml节点值, 可以为0
// 
// 返回值:
//      获取到的值
IDRAW_FUNC3(ICTinyXml2_XMLNode, iDraw_Xml_InsertChildNode, ICTinyXml2_XMLNode, pNode, LPCWSTR, lpszNodeName, LPCWSTR, pszNodeValue)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      设置指定节点属性值
// 
// 参数:
//      参数1: xml节点名
//      参数2: 要设置的属性名
//      参数3: 要设置的属性值
// 
// 返回值:
//      获取到的值
IDRAW_FUNC3(bool, iDraw_Xml_SetAttrValue, ICTinyXml2_XMLNode, node, LPCWSTR, lpszAttrName, LPCWSTR, lpszAttrValue)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取指定节点属性值, 返回属性值文本指针
// 
// 参数:
//      参数1: xml节点名
//      参数2: 要获取的属性名
//      参数3: 要获取的这个属性是否存在, 可以为0, 如果属性不存在, 这个值为0, 存在则为1
// 
// 返回值:
//      获取到的值
IDRAW_FUNC3(LPCWSTR, iDraw_Xml_GetAttrValue, ICTinyXml2_XMLNode, node, LPCWSTR, lpszAttrName, BOOL*, isAttribute)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      设置指定节点值
// 
// 参数:
//      参数1: xml节点指针
//      参数2: 要设置的节点值
// 
// 返回值:
//      获取到的值
IDRAW_FUNC2(bool, iDraw_Xml_SetNodeValue, ICTinyXml2_XMLNode, node, LPCWSTR, lpszNodeValue)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      保存xml到磁盘
// 
// 参数:
//      无参数
// 
// 返回值:
//      获取到的值
IDRAW_FUNC0(bool, iDraw_Xml_SaveFile)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取指定节点值, 返回节点值文本指针
// 
// 参数:
//      参数1: xml节点指针
// 
// 返回值:
//      获取到的值
IDRAW_FUNC1(LPCWSTR, iDraw_Xml_GetNodeValue, ICTinyXml2_XMLNode, node)




//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化指定窗口, 并设置一些默认的绘画信息, 内部不会对其进行重画
//      支持库所有子类化都是通过这个接口进行二次封装
// 参数:
//      参数1: 需要子类化的窗口句柄
//      参数2: 接收子类化消息回调的函数, 有5个参数, 函数调用约定为 __stdcall, 返回值为 LRESULT 类型
//              前4个参数与普通子类化相同, 最后多的那个参数就是调用这个接口的返回值
//      参数3: 是否需要删除边框, 为true则把边框相关的样式都删除
//      参数4: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
// 返回值:
//      返回窗口数据, 可以调用 iDraw_SetLong() 对窗口数据进行修改
//      如果需要自己接收消息, 请调用 iDraw_SetLong(窗口数据, IGL_WNDPROC, 回调函数), 可以参考 IGL_WNDPROC 常量说明
IDRAW_FUNC4(LPOBJSTRUCT, iDraw_SubclassWindow, HWND, hWnd, pfn_SubClassWndProc, proc, bool, reMoveBorder, bool, isSetBack)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      把窗口的消息转到重画窗口的过程去处理
// 参数:
//      参数1: 需要重画的窗口句柄
//      参数2: 需要处理的消息
//      参数3: 参数1
//      参数4: 参数2
//      参数5: 参数2
//      参数6: 窗口原来处理消息的回调地址
//      参数7: 接收处理完的消息返回值
// 返回值:
//      返回是否处理了这条消息
IDRAW_FUNC6(bool, iDraw_DrawWindowProc, HWND, hWnd, UINT, message, WPARAM, wParam, LPARAM, lParam, WNDPROC, oldProc, LRESULT*, pRet)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取默认参数
// 参数:
//      参数1: 需要重画的窗口句柄, 为0则在返回后自己给窗口句柄赋值
//      参数2: DRAW_STRUCT_WINDOW_ARGUMENT 结构指针, 内部把默认参数填充到这个结构里
// 返回值:
//      返回保存窗口信息的结构指针, 组件的各种配色信息都在这个结构中保存
IDRAW_FUNC2(bool, iDraw_DrawGetDefArg, HWND, hWnd, LPIDRAW_STRUCT_WINDOW_ARGUMENT, arg)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      把一个顶层窗口重画成与视觉库相同的视觉效果
// 参数:
//      参数1: 需要重画的窗口句柄
//      参数2: IDRAW_STRUCT_WINDOW_ARGUMENT 结构指针, 为0则使用与当前主题匹配的配色信息
// 返回值:
//      返回保存窗口信息的结构指针, 组件的各种配色信息都在这个结构中保存
IDRAW_FUNC2(LPOBJSTRUCT, iDraw_DrawWindow, HWND, hWnd, LPIDRAW_STRUCT_WINDOW_ARGUMENT, arg)



//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建弹出式窗口, 这个窗口和视觉库有相同的视觉效果
// 参数:
//      参数1: 窗口的扩展样式, 详情请参考 CreateWindowEx 的说明
//      参数2: 窗口的标题, 为0则没有标题, unicode文本
//      参数3: 窗口的样式, 详情请参考 CreateWindowEx 的说明
//      参数4: 窗口在屏幕的左边位置
//      参数5: 窗口在屏幕的顶边位置
//      参数6: 窗口宽度
//      参数7: 窗口高度
//      参数8: 父窗口, 如果为0, 则没有父窗口, 如果有则创建的窗口始终在这个窗口之上
//      参数9: IDRAW_STRUCT_WINDOW_ARGUMENT 结构指针, 为0则使用与当前主题匹配的配色信息
// 返回值:
//      返回保存窗口信息的结构指针, 组件的各种配色信息都在这个结构中保存
IDRAW_FUNC9(LPOBJSTRUCT, iDraw_CreateWindow, DWORD, dwExStyle, LPCWSTR, lpWindowName, DWORD, dwStyle, int, x, int, y, int, cx, int, cy, HWND, hWndParent, LPIDRAW_STRUCT_WINDOW_ARGUMENT, arg)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      挂接窗口绘画事件, 只有调用 iDraw_SubclassWindowEx, 或者 iDraw_SubclassWindow 的窗口才有效
//      当窗口在绘画 系统图标, 标题, 系统按时, 会触发事件
// 参数:
//      参数1: 有调用过 iDraw_SubclassWindowEx 的窗口句柄, 或者 iDraw_SubclassWindow 返回的窗口句柄
//      参数2: 接收绘画回调的函数, 函数原型 = typedef bool(CALLBACK* PFN_SysDraw)(LPDRAW_PAINTSTRUCT dps);
//      参数3: 传递到回调函数里的参数
// 返回值:
//      返回是否成功
IDRAW_FUNC3(bool, iDraw_Event_Draw, HWND, hWnd, PFN_SysDraw, pfn, LPARAM, param)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      挂接系统按钮点击事件, 只有调用 iDraw_SubclassWindowEx, 或者 iDraw_SubclassWindow 的窗口才有效
// 参数:
//      参数1: 有调用过 iDraw_SubclassWindowEx 的窗口句柄, 或者 iDraw_SubclassWindow 返回的窗口句柄
//      参数2: 接收绘画回调的函数, 函数原型 = typedef bool(CALLBACK* PFN_SysBtnClick)(int id, BOOL isRButton, LPARAM param);
//      参数3: 传递到回调函数里的参数
// 返回值:
//      返回是否成功
IDRAW_FUNC3(bool, iDraw_Event_SysButtonClick, HWND, hWnd, PFN_SysBtnClick, pfn, LPARAM, param)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      添加系统按钮, 系统按钮索引是从右往左, 只有调用 iDraw_SubclassWindowEx, 或者 iDraw_SubclassWindow 的窗口才有效
// 参数:
//      参数1: 有调用过 iDraw_SubclassWindowEx 的窗口句柄, 或者 iDraw_SubclassWindow 返回的窗口句柄
//      参数2: 按钮的ID, 如果有相同ID, 则替换数值
//      参数3: 按钮数值
// 返回值:
//      返回新增加按钮的索引, 系统按钮索引是从右往左
IDRAW_FUNC3(int, iDraw_AddSysButton, HWND, hWnd, int, id, LPARAM, param)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      删除系统按钮, 系统按钮索引是从右往左, 只有调用 iDraw_SubclassWindowEx, 或者 iDraw_SubclassWindow 的窗口才有效
// 参数:
//      参数1: 有调用过 iDraw_SubclassWindowEx 的窗口句柄, 或者 iDraw_SubclassWindow 返回的窗口句柄
//      参数2: 要删除的按钮ID
//      参数3: 指明参数2是位置还是ID, DRAW_WND_ 开头常量
// 返回值:
//      返回是否删除成功
IDRAW_FUNC3(bool, iDraw_DelSysButton, HWND, hWnd, int, nPosition, int, flags)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      插入系统按钮, 系统按钮索引是从右往左, 只有调用 iDraw_SubclassWindowEx, 或者 iDraw_SubclassWindow 的窗口才有效
// 参数:
//      参数1: 有调用过 iDraw_SubclassWindowEx 的窗口句柄, 或者 iDraw_SubclassWindow 返回的窗口句柄
//      参数2: 要插入的位置, -1则插入到最后(最左边), 0则插入到第一个(最右边)
//      参数3: 要插入的按钮ID
//      参数4: 按钮数值
//      参数5: 指明参数2是位置还是ID, DRAW_WND_ 开头常量
// 返回值:
//      返回插入的位置
IDRAW_FUNC5(int, iDraw_InsSysButton, HWND, hWnd, int, nPosition, int, id, LPARAM, param, int, flags)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      设置系统按钮项目数值, 返回设置前的项目数值
// 参数:
//      参数1: 有调用过 iDraw_SubclassWindowEx 的窗口句柄, 或者 iDraw_SubclassWindow 返回的窗口句柄
//      参数2: 要设置的按钮ID或位置
//      参数3: 按钮数值
//      参数4: 指明参数2是位置还是ID, DRAW_WND_ 开头常量
// 返回值:
//      返回设置前的项目数值
IDRAW_FUNC4(LPARAM, iDraw_SetButtonParam, HWND, hWnd, int, nPosition, LPARAM, param, int, flags)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取系统按钮项目数值
// 参数:
//      参数1: 有调用过 iDraw_SubclassWindowEx 的窗口句柄, 或者 iDraw_SubclassWindow 返回的窗口句柄
//      参数2: 要获取的按钮ID或位置
//      参数3: 指明参数2是位置还是ID, DRAW_WND_ 开头常量
// 返回值:
//      返回按钮的项目数值
IDRAW_FUNC3(LPARAM, iDraw_GetButtonParam, HWND, hWnd, int, nPosition, int, flags)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      调用原回调过程
// 参数:
//      参数1: 窗口句柄, 消息回调里的第一个参数
//      参数2: 消息值, 消息回调里的第二个参数
//      参数3: 参数1, 消息回调里的第三个参数
//      参数4: 参数2, 消息回调里的第四个参数
//      参数5: 组件数据, 消息回调里的第五个参数
// 返回值:
//      返回系统默认处理的返回值
IDRAW_FUNC5(LRESULT, iDraw_CallWindowProc, HWND, hWnd, UINT, message, WPARAM, wParam, LPARAM, lParam, LPOBJSTRUCT, pData)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画按钮, 按钮的类名为 Button
// 参数:
//      参数1: 按钮的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
// 返回值:
//      返回窗口数据, 可以调用 iDraw_SetLong() 对窗口数据进行修改
//      如果需要自己接收消息, 请调用 iDraw_SetLong(窗口数据, IGL_WNDPROC, 回调函数), 可以参考 IGL_WNDPROC 常量说明
IDRAW_FUNC4(LPOBJSTRUCT, iDraw_Subclass_Button, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画编辑框, 编辑框的类名为 Edit
// 参数:
//      参数1: 编辑框的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
//      参数5: 是否重画滚动条
// 返回值:
//      返回窗口数据, 可以调用 iDraw_SetLong() 对窗口数据进行修改
//      如果需要自己接收消息, 请调用 iDraw_SetLong(窗口数据, IGL_WNDPROC, 回调函数), 可以参考 IGL_WNDPROC 常量说明
IDRAW_FUNC5(LPOBJSTRUCT, iDraw_Subclass_Edit, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle, bool, isScroll)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画标签, 标签的类名为 static
// 参数:
//      参数1: 标签的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
// 返回值:
//      返回窗口数据, 可以调用 iDraw_SetLong() 对窗口数据进行修改
//      如果需要自己接收消息, 请调用 iDraw_SetLong(窗口数据, IGL_WNDPROC, 回调函数), 可以参考 IGL_WNDPROC 常量说明
IDRAW_FUNC4(LPOBJSTRUCT, iDraw_Subclass_Static, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画状态条, 状态条的类名为 msctls_statusbar32
// 参数:
//      参数1: 状态条的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
// 返回值:
//      返回窗口数据, 可以调用 iDraw_SetLong() 对窗口数据进行修改
//      如果需要自己接收消息, 请调用 iDraw_SetLong(窗口数据, IGL_WNDPROC, 回调函数), 可以参考 IGL_WNDPROC 常量说明
IDRAW_FUNC4(LPOBJSTRUCT, iDraw_Subclass_StatusBar, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画树型框, 树型框的类名为 SysTreeView32
// 参数:
//      参数1: 树型框的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
//      参数5: 树型框的项目高度
//      参数6: 是否重画树型框的滚动条
//      参数7: 是否有热点效果
// 返回值:
//      返回窗口数据, 可以调用 iDraw_SetLong() 对窗口数据进行修改
//      如果需要自己接收消息, 请调用 iDraw_SetLong(窗口数据, IGL_WNDPROC, 回调函数), 可以参考 IGL_WNDPROC 常量说明
IDRAW_FUNC7(LPOBJSTRUCT, iDraw_Subclass_TreeView, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle, int, nItemHeight, bool, isScroll, bool, isHot)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画列表框, 列表框的类名为 ListBox
// 参数:
//      参数1: 列表框的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
//      参数5: 列表框的项目高度
//      参数6: 是否重画滚动条
//      参数7: 是否有热点效果
// 返回值:
//      返回窗口数据, 可以调用 iDraw_SetLong() 对窗口数据进行修改
//      如果需要自己接收消息, 请调用 iDraw_SetLong(窗口数据, IGL_WNDPROC, 回调函数), 可以参考 IGL_WNDPROC 常量说明
IDRAW_FUNC7(LPOBJSTRUCT, iDraw_Subclass_ListBox, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle, int, nItemHeight, bool, isScroll, bool, isHot)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画超级列表框, 超级列表框的类名为 SysListView32
// 参数:
//      参数1: 超级列表框的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
//      参数5: 超级列表框的项目高度
//      参数6: 是否重画滚动条
//      参数7: 是否有热点效果
//      参数8: 列表对齐方式
// 返回值:
//      返回窗口数据, 可以调用 iDraw_SetLong() 对窗口数据进行修改
//      如果需要自己接收消息, 请调用 iDraw_SetLong(窗口数据, IGL_WNDPROC, 回调函数), 可以参考 IGL_WNDPROC 常量说明
IDRAW_FUNC8(LPOBJSTRUCT, iDraw_Subclass_ListView, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle, int, nItemHeight, bool, isScroll, bool, isHot, int, fmt)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画工具条, 工具条的类名为 ToolbarWindow32
// 参数:
//      参数1: 工具条的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
// 返回值:
//      返回窗口数据, 可以调用 iDraw_SetLong() 对窗口数据进行修改
//      如果需要自己接收消息, 请调用 iDraw_SetLong(窗口数据, IGL_WNDPROC, 回调函数), 可以参考 IGL_WNDPROC 常量说明
IDRAW_FUNC4(LPOBJSTRUCT, iDraw_Subclass_Tool, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle)


// 函数功能: 
//      子类化并重画选择夹, 选择夹的类名为 SysTabControl32
// 参数:
//      参数1: 选择夹窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
// 返回值:
//      返回窗口数据, 可以调用 iDraw_SetLong() 对窗口数据进行修改
//      如果需要自己接收消息, 请调用 iDraw_SetLong(窗口数据, IGL_WNDPROC, 回调函数), 可以参考 IGL_WNDPROC 常量说明
IDRAW_FUNC4(LPOBJSTRUCT, iDraw_Subclass_Tab, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画调节器, 调节器的类名为 msctls_updown32
// 参数:
//      参数1: 调节器的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
// 返回值:
//      返回窗口数据, 可以调用 iDraw_SetLong() 对窗口数据进行修改
//      如果需要自己接收消息, 请调用 iDraw_SetLong(窗口数据, IGL_WNDPROC, 回调函数), 可以参考 IGL_WNDPROC 常量说明
IDRAW_FUNC4(LPOBJSTRUCT, iDraw_Subclass_msctls_updown32, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画组合框, 组合框的类名为 ComboBox
// 参数:
//      参数1: 组合框的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
//      参数5: 组合框的列表表项高度
// 返回值:
//      返回窗口数据, 可以调用 iDraw_SetLong() 对窗口数据进行修改
//      如果需要自己接收消息, 请调用 iDraw_SetLong(窗口数据, IGL_WNDPROC, 回调函数), 可以参考 IGL_WNDPROC 常量说明
IDRAW_FUNC5(LPOBJSTRUCT, iDraw_Subclass_ComboBox, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle, int, nItemHeight)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      重画指定控件的滚动条, 这个绘画有点问题, 不是所有控件都支持
// 参数:
//      参数1: 需要重画滚动条的窗口句柄
//      参数2: 如果是超级列表框, 则这个参数是超级列表框的表项高度, 超列是特别处理
//      参数3: 如果是树型框, 则为真, 树型框会做特别处理
//      参数4: 是否调用原函数
// 返回值:
//      返回滚动条对象指针
IDRAW_FUNC4(LPVOID, iDraw_DrawScroll, HWND, hWnd, int, nListViewItemHeight, bool, isTreeView, bool, isCallSetScrollInfo)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      初始化绘画信息, 必须是在 WM_PAINT 消息下调用, 配套的函数是 iDraw_EndPaint()
//      会调用 BeginPaint, 会创建内存DC, 返回内存DC
//      把组件设置的字体颜色, 背景画刷, 边框画笔都选入DC
//      如果 不调用此接口, 也可以按一般处理WM_PAINT消息时处理
// 
// 参数:
//      参数1: 子类化的第五个参数, 或者是调用子类化接口的返回值
//      参数2: BEGIN_END_PAINT_STRUCT 结构指针, 内部会把初始化的数据记录到这个结构中
//      参数3: 是否为DX渲染, 如果是DX渲染, 则内部会调用DX的初始化函数
//      参数4: 是否填充背景, 如果为true, 则返回的HDC已经填充好了背景
// 
// 返回值:
//      返回内存DC, DX渲染则返回0
IDRAW_FUNC4(HDC, iDraw_BeginPaint, LPOBJSTRUCT, pData, BEGIN_END_PAINT_STRUCT*, data, bool, isDxRender, bool, isFillBack)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      提交绘画信息, 必须是在 WM_PAINT 消息下调用, 调用此函数前必须已经调用了 iDraw_BeginPaint()
//      把iDraw_BeginPaint() 时创建的内存DC拷贝到窗口上
//      并调用 EndPaint 提交绘画信息
// 
// 参数:
//      参数1: BEGIN_END_PAINT_STRUCT 结构指针, 此函数会清理该结构的数据
// 
// 返回值:
//      返回是否拷贝成功
IDRAW_FUNC1(bool, iDraw_EndPaint, BEGIN_END_PAINT_STRUCT*, data)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取DX绘画对象指针, 一般易语言用不上, 易语言使用 iDraw_ 开头的绘画相关函数
// 
// 参数:
//      无参数
// 
// 返回值:
//      返回dx绘画对象指针
IDRAW_FUNC0(ID2DDraw*, iDraw_GetDxObject)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取初始化时的参数
// 
// 参数:
//      参数1: 提供一个指针, 接收 传入 iDraw_Interface 接口的第一个参数
//      参数2: 提供一个指针, 接收 传入 iDraw_Interface 接口的第二个参数
//      参数3: 提供一个指针, 接收 传入 iDraw_Interface 接口的第三个参数
//      参数4: 提供一个指针, 接收 传入 iDraw_Interface 接口的第四个参数
// 
// 返回值:
//      返回值暂未使用
IDRAW_FUNC4(int, iDraw_InterfaceArgument, LPIDRAW_INFO*, pInfo, IGetEWindow**, eWnd, int*, reserved1, int*, reserved2)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      重画阴影窗口, 注意: 如果绑定了 IRE_DRAW_BCAKGROUND 这个事件, 调用后会触发这个事件, 请不要在这个事件下调用该接口
// 
// 参数:
//      参数1: 是否异步重画, 如果不需要马上绘画请设置为true, 为true则距离最后一次调用20毫秒后开始重画
// 
// 返回值:
//      返回阴影窗口句柄
IDRAW_FUNC1(HWND, iDraw_UpdateShadow, bool, bAsync)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      从文件加载ext, 成功返回资源句柄, 失败返回0
// 
// 参数:
//      参数1: ext的文件路径
// 
// 返回值:
//      返回ext文件的资源句柄, 可以使用 User_GetIcon/User_GetIconDx 这两个函数来获取图标
IDRAW_FUNC1(IDRAW_HRESOURCE, iDraw_LoadExtFromFile, LPCWSTR, pszFile)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      从内存加载ext, 成功返回资源句柄, 失败返回0
// 
// 参数:
//      参数1: ext的文件数据
//      参数2: 文数据尺寸
// 
// 返回值:
//      返回ext文件的资源句柄, 可以使用 User_GetIcon/User_GetIconDx 这两个函数来获取图标
IDRAW_FUNC2(IDRAW_HRESOURCE, iDraw_LoadExtFromMemory, const void*, ptr, int, size)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取默认组件的资源句柄, 传递窗口句柄获取
// 
// 参数:
//      参数1: 从哪个窗口上获取, 现在只有 标准工具条, 程序/支持库/组件箱 树型框这几个有默认图标
// 
// 返回值:
//      返回窗口关联的资源句柄
IDRAW_FUNC1(IDRAW_HRESOURCE, iDraw_GetDefControlIcon, HWND, hWnd)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      取消插件所有绑定窗口资源的回调
// 
// 参数:
//      参数1: 插件唯一标识符
// 
// 返回值:
//      取消绑定成功返回true, 失败返回false
IDRAW_FUNC1(bool, iDraw_UnBindWindow, LPCSTR, guid)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      将资源绑定到窗口上, 下次重画这个窗口时会从这个资源获取图标
// 
// 参数:
//      参数1: 插件唯一标识符
//      参数2: 要绑定的窗口句柄
//      参数3: 加载的资源句柄, 如果这个参数为0, 且最后一个参数为0, 则不会加载设定的图标
//      参数4: 回调函数, 当这个窗口需要加载图标时调用, 为0则使用默认的资源加载, 如果没有默认的资源则不加载
// 
// 返回值:
//      绑定成功返回true, 失败返回false
IDRAW_FUNC4(bool, iDraw_BindWindow, LPCSTR, guid, HWND, hWnd, IDRAW_HRESOURCE, hResource, pfn_GetWindowIcon, callback)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      从窗口句柄获取资源, 如果没有绑定过这个窗口, 则返回0
// 
// 参数:
//      参数1: 插件唯一标识符
//      参数2: 之前绑定的窗口句柄
//      参数3: 接收绑定时传递的回调函数, 可以为0
// 
// 返回值:
//      绑定成功返回true, 失败返回false
IDRAW_FUNC3(IDRAW_HRESOURCE, iDraw_GetResourceFromHwnd, LPCSTR, guid, HWND, hWnd, pfn_GetWindowIcon*, callback)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      从资源句柄里获取图标数据
// 
// 参数:
//      参数1: 资源句柄, 使用 iDraw_LoadExtFromFile/iDraw_LoadExtFromMemory/iDraw_GetDefControlIcon 返回的值
//      参数2: 图标ID
//      参数3: RECT_F结构指针, 接收这个ID在资源中的位置
//      参数4: 要获取的状态, 目前只支持 STATE::HOT, STATE::DOWN, STATE::BAN, STATE::NORMAL 这4个
//      参数5: dpi
// 
// 返回值:
//      返回图标数据, 如果没有图标则返回0
IDRAW_FUNC5(LPWINDOW_ICON_DATA, iDraw_GetIcon, IDRAW_HRESOURCE, hResource, LPCWSTR, pszID, RECT_F*, prc, int, state, UINT, dpi)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      从资源句柄里获取图标数据
// 
// 参数:
//      参数1: 资源句柄, 使用 iDraw_LoadExtFromFile/iDraw_LoadExtFromMemory/iDraw_GetDefControlIcon 返回的值
//      参数2: 图标ID
//      参数3: RECT_F结构指针, 接收这个ID在资源中的位置
//      参数4: 要获取的状态, 目前只支持 STATE::HOT, STATE::DOWN, STATE::BAN, STATE::NORMAL 这4个
//      参数5: dpi
// 
// 返回值:
//      返回图标数据, 如果没有图标则返回0
IDRAW_FUNC5(LPWINDOW_ICON_DATA, iDraw_GetIconDx, IDRAW_HRESOURCE, hResource, LPCWSTR, pszID, RECT_F*, prc, int, state, UINT, dpi)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      从图标数据里获取dx或者gdi+的图片指针, gdi+返回的是 Gdiplus::Bitmap* 指针, dx返回内部使用的图片指针
// 
// 参数:
//      参数1: 图标数据, iDraw_GetIcon 或者 iDraw_MakeResourceReturn 返回的值
//      参数2: 是否返回gdiplus位图句柄, 为true则返回Gdiplus位图句柄, false则返回dx位图句柄
// 
// 返回值:
//      返回位图句柄, 根据第二个参数返回
IDRAW_FUNC2(void*, iDraw_GetImageHandle, LPWINDOW_ICON_DATA, pIconData, bool, isGdiplus)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      初始化插件, 只有初始化后才能往扩展菜单里添加菜单项和绑定资源
// 
// 参数:
//      参数1: 插件唯一标识符
//      参数2: 插件的模块句柄
//      参数3: 插件的插件信息结构指针, 在取消初始化前必须保证这个指针的存活
//      参数4: 是否加入扩展菜单, 如果为false, 则调用 iDraw_MenuExt_Insert 会失败
//      参数5: 与视觉库通讯的回调函数
// 
// 返回值:
//      返回错误码, 0=成功
IDRAW_FUNC5(bool, iDraw_InitPlugin, LPCSTR, guid, HMODULE, hModule, PLUGIN_INFO*, etInfo, pfn_MenuExt_Command, pfnMenuExt_Command, pfn_IDraw_Callback, callback)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      取消初始化插件, 把之前初始化时加入到扩展菜单的菜单项删除
// 
// 参数:
//      参数1: 插件唯一标识符
// 
// 返回值:
//      返回错误码, 0=成功
IDRAW_FUNC1(bool, iDraw_UnInitPlugin, LPCSTR, guid)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      把菜单项插入到扩展菜单里, 菜单点击事件由 iDraw_InitPlugin 里的回调函数接收
// 
// 参数:
//      参数1: 插件唯一标识符
//      参数2: 加入的位置, -1则加入到最后
//      参数3: 标识
//      参数4: 菜单项ID, 如果uFlags包含MF_POPUP 则这个参数是弹出式的菜单句柄
//      参数5: 菜单名
//      参数6: 菜单项目数值
// 
// 返回值:
//      返回错误码, 0=成功
IDRAW_FUNC6(bool, iDraw_MenuExt_Insert, LPCSTR, guid, UINT, uPosition, UINT, uFlags, UINT_PTR, uIDNewItem, LPCWSTR, lpNewItem, LPARAM, param)





//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取子类化返回的指针的数据, 或者是子类化接收消息回调函数里的第五个参数
// 
// 参数:
//      参数1: 子类化返回的指针, 或者是子类化接收消息回调函数里的第五个参数
//      参数2: 数据索引, 具体请参考 IGL_ 开头常量
// 
// 返回值:
//      获取到的值
IDRAW_FUNC2(LONG_PTR, iDraw_GetLong, LPOBJSTRUCT, pData, int, index)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      设置子类化返回的指针的数据, 或者是子类化接收消息回调函数里的第五个参数, 返回设置前的数据
// 
// 参数:
//      参数1: 子类化返回的指针, 或者是子类化接收消息回调函数里的第五个参数
//      参数2: 数据索引, 具体请参考 IGL_ 开头常量
//      参数3: 要设置的数据
// 
// 返回值:
//      返回设置前的值
IDRAW_FUNC3(LONG_PTR, iDraw_SetLong, LPOBJSTRUCT, pData, int, index, LONG_PTR, value)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建画刷, 不允许手动释放, 程序在结束时会释放, 相同颜色的画刷只会创建一次
// 
// 参数:
//      参数1: 需要创建的画刷, RGB颜色
// 
// 返回值:
//      返回创建好的GDI画刷, 调用此函数创建的画刷不允许手动释放
IDRAW_FUNC1(HBRUSH, iDraw_CreateBrush, COLORREF, cr)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建画刷, 不允许手动释放, 程序在结束时会释放, 相同颜色的画刷只会创建一次
// 
// 参数:
//      参数1: 需要创建的画刷, ARGB颜色
// 
// 返回值:
//      返回创建好的画刷, 调用此函数创建的画刷不允许手动释放
IDRAW_FUNC1(LPEX_BRUSH, iDraw_CreateBrushDx, DWORD, cr)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建画笔, 不允许手动释放, 程序在结束时会释放, 相同颜色只会创建一次
// 
// 参数:
//      参数1: 需要创建的画笔, ARGB颜色
// 
// 返回值:
//      返回创建好的GDI画笔, 调用此函数创建的画笔不允许手动释放
IDRAW_FUNC3(HPEN, iDraw_CreatePen, int, style, int, width, COLORREF, cr)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建画笔, 不允许手动释放, 程序在结束时会释放, 相同颜色只会创建一次
// 
// 参数:
//      参数1: 需要创建的画笔, ARGB颜色
// 
// 返回值:
//      返回创建好的GDI画笔, 调用此函数创建的画笔不允许手动释放
IDRAW_FUNC3(LPEX_PEN, iDraw_CreatePenDx, int, style, float, width, DWORD, cr)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建字体, 不允许手动释放, 程序在结束时会释放, 相同颜色只会创建一次
// 
// 参数:
//      参数1: 字体名
//      参数2: 字体大小
//      参数3: 字体样式, 0=一般, 1=粗体, 2=斜体, 4=下划线, 8=删除线, 多个样式需要或起来
//
// 返回值:
//      返回创建好的GDI字体, 调用此函数创建的字体不允许手动释放
IDRAW_FUNC3(HFONT, iDraw_CreateFont, LPCWSTR, name, int, lfHeight, int, style)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建字体, 不允许手动释放, 程序在结束时会释放, 相同颜色只会创建一次
// 
// 参数:
//      参数1: LPLOGFONTW 结构指针
//
// 返回值:
//      返回创建好的GDI字体, 调用此函数创建的字体不允许手动释放
IDRAW_FUNC1(HFONT, iDraw_CreateFontLf, LPLOGFONTW, lf)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建字体, 不允许手动释放, 程序在结束时会释放, 相同颜色只会创建一次
// 
// 参数:
//      参数1: 字体名
//      参数2: 字体大小
//      参数3: 字体样式, 0=一般, 1=粗体, 2=斜体, 4=下划线, 8=删除线, 多个样式需要或起来
//
// 返回值:
//      返回创建好的DX字体, 调用此函数创建的字体不允许手动释放
IDRAW_FUNC3(LPEX_FONT, iDraw_CreateFontDx, LPCWSTR, name, int, lfHeight, int, style)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建字体, 不允许手动释放, 程序在结束时会释放, 相同颜色只会创建一次
// 
// 参数:
//      参数1: LPLOGFONTW 结构指针
//
// 返回值:
//      返回创建好的DX字体, 调用此函数创建的字体不允许手动释放
IDRAW_FUNC1(LPEX_FONT, iDraw_CreateFontLfDx, LPLOGFONTW, lf)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
// 暂时未对外公开
//      通过窗口句柄和消息值获取该消息绑定的回调
// 
// 参数:
//      参数1: 窗口句柄
//      参数2: 注册的消息值
//      参数3: 缓冲区数组指针, 一个成员占用20个字节
//      参数4: 缓冲区数组成员数
// 
// 返回值:
//      返回写入缓冲区成员数
IDRAW_FUNC4(int, iDraw_Event_GetMessageInfo, HWND, hWnd, UINT, message, LPREGISTER_MESSAGE_INFO*, pArr, int, bufSize)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
// 暂时未对外公开
//      通过事件ID获取回调信息
// 
// 参数:
//      参数1: 事件ID
//      参数2: 缓冲区数组指针, 一个成员占用24个字节
//      参数3: 缓冲区数组成员数
// 
// 返回值:
//      返回写入缓冲区成员数
IDRAW_FUNC3(int, iDraw_Event_GetEventInfo, UINT, nEvent, LPREGISTER_EVENT_INFO*, pArr, int, bufSize)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      注册消息回调
// 
// 参数:
//      参数1: 插件唯一标识
//      参数2: 注册哪个窗口的消息, 如果该窗口不存在, 则调用失败
//      参数3: 注册的消息值, 当窗口触发这个消息时会调用回调函数
//      参数4: 处理前的回调函数, 窗口处理这个消息前调用这个回调函数, 为0则取消挂接这个事件
//      参数5: 处理后的回调函数, 窗口已经处理这个消息后调用这个回调函数, 为0则取消挂接这个事件
//          参数4 和 参数5 的回调原型都是相同的
//              需要5个参数, 整数返回值, 参数数量必须匹配, 否则会导致堆栈不平衡从而导致程序崩溃
//              整数型 回调函数(窗口句柄, 消息值, 参数1, 参数2, 窗口数据指针)
//              与子类化接收消息函数一样
// 
// 返回值:
//      返回错误码, 0=成功
IDRAW_FUNC5(int, iDraw_Event_RegisterMessage, LPCSTR, guid, HWND, hWnd, UINT, message, pfn_SubClassWndProc, pfnBefore, pfn_SubClassWndProc, pfnAfter)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      注册事件回调
// 
// 参数:
//      参数1: 唯一标识符
//      参数2: 需要注册的事件, 会传递到回调函数里
//      参数3: 处理前的回调函数参数, 这个参数会在调用 参数4 时传递过去
//      参数4: 处理前的回调函数, 为0则取消挂接这个事件
//      参数5: 处理后的回调函数参数, 这个参数会在调用 参数6 时传递过去
//      参数6: 处理后的回调函数, 为0则取消挂接这个事件
//          参数4 和 参数6 的回调原型都是相同的
//              需要3个参数, 整数返回值, 参数数量必须匹配, 否则会导致堆栈不平衡从而导致程序崩溃
//              整数型 回调函数(事件类型, 回调参数, 事件指针)
// 
// 返回值:
//      返回错误码, 0=成功
IDRAW_FUNC6(int, iDraw_Event_RegisterEvent, LPCSTR, guid, UINT, nEvent, LONG_PTR, paramBefore, pfn_Register_Event, pfnBefore, LONG_PTR, paramAfter, pfn_Register_Event, pfnAfter)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      取消指定插件的所有消息回调
// 
// 参数:
//      参数1: 插件唯一标识符
// 
// 返回值:
//      返回错误码, 0=成功
IDRAW_FUNC1(int, iDraw_UnRegister_Message, LPCSTR, guid)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      取消指定插件的所有事件回调
// 
// 参数:
//      参数1: 插件唯一标识符
// 
// 返回值:
//      返回错误码, 0=成功
IDRAW_FUNC1(int, iDraw_UnRegister_Event, LPCSTR, guid)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取pInfo的信息, 接口程序的第一个参数
// 
// 参数:
//      参数1: 接口程序的第一个参数
//      参数2: 获取的编号, IGI_ 开头常量
//      参数3: 获取的参数, 根据编号传递
// 
// 返回值:
//      获取到的值
IDRAW_FUNC3(int, iDraw_GetInfo, LPIDRAW_INFO, pInfo, int, index, LPARAM, argument)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      设置pInfo的信息, 接口程序的第一个参数, 谨慎设置, 这里的配置是全局使用, 一旦出错就会导致崩溃
// 
// 参数:
//      参数1: 接口程序的第一个参数
//      参数2: 获取的编号, IGI_ 开头常量
//      参数3: 要设置的值
// 
// 返回值:
//      返回设置之前的值
IDRAW_FUNC3(int, iDraw_SetInfo, LPIDRAW_INFO, pInfo, int, index, LONG_PTR, value)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      把函数放到易语言窗口线程里调用
// 
// 参数:
//      参数1: 函数地址, 没有返回值, 必须没有返回值和一个整数型参数, 否则会堆栈不平衡导致崩溃
//      参数2: 传递到函数里的参数
// 
// 返回值:
//      失败返回0, 成功返回非0值
IDRAW_FUNC2(bool, iDraw_EWindowThreadCallback, pfn_EWindowThreadCallback, pfnCallback, LPARAM, param)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      从图片数据里生成资源返回值, 所有指针必须保证存活, 内部不会申请内存来保存传递进来的文本和位图
// 
// 参数:
//      参数1: 图片数据地址
//      参数2: 图片数据大小
//      参数3: 绘制当前项目显示的文本, 不会修改原组件文本, 只是绘画时显示这个文本, 可以为0
//      参数4: 备注文本, 可以为0
//      参数5: 文本ARGB颜色, 可以为0, 不为0则必须是4个成员, 分别是 正常, 热点, 按下, 禁止
//      参数6: 背景ARGB颜色, 可以为0, 不为0则必须是4个成员, 分别是 正常, 热点, 按下, 禁止
// 
// 返回值:
//      返回资源返回值, 返回值不使用时必须调用 iDraw_FreeResourceReturn 来释放
IDRAW_FUNC6(LPWINDOW_ICON_DATA, iDraw_MakeResourceReturn, const void*, pImg, int, size, LPCWSTR, text, LPCWSTR, remarks, DWORD*, crText, DWORD*, crBack)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      释放 iDraw_MakeResourceReturn() 生成的返回值
// 
// 参数:
//      参数1: iDraw_MakeResourceReturn() 的返回值
// 
// 返回值:
//      返回是否释放成功
IDRAW_FUNC1(bool, iDraw_FreeResourceReturn, LPWINDOW_ICON_DATA, pData)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      从内存加载gdi+图片
// 
// 参数:
//      参数1: 图片数据
//      参数2: 图片数据大小
// 
// 返回值:
//      成功返回gdi+图片指针, 失败返回0
IDRAW_FUNC2(void*, iDraw_LoadGpImageFromMemory, const void*, pData, int, size)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      释放从 iDraw_LoadGpImageFromMemory() 返回的值
// 
// 参数:
//      参数1: iDraw_LoadGpImageFromMemory() 返回的值
// 
// 返回值:
//      成功返回true, 失败返回false
IDRAW_FUNC1(bool, iDraw_FreeGpImage, void*, hImg)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      添加按钮到接口工具条中, 成功返回工具条按钮ID, 触发工具条按钮被点击时会把这个ID传递过去
// 
// 参数:
//      参数1: 插件唯一标识符, 触发点击/重画事件时会调用这个插件的回调函数
//      参数2: 按钮的ID, 触发事件时会把这个ID传递过去
//      参数3: 按钮标题, 可以为0, 为0则没有标题, 鼠标移动上去时弹出的提示说明标题, 加粗显示, 这个标题不会显示在工具条上
//      参数4: 按钮说明, 可以为0, 为0则没有说明, 鼠标移动上去时弹出的提示说明
//      参数5: 项目数值, 会跟随点击事件传递过去
//      参数6: 图片数据指针, 这个是显示到工具条上的图标
//      参数7: 图片数据大小
//      参数8: 绘画回调函数, pfn_ToolDrawButton 类型
//              typedef int(IDRAW_CALL* pfn_ToolDrawButton)(LPCANVAS hCanvas, int id, LPARAM param, const RECT_F* prc, STATE state, UINT dpi);
// 返回值:
//      成功返回工具条按钮ID, 失败返回0
IDRAW_FUNC8(int, iDraw_Tool_InsertButton, LPCSTR, guid, int, idCommand, LPCWSTR, lpszTitle, LPCWSTR, lpszTips, LPARAM, param, const void*, pImageData, int, nImageSize, pfn_ToolDrawButton, pfnDraw)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      删除接口工具条的按钮, 只能删除自己插件添加的按钮
// 
// 参数:
//      参数1: 插件唯一标识符, 触发点击/重画事件时会调用这个插件的回调函数
//      参数2: 按钮的ID, 添加时的ID
// 
// 返回值:
//      返回是否成功
IDRAW_FUNC2(bool, iDraw_Tool_DeleteButton, LPCSTR, guid, int, idCommand)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取接口工具条的按钮信息, 只能获取自己插件添加的按钮
// 
// 参数:
//      参数1: 插件唯一标识符
//      参数2: 按钮的ID, 添加时的ID
//      参数3: 为0则不接收, 接收按钮标题
//      参数4: 为0则不接收, 接收按钮说明
//      参数5: 为0则不接收, 接收项目数值
//      参数6: 为0则不接收, 接收图片数据指针
//      参数7: 为0则不接收, 接收图片数据大小
//      参数8: 为0则不接收, 接收绘画回调函数
// 
// 返回值:
//      返回是否成功
IDRAW_FUNC8(bool, iDraw_Tool_GetButtonData, LPCSTR, guid, int, idCommand, LPWSTR*, lpszTitle, LPWSTR*, lpszTips, LPARAM*, param, LPCVOID*, pImageData, int*, nImageSize, pfn_ToolDrawButton*, pfnDraw)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      设置接口工具条的按钮信息, 只能设置自己插件添加的按钮
// 
// 参数:
//      参数1: 插件唯一标识符
//      参数2: 按钮的ID, 添加时的ID
//      参数3: 为0则不设置, 设置按钮标题
//      参数4: 为0则不设置, 设置按钮说明
//      参数5: 为0则不设置, 设置项目数值
//      参数6: 为0则不设置, 设置图片数据指针
//      参数7: 为0则不设置, 设置图片数据大小
//      参数8: 为0则不设置, 设置绘画回调函数
// 
// 返回值:
//      返回是否成功
IDRAW_FUNC8(bool, iDraw_Tool_SetButtonData, LPCSTR, guid, int, idCommand, LPCWSTR, lpszTitle, LPCWSTR, lpszTips, LPARAM, param, const void*, pImageData, int, nImageSize, pfn_ToolDrawButton, pfnDraw)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      调用易语言交互函数, 调用这个函数就相当于调用特殊库的 调用易系统功能() 命令
// 
// 参数:
//      参数1: 功能号, 功能号请参考易语言提供的 lib2.h 文件, NES_, NAS_, NRS_ 开头常量
//      参数2: 请参阅 lib2.h 功能号说明
//      参数3: 请参阅 lib2.h 功能号说明
// 
// 返回值:
//      请参阅 lib2.h 功能号说明
IDRAW_FUNC3(int, iDraw_EIDE_Notify, int, nMsg, DWORD, dwParam1, DWORD, dwParam2)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      调用易语言IDE接口功能, 调用这个函数相当于调用 iDraw_EIDE_Notify(NES_RUN_FUNC, 参数1, 数组[参数2, 参数3]);
// 
// 参数:
//      参数1: 功能号, 功能号请参考易语言提供的 PublicIDEFunctions.h 文件, 
//      参数2: 请参阅 PublicIDEFunctions.h 功能号说明
//      参数3: 请参阅 PublicIDEFunctions.h 功能号说明
// 
// 返回值:
//      如果该功能被处理返回真, 未被处理则返回假
IDRAW_FUNC3(bool, iDraw_EIDE_RunFunctions, int, nMsg, DWORD, dwParam1, DWORD, dwParam2)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      将一个文本颜色转换成RGB颜色
// 
// 参数:
//      参数1: 默认颜色, 调用失败返回这个颜色
//      参数2: 指向颜色文本指针
// 
// 返回值:
//      返回转换好的RGB颜色值
IDRAW_FUNC2(COLORREF, iDraw_STR2RGB, COLORREF, crDef, LPCWSTR, lpszColorText)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      将一个文本颜色转换成ARGB颜色
// 
// 参数:
//      参数1: 默认颜色, 调用失败返回这个颜色
//      参数2: 指向颜色文本指针
// 
// 返回值:
//      返回转换好的ARGB颜色值
IDRAW_FUNC2(ARGB, iDraw_STR2ARGB, ARGB, crDef, LPCWSTR, lpszColorText)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      将RGB颜色转换成文本 RGB(1,2,3)
// 
// 参数:
//      参数1: 要转换的rgb
//      参数2: 至少20个WCHAR缓冲区指针
// 
// 返回值:
//      返回写入字符数
IDRAW_FUNC2(int, iDraw_RGB2STR, COLORREF, crDef, LPWSTR, lpszBuffer)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      将RGB颜色转换成文本 ARGB(255,1,2,3)
// 
// 参数:
//      参数1: 要转换的argb
//      参数2: 至少26个WCHAR缓冲区指针
// 
// 返回值:
//      返回写入字符数
IDRAW_FUNC2(int, iDraw_ARGB2STR, ARGB, crDef, LPWSTR, lpszBuffer)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取窗口数据
// 
// 参数:
//      参数1: 要获取的窗口句柄
// 
// 返回值:
//      成功返回数据指针, 失败返回0
IDRAW_FUNC1(LPOBJSTRUCT, iDraw_GetWindowData, HWND, hWnd)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取代码窗口可视的矩形
// 
// 参数:
//      参数1: 接收矩形的指针(基于易语言主窗口位置)
// 
// 返回值:
//      返回是否获取成功
IDRAW_FUNC1(BOOL, iDraw_GetCodeVisibleRect, RECT*, prc)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      设置InlineHook, 内部提供两种hook方式
// 
// 参数:
//      参数1: 被hook的地址
//      参数2: 接收hook的函数地址
//      参数3: 使用的hook库, 0=detoursHook, 1=MinHook, 目前只提供这两种
// 
// 返回值:
//      返回被hook地址的原地址, 可以直接调用这个地址
IDRAW_FUNC3(LPVOID, iDraw_InlineHook, LPVOID, pOldFun, LPVOID, pNewFun, int, hookMode)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      设置InlineHook, 内部提供两种hook方式
// 
// 参数:
//      参数1: iDraw_InlineHook 的返回值
// 
// 返回值:
//      返回是否取消成功
IDRAW_FUNC1(bool, iDraw_UnInlineHook, LPVOID, pOldFun)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建有自绘样式的弹出式菜单, 所有加入到这个弹出式菜单里的子菜单都会被自绘
// 
// 参数:
//      无参数
// 
// 返回值:
//      返回弹出式菜单句柄
IDRAW_FUNC0(HMENU, iDraw_CreatePopupMenu)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取IDE代码管理(IEIDE_CodeManage)类指针
// 
// 参数:
//      参数1: 代码窗口窗口句柄, 为0则获取当前窗口
// 
// 返回值:
//      返回代码窗口对应的 IEIDE_CodeManage 类指针, 失败返回0
IDRAW_FUNC1(IEIDE_CodeManage*, iDraw_GetCodeManage, HWND, hCode)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取当前代码框句柄, 如果当前是起始页或者设计器窗口则返回0
// 
// 参数:
//      无参数
// 
// 返回值:
//      返回当前代码框句柄, 如果当前是起始页或者设计器窗口则返回0
IDRAW_FUNC0(HWND, iDraw_GetCodeWindow)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取滚动条位置, 可以通过这个事件修改边框达到指定绘画滚动条位置
// 
// 参数:
//      参数1: pfn_GetScrollRect 回调函数
//      参数2: 窗口数据指针
// 
// 返回值:
//      成功返回非0, 失败返回0
IDRAW_FUNC2(BOOL, iDraw_Scroll_GetRect, pfn_GetScrollRect, pfnCallback, LPOBJSTRUCT, pData)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      计算滚动条位置, 可以通过这个事件修改窗口非客户区位置
// 
// 参数:
//      参数1: pfn_CalcScroll 回调函数
//      参数2: 窗口数据指针
// 
// 返回值:
//      成功返回非0, 失败返回0
IDRAW_FUNC2(BOOL, iDraw_Scroll_CalcRect, pfn_CalcScroll, pfnCallback, LPOBJSTRUCT, pData)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      绘画滚动条
// 
// 参数:
//      参数1: pfn_DrawScroll 回调函数
//      参数2: 窗口数据指针
// 
// 返回值:
//      成功返回非0, 失败返回0
IDRAW_FUNC2(BOOL, iDraw_Scroll_Draw, pfn_DrawScroll, pfnCallback, LPOBJSTRUCT, pData)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取当前MDIClient 窗口下是显示什么窗口
//      -1=没有窗口, 0=代码区, 1=设计器窗口, 2=起始页, 高位是窗口类型, 0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源
// 
// 参数:
//      无参数
// 
// 返回值:
//      返回以下值之一: -1=没有窗口, 0=代码区, 1=设计器窗口, 2=起始页, 高位是窗口类型, 0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源
IDRAW_FUNC0(int, iDraw_GetMDIClientType)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取当前MDIClient 窗口下是显示什么窗口类型
//      0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源
// 
// 参数:
//      无参数
// 
// 返回值:
//      返回以下值之一: 0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源
IDRAW_FUNC0(int, iDraw_GetMDIClientCodeType)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取滚动条对象数据
// 
// 参数:
//      参数1: 滚动条对象指针
//      参数2: 接收滚动条数据指针, 调用前需要把 IDRAW_STRUCT_SCROLL_DATA.cbSize 赋值
// 
// 返回值:
//      返回是否成功
IDRAW_FUNC2(BOOL, iDraw_GetScrollData, LPVOID, pScroll, IDRAW_STRUCT_SCROLL_DATA*, ptr)

// API接口结束











// 接口_IDE代码编辑框管理
class IEIDE_CodeManage
{
public:
    // 从窗口句柄中初始化本类
    virtual bool init(HWND hWnd) = 0;
    // 销毁
    virtual void destroy() = 0;
    // 获取光标位置, 位置为表格内部序号
    virtual POINT GetCursorPos() = 0;
    // 设置光标位置, 会把表列号置为0
    virtual void SetCursorPos(POINT pt) = 0;
    // 获取选中信息
    // pStartCol = 接收起始列
    // pEndCol = 接收结束列
    // pNowLine = 接收当前行
    // pNowLine = 接收当前行
    // pSel = 接收选中范围
    virtual void GetSelInfo(int* pStartCol = 0, int* pEndCol = 0, int* pNowLine = 0, POINT* pSel = 0) = 0;

    // 设置选中信息
    // pStartCol = 起始列指针, 为0则不写入
    // pEndCol = 结束列指针, 为0则不写入
    // pNowLine = 当前行指针, 为0则不写入
    // pNowLine = 当前行指针, 为0则不写入
    // pSel = 选中范围指针, 为0则不写入
    virtual void SetSelInfo(int* pStartCol = 0, int* pEndCol = 0, int* pNowLine = 0, POINT* pSel = 0) = 0;

    // 是否选中代码
    virtual bool IsSelCode() = 0;

    // 获取选中行数
    virtual int GetSelLineCount() = 0;

    // 获取选中列数
    virtual int GetSelColumnCount() = 0;

    // 获取行信息
    // lineNumber = 行号
    // flags = 项标识, ETAG
    // pData = 项数据指针, EBase*
    virtual int GetLineInfo(int lineNumber, int* flags, int* pData) = 0;

    // 获取代码类型
    virtual int GetCodeType() = 0;

    // 获取对象指针
    virtual void* GetObjectThis() = 0;

    // 设置对象指针
    virtual void SetObjectThis(void* pThis) = 0;

    // 光标定位
    virtual void CursorLocation(int lineNumber) = 0;

    // 保证显示光标位置
    virtual void EnsurVisibleCursor() = 0;

    // 获取代码文本, 返回的文本需要调用 本类对象的 free() 方法来释放
    // nTableIndex = 表行
    // nTableindexCol = 表列
    virtual LPSTR GetCodeText(int nTableIndex, int nTableindexCol) = 0;

    // 申请内存
    virtual void* alloc(int size) = 0;
    // 释放内存, 只能传递本类对象 alloc() 方法返回的指针, 传递其他指针可能会出问题
    virtual void free(void* p) = 0;

    // 获取代码数据偏移
    // nTableIndex = 表行
    // nTableindexCol = 表列
    // type = 接收类型指针
    // offset = 接收偏移指针
    // pData = 数据指针, EBase*
    virtual bool GetCodeDataOffset(int nTableIndex, int nTableindexCol, int* pType, int* pOffset, int* pData) = 0;

    // 获取光标表位置, 位置为表格序号
    virtual POINT GetCursorTablePos() = 0;

    // 设置光标表位置, 位置为表格序号
    // pt = 表格序号
    virtual void SetCursorTablePos(POINT pt) = 0;

    // 获取当前对象指向的代码框句柄
    virtual HWND GetWindow() = 0;
};










