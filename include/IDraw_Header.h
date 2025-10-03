#pragma once
#include <control/WndBase.h>
#pragma comment(lib,"Comctl32.lib")
#include <e/CGetEWindow.h>
#include <control/CLayered.h>
#include <control/EControlID.h>
#include <assist/CFileRW.h>
#include "iDraw_virtual.h"
#include "iDraw_Event_typedef.h"

#define GUID_INLINE_HOOK                L"{790EFE11-D2FB-49A6-864E-D733038CBEE0}"   // GetProp 调用接口设置hook时记录到易语言窗口中, 释放插件需要到这里释放, INLINEHOOK_CALLBACK, key记录hook返回的地址
#define GUID_SCROLL_CALLBACK_EWINDOW    L"{790EFE11-D2FB-49A6-864E-D733038CBEE1}"   // GetProp 滚动条回调, 释放插件时需要从这里取出指针逐个释放, SCROLL_CALLBACK
#define GUID_TABPARAMS                  L"{9AAA4259-9DDD-4D53-9E74-C9C1CDEAF315}"   // 记录状态夹右边选择夹的子夹项目数值, std::vector<int>*

#define HOOKMODE_DETOURS    0
#define HOOKMODE_MINHOOK    1

struct SCROLL_CALLBACK_SET_DATA
{
    std::unordered_set<pfn_GetScrollRect>* pGet;
    std::unordered_set<pfn_CalcScroll>* pCalc;
    std::unordered_set<pfn_DrawScroll>* pDraw;
};

struct INLINEHOOK_CALLBACK_DATA
{
    LPVOID pOldFun;     // 被hook的地址
    LPVOID pNewFun;     // 接收hook的地址, key是hook的返回值
    int hookMode;       // hook的模式, 0=detoursHook, 1=MinHook
};

typedef std::map<HWND, SCROLL_CALLBACK_SET_DATA> SCROLL_CALLBACK;
typedef std::map<LPVOID, INLINEHOOK_CALLBACK_DATA> INLINEHOOK_CALLBACK;

#ifdef _DEBUG
#include <assert.h>
#else
  #ifndef assert
    #define assert(_a) 
  #endif
#endif

#define IDRAW_FONT              L"微软雅黑"
#define IDRAW_FONT_LENGTH       10
#define __HOOK_MOVE_BREPAINT    1211

#define WINDOW_CAPTION_HEIGHT   30      // 窗口标题高度
#define WINDOW_BORDER_WIDTH     5       // 窗口边框宽度, 这个宽度是可调整窗口大小的宽度
#define WINDOW_STATUS_HEIGHT    20      // 底部状态条的高度
#define WINDOW_TOP_STATE        24      // 可以拖动窗口时顶边是状态条高度, 组件箱有4个按钮
#define WINDOW_CUSTOMTAB_HEIGHT 24      // 自绘选择夹高度, 这个选择夹是切换代码窗口的选择夹
#define WINDOW_AUTO_HEIGHT      24      // 自动隐藏时的窗口高度, 左边右边的话就是宽度
//#define WINDOW_MENU_WIDTH       60      // 菜单宽度, 一个菜单项的宽度
#define WINDOW_SPACING          8       // 区域之间的间隔

#define WINDOW_MIN_MDIWIDTH     200     // MDI窗口最小宽度, 调整工作夹, 组件箱时必须处理
#define WINDOW_MIN_MDIHEIGHT    200     // MDI窗口最小高度, 调整状态夹时必须处理
#define WINDOW_MINTRACKSIZE     135     // 各个窗口最小允许调整的尺寸
#define EVENT_MAX_ITEM          128     // 同一个消息/事件最大支持的回调数, 目前128足够使用了, 这个数量填多少就表示支持这么多插件注册
// 1=提示, 2=输出, 3=调用表, 4=监视表, 5=变量表, 6=搜寻1, 7=搜寻2, 8=剪辑历史9=支持库, 10=程序, 11=属性12 组件箱
#define TAB_PARAM_TIPS          1       // 提示, 状态夹里面的提示子夹的项目数值, 点击根据项目数值判断点击了哪个项目
#define TAB_PARAM_OUTPUT        2       // 输出, 状态夹里面的输出子夹的项目数值, 点击根据项目数值判断点击了哪个项目
#define TAB_PARAM_CALLTABLE     3       // 调用表, 状态夹里面的调用表子夹的项目数值, 点击根据项目数值判断点击了哪个项目
#define TAB_PARAM_MONITORTABLE  4       // 监视表, 状态夹里面的监视表子夹的项目数值, 点击根据项目数值判断点击了哪个项目
#define TAB_PARAM_VARTABLE      5       // 变量表, 状态夹里面的变量表子夹的项目数值, 点击根据项目数值判断点击了哪个项目
#define TAB_PARAM_FIND1         6       // 搜寻1, 状态夹里面的搜寻1子夹的项目数值, 点击根据项目数值判断点击了哪个项目
#define TAB_PARAM_FIND2         7       // 搜寻2, 状态夹里面的搜寻2子夹的项目数值, 点击根据项目数值判断点击了哪个项目
#define TAB_PARAM_CLIP          8       // 剪辑历史, 状态夹里面的剪辑历史子夹的项目数值, 点击根据项目数值判断点击了哪个项目
#define TAB_PARAM_FNE           9       // 支持库, 工作夹里面的支持库子夹的项目数值, 点击根据项目数值判断点击了哪个项目
#define TAB_PARAM_PROGRAM       10      // 程序, 工作夹里面的程序子夹的项目数值, 点击根据项目数值判断点击了哪个项目
#define TAB_PARAM_PROPER        11      // 属性, 工作夹里面的属性子夹的项目数值, 点击根据项目数值判断点击了哪个项目
#define TAB_PARAM_CONTROLBOX    12      // 组件箱, 右边的组件箱




#define IDRAW_FNE_IDRAW     L"iDraw"           // 支持库使用的目录, 这个算是主目录, 系统插件在这里
#define IDRAW_FNE_DBGPATH   L"iDraw\\debug\\"  // 支持库调试插件目录
#define IDRAW_FNE_NAME      L"易IDE视觉库"      // 支持库名
#define IDRAW_FNE_NAMEA     "易IDE视觉库"      // 支持库名

struct IDRAW_INFO;

DECLARE_HANDLE(IDRAW_HPLUGIN);  // 插件句柄



typedef struct EMENU_DATA
{
    HMENU hMenu;
    wchar_t text[260];
    int id;
    DWORD dwItemData;
    RECT rcItem;        // 菜单宽高
    bool isPopup;       // 鼠标经过是否允许弹出菜单
    bool isHide;        // 是否隐藏这个菜单项
}*LPEMENU_DATA;

