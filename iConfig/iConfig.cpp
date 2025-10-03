#include "iConfig.h"
#include <control/ReadColor.h>
#include <iDraw_const.h>
static bool m_isReadFile;                       // �Ƿ���ļ��ж�ȡ
static std::vector<int> m_colorIsChange;        // ��ɫ���ñ��ı����Ŀ
static std::vector<int> m_configIsChange;       // �����ñ��ı����Ŀ
static bool m_isFirst = true;                   // �Ƿ��ǵ�һ�μ���, ����һ��֮���Ϊfalse


inline static RECT _iconfig_setvalueRECT(CPrivateProfile* ini, LPCWSTR appName, RECT* value, RECT* pValue, LPCWSTR nodeName, LPCWSTR attrName)
{
    LPCWSTR file = ini->m_file.c_str();
    RECT& ret = *value;
    RECT r = ret;

    wstr name(260);
    name.assign(nodeName);
    if (attrName) name.append(attrName);
    ret = *pValue;
    wstr vl; vl.Format(L"%d,%d,%d,%d", ret.left, ret.top, ret.right, ret.bottom);    // Ҫд���������ֵ
    WritePrivateProfileStringW(appName, name.c_str(), vl.c_str(), file);
    return r;
}
// ��ֵд�뵽�����ļ���, ����ֵд�뵽���Ա������, ֻ֧��4���ֽڵĳ�Ա, ��������֮ǰ��ֵ
// ini = д�뵽�ĸ�ini��
// appName = ����
// value = ���Ա��ַ, �����µ�ֵ���µ�ֵ��д�������ַ��
// pValue = Ҫд���ֵ�ĵ�ַ, �������ַ�������д�뵽�����ļ���
// nodeName = ����ǰ׺
// attrName = ��Ŀ��׺, ���Ϊ0��ʹ�����
inline static int _iconfig_setvalue(CPrivateProfile* ini, LPCWSTR appName, void* value, void* pValue, LPCWSTR nodeName, LPCWSTR attrName)
{
    LPCWSTR file = ini->m_file.c_str();
    int& ret = (*((int*)(value)));
    int r = ret;

    wstr name(260);
    name.assign(nodeName);
    if (attrName) name.append(attrName);
    ret = (*((int*)pValue));
    wstr vl; vl.assign(ret);    // Ҫд���������ֵ
    WritePrivateProfileStringW(appName, name.c_str(), vl.c_str(), file);
    return r;
}

