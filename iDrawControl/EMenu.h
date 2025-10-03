#pragma once
#include "EWindow_Fne_Header.h"

#define IDMENU_LOCATIONFILE     0x1A500     // 定位源码文件
#define IDMENU_COPYECODEFILE    0x1A501     // 复制源码路径
#define IDMENU_LOCATIONTOOLBAR  0x1A502     // 显示/隐藏定位工具条
#define IDMENU_UNINSERT         0x1A503     // 卸载视觉库
#define IDMENU_AD               0x1A504     // 广告

#define IDMENU_PATH_E           0x1A504     // 打开易语言目录
#define IDMENU_PATH_ELIB        0x1A505     // 打开易语言支持库目录
#define IDMENU_PATH_IDRAW       0x1A506     // 打开视觉库目录
#define IDMENU_PATH_IDRAW_DATA  0x1A507     // 打开视觉库数据目录
#define IDMENU_PATH_CONFIG      0x1A508     // 打开视觉库配置文件目录
#define IDMENU_PATH_THEME       0x1A509     // 打开视觉库当前配色文件路径
#define IDMENU_PATH_PLUGIN      0x1A50A     // 打开视觉库插件目录
#define IDMENU_PATH_ET_PLUGIN   0x1A50B     // 打开助手插件目录

#define IDMENU_LOADDBGPLUGIN    0x1A510     // 加载调试插件
#define IDMENU_FREEDBGPLUGIN    0x1A511     // 卸载调试插件

#define IDMENU_HISTORYCODE_START 0x1AA00    // 历史打开的源码, 起始索引
#define IDMENU_HISTORYCODE_END  0x1AAFF     // 历史打开的源码结束索引, 最大只支持显示这么多源码
#define IDMENU_MODULE_START     0x1AB00     // 常用模块, 起始索引
#define IDMENU_MODULE_END       0x1ABFF     // 常用模块结束索引, 最大只支持显示这么多模块
#define IDMENU_SOURCECODE_START 0x1AC00     // 常用源码, 起始索引
#define IDMENU_SOURCECODE_END   0x1ACFF     // 常用源码结束索引, 最大只支持显示这么多源码

#define IDMENU_SWITCH_THEME     0xCA5B0     // 切换主题的菜单ID
#define IDMENU_SWITCH_THEME_END 0xCA6FF     // 切换主题的菜单最大ID


#define IDMENU_RUNGUID          0x1A600     // 运行guid
#define IDMENU_RUNSPY__         0x1A601     // 运行spy++
#define IDMENU_RUNERROR         0x1A602     // 运行错误查找程序

#define IDMENU_USER_START       0x1A800     // 用户开始的菜单ID

#define ID_MENU_CLOSE           0x2DA49 // 关闭
#define ID_MENU_CLOSEOTHER      0x2DA4A // 关闭其他
#define ID_MENU_CLOSEALL        0x2DA4B // 关闭全部
#define ID_MENU_CLOSE_LEFT      0x2DA4C // 关闭左边
#define ID_MENU_CLOSE_RIGHT     0x2DA4D // 关闭右边
#define ID_MENU_CLOSE_TOP       0x2DA4E // 顶部显示
#define ID_MENU_CLOSE_BOTTOM    0x2DA4F // 底部显示
#define ID_MENU_CLOSE_SHOWCLOSE 0x2DA50 // 显示关闭按钮




struct DRAWITEM_MENU_INFO
{
    HWND    hWnd;
    HMENU   hMenu;
    HDC     hdc;
    int     cxClient;
    int     cyClient;
    RECT    rc;
    bool    isTopTriangle;  // 是否需要屏蔽顶部的三角, 菜单窗口总高度大于屏幕高度-任务栏高度就需要改变
};

