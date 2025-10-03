#include "iConfig.h"
#include <control/ReadColor.h>
#include <iDraw_const.h>

#define _NEW_PEN_(_name)   _name = s_info->pfnCreatePenDx  (RGB2ARGB(newValue, 255), 1.f)
#define _NEW_BRUSH_(_name) _name = s_info->pfnCreateBrushDx(RGB2ARGB(newValue, 255))


inline void _window_changed_color(COLORREF crShadow, COLORREF crBorder)
{
    LPOBJSTRUCT pData = s_info->pfnGetDataFromHwnd(s_info->hEWnd);
    if (!pData)return;

    SendMessageW(s_info->hEWnd, WM_ACTIVATEAPP, s_info->isActivation, 0);
    return;
    crShadow = RGB2ARGB(crShadow, 255);
    crBorder = RGB2ARGB(crBorder, 255);
    s_info->shadow->SetColor(crShadow, crBorder, true);
    RECT rc = { 0 };
    rc.left = 0;
    rc.top = 0;
    rc.right = pData->pos.rcClient.right;
    rc.bottom = WINDOW_CAPTION_HEIGHT;
    InvalidateRect(s_info->hEWnd, &rc, 0);      // 重画顶边标题栏
    if (s_info->rcStatusBar.bottom - s_info->rcStatusBar.top > 0)   // 需要重画状态条
    {
        rc.top = s_info->rcStatusBar.top;
        rc.bottom = pData->pos.rcClient.bottom;
        InvalidateRect(s_info->hEWnd, &rc, 0);
    }

}

// 默认配色被改变, 需要把所有子类化的默认配色改变
// 还需要判断哪些配色是被改变, 哪些是默认的
inline void _config_defcolor_change(int index, LONG_PTR oldValue, LONG_PTR newValue, bool isUpdateWindow)
{

}

