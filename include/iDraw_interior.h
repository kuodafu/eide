/*
*  这个文件是内部调用对外公开的接口
*  内部插件/ 内部调用的, 应该使用这个支持库, 对外公开的没有数据结构
*/

#pragma once
#include "IDraw_Header.h"
#define IDRAW_CALL __stdcall



// 设置dll的路径, 这个是易语言支持库目录下的 iDraw.fne 
inline const wchar_t* iDrawSetDllName(const wchar_t* dllFileName)
{
    static wchar_t fileName[260] = L"iDraw.fne";
    if (!dllFileName || !dllFileName[0]) return fileName;
    size_t len = wcslen(dllFileName);
    if (len >= 259)len = 259;
    memcpy(fileName, dllFileName, len * sizeof(wchar_t));
    fileName[len] = 0;
    return fileName;
}

// 获取dll路径
inline const wchar_t* iDrawGetDllName()
{
    return iDrawSetDllName(0);
}

inline void* iDrawGetAddress(const char* funName)
{
    static HMODULE hModule = 0;
    if (!hModule)
        hModule = LoadLibraryW(iDrawGetDllName());
    
    if (!hModule)throw; // 加载失败直接抛出异常
    return GetProcAddress(hModule, funName);
}

// 内部头文件不需要定义D2D类接口