// ��ȡ������, pValue = 0���ȡ
// ini = ���ĸ�ini���в���
// appName = ����
// nodeName = ����ǰ׺
// attrName = ��Ŀ��׺, ���Ϊ0��ʹ�����
// defValue = Ĭ���ı�, ������ò�����, ������Ĭ��ֵд�������ļ���
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
        // �Ƿ�������øı��¼�, ���ǵ�һ�μ���, ���¾����ò�һ��
        const bool isCallConfigChange = !m_isFirst && m_vl != oldValue;

        if (isCallConfigChange)
        {
            const bool isReadFile = m_isReadFile;
            m_isReadFile = false;
            iConfig_InfoChanged(IGI, oldValue, m_vl, true);   // ֪ͨ���ñ��ı�
            m_isReadFile = isReadFile;
        }
        return m_vl;
    }
    inline RECT _read_rc(RECT& m_vl, RECT* pValue, LPCWSTR appName, LPCWSTR name, LPCWSTR attrName, LPCWSTR def)
    {
        if (pValue)
            return _iconfig_setvalueRECT(m_ini, appName, &m_vl, pValue, name, attrName); 
        if(!m_isReadFile) return m_vl;
        RECT& rc = m_vl;    // λ�ò����ı��ж�
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
    _IIConfig_fun_name(L"iDraw", IsRunSave            ,BOOL , L"IsRunSave",          0          , 1 , IGI_CONFIG_ISRUNSAVE)             // ����/������Ƿ񱣴�Դ��, ȡֵ��Ҫ����һ��ָ����ս��
    _IIConfig_fun_name(L"iDraw", IsDxRender           ,BOOL , L"isDxRender",         0          , 1 , IGI_CONFIG_ISDXRENDER)            // �Ƿ�DX��Ⱦ, ȡֵ��Ҫ����һ��ָ����ս��
    _IIConfig_fun_name(L"iDraw", IsDPIAware           ,BOOL , L"IsDPIAware",         0          , 0 , IGI_CONFIG_ISDPIAWARE)            // �Ƿ���DPI��֪
    _IIConfig_fun_name(L"iDraw", IsDPIScaleWnd        ,BOOL , L"IsDPIScaleWnd",      0          , 1 , IGI_CONFIG_ISDPISCALEWND)         // DPI�ı�ʱ�Ƿ����Ŵ���
    _IIConfig_fun_name(L"iDraw", IsShowCodeTabClose   ,BOOL , L"IsShowCodeTabClose", 0          , 1 , IGI_CONFIG_SHOWCODETABCLOSE)      // ��ʾ����ѡ��еĹرհ�ť
    _IIConfig_fun_name(L"iDraw", IsShowLocationTool   ,BOOL , L"IsShowLocationTool", 0          , 1 , IGI_CONFIG_ISLOCATIONTOOLBAR)     // �Ƿ���ʾ��λ������
    _IIConfig_fun_name(L"iDraw", programTreeDblclk    ,int  , L"programTreeDblclk",  0          , 2 , IGI_CONFIG_PROGRAMDBLCLKTYPE)     // �������Ϳ�˫��ģʽ, 0=ֻ�л�, 1=ֻչ��, 2=չ��+�л�
    _IIConfig_fun_name(L"iDraw", wheelPos             ,int  , L"wheelPos",           0          , 3 , IGI_CONFIG_WHEELPOS)              // ������һ�εľ���
    _IIConfig_fun_name(L"iDraw", noClipChildren       ,BOOL , L"noClipChildren",     0          , 0 , IGI_CONFIG_NOCLIPCHILDREN)        // ���ų��Ӵ�������
    _IIConfig_fun_name(L"iDraw", MenuItemHeight       ,int  , L"MenuItemHeight",     0          , 24, IGI_CONFIG_MENUITEMHEIGHT)        // �˵���߶�
    _IIConfig_fun_name(L"iDraw", MenuSeparatorHeight  ,int  , L"MenuItemHeight", L"nSeparator"  , 5 , IGI_CONFIG_MENUSEPARATORHEIGHT)   // �ָ��˵���߶�
    _IIConfig_fun_name(L"iDraw", noChangeTab_work     ,int  , L"noChangeTab_work",   0          , 0 , IGI_CONFIG_NOCHANGETAB_WORK)      // ���Զ��л�������
    _IIConfig_fun_name(L"iDraw", noChangeTab_state    ,int  , L"noChangeTab_state",  0          , 0 , IGI_CONFIG_NOCHANGETAB_STATE)     // ���Զ��л�״̬��
    _IIConfig_fun_name(L"iDraw", scroll_width         ,int  , L"scroll_width",       0          , 12, IGI_CONFIG_SCROLL_WIDTH)          // ���������
    _IIConfig_fun_name(L"iDraw", scroll_btn_size      ,int  , L"scroll_btn_size",    0          , 10, IGI_CONFIG_SCROLL_BTN_SIZE)       // ��������ť�ߴ�

    _IIConfig_fun_name(L"iDraw", tree_fne_ishot       ,BOOL , L"fne_tree",       L"ishot"       , 1 , IGI_CONFIG_TREE_FNE_ISHOT)        // ֧�ֿ����Ϳ��Ƿ�֧���ȵ�
    _IIConfig_fun_name(L"iDraw", tree_program_ishot   ,BOOL , L"tree_program",   L"ishot"       , 1 , IGI_CONFIG_TREE_PROGRAM_ISHOT)    // �������Ϳ��Ƿ�֧���ȵ�
    _IIConfig_fun_name(L"iDraw", tree_control_ishot   ,BOOL , L"tree_control",   L"ishot"       , 1 , IGI_CONFIG_TREE_CONTROL_ISHOT)    // ��������Ϳ��Ƿ�֧���ȵ�
    _IIConfig_fun_name(L"iDraw", tree_control_orgIcon ,BOOL , L"tree_control",   L"orgIcon"     , 0 , IGI_CONFIG_TREE_CONTROL_ORGICON)  // ��������Ϳ���ʾԭͼ��
    _IIConfig_fun_name(L"iDraw", tree_var_ishot       ,BOOL , L"tree_var",       L"ishot"       , 1 , IGI_CONFIG_TREE_VAR_ISHOT)        // ���������Ϳ��Ƿ�֧���ȵ�
    _IIConfig_fun_name(L"iDraw", tree_call_ishot      ,BOOL , L"tree_call",      L"ishot"       , 1 , IGI_CONFIG_TREE_CALL_ISHOT)       // ���ñ����Ϳ��Ƿ�֧���ȵ�
    _IIConfig_fun_name(L"iDraw", list_find1_ishot     ,BOOL , L"list_find1",     L"ishot"       , 1 , IGI_CONFIG_LIST_FIND1_ISHOT)      // ��Ѱ1�б���Ƿ�֧���ȵ�
    _IIConfig_fun_name(L"iDraw", list_find2_ishot     ,BOOL , L"list_find2",     L"ishot"       , 1 , IGI_CONFIG_LIST_FIND2_ISHOT)      // ��Ѱ1�б�����Ϳ��Ƿ�֧���ȵ�

    _IIConfig_fun_name(L"iDraw", tree_fne_height      ,int  , L"fne_tree",       L"itemHeight"  , 24, IGI_CONFIG_TREE_FNE_HEIGHT)       // ֧�ֿ����Ϳ���Ŀ�߶�
    _IIConfig_fun_name(L"iDraw", tree_program_height  ,int  , L"tree_program",   L"itemHeight"  , 24, IGI_CONFIG_TREE_PROGRAM_HEIGHT)   // �������Ϳ���Ŀ�߶�
    _IIConfig_fun_name(L"iDraw", tree_control_height  ,int  , L"tree_control",   L"itemHeight"  , 32, IGI_CONFIG_TREE_CONTROL_HEIGHT)   // ��������Ϳ���Ŀ�߶�
    _IIConfig_fun_name(L"iDraw", tree_var_height      ,int  , L"tree_var",       L"itemHeight"  , 24, IGI_CONFIG_TREE_VAR_HEIGHT)       // ���������Ϳ���Ŀ�߶�
    _IIConfig_fun_name(L"iDraw", tree_call_height     ,int  , L"tree_call",      L"itemHeight"  , 24, IGI_CONFIG_TREE_CALL_HEIGHT)      // ���ñ����Ϳ���Ŀ�߶�
    _IIConfig_fun_name(L"iDraw", list_find1_height    ,int  , L"list_find1",     L"itemHeight"  , 24, IGI_CONFIG_LIST_FIND1_HEIGHT)     // ��Ѱ1�б����Ŀ�߶�
    _IIConfig_fun_name(L"iDraw", list_find2_height    ,int  , L"list_find2",     L"itemHeight"  , 24, IGI_CONFIG_LIST_FIND2_HEIGHT)     // ��Ѱ2�б����Ŀ�߶�


            
    _IIConfig_fun_name(L"EWindowSize", tab_work_direction   , int,  L"work",      L"direction",3, IGI_CONFIG_WORK_DIRECTION)     // �����з���, 0=���, 1=����, 2=�ұ�, 3=�ױ�
    _IIConfig_fun_name(L"EWindowSize", tab_work_isIcon      , BOOL, L"work",      L"isIcon",   0, IGI_CONFIG_WORK_ISICON)        // �������Ƿ���ʾͼ��
    _IIConfig_fun_name(L"EWindowSize", tab_work_flags       , BOOL, L"work",      L"flags",    0, IGI_CONFIG_WORK_FLAGS)         // ������λ��, 0=���, 1=����, 2=�ұ�, 3=�ױ�

    _IIConfig_fun_name(L"EWindowSize", tab_state_direction  , int,  L"state",     L"direction",3, IGI_CONFIG_STATE_DIRECTION)    // ״̬�з���, 0=���, 1=����, 2=�ұ�, 3=�ױ�
    _IIConfig_fun_name(L"EWindowSize", tab_state_isIcon     , BOOL, L"state",     L"isIcon",   0, IGI_CONFIG_STATE_ISICON)       // ״̬���Ƿ���ʾͼ��
    _IIConfig_fun_name(L"EWindowSize", tab_state_flags      , BOOL, L"state",     L"flags",    0, IGI_CONFIG_STATE_FLAGS)        // ״̬��λ��, 0=���, 1=����, 2=�ұ�, 3=�ױ�

    _IIConfig_fun_name(L"EWindowSize", tab_control_flags    , BOOL, L"control",   L"flags",    0, IGI_CONFIG_CONTROL_FLAGS)      // �����λ��, 0=���, 1=����, 2=�ұ�, 3=�ױ�


    _IIConfig_fun_name(L"EWindowSize", tab_code_direction   , int,  L"code",      L"direction",1, IGI_CONFIG_CODE_DIRECTION)     // �����л�ѡ��з���, 3=�ױ�, ��3=����
    _IIConfig_fun_name(L"EWindowSize", tab_code_isIcon      , BOOL, L"code",      L"isIcon",   1, IGI_CONFIG_CODE_ISICON)        // �����л�ѡ����Ƿ���ʾͼ��

    _IIConfig_fun_name_RECT(L"EWindowSize", rcWork          , L"work",        L"RECT", L"0,0,0,0")     // �����д���λ��, �����������������μ���λ��
    _IIConfig_fun_name_RECT(L"EWindowSize", rcState         , L"state",       L"RECT", L"0,0,0,0")     // ״̬�д���λ��, �����������������μ���λ��
    _IIConfig_fun_name_RECT(L"EWindowSize", rcControl       , L"control",     L"RECT", L"0,0,0,0")     // ����䴰��λ��, �����������������μ���λ��
    _IIConfig_fun_name_RECT(L"EWindowSize", rcMenu          , L"menu",        L"RECT", L"0,0,0,0")     // �˵��д���λ��, �����������������μ���λ��
    _IIConfig_fun_name_RECT(L"EWindowSize", rcMDIClient     , L"MDIClient",   L"RECT", L"0,0,0,0")     // MDIClient����λ��, �����������������μ���λ��

    _IIConfig_fun_name_RECT(L"EWindowSize", rcWork1         , L"work1",       L"RECT", L"0,0,0,0")     // �����д���λ��, ��ʾ����Ļ��λ��
    _IIConfig_fun_name_RECT(L"EWindowSize", rcState1        , L"state1",      L"RECT", L"0,0,0,0")     // ״̬�д���λ��, ��ʾ����Ļ��λ��
    _IIConfig_fun_name_RECT(L"EWindowSize", rcControl1      , L"control1",    L"RECT", L"0,0,0,0")     // ����䴰��λ��, ��ʾ����Ļ��λ��
};