typedef struct TABCHANGE_INFO
{
    NMHDR hdr;
    int index;      // 新的选中项
    int indexOld;   // 切换前的选中项
    int selMode;    // 触发选中事件的模式, TAB_SEL_MODE 枚举常量
}*LPTABCHANGE_INFO;


typedef INT(WINAPI* pfnEIDE_Notify) (INT nMsg, DWORD dwParam1, DWORD dwParam2);    // 易语言IDE通知事件
typedef LRESULT(CALLBACK* WND_EWINDOW_SUBPROC)(HWND, UINT, WPARAM, LPARAM, LPOBJSTRUCT);

//#define CUSTOMTAB_INFO_WINDOW               1   // 窗口设计器
//#define CUSTOMTAB_INFO_ASSEMBLY             2   // 普通程序集
//#define CUSTOMTAB_INFO_ASSEMBLY_WINDOW      3   // 窗口程序集
//#define CUSTOMTAB_INFO_CLASS                4   // 类
//#define CUSTOMTAB_INFO_GLOBALVAR            5   // 全局变量
//#define CUSTOMTAB_INFO_STRUCT               6   // 数据类型
//#define CUSTOMTAB_INFO_DLL                  7   // DLL命令定义表
//#define CUSTOMTAB_INFO_CONST                8   // 常量
//#define CUSTOMTAB_INFO_IMAGE                9   // 图片资源
//#define CUSTOMTAB_INFO_SOUND                10  // 声音资源
enum CUSTOMTAB_INFO
{
    CUSTOMTAB_INFO_UNDEFINE                 = 0,   // 未定义
    CUSTOMTAB_INFO_WINDOW                   = 1,   // 窗口设计器
    CUSTOMTAB_INFO_ASSEMBLY                 = 2,   // 普通程序集
    CUSTOMTAB_INFO_ASSEMBLY_WINDOW          = 3,   // 窗口程序集
    CUSTOMTAB_INFO_CLASS                    = 4,   // 类
    CUSTOMTAB_INFO_GLOBALVAR                = 5,   // 全局变量
    CUSTOMTAB_INFO_STRUCT                   = 6,   // 数据类型
    CUSTOMTAB_INFO_DLL                      = 7,   // DLL命令定义表
    CUSTOMTAB_INFO_CONST                    = 8,   // 常量
    CUSTOMTAB_INFO_IMAGE                    = 9,   // 图片资源
    CUSTOMTAB_INFO_SOUND                    = 10,  // 声音资源
    CUSTOMTAB_INFO_CLOSEHOT                 = 11,  // 关闭按钮热点
    CUSTOMTAB_INFO_CLOSEDOWN                = 12,  // 关闭按钮按下
};


// 视觉库插件信息
struct PLUGIN_INFO;
// 助手插件信息结构
struct ETOOLS_PLUGIN_INFO;

typedef int (CALLBACK* pfn_IDraw_Callback)(UINT, WPARAM, LPARAM);
// 助手顶部菜单被单击
typedef int (WINAPI* pfn_etMenuSelect)(int id, HMENU hMenu);
typedef int(CALLBACK* pfn_MenuExt_Command)(int id, LPARAM param);
typedef int (CALLBACK* pfn_IDraw_UnInterface)();
typedef PLUGIN_INFO* (CALLBACK* pfn_IDraw_Interface)(IDRAW_INFO*, IGetEWindow*, int, int);
typedef void* (CALLBACK* pfn_IDraw_ConfigChange)(int, int, int);
typedef void(CALLBACK* pfn_EWindowThreadCallback)(int);
typedef int(CALLBACK* pfn_GetAllSubclassWindow)(LPOBJSTRUCT* pArr, int count);
typedef LPWSTR(CALLBACK* pfn_CopyStrW)(LPCWSTR, LPWSTR&);
typedef LPSTR(CALLBACK* pfn_CopyStrA)(LPCSTR, LPSTR&);
typedef void(CALLBACK* pfn_FreeStr)(void*);

typedef struct IDRAW_STRUCT_SCROLL_DATA
{
    DWORD cbSize;       // 结构尺寸, 调用前需要先给这个成员赋值
    float btn1Size;     // 按钮1的宽高, 横向是左边按钮宽度, 纵向是顶边按钮高度
    float btn2Size;     // 按钮2的宽高, 横向是右边按钮宽度, 纵向是底边按钮高度
    float btnThumbSize; // 滑块宽度, 这个决定了整个滚动条的宽高, 横向是高度, 纵向是宽度
}*LPIDRAW_STRUCT_SCROLL_DATA;

typedef struct TAB_DRAWINFO
{
    LPEX_BRUSH  hbrState;       // 顶边这条框背景画刷
    LPEX_BRUSH  hbrStateAct;    // 顶边这条框激活时的背景画刷

    LPEX_BRUSH  hbrSel;         // 选择夹子夹选中画刷
    LPEX_BRUSH  hbrExtern;      // 选择夹子夹背景画刷, 这个需要从左边填充到选择夹的右边
    LPEX_BRUSH  hbrBack;        // 选择夹背景画刷
    LPEX_BRUSH  hbrHot;         // 选择夹热点画刷
    LPEX_BRUSH  hbrDown;        // 选择夹按下画刷
    LPEX_BRUSH  hbrText;        // 选择夹文本画刷
    LPEX_BRUSH  hbrTextHot;     // 选择夹文本热点画刷
    LPEX_BRUSH  hbrTextDown;    // 选择夹文本按下画刷

    LPEX_PEN    hPenBorder;     // 选择夹外边框颜色
    LPEX_PEN    hPenDot;        // 顶边状态条中间点没激活的画笔
    LPEX_PEN    hPenDotAct;     // 顶边状态条中间点激活时的画笔
}*LPTAB_DRAWINFO;

typedef struct TAB_INFO
{
    DWORD size;                 // 固定宽高的宽度高度, 低位=宽度, 高位=高度
    int minWidth;               // 项卡控件中项的最小宽度
    DWORD padding;              // 每个选项卡的填充空间, 低位 = 填充宽度, 高位 = 填充高度
    TAB_DRAWINFO draw;          // 绘画需要的对象
    int state;                  // 这个状态 == STATE::DOWN 就是关闭按钮被按下
    int indexSel;               // 选中项
    int indexDown;              // 鼠标按下的项目
    int indexHot;               // 鼠标移动的热点项目
    int isUpdown;               // 是否显示调节器
    int lState;                 // 调节器左边的状态
    int rState;                 // 调节器右边的状态
    int indexFirst;             // 第一个显示的子夹
}*LPTAB_INFO;

