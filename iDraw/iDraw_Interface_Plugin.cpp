#if 1
#include "EWindow_Fne_Main.h"



//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      枚举插件列表
// 
// 参数:
//      参数1: 接收插件句柄数组指针, 结果会写入这个地址里, 为0则返回需要的成员数, 每个成员4字节
//      参数2: 缓冲区成员数, 一个成员占用4个字节
// 
// 返回值:
//      返回写入缓冲区的成员数, 如果第一个参数为0则返回所有列表的数量
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
// 函数功能: 
//      获取插件信息
// 
// 参数:
//      参数1: 插件句柄, 通过 iDraw_Plugin_Enum() 获取到的插件句柄数组
//      参数2: 获取的值
// 
// 返回值:
//      返回写入缓冲区的成员数, 如果第一个参数为0则返回所有列表的数量
EXTERN_C _declspec(dllexport) int WINAPI iDraw_Plugin_GetInfo(IDRAW_HPLUGIN hPlugin)
{
    bool isReturn = true;   // 用于判断传入的插件句柄是否是合法地址
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