struct MENU_ID_DATA
{
    UINT id;
    HMENU hMenu;        // 这个菜单条目所在的菜单句柄
    int width;
    int height;
    wchar_t text[100];  // 菜单标题文本
    UINT flags;         // 菜单标识
    UINT     fType;         // used if MIIM_TYPE (4.0) or MIIM_FTYPE (>4.0)
    UINT     fState;        // used if MIIM_STATE
    HMENU    hSubMenu;      // used if MIIM_SUBMENU
    HBITMAP  hbmpChecked;   // used if MIIM_CHECKMARKS
    HBITMAP  hbmpUnchecked; // used if MIIM_CHECKMARKS
    ULONG_PTR dwItemData;   // used if MIIM_DATA
    HBITMAP  hbmpItem;      // used if MIIM_BITMAP
};

//struct EWIDNOW_MENU_DRAWINFO
//{
//    LPBRUSHPEN hPenBorder;
//
//    LPBRUSHPEN menu_down;           // 左上角菜单列的按下画刷
//    LPBRUSHPEN menu_hot;            // 左上角菜单列的热点画刷
//
//    LPBRUSHPEN hbr_StatusBar;       // 底部状态条的正常颜色, 黑色风格这个是蓝色
//    LPBRUSHPEN hbr_StatusBar_run;   // 底部状态条的运行颜色, 黑色风格这个是黄色
//
//
//
//    LPBRUSHPEN hbrText;             // 易语言窗口绘画文本时默认使用的颜色
//    LPBRUSHPEN hbrText_zhushou;     // 助手菜单显示的颜色
//    
//    LPBRUSHPEN hbrTitleBack;        // 中间绘画标题的背景画刷
//    LPBRUSHPEN hbrTitleText1;       // 中间绘画标题的文本画刷, 有焦点时的画刷
//    LPBRUSHPEN hbrTitleText2;       // 中间绘画标题的文本画刷, 没焦点时的画刷
//    LPBRUSHPEN hbrTitleText3;       // 中间绘画标题的文本画刷, 程序类型
//    LPEX_FONT  hFontTitle;          // 中间绘画标题使用的字体
//    LPEX_FONT  hFontTitle2;         // 中间绘画标题使用的字体
//
//};

struct MENU_COPY_STRUCT
{
    struct DEL_MENU
    {
        HMENU hMenu;
        UINT  hDel;
    };
    std::vector<DEL_MENU> arr_delete;  // 销毁菜单前需要删除的菜单项
    HMENU   hMenu;
    BOOL    isDestroy;
    MENU_COPY_STRUCT() : hMenu(0), isDestroy(0)
    {

    }
    ~MENU_COPY_STRUCT()
    {
        BOOL isOk = false;
        for ( DEL_MENU& item : arr_delete )
        {
            isOk = RemoveMenu(item.hMenu, item.hDel, MF_BYCOMMAND);
        }
        if ( isDestroy )
            DestroyMenu(hMenu);

    }
    inline operator HMENU()
    {
        return hMenu;
    }
};
typedef BOOL(WINAPI* FN_TrackPopupMenuEx)(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm);
FN_TrackPopupMenuEx GetTrackPopupMenuEx();
MENU_COPY_STRUCT _menu_copy_menu(HMENU hMenu, HWND hWnd);


// 初始化菜单, 枚举并隐藏易语言菜单, 新建菜单项到窗口标题位置
LPOBJSTRUCT _init_menu_(LPOBJSTRUCT pWindowData);
int __menu_calc_rect(HDC hdc, EWINDOW_MENUSTRUCT& data, bool first);

void modify_menu_style(HMENU hMenu, LPEMENU_DATA pMenu, int count);
void _apihook_menu_init(HINSTANCE hInst);
// 延时初始化hook菜单项, 弹出/启用禁止菜单, 这些API都是等窗口显示后在hook
void _apihook_menu_init_sleep();
// 将一个菜单句柄添加到重画列表里, 如果没有加入到这个列表上, 则不会重画这个菜单项及子项
void EMenu_InsertMenu(HMENU hMenu);
// 是否子类化菜单窗口, 0=不需要, 1=需要子类化, 2=不处理LoadMenu
void EMenu_IsSubClass(int code);
// 是否子类化菜单窗口, 0=不需要, 1=需要子类化, 2=不处理LoadMenu
int EMenu_GetIsSubClass();
void EMenu_Get_Help_Wnd(HMENU& hMenuWnd, HMENU& hMenuHelp);
MENU_ID_DATA* EMenu_GetMenuItemInfoFromID(UINT id);