// 助手插件信息结构
typedef struct ETOOLS_PLUGIN_INFO
{
    DWORD cbSize;           // 结构尺寸
    LPCSTR name;            // 插件名字
    LPCSTR remarks;         // 描述
    LPCSTR version;         // 版本
    LPCSTR author;          // 作者
    DWORD Versionlimit;     // 版本限制
    void* Dependence;       // 依赖插件
}*LPETOOLS_PLUGIN_INFO;

struct IDRAW_STRUCT_TOOL_INSERTEX;
// 插件信息, 从助手插件结构继承过来
typedef struct PLUGIN_INFO : ETOOLS_PLUGIN_INFO
{
    LPCSTR  guid;                           // 插件唯一标识符, 直接记录初始化时传递的指针, hash 这个成员是内部申请存放guid的地址
    pfn_MenuExt_Command pfnMenuExt_Command; // 菜单被调用事件
    pfn_IDraw_Callback  pfnCallback;        // 与视觉库通讯的回调函数

    // 下面的成员是内部使用的, 插件公开的就前面的成员
    HMENU   hPopupMenu;             // 插件自己的弹出式菜单句柄, 自己插件只能加入到自己的菜单下面
    HMODULE hModule;                // 当前插件的模块句柄
    int     fileSize;               // 当前模块文件尺寸, 释放插件时需要释放模块+文件大小这个区域的地址
    int     hash;                   // 插件的哈希值, 通过这个值查找到插件数据在内存里的位置

    pfn_IDraw_UnInterface   pfnUnInterface;     // 取消初始化接口
    pfn_IDraw_Interface     pfnInterface;       // 初始化接口
    pfn_IDraw_ConfigChange  pfnEConfigChange;   // 配置改变接口

    IDRAW_STRUCT_TOOL_INSERTEX* pToolButton;            // 当前插件工具条按钮数据
    int                         nToolButtonCount;       // 当前插件工具条按钮数量
    int                         nToolButtonBufferCount; // 当前插件工具条按钮缓冲区数量
    LPCSTR                      dllName;                // 当前插件的dll文件名
}*LPPLUGIN_INFO;

typedef struct IDRAW_STRUCT_TOOL_INSERTEX : IDRAW_STRUCT_TOOL_INSERT
{
    int key;                    // 这个结构在map里的key
    wstr title;                 // 标题, lpszTitle = title.c_str()
    wstr tips;                  // 提示文本, lpszTips = tips.c_str()
    LPEX_IMAGE hImage;          // 绘画使用的图片
    LPPLUGIN_INFO plugin;       // 触发事件调用的插件回调

}*LPIDRAW_STRUCT_TOOL_INSERTEX;

// xml配置, 构造函数必须传入要操作的节点名, 而且必须保证传入指针的存活
class XMLConfig;

typedef LRESULT             (WINAPI* pfn_ALLWINDOW_MESSAGE) (HWND hWnd, UINT message, WPARAM* wParam, LPARAM* lParam, LPOBJSTRUCT pData);
typedef Gdiplus::Bitmap*    (WINAPI* pfn_GetResourceData)   (LPCWSTR id, RECT* prc, STATE state);
typedef LPOBJSTRUCT         (__cdecl* pfn_GetDataFromHwnd)  (HWND hWnd);
typedef int                 (__cdecl* pfn_event_before)     (LPREGISTER_EVENT_INFO arr[EVENT_MAX_ITEM], UINT nEvent, LPVOID pArgs, int* isReturn, bool isStop);
typedef int                 (__cdecl* pfn_event_after)      (LPREGISTER_EVENT_INFO arr[EVENT_MAX_ITEM], UINT nEvent, LPVOID pArgs, int evtCount, bool isStop);
typedef HBRUSH              (__cdecl* pfn_CreateBrush)      (COLORREF cr);
typedef HPEN                (__cdecl* pfn_CreatePen)        (int iStyle, int cWidth, COLORREF color);
typedef LPEX_BRUSH          (__cdecl* pfn_CreateBrushDx)    (DWORD argb);
typedef LPEX_PEN            (__cdecl* pfn_CreatePenDx)      (DWORD argb, float width);
typedef LPEX_FONT           (__cdecl* pfn_CreateFontDx)     (LPCWSTR name, LONG lfHeight, FONTSTYLE fontStyle);
typedef LPEX_FONT           (__cdecl* pfn_CreateFontLfDx)   (LPLOGFONTW lf);
typedef LPEX_FONT           (__cdecl* pfn_CreateFontDpi)    (LPEX_FONT, UINT dpi);
typedef LPOBJSTRUCT         (__cdecl* pfn_GetDefData)       (LPOBJSTRUCT pData);
typedef void                (__cdecl* pfn_ReloadDxResource) ();
typedef LONG_PTR            (CALLBACK* pfn_GetInfo)         (IDRAW_INFO*, int, LPARAM);
typedef LONG_PTR            (CALLBACK* pfn_SetInfo)         (IDRAW_INFO*, int, LONG_PTR, BOOL);
typedef void                (CALLBACK* pfn_InfoChanged)     (int, LONG_PTR, LONG_PTR, bool);
typedef int                 (__cdecl*  pfn_Scale)           (int);




// 调用易语言指定菜单功能, 传递菜单ID
typedef LRESULT(__cdecl* pfn_call_menu)(int id);


typedef LPPLUGIN_INFO(__cdecl* pfn_Plugin_init)(LPCSTR guid, HMODULE hModule, ETOOLS_PLUGIN_INFO* etoolsInfo, pfn_MenuExt_Command pfnMenuExt_Command, pfn_IDraw_Callback callback);
typedef LPPLUGIN_INFO(__cdecl* pfn_Plugin_GetInfo)(LPCSTR guid);
typedef LPPLUGIN_INFO(__cdecl* pfn_Plugin_Query)(LPCSTR hash);
typedef bool(__cdecl* pfn_Plugin_UnInit)(LPCSTR guid);
typedef int (__cdecl* pfn_Plugin_Enum)(LPPLUGIN_INFO* pInfo, int bufCount);
typedef bool(__cdecl* pfn_MenuExt_Insert)(LPCSTR guid, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItem, LPARAM param);
typedef bool(__cdecl* pfn_MenuExt_Delete)(LPCSTR guid);


