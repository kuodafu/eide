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
    InvalidateRect(s_info->hEWnd, &rc, 0);      // �ػ����߱�����
    if (s_info->rcStatusBar.bottom - s_info->rcStatusBar.top > 0)   // ��Ҫ�ػ�״̬��
    {
        rc.top = s_info->rcStatusBar.top;
        rc.bottom = pData->pos.rcClient.bottom;
        InvalidateRect(s_info->hEWnd, &rc, 0);
    }

}

// Ĭ����ɫ���ı�, ��Ҫ���������໯��Ĭ����ɫ�ı�
// ����Ҫ�ж���Щ��ɫ�Ǳ��ı�, ��Щ��Ĭ�ϵ�
inline void _config_defcolor_change(int index, LONG_PTR oldValue, LONG_PTR newValue, bool isUpdateWindow)
{

}

// ���߿���صĶ��������ﴦ��, ��ѻ�ˢ�ͻ��ʶ����ó��µ���ɫ
inline void _config_boeder_change(int index, LONG_PTR oldValue, LONG_PTR newValue, bool isUpdateWindow)
{
    IThemeColor& theme = *s_info->themeColor;
    THEME_D2D_INFO& d2d = s_info->window_d2d_info;

    switch (index)
    {
    case IGI_CONFIG_CRBORDER:           // ���ڱ߿���ɫ����
    {
        _NEW_PEN_(d2d.hPenBorder);
        _NEW_BRUSH_(d2d.hbrBorder);
        if (isUpdateWindow)
            _window_changed_color(theme.crShadow(), newValue);
        break;
    }
    case IGI_CONFIG_CRSHADOW:           // ������Ӱ��ɫ����
    {
        _NEW_PEN_(d2d.hPenShadow);
        _NEW_BRUSH_(d2d.hbrShadow);
        if (isUpdateWindow)
            _window_changed_color(newValue, theme.crBorder());
        break;
    }
    case IGI_CONFIG_CRBORDER_RUN:       // ����ʱ���ڱ߿����ɫ
    {
        _NEW_PEN_(d2d.hPenBorder_run);
        _NEW_BRUSH_(d2d.hbrBorder_run);
        if (isUpdateWindow)
            _window_changed_color(theme.crShadow_run(), newValue);
        break;
    }
    case IGI_CONFIG_CRSHADOW_RUN:       // ����ʱ������Ӱ��ɫ
    {
        _NEW_PEN_(d2d.hPenShadow_run);
        _NEW_BRUSH_(d2d.hbrShadow_run);
        if (isUpdateWindow)
            _window_changed_color(newValue, theme.crBorder_run());
        break;
    }
    case IGI_CONFIG_CRBORDER_NF:        // ʧȥ����ʱ���ڱ߿����ɫ
    {
        _NEW_PEN_(d2d.hPenBorder_nf);
        _NEW_BRUSH_(d2d.hbrBorder_nf);
        if (isUpdateWindow)
            _window_changed_color(theme.crShadow_nf(), newValue);
        break;
    }
    case IGI_CONFIG_CRSHADOW_NF:        // ʧȥ����ʱ������Ӱ��ɫ
    {
        _NEW_PEN_(d2d.hPenShadow_nf);
        _NEW_BRUSH_(d2d.hbrShadow_nf);
        if (isUpdateWindow)_window_changed_color(newValue, theme.crBorder_nf());
        break;
    }

    case IGI_CONFIG_CRMENUBORDER:           // �˵���ı߿���ɫ
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
    //! �����������֮�󻹻ᴥ���滭������ñ��ı�, ��Ҫ�ڻ滭������޸Ĺ�������ɫ
    // �������滭ʱÿ�ζ��ᴴ����ˢ, ֻҪö�ٳ������������޸���ɫ�Ϳ���
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
        case CUSTOMTAB_INFO_UNDEFINE:           // δ����
            break;
        case CUSTOMTAB_INFO_WINDOW:             // ���������
            CustomTabData.crText = theme.crTab_text_window();
            break;
        case CUSTOMTAB_INFO_ASSEMBLY:           // ��ͨ����
            CustomTabData.crText = theme.crTab_text_assembly();
            break;
        case CUSTOMTAB_INFO_ASSEMBLY_WINDOW:    // ���ڳ���
            CustomTabData.crText = theme.crTab_text_assembly_window();
            break;
        case CUSTOMTAB_INFO_CLASS:              // ��
            CustomTabData.crText = theme.crTab_text_class();
            break;
        case CUSTOMTAB_INFO_GLOBALVAR:          // ȫ�ֱ���
            CustomTabData.crText = theme.crTab_text_globalvar();
            break;
        case CUSTOMTAB_INFO_STRUCT:             // ��������
            CustomTabData.crText = theme.crTab_text_struct();
            break;
        case CUSTOMTAB_INFO_DLL:                // DLL������
            CustomTabData.crText = theme.crTab_text_dll();
            break;
        case CUSTOMTAB_INFO_CONST:              // ����
            CustomTabData.crText = theme.crTab_text_const();
            break;
        case CUSTOMTAB_INFO_IMAGE:              // ͼƬ��Դ
            CustomTabData.crText = theme.crTab_text_image();
            break;
        case CUSTOMTAB_INFO_SOUND:              // ������Դ
            CustomTabData.crText = theme.crTab_text_sound();
            break;
        case CUSTOMTAB_INFO_CLOSEHOT:           // �رհ�ť�ȵ�
            CustomTabData.crText = theme.close_hot();
            break;
        case CUSTOMTAB_INFO_CLOSEDOWN:          // �رհ�ť����
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
    // Ĭ����ɫ
    case IGI_CONFIG_CRDEF_BACK:         // [DefaultColor], Ĭ����ɫ, ������ɫ
    case IGI_CONFIG_CRDEF_NORMAL:       // [DefaultColor], Ĭ����ɫ, ������ɫ
    case IGI_CONFIG_CRDEF_HOT:          // [DefaultColor], Ĭ����ɫ, �ȵ���ɫ
    case IGI_CONFIG_CRDEF_DOWN:         // [DefaultColor], Ĭ����ɫ, ������ɫ
    case IGI_CONFIG_CRDEF_BAN:          // [DefaultColor], Ĭ����ɫ, ��ֹ��ɫ
    case IGI_CONFIG_CRDEF_HOVER:        // [DefaultColor], Ĭ����ɫ, ������ɫ
    case IGI_CONFIG_CRDEF_FOCUS:        // [DefaultColor], Ĭ����ɫ, ������ɫ
    case IGI_CONFIG_CRDEF_CHECKED:      // [DefaultColor], Ĭ����ɫ, ѡ����ɫ
    case IGI_CONFIG_CRDEF_SELECTED:     // [DefaultColor], Ĭ����ɫ, ѡ����ɫ
    case IGI_CONFIG_CRDEF_VISTED:       // [DefaultColor], Ĭ����ɫ, 
    case IGI_CONFIG_CRDEF_SHADOW:       // [DefaultColor], Ĭ����ɫ, ��Ӱ��ɫ
    case IGI_CONFIG_CRDEF_BORDER:       // [DefaultColor], Ĭ����ɫ, �߿���ɫ
    case IGI_CONFIG_CRDEF_TEXT:         // [DefaultColor], Ĭ����ɫ, �ı���ɫ
    case IGI_CONFIG_CRDEF_TEXTHOT:      // [DefaultColor], Ĭ����ɫ, �ı��ȵ���ɫ
    case IGI_CONFIG_CRDEF_TEXTDOWN:     // [DefaultColor], Ĭ����ɫ, �ı�������ɫ
    case IGI_CONFIG_CRDEF_TEXTBAN:      // [DefaultColor], Ĭ����ɫ, �ı���ֹ��ɫ
        _config_defcolor_change(index, oldValue, newValue, isUpdateWindow);
        break;
    case IGI_CONFIG_CRBACK:             // ������ɫ
    {
        _NEW_BRUSH_(d2d.hbrBack);
        if (isUpdateWindow)
            InvalidateRect(s_info->hEWnd, 0, 0);   // �ػ���������
        break;
    }
    case IGI_CONFIG_CRBACKEXTERN:       // �ⲿ�ı�����ɫ, ����ѡ��е��Ӽ�
    {
        //TODO ��Ҫ�������õ������ɫ��������ػ�, �����ɫ��ʱû�д���dx�滭����
        if (isUpdateWindow)
            InvalidateRect(s_info->hEWnd, 0, 0);   // �ػ���������
        break;
    }
    case IGI_CONFIG_CRSTATEBAR:         // �����/������/״̬�ж����������ɫ
    {
        // �滭����״̬������ɫ��ÿ�λ滭�����������ȡȻ�󴴽�, �����þ�����
        break;
    }
    case IGI_CONFIG_CRTEXT:             // һ���ı���ɫ
    {
        _NEW_BRUSH_(d2d.hbrText);
        if (isUpdateWindow)
            InvalidateRect(s_info->hEWnd, 0, 0);   // �ػ���������
        break;
    }
    // �߿�/��Ӱ��ɫ 0x0230 - 0x023F
    case IGI_CONFIG_CRBORDER:               // ���ڱ߿���ɫ
    case IGI_CONFIG_CRBORDER_RUN:           // ���ڱ߿�����ʱ����ɫ
    case IGI_CONFIG_CRBORDER_NF:            // ���ڱ߿�ʧȥ����ʱ����ɫ
    case IGI_CONFIG_CRSHADOW:               // ������Ӱ��ɫ
    case IGI_CONFIG_CRSHADOW_RUN:           // ������Ӱ����ʱ����ɫ
    case IGI_CONFIG_CRSHADOW_NF:            // ������Ӱʧȥ����ʱ����ɫ
    case IGI_CONFIG_CRMENUBORDER:           // �˵���ı߿���ɫ
        _config_boeder_change(index, oldValue, newValue, isUpdateWindow);
        break;

    // �˵��� 0x0240 - 0x024F
    case IGI_CONFIG_CRMENUBACK:             // �˵���ı�����ɫ, û���ȵ�, ���ǽ�ֹʱʹ�������ɫ�滭
        _NEW_BRUSH_(d2d.hbrMenuBack);
        _NEW_BRUSH_(d2d.hbrMenuDown);
        break;
    case IGI_CONFIG_CRMENUHOT:              // �˵�����ȵ���ɫ
        _NEW_BRUSH_(d2d.hbrMenuHot);
        break;
    case IGI_CONFIG_CRMENUBAN:              // �˵���Ľ�ֹ��ɫ
        _NEW_BRUSH_(d2d.hbrMenuBan);
        break;
    case IGI_CONFIG_CRMENUTEXT:             // �˵��������ı���ɫ
        _NEW_BRUSH_(d2d.hbrMenuText);
        break;
    case IGI_CONFIG_CRMENUTEXTHOT:          // �˵����ȵ��ı���ɫ
        _NEW_BRUSH_(d2d.hbrMenuTextHot);
        break;
    case IGI_CONFIG_CRMENUTEXTBAN:          // �˵����ֹ�ı���ɫ
        _NEW_BRUSH_(d2d.hbrMenuTextBan);
        break;

    // ��ʾ�ڱ���������Ĳ˵��� 0x0250 - 0x025F
    case IGI_CONFIG_CRMENUBARHOT:           // �����˵����ȵ���ɫ
        _NEW_BRUSH_(d2d.hbrMenuBarHot);
        if (isUpdateWindow)InvalidateRect(s_info->hEWnd, &s_info->rcMenu, 0);   // �ػ��˵���
        break;
    case IGI_CONFIG_CRMENUBARTEXT:          // �����˵������ֲ˵����ı���ɫ
        _NEW_BRUSH_(d2d.hbrMenuBarToolsText);
        if (isUpdateWindow)InvalidateRect(s_info->hEWnd, &s_info->rcMenu, 0);   // �ػ��˵���
        break;
    case IGI_CONFIG_CRMENUBARTEXTHOT:       // �����˵������ֲ˵����ı��ȵ���ɫ
        _NEW_BRUSH_(d2d.hbrMenuBarToolsTextHot);
        if (isUpdateWindow)InvalidateRect(s_info->hEWnd, &s_info->rcMenu, 0);   // �ػ��˵���
        break;

    // ��ʾ�ڱ��������ɫ�����ɫ 0x0260 - 0x026F
    case IGI_CONFIG_CRTITLEBACK:            // ���ߺڿ鲿�ֱ�����ɫ
    case IGI_CONFIG_CRTITLELEFT_ACT:        // �����ڿ鲿����߼���ʱ�ı���ɫ, �����Դ����������Ŀ��
    case IGI_CONFIG_CRTITLERIGHT_ACT:       // �����ڿ鲿���ұ߼���ʱ�ı���ɫ, ����ǳ�������, ����δ����ʱ���Ǻ�ʹ�õ���ɫ
    case IGI_CONFIG_CRTITLELEFT:            // �����ڿ鲿������ı���ɫ, �����Դ����������Ŀ��
    case IGI_CONFIG_CRTITLERIGHT:           // �����ڿ鲿���ұ��ı���ɫ, ����ǳ�������, ����δ����ʱ���Ǻ�ʹ�õ���ɫ
    {
        switch (index)
        {
        case IGI_CONFIG_CRTITLEBACK:            // ���ߺڿ鲿�ֱ�����ɫ
            _NEW_BRUSH_(d2d.hbrTitleback); break;
        case IGI_CONFIG_CRTITLELEFT_ACT:        // �����ڿ鲿����߼���ʱ�ı���ɫ, �����Դ����������Ŀ��
            _NEW_BRUSH_(d2d.hbrTitle_left_act); break;
        case IGI_CONFIG_CRTITLERIGHT_ACT:       // �����ڿ鲿���ұ߼���ʱ�ı���ɫ, ����ǳ�������, ����δ����ʱ���Ǻ�ʹ�õ���ɫ
            _NEW_BRUSH_(d2d.hbrTitle_right_act); break;
        case IGI_CONFIG_CRTITLELEFT:            // �����ڿ鲿������ı���ɫ, �����Դ����������Ŀ��
            _NEW_BRUSH_(d2d.hbrTitle_left); break;
        default:
            _NEW_BRUSH_(d2d.hbrTitle_right); break;
        }
        RECT rc;
        GetClientRect(s_info->hEWnd, &rc);
        rc.bottom = s_info->rcMenu.bottom;
        if (isUpdateWindow)InvalidateRect(s_info->hEWnd, &rc, 0);   // �ػ�����������λ��
        break;
    }

    // �ײ�״̬������ɫ 0x0270 - 0x027F
    case IGI_CONFIG_CRTEXTSTATUSBAR:        // �ײ�״̬�����ı���ɫ, �����Ӧ��ɫû�д�����ˢ���߻����򷵻�0
    case IGI_CONFIG_CRTEXTSTATUSBAR_RUN:    // �ײ�״̬������ʱ���ı���ɫ, �����Ӧ��ɫû�д�����ˢ���߻����򷵻�0
    case IGI_CONFIG_CRTEXTSTATUSBAR_NF:     // �ײ�״̬����������ı���ɫ, �����Ӧ��ɫû�д�����ˢ���߻����򷵻�0
    {
        switch (index)
        {
        case IGI_CONFIG_CRTEXTSTATUSBAR:        // �ײ�״̬�����ı���ɫ, �����Ӧ��ɫû�д�����ˢ���߻����򷵻�0
            _NEW_BRUSH_(d2d.hbrTextStatusBar);
            break;
        case IGI_CONFIG_CRTEXTSTATUSBAR_RUN:    // �ײ�״̬������ʱ���ı���ɫ, �����Ӧ��ɫû�д�����ˢ���߻����򷵻�0
            _NEW_BRUSH_(d2d.hbrTextStatusBar_run);
            break;
        default:
            _NEW_BRUSH_(d2d.hbrTextStatusBar_nf);
            break;
        }
        RECT rc;
        GetClientRect(s_info->hEWnd, &rc);
        rc.top = s_info->rcStatusBar.top;
        if (isUpdateWindow)InvalidateRect(s_info->hEWnd, &rc, 0);   // �ػ��ױ�״̬��λ��
        break;
    }

    // ��������ɫ 0x0280 - 0x028F
    case IGI_CONFIG_CRSCROLLBTN1NORRMAL:    // ��������ť1������ɫ, ��߻��߶��ߵ�С��ť
    case IGI_CONFIG_CRSCROLLBTN1HOT:        // ��������ť1�ȵ���ɫ, ��߻��߶��ߵ�С��ť
    case IGI_CONFIG_CRSCROLLBTN1DOWN:       // ��������ť1������ɫ, ��߻��߶��ߵ�С��ť
    case IGI_CONFIG_CRSCROLLBTN2NORRMAL:    // ��������ť2������ɫ, �ұ߻��ߵױߵ�С��ť
    case IGI_CONFIG_CRSCROLLBTN2HOT:        // ��������ť2�ȵ���ɫ, �ұ߻��ߵױߵ�С��ť
    case IGI_CONFIG_CRSCROLLBTN2DOWN:       // ��������ť2������ɫ, �ұ߻��ߵױߵ�С��ť
    case IGI_CONFIG_CRSCROLLTHUMBNORRMAL:   // ������������ɫ
    case IGI_CONFIG_CRSCROLLTHUMBHOT:       // �����������ȵ���ɫ
    case IGI_CONFIG_CRSCROLLTHUMBDOWN:      // ���������鰴����ɫ
    case IGI_CONFIG_CRSCROLLBACKGROUND:     // ������������ɫ
        _config_scroll_change(index, oldValue, newValue, isUpdateWindow);
        break;
    // �ָ�����ɫ 0x0290 - 0x029F
    case IGI_CONFIG_CRLINELEFT:             // ������, ���б�ͷ, ������ķָ��������ɫ, �ָ��߶�����������
    case IGI_CONFIG_CRLINERIGHT:            // ������, ���б�ͷ, ������ķָ����ұ���ɫ, �ָ��߶�����������
        break;

        // �Ի�����л�ѡ��йرհ�ť��ɫ, 0x02A0 - 0x02AF
    case IGI_CONFIG_CODETAB_CRCLOSEHOT:             // [CustomCodeTab], �����л�ѡ��йرհ�ť�ȵ���ɫ
    case IGI_CONFIG_CODETAB_CRCLOSEDOWN:            // [CustomCodeTab], �����л�ѡ��йرհ�ť������ɫ
    // �Ի�����л�ѡ����ı���ɫ, 0x02B0 - 0x02BF
    case IGI_CONFIG_CODETAB_CRTEXT_WINDOW:          // [CustomCodeTab], �����л�ѡ���, �����ı���ɫ
    case IGI_CONFIG_CODETAB_CRTEXT_ASSEMBLY:        // [CustomCodeTab], �����л�ѡ���, �����ı���ɫ
    case IGI_CONFIG_CODETAB_CRTEXT_ASSEMBLY_WINDOW: // [CustomCodeTab], �����л�ѡ���, ���ڳ����ı���ɫ
    case IGI_CONFIG_CODETAB_CRTEXT_CLASS:           // [CustomCodeTab], �����л�ѡ���, ���ı���ɫ
    case IGI_CONFIG_CODETAB_CRTEXT_GLOBALVAR:       // [CustomCodeTab], �����л�ѡ���, ȫ�ֱ����ı���ɫ
    case IGI_CONFIG_CODETAB_CRTEXT_STRUCT:          // [CustomCodeTab], �����л�ѡ���, �Զ������������ı���ɫ
    case IGI_CONFIG_CODETAB_CRTEXT_DLL:             // [CustomCodeTab], �����л�ѡ���, DLL�ı���ɫ
    case IGI_CONFIG_CODETAB_CRTEXT_CONST:           // [CustomCodeTab], �����л�ѡ���, �����ı���ɫ
    case IGI_CONFIG_CODETAB_CRTEXT_IMAGE:           // [CustomCodeTab], �����л�ѡ���, ͼƬ�ı���ɫ
    case IGI_CONFIG_CODETAB_CRTEXT_SOUND:           // [CustomCodeTab], �����л�ѡ���, �����ı���ɫ
    // �Ի�����л�ѡ��е���������ɫ, 0x02C0 - 0x02CF
    case IGI_CONFIG_CODETAB_CRLINE_WINDOW:          // [CustomCodeTab], �����л�ѡ���, �����±�����ɫ
    case IGI_CONFIG_CODETAB_CRLINE_ASSEMBLY:        // [CustomCodeTab], �����л�ѡ���, �����±�����ɫ
    case IGI_CONFIG_CODETAB_CRLINE_ASSEMBLY_WINDOW: // [CustomCodeTab], �����л�ѡ���, ���ڳ����±�����ɫ
    case IGI_CONFIG_CODETAB_CRLINE_CLASS:           // [CustomCodeTab], �����л�ѡ���, ���±�����ɫ
    case IGI_CONFIG_CODETAB_CRLINE_GLOBALVAR:       // [CustomCodeTab], �����л�ѡ���, ȫ�ֱ����±�����ɫ
    case IGI_CONFIG_CODETAB_CRLINE_STRUCT:          // [CustomCodeTab], �����л�ѡ���, �Զ������������±�����ɫ
    case IGI_CONFIG_CODETAB_CRLINE_DLL:             // [CustomCodeTab], �����л�ѡ���, DLL�±�����ɫ
    case IGI_CONFIG_CODETAB_CRLINE_CONST:           // [CustomCodeTab], �����л�ѡ���, �����±�����ɫ
    case IGI_CONFIG_CODETAB_CRLINE_IMAGE:           // [CustomCodeTab], �����л�ѡ���, ͼƬ�±�����ɫ
    case IGI_CONFIG_CODETAB_CRLINE_SOUND:           // [CustomCodeTab], �����л�ѡ���, �����±�����ɫ
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
    case IGI_CONFIG_ISDXRENDER:          // �Ƿ�ʹ��DX��Ⱦ, ������Ҫ������Ч
        break;  // DX��Ⱦ��Ҫ����
    case IGI_CONFIG_ISRUNSAVE:           // ����/������Ƿ񱣴�Դ��
        break;  // ��������������ڴ���, �Ӿ�����ʱ��ʹ��
    case IGI_CONFIG_WHEELPOS:            // ������һ�εľ���
        break;  // ���ֵ�޸ĺ�ʵʱ��Ч, �ڹ�������ÿ�ζ�����������
    case IGI_CONFIG_NOCLIPCHILDREN:      // �����ڻ滭ʱ���ų��Ӵ�������
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

    case IGI_CONFIG_MENUITEMHEIGHT:         // �˵���߶�
    case IGI_CONFIG_MENUSEPARATORHEIGHT:    // �ָ����˵���߶�
        break;  // ������Ч
    case IGI_CONFIG_ISLOCATIONTOOLBAR:      // �Ƿ���ʾ��λ������
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
    case IGI_CONFIG_PROGRAMDBLCLKTYPE:      // �������Ϳ�˫��ģʽ, 0=ֻ�л�, 1=ֻչ��, 2=չ��+�л�
        break;  // �����ÿ��ʹ��ʱ��ȡ


    case IGI_CONFIG_NOCHANGETAB_WORK:   // ���Զ��л�������
        break;  // �����ÿ��ʹ��ʱ��ȡ
    case IGI_CONFIG_NOCHANGETAB_STATE:  // ���Զ��л�״̬��
        break;  // �����ÿ��ʹ��ʱ��ȡ
    case IGI_CONFIG_SCROLL_WIDTH:       // ���������
    case IGI_CONFIG_SCROLL_BTN_SIZE:    // ��������ť�ߴ�
        if ( isUpdateWindow )
            InvalidateRect(s_info->hEWnd, 0, 0);
        break;
    case IGI_CONFIG_WORK_DIRECTION:     // �����е��Ӽз���, 0=���, 1=����, 2=�ұ�, 3=�ױ�
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetWorkTab());
        if (pData)
            _tab_modify_direction(newValue, pData);

        break;
    }
    case IGI_CONFIG_WORK_ISICON:       // ��ߵĹ������Ƿ���ʾͼ��
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetWorkTab());
        if (pData)
            _tab_modify_icon(newValue, pData);
        break;
    }
    //case IGI_CONFIG_WORK_ISSHOW:       // �������Ƿ���ʾ, ���Ϊfalse, ��ʧȥ�������������
    //    break;  // ��ʱ��ûʹ��
    //case IGI_CONFIG_WORK_POS:           // ������λ��, 0=���, 1=����, 2=�ұ�, 3=�ױ�
    //    break;  // ��ʱ��ûʹ��
    case IGI_CONFIG_WORK_FLAGS:         // �����б�ʶ, ��¼λ��, �Ƿ����, �Ƿ񸡶�, �Ƿ�ͣ������Ϣ TOP_STATE_BUTTON ö�ٳ������ֵ, ���� __query_flags �ж�ֵ
        break;
    case IGI_CONFIG_STATE_DIRECTION:    // ״̬�е��Ӽз���, 0=���, 1=����, 2=�ұ�, 3=�ױ�
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetStateTab());
        if (pData)
            _tab_modify_direction(newValue, pData);
        break;
    }
    case IGI_CONFIG_STATE_ISICON:     // ���µ�״̬ѡ����Ƿ���ʾͼ��
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetStateTab());
        if (pData)
            _tab_modify_icon(newValue, pData);
        break;
    }
    //case IGI_CONFIG_STATE_ISSHOW:       // ״̬���Ƿ���ʾ, ���Ϊfalse, ��ʧȥ�������������
    //    break;  // ��ʱ��ûʹ��
    //case IGI_CONFIG_STATE_POS:          // ״̬��λ��, 0=���, 1=����, 2=�ұ�, 3=�ױ�
    //    break;  // ��ʱ��ûʹ��
    case IGI_CONFIG_STATE_FLAGS:        // ״̬�б�ʶ, ��¼λ��, �Ƿ����, �Ƿ񸡶�, �Ƿ�ͣ������Ϣ TOP_STATE_BUTTON ö�ٳ������ֵ, ���� __query_flags �ж�ֵ
        break;
    case IGI_CONFIG_CODE_DIRECTION:     // ����ѡ��е��Ӽз���, 3=�ױ�, ���� = ����
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->hEWnd);
        SendEWindowSize(pData); // �������������ڷ��͸ı�ߴ���¼�, ���µ�������
        break;
    }
    case IGI_CONFIG_CODE_ISICON:        // �����л�ѡ����Ƿ���ʾͼ��
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
    //case IGI_CONFIG_CONTROL_ISSHOW:     // ������Ƿ���ʾ, ���Ϊfalse, ��ʧȥ�������������
    //    break;  // ��ʱ��ûʹ��
    //case IGI_CONFIG_CONTROL_POS:        // �����λ��, 0=���, 1=����, 2=�ұ�, 3=�ױ�
    //    break;  // ��ʱ��ûʹ��
    case IGI_CONFIG_CONTROL_FLAGS:      // ������ʶ, ��¼λ��, �Ƿ����, �Ƿ񸡶�, �Ƿ�ͣ������Ϣ TOP_STATE_BUTTON ö�ٳ������ֵ, ���� __query_flags �ж�ֵ
        break;

    case IGI_CONFIG_TREE_FNE_ISHOT:         // ֧�ֿ����Ϳ��Ƿ�֧���ȵ�
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetWorkFneTree());
        if (pData)
            pData->draw.isHot = newValue;
        break;
    }
    case IGI_CONFIG_TREE_PROGRAM_ISHOT:     // �������Ϳ��Ƿ�֧���ȵ�
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetWorkProgramTree());
        if (pData)
            pData->draw.isHot = newValue;
        break;
    }
    case IGI_CONFIG_TREE_CONTROL_ISHOT:     // ��������Ϳ��Ƿ�֧���ȵ�
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetControlTree());
        if (pData)
            pData->draw.isHot = newValue;
        break;
    }
    case IGI_CONFIG_TREE_CONTROL_ORGICON:   // ��������Ϳ���ʾԭͼ��
    {
        //! TVM_SETITEMW wParam == 121007124 && lParam == 20752843 ��ʱ�����֪ͨ�������ʾԭͼ������ñ��ı���, ��Ҫ���´���ͼƬ��
        SendMessageW(s_info->eWnd->GetControlTree(), TVM_SETITEMW, 121007124, 20752843);
        InvalidateRect(s_info->eWnd->GetControlTree(), 0, 0);
        break;  // �����ÿ�λ滭���Ϳ���Ŀ�����ȡ, ������Ҫ�ػ�
    }
    case IGI_CONFIG_TREE_VAR_ISHOT:         // ���������Ϳ��Ƿ�֧���ȵ�
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetStateVarTableTree());
        if (pData)
            pData->draw.isHot = newValue;
        break;
    }
    case IGI_CONFIG_TREE_CALL_ISHOT:        // ���ñ����Ϳ��Ƿ�֧���ȵ�
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetStateTabCallTable());
        if (pData)
            pData->draw.isHot = newValue;
        break;
    }
    case IGI_CONFIG_LIST_FIND1_ISHOT:       // ��Ѱ1�б���Ƿ�֧���ȵ�
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetStateTabFind1());
        if (pData)
            pData->draw.isHot = newValue;
        break;
    }
    case IGI_CONFIG_LIST_FIND2_ISHOT:       // ��Ѱ2�б�����Ϳ��Ƿ�֧���ȵ�
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetStateTabFind2());
        if (pData)
            pData->draw.isHot = newValue;
        break;
    }

    case IGI_CONFIG_TREE_FNE_HEIGHT:        // ֧�ֿ����Ϳ���Ŀ�߶�
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetWorkFneTree());
        if (!pData) break;
        pData->draw.nItemHeight = newValue;
        SendMessageW(pData->hWnd, TVM_SETITEMHEIGHT, pData->draw.nItemHeight, 0);
        break;
    }
    case IGI_CONFIG_TREE_PROGRAM_HEIGHT:    // �������Ϳ���Ŀ�߶�
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetWorkProgramTree());
        if (!pData) break;
        pData->draw.nItemHeight = newValue;
        SendMessageW(pData->hWnd, TVM_SETITEMHEIGHT, pData->draw.nItemHeight, 0);
        break;
    }
    case IGI_CONFIG_TREE_CONTROL_HEIGHT:    // ��������Ϳ���Ŀ�߶�
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetControlTree());
        if (!pData) break;
        pData->draw.nItemHeight = newValue;
        SendMessageW(pData->hWnd, TVM_SETITEMHEIGHT, pData->draw.nItemHeight, 0);
        break;
    }
    case IGI_CONFIG_TREE_VAR_HEIGHT:        // ���������Ϳ���Ŀ�߶�
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetStateVarTableTree());
        if (!pData) break;
        pData->draw.nItemHeight = newValue;
        SendMessageW(pData->hWnd, TVM_SETITEMHEIGHT, pData->draw.nItemHeight, 0);
        break;
    }
    case IGI_CONFIG_TREE_CALL_HEIGHT:       // ���ñ����Ϳ���Ŀ�߶�
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetStateTabCallTable());
        if (!pData) break;
        pData->draw.nItemHeight = newValue;
        SendMessageW(pData->hWnd, TVM_SETITEMHEIGHT, pData->draw.nItemHeight, 0);
        break;
    }
    case IGI_CONFIG_LIST_FIND1_HEIGHT:      // ��Ѱ1�б����Ŀ�߶�
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetStateTabFind1());
        if (!pData) break;
        pData->draw.nItemHeight = newValue;
        SendMessageW(pData->hWnd, LB_SETITEMHEIGHT, pData->draw.nItemHeight, 0);
        break;
    }
    case IGI_CONFIG_LIST_FIND2_HEIGHT:      // ��Ѱ2�б����Ŀ�߶�
    {
        pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetStateTabFind2());
        if (!pData) break;
        pData->draw.nItemHeight = newValue;
        SendMessageW(pData->hWnd, LB_SETITEMHEIGHT, pData->draw.nItemHeight, 0);
        break;
    }
    default:
        if (index >= 0x200 && index < 0x400)    // 0x0200 - 0x03FF ����ɫ��ɫ
            _config_change_color(index, oldValue, newValue, isUpdateWindow);
        break;
    }
}