#define IDRAW_GET_PTR_ITERATOR(_name) \
    static __FN_##_name__ _PFN_CALLBACK_ = 0;\
    if(!_PFN_CALLBACK_) _PFN_CALLBACK_ = (__FN_##_name__)iDrawGetAddress(#_name);\
    if (!_PFN_CALLBACK_) {\
        MessageBoxA(((HWND)0), "未能找到\""## #_name ##"\"接口, 程序无法正常启动", #_name ## "未找到", 0);\
        exit(0);\
    }

#define IDRAW_FUNC0(_ret, _name) \
        inline _ret IDRAW_CALL _name(){\
        typedef _ret(IDRAW_CALL* __FN_##_name__)();\
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_();}

#define IDRAW_FUNC1(_ret, _name, _1, _2) \
        inline _ret IDRAW_CALL _name(_1 _2){\
        typedef _ret(IDRAW_CALL* __FN_##_name__)(_1);\
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2);}


#define IDRAW_FUNC2(_ret, _name, _1, _2, _3, _4) \
        inline _ret IDRAW_CALL _name(_1 _2, _3 _4){\
        typedef _ret(IDRAW_CALL * __FN_##_name__)(_1, _3); \
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4);}

#define IDRAW_FUNC3(_ret, _name, _1, _2, _3, _4, _5, _6) \
        inline _ret IDRAW_CALL _name(_1 _2, _3 _4, _5 _6){\
        typedef _ret(IDRAW_CALL * __FN_##_name__)(_1, _3, _5); \
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6);}

#define IDRAW_FUNC4(_ret, _name, _1, _2, _3, _4, _5, _6, _7, _8) \
        inline _ret IDRAW_CALL _name(_1 _2, _3 _4, _5 _6, _7 _8){\
        typedef _ret(IDRAW_CALL * __FN_##_name__)(_1, _3, _5, _7); \
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6, _8);}

#define IDRAW_FUNC5(_ret, _name, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) \
        inline _ret IDRAW_CALL _name(_1 _2, _3 _4, _5 _6, _7 _8, _9 _10){\
        typedef _ret(IDRAW_CALL * __FN_##_name__)(_1, _3, _5, _7, _9); \
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6, _8, _10);}

#define IDRAW_FUNC6(_ret, _name, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) \
        inline _ret IDRAW_CALL _name(_1 _2, _3 _4, _5 _6, _7 _8, _9 _10, _11 _12){\
        typedef _ret(IDRAW_CALL * __FN_##_name__)(_1, _3, _5, _7, _9, _11); \
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6, _8, _10, _12);}

#define IDRAW_FUNC7(_ret, _name, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) \
        inline _ret IDRAW_CALL _name(_1 _2, _3 _4, _5 _6, _7 _8, _9 _10, _11 _12, _13 _14){\
        typedef _ret(IDRAW_CALL * __FN_##_name__)(_1, _3, _5, _7, _9, _11, _13); \
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6, _8, _10, _12, _14);}

#define IDRAW_FUNC8(_ret, _name, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16) \
        inline _ret IDRAW_CALL _name(_1 _2, _3 _4, _5 _6, _7 _8, _9 _10, _11 _12, _13 _14, _15 _16){\
        typedef _ret(IDRAW_CALL * __FN_##_name__)(_1, _3, _5, _7, _9, _11, _13, _15); \
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6, _8, _10, _12, _14, _16);}


// API接口开始

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      调用视觉库提供的功能
// 
// 参数:
//      参数1: 功能编号, IDC_ 开头常量
//      参数2: 根据功能传递参数
//      参数3: 根据功能传递参数
// 
// 返回值:
//      参考 IDC_ 开头常量的说明
IDRAW_FUNC3(int, iDraw_IDCall, int, nCode, WPARAM, wParam, LPARAM, lParam)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      调用视觉库提供的功能
// 
// 参数:
//      参数1: 功能编号
//      参数2: 根据功能传递参数
//      参数3: 根据功能传递参数
// 
// 返回值:
//      获取到的值
IDRAW_FUNC3(int, iDraw_PluginFunction, int, nCode, WPARAM, wParam, LPARAM, lParam)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取当前支持库的版本号
// 
// 参数:
//      参数1: 接收主版本号指针, 可以为0
//      参数2: 接收次版本号指针, 可以为0
//      参数3: 接收编译版本号指针, 可以为0
// 
// 返回值:
//      返回版本号unicode文本指针
IDRAW_FUNC3(LPCWSTR, iDraw_GetVersion, int*, pMajor, int*, pMinor, int*, pBuild)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取当前支持库的版本号
// 
// 参数:
//      参数1: 接收主版本号指针, 可以为0
//      参数2: 接收次版本号指针, 可以为0
//      参数3: 接收编译版本号指针, 可以为0
// 
// 返回值:
//      返回版本号文本指针
IDRAW_FUNC3(LPCSTR, iDraw_GetVersionA, int*, pMajor, int*, pMinor, int*, pBuild)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取当前易语言的版本号
// 
// 参数:
//      无参数
// 
// 返回值:
//      获取到的值, 5.93 返回593, 5.11 返回511, 5.92有两个版本, 会返回592和5922
IDRAW_FUNC0(int, iDraw_GetEVersion)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取从支持库里申请内存, 内存是从 iDraw.fne 这个模块里申请的, 内部调用的是 malloc
// 
// 参数:
//      参数1: 申请的尺寸
// 
// 返回值:
//      申请的内存地址, 出错返回0, 申请失败不会抛出异常
IDRAW_FUNC1(void*, iDraw_malloc, int, size)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      释放 iDraw_malloc 申请的内存, 这里释放的是 iDraw.fne 这个模块的内存, 内部调用的是 free
//      如果由其他模块去释放 iDraw_malloc 申请的内存, 一般会有一个堆不相同的异常
// 
// 参数:
//      参数1: iDraw_malloc 返回的地址
// 
// 返回值:
//      无返回值
IDRAW_FUNC1(void, iDraw_free, void*, ptr)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取xml指定节点, 返回节点指针, 所有读写操作都需要节点指针
// 
// 参数:
//      参数1: xml节点名
//      参数2: 节点值文本, 可以为0
//      参数3: 节点不存在时是否创建该节点, 可以为0
//      参数4: 传递一个指针接收返回的节点是否是新创建的, 可以为0
// 
// 返回值:
//      获取到的值
IDRAW_FUNC4(ICTinyXml2_XMLNode, iDraw_Xml_GetNode, LPCWSTR, lpszNodeName, LPCWSTR, lpszNodeValue, BOOL, isCreate, BOOL*, lpIsNewNode)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取xml指定节点下的子节点, 返回节点指针, 所有读写操作都需要节点指针
// 
// 参数:
//      参数1: xml节点, 如果这个值为0, 则功能与 iDraw_Xml_GetNode() 一样
//      参数2: 要获取的子节点节点名, 如果为空则调用失败
//      参数3: 节点值文本, 可以为0
//      参数4: 节点不存在时是否创建该节点, 可以为0
//      参数5: 传递一个指针接收返回的节点是否是新创建的, 可以为0
// 
// 返回值:
//      获取到的值
IDRAW_FUNC5(ICTinyXml2_XMLNode, iDraw_Xml_GetChildNode, ICTinyXml2_XMLNode, nodeParent, LPCWSTR, lpszNodeName, LPCWSTR, lpszNodeValue, BOOL, isCreate, BOOL*, lpIsNewNode)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      添加子节点到某个节点下, 如果被添加的节点已经存在, 则不会添加, 返回节点指针
// 
// 参数:
//      参数1: xml节点指针
//      参数2: 要插入的xml节点名
//      参数3: 要插入的xml节点值, 可以为0
// 
// 返回值:
//      获取到的值
IDRAW_FUNC3(ICTinyXml2_XMLNode, iDraw_Xml_InsertChildNode, ICTinyXml2_XMLNode, pNode, LPCWSTR, lpszNodeName, LPCWSTR, pszNodeValue)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      设置指定节点属性值
// 
// 参数:
//      参数1: xml节点名
//      参数2: 要设置的属性名
//      参数3: 要设置的属性值
// 
// 返回值:
//      获取到的值
IDRAW_FUNC3(bool, iDraw_Xml_SetAttrValue, ICTinyXml2_XMLNode, node, LPCWSTR, lpszAttrName, LPCWSTR, lpszAttrValue)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取指定节点属性值, 返回属性值文本指针
// 
// 参数:
//      参数1: xml节点名
//      参数2: 要获取的属性名
//      参数3: 要获取的这个属性是否存在, 可以为0, 如果属性不存在, 这个值为0, 存在则为1
// 
// 返回值:
//      获取到的值
IDRAW_FUNC3(LPCWSTR, iDraw_Xml_GetAttrValue, ICTinyXml2_XMLNode, node, LPCWSTR, lpszAttrName, BOOL*, isAttribute)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      设置指定节点值
// 
// 参数:
//      参数1: xml节点指针
//      参数2: 要设置的节点值
// 
// 返回值:
//      获取到的值
IDRAW_FUNC2(bool, iDraw_Xml_SetNodeValue, ICTinyXml2_XMLNode, node, LPCWSTR, lpszNodeValue)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      保存xml到磁盘
// 
// 参数:
//      无参数
// 
// 返回值:
//      获取到的值
IDRAW_FUNC0(bool, iDraw_Xml_SaveFile)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取指定节点值, 返回节点值文本指针
// 
// 参数:
//      参数1: xml节点指针
// 
// 返回值:
//      获取到的值
IDRAW_FUNC1(LPCWSTR, iDraw_Xml_GetNodeValue, ICTinyXml2_XMLNode, node)




//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化指定窗口, 并设置一些默认的绘画信息, 内部不会对其进行重画
//      支持库所有子类化都是通过这个接口进行二次封装
// 参数:
//      参数1: 需要子类化的窗口句柄
//      参数2: 接收子类化消息回调的函数, 有5个参数, 函数调用约定为 __stdcall, 返回值为 LRESULT 类型
//              前4个参数与普通子类化相同, 最后多的那个参数就是调用这个接口的返回值
//      参数3: 是否需要删除边框, 为true则把边框相关的样式都删除
//      参数4: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
// 返回值:
//      返回窗口数据, 可以调用 iDraw_SetLong() 对窗口数据进行修改
//      如果需要自己接收消息, 请调用 iDraw_SetLong(窗口数据, IGL_WNDPROC, 回调函数), 可以参考 IGL_WNDPROC 常量说明
IDRAW_FUNC4(LPOBJSTRUCT, iDraw_SubclassWindow, HWND, hWnd, pfn_SubClassWndProc, proc, bool, reMoveBorder, bool, isSetBack)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      调用原回调过程
// 参数:
//      参数1: 窗口句柄, 消息回调里的第一个参数
//      参数2: 消息值, 消息回调里的第二个参数
//      参数3: 参数1, 消息回调里的第三个参数
//      参数4: 参数2, 消息回调里的第四个参数
//      参数5: 组件数据, 消息回调里的第五个参数
// 返回值:
//      返回系统默认处理的返回值
IDRAW_FUNC5(LRESULT, iDraw_CallWindowProc, HWND, hWnd, UINT, message, WPARAM, wParam, LPARAM, lParam, LPOBJSTRUCT, pData)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画按钮, 按钮的类名为 Button
// 参数:
//      参数1: 按钮的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
// 返回值:
//      返回窗口数据, 可以调用 iDraw_SetLong() 对窗口数据进行修改
//      如果需要自己接收消息, 请调用 iDraw_SetLong(窗口数据, IGL_WNDPROC, 回调函数), 可以参考 IGL_WNDPROC 常量说明
IDRAW_FUNC4(LPOBJSTRUCT, iDraw_Subclass_Button, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画编辑框, 编辑框的类名为 Edit
// 参数:
//      参数1: 编辑框的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
//      参数5: 是否重画滚动条
// 返回值:
//      返回窗口数据, 可以调用 iDraw_SetLong() 对窗口数据进行修改
//      如果需要自己接收消息, 请调用 iDraw_SetLong(窗口数据, IGL_WNDPROC, 回调函数), 可以参考 IGL_WNDPROC 常量说明
IDRAW_FUNC5(LPOBJSTRUCT, iDraw_Subclass_Edit, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle, bool, isScroll)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画标签, 标签的类名为 static
// 参数:
//      参数1: 标签的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
// 返回值:
//      返回窗口数据, 可以调用 iDraw_SetLong() 对窗口数据进行修改
//      如果需要自己接收消息, 请调用 iDraw_SetLong(窗口数据, IGL_WNDPROC, 回调函数), 可以参考 IGL_WNDPROC 常量说明
IDRAW_FUNC4(LPOBJSTRUCT, iDraw_Subclass_Static, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画状态条, 状态条的类名为 msctls_statusbar32
// 参数:
//      参数1: 状态条的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
// 返回值:
//      返回窗口数据, 可以调用 iDraw_SetLong() 对窗口数据进行修改
//      如果需要自己接收消息, 请调用 iDraw_SetLong(窗口数据, IGL_WNDPROC, 回调函数), 可以参考 IGL_WNDPROC 常量说明
IDRAW_FUNC4(LPOBJSTRUCT, iDraw_Subclass_StatusBar, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画树型框, 树型框的类名为 SysTreeView32
// 参数:
//      参数1: 树型框的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
//      参数5: 树型框的项目高度
//      参数6: 是否重画树型框的滚动条
//      参数7: 是否有热点效果
// 返回值:
//      返回窗口数据, 可以调用 iDraw_SetLong() 对窗口数据进行修改
//      如果需要自己接收消息, 请调用 iDraw_SetLong(窗口数据, IGL_WNDPROC, 回调函数), 可以参考 IGL_WNDPROC 常量说明
IDRAW_FUNC7(LPOBJSTRUCT, iDraw_Subclass_TreeView, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle, int, nItemHeight, bool, isScroll, bool, isHot)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画列表框, 列表框的类名为 ListBox
// 参数:
//      参数1: 列表框的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
//      参数5: 列表框的项目高度
//      参数6: 是否重画滚动条
//      参数7: 是否有热点效果
// 返回值:
//      返回窗口数据, 可以调用 iDraw_SetLong() 对窗口数据进行修改
//      如果需要自己接收消息, 请调用 iDraw_SetLong(窗口数据, IGL_WNDPROC, 回调函数), 可以参考 IGL_WNDPROC 常量说明
IDRAW_FUNC7(LPOBJSTRUCT, iDraw_Subclass_ListBox, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle, int, nItemHeight, bool, isScroll, bool, isHot)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画超级列表框, 超级列表框的类名为 SysListView32
// 参数:
//      参数1: 超级列表框的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
//      参数5: 超级列表框的项目高度
//      参数6: 是否重画滚动条
//      参数7: 是否有热点效果
//      参数8: 列表对齐方式
// 返回值:
//      返回窗口数据, 可以调用 iDraw_SetLong() 对窗口数据进行修改
//      如果需要自己接收消息, 请调用 iDraw_SetLong(窗口数据, IGL_WNDPROC, 回调函数), 可以参考 IGL_WNDPROC 常量说明
IDRAW_FUNC8(LPOBJSTRUCT, iDraw_Subclass_ListView, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle, int, nItemHeight, bool, isScroll, bool, isHot, int, fmt)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画工具条, 工具条的类名为 ToolbarWindow32
// 参数:
//      参数1: 工具条的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
// 返回值:
//      返回窗口数据, 可以调用 iDraw_SetLong() 对窗口数据进行修改
//      如果需要自己接收消息, 请调用 iDraw_SetLong(窗口数据, IGL_WNDPROC, 回调函数), 可以参考 IGL_WNDPROC 常量说明
IDRAW_FUNC4(LPOBJSTRUCT, iDraw_Subclass_Tool, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle)


// 函数功能: 
//      子类化并重画选择夹, 选择夹的类名为 SysTabControl32
// 参数:
//      参数1: 选择夹窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
// 返回值:
//      返回窗口数据, 可以调用 iDraw_SetLong() 对窗口数据进行修改
//      如果需要自己接收消息, 请调用 iDraw_SetLong(窗口数据, IGL_WNDPROC, 回调函数), 可以参考 IGL_WNDPROC 常量说明
IDRAW_FUNC4(LPOBJSTRUCT, iDraw_Subclass_Tab, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画调节器, 调节器的类名为 msctls_updown32
// 参数:
//      参数1: 调节器的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
// 返回值:
//      返回窗口数据, 可以调用 iDraw_SetLong() 对窗口数据进行修改
//      如果需要自己接收消息, 请调用 iDraw_SetLong(窗口数据, IGL_WNDPROC, 回调函数), 可以参考 IGL_WNDPROC 常量说明
IDRAW_FUNC4(LPOBJSTRUCT, iDraw_Subclass_msctls_updown32, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画组合框, 组合框的类名为 ComboBox
// 参数:
//      参数1: 组合框的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
//      参数5: 组合框的列表表项高度
// 返回值:
//      返回窗口数据, 可以调用 iDraw_SetLong() 对窗口数据进行修改
//      如果需要自己接收消息, 请调用 iDraw_SetLong(窗口数据, IGL_WNDPROC, 回调函数), 可以参考 IGL_WNDPROC 常量说明
IDRAW_FUNC5(LPOBJSTRUCT, iDraw_Subclass_ComboBox, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle, int, nItemHeight)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      重画指定控件的滚动条, 这个绘画有点问题, 不是所有控件都支持
// 参数:
//      参数1: 需要重画滚动条的窗口句柄
//      参数2: 如果是超级列表框, 则这个参数是超级列表框的表项高度, 超列是特别处理
//      参数3: 如果是树型框, 则为真, 树型框会做特别处理
//      参数4: 是否调用原函数
// 返回值:
//      返回滚动条对象指针
IDRAW_FUNC4(LPVOID, iDraw_DrawScroll, HWND, hWnd, int, nListViewItemHeight, bool, isTreeView, bool, isCallSetScrollInfo)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      初始化绘画信息, 必须是在 WM_PAINT 消息下调用, 配套的函数是 iDraw_EndPaint()
//      会调用 BeginPaint, 会创建内存DC, 返回内存DC
//      把组件设置的字体颜色, 背景画刷, 边框画笔都选入DC
//      如果 不调用此接口, 也可以按一般处理WM_PAINT消息时处理
// 
// 参数:
//      参数1: 子类化的第五个参数, 或者是调用子类化接口的返回值
//      参数2: BEGIN_END_PAINT_STRUCT 结构指针, 内部会把初始化的数据记录到这个结构中
//      参数3: 是否为DX渲染, 如果是DX渲染, 则内部会调用DX的初始化函数
//      参数4: 是否填充背景, 如果为true, 则返回的HDC已经填充好了背景
// 
// 返回值:
//      返回内存DC, DX渲染则返回0
IDRAW_FUNC4(HDC, iDraw_BeginPaint, LPOBJSTRUCT, pData, BEGIN_END_PAINT_STRUCT*, data, bool, isDxRender, bool, isFillBack)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      提交绘画信息, 必须是在 WM_PAINT 消息下调用, 调用此函数前必须已经调用了 iDraw_BeginPaint()
//      把iDraw_BeginPaint() 时创建的内存DC拷贝到窗口上
//      并调用 EndPaint 提交绘画信息
// 
// 参数:
//      参数1: BEGIN_END_PAINT_STRUCT 结构指针, 此函数会清理该结构的数据
// 
// 返回值:
//      返回是否拷贝成功
IDRAW_FUNC1(bool, iDraw_EndPaint, BEGIN_END_PAINT_STRUCT*, data)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取DX绘画对象指针, 一般易语言用不上, 易语言使用 iDraw_ 开头的绘画相关函数
// 
// 参数:
//      无参数
// 
// 返回值:
//      返回dx绘画对象指针
IDRAW_FUNC0(ID2DDraw*, iDraw_GetDxObject)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取初始化时的参数
// 
// 参数:
//      参数1: 提供一个指针, 接收 传入 iDraw_Interface 接口的第一个参数
//      参数2: 提供一个指针, 接收 传入 iDraw_Interface 接口的第二个参数
//      参数3: 提供一个指针, 接收 传入 iDraw_Interface 接口的第三个参数
//      参数4: 提供一个指针, 接收 传入 iDraw_Interface 接口的第四个参数
// 
// 返回值:
//      返回值暂未使用
IDRAW_FUNC4(int, iDraw_InterfaceArgument, LPIDRAW_INFO*, pInfo, IGetEWindow**, eWnd, int*, reserved1, int*, reserved2)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      重画阴影窗口, 注意: 如果绑定了 IRE_DRAW_BCAKGROUND 这个事件, 调用后会触发这个事件, 请不要在这个事件下调用该接口
// 
// 参数:
//      参数1: 是否异步重画, 如果不需要马上绘画请设置为true, 为true则距离最后一次调用20毫秒后开始重画
// 
// 返回值:
//      返回阴影窗口句柄
IDRAW_FUNC1(HWND, iDraw_UpdateShadow, bool, bAsync)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      从文件加载ext, 成功返回资源句柄, 失败返回0
// 
// 参数:
//      参数1: ext的文件路径
// 
// 返回值:
//      返回ext文件的资源句柄, 可以使用 User_GetIcon/User_GetIconDx 这两个函数来获取图标
IDRAW_FUNC1(IDRAW_HRESOURCE, iDraw_LoadExtFromFile, LPCWSTR, pszFile)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      从内存加载ext, 成功返回资源句柄, 失败返回0
// 
// 参数:
//      参数1: ext的文件数据
//      参数2: 文数据尺寸
// 
// 返回值:
//      返回ext文件的资源句柄, 可以使用 User_GetIcon/User_GetIconDx 这两个函数来获取图标
IDRAW_FUNC2(IDRAW_HRESOURCE, iDraw_LoadExtFromMemory, const void*, ptr, int, size)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取默认组件的资源句柄, 传递窗口句柄获取
// 
// 参数:
//      参数1: 从哪个窗口上获取, 现在只有 标准工具条, 程序/支持库/组件箱 树型框这几个有默认图标
// 
// 返回值:
//      返回窗口关联的资源句柄
IDRAW_FUNC1(IDRAW_HRESOURCE, iDraw_GetDefControlIcon, HWND, hWnd)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      取消插件所有绑定窗口资源的回调
// 
// 参数:
//      参数1: 插件唯一标识符
// 
// 返回值:
//      取消绑定成功返回true, 失败返回false
IDRAW_FUNC1(bool, iDraw_UnBindWindow, LPCSTR, guid)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      将资源绑定到窗口上, 下次重画这个窗口时会从这个资源获取图标
// 
// 参数:
//      参数1: 插件唯一标识符
//      参数2: 要绑定的窗口句柄
//      参数3: 加载的资源句柄, 如果这个参数为0, 且最后一个参数为0, 则不会加载设定的图标
//      参数4: 回调函数, 当这个窗口需要加载图标时调用, 为0则使用默认的资源加载, 如果没有默认的资源则不加载
// 
// 返回值:
//      绑定成功返回true, 失败返回false
IDRAW_FUNC4(bool, iDraw_BindWindow, LPCSTR, guid, HWND, hWnd, IDRAW_HRESOURCE, hResource, pfn_GetWindowIcon, callback)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      从窗口句柄获取资源, 如果没有绑定过这个窗口, 则返回0
// 
// 参数:
//      参数1: 插件唯一标识符
//      参数2: 之前绑定的窗口句柄
//      参数3: 接收绑定时传递的回调函数, 可以为0
// 
// 返回值:
//      绑定成功返回true, 失败返回false
IDRAW_FUNC3(IDRAW_HRESOURCE, iDraw_GetResourceFromHwnd, LPCSTR, guid, HWND, hWnd, pfn_GetWindowIcon*, callback)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      从资源句柄里获取图标数据
// 
// 参数:
//      参数1: 资源句柄, 使用 iDraw_LoadExtFromFile/iDraw_LoadExtFromMemory/iDraw_GetDefControlIcon 返回的值
//      参数2: 图标ID
//      参数3: RECT_F结构指针, 接收这个ID在资源中的位置
//      参数4: 要获取的状态, 目前只支持 STATE::HOT, STATE::DOWN, STATE::BAN, STATE::NORMAL 这4个
//      参数5: 接收图标来源, 0=从ID中获取, 1=从默认图标里获取
// 
// 返回值:
//      返回图标数据, 如果没有图标则返回0
IDRAW_FUNC5(LPWINDOW_ICON_DATA, iDraw_GetIcon, IDRAW_HRESOURCE, hResource, LPCWSTR, pszID, RECT_F*, prc, int, state, UINT, dpi)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      从资源句柄里获取图标数据
// 
// 参数:
//      参数1: 资源句柄, 使用 iDraw_LoadExtFromFile/iDraw_LoadExtFromMemory/iDraw_GetDefControlIcon 返回的值
//      参数2: 图标ID
//      参数3: RECT_F结构指针, 接收这个ID在资源中的位置
//      参数4: 要获取的状态, 目前只支持 STATE::HOT, STATE::DOWN, STATE::BAN, STATE::NORMAL 这4个
//      参数5: 接收图标来源, 0=从ID中获取, 1=从默认图标里获取
// 
// 返回值:
//      返回图标数据, 如果没有图标则返回0
IDRAW_FUNC5(LPWINDOW_ICON_DATA, iDraw_GetIconDx, IDRAW_HRESOURCE, hResource, LPCWSTR, pszID, RECT_F*, prc, int, state, UINT, dpi)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      从图标数据里获取dx或者gdi+的图片指针, gdi+返回的是 Gdiplus::Bitmap* 指针, dx返回内部使用的图片指针
// 
// 参数:
//      参数1: 图标数据, iDraw_GetIcon 或者 iDraw_MakeResourceReturn 返回的值
//      参数2: 是否返回gdiplus位图句柄, 为true则返回Gdiplus位图句柄, false则返回dx位图句柄
// 
// 返回值:
//      返回位图句柄, 根据第二个参数返回
IDRAW_FUNC2(void*, iDraw_GetImageHandle, LPWINDOW_ICON_DATA, pIconData, bool, isGdiplus)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      初始化插件, 只有初始化后才能往扩展菜单里添加菜单项和绑定资源
// 
// 参数:
//      参数1: 插件唯一标识符
//      参数2: 插件的模块句柄
//      参数3: 插件的插件信息结构指针, 在取消初始化前必须保证这个指针的存活
//      参数4: 是否加入扩展菜单, 如果为false, 则调用 iDraw_MenuExt_Insert 会失败
//      参数5: 与视觉库通讯的回调函数
// 
// 返回值:
//      返回错误码, 0=成功
IDRAW_FUNC5(bool, iDraw_InitPlugin, LPCSTR, guid, HMODULE, hModule, PLUGIN_INFO*, etInfo, pfn_MenuExt_Command, pfnMenuExt_Command, pfn_IDraw_Callback, callback)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      取消初始化插件, 把之前初始化时加入到扩展菜单的菜单项删除
// 
// 参数:
//      参数1: 插件唯一标识符
// 
// 返回值:
//      返回错误码, 0=成功
IDRAW_FUNC1(bool, iDraw_UnInitPlugin, LPCSTR, guid)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      把菜单项插入到扩展菜单里, 菜单点击事件由 iDraw_InitPlugin 里的回调函数接收
// 
// 参数:
//      参数1: 插件唯一标识符
//      参数2: 加入的位置, -1则加入到最后
//      参数3: 标识
//      参数4: 菜单项ID, 如果uFlags包含MF_POPUP 则这个参数是弹出式的菜单句柄
//      参数5: 菜单名
//      参数6: 菜单项目数值
// 
// 返回值:
//      返回错误码, 0=成功
IDRAW_FUNC6(bool, iDraw_MenuExt_Insert, LPCSTR, guid, UINT, uPosition, UINT, uFlags, UINT_PTR, uIDNewItem, LPCWSTR, lpNewItem, LPARAM, param)





//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取子类化返回的指针的数据, 或者是子类化接收消息回调函数里的第五个参数
// 
// 参数:
//      参数1: 子类化返回的指针, 或者是子类化接收消息回调函数里的第五个参数
//      参数2: 数据索引, 具体请参考 IGL_ 开头常量
// 
// 返回值:
//      获取到的值
IDRAW_FUNC2(LONG_PTR, iDraw_GetLong, LPOBJSTRUCT, pData, int, index)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      设置子类化返回的指针的数据, 或者是子类化接收消息回调函数里的第五个参数, 返回设置前的数据
// 
// 参数:
//      参数1: 子类化返回的指针, 或者是子类化接收消息回调函数里的第五个参数
//      参数2: 数据索引, 具体请参考 IGL_ 开头常量
//      参数3: 要设置的数据
// 
// 返回值:
//      返回设置前的值
IDRAW_FUNC3(LONG_PTR, iDraw_SetLong, LPOBJSTRUCT, pData, int, index, LONG_PTR, value)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建画刷, 不允许手动释放, 程序在结束时会释放, 相同颜色的画刷只会创建一次
// 
// 参数:
//      参数1: 需要创建的画刷, RGB颜色
// 
// 返回值:
//      返回创建好的GDI画刷, 调用此函数创建的画刷不允许手动释放
IDRAW_FUNC1(HBRUSH, iDraw_CreateBrush, COLORREF, cr)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建画刷, 不允许手动释放, 程序在结束时会释放, 相同颜色的画刷只会创建一次
// 
// 参数:
//      参数1: 需要创建的画刷, ARGB颜色
// 
// 返回值:
//      返回创建好的画刷, 调用此函数创建的画刷不允许手动释放
IDRAW_FUNC1(LPEX_BRUSH, iDraw_CreateBrushDx, DWORD, cr)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建画笔, 不允许手动释放, 程序在结束时会释放, 相同颜色只会创建一次
// 
// 参数:
//      参数1: 需要创建的画笔, ARGB颜色
// 
// 返回值:
//      返回创建好的GDI画笔, 调用此函数创建的画笔不允许手动释放
IDRAW_FUNC3(HPEN, iDraw_CreatePen, int, style, int, width, COLORREF, cr)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建画笔, 不允许手动释放, 程序在结束时会释放, 相同颜色只会创建一次
// 
// 参数:
//      参数1: 需要创建的画笔, ARGB颜色
// 
// 返回值:
//      返回创建好的GDI画笔, 调用此函数创建的画笔不允许手动释放
IDRAW_FUNC3(LPEX_PEN, iDraw_CreatePenDx, int, style, float, width, DWORD, cr)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建字体, 不允许手动释放, 程序在结束时会释放, 相同颜色只会创建一次
// 
// 参数:
//      参数1: 字体名
//      参数2: 字体大小
//      参数3: 字体样式, 0=一般, 1=粗体, 2=斜体, 4=下划线, 8=删除线, 多个样式需要或起来
//
// 返回值:
//      返回创建好的GDI字体, 调用此函数创建的字体不允许手动释放
IDRAW_FUNC3(HFONT, iDraw_CreateFont, LPCWSTR, name, int, lfHeight, int, style)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建字体, 不允许手动释放, 程序在结束时会释放, 相同颜色只会创建一次
// 
// 参数:
//      参数1: LPLOGFONTW 结构指针
//
// 返回值:
//      返回创建好的GDI字体, 调用此函数创建的字体不允许手动释放
IDRAW_FUNC1(HFONT, iDraw_CreateFontLf, LPLOGFONTW, lf)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建字体, 不允许手动释放, 程序在结束时会释放, 相同颜色只会创建一次
// 
// 参数:
//      参数1: 字体名
//      参数2: 字体大小
//      参数3: 字体样式, 0=一般, 1=粗体, 2=斜体, 4=下划线, 8=删除线, 多个样式需要或起来
//
// 返回值:
//      返回创建好的DX字体, 调用此函数创建的字体不允许手动释放
IDRAW_FUNC3(LPEX_FONT, iDraw_CreateFontDx, LPCWSTR, name, int, lfHeight, int, style)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建字体, 不允许手动释放, 程序在结束时会释放, 相同颜色只会创建一次
// 
// 参数:
//      参数1: LPLOGFONTW 结构指针
//
// 返回值:
//      返回创建好的DX字体, 调用此函数创建的字体不允许手动释放
IDRAW_FUNC1(LPEX_FONT, iDraw_CreateFontLfDx, LPLOGFONTW, lf)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
// 暂时未对外公开
//      通过窗口句柄和消息值获取该消息绑定的回调
// 
// 参数:
//      参数1: 窗口句柄
//      参数2: 注册的消息值
//      参数3: 缓冲区数组指针, 一个成员占用20个字节
//      参数4: 缓冲区数组成员数
// 
// 返回值:
//      返回写入缓冲区成员数
IDRAW_FUNC4(int, iDraw_Event_GetMessageInfo, HWND, hWnd, UINT, message, LPREGISTER_MESSAGE_INFO*, pArr, int, bufSize)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
// 暂时未对外公开
//      通过事件ID获取回调信息
// 
// 参数:
//      参数1: 事件ID
//      参数2: 缓冲区数组指针, 一个成员占用24个字节
//      参数3: 缓冲区数组成员数
// 
// 返回值:
//      返回写入缓冲区成员数
IDRAW_FUNC3(int, iDraw_Event_GetEventInfo, UINT, nEvent, LPREGISTER_EVENT_INFO*, pArr, int, bufSize)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      注册消息回调
// 
// 参数:
//      参数1: 插件唯一标识
//      参数2: 注册哪个窗口的消息, 如果该窗口不存在, 则调用失败
//      参数3: 注册的消息值, 当窗口触发这个消息时会调用回调函数
//      参数4: 处理前的回调函数, 窗口处理这个消息前调用这个回调函数, 为0则取消挂接这个事件
//      参数5: 处理后的回调函数, 窗口已经处理这个消息后调用这个回调函数, 为0则取消挂接这个事件
//          参数4 和 参数5 的回调原型都是相同的
//              需要5个参数, 整数返回值, 参数数量必须匹配, 否则会导致堆栈不平衡从而导致程序崩溃
//              整数型 回调函数(窗口句柄, 消息值, 参数1, 参数2, 窗口数据指针)
//              与子类化接收消息函数一样
// 
// 返回值:
//      返回错误码, 0=成功
IDRAW_FUNC5(int, iDraw_Event_RegisterMessage, LPCSTR, guid, HWND, hWnd, UINT, message, pfn_SubClassWndProc, pfnBefore, pfn_SubClassWndProc, pfnAfter)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      注册事件回调
// 
// 参数:
//      参数1: 唯一标识符
//      参数2: 需要注册的事件, 会传递到回调函数里
//      参数3: 处理前的回调函数参数, 这个参数会在调用 参数4 时传递过去
//      参数4: 处理前的回调函数, 为0则取消挂接这个事件
//      参数5: 处理后的回调函数参数, 这个参数会在调用 参数6 时传递过去
//      参数6: 处理后的回调函数, 为0则取消挂接这个事件
//          参数4 和 参数6 的回调原型都是相同的
//              需要3个参数, 整数返回值, 参数数量必须匹配, 否则会导致堆栈不平衡从而导致程序崩溃
//              整数型 回调函数(事件类型, 回调参数, 事件指针)
// 
// 返回值:
//      返回错误码, 0=成功
IDRAW_FUNC6(int, iDraw_Event_RegisterEvent, LPCSTR, guid, UINT, nEvent, LONG_PTR, paramBefore, pfn_Register_Event, pfnBefore, LONG_PTR, paramAfter, pfn_Register_Event, pfnAfter)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      取消指定插件的所有消息回调
// 
// 参数:
//      参数1: 插件唯一标识符
// 
// 返回值:
//      返回错误码, 0=成功
IDRAW_FUNC1(int, iDraw_UnRegister_Message, LPCSTR, guid)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      取消指定插件的所有事件回调
// 
// 参数:
//      参数1: 插件唯一标识符
// 
// 返回值:
//      返回错误码, 0=成功
IDRAW_FUNC1(int, iDraw_UnRegister_Event, LPCSTR, guid)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取pInfo的信息, 接口程序的第一个参数
// 
// 参数:
//      参数1: 接口程序的第一个参数
//      参数2: 获取的编号, IGI_ 开头常量
//      参数3: 获取的参数, 根据编号传递
// 
// 返回值:
//      获取到的值
IDRAW_FUNC3(int, iDraw_GetInfo, LPIDRAW_INFO, pInfo, int, index, LPARAM, argument)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      设置pInfo的信息, 接口程序的第一个参数, 谨慎设置, 这里的配置是全局使用, 一旦出错就会导致崩溃
// 
// 参数:
//      参数1: 接口程序的第一个参数
//      参数2: 获取的编号, IGI_ 开头常量
//      参数3: 要设置的值
// 
// 返回值:
//      返回设置之前的值
IDRAW_FUNC3(int, iDraw_SetInfo, LPIDRAW_INFO, pInfo, int, index, LONG_PTR, value)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      把函数放到易语言窗口线程里调用
// 
// 参数:
//      参数1: 函数地址, 没有返回值, 必须没有返回值和一个整数型参数, 否则会堆栈不平衡导致崩溃
//      参数2: 传递到函数里的参数
// 
// 返回值:
//      失败返回0, 成功返回非0值
IDRAW_FUNC2(bool, iDraw_EWindowThreadCallback, pfn_EWindowThreadCallback, pfnCallback, LPARAM, param)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      从图片数据里生成资源返回值, 所有指针必须保证存活, 内部不会申请内存来保存传递进来的文本和位图
// 
// 参数:
//      参数1: 图片数据地址
//      参数2: 图片数据大小
//      参数3: 绘制当前项目显示的文本, 不会修改原组件文本, 只是绘画时显示这个文本, 可以为0
//      参数4: 备注文本, 可以为0
//      参数5: 文本ARGB颜色, 可以为0, 不为0则必须是4个成员, 分别是 正常, 热点, 按下, 禁止
//      参数6: 背景ARGB颜色, 可以为0, 不为0则必须是4个成员, 分别是 正常, 热点, 按下, 禁止
// 
// 返回值:
//      返回资源返回值, 返回值不使用时必须调用 iDraw_FreeResourceReturn 来释放
IDRAW_FUNC6(LPWINDOW_ICON_DATA, iDraw_MakeResourceReturn, const void*, pImg, int, size, LPCWSTR, text, LPCWSTR, remarks, DWORD*, crText, DWORD*, crBack)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      释放 iDraw_MakeResourceReturn() 生成的返回值
// 
// 参数:
//      参数1: iDraw_MakeResourceReturn() 的返回值
// 
// 返回值:
//      返回是否释放成功
IDRAW_FUNC1(bool, iDraw_FreeResourceReturn, LPWINDOW_ICON_DATA, pData)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      从内存加载gdi+图片
// 
// 参数:
//      参数1: 图片数据
//      参数2: 图片数据大小
// 
// 返回值:
//      成功返回gdi+图片指针, 失败返回0
IDRAW_FUNC2(void*, iDraw_LoadGpImageFromMemory, const void*, pData, int, size)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      释放从 iDraw_LoadGpImageFromMemory() 返回的值
// 
// 参数:
//      参数1: iDraw_LoadGpImageFromMemory() 返回的值
// 
// 返回值:
//      成功返回true, 失败返回false
IDRAW_FUNC1(bool, iDraw_FreeGpImage, void*, hImg)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      添加按钮到接口工具条中, 成功返回工具条按钮ID, 触发工具条按钮被点击时会把这个ID传递过去
// 
// 参数:
//      参数1: 插件唯一标识符, 触发点击/重画事件时会调用这个插件的回调函数
//      参数2: 按钮的ID, 触发事件时会把这个ID传递过去
//      参数3: 按钮标题, 可以为0, 为0则没有标题, 鼠标移动上去时弹出的提示说明标题, 加粗显示, 这个标题不会显示在工具条上
//      参数4: 按钮说明, 可以为0, 为0则没有说明, 鼠标移动上去时弹出的提示说明
//      参数5: 项目数值, 会跟随点击事件传递过去
//      参数6: 图片数据指针, 这个是显示到工具条上的图标
//      参数7: 图片数据大小
//      参数8: 绘画回调函数, pfn_ToolDrawButton 类型
//              typedef int(IDRAW_CALL* pfn_ToolDrawButton)(LPCANVAS hCanvas, int id, LPARAM param, const RECT_F* prc, STATE state, UINT dpi);
// 返回值:
//      成功返回工具条按钮ID, 失败返回0
IDRAW_FUNC8(int, iDraw_Tool_InsertButton, LPCSTR, guid, int, idCommand, LPCWSTR, lpszTitle, LPCWSTR, lpszTips, LPARAM, param, const void*, pImageData, int, nImageSize, pfn_ToolDrawButton, pfnDraw)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      删除接口工具条的按钮, 只能删除自己插件添加的按钮
// 
// 参数:
//      参数1: 插件唯一标识符, 触发点击/重画事件时会调用这个插件的回调函数
//      参数2: 按钮的ID, 添加时的ID
// 
// 返回值:
//      返回是否成功
IDRAW_FUNC2(bool, iDraw_Tool_DeleteButton, LPCSTR, guid, int, idCommand)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取接口工具条的按钮信息, 只能获取自己插件添加的按钮
// 
// 参数:
//      参数1: 插件唯一标识符
//      参数2: 按钮的ID, 添加时的ID
//      参数3: 为0则不接收, 接收按钮标题
//      参数4: 为0则不接收, 接收按钮说明
//      参数5: 为0则不接收, 接收项目数值
//      参数6: 为0则不接收, 接收图片数据指针
//      参数7: 为0则不接收, 接收图片数据大小
//      参数8: 为0则不接收, 接收绘画回调函数
// 
// 返回值:
//      返回是否成功
IDRAW_FUNC8(bool, iDraw_Tool_GetButtonData, LPCSTR, guid, int, idCommand, LPWSTR*, lpszTitle, LPWSTR*, lpszTips, LPARAM*, param, LPCVOID*, pImageData, int*, nImageSize, pfn_ToolDrawButton*, pfnDraw)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      设置接口工具条的按钮信息, 只能设置自己插件添加的按钮
// 
// 参数:
//      参数1: 插件唯一标识符
//      参数2: 按钮的ID, 添加时的ID
//      参数3: 为0则不设置, 设置按钮标题
//      参数4: 为0则不设置, 设置按钮说明
//      参数5: 为0则不设置, 设置项目数值
//      参数6: 为0则不设置, 设置图片数据指针
//      参数7: 为0则不设置, 设置图片数据大小
//      参数8: 为0则不设置, 设置绘画回调函数
// 
// 返回值:
//      返回是否成功
IDRAW_FUNC8(bool, iDraw_Tool_SetButtonData, LPCSTR, guid, int, idCommand, LPCWSTR, lpszTitle, LPCWSTR, lpszTips, LPARAM, param, const void*, pImageData, int, nImageSize, pfn_ToolDrawButton, pfnDraw)


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      调用易语言交互函数, 调用这个函数就相当于调用特殊库的 调用易系统功能() 命令
// 
// 参数:
//      参数1: 功能号, 功能号请参考易语言提供的 lib2.h 文件, NES_, NAS_, NRS_ 开头常量
//      参数2: 请参阅 lib2.h 功能号说明
//      参数3: 请参阅 lib2.h 功能号说明
// 
// 返回值:
//      请参阅 lib2.h 功能号说明
IDRAW_FUNC3(bool, iDraw_EIDE_Notify, int, nMsg, DWORD, dwParam1, DWORD, dwParam2)

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      调用易语言IDE接口功能, 调用这个函数相当于调用 iDraw_EIDE_Notify(NES_RUN_FUNC, 参数1, 数组[参数2, 参数3]);
// 
// 参数:
//      参数1: 功能号, 功能号请参考易语言提供的 PublicIDEFunctions.h 文件, 
//      参数2: 请参阅 PublicIDEFunctions.h 功能号说明
//      参数3: 请参阅 PublicIDEFunctions.h 功能号说明
// 
// 返回值:
//      请参阅 lib2.h 功能号说明
IDRAW_FUNC3(bool, iDraw_EIDE_RunFunctions, int, nMsg, DWORD, dwParam1, DWORD, dwParam2)

// API接口结束