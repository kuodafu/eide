// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "framework.h"
#include <control/CTreeView.h>
#include "EWindow_Fne_Header.h"
#include <iDraw_Version.h>

#include "EMenu.h"
#include <EIDE_Interface.h>
#include <map>
#include "CIDC_Fun.h"
#include <shlwapi.h>
#include <control/draw_Window.h>


static CIDC_Fun g_idc;
static PLUGIN_INFO pluginInfo;
static CDrawWindow m_drawWindow;    // 绘画窗口边框, 标题, 图标, 系统按钮
static HINSTANCE m_hInst;
// 把所有已经子类化的窗口绘画信息更新一下
void Reload_SubclassDrawInfo();



int _nl_ide_ready();

// 配置改变
int _ine_Control_Changed(LPIDRAW_INFO info);

// 设置hook, wParam=hook方式, 0=detoursHook, 1=MinHook, lParam=[被hook地址, 接收地址], 返回原地址
int _idc_InlineHook(LPVOID pOldFun, LPVOID pNewFun, int hookMode);
int _idc_UnInlineHook(LPVOID pOldFun);
int _idc_UnInlineHook_Plugin(LPBYTE pBegin, LPBYTE pEnd);

// 获取代码窗口可视的矩形, wParam保留, lParam = 接收矩形的指针(基于易语言主窗口位置)
int _idc_GetCodeVisibleRect(RECT* prc);
int _idc_Scroll_cgetrect(pfn_GetScrollRect pfn, LPOBJSTRUCT pData);
int _idc_Scroll_calcrect(pfn_CalcScroll pfn, LPOBJSTRUCT pData);
int _idc_Scroll_draw(pfn_DrawScroll pfn, LPOBJSTRUCT pData);
int _idc_GetScrollData(IDrawScroll* pObj, IDRAW_STRUCT_SCROLL_DATA* data);
int iDraw_AddBtn_BindEvent(int code, IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT* arg);
int iDraw_BindWindow(HWND hWnd, LPDRAW_WINDOW_ARGUMENT arg);
int iDraw_CreateWindow(LPCREATESTRUCTW c, LPDRAW_WINDOW_ARGUMENT arg);
int iDraw_AddBtn_BindEvent(int code, IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT* arg);
int iDraw_WindowProc(IDRAW_SYSBUTTON_ICON_TITLE_WINDOWPROC_STRUCT* arg);
int iDraw_GetDefArgument(HWND hWnd, LPDRAW_WINDOW_ARGUMENT arg);



//IDRAW_INFO* s_info;
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        m_hInst = hModule;
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}



class iDraw : public IiDraw
{
public:
    virtual int Release()
    {
        return 0;
    }
    virtual int Addref()
    {
        return 0;
    }


public:

    // 子类化窗口, 内部使用固定的配色, 需要单独修改颜色的可以返回值的指针进行替换
    // hWnd = 被子类化的窗口句柄
    // proc = 接收子类化的回调函数
    // reMoveBorder = 是否删除边框, 会删除 WS_BORDER 和 扩展样式几个会添加边框的样式
    // isSetBack = 是否设置背景颜色, 如果设置, 那么相同类名绘画背景时都会使用这个颜色绘画, 包括设计器窗口也一样
    // reMoveClassStyle = 要删除的类样式, 删除这个样式后所有这个类名的窗口都会被删除这个样式
    virtual LPOBJSTRUCT _subclass(HWND hWnd, pfn_SubClassWndProc proc, bool reMoveBorder = true, bool isSetBack = true, DWORD reMoveClassStyle = CS_HREDRAW | CS_VREDRAW)
    {
        return ::_subclass(hWnd, proc, reMoveBorder, isSetBack, reMoveClassStyle);
    }
    


