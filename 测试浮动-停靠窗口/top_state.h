#pragma once
#include <tstr.h>
#include <control/WndBase.h>
#include <assert.h>
#include <vector>
#include <map>
#include <d2d/d2d.h>
#include <IDraw_Header.h>

#define GS_VERTICAL     0x0100  // ѡ������
#define GS_BOTTOM       0x0200  // ѡ��ڵײ�
#define GS_RIGHT        0x0200  // ѡ��ڵײ�
#define GS_ICONRIGHT    0x0400  // ͼ�����ұ�
#define GS_NOGROP       0x1000  // ���Ƿ���, ����Ҫ�滭ѡ���
#define GS_FIXEDWIDTH   0x4000  // ѡ���ȹ̶�


#define __isshow(_info)             __query_flags(_info, TOP_STATE_ISSHOW)
#define __iswidth(_info)            __query_flags(_info, TOP_STATE_ISTYPE_WIDTH)
#define __iscalclayout(_info)       __query_flags(_info, TOP_STATE_ISCALCLAYOUT)
#define __isautohide(_info)         __query_flags(_info, TOP_STATE_BUTTON_ISAOTUHIDE)

#define __is_reserve_left(_info)    __query_flags(_info, TOP_STATE_RESERVE_LEFT)
#define __is_reserve_top(_info)     __query_flags(_info, TOP_STATE_RESERVE_TOP)
#define __is_reserve_right(_info)   __query_flags(_info, TOP_STATE_RESERVE_RIGHT)
#define __is_reserve_bottom(_info)  __query_flags(_info, TOP_STATE_RESERVE_BOTTOM)

#define __add_isshow(_info)         __add_flags(_info, TOP_STATE_ISSHOW)
#define __add_iswidth(_info)        __add_flags(_info, TOP_STATE_ISTYPE_WIDTH)
#define __add_iscalclayout(_info)   __add_flags(_info, TOP_STATE_ISCALCLAYOUT)
#define __add_isautohide(_info)     __add_flags(_info, TOP_STATE_BUTTON_ISAOTUHIDE)

#define __del_isshow(_info)         __del_flags(_info, TOP_STATE_ISSHOW)
#define __del_iswidth(_info)        __del_flags(_info, TOP_STATE_ISTYPE_WIDTH)
#define __del_iscalclayout(_info)   __del_flags(_info, TOP_STATE_ISCALCLAYOUT)
#define __del_isautohide(_info)     __del_flags(_info, TOP_STATE_BUTTON_ISAOTUHIDE)

#define TCM_INSERT_TOPSTATE     (TCM_FIRST + 100)   // wParam = 1=���,0=ɾ��,-1=��ѯ, lParam = Ҫ���/ɾ��/��ѯ��ֵ, �����Ƿ�ɹ�

// ��������
HWND CreateGroup(DWORD dwExStyle, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR id, LPVOID lpParam = 0);
LRESULT CALLBACK Group_Cell_OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);


// ����һ���ַ����������ڶ����ַ���, ����ڶ����ַ����������, ���������ڴ���, ���صڶ����ַ����ĵ�ַ
// ��ʹ��ʱ��Ҫ���� _free_str �����ͷ�
inline wchar_t* _copy_str(const wchar_t* str, wchar_t*& freeStr)
{
    if (!str)
    {
        if (freeStr)
            *freeStr = 0;
        return 0;
    }
    if (str == freeStr)return freeStr;

    size_t len1 = wcslen(str) + 1;
    size_t len2 = 0;
    if (freeStr)
        len2 = wcslen(freeStr) + 1;

    if (freeStr && len1 == len2 && wcscmp(str, freeStr) == 0)
        return 0;

    if (len2 < len1)	// ���ͷŵ��ı�������������
    {
        if (freeStr)
            delete[] freeStr;
        freeStr = new wchar_t[len1];
    }
    if (!freeStr)
        return 0;

    memcpy(freeStr, str, len1 * sizeof(wchar_t));
    return freeStr;
}

// �ͷ���_copy_str���ص��ַ���
inline void _free_str(wchar_t*& str)
{
    if (str)
        delete[] str;
    str = 0;
}


