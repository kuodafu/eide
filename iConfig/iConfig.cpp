#include "iConfig.h"
#include <control/ReadColor.h>
#include <iDraw_const.h>
static bool m_isReadFile;                       // 是否从文件中读取
static std::vector<int> m_colorIsChange;        // 颜色配置被改变的项目
static std::vector<int> m_configIsChange;       // 主配置被改变的项目
static bool m_isFirst = true;                   // 是否是第一次加载, 读过一次之后就为false


inline static RECT _iconfig_setvalueRECT(CPrivateProfile* ini, LPCWSTR appName, RECT* value, RECT* pValue, LPCWSTR nodeName, LPCWSTR attrName)
{
    LPCWSTR file = ini->m_file.c_str();
    RECT& ret = *value;
    RECT r = ret;

    wstr name(260);
    name.assign(nodeName);
    if (attrName) name.append(attrName);
    ret = *pValue;
    wstr vl; vl.Format(L"%d,%d,%d,%d", ret.left, ret.top, ret.right, ret.bottom);    // 要写入配置项的值
    WritePrivateProfileStringW(appName, name.c_str(), vl.c_str(), file);
    return r;
}
// 把值写入到配置文件中, 并把值写入到类成员变量里, 只支持4个字节的成员, 返回设置之前的值
// ini = 写入到哪个ini里
// appName = 节名
// value = 类成员地址, 设置新的值后新的值会写入这个地址里
// pValue = 要写入的值的地址, 把这个地址里的数据写入到配置文件里
// nodeName = 项名前缀
// attrName = 项目后缀, 如果为0则不使用这个
inline static int _iconfig_setvalue(CPrivateProfile* ini, LPCWSTR appName, void* value, void* pValue, LPCWSTR nodeName, LPCWSTR attrName)
{
    LPCWSTR file = ini->m_file.c_str();
    int& ret = (*((int*)(value)));
    int r = ret;

    wstr name(260);
    name.assign(nodeName);
    if (attrName) name.append(attrName);
    ret = (*((int*)pValue));
    wstr vl; vl.assign(ret);    // 要写入配置项的值
    WritePrivateProfileStringW(appName, name.c_str(), vl.c_str(), file);
    return r;
}