    virtual LPOBJSTRUCT SubButton(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
    {
        return ::_subclass_Button(hWnd, reMoveBorder, isSetBack, reMoveClassStyle);
    }
    virtual LPOBJSTRUCT SubEdit(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, bool isScroll = true)
    {
        return ::_subclass_Edit(hWnd, reMoveBorder, isSetBack, reMoveClassStyle, isScroll);
    }
    virtual LPOBJSTRUCT SubStatic(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
    {
        return ::_subclass_Static(hWnd, reMoveBorder, isSetBack, reMoveClassStyle);
    }
    virtual LPOBJSTRUCT SubStatusBar(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
    {
        return ::_subclass_statusBar(hWnd, reMoveBorder, isSetBack, reMoveClassStyle);
    }

    // 重画树型框
    // hWnd = 树型框窗口句柄
    // iItemHeight = 项目高度
    // isScroll = 是否重画滚动条
    // isHot = 是否有热点效果
    virtual LPOBJSTRUCT SubTreeView(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int ItemHeight, bool isScroll = true, bool isHot = false)
    {
        return ::_subclass_tree(hWnd, reMoveBorder, isSetBack, reMoveClassStyle, ItemHeight, isScroll, isHot);
    }

    virtual LPOBJSTRUCT SubListBox(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int nItemHeight, bool isScroll = true, bool isHot = false)
    {
        return ::_subclass_ListBox(hWnd, reMoveBorder, isSetBack, reMoveClassStyle, nItemHeight, isScroll, isHot);
    }
    virtual LPOBJSTRUCT SubListView(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int nItemHeight, bool isScroll = true, bool isHot = false, int fmt = DT_VCENTER | DT_SINGLELINE)
    {
        return ::_subclass_listView(hWnd, reMoveBorder, isSetBack, reMoveClassStyle, nItemHeight, isScroll, isHot, fmt);
    }

    virtual LPOBJSTRUCT SubTool(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
    {
        return ::_subclass_tool(hWnd, reMoveBorder, isSetBack, reMoveClassStyle);
    }

    virtual LPOBJSTRUCT Submsctls_updown32(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
    {
        return ::_subclass_msctls_updown32(hWnd, reMoveBorder, isSetBack, reMoveClassStyle);
    }

    virtual LPOBJSTRUCT SubTab(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
    {
        return ::_subclass_Tab(hWnd, reMoveBorder, isSetBack, reMoveClassStyle);
    }
    virtual LPOBJSTRUCT SubComboBox(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int nItemHeight)
    {
        return ::_subclass_combobox(hWnd, reMoveBorder, isSetBack, reMoveClassStyle, nItemHeight);
    }
    static LRESULT CALLBACK _scroll_callback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
    {
        IDrawScroll* pObj = (IDrawScroll*)pData->drawScroll;
        if (pObj && s_info->MY_MF_OWNERDRAW)
        {
            LRESULT ret = 0;
            if (pObj->OnWndProc(pData->oldProc, hWnd, message, wParam, lParam, &ret))
            {
                if (message == WM_HSCROLL)
                    InvalidateRect(hWnd, 0, 0);
                return ret;
            }
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    virtual LPVOID DrawScroll(HWND hWnd, int nListViewItemHeight, bool isTreeView, bool isCallSetScrollInfo)
    {
        COLORREF crBorder = s_info->defData->draw.color.crBorder;
        LPOBJSTRUCT pData = GetDataFromHwnd(hWnd);
        if (pData)
            crBorder = pData->draw.color.crBorder;

        IDrawScroll* pObj = IDrawScroll::GetobjFromHwnd(hWnd);

        if (!pObj && s_info->MY_MF_OWNERDRAW)
        {
            LPOBJSTRUCT pData = ::_subclass(hWnd, _scroll_callback, false, false, 0);
            std::map<HWND, OBJSTRUCT>& map = ::_get_subclass_allWindow();
            OBJSTRUCT& data = map[hWnd];

            pObj = ScrollCreate(hWnd, crBorder,
                                0,          // m_isSysListView, 如果是超级列表框的话这个值就是超级列表框的表项高度
                                false,      // m_isSysTreeView, 是否为树型框, 如果为真, 则先设置滚动条位置, 后发送滚动消息, 为假则先发送滚动消息后设置位置
                                false,      // m_isCallSetScroll, 是否调用原来的设置滚动条位置函数
                                true,      // isSubClass, 是否子类化, 如果为true, 内部会子类化这个窗口, 如果为假, 则必须外部把消息传递到 OnWndProc 方法里
                                true        // isReCalcSize, 是否重新计算尺寸, 为true则内部会调用 SetWindowPos 进行重新计算非客户区
            );
            if (pObj)
            {            //pData->drawScroll = pObj;
                data.drawScroll = pObj;
                data.hWnd = hWnd;
            }
        }
        if (pObj)
        {
            pObj->m_isSysTreeView = isTreeView;
            pObj->m_isSysListView = nListViewItemHeight;
            pObj->m_isCallSetScroll = isCallSetScrollInfo;
        }
        return pObj;
    }
    // 子类化菜单句柄所在的窗口, 接收WM_DRAWITEM消息, 如果已经子类化, 则不处理
    virtual LPOBJSTRUCT _subclass_hMenu_Window(HWND hWnd, pfn_SubClassWndProc proc)
    {
        return ::_subclass_hMenu_Window(hWnd, proc);
    }

    virtual LPOBJSTRUCT _subclass_EWindow(HWND hWnd, HINSTANCE hInst, LPSYSBUTTONIMAGE pNskn)
    {
        return ::_subclass_EWindow(hWnd, hInst, pNskn);
    }



    // 子类化易语言代码中间的那个自绘选择夹
    virtual LPOBJSTRUCT _subclass_CustomTab(HWND hWnd)
    {
        return ::_subclass_CustomTab(hWnd);
    }
    // 初始化工作夹的选择夹, 修改选择夹位置, 设置选择框索引
    // hTab = 被初始化的选择夹, hTabOld = 易语言自带的选择夹, isMove = 为真则只移动窗口, 忽略第二个参数
    virtual void _Tab_Work_init(HWND hTab, bool isMove)
    {
        ::_Tab_Work_init(hTab, isMove);
    }
    // 初始化状态夹的选择夹, 修改选择夹位置, 设置选择框索引
    // hTab = 被初始化的选择夹, hTabOld = 易语言自带的选择夹, isMove = 为真则只移动窗口, 忽略第二个参数
    virtual void _Tab_State_init(HWND hTab, bool isMove)
    {
        ::_Tab_State_init(hTab, isMove);
    }


    // hook消息即将到达窗口
    virtual void EWindow_Hook_SetWindowsHook()
    {
        ::EWindow_Hook_SetWindowsHook();
    }


    // 创建画刷, 返回的画刷不能释放, 要释放只能在支持库被卸载或者关闭易语言时调用本对象的 Release() 释放
    // 相同颜色不会创建多次, 内部使用 map 记录所有创建的画刷
    virtual HBRUSH Brush_Create(COLORREF cr)
    {
        return s_info->pfnCreateBrush(cr);
    }


    inline static void WINAPI _tool_1_crChange(LPOBJSTRUCT pData)
    {
        pData->draw.color.crBackGround = s_info->themeColor->crBackExtern();
        pData->draw.color.hbrBackGround = s_info->pfnCreateBrush(pData->draw.color.crBackGround);
    }
    virtual LPOBJSTRUCT _subclass_tool(HWND hWnd, pfn_SubClassWndProc proc, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
    {
        LPOBJSTRUCT pData = _subclass(hWnd, proc, reMoveBorder, false, reMoveClassStyle);
        if (!pData)return 0;
        _tool_1_crChange(pData);
        pData->fnCrChange = _tool_1_crChange;
        if (isSetBack)
            SetClassLongPtrW(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)pData->draw.color.hbrBackGround);
        return pData;
    }



    // 更新当前激活的窗口, 会重画激活的这个窗口和之前激活的窗口
    virtual void TopState_UpdateStateWindow(HWND hWnd)
    {
        ::TopState_UpdateStateWindow(hWnd);
    }
    // 获取当前激活的窗口
    virtual HWND TopState_GetActivateWindow()
    {
        return ::TopState_GetActivateWindow();
    }

    virtual void TopState_SetWindowText(HWND hWnd, LPCWSTR lpszText, bool isUpdate)
    {
        ::TopState_SetWindowText(hWnd, lpszText, isUpdate);
    }




    // 加载易语言的菜单, id必须是易语言的菜单id
    //127,    // 顶部菜单栏的菜单, 一大堆顶级菜单+子级菜单
    //235,    // 其他地方右键菜单, 各种对齐工具条, 顶边菜单  查看->工具条
    //236,    // 代码区 右键菜单
    //237,    // 支持库 -> 树型框 -> 右键菜单
    //238,    // 程序 -> 树型框 -> 模块引用表 右键菜单, 除了模块/外部文件记录表, 其他地方右键菜单也是238, 不过父菜单项不同
    //239,    // 程序 -> 树型框 -> 程序数据 右键菜单
    //240,    // 窗口设计器右键菜单
    //276,    // 运行时 代码区 右键菜单
    //293,    // 左边易语言属性框 -> 属性栏右键菜单
    //587,    // 底边状态夹 -> 监视表右键
    //606,    // 代码切换选择夹右键菜单
    //609,    // 书签右键菜单
    //610,    // 常量右键菜单
    //611,    // 底边状态夹 -> 剪辑历史
    //626,    // 程序 -> 分类文件夹右键菜单
    //627,    // 程序 -> 树型框 -> 外部文件记录表 右键菜单
    //1,      // 编辑框右键菜单
    //12,     // 编辑框右键菜单 多了个打开/关闭输入法
    virtual HMENU EMenu_LoadMenu(int id)
    {
        return LoadMenuW(GetModuleHandleW(0), MAKEINTRESOURCEW(id));
    }

    // 弹出菜单
    virtual BOOL EMenu_PopUpMenu(HWND hWnd, HMENU hMenu, int x = 0, int y = 0)
    {
        POINT pt;
        GetCursorPos(&pt);
        if (x == 0) x = pt.x;
        if (y == 0) y = pt.y;
        return TrackPopupMenu(hMenu, TPM_LEFTALIGN, x, y, 0, hWnd, 0);
        //return TrackPopupMenuEx(hMenu, TPM_VERTICAL, x, y, hWnd, 0);
    }

private:
    // 判断子类化返回的结果是否有效, 无效就输出, 有效就加入传递的样式, 返回指针是否有效
    // _data = 子类化返回的值
    // _hWnd = 显示到输出里的数据, s_info->eWnd->xxxx()
    // _style = 要添加的样式, 为0则不处理
#define _SUBCLASS_DBG(_data, _hWnd, _style) _cmp_insert_style(_data, _hWnd, L###_hWnd, _style, __LINE__, __FUNCTIONW__, __FILEW__)
    inline bool _cmp_insert_style(LPOBJSTRUCT pData, HWND hWnd, LPCWSTR hWndStr, DWORD style, int line, LPCWSTR funname, LPCWSTR file)
    {
        if (!pData)
        {
            wstr::debug(L"error 行: %d, 函数名: %s, 文件: %s\n %s = %d 子类化失败, ", line, funname, file, hWndStr, hWnd);
            return false;
        }
        if (style)
            InsertStyle(pData->hWnd, style);
        return true;
    };


public:

    // 处理顶部菜单栏里的控件, 去除组件边框, 子类化各个组件
    virtual void _subclass_menu()
    {
        LPOBJSTRUCT data;
        data = _subclass_tool(s_info->eWnd->GetMenu(), WndProc_OncePaint, true, true, 0);         // 菜单栏容器
        if (!_SUBCLASS_DBG(data, s_info->eWnd->GetMenu(), 0))
            return;

        ShowWindowEx(data->hWnd, SW_HIDE);
        //ShowWindowEx(data->hWnd, SW_HIDE);

        data = _subclass_tool(s_info->eWnd->GetMenuMenu(), WndProc_MenuMenu, true, true, 0);      // 显示菜单的组件, 菜单背景需要处理
        if (_SUBCLASS_DBG(data, s_info->eWnd->GetMenuMenu(), 0))
        {
            //data->draw.isNoNcPaint = true;
            data->draw.isNcCalcSize = true;
            ShowWindowEx(s_info->eWnd->GetMenuMenu(), SW_HIDE);
        }

        HWND hWndTool = s_info->eWnd->GetMenuStandard();
        //SetWindowLongA(hWndTool, GWLP_ID, 9392);
        // 新增3个按钮, 一个后退, 一个前进, 一个分割线
        {
            TBBUTTON tb = { 0 };
            tb.iBitmap = 0;
            tb.idCommand = TOOL_STANDARD_BACK;
            tb.fsState = TBSTATE_ENABLED;
            tb.fsStyle = BTNS_BUTTON;
            BOOL isOk = SendMessageW(hWndTool, TB_INSERTBUTTONW, 0, (LPARAM)&tb);

            tb.idCommand = TOOL_STANDARD_FORWARD;
            isOk = SendMessageW(hWndTool, TB_INSERTBUTTONW, 1, (LPARAM)&tb);

            tb.fsStyle = BTNS_SEP;
            tb.idCommand = 0;
            isOk = SendMessageW(hWndTool, TB_INSERTBUTTONW, 2, (LPARAM)&tb);

            int index = 20;
            tb.fsStyle = BTNS_BUTTON;

            tb.idCommand = TOOL_STANDARD_RESTART;           // 重新启动
            tb.fsState = TBSTATE_INDETERMINATE;
            isOk = SendMessageW(hWndTool, TB_INSERTBUTTONW, ++index, (LPARAM)&tb);
            tb.fsState = TBSTATE_ENABLED;


            //tb.fsState = TBSTATE_ENABLED;
            //tb.idCommand = TOOL_STANDARD_BUILD;             // 编译
            //isOk = SendMessageW(hWndTool, TB_INSERTBUTTONW, ++index, (LPARAM)&tb);

            //TODO: 这里需要黑月
            if (s_info->menuDoc.menu.size() > 0)
            {
                HMENU hMenu = s_info->menuDoc.menu[6].hMenu;
                if (hMenu)
                {
                    // 0x9001 36865
                    MENUITEMINFOW item;
                    item.cbSize = sizeof(item);
                    item.fMask = MIIM_STATE;
                    if (GetMenuItemInfoW(hMenu, 0x9001, FALSE, &item))
                    {
                        tb.idCommand = TOOL_STANDARD_BUILD_BLACKMOON;   // 黑月编译
                        isOk = SendMessageW(hWndTool, TB_INSERTBUTTONW, ++index, (LPARAM)&tb);
                    }

                }
            }

            tb.idCommand = TOOL_STANDARD_BUILD_STATIC;      // 静态编译
            isOk = SendMessageW(hWndTool, TB_INSERTBUTTONW, ++index, (LPARAM)&tb);

            //tb.idCommand = TOOL_STANDARD_BUILD_DULI;        // 独立编译
            ////tb.idCommand = 0xFFFF1234;        // 独立编译
            //isOk = SendMessageW(hWndTool, TB_INSERTBUTTONW, ++index, (LPARAM)&tb);


            tb.fsStyle = BTNS_SEP;
            tb.idCommand = 0;
            isOk = SendMessageW(hWndTool, TB_INSERTBUTTONW, ++index, (LPARAM)&tb);
        }

        // 删除工具条那几个调整布局的按钮
        {
            SendMessageW(hWndTool, TB_DELETEBUTTON, 14, 0);
            SendMessageW(hWndTool, TB_DELETEBUTTON, 14, 0);
            SendMessageW(hWndTool, TB_DELETEBUTTON, 14, 0);
            SendMessageW(hWndTool, TB_DELETEBUTTON, 14, 0);
        }



        LPOBJSTRUCT normalTool = ::_subclass_tool(hWndTool, false, false, 0);    // 标准工具条
        if (_SUBCLASS_DBG(normalTool, s_info->eWnd->GetMenuStandard(), 0))
        {

            s_info->hWndTool = CreateWindowExW(normalTool->styleEx, TOOLBARCLASSNAMEW, L"接口工具条",
                normalTool->style | WS_VISIBLE, 0, 0, 0, 0, s_info->hEWnd, 0, 0, 0);
            SendMessageW(s_info->hWndTool, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
            SendMessageW(s_info->hWndTool, TB_SETBUTTONSIZE, 0, SendMessageW(normalTool->hWnd, TB_GETBUTTONSIZE, 0, 0));

            
            if (0)  // 测试添加按钮
            {
                IDRAW_STRUCT_TOOL_INSERT asdkj = { 0 };
                asdkj.pPlugin = &pluginInfo;
                asdkj.idCommand = 1232131;
                asdkj.lpszTitle = L"这个是标题";
                asdkj.lpszTips = L"这个是提示文本\n支持\\n换行\r\n也支持\\r\\n换行";
                static auto pfn_draw = [](LPCANVAS hCanvas, int id, LPARAM param, const RECT_F* prc, STATE state, UINT dpi) -> int
                {
                    RECT_F rcIconDraw;
                    auto img = s_info->pResource->Tool_GetIcon(L"32854", &rcIconDraw, STATE::NORMAL, dpi);
                    if (img && img->hBitmapDx)
                    {
                        const float cxItem = (float)(int)prc->width();
                        const float cyItem = (float)(int)prc->height();
                        const float cxIcon = (float)(int)rcIconDraw.width();
                        const float cyIcon = (float)(int)rcIconDraw.height();

                        RECT_F rcDest;
                        rcDest.left = (float)(int)(prc->left + (cxItem - cxIcon) / 2);
                        rcDest.top = (float)(int)(prc->top + (cyItem - cyIcon) / 2);
                        rcDest.right = rcDest.left + cxIcon;
                        rcDest.bottom = rcDest.top + cyIcon;

                        if (state == STATE::HOT)
                            hCanvas->FillRectangle(s_info->pfnCreateBrushDx(RGB2ARGB(65535, 255)), prc);
                        else if (state == STATE::DOWN)
                            hCanvas->FillRectangle(s_info->pfnCreateBrushDx(RGB2ARGB(255, 255)), prc);

                        hCanvas->DrawImage(img->hBitmapDx, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom,
                            rcIconDraw.left, rcIconDraw.top, rcIconDraw.right, rcIconDraw.bottom, 255);
                        return 1;
                    }
                    return 0;
                };
                asdkj.pfnDraw = pfn_draw;

                Tool_InsertButton((LPIDRAW_STRUCT_TOOL_INSERT)&asdkj);
            }

            ::_subclass_tool(s_info->hWndTool, false, false, 0);    // 新建的工具条

            normalTool->draw.isNcCalcSize = true;
            //normalTool->draw.isNoNcPaint = true;
            //HWND hParentTool = GetParent(hWndTool);
            //int indexToolButton = 0;
            //{
            //    DWORD styleTool = GetWindowLongPtrW(hWndTool, GWL_STYLE);
            //    DWORD styleEx = GetWindowLongPtrW(hWndTool, GWL_EXSTYLE);
            //    DWORD styleExTool = SendMessageW(hWndTool, TB_GETEXTENDEDSTYLE, 0, 0);

            //    int id = GetDlgCtrlID(hWndTool);
            //    s_info->hMenu_Standard = CreateWindowExW(styleEx, L"ToolbarWindow32", L"新建标准工具条", styleTool, 0, 0, 1110, 30, s_info->hEWnd, (HMENU)id, 0, 0);
            //    SendMessageW(s_info->hMenu_Standard, TB_SETEXTENDEDSTYLE, 0, 0);
            //    LPOBJSTRUCT pNewData = ::_subclass_tool(s_info->hMenu_Standard, false, false, 0);    // 标准工具条
            //    pNewData->draw.isNcCalcSize = true;
            //    int count = SendMessageW(hWndTool, TB_BUTTONCOUNT, 0, styleExTool);
            //    
            //    TBBUTTON tb = { 0 };
            //    for (int i = 0; i < count; i++)
            //    {
            //        SendMessageW(hWndTool, TB_GETBUTTON, i, (LPARAM)&tb);
            //        tb.iBitmap = 0;
            //        tb.idCommand = TOOL_STANDARD_BACK + i;
            //        tb.fsState = TBSTATE_ENABLED;
            //        tb.fsStyle = BTNS_BUTTON;
            //        tb.dwData = 1;
            //        tb.dwData = SendMessageW(s_info->hMenu_Standard, TB_INSERTBUTTONW, 0, (LPARAM)&tb);
            //        tb.dwData = SendMessageW(s_info->hMenu_Standard, TB_INSERTBUTTONW, indexToolButton++, (LPARAM)&tb);
            //    }
            //}
        }

        hWndTool = s_info->eWnd->GetMenuAlign();
        LPOBJSTRUCT AlignTool = ::_subclass_tool(hWndTool, false, false, 0);      // 对齐工具条
        ShowWindowEx(hWndTool, SW_HIDE);
        if (AlignTool && _SUBCLASS_DBG(AlignTool, s_info->eWnd->GetMenuAlign(), 0))
        {
            AlignTool->draw.isNcCalcSize = true;
            AlignTool->draw.isNoNcPaint = true;
        }

        if (normalTool)
        {
            int indexToolButton = SendMessageW(normalTool->hWnd, TB_BUTTONCOUNT, 0, 0);
            int count = SendMessageW(hWndTool, TB_BUTTONCOUNT, 0, 0);
            TBBUTTON tb = { 0 };
            for (int i = 0; i < count; i++)
            {
                SendMessageW(hWndTool, TB_GETBUTTON, i, (LPARAM)&tb);
                tb.fsState = i == 0 ? tb.fsState : TBSTATE_INDETERMINATE;
                tb.dwData = SendMessageW(normalTool->hWnd, TB_INSERTBUTTONW, indexToolButton++, (LPARAM)&tb);
            }
            //SendMessageW(s_info->hMenu_Standard, TB_GETITEMRECT, indexToolButton - 1, (LPARAM)&s_info->rcWndMenu);  // 取最后一个按钮的右边
        }

        if (!IsWindow(s_info->eWnd->GetMenuLocation())) return;    // 如果没有标准工具条就返回不处理
        //ShowWindowEx(s_info->eWnd->GetMenuLocation(), SW_HIDE);

        //data = _subclass_tool(s_info->eWnd->GetMenuLocation(), WndProc_ToolBar);   // 定位工具条
        SetWindowPos(s_info->eWnd->GetMenuLocation(), HWND_BOTTOM, -5000, -5000, 0, 0, SWP_NOSIZE | SWP_FRAMECHANGED | SWP_HIDEWINDOW);

        bool isShowLocationTool = s_info->config->IsShowLocationTool();
        if (data && !isShowLocationTool)
            ShowWindowEx(data->hWnd, SW_HIDE);

        data = _subclass_combobox(s_info->eWnd->GetMenuComboBox1(), false, false, 0, 24);   // 定位工具条左边的组合框
        if (_SUBCLASS_DBG(data, s_info->eWnd->GetMenuComboBox1(), 0))
        {
            data->SetWndLong(data->hWnd, GWL_STYLE, data->style | WS_CLIPSIBLINGS);
            SetParent(data->hWnd, s_info->hEWnd);
            //if (!isShowLocationTool)
            //    ShowWindowEx(data->hWnd, SW_HIDE);
            ShowWindowEx(data->hWnd, SW_HIDE);
        }

        //SetWindowPos(data->hWnd, 0, 0, -100, 0, 0, SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOZORDER);

        data = _subclass_combobox(s_info->eWnd->GetMenuComboBox2(), false, false, 0, 24);   // 定位工具条右边的组合框
        if (_SUBCLASS_DBG(data, s_info->eWnd->GetMenuComboBox2(), 0))
        {
            data->SetWndLong(data->hWnd, GWL_STYLE, data->style | WS_CLIPSIBLINGS);
            SetParent(data->hWnd, s_info->hEWnd);
            //if (!isShowLocationTool)
            //    ShowWindowEx(data->hWnd, SW_HIDE);
            ShowWindowEx(data->hWnd, SW_HIDE);
        }

        //SetWindowPos(data->hWnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED | SWP_NOZORDER);

        //SetWindowPos(s_info->eWnd->GetMenu(), 0, 0, -100, 0, 0, SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOZORDER);
    }


    // 处理工作夹里的控件, 去除组件边框, 子类化各个组件
    virtual void _subclass_work()
    {
        LPOBJSTRUCT data;

        data = _subclass_Tab(s_info->eWnd->GetWorkTab(), true, false, 0);                // 工作夹
        if (!_SUBCLASS_DBG(data, s_info->eWnd->GetWorkTab(), 0))
            return;

        // WS_CLIPCHILDREN 加上这个样式后不会闪烁, 但是最小化还原后滚动条没了
        _tab_modify_direction(s_info->config->tab_work_direction(), data, 0, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TCS_BUTTONS);
        _tab_modify_icon(s_info->config->tab_work_isIcon(), data);

        _Tab_Work_init(data->hWnd, false);
        //HWND hTab = __copy_tab(data->hWnd, s_info->hEWnd, L"左边自绘工作夹");
        //_Tab_Work_init(hTab, s_info->hEWnd, false);
        //s_info->hWork = hTab;
        //SetWindowLongPtrW(hTab, GWL_STYLE, GetWindowLongPtrW(hTab, GWL_STYLE) & ~TCS_MULTILINE);
        RECT rcOffset = { 0, 0, -WINDOW_SPACING, 0 };
        
        data = _subclass_top_state(s_info->eWnd->GetWork(), rcOffset);
        if (_SUBCLASS_DBG(data, s_info->eWnd->GetWork(), 0))
        {
            ShowWindowEx(data->hWnd, SW_HIDE);
        }
        data = _subclass_tool(s_info->eWnd->GetWork32770(), WndProc_OncePaint, true, true, 0);
        if (_SUBCLASS_DBG(data, s_info->eWnd->GetWork32770(), 0))
        {
            data->draw.isClient = true;
            InsertStyle(data->hWnd, WS_CLIPSIBLINGS | 0);
            data->draw.isNcCalcSize = true;
        }

        data = _subclass_tool(s_info->eWnd->GetWork32770Parent(), WndProc_OncePaint, true, false, 0);
        if (_SUBCLASS_DBG(data, s_info->eWnd->GetWork32770Parent(), 0))
        {
            data->draw.isClient = true;
            InsertStyle(data->hWnd, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
            data->draw.isNcCalcSize = true;
        }

        int itemHeight = s_info->config->tree_fne_height();
        bool isHot = s_info->config->tree_fne_ishot();
        data = _subclass_tree(s_info->eWnd->GetWorkFneTree(), true, false, 0, itemHeight, true, isHot); // 支持库树型框
        if (_SUBCLASS_DBG(data, s_info->eWnd->GetWorkFneTree(), 0))
        {
            InsertStyle(data->hWnd, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
            //SetParent(data->hWnd, hTab);
        }
        itemHeight = s_info->config->tree_program_height();
        isHot = s_info->config->tree_program_ishot();
        data = _subclass_tree(s_info->eWnd->GetWorkProgramTree(), true, false, 0, itemHeight, true, isHot);    // 程序树型框
        if (_SUBCLASS_DBG(data, s_info->eWnd->GetWorkProgramTree(), 0))
        {
            InsertStyle(data->hWnd, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
        }

#if 0
        TIMERPROC pfnTimer = [] (HWND hWnd, UINT message, UINT_PTR id, DWORD dwTimer) -> void
        {
            if (1) return;

            const int bufSize = 0x1000;
            wchar_t szText[bufSize];
            TVITEMEXW item;
            item.hItem = 0;
            item.pszText = szText;
            item.cchTextMax = bufSize;
            item.mask = TVIF_STATE | TVIF_CHILDREN | TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_IMAGE;

            CTreeView tree;
            tree.m_hWnd = hWnd;
            HTREEITEM* hItems = 0;
            int count = tree.enumHItem(hItems);
            for (int i = 0; i < count; i++)
            {
                item.hItem = hItems[i];
                TreeView_GetItem(hWnd, &item);
                wstr::dbg(L"项目数值 = %d(0x%08x), 图标索引 = %d, 标题 = %s\n", item.lParam, item.lParam, item.iImage, szText);
            }
            delete[] hItems;
        };

        //SetParent(data->hWnd, hTab);     // 置父到自己的容器上
        //SetTimer(s_info->eWnd->GetWorkFneTree(), 10, 1000, pfnTimer);
        //RECT rc;
        //GetWindowRect(data->hWnd, &rc);
        //MoveWindow(hTab, 0, 0, rc.right - rc.left, rc.bottom - rc.top, false);
#endif

        data = _subclass_combobox(s_info->eWnd->GetWorkEventComboBox(), false, false, 0, 24);      // 获取事件组合框, 属性框下面
        if (_SUBCLASS_DBG(data, s_info->eWnd->GetWorkEventComboBox(), 0))
        {

        }
        //SetParent(data->hWnd, hTab);
        data = _subclass_combobox(s_info->eWnd->GetWorkControlComboBox(), false, false, 0, 24);    // 获取组件组合框, 属性框上面
        if (_SUBCLASS_DBG(data, s_info->eWnd->GetWorkControlComboBox(), 0))
        {

        }
        //SetParent(data->hWnd, hTab);
        data = _subclass(s_info->eWnd->GetWorkProperty(), WndProc_PropProc);            // 获取属性框
        if (_SUBCLASS_DBG(data, s_info->eWnd->GetWorkProperty(), 0) && s_info->MY_MF_OWNERDRAW)
        {
            data->drawScroll = ScrollCreate(data->hWnd, data->draw.color.crBorder,
                0,          // m_isSysListView, 如果是超级列表框的话这个值就是超级列表框的表项高度
                false,      // m_isSysTreeView, 是否为树型框, 如果为真, 则先设置滚动条位置, 后发送滚动消息, 为假则先发送滚动消息后设置位置
                true,       // m_isCallSetScroll, 是否调用原来的设置滚动条位置函数
                false,      // isSubClass, 是否子类化, 如果为true, 内部会子类化这个窗口, 如果为假, 则必须外部把消息传递到 OnWndProc 方法里
                true        // isReCalcSize, 是否重新计算尺寸, 为true则内部会调用 SetWindowPos 进行重新计算非客户区
            );
        }

        //SetParent(data->hWnd, hTab);

        //SetWindowPos(s_info->eWnd->GetWorkTab(), 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
    }
    inline void _init_state(LPOBJSTRUCT pTab)
    {
        //ShowWindowEx(pTab->hWnd, SW_HIDE);
        //const LPCWSTR appName1 = L"CustmpTab1";
        //const LPCWSTR appName2 = L"CustmpTab2";
        //DWORD style = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE | WS_CHILD | TCS_BOTTOM | TCS_TOOLTIPS | TCS_FOCUSNEVER;
        //HWND hWndNewTab = CreateWindowExW(0, L"SysTabControl32", L"1", style, 0, 0, 500, pTab->pos.height, pTab->hParent, 0, 0, 0);
        //LPOBJSTRUCT pData = _subclass_Tab(hWndNewTab, true, false, 0);
        //hWndNewTab = CreateWindowExW(0, L"SysTabControl32", L"2", style, 500, 0, 500, pTab->pos.height, pTab->hParent, 0, 0, 0);
        //pData = _subclass_Tab(hWndNewTab, true, false, 0);
        //CPrivateProfile& ini = *s_info->ini;
        //int count1 = ini.EnumChild_int(appName1, 0, 0);
        //int count2 = ini.EnumChild_int(appName2, 0, 0);
        //if (count1)
        //{
        //    int* vl = new int[count1];
        //    ini.EnumChild_int(appName1, vl, count1);
        //    delete[] vl;
        //}
        //if (count2)
        //{
        //    int* vl = new int[count2];
        //    ini.EnumChild_int(appName1, vl, count2);
        //    delete[] vl;
        //}
        // 1=提示, 2=输出, 3=调用表, 4=监视表, 5=变量表, 6=搜寻1, 7=搜寻2, 8=剪辑历史
        // 9=支持库, 10=程序, 11=属性, 12 组件箱



    }

    // 获取子夹包含的窗口数量, 通过项目数值判断, 目前只支持易语言默认的那几个窗口
    inline int _GetTabItemParamHwnds(int param, std::vector<HWND>& hWnds)
    {
        IGetEWindow& eWnd = *s_info->eWnd;
        hWnds.clear();
        hWnds.reserve(20);

        switch (param)
        {
        case TAB_PARAM_TIPS:            // 提示, 状态夹里面的提示子夹的项目数值, 点击根据项目数值判断点击了哪个项目
            hWnds.push_back(eWnd.GetStateTabTips());
            break;
        case TAB_PARAM_OUTPUT:          // 输出, 状态夹里面的输出子夹的项目数值, 点击根据项目数值判断点击了哪个项目
            hWnds.push_back(eWnd.GetStateTabOutput());
            break;
        case TAB_PARAM_CALLTABLE:       // 调用表, 状态夹里面的调用表子夹的项目数值, 点击根据项目数值判断点击了哪个项目
            hWnds.push_back(eWnd.GetStateTabCallTable());
            break;
        case TAB_PARAM_MONITORTABLE:    // 监视表, 状态夹里面的监视表子夹的项目数值, 点击根据项目数值判断点击了哪个项目
            hWnds.push_back(eWnd.GetStateTabMonitorTable());
            break;
        case TAB_PARAM_VARTABLE:        // 变量表, 状态夹里面的变量表子夹的项目数值, 点击根据项目数值判断点击了哪个项目
        {
            HWND arr[] =
            {
                 eWnd.GetStateTabVarTableStatic(),
                 eWnd.GetStateVarTableTree(),
                 eWnd.GetStateStackCombobox(),
                 eWnd.GetStateTabVarTableWriteBtn(),
                 eWnd.GetStateTypeSize(),
                 eWnd.GetStateTabVarTableEdit()
            };
            for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
                hWnds.push_back(arr[i]);
            break;
        }
        case TAB_PARAM_FIND1:           // 搜寻1, 状态夹里面的搜寻1子夹的项目数值, 点击根据项目数值判断点击了哪个项目
            hWnds.push_back(eWnd.GetStateTabFind1());
            break;
        case TAB_PARAM_FIND2:           // 搜寻2, 状态夹里面的搜寻2子夹的项目数值, 点击根据项目数值判断点击了哪个项目
            hWnds.push_back(eWnd.GetStateTabFind2());
            break;
        case TAB_PARAM_CLIP:            // 剪辑历史, 状态夹里面的剪辑历史子夹的项目数值, 点击根据项目数值判断点击了哪个项目
            hWnds.push_back(eWnd.GetStateTabClip());
            break;
        default:
            break;
        }
        return (int)hWnds.size();
    }

    // 恢复所有的选择夹子项, 把左右两个子夹的窗口置父到原来的选择夹上
    // hTab = 左边或者右边的子夹句柄
    inline void _restore_TabItems(HWND hTab)
    {
        const int len = TabCtrl_GetItemCount(hTab);
        if (!len)
            return; // 没有项目就返回
        HWND hStateTab = s_info->eWnd->GetStateTab();
        for (int i = 0; i < len; i++)
        {
            std::vector<HWND> hWnds;
            TCITEMW item = { 0 };
            item.mask = TCIF_PARAM;
            TabCtrl_GetItem(hTab, i, &item);
            int size = _GetTabItemParamHwnds(item.lParam, hWnds);
            for (auto it = hWnds.begin(); it != hWnds.end(); ++it)
                SetParent(*it, hStateTab);  // 把当前子夹下记录的所有窗口置父到原来的选择夹中
            
        }
        TabCtrl_DeleteAllItems(hTab);

    }
    // 重新分配选择夹项目
    inline void _state_Dispatch_TabItem(std::vector<int>* params)
    {
        _restore_TabItems(s_info->hTabStateLeft);
        _restore_TabItems(s_info->hTabStateRight);
        HWND hStateTab = s_info->eWnd->GetStateTab();
        TCITEMW item = { 0 };
        wchar_t text[260];
        std::vector<int> arr;

        const int len = TabCtrl_GetItemCount(hStateTab);
        for (int i = 0; i < len; i++)
        {
            item.mask = TCIF_TEXT | TCIF_PARAM;
            item.pszText = text;
            item.cchTextMax = 260;
            TabCtrl_GetItem(hStateTab, i, &item);
            const bool isRight = TabCode_IsRightTab(item.lParam);   // 是否添加到右边的选择夹, 如果为false, 就添加到左边

            // 不是添加到右边就添加到左边选择夹
            HWND hWndInsert = isRight ? s_info->hTabStateRight : s_info->hTabStateLeft;
            const int newIndex = TabCtrl_GetItemCount(hWndInsert);
            int insertIndex = TabCtrl_InsertItem(hWndInsert, newIndex, &item);
            IGetEWindow& eWnd = *s_info->eWnd;
            // 需要把窗口置父到新的选择夹里
            std::vector<HWND> hWnds;
            _GetTabItemParamHwnds(item.lParam, hWnds);
            int nShow = insertIndex == 0 ? SW_SHOW : SW_HIDE;
            for (auto it = hWnds.begin(); it != hWnds.end(); ++it)
            {
                SetParent(*it, hWndInsert);  // 把当前子夹下记录的所有窗口置父到新的选择夹中
                ShowWindowEx(*it, nShow);
            }
            if (insertIndex == 0)
            {
                RECT rc;
                GetClientRect(hWndInsert, &rc);
                //SendMessageW(hWndInsert, WM_SIZE, 0, MAKELONG(rc.right - rc.left, rc.bottom - rc.top));
            }
            if (isRight)
                arr.push_back(item.lParam);
        }
        params->swap(arr);
    }
    
    // 处理状态夹里的控件, 去除组件边框, 子类化各个组件
    virtual void _subclass_state()
    {
        LPOBJSTRUCT pTab, pTab1, pTab2;
        HWND hWnd = s_info->eWnd->GetStateTab();

        pTab = _subclass_Tab(hWnd, true, false, 0);               // 状态夹
        if (!_SUBCLASS_DBG(pTab, s_info->eWnd->GetStateTab(), 0))
            return;


        // WS_CLIPCHILDREN 加上这个样式后不会闪烁, 但是最小化还原后滚动条没了
        _tab_modify_direction(s_info->config->tab_state_direction(), pTab, 0, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TCS_BUTTONS);
        _tab_modify_icon(s_info->config->tab_state_isIcon(), pTab);

        s_info->hTabStateLeft = CreateWindowExW(pTab->styleEx, L"SysTabControl32", L"状态夹左边", pTab->style, 0, 0, 0, 0, pTab->hParent, 0, 0, 0);
        s_info->hTabStateRight = CreateWindowExW(pTab->styleEx, L"SysTabControl32", L"状态夹右边", pTab->style, 0, 0, 0, 0, pTab->hParent, 0, 0, 0);
        pTab1 = _subclass_Tab(s_info->hTabStateLeft, true, false, 0);   // 状态夹右边的选择夹
        pTab2 = _subclass_Tab(s_info->hTabStateRight, true, false, 0);   // 状态夹右边的选择夹
        if (!_SUBCLASS_DBG(pTab1, s_info->hTabStateLeft, 0) || !_SUBCLASS_DBG(pTab2, s_info->hTabStateRight, 0))
            return;
        SetWindowPos(pTab->hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
        SetWindowPos(pTab1->hWnd, HWND_TOP  , 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
        SetWindowPos(pTab2->hWnd, HWND_TOP  , 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

        //TODO 需要从配置里读取哪些在右边, 然后把左边的选择夹项目添加过来
        {
            TCITEMW item = { 0 };

            const LPCWSTR appName = L"iDraw";
            DWORD iniKeyCount = 0;
            wstr value = s_info->ini->read(appName, L"StateTabItems", L"-1").TrimAll();
            std::vector<int>* params = prop_GetTabParams(true);

            strArrW arr;
            size_t arrLen = value.split(&arr, L",");
            for (size_t i = 0; i < arrLen; ++i)
                params->push_back(wstr::stoi(arr[i]));

            _state_Dispatch_TabItem(params);

        }


        
        RECT rcOffset = { 0, WINDOW_SPACING, 0, 0 };
        LPOBJSTRUCT data = _subclass_top_state(s_info->eWnd->GetState(), rcOffset);
        if (_SUBCLASS_DBG(data, s_info->eWnd->GetState(), 0))
        {
            ShowWindowEx(data->hWnd, SW_HIDE);

        }

        _init_state(pTab);
        DWORD dwStyleEx = 0;
        data = _subclass_tool(s_info->eWnd->GetState32770(), WndProc_OncePaint, true, true, 0);
        if (_SUBCLASS_DBG(data, s_info->eWnd->GetState32770(), 0))
        {
            InsertStyle(data->hWnd, WS_CLIPSIBLINGS | 0);
            data->draw.isClient = true;
            data->draw.isNcCalcSize = true;
        }

        data = _subclass_tool(s_info->eWnd->GetState32770Parent(), WndProc_OncePaint, true, false, 0);
        if (_SUBCLASS_DBG(data, s_info->eWnd->GetState32770Parent(), 0))
        {
            InsertStyle(data->hWnd, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
            data->draw.isNcCalcSize = true;
            data->draw.isClient = true;
        }

        data = _subclass_Edit(s_info->eWnd->GetStateTabTips(), true, false, 0, true);               // 提示编辑框
        _SUBCLASS_DBG(data, s_info->eWnd->GetStateTabTips(), WS_CLIPSIBLINGS | WS_CLIPCHILDREN);


        data = _subclass_Edit(s_info->eWnd->GetStateTabOutput(), true, false, 0, true);             // 输出编辑框
        _SUBCLASS_DBG(data, s_info->eWnd->GetStateTabOutput(), WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

        HWND hWndTmp = s_info->eWnd->GetStateTabVarTableEdit();
        dwStyleEx = GetWindowLongW(hWndTmp, GWL_EXSTYLE);
        dwStyleEx &= ~(WS_EX_DLGMODALFRAME | WS_EX_OVERLAPPEDWINDOW);
        SetWindowLongW(hWndTmp, GWL_EXSTYLE, dwStyleEx);
        data = _subclass_Edit(hWndTmp, false, false, 0, true);       // 变量表编辑框
        _SUBCLASS_DBG(data, hWndTmp, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_BORDER);


        // fnWndproc;
        int itemHeight = s_info->config->tree_call_height();
        bool isHot = s_info->config->tree_call_ishot();
        hWndTmp = s_info->eWnd->GetStateTabCallTable();
        data = _subclass_tree(hWndTmp, true, false, 0, itemHeight, true, isHot);  // 调用表树型框
        _SUBCLASS_DBG(data, hWndTmp, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);


        itemHeight = s_info->config->tree_var_height();
        isHot = s_info->config->tree_var_ishot();

        hWndTmp = s_info->eWnd->GetStateVarTableTree();
        data = _subclass_tree(hWndTmp, true, false, 0, itemHeight, true, isHot);  // 变量表树型框
        _SUBCLASS_DBG(data, hWndTmp, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

        data = _subclass_combobox(s_info->eWnd->GetStateStackCombobox(), true, false, 0, 24);     // 变量表堆栈查看组合框
        if (_SUBCLASS_DBG(data, s_info->eWnd->GetStateStackCombobox(), 0))
        {

        }


        data = _subclass_Button(s_info->eWnd->GetStateTabVarTableWriteBtn(), false, false, 0);  // 变量表写入按钮
        _SUBCLASS_DBG(data, s_info->eWnd->GetStateTabVarTableWriteBtn(), WS_BORDER);

        data = _subclass_Static(s_info->eWnd->GetStateTabVarTableStatic(), true, false, 0);   // 获取变量表标签, 仅调试中断状态下有效
        if (_SUBCLASS_DBG(data, s_info->eWnd->GetStateTabVarTableStatic(), WS_CLIPSIBLINGS | WS_CLIPCHILDREN))
            data->draw.isTransparent = true;

        data = _subclass_Static(s_info->eWnd->GetStateTypeSize(), true, false, 0);    // 变量数据类型, 长度, 比如 数据类型: 字节集 / 长度: 0字节
        if (_SUBCLASS_DBG(data, s_info->eWnd->GetStateTypeSize(), WS_CLIPSIBLINGS | WS_CLIPCHILDREN))
            data->draw.isTransparent = true;


        data = ::_subclass_listView(s_info->eWnd->GetStateTabClip(), true, false, 0, 20, true, false, DT_CENTER);         // 剪辑历史超级列表框
        _SUBCLASS_DBG(data, s_info->eWnd->GetStateTabClip(), WS_CLIPSIBLINGS | WS_CLIPCHILDREN);


        data = ::_subclass_listView(s_info->eWnd->GetStateTabMonitorTable(), true, false, 0, 20, true, false);            // 监视表超级列表框
        if (_SUBCLASS_DBG(data, s_info->eWnd->GetStateTabMonitorTable(), WS_CLIPSIBLINGS | WS_CLIPCHILDREN))
        {
            int colWidth = ListView_GetColumnWidth(data->hWnd, 3) - 20;
            ListView_SetColumnWidth(data->hWnd, 3, colWidth);
        }

        itemHeight = s_info->config->list_find1_height();
        isHot = s_info->config->list_find1_ishot();
        data = _subclass_ListBox(s_info->eWnd->GetStateTabFind1(), true, false, 0, itemHeight, true, isHot);    // 搜寻1列表框句柄
        if (_SUBCLASS_DBG(data, s_info->eWnd->GetStateTabFind1(), WS_CLIPSIBLINGS | WS_CLIPCHILDREN))
        {

        }

        itemHeight = s_info->config->list_find2_height();
        isHot = s_info->config->list_find2_ishot();
        data = _subclass_ListBox(s_info->eWnd->GetStateTabFind2(), true, false, 0, itemHeight, true, isHot);    // 搜寻2列表框句柄
        if (_SUBCLASS_DBG(data, s_info->eWnd->GetStateTabFind2(), WS_CLIPSIBLINGS | WS_CLIPCHILDREN))
        {

        }

        SetWindowPos(s_info->eWnd->GetState(), 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
    }
    // 处理底部状态条, 去除组件边框, 子类化各个组件
    virtual void _subclass_statusBar()
    {
        LPOBJSTRUCT data;

        data = ::_subclass_statusBar(s_info->eWnd->GetBottomStatusBar(), true, false, 0);      // 底部状态条
        if (!_SUBCLASS_DBG(data, s_info->eWnd->GetBottomStatusBar(), 0))
            return;

        SetWindowPos(data->hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOREDRAW);
        data->param1 = s_info;
        data->draw.color.crBackGround = RGB(0, 122, 204);
        //data->draw.offsetRect.top = -100;
        //data->draw.offsetRect.right = -(WINDOW_BORDER_WIDTH * 2);
        //data->draw.offsetRect.top = -(WINDOW_BORDER_WIDTH - 2);
        //data->draw.offsetRect.bottom = -WINDOW_BORDER_WIDTH;
        //data->draw.offsetRect.right = -99999;
        //data->draw.offsetRect.bottom = -99999;
        data->draw.fmt = DT_LEFT | DT_BOTTOM;

        data->draw.color.hbrBackGround = Brush_Create(data->draw.color.crBackGround);
        SetClassLongPtrW(data->hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)data->draw.color.hbrBackGround);
        SetPropW(s_info->hEWnd, L"StatusBar", data->hWnd);


        data = _subclass_tool(s_info->eWnd->GetBottomStatusBarBtn(), ::WndProc_EraseBkgnd, true, true, 0);  // 状态条里的按钮
        _SUBCLASS_DBG(data, s_info->eWnd->GetBottomStatusBarBtn(), 0);

        SetWindowPos(s_info->eWnd->GetBottomStatusBar(), 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
    }
    // 处理中间代码区的控件, 去除组件边框, 子类化各个组件
    virtual void _subclass_MDIControl()
    {
        LPOBJSTRUCT data;

        data = _subclass_CustomTab(s_info->eWnd->GetCodeTab());
        if (!_SUBCLASS_DBG(data, s_info->eWnd->GetCodeTab(), 0))
            return;
        //ShowWindowEx(s_info->eWnd->GetCodeTab(), SW_HIDE);




        data = _subclass_MDIClient(s_info->eWnd->GetMDIClient());             // 代码区切换窗口选择夹
        if (_SUBCLASS_DBG(data, s_info->eWnd->GetMDIClient(), 0))
        {
            ShowWindowEx(data->hWnd, SW_HIDE);
        }
        //SetClassLongPtrW(data->hWnd, GCL_STYLE, GetClassLongPtrW(data->hWnd, GCL_STYLE) | CS_VREDRAW | CS_HREDRAW);
        //
        //    RECT rc;
        //    GetWindowRect(data->hWnd, &rc);
        //    const int cx = rc.right - rc.left;
        //    const int cy = rc.bottom - rc.top;
        //    const int id = GetDlgCtrlID(data->hWnd);
        //    DWORD styleEx = GetWindowLongPtrW(data->hWnd, GWL_EXSTYLE);
        //    DWORD style = GetWindowLongPtrW(data->hWnd, GWL_STYLE);
        //    LPCWSTR name = L"自绘代码框";
        //#ifndef _DEBUG
        //    name = 0;
        //#endif
        //    HWND hMDIClient = CreateWindowExW(styleEx, L"#32770", name, style, 0, 0, cx, cy, s_info->hEWnd, (HMENU)id, 0, 0);
        //    s_info->hMDIClient = hMDIClient;

        HWND hChild = GetWindow(s_info->eWnd->GetMDIClient(), GW_CHILD);
        //SetParent(hChild, hMDIClient);
        int i = 0;
        TCITEMW item = { 0 };
        item.mask = TCIF_TEXT | TCIF_PARAM;
        wstr text;
        //while (hChild)
        //{
        //    data = _subclass_MDIClient_Child(hChild, hWnds, hTab);        // 代码区切换窗口选择夹

        //    int len = GetWindowTextLengthW(hChild) + 1;
        //    text.resize(len);
        //    GetWindowTextW(hChild, text.data(), len);
        //    item.pszText = text.data();
        //    item.lParam = (LPARAM)hChild;
        //    len = TabCtrl_InsertItem(hTab, i, &item);
        //    hWnds->push_back(hChild);
        //    hChild = GetWindow(hWnd, GW_HWNDNEXT);
        //}



        //data->draw.nItemHeight = MAKELONG(offsetPadding, 0);
    }
    // 处理组件箱, 去除组件边框, 子类化各个组件
    virtual void _subclass_control()
    {
        static bool isInit;
        if (isInit)return;
        isInit = true;

        DWORD style = WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE;
        // 创建一个选择夹, 用来存放放在这里的组件, 目前就只有一个组件箱
        s_info->hWndTabControl = CreateWindowExW(0, WC_TABCONTROLW, 0, style, 0, 0, 1, 1, s_info->eWnd->GetControl32770(), 0, 0, 0);
        TCITEMW item = { 0 };
        item.mask = TCIF_TEXT | TCIF_PARAM;
        item.pszText = (LPWSTR)L"组件箱";
        item.lParam = TAB_PARAM_CONTROLBOX;
        TabCtrl_InsertItem(s_info->hWndTabControl, 0, &item);


        LPOBJSTRUCT data;
        data = _subclass_Tab(s_info->hWndTabControl, true, false, 0);  // 
        _SUBCLASS_DBG(data, s_info->hWndTabControl, 0);

        RECT rcOffset = { WINDOW_SPACING, 0, 0, 0 };
        data = _subclass_top_state(s_info->eWnd->GetControl(), rcOffset);
        if (_SUBCLASS_DBG(data, s_info->eWnd->GetControl(), WS_CLIPSIBLINGS))
        {
            ShowWindowEx(data->hWnd, SW_HIDE);

        }


        data = _subclass_tool(s_info->eWnd->GetControl32770(), WndProc_Control_32770, true, false, 0);
        if (_SUBCLASS_DBG(data, s_info->eWnd->GetControl32770(), WS_CLIPSIBLINGS | WS_CLIPCHILDREN))
        {
            data->draw.isNcCalcSize = true;
            data->draw.isClient = true;
        }

        data = _subclass_tool(s_info->eWnd->GetControl32770Parent(), WndProc_Control_32770Parent, true, false, 0);
        if (_SUBCLASS_DBG(data, s_info->eWnd->GetControl32770Parent(), WS_CLIPSIBLINGS | 0))
        {
            data->draw.isNcCalcSize = true;
            data->draw.isClient = true;
        }

        HWND hWnd = s_info->eWnd->GetControl32770();
        HWND hChild = GetWindow(hWnd, GW_CHILD);
        while (hChild)
        {
            wchar_t cls[260];
            GetClassNameW(hChild, cls, 260);
            if (wcscmp(cls, L"SysTreeView32") == 0)
            {
                int itemHeight = s_info->config->tree_control_height();
                bool isHot = s_info->config->tree_control_ishot();
                _subclass_tree(hChild, true, false, 0, itemHeight, true, isHot);
                //tv.m_hWnd = hChild;
                //int asjkhk = tv.GetItemCount();
                //HTREEITEM* arr;
                //int asjkhk1 = tv.enumHItem(arr);
                //for (int i = 0; i < asjkhk1; i++)
                //{
                //    wstr ret = tv.GetItemText(arr[i]);
                //    LPARAM lpa = tv.GetItemNum(arr[i]);
                //    wstr::dbg(L"lo = %d, hi = %d, 项目标题 = %s\n", LOWORD(lpa), HIWORD(lpa), ret.c_str());
                //}
                //delete[] arr;
            }
            else if (_wcsicmp(cls, L"button") == 0)
            {
                //_subclass_Button(hChild);
                int textLen = GetWindowTextLengthW(hChild);

                if (textLen > 0)
                {
                    _subclass_Button(hChild, true, false, 0);
                    //wstr str(textLen+1);
                    //GetWindowTextW(hChild, str.data(), textLen + 1);
                    //str.resize(0);
                }
                else
                {
                    BUTTON_IMAGELIST b = { 0 };
                    BUTTON_SPLITINFO bs = { 0 };
                    bs.mask = BCSIF_GLYPH | BCSIF_IMAGE | BCSIF_SIZE | BCSIF_STYLE;
                    SendMessageW(hChild, BCM_GETIMAGELIST, 0, (LPARAM)&b);
                    SendMessageW(hChild, BCM_GETSPLITINFO, 0, (LPARAM)&bs);
                    wstr::dbg(L"按钮信息, 图片组 = 0x%08x, margin = {%d,%d,%d,%d}, uAlign = %dn, BUTTON_SPLITINFO={%d, 0x%08x, %d, {%d, %d}}", b, bs);
                    //static HBRUSH hbr = Brush_Create(RGB(37, 37, 38));
                    //SetClassLongPtrW(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)hbr);
                }
            }

            hChild = GetWindow(hChild, GW_HWNDNEXT);
        }



        //SetWindowPos(s_info->eWnd->GetControl(), 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
    }


public:

    // 初始化绘画信息, 会调用 BeginPaint, 会创建内存DC, 返回内存DC, DX渲染返回0
    // 把组件设置的字体颜色, 背景画刷, 边框画笔都选入DC
    // 如果 isFillBack 为true, 则内部会使用背景画刷填充内存DC
    virtual HDC BeginPaint(LPOBJSTRUCT pData, BEGIN_END_PAINT_STRUCT& ps, bool isDxRender = false, bool isFillBack = true)
    {
        ps.md = 0;
        ps.pData = pData;
        ps.isDxRender = isDxRender;
        ::BeginPaint(pData->hWnd, &ps);
        GetClientRect(pData->hWnd, &ps.rc);
        ps.cxClient = ps.rc.right - ps.rc.left;
        ps.cyClient = ps.rc.bottom - ps.rc.top;
        ps.pFont = g_scale.GetFont(pData->draw.hFont ? pData->draw.hFont : s_info->defData->draw.hFont);    // 绘画字体
        ps.hFont = ps.pFont->GetFont();    // 绘画字体

        if (isDxRender)
        {
            if (!pData->draw.hCanvas)
                pData->draw.hCanvas = _canvas_create(0, ps.cxClient, ps.cyClient);
            LPCANVAS hCanvas = pData->draw.hCanvas;
            const int cxCanvas = (int)hCanvas->GetWidth();
            const int cyCanvas = (int)hCanvas->GetHeight();
            if (cxCanvas < ps.cxClient || cyCanvas < ps.cyClient)
                _canvas_resize(hCanvas, ps.cxClient, ps.cyClient);

            _canvas_begindraw(hCanvas);
            if (isFillBack)
                _canvas_clear(hCanvas, RGB2ARGB(pData->draw.color.crBackGround, 255));
            return 0;
        }

        ps.md = md_create(ps.cxClient, ps.cyClient);
        assert(ps.md);
        HDC hdc = ps.md->hdc;  // 如果md=空, 就让他崩溃
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, pData->draw.color.crText);
        SelectObject(hdc, ps.hFont);
        SelectObject(hdc, pData->draw.color.hbrBackGround);
        SelectObject(hdc, pData->draw.color.hpenBorder);

        if (isFillBack)
            FillRect(hdc, &ps.rc, pData->draw.color.hbrBackGround);
        return hdc;
    }


    // 结束绘画, 会调用 EndPaint, 会根据dx渲染调用dx函数
    virtual bool EndPaint(BEGIN_END_PAINT_STRUCT& data)
    {
        bool ret = false;
        if (data.isDxRender)
        {
            HDC hdc = _canvas_getdc(data.pData->draw.hCanvas);
            BitBlt(data.hdc, 0, 0, data.cxClient, data.cyClient, hdc, 0, 0, SRCCOPY);
            _canvas_releasedc(data.pData->draw.hCanvas);
            ret = _canvas_enddraw(data.pData->draw.hCanvas);
        }
        else
        {
            ret = BitBlt(data.hdc, 0, 0, data.cxClient, data.cyClient, data.md->hdc, 0, 0, SRCCOPY);
            md_destroy(data.md);

        }
        ::EndPaint(data.pData->hWnd, &data);
        return ret;
    }


public:
    

};
static ULONGLONG s_readTimer;
ULONGLONG GetReadTimer()
{
    return s_readTimer;
}

inline SCROLL_CALLBACK_SET_DATA& _scrollcallback_init_data(LPOBJSTRUCT pData)
{
    typedef std::map<HWND, SCROLL_CALLBACK_SET_DATA> SCROLL_CALLBACK;
    SCROLL_CALLBACK* pArr = prop_GetScrollCallback(true);

    SCROLL_CALLBACK_SET_DATA& data = (*pArr)[pData->hWnd];
    SetPropW(pData->hWnd, GUID_SCROLL_CALLBACK, &data);
    if (!data.pGet)
        data.pGet = new std::unordered_set<pfn_GetScrollRect>;
    if (!data.pCalc)
        data.pCalc = new std::unordered_set<pfn_CalcScroll>;
    if (!data.pDraw)
        data.pDraw = new std::unordered_set<pfn_DrawScroll>;
    return data;
}
// 与支持库交互的回调函数, 支持库调用这个库的功能会通过这个函数调用
int CALLBACK IDraw_NotifyCallback(UINT nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= IDC_ICONTROL_MENU_BEGIN && nCode <= IDC_ICONTROL_MENU_END)
    {
        return IDraw_Menu_Function(nCode, wParam, lParam);
    }
    switch (nCode)
    {
    case NL_IDE_READY:  // IDE准备好之后才去hook行号
        return _nl_ide_ready();
    case IDC_IDRAW_GETWINDOWDATA:
        return (int)s_info->pfnGetDataFromHwnd((HWND)lParam);
    case INE_CONFIG_CHANGED:    // 配置改变
        return _ine_Control_Changed((LPIDRAW_INFO)lParam);
    case IDC_IDRAW_INLINEHOOK:  // 设置inlineHook, wParam = 原函数, lParam = 新函数
        return (int)_idc_InlineHook((LPVOID)wParam, (LPVOID)lParam, HOOKMODE_DETOURS);
    case IDC_IDRAW_UNINLINEHOOK:    // 取消inlineHook, wParam = 原函数, lParam = 新函数, 返回是否成功
        return (int)_idc_UnInlineHook((LPVOID)wParam);
    case IDC_INLINEHOOK:            // 设置hook, wParam=hook方式, 0=detoursHook, 1=MinHook, lParam=[被hook地址, 接收地址], 返回原地址
        return (int)_idc_InlineHook(((LPVOID*)lParam)[0], ((LPVOID*)lParam)[1], wParam);
    case IDC_UNINLINEHOOK:          // 取消hook, wParam=hook方式, 0=detoursHook, 1=MinHook, lParam=hook时的返回值
        return (int)_idc_UnInlineHook((LPVOID)lParam);
    
    case IDC_IDRAW_UNINLINEHOOK_PLUGIN: // 取消被释放插件的inlineHook, wParam = 模块起始地址, lParam = 模块结束地址
        return _idc_UnInlineHook_Plugin((LPBYTE)wParam, (LPBYTE)lParam);
    case IDC_IDRAW_CREATEPOPUPMENU: // 创建有自绘样式的弹出式菜单, 所有加入到这个弹出式菜单里的子菜单都会被自绘
    {
        HMENU hMenu = CreatePopupMenu();
        EMenu_InsertMenu(hMenu);
        return (int)hMenu;
    }
    case IDC_IDRAW_GETCODEVISIBLERECT:  // 获取代码窗口可视的矩形, wParam保留, lParam = 接收矩形的指针(基于易语言主窗口位置)
        return _idc_GetCodeVisibleRect((RECT*)lParam);
    case IDC_EIDE_GETCODEMANAGE:    // 获取IDE代码管理(IEIDE_CodeManage)类指针, 忽略wParam参数, lParma = 代码窗口窗口句柄, 为0则获取当前窗口
        return (int)GetIEIDE_CodeManage((HWND)lParam);
    case IDC_EIDE_GETLINESPACING:   // 获取单行间隔, 忽略wParam和 lParam参数
    {
        int* pLineSpacing = GetLineSpacing();
        if (pLineSpacing)
            return *pLineSpacing;
        return 0;
    }
    case IDC_EIDE_SETLINESPACING:   // 暂未实现, 设置单行间隔, 忽略wParam和 lParam参数
        return 0;
    case IDC_EIDE_GETLINEHEIGHT:    // 获取单行高度, 忽略wParam和 lParam参数
    {
        int* pLineHeight = GetLineHeight();
        if (pLineHeight)
            return *pLineHeight;
        return 0;
    }
    case IDC_EIDE_SETLINEHEIGHT:    // 暂未实现, 设置单行高度, 忽略wParam和 lParam参数
        return 0;
    case IDC_EIDE_GETCODEWINDOW:    // 获取当前代码框句柄, 如果当前是起始页或者设计器窗口则返回0, 忽略wParam和 lParam参数
        return (int)GetIEIDE_CodeWindow(0);
    case IDC_EIDE_GETECOLOR:        // 获取易语言当前使用的配色, wParam = 缓冲区尺寸, lParam = 缓冲区地址, 返回写入缓冲区字节数, wParam/lParam为0则返回缓冲区需要的尺寸
        return EColor_GetData((void*)lParam, (int)wParam);
    case IDC_EIDE_SETECOLOR:            // 设置易语言当前使用的配色, wParam为0则lParam是完整的配色文件路径Unicode版, 不为0则lParam指向配色数据, wParam为数据尺寸, lParam = 根据wParam决定是什么值
        return EColor_SetData((void*)lParam, (int)wParam);

    case IDC_SCROLL_GETRECT:            // 获取滚动条位置, 可以通过这个事件修改边框达到指定绘画滚动条位置, wParam = pfn_GetScrollRect 回调函数, lParam = 窗口数据指针
        return _idc_Scroll_cgetrect((pfn_GetScrollRect)wParam, (LPOBJSTRUCT)lParam);
    case IDC_SCROLL_CALCRECT:           // 计算滚动条位置, 可以通过这个事件修改窗口非客户区位置, wParam = pfn_CalcScroll 回调函数, lParam = 窗口数据指针
        return _idc_Scroll_calcrect((pfn_CalcScroll)wParam, (LPOBJSTRUCT)lParam);
    case IDC_SCROLL_DRAW:               // 绘画滚动条, wParam = pfn_DrawScroll 回调函数, lParam = 窗口数据指针
        return _idc_Scroll_draw((pfn_DrawScroll)wParam, (LPOBJSTRUCT)lParam);
    case IDC_GETSCROLLDATA: // 获取滚动条数据, wParam = 滚动条对象指针, lParam = IDRAW_STRUCT_SCROLL_DATA 结构指针
        return _idc_GetScrollData((IDrawScroll*)wParam, (IDRAW_STRUCT_SCROLL_DATA*)lParam);
    case IDC_ICONTROL_TOOL_INSERT:      // 往接口工具条中添加一个按钮, wParam = 保留, lParam = IDRAW_STRUCT_TOOL_INSERT指针
        return Tool_InsertButton((LPIDRAW_STRUCT_TOOL_INSERT)lParam);
    case IDC_ICONTROL_TOOL_DELETE:
        return Tool_DeleteButton((LPPLUGIN_INFO)lParam, (int)wParam);
    case IDC_ICONTROL_TOOL_GETBBUTTON:
        return (int)Tool_GetButtonData((LPPLUGIN_INFO)lParam, (int)wParam);
    case IDC_ICONTROL_TOOL_SETBBUTTON:
        return Tool_SetButtonData((int)wParam, (LPIDRAW_STRUCT_TOOL_INSERT)lParam);
    case IDC_IDRAW_SUBWINDOWEX:         // 子类化窗口, 把这个窗口重画成视觉库窗口一样的风格, 只限顶层窗口, wParam保留, lParam = DRAW_WINDOW_ARGUMENT 结构指针, 返回窗口句柄
        return (int)iDraw_BindWindow((HWND)wParam, (LPDRAW_WINDOW_ARGUMENT)lParam);
    case IDC_IDRAW_CREATEWINDOW:        // 创建一个和视觉库窗口一样风格的顶层窗口, wParam= CREATESTRUCTW 结构指针, lParam = DRAW_WINDOW_ARGUMENT 结构指针, 返回窗口句柄
        return iDraw_CreateWindow((LPCREATESTRUCTW)wParam, (LPDRAW_WINDOW_ARGUMENT)lParam);
    case IDC_IDRAW_SYSDRAW:             // 窗口重画事件, wParam保留, lParam = IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT 结构指针, 无返回值
    case IDC_IDRAW_SYSBTNCLICK:         // 系统按钮点击事件, wParam保留, lParam = IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT 结构指针, 无返回值
    case IDC_IDRAW_ADDSYSBUTTON:        // 添加按钮, wParam保留, lParam = IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT 结构指针, 无返回值
    case IDC_IDRAW_DELSYSBUTTON:        // 删除按钮, wParam保留, lParam = IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT 结构指针, 无返回值
    case IDC_IDRAW_INSSYSBUTTON:        // 插入按钮, wParam保留, lParam = IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT 结构指针, 无返回值
    case IDC_IDRAW_SETBTNPARAM:         // 设置系统按钮数值, wParam保留, lParam = DRAW_WINDOW_ARGUMENT结构地址, 返回是否成功
    case IDC_IDRAW_GETBTNPARAM:         // 获取系统按钮数值, wParam保留, lParam = DRAW_WINDOW_ARGUMENT结构地址, 返回是否成功
    case IDC_IDRAW_GETBTNID:            // 获取系统按钮ID, wParam保留, lParam = DRAW_WINDOW_ARGUMENT结构地址, 返回是否成功
    case IDC_IDRAW_GETBTNPOS:           // 获取系统按钮索引, wParam保留, lParam = DRAW_WINDOW_ARGUMENT结构地址, 返回是否成功
        return iDraw_AddBtn_BindEvent(nCode, (IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT*)lParam);
    case IDC_IDRAW_DRAW_WINDOWPROC:     // 消息处理函数, 把消息传递到绘画系统按钮的消息过程里处理, wParam保留, lParam = IDRAW_SYSBUTTON_ICON_TITLE_WINDOWPROC_STRUCT结构地址
        return iDraw_WindowProc((IDRAW_SYSBUTTON_ICON_TITLE_WINDOWPROC_STRUCT*)lParam);
    case IDC_IDRAW_DRAW_DEFARG :        // 获取默认参数, wParam = 窗口句柄, lParam = DRAW_WINDOW_ARGUMENT结构地址, 返回是否成功
        return iDraw_GetDefArgument((HWND)wParam, (LPDRAW_WINDOW_ARGUMENT)lParam);
    default:
        break;
    }
    return 0;
}

int _nl_ide_ready()
{
    ICustomCodeTab& arr = *s_info->hWnds;
    for (int i = 0; i < arr.size(); i++)
    {
        CUSTOMTAB_DATA& data = arr[i];
        HWND hWnd = GetIEIDE_CodeWindow(data.hWnd);
        if (hWnd)
            SendMessageW(hWnd, WM_HSCROLL, MAKELONG(SB_THUMBTRACK, 0), 0);
    }

    s_readTimer = GetTickCount64();
    //WndProc_MDIClient_CustomTabCtrl(WM_SIZE, 0, MAKELONG(s_info->pWnd->pos.width, s_info->pWnd->pos.height));
    //EIDE_Init();
    return 0;
}

// 配置改变
int _ine_Control_Changed(LPIDRAW_INFO info)
{
    if (info != s_info) return 0;    // 配置插件修改完毕后会通知这里, lParam是s_info
    //THEME_D2D_INFO& d2d = s_info->window_d2d_info;    // D2D绘画对象不需要重新创建, 在配置改变里就已经设置了新的颜色
    //IThemeColor& theme = *s_info->themeColor;
    //IDraw_ReloadDrawObject(d2d, theme);     // 重新加载各种绘画对象
    InvalidateRect(s_info->hEWnd, &s_info->rcCustomTab, 0);
    std::map<HWND, OBJSTRUCT>& map = _get_subclass_allWindow();
    for (auto it = map.begin(); it != map.end(); ++it)
    {
        OBJSTRUCT& data = it->second;
        s_info->pfnGetDefData(&data);
        if (data.fnCrChange)
            data.fnCrChange(&data);
        // 滚动条绘画时每次都会创建画刷, 只要枚举出滚动条对象修改颜色就可以
        IDrawScroll* iScroll = (IDrawScroll*)data.drawScroll;
        if (iScroll)
        {
            static SCROLL_RECT_INFO v;
            //TODO 需要增加修改滚动条宽高的配置
            v.btn1Size      = iScroll->m_vInfo.btn1Size;
            v.btn2Size      = iScroll->m_vInfo.btn2Size;

            //v.btn1Size      = 0;
            //v.btn2Size      = 0;
            v.btnThumbSize  = iScroll->m_vInfo.btnThumbSize;
            v.btnMinSize    = iScroll->m_vInfo.btnMinSize;
    
            v.crBtn1        = RGB2ARGB(s_info->themeColor->crScrollBtn1(),      255);
            v.crBtnHot1     = RGB2ARGB(s_info->themeColor->crScrollBtnHot1(),   255);
            v.crBtnDown1    = RGB2ARGB(s_info->themeColor->crScrollBtnDown1(),  255);

            v.crBtn2        = RGB2ARGB(s_info->themeColor->crScrollBtn2(),      255);
            v.crBtnHot2     = RGB2ARGB(s_info->themeColor->crScrollBtnHot2(),   255);
            v.crBtnDown2    = RGB2ARGB(s_info->themeColor->crScrollBtnDown2(),  255);

            v.crThumb       = RGB2ARGB(s_info->themeColor->crScrollThumb(),     255);
            v.crThumbHot    = RGB2ARGB(s_info->themeColor->crScrollThumbHot(),  255);
            v.crThumbDown   = RGB2ARGB(s_info->themeColor->crScrollThumbDown(), 255);
            v.crBack        = RGB2ARGB(s_info->themeColor->crScrollBack(),      255);

            iScroll->m_vInfo = v;
            iScroll->m_hInfo = v;

            if (iScroll->m_hPen)
                DeleteObject(iScroll->m_hPen);
            iScroll->m_hPen = CreatePen(PS_SOLID, 1, data.draw.color.crBorder);
        }
        if (data.hWnd != s_info->hEWnd && IsWindow(data.hWnd))
        {
            if (iScroll)
                SetWindowPos(data.hWnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME);
            else
                InvalidateRect(data.hWnd, 0, 0);
        }
    }
    std::vector<int>* params = prop_GetTabParams();
    if (params)
    {
        wstr vl(100);
        for (auto it = params->begin(); it != params->end(); ++it)
        {
            vl.append(*it).append(L",");
        }
        vl.pop_back();
        wstr StateTabItems = s_info->ini->read(L"iDraw", L"StateTabItems");
        StateTabItems = StateTabItems.TrimAll();
        if (StateTabItems != vl)
        {
            params->clear();
            strArrW arr;
            size_t arrLen = StateTabItems.split(&arr, L",");
            for (size_t i = 0; i < arrLen; ++i)
                params->push_back(wstr::stoi(arr[i]));

            iDraw* obj = (iDraw*)s_info->iDraw;
            obj->_state_Dispatch_TabItem(params);

            RECT rc;
            GetWindowRect(s_info->hState, &rc);
            SendMessageA(s_info->hState, WM_SIZE, 0, MAKELONG(rc.right - rc.left, rc.bottom - rc.top));


        }
    }
    SetWindowPos(s_info->hEWnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME);
    return 0;
}

// 设置hook, wParam=hook方式, 0=detoursHook, 1=MinHook, lParam=[被hook地址, 接收地址], 返回原地址
int _idc_InlineHook(LPVOID pOldFun, LPVOID pNewFun, int hookMode)
{
    return (int)g_idc.InlineHook(pOldFun, pNewFun, hookMode);
}
int _idc_UnInlineHook(LPVOID pOldFun)
{
    return (int)g_idc.UnInlineHook(pOldFun);
}
int _idc_UnInlineHook_Plugin(LPBYTE pBegin, LPBYTE pEnd)
{
    INLINEHOOK_CALLBACK* hook = prop_GetInlineHookCallback();
    if (!hook) return 0;

    // 因为是遍历循环删除map, 所以不能调用 g_idc.UnInlineHook()
    for (auto it = hook->begin(); it != hook->end(); ++it)
    {
        INLINEHOOK_CALLBACK_DATA& data = it->second;
        LPVOID pOldFun = it->first;         // hook返回的值
        LPVOID pNewFun = data.pNewFun;      // hook接收函数
        if ((LPBYTE)pNewFun >= pBegin && (LPBYTE)pNewFun < pEnd)    // hook接收函数是在被释放的插件内, 需要取消hook
        {
            switch (data.hookMode)
            {
            case HOOKMODE_DETOURS:
            {
                DetourTransactionBegin();
                DetourUpdateThread(GetCurrentThread());
                DetourDetach(&pOldFun, pNewFun);            // 撤销拦截函数
                DetourTransactionCommit();                  // 拦截生效
                break;
            }
            case HOOKMODE_MINHOOK:
            {
                MH_DisableHook(data.pOldFun);
                MH_Uninitialize();
                break;
            }
            default:
                break;
            }
            hook->erase(it++);
            if (it == hook->end())
                break;
        }

    }
    return 0;
}

// 获取代码窗口可视的矩形, wParam保留, lParam = 接收矩形的指针(基于易语言主窗口位置)
int _idc_GetCodeVisibleRect(RECT* prc)
{
    if (!prc) return 0;
    static LPOBJSTRUCT pWork = s_info->pfnGetDataFromHwnd(s_info->hWork);
    static LPOBJSTRUCT pState = s_info->pfnGetDataFromHwnd(s_info->hState);
    static LPOBJSTRUCT pControl = s_info->pfnGetDataFromHwnd(s_info->hControl);
    static LPOBJSTRUCT s_pData = s_info->pfnGetDataFromHwnd(s_info->hEWnd);
    if (!pWork || !pState || !pControl)
        return 0;
    static LPTOPSTATE_INFO work = (LPTOPSTATE_INFO)pWork->param;
    static LPTOPSTATE_INFO state = (LPTOPSTATE_INFO)pState->param;
    static LPTOPSTATE_INFO control = (LPTOPSTATE_INFO)pControl->param;



    HWND hCode = GetIEIDE_CodeWindow(0);
    if (!hCode)
        hCode = s_info->eWnd->GetMDIClient1();
    if (!IsWindow(hCode))
    {
        memset(prc, 0, sizeof(RECT));
        return 0;
    }
    LPOBJSTRUCT pData = GetDataFromHwnd(hCode);
    IDrawScroll* pScroll = pData ? (IDrawScroll*)pData->drawScroll : 0;


    GetWindowRect(hCode, prc);

    HWND hAtiavete = TopState_GetActivateWindow();
    if (pScroll && hAtiavete)
    {
        LONG vThumbSize = (LONG)pScroll->m_vInfo.btnThumbSize;
        LONG hThumbSize = (LONG)pScroll->m_hInfo.btnThumbSize;
        if (__query_flags(work, TOP_STATE_BUTTON_ISAOTUHIDE) && hAtiavete == work->hWnd)
        {
            prc->left = s_info->rcWork1.right + s_info->rcEWindow.left;
            prc->right -= vThumbSize;
            prc->bottom -= hThumbSize;
        }
        else if (__query_flags(state, TOP_STATE_BUTTON_ISAOTUHIDE) && hAtiavete == state->hWnd)
        {
            prc->bottom = s_info->rcState1.top + s_info->rcEWindow.top;
            prc->right -= vThumbSize;
        }
        else if (__query_flags(control, TOP_STATE_BUTTON_ISAOTUHIDE) && hAtiavete == control->hWnd)
        {
            RECT rc;
            GetWindowRect(hCode, &rc);
            int rightCode = rc.right - vThumbSize;
            int rightCtl = s_info->rcEWindow.left + s_info->rcControl1.left;

            prc->right = min(rightCode, rightCtl);  // 代码窗口右边和组件箱右边选最小的那个值
            prc->bottom -= hThumbSize;

        }

    }
    else if (pScroll)
    {
        LONG vThumbSize = (LONG)pScroll->m_vInfo.btnThumbSize;
        LONG hThumbSize = (LONG)pScroll->m_hInfo.btnThumbSize;
        prc->right -= vThumbSize;
        prc->bottom -= hThumbSize;
    }

    const int cxClient = prc->right - prc->left;
    const int cyClient = prc->bottom - prc->top;
    ScreenToClient(s_info->hEWnd, (LPPOINT)prc);
    prc->right = prc->left + cxClient;
    prc->bottom = prc->top + cyClient;
    return 1;
}
int _idc_Scroll_cgetrect(pfn_GetScrollRect pfn, LPOBJSTRUCT pData)
{
    if (!pData) return 0;
    SCROLL_CALLBACK_SET_DATA& data = _scrollcallback_init_data(pData);

    std::unordered_set<pfn_GetScrollRect>& set = *data.pGet;
    set.insert(pfn);
    return (int)pfn;
}
int _idc_Scroll_calcrect(pfn_CalcScroll pfn, LPOBJSTRUCT pData)
{
    if (!pData) return 0;
    SCROLL_CALLBACK_SET_DATA& data = _scrollcallback_init_data(pData);

    std::unordered_set<pfn_CalcScroll>& set = *data.pCalc;
    set.insert(pfn);
    return (int)pfn;
}
int _idc_Scroll_draw(pfn_DrawScroll pfn, LPOBJSTRUCT pData)
{
    if (!pData) return 0;
    SCROLL_CALLBACK_SET_DATA& data = _scrollcallback_init_data(pData);
    std::unordered_set<pfn_DrawScroll>& set = *data.pDraw;
    set.insert(pfn);
    return (int)pfn;
}
int _idc_GetScrollData(IDrawScroll* pObj, IDRAW_STRUCT_SCROLL_DATA* data)
{
    if (!pObj || !data) return 0;
    switch (data->cbSize)
    {
    case 16:    // 第一个版本尺寸是16, 4个成员
    {
        data->btn1Size = pObj->m_vInfo.btn1Size;
        data->btn2Size = pObj->m_vInfo.btn2Size;
        data->btnThumbSize = pObj->m_vInfo.btnThumbSize;
        break;
    }
    //UNDONE 升级这个结构的话就需要case其他尺寸, 写入更多的数据
    default:
        return 0;
    }
    return 1;
}

int iDraw_AddBtn_BindEvent(int code, IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT* arg)
{
    if (!arg) return 0;
    switch (code)
    {
    case IDC_IDRAW_SYSDRAW:             // 窗口重画事件, wParam保留, lParam = IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT 结构指针, 无返回值
        return CDrawWindow::EventDraw(arg->hWnd, (PFN_SysDraw)arg->pfn, arg->param);
    case IDC_IDRAW_SYSBTNCLICK:         // 系统按钮点击事件, wParam保留, lParam = IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT 结构指针, 无返回值
        return CDrawWindow::EventBtnClick(arg->hWnd, (PFN_SysBtnClick)arg->pfn, arg->param);
    case IDC_IDRAW_ADDSYSBUTTON:        // 添加按钮, wParam保留, lParam = IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT 结构指针, 无返回值
        return CDrawWindow::AddSysButton(arg->hWnd, arg->id, arg->param);
    case IDC_IDRAW_DELSYSBUTTON:        // 删除按钮, wParam保留, lParam = IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT 结构指针, 无返回值
        return CDrawWindow::DelSysButton(arg->hWnd, arg->nPosition, arg->flags);
    case IDC_IDRAW_INSSYSBUTTON:        // 插入按钮, wParam保留, lParam = IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT 结构指针, 无返回值
        return CDrawWindow::InsSysButton(arg->hWnd, arg->nPosition, arg->id, arg->param, arg->flags);
    case IDC_IDRAW_SETBTNPARAM:         // 设置系统按钮数值, wParam保留, lParam = DRAW_WINDOW_ARGUMENT结构地址, 返回是否成功
        return CDrawWindow::SetButtonParam(arg->hWnd, arg->nPosition, arg->param, arg->flags);
    case IDC_IDRAW_GETBTNPARAM:         // 获取系统按钮数值, wParam保留, lParam = DRAW_WINDOW_ARGUMENT结构地址, 返回是否成功
        return CDrawWindow::GetButtonParam(arg->hWnd, arg->nPosition, arg->flags);
    case IDC_IDRAW_GETBTNID:            // 获取系统按钮ID, wParam保留, lParam = DRAW_WINDOW_ARGUMENT结构地址, 返回是否成功
        return CDrawWindow::GetButtonIdFromPos(arg->hWnd, arg->nPosition);
    case IDC_IDRAW_GETBTNPOS:           // 获取系统按钮索引, wParam保留, lParam = DRAW_WINDOW_ARGUMENT结构地址, 返回是否成功
        return CDrawWindow::GetButtonPosFromId(arg->hWnd, arg->id);
    default:
        return 0;
    }
    return 1;
}

inline LRESULT CALLBACK _draw_window_proc_(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, OBJSTRUCT* pData)
{
    LRESULT ret = 0;
    if (CDrawWindow::WindowProc(hWnd, message, wParam, lParam, pData->oldProc, &ret))
        return ret;
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
};

inline bool _draw_get_def_arg_(HWND hWnd, LPDRAW_WINDOW_ARGUMENT& arg, DRAW_WINDOW_ARGUMENT& def)
{
    memset(&def, 0, sizeof(def));
    const bool isEmpty = arg == 0;
    if (!arg)
        arg = &def;
    
    LPOBJSTRUCT pData = GetDataFromHwnd(hWnd);
    bool isSubClass = pData == 0;       // 没有子类化这个窗口, 需要子类化一下
    if (!isEmpty)
        isSubClass = arg->isSubClass;   // 如果是外部控制是否子类化, 那就获取外部的值

    // 需要每次调用每次获取, 因为有可能改变主题, 配色会不同
    if (!pData)
        pData = s_info->defData;    // 窗口没有数据就从默认配色里获取


    if (hWnd)
        arg->hWnd       = hWnd;

    if (arg->sysbtnHeight <= 0)
        arg->sysbtnHeight = 30;
    if (arg->sysbtnWidth <= 0)
        arg->sysbtnWidth = 30;
    if (isEmpty)
    {
        iDraw_GetDefArgument(hWnd, arg);
    }
    if (hWnd && !arg->isUnicode)
        arg->isUnicode = IsWindowUnicode(hWnd);
    return isSubClass;
}

int iDraw_BindWindow(HWND hWnd, LPDRAW_WINDOW_ARGUMENT arg)
{
    if (!hWnd)
        return 0;
    DRAW_WINDOW_ARGUMENT def;
    const bool isSubClass = _draw_get_def_arg_(hWnd, arg, def);

    if (isSubClass)
    {
        // 由外部控制是否子类化, 如果外部没有传递参数, 就内部控制
        _subclass(hWnd, _draw_window_proc_, 0, 0, 0);   // 子类化窗口
    }

    CDrawWindow::BindWindow_s(arg);
    return (int)hWnd;
}


int iDraw_CreateWindow(LPCREATESTRUCTW cs, LPDRAW_WINDOW_ARGUMENT arg)
{
    static WNDCLASSEXW wcex;
    if (wcex.lpszClassName == 0)
    {
        wcex.cbSize         = sizeof(WNDCLASSEX);
        wcex.style          = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc    = DefWindowProcA;
        wcex.cbClsExtra     = 0;
        wcex.cbWndExtra     = 0;
        wcex.hInstance      = GetModuleHandleW(0);
        wcex.hIcon          = LoadIconW(0, IDI_APPLICATION);
        wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName   = 0;
        wcex.lpszClassName  = L"draw_window_classname";
        wcex.hIconSm        = wcex.hIcon;
        RegisterClassExW(&wcex);
    }
    HWND hWnd = CreateWindowExW(cs->dwExStyle, wcex.lpszClassName, cs->lpszName, cs->style, cs->x, cs->y, cs->cx, cs->cy, cs->hwndParent, 0, wcex.hInstance, 0);
    if (arg && !arg->isSubClass)
        _subclass(hWnd, _draw_window_proc_, 0, 0, 0);   // 外部没有调用子类化的话, 内部就子类化窗口, 不然没法处理消息

    return iDraw_BindWindow(hWnd, arg);
}

int iDraw_WindowProc(IDRAW_SYSBUTTON_ICON_TITLE_WINDOWPROC_STRUCT* arg)
{
    return CDrawWindow::WindowProc(arg->hWnd, arg->message, arg->wParam, arg->lParam, arg->oldProc, arg->pRet);
}

int iDraw_GetDefArgument(HWND hWnd, LPDRAW_WINDOW_ARGUMENT arg)
{
    if (!arg)
        return 0;

    LPOBJSTRUCT pData = s_info->defData;
    if (hWnd)
    {
        arg->hWnd = hWnd;
        LPOBJSTRUCT p = GetDataFromHwnd(hWnd);
        if (p)
            pData = p;
    }

    COLORINFO& color = pData->draw.color;
    IThemeColor* theme = s_info->themeColor;

    if (arg->sysbtnHeight <= 0) arg->sysbtnHeight = 30;
    if (arg->sysbtnWidth  <= 0) arg->sysbtnWidth  = 30;

    // 为空则使用默认参数
    arg->cbSize         = sizeof(DRAW_WINDOW_ARGUMENT);
    arg->isSubClass     = false;    // 内部处理的话是不子类化, 所有消息由外部传递进来
    if (hWnd && !arg->isUnicode)
        arg->isUnicode  = IsWindowUnicode(hWnd);
    arg->isCasualMove   = false;
    arg->isDrawIcon     = true;
    arg->isDrawDefBtn   = true;

    //arg->nMinWidth = 0;
    //arg->nMinHeight = 0;
    
    if (!arg->crNormal          )   arg->crNormal           = RGB2ARGB(color.crNormal, 255)             ;
    if (!arg->crHot             )   arg->crHot              = RGB2ARGB(color.crHot, 255)                ;
    if (!arg->crDown            )   arg->crDown             = RGB2ARGB(color.crDown, 255)               ;
    if (!arg->crBack            )   arg->crBack             = RGB2ARGB(theme->crBackExtern(), 255)      ;
    if (!arg->crShadow          )   arg->crShadow           = RGB2ARGB(theme->crShadow(), 255)          ;
    if (!arg->crShadow_nf       )   arg->crShadow_nf        = RGB2ARGB(theme->crShadow_nf(), 255)       ;
    if (!arg->crBorder          )   arg->crBorder           = RGB2ARGB(theme->crBorder(), 255)          ;
    if (!arg->crBorder_nf       )   arg->crBorder_nf        = RGB2ARGB(theme->crBorder_nf(), 255)       ;
    if (!arg->crTitle           )   arg->crTitle            = RGB2ARGB(theme->crTitle_left_act(), 255)  ;
    if (!arg->crTitle_nf        )   arg->crTitle_nf         = RGB2ARGB(theme->crTitle_left(), 255)      ;

    if (!arg->hCanvas           )   arg->hCanvas            = pData->draw.hCanvas                       ;
    if (!arg->hFont             )   arg->hFont              = pData->draw.hFont                         ;
    if (!arg->hbrTextTitle      )   arg->hbrTextTitle       = s_info->pfnCreateBrushDx(arg->crTitle)    ;
    if (!arg->hbrTextTitle_nf   )   arg->hbrTextTitle_nf    = s_info->pfnCreateBrushDx(arg->crTitle_nf) ;



    return 1;
}

// 获取所有已经子类化的窗口, 返回写入缓冲区的成员数, 缓冲区为0则返回所需要的缓冲区数量
// pArr = 接收所有子类化窗口的窗口数据数组指针, 为0则返回所需要的成员数
// count = 缓冲区成员数, 为0则返回所需要的成员数
inline int WINAPI EControls_GetAllSubclassWindow(LPOBJSTRUCT* pArr, int count)
{
    std::map<HWND, OBJSTRUCT>& map = _get_subclass_allWindow();

    if (!pArr || !count)
        return (int)map.size();
    int i = 0;
    for (auto it = map.begin(); it != map.end(); ++it)
    {
        if (i + 1 >= count)break;
        OBJSTRUCT& data = it->second;
        pArr[i++] = &data;
    }
    return i;
}


// 重新加载所有绘画对象, 画笔/画刷等....
void IDraw_ReloadDrawObject(THEME_D2D_INFO& d2d, IThemeColor& theme)
{
    COLORREF color = 0;
    
#define _D2D_CRE_HBR(_rgb, _vl)         color = _rgb; _vl = s_info->pfnCreateBrushDx(RGB2ARGB(color, 255));
#define _D2D_CRE_PEN(_rgb, _vl, _vl1)   color = _rgb; _vl = s_info->pfnCreatePenDx(RGB2ARGB(color, 255), 1.f);\
                                                     _vl1 = s_info->pfnCreateBrushDx(RGB2ARGB(color, 255))

    _D2D_CRE_PEN(theme.crBorder              () , d2d.hPenBorder        , d2d.hbrBorder         );
    _D2D_CRE_PEN(theme.crBorder_run          () , d2d.hPenBorder_run    , d2d.hbrBorder_run     );
    _D2D_CRE_PEN(theme.crBorder_nf           () , d2d.hPenBorder_nf     , d2d.hbrBorder_nf      );
    _D2D_CRE_PEN(theme.crShadow              () , d2d.hPenShadow        , d2d.hbrShadow         );
    _D2D_CRE_PEN(theme.crShadow_run          () , d2d.hPenShadow_run    , d2d.hbrShadow_run     );
    _D2D_CRE_PEN(theme.crShadow_nf           () , d2d.hPenShadow_nf     , d2d.hbrShadow_nf      );
    _D2D_CRE_PEN(theme.crMenuBorder          () , d2d.hPenMenuBorder    , d2d.hbrMenuBorder     );

    _D2D_CRE_HBR(theme.crBack                () , d2d.hbrBack                   );

    _D2D_CRE_HBR(theme.crMenuBack            () , d2d.hbrMenuBack               );
    _D2D_CRE_HBR(theme.crMenuHot             () , d2d.hbrMenuHot                );
    _D2D_CRE_HBR(theme.crMenuBan             () , d2d.hbrMenuBan                );
    _D2D_CRE_HBR(theme.crMenuText            () , d2d.hbrMenuText               );
    _D2D_CRE_HBR(theme.crMenuTextHot         () , d2d.hbrMenuTextHot            );
    _D2D_CRE_HBR(theme.crMenuTextBan         () , d2d.hbrMenuTextBan            );

    _D2D_CRE_HBR(theme.crMenuBack            () , d2d.hbrMenuDown               );
    _D2D_CRE_HBR(theme.crMenuBarHot          () , d2d.hbrMenuBarHot             );
    _D2D_CRE_HBR(theme.crMenuBarToolsText    () , d2d.hbrMenuBarToolsText       );
    _D2D_CRE_HBR(theme.crMenuBarToolsTextHot () , d2d.hbrMenuBarToolsTextHot    );
    _D2D_CRE_HBR(theme.crTitleback           () , d2d.hbrTitleback              );
    _D2D_CRE_HBR(theme.crTitle_left_act      () , d2d.hbrTitle_left_act         );
    _D2D_CRE_HBR(theme.crTitle_right_act     () , d2d.hbrTitle_right_act        );
    _D2D_CRE_HBR(theme.crTitle_left          () , d2d.hbrTitle_left             );
    _D2D_CRE_HBR(theme.crTitle_right         () , d2d.hbrTitle_right            );

    _D2D_CRE_HBR(theme.crText                () , d2d.hbrText                   );
    _D2D_CRE_HBR(theme.crTextStatusBar       () , d2d.hbrTextStatusBar          );
    _D2D_CRE_HBR(theme.crTextStatusBar_run   () , d2d.hbrTextStatusBar_run      );
    _D2D_CRE_HBR(theme.crTextStatusBar_nf    () , d2d.hbrTextStatusBar_nf       );

    d2d.hFont = s_info->pfnCreateFontDx(IDRAW_FONT, -13, FONTSTYLE::FontStyleBold);
    d2d.hFont1 = s_info->pfnCreateFontDx(IDRAW_FONT, -13, FONTSTYLE::FontStyleRegular);
    if (s_info->window_d2d_info.hCanvas)
    {
        s_info->d2d->_canvas_destroy(s_info->window_d2d_info.hCanvas);
    }
    s_info->window_d2d_info.hCanvas = _canvas_create(0, 1920, 1080);    // 创建的时候就创建个大的, 后面在调整大小

}

inline void CALLBACK EColor_change()
{
    // 判断配色文件有没有clr, 有的话取出颜色数据写入clr文件
    LPCWSTR clr = s_info->themeColor->clr();    // 重新设置易语言配色
    if (!clr || !clr[0]) return;
    
    // 有clr配置项, 需要把数据写入这个文件
    wstr file(260); // clr文件完整路径
    file.assign(s_info->path_e).append(L"clr\\").append(clr);
    if (!PathFileExistsW(file.c_str())) return;

    CFileRW f;
    if (!f.open(file.c_str(), 4, -1))
        return;

    int len = EColor_GetData(0, 0);
    LPBYTE ptr = new BYTE[len] {0};
    EColor_GetData(ptr, len);
    f.write(ptr, len);
    delete[] ptr;

    wstr::dbg(L"配色被改变, 重新写入配色\n");
}

static bool s_isDPIAware;
// 将一个值进行缩放
inline int _idraw_scale(int n)
{
    if (!s_isDPIAware || s_info->dpiX == 0)
        return n;
    float scale = (float)s_info->dpiX / 96.f;
    return (int)((float)n * scale);
}

typedef BOOL(WINAPI* PFN_ShowWindow)(HWND hWnd, int nCmdShow);
typedef BOOL(WINAPI* PFN_ShowWindowAsync)(HWND hWnd, int nCmdShow);
typedef INT_PTR(WINAPI* PFN_DialogBoxParamA)(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
static PFN_ShowWindow pfn_ShowWindow;
static PFN_ShowWindowAsync pfn_ShowWindowAsync;
static PFN_DialogBoxParamA pfn_DialogBoxParamA;
inline INT_PTR WINAPI MyDialogBoxParamA(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
    DLGTEMPLATE* asda = (DLGTEMPLATE*)lpTemplateName;
    asda++;
    asda++;
    asda++;
    return pfn_DialogBoxParamA(hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);
}

inline int __GetState_ECtl(HWND** pHwnds)
{
    IGetEWindow& wnd = *s_info->eWnd;
    static HWND hWnds[] =
    {
        //wnd.GetState(),
        //wnd.GetStateTab(),
        //wnd.GetState32770(),
        //wnd.GetState32770Parent(),
        wnd.GetStateTabOutput(),
        wnd.GetStateTabTips(),
        wnd.GetStateTabVarTableEdit(),
        wnd.GetStateTabCallTable(),
        wnd.GetStateStackCombobox(),
        wnd.GetStateTypeSize(),
        wnd.GetStateVarTableTree(),
        wnd.GetStateTabClip(),
        wnd.GetStateTabFind1(),
        wnd.GetStateTabFind2(),
        wnd.GetStateTabMonitorTable(),
        wnd.GetStateTabVarTableWriteBtn(),
        wnd.GetStateTabVarTableStatic(),
    };
    *pHwnds = hWnds;
    return (sizeof(hWnds) / sizeof(hWnds[0]));
}

// 隐藏状态夹内所有易语言自带的组件
inline void __hideState_ECtl()
{
    HWND* hWnds;
    int size = __GetState_ECtl(&hWnds);
    for (int i = 0; i < size; i++)
        pfn_ShowWindow(hWnds[i], SW_HIDE);

}
inline bool __isState_ECtl(HWND hWnd)
{
    IGetEWindow& wnd = *s_info->eWnd;
    return hWnd == wnd.GetState() || hWnd == wnd.GetStateTab() || hWnd == wnd.GetState32770() ||
        hWnd == wnd.GetState32770Parent() || hWnd == wnd.GetStateTabOutput() || hWnd == wnd.GetStateTabTips() ||
        hWnd == wnd.GetStateTabVarTableEdit() || hWnd == wnd.GetStateTabCallTable() || hWnd == wnd.GetStateStackCombobox() ||
        hWnd == wnd.GetStateTypeSize() || hWnd == wnd.GetStateVarTableTree() || hWnd == wnd.GetStateTabClip() ||
        hWnd == wnd.GetStateTabFind1() || hWnd == wnd.GetStateTabFind2() || hWnd == wnd.GetStateTabMonitorTable() ||
        hWnd == wnd.GetStateTabVarTableWriteBtn() || hWnd == wnd.GetStateTabVarTableStatic();
};
inline BOOL WINAPI MyShowWindow(HWND hWnd, int nCmdShow)
{
    if (!pfn_ShowWindow)
        return 0;

    if (LOWORD(nCmdShow) == __HOOK_MOVE_BREPAINT)
    {
        nCmdShow = HIWORD(nCmdShow);
        return pfn_ShowWindow(hWnd, nCmdShow);
    }
    HWND hParent = GetParent(hWnd);
    if (nCmdShow == SW_HIDE && (hParent == s_info->hTabStateLeft || hParent == s_info->hTabStateRight))
    {
        if (__isState_ECtl(hWnd))  // 如果是选择夹里的组件, 而且是易语言自带的, 就只置到底层, 不隐藏
        {
            // 把被隐藏的窗口调整到底层
            SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOREDRAW | SWP_NOACTIVATE);
#if _DEBUG
            int id = GetDlgCtrlID(hWnd);
#endif
            return 1;
        }
    }
    return pfn_ShowWindow(hWnd, nCmdShow);

}
inline BOOL WINAPI MyShowWindowAsync(HWND hWnd, int nCmdShow)
{
    HWND hParent = GetParent(hWnd);
    if (nCmdShow == SW_HIDE && (hParent == s_info->hTabStateLeft || hParent == s_info->hTabStateRight))
    {
        if (__isState_ECtl(hWnd))  // 如果是选择夹里的组件, 而且是易语言自带的, 就只置到底层, 不隐藏
        {
            // 把被隐藏的窗口调整到底层
            SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOREDRAW | SWP_NOACTIVATE);
#if _DEBUG
            int id = GetDlgCtrlID(hWnd);
#endif
            return 1;
        }
    }
    return pfn_ShowWindowAsync(hWnd, nCmdShow);
}

static std::vector<pfn_PluginFree> s_free_arr;

EXTERN_C _declspec(dllexport) void* WINAPI IDraw_Interface(IDRAW_INFO* pInfo, IGetEWindow* eWnd, int reserved1, int reserved2)
{
    static iDraw s_iDraw;
    s_free_arr.reserve(30);
    s_info = pInfo;

#if 1
    if (pfn_ShowWindow == 0)
    {
        pfn_ShowWindow = (PFN_ShowWindow)_idc_InlineHook(ShowWindow, MyShowWindow, HOOKMODE_DETOURS);
        pfn_ShowWindowAsync = (PFN_ShowWindow)_idc_InlineHook(ShowWindowAsync, MyShowWindowAsync, HOOKMODE_DETOURS);
        pfn_DialogBoxParamA = (PFN_DialogBoxParamA)_idc_InlineHook(CreateDialogIndirectParamA, MyDialogBoxParamA, HOOKMODE_DETOURS);
        _hook_CreateWindowEx();

        //__hideState_ECtl();
    }
    bool isDPIAware = false, isDxRend = false;
    if (s_info->config)
    {
        isDPIAware = s_info->config->IsDPIAware();
        isDxRend = s_info->config->IsDxRender();
    }
    s_isDPIAware = isDPIAware;
    s_info->dpiX = USER_DEFAULT_SCREEN_DPI;
    s_info->dpiY = USER_DEFAULT_SCREEN_DPI;

    if (s_isDPIAware)
    {
        //TODO 这里可以做个开关是否启用DPI
        typedef HRESULT(WINAPI* pfn_SetProcessDpiAwareness)(int value);
        HMODULE Shcore = GetModuleHandleW(L"Shcore.dll");
        if (Shcore)
        {
            pfn_SetProcessDpiAwareness pfn = (pfn_SetProcessDpiAwareness)GetProcAddress(Shcore, "SetProcessDpiAwareness");
            if (pfn)
            {
                HRESULT hr = pfn(2);
                HMODULE user32 = GetModuleHandleW(L"user32.dll");
                if (user32)
                {
                    typedef UINT(WINAPI* pfn_GetDpiForSystem)();
                    typedef UINT(WINAPI* pfn_GetDpiForWindow)(HWND hWnd);

                    pfn_GetDpiForSystem pfnGetDpiForSystem = (pfn_GetDpiForSystem)GetProcAddress(user32, "GetDpiForSystem");
                    pfn_GetDpiForWindow pfnGetDpiForWindow = (pfn_GetDpiForWindow)GetProcAddress(user32, "GetDpiForWindow");
                    if (pfnGetDpiForSystem && pfnGetDpiForWindow)
                    {
                        s_info->dpiX = 0 ? pfnGetDpiForSystem() : pfnGetDpiForWindow(s_info->hEWnd);
                        s_info->dpiY = s_info->dpiX;
                    }
                }


                if (FAILED(hr))
                {
                    wstr::dbg(L"开启DPI感知失败, 错误码 = 0x%08X\n", GetLastError());
#ifdef _DEBUG
                    __debugbreak();
#endif
                }
            }
        }
    }


    s_info->scale = _idraw_scale;
    s_info->pfnGetDataFromHwnd = GetDataFromHwnd;
    s_info->pfnGetAllSubclassWindow = EControls_GetAllSubclassWindow;
    s_info->d2d = _canvas_new();
    //if (s_info->MY_MF_OWNERDRAW)
        IDrawScroll_Init();
    _canvas_init(isDxRend, false);
    s_info->nRenderMode = _canvas_GetDxRender();
    s_info->isDxRender = (s_info->nRenderMode & EX_RENDER_D2D) == EX_RENDER_D2D;
    s_info->isGpRender = (s_info->nRenderMode & EX_RENDER_GDI) == EX_RENDER_GDI;

    //_canvas_init(false);
    s_info->iDraw = &s_iDraw;

    THEME_D2D_INFO& d2d = s_info->window_d2d_info;
    IThemeColor& theme = *s_info->themeColor;


    IDraw_ReloadDrawObject(d2d, theme);

    // 设置默认的文本效果
    _canvas_SetTextRenderingHint(s_info->window_d2d_info.hCanvas, 1, 0);


#endif
    pluginInfo.cbSize           = sizeof(pluginInfo);
    pluginInfo.name             = "核心绘画插件";
    pluginInfo.remarks          = "所有控件的重画操作都由这个插件完成";
    pluginInfo.version          = IDRAW_VERSIONA;
    pluginInfo.author           = "精易论坛";
    pluginInfo.Versionlimit     = 0;
    pluginInfo.Dependence       = 0;


    pluginInfo.guid                 = "{05A57CE4-A1A4-473D-0002-F90CAF05FE4A}";
    pluginInfo.pfnMenuExt_Command   = 0;
    pluginInfo.pfnCallback          = IDraw_NotifyCallback;

    EIDE_Init();
    MDIClientCode_Hook();

    if (0)  // 不知道是易语言的反调试机制还是助手的反调试机制, 自己挂异常处理后就不能正常运行了
    {
        wstr BeaEngineFile(260);
        BeaEngineFile.assign(s_info->path_iDraw).append(L"BeaEngine.dll");
        EColor_SetBeaEnginePath(BeaEngineFile.c_str());
        EColor_ColorChangeCallback(EColor_change);
    }
    return &pluginInfo;
}
// 插件被卸载, 需要把所有注册的事件/消息给释放
extern "C" __declspec(dllexport) void CALLBACK IDraw_UnInterface()
{
    ShowWindowEx(s_info->eWnd->GetMenuMenu(), SW_SHOW);

    if (pfn_ShowWindow)
        _idc_UnInlineHook(pfn_ShowWindow);
    if (pfn_ShowWindowAsync)
        _idc_UnInlineHook(pfn_ShowWindowAsync);
    if (pfn_DialogBoxParamA)
        _idc_UnInlineHook(pfn_DialogBoxParamA);

    pfn_ShowWindow = 0;
    pfn_ShowWindowAsync = 0;
    pfn_DialogBoxParamA = 0;


    _Unhook_CreateWindowEx();
    _canvas_destroy(s_info->window_d2d_info.hCanvas);
    for (auto it = s_free_arr.begin(); it != s_free_arr.end(); ++it)
        (*it)();
    s_free_arr.clear();
    //_canvas_uninit();   // 这个就让他泄漏吧, 反正程序都结束了
}

void IDraw_PushPluginFreeCallback(pfn_PluginFree pfn)
{
    s_free_arr.push_back(pfn);
}

void Reload_SubclassDrawInfo()
{

}