// 加入菜单到窗口顶部的菜单夹中
// id = 菜单id, 通过这个id区分是哪个按钮
// lpszMenuText = 显示的菜单文本
// index = 要插入到哪个位置, 从10开始, -1则插入到最后, 前面10个菜单是易语言自带的, 所以这里从10开始
// hPopupMenu = 是否要弹出菜单, 如果是要弹出菜单, 需要传递菜单句柄, 点击这个按钮时会弹出菜单, 传递0的话点击会触发点击事件
// isPopup = 是否弹出菜单, 如果为真, 则在鼠标经过的时候会弹出菜单
typedef void(__cdecl* pfn_etInsertMenu)(int id, LPCWSTR lpszMenuText, int index, HMENU hPopupMenu, bool isPopup);
typedef void(__cdecl* pfn_etRemoveMenu)(int id, bool isIndex);
typedef void(__cdecl* pfn_SetLastError)(LPCWSTR fmt, ...);
typedef LPCWSTR(__stdcall* pfn_GetLastError)();
typedef void(__cdecl* PFN_log_dbg)(LPCSTR fmt, ...);
typedef const char* (__cdecl* PFN_GetDllNameFromAddress)(LPCVOID pAddress);

struct EWINDOW_MENUSTRUCT
{
    std::vector<EMENU_DATA> menu;       // 菜单数据
    std::vector<EMENU_DATA> menu_etool; // 助手菜单
    HMENU hMenu;                // 菜单句柄, 这个结构的数据就是这个菜单的子菜单
};

// 事件插件的各个回调函数, 使用一个结构记录保存
typedef struct EVENT_CALLBACK_FUN
{
    pfn_iDraw_Event_GetMessageInfo  pfnGetMessageInfo;          // 事件插件里的获取消息回调数据, 通过窗口句柄-消息值-GUID获取, 在事件插件初始化的时候赋值
    pfn_iDraw_Event_GetEventInfo    pfnGetEventInfo;            // 事件插件里的获取事件回调数据, 通过窗口句柄-消息值-GUID获取, 在事件插件初始化的时候赋值
    pfn_iDraw_Register_Message      pfnRegister_Message;        // 事件插件里的注册消息回调, 在事件插件初始化的时候赋值
    pfn_iDraw_Register_Event        pfnRegister_Event;          // 事件插件里的注册事件回调函数, 在事件插件初始化的时候赋值
    pfn_DetachRegister_Message      pfnDetachRegister_Message;  // 窗口被销毁, 内部使用, 删除这个窗口下的所有消息回调
    pfn_iDraw_UnRegister            pfnUnRegister_Message;      // 取消指定插件的所有消息回调
    pfn_iDraw_UnRegister            pfnUnRegister_Event;        // 取消指定插件的所有事件回调
    pfn_event_before                pfn_Event_callback_before;  // 事件处理前调用, 封装的一个简单的函数
    pfn_event_after                 pfn_Event_callback_after;   // 事件处理后调用, 封装的一个简单的函数
}*LPEVENT_CALLBACK_FUN;

typedef void (__cdecl* pfn_load_plugin)();


// 窗口配色信息, 这些颜色都需要D2D创建, 需要在自绘插件里初始化D2D后创建对应数据
typedef struct THEME_D2D_INFO
{
    LPEX_BRUSH hbrBack;                 // 背景画刷

    LPEX_PEN hPenBorder;                // 边框画笔
    LPEX_PEN hPenBorder_run;            // 运行时边框画笔
    LPEX_PEN hPenBorder_nf;             // 取消激活时的边框画笔
    LPEX_PEN hPenShadow;                // 阴影颜色
    LPEX_PEN hPenShadow_run;            // 运行时阴影颜色
    LPEX_PEN hPenShadow_nf;             // 取消激活时的边框颜色

    LPEX_BRUSH hbrBorder;               // 边框画刷
    LPEX_BRUSH hbrBorder_run;           // 运行时边框画刷
    LPEX_BRUSH hbrBorder_nf;            // 取消激活时的边框画刷
    LPEX_BRUSH hbrShadow;               // 阴影画刷
    LPEX_BRUSH hbrShadow_run;           // 运行时阴影画刷
    LPEX_BRUSH hbrShadow_nf;            // 取消激活时的边框画刷


    LPEX_PEN   hPenMenuBorder;          // 菜单边框画笔
    LPEX_BRUSH hbrMenuBorder;           // 菜单边框画刷
    LPEX_BRUSH hbrMenuBack;             // 菜单背景画刷, 弹出菜单使用的背景画刷, 菜单条按下时使用的背景画刷
    LPEX_BRUSH hbrMenuHot;              // 菜单热点项画刷
    LPEX_BRUSH hbrMenuBan;              // 菜单禁止画刷
    LPEX_BRUSH hbrMenuText;             // 菜单文本画刷
    LPEX_BRUSH hbrMenuTextHot;          // 菜单文本热点画刷
    LPEX_BRUSH hbrMenuTextBan;          // 菜单文本禁止画刷

    LPEX_BRUSH hbrMenuDown;             // 菜单背景画刷, 弹出菜单使用的背景画刷, 菜单条按下时使用的背景画刷
    LPEX_BRUSH hbrMenuBarHot;           // 顶部菜单条热点项画刷
    LPEX_BRUSH hbrMenuBarToolsText;     // 顶部菜单条助手的文本画刷
    LPEX_BRUSH hbrMenuBarToolsTextHot;  // 顶部菜单条助手的文本热点画刷

    LPEX_BRUSH hbrTitleback;            // 中间黑块标题的背景画刷
    LPEX_BRUSH hbrTitle_left_act;       // 中间黑块标题左边的画刷, 这个是源码名或者项目名, 激活时的文本画刷
    LPEX_BRUSH hbrTitle_right_act;      // 中间黑块标题右边的画刷, 这个是程序类型, 或者未保存时的星号使用的画刷, 激活时的文本画刷
    LPEX_BRUSH hbrTitle_left;           // 中间黑块标题左边的画刷, 这个是源码名或者项目名
    LPEX_BRUSH hbrTitle_right;          // 中间黑块标题右边的画刷, 这个是程序类型, 或者未保存时的星号使用的画刷

    LPEX_BRUSH hbrText;                 // 一般绘画文本的画刷
    LPEX_BRUSH hbrTextStatusBar;        // 底部状态条的文本画刷
    LPEX_BRUSH hbrTextStatusBar_run;    // 底部状态条运行时的文本画刷
    LPEX_BRUSH hbrTextStatusBar_nf;     // 底部状态条不激活的文本画刷


    LPEX_FONT  hFont;                   // 中间标题使用的字体, -13加粗
    LPEX_FONT  hFont1;                  // 中间标题使用的字体, -13不加粗

    LPCANVAS hCanvas;                   // 菜单专用d2d绘画句柄

}*LPTHEME_D2D_INFO;


