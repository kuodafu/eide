#pragma once
#include <tstr.h>
#include <control/WndBase.h>
#include <assert.h>
#include <vector>
#include <map>
#include <d2d/d2d.h>
#include <IDraw_Header.h>

#define GS_VERTICAL     0x0100  // 选项卡在左边
#define GS_BOTTOM       0x0200  // 选项卡在底部
#define GS_RIGHT        0x0200  // 选项卡在底部
#define GS_ICONRIGHT    0x0400  // 图标在右边
#define GS_NOGROP       0x1000  // 不是分组, 不需要绘画选择夹
#define GS_FIXEDWIDTH   0x4000  // 选项卡宽度固定


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

#define TCM_INSERT_TOPSTATE     (TCM_FIRST + 100)   // wParam = 1=添加,0=删除,-1=查询, lParam = 要添加/删除/查询的值, 返回是否成功

// 创建分组
HWND CreateGroup(DWORD dwExStyle, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR id, LPVOID lpParam = 0);
LRESULT CALLBACK Group_Cell_OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);


// 将第一个字符串拷贝到第二个字符串, 如果第二个字符串不够存放, 会新申请内存存放, 返回第二个字符串的地址
// 不使用时需要调用 _free_str 进行释放
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

    if (len2 < len1)	// 被释放的文本缓冲区不够大
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

// 释放由_copy_str返回的字符串
inline void _free_str(wchar_t*& str)
{
    if (str)
        delete[] str;
    str = 0;
}


struct TAB_ITEMINFO
{
    LPWSTR  name;       // 当前子夹名字, 删除成员需要释放
    int     width;      // 当前子夹宽度
    LPARAM  lParam;     // 项目数值
    int     iImage;     // 图标索引
    int     state;      // 当前子夹状态

    // 设置名字, 内部会申请空间存放新的字符串
    inline LPCWSTR SetName(LPCWSTR str) { return _copy_str(str, name); }

    // 释放名字, 会释放空间
    inline void FreeName() { _free_str(name); }
};
typedef std::vector<TAB_ITEMINFO>   TAB_ITEMINFO_ARR;
typedef std::vector<TAB_ITEMINFO>* LPTAB_ITEMINFO_ARR;




typedef struct TAB_INFO_TOPSTATE
{
    DWORD size;                 // 固定宽高的宽度高度, 低位=宽度, 高位=高度
    int minWidth;               // 项卡控件中项的最小宽度
    DWORD padding;              // 每个选项卡的填充空间, 低位 = 填充宽度, 高位 = 填充高度
    TAB_DRAWINFO draw;          // 绘画需要的对象
    LPOBJSTRUCT pData;          // 窗口数据
    LPTAB_ITEMINFO_ARR arr;     // 子夹的数据数组指针, 这个数组记录所有子夹的信息
    LPTOPSTATE_INFO info;       // 状态信息
}*LPTAB_INFO_TOPSTATE;
// 获取与选择夹绑定的数据, 选择夹所有项目数据都在这里
// 如果没有这个数据就会创建一个, 内部有一个数组记录
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
        info->arr->reserve(20); // 预先分配20个成员的内存
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

// 计算子夹绘画顶边
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

// 将创建的窗口加入到列表中, 当窗口被单击的时候会判断是不是这个列表里的窗口
// 如果是列表里的窗口, 就需要激活
void Tab_InsertHwnd(HWND hWnd);
LPOBJSTRUCT GetDataFromHwnd(HWND hWnd);

// 更新当前激活的窗口, 会重画激活的这个窗口和之前激活的窗口
void TopState_UpdateStateWindow(HWND hWnd);
HWND TopState_GetActivateWindow();
