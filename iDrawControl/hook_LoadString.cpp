#include "EWindow_Fne_Header.h"
#include <hook_detours/apiHook.h>
#include "EWindow.h"
#include <hook_detours/apiHook.h>
#include "EMenu.h"



static apiHook hook_LoadStringA;
static apiHook hook_LoadStringW;
static BOOL(WINAPI* LoadStringA_hook)(HINSTANCE hInstance, UINT uID, LPSTR lpBuffer, int cchBufferMax) = LoadStringA;
static BOOL(WINAPI* LoadStringW_hook)(HINSTANCE hInstance, UINT uID, LPWSTR lpBuffer, int cchBufferMax) = LoadStringW;
static std::unordered_set<PFN_LoadStringCallback> m_set;

// ���ز���Ĭ�ϵ��ַ���, ID�Ǿ��������, �ǲ˵����ID
int LoadString_Other(HINSTANCE hInstance, UINT uID, LPWSTR lpBuffer, int cchBufferMax)
{

    switch (uID)
    {
    case ID_MENU_CLOSE:   // �ر�
        wcscpy_s(lpBuffer, cchBufferMax, L"�رյ�ǰѡ�еĴ���\n�رմ���");
        break;
    case ID_MENU_CLOSEOTHER:   // �ر�����
        wcscpy_s(lpBuffer, cchBufferMax, L"�رշǵ�ǰ����\n�ر�����");
        break;
    case ID_MENU_CLOSEALL:   // �ر�ȫ��
        wcscpy_s(lpBuffer, cchBufferMax, L"�ر�ȫ������\n�ر�ȫ��");
        break;
    case ID_MENU_CLOSE_LEFT:   // �ر����
        wcscpy_s(lpBuffer, cchBufferMax, L"�ر��������\n�ر����");
        break;
    case ID_MENU_CLOSE_RIGHT:   // �ر��ұ�
        wcscpy_s(lpBuffer, cchBufferMax, L"�ر��ұ�����\n�ر��ұ�");
        break;
    case ID_MENU_CLOSE_TOP:   // ������ʾ
        wcscpy_s(lpBuffer, cchBufferMax, L"�����л�ѡ����ڶ�����ʾ\n������ʾ");
        break;
    case ID_MENU_CLOSE_BOTTOM:   // �ײ���ʾ
        wcscpy_s(lpBuffer, cchBufferMax, L"�����л�ѡ����ڵײ���ʾ\n�ײ���ʾ");
        break;
    case IDMENU_LOCATIONTOOLBAR:        // ��ʾ/���ض�λ������
        return LoadStringW_hook(hInstance, 135, lpBuffer, cchBufferMax);    // ����û�д���Ķ�����ԭ��������
        break;


    default:
        return 0;
    }
    return (int)wcslen(lpBuffer);
}

