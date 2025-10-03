#pragma once
#include <IDraw_Header.h>

// 资源模块
class EWindowResource : public IEWindowResource
{
public:

    // 获取窗口主题数据
    virtual LPBYTE GetELogo(DWORD* size);

    // 获取默认主题配色和clr文件, 返回文件数据, 长度+数据, 目前一共有6个文件
    virtual LPBYTE GetDefThemeData(DWORD* size);

    void SetModule(HINSTANCE hInst);


    // 获取窗口主题数据
    virtual LPSYSBUTTONIMAGE GetNskn();

    // 获取工具条图标, pIconType = 是否是从默认值里得到的图标, 仅在调用成功时赋值
    // pIconType = 接收获取到的图标来源, 0=用户图标, 1=用户默认, 2=默认皮肤里图标, 3=默认里的默认图标
    virtual LPWINDOW_ICON_DATA Tool_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96);

    // 获取支持库树型框图标, pIconType = 是否是从默认值里得到的图标, 仅在调用成功时赋值
    // pIconType = 接收获取到的图标来源, 0=用户图标, 1=用户默认, 2=默认皮肤里图标, 3=默认里的默认图标
    virtual LPWINDOW_ICON_DATA TreeFne_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96);

    // 获取程序树型框图标, 返回图标数据, pIconType = 是否是从默认值里得到的图标, 仅在调用成功时赋值
    // pIconType = 接收获取到的图标来源, 0=用户图标, 1=用户默认, 2=默认皮肤里图标, 3=默认里的默认图标
    virtual LPWINDOW_ICON_DATA TreeProgram_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96);

    // 获取组件箱图标, pIconType = 是否是从默认值里得到的图标, 仅在调用成功时赋值
    // pIconType = 接收获取到的图标来源, 0=用户图标, 1=用户默认, 2=默认皮肤里图标, 3=默认里的默认图标
    virtual LPWINDOW_ICON_DATA TreeControl_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96);

    // 获取树型框加减号图标, pIconType = 是否是从默认值里得到的图标, 仅在调用成功时赋值
    // pIconType = 接收获取到的图标来源, 0=用户图标, 1=用户默认, 2=默认皮肤里图标, 3=默认里的默认图标
    virtual LPWINDOW_ICON_DATA TreeExpand_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96);

    // 获取组合框图标, pIconType = 是否是从默认值里得到的图标, 仅在调用成功时赋值
    // pIconType = 接收获取到的图标来源, 0=用户图标, 1=用户默认, 2=默认皮肤里图标, 3=默认里的默认图标
    virtual LPWINDOW_ICON_DATA Combobox_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96);

    // 获取菜单项的图标, pIconType = 是否是从默认值里得到的图标, 仅在调用成功时赋值
    // pIconType = 接收获取到的图标来源, 0=用户图标, 1=用户默认, 2=默认皮肤里图标, 3=默认里的默认图标
    virtual LPWINDOW_ICON_DATA Menu_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96);

    // 获取左右调节器的图标, pIconType = 是否是从默认值里得到的图标, 仅在调用成功时赋值
    // pIconType = 接收获取到的图标来源, 0=用户图标, 1=用户默认, 2=默认皮肤里图标, 3=默认里的默认图标
    virtual LPWINDOW_ICON_DATA MsctlsUpdowm32_GetIcon_lr(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96);

    // 获取上下调节器的图标, pIconType = 是否是从默认值里得到的图标, 仅在调用成功时赋值
    // pIconType = 接收获取到的图标来源, 0=用户图标, 1=用户默认, 2=默认皮肤里图标, 3=默认里的默认图标
    virtual LPWINDOW_ICON_DATA MsctlsUpdowm32_GetIcon_tb(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96);

    // 获取顶部状态条的按钮图标, pIconType = 是否是从默认值里得到的图标, 仅在调用成功时赋值
    // pIconType = 接收获取到的图标来源, 0=用户图标, 1=用户默认, 2=默认皮肤里图标, 3=默认里的默认图标
    virtual LPWINDOW_ICON_DATA TopState_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96);

    // 从常用图标里获取图标, 返回图标数据, 没有这个图标返回0
    virtual LPWINDOW_ICON_DATA GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96);


    // 释放资源
    virtual void EWindow_Resource_Free();

    // 从文件加载资源, 返回资源句柄, 加载会影响效率, 一个ext文件加载一次就行, 销毁时会自动销毁
    virtual IDRAW_HRESOURCE LoadFromFile(LPCWSTR szFile);
    // 从内存加载资源, 返回资源句柄, 加载会影响效率, 一个ext文件加载一次就行, 销毁时会自动销毁
    virtual IDRAW_HRESOURCE LoadFromMemory(const void* pData, int size);

    // 从用户加载的资源里获取指定ID的图标, 返回id指向的数据
    virtual LPWINDOW_ICON_DATA User_GetIcon(IDRAW_HRESOURCE hResource, LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96);


    // 取消插件所有绑定窗口资源的回调
    virtual bool UnBindWindow(LPCSTR guid);

    // 获取默认组件的资源句柄, 传递窗口句柄获取
    // hWnd = 要获取资源的窗口句柄, 获取菜单使用 HRESOURCE_MENU, 获取树型框展开收缩按钮使用 HRESOURCE_TREEEXPAND
    virtual IDRAW_HRESOURCE GetDefControlIcon(HWND hWnd);

    // 将资源绑定到窗口上, 下次重画这个窗口时会从这个资源获取图标
    // hWnd = 要绑定的窗口句柄, 绑定菜单使用 HRESOURCE_MENU, 绑定树型框展开收缩按钮使用 HRESOURCE_TREEEXPAND
    // hResource = 加载的资源句柄, 如果这个参数为0, 且最后一个参数为0, 则不会加载设定的图标
    // callback = 回调函数, 当这个窗口需要加载图标时调用, 为0则使用默认的资源加载, 如果没有默认的资源则不加载
    //              回调函数里需要根据是否为dx渲染来写入 prc 和返回值
    virtual bool BindWindow(LPCSTR guid, HWND hWnd, IDRAW_HRESOURCE hResource, pfn_GetWindowIcon callback);


    // 从窗口句柄获取回调函数, 可以调用回调函数来获取图标, 返回写入缓冲区的数量, bufCallback 为0则返回需要的缓冲区成员数
    // hWnd = 从哪个窗口上获取回调信息
    // bufCallback = 接收绑定时传递的回调函数数组, 如果为0则返回绑定的数量
    // bufCount = 表示 bufCallback 这个缓冲区有多少个成员
    virtual int GetResourceCallbackFromHwnd(HWND hWnd, BINDWINDOW_INFO* bufCallback = 0, int bufCount = 0);

    // 从窗口句柄里获取图标, 如果在绑定的时候给了回调函数, 则会调用回调函数处理
    // 内部调用 GetResourceFromHwnd() + User_GetIcon()/User_GetIconDx() 来实现
    // hWnd = 绑定的窗口句柄, 如果没有绑定则返回0
    // pszID = 要取的图标ID, 这里一般是项目文本
    // arg = 这个根据不同组件传递不同结构, 树型框传递 TVITEMEXW结构指针, 
    // prc = 接收图标在图片里的位置
    // state = 图片状态, STATE 枚举常量里的值, 目前只支持 STATE::HOT, STATE::DOWN, STATE::BAN, STATE::NORMAL 这4个
    // pIconType = 接收图标来源, 0=从ID中获取, 1=从默认图标里获取
    virtual LPWINDOW_ICON_DATA GetIconFromHwnd(HWND hWnd, LPCWSTR pszID, void* arg, RECT_F* prc, STATE state, UINT dpi = 96);

    // 从gdi+位图或者dx位图上生成资源返回值, 所有指针必须保证存活, 内部不会申请内存来保存传递进来的文本和位图
    // pImg = 图片数据地址
    // pImgSize = 图片数据大小
    // text = 绘制当前项目显示的文本, 不会修改原组件文本, 只是绘画时显示这个文本, 可以为0
    // remarks = 备注文本, 可以为0
    // crText = 文本ARGB颜色, 可以为0, 不为0则必须是4个成员, 分别是 正常, 热点, 按下, 禁止
    // crBack = 背景ARGB颜色, 可以为0, 不为0则必须是4个成员, 分别是 正常, 热点, 按下, 禁止
    virtual LPWINDOW_ICON_DATA MakeResourceReturn(const void* pImg, int pImgSize, LPCWSTR text, LPCWSTR remarks, DWORD* crText, DWORD* crBack);

    // 释放 MakeResourceReturn() 生成的返回值
    virtual bool FreeResourceReturn(LPWINDOW_ICON_DATA pData);

    // 从内存加载gdi+图片
    // pData = 图片数据
    // size = 图片数据大小
    virtual Gdiplus::Bitmap* LoadGpImageFromMemory(const void* pData, int size);

    // 释放从 LoadGpImageFromMemory() 返回的值
    virtual bool FreeGpImage(Gdiplus::Bitmap*);

    // 获取marlett字体, 图标字体, 绘画三角用
    virtual HFONT GetMarlettFont(int size);

    // 获取字体数据
    virtual LPBYTE GetMarlettData(int* size);
};

