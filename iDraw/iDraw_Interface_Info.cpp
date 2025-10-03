#if 1
#include "EWindow_Fne_Main.h"

// 设置或者获取配置的颜色
inline bool _info_case_color(int index, LONG_PTR value, LONG_PTR& ret, bool isSet, int argument)
{
    IThemeColor& theme = *s_info->themeColor;
    COLORINFO& color = s_info->defData->draw.color;

    // 取值, 返回对象
#define _info_ret_obj(_cr) \
        ret = _cr;\
        if (argument == IGICR_BRUSH)\
            ret = (LONG_PTR)s_info->pfnCreateBrush((COLORREF)ret);\
        else if (argument == IGICR_PEN)\
            ret = (LONG_PTR)s_info->pfnCreatePen(PS_SOLID, 1, (COLORREF)ret);\
        else if (argument == IGICR_BRUSHDX)\
            ret = (LONG_PTR)s_info->pfnCreateBrushDx(RGB2ARGB((COLORREF)ret, 255));\
        else if (argument == IGICR_PENDX)\
            ret = (LONG_PTR)s_info->pfnCreatePenDx(RGB2ARGB((COLORREF)ret, 255), 1.f);\
        break

    // 如果是通过配置被改变事件是不会走到这个函数, 所以这里也得修改默认颜色的值
    // 走完这个函数会调用配置改变的函数, 那里需要根据改变的配置做相应的处理
    // 这里把默认色的绘画对象和颜色值都改变了, 设置插件里的配置改变就只处理所有窗口的默认值
    // 这个函数调用后会调用 iConfig_InfoChanged(), 默认色会调用 _config_defcolor_change()

#define _info_color(_case, _fun) \
    case _case:\
        if(isSet){\
            ret = (LONG_PTR)_fun((COLORREF*)&value);\
            break;\
        }\
        _info_ret_obj(_fun())


#define _info_defcolor(_case, _cr, _hbr) \
    case _case:\
        if(isSet){\
            ret = (LONG_PTR)_cr;\
            _cr = (COLORREF)value;\
            _hbr = s_info->pfnCreateBrush(_cr);\
            break;\
        }\
        _info_ret_obj(_cr)


#define _info_pencolor(_case, _cr, _hpen) \
    case _case:\
        if(isSet){\
            ret = (LONG_PTR)_cr;\
            _cr = (COLORREF)value;\
            _hpen = s_info->pfnCreatePen(PS_SOLID, 1, _cr);\
            break;\
        }\
        _info_ret_obj(_cr)


    switch (index)
    {
        // 默认配色 0x0200 - 0x021F
        _info_defcolor(IGI_CONFIG_CRDEF_BACK        , color.crBackGround  , color.hbrBackGround   );    // [DefaultColor], 默认配色, 背景颜色
        _info_defcolor(IGI_CONFIG_CRDEF_NORMAL      , color.crNormal      , color.hbrNormal       );    // [DefaultColor], 默认配色, 正常颜色
        _info_defcolor(IGI_CONFIG_CRDEF_HOT         , color.crHot         , color.hbrHot          );    // [DefaultColor], 默认配色, 热点颜色
        _info_defcolor(IGI_CONFIG_CRDEF_DOWN        , color.crDown        , color.hbrDown         );    // [DefaultColor], 默认配色, 按下颜色
        _info_defcolor(IGI_CONFIG_CRDEF_BAN         , color.crBan         , color.hbrBan          );    // [DefaultColor], 默认配色, 禁止颜色
        _info_defcolor(IGI_CONFIG_CRDEF_HOVER       , color.crHover       , color.hbrHover        );    // [DefaultColor], 默认配色, 进入颜色
        _info_defcolor(IGI_CONFIG_CRDEF_FOCUS       , color.crFocus       , color.hbrFocus        );    // [DefaultColor], 默认配色, 焦点颜色
        _info_defcolor(IGI_CONFIG_CRDEF_CHECKED     , color.crChecked     , color.hbrChecked      );    // [DefaultColor], 默认配色, 选中颜色
        _info_defcolor(IGI_CONFIG_CRDEF_SELECTED    , color.crSelected    , color.hbrSelected     );    // [DefaultColor], 默认配色, 选择颜色
        _info_defcolor(IGI_CONFIG_CRDEF_VISTED      , color.crVisted      , color.hbrVisted       );    // [DefaultColor], 默认配色, 
        _info_defcolor(IGI_CONFIG_CRDEF_SHADOW      , color.crShadow      , color.hbrShadow       );    // [DefaultColor], 默认配色, 阴影颜色
        _info_pencolor(IGI_CONFIG_CRDEF_BORDER      , color.crBorder      , color.hpenBorder      );    // [DefaultColor], 默认配色, 边框颜色
        _info_defcolor(IGI_CONFIG_CRDEF_TEXT        , color.crText        , color.hbrText         );    // [DefaultColor], 默认配色, 文本颜色
        _info_defcolor(IGI_CONFIG_CRDEF_TEXTHOT     , color.crTextHot     , color.hbrTextHot      );    // [DefaultColor], 默认配色, 文本热点颜色
        _info_defcolor(IGI_CONFIG_CRDEF_TEXTDOWN    , color.crTextDown    , color.hbrTextDown     );    // [DefaultColor], 默认配色, 文本按下颜色
        _info_defcolor(IGI_CONFIG_CRDEF_TEXTBAN     , color.crTextBan     , color.hbrTextBan      );    // [DefaultColor], 默认配色, 文本禁止颜色

        // 基础颜色 0x0220 - 0x022F
        _info_color(IGI_CONFIG_CRBACK               , theme.crBack                   );  // [Basics], 背景颜色
        _info_color(IGI_CONFIG_CRBACKEXTERN         , theme.crBackExtern             );  // [Basics], 外部的背景颜色, 比如选择夹的子夹
        _info_color(IGI_CONFIG_CRSTATEBAR           , theme.crStateBar               );  // [Basics], 组件箱/工作夹/状态夹顶部激活的颜色
        _info_color(IGI_CONFIG_CRTEXT               , theme.crText                   );  // [Basics], 一般文本颜色

        // 边框/阴影配色 0x0230 - 0x023F
        _info_color(IGI_CONFIG_CRBORDER             , theme.crBorder                 );  // [Basics], 窗口边框颜色配置
        _info_color(IGI_CONFIG_CRSHADOW             , theme.crShadow                 );  // [Basics], 窗口阴影颜色配置
        _info_color(IGI_CONFIG_CRBORDER_RUN         , theme.crBorder_run             );  // [Basics], 运行时窗口边框的颜色
        _info_color(IGI_CONFIG_CRSHADOW_RUN         , theme.crShadow_run             );  // [Basics], 运行时窗口阴影颜色
        _info_color(IGI_CONFIG_CRBORDER_NF          , theme.crBorder_nf              );  // [Basics], 运行时窗口阴影颜色
        _info_color(IGI_CONFIG_CRSHADOW_NF          , theme.crShadow_nf              );  // [Basics], 失去焦点时窗口阴影颜色

        // 菜单项 0x0240 - 0x024F
        _info_color(IGI_CONFIG_CRMENUBORDER         , theme.crMenuBorder             );  // [Basics], 菜单项的边框颜色
        _info_color(IGI_CONFIG_CRMENUBACK           , theme.crMenuBack               );  // [Basics], 菜单项的背景颜色, 没有热点, 不是禁止时使用这个颜色绘画
        _info_color(IGI_CONFIG_CRMENUHOT            , theme.crMenuHot                );  // [Basics], 菜单项的热点颜色
        _info_color(IGI_CONFIG_CRMENUBAN            , theme.crMenuBan                );  // [Basics], 菜单项的禁止颜色
        _info_color(IGI_CONFIG_CRMENUTEXT           , theme.crMenuText               );  // [Basics], 菜单项正常文本颜色
        _info_color(IGI_CONFIG_CRMENUTEXTHOT        , theme.crMenuTextHot            );  // [Basics], 菜单项热点文本颜色
        _info_color(IGI_CONFIG_CRMENUTEXTBAN        , theme.crMenuTextBan            );  // [Basics], 菜单项禁止文本颜色

        // 显示在标题栏那里的菜单夹 0x0250 - 0x025F
        _info_color(IGI_CONFIG_CRMENUBARHOT         , theme.crMenuBarHot             );  // [Basics], 顶部菜单夹热点颜色
        _info_color(IGI_CONFIG_CRMENUBARTEXT        , theme.crMenuBarToolsText       );  // [Basics], 顶部菜单夹助手菜单的文本颜色
        _info_color(IGI_CONFIG_CRMENUBARTEXTHOT     , theme.crMenuBarToolsTextHot    );  // [Basics], 顶部菜单夹助手菜单的文本热点颜色

        // 显示在标题栏里黑色块的配色 0x0260 - 0x026F
        _info_color(IGI_CONFIG_CRTITLEBACK          , theme.crTitleback              );  // [Basics], 顶边黑块部分背景颜色
        _info_color(IGI_CONFIG_CRTITLELEFT_ACT      , theme.crTitle_left_act         );  // [Basics], 顶部黑块部分左边激活时文本颜色, 这个是源码名或者项目名
        _info_color(IGI_CONFIG_CRTITLERIGHT_ACT     , theme.crTitle_right_act        );  // [Basics], 顶部黑块部分右边激活时文本颜色, 这个是程序类型, 或者未保存时的星号使用的颜色
        _info_color(IGI_CONFIG_CRTITLELEFT          , theme.crTitle_left             );  // [Basics], 顶部黑块部分左边文本颜色, 这个是源码名或者项目名
        _info_color(IGI_CONFIG_CRTITLERIGHT         , theme.crTitle_right            );  // [Basics], 顶部黑块部分右边文本颜色, 这个是程序类型, 或者未保存时的星号使用的颜色

        // 底部状态条的配色 0x0270 - 0x027F
        _info_color(IGI_CONFIG_CRTEXTSTATUSBAR      , theme.crTextStatusBar          );  // [Basics], 底部状态条的文本颜色
        _info_color(IGI_CONFIG_CRTEXTSTATUSBAR_RUN  , theme.crTextStatusBar_run      );  // [Basics], 底部状态条运行时的文本颜色
        _info_color(IGI_CONFIG_CRTEXTSTATUSBAR_NF   , theme.crTextStatusBar_nf       );  // [Basics], 底部状态条不激活的文本颜色

        // 滚动条配色 0x0280 - 0x028F
        _info_color(IGI_CONFIG_CRSCROLLBTN1NORRMAL  , theme.crScrollBtn1             );  // [Basics], 滚动条按钮1正常颜色, 左边或者顶边的小按钮
        _info_color(IGI_CONFIG_CRSCROLLBTN1HOT      , theme.crScrollBtnHot1          );  // [Basics], 滚动条按钮1热点颜色, 左边或者顶边的小按钮
        _info_color(IGI_CONFIG_CRSCROLLBTN1DOWN     , theme.crScrollBtnDown1         );  // [Basics], 滚动条按钮1按下颜色, 左边或者顶边的小按钮
        _info_color(IGI_CONFIG_CRSCROLLBTN2NORRMAL  , theme.crScrollBtn2             );  // [Basics], 滚动条按钮2正常颜色, 右边或者底边的小按钮
        _info_color(IGI_CONFIG_CRSCROLLBTN2HOT      , theme.crScrollBtnHot2          );  // [Basics], 滚动条按钮2热点颜色, 右边或者底边的小按钮
        _info_color(IGI_CONFIG_CRSCROLLBTN2DOWN     , theme.crScrollBtnDown2         );  // [Basics], 滚动条按钮2按下颜色, 右边或者底边的小按钮
        _info_color(IGI_CONFIG_CRSCROLLTHUMBNORRMAL , theme.crScrollThumb            );  // [Basics], 滚动条滑块颜色
        _info_color(IGI_CONFIG_CRSCROLLTHUMBHOT     , theme.crScrollThumbHot         );  // [Basics], 滚动条滑块热点颜色
        _info_color(IGI_CONFIG_CRSCROLLTHUMBDOWN    , theme.crScrollThumbDown        );  // [Basics], 滚动条滑块按下颜色
        _info_color(IGI_CONFIG_CRSCROLLBACKGROUND   , theme.crScrollBack             );  // [Basics], 滚动条背景颜色

        // 分割线颜色 0x0290 - 0x029F
        _info_color(IGI_CONFIG_CRLINELEFT           , theme.crToolSeplineLeft        );  // [Basics], 工具条, 超列表头, 等组件的分割线左边颜色, 分割线都是两个像素
        _info_color(IGI_CONFIG_CRLINERIGHT          , theme.crToolSeplineRight       );  // [Basics], 工具条, 超列表头, 等组件的分割线右边颜色, 分割线都是两个像素

        // 自绘代码切换选择夹文本颜色, 0x02B0 - 0x02BF
        _info_color(IGI_CONFIG_CODETAB_CRCLOSEHOT       , theme.close_hot           );  // [Basics], 工具条, 超列表头, 等组件的分割线右边颜色, 分割线都是两个像素
        _info_color(IGI_CONFIG_CODETAB_CRCLOSEDOWN      , theme.close_down          );  // [Basics], 工具条, 超列表头, 等组件的分割线右边颜色, 分割线都是两个像素

        // 自绘代码切换选择夹文本颜色, 0x02B0 - 0x02BF
        _info_color(IGI_CONFIG_CODETAB_CRTEXT_WINDOW            , theme.crTab_text_window             );    // [CustomCodeTab], 代码切换选择夹, 窗口文本颜色
        _info_color(IGI_CONFIG_CODETAB_CRTEXT_ASSEMBLY          , theme.crTab_text_assembly           );    // [CustomCodeTab], 代码切换选择夹, 程序集文本颜色
        _info_color(IGI_CONFIG_CODETAB_CRTEXT_ASSEMBLY_WINDOW   , theme.crTab_text_assembly_window    );    // [CustomCodeTab], 代码切换选择夹, 窗口程序集文本颜色
        _info_color(IGI_CONFIG_CODETAB_CRTEXT_CLASS             , theme.crTab_text_class              );    // [CustomCodeTab], 代码切换选择夹, 类文本颜色
        _info_color(IGI_CONFIG_CODETAB_CRTEXT_GLOBALVAR         , theme.crTab_text_globalvar          );    // [CustomCodeTab], 代码切换选择夹, 全局变量文本颜色
        _info_color(IGI_CONFIG_CODETAB_CRTEXT_STRUCT            , theme.crTab_text_struct             );    // [CustomCodeTab], 代码切换选择夹, 自定义数据类型文本颜色
        _info_color(IGI_CONFIG_CODETAB_CRTEXT_DLL               , theme.crTab_text_dll                );    // [CustomCodeTab], 代码切换选择夹, DLL文本颜色
        _info_color(IGI_CONFIG_CODETAB_CRTEXT_CONST             , theme.crTab_text_const              );    // [CustomCodeTab], 代码切换选择夹, 常量文本颜色
        _info_color(IGI_CONFIG_CODETAB_CRTEXT_IMAGE             , theme.crTab_text_image              );    // [CustomCodeTab], 代码切换选择夹, 图片文本颜色
        _info_color(IGI_CONFIG_CODETAB_CRTEXT_SOUND             , theme.crTab_text_sound              );    // [CustomCodeTab], 代码切换选择夹, 声音文本颜色

        // 自绘代码切换选择夹底下线条颜色, 0x02C0 - 0x02CF
        _info_color(IGI_CONFIG_CODETAB_CRLINE_WINDOW            , theme.crTab_line_window             );    // [CustomCodeTab], 代码切换选择夹, 窗口下边线颜色
        _info_color(IGI_CONFIG_CODETAB_CRLINE_ASSEMBLY          , theme.crTab_line_assembly           );    // [CustomCodeTab], 代码切换选择夹, 程序集下边线颜色
        _info_color(IGI_CONFIG_CODETAB_CRLINE_ASSEMBLY_WINDOW   , theme.crTab_line_assembly_window    );    // [CustomCodeTab], 代码切换选择夹, 窗口程序集下边线颜色
        _info_color(IGI_CONFIG_CODETAB_CRLINE_CLASS             , theme.crTab_line_class              );    // [CustomCodeTab], 代码切换选择夹, 类下边线颜色
        _info_color(IGI_CONFIG_CODETAB_CRLINE_GLOBALVAR         , theme.crTab_line_globalvar          );    // [CustomCodeTab], 代码切换选择夹, 全局变量下边线颜色
        _info_color(IGI_CONFIG_CODETAB_CRLINE_STRUCT            , theme.crTab_line_struct             );    // [CustomCodeTab], 代码切换选择夹, 自定义数据类型下边线颜色
        _info_color(IGI_CONFIG_CODETAB_CRLINE_DLL               , theme.crTab_line_dll                );    // [CustomCodeTab], 代码切换选择夹, DLL下边线颜色
        _info_color(IGI_CONFIG_CODETAB_CRLINE_CONST             , theme.crTab_line_const              );    // [CustomCodeTab], 代码切换选择夹, 常量下边线颜色
        _info_color(IGI_CONFIG_CODETAB_CRLINE_IMAGE             , theme.crTab_line_image              );    // [CustomCodeTab], 代码切换选择夹, 图片下边线颜色
        _info_color(IGI_CONFIG_CODETAB_CRLINE_SOUND             , theme.crTab_line_sound              );    // [CustomCodeTab], 代码切换选择夹, 声音下边线颜色

        // 颜色配色最大到 0x03FF


    default:
        return false;
    }

    return true;
}

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
EXTERN_C _declspec(dllexport) LONG_PTR WINAPI iDraw_GetInfo(IDRAW_INFO* pInfo, int index, LPARAM argument)
{
    if (!s_info || pInfo != s_info) return 0;
    //if (!s_info || (pInfo && pInfo != s_info)) return 0;
    IConfig& ini = *s_info->config;

    switch (index)
    {
    case IGI_EWINDOW:                   // 易语言主窗口
        return (LONG_PTR)s_info->hEWnd;
    case IGI_CUSTOMCODETAB:             // 代码选择夹的窗口句柄
        return (LONG_PTR)s_info->hEWnd;
    case IGI_CODE_HWND:                 // 只读, 根据索引获取 MDIClient 下的第一层窗口句柄
    {
        LONG_PTR count = (LONG_PTR)s_info->hWnds->size();
        if (argument < 0 || argument >= count)
            return 0;
        CUSTOMTAB_DATA& data = s_info->hWnds->at(argument);
        return (LONG_PTR)data.hWnd;
    }
    case IGI_SHADOWWINDOW:              // 只读, 阴影窗口, 背景插件是基于这个窗口绘画的, 这个窗口是层窗口
        return (LONG_PTR)s_info->hShadow;
    case IGI_MDICLIENTTYPE:             // 只读, 当前MDIClient 窗口下是显示什么窗口, -1=没有窗口, 0=代码区, 1=设计器窗口, 2=起始页, 高位是窗口类型, 0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源
        argument = -1;  // 让下面自动获取当前选中项
    case IGI_MDICLIENTCODETYPE:         // 只读, 当前MDIClient 窗口下是显示什么窗口类型, 0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源
    case IGI_MDICLIENTTYPE_FROMINDEX:   // 从指定索引获取MDIClient 窗口下是显示什么窗口类型
    {
        if (argument == -1)
            argument = TabCtrl_GetCurSel(s_info->hEWnd);   // 当前选择夹选中项

        ICustomCodeTab& arr = *s_info->hWnds;
        if (argument < 0 || argument >= arr.size())
            return 0;   // return (LONG_PTR)s_info->pfnMove(IDC_IMOVE_GETCODETYPE, argument, 0);
        MDICLIENT_CODETYPE codeType = (MDICLIENT_CODETYPE)arr[argument].type;
        if (IGI_MDICLIENTCODETYPE == index)
            return (LONG_PTR)codeType;

        MDICLIENT_TYPE type;
        switch (codeType)
        {
        case MDICLIENT_CODETYPE_UNDEFINE:
            type = MDICLIENT_TYPE::MDICLIENT_TYPE_HONE;
            break;
        case MDICLIENT_CODETYPE_WINDOW:
            type = MDICLIENT_TYPE::MDICLIENT_TYPE_IDE;
            break;
        case MDICLIENT_CODETYPE_ASSEMBLY:
        case MDICLIENT_CODETYPE_ASSEMBLY_WINDOW:
        case MDICLIENT_CODETYPE_CLASS:
        case MDICLIENT_CODETYPE_GLOBALVAR:
        case MDICLIENT_CODETYPE_STRUCT:
        case MDICLIENT_CODETYPE_DLL:
        case MDICLIENT_CODETYPE_CONST:
        case MDICLIENT_CODETYPE_IMAGE:
        case MDICLIENT_CODETYPE_SOUND:
            type = MDICLIENT_TYPE::MDICLIENT_TYPE_CODE;
            break;
        default:
            type = MDICLIENT_TYPE::MDICLIENT_TYPE_NONE;
            break;
        }
        return MAKELONG(type, codeType);
    }
    case IGI_DPI:
        return (LONG_PTR)s_info->dpiX;

    case IGI_RCCUSTOMCODETAB:           // 代码选择夹的位置, 基于父窗口的位置
        return (LONG_PTR)&s_info->rcCustomTab;
    case IGI_RCSTATE:                   // 下面状态夹的位置, 基于父窗口的位置
        return (LONG_PTR)&s_info->rcState;
    case IGI_RCSTATUSBAR:               // 下面状态条的位置, 基于父窗口的位置
        return (LONG_PTR)&s_info->rcStatusBar;
    case IGI_RCMDICLIENT:               // 中间代码区的位置, 基于父窗口的位置
        return (LONG_PTR)&s_info->rcMDIClient;
    case IGI_RCCONTROL:                 // 组件箱的位置, 基于父窗口的位置
        return (LONG_PTR)&s_info->rcControl;
    case IGI_RCWORK:                    // 工作夹的位置, 基于父窗口的位置
        return (LONG_PTR)&s_info->rcWork;
    case IGI_RCMENUWND:                 // 菜单栏容器的位置, 基于父窗口的位置
        return (LONG_PTR)&s_info->rcWndMenu;

    case IGI_IGETEWINDOW:               // 易语言获取各个窗口的类指针
        return (LONG_PTR)s_info->eWnd;
    case IGI_IDENOTIFY:                 // 易语言交互函数, 调用这个函数就相当于调用特殊库的 调用易系统功能() 命令
        return (LONG_PTR)s_info->pfnIDE;

    case IGI_LIBPATH:                   // 易语言支持库目录, 后面带反斜杠
        return (LONG_PTR)s_info->path_run;
    case IGI_DATAPATH:                  // 易IDE数据存储目录, 后面带反斜杠, 一般是 易语言支持库目录/iDraw/
        return (LONG_PTR)s_info->path_data;
    case IGI_IDRAW:                     // 自绘类接口, 目前暂时未对外公开
        return (LONG_PTR)s_info->iDraw;
    case IGI_D2D:                       // D2D类接口, 目前暂时未对外公开
        return (LONG_PTR)s_info->d2d;

    case IGI_PATH_LIB:                  // 只读, 易语言支持库目录, 后面带反斜杠, 返回unicode文本指针
        return (LONG_PTR)s_info->path_run;
    case IGI_PATH_IDRAW:                // 只读, 易IDE主要使用的目录, 后面带反斜杠, 一般是 易语言支持库目录/iDraw/, 返回unicode文本指针
        return (LONG_PTR)s_info->path_iDraw;
    case IGI_PATH_DATA:                 // 只读, 易IDE数据存储目录, 后面带反斜杠, 一般是 易语言支持库目录/iDraw/, 返回unicode文本指针
        return (LONG_PTR)s_info->path_data;
    case IGI_PATH_PLUGIN:               // 只读, 易IDE插件路径
        return (LONG_PTR)s_info->path_plugin;
    case IGI_PATH_CONFIG:               // 只读, 易IDE数据配置文件完整路径
        return (LONG_PTR)s_info->path_config;

    case IGI_STATE_TOOL_GET:                // 只读, 获取指定工具条ID的状态, 参数3需要传递按钮ID
    {
        DWORD state = SendMessageW(s_info->eWnd->GetMenuStandard(), TB_GETSTATE, argument, 0);
        return (state & TBSTATE_ENABLED) == TBSTATE_ENABLED;    // 有 TBSTATE_ENABLED 这个标志才是启用状态
    }
    case IGI_STATE_TOOL_RUN:                // 只读, 当前易语言运行按钮的状态
        return (LONG_PTR)s_info->isRun;
    case IGI_STATE_TOOL_STOP:               // 只读, 当前易语言停止按钮的状态
        return (LONG_PTR)s_info->isStop;

    case IGI_CONFIG_DEFDXFONT:              // 只读, 菜单使用的字体
        return (LONG_PTR)s_info->pfnCreateFontDpi(s_info->defData->draw.hFont, s_info->dpiX);
    case IGI_CONFIG_DEFGDIFONT:             // 只读, 程序默认使用的GDI字体
        return (LONG_PTR)s_info->pfnCreateFontDpi(s_info->defData->draw.hFont, s_info->dpiX)->GetFont();


#define _igi_get(_case, _fun) \
    case _case:\
        return (LONG_PTR)_fun()

        _igi_get(IGI_CONFIG_WORK_DIRECTION      , ini.tab_work_direction    );  // 工作夹的子夹方向, 0=左边, 1=顶边, 2=右边, 3=底边
        _igi_get(IGI_CONFIG_STATE_DIRECTION     , ini.tab_state_direction   );  // 状态夹的子夹方向, 0=左边, 1=顶边, 2=右边, 3=底边
        _igi_get(IGI_CONFIG_CODE_DIRECTION      , ini.tab_code_direction    );  // 代码选择夹的子夹方向, 3=底边, 其他 = 顶边
        _igi_get(IGI_CONFIG_WORK_ISICON         , ini.tab_work_isIcon       );  // 底下的状态选择夹是否显示图标
        _igi_get(IGI_CONFIG_STATE_ISICON        , ini.tab_state_isIcon      );  // 左边的工作夹是否显示图标
        _igi_get(IGI_CONFIG_CODE_ISICON         , ini.tab_code_isIcon       );  // 代码切换选择夹是否显示图标
        //_igi_get(IGI_CONFIG_WORK_POS            , ini.tab_work_pos          );  // 工作夹位置, 0=左边, 1=顶边, 2=右边, 3=底边
        //_igi_get(IGI_CONFIG_STATE_POS           , ini.tab_state_pos         );  // 状态夹位置, 0=左边, 1=顶边, 2=右边, 3=底边
        //_igi_get(IGI_CONFIG_CONTROL_POS         , ini.tab_control_pos       );  // 组件箱位置, 0=左边, 1=顶边, 2=右边, 3=底边
        _igi_get(IGI_CONFIG_WORK_FLAGS          , ini.tab_work_flags        );  // 工作夹位置, 记录位置, 是否最大化, 是否浮动, 是否停靠等信息 TOP_STATE_BUTTON 枚举常量里的值, 调用 __query_flags 判断值
        _igi_get(IGI_CONFIG_STATE_FLAGS         , ini.tab_state_flags       );  // 状态夹位置, 记录位置, 是否最大化, 是否浮动, 是否停靠等信息 TOP_STATE_BUTTON 枚举常量里的值, 调用 __query_flags 判断值
        _igi_get(IGI_CONFIG_CONTROL_FLAGS       , ini.tab_control_flags     );  // 组件箱位置, 记录位置, 是否最大化, 是否浮动, 是否停靠等信息 TOP_STATE_BUTTON 枚举常量里的值, 调用 __query_flags 判断值
        _igi_get(IGI_CONFIG_ISDXRENDER          , ini.IsDxRender            );  // 是否使用DX渲染
        _igi_get(IGI_CONFIG_ISRUNSAVE           , ini.IsRunSave             );  // 运行/编译后是否保存源码
        _igi_get(IGI_CONFIG_WHEELPOS            , ini.wheelPos              );  // 鼠标滚动一次的距离
        _igi_get(IGI_CONFIG_NOCLIPCHILDREN      , ini.noClipChildren        );  // 不排除子窗口区域
        _igi_get(IGI_CONFIG_MENUITEMHEIGHT      , ini.MenuItemHeight        );  // 菜单项高度
        _igi_get(IGI_CONFIG_MENUSEPARATORHEIGHT , ini.MenuSeparatorHeight   );  // 分隔条菜单项高度
        _igi_get(IGI_CONFIG_ISLOCATIONTOOLBAR   , ini.IsShowLocationTool    );  // 是否显示定位工具条
        _igi_get(IGI_CONFIG_PROGRAMDBLCLKTYPE   , ini.programTreeDblclk     );  // 程序树型框双击模式, 0=只切换, 1=只展开, 2=展开+切换
        _igi_get(IGI_CONFIG_NOCHANGETAB_WORK    , ini.noChangeTab_work      );  // 不自动切换工作夹
        _igi_get(IGI_CONFIG_NOCHANGETAB_STATE   , ini.noChangeTab_state     );  // 不自动切换状态夹
        _igi_get(IGI_CONFIG_SCROLL_WIDTH        , ini.scroll_width          );  // 滚动条宽度
        _igi_get(IGI_CONFIG_SCROLL_BTN_SIZE     , ini.scroll_btn_size       );  // 滚动条按钮尺寸


        _igi_get(IGI_CONFIG_TREE_FNE_ISHOT      , ini.tree_fne_ishot        );  // 支持库树型框是否支持热点
        _igi_get(IGI_CONFIG_TREE_PROGRAM_ISHOT  , ini.tree_program_ishot    );  // 程序树型框是否支持热点
        _igi_get(IGI_CONFIG_TREE_CONTROL_ISHOT  , ini.tree_control_ishot    );  // 组件箱树型框是否支持热点
        _igi_get(IGI_CONFIG_TREE_VAR_ISHOT      , ini.tree_var_ishot        );  // 变量表树型框是否支持热点
        _igi_get(IGI_CONFIG_TREE_CALL_ISHOT     , ini.tree_call_ishot       );  // 调用表树型框是否支持热点
        _igi_get(IGI_CONFIG_LIST_FIND1_ISHOT    , ini.list_find1_ishot      );  // 搜寻1列表框是否支持热点
        _igi_get(IGI_CONFIG_LIST_FIND2_ISHOT    , ini.list_find2_ishot      );  // 搜寻2列表框是否支持热点

        _igi_get(IGI_CONFIG_TREE_FNE_HEIGHT     , ini.tree_fne_height       );  // 支持库树型框项目高度
        _igi_get(IGI_CONFIG_TREE_PROGRAM_HEIGHT , ini.tree_program_height   );  // 程序树型框项目高度
        _igi_get(IGI_CONFIG_TREE_CONTROL_HEIGHT , ini.tree_control_height   );  // 组件箱树型框项目高度
        _igi_get(IGI_CONFIG_TREE_VAR_HEIGHT     , ini.tree_var_height       );  // 变量表树型框项目高度
        _igi_get(IGI_CONFIG_TREE_CALL_HEIGHT    , ini.tree_call_height      );  // 调用表树型框项目高度
        _igi_get(IGI_CONFIG_LIST_FIND1_HEIGHT   , ini.list_find1_height     );  // 搜寻1列表框项目高度
        _igi_get(IGI_CONFIG_LIST_FIND2_HEIGHT   , ini.list_find2_height     );  // 搜寻2列表框项目高度

    default:
    {
        LONG_PTR ret = 0;
        _info_case_color(index, argument, ret, false, argument);
        return ret;
    }
    }
    return 0;
}


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      设置pInfo的信息, 接口程序的第一个参数, 谨慎设置, 这里的配置是全局使用, 一旦出错就会导致崩溃
// 
// 参数:
//      参数1: 接口程序的第一个参数
//      参数2: 获取的编号, IGI_ 开头常量
//      参数3: 要设置的值
//      参数3: 是否重画窗口, 仅针对部分影响窗口的配置有效
// 
// 返回值:
//      返回设置之前的值
EXTERN_C _declspec(dllexport) LONG_PTR WINAPI iDraw_SetInfo(IDRAW_INFO* pInfo, int index, LONG_PTR value, BOOL isUpdatwWindow)
{
    if (!s_info || pInfo != s_info) return 0;
    //if (!s_info || (pInfo && pInfo != s_info)) return 0;
    LONG_PTR ret = 0;
    DWORD style = 0;
    IConfig& ini = *s_info->config;

    switch (index)
    {
    case IGI_EWINDOW:                   // 易语言主窗口
    case IGI_CUSTOMCODETAB:             // 代码选择夹的窗口句柄
    case IGI_CODE_HWND:                 // 只读, 根据索引获取 MDIClient 下的第一层窗口句柄
    case IGI_SHADOWWINDOW:              // 只读, 阴影窗口, 背景插件是基于这个窗口绘画的, 这个窗口是层窗口
    case IGI_MDICLIENTTYPE:             // 只读, 当前MDIClient 窗口下是显示什么窗口, -1=没有窗口, 0=代码区, 1=设计器窗口, 2=起始页, 高位是窗口类型, 0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源
    case IGI_MDICLIENTCODETYPE:         // 只读, 当前MDIClient 窗口下是显示什么窗口类型, 0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源
    case IGI_MDICLIENTTYPE_FROMINDEX:   // 从指定索引获取MDIClient 窗口下是显示什么窗口类型
    case IGI_RCCUSTOMCODETAB:           // 代码选择夹的位置, 基于父窗口的位置
    case IGI_RCSTATE:                   // 下面状态夹的位置, 基于父窗口的位置
    case IGI_RCSTATUSBAR:               // 下面状态条的位置, 基于父窗口的位置
    case IGI_RCMDICLIENT:               // 中间代码区的位置, 基于父窗口的位置
    case IGI_RCCONTROL:                 // 组件箱的位置, 基于父窗口的位置
    case IGI_RCWORK:                    // 工作夹的位置, 基于父窗口的位置
    case IGI_RCMENUWND:                 // 菜单栏容器的位置, 基于父窗口的位置
    case IGI_IGETEWINDOW:               // 易语言获取各个窗口的类指针
    case IGI_IDENOTIFY:                 // 易语言交互函数, 调用这个函数就相当于调用特殊库的 调用易系统功能() 命令
    case IGI_LIBPATH:                   // 易语言支持库目录, 后面带反斜杠
    case IGI_DATAPATH:                  // 易IDE数据存储目录, 后面带反斜杠, 一般是 易语言支持库目录/iDraw/
    case IGI_IDRAW:                     // 自绘类接口, 目前暂时未对外公开
    case IGI_D2D:                       // D2D类接口, 目前暂时未对外公开
    case IGI_PATH_LIB:                  // 只读, 易语言支持库目录, 后面带反斜杠, 返回unicode文本指针
    case IGI_PATH_IDRAW:                // 只读, 易IDE主要使用的目录, 后面带反斜杠, 一般是 易语言支持库目录/iDraw/, 返回unicode文本指针
    case IGI_PATH_DATA:                 // 只读, 易IDE数据存储目录, 后面带反斜杠, 一般是 易语言支持库目录/iDraw/, 返回unicode文本指针
    case IGI_PATH_PLUGIN:               // 只读, 易IDE插件路径
    case IGI_PATH_CONFIG:               // 只读, 易IDE数据配置文件完整路径
    case IGI_STATE_TOOL_GET:            // 只读, 获取指定工具条ID的状态, 参数3需要传递按钮ID
    case IGI_STATE_TOOL_RUN:            // 只读, 当前易语言运行按钮的状态
    case IGI_STATE_TOOL_STOP:           // 只读, 当前易语言停止按钮的状态
    case IGI_CONFIG_DEFDXFONT:          // 只读, 菜单使用的字体
    case IGI_CONFIG_DEFGDIFONT:         // 只读, 程序默认使用的GDI字体
        return 0;   // 上面这些都是只读, 设置直接返回0

#define _igi_set(_case, _fun) \
    case _case:\
        ret = (LONG_PTR)_fun((int*)&value); break

        _igi_set(IGI_CONFIG_WORK_DIRECTION      , ini.tab_work_direction    );  // 工作夹的子夹方向, 0=左边, 1=顶边, 2=右边, 3=底边
        _igi_set(IGI_CONFIG_STATE_DIRECTION     , ini.tab_state_direction   );  // 状态夹的子夹方向, 0=左边, 1=顶边, 2=右边, 3=底边
        _igi_set(IGI_CONFIG_CODE_DIRECTION      , ini.tab_code_direction    );  // 代码选择夹的子夹方向, 3=底边, 其他 = 顶边
        _igi_set(IGI_CONFIG_WORK_ISICON         , ini.tab_work_isIcon       );  // 底下的状态选择夹是否显示图标
        _igi_set(IGI_CONFIG_STATE_ISICON        , ini.tab_state_isIcon      );  // 左边的工作夹是否显示图标
        _igi_set(IGI_CONFIG_CODE_ISICON         , ini.tab_code_isIcon       );  // 代码切换选择夹是否显示图标
        // 下面几个是只读
        //_igi_set(IGI_CONFIG_WORK_POS            , ini.tab_work_pos          );  // 工作夹位置, 0=左边, 1=顶边, 2=右边, 3=底边
        //_igi_set(IGI_CONFIG_STATE_POS           , ini.tab_state_pos         );  // 状态夹位置, 0=左边, 1=顶边, 2=右边, 3=底边
        //_igi_set(IGI_CONFIG_CONTROL_POS         , ini.tab_control_pos       );  // 组件箱位置, 0=左边, 1=顶边, 2=右边, 3=底边
        //_igi_set(IGI_CONFIG_WORK_FLAGS          , ini.tab_work_flags        );  // 工作夹位置, 记录位置, 是否最大化, 是否浮动, 是否停靠等信息 TOP_STATE_BUTTON 枚举常量里的值, 调用 __query_flags 判断值
        //_igi_set(IGI_CONFIG_STATE_FLAGS         , ini.tab_state_flags       );  // 状态夹位置, 记录位置, 是否最大化, 是否浮动, 是否停靠等信息 TOP_STATE_BUTTON 枚举常量里的值, 调用 __query_flags 判断值
        //_igi_set(IGI_CONFIG_CONTROL_FLAGS       , ini.tab_control_flags     );  // 组件箱位置, 记录位置, 是否最大化, 是否浮动, 是否停靠等信息 TOP_STATE_BUTTON 枚举常量里的值, 调用 __query_flags 判断值
        _igi_set(IGI_CONFIG_ISDXRENDER          , ini.IsDxRender            );  // 是否使用DX渲染
        _igi_set(IGI_CONFIG_ISRUNSAVE           , ini.IsRunSave             );  // 运行/编译后是否保存源码
        _igi_set(IGI_CONFIG_WHEELPOS            , ini.wheelPos              );  // 鼠标滚动一次的距离
        _igi_set(IGI_CONFIG_NOCLIPCHILDREN      , ini.noClipChildren        );  // 不排除子窗口区域
        _igi_set(IGI_CONFIG_MENUITEMHEIGHT      , ini.MenuItemHeight        );  // 菜单项高度
        _igi_set(IGI_CONFIG_MENUSEPARATORHEIGHT , ini.MenuSeparatorHeight   );  // 分隔条菜单项高度
        _igi_set(IGI_CONFIG_ISLOCATIONTOOLBAR   , ini.IsShowLocationTool    );  // 是否显示定位工具条
        _igi_set(IGI_CONFIG_PROGRAMDBLCLKTYPE   , ini.programTreeDblclk     );  // 程序树型框双击模式, 0=只切换, 1=只展开, 2=展开+切换
        _igi_set(IGI_CONFIG_NOCHANGETAB_WORK    , ini.noChangeTab_work      );  // 不自动切换工作夹
        _igi_set(IGI_CONFIG_NOCHANGETAB_STATE   , ini.noChangeTab_state     );  // 不自动切换状态夹
        _igi_set(IGI_CONFIG_SCROLL_WIDTH        , ini.scroll_width          );  // 滚动条宽度
        _igi_set(IGI_CONFIG_SCROLL_BTN_SIZE     , ini.scroll_btn_size       );  // 滚动条按钮尺寸


        _igi_set(IGI_CONFIG_TREE_FNE_ISHOT      , ini.tree_fne_ishot        );  // 支持库树型框是否支持热点
        _igi_set(IGI_CONFIG_TREE_PROGRAM_ISHOT  , ini.tree_program_ishot    );  // 程序树型框是否支持热点
        _igi_set(IGI_CONFIG_TREE_CONTROL_ISHOT  , ini.tree_control_ishot    );  // 组件箱树型框是否支持热点
        _igi_set(IGI_CONFIG_TREE_VAR_ISHOT      , ini.tree_var_ishot        );  // 变量表树型框是否支持热点
        _igi_set(IGI_CONFIG_TREE_CALL_ISHOT     , ini.tree_call_ishot       );  // 调用表树型框是否支持热点
        _igi_set(IGI_CONFIG_LIST_FIND1_ISHOT    , ini.list_find1_ishot      );  // 搜寻1列表框是否支持热点
        _igi_set(IGI_CONFIG_LIST_FIND2_ISHOT    , ini.list_find2_ishot      );  // 搜寻2列表框是否支持热点

        _igi_set(IGI_CONFIG_TREE_FNE_HEIGHT     , ini.tree_fne_height       );  // 支持库树型框项目高度
        _igi_set(IGI_CONFIG_TREE_PROGRAM_HEIGHT , ini.tree_program_height   );  // 程序树型框项目高度
        _igi_set(IGI_CONFIG_TREE_CONTROL_HEIGHT , ini.tree_control_height   );  // 组件箱树型框项目高度
        _igi_set(IGI_CONFIG_TREE_VAR_HEIGHT     , ini.tree_var_height       );  // 变量表树型框项目高度
        _igi_set(IGI_CONFIG_TREE_CALL_HEIGHT    , ini.tree_call_height      );  // 调用表树型框项目高度
        _igi_set(IGI_CONFIG_LIST_FIND1_HEIGHT   , ini.list_find1_height     );  // 搜寻1列表框项目高度
        _igi_set(IGI_CONFIG_LIST_FIND2_HEIGHT   , ini.list_find2_height     );  // 搜寻2列表框项目高度

    default:
        if (_info_case_color(index, value, ret, true, isUpdatwWindow))
            break;
        return 0;
    }

    s_info->pfnInfoChanged(index, ret, value, isUpdatwWindow);

    return ret;
}
#endif