static BOOL WINAPI MyLoadStringW_hook(HINSTANCE hInstance, UINT uID, LPWSTR lpBuffer, int cchBufferMax)
{
    int ret = 0;
    switch (uID)
    {
    case 32853:     // ����
        wcscpy_s(lpBuffer, cchBufferMax, L"�ڳ�����������λ�õ���ִ��һ�г�������˳����е������ӳ���ϵͳ������ٵ����ӳ�����ȥ��\n����(F8/F10)");
        break;
    case 32852:     // ��������
        wcscpy_s(lpBuffer, cchBufferMax, L"�ڳ�����������λ�õ���ִ��һ�г�������˳����е������ӳ�������ٽ����ӳ���\n��������(Shift+F8/F11)");
        break;
    case 32867:     // ���ٷ���
        wcscpy_s(lpBuffer, cchBufferMax, L"���ϼ��ӳ�����������ӳ���������жϡ�\n������ǰ�ӳ���(Ctrl+Shift+F8/Shift+F11)");
        break;
    case 57600:     // �½�Դ��
        wcscpy_s(lpBuffer, cchBufferMax, L"�½�Դ��\n�����³���(Shift+Ins)");
        break;
    case 32934:     // ������Ѱ
        wcscpy_s(lpBuffer, cchBufferMax, L"�ڳ�����Ѱ��ָ���ı����г������ҵ�����Ŀ\n������Ѱ(Ctrl+Shift+F)");
        break;
    case 32854:     // ����
        wcscpy_s(lpBuffer, cchBufferMax, L"���������׳���ĵ��԰汾.EXE�ļ�����������\n����(F5)");
        break;
    case 32860:     // ֹͣ
        wcscpy_s(lpBuffer, cchBufferMax, L"��ֹ�����׳��������\nֹͣ����(Shift+F5)");
        break;
    case 32905:     // ����
        wcscpy_s(lpBuffer, cchBufferMax, L"���ص���ǰ��תʱ�ĳ���λ��\n������ǰλ��(Ctrl+J/Ctrl+Shift+Backspace)"); //(Ctrl+Shift+Backspace)
        break;
    case 32906:     // ǰ��
        wcscpy_s(lpBuffer, cchBufferMax, L"��ת����ǰ����������ŵĶ���λ��\n��ת�����崦(Ctrl+B/Ctrl+����/Ctrl+Shift+Alt+Backspace)"); //(Ctrl+Shift+Alt+Backspace)
        break;
    case 32917:     // "&C.����\tF7", 32851
        wcscpy_s(lpBuffer, cchBufferMax, L"���������׳�������շ����汾��������Ӧ��.EXE��ִ�г����ļ�\n����(F7)");
        break;
    case 32894:     // "&S.��̬����\tShift+F7", 32912
        wcscpy_s(lpBuffer, cchBufferMax, L"�������̬��ִ��EXE�ļ������ļ��������κ�������ϵͳ�ļ���������δ��װ������ϵͳ�ĵ��������С�\n��̬����(Shift+F7)");
        break;
    case 32884: // "&B.���¾�̬����", 32912
        wcscpy_s(lpBuffer, cchBufferMax, L"���¾�̬����\n���¾�̬����");
        break;
    case 32882:      // "&D.��������", 32911
        wcscpy_s(lpBuffer, cchBufferMax, L"�����������ִ��EXE�ļ������ļ��������κ�������ϵͳ�ļ���������δ��װ������ϵͳ�ĵ��������С�\n��������");
        break;
    case 32915:         // ��������
        wcscpy_s(lpBuffer, cchBufferMax, L"ֹͣ��ǰ���Եĳ������±�������\n��������(Ctrl+Shift+F5)");
        break;
    case 32914:    // ��׼������ -> ��һ����ǩ
        wcscpy_s(lpBuffer, cchBufferMax, L"��ת������������ǩλ��\n��һ����ǩ");
        break;
    case 32913:    // ��׼������ -> ��һ����ǩ
        wcscpy_s(lpBuffer, cchBufferMax, L"��ת��ǰ��������ǩλ��\n��һ����ǩ");
        break;
    case 32908:     // ��׼������ -> ������ǩ
        wcscpy_s(lpBuffer, cchBufferMax, L"���û�ȡ����ǰλ�ô�����ǩ��־\n������ǩ");
        break;
    case 32897:     // ��׼������ -> ɾ����ǩ
        wcscpy_s(lpBuffer, cchBufferMax, L"ɾ����ǰλ�ô�����ǩ��־\nɾ����ǩ");
        break;
    case 32892:       // ��׼������ -> ע��
        wcscpy_s(lpBuffer, cchBufferMax, L"���ε�ǰ��ѡ�еĴ����\nע��");
        break;
    case 32898:     // ��׼������ -> ȡ��ע�� 
        wcscpy_s(lpBuffer, cchBufferMax, L"������ε�ǰ��ѡ�еĴ����\nȡ��ע��");
        break;
    default:
    {
        UINT lowID = uID;
        if (s_info->menuSelHmenu)
        {
            uID = s_info->menuSelId;
            int retLen = LoadString_Other(hInstance, uID, lpBuffer, cchBufferMax);
            if (retLen > 0)
                return retLen;

            LPCWSTR text = _menu_istool_(uID);
            if (text)
            {
                ret = wcslen(text);
                if (ret >= cchBufferMax)
                    ret = cchBufferMax - 1;
                wmemcpy(lpBuffer, text, ret);
                lpBuffer[ret] = 0;
                break;
            }

            if (uID >= IDMENU_SWITCH_THEME && uID < IDMENU_SWITCH_THEME_END)
                return ThemeMenu_LoadString(uID, lpBuffer, cchBufferMax);

        }
        //wstr::dbg(L"�����ַ���, ��ȡ���Ĳ˵����� = %d, ID = 0x%X(0x%04X), ��� = 0x%p\n", s_info->menuSelIndex, s_info->menuSelId, uID, s_info->menuSelHmenu);

        for (auto& item : m_set)
        {
            if (item)
            {
                int retLen = item(uID, lpBuffer, cchBufferMax);
                if (retLen > 0)
                    return retLen;
            }
        }

        return LoadStringW_hook(hInstance, lowID, lpBuffer, cchBufferMax);    // ����û�д���Ķ�����ԭ��������
    }
    }
    if (ret == 0)
        ret = (int)wcslen(lpBuffer);
    return ret;
}
static BOOL WINAPI MyLoadStringA_hook(HINSTANCE hInstance, UINT uID, LPSTR lpBuffer, int cchBufferMax)
{
    wstr text(cchBufferMax);
    int len = MyLoadStringW_hook(hInstance, uID, text.data(), cchBufferMax);
    text.resize(len);
    _str a = text.w2a();
    int ret = (int)a.size();
    if (ret >= cchBufferMax)
        ret = cchBufferMax - 1;

    memcpy(lpBuffer, a.c_str(), ret);
    lpBuffer[ret] = 0;
    return (int)strlen(lpBuffer);
}

// ��ʼ��hook�����ַ����ĺ���
void HookLoadStringInit()
{
    hook_LoadStringA.hook(&(PVOID&)LoadStringA_hook, MyLoadStringA_hook);
    hook_LoadStringW.hook(&(PVOID&)LoadStringW_hook, MyLoadStringW_hook);
}

void HookLoadString_PushCallback(PFN_LoadStringCallback pfn)
{
    m_set.insert(pfn);
}