// 跟边框相关的都进来这里处理, 会把画刷和画笔都设置成新的颜色
inline void _config_boeder_change(int index, LONG_PTR oldValue, LONG_PTR newValue, bool isUpdateWindow)
{
    IThemeColor& theme = *s_info->themeColor;
    THEME_D2D_INFO& d2d = s_info->window_d2d_info;

    switch (index)
    {
    case IGI_CONFIG_CRBORDER:           // 窗口边框颜色配置
    {
        _NEW_PEN_(d2d.hPenBorder);
        _NEW_BRUSH_(d2d.hbrBorder);
        if (isUpdateWindow)
            _window_changed_color(theme.crShadow(), newValue);
        break;
    }
    case IGI_CONFIG_CRSHADOW:           // 窗口阴影颜色配置
    {
        _NEW_PEN_(d2d.hPenShadow);
        _NEW_BRUSH_(d2d.hbrShadow);
        if (isUpdateWindow)
            _window_changed_color(newValue, theme.crBorder());
        break;
    }
    case IGI_CONFIG_CRBORDER_RUN:       // 运行时窗口边框的颜色
    {
        _NEW_PEN_(d2d.hPenBorder_run);
        _NEW_BRUSH_(d2d.hbrBorder_run);
        if (isUpdateWindow)
            _window_changed_color(theme.crShadow_run(), newValue);
        break;
    }
    case IGI_CONFIG_CRSHADOW_RUN:       // 运行时窗口阴影颜色
    {
        _NEW_PEN_(d2d.hPenShadow_run);
        _NEW_BRUSH_(d2d.hbrShadow_run);
        if (isUpdateWindow)
            _window_changed_color(newValue, theme.crBorder_run());
        break;
    }
    case IGI_CONFIG_CRBORDER_NF:        // 失去焦点时窗口边框的颜色
    {
        _NEW_PEN_(d2d.hPenBorder_nf);
        _NEW_BRUSH_(d2d.hbrBorder_nf);
        if (isUpdateWindow)
            _window_changed_color(theme.crShadow_nf(), newValue);
        break;
    }
    case IGI_CONFIG_CRSHADOW_NF:        // 失去焦点时窗口阴影颜色
    {
        _NEW_PEN_(d2d.hPenShadow_nf);
        _NEW_BRUSH_(d2d.hbrShadow_nf);
        if (isUpdateWindow)_window_changed_color(newValue, theme.crBorder_nf());
        break;
    }

    case IGI_CONFIG_CRMENUBORDER:           // 菜单项的边框颜色
    {
        _NEW_PEN_(d2d.hPenMenuBorder);
        _NEW_BRUSH_(d2d.hbrMenuBorder);
        break;
    }
    default:
        break;
    }
}
inline void _config_scroll_change(int index, LONG_PTR oldValue, LONG_PTR newValue, bool isUpdateWindow)
{
    //! 触发这个函数之后还会触发绘画插件配置被改变, 需要在绘画插件里修改滚动条配色
    // 滚动条绘画时每次都会创建画刷, 只要枚举出滚动条对象修改颜色就可以
    //int size = s_info->pfnGetAllSubclassWindow(0, 0);
    //LPOBJSTRUCT* arr = new LPOBJSTRUCT[size];
    //s_info->pfnGetAllSubclassWindow(arr, size);
    //for (int i = 0; i < size; i++)
    //{
    //    IDrawScroll* pScroll = 
    //    if()
    //}

}
inline void _config_customtabcode_change(int index, LONG_PTR oldValue, LONG_PTR newValue, bool isUpdateWindow)
{
    iConfig_Reload_CustomTabColor(true);
    ICustomCodeTab& arr = *s_info->hWnds;
    IThemeColor& theme = *s_info->themeColor;
    for (int i = 0; i < arr.size(); i++)
    {
        CUSTOMTAB_DATA& CustomTabData = arr[i];
        switch (CustomTabData.type)
        {
        case CUSTOMTAB_INFO_UNDEFINE:           // 未定义
            break;
        case CUSTOMTAB_INFO_WINDOW:             // 窗口设计器
            CustomTabData.crText = theme.crTab_text_window();
            break;
        case CUSTOMTAB_INFO_ASSEMBLY:           // 普通程序集
            CustomTabData.crText = theme.crTab_text_assembly();
            break;
        case CUSTOMTAB_INFO_ASSEMBLY_WINDOW:    // 窗口程序集
            CustomTabData.crText = theme.crTab_text_assembly_window();
            break;
        case CUSTOMTAB_INFO_CLASS:              // 类
            CustomTabData.crText = theme.crTab_text_class();
            break;
        case CUSTOMTAB_INFO_GLOBALVAR:          // 全局变量
            CustomTabData.crText = theme.crTab_text_globalvar();
            break;
        case CUSTOMTAB_INFO_STRUCT:             // 数据类型
            CustomTabData.crText = theme.crTab_text_struct();
            break;
        case CUSTOMTAB_INFO_DLL:                // DLL命令定义表
            CustomTabData.crText = theme.crTab_text_dll();
            break;
        case CUSTOMTAB_INFO_CONST:              // 常量
            CustomTabData.crText = theme.crTab_text_const();
            break;
        case CUSTOMTAB_INFO_IMAGE:              // 图片资源
            CustomTabData.crText = theme.crTab_text_image();
            break;
        case CUSTOMTAB_INFO_SOUND:              // 声音资源
            CustomTabData.crText = theme.crTab_text_sound();
            break;
        case CUSTOMTAB_INFO_CLOSEHOT:           // 关闭按钮热点
            CustomTabData.crText = theme.close_hot();
            break;
        case CUSTOMTAB_INFO_CLOSEDOWN:          // 关闭按钮按下
            CustomTabData.crText = theme.close_down();
            break;
        default:
            break;
        }
    }
    InvalidateRect(s_info->hEWnd, &s_info->rcCustomTab, 0);
    InvalidateRect(s_info->eWnd->GetWorkProgramTree(), 0, 0);
}
inline void _config_change_color(int index, LONG_PTR oldValue, LONG_PTR newValue, bool isUpdateWindow)
{
    IThemeColor& theme = *s_info->themeColor;
    THEME_D2D_INFO& d2d = s_info->window_d2d_info;

    switch (index)
    {
    // 默认配色
    case IGI_CONFIG_CRDEF_BACK:         // [DefaultColor], 默认配色, 背景颜色
    case IGI_CONFIG_CRDEF_NORMAL:       // [DefaultColor], 默认配色, 正常颜色
    case IGI_CONFIG_CRDEF_HOT:          // [DefaultColor], 默认配色, 热点颜色
    case IGI_CONFIG_CRDEF_DOWN:         // [DefaultColor], 默认配色, 按下颜色
    case IGI_CONFIG_CRDEF_BAN:          // [DefaultColor], 默认配色, 禁止颜色
    case IGI_CONFIG_CRDEF_HOVER:        // [DefaultColor], 默认配色, 进入颜色
    case IGI_CONFIG_CRDEF_FOCUS:        // [DefaultColor], 默认配色, 焦点颜色
    case IGI_CONFIG_CRDEF_CHECKED:      // [DefaultColor], 默认配色, 选中颜色
    case IGI_CONFIG_CRDEF_SELECTED:     // [DefaultColor], 默认配色, 选择颜色
    case IGI_CONFIG_CRDEF_VISTED:       // [DefaultColor], 默认配色, 
    case IGI_CONFIG_CRDEF_SHADOW:       // [DefaultColor], 默认配色, 阴影颜色
    case IGI_CONFIG_CRDEF_BORDER:       // [DefaultColor], 默认配色, 边框颜色
    case IGI_CONFIG_CRDEF_TEXT:         // [DefaultColor], 默认配色, 文本颜色
    case IGI_CONFIG_CRDEF_TEXTHOT:      // [DefaultColor], 默认配色, 文本热点颜色
    case IGI_CONFIG_CRDEF_TEXTDOWN:     // [DefaultColor], 默认配色, 文本按下颜色
    case IGI_CONFIG_CRDEF_TEXTBAN:      // [DefaultColor], 默认配色, 文本禁止颜色
        _config_defcolor_change(index, oldValue, newValue, isUpdateWindow);
        break;
    case IGI_CONFIG_CRBACK:             // 背景颜色
    {
        _NEW_BRUSH_(d2d.hbrBack);
        if (isUpdateWindow)
            InvalidateRect(s_info->hEWnd, 0, 0);   // 重画整个窗口
        break;
    }
    case IGI_CONFIG_CRBACKEXTERN:       // 外部的背景颜色, 比如选择夹的子夹
    {
        //TODO 需要把所有用到这个颜色的组件都重画, 这个颜色暂时没有创建dx绘画对象
        if (isUpdateWindow)
            InvalidateRect(s_info->hEWnd, 0, 0);   // 重画整个窗口
        break;
    }
    case IGI_CONFIG_CRSTATEBAR:         // 组件箱/工作夹/状态夹顶部激活的颜色
    {
        // 绘画顶部状态条的颜色是每次绘画都从配置里获取然后创建, 改配置就行了
        break;
    }
    case IGI_CONFIG_CRTEXT:             // 一般文本颜色
    {
        _NEW_BRUSH_(d2d.hbrText);
        if (isUpdateWindow)
            InvalidateRect(s_info->hEWnd, 0, 0);   // 重画整个窗口
        break;
    }
    // 边框/阴影配色 0x0230 - 0x023F
    case IGI_CONFIG_CRBORDER:               // 窗口边框颜色
    case IGI_CONFIG_CRBORDER_RUN:           // 窗口边框运行时的颜色
    case IGI_CONFIG_CRBORDER_NF:            // 窗口边框失去焦点时的颜色
    case IGI_CONFIG_CRSHADOW:               // 窗口阴影颜色
    case IGI_CONFIG_CRSHADOW_RUN:           // 窗口阴影运行时的颜色
    case IGI_CONFIG_CRSHADOW_NF:            // 窗口阴影失去焦点时的颜色
    case IGI_CONFIG_CRMENUBORDER:           // 菜单项的边框颜色
        _config_boeder_change(index, oldValue, newValue, isUpdateWindow);
        break;

    // 菜单项 0x0240 - 0x024F
    case IGI_CONFIG_CRMENUBACK:             // 菜单项的背景颜色, 没有热点, 不是禁止时使用这个颜色绘画
        _NEW_BRUSH_(d2d.hbrMenuBack);
        _NEW_BRUSH_(d2d.hbrMenuDown);
        break;
    case IGI_CONFIG_CRMENUHOT:              // 菜单项的热点颜色
        _NEW_BRUSH_(d2d.hbrMenuHot);
        break;
    case IGI_CONFIG_CRMENUBAN:              // 菜单项的禁止颜色
        _NEW_BRUSH_(d2d.hbrMenuBan);
        break;
    case IGI_CONFIG_CRMENUTEXT:             // 菜单项正常文本颜色
        _NEW_BRUSH_(d2d.hbrMenuText);
        break;
    case IGI_CONFIG_CRMENUTEXTHOT:          // 菜单项热点文本颜色
        _NEW_BRUSH_(d2d.hbrMenuTextHot);
        break;
    case IGI_CONFIG_CRMENUTEXTBAN:          // 菜单项禁止文本颜色
        _NEW_BRUSH_(d2d.hbrMenuTextBan);
        break;

    // 显示在标题栏那里的菜单夹 0x0250 - 0x025F
    case IGI_CONFIG_CRMENUBARHOT:           // 顶部菜单夹热点颜色
        _NEW_BRUSH_(d2d.hbrMenuBarHot);
        if (isUpdateWindow)InvalidateRect(s_info->hEWnd, &s_info->rcMenu, 0);   // 重画菜单夹
        break;
    case IGI_CONFIG_CRMENUBARTEXT:          // 顶部菜单夹助手菜单的文本颜色
        _NEW_BRUSH_(d2d.hbrMenuBarToolsText);
        if (isUpdateWindow)InvalidateRect(s_info->hEWnd, &s_info->rcMenu, 0);   // 重画菜单夹
        break;
    case IGI_CONFIG_CRMENUBARTEXTHOT:       // 顶部菜单夹助手菜单的文本热点颜色
        _NEW_BRUSH_(d2d.hbrMenuBarToolsTextHot);
        if (isUpdateWindow)InvalidateRect(s_info->hEWnd, &s_info->rcMenu, 0);   // 重画菜单夹
        break;

    // 显示在标题栏里黑色块的配色 0x0260 - 0x026F
    case IGI_CONFIG_CRTITLEBACK:            // 顶边黑块部分背景颜色
    case IGI_CONFIG_CRTITLELEFT_ACT:        // 顶部黑块部分左边激活时文本颜色, 这个是源码名或者项目名
    case IGI_CONFIG_CRTITLERIGHT_ACT:       // 顶部黑块部分右边激活时文本颜色, 这个是程序类型, 或者未保存时的星号使用的颜色
    case IGI_CONFIG_CRTITLELEFT:            // 顶部黑块部分左边文本颜色, 这个是源码名或者项目名
    case IGI_CONFIG_CRTITLERIGHT:           // 顶部黑块部分右边文本颜色, 这个是程序类型, 或者未保存时的星号使用的颜色
    {
        switch (index)
        {
        case IGI_CONFIG_CRTITLEBACK:            // 顶边黑块部分背景颜色
            _NEW_BRUSH_(d2d.hbrTitleback); break;
        case IGI_CONFIG_CRTITLELEFT_ACT:        // 顶部黑块部分左边激活时文本颜色, 这个是源码名或者项目名
            _NEW_BRUSH_(d2d.hbrTitle_left_act); break;
        case IGI_CONFIG_CRTITLERIGHT_ACT:       // 顶部黑块部分右边激活时文本颜色, 这个是程序类型, 或者未保存时的星号使用的颜色
            _NEW_BRUSH_(d2d.hbrTitle_right_act); break;
        case IGI_CONFIG_CRTITLELEFT:            // 顶部黑块部分左边文本颜色, 这个是源码名或者项目名
            _NEW_BRUSH_(d2d.hbrTitle_left); break;
        default:
            _NEW_BRUSH_(d2d.hbrTitle_right); break;
        }
        RECT rc;
        GetClientRect(s_info->hEWnd, &rc);
        rc.bottom = s_info->rcMenu.bottom;
        if (isUpdateWindow)InvalidateRect(s_info->hEWnd, &rc, 0);   // 重画顶部标题栏位置
        break;
    }

    // 底部状态条的配色 0x0270 - 0x027F
    case IGI_CONFIG_CRTEXTSTATUSBAR:        // 底部状态条的文本颜色, 如果对应颜色没有创建画刷或者画笔则返回0
    case IGI_CONFIG_CRTEXTSTATUSBAR_RUN:    // 底部状态条运行时的文本颜色, 如果对应颜色没有创建画刷或者画笔则返回0
    case IGI_CONFIG_CRTEXTSTATUSBAR_NF:     // 底部状态条不激活的文本颜色, 如果对应颜色没有创建画刷或者画笔则返回0
    {
        switch (index)
        {
        case IGI_CONFIG_CRTEXTSTATUSBAR:        // 底部状态条的文本颜色, 如果对应颜色没有创建画刷或者画笔则返回0
            _NEW_BRUSH_(d2d.hbrTextStatusBar);
            break;
        case IGI_CONFIG_CRTEXTSTATUSBAR_RUN:    // 底部状态条运行时的文本颜色, 如果对应颜色没有创建画刷或者画笔则返回0
            _NEW_BRUSH_(d2d.hbrTextStatusBar_run);
            break;
        default:
            _NEW_BRUSH_(d2d.hbrTextStatusBar_nf);
            break;
        }
        RECT rc;
        GetClientRect(s_info->hEWnd, &rc);
        rc.top = s_info->rcStatusBar.top;
        if (isUpdateWindow)InvalidateRect(s_info->hEWnd, &rc, 0);   // 重画底边状态夹位置
        break;
    }

    // 滚动条配色 0x0280 - 0x028F
    case IGI_CONFIG_CRSCROLLBTN1NORRMAL:    // 滚动条按钮1正常颜色, 左边或者顶边的小按钮
    case IGI_CONFIG_CRSCROLLBTN1HOT:        // 滚动条按钮1热点颜色, 左边或者顶边的小按钮
    case IGI_CONFIG_CRSCROLLBTN1DOWN:       // 滚动条按钮1按下颜色, 左边或者顶边的小按钮
    case IGI_CONFIG_CRSCROLLBTN2NORRMAL:    // 滚动条按钮2正常颜色, 右边或者底边的小按钮
    case IGI_CONFIG_CRSCROLLBTN2HOT:        // 滚动条按钮2热点颜色, 右边或者底边的小按钮
    case IGI_CONFIG_CRSCROLLBTN2DOWN:       // 滚动条按钮2按下颜色, 右边或者底边的小按钮
    case IGI_CONFIG_CRSCROLLTHUMBNORRMAL:   // 滚动条滑块颜色
    case IGI_CONFIG_CRSCROLLTHUMBHOT:       // 滚动条滑块热点颜色
    case IGI_CONFIG_CRSCROLLTHUMBDOWN:      // 滚动条滑块按下颜色
    case IGI_CONFIG_CRSCROLLBACKGROUND:     // 滚动条背景颜色
        _config_scroll_change(index, oldValue, newValue, isUpdateWindow);
        break;
    // 分割线颜色 0x0290 - 0x029F
    case IGI_CONFIG_CRLINELEFT:             // 工具条, 超列表头, 等组件的分割线左边颜色, 分割线都是两个像素
    case IGI_CONFIG_CRLINERIGHT:            // 工具条, 超列表头, 等组件的分割线右边颜色, 分割线都是两个像素
        break;

        // 自绘代码切换选择夹关闭按钮颜色, 0x02A0 - 0x02AF
    case IGI_CONFIG_CODETAB_CRCLOSEHOT:             // [CustomCodeTab], 代码切换选择夹关闭按钮热点颜色
    case IGI_CONFIG_CODETAB_CRCLOSEDOWN:            // [CustomCodeTab], 代码切换选择夹关闭按钮按下颜色
    // 自绘代码切换选择夹文本颜色, 0x02B0 - 0x02BF
    case IGI_CONFIG_CODETAB_CRTEXT_WINDOW:          // [CustomCodeTab], 代码切换选择夹, 窗口文本颜色
    case IGI_CONFIG_CODETAB_CRTEXT_ASSEMBLY:        // [CustomCodeTab], 代码切换选择夹, 程序集文本颜色
    case IGI_CONFIG_CODETAB_CRTEXT_ASSEMBLY_WINDOW: // [CustomCodeTab], 代码切换选择夹, 窗口程序集文本颜色
    case IGI_CONFIG_CODETAB_CRTEXT_CLASS:           // [CustomCodeTab], 代码切换选择夹, 类文本颜色
    case IGI_CONFIG_CODETAB_CRTEXT_GLOBALVAR:       // [CustomCodeTab], 代码切换选择夹, 全局变量文本颜色
    case IGI_CONFIG_CODETAB_CRTEXT_STRUCT:          // [CustomCodeTab], 代码切换选择夹, 自定义数据类型文本颜色
    case IGI_CONFIG_CODETAB_CRTEXT_DLL:             // [CustomCodeTab], 代码切换选择夹, DLL文本颜色
    case IGI_CONFIG_CODETAB_CRTEXT_CONST:           // [CustomCodeTab], 代码切换选择夹, 常量文本颜色
    case IGI_CONFIG_CODETAB_CRTEXT_IMAGE:           // [CustomCodeTab], 代码切换选择夹, 图片文本颜色
    case IGI_CONFIG_CODETAB_CRTEXT_SOUND:           // [CustomCodeTab], 代码切换选择夹, 声音文本颜色
    // 自绘代码切换选择夹底下线条颜色, 0x02C0 - 0x02CF
    case IGI_CONFIG_CODETAB_CRLINE_WINDOW:          // [CustomCodeTab], 代码切换选择夹, 窗口下边线颜色
    case IGI_CONFIG_CODETAB_CRLINE_ASSEMBLY:        // [CustomCodeTab], 代码切换选择夹, 程序集下边线颜色
    case IGI_CONFIG_CODETAB_CRLINE_ASSEMBLY_WINDOW: // [CustomCodeTab], 代码切换选择夹, 窗口程序集下边线颜色
    case IGI_CONFIG_CODETAB_CRLINE_CLASS:           // [CustomCodeTab], 代码切换选择夹, 类下边线颜色
    case IGI_CONFIG_CODETAB_CRLINE_GLOBALVAR:       // [CustomCodeTab], 代码切换选择夹, 全局变量下边线颜色
    case IGI_CONFIG_CODETAB_CRLINE_STRUCT:          // [CustomCodeTab], 代码切换选择夹, 自定义数据类型下边线颜色
    case IGI_CONFIG_CODETAB_CRLINE_DLL:             // [CustomCodeTab], 代码切换选择夹, DLL下边线颜色
    case IGI_CONFIG_CODETAB_CRLINE_CONST:           // [CustomCodeTab], 代码切换选择夹, 常量下边线颜色
    case IGI_CONFIG_CODETAB_CRLINE_IMAGE:           // [CustomCodeTab], 代码切换选择夹, 图片下边线颜色
    case IGI_CONFIG_CODETAB_CRLINE_SOUND:           // [CustomCodeTab], 代码切换选择夹, 声音下边线颜色
        _config_customtabcode_change(index, oldValue, newValue, isUpdateWindow);
        break;
    default:
        break;
    }

}
void WINAPI iConfig_InfoChanged(int index, LONG_PTR oldValue, LONG_PTR newValue, bool isUpdateWindow)
{
    LPOBJSTRUCT pData;
    switch (index)
    {
    case IGI_CONFIG_ISDXRENDER:          // 是否使用DX渲染, 设置需要重启生效
        break;  // DX渲染需要重启
    case IGI_CONFIG_ISRUNSAVE:           // 运行/编译后是否保存源码
        break;  // 这个设置是助手在处理, 视觉库暂时不使用
    case IGI_CONFIG_WHEELPOS:            // 鼠标滚动一次的距离
        break;  // 这个值修改后实时生效, 在滚动那里每次都读出来滚动
    case IGI_CONFIG_NOCLIPCHILDREN:      // 父窗口绘画时不排除子窗口区域
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->hEWnd);
        if (!pData)break;

        DWORD style = pData->GetWndLong(pData->hWnd, GWL_STYLE);
        style = (newValue ? (style & ~WS_CLIPCHILDREN) : (style | WS_CLIPCHILDREN));
        pData->SetWndLong(pData->hWnd, GWL_STYLE, style);
        break;
    }
    case IGI_CONFIG_ISDPIAWARE:
        break;
    case IGI_CONFIG_ISDPISCALEWND:
        break;
    case IGI_CONFIG_SHOWCODETABCLOSE:
        if ( isUpdateWindow )
            InvalidateRect(s_info->hEWnd, 0, 0);
        break;

    case IGI_CONFIG_MENUITEMHEIGHT:         // 菜单项高度
    case IGI_CONFIG_MENUSEPARATORHEIGHT:    // 分隔条菜单项高度
        break;  // 重启生效
    case IGI_CONFIG_ISLOCATIONTOOLBAR:      // 是否显示定位工具条
    {
        const bool isShow = newValue != 0;
        if (!isShow)
        {
            static HWND hComboBox1 = s_info->eWnd->GetMenuComboBox1();
            static HWND hComboBox2 = s_info->eWnd->GetMenuComboBox2();
            if (!IsWindow(hComboBox1))break;
            ShowWindowEx(hComboBox1, SW_HIDE);
            ShowWindowEx(hComboBox2, SW_HIDE);
        }
        SendEWindowSize();
        break;
    }
    case IGI_CONFIG_PROGRAMDBLCLKTYPE:      // 程序树型框双击模式, 0=只切换, 1=只展开, 2=展开+切换
        break;  // 这个是每次使用时获取


    case IGI_CONFIG_NOCHANGETAB_WORK:   // 不自动切换工作夹
        break;  // 这个是每次使用时获取
    case IGI_CONFIG_NOCHANGETAB_STATE:  // 不自动切换状态夹
        break;  // 这个是每次使用时获取
    case IGI_CONFIG_SCROLL_WIDTH:       // 滚动条宽度
    case IGI_CONFIG_SCROLL_BTN_SIZE:    // 滚动条按钮尺寸
        if ( isUpdateWindow )
            InvalidateRect(s_info->hEWnd, 0, 0);
        break;
    case IGI_CONFIG_WORK_DIRECTION:     // 工作夹的子夹方向, 0=左边, 1=顶边, 2=右边, 3=底边
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetWorkTab());
        if (pData)
            _tab_modify_direction(newValue, pData);

        break;
    }
    case IGI_CONFIG_WORK_ISICON:       // 左边的工作夹是否显示图标
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetWorkTab());
        if (pData)
            _tab_modify_icon(newValue, pData);
        break;
    }
    //case IGI_CONFIG_WORK_ISSHOW:       // 工作夹是否显示, 如果为false, 则失去焦点就隐藏起来
    //    break;  // 暂时还没使用
    //case IGI_CONFIG_WORK_POS:           // 工作夹位置, 0=左边, 1=顶边, 2=右边, 3=底边
    //    break;  // 暂时还没使用
    case IGI_CONFIG_WORK_FLAGS:         // 工作夹标识, 记录位置, 是否最大化, 是否浮动, 是否停靠等信息 TOP_STATE_BUTTON 枚举常量里的值, 调用 __query_flags 判断值
        break;
    case IGI_CONFIG_STATE_DIRECTION:    // 状态夹的子夹方向, 0=左边, 1=顶边, 2=右边, 3=底边
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetStateTab());
        if (pData)
            _tab_modify_direction(newValue, pData);
        break;
    }
    case IGI_CONFIG_STATE_ISICON:     // 底下的状态选择夹是否显示图标
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetStateTab());
        if (pData)
            _tab_modify_icon(newValue, pData);
        break;
    }
    //case IGI_CONFIG_STATE_ISSHOW:       // 状态夹是否显示, 如果为false, 则失去焦点就隐藏起来
    //    break;  // 暂时还没使用
    //case IGI_CONFIG_STATE_POS:          // 状态夹位置, 0=左边, 1=顶边, 2=右边, 3=底边
    //    break;  // 暂时还没使用
    case IGI_CONFIG_STATE_FLAGS:        // 状态夹标识, 记录位置, 是否最大化, 是否浮动, 是否停靠等信息 TOP_STATE_BUTTON 枚举常量里的值, 调用 __query_flags 判断值
        break;
    case IGI_CONFIG_CODE_DIRECTION:     // 代码选择夹的子夹方向, 3=底边, 其他 = 顶边
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->hEWnd);
        SendEWindowSize(pData); // 给易语言主窗口发送改变尺寸的事件, 重新调整布局
        break;
    }
    case IGI_CONFIG_CODE_ISICON:        // 代码切换选择夹是否显示图标
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->hEWnd);
        if (!pData)break;
        const int hi = HIWORD(pData->draw.nItemHeight);
        const int lo = LOWORD(pData->draw.nItemHeight) + (newValue ? 20 : -20);
        pData->draw.nItemHeight = MAKELONG(lo, hi);
        TabCtrl_SetPadding(s_info->hEWnd, lo, 0);
        SetWindowPos(pData->hWnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME);
        InvalidateRect(pData->hWnd, 0, 0);
        break;
    }
    //case IGI_CONFIG_CONTROL_ISSHOW:     // 组件箱是否显示, 如果为false, 则失去焦点就隐藏起来
    //    break;  // 暂时还没使用
    //case IGI_CONFIG_CONTROL_POS:        // 组件箱位置, 0=左边, 1=顶边, 2=右边, 3=底边
    //    break;  // 暂时还没使用
    case IGI_CONFIG_CONTROL_FLAGS:      // 组件箱标识, 记录位置, 是否最大化, 是否浮动, 是否停靠等信息 TOP_STATE_BUTTON 枚举常量里的值, 调用 __query_flags 判断值
        break;

    case IGI_CONFIG_TREE_FNE_ISHOT:         // 支持库树型框是否支持热点
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetWorkFneTree());
        if (pData)
            pData->draw.isHot = newValue;
        break;
    }
    case IGI_CONFIG_TREE_PROGRAM_ISHOT:     // 程序树型框是否支持热点
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetWorkProgramTree());
        if (pData)
            pData->draw.isHot = newValue;
        break;
    }
    case IGI_CONFIG_TREE_CONTROL_ISHOT:     // 组件箱树型框是否支持热点
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetControlTree());
        if (pData)
            pData->draw.isHot = newValue;
        break;
    }
    case IGI_CONFIG_TREE_CONTROL_ORGICON:   // 组件箱树型框显示原图标
    {
        //! TVM_SETITEMW wParam == 121007124 && lParam == 20752843 的时候就是通知组件箱显示原图标的配置被改变了, 需要重新创建图片组
        SendMessageW(s_info->eWnd->GetControlTree(), TVM_SETITEMW, 121007124, 20752843);
        InvalidateRect(s_info->eWnd->GetControlTree(), 0, 0);
        break;  // 这个是每次绘画树型框项目都会获取, 这里需要重画
    }
    case IGI_CONFIG_TREE_VAR_ISHOT:         // 变量表树型框是否支持热点
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetStateVarTableTree());
        if (pData)
            pData->draw.isHot = newValue;
        break;
    }
    case IGI_CONFIG_TREE_CALL_ISHOT:        // 调用表树型框是否支持热点
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetStateTabCallTable());
        if (pData)
            pData->draw.isHot = newValue;
        break;
    }
    case IGI_CONFIG_LIST_FIND1_ISHOT:       // 搜寻1列表框是否支持热点
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetStateTabFind1());
        if (pData)
            pData->draw.isHot = newValue;
        break;
    }
    case IGI_CONFIG_LIST_FIND2_ISHOT:       // 搜寻2列表框树型框是否支持热点
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetStateTabFind2());
        if (pData)
            pData->draw.isHot = newValue;
        break;
    }

    case IGI_CONFIG_TREE_FNE_HEIGHT:        // 支持库树型框项目高度
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetWorkFneTree());
        if (!pData) break;
        pData->draw.nItemHeight = newValue;
        SendMessageW(pData->hWnd, TVM_SETITEMHEIGHT, pData->draw.nItemHeight, 0);
        break;
    }
    case IGI_CONFIG_TREE_PROGRAM_HEIGHT:    // 程序树型框项目高度
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetWorkProgramTree());
        if (!pData) break;
        pData->draw.nItemHeight = newValue;
        SendMessageW(pData->hWnd, TVM_SETITEMHEIGHT, pData->draw.nItemHeight, 0);
        break;
    }
    case IGI_CONFIG_TREE_CONTROL_HEIGHT:    // 组件箱树型框项目高度
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetControlTree());
        if (!pData) break;
        pData->draw.nItemHeight = newValue;
        SendMessageW(pData->hWnd, TVM_SETITEMHEIGHT, pData->draw.nItemHeight, 0);
        break;
    }
    case IGI_CONFIG_TREE_VAR_HEIGHT:        // 变量表树型框项目高度
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetStateVarTableTree());
        if (!pData) break;
        pData->draw.nItemHeight = newValue;
        SendMessageW(pData->hWnd, TVM_SETITEMHEIGHT, pData->draw.nItemHeight, 0);
        break;
    }
    case IGI_CONFIG_TREE_CALL_HEIGHT:       // 调用表树型框项目高度
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetStateTabCallTable());
        if (!pData) break;
        pData->draw.nItemHeight = newValue;
        SendMessageW(pData->hWnd, TVM_SETITEMHEIGHT, pData->draw.nItemHeight, 0);
        break;
    }
    case IGI_CONFIG_LIST_FIND1_HEIGHT:      // 搜寻1列表框项目高度
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetStateTabFind1());
        if (!pData) break;
        pData->draw.nItemHeight = newValue;
        SendMessageW(pData->hWnd, LB_SETITEMHEIGHT, pData->draw.nItemHeight, 0);
        break;
    }
    case IGI_CONFIG_LIST_FIND2_HEIGHT:      // 搜寻2列表框项目高度
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetStateTabFind2());
        if (!pData) break;
        pData->draw.nItemHeight = newValue;
        SendMessageW(pData->hWnd, LB_SETITEMHEIGHT, pData->draw.nItemHeight, 0);
        break;
    }
    default:
        if (index >= 0x200 && index < 0x400)    // 0x0200 - 0x03FF 是颜色配色
            _config_change_color(index, oldValue, newValue, isUpdateWindow);
        break;
    }
}