struct IDRAW_INFO
{
    HWND hEWnd;                 // 易语言主窗口句柄
    HWND hShadow;               // 易语言阴影窗口

    HWND hCodeTab;              // 代码区下面的选择夹句柄
    HWND hState;                // 状态夹句柄, 输出编辑框/变量表都在这里
    HWND hStatusBar;            // 底部状态条
    HWND hMDIClient;            // 代码框的窗口句柄, 代码框里包含所有打开的标签, 这个的父窗口的易语言主窗口, 自绘选择夹在这个窗口的上面
    HWND hControl;              // 组件箱
    HWND hWork;                 // 工作夹, 左边支持库/程序
    HWND hWndMenu;              // 菜单栏容器, 里面放着对齐工具条,标准工具条
    HWND hMenuMenu;             // 存放菜单列的窗口句柄
    HWND hMenu_Standard;        // 新创建的工具条, 对齐和标准工具条都使用这个
    HWND hWndTabControl;        // 组件箱里的选择夹, 自己创建的, 需要响应子夹切换事件来显示不同的窗口
    HWND hWndTool;              // 标准工具条后面的工具条, 这个是作为一个接口让用户增加按钮到这里
    HWND hTabStateLeft;         // 自己新创建的选择夹, 在状态夹里分左右两块窗口, 跟状态夹带的选择夹是同级窗口, 左边的窗口
    HWND hTabStateRight;        // 自己新创建的选择夹, 在状态夹里分左右两块窗口, 跟状态夹带的选择夹是同级窗口, 右边的窗口

    EWINDOW_MENUSTRUCT menu;    // 菜单数据
    EWINDOW_MENUSTRUCT menuDoc; // 文档菜单数据

    RECT rcEWindow;             // 易语言窗口客户区位置, 左顶不是屏幕坐标
    RECT rcCustomTab;           // 自绘的选择夹, 这个选择夹是切换源码显示的选择夹
    RECT rcCodeTab;             // 代码区下面的选择夹句柄
    RECT rcState;               // 状态夹句柄, 输出编辑框/变量表都在这里
    RECT rcStatusBar;           // 底部状态条
    RECT rcMDIClient;           // 代码框的窗口句柄, 代码框里包含所有打开的标签, 这个的父窗口的易语言主窗口, 自绘选择夹在这个窗口的上面
    RECT rcControl;             // 组件箱
    RECT rcWork;                // 工作夹, 左边支持库/程序
    RECT rcWndMenu;             // 菜单栏容器, 里面放着对齐工具条,标准工具条
    RECT rcStandard;            // 菜单栏容器里的标准工具条位置
    RECT rcMenu;                // 菜单栏位置
    RECT rcControl1;            // 组件箱的位置, 自动隐藏时的位置
    RECT rcState1;              // 状态夹位置, 自动隐藏时的位置
    RECT rcWork1;               // 工作夹位置, 自动隐藏时的位置
    int cxClient;               // 易语言窗口客户区宽度
    int cyClient;               // 易语言窗口客户区高度

    bool isActivation;          // 是否激活窗口, 这个值被改变时会触发窗口标题位置重画, 还有触发阴影窗口重画
    bool isZoomed;              // 当前窗口是否已经最大化, 在窗口尺寸改变里修改

    HWND hMenuLocation;         // 定位工具条, 在子类化前设置父窗口为易语言主界面, 位置在代码框的上面, 自绘选择夹的下面
    HWND hComboBox1;            // 定位工具条左边的组合框
    HWND hComboBox2;            // 定位工具条右边的组合框

    ICustomCodeTab* hWnds;      // 自绘选择夹的窗口数组, 下标对应选择夹的下标, 选择夹选中时激活数组的窗口

    BOOL isRun;                 // 运行是否是启用状态, 这个值被改变时会触发窗口标题位置重画, 还有触发阴影窗口重画
    BOOL isStop;                // 停止是否是启用状态

    IGetEWindow*    eWnd;       // 易语言窗口
    pfnEIDE_Notify  pfnIDE;     // 易语言交互的函数
    pfn_Scale       scale;      // 缩放函数, 需要调用这个函数来计算各种尺寸
    UINT            dpiX;       // 当前屏幕的dpi缩放X值
    UINT            dpiY;       // 当前屏幕的dpi缩放Y值
    bool            isReady;    // 是否就绪, 可以根据需要决定是否就绪后才改变尺寸


    UINT    MY_MF_OWNERDRAW;    // 值为 MF_OWNERDRAW 就是使用内部的自绘, 否则使用win10的深色主题

    LPCWSTR path_e;             // 易语言e.exe所在的目录, 后面带"\"
    LPCWSTR path_run;           // 易语言支持库的目录, 后面带"\"    lib/
    LPCWSTR path_lib;           // 易语言支持库的目录, 后面带"\"    lib/
    LPCWSTR path_file;          // 易语言支持库的目录, 后面带"\"    lib/
    LPCWSTR path_data;          // 数据保存目录, 后面带 "\", 这个是支持库目录下的 lib/iDraw/data/ 目录
    LPCWSTR path_iDraw;         // 核心插件目录, 后面带 "\", 这个是支持库目录下的 lib/iDraw/ 目录
    LPCWSTR path_plugin;        // 插件目录, 后面带 "\", 这个是支持库目录下的 lib/iDraw/plugin/ 目录
    LPCWSTR path_ETplugin;      // 助手插件目录, 后面带 "\", 这个是支持库目录下的 lib/iDraw/eTools/plugin/ 目录
    LPCWSTR path_config;        // 配置文件完整路径, lib/iDraw/data/config.ini

    LPOBJSTRUCT defData;        // 默认的配置, 默认配色
    IiDraw* iDraw;              // 自绘相关接口, 所有自绘有关的都应该通过此接口调用
    ID2DDraw* d2d;              // D2D绘画对象接口, 由绘画组件的插件创建, 用到D2D的都使用这个对象进行操作
    CLayered* pTips;            // 提示组件
    wstr* pszTitle;             // 窗口标题, 只读, 在易语言主窗口子类化时赋值
    ICShadow* shadow;           // 阴影窗口类
    IEWindowResource* pResource;// 资源相关接口, 所有都从这个模块里返回
    pfn_GetDataFromHwnd pfnGetDataFromHwnd; // 获取窗口句柄对应信息的函数, 没有则返回0
    pfn_ALLWINDOW_MESSAGE pfnAllWindowProc; // 移动模块需要处理消息的函数, 遇到需要处理的消息时就调用这个函数
    EVENT_CALLBACK_FUN evt;     // 事件插件的函数, 这个值会在事件插件初始化时初始化

