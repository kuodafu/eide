#if 1
#include "EWindow_Fne_Main.h"
#include <elib/lib2.h>
#include <control/draw_Window_include.h>


// 对外提供的接口

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
//      获取到的值
EXTERN_C _declspec(dllexport) LPCWSTR WINAPI iDraw_GetVersion(int* pMajor, int* pMinor, int* pBuild)
{
    if (pMajor) *pMajor = IDRAW_MAJORVERSION;
    if (pMinor) *pMinor = IDRAW_MINORVERSION;
    if (pBuild) *pBuild = IDRAW_BUILDVERSION;
    return IDRAW_VERSION;
}

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
//      获取到的值
EXTERN_C _declspec(dllexport) LPCSTR WINAPI iDraw_GetVersionA(int* pMajor, int* pMinor, int* pBuild)
{
    if (pMajor) *pMajor = IDRAW_MAJORVERSION;
    if (pMinor) *pMinor = IDRAW_MINORVERSION;
    if (pBuild) *pBuild = IDRAW_BUILDVERSION;
    return IDRAW_VERSIONA;
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取当前易语言的版本号
// 
// 参数:
//      无参数
// 
// 返回值:
//      获取到的值, 5.93 返回593, 5.11 返回511, 5.92有两个版本, 会返回592和5922
EXTERN_C _declspec(dllexport) int WINAPI iDraw_GetEVersion()
{
    return s_info->EVersion;
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取从支持库里申请内存, 内存是从 iDraw.fne 这个模块里申请的, 内部调用的是 malloc
// 
// 参数:
//      参数1: 申请的尺寸
// 
// 返回值:
//      申请的内存地址, 出错返回0, 申请失败不会抛出异常
EXTERN_C _declspec(dllexport) void* WINAPI iDraw_malloc(int size)
{
    return malloc(size);
}

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
EXTERN_C _declspec(dllexport) void WINAPI iDraw_free(void* ptr)
{
    if (ptr)
        free(ptr);
}

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
EXTERN_C _declspec(dllexport) LONG_PTR WINAPI iDraw_AppendMenu(LPOBJSTRUCT pData, int index)
{

    return 0;
}

inline ICTinyXml2_XMLNode _get_node(LPCWSTR lpszNodeName, LPCWSTR lpszNodeValue, BOOL isCreate, BOOL* lpIsNewNode)
{

}

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
EXTERN_C _declspec(dllexport) ICTinyXml2_XMLNode WINAPI iDraw_Xml_GetNode(LPCWSTR lpszNodeName, LPCWSTR lpszNodeValue, BOOL isCreate, BOOL* lpIsNewNode)
{
    //if (!lpszNodeName || !lpszNodeName[0]) return 0;
    //if (lpIsNewNode)*lpIsNewNode = FALSE;
    //ICTinyXml2_XMLNode node;
    //wchar_t nodeName[100];
    //swprintf_s(nodeName, 100, L"@1/%s", lpszNodeName);
    //if (!s_info->xml->GetNode(nodeName, node))
    //{
    //    if (!isCreate) return 0;
    //    // 走到这就是需要创建新的节点
    //    s_info->xml->InsertChildNode(L"@1", lpszNodeName, lpszNodeValue, 0, &node);
    //    if (!node) return 0;
    //    if (lpIsNewNode)*lpIsNewNode = TRUE;
    //}
    //return node;
    return 0;
}

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
EXTERN_C _declspec(dllexport) ICTinyXml2_XMLNode WINAPI iDraw_Xml_GetChildNode(ICTinyXml2_XMLNode nodeParent, LPCWSTR lpszNodeName, LPCWSTR lpszNodeValue, BOOL isCreate, BOOL* lpIsNewNode)
{
    return 0;
    //if (!lpszNodeName || !lpszNodeName[0]) return 0;
    //if (!nodeParent)
    //    return iDraw_Xml_GetNode(lpszNodeName, lpszNodeValue, isCreate, lpIsNewNode);
    //if (lpIsNewNode)*lpIsNewNode = FALSE;
    //_str nodeNameA = wstr::W2A(lpszNodeName);
    //ICTinyXml2_XMLNode node = s_info->xml->FirstChildElement(nodeParent, nodeNameA.c_str());
    //if (!node)  // 没有这个节点, 需要判断是否创建这个节点
    //{
    //    if (!isCreate) return 0;
    //    // 走到这就是需要创建新的节点
    //    s_info->xml->InsertChildNode(nodeParent, lpszNodeName, lpszNodeValue, 0, &node);
    //    if (!node) return 0;
    //    if (lpIsNewNode)*lpIsNewNode = TRUE;
    //}
    //return node;
}

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
EXTERN_C _declspec(dllexport) ICTinyXml2_XMLNode WINAPI iDraw_Xml_InsertChildNode(ICTinyXml2_XMLNode pNode, LPCWSTR lpszNodeName, LPCWSTR pszNodeValue)
{
    return 0;
    //if (!pNode || !lpszNodeName || !lpszNodeName[0])return 0;
    //_str nodeNameA = wstr::W2A(lpszNodeName);

    //ICTinyXml2_XMLNode node = s_info->xml->FirstChildElement(pNode, nodeNameA.c_str());

    //if (!node)  // 没有这个节点, 需要创建一个
    //{
    //    ICTinyXml2_XMLElement child = s_info->xml->NewElement(nodeNameA.c_str());
    //    if (pszNodeValue)s_info->xml->SetText(child, wstr::W2A(pszNodeValue).c_str());
    //    node = s_info->xml->InsertEndChild(pNode, child);
    //}
    //return node;
}

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
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_Xml_SetAttrValue(ICTinyXml2_XMLNode node, LPCWSTR lpszAttrName, LPCWSTR lpszAttrValue)
{
    return 0;
    //if (!node || !lpszAttrName || !lpszAttrName[0])return false;
    //return s_info->xml->SetAttrValue(node, lpszAttrName, lpszAttrValue);
}

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
EXTERN_C _declspec(dllexport) LPCWSTR WINAPI iDraw_Xml_GetAttrValue(ICTinyXml2_XMLNode node, LPCWSTR lpszAttrName, BOOL* isAttribute)
{
    return 0;
    //if (!node || !lpszAttrName || !lpszAttrName[0])return 0;
    //return s_info->xml->GetAttrValue(node, lpszAttrName, isAttribute);
}


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
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_Xml_SetNodeValue(ICTinyXml2_XMLNode node, LPCWSTR lpszNodeValue)
{
    return 0;
    //if (!node)return false;
    //return s_info->xml->SetNodeValue(node, lpszNodeValue);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      保存xml到磁盘
// 
// 参数:
//      无参数
// 
// 返回值:
//      获取到的值
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_Xml_SaveFile()
{
    return 0;
    //return s_info->xml->SaveFile(); // 支持库接口保存xml到磁盘
}


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取指定节点值, 返回节点值文本指针
// 
// 参数:
//      参数1: xml节点指针
// 
// 返回值:
//      获取到的值
EXTERN_C _declspec(dllexport) LPCWSTR WINAPI iDraw_Xml_GetNodeValue(ICTinyXml2_XMLNode node)
{
    return 0;
    //if (!node)return 0;
    //return s_info->xml->GetNodeValue(node);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化指定窗口, 并设置一些默认的绘画信息, 内部不会对其进行重画
//      支持库所有子类化都是通过这个接口进行二次封装
// 参数:
//      参数1: 需要子类化的窗口句柄
//      参数2: 接收子类化消息回调的函数, 有5个参数, 函数调用约定为 __stdcall, 返回值为 LRESULT 类型
//              前4个参数与普通子类化相同, 最后多的那个参数就是调用这个接口的返回值
//      参数3: 是否需要删除边框, 为true则把边框相关的样式都删除
//      参数4: 是否设置类画刷, 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
// 返回值:
//      返回保存窗口信息的结构指针, 组件的各种配色信息都在这个结构中保存
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_SubclassWindow(HWND hWnd, pfn_SubClassWndProc proc, bool reMoveBorder, bool isSetBack)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->_subclass(hWnd, proc, reMoveBorder, isSetBack, 0);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      把窗口的消息转到重画窗口的过程去处理
// 参数:
//      参数1: 需要重画的窗口句柄
//      参数2: 需要处理的消息
//      参数3: 参数1
//      参数4: 参数2
//      参数5: 参数2
//      参数6: 窗口原来处理消息的回调地址
//      参数7: 接收处理完的消息返回值
// 返回值:
//      返回是否处理了这条消息
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_DrawWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc, LRESULT* pRet)
{
    if (!s_info || !s_info->iDraw || !hWnd)
        return 0;

    IDRAW_SYSBUTTON_ICON_TITLE_WINDOWPROC_STRUCT arg;
    arg.hWnd    = hWnd;
    arg.message = message;
    arg.wParam  = wParam;
    arg.lParam  = lParam;
    arg.oldProc = oldProc;
    arg.pRet    = pRet;
    return s_info->pfnControls(IDC_IDRAW_DRAW_WINDOWPROC, (WPARAM)0, (LPARAM)&arg);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取默认参数
// 参数:
//      参数1: 需要重画的窗口句柄, 为0则在返回后自己给窗口句柄赋值
//      参数2: DRAW_WINDOW_ARGUMENT 结构指针, 内部把默认参数填充到这个结构里
// 返回值:
//      返回保存窗口信息的结构指针, 组件的各种配色信息都在这个结构中保存
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_DrawGetDefArg(HWND hWnd, LPDRAW_WINDOW_ARGUMENT arg)
{
    if (!s_info || !s_info->iDraw || !arg)
        return 0;
    return s_info->pfnControls(IDC_IDRAW_DRAW_DEFARG, (WPARAM)hWnd, (LPARAM)arg);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      把一个顶层窗口重画成与视觉库相同的视觉效果
// 参数:
//      参数1: 需要重画的窗口句柄
//      参数2: DRAW_WINDOW_ARGUMENT 结构指针, 为0则使用与当前主题匹配的配色信息
// 返回值:
//      返回保存窗口信息的结构指针, 组件的各种配色信息都在这个结构中保存
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_DrawWindow(HWND hWnd, LPDRAW_WINDOW_ARGUMENT arg)
{
    if (!s_info || !s_info->iDraw || !hWnd)
        return 0;
    if (arg && (arg->cbSize == 0 || arg->cbSize > 0x7ffffff))
        return 0;
    s_info->pfnControls(IDC_IDRAW_SUBWINDOWEX, (WPARAM)hWnd, (LPARAM)arg);
    return s_info->pfnGetDataFromHwnd(hWnd);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建弹出式窗口, 这个窗口和视觉库有相同的视觉效果
// 参数:
//      参数1: 窗口的扩展样式, 详情请参考 CreateWindowEx 的说明
//      参数2: 窗口的标题, 为0则没有标题, unicode文本
//      参数3: 窗口的样式, 详情请参考 CreateWindowEx 的说明
//      参数4: 窗口在屏幕的左边位置
//      参数5: 窗口在屏幕的顶边位置
//      参数6: 窗口宽度
//      参数7: 窗口高度
//      参数8: 父窗口, 如果为0, 则没有父窗口, 如果有则创建的窗口始终在这个窗口之上
//      参数9: DRAW_WINDOW_ARGUMENT 结构指针, 为0则使用与当前主题匹配的配色信息
// 返回值:
//      返回保存窗口信息的结构指针, 组件的各种配色信息都在这个结构中保存
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_CreateWindow(DWORD dwExStyle, LPCWSTR lpWindowName, DWORD dwStyle, int x, int y, int cx, int cy, HWND hWndParent, LPDRAW_WINDOW_ARGUMENT arg)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    if (arg && (arg->cbSize == 0 || arg->cbSize > 0x7ffffff))
        return 0;
    CREATESTRUCTW cs = { 0 };
    cs.x = x, cs.y = y, cs.cx = cx, cs.cy = cy;
    cs.lpszName = lpWindowName;
    cs.style = dwStyle;
    cs.dwExStyle = dwExStyle;
    cs.hwndParent = hWndParent;

    HWND hWnd = (HWND)s_info->pfnControls(IDC_IDRAW_CREATEWINDOW, (WPARAM)&cs, (LPARAM)arg);
    if (!hWnd)
        return 0;
    return s_info->pfnGetDataFromHwnd(hWnd);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      挂接窗口绘画事件, 只有调用 iDraw_SubclassWindowEx, 或者 iDraw_SubclassWindow 的窗口才有效
//      当窗口在绘画 系统图标, 标题, 系统按时, 会触发事件
// 参数:
//      参数1: 有调用过 iDraw_SubclassWindowEx 的窗口句柄, 或者 iDraw_SubclassWindow 返回的窗口句柄
//      参数2: 接收绘画回调的函数, 函数原型 = typedef bool(CALLBACK* PFN_SysDraw)(LPIDRAW_STRUCT_DRAWSTRUCT ps);
//      参数3: 传递到回调函数里的参数
// 返回值:
//      返回是否成功
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_Event_Draw(HWND hWnd, PFN_SysDraw pfn, LPARAM param)
{
    if (!s_info || !s_info->iDraw || !hWnd)
        return 0;
    IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT arg = { 0 };
    arg.hWnd = hWnd;
    arg.pfn = pfn;
    arg.param = param;
    return s_info->pfnControls(IDC_IDRAW_SYSDRAW, (WPARAM)0, (LPARAM)&arg);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      挂接系统按钮点击事件, 只有调用 iDraw_SubclassWindowEx, 或者 iDraw_SubclassWindow 的窗口才有效
// 参数:
//      参数1: 有调用过 iDraw_SubclassWindowEx 的窗口句柄, 或者 iDraw_SubclassWindow 返回的窗口句柄
//      参数2: 接收绘画回调的函数, 函数原型 = typedef bool(CALLBACK* PFN_SysBtnClick)(int id, BOOL isRButton, LPARAM param);
//      参数3: 传递到回调函数里的参数
// 返回值:
//      返回是否成功
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_Event_SysButtonClick(HWND hWnd, PFN_SysBtnClick pfn, LPARAM param)
{
    if (!s_info || !s_info->iDraw || !hWnd)
        return 0;
    IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT arg = { 0 };
    arg.hWnd = hWnd;
    arg.pfn = pfn;
    arg.param = param;
    return s_info->pfnControls(IDC_IDRAW_SYSBTNCLICK, (WPARAM)0, (LPARAM)&arg);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      添加系统按钮, 系统按钮索引是从右往左, 只有调用 iDraw_SubclassWindowEx, 或者 iDraw_SubclassWindow 的窗口才有效
// 参数:
//      参数1: 有调用过 iDraw_SubclassWindowEx 的窗口句柄, 或者 iDraw_SubclassWindow 返回的窗口句柄
//      参数2: 按钮的ID, 如果有相同ID, 则替换数值
//      参数3: 按钮数值
// 返回值:
//      返回新增加按钮的索引, 系统按钮索引是从右往左
EXTERN_C _declspec(dllexport) int WINAPI iDraw_AddSysButton(HWND hWnd, int id, LPARAM param)
{
    if (!s_info || !s_info->iDraw || !hWnd)
        return 0;
    IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT arg = { 0 };
    arg.hWnd = hWnd;
    arg.id = id;
    arg.param = param;
    return s_info->pfnControls(IDC_IDRAW_ADDSYSBUTTON, (WPARAM)0, (LPARAM)&arg);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      删除系统按钮, 系统按钮索引是从右往左, 只有调用 iDraw_SubclassWindowEx, 或者 iDraw_SubclassWindow 的窗口才有效
// 参数:
//      参数1: 有调用过 iDraw_SubclassWindowEx 的窗口句柄, 或者 iDraw_SubclassWindow 返回的窗口句柄
//      参数2: 要删除的按钮位置或ID
//      参数3: 指明参数2是位置还是ID, DRAW_WND_ 开头常量
// 返回值:
//      返回是否删除成功
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_DelSysButton(HWND hWnd, int nPosition, int flags)
{
    if (!s_info || !s_info->iDraw || !hWnd)
        return 0;
    IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT arg = { 0 };
    arg.hWnd = hWnd;
    arg.nPosition = nPosition;
    arg.flags = flags;
    return s_info->pfnControls(IDC_IDRAW_DELSYSBUTTON, (WPARAM)0, (LPARAM)&arg);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      插入系统按钮, 系统按钮索引是从右往左, 只有调用 iDraw_SubclassWindowEx, 或者 iDraw_SubclassWindow 的窗口才有效
// 参数:
//      参数1: 有调用过 iDraw_SubclassWindowEx 的窗口句柄, 或者 iDraw_SubclassWindow 返回的窗口句柄
//      参数2: 要插入的位置或ID, -1则插入到最后(最左边), 0则插入到第一个(最右边)
//      参数3: 要插入按钮的ID
//      参数4: 按钮数值
//      参数5: 指明参数2是位置还是ID, DRAW_WND_ 开头常量
// 返回值:
//      返回插入的位置
EXTERN_C _declspec(dllexport) int WINAPI iDraw_InsSysButton(HWND hWnd, int nPosition, int id, LPARAM param, int flags)
{
    if (!s_info || !s_info->iDraw || !hWnd)
        return 0;
    IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT arg = { 0 };
    arg.hWnd = hWnd;
    arg.id = id;
    arg.nPosition = nPosition;
    arg.param = param;
    arg.flags = flags;
    return s_info->pfnControls(IDC_IDRAW_INSSYSBUTTON, (WPARAM)0, (LPARAM)&arg);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      设置系统按钮项目数值, 返回设置前的项目数值
// 参数:
//      参数1: 有调用过 iDraw_SubclassWindowEx 的窗口句柄, 或者 iDraw_SubclassWindow 返回的窗口句柄
//      参数2: 要设置的按钮ID或位置
//      参数3: 按钮数值
//      参数4: 指明参数2是位置还是ID, DRAW_WND_ 开头常量
// 返回值:
//      返回设置前的项目数值
EXTERN_C _declspec(dllexport) LPARAM WINAPI iDraw_SetButtonParam(HWND hWnd, int nPosition, LPARAM param, int flags)
{
    if (!s_info || !s_info->iDraw || !hWnd)
        return 0;
    IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT arg = { 0 };
    arg.hWnd = hWnd;
    arg.nPosition = nPosition;
    arg.param = param;
    arg.flags = flags;
    return s_info->pfnControls(IDC_IDRAW_SETBTNPARAM, (WPARAM)0, (LPARAM)&arg);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取系统按钮项目数值
// 参数:
//      参数1: 有调用过 iDraw_SubclassWindowEx 的窗口句柄, 或者 iDraw_SubclassWindow 返回的窗口句柄
//      参数2: 要获取的按钮ID或位置
//      参数3: 指明参数2是位置还是ID, DRAW_WND_ 开头常量
// 返回值:
//      返回按钮的项目数值
EXTERN_C _declspec(dllexport) LPARAM WINAPI iDraw_GetButtonParam(HWND hWnd, int nPosition, int flags)
{
    if (!s_info || !s_info->iDraw || !hWnd)
        return 0;
    IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT arg = { 0 };
    arg.hWnd = hWnd;
    arg.nPosition = nPosition;
    arg.flags = flags;
    return s_info->pfnControls(IDC_IDRAW_GETBTNPARAM, (WPARAM)0, (LPARAM)&arg);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      从按钮索引获取系统按钮ID
// 参数:
//      参数1: 有调用过 iDraw_SubclassWindowEx 的窗口句柄, 或者 iDraw_SubclassWindow 返回的窗口句柄
//      参数2: 要获取的按钮位置, 从0开始
// 返回值:
//      返回按钮的ID, 失败返回0
EXTERN_C _declspec(dllexport) LPARAM WINAPI iDraw_GetButtonIdFromPos(HWND hWnd, int nPosition)
{
    if (!s_info || !s_info->iDraw || !hWnd)
        return 0;
    IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT arg = { 0 };
    arg.hWnd = hWnd;
    arg.nPosition = nPosition;
    return s_info->pfnControls(IDC_IDRAW_GETBTNID, (WPARAM)0, (LPARAM)&arg);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      从按钮ID获取系统按钮的索引, 从0开始
// 参数:
//      参数1: 有调用过 iDraw_SubclassWindowEx 的窗口句柄, 或者 iDraw_SubclassWindow 返回的窗口句柄
//      参数2: 要获取的按钮位置
// 返回值:
//      返回按钮的索引, 失败返回-1
EXTERN_C _declspec(dllexport) LPARAM WINAPI iDraw_GetButtonPosFromId(HWND hWnd, int id)
{
    if (!s_info || !s_info->iDraw || !hWnd)
        return 0;
    IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT arg = { 0 };
    arg.hWnd = hWnd;
    arg.id = id;
    return s_info->pfnControls(IDC_IDRAW_GETBTNPOS, (WPARAM)0, (LPARAM)&arg);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      调用原回调过程
// 参数:
//      参数1: 窗口句柄, 消息回调里的第一个参数
//      参数2: 消息值, 消息回调里的第二个参数
//      参数3: 参数1, 消息回调里的第三个参数
//      参数4: 参数2, 消息回调里的第四个参数
//      参数4: 组件数据, 消息回调里的第五个参数
// 返回值:
//      返回保存窗口信息的结构指针, 组件的各种配色信息都在这个结构中保存
EXTERN_C _declspec(dllexport) LRESULT WINAPI iDraw_CallWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    if (!s_info)
        return 0;
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画按钮, 按钮的类名为 Button
// 参数:
//      参数1: 按钮的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数3: 需要删除的类样式, 为0则不删除
// 返回值:
//      返回保存窗口信息的结构指针, 组件的各种配色信息都在这个结构中保存
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_Subclass_Button(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->SubButton(hWnd, reMoveBorder, isSetBack, reMoveClassStyle);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画编辑框, 编辑框的类名为 Edit, 目前还有个问题就是无法指定边框, 后期会慢慢解决
// 参数:
//      参数1: 编辑框的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
//      参数5: 是否重画滚动条
// 返回值:
//      返回保存窗口信息的结构指针, 组件的各种配色信息都在这个结构中保存
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_Subclass_Edit(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, bool isScroll)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->SubEdit(hWnd, reMoveBorder, isSetBack, reMoveClassStyle, isScroll);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画标签, 标签的类名为 static
// 参数:
//      参数1: 标签的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
// 返回值:
//      返回保存窗口信息的结构指针, 组件的各种配色信息都在这个结构中保存
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_Subclass_Static(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->SubStatic(hWnd, reMoveBorder, isSetBack, reMoveClassStyle);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画状态条, 状态条的类名为 msctls_statusbar32
// 参数:
//      参数1: 状态条的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
// 返回值:
//      返回保存窗口信息的结构指针, 组件的各种配色信息都在这个结构中保存
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_Subclass_StatusBar(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->SubStatusBar(hWnd, reMoveBorder, isSetBack, reMoveClassStyle);
}

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
//      返回保存窗口信息的结构指针, 组件的各种配色信息都在这个结构中保存
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_Subclass_TreeView(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int ItemHeight, bool isScroll, bool isHot)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->SubTreeView(hWnd, reMoveBorder, isSetBack, reMoveClassStyle, ItemHeight, isScroll, isHot);
}

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
//      返回保存窗口信息的结构指针, 组件的各种配色信息都在这个结构中保存
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_Subclass_ListBox(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int nItemHeight, bool isScroll, bool isHot)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->SubListBox(hWnd, reMoveBorder, isSetBack, reMoveClassStyle, nItemHeight, isScroll, isHot);
}

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
//      返回保存窗口信息的结构指针, 组件的各种配色信息都在这个结构中保存
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_Subclass_ListView(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int nItemHeight, bool isScroll, bool isHot, int fmt)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->SubListView(hWnd, reMoveBorder, isSetBack, reMoveClassStyle, nItemHeight, isScroll, isHot, fmt);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画工具条, 工具条的类名为 ToolbarWindow32
// 参数:
//      参数1: 工具条的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
// 返回值:
//      返回保存窗口信息的结构指针, 组件的各种配色信息都在这个结构中保存
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_Subclass_Tool(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->SubTool(hWnd, reMoveBorder, isSetBack, reMoveClassStyle);
}



// 函数功能: 
//      子类化并重画选择夹, 选择夹的类名为 SysTabControl32
// 参数:
//      参数1: 选择夹窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
// 返回值:
//      返回保存窗口信息的结构指针, 组件的各种配色信息都在这个结构中保存
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_Subclass_Tab(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->SubTab(hWnd, reMoveBorder, isSetBack, reMoveClassStyle);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      子类化并重画调节器, 调节器的类名为 msctls_updown32
// 参数:
//      参数1: 调节器的窗口句柄
//      参数2: 是否删除边框
//      参数3: 如果为true, 则会调用 SetClassLong() 设置背景画刷, 相同类名的窗口都会受到影响
//      参数4: 需要删除的类样式, 为0则不删除
// 返回值:
//      返回保存窗口信息的结构指针, 组件的各种配色信息都在这个结构中保存
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_Subclass_msctls_updown32(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->Submsctls_updown32(hWnd, reMoveBorder, isSetBack, reMoveClassStyle);
}

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
//      返回保存窗口信息的结构指针, 组件的各种配色信息都在这个结构中保存
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_Subclass_ComboBox(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int nItemHeight)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->SubComboBox(hWnd, reMoveBorder, isSetBack, reMoveClassStyle, nItemHeight);
}

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
EXTERN_C _declspec(dllexport) LPVOID WINAPI iDraw_DrawScroll(HWND hWnd, int nListViewItemHeight, bool isTreeView, bool isCallSetScrollInfo)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->DrawScroll(hWnd, nListViewItemHeight, isTreeView, isCallSetScrollInfo);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      初始化绘画信息, 必须是在 WM_PAINT 消息下调用, 配套的函数是 iDraw_Control_EndPaint()
//      会调用 BeginPaint, 会创建内存DC, 返回内存DC, DX渲染返回0
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
//      返回内存DC
EXTERN_C _declspec(dllexport) HDC WINAPI iDraw_BeginPaint(LPOBJSTRUCT pData, BEGIN_END_PAINT_STRUCT* data, bool isDxRender, bool isFillBack)
{
    if (!s_info || !s_info->iDraw || !data)
        return 0;
    return s_info->iDraw->BeginPaint(pData, *data, isDxRender, isFillBack);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      提交绘画信息, 必须是在 WM_PAINT 消息下调用, 调用此函数前必须已经调用了 iDraw_Control_BeginPaint()
//      把iDraw_Control_BeginPaint() 时创建的内存DC拷贝到窗口上
//      并调用 EndPaint 提交绘画信息
// 
// 参数:
//      参数1: BEGIN_END_PAINT_STRUCT 结构指针, 此函数会清理该结构的数据
// 
// 返回值:
//      返回是否拷贝成功
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_EndPaint(BEGIN_END_PAINT_STRUCT* data)
{
    if (!s_info || !s_info->iDraw || !data)
        return 0;
    return s_info->iDraw->EndPaint(*data);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取DX绘画对象指针
// 
// 参数:
//      无参数
// 
// 返回值:
//      返回dx绘画对象指针
EXTERN_C _declspec(dllexport) ID2DDraw* WINAPI iDraw_GetDxObject()
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d;
}

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
EXTERN_C _declspec(dllexport) int WINAPI iDraw_InterfaceArgument(IDRAW_INFO** pInfo, IGetEWindow** eWnd, int* reserved1, int* reserved2)
{
    if (!s_info)
        return 0;
    if (pInfo)
        *pInfo = s_info;
    if (eWnd)
        *eWnd = s_info->eWnd;
    if (reserved1)
        *reserved1 = 0;
    if (reserved2)
        *reserved2 = 0;
    return 0;
}

inline void CALLBACK _shadow_update(HWND hWnd, UINT message, UINT_PTR id, DWORD timer)
{
    if (id)
        KillTimer(hWnd, id);
    RECT rc;
    GetWindowRect(s_info->hEWnd, &rc);
    s_info->shadow->Update(rc, true);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      重画阴影窗口
// 
// 参数:
//      参数1: 是否异步重画, 如果不需要马上绘画请设置为true
// 
// 返回值:
//      返回阴影窗口句柄
EXTERN_C _declspec(dllexport) HWND WINAPI iDraw_UpdateShadow(bool bAsync)
{
    if (!s_info || !s_info->shadow)
        return 0;
    if (bAsync)
    {
        // 20毫秒后才调用重画, 防止多个插件重复频繁调用, 这里只支持20毫秒内最后一次调用
        SetTimer(s_info->shadow->GetWindow(), 10001, 20, _shadow_update);
    }
    else
    {
        _shadow_update(0, 0, 0, 0);
    }

    return s_info->hShadow;

}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      从文件加载ext, 成功返回资源句柄, 失败返回0
// 
// 参数:
//      参数1: ext的文件路径
// 
// 返回值:
//      返回ext文件的资源句柄, 可以使用 User_GetIcon/User_GetIconDx 这两个函数来获取图标
EXTERN_C _declspec(dllexport) IDRAW_HRESOURCE WINAPI iDraw_LoadExtFromFile(LPCWSTR pszFile)
{
    if (!s_info || !s_info->pResource)
        return 0;
    return s_info->pResource->LoadFromFile(pszFile);
}

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
EXTERN_C _declspec(dllexport) IDRAW_HRESOURCE WINAPI iDraw_LoadExtFromMemory(const void* ptr, int size)
{
    if (!s_info || !s_info->pResource)
        return 0;
    return s_info->pResource->LoadFromMemory(ptr, size);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取默认组件的资源句柄, 传递窗口句柄获取
// 
// 参数:
//      参数1: 从哪个窗口上获取, 现在只有 标准工具条, 程序/支持库/组件箱 树型框这几个有默认图标
// 
// 返回值:
//      返回窗口关联的资源句柄
EXTERN_C _declspec(dllexport) IDRAW_HRESOURCE WINAPI iDraw_GetDefControlIcon(HWND hWnd)
{
    if (!s_info || !s_info->pResource)
        return 0;
    return s_info->pResource->GetDefControlIcon(hWnd);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      取消插件所有绑定窗口资源的回调
// 
// 参数:
//      参数1: 插件唯一标识符
// 
// 返回值:
//      取消绑定成功返回true, 失败返回false
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_UnBindWindow(LPCSTR guid)
{
    if (!s_info || !s_info->pResource)
        return 0;
    return s_info->pResource->UnBindWindow(guid);
}

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
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_BindWindow(LPCSTR guid, HWND hWnd, IDRAW_HRESOURCE hResource, pfn_GetWindowIcon callback)
{
    if (!s_info || !s_info->pResource)
        return 0;
    return s_info->pResource->BindWindow(guid, hWnd, hResource, callback);
}


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
//      成功返回绑定时的资源句柄, 失败返回0
EXTERN_C _declspec(dllexport) IDRAW_HRESOURCE WINAPI iDraw_GetResourceFromHwnd(LPCSTR guid, HWND hWnd, pfn_GetWindowIcon* callback)
{
    if (!s_info || !s_info->pResource)
        return 0;
    if (callback)*callback = 0;
    LPPLUGIN_INFO plugin = s_info->pfnPlugin_GetInfo(guid);
    if (!plugin) return 0;
    int callbackCount = s_info->pResource->GetResourceCallbackFromHwnd(hWnd, 0, 0);
    BINDWINDOW_INFO* callbackArr = 0;
    if (callbackCount <= 0) return 0;

    IDRAW_HRESOURCE hResource = 0;
    callbackArr = new BINDWINDOW_INFO[callbackCount];
    memset(callbackArr, 0, sizeof(BINDWINDOW_INFO) * callbackCount);
    callbackCount = s_info->pResource->GetResourceCallbackFromHwnd(hWnd, callbackArr, callbackCount);
    for (int i = 0; i < callbackCount; i++)
    {
        if (callbackArr[i].guid == (LPCSTR)plugin->hash)
        {
            hResource = callbackArr[i].hResource;
            if (callback)*callback = callbackArr[i].callback;
            break;
        }
    }
    delete[] callbackArr;
    return hResource;
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      从资源句柄里获取图标数据
// 
// 参数:
//      参数1: 资源句柄, 使用 iDraw_LoadExtFromFile/iDraw_LoadExtFromMemory/iDraw_GetDefControlIcon 返回的值
//      参数2: 图标ID
//      参数3: RECT结构指针, 接收这个ID在ext中的位置
//      参数4: 要获取的状态, 目前只支持 STATE::HOT, STATE::DOWN, STATE::BAN, STATE::NORMAL 这4个
//      参数5: dpi
// 
// 返回值:
//      返回图标数据, 如果没有图标则返回0
EXTERN_C _declspec(dllexport) LPWINDOW_ICON_DATA WINAPI iDraw_GetIcon(IDRAW_HRESOURCE hResource, LPCWSTR pszID, RECT_F* prc, int state, UINT dpi)
{
    if (!s_info || !s_info->pResource)
        return 0;
    return s_info->pResource->User_GetIcon(hResource, pszID, prc, (STATE)state, dpi);
}
//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      从资源句柄里获取图标数据
// 
// 参数:
//      参数1: 资源句柄, 使用 iDraw_LoadExtFromFile/iDraw_LoadExtFromMemory/iDraw_GetDefControlIcon 返回的值
//      参数2: 图标ID
//      参数3: RECT结构指针, 接收这个ID在ext中的位置
//      参数4: 要获取的状态, 目前只支持 STATE::HOT, STATE::DOWN, STATE::BAN, STATE::NORMAL 这4个
//      参数5: dpi
// 
// 返回值:
//      返回dx图标数据, 如果没有图标则返回0
EXTERN_C _declspec(dllexport) LPWINDOW_ICON_DATA WINAPI iDraw_GetIconDx(IDRAW_HRESOURCE hResource, LPCWSTR pszID, RECT_F* prc, int state, UINT dpi)
{
    if (!s_info || !s_info->pResource)
        return 0;
    return s_info->pResource->User_GetIcon(hResource, pszID, prc, (STATE)state, dpi);
}

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
EXTERN_C _declspec(dllexport) void* WINAPI iDraw_GetImageHandle(LPWINDOW_ICON_DATA pIconData, bool isGdiplus)
{
    if (!pIconData)
        return 0;
    try
    {
        if (isGdiplus)
            pIconData->hBitmapGdip;
        return pIconData->hBitmapDx;
    }
    catch (...)
    {
        return 0;
    }
    return 0;
}


//////////////////////////////////////////////////////////////////////////
// 以下两个函数是内部调用
EXTERN_C _declspec(dllexport) void WINAPI iDraw_MenuInsert(int id, LPCWSTR lpszMenuText, int index, HMENU hPopupMenu, bool isPopup)
{
    if (!s_info || !s_info->pfnControls)
        return;
    IDC_STRUCT_MENU_INSERT arg = { id, lpszMenuText, index, hPopupMenu, isPopup };
    s_info->pfnControls(IDC_ICONTROL_INSERTMENUBAR, 0, (LPARAM)&arg);
}
EXTERN_C _declspec(dllexport) void WINAPI iDraw_MenuRemove(int id, bool isIndex)
{
    if (!s_info || !s_info->pfnControls)
        return;
    s_info->pfnControls(IDC_ICONTROL_REMOVEMENUBAR, id, isIndex);
}
//////////////////////////////////////////////////////////////////////////






//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建画刷, 不允许手动释放, 程序在结束时会释放, 相同颜色的画刷只会创建一次
// 
// 参数:
//      参数1: 需要创建的画刷, RGB颜色
// 
// 返回值:
//      返回创建好的GDI画刷, 调用此函数创建的画刷不允许手动释放
EXTERN_C _declspec(dllexport) HBRUSH WINAPI iDraw_CreateBrush(COLORREF cr)
{
    if (!s_info)
        return 0;
    return s_info->pfnCreateBrush(cr);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建画刷, 不允许手动释放, 程序在结束时会释放, 相同颜色的画刷只会创建一次
// 
// 参数:
//      参数1: 需要创建的画刷, ARGB颜色
// 
// 返回值:
//      返回创建好的画刷, 调用此函数创建的画刷不允许手动释放
EXTERN_C _declspec(dllexport) LPEX_BRUSH WINAPI iDraw_CreateBrushDx(DWORD cr)
{
    if (!s_info)
        return 0;
    return s_info->pfnCreateBrushDx(cr);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建画笔, 不允许手动释放, 程序在结束时会释放, 相同颜色只会创建一次
// 
// 参数:
//      参数1: 需要创建的画笔, ARGB颜色
// 
// 返回值:
//      返回创建好的GDI画笔, 调用此函数创建的画笔不允许手动释放
EXTERN_C _declspec(dllexport) HPEN WINAPI iDraw_CreatePen(int style, int width, COLORREF cr)
{
    if (!s_info)
        return 0;
    return s_info->pfnCreatePen(style, width, cr);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建画笔, 不允许手动释放, 程序在结束时会释放, 相同颜色只会创建一次
// 
// 参数:
//      参数1: 需要创建的画笔, ARGB颜色
// 
// 返回值:
//      返回创建好的GDI画笔, 调用此函数创建的画笔不允许手动释放
EXTERN_C _declspec(dllexport) LPEX_PEN WINAPI iDraw_CreatePenDx(int style, float width, DWORD cr)
{
    if (!s_info)
        return 0;
    return s_info->pfnCreatePenDx(cr, width);
}

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
EXTERN_C _declspec(dllexport) HFONT WINAPI iDraw_CreateFont(LPCWSTR name, int lfHeight, int style)
{
    if (!s_info)
        return 0;
    LPEX_FONT hFont = s_info->pfnCreateFontDx(name, lfHeight, (FONTSTYLE)style);
    if (!hFont) return 0;
    return hFont->GetFont();
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建字体, 不允许手动释放, 程序在结束时会释放, 相同颜色只会创建一次
// 
// 参数:
//      参数1: LPLOGFONTW 结构指针
//
// 返回值:
//      返回创建好的GDI字体, 调用此函数创建的字体不允许手动释放
EXTERN_C _declspec(dllexport) HFONT WINAPI iDraw_CreateFontLf(LPLOGFONTW lf)
{
    if (!s_info)
        return 0;
    LPEX_FONT hFont = s_info->pfnCreateFontLfDx(lf);
    if (!hFont) return 0;
    return hFont->GetFont();
}

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
EXTERN_C _declspec(dllexport) LPEX_FONT WINAPI iDraw_CreateFontDx(LPCWSTR name, int lfHeight, int style)
{
    if (!s_info)
        return 0;
    return s_info->pfnCreateFontDx(name, lfHeight, (FONTSTYLE)style);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建字体, 不允许手动释放, 程序在结束时会释放, 相同颜色只会创建一次
// 
// 参数:
//      参数1: LPLOGFONTW 结构指针
//
// 返回值:
//      返回创建好的DX字体, 调用此函数创建的字体不允许手动释放
EXTERN_C _declspec(dllexport) LPEX_FONT WINAPI iDraw_CreateFontLfDx(LPLOGFONTW lf)
{
    if (!s_info)
        return 0;
    return s_info->pfnCreateFontLfDx(lf);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      通过窗口句柄和消息值获取该消息绑定的回调
// 
// 参数:
//      参数1: 插件唯一标识
//      参数1: 窗口句柄
//      参数2: 注册的消息值
// 
// 返回值:
//      返回这个消息的回调信息结构指针, 如果没有找到则返回0
EXTERN_C _declspec(dllexport) int WINAPI iDraw_Event_GetMessageInfo(HWND hWnd, UINT message, LPREGISTER_MESSAGE_INFO* pArr, int bufSize)
{
    if (!s_info || !s_info->evt.pfnGetMessageInfo)
        return 0;
    return s_info->evt.pfnGetMessageInfo(hWnd, message, pArr, bufSize);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      通过事件ID获取回调信息
// 
// 参数:
//      参数1: 插件唯一标识
//      参数2: 事件ID
// 
// 返回值:
//      返回这个事件的回调信息结构指针, 如果没有找到则返回0
EXTERN_C _declspec(dllexport) int WINAPI iDraw_Event_GetEventInfo(UINT nEvent, LPREGISTER_EVENT_INFO* pArr, int bufSize)
{
    if (!s_info || !s_info->evt.pfnGetEventInfo)
        return 0;
    return s_info->evt.pfnGetEventInfo(nEvent, pArr, bufSize);
}

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
EXTERN_C _declspec(dllexport) int WINAPI iDraw_Event_RegisterMessage(LPCSTR guid, HWND hWnd, UINT message, pfn_Register_Message pfnBefore, pfn_Register_Message pfnAfter)
{
    if (!s_info || !s_info->evt.pfnRegister_Message)
        return 0;
    return s_info->evt.pfnRegister_Message(guid, hWnd, message, pfnBefore, pfnAfter);
}


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
EXTERN_C _declspec(dllexport) int WINAPI iDraw_Event_RegisterEvent(LPCSTR guid, UINT nEvent, LONG_PTR paramBefore, pfn_Register_Event pfnBefore, LONG_PTR paramAfter, pfn_Register_Event pfnAfter)
{
    if (!s_info || !s_info->evt.pfnRegister_Event)
        return 0;
    return s_info->evt.pfnRegister_Event(guid, nEvent, paramBefore, pfnBefore, paramAfter, pfnAfter);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      取消指定插件的所有消息回调
// 
// 参数:
//      参数1: 插件唯一标识符
// 
// 返回值:
//      返回错误码, 0=成功
EXTERN_C _declspec(dllexport) int WINAPI iDraw_UnRegister_Message(LPCSTR guid)
{
    if (!s_info || !s_info->evt.pfnUnRegister_Message)
        return 0;
    return s_info->evt.pfnUnRegister_Message(guid);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      取消指定插件的所有事件回调
// 
// 参数:
//      参数1: 插件唯一标识符
// 
// 返回值:
//      返回错误码, 0=成功
EXTERN_C _declspec(dllexport) int WINAPI iDraw_UnRegister_Event(LPCSTR guid)
{
    if (!s_info || !s_info->evt.pfnUnRegister_Event)
        return 0;
    return s_info->evt.pfnUnRegister_Event(guid);
}


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      初始化插件, 只有初始化后才能往扩展菜单里添加菜单项
// 
// 参数:
//      参数1: 插件唯一标识符
//      参数2: 插件的模块句柄
//      参数3: 插件的插件信息结构指针, 在取消初始化前必须保证这个指针的存活
//      参数4: 是否加入扩展菜单, 如果为false, 则调用 iDraw_MenuExt_Insert 会失败
//      参数5: 与视觉库通讯的回调函数
// 
// 返回值:
//      返回是否成功
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_InitPlugin(LPCSTR guid, HMODULE hModule, ETOOLS_PLUGIN_INFO* etInfo, pfn_MenuExt_Command pfnMenuExt_Command, pfn_IDraw_Callback callback)
{
    if (!s_info || !s_info->pfnPlugin_init)
        return 0;
    return s_info->pfnPlugin_init(guid, hModule, etInfo, pfnMenuExt_Command, callback);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      取消初始化插件, 把之前初始化时加入到扩展菜单的菜单项删除
// 
// 参数:
//      参数1: 插件唯一标识符
// 
// 返回值:
//      返回是否成功
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_UnInitPlugin(LPCSTR guid)
{
    if (!s_info || !s_info->pfnPlugin_UnInit)
        return 0;
    return s_info->pfnPlugin_UnInit(guid);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      把菜单项插入到扩展菜单里, 菜单点击事件由 iDraw_InitPlugin 里的回调函数接收
// 
// 参数:
//      参数1: 插件唯一标识符
//      参数2: 加入的位置, -1则加入到最后
//      参数3: 标识
//      参数4: 菜单项ID, 如果uFlags包含MF_POPUP 则这个参数是弹出式的菜单句柄
//      参数5: 菜单项目数值
// 
// 返回值:
//      返回是否成功
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_MenuExt_Insert(LPCSTR guid, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItem, LPARAM param)
{
    if (!s_info || !s_info->pfnControls)
        return 0;
    IDC_STRUCT_MENUEXT_INSERT arg = { guid, uPosition, uFlags, uIDNewItem, lpNewItem, param };
    return s_info->pfnControls(IDC_ICONTROL_INSERT_MENUEXT, 0, (LPARAM)&arg);
}


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      把函数放到易语言窗口线程里调用
// 
// 参数:
//      参数1: 函数地址, 没有返回值, 必须没有饭活着和一个整数型参数, 否则会堆栈不平衡导致崩溃
//      参数2: 传递到函数里的参数
// 
// 返回值:
//      失败返回0, 成功返回非0值
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_EWindowThreadCallback(pfn_EWindowThreadCallback pfn, LPARAM param)
{
    if (!s_info)
        return 0;
    return PostMessageA(s_info->hEWnd, s_info->uMsgEWindowThreadCallback, (WPARAM)pfn, param);
}



//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      从gdi+位图或者dx位图上生成资源返回值, 所有指针必须保证存活, 内部不会申请内存来保存传递进来的文本和位图
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
EXTERN_C _declspec(dllexport) LPWINDOW_ICON_DATA WINAPI iDraw_MakeResourceReturn(const void* pImg, int size, LPCWSTR text, LPCWSTR remarks, DWORD* crText, DWORD* crBack)
{
    if (!s_info || !s_info->pResource)
        return 0;
    return s_info->pResource->MakeResourceReturn(pImg, size, text, remarks, crText, crBack);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      释放 iDraw_MakeResourceReturn() 生成的返回值
// 
// 参数:
//      参数1: iDraw_MakeResourceReturn() 的返回值
// 
// 返回值:
//      返回是否释放成功
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_FreeResourceReturn(LPWINDOW_ICON_DATA pData)
{
    if (!s_info || !s_info->pResource)
        return 0;
    return s_info->pResource->FreeResourceReturn(pData);
}

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
EXTERN_C _declspec(dllexport) Gdiplus::Bitmap* WINAPI iDraw_LoadGpImageFromMemory(const void* pData, int size)
{
    if (!s_info || !s_info->pResource)
        return 0;
    return s_info->pResource->LoadGpImageFromMemory(pData, size);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      释放从 iDraw_LoadGpImageFromMemory() 返回的值
// 
// 参数:
//      参数1: iDraw_LoadGpImageFromMemory() 返回的值
// 
// 返回值:
//      成功返回true, 失败返回false
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_FreeGpImage(Gdiplus::Bitmap* hImg)
{
    if (!s_info || !s_info->pResource)
        return 0;
    return s_info->pResource->FreeGpImage(hImg);
}

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
//              typedef int(WINAPI* pfn_ToolDrawButton)(LPCANVAS hCanvas, int id, LPARAM param, const RECT_F* prc, STATE state, UINT dpi);
// 返回值:
//      成功返回工具条按钮ID, 失败返回0
EXTERN_C _declspec(dllexport) int WINAPI iDraw_Tool_InsertButton(LPCSTR guid, int idCommand, LPCWSTR lpszTitle, LPCWSTR lpszTips, LPARAM param, const void* pImageData, int nImageSize, pfn_ToolDrawButton pfnDraw)
{
    if (!s_info || !s_info->pfnControls)
        return 0;
    LPPLUGIN_INFO info = Plugin_GetInfo(guid);
    if (!info)
    {
        s_info->SetLastError(L"调用 iDraw_Tool_InsertButton() 时 %s 插件未初始化", _str::A2W(guid).c_str());
        return 0;
    }

    IDRAW_STRUCT_TOOL_INSERT arg = { 0 };
    arg.lpszTitle = lpszTitle;
    arg.lpszTips = lpszTips;
    arg.idCommand = idCommand;
    arg.pImageData = pImageData;
    arg.nImageSize = nImageSize;
    arg.param = param;
    arg.pfnDraw = pfnDraw;
    arg.pPlugin = info;
    return s_info->pfnControls(IDC_ICONTROL_TOOL_INSERT, (WPARAM)info, (LPARAM)&arg);
}

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
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_Tool_DeleteButton(LPCSTR guid, int idCommand)
{
    if (!s_info || !s_info->pfnControls)
        return 0;
    LPPLUGIN_INFO info = Plugin_GetInfo(guid);
    if (!info)
    {
        s_info->SetLastError(L"调用 iDraw_Tool_DeleteButton() 时 %s 插件未初始化", _str::A2W(guid).c_str());
        return 0;
    }
    return s_info->pfnControls(IDC_ICONTROL_TOOL_DELETE, (WPARAM)idCommand, (LPARAM)info) != 0;
}

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
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_Tool_GetButtonData(LPCSTR guid, int idCommand, LPWSTR* lpszTitle, LPWSTR* lpszTips, LPARAM* param, LPCVOID* pImageData, int* nImageSize, pfn_ToolDrawButton* pfnDraw)
{
    if (!s_info || !s_info->pfnControls)
        return false;
    if (!lpszTitle && !lpszTips && param && pImageData && nImageSize && !pfnDraw)
    {
        s_info->SetLastError(L"调用 " __FUNCTIONW__ " 时 未传递任何参数");
        return false;
    }
    LPPLUGIN_INFO info = Plugin_GetInfo(guid);
    if (!info)
    {
        s_info->SetLastError(L"调用 " __FUNCTIONW__ " 时 % s 插件未初始化", _str::A2W(guid).c_str());
        return false;
    }

    LPIDRAW_STRUCT_TOOL_INSERTEX data = (LPIDRAW_STRUCT_TOOL_INSERTEX)s_info->pfnControls(IDC_ICONTROL_TOOL_GETBBUTTON, (WPARAM)idCommand, (LPARAM)info);
    if (!data)
        return false;
    auto pfn_copystr = [](LPWSTR* pSet, LPCWSTR str)
    {
        if (!pSet) return;

        *pSet = 0;
        if (str)
        {
            int len = wcslen(str) * sizeof(wchar_t) + sizeof(wchar_t);
            LPWSTR ptr = (LPWSTR)iDraw_malloc(len);
            memcpy(ptr, str, len);
            *pSet = ptr;
        }
    };

    pfn_copystr(lpszTitle, data->lpszTitle);
    pfn_copystr(lpszTips , data->lpszTips);
    if (param)      *param      = data->param;
    if (pImageData) *pImageData = data->pImageData;
    if (nImageSize) *nImageSize = data->nImageSize;
    if (pfnDraw)    *pfnDraw    = data->pfnDraw;
    return true;
}


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
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_Tool_SetButtonData(LPCSTR guid, int idCommand, LPCWSTR lpszTitle, LPCWSTR lpszTips, LPARAM param, const void* pImageData, int nImageSize, pfn_ToolDrawButton pfnDraw)
{
    if (!s_info || !s_info->pfnControls)
        return 0;
    if (!lpszTitle && !lpszTips && param && pImageData && nImageSize && !pfnDraw)
    {
        s_info->SetLastError(L"调用 " __FUNCTIONW__ " 时 未传递任何参数");
        return false;
    }
    LPPLUGIN_INFO info = Plugin_GetInfo(guid);
    if (!info)
    {
        s_info->SetLastError(L"调用 " __FUNCTIONW__ " 时 % s 插件未初始化", _str::A2W(guid).c_str());
        return 0;
    }

    IDRAW_STRUCT_TOOL_INSERT arg = { 0 };
    arg.idCommand = idCommand;
    arg.pPlugin = info;
    return s_info->pfnControls(IDC_ICONTROL_TOOL_SETBBUTTON, (WPARAM)idCommand, (LPARAM)info) != 0;
}



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
EXTERN_C _declspec(dllexport) int WINAPI iDraw_EIDE_Notify(int nMsg, DWORD dwParam1, DWORD dwParam2)
{
    return s_info->pfnIDE(nMsg, dwParam1, dwParam2);
}

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
//      如果该功能被处理返回真, 未被处理则返回假
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_EIDE_RunFunctions(int nMsg, DWORD dwParam1, DWORD dwParam2)
{
    DWORD arg[] = { dwParam1, dwParam2 };
    return s_info->pfnIDE(NES_RUN_FUNC, nMsg, (DWORD)arg);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      将一个文本颜色转换成RGB颜色
// 
// 参数:
//      参数1: 默认颜色, 调用失败返回这个颜色
//      参数2: 指向颜色文本指针
// 
// 返回值:
//      返回转换好的RGB颜色值
EXTERN_C _declspec(dllexport) COLORREF WINAPI iDraw_STR2RGB(COLORREF crDef, LPCWSTR lpszColorText)
{
    return s_info->pfnControls(IDC_IDRAW_STR2RGB, crDef, (DWORD)lpszColorText);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      将一个文本颜色转换成ARGB颜色
// 
// 参数:
//      参数1: 默认颜色, 调用失败返回这个颜色
//      参数2: 指向颜色文本指针
// 
// 返回值:
//      返回转换好的ARGB颜色值
EXTERN_C _declspec(dllexport) ARGB WINAPI iDraw_STR2ARGB(ARGB crDef, LPCWSTR lpszColorText)
{
    return s_info->pfnControls(IDC_IDRAW_STR2ARGB, crDef, (DWORD)lpszColorText);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      将RGB颜色转换成文本 RGB(1,2,3)
// 
// 参数:
//      参数1: 要转换的rgb
//      参数2: 至少20个WCHAR缓冲区指针
// 
// 返回值:
//      返回写入字符数
EXTERN_C _declspec(dllexport) int WINAPI iDraw_RGB2STR(COLORREF crDef, LPWSTR lpszBuffer)
{
    return s_info->pfnControls(IDC_IDRAW_RGB2STR, crDef, (DWORD)lpszBuffer);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      将RGB颜色转换成文本 ARGB(255,1,2,3)
// 
// 参数:
//      参数1: 要转换的argb
//      参数2: 至少26个WCHAR缓冲区指针
// 
// 返回值:
//      返回写入字符数
EXTERN_C _declspec(dllexport) int WINAPI iDraw_ARGB2STR(ARGB crDef, LPWSTR lpszBuffer)
{
    return s_info->pfnControls(IDC_IDRAW_ARGB2STR, crDef, (DWORD)lpszBuffer);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取窗口数据
// 
// 参数:
//      参数1: 要获取的窗口句柄
// 
// 返回值:
//      成功返回数据指针, 失败返回0
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_GetWindowData(HWND hWnd)
{
    return (LPOBJSTRUCT)s_info->pfnControls(IDC_IDRAW_GETWINDOWDATA, 0, (DWORD)hWnd);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取代码窗口可视的矩形
// 
// 参数:
//      参数1: 接收矩形的指针(基于易语言主窗口位置)
// 
// 返回值:
//      返回是否获取成功
EXTERN_C _declspec(dllexport) BOOL WINAPI iDraw_GetCodeVisibleRect(RECT* prc)
{
    return (BOOL)s_info->pfnControls(IDC_IDRAW_GETCODEVISIBLERECT, 0, (DWORD)prc);
}


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      设置InlineHook, 内部提供两种hook方式
// 
// 参数:
//      参数1: 被hook的地址
//      参数2: 接收hook的函数地址
//      参数3: 使用的hook库, 0=detoursHook, 1=MinHook, 目前只提供这两种
// 
// 返回值:
//      返回被hook地址的原地址, 可以直接调用这个地址
EXTERN_C _declspec(dllexport) LPVOID WINAPI iDraw_InlineHook(LPVOID pOldFun, LPVOID pNewFun, int hookMode)
{
    LPVOID arg[] = { pOldFun, pNewFun };
    return (LPVOID)s_info->pfnControls(IDC_INLINEHOOK, hookMode, (DWORD)arg);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      设置InlineHook, 内部提供两种hook方式
// 
// 参数:
//      参数1: iDraw_InlineHook 的返回值
// 
// 返回值:
//      返回是否取消成功
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_UnInlineHook(LPVOID pOldFun)
{
    return s_info->pfnControls(IDC_UNINLINEHOOK, 0, (DWORD)pOldFun);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      创建有自绘样式的弹出式菜单, 所有加入到这个弹出式菜单里的子菜单都会被自绘
// 
// 参数:
//      无参数
// 
// 返回值:
//      返回弹出式菜单句柄
EXTERN_C _declspec(dllexport) HMENU WINAPI iDraw_CreatePopupMenu()
{
    return (HMENU)s_info->pfnControls(IDC_IDRAW_CREATEPOPUPMENU, (WPARAM)0, (LPARAM)0);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取IDE代码管理(IEIDE_CodeManage)类指针
// 
// 参数:
//      参数1: 代码窗口窗口句柄, 为0则获取当前窗口
// 
// 返回值:
//      返回代码窗口对应的 IEIDE_CodeManage 类指针, 失败返回0
EXTERN_C _declspec(dllexport) IEIDE_CodeManage* WINAPI iDraw_GetCodeManage(HWND hCode)
{
    return (IEIDE_CodeManage*)s_info->pfnControls(IDC_EIDE_GETCODEMANAGE, (WPARAM)0, (LPARAM)hCode);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取当前代码框句柄, 如果当前是起始页或者设计器窗口则返回0
// 
// 参数:
//      无参数
// 
// 返回值:
//      返回当前代码框句柄, 如果当前是起始页或者设计器窗口则返回0
EXTERN_C _declspec(dllexport) HWND WINAPI iDraw_GetCodeWindow()
{
    return (HWND)s_info->pfnControls(IDC_EIDE_GETCODEWINDOW, (WPARAM)0, (LPARAM)0);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取滚动条位置, 可以通过这个事件修改边框达到指定绘画滚动条位置
// 
// 参数:
//      参数1: pfn_GetScrollRect 回调函数
//      参数2: 窗口数据指针
// 
// 返回值:
//      成功返回非0, 失败返回0
EXTERN_C _declspec(dllexport) BOOL WINAPI iDraw_Scroll_GetRect(pfn_GetScrollRect pfnCallback, LPOBJSTRUCT pData)
{
    return s_info->pfnControls(IDC_SCROLL_GETRECT, (WPARAM)pfnCallback, (LPARAM)pData);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      计算滚动条位置, 可以通过这个事件修改窗口非客户区位置
// 
// 参数:
//      参数1: pfn_CalcScroll 回调函数
//      参数2: 窗口数据指针
// 
// 返回值:
//      成功返回非0, 失败返回0
EXTERN_C _declspec(dllexport) BOOL WINAPI iDraw_Scroll_CalcRect(pfn_CalcScroll pfnCallback, LPOBJSTRUCT pData)
{
    return s_info->pfnControls(IDC_SCROLL_CALCRECT, (WPARAM)pfnCallback, (LPARAM)pData);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      绘画滚动条
// 
// 参数:
//      参数1: pfn_DrawScroll 回调函数
//      参数2: 窗口数据指针
// 
// 返回值:
//      成功返回非0, 失败返回0
EXTERN_C _declspec(dllexport) BOOL WINAPI iDraw_Scroll_Draw(pfn_DrawScroll pfnCallback, LPOBJSTRUCT pData)
{
    return s_info->pfnControls(IDC_SCROLL_DRAW, (WPARAM)pfnCallback, (LPARAM)pData);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取当前MDIClient 窗口下是显示什么窗口
//      -1=没有窗口, 0=代码区, 1=设计器窗口, 2=起始页, 高位是窗口类型, 0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源
// 
// 参数:
//      无参数
// 
// 返回值:
//      返回以下值之一: -1=没有窗口, 0=代码区, 1=设计器窗口, 2=起始页, 高位是窗口类型, 0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源
EXTERN_C _declspec(dllexport) int WINAPI iDraw_GetMDIClientType()
{
    return (int)s_info->pfnControls(IDC_IMOVE_GETWINDOWTYPE, (WPARAM)0, (LPARAM)0);
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取当前MDIClient 窗口下是显示什么窗口类型
//      0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源
// 
// 参数:
//      无参数
// 
// 返回值:
//      返回以下值之一: 0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源
EXTERN_C _declspec(dllexport) int WINAPI iDraw_GetMDIClientCodeType()
{
    return (int)s_info->pfnControls(IDC_IMOVE_GETCODETYPE, (WPARAM)0, (LPARAM)0);
}


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取滚动条对象数据
// 
// 参数:
//      参数1: 滚动条对象指针
//      参数2: 接收滚动条数据指针, 调用前需要把 IDRAW_STRUCT_SCROLL_DATA.cbSize 赋值
// 
// 返回值:
//      返回是否成功
EXTERN_C _declspec(dllexport) BOOL WINAPI iDraw_GetScrollData(LPVOID pScroll, IDRAW_STRUCT_SCROLL_DATA* ptr)
{
    return (BOOL)s_info->pfnControls(IDC_GETSCROLLDATA, (WPARAM)pScroll, (LPARAM)ptr);
}
#endif