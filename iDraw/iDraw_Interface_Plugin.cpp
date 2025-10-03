#if 1
#include "EWindow_Fne_Main.h"



//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ö�ٲ���б�
// 
// ����:
//      ����1: ���ղ���������ָ��, �����д�������ַ��, Ϊ0�򷵻���Ҫ�ĳ�Ա��, ÿ����Ա4�ֽ�
//      ����2: ��������Ա��, һ����Առ��4���ֽ�
// 
// ����ֵ:
//      ����д�뻺�����ĳ�Ա��, �����һ������Ϊ0�򷵻������б������
EXTERN_C _declspec(dllexport) int WINAPI iDraw_Plugin_Enum(IDRAW_HPLUGIN* pArr, int bufCount)
{
    int count = (int)s_plugin_array.size();
    if (!pArr) return count;

    int i = 0;
    for (i = 0; i < count; i++)
    {
        if (i + 1 >= bufCount)
            break;
        
        pArr[i] = (IDRAW_HPLUGIN)s_plugin_array[i];;
    }
    return i;
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ�����Ϣ
// 
// ����:
//      ����1: ������, ͨ�� iDraw_Plugin_Enum() ��ȡ���Ĳ���������
//      ����2: ��ȡ��ֵ
// 
// ����ֵ:
//      ����д�뻺�����ĳ�Ա��, �����һ������Ϊ0�򷵻������б������
EXTERN_C _declspec(dllexport) int WINAPI iDraw_Plugin_GetInfo(IDRAW_HPLUGIN hPlugin)
{
    bool isReturn = true;   // �����жϴ���Ĳ������Ƿ��ǺϷ���ַ
    for (auto it = s_plugin_array.begin(); it != s_plugin_array.end(); ++it)
    {
        LPPLUGIN_INFO info = *it;
        if ((IDRAW_HPLUGIN)info == hPlugin)
        {
            isReturn = false;
            break;
        }
    }
    if (isReturn) return 0;

    return 0;
}


#endif