// 读取配置项, pValue = 0则读取
// ini = 对哪个ini进行操作
// appName = 节名
// nodeName = 项名前缀
// attrName = 项目后缀, 如果为0则不使用这个
// defValue = 默认文本, 如果配置不存在, 则把这个默认值写入配置文件中
inline static wstr _iconfig_value(CPrivateProfile* ini, LPCWSTR appName, LPCWSTR nodeName, LPCWSTR attrName, LPCWSTR defValue)
{
    LPCWSTR file = ini->m_file.c_str();
    wstr name(260);
    name.assign(nodeName);
    if (attrName) name.append(attrName);
    return ini->read(appName, name.c_str(), defValue);
}
class Config : public IConfig
{
    inline int _read(int& m_vl, int* pValue, LPCWSTR appName, LPCWSTR name, LPCWSTR attrName, int def, int IGI)
    {
        if (pValue)
            return _iconfig_setvalue(m_ini, appName, &m_vl, pValue, name, attrName); 
        if(!m_isReadFile) return m_vl;
        wstr szDef; szDef.assign(def);
        wstr ret = _iconfig_value(m_ini, appName, name, attrName, szDef.c_str());
        int oldValue = m_vl;
        m_vl = ret.stol();
        // 是否调用配置改变事件, 不是第一次加载, 且新旧配置不一样
        const bool isCallConfigChange = !m_isFirst && m_vl != oldValue;

        if (isCallConfigChange)
        {
            const bool isReadFile = m_isReadFile;
            m_isReadFile = false;
            iConfig_InfoChanged(IGI, oldValue, m_vl, true);   // 通知配置被改变
            m_isReadFile = isReadFile;
        }
        return m_vl;
    }
    inline RECT _read_rc(RECT& m_vl, RECT* pValue, LPCWSTR appName, LPCWSTR name, LPCWSTR attrName, LPCWSTR def)
    {
        if (pValue)
            return _iconfig_setvalueRECT(m_ini, appName, &m_vl, pValue, name, attrName); 
        if(!m_isReadFile) return m_vl;
        RECT& rc = m_vl;    // 位置不做改变判断
        wstr ret = _iconfig_value(m_ini, appName, name, attrName, def);
        ret.sscanf(L"%d,%d,%d,%d", &rc.left, &rc.top, &rc.right, &rc.bottom);
        return rc;
    }
public:



#define _IIConfig_fun_name(_appName, _funname, _ret, _name, _attrName, _def, _IGI) \
    _ret m_##_funname = 0;  \
    virtual _ret _funname (_ret* pValue = 0) { return _read(m_##_funname, pValue, _appName, _name, _attrName, _def, _IGI); }

#define _IIConfig_fun_name_RECT(_appName, _funname, _name, _attrName, _def) \
    RECT m_##_funname = {0};  \
    virtual RECT _funname (RECT* pValue = 0) { return _read_rc(m_##_funname, pValue, _appName, _name, _attrName, _def); }

    CPrivateProfile* m_ini;
    Config(CPrivateProfile* ini, bool isReadFile) :m_ini(ini) { }
    _IIConfig_fun_name(L"iDraw", IsRunSave            ,BOOL , L"IsRunSave",          0          , 1 , IGI_CONFIG_ISRUNSAVE)             // 运行/编译后是否保存源码, 取值需要传递一个指针接收结果
    _IIConfig_fun_name(L"iDraw", IsDxRender           ,BOOL , L"isDxRender",         0          , 1 , IGI_CONFIG_ISDXRENDER)            // 是否DX渲染, 取值需要传递一个指针接收结果
    _IIConfig_fun_name(L"iDraw", IsDPIAware           ,BOOL , L"IsDPIAware",         0          , 0 , IGI_CONFIG_ISDPIAWARE)            // 是否开启DPI感知
    _IIConfig_fun_name(L"iDraw", IsDPIScaleWnd        ,BOOL , L"IsDPIScaleWnd",      0          , 1 , IGI_CONFIG_ISDPISCALEWND)         // DPI改变时是否缩放窗口
    _IIConfig_fun_name(L"iDraw", IsShowCodeTabClose   ,BOOL , L"IsShowCodeTabClose", 0          , 1 , IGI_CONFIG_SHOWCODETABCLOSE)      // 显示代码选择夹的关闭按钮
    _IIConfig_fun_name(L"iDraw", IsShowLocationTool   ,BOOL , L"IsShowLocationTool", 0          , 1 , IGI_CONFIG_ISLOCATIONTOOLBAR)     // 是否显示定位工具条
    _IIConfig_fun_name(L"iDraw", programTreeDblclk    ,int  , L"programTreeDblclk",  0          , 2 , IGI_CONFIG_PROGRAMDBLCLKTYPE)     // 程序树型框双击模式, 0=只切换, 1=只展开, 2=展开+切换
    _IIConfig_fun_name(L"iDraw", wheelPos             ,int  , L"wheelPos",           0          , 3 , IGI_CONFIG_WHEELPOS)              // 鼠标滚动一次的距离
    _IIConfig_fun_name(L"iDraw", noClipChildren       ,BOOL , L"noClipChildren",     0          , 0 , IGI_CONFIG_NOCLIPCHILDREN)        // 不排除子窗口区域
    _IIConfig_fun_name(L"iDraw", MenuItemHeight       ,int  , L"MenuItemHeight",     0          , 24, IGI_CONFIG_MENUITEMHEIGHT)        // 菜单项高度
    _IIConfig_fun_name(L"iDraw", MenuSeparatorHeight  ,int  , L"MenuItemHeight", L"nSeparator"  , 5 , IGI_CONFIG_MENUSEPARATORHEIGHT)   // 分隔菜单项高度
    _IIConfig_fun_name(L"iDraw", noChangeTab_work     ,int  , L"noChangeTab_work",   0          , 0 , IGI_CONFIG_NOCHANGETAB_WORK)      // 不自动切换工作夹
    _IIConfig_fun_name(L"iDraw", noChangeTab_state    ,int  , L"noChangeTab_state",  0          , 0 , IGI_CONFIG_NOCHANGETAB_STATE)     // 不自动切换状态夹
    _IIConfig_fun_name(L"iDraw", scroll_width         ,int  , L"scroll_width",       0          , 12, IGI_CONFIG_SCROLL_WIDTH)          // 滚动条宽度
    _IIConfig_fun_name(L"iDraw", scroll_btn_size      ,int  , L"scroll_btn_size",    0          , 10, IGI_CONFIG_SCROLL_BTN_SIZE)       // 滚动条按钮尺寸

    _IIConfig_fun_name(L"iDraw", tree_fne_ishot       ,BOOL , L"fne_tree",       L"ishot"       , 1 , IGI_CONFIG_TREE_FNE_ISHOT)        // 支持库树型框是否支持热点
    _IIConfig_fun_name(L"iDraw", tree_program_ishot   ,BOOL , L"tree_program",   L"ishot"       , 1 , IGI_CONFIG_TREE_PROGRAM_ISHOT)    // 程序树型框是否支持热点
    _IIConfig_fun_name(L"iDraw", tree_control_ishot   ,BOOL , L"tree_control",   L"ishot"       , 1 , IGI_CONFIG_TREE_CONTROL_ISHOT)    // 组件箱树型框是否支持热点
    _IIConfig_fun_name(L"iDraw", tree_control_orgIcon ,BOOL , L"tree_control",   L"orgIcon"     , 0 , IGI_CONFIG_TREE_CONTROL_ORGICON)  // 组件箱树型框显示原图标
    _IIConfig_fun_name(L"iDraw", tree_var_ishot       ,BOOL , L"tree_var",       L"ishot"       , 1 , IGI_CONFIG_TREE_VAR_ISHOT)        // 变量表树型框是否支持热点
    _IIConfig_fun_name(L"iDraw", tree_call_ishot      ,BOOL , L"tree_call",      L"ishot"       , 1 , IGI_CONFIG_TREE_CALL_ISHOT)       // 调用表树型框是否支持热点
    _IIConfig_fun_name(L"iDraw", list_find1_ishot     ,BOOL , L"list_find1",     L"ishot"       , 1 , IGI_CONFIG_LIST_FIND1_ISHOT)      // 搜寻1列表框是否支持热点
    _IIConfig_fun_name(L"iDraw", list_find2_ishot     ,BOOL , L"list_find2",     L"ishot"       , 1 , IGI_CONFIG_LIST_FIND2_ISHOT)      // 搜寻1列表框树型框是否支持热点

    _IIConfig_fun_name(L"iDraw", tree_fne_height      ,int  , L"fne_tree",       L"itemHeight"  , 24, IGI_CONFIG_TREE_FNE_HEIGHT)       // 支持库树型框项目高度
    _IIConfig_fun_name(L"iDraw", tree_program_height  ,int  , L"tree_program",   L"itemHeight"  , 24, IGI_CONFIG_TREE_PROGRAM_HEIGHT)   // 程序树型框项目高度
    _IIConfig_fun_name(L"iDraw", tree_control_height  ,int  , L"tree_control",   L"itemHeight"  , 32, IGI_CONFIG_TREE_CONTROL_HEIGHT)   // 组件箱树型框项目高度
    _IIConfig_fun_name(L"iDraw", tree_var_height      ,int  , L"tree_var",       L"itemHeight"  , 24, IGI_CONFIG_TREE_VAR_HEIGHT)       // 变量表树型框项目高度
    _IIConfig_fun_name(L"iDraw", tree_call_height     ,int  , L"tree_call",      L"itemHeight"  , 24, IGI_CONFIG_TREE_CALL_HEIGHT)      // 调用表树型框项目高度
    _IIConfig_fun_name(L"iDraw", list_find1_height    ,int  , L"list_find1",     L"itemHeight"  , 24, IGI_CONFIG_LIST_FIND1_HEIGHT)     // 搜寻1列表框项目高度
    _IIConfig_fun_name(L"iDraw", list_find2_height    ,int  , L"list_find2",     L"itemHeight"  , 24, IGI_CONFIG_LIST_FIND2_HEIGHT)     // 搜寻2列表框项目高度


            
    _IIConfig_fun_name(L"EWindowSize", tab_work_direction   , int,  L"work",      L"direction",3, IGI_CONFIG_WORK_DIRECTION)     // 工作夹方向, 0=左边, 1=顶边, 2=右边, 3=底边
    _IIConfig_fun_name(L"EWindowSize", tab_work_isIcon      , BOOL, L"work",      L"isIcon",   0, IGI_CONFIG_WORK_ISICON)        // 工作夹是否显示图标
    _IIConfig_fun_name(L"EWindowSize", tab_work_flags       , BOOL, L"work",      L"flags",    0, IGI_CONFIG_WORK_FLAGS)         // 工作夹位置, 0=左边, 1=顶边, 2=右边, 3=底边

    _IIConfig_fun_name(L"EWindowSize", tab_state_direction  , int,  L"state",     L"direction",3, IGI_CONFIG_STATE_DIRECTION)    // 状态夹方向, 0=左边, 1=顶边, 2=右边, 3=底边
    _IIConfig_fun_name(L"EWindowSize", tab_state_isIcon     , BOOL, L"state",     L"isIcon",   0, IGI_CONFIG_STATE_ISICON)       // 状态夹是否显示图标
    _IIConfig_fun_name(L"EWindowSize", tab_state_flags      , BOOL, L"state",     L"flags",    0, IGI_CONFIG_STATE_FLAGS)        // 状态夹位置, 0=左边, 1=顶边, 2=右边, 3=底边

    _IIConfig_fun_name(L"EWindowSize", tab_control_flags    , BOOL, L"control",   L"flags",    0, IGI_CONFIG_CONTROL_FLAGS)      // 组件箱位置, 0=左边, 1=顶边, 2=右边, 3=底边


    _IIConfig_fun_name(L"EWindowSize", tab_code_direction   , int,  L"code",      L"direction",1, IGI_CONFIG_CODE_DIRECTION)     // 代码切换选择夹方向, 3=底边, 非3=顶边
    _IIConfig_fun_name(L"EWindowSize", tab_code_isIcon      , BOOL, L"code",      L"isIcon",   1, IGI_CONFIG_CODE_ISICON)        // 代码切换选择夹是否显示图标

    _IIConfig_fun_name_RECT(L"EWindowSize", rcWork          , L"work",        L"RECT", L"0,0,0,0")     // 工作夹窗口位置, 其他组件根据这个矩形计算位置
    _IIConfig_fun_name_RECT(L"EWindowSize", rcState         , L"state",       L"RECT", L"0,0,0,0")     // 状态夹窗口位置, 其他组件根据这个矩形计算位置
    _IIConfig_fun_name_RECT(L"EWindowSize", rcControl       , L"control",     L"RECT", L"0,0,0,0")     // 组件箱窗口位置, 其他组件根据这个矩形计算位置
    _IIConfig_fun_name_RECT(L"EWindowSize", rcMenu          , L"menu",        L"RECT", L"0,0,0,0")     // 菜单夹窗口位置, 其他组件根据这个矩形计算位置
    _IIConfig_fun_name_RECT(L"EWindowSize", rcMDIClient     , L"MDIClient",   L"RECT", L"0,0,0,0")     // MDIClient窗口位置, 其他组件根据这个矩形计算位置

    _IIConfig_fun_name_RECT(L"EWindowSize", rcWork1         , L"work1",       L"RECT", L"0,0,0,0")     // 工作夹窗口位置, 显示到屏幕的位置
    _IIConfig_fun_name_RECT(L"EWindowSize", rcState1        , L"state1",      L"RECT", L"0,0,0,0")     // 状态夹窗口位置, 显示到屏幕的位置
    _IIConfig_fun_name_RECT(L"EWindowSize", rcControl1      , L"control1",    L"RECT", L"0,0,0,0")     // 组件箱窗口位置, 显示到屏幕的位置
};

class ThemeColor : public IThemeColor
{

    // 读取/写入节点/属性颜色值, 返回读取到的值
    // xml = 从哪个配置的xml里读写
    // value = 从哪个配置读写, 需要传递结构成员的指针, 设置的话内部会写入这个地址, 读取的话就从这个地址读取
    // pValue = 如果不为0, 则表示是设置值, 设置会把数据写入xml
    // nodeName = 节点名
    // attrName = 属性名, 如果为0, 则读写操作就从这个节点读写, 不为0则从这个属性读写
    // defColor = 读属性值的默认颜色值, 如果没有这个属性, 会把这个值写入到属性里
    // isReadFile = 是否从文件里读取
    inline int _iconfig_color(LPCWSTR appName, COLORREF& value, COLORREF* pValue, LPCWSTR nodeName, LPCWSTR attrName, int defColor, int IGI)
    {
        if (pValue) // 这个有值就是需要写入
        {
            LPCWSTR file = m_ini->m_file.c_str();
            int r = value;

            wstr name(260);
            name.assign(nodeName);
            if (attrName) name.append(attrName);
            value = *pValue;
            wstr vl = rgb2str(value); // 要写入配置项的值
            WritePrivateProfileStringW(appName, name.c_str(), vl.c_str(), file);
            return r;
        }
        if (m_isReadFile)
        {
            wstr defVel = rgb2str(defColor);
            wstr vl = _iconfig_value(m_ini, appName, nodeName, attrName, defVel.c_str());
            COLORREF oldValue = value;
            value = str2rgb(vl.c_str(), defColor);
            if (oldValue != value) m_colorIsChange.push_back(IGI);

            // 是否调用配置改变事件, 不是第一次加载, 且新旧配置不一样
            const bool isCallConfigChange = !m_isFirst && oldValue != value;
            if (isCallConfigChange)
            {
                const bool isReadFile = m_isReadFile;
                m_isReadFile = false;
                iConfig_InfoChanged(IGI, oldValue, value, true);   // 通知配置被改变
                m_isReadFile = isReadFile;
            }
            return value;
        }
        return value;
    }
#define _Basics_GETCOLOR(_name, _attrName, _funname, _def, _IGI) \
    COLORREF m_Basics_##_funname = 0;  \
    virtual COLORREF _funname (COLORREF* pValue = 0) {\
        return _iconfig_color(L"Basics", m_Basics_##_funname, pValue, _name, _attrName, _def, _IGI);\
    }

public:
    CPrivateProfile* m_ini;
    ThemeColor(CPrivateProfile* ini, bool isReadFile) :m_ini(ini){ ; }
    _Basics_GETCOLOR(L"crBack",    0,          crBack                , RGB(45, 45, 48)   , IGI_CONFIG_CRBACK            ) // [Basics], 背景颜色
    _Basics_GETCOLOR(L"crBackExtern", 0,       crBackExtern          , RGB(45, 45, 48)   , IGI_CONFIG_CRBACKEXTERN      ) // [Basics], 外部的背景颜色, 比如选择夹的子夹
    _Basics_GETCOLOR(L"crStateBar", 0,         crStateBar            , RGB(0, 122, 204)  , IGI_CONFIG_CRSTATEBAR        ) // [Basics], 组件箱/工作夹/状态夹顶部激活的颜色

    _Basics_GETCOLOR(L"crBorder",  0,          crBorder              , RGB(0, 122, 204)  , IGI_CONFIG_CRBORDER          ) // [Basics], 边框颜色
    _Basics_GETCOLOR(L"crBorder",  L"run",     crBorder_run          , RGB(202, 81, 0)   , IGI_CONFIG_CRBORDER_RUN      ) // [Basics], 运行时边框颜色
    _Basics_GETCOLOR(L"crBorder",  L"nofocus", crBorder_nf           , RGB(66, 66, 69)   , IGI_CONFIG_CRBORDER_NF       ) // [Basics], 取消激活时的边框颜色

    _Basics_GETCOLOR(L"crShadow",  0,          crShadow              , RGB(0, 122, 204)  , IGI_CONFIG_CRSHADOW          ) // [Basics], 阴影颜色
    _Basics_GETCOLOR(L"crShadow",  L"run",     crShadow_run          , RGB(202, 81, 0)   , IGI_CONFIG_CRSHADOW_RUN      ) // [Basics], 运行时阴影颜色
    _Basics_GETCOLOR(L"crShadow",  L"nofocus", crShadow_nf           , RGB(66, 66, 69)   , IGI_CONFIG_CRSHADOW_NF       ) // [Basics], 取消激活时的边框颜色

    _Basics_GETCOLOR(L"crMenu",    0,          crMenuBorder          , RGB(63, 63, 70)   , IGI_CONFIG_CRMENUBORDER      ) // [Basics], 菜单边框颜色
    _Basics_GETCOLOR(L"crMenu",    L"back",    crMenuBack            , RGB(27, 27, 28)   , IGI_CONFIG_CRMENUBACK        ) // [Basics], 菜单背景颜色, 弹出菜单使用的背景色, 菜单条按下时使用的背景色
    _Basics_GETCOLOR(L"crMenu",    L"hot",     crMenuHot             , RGB(51, 51, 52)   , IGI_CONFIG_CRMENUHOT         ) // [Basics], 菜单热点项颜色
    _Basics_GETCOLOR(L"crMenu",    L"ban",     crMenuBan             , RGB(32, 32, 33)   , IGI_CONFIG_CRMENUBAN         ) // [Basics], 菜单禁止颜色
    _Basics_GETCOLOR(L"crMenu",    L"text",    crMenuText            , RGB(222, 222, 222), IGI_CONFIG_CRMENUTEXT        ) // [Basics], 菜单文本颜色
    _Basics_GETCOLOR(L"crMenu",    L"texthot", crMenuTextHot         , RGB(222, 222, 222), IGI_CONFIG_CRMENUTEXTHOT     ) // [Basics], 菜单文本热点颜色
    _Basics_GETCOLOR(L"crMenu",    L"textban", crMenuTextBan         , RGB(101, 101, 101), IGI_CONFIG_CRMENUTEXTBAN     ) // [Basics], 菜单文本禁止颜色

    _Basics_GETCOLOR(L"crMenuBar", L"hot",     crMenuBarHot          , RGB(62, 62, 64)   , IGI_CONFIG_CRMENUBARHOT      ) // [Basics], 顶部菜单条热点项颜色
    _Basics_GETCOLOR(L"crMenuBar", L"text",    crMenuBarToolsText    , RGB(0, 133, 196)  , IGI_CONFIG_CRMENUBARTEXT     ) // [Basics], 顶部菜单条助手的文本颜色
    _Basics_GETCOLOR(L"crMenuBar", L"texthot", crMenuBarToolsTextHot , RGB(193, 249, 253), IGI_CONFIG_CRMENUBARTEXTHOT  ) // [Basics], 顶部菜单条助手的文本热点颜色

                                                                                            
    _Basics_GETCOLOR(L"crCaption", 0,          crTitleback           , RGB(37, 37, 38)   , IGI_CONFIG_CRTITLEBACK           ) // [Basics], 中间黑块标题的背景颜色
    _Basics_GETCOLOR(L"crCaption", L"lact",    crTitle_left_act      , RGB(241, 241, 241), IGI_CONFIG_CRTITLELEFT_ACT       ) // [Basics], 中间黑块标题左边的颜色, 这个是源码名或者项目名, 激活时的文本颜色
    _Basics_GETCOLOR(L"crCaption", L"ract",    crTitle_right_act     , RGB(122, 193, 255), IGI_CONFIG_CRTITLERIGHT_ACT      ) // [Basics], 中间黑块标题右边的颜色, 这个是程序类型, 或者未保存时的星号使用的颜色, 激活时的文本颜色
    _Basics_GETCOLOR(L"crCaption", L"left",    crTitle_left          , RGB(137, 137, 138), IGI_CONFIG_CRTITLELEFT           ) // [Basics], 中间黑块标题左边的颜色, 这个是源码名或者项目名
    _Basics_GETCOLOR(L"crCaption", L"right",   crTitle_right         , RGB(122, 193, 255), IGI_CONFIG_CRTITLERIGHT          ) // [Basics], 中间黑块标题右边的颜色, 这个是程序类型, 或者未保存时的星号使用的颜色
                                                                                            
    _Basics_GETCOLOR(L"crText",    0,          crText                , RGB(222, 222, 222), IGI_CONFIG_CRTEXT                ) // [Basics], 普通文本绘画用的颜色
    _Basics_GETCOLOR(L"crText",    L"sb",      crTextStatusBar       , RGB(222, 222, 222), IGI_CONFIG_CRTEXTSTATUSBAR       ) // [Basics], 底部状态条的文本颜色
    _Basics_GETCOLOR(L"crText",    L"sb_run",  crTextStatusBar_run   , RGB(222, 222, 222), IGI_CONFIG_CRTEXTSTATUSBAR_RUN   ) // [Basics], 底部状态条运行时的文本颜色
    _Basics_GETCOLOR(L"crText",    L"sb_nf",   crTextStatusBar_nf    , RGB(222, 222, 222), IGI_CONFIG_CRTEXTSTATUSBAR_NF    ) // [Basics], 底部状态条不激活的文本颜色


    _Basics_GETCOLOR(L"crScrollBtn1", L"normal",   crScrollBtn1      , RGB(153, 153, 153), IGI_CONFIG_CRSCROLLBTN1NORRMAL   ) // [Basics], 滚动条按钮1正常颜色
    _Basics_GETCOLOR(L"crScrollBtn1", L"hot",      crScrollBtnHot1   , RGB(28 , 151, 234), IGI_CONFIG_CRSCROLLBTN1HOT       ) // [Basics], 滚动条按钮1热点颜色
    _Basics_GETCOLOR(L"crScrollBtn1", L"down",     crScrollBtnDown1  , RGB(0  , 122, 204), IGI_CONFIG_CRSCROLLBTN1DOWN      ) // [Basics], 滚动条按钮1按下颜色
    _Basics_GETCOLOR(L"crScrollBtn2", L"normal",   crScrollBtn2      , RGB(153, 153, 153), IGI_CONFIG_CRSCROLLBTN2NORRMAL   ) // [Basics], 滚动条按钮2正常颜色
    _Basics_GETCOLOR(L"crScrollBtn2", L"hot",      crScrollBtnHot2   , RGB(28 , 151, 234), IGI_CONFIG_CRSCROLLBTN2HOT       ) // [Basics], 滚动条按钮2热点颜色
    _Basics_GETCOLOR(L"crScrollBtn2", L"down",     crScrollBtnDown2  , RGB(0  , 122, 204), IGI_CONFIG_CRSCROLLBTN2DOWN      ) // [Basics], 滚动条按钮2按下颜色
    _Basics_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb     , RGB(104, 104, 104), IGI_CONFIG_CRSCROLLTHUMBNORRMAL  ) // [Basics], 滚动条滑块颜色
    _Basics_GETCOLOR(L"crScrollThumb",L"hot",      crScrollThumbHot  , RGB(158, 158, 158), IGI_CONFIG_CRSCROLLTHUMBHOT      ) // [Basics], 滚动条滑块热点颜色
    _Basics_GETCOLOR(L"crScrollThumb",L"down",     crScrollThumbDown , RGB(239, 235, 239), IGI_CONFIG_CRSCROLLTHUMBDOWN     ) // [Basics], 滚动条滑块按下颜色
    _Basics_GETCOLOR(L"crScrollBack", 0,           crScrollBack      , RGB(62 , 62 , 66 ), IGI_CONFIG_CRSCROLLBACKGROUND    ) // [Basics], 滚动条背景颜色

    _Basics_GETCOLOR(L"crToolSepline", L"left",    crToolSeplineLeft , RGB(34 , 34 , 34 ), IGI_CONFIG_CRLINELEFT            ) // [Basics], 工具条分割线左边颜色
    _Basics_GETCOLOR(L"crToolSepline", L"right",   crToolSeplineRight, RGB(70 , 70 , 74 ), IGI_CONFIG_CRLINERIGHT           ) // [Basics], 工具条分割线右边颜色
    
    _Basics_GETCOLOR(L"crStatelineNormal"   , 0,   crStatelineNormal , RGB(70 , 70 , 74 ), IGI_CONFIG_CRLINERIGHT           ) // [Basics], 工具条分割线右边颜色
    _Basics_GETCOLOR(L"crStatelineHot"      , 0,   crStatelineHot    , RGB(89 ,168 ,222 ), IGI_CONFIG_CRLINERIGHT           ) // [Basics], 工具条分割线右边颜色

#define _iresFile_IRES(_keyName, _funname, _def) \
    wstr m_iresFile_##_funname;  \
    virtual LPCWSTR _funname(LPCWSTR pValue = 0) {\
        return _ires_name(L"iresFile", m_iresFile_##_funname, pValue, _keyName, _def);\
    }

    _iresFile_IRES(L"Toolbar",            ires_Toolbar            , L"Toolbar.ires"            )  // [iresFile], 工具条资源名
    _iresFile_IRES(L"TreeFne",            ires_TreeFne            , L"TreeFnd.ires"            )  // [iresFile], 支持库树型框资源名
    _iresFile_IRES(L"TreeProgram",        ires_TreeProgram        , L"TreeProgram.ires"        )  // [iresFile], 程序树型框资源名
    _iresFile_IRES(L"TreeControl",        ires_TreeControl        , L"TreeControl.ires"        )  // [iresFile], 组件箱树型框资源名
    _iresFile_IRES(L"TreeExpand",         ires_TreeExpand         , L"TreeExpand.ires"         )  // [iresFile], 树型框展开收缩按钮资源名
    _iresFile_IRES(L"ComboboxLocation",   ires_ComboboxLocation   , L"ComboboxLocation.ires"   )  // [iresFile], 组合框资源名
    _iresFile_IRES(L"MenuItem",           ires_MenuItem           , L"MenuItem.ires"           )  // [iresFile], 菜单资源名
    _iresFile_IRES(L"msctls_updown32_lr", ires_msctls_updown32_lr , L"msctls_updown32_lr.ires" )  // [iresFile], 调节器左右箭头资源名
    _iresFile_IRES(L"msctls_updown32_tb", ires_msctls_updown32_tb , L"msctls_updown32_tb.ires" )  // [iresFile], 调节器上下箭头资源名
    _iresFile_IRES(L"TopState",           ires_TopState           , L"TopState.ires"           )  // [iresFile], 顶部状态资源名
    _iresFile_IRES(L"icon",               ires_icon               , L"icon.ires"               )  // [iresFile], 默认资源资源名

#define _iresFile_CLR(_keyName, _funname, _def) \
    wstr m_clr_##_funname;  \
    virtual LPCWSTR _funname(LPCWSTR pValue = 0) {\
        return _ires_name(L"clr", m_clr_##_funname, pValue, _keyName, _def);\
    }
    _iresFile_CLR (L"clr",                clr                     , L""                        )  // [clr], 当前配色使用的配色文件

#define _DefaultColor_GETCOLOR(_name, _attrName, _funname, _def, _IGI) \
    COLORREF m_##_funname_DefaultColor = 0;  \
    virtual COLORREF _funname (COLORREF* pValue = 0) {\
        return _iconfig_color(L"DefaultColor", m_##_funname_DefaultColor, pValue, _name, _attrName, _def, _IGI);\
    }

    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_BACK      )   // [DefaultColor], 默认配色, 背景颜色
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_NORMAL    )   // [DefaultColor], 默认配色, 正常颜色
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_HOT       )   // [DefaultColor], 默认配色, 热点颜色
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_DOWN      )   // [DefaultColor], 默认配色, 按下颜色
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_BAN       )   // [DefaultColor], 默认配色, 禁止颜色
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_HOVER     )   // [DefaultColor], 默认配色, 进入颜色
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_FOCUS     )   // [DefaultColor], 默认配色, 焦点颜色
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_CHECKED   )   // [DefaultColor], 默认配色, 选中颜色
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_SELECTED  )   // [DefaultColor], 默认配色, 选择颜色
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_VISTED    )   // [DefaultColor], 默认配色, 
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_SHADOW    )   // [DefaultColor], 默认配色, 阴影颜色
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_BORDER    )   // [DefaultColor], 默认配色, 边框颜色
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_TEXT      )   // [DefaultColor], 默认配色, 文本颜色
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_TEXTHOT   )   // [DefaultColor], 默认配色, 文本热点颜色
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_TEXTDOWN  )   // [DefaultColor], 默认配色, 文本按下颜色
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_TEXTBAN   )   // [DefaultColor], 默认配色, 文本禁止颜色




