#if 1
#include "EWindow_Fne_Main.h"
#include <control/ReadColor.h>
#include <elib/fne_decrypt.h>


inline int _idraw_function(int nCode, WPARAM wParam, LPARAM lParam)
{
    switch (nCode)
    {
    case IDC_IDRAW_SET_ETOOLSFUN:
    {
        s_info->pfnETools = (pfn_IDraw_Callback)lParam;
        return 0;
    }
    case IDC_IDRAW_STR2RGB:     // ��һ���ı���ɫת����RGB��ɫ, wParam = Ĭ����ɫ, ����ʧ�ܷ��������ɫ, lParamָ����ɫ�ı�ָ��
    {
        if (!lParam)return wParam;
        return str2rgb((LPCWSTR)lParam, wParam);
    }
    case IDC_IDRAW_STR2ARGB:    // ��һ���ı���ɫת����ARGB��ɫ, wParam = Ĭ����ɫ, ����ʧ�ܷ��������ɫ, lParamָ����ɫ�ı�ָ��
    {
        if (!lParam)return wParam;
        return str2argb((LPCWSTR)lParam, wParam);
    }
    case IDC_IDRAW_RGB2STR:     // ��RGB��ɫת�����ı� RGB(1,2,3), wParam=Ҫת����rgb, lParam = ����20��WCHAR������ָ��, ����д���ַ���
    {
        if (!lParam) return 0;
        const wstr str = rgb2str(wParam);
        const size_t ret = str.size();
        const size_t len = ret * sizeof(wchar_t) + sizeof(wchar_t);
        memcpy((LPWSTR)lParam, str.c_str(), len);
        return (int)ret;
    }
    case IDC_IDRAW_ARGB2STR:    // ��ARGB��ɫת�����ı� ARGB(255,1,2,3), wParam=Ҫת����argb, lParam = ����26��WCHAR������ָ��, ����д���ַ���
    {
        if (!lParam) return 0;
        const wstr str = argb2str(wParam);
        const size_t ret = str.size();
        const size_t len = ret * sizeof(wchar_t) + sizeof(wchar_t);
        memcpy((LPWSTR)lParam, str.c_str(), len);
        return (int)ret;
    }
    case IDC_IDRAW_GETWINDOWDATA:
    {
        if (!lParam || !s_info) return 0;
        return (int)s_info->pfnGetDataFromHwnd((HWND)lParam);
    }
    case IDC_IDRAW_GETCODEVISIBLERECT:  // ��ȡ���봰�ڿ��ӵľ���, wParam����, lParam = ���վ��ε�ָ��(����������������λ��)
    case IDC_SCROLL_GETRECT:            // ��ȡ������λ��, ����ͨ������¼��޸ı߿�ﵽָ���滭������λ��, wParam = pfn_GetScrollRect �ص�����, lParam = ��������ָ��
    case IDC_SCROLL_CALCRECT:           // ���������λ��, ����ͨ������¼��޸Ĵ��ڷǿͻ���λ��, wParam = pfn_CalcScroll �ص�����, lParam = ��������ָ��
    case IDC_SCROLL_DRAW:               // �滭������, wParam = pfn_DrawScroll �ص�����, lParam = ��������ָ��
    case IDC_IDRAW_INLINEHOOK:          // ����inlineHook, wParam = ԭ����, lParam = �º���
    case IDC_IDRAW_UNINLINEHOOK:        // ȡ��inlineHook, wParam = ԭ����, lParam = �º���, �����Ƿ�ɹ�
    case IDC_IDRAW_UNINLINEHOOK_PLUGIN: // ȡ�����ͷŲ����inlineHook, wParam = ģ����ʼ��ַ, lParam = ģ�������ַ
    case IDC_IDRAW_CREATEPOPUPMENU:     // �������Ի���ʽ�ĵ���ʽ�˵�, ���м��뵽�������ʽ�˵�����Ӳ˵����ᱻ�Ի�
    case IDC_EIDE_GETCODEMANAGE:        // ��ȡIDE�������(IEIDE_CodeManage)��ָ��, ����wParam����, lParma = ���봰�ڴ��ھ��, Ϊ0���ȡ��ǰ����
    case IDC_EIDE_GETLINESPACING:       // ��ȡ���м��, ����wParam�� lParam����
    case IDC_EIDE_SETLINESPACING:       // ��δʵ��, ���õ��м��, ����wParam�� lParam����
    case IDC_EIDE_GETLINEHEIGHT:        // ��ȡ���и߶�, ����wParam�� lParam����
    case IDC_EIDE_SETLINEHEIGHT:        // ��δʵ��, ���õ��и߶�, ����wParam�� lParam����
    case IDC_EIDE_GETCODEWINDOW:        // ��ȡ��ǰ�������, �����ǰ����ʼҳ��������������򷵻�0, ����wParam�� lParam����
    case IDC_EIDE_GETECOLOR:            // ��ȡ�����Ե�ǰʹ�õ���ɫ, wParam = �������ߴ�, lParam = ��������ַ, ����д�뻺�����ֽ���, wParam/lParamΪ0�򷵻ػ�������Ҫ�ĳߴ�
    case IDC_EIDE_SETECOLOR:            // ���������Ե�ǰʹ�õ���ɫ, wParamΪ0��lParam����������ɫ�ļ�·��Unicode��, ��Ϊ0��lParamָ����ɫ����, wParamΪ���ݳߴ�, lParam = ����wParam������ʲôֵ
    case IDC_IDRAW_SUBWINDOWEX:         // ���໯����, ����������ػ����Ӿ��ⴰ��һ���ķ��, ֻ�޶��㴰��, wParam����, lParam = DRAW_WINDOW_ARGUMENT �ṹָ��, ���ش��ھ��
    case IDC_IDRAW_CREATEWINDOW:        // ����һ�����Ӿ��ⴰ��һ�����Ķ��㴰��, wParam= CREATESTRUCTW �ṹָ��, lParam = DRAW_WINDOW_ARGUMENT �ṹָ��, ���ش��ھ��
    {
        return s_info->pfnControls(nCode, wParam, lParam);
    }
    default:
        break;
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �����Ӿ����ṩ�Ĺ���
// 
// ����:
//      ����1: ���ܱ��, IDC_ ��ͷ����
//      ����2: ���ݹ��ܴ��ݲ���
//      ����3: ���ݹ��ܴ��ݲ���
// 
// ����ֵ:
//      �ο� IDC_ ��ͷ������˵��
EXTERN_C _declspec(dllexport) int WINAPI iDraw_IDCall(int nCode, WPARAM wParam, LPARAM lParam)
{
    switch (nCode)
    {
    case IDC_ICONTROL_UNLOADLIB:
    {
        bool isOk = false;
        const LPCSTR name = "iDraw";
        if (wParam == 1)
        {
            isOk = fne::addFne(name);
            fne::deleteFne("eTools");
        }
        else if (wParam == 0)
        {
            isOk = fne::deleteFne(name);
        }
        else if (wParam == -1)
        {
            isOk = fne::isFne(name);
        }
        return isOk;
    }
    default:
        break;
    }
    if (nCode >= IDC_IDRAW_BEGIN && nCode <= IDC_IDRAW_END)                 // ��֧�ֿ⹦��
    {
        return _idraw_function(nCode, wParam, lParam);
    }
    else if (nCode >= IDC_ICONTROL_BEGIN && nCode <= IDC_ICONTROL_END)      // �滭���
    {
        return s_info->pfnControls(nCode, wParam, lParam);
    }
    else if (nCode >= IDC_ICONFIG_BEGIN && nCode <= IDC_ICONFIG_END)        // ���ò��
    {
        return s_info->pfnConfig(nCode, wParam, lParam);
    }
    else if (nCode >= IDC_ITHEME_BEGIN && nCode <= IDC_ITHEME_END)          // ������
    {
        return s_info->pfnTheme(nCode, wParam, lParam);
    }
    else if (nCode >= IDC_IEVENT_BEGIN && nCode <= IDC_IEVENT_END)          // �¼����
    {
        return s_info->pfnEvent(nCode, wParam, lParam);
    }
    else if (nCode >= IDC_IRESOURCE_BEGIN && nCode <= IDC_IRESOURCE_END)    // ��Դ���
    {
        return s_info->pfnResource(nCode, wParam, lParam);
    }
    else if (nCode >= IDC_IMOVE_BEGIN && nCode <= IDC_IMOVE_END)            // �ƶ����
    {
        return s_info->pfnMove(nCode, wParam, lParam);
    }
    else if (nCode >= IDC_IETOOLS_BEGIN && nCode <= IDC_IETOOLS_END)        // ���ֲ��
    {
        return s_info->pfnETools(nCode, wParam, lParam);
    }
    return s_info->pfnControls(nCode, wParam, lParam);
}

EXTERN_C _declspec(dllexport) int WINAPI iDraw_PluginFunction(int nCode, WPARAM wParam, LPARAM lParam)
{
    return iDraw_IDCall(nCode, wParam, lParam);
}

#endif