LRESULT CALLBACK WndProc_Custom_Menu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

// 创建菜单, 这个内部会处理自绘数据
HMENU _menu_create();
// 添加菜单项, 内部会处理自绘信息
bool _menu_add(HMENU hMenu, UINT_PTR uIDNewItem, LPCWSTR lpNewItem, UINT uFlags = MF_STRING);
bool _menu_insert(HMENU hMenu, UINT uPosition, UINT_PTR uIDNewItem, LPCWSTR lpNewItem, UINT uFlags = MF_STRING);
// 弹出菜单
bool _menu_pop(HWND hWnd, HMENU hMenu, int x = 0, int y = 0);

EWINDOW_MENUSTRUCT* _menu_getindex();

// 重新计算标题位置, 标题位置是根据菜单位置和系统按钮计算的, 函数内部不会获取标题
void _window_recalc_title(LPOBJSTRUCT pData, bool isDrawMenu);

// 根据guid来决定加入到哪个菜单项下
bool MenuExt_Insert(LPCSTR guid, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItem, LPARAM param);
bool MenuExt_Delete(LPCSTR guid);
UINT_PTR MenuExt_Find(LPPLUGIN_INFO info, int id);
HWND  GetHwndFromMenu(HMENU hMenu);
HMENU GetMenuFromHwnd(HWND hWnd);
// 获取当前弹出菜单的窗口句柄
HWND GetCurrentMenuHwnd();

DRAWITEM_MENU_INFO* GetDrawItemInfo(HDC hdc);

// 添加易语言窗口的右键菜单
// 其他地方右键菜单, 各种对齐工具条, 顶边菜单  查看->工具条, 需要删除几个影响布局的菜单项
void EMenu_InsertRButton(HMENU hMenuParent);

typedef int(__cdecl* PFN_LoadStringCallback)(int id, LPWSTR lpBuffer, int cchBufferMax);
typedef int(__cdecl* PFN_EMenuCommand)(int id);

// 初始化hook加载字符串的函数
void HookLoadStringInit();
// 增加一个处理加载字符串的回调, 回调如果不处理, 请返回0
void HookLoadString_PushCallback(PFN_LoadStringCallback pfn);

// 绑定菜单被点击事件, 回调返回是否已处理, 如果回调里已经处理, 则不继续往下分发事件
void EMenu_PushCommand(PFN_EMenuCommand pfn);
void EMenu_CustomMenu(EWINDOW_MENUSTRUCT* menu);
// 菜单栏菜单项目被改变
void EMenu_MenuBarChange();
// 获取配置文件里的隐藏菜单列表, str是存放获取到的菜单列表字符串, arr存放每个菜单的起始地址
int CustomMenu_GetMenuList(_str& str, std::vector<LPCWSTR>& arr);
// 把指定菜单名从菜单列表里删除, 删除后菜单会显示在菜单栏里, 返回0表示成功, 1表示菜单名不存在
bool CustomMenu_RemoveMenuList(LPCWSTR menu_name);
// 把指定菜单名加入到菜单列表里, 加入后这个菜单名就不显示在菜单栏里, 返回0表示成功, 1表示菜单名已存在
bool CustomMenu_InsertMenuList(LPCWSTR menu_name);


// 获取文件内容, 返回文件尺寸, 返回0表示读取失败, 或者文件没有内容
inline int _get_file_data(LPCWSTR file, _str& ret)
{
    CFileRW f;
    if (!f.open(file))
        return 0;
    int size = f.size();
    if (size <= 0)
        return 0;
    ret.resize(size);
    f.read((LPBYTE)ret.data(), size);
    return size;
}