class ThemeColor : public IThemeColor
{

    // ��ȡ/д��ڵ�/������ɫֵ, ���ض�ȡ����ֵ
    // xml = ���ĸ����õ�xml���д
    // value = ���ĸ����ö�д, ��Ҫ���ݽṹ��Ա��ָ��, ���õĻ��ڲ���д�������ַ, ��ȡ�Ļ��ʹ������ַ��ȡ
    // pValue = �����Ϊ0, ���ʾ������ֵ, ���û������д��xml
    // nodeName = �ڵ���
    // attrName = ������, ���Ϊ0, ���д�����ʹ�����ڵ��д, ��Ϊ0���������Զ�д
    // defColor = ������ֵ��Ĭ����ɫֵ, ���û���������, ������ֵд�뵽������
    // isReadFile = �Ƿ���ļ����ȡ
    inline int _iconfig_color(LPCWSTR appName, COLORREF& value, COLORREF* pValue, LPCWSTR nodeName, LPCWSTR attrName, int defColor, int IGI)
    {
        if (pValue) // �����ֵ������Ҫд��
        {
            LPCWSTR file = m_ini->m_file.c_str();
            int r = value;

            wstr name(260);
            name.assign(nodeName);
            if (attrName) name.append(attrName);
            value = *pValue;
            wstr vl = rgb2str(value); // Ҫд���������ֵ
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

            // �Ƿ�������øı��¼�, ���ǵ�һ�μ���, ���¾����ò�һ��
            const bool isCallConfigChange = !m_isFirst && oldValue != value;
            if (isCallConfigChange)
            {
                const bool isReadFile = m_isReadFile;
                m_isReadFile = false;
                iConfig_InfoChanged(IGI, oldValue, value, true);   // ֪ͨ���ñ��ı�
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
    _Basics_GETCOLOR(L"crBack",    0,          crBack                , RGB(45, 45, 48)   , IGI_CONFIG_CRBACK            ) // [Basics], ������ɫ
    _Basics_GETCOLOR(L"crBackExtern", 0,       crBackExtern          , RGB(45, 45, 48)   , IGI_CONFIG_CRBACKEXTERN      ) // [Basics], �ⲿ�ı�����ɫ, ����ѡ��е��Ӽ�
    _Basics_GETCOLOR(L"crStateBar", 0,         crStateBar            , RGB(0, 122, 204)  , IGI_CONFIG_CRSTATEBAR        ) // [Basics], �����/������/״̬�ж����������ɫ

    _Basics_GETCOLOR(L"crBorder",  0,          crBorder              , RGB(0, 122, 204)  , IGI_CONFIG_CRBORDER          ) // [Basics], �߿���ɫ
    _Basics_GETCOLOR(L"crBorder",  L"run",     crBorder_run          , RGB(202, 81, 0)   , IGI_CONFIG_CRBORDER_RUN      ) // [Basics], ����ʱ�߿���ɫ
    _Basics_GETCOLOR(L"crBorder",  L"nofocus", crBorder_nf           , RGB(66, 66, 69)   , IGI_CONFIG_CRBORDER_NF       ) // [Basics], ȡ������ʱ�ı߿���ɫ

    _Basics_GETCOLOR(L"crShadow",  0,          crShadow              , RGB(0, 122, 204)  , IGI_CONFIG_CRSHADOW          ) // [Basics], ��Ӱ��ɫ
    _Basics_GETCOLOR(L"crShadow",  L"run",     crShadow_run          , RGB(202, 81, 0)   , IGI_CONFIG_CRSHADOW_RUN      ) // [Basics], ����ʱ��Ӱ��ɫ
    _Basics_GETCOLOR(L"crShadow",  L"nofocus", crShadow_nf           , RGB(66, 66, 69)   , IGI_CONFIG_CRSHADOW_NF       ) // [Basics], ȡ������ʱ�ı߿���ɫ

    _Basics_GETCOLOR(L"crMenu",    0,          crMenuBorder          , RGB(63, 63, 70)   , IGI_CONFIG_CRMENUBORDER      ) // [Basics], �˵��߿���ɫ
    _Basics_GETCOLOR(L"crMenu",    L"back",    crMenuBack            , RGB(27, 27, 28)   , IGI_CONFIG_CRMENUBACK        ) // [Basics], �˵�������ɫ, �����˵�ʹ�õı���ɫ, �˵�������ʱʹ�õı���ɫ
    _Basics_GETCOLOR(L"crMenu",    L"hot",     crMenuHot             , RGB(51, 51, 52)   , IGI_CONFIG_CRMENUHOT         ) // [Basics], �˵��ȵ�����ɫ
    _Basics_GETCOLOR(L"crMenu",    L"ban",     crMenuBan             , RGB(32, 32, 33)   , IGI_CONFIG_CRMENUBAN         ) // [Basics], �˵���ֹ��ɫ
    _Basics_GETCOLOR(L"crMenu",    L"text",    crMenuText            , RGB(222, 222, 222), IGI_CONFIG_CRMENUTEXT        ) // [Basics], �˵��ı���ɫ
    _Basics_GETCOLOR(L"crMenu",    L"texthot", crMenuTextHot         , RGB(222, 222, 222), IGI_CONFIG_CRMENUTEXTHOT     ) // [Basics], �˵��ı��ȵ���ɫ
    _Basics_GETCOLOR(L"crMenu",    L"textban", crMenuTextBan         , RGB(101, 101, 101), IGI_CONFIG_CRMENUTEXTBAN     ) // [Basics], �˵��ı���ֹ��ɫ

    _Basics_GETCOLOR(L"crMenuBar", L"hot",     crMenuBarHot          , RGB(62, 62, 64)   , IGI_CONFIG_CRMENUBARHOT      ) // [Basics], �����˵����ȵ�����ɫ
    _Basics_GETCOLOR(L"crMenuBar", L"text",    crMenuBarToolsText    , RGB(0, 133, 196)  , IGI_CONFIG_CRMENUBARTEXT     ) // [Basics], �����˵������ֵ��ı���ɫ
    _Basics_GETCOLOR(L"crMenuBar", L"texthot", crMenuBarToolsTextHot , RGB(193, 249, 253), IGI_CONFIG_CRMENUBARTEXTHOT  ) // [Basics], �����˵������ֵ��ı��ȵ���ɫ

                                                                                            
    _Basics_GETCOLOR(L"crCaption", 0,          crTitleback           , RGB(37, 37, 38)   , IGI_CONFIG_CRTITLEBACK           ) // [Basics], �м�ڿ����ı�����ɫ
    _Basics_GETCOLOR(L"crCaption", L"lact",    crTitle_left_act      , RGB(241, 241, 241), IGI_CONFIG_CRTITLELEFT_ACT       ) // [Basics], �м�ڿ������ߵ���ɫ, �����Դ����������Ŀ��, ����ʱ���ı���ɫ
    _Basics_GETCOLOR(L"crCaption", L"ract",    crTitle_right_act     , RGB(122, 193, 255), IGI_CONFIG_CRTITLERIGHT_ACT      ) // [Basics], �м�ڿ�����ұߵ���ɫ, ����ǳ�������, ����δ����ʱ���Ǻ�ʹ�õ���ɫ, ����ʱ���ı���ɫ
    _Basics_GETCOLOR(L"crCaption", L"left",    crTitle_left          , RGB(137, 137, 138), IGI_CONFIG_CRTITLELEFT           ) // [Basics], �м�ڿ������ߵ���ɫ, �����Դ����������Ŀ��
    _Basics_GETCOLOR(L"crCaption", L"right",   crTitle_right         , RGB(122, 193, 255), IGI_CONFIG_CRTITLERIGHT          ) // [Basics], �м�ڿ�����ұߵ���ɫ, ����ǳ�������, ����δ����ʱ���Ǻ�ʹ�õ���ɫ
                                                                                            
    _Basics_GETCOLOR(L"crText",    0,          crText                , RGB(222, 222, 222), IGI_CONFIG_CRTEXT                ) // [Basics], ��ͨ�ı��滭�õ���ɫ
    _Basics_GETCOLOR(L"crText",    L"sb",      crTextStatusBar       , RGB(222, 222, 222), IGI_CONFIG_CRTEXTSTATUSBAR       ) // [Basics], �ײ�״̬�����ı���ɫ
    _Basics_GETCOLOR(L"crText",    L"sb_run",  crTextStatusBar_run   , RGB(222, 222, 222), IGI_CONFIG_CRTEXTSTATUSBAR_RUN   ) // [Basics], �ײ�״̬������ʱ���ı���ɫ
    _Basics_GETCOLOR(L"crText",    L"sb_nf",   crTextStatusBar_nf    , RGB(222, 222, 222), IGI_CONFIG_CRTEXTSTATUSBAR_NF    ) // [Basics], �ײ�״̬����������ı���ɫ


    _Basics_GETCOLOR(L"crScrollBtn1", L"normal",   crScrollBtn1      , RGB(153, 153, 153), IGI_CONFIG_CRSCROLLBTN1NORRMAL   ) // [Basics], ��������ť1������ɫ
    _Basics_GETCOLOR(L"crScrollBtn1", L"hot",      crScrollBtnHot1   , RGB(28 , 151, 234), IGI_CONFIG_CRSCROLLBTN1HOT       ) // [Basics], ��������ť1�ȵ���ɫ
    _Basics_GETCOLOR(L"crScrollBtn1", L"down",     crScrollBtnDown1  , RGB(0  , 122, 204), IGI_CONFIG_CRSCROLLBTN1DOWN      ) // [Basics], ��������ť1������ɫ
    _Basics_GETCOLOR(L"crScrollBtn2", L"normal",   crScrollBtn2      , RGB(153, 153, 153), IGI_CONFIG_CRSCROLLBTN2NORRMAL   ) // [Basics], ��������ť2������ɫ
    _Basics_GETCOLOR(L"crScrollBtn2", L"hot",      crScrollBtnHot2   , RGB(28 , 151, 234), IGI_CONFIG_CRSCROLLBTN2HOT       ) // [Basics], ��������ť2�ȵ���ɫ
    _Basics_GETCOLOR(L"crScrollBtn2", L"down",     crScrollBtnDown2  , RGB(0  , 122, 204), IGI_CONFIG_CRSCROLLBTN2DOWN      ) // [Basics], ��������ť2������ɫ
    _Basics_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb     , RGB(104, 104, 104), IGI_CONFIG_CRSCROLLTHUMBNORRMAL  ) // [Basics], ������������ɫ
    _Basics_GETCOLOR(L"crScrollThumb",L"hot",      crScrollThumbHot  , RGB(158, 158, 158), IGI_CONFIG_CRSCROLLTHUMBHOT      ) // [Basics], �����������ȵ���ɫ
    _Basics_GETCOLOR(L"crScrollThumb",L"down",     crScrollThumbDown , RGB(239, 235, 239), IGI_CONFIG_CRSCROLLTHUMBDOWN     ) // [Basics], ���������鰴����ɫ
    _Basics_GETCOLOR(L"crScrollBack", 0,           crScrollBack      , RGB(62 , 62 , 66 ), IGI_CONFIG_CRSCROLLBACKGROUND    ) // [Basics], ������������ɫ

    _Basics_GETCOLOR(L"crToolSepline", L"left",    crToolSeplineLeft , RGB(34 , 34 , 34 ), IGI_CONFIG_CRLINELEFT            ) // [Basics], �������ָ��������ɫ
    _Basics_GETCOLOR(L"crToolSepline", L"right",   crToolSeplineRight, RGB(70 , 70 , 74 ), IGI_CONFIG_CRLINERIGHT           ) // [Basics], �������ָ����ұ���ɫ
    
    _Basics_GETCOLOR(L"crStatelineNormal"   , 0,   crStatelineNormal , RGB(70 , 70 , 74 ), IGI_CONFIG_CRLINERIGHT           ) // [Basics], �������ָ����ұ���ɫ
    _Basics_GETCOLOR(L"crStatelineHot"      , 0,   crStatelineHot    , RGB(89 ,168 ,222 ), IGI_CONFIG_CRLINERIGHT           ) // [Basics], �������ָ����ұ���ɫ

#define _iresFile_IRES(_keyName, _funname, _def) \
    wstr m_iresFile_##_funname;  \
    virtual LPCWSTR _funname(LPCWSTR pValue = 0) {\
        return _ires_name(L"iresFile", m_iresFile_##_funname, pValue, _keyName, _def);\
    }

    _iresFile_IRES(L"Toolbar",            ires_Toolbar            , L"Toolbar.ires"            )  // [iresFile], ��������Դ��
    _iresFile_IRES(L"TreeFne",            ires_TreeFne            , L"TreeFnd.ires"            )  // [iresFile], ֧�ֿ����Ϳ���Դ��
    _iresFile_IRES(L"TreeProgram",        ires_TreeProgram        , L"TreeProgram.ires"        )  // [iresFile], �������Ϳ���Դ��
    _iresFile_IRES(L"TreeControl",        ires_TreeControl        , L"TreeControl.ires"        )  // [iresFile], ��������Ϳ���Դ��
    _iresFile_IRES(L"TreeExpand",         ires_TreeExpand         , L"TreeExpand.ires"         )  // [iresFile], ���Ϳ�չ��������ť��Դ��
    _iresFile_IRES(L"ComboboxLocation",   ires_ComboboxLocation   , L"ComboboxLocation.ires"   )  // [iresFile], ��Ͽ���Դ��
    _iresFile_IRES(L"MenuItem",           ires_MenuItem           , L"MenuItem.ires"           )  // [iresFile], �˵���Դ��
    _iresFile_IRES(L"msctls_updown32_lr", ires_msctls_updown32_lr , L"msctls_updown32_lr.ires" )  // [iresFile], ���������Ҽ�ͷ��Դ��
    _iresFile_IRES(L"msctls_updown32_tb", ires_msctls_updown32_tb , L"msctls_updown32_tb.ires" )  // [iresFile], ���������¼�ͷ��Դ��
    _iresFile_IRES(L"TopState",           ires_TopState           , L"TopState.ires"           )  // [iresFile], ����״̬��Դ��
    _iresFile_IRES(L"icon",               ires_icon               , L"icon.ires"               )  // [iresFile], Ĭ����Դ��Դ��

#define _iresFile_CLR(_keyName, _funname, _def) \
    wstr m_clr_##_funname;  \
    virtual LPCWSTR _funname(LPCWSTR pValue = 0) {\
        return _ires_name(L"clr", m_clr_##_funname, pValue, _keyName, _def);\
    }
    _iresFile_CLR (L"clr",                clr                     , L""                        )  // [clr], ��ǰ��ɫʹ�õ���ɫ�ļ�

#define _DefaultColor_GETCOLOR(_name, _attrName, _funname, _def, _IGI) \
    COLORREF m_##_funname_DefaultColor = 0;  \
    virtual COLORREF _funname (COLORREF* pValue = 0) {\
        return _iconfig_color(L"DefaultColor", m_##_funname_DefaultColor, pValue, _name, _attrName, _def, _IGI);\
    }

    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_BACK      )   // [DefaultColor], Ĭ����ɫ, ������ɫ
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_NORMAL    )   // [DefaultColor], Ĭ����ɫ, ������ɫ
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_HOT       )   // [DefaultColor], Ĭ����ɫ, �ȵ���ɫ
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_DOWN      )   // [DefaultColor], Ĭ����ɫ, ������ɫ
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_BAN       )   // [DefaultColor], Ĭ����ɫ, ��ֹ��ɫ
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_HOVER     )   // [DefaultColor], Ĭ����ɫ, ������ɫ
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_FOCUS     )   // [DefaultColor], Ĭ����ɫ, ������ɫ
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_CHECKED   )   // [DefaultColor], Ĭ����ɫ, ѡ����ɫ
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_SELECTED  )   // [DefaultColor], Ĭ����ɫ, ѡ����ɫ
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_VISTED    )   // [DefaultColor], Ĭ����ɫ, 
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_SHADOW    )   // [DefaultColor], Ĭ����ɫ, ��Ӱ��ɫ
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_BORDER    )   // [DefaultColor], Ĭ����ɫ, �߿���ɫ
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_TEXT      )   // [DefaultColor], Ĭ����ɫ, �ı���ɫ
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_TEXTHOT   )   // [DefaultColor], Ĭ����ɫ, �ı��ȵ���ɫ
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_TEXTDOWN  )   // [DefaultColor], Ĭ����ɫ, �ı�������ɫ
    //_DefaultColor_GETCOLOR(L"crScrollThumb",L"normal",   crScrollThumb,      RGB(104, 104, 104), IGI_CONFIG_CRDEF_TEXTBAN   )   // [DefaultColor], Ĭ����ɫ, �ı���ֹ��ɫ