struct TAB_ITEMINFO
{
    LPWSTR  name;       // ��ǰ�Ӽ�����, ɾ����Ա��Ҫ�ͷ�
    int     width;      // ��ǰ�Ӽп��
    LPARAM  lParam;     // ��Ŀ��ֵ
    int     iImage;     // ͼ������
    int     state;      // ��ǰ�Ӽ�״̬

    // ��������, �ڲ�������ռ����µ��ַ���
    inline LPCWSTR SetName(LPCWSTR str) { return _copy_str(str, name); }

    // �ͷ�����, ���ͷſռ�
    inline void FreeName() { _free_str(name); }
};
typedef std::vector<TAB_ITEMINFO>   TAB_ITEMINFO_ARR;
typedef std::vector<TAB_ITEMINFO>* LPTAB_ITEMINFO_ARR;




typedef struct TAB_INFO_TOPSTATE
{
    DWORD size;                 // �̶���ߵĿ�ȸ߶�, ��λ=���, ��λ=�߶�
    int minWidth;               // ��ؼ��������С���
    DWORD padding;              // ÿ��ѡ������ռ�, ��λ = �����, ��λ = ���߶�
    TAB_DRAWINFO draw;          // �滭��Ҫ�Ķ���
    LPOBJSTRUCT pData;          // ��������
    LPTAB_ITEMINFO_ARR arr;     // �Ӽе���������ָ��, ��������¼�����Ӽе���Ϣ
    LPTOPSTATE_INFO info;       // ״̬��Ϣ
}*LPTAB_INFO_TOPSTATE;
// ��ȡ��ѡ��а󶨵�����, ѡ���������Ŀ���ݶ�������
// ���û��������ݾͻᴴ��һ��, �ڲ���һ�������¼
inline LPTAB_INFO_TOPSTATE _get_tabinfo(LPOBJSTRUCT pData)
{
    LPTAB_INFO_TOPSTATE info = (LPTAB_INFO_TOPSTATE)pData->param;
    if (!info)
    {
        info = new TAB_INFO_TOPSTATE;
        memset(info, 0, sizeof(TAB_INFO_TOPSTATE));
        info->minWidth = -1;
        info->pData = pData;
        info->arr = new TAB_ITEMINFO_ARR;
        info->arr->reserve(20); // Ԥ�ȷ���20����Ա���ڴ�
        info->info = new TOPSTATE_INFO;
        memset(info->info, 0, sizeof(TOPSTATE_INFO));
        info->info->flags = TOP_STATE_BUTTON_CONTROL | TOP_STATE_BUTTON_CONTROL;
        pData->param = info;
    }
    return info;

}
inline void _free_tabinfo(LPOBJSTRUCT pData)
{
    LPTAB_INFO_TOPSTATE info = (LPTAB_INFO_TOPSTATE)pData->param;
    if (info)
    {
        delete info->arr;
        delete info->info;
        delete info;
        pData->param = 0;
    }
    return;
}

// �����Ӽл滭����
inline int Tab_CalcTop(LPOBJSTRUCT pData)
{
    LPTAB_INFO_TOPSTATE info = (LPTAB_INFO_TOPSTATE)pData->param;
    int top = pData->pos.height - pData->draw.nItemHeight - HIWORD(info->padding);
    return top;
}

inline LPTOPSTATE_INFO _drawtab_gettopstate_info(LPOBJSTRUCT pData)
{
    LPTAB_INFO_TOPSTATE info = (LPTAB_INFO_TOPSTATE)pData->param;
    return info->info;
}

// �������Ĵ��ڼ��뵽�б���, �����ڱ�������ʱ����ж��ǲ�������б���Ĵ���
// ������б���Ĵ���, ����Ҫ����
void Tab_InsertHwnd(HWND hWnd);
LPOBJSTRUCT GetDataFromHwnd(HWND hWnd);

// ���µ�ǰ����Ĵ���, ���ػ������������ں�֮ǰ����Ĵ���
void TopState_UpdateStateWindow(HWND hWnd);
HWND TopState_GetActivateWindow();