#define _ICustomCodeTab_(_name, _def, _IGI) \
    COLORREF m_CustomCodeTab_##_name = 0;  \
    virtual COLORREF _name (COLORREF* pValue = 0) {\
        return _iconfig_color(L"CustomCodeTab", m_CustomCodeTab_##_name, pValue, L###_name, 0, _def, _IGI);\
    }

    _ICustomCodeTab_(close_hot , RGB(65, 65, 68), IGI_CONFIG_CODETAB_CRCLOSEHOT   )   // [CustomCodeTab], 代码切换选择夹关闭按钮热点颜色
    _ICustomCodeTab_(close_down, RGB(45, 45, 48), IGI_CONFIG_CODETAB_CRCLOSEDOWN  )   // [CustomCodeTab], 代码切换选择夹关闭按钮按下颜色


    _ICustomCodeTab_(crTab_text_window          , RGB(232, 201, 187) , IGI_CONFIG_CODETAB_CRTEXT_WINDOW           )   // [CustomCodeTab], 代码切换选择夹, 窗口文本颜色
    _ICustomCodeTab_(crTab_text_assembly        , RGB(220, 220, 170) , IGI_CONFIG_CODETAB_CRTEXT_ASSEMBLY         )   // [CustomCodeTab], 代码切换选择夹, 程序集文本颜色
    _ICustomCodeTab_(crTab_text_assembly_window , RGB(232, 201, 187) , IGI_CONFIG_CODETAB_CRTEXT_ASSEMBLY_WINDOW  )   // [CustomCodeTab], 代码切换选择夹, 窗口程序集文本颜色
    _ICustomCodeTab_(crTab_text_class           , RGB(78, 201, 176)  , IGI_CONFIG_CODETAB_CRTEXT_CLASS            )   // [CustomCodeTab], 代码切换选择夹, 类文本颜色
    _ICustomCodeTab_(crTab_text_globalvar       , RGB(128, 128, 0)   , IGI_CONFIG_CODETAB_CRTEXT_GLOBALVAR        )   // [CustomCodeTab], 代码切换选择夹, 全局变量文本颜色
    _ICustomCodeTab_(crTab_text_struct          , RGB(78, 201, 176)  , IGI_CONFIG_CODETAB_CRTEXT_STRUCT           )   // [CustomCodeTab], 代码切换选择夹, 自定义数据类型文本颜色
    _ICustomCodeTab_(crTab_text_dll             , RGB(128, 128, 0)   , IGI_CONFIG_CODETAB_CRTEXT_DLL              )   // [CustomCodeTab], 代码切换选择夹, DLL文本颜色
    _ICustomCodeTab_(crTab_text_const           , RGB(222, 97, 222)  , IGI_CONFIG_CODETAB_CRTEXT_CONST            )   // [CustomCodeTab], 代码切换选择夹, 常量文本颜色
    _ICustomCodeTab_(crTab_text_image           , RGB(128, 128, 0)   , IGI_CONFIG_CODETAB_CRTEXT_IMAGE            )   // [CustomCodeTab], 代码切换选择夹, 图片文本颜色
    _ICustomCodeTab_(crTab_text_sound           , RGB(128, 128, 0)   , IGI_CONFIG_CODETAB_CRTEXT_SOUND            )   // [CustomCodeTab], 代码切换选择夹, 声音文本颜色
    
    _ICustomCodeTab_(crTab_line_window          , RGB(0, 0, 0)       , IGI_CONFIG_CODETAB_CRLINE_WINDOW           )   // [CustomCodeTab], 代码切换选择夹, 窗口下边线颜色
    _ICustomCodeTab_(crTab_line_assembly        , RGB(0, 0, 0)       , IGI_CONFIG_CODETAB_CRLINE_ASSEMBLY         )   // [CustomCodeTab], 代码切换选择夹, 程序集下边线颜色
    _ICustomCodeTab_(crTab_line_assembly_window , RGB(0, 0, 0)       , IGI_CONFIG_CODETAB_CRLINE_ASSEMBLY_WINDOW  )   // [CustomCodeTab], 代码切换选择夹, 窗口程序集下边线颜色
    _ICustomCodeTab_(crTab_line_class           , RGB(0, 0, 0)       , IGI_CONFIG_CODETAB_CRLINE_CLASS            )   // [CustomCodeTab], 代码切换选择夹, 类下边线颜色
    _ICustomCodeTab_(crTab_line_globalvar       , RGB(0, 0, 0)       , IGI_CONFIG_CODETAB_CRLINE_GLOBALVAR        )   // [CustomCodeTab], 代码切换选择夹, 全局变量下边线颜色
    _ICustomCodeTab_(crTab_line_struct          , RGB(0, 0, 0)       , IGI_CONFIG_CODETAB_CRLINE_STRUCT           )   // [CustomCodeTab], 代码切换选择夹, 自定义数据类型下边线颜色
    _ICustomCodeTab_(crTab_line_dll             , RGB(0, 0, 0)       , IGI_CONFIG_CODETAB_CRLINE_DLL              )   // [CustomCodeTab], 代码切换选择夹, DLL下边线颜色
    _ICustomCodeTab_(crTab_line_const           , RGB(0, 0, 0)       , IGI_CONFIG_CODETAB_CRLINE_CONST            )   // [CustomCodeTab], 代码切换选择夹, 常量下边线颜色
    _ICustomCodeTab_(crTab_line_image           , RGB(0, 0, 0)       , IGI_CONFIG_CODETAB_CRLINE_IMAGE            )   // [CustomCodeTab], 代码切换选择夹, 图片下边线颜色
    _ICustomCodeTab_(crTab_line_sound           , RGB(0, 0, 0)       , IGI_CONFIG_CODETAB_CRLINE_SOUND            )   // [CustomCodeTab], 代码切换选择夹, 声音下边线颜色

private:
    // 不会返回原来的值, 只会返回设置后的值
    LPCWSTR _ires_name(LPCWSTR appName, wstr& value, LPCWSTR pValue, LPCWSTR keyName, LPCWSTR defValue)
    {
        if (pValue) // 这个有值就是需要写入
        {
            value.assign(pValue);
            m_ini->write(appName, keyName, pValue);
            return value.c_str();
        }
        if (m_isReadFile)
        {
            value = m_ini->read(appName, keyName, defValue);
            return value.c_str();
        }
        return value.c_str();
    }

};