#define _ICustomCodeTab_(_name, _def, _IGI) \
    COLORREF m_CustomCodeTab_##_name = 0;  \
    virtual COLORREF _name (COLORREF* pValue = 0) {\
        return _iconfig_color(L"CustomCodeTab", m_CustomCodeTab_##_name, pValue, L###_name, 0, _def, _IGI);\
    }

    _ICustomCodeTab_(close_hot , RGB(65, 65, 68), IGI_CONFIG_CODETAB_CRCLOSEHOT   )   // [CustomCodeTab], �����л�ѡ��йرհ�ť�ȵ���ɫ
    _ICustomCodeTab_(close_down, RGB(45, 45, 48), IGI_CONFIG_CODETAB_CRCLOSEDOWN  )   // [CustomCodeTab], �����л�ѡ��йرհ�ť������ɫ


    _ICustomCodeTab_(crTab_text_window          , RGB(232, 201, 187) , IGI_CONFIG_CODETAB_CRTEXT_WINDOW           )   // [CustomCodeTab], �����л�ѡ���, �����ı���ɫ
    _ICustomCodeTab_(crTab_text_assembly        , RGB(220, 220, 170) , IGI_CONFIG_CODETAB_CRTEXT_ASSEMBLY         )   // [CustomCodeTab], �����л�ѡ���, �����ı���ɫ
    _ICustomCodeTab_(crTab_text_assembly_window , RGB(232, 201, 187) , IGI_CONFIG_CODETAB_CRTEXT_ASSEMBLY_WINDOW  )   // [CustomCodeTab], �����л�ѡ���, ���ڳ����ı���ɫ
    _ICustomCodeTab_(crTab_text_class           , RGB(78, 201, 176)  , IGI_CONFIG_CODETAB_CRTEXT_CLASS            )   // [CustomCodeTab], �����л�ѡ���, ���ı���ɫ
    _ICustomCodeTab_(crTab_text_globalvar       , RGB(128, 128, 0)   , IGI_CONFIG_CODETAB_CRTEXT_GLOBALVAR        )   // [CustomCodeTab], �����л�ѡ���, ȫ�ֱ����ı���ɫ
    _ICustomCodeTab_(crTab_text_struct          , RGB(78, 201, 176)  , IGI_CONFIG_CODETAB_CRTEXT_STRUCT           )   // [CustomCodeTab], �����л�ѡ���, �Զ������������ı���ɫ
    _ICustomCodeTab_(crTab_text_dll             , RGB(128, 128, 0)   , IGI_CONFIG_CODETAB_CRTEXT_DLL              )   // [CustomCodeTab], �����л�ѡ���, DLL�ı���ɫ
    _ICustomCodeTab_(crTab_text_const           , RGB(222, 97, 222)  , IGI_CONFIG_CODETAB_CRTEXT_CONST            )   // [CustomCodeTab], �����л�ѡ���, �����ı���ɫ
    _ICustomCodeTab_(crTab_text_image           , RGB(128, 128, 0)   , IGI_CONFIG_CODETAB_CRTEXT_IMAGE            )   // [CustomCodeTab], �����л�ѡ���, ͼƬ�ı���ɫ
    _ICustomCodeTab_(crTab_text_sound           , RGB(128, 128, 0)   , IGI_CONFIG_CODETAB_CRTEXT_SOUND            )   // [CustomCodeTab], �����л�ѡ���, �����ı���ɫ
    
    _ICustomCodeTab_(crTab_line_window          , RGB(0, 0, 0)       , IGI_CONFIG_CODETAB_CRLINE_WINDOW           )   // [CustomCodeTab], �����л�ѡ���, �����±�����ɫ
    _ICustomCodeTab_(crTab_line_assembly        , RGB(0, 0, 0)       , IGI_CONFIG_CODETAB_CRLINE_ASSEMBLY         )   // [CustomCodeTab], �����л�ѡ���, �����±�����ɫ
    _ICustomCodeTab_(crTab_line_assembly_window , RGB(0, 0, 0)       , IGI_CONFIG_CODETAB_CRLINE_ASSEMBLY_WINDOW  )   // [CustomCodeTab], �����л�ѡ���, ���ڳ����±�����ɫ
    _ICustomCodeTab_(crTab_line_class           , RGB(0, 0, 0)       , IGI_CONFIG_CODETAB_CRLINE_CLASS            )   // [CustomCodeTab], �����л�ѡ���, ���±�����ɫ
    _ICustomCodeTab_(crTab_line_globalvar       , RGB(0, 0, 0)       , IGI_CONFIG_CODETAB_CRLINE_GLOBALVAR        )   // [CustomCodeTab], �����л�ѡ���, ȫ�ֱ����±�����ɫ
    _ICustomCodeTab_(crTab_line_struct          , RGB(0, 0, 0)       , IGI_CONFIG_CODETAB_CRLINE_STRUCT           )   // [CustomCodeTab], �����л�ѡ���, �Զ������������±�����ɫ
    _ICustomCodeTab_(crTab_line_dll             , RGB(0, 0, 0)       , IGI_CONFIG_CODETAB_CRLINE_DLL              )   // [CustomCodeTab], �����л�ѡ���, DLL�±�����ɫ
    _ICustomCodeTab_(crTab_line_const           , RGB(0, 0, 0)       , IGI_CONFIG_CODETAB_CRLINE_CONST            )   // [CustomCodeTab], �����л�ѡ���, �����±�����ɫ
    _ICustomCodeTab_(crTab_line_image           , RGB(0, 0, 0)       , IGI_CONFIG_CODETAB_CRLINE_IMAGE            )   // [CustomCodeTab], �����л�ѡ���, ͼƬ�±�����ɫ
    _ICustomCodeTab_(crTab_line_sound           , RGB(0, 0, 0)       , IGI_CONFIG_CODETAB_CRLINE_SOUND            )   // [CustomCodeTab], �����л�ѡ���, �����±�����ɫ

private:
    // ���᷵��ԭ����ֵ, ֻ�᷵�����ú��ֵ
    LPCWSTR _ires_name(LPCWSTR appName, wstr& value, LPCWSTR pValue, LPCWSTR keyName, LPCWSTR defValue)
    {
        if (pValue) // �����ֵ������Ҫд��
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

    m_config.IsRunSave                      ();     // ����/������Ƿ񱣴�Դ��, ȡֵ��Ҫ����һ��ָ����ս��
    m_config.IsDxRender                     ();     // �Ƿ�DX��Ⱦ, ȡֵ��Ҫ����һ��ָ����ս��
    m_config.IsDPIAware                     ();     // �Ƿ���DPI��֪
    m_config.IsDPIScaleWnd                  ();     // DPI�ı�ʱ�Ƿ����Ŵ���
    m_config.IsShowCodeTabClose             ();     // ��ʾ����ѡ��еĹرհ�ť
    m_config.IsShowLocationTool             ();     // �Ƿ���ʾ��λ������
    m_config.programTreeDblclk              ();     // �������Ϳ�˫��ģʽ, 0=ֻ�л�, 1=ֻչ��, 2=չ��+�л�
    m_config.wheelPos                       ();     // ������һ�εľ���
    m_config.noClipChildren                 ();     // ���ų��Ӵ�������
    m_config.MenuItemHeight                 ();     // �˵���߶�
    m_config.MenuSeparatorHeight            ();     // �ָ��˵���߶�
    m_config.noChangeTab_work               ();     // ���Զ��л�������
    m_config.noChangeTab_state              ();     // ���Զ��л�״̬��
    int scrollWidth = m_config.scroll_width ();     // �������Ŀ��, Ĭ����16, ������������Ǹ߶�

    if ( scrollWidth <= 0 )
    {
        scrollWidth = 12;
        m_config.scroll_width(&scrollWidth);    // С�ڵ���0, �Ǿ͸���Ĭ��ֵ
    }
    scrollWidth = m_config.scroll_btn_size();   // ��������ť�ߴ�

    if ( scrollWidth <= 0 )
    {
        scrollWidth = 10;
        m_config.scroll_btn_size(&scrollWidth);    // С�ڵ���0, �Ǿ͸���Ĭ��ֵ
    }

    m_config.tree_fne_ishot                 ();     // ֧�ֿ����Ϳ��Ƿ�֧���ȵ�
    m_config.tree_program_ishot             ();     // �������Ϳ��Ƿ�֧���ȵ�
    m_config.tree_control_ishot             ();     // ��������Ϳ��Ƿ�֧���ȵ�
    m_config.tree_control_orgIcon           ();     // ��������Ϳ���ʾԭͼ��
    m_config.tree_var_ishot                 ();     // ���������Ϳ��Ƿ�֧���ȵ�
    m_config.tree_call_ishot                ();     // ���ñ����Ϳ��Ƿ�֧���ȵ�
    m_config.list_find1_ishot               ();     // ��Ѱ1�б���Ƿ�֧���ȵ�
    m_config.list_find2_ishot               ();     // ��Ѱ1�б�����Ϳ��Ƿ�֧���ȵ�

    m_config.tree_fne_height                ();     // ֧�ֿ����Ϳ���Ŀ�߶�
    m_config.tree_program_height            ();     // �������Ϳ���Ŀ�߶�
    m_config.tree_control_height            ();     // ��������Ϳ���Ŀ�߶�
    m_config.tree_var_height                ();     // ���������Ϳ���Ŀ�߶�
    m_config.tree_call_height               ();     // ���ñ����Ϳ���Ŀ�߶�
    m_config.list_find1_height              ();     // ��Ѱ1�б����Ŀ�߶�
    m_config.list_find2_height              ();     // ��Ѱ2�б����Ŀ�߶�


    m_config.tab_work_direction             ();     // �����з���, 0=���, 1=����, 2=�ұ�, 3=�ױ�
    m_config.tab_work_isIcon                ();     // �������Ƿ���ʾͼ��
    m_config.tab_work_flags                 ();     // �����б�ʶ, ��¼λ��, �Ƿ����, �Ƿ񸡶�, �Ƿ�ͣ������Ϣ TOP_STATE_BUTTON ö�ٳ������ֵ, ���� __query_flags �ж�ֵ

    m_config.tab_state_direction            ();     // ״̬�з���, 0=���, 1=����, 2=�ұ�, 3=�ױ�
    m_config.tab_state_isIcon               ();     // ״̬���Ƿ���ʾͼ��
    m_config.tab_state_flags                ();     // ״̬�б�ʶ, ��¼λ��, �Ƿ����, �Ƿ񸡶�, �Ƿ�ͣ������Ϣ TOP_STATE_BUTTON ö�ٳ������ֵ, ���� __query_flags �ж�ֵ

    m_config.tab_control_flags              ();     // ������ʶ, ��¼λ��, �Ƿ����, �Ƿ񸡶�, �Ƿ�ͣ������Ϣ TOP_STATE_BUTTON ö�ٳ������ֵ, ���� __query_flags �ж�ֵ

    m_config.tab_code_direction             ();     // �����л�ѡ��з���, 3=�ױ�, ��3=����
    m_config.tab_code_isIcon                ();     // �����л�ѡ����Ƿ���ʾͼ��

    m_config.rcWork                         ();     // �����д���λ��, �����������������μ���λ��
    m_config.rcState                        ();     // ״̬�д���λ��, �����������������μ���λ��
    m_config.rcControl                      ();     // ����䴰��λ��, �����������������μ���λ��
    m_config.rcMenu                         ();     // �˵��д���λ��, �����������������μ���λ��
    m_config.rcMDIClient                    ();     // MDIClient����λ��, �����������������μ���λ��

    m_config.rcWork1                        ();     // �����д���λ��, ��ʾ����Ļ��λ��
    m_config.rcState1                       ();     // ״̬�д���λ��, ��ʾ����Ļ��λ��
    m_config.rcControl1                     ();     // ����䴰��λ��, ��ʾ����Ļ��λ��

    m_theme_color.crBack                    ();     // ������ɫ
    m_theme_color.crBackExtern              ();     // �ⲿ�ı�����ɫ, ����ѡ��е��Ӽ�
    m_theme_color.crStateBar                ();     // �����/������/״̬�ж����������ɫ
    m_theme_color.crBorder                  ();     // �߿���ɫ
    m_theme_color.crBorder_run              ();     // ����ʱ�߿���ɫ
    m_theme_color.crBorder_nf               ();     // ȡ������ʱ�ı߿���ɫ

    m_theme_color.crShadow                  ();     // ��Ӱ��ɫ
    m_theme_color.crShadow_run              ();     // ����ʱ��Ӱ��ɫ
    m_theme_color.crShadow_nf               ();     // ȡ������ʱ�ı߿���ɫ

    m_theme_color.crMenuBorder              ();     // �˵��߿���ɫ
    m_theme_color.crMenuBack                ();     // �˵�������ɫ, �����˵�ʹ�õı���ɫ, �˵�������ʱʹ�õı���ɫ
    m_theme_color.crMenuHot                 ();     // �˵��ȵ�����ɫ
    m_theme_color.crMenuBan                 ();     // �˵���ֹ��ɫ
    m_theme_color.crMenuText                ();     // �˵��ı���ɫ
    m_theme_color.crMenuTextHot             ();     // �˵��ı��ȵ���ɫ
    m_theme_color.crMenuTextBan             ();     // �˵��ı���ֹ��ɫ

    m_theme_color.crMenuBarHot              ();     // �����˵����ȵ�����ɫ
    m_theme_color.crMenuBarToolsText        ();     // �����˵������ֵ��ı���ɫ
    m_theme_color.crMenuBarToolsTextHot     ();     // �����˵������ֵ��ı��ȵ���ɫ

    m_theme_color.crTitleback               ();     // �м�ڿ����ı�����ɫ
    m_theme_color.crTitle_left_act          ();     // �м�ڿ������ߵ���ɫ, �����Դ����������Ŀ��, ����ʱ���ı���ɫ
    m_theme_color.crTitle_right_act         ();     // �м�ڿ�����ұߵ���ɫ, ����ǳ�������, ����δ����ʱ���Ǻ�ʹ�õ���ɫ, ����ʱ���ı���ɫ
    m_theme_color.crTitle_left              ();     // �м�ڿ������ߵ���ɫ, �����Դ����������Ŀ��
    m_theme_color.crTitle_right             ();     // �м�ڿ�����ұߵ���ɫ, ����ǳ�������, ����δ����ʱ���Ǻ�ʹ�õ���ɫ

    m_theme_color.crText                    ();     // ��ͨ�ı��滭�õ���ɫ
    m_theme_color.crTextStatusBar           ();     // �ײ�״̬�����ı���ɫ
    m_theme_color.crTextStatusBar_run       ();     // �ײ�״̬������ʱ���ı���ɫ
    m_theme_color.crTextStatusBar_nf        ();     // �ײ�״̬����������ı���ɫ


    m_theme_color.crScrollBtn1              ();     // ��������ť1������ɫ
    m_theme_color.crScrollBtnHot1           ();     // ��������ť1�ȵ���ɫ
    m_theme_color.crScrollBtnDown1          ();     // ��������ť1������ɫ
    m_theme_color.crScrollBtn2              ();     // ��������ť2������ɫ
    m_theme_color.crScrollBtnHot2           ();     // ��������ť2�ȵ���ɫ
    m_theme_color.crScrollBtnDown2          ();     // ��������ť2������ɫ
    m_theme_color.crScrollThumb             ();     // ������������ɫ
    m_theme_color.crScrollThumbHot          ();     // �����������ȵ���ɫ
    m_theme_color.crScrollThumbDown         ();     // ���������鰴����ɫ
    m_theme_color.crScrollBack              ();     // ������������ɫ

    m_theme_color.crToolSeplineLeft         ();     // �������ָ��������ɫ
    m_theme_color.crToolSeplineRight        ();     // �������ָ����ұ���ɫ


    m_theme_color.crStatelineNormal         ();     // [Basics], �������ָ����ұ���ɫ
    m_theme_color.crStatelineHot            ();     // [Basics], �������ָ����ұ���ɫ

    m_theme_color.ires_Toolbar              ();     // ��������Դ��
    m_theme_color.ires_TreeFne              ();     // ֧�ֿ����Ϳ���Դ��
    m_theme_color.ires_TreeProgram          ();     // �������Ϳ���Դ��
    m_theme_color.ires_TreeControl          ();     // ��������Ϳ���Դ��
    m_theme_color.ires_TreeExpand           ();     // ���Ϳ�չ��������ť��Դ��
    m_theme_color.ires_ComboboxLocation     ();     // ��Ͽ���Դ��
    m_theme_color.ires_MenuItem             ();     // �˵���Դ��
    m_theme_color.ires_msctls_updown32_lr   ();     // ���������Ҽ�ͷ��Դ��
    m_theme_color.ires_msctls_updown32_tb   ();     // ���������¼�ͷ��Դ��
    m_theme_color.ires_TopState             ();     // ����״̬��Դ��
    m_theme_color.ires_icon                 ();     // Ĭ����Դ��Դ��
    m_theme_color.clr                       ();     // Ĭ����Դ��Դ��


    
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
        m_isFirst = true;   // ���¶�ȡ
    IThemeColor& m_theme_color = *s_info->themeColor;


    m_theme_color.close_hot                     ();   // [CustomCodeTab], �����л�ѡ��йرհ�ť�ȵ���ɫ
    m_theme_color.close_down                    ();   // [CustomCodeTab], �����л�ѡ��йرհ�ť������ɫ

    m_theme_color.crTab_text_window             ();   // [CustomCodeTab], �����л�ѡ���, �����ı���ɫ
    m_theme_color.crTab_text_assembly           ();   // [CustomCodeTab], �����л�ѡ���, �����ı���ɫ
    m_theme_color.crTab_text_assembly_window    ();   // [CustomCodeTab], �����л�ѡ���, ���ڳ����ı���ɫ
    m_theme_color.crTab_text_class              ();   // [CustomCodeTab], �����л�ѡ���, ���ı���ɫ
    m_theme_color.crTab_text_globalvar          ();   // [CustomCodeTab], �����л�ѡ���, ȫ�ֱ����ı���ɫ
    m_theme_color.crTab_text_struct             ();   // [CustomCodeTab], �����л�ѡ���, �Զ������������ı���ɫ
    m_theme_color.crTab_text_dll                ();   // [CustomCodeTab], �����л�ѡ���, DLL�ı���ɫ
    m_theme_color.crTab_text_const              ();   // [CustomCodeTab], �����л�ѡ���, �����ı���ɫ
    m_theme_color.crTab_text_image              ();   // [CustomCodeTab], �����л�ѡ���, ͼƬ�ı���ɫ
    m_theme_color.crTab_text_sound              ();   // [CustomCodeTab], �����л�ѡ���, �����ı���ɫ

    m_theme_color.crTab_line_window             ();   // [CustomCodeTab], �����л�ѡ���, �����±�����ɫ
    m_theme_color.crTab_line_assembly           ();   // [CustomCodeTab], �����л�ѡ���, �����±�����ɫ
    m_theme_color.crTab_line_assembly_window    ();   // [CustomCodeTab], �����л�ѡ���, ���ڳ����±�����ɫ
    m_theme_color.crTab_line_class              ();   // [CustomCodeTab], �����л�ѡ���, ���±�����ɫ
    m_theme_color.crTab_line_globalvar          ();   // [CustomCodeTab], �����л�ѡ���, ȫ�ֱ����±�����ɫ
    m_theme_color.crTab_line_struct             ();   // [CustomCodeTab], �����л�ѡ���, �Զ������������±�����ɫ
    m_theme_color.crTab_line_dll                ();   // [CustomCodeTab], �����л�ѡ���, DLL�±�����ɫ
    m_theme_color.crTab_line_const              ();   // [CustomCodeTab], �����л�ѡ���, �����±�����ɫ
    m_theme_color.crTab_line_image              ();   // [CustomCodeTab], �����л�ѡ���, ͼƬ�±�����ɫ
    m_theme_color.crTab_line_sound              ();   // [CustomCodeTab], �����л�ѡ���, �����±�����ɫ


    m_isReadFile = isReadFile;
    if (isFirst)
        m_isFirst = false;
}