    LPPLUGIN_INFO pInfoMoveWindow;
    LPPLUGIN_INFO pInfoDrawControl;
    LPPLUGIN_INFO pInfoResource;
    LPPLUGIN_INFO pInfoEvent;
    LPPLUGIN_INFO pInfoConfig;
    LPPLUGIN_INFO pInfoETools;
    LPPLUGIN_INFO pInfoTheme;

    //THEME_COLOR_INFO    window_color_info;  // 窗口的配色信息, 在初始化主题插件的时候初始化这个结构
    THEME_D2D_INFO      window_d2d_info;    // 窗口的配色画刷画笔, 在初始化自绘插件的时候初始化这个结构
    bool isDxRender;            // 是否为dx渲染, 在初始化绘画插件时赋值
    bool isGpRender;            // 是否为Gdiplus渲染, 在初始化绘画插件时赋值
    int nRenderMode;            // 有赋值暂时未使用, 渲染模式, 在初始化绘画插件时赋值, 未来可能会增加dx9-dx11, openGL等模式

    HMENU hMenuPopup;           // 当前弹出的菜单句柄, 在 TrackPopupMenu 或者 TrackPopupMenu 调用前赋值, 调用后清0, 如果不是这两个API弹出的菜单则不记录

    pfn_load_plugin pfnLoadPlugin;
    pfn_load_plugin pfnFreePlugin;

    LPOBJSTRUCT     pWnd;               // 易语言主窗口数据
    TAB_INFO        tab;                // 代码切换选择夹数据
    pfn_CopyStrW    copyStrW;           // 统一一个位置申请内存
    pfn_CopyStrA    copyStrA;           // 统一一个位置申请内存
    pfn_FreeStr     freeStr;            // 统一一个位置释放内存

    CPrivateProfile* ini;               // 主配置
    CPrivateProfile* iniColor;          // 颜色配置
    IConfig* config;                    // 配置信息, 设置值时会写入xml里, 这个由配置插件初始化, 在初始化的时候已经有了默认值
    IThemeColor* themeColor;            // 主题的配色, 设置值时会写入xml里, 这个由配置插件初始化, 在初始化的时候已经有了默认值



    pfn_CreateBrush     pfnCreateBrush;     // 内部使用的创建画刷函数, 返回的画刷不能释放, 在iTheme插件初始化时初始化
    pfn_CreatePen       pfnCreatePen;       // 内部使用的创建画笔函数, 返回的画笔不能释放, 在iTheme插件初始化时初始化
    pfn_CreateBrushDx   pfnCreateBrushDx;   // 内部使用的创建Dx画刷函数, 返回的画刷不能释放, 在iTheme插件初始化时初始化
    pfn_CreatePenDx     pfnCreatePenDx;     // 内部使用的创建Dx画笔函数, 返回的画笔不能释放, 在iTheme插件初始化时初始化
    pfn_CreateFontDx    pfnCreateFontDx;    // 内部使用的创建Dx字体函数, 返回的字体不能释放, 在iTheme插件初始化时初始化
    pfn_CreateFontLfDx  pfnCreateFontLfDx;  // 内部使用的创建Dx字体函数, 返回的字体不能释放, 在iTheme插件初始化时初始化
    pfn_CreateFontDpi   pfnCreateFontDpi;   // 内部使用的根据dpi创建Dx字体函数, 返回的字体不能释放, 在iTheme插件初始化时初始化
    pfn_GetDefData      pfnGetDefData;      // 获取默认绘画信息, 会把默认信息写入传递进来的参数里, 在iTheme插件初始化时初始化
    pfn_ReloadDxResource pfnReloadDxResource;// 重新创建所有画笔画刷字体

    pfn_GetInfo pfnGetInfo;             // 获取配置信息
    pfn_SetInfo pfnSetInfo;             // 设置配置信息, 调用这个的话有部分配置是会直接写入xml或者即时生效的
    pfn_InfoChanged pfnInfoChanged;     // 配置被改变回调
    pfn_GetAllSubclassWindow pfnGetAllSubclassWindow;   // 获取所有已经子类化的窗口数据, 返回写入缓冲区成员数, 参数为0则返回缓冲区所需数量

    pfn_call_menu pfnCallMenu;          // 调用易语言指定菜单功能, 传递菜单ID

    pfn_Plugin_init     pfnPlugin_init;         // 初始化插件, 把插件注册到界面库里
    pfn_Plugin_GetInfo  pfnPlugin_GetInfo;      // 从指定guid获取插件信息
    pfn_Plugin_Query    pfnPlugin_Query;        // 从指定值查询是否为插件的guid
    pfn_Plugin_Enum     pfnPlugin_Enum;         // 枚举当前所有已经初始化的插件, 需要外部传递缓冲区
    pfn_Plugin_UnInit   pfnPlugin_UnInit;       // 把指定guid的插件从列表里删除


    pfn_IDraw_Callback  pfnConfig;              // 配置插件的回调函数, 要设置或者获取数据就调用这个函数
    pfn_IDraw_Callback  pfnTheme;               // 主题插件的回调函数, 要设置或者获取数据就调用这个函数
    pfn_IDraw_Callback  pfnEvent;               // 事件插件的回调函数, 要设置或者获取数据就调用这个函数
    pfn_IDraw_Callback  pfnControls;            // 绘画插件的回调函数, 要设置或者获取数据就调用这个函数, 这个比较重要, 菜单什么的都是这个插件在实现
    pfn_IDraw_Callback  pfnResource;            // 资源插件的回调函数, 要设置或者获取数据就调用这个函数
    pfn_IDraw_Callback  pfnMove;                // 移动插件的回调函数, 要设置或者获取数据就调用这个函数
    pfn_IDraw_Callback  pfnETools;              // 助手回调函数, 需要助手设置这个值, 可以调用这个函数去获取或者设置助手的一些功能

    HMENU hMenuExtPopup;                        // 扩展弹出式菜单, 所有插件的菜单都是加入到这个菜单下

    UINT uMsgEWindowThreadCallback;             // 易语言窗口线程调用函数的消息值, wParam是函数地址, lParam是函数参数

    pfn_SetLastError SetLastError;              // 设置最后错误
    pfn_GetLastError GetLastError;              // 获取最后错误
    PFN_log_dbg      logMsgEvt;                 // 输出注册事件, 注册消息, 拦截消息等事件数据到文件
    PFN_log_dbg      logPluginInit;             // 插件初始化信息
    PFN_GetDllNameFromAddress   pfn_GetDllNameFromAddress;  // 获取指定地址的dll名字