IConfig* _window_get_config()
{
    static Config m_config(s_info->ini, true);
    static ThemeColor m_theme_color(s_info->iniColor, true);
    s_info->themeColor = &m_theme_color;
    s_info->config = &m_config;
    m_isReadFile = true;

    m_configIsChange.clear();
    m_colorIsChange.clear();

    m_config.IsRunSave                      ();     // 运行/编译后是否保存源码, 取值需要传递一个指针接收结果
    m_config.IsDxRender                     ();     // 是否DX渲染, 取值需要传递一个指针接收结果
    m_config.IsDPIAware                     ();     // 是否开启DPI感知
    m_config.IsDPIScaleWnd                  ();     // DPI改变时是否缩放窗口
    m_config.IsShowCodeTabClose             ();     // 显示代码选择夹的关闭按钮
    m_config.IsShowLocationTool             ();     // 是否显示定位工具条
    m_config.programTreeDblclk              ();     // 程序树型框双击模式, 0=只切换, 1=只展开, 2=展开+切换
    m_config.wheelPos                       ();     // 鼠标滚动一次的距离
    m_config.noClipChildren                 ();     // 不排除子窗口区域
    m_config.MenuItemHeight                 ();     // 菜单项高度
    m_config.MenuSeparatorHeight            ();     // 分隔菜单项高度
    m_config.noChangeTab_work               ();     // 不自动切换工作夹
    m_config.noChangeTab_state              ();     // 不自动切换状态夹
    int scrollWidth = m_config.scroll_width ();     // 滚动条的宽度, 默认是16, 横向滚动条就是高度

    if ( scrollWidth <= 0 )
    {
        scrollWidth = 12;
        m_config.scroll_width(&scrollWidth);    // 小于等于0, 那就给个默认值
    }
    scrollWidth = m_config.scroll_btn_size();   // 滚动条按钮尺寸

    if ( scrollWidth <= 0 )
    {
        scrollWidth = 10;
        m_config.scroll_btn_size(&scrollWidth);    // 小于等于0, 那就给个默认值
    }

    m_config.tree_fne_ishot                 ();     // 支持库树型框是否支持热点
    m_config.tree_program_ishot             ();     // 程序树型框是否支持热点
    m_config.tree_control_ishot             ();     // 组件箱树型框是否支持热点
    m_config.tree_control_orgIcon           ();     // 组件箱树型框显示原图标
    m_config.tree_var_ishot                 ();     // 变量表树型框是否支持热点
    m_config.tree_call_ishot                ();     // 调用表树型框是否支持热点
    m_config.list_find1_ishot               ();     // 搜寻1列表框是否支持热点
    m_config.list_find2_ishot               ();     // 搜寻1列表框树型框是否支持热点

    m_config.tree_fne_height                ();     // 支持库树型框项目高度
    m_config.tree_program_height            ();     // 程序树型框项目高度
    m_config.tree_control_height            ();     // 组件箱树型框项目高度
    m_config.tree_var_height                ();     // 变量表树型框项目高度
    m_config.tree_call_height               ();     // 调用表树型框项目高度
    m_config.list_find1_height              ();     // 搜寻1列表框项目高度
    m_config.list_find2_height              ();     // 搜寻2列表框项目高度


    m_config.tab_work_direction             ();     // 工作夹方向, 0=左边, 1=顶边, 2=右边, 3=底边
    m_config.tab_work_isIcon                ();     // 工作夹是否显示图标
    m_config.tab_work_flags                 ();     // 工作夹标识, 记录位置, 是否最大化, 是否浮动, 是否停靠等信息 TOP_STATE_BUTTON 枚举常量里的值, 调用 __query_flags 判断值

    m_config.tab_state_direction            ();     // 状态夹方向, 0=左边, 1=顶边, 2=右边, 3=底边
    m_config.tab_state_isIcon               ();     // 状态夹是否显示图标
    m_config.tab_state_flags                ();     // 状态夹标识, 记录位置, 是否最大化, 是否浮动, 是否停靠等信息 TOP_STATE_BUTTON 枚举常量里的值, 调用 __query_flags 判断值

    m_config.tab_control_flags              ();     // 组件箱标识, 记录位置, 是否最大化, 是否浮动, 是否停靠等信息 TOP_STATE_BUTTON 枚举常量里的值, 调用 __query_flags 判断值

    m_config.tab_code_direction             ();     // 代码切换选择夹方向, 3=底边, 非3=顶边
    m_config.tab_code_isIcon                ();     // 代码切换选择夹是否显示图标

    m_config.rcWork                         ();     // 工作夹窗口位置, 其他组件根据这个矩形计算位置
    m_config.rcState                        ();     // 状态夹窗口位置, 其他组件根据这个矩形计算位置
    m_config.rcControl                      ();     // 组件箱窗口位置, 其他组件根据这个矩形计算位置
    m_config.rcMenu                         ();     // 菜单夹窗口位置, 其他组件根据这个矩形计算位置
    m_config.rcMDIClient                    ();     // MDIClient窗口位置, 其他组件根据这个矩形计算位置

    m_config.rcWork1                        ();     // 工作夹窗口位置, 显示到屏幕的位置
    m_config.rcState1                       ();     // 状态夹窗口位置, 显示到屏幕的位置
    m_config.rcControl1                     ();     // 组件箱窗口位置, 显示到屏幕的位置

    m_theme_color.crBack                    ();     // 背景颜色
    m_theme_color.crBackExtern              ();     // 外部的背景颜色, 比如选择夹的子夹
    m_theme_color.crStateBar                ();     // 组件箱/工作夹/状态夹顶部激活的颜色
    m_theme_color.crBorder                  ();     // 边框颜色
    m_theme_color.crBorder_run              ();     // 运行时边框颜色
    m_theme_color.crBorder_nf               ();     // 取消激活时的边框颜色

    m_theme_color.crShadow                  ();     // 阴影颜色
    m_theme_color.crShadow_run              ();     // 运行时阴影颜色
    m_theme_color.crShadow_nf               ();     // 取消激活时的边框颜色

    m_theme_color.crMenuBorder              ();     // 菜单边框颜色
    m_theme_color.crMenuBack                ();     // 菜单背景颜色, 弹出菜单使用的背景色, 菜单条按下时使用的背景色
    m_theme_color.crMenuHot                 ();     // 菜单热点项颜色
    m_theme_color.crMenuBan                 ();     // 菜单禁止颜色
    m_theme_color.crMenuText                ();     // 菜单文本颜色
    m_theme_color.crMenuTextHot             ();     // 菜单文本热点颜色
    m_theme_color.crMenuTextBan             ();     // 菜单文本禁止颜色

    m_theme_color.crMenuBarHot              ();     // 顶部菜单条热点项颜色
    m_theme_color.crMenuBarToolsText        ();     // 顶部菜单条助手的文本颜色
    m_theme_color.crMenuBarToolsTextHot     ();     // 顶部菜单条助手的文本热点颜色

    m_theme_color.crTitleback               ();     // 中间黑块标题的背景颜色
    m_theme_color.crTitle_left_act          ();     // 中间黑块标题左边的颜色, 这个是源码名或者项目名, 激活时的文本颜色
    m_theme_color.crTitle_right_act         ();     // 中间黑块标题右边的颜色, 这个是程序类型, 或者未保存时的星号使用的颜色, 激活时的文本颜色
    m_theme_color.crTitle_left              ();     // 中间黑块标题左边的颜色, 这个是源码名或者项目名
    m_theme_color.crTitle_right             ();     // 中间黑块标题右边的颜色, 这个是程序类型, 或者未保存时的星号使用的颜色

    m_theme_color.crText                    ();     // 普通文本绘画用的颜色
    m_theme_color.crTextStatusBar           ();     // 底部状态条的文本颜色
    m_theme_color.crTextStatusBar_run       ();     // 底部状态条运行时的文本颜色
    m_theme_color.crTextStatusBar_nf        ();     // 底部状态条不激活的文本颜色


    m_theme_color.crScrollBtn1              ();     // 滚动条按钮1正常颜色
    m_theme_color.crScrollBtnHot1           ();     // 滚动条按钮1热点颜色
    m_theme_color.crScrollBtnDown1          ();     // 滚动条按钮1按下颜色
    m_theme_color.crScrollBtn2              ();     // 滚动条按钮2正常颜色
    m_theme_color.crScrollBtnHot2           ();     // 滚动条按钮2热点颜色
    m_theme_color.crScrollBtnDown2          ();     // 滚动条按钮2按下颜色
    m_theme_color.crScrollThumb             ();     // 滚动条滑块颜色
    m_theme_color.crScrollThumbHot          ();     // 滚动条滑块热点颜色
    m_theme_color.crScrollThumbDown         ();     // 滚动条滑块按下颜色
    m_theme_color.crScrollBack              ();     // 滚动条背景颜色

    m_theme_color.crToolSeplineLeft         ();     // 工具条分割线左边颜色
    m_theme_color.crToolSeplineRight        ();     // 工具条分割线右边颜色


    m_theme_color.crStatelineNormal         ();     // [Basics], 工具条分割线右边颜色
    m_theme_color.crStatelineHot            ();     // [Basics], 工具条分割线右边颜色

    m_theme_color.ires_Toolbar              ();     // 工具条资源名
    m_theme_color.ires_TreeFne              ();     // 支持库树型框资源名
    m_theme_color.ires_TreeProgram          ();     // 程序树型框资源名
    m_theme_color.ires_TreeControl          ();     // 组件箱树型框资源名
    m_theme_color.ires_TreeExpand           ();     // 树型框展开收缩按钮资源名
    m_theme_color.ires_ComboboxLocation     ();     // 组合框资源名
    m_theme_color.ires_MenuItem             ();     // 菜单资源名
    m_theme_color.ires_msctls_updown32_lr   ();     // 调节器左右箭头资源名
    m_theme_color.ires_msctls_updown32_tb   ();     // 调节器上下箭头资源名
    m_theme_color.ires_TopState             ();     // 顶部状态资源名
    m_theme_color.ires_icon                 ();     // 默认资源资源名
    m_theme_color.clr                       ();     // 默认资源资源名


    
    iConfig_Reload_CustomTabColor(false);


    m_isReadFile = false;
    m_isFirst = false;
    return &m_config;
}



