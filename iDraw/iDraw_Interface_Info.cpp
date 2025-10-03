#if 1
#include "EWindow_Fne_Main.h"

// ���û��߻�ȡ���õ���ɫ
inline bool _info_case_color(int index, LONG_PTR value, LONG_PTR& ret, bool isSet, int argument)
{
    IThemeColor& theme = *s_info->themeColor;
    COLORINFO& color = s_info->defData->draw.color;

    // ȡֵ, ���ض���
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

    // �����ͨ�����ñ��ı��¼��ǲ����ߵ��������, ��������Ҳ���޸�Ĭ����ɫ��ֵ
    // �������������������øı�ĺ���, ������Ҫ���ݸı����������Ӧ�Ĵ���
    // �����Ĭ��ɫ�Ļ滭�������ɫֵ���ı���, ���ò��������øı��ֻ�������д��ڵ�Ĭ��ֵ
    // ����������ú����� iConfig_InfoChanged(), Ĭ��ɫ����� _config_defcolor_change()

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
        // Ĭ����ɫ 0x0200 - 0x021F
        _info_defcolor(IGI_CONFIG_CRDEF_BACK        , color.crBackGround  , color.hbrBackGround   );    // [DefaultColor], Ĭ����ɫ, ������ɫ
        _info_defcolor(IGI_CONFIG_CRDEF_NORMAL      , color.crNormal      , color.hbrNormal       );    // [DefaultColor], Ĭ����ɫ, ������ɫ
        _info_defcolor(IGI_CONFIG_CRDEF_HOT         , color.crHot         , color.hbrHot          );    // [DefaultColor], Ĭ����ɫ, �ȵ���ɫ
        _info_defcolor(IGI_CONFIG_CRDEF_DOWN        , color.crDown        , color.hbrDown         );    // [DefaultColor], Ĭ����ɫ, ������ɫ
        _info_defcolor(IGI_CONFIG_CRDEF_BAN         , color.crBan         , color.hbrBan          );    // [DefaultColor], Ĭ����ɫ, ��ֹ��ɫ
        _info_defcolor(IGI_CONFIG_CRDEF_HOVER       , color.crHover       , color.hbrHover        );    // [DefaultColor], Ĭ����ɫ, ������ɫ
        _info_defcolor(IGI_CONFIG_CRDEF_FOCUS       , color.crFocus       , color.hbrFocus        );    // [DefaultColor], Ĭ����ɫ, ������ɫ
        _info_defcolor(IGI_CONFIG_CRDEF_CHECKED     , color.crChecked     , color.hbrChecked      );    // [DefaultColor], Ĭ����ɫ, ѡ����ɫ
        _info_defcolor(IGI_CONFIG_CRDEF_SELECTED    , color.crSelected    , color.hbrSelected     );    // [DefaultColor], Ĭ����ɫ, ѡ����ɫ
        _info_defcolor(IGI_CONFIG_CRDEF_VISTED      , color.crVisted      , color.hbrVisted       );    // [DefaultColor], Ĭ����ɫ, 
        _info_defcolor(IGI_CONFIG_CRDEF_SHADOW      , color.crShadow      , color.hbrShadow       );    // [DefaultColor], Ĭ����ɫ, ��Ӱ��ɫ
        _info_pencolor(IGI_CONFIG_CRDEF_BORDER      , color.crBorder      , color.hpenBorder      );    // [DefaultColor], Ĭ����ɫ, �߿���ɫ
        _info_defcolor(IGI_CONFIG_CRDEF_TEXT        , color.crText        , color.hbrText         );    // [DefaultColor], Ĭ����ɫ, �ı���ɫ
        _info_defcolor(IGI_CONFIG_CRDEF_TEXTHOT     , color.crTextHot     , color.hbrTextHot      );    // [DefaultColor], Ĭ����ɫ, �ı��ȵ���ɫ
        _info_defcolor(IGI_CONFIG_CRDEF_TEXTDOWN    , color.crTextDown    , color.hbrTextDown     );    // [DefaultColor], Ĭ����ɫ, �ı�������ɫ
        _info_defcolor(IGI_CONFIG_CRDEF_TEXTBAN     , color.crTextBan     , color.hbrTextBan      );    // [DefaultColor], Ĭ����ɫ, �ı���ֹ��ɫ

        // ������ɫ 0x0220 - 0x022F
        _info_color(IGI_CONFIG_CRBACK               , theme.crBack                   );  // [Basics], ������ɫ
        _info_color(IGI_CONFIG_CRBACKEXTERN         , theme.crBackExtern             );  // [Basics], �ⲿ�ı�����ɫ, ����ѡ��е��Ӽ�
        _info_color(IGI_CONFIG_CRSTATEBAR           , theme.crStateBar               );  // [Basics], �����/������/״̬�ж����������ɫ
        _info_color(IGI_CONFIG_CRTEXT               , theme.crText                   );  // [Basics], һ���ı���ɫ

        // �߿�/��Ӱ��ɫ 0x0230 - 0x023F
        _info_color(IGI_CONFIG_CRBORDER             , theme.crBorder                 );  // [Basics], ���ڱ߿���ɫ����
        _info_color(IGI_CONFIG_CRSHADOW             , theme.crShadow                 );  // [Basics], ������Ӱ��ɫ����
        _info_color(IGI_CONFIG_CRBORDER_RUN         , theme.crBorder_run             );  // [Basics], ����ʱ���ڱ߿����ɫ
        _info_color(IGI_CONFIG_CRSHADOW_RUN         , theme.crShadow_run             );  // [Basics], ����ʱ������Ӱ��ɫ
        _info_color(IGI_CONFIG_CRBORDER_NF          , theme.crBorder_nf              );  // [Basics], ����ʱ������Ӱ��ɫ
        _info_color(IGI_CONFIG_CRSHADOW_NF          , theme.crShadow_nf              );  // [Basics], ʧȥ����ʱ������Ӱ��ɫ

        // �˵��� 0x0240 - 0x024F
        _info_color(IGI_CONFIG_CRMENUBORDER         , theme.crMenuBorder             );  // [Basics], �˵���ı߿���ɫ
        _info_color(IGI_CONFIG_CRMENUBACK           , theme.crMenuBack               );  // [Basics], �˵���ı�����ɫ, û���ȵ�, ���ǽ�ֹʱʹ�������ɫ�滭
        _info_color(IGI_CONFIG_CRMENUHOT            , theme.crMenuHot                );  // [Basics], �˵�����ȵ���ɫ
        _info_color(IGI_CONFIG_CRMENUBAN            , theme.crMenuBan                );  // [Basics], �˵���Ľ�ֹ��ɫ
        _info_color(IGI_CONFIG_CRMENUTEXT           , theme.crMenuText               );  // [Basics], �˵��������ı���ɫ
        _info_color(IGI_CONFIG_CRMENUTEXTHOT        , theme.crMenuTextHot            );  // [Basics], �˵����ȵ��ı���ɫ
        _info_color(IGI_CONFIG_CRMENUTEXTBAN        , theme.crMenuTextBan            );  // [Basics], �˵����ֹ�ı���ɫ

        // ��ʾ�ڱ���������Ĳ˵��� 0x0250 - 0x025F
        _info_color(IGI_CONFIG_CRMENUBARHOT         , theme.crMenuBarHot             );  // [Basics], �����˵����ȵ���ɫ
        _info_color(IGI_CONFIG_CRMENUBARTEXT        , theme.crMenuBarToolsText       );  // [Basics], �����˵������ֲ˵����ı���ɫ
        _info_color(IGI_CONFIG_CRMENUBARTEXTHOT     , theme.crMenuBarToolsTextHot    );  // [Basics], �����˵������ֲ˵����ı��ȵ���ɫ

        // ��ʾ�ڱ��������ɫ�����ɫ 0x0260 - 0x026F
        _info_color(IGI_CONFIG_CRTITLEBACK          , theme.crTitleback              );  // [Basics], ���ߺڿ鲿�ֱ�����ɫ
        _info_color(IGI_CONFIG_CRTITLELEFT_ACT      , theme.crTitle_left_act         );  // [Basics], �����ڿ鲿����߼���ʱ�ı���ɫ, �����Դ����������Ŀ��
        _info_color(IGI_CONFIG_CRTITLERIGHT_ACT     , theme.crTitle_right_act        );  // [Basics], �����ڿ鲿���ұ߼���ʱ�ı���ɫ, ����ǳ�������, ����δ����ʱ���Ǻ�ʹ�õ���ɫ
        _info_color(IGI_CONFIG_CRTITLELEFT          , theme.crTitle_left             );  // [Basics], �����ڿ鲿������ı���ɫ, �����Դ����������Ŀ��
        _info_color(IGI_CONFIG_CRTITLERIGHT         , theme.crTitle_right            );  // [Basics], �����ڿ鲿���ұ��ı���ɫ, ����ǳ�������, ����δ����ʱ���Ǻ�ʹ�õ���ɫ

        // �ײ�״̬������ɫ 0x0270 - 0x027F
        _info_color(IGI_CONFIG_CRTEXTSTATUSBAR      , theme.crTextStatusBar          );  // [Basics], �ײ�״̬�����ı���ɫ
        _info_color(IGI_CONFIG_CRTEXTSTATUSBAR_RUN  , theme.crTextStatusBar_run      );  // [Basics], �ײ�״̬������ʱ���ı���ɫ
        _info_color(IGI_CONFIG_CRTEXTSTATUSBAR_NF   , theme.crTextStatusBar_nf       );  // [Basics], �ײ�״̬����������ı���ɫ

        // ��������ɫ 0x0280 - 0x028F
        _info_color(IGI_CONFIG_CRSCROLLBTN1NORRMAL  , theme.crScrollBtn1             );  // [Basics], ��������ť1������ɫ, ��߻��߶��ߵ�С��ť
        _info_color(IGI_CONFIG_CRSCROLLBTN1HOT      , theme.crScrollBtnHot1          );  // [Basics], ��������ť1�ȵ���ɫ, ��߻��߶��ߵ�С��ť
        _info_color(IGI_CONFIG_CRSCROLLBTN1DOWN     , theme.crScrollBtnDown1         );  // [Basics], ��������ť1������ɫ, ��߻��߶��ߵ�С��ť
        _info_color(IGI_CONFIG_CRSCROLLBTN2NORRMAL  , theme.crScrollBtn2             );  // [Basics], ��������ť2������ɫ, �ұ߻��ߵױߵ�С��ť
        _info_color(IGI_CONFIG_CRSCROLLBTN2HOT      , theme.crScrollBtnHot2          );  // [Basics], ��������ť2�ȵ���ɫ, �ұ߻��ߵױߵ�С��ť
        _info_color(IGI_CONFIG_CRSCROLLBTN2DOWN     , theme.crScrollBtnDown2         );  // [Basics], ��������ť2������ɫ, �ұ߻��ߵױߵ�С��ť
        _info_color(IGI_CONFIG_CRSCROLLTHUMBNORRMAL , theme.crScrollThumb            );  // [Basics], ������������ɫ
        _info_color(IGI_CONFIG_CRSCROLLTHUMBHOT     , theme.crScrollThumbHot         );  // [Basics], �����������ȵ���ɫ
        _info_color(IGI_CONFIG_CRSCROLLTHUMBDOWN    , theme.crScrollThumbDown        );  // [Basics], ���������鰴����ɫ
        _info_color(IGI_CONFIG_CRSCROLLBACKGROUND   , theme.crScrollBack             );  // [Basics], ������������ɫ

        // �ָ�����ɫ 0x0290 - 0x029F
        _info_color(IGI_CONFIG_CRLINELEFT           , theme.crToolSeplineLeft        );  // [Basics], ������, ���б�ͷ, ������ķָ��������ɫ, �ָ��߶�����������
        _info_color(IGI_CONFIG_CRLINERIGHT          , theme.crToolSeplineRight       );  // [Basics], ������, ���б�ͷ, ������ķָ����ұ���ɫ, �ָ��߶�����������

        // �Ի�����л�ѡ����ı���ɫ, 0x02B0 - 0x02BF
        _info_color(IGI_CONFIG_CODETAB_CRCLOSEHOT       , theme.close_hot           );  // [Basics], ������, ���б�ͷ, ������ķָ����ұ���ɫ, �ָ��߶�����������
        _info_color(IGI_CONFIG_CODETAB_CRCLOSEDOWN      , theme.close_down          );  // [Basics], ������, ���б�ͷ, ������ķָ����ұ���ɫ, �ָ��߶�����������

        // �Ի�����л�ѡ����ı���ɫ, 0x02B0 - 0x02BF
        _info_color(IGI_CONFIG_CODETAB_CRTEXT_WINDOW            , theme.crTab_text_window             );    // [CustomCodeTab], �����л�ѡ���, �����ı���ɫ
        _info_color(IGI_CONFIG_CODETAB_CRTEXT_ASSEMBLY          , theme.crTab_text_assembly           );    // [CustomCodeTab], �����л�ѡ���, �����ı���ɫ
        _info_color(IGI_CONFIG_CODETAB_CRTEXT_ASSEMBLY_WINDOW   , theme.crTab_text_assembly_window    );    // [CustomCodeTab], �����л�ѡ���, ���ڳ����ı���ɫ
        _info_color(IGI_CONFIG_CODETAB_CRTEXT_CLASS             , theme.crTab_text_class              );    // [CustomCodeTab], �����л�ѡ���, ���ı���ɫ
        _info_color(IGI_CONFIG_CODETAB_CRTEXT_GLOBALVAR         , theme.crTab_text_globalvar          );    // [CustomCodeTab], �����л�ѡ���, ȫ�ֱ����ı���ɫ
        _info_color(IGI_CONFIG_CODETAB_CRTEXT_STRUCT            , theme.crTab_text_struct             );    // [CustomCodeTab], �����л�ѡ���, �Զ������������ı���ɫ
        _info_color(IGI_CONFIG_CODETAB_CRTEXT_DLL               , theme.crTab_text_dll                );    // [CustomCodeTab], �����л�ѡ���, DLL�ı���ɫ
        _info_color(IGI_CONFIG_CODETAB_CRTEXT_CONST             , theme.crTab_text_const              );    // [CustomCodeTab], �����л�ѡ���, �����ı���ɫ
        _info_color(IGI_CONFIG_CODETAB_CRTEXT_IMAGE             , theme.crTab_text_image              );    // [CustomCodeTab], �����л�ѡ���, ͼƬ�ı���ɫ
        _info_color(IGI_CONFIG_CODETAB_CRTEXT_SOUND             , theme.crTab_text_sound              );    // [CustomCodeTab], �����л�ѡ���, �����ı���ɫ

        // �Ի�����л�ѡ��е���������ɫ, 0x02C0 - 0x02CF
        _info_color(IGI_CONFIG_CODETAB_CRLINE_WINDOW            , theme.crTab_line_window             );    // [CustomCodeTab], �����л�ѡ���, �����±�����ɫ
        _info_color(IGI_CONFIG_CODETAB_CRLINE_ASSEMBLY          , theme.crTab_line_assembly           );    // [CustomCodeTab], �����л�ѡ���, �����±�����ɫ
        _info_color(IGI_CONFIG_CODETAB_CRLINE_ASSEMBLY_WINDOW   , theme.crTab_line_assembly_window    );    // [CustomCodeTab], �����л�ѡ���, ���ڳ����±�����ɫ
        _info_color(IGI_CONFIG_CODETAB_CRLINE_CLASS             , theme.crTab_line_class              );    // [CustomCodeTab], �����л�ѡ���, ���±�����ɫ
        _info_color(IGI_CONFIG_CODETAB_CRLINE_GLOBALVAR         , theme.crTab_line_globalvar          );    // [CustomCodeTab], �����л�ѡ���, ȫ�ֱ����±�����ɫ
        _info_color(IGI_CONFIG_CODETAB_CRLINE_STRUCT            , theme.crTab_line_struct             );    // [CustomCodeTab], �����л�ѡ���, �Զ������������±�����ɫ
        _info_color(IGI_CONFIG_CODETAB_CRLINE_DLL               , theme.crTab_line_dll                );    // [CustomCodeTab], �����л�ѡ���, DLL�±�����ɫ
        _info_color(IGI_CONFIG_CODETAB_CRLINE_CONST             , theme.crTab_line_const              );    // [CustomCodeTab], �����л�ѡ���, �����±�����ɫ
        _info_color(IGI_CONFIG_CODETAB_CRLINE_IMAGE             , theme.crTab_line_image              );    // [CustomCodeTab], �����л�ѡ���, ͼƬ�±�����ɫ
        _info_color(IGI_CONFIG_CODETAB_CRLINE_SOUND             , theme.crTab_line_sound              );    // [CustomCodeTab], �����л�ѡ���, �����±�����ɫ

        // ��ɫ��ɫ��� 0x03FF


    default:
        return false;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡpInfo����Ϣ, �ӿڳ���ĵ�һ������
// 
// ����:
//      ����1: �ӿڳ���ĵ�һ������
//      ����2: ��ȡ�ı��, IGI_ ��ͷ����
//      ����3: ��ȡ�Ĳ���, ���ݱ�Ŵ���
// 
// ����ֵ:
//      ��ȡ����ֵ
EXTERN_C _declspec(dllexport) LONG_PTR WINAPI iDraw_GetInfo(IDRAW_INFO* pInfo, int index, LPARAM argument)
{
    if (!s_info || pInfo != s_info) return 0;
    //if (!s_info || (pInfo && pInfo != s_info)) return 0;
    IConfig& ini = *s_info->config;

    switch (index)
    {
    case IGI_EWINDOW:                   // ������������
        return (LONG_PTR)s_info->hEWnd;
    case IGI_CUSTOMCODETAB:             // ����ѡ��еĴ��ھ��
        return (LONG_PTR)s_info->hEWnd;
    case IGI_CODE_HWND:                 // ֻ��, ����������ȡ MDIClient �µĵ�һ�㴰�ھ��
    {
        LONG_PTR count = (LONG_PTR)s_info->hWnds->size();
        if (argument < 0 || argument >= count)
            return 0;
        CUSTOMTAB_DATA& data = s_info->hWnds->at(argument);
        return (LONG_PTR)data.hWnd;
    }
    case IGI_SHADOWWINDOW:              // ֻ��, ��Ӱ����, ��������ǻ���������ڻ滭��, ��������ǲ㴰��
        return (LONG_PTR)s_info->hShadow;
    case IGI_MDICLIENTTYPE:             // ֻ��, ��ǰMDIClient ����������ʾʲô����, -1=û�д���, 0=������, 1=���������, 2=��ʼҳ, ��λ�Ǵ�������, 0=δ֪, 1=���������, 2=��ͨ����, 3=���ڳ���, 4=��, 5=ȫ�ֱ���, 6=��������, 7=DLLĿ¼�����, 8=����, 9=ͼƬ��Դ, 10=������Դ
        argument = -1;  // �������Զ���ȡ��ǰѡ����
    case IGI_MDICLIENTCODETYPE:         // ֻ��, ��ǰMDIClient ����������ʾʲô��������, 0=δ֪, 1=���������, 2=��ͨ����, 3=���ڳ���, 4=��, 5=ȫ�ֱ���, 6=��������, 7=DLLĿ¼�����, 8=����, 9=ͼƬ��Դ, 10=������Դ
    case IGI_MDICLIENTTYPE_FROMINDEX:   // ��ָ��������ȡMDIClient ����������ʾʲô��������
    {
        if (argument == -1)
            argument = TabCtrl_GetCurSel(s_info->hEWnd);   // ��ǰѡ���ѡ����

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

    case IGI_RCCUSTOMCODETAB:           // ����ѡ��е�λ��, ���ڸ����ڵ�λ��
        return (LONG_PTR)&s_info->rcCustomTab;
    case IGI_RCSTATE:                   // ����״̬�е�λ��, ���ڸ����ڵ�λ��
        return (LONG_PTR)&s_info->rcState;
    case IGI_RCSTATUSBAR:               // ����״̬����λ��, ���ڸ����ڵ�λ��
        return (LONG_PTR)&s_info->rcStatusBar;
    case IGI_RCMDICLIENT:               // �м��������λ��, ���ڸ����ڵ�λ��
        return (LONG_PTR)&s_info->rcMDIClient;
    case IGI_RCCONTROL:                 // ������λ��, ���ڸ����ڵ�λ��
        return (LONG_PTR)&s_info->rcControl;
    case IGI_RCWORK:                    // �����е�λ��, ���ڸ����ڵ�λ��
        return (LONG_PTR)&s_info->rcWork;
    case IGI_RCMENUWND:                 // �˵���������λ��, ���ڸ����ڵ�λ��
        return (LONG_PTR)&s_info->rcWndMenu;

    case IGI_IGETEWINDOW:               // �����Ի�ȡ�������ڵ���ָ��
        return (LONG_PTR)s_info->eWnd;
    case IGI_IDENOTIFY:                 // �����Խ�������, ��������������൱�ڵ��������� ������ϵͳ����() ����
        return (LONG_PTR)s_info->pfnIDE;

    case IGI_LIBPATH:                   // ������֧�ֿ�Ŀ¼, �������б��
        return (LONG_PTR)s_info->path_run;
    case IGI_DATAPATH:                  // ��IDE���ݴ洢Ŀ¼, �������б��, һ���� ������֧�ֿ�Ŀ¼/iDraw/
        return (LONG_PTR)s_info->path_data;
    case IGI_IDRAW:                     // �Ի���ӿ�, Ŀǰ��ʱδ���⹫��
        return (LONG_PTR)s_info->iDraw;
    case IGI_D2D:                       // D2D��ӿ�, Ŀǰ��ʱδ���⹫��
        return (LONG_PTR)s_info->d2d;

    case IGI_PATH_LIB:                  // ֻ��, ������֧�ֿ�Ŀ¼, �������б��, ����unicode�ı�ָ��
        return (LONG_PTR)s_info->path_run;
    case IGI_PATH_IDRAW:                // ֻ��, ��IDE��Ҫʹ�õ�Ŀ¼, �������б��, һ���� ������֧�ֿ�Ŀ¼/iDraw/, ����unicode�ı�ָ��
        return (LONG_PTR)s_info->path_iDraw;
    case IGI_PATH_DATA:                 // ֻ��, ��IDE���ݴ洢Ŀ¼, �������б��, һ���� ������֧�ֿ�Ŀ¼/iDraw/, ����unicode�ı�ָ��
        return (LONG_PTR)s_info->path_data;
    case IGI_PATH_PLUGIN:               // ֻ��, ��IDE���·��
        return (LONG_PTR)s_info->path_plugin;
    case IGI_PATH_CONFIG:               // ֻ��, ��IDE���������ļ�����·��
        return (LONG_PTR)s_info->path_config;

    case IGI_STATE_TOOL_GET:                // ֻ��, ��ȡָ��������ID��״̬, ����3��Ҫ���ݰ�ťID
    {
        DWORD state = SendMessageW(s_info->eWnd->GetMenuStandard(), TB_GETSTATE, argument, 0);
        return (state & TBSTATE_ENABLED) == TBSTATE_ENABLED;    // �� TBSTATE_ENABLED �����־��������״̬
    }
    case IGI_STATE_TOOL_RUN:                // ֻ��, ��ǰ���������а�ť��״̬
        return (LONG_PTR)s_info->isRun;
    case IGI_STATE_TOOL_STOP:               // ֻ��, ��ǰ������ֹͣ��ť��״̬
        return (LONG_PTR)s_info->isStop;

    case IGI_CONFIG_DEFDXFONT:              // ֻ��, �˵�ʹ�õ�����
        return (LONG_PTR)s_info->pfnCreateFontDpi(s_info->defData->draw.hFont, s_info->dpiX);
    case IGI_CONFIG_DEFGDIFONT:             // ֻ��, ����Ĭ��ʹ�õ�GDI����
        return (LONG_PTR)s_info->pfnCreateFontDpi(s_info->defData->draw.hFont, s_info->dpiX)->GetFont();


#define _igi_get(_case, _fun) \
    case _case:\
        return (LONG_PTR)_fun()

        _igi_get(IGI_CONFIG_WORK_DIRECTION      , ini.tab_work_direction    );  // �����е��Ӽз���, 0=���, 1=����, 2=�ұ�, 3=�ױ�
        _igi_get(IGI_CONFIG_STATE_DIRECTION     , ini.tab_state_direction   );  // ״̬�е��Ӽз���, 0=���, 1=����, 2=�ұ�, 3=�ױ�
        _igi_get(IGI_CONFIG_CODE_DIRECTION      , ini.tab_code_direction    );  // ����ѡ��е��Ӽз���, 3=�ױ�, ���� = ����
        _igi_get(IGI_CONFIG_WORK_ISICON         , ini.tab_work_isIcon       );  // ���µ�״̬ѡ����Ƿ���ʾͼ��
        _igi_get(IGI_CONFIG_STATE_ISICON        , ini.tab_state_isIcon      );  // ��ߵĹ������Ƿ���ʾͼ��
        _igi_get(IGI_CONFIG_CODE_ISICON         , ini.tab_code_isIcon       );  // �����л�ѡ����Ƿ���ʾͼ��
        //_igi_get(IGI_CONFIG_WORK_POS            , ini.tab_work_pos          );  // ������λ��, 0=���, 1=����, 2=�ұ�, 3=�ױ�
        //_igi_get(IGI_CONFIG_STATE_POS           , ini.tab_state_pos         );  // ״̬��λ��, 0=���, 1=����, 2=�ұ�, 3=�ױ�
        //_igi_get(IGI_CONFIG_CONTROL_POS         , ini.tab_control_pos       );  // �����λ��, 0=���, 1=����, 2=�ұ�, 3=�ױ�
        _igi_get(IGI_CONFIG_WORK_FLAGS          , ini.tab_work_flags        );  // ������λ��, ��¼λ��, �Ƿ����, �Ƿ񸡶�, �Ƿ�ͣ������Ϣ TOP_STATE_BUTTON ö�ٳ������ֵ, ���� __query_flags �ж�ֵ
        _igi_get(IGI_CONFIG_STATE_FLAGS         , ini.tab_state_flags       );  // ״̬��λ��, ��¼λ��, �Ƿ����, �Ƿ񸡶�, �Ƿ�ͣ������Ϣ TOP_STATE_BUTTON ö�ٳ������ֵ, ���� __query_flags �ж�ֵ
        _igi_get(IGI_CONFIG_CONTROL_FLAGS       , ini.tab_control_flags     );  // �����λ��, ��¼λ��, �Ƿ����, �Ƿ񸡶�, �Ƿ�ͣ������Ϣ TOP_STATE_BUTTON ö�ٳ������ֵ, ���� __query_flags �ж�ֵ
        _igi_get(IGI_CONFIG_ISDXRENDER          , ini.IsDxRender            );  // �Ƿ�ʹ��DX��Ⱦ
        _igi_get(IGI_CONFIG_ISRUNSAVE           , ini.IsRunSave             );  // ����/������Ƿ񱣴�Դ��
        _igi_get(IGI_CONFIG_WHEELPOS            , ini.wheelPos              );  // ������һ�εľ���
        _igi_get(IGI_CONFIG_NOCLIPCHILDREN      , ini.noClipChildren        );  // ���ų��Ӵ�������
        _igi_get(IGI_CONFIG_MENUITEMHEIGHT      , ini.MenuItemHeight        );  // �˵���߶�
        _igi_get(IGI_CONFIG_MENUSEPARATORHEIGHT , ini.MenuSeparatorHeight   );  // �ָ����˵���߶�
        _igi_get(IGI_CONFIG_ISLOCATIONTOOLBAR   , ini.IsShowLocationTool    );  // �Ƿ���ʾ��λ������
        _igi_get(IGI_CONFIG_PROGRAMDBLCLKTYPE   , ini.programTreeDblclk     );  // �������Ϳ�˫��ģʽ, 0=ֻ�л�, 1=ֻչ��, 2=չ��+�л�
        _igi_get(IGI_CONFIG_NOCHANGETAB_WORK    , ini.noChangeTab_work      );  // ���Զ��л�������
        _igi_get(IGI_CONFIG_NOCHANGETAB_STATE   , ini.noChangeTab_state     );  // ���Զ��л�״̬��
        _igi_get(IGI_CONFIG_SCROLL_WIDTH        , ini.scroll_width          );  // ���������
        _igi_get(IGI_CONFIG_SCROLL_BTN_SIZE     , ini.scroll_btn_size       );  // ��������ť�ߴ�


        _igi_get(IGI_CONFIG_TREE_FNE_ISHOT      , ini.tree_fne_ishot        );  // ֧�ֿ����Ϳ��Ƿ�֧���ȵ�
        _igi_get(IGI_CONFIG_TREE_PROGRAM_ISHOT  , ini.tree_program_ishot    );  // �������Ϳ��Ƿ�֧���ȵ�
        _igi_get(IGI_CONFIG_TREE_CONTROL_ISHOT  , ini.tree_control_ishot    );  // ��������Ϳ��Ƿ�֧���ȵ�
        _igi_get(IGI_CONFIG_TREE_VAR_ISHOT      , ini.tree_var_ishot        );  // ���������Ϳ��Ƿ�֧���ȵ�
        _igi_get(IGI_CONFIG_TREE_CALL_ISHOT     , ini.tree_call_ishot       );  // ���ñ����Ϳ��Ƿ�֧���ȵ�
        _igi_get(IGI_CONFIG_LIST_FIND1_ISHOT    , ini.list_find1_ishot      );  // ��Ѱ1�б���Ƿ�֧���ȵ�
        _igi_get(IGI_CONFIG_LIST_FIND2_ISHOT    , ini.list_find2_ishot      );  // ��Ѱ2�б���Ƿ�֧���ȵ�

        _igi_get(IGI_CONFIG_TREE_FNE_HEIGHT     , ini.tree_fne_height       );  // ֧�ֿ����Ϳ���Ŀ�߶�
        _igi_get(IGI_CONFIG_TREE_PROGRAM_HEIGHT , ini.tree_program_height   );  // �������Ϳ���Ŀ�߶�
        _igi_get(IGI_CONFIG_TREE_CONTROL_HEIGHT , ini.tree_control_height   );  // ��������Ϳ���Ŀ�߶�
        _igi_get(IGI_CONFIG_TREE_VAR_HEIGHT     , ini.tree_var_height       );  // ���������Ϳ���Ŀ�߶�
        _igi_get(IGI_CONFIG_TREE_CALL_HEIGHT    , ini.tree_call_height      );  // ���ñ����Ϳ���Ŀ�߶�
        _igi_get(IGI_CONFIG_LIST_FIND1_HEIGHT   , ini.list_find1_height     );  // ��Ѱ1�б����Ŀ�߶�
        _igi_get(IGI_CONFIG_LIST_FIND2_HEIGHT   , ini.list_find2_height     );  // ��Ѱ2�б����Ŀ�߶�

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
// ��������: 
//      ����pInfo����Ϣ, �ӿڳ���ĵ�һ������, ��������, �����������ȫ��ʹ��, һ������ͻᵼ�±���
// 
// ����:
//      ����1: �ӿڳ���ĵ�һ������
//      ����2: ��ȡ�ı��, IGI_ ��ͷ����
//      ����3: Ҫ���õ�ֵ
//      ����3: �Ƿ��ػ�����, ����Բ���Ӱ�촰�ڵ�������Ч
// 
// ����ֵ:
//      ��������֮ǰ��ֵ
EXTERN_C _declspec(dllexport) LONG_PTR WINAPI iDraw_SetInfo(IDRAW_INFO* pInfo, int index, LONG_PTR value, BOOL isUpdatwWindow)
{
    if (!s_info || pInfo != s_info) return 0;
    //if (!s_info || (pInfo && pInfo != s_info)) return 0;
    LONG_PTR ret = 0;
    DWORD style = 0;
    IConfig& ini = *s_info->config;

    switch (index)
    {
    case IGI_EWINDOW:                   // ������������
    case IGI_CUSTOMCODETAB:             // ����ѡ��еĴ��ھ��
    case IGI_CODE_HWND:                 // ֻ��, ����������ȡ MDIClient �µĵ�һ�㴰�ھ��
    case IGI_SHADOWWINDOW:              // ֻ��, ��Ӱ����, ��������ǻ���������ڻ滭��, ��������ǲ㴰��
    case IGI_MDICLIENTTYPE:             // ֻ��, ��ǰMDIClient ����������ʾʲô����, -1=û�д���, 0=������, 1=���������, 2=��ʼҳ, ��λ�Ǵ�������, 0=δ֪, 1=���������, 2=��ͨ����, 3=���ڳ���, 4=��, 5=ȫ�ֱ���, 6=��������, 7=DLLĿ¼�����, 8=����, 9=ͼƬ��Դ, 10=������Դ
    case IGI_MDICLIENTCODETYPE:         // ֻ��, ��ǰMDIClient ����������ʾʲô��������, 0=δ֪, 1=���������, 2=��ͨ����, 3=���ڳ���, 4=��, 5=ȫ�ֱ���, 6=��������, 7=DLLĿ¼�����, 8=����, 9=ͼƬ��Դ, 10=������Դ
    case IGI_MDICLIENTTYPE_FROMINDEX:   // ��ָ��������ȡMDIClient ����������ʾʲô��������
    case IGI_RCCUSTOMCODETAB:           // ����ѡ��е�λ��, ���ڸ����ڵ�λ��
    case IGI_RCSTATE:                   // ����״̬�е�λ��, ���ڸ����ڵ�λ��
    case IGI_RCSTATUSBAR:               // ����״̬����λ��, ���ڸ����ڵ�λ��
    case IGI_RCMDICLIENT:               // �м��������λ��, ���ڸ����ڵ�λ��
    case IGI_RCCONTROL:                 // ������λ��, ���ڸ����ڵ�λ��
    case IGI_RCWORK:                    // �����е�λ��, ���ڸ����ڵ�λ��
    case IGI_RCMENUWND:                 // �˵���������λ��, ���ڸ����ڵ�λ��
    case IGI_IGETEWINDOW:               // �����Ի�ȡ�������ڵ���ָ��
    case IGI_IDENOTIFY:                 // �����Խ�������, ��������������൱�ڵ��������� ������ϵͳ����() ����
    case IGI_LIBPATH:                   // ������֧�ֿ�Ŀ¼, �������б��
    case IGI_DATAPATH:                  // ��IDE���ݴ洢Ŀ¼, �������б��, һ���� ������֧�ֿ�Ŀ¼/iDraw/
    case IGI_IDRAW:                     // �Ի���ӿ�, Ŀǰ��ʱδ���⹫��
    case IGI_D2D:                       // D2D��ӿ�, Ŀǰ��ʱδ���⹫��
    case IGI_PATH_LIB:                  // ֻ��, ������֧�ֿ�Ŀ¼, �������б��, ����unicode�ı�ָ��
    case IGI_PATH_IDRAW:                // ֻ��, ��IDE��Ҫʹ�õ�Ŀ¼, �������б��, һ���� ������֧�ֿ�Ŀ¼/iDraw/, ����unicode�ı�ָ��
    case IGI_PATH_DATA:                 // ֻ��, ��IDE���ݴ洢Ŀ¼, �������б��, һ���� ������֧�ֿ�Ŀ¼/iDraw/, ����unicode�ı�ָ��
    case IGI_PATH_PLUGIN:               // ֻ��, ��IDE���·��
    case IGI_PATH_CONFIG:               // ֻ��, ��IDE���������ļ�����·��
    case IGI_STATE_TOOL_GET:            // ֻ��, ��ȡָ��������ID��״̬, ����3��Ҫ���ݰ�ťID
    case IGI_STATE_TOOL_RUN:            // ֻ��, ��ǰ���������а�ť��״̬
    case IGI_STATE_TOOL_STOP:           // ֻ��, ��ǰ������ֹͣ��ť��״̬
    case IGI_CONFIG_DEFDXFONT:          // ֻ��, �˵�ʹ�õ�����
    case IGI_CONFIG_DEFGDIFONT:         // ֻ��, ����Ĭ��ʹ�õ�GDI����
        return 0;   // ������Щ����ֻ��, ����ֱ�ӷ���0

#define _igi_set(_case, _fun) \
    case _case:\
        ret = (LONG_PTR)_fun((int*)&value); break

        _igi_set(IGI_CONFIG_WORK_DIRECTION      , ini.tab_work_direction    );  // �����е��Ӽз���, 0=���, 1=����, 2=�ұ�, 3=�ױ�
        _igi_set(IGI_CONFIG_STATE_DIRECTION     , ini.tab_state_direction   );  // ״̬�е��Ӽз���, 0=���, 1=����, 2=�ұ�, 3=�ױ�
        _igi_set(IGI_CONFIG_CODE_DIRECTION      , ini.tab_code_direction    );  // ����ѡ��е��Ӽз���, 3=�ױ�, ���� = ����
        _igi_set(IGI_CONFIG_WORK_ISICON         , ini.tab_work_isIcon       );  // ���µ�״̬ѡ����Ƿ���ʾͼ��
        _igi_set(IGI_CONFIG_STATE_ISICON        , ini.tab_state_isIcon      );  // ��ߵĹ������Ƿ���ʾͼ��
        _igi_set(IGI_CONFIG_CODE_ISICON         , ini.tab_code_isIcon       );  // �����л�ѡ����Ƿ���ʾͼ��
        // ���漸����ֻ��
        //_igi_set(IGI_CONFIG_WORK_POS            , ini.tab_work_pos          );  // ������λ��, 0=���, 1=����, 2=�ұ�, 3=�ױ�
        //_igi_set(IGI_CONFIG_STATE_POS           , ini.tab_state_pos         );  // ״̬��λ��, 0=���, 1=����, 2=�ұ�, 3=�ױ�
        //_igi_set(IGI_CONFIG_CONTROL_POS         , ini.tab_control_pos       );  // �����λ��, 0=���, 1=����, 2=�ұ�, 3=�ױ�
        //_igi_set(IGI_CONFIG_WORK_FLAGS          , ini.tab_work_flags        );  // ������λ��, ��¼λ��, �Ƿ����, �Ƿ񸡶�, �Ƿ�ͣ������Ϣ TOP_STATE_BUTTON ö�ٳ������ֵ, ���� __query_flags �ж�ֵ
        //_igi_set(IGI_CONFIG_STATE_FLAGS         , ini.tab_state_flags       );  // ״̬��λ��, ��¼λ��, �Ƿ����, �Ƿ񸡶�, �Ƿ�ͣ������Ϣ TOP_STATE_BUTTON ö�ٳ������ֵ, ���� __query_flags �ж�ֵ
        //_igi_set(IGI_CONFIG_CONTROL_FLAGS       , ini.tab_control_flags     );  // �����λ��, ��¼λ��, �Ƿ����, �Ƿ񸡶�, �Ƿ�ͣ������Ϣ TOP_STATE_BUTTON ö�ٳ������ֵ, ���� __query_flags �ж�ֵ
        _igi_set(IGI_CONFIG_ISDXRENDER          , ini.IsDxRender            );  // �Ƿ�ʹ��DX��Ⱦ
        _igi_set(IGI_CONFIG_ISRUNSAVE           , ini.IsRunSave             );  // ����/������Ƿ񱣴�Դ��
        _igi_set(IGI_CONFIG_WHEELPOS            , ini.wheelPos              );  // ������һ�εľ���
        _igi_set(IGI_CONFIG_NOCLIPCHILDREN      , ini.noClipChildren        );  // ���ų��Ӵ�������
        _igi_set(IGI_CONFIG_MENUITEMHEIGHT      , ini.MenuItemHeight        );  // �˵���߶�
        _igi_set(IGI_CONFIG_MENUSEPARATORHEIGHT , ini.MenuSeparatorHeight   );  // �ָ����˵���߶�
        _igi_set(IGI_CONFIG_ISLOCATIONTOOLBAR   , ini.IsShowLocationTool    );  // �Ƿ���ʾ��λ������
        _igi_set(IGI_CONFIG_PROGRAMDBLCLKTYPE   , ini.programTreeDblclk     );  // �������Ϳ�˫��ģʽ, 0=ֻ�л�, 1=ֻչ��, 2=չ��+�л�
        _igi_set(IGI_CONFIG_NOCHANGETAB_WORK    , ini.noChangeTab_work      );  // ���Զ��л�������
        _igi_set(IGI_CONFIG_NOCHANGETAB_STATE   , ini.noChangeTab_state     );  // ���Զ��л�״̬��
        _igi_set(IGI_CONFIG_SCROLL_WIDTH        , ini.scroll_width          );  // ���������
        _igi_set(IGI_CONFIG_SCROLL_BTN_SIZE     , ini.scroll_btn_size       );  // ��������ť�ߴ�


        _igi_set(IGI_CONFIG_TREE_FNE_ISHOT      , ini.tree_fne_ishot        );  // ֧�ֿ����Ϳ��Ƿ�֧���ȵ�
        _igi_set(IGI_CONFIG_TREE_PROGRAM_ISHOT  , ini.tree_program_ishot    );  // �������Ϳ��Ƿ�֧���ȵ�
        _igi_set(IGI_CONFIG_TREE_CONTROL_ISHOT  , ini.tree_control_ishot    );  // ��������Ϳ��Ƿ�֧���ȵ�
        _igi_set(IGI_CONFIG_TREE_VAR_ISHOT      , ini.tree_var_ishot        );  // ���������Ϳ��Ƿ�֧���ȵ�
        _igi_set(IGI_CONFIG_TREE_CALL_ISHOT     , ini.tree_call_ishot       );  // ���ñ����Ϳ��Ƿ�֧���ȵ�
        _igi_set(IGI_CONFIG_LIST_FIND1_ISHOT    , ini.list_find1_ishot      );  // ��Ѱ1�б���Ƿ�֧���ȵ�
        _igi_set(IGI_CONFIG_LIST_FIND2_ISHOT    , ini.list_find2_ishot      );  // ��Ѱ2�б���Ƿ�֧���ȵ�

        _igi_set(IGI_CONFIG_TREE_FNE_HEIGHT     , ini.tree_fne_height       );  // ֧�ֿ����Ϳ���Ŀ�߶�
        _igi_set(IGI_CONFIG_TREE_PROGRAM_HEIGHT , ini.tree_program_height   );  // �������Ϳ���Ŀ�߶�
        _igi_set(IGI_CONFIG_TREE_CONTROL_HEIGHT , ini.tree_control_height   );  // ��������Ϳ���Ŀ�߶�
        _igi_set(IGI_CONFIG_TREE_VAR_HEIGHT     , ini.tree_var_height       );  // ���������Ϳ���Ŀ�߶�
        _igi_set(IGI_CONFIG_TREE_CALL_HEIGHT    , ini.tree_call_height      );  // ���ñ����Ϳ���Ŀ�߶�
        _igi_set(IGI_CONFIG_LIST_FIND1_HEIGHT   , ini.list_find1_height     );  // ��Ѱ1�б����Ŀ�߶�
        _igi_set(IGI_CONFIG_LIST_FIND2_HEIGHT   , ini.list_find2_height     );  // ��Ѱ2�б����Ŀ�߶�

    default:
        if (_info_case_color(index, value, ret, true, isUpdatwWindow))
            break;
        return 0;
    }

    s_info->pfnInfoChanged(index, ret, value, isUpdatwWindow);

    return ret;
}
#endif