    int             menuSelIndex;               // 当前弹出的菜单鼠标经过的项目索引
    int             menuSelId;                  // 当前弹出的菜单鼠标经过的项目ID
    HMENU           menuSelHmenu;               // 当前弹出的菜单鼠标经过的项目所在的菜单句柄

    int             EVersion;                   // 易语言版本号593, 591, 595, 5922 这种格式
};
typedef IDRAW_INFO* LPIDRAW_INFO;
extern LPIDRAW_INFO s_info;     // 包含各个窗口的信息, 支持库和其他动态库都有使用, 支持库调用动态库的时候会传递这个指针过去

inline BOOL WINAPI ShowWindowEx(HWND hWnd, int nCmdShow)
{
    if (hWnd)
    {
        LPBYTE pShowWindow = (LPBYTE)ShowWindow;
        LPBYTE pJmp = 0;
        if (*pShowWindow == 0xe9)
        {
            DWORD offset = *(DWORD*)(pShowWindow + 1);
            pJmp = pShowWindow + 5 + offset;
        }
        else if (*pShowWindow == 0xff && pShowWindow[1] == 0x25)
        {
            DWORD offset = *(DWORD*)(pShowWindow + 2);
            pJmp = (LPBYTE)offset;
        }

        if (pJmp)
        {
            MEMORY_BASIC_INFORMATION mbi;
            if (VirtualQuery(pJmp, &mbi, sizeof(mbi)) != sizeof(mbi))
            {
                return FALSE;
            }
        }
        return ShowWindow(hWnd, MAKELONG(__HOOK_MOVE_BREPAINT, nCmdShow));
    }
    return FALSE;
}

inline BOOL WINAPI MoveWindowEx(HWND hWnd, const RECT& rc, bool bRepaint)
{
    //return SetWindowPos(hWnd, 0, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_DRAWFRAME);
    return MoveWindow(hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, MAKELONG(__HOOK_MOVE_BREPAINT, bRepaint));
}

//// 获取指定名字的node节点, 如果没有这个节点, 会创建一个新的节点
//inline ICTinyXml2_XMLNode XML_GetNode(CTinyXml2W* lpXml, LPCWSTR lpszNodeName)
//{
//    wchar_t name[100];
//    swprintf_s(name, 100, L"@1/%s", lpszNodeName);
//    ICTinyXml2_XMLNode node = 0;
//    if (!lpXml->GetNode(name, node))
//        lpXml->InsertChildNode(L"@1", lpszNodeName, 0, false, &node);
//    return node;
//}
//
//// 获取xml指定节点的属性值
//inline wstr XML_Read(CPrivateProfile* lpXml, ICTinyXml2_XMLNode node, LPCWSTR lpszAttrName, LPCWSTR lpszDefValue = 0)
//{
//    if (!node)return wstr();
//    LPCWSTR value =  lpXml->GetAttrValue(node, lpszAttrName);
//    if (!value || !value[0])value = lpszDefValue;
//    return value;
//}
//
//// 设置xml指定节点的属性值
//inline bool XML_Write(CPrivateProfile* lpXml, ICTinyXml2_XMLNode node, LPCWSTR lpszAttrName, LPCWSTR lpszValue)
//{
//    if (!node)return false;
//    return lpXml->SetAttrValue(node, lpszAttrName, lpszValue);
//}
//


inline wstr INI_Read(CPrivateProfile* pIni, LPCWSTR appName, LPCWSTR keyName, LPCWSTR defValue)
{
    return pIni->read(appName, keyName, defValue);
}
inline bool INI_Write(CPrivateProfile* pIni, LPCWSTR appName, LPCWSTR keyName, LPCWSTR value)
{
    return pIni->write(appName, keyName, value);
}
inline bool Rect_isOverlap(const RECT& rc1, const RECT& rc2)
{
    return (rc1.right > rc2.left && rc2.right > rc1.left
        && rc1.bottom > rc2.top && rc2.bottom > rc1.top);
}
// 从样式里判断是否有边框
inline bool IsBorder(DWORD style, DWORD styleEx)
{
    if (style & WS_BORDER)return true;
    if ((styleEx & WS_EX_CLIENTEDGE)
        || (styleEx & WS_EX_DLGMODALFRAME)
        || (styleEx & WS_EX_WINDOWEDGE)
        )
        return true;
    return false;
}
inline bool IsBorder(HWND hWnd)
{
    DWORD style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if (style & WS_BORDER)return true;
    DWORD styleEx = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if ((styleEx & WS_EX_CLIENTEDGE)
        || (styleEx & WS_EX_DLGMODALFRAME)
        || (styleEx & WS_EX_WINDOWEDGE)
        )
        return true;
    return false;
}


template<typename T, typename R>bool __query(T s, R n)
{
    return (s & n) != 0;
}
inline DWORD InsertStyle(HWND hWnd, DWORD style)
{
    DWORD dwStyle = GetWindowLongPtrA(hWnd, GWL_STYLE);
    SetWindowLongPtrA(hWnd, GWL_STYLE, dwStyle | style);
    return dwStyle;
}

inline DWORD RemoveStyle(HWND hWnd, DWORD style)
{
    DWORD dwStyle = GetWindowLongPtrA(hWnd, GWL_STYLE);
    SetWindowLongPtrA(hWnd, GWL_STYLE, dwStyle & ~style);
    return dwStyle;
}

// 调用易语言IDE接口功能
inline int pfn_NES_RUN_FUNC(int message, DWORD wParam = 0, DWORD lParam = 0)
{
    // 通知易编辑环境运行指定的功能,返回一个BOOL值。
    // dwParam1为功能号。
    // dwParam2为一个双DWORD数组指针,分别提供功能参数1和2。
    const int n_NES_RUN_FUNC = 2;
    DWORD arg[2] = { wParam, lParam };
    return s_info->pfnIDE(n_NES_RUN_FUNC, message, (DWORD)arg);
}

inline LRESULT CALLBACK WndProc_CallWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    pfn_CallWindowProc proc = pData->fnCallProc;
    if (!proc)proc = CallWindowProcW;
    return proc(pData->oldProc, hWnd, message, wParam, lParam);
}

inline int _tab_getdirection(DWORD style)
{
    const bool isBottom = (style & TCS_BOTTOM) == TCS_BOTTOM;   // 是在底边, 左边右边没处理   
    int direction = (style & TCS_VERTICAL) == TCS_VERTICAL;     // 选项卡方向, 0=左边, 1=顶边, 2=右边, 3=底边
    if (direction)  direction = isBottom ? 2 : 0;
    else            direction = isBottom ? 3 : 1;
    return direction;
}