void iConfig_Reload_CustomTabColor(bool isFirst)
{
    bool isReadFile = m_isReadFile;
    m_isReadFile = true;
    if (isFirst)
        m_isFirst = true;   // 重新读取
    IThemeColor& m_theme_color = *s_info->themeColor;


    m_theme_color.close_hot                     ();   // [CustomCodeTab], 代码切换选择夹关闭按钮热点颜色
    m_theme_color.close_down                    ();   // [CustomCodeTab], 代码切换选择夹关闭按钮按下颜色

    m_theme_color.crTab_text_window             ();   // [CustomCodeTab], 代码切换选择夹, 窗口文本颜色
    m_theme_color.crTab_text_assembly           ();   // [CustomCodeTab], 代码切换选择夹, 程序集文本颜色
    m_theme_color.crTab_text_assembly_window    ();   // [CustomCodeTab], 代码切换选择夹, 窗口程序集文本颜色
    m_theme_color.crTab_text_class              ();   // [CustomCodeTab], 代码切换选择夹, 类文本颜色
    m_theme_color.crTab_text_globalvar          ();   // [CustomCodeTab], 代码切换选择夹, 全局变量文本颜色
    m_theme_color.crTab_text_struct             ();   // [CustomCodeTab], 代码切换选择夹, 自定义数据类型文本颜色
    m_theme_color.crTab_text_dll                ();   // [CustomCodeTab], 代码切换选择夹, DLL文本颜色
    m_theme_color.crTab_text_const              ();   // [CustomCodeTab], 代码切换选择夹, 常量文本颜色
    m_theme_color.crTab_text_image              ();   // [CustomCodeTab], 代码切换选择夹, 图片文本颜色
    m_theme_color.crTab_text_sound              ();   // [CustomCodeTab], 代码切换选择夹, 声音文本颜色

    m_theme_color.crTab_line_window             ();   // [CustomCodeTab], 代码切换选择夹, 窗口下边线颜色
    m_theme_color.crTab_line_assembly           ();   // [CustomCodeTab], 代码切换选择夹, 程序集下边线颜色
    m_theme_color.crTab_line_assembly_window    ();   // [CustomCodeTab], 代码切换选择夹, 窗口程序集下边线颜色
    m_theme_color.crTab_line_class              ();   // [CustomCodeTab], 代码切换选择夹, 类下边线颜色
    m_theme_color.crTab_line_globalvar          ();   // [CustomCodeTab], 代码切换选择夹, 全局变量下边线颜色
    m_theme_color.crTab_line_struct             ();   // [CustomCodeTab], 代码切换选择夹, 自定义数据类型下边线颜色
    m_theme_color.crTab_line_dll                ();   // [CustomCodeTab], 代码切换选择夹, DLL下边线颜色
    m_theme_color.crTab_line_const              ();   // [CustomCodeTab], 代码切换选择夹, 常量下边线颜色
    m_theme_color.crTab_line_image              ();   // [CustomCodeTab], 代码切换选择夹, 图片下边线颜色
    m_theme_color.crTab_line_sound              ();   // [CustomCodeTab], 代码切换选择夹, 声音下边线颜色


    m_isReadFile = isReadFile;
    if (isFirst)
        m_isFirst = false;
}