// 返回选择夹的方向, 0=左边, 1=顶边, 2=右边, 3=底边
// hWnd = 选择夹的窗口句柄
// rcTanControl = 选择夹控件的矩形位置
// rcCalc = 接收计算出的选择夹客户区位置
inline int _tab_calc_client(HWND hWnd, const RECT& rcTabControl, RECT& rcCalc)
{
    RECT rcItem = { 0 };
    TabCtrl_GetItemRect(hWnd, 0, &rcItem);

    const int cxClient = rcTabControl.right - rcTabControl.left;
    const int cyClient = rcTabControl.bottom - rcTabControl.top;

    const DWORD dwStyle = GetWindowLongPtrW(hWnd, GWL_STYLE);
    int direction = _tab_getdirection(dwStyle);

    switch (direction)
    {
    case 0:     // 左边, 顶边=1, 右边底边各减1
    {
        rcCalc.left = rcItem.right;
        rcCalc.top = 0;
        rcCalc.right = cxClient - 1;
        rcCalc.bottom = cyClient - 1;
        break;
    }
    case 1:     // 顶边, 左边=1, 右边底边各减1
    {
        rcCalc.left = 1;
        rcCalc.top = rcItem.bottom - 1;
        rcCalc.right = cxClient - 1;
        rcCalc.bottom = cyClient - 1;
        break;
    }
    case 2:     // 右边, 左边顶边=1, 底边减1
    {
        rcCalc.left = 1;
        rcCalc.top = 0;
        rcCalc.right = rcItem.left;
        rcCalc.bottom = cyClient - 1;
        break;
    }
    default:    // 底边, 左边顶边=1, 右边减1
    {
        rcCalc.left = 1;
        rcCalc.top = 0;
        rcCalc.right = cxClient - 1;
        rcCalc.bottom = rcItem.top;
        break;
    }
    }
    return direction;
}

inline void _tab_modify_icon(bool isIcon, LPOBJSTRUCT pData)
{
    if (!isIcon)
    {
        HIMAGELIST hList = TabCtrl_GetImageList(pData->hWnd);
        SetPropW(pData->hWnd, L"ImageList", hList);
        TabCtrl_SetImageList(pData->hWnd, 0);
        return;
    }

    HIMAGELIST hList = (HIMAGELIST)GetPropW(pData->hWnd, L"ImageList");
    if (hList)
        TabCtrl_SetImageList(pData->hWnd, hList);
}


// 修改选择夹方向, 返回新的窗口样式, 计算好窗口样式并设置, 选择夹子类化处理程序会把值记录到xml里, 并保存到文件
// direction = 方向, 0=左边, 1=顶边, 2=右边, 3=底边
// pData = 选择夹的数据, 传递进来必须保证值是正确的, 内部不做检查
// style = 选择夹当前窗口样式, 如果为0, 内部会获取
// insertStyle = 要加入的样式, 为0则不加入任何样式
inline DWORD _tab_modify_direction(int direction, LPOBJSTRUCT pData, DWORD style = 0, DWORD insertStyle = 0)
{
    // 0=左边, 1=顶边, 2=右边, 3=底边
    if (style == 0) style = GetWindowLongPtrW(pData->hWnd, GWL_STYLE);
    switch (direction)
    {
    case 0:     // 左边
    {
        style &= ~(TCS_RIGHT);
        style |= TCS_VERTICAL;
        break;
    }
    case 1:     // 顶边
    {
        style &= ~(TCS_RIGHT | TCS_VERTICAL);
        break;
    }
    case 2:     // 右边
    {
        style |= (TCS_RIGHT | TCS_VERTICAL);
        break;
    }
    default:    // 底边
    {
        style &= ~(TCS_VERTICAL);
        style |= TCS_BOTTOM;
        direction = 3;
        break;
    }
    }
    style &= ~(TCS_MULTILINE);
    style |= insertStyle;
    SetWindowLongPtrW(pData->hWnd, GWL_STYLE, style);
    return style;
}

// 给易语言主窗口发送尺寸改变事件
inline void SendEWindowSize(LPOBJSTRUCT pData = 0)
{
    if (!pData)
        pData = s_info->pfnGetDataFromHwnd(s_info->hEWnd);
    if (!pData)
        return;
    WPARAM wParam = s_info->isZoomed ? SIZE_MAXIMIZED : SIZE_RESTORED;
    LPARAM lParam = MAKELONG(pData->pos.width, pData->pos.height);
    SendMessageW(s_info->hEWnd, WM_SIZE, wParam, lParam);
}


// 获取右边选择夹项目数值数组
inline std::vector<int>* prop_GetTabParams(bool isAlloc = false)
{
    std::vector<int>* params = (std::vector<int>*)GetPropW(s_info->hEWnd, GUID_TABPARAMS);
    if (!params && isAlloc)
    {
        params = new std::vector<int>;
        params->reserve(20);
        SetPropW(s_info->hEWnd, GUID_TABPARAMS, params);
    }
    return params;
}

// 判断指定项目数值是否是右边的选择夹
inline bool TabCode_IsRightTab(int param)
{
    std::vector<int>* params = prop_GetTabParams();
    if (params)
    {
        for (auto it = params->begin(); it != params->end(); ++it)
        {
            if (param == *it)
                return true;
        }
    }
    return false;
}
// 获取右边子夹数量, 没有就返回0
inline int TabCode_GetRightItemCount()
{
    std::vector<int>* params = prop_GetTabParams();
    if (!params)return 0;
    return (int)params->size();
}

// 获取所有滚动条回调记录
inline SCROLL_CALLBACK* prop_GetScrollCallback(bool isAlloc = false)
{
    SCROLL_CALLBACK* ptr = (SCROLL_CALLBACK*)GetPropW(s_info->hEWnd, GUID_SCROLL_CALLBACK_EWINDOW);
    if (!ptr && isAlloc)
    {
        ptr = new SCROLL_CALLBACK;
        SetPropW(s_info->hEWnd, GUID_SCROLL_CALLBACK_EWINDOW, ptr);
    }
    return ptr;
}

// 获取记录inlineHook的记录
inline INLINEHOOK_CALLBACK* prop_GetInlineHookCallback(bool isAlloc = false)
{
    INLINEHOOK_CALLBACK* ptr = (INLINEHOOK_CALLBACK*)GetPropW(s_info->hEWnd, GUID_INLINE_HOOK);
    if (!ptr && isAlloc)
    {
        ptr = new INLINEHOOK_CALLBACK;
        SetPropW(s_info->hEWnd, GUID_INLINE_HOOK, ptr);
    }
    return ptr;
}