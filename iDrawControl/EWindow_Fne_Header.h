#pragma once
#include "iDrawControls_Subclass.h"
#include <control/DrawScroll.h>
#include <iDraw_const.h>
#include <elib/lib2.h>
#include <elib/PublicIDEFunctions.h>
#include "_scale.h"

#define GUID_SCROLL_CALLBACK L"{933F7943-E3A3-4E07-A698-0EA57A62AEAC}"


// 获取ide已经就绪时间
ULONGLONG GetReadTimer();

// 初始化工作夹的选择夹, 修改选择夹位置, 设置选择框索引
// hTab = 被初始化的选择夹, hTabOld = 易语言自带的选择夹, isMove = 为真则只移动窗口, 忽略第二个参数
void _Tab_Work_init(HWND hTab, bool isMove);
// 初始化状态夹的选择夹, 修改选择夹位置, 设置选择框索引
// hTab = 被初始化的选择夹, hTabOld = 易语言自带的选择夹, isMove = 为真则只移动窗口, 忽略第二个参数
void _Tab_State_init(HWND hTab, bool isMove);
// 设置或者获取选择夹里的数组, 这个数组记录选择夹项目的宽度
// pData = 选择夹的窗口数据
// vl = 设置的值, isSet 为true时才有效
// isSet = 是否为设置值
std::vector<int>* Tab_GetArr(LPOBJSTRUCT pData, std::vector<int>* vl, bool isSet);

LRESULT CALLBACK WndProc_Tab(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

// 从句柄里获取窗口数据, 如果map里没有这个数据, 则返回0, 否则返回对应的数据指针
LPOBJSTRUCT GetDataFromHwnd(HWND hWnd);
LPOBJSTRUCT SetDataFromHwnd(HWND hWnd);

// hook消息即将到达窗口
void EWindow_Hook_SetWindowsHook();

enum TAB_SEL_MODE : int
{
    TAB_SEL_MODE_CODE    = 0,    // 代码选中
    TAB_SEL_MODE_LBUTTON = 1,    // 鼠标左键点击选中
    TAB_SEL_MODE_RBUTTON = 2,    // 鼠标右键点击选中
    TAB_SEL_MODE_MBUTTON = 3,    // 鼠标中键点击选中
};

void MDIClient_CustomTabCtrl_ClickItem(int index, bool isClose, LPOBJSTRUCT pData, int mode = TAB_SEL_MODE_LBUTTON);
LRESULT CALLBACK WndProc_MDIClient_CustomTabCtrl(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


bool save_eil(HWND hWnd, HIMAGELIST hList, LPCWSTR name = L"");


// 给定一个绘画图形, 绘画位置, 点的颜色 内部往这个hdc上绘画三条点线+填充背景+绘画文本
// hCanvas = 绘画图形
// rcDraw = 绘画的范围, 内部根据这个矩形绘画, 这个一般是顶边0,0 开始到 右边,WINDOW_TOP_STATE 的位置
// pData = 
// rcClient = 窗口的客户区 GetClientRect 的值
void _draw_top_state(LPCANVAS hCanvas, const RECT& rcDraw, LPOBJSTRUCT pData, const RECT& rcClient);


// 给定一个绘画图形, 绘画位置, 点的颜色 内部往这个hdc上绘画三条点线
// hCanvas = 绘画图形
// rcDraw = 绘画的范围, 内部根据这个矩形绘画
// crDot = 点的颜色
// isTransverse = 是否横向, 为 true 则从左往右绘画, false 则从上往下绘画
void _draw_top_state_dot(LPCANVAS hCanvas, const RECT_F& rcDraw, ARGB crDot, bool isTransverse);

// 更新当前激活的窗口, 会重画激活的这个窗口和之前激活的窗口
void TopState_UpdateStateWindow(HWND hWnd);
// 获取当前激活的窗口
HWND TopState_GetActivateWindow();

void TopState_SetWindowText(HWND hWnd, LPCWSTR lpszText, bool isUpdate);



LRESULT CALLBACK WndProc_OncePaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_MenuMenu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_Control_32770Parent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_Control_32770(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

LRESULT CALLBACK WndProc_PropProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

// 树型框的按键按下事件, 只会传递 WM_KEYDOWN 消息
LRESULT CALLBACK WndProc_TreeView_KeyDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);



LRESULT CALLBACK EMenu_OnDrawItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK EMenu_OnMeasureItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK EMenu_OnInitMenuPopup(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK EMenu_OnUnInitMenuPopup(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK ListView_OnMeasureItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK List_OnDrawItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);


int  Tool_InsertButton(LPIDRAW_STRUCT_TOOL_INSERT pArg);
bool Tool_DeleteButton(LPPLUGIN_INFO plugin, int idCommand);
bool Tool_SetButtonData(int id, LPIDRAW_STRUCT_TOOL_INSERT pArg);
LPIDRAW_STRUCT_TOOL_INSERTEX Tool_GetButtonData(LPPLUGIN_INFO plugin, int idCommand);




//////////////////////////////////////////////////////////////////////////
// 默认处理消息
LRESULT CALLBACK WndProc_DefNcPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_DefPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_CtlColor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
// 清除背景的子类化
LRESULT CALLBACK WndProc_EraseBkgnd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
//////////////////////////////////////////////////////////////////////////


// 显示提示窗口
// rc = 鼠标离开这个区域会隐藏提示
// title = 显示的标题
// tips = 提示的内容
// x,y = 显示的坐标, 默认是 rc的左边和底边
void EWidnow_ShowTips(const RECT& rc, LPCWSTR title, LPCWSTR tips, int x = -1, int y = -1);

// 保存原始的字体尺寸, 根据当前DPI设置组件新的字体
void EWindow_DPIChange(LPOBJSTRUCT pData);
// 重新设置字体, 一定要在重新创建字体前调用
void EWindow_ResetFont();

// 按下了重新启动按钮, 需要先停止当前程序然后在点击运行
void EWindow_ReStart(int idRun = TOOL_STANDARD_RUN, int idStop = TOOL_STANDARD_STOP);

std::vector<int>* _drawtab_getarr(LPOBJSTRUCT pData, std::vector<int>* pArr, bool isSet);
LPTOPSTATE_INFO _drawtab_gettopstate_info(LPOBJSTRUCT pData, LPTOPSTATE_INFO info, bool isSet);
HWND    _draw_top_gettab(LPOBJSTRUCT pData, HWND hTab, bool isSet);

// 查询指定值是否是工具菜单ID, 返回菜单ID关联的文件路径, 不是工具菜单返回0
LPCWSTR _menu_istool_(WPARAM id);

typedef struct MENUEXT_ID_INFO
{
    int id;                         // 用户自定义的ID
    LPARAM param;                   // 菜单项目数值
    LPPLUGIN_INFO info;             // 这个菜单所在的插件信息
}*LPMENUEXT_ID_INFO;
// 查询指定值是否是扩展菜单ID, 返回菜单ID关联的数据, 不是扩展菜单返回0
LPMENUEXT_ID_INFO _menu_isext_(WPARAM id);

// cbt钩子的处理过程, 内部会调用这个函数优先处理
bool _menu_cbthook_proc(int code, WPARAM wParam, LPARAM lParam, LRESULT& ret);
void MenuExt_Command(LPMENUEXT_ID_INFO info);
void _hook_CreateWindowEx();
void _Unhook_CreateWindowEx();
void _hook_deal_wnd(HWND hWnd, LPCWSTR lpClassName = 0);
void _hook_ThemeChange(bool isDark);

// 与支持库交互的回调函数, 支持库调用这个库的功能会通过这个函数调用
int CALLBACK IDraw_NotifyCallback(UINT nCode, WPARAM wParam, LPARAM lParam);
int CALLBACK IDraw_Menu_Function(UINT nCode, WPARAM wParam, LPARAM lParam);

typedef void(__cdecl* pfn_PluginFree)();
// 加入一个结束回调函数, 在插件被卸载的时候调用
void IDraw_PushPluginFreeCallback(pfn_PluginFree pfn);

// 通知菜单这边程序已经结束, 需要释放资源
void IDraw_Menu_Destroy();

// 获取当前代码框句柄, 如果当前显示的是设计器窗口或者起始页都返回0
// hWnd = MDI下第一层窗口, 如果为0则自动获取
HWND GetIEIDE_CodeWindow(HWND hWnd);

// 获取指定代码窗口的代码管理类, 如果句柄为0则获取当前的代码框句柄, 如果当前显示的是设计器窗口或者起始页都返回0
IEIDE_CodeManage* GetIEIDE_CodeManage(HWND hWnd);
// 初始化行号, 会hook行号点击的地址
void EIDE_Init(bool isHook = false);
// 取单行间隔
int* GetLineSpacing();

// 取单行高度
int* GetLineHeight();

// 标准工具条后退按钮被按下
void EIDE_Toolbar_Back();
// 标准工具条前进按钮被按下
void EIDE_Toolbar_Forward();
// 代码框鼠标按下
void EIDE_LButtonDown(IRE_STRUCT_EIDE_SELCHANGEDINFO* info);

// 重新加载所有绘画对象, 画笔/画刷等....
void IDraw_ReloadDrawObject(THEME_D2D_INFO& d2d, IThemeColor& theme);

// 把当前代码行号压入前进后退栈
size_t EIDE_PushStack(IEIDE_CodeManage* ide, HWND hChild1, bool isDown = false);

// 代码框即将绘画, 需要hook部分API自己绘画
LRESULT MDIClientCode_BeginPaint(HWND hWnd, UINT message, WPARAM wparam, LPARAM lParam, LPOBJSTRUCT pData, IDrawScroll* pObj);
void MDIClientCode_Hook();
// 绘画行号线, 内部直接调用, 在所有背景绘画结束后调用, 不会被拦截, 5.93才支持, 需要做判断
void MDIClientCode_DrawLine(IRE_STRUCT_BACKGROUND_DRAW* arg);

// 编译事件, 由助手通知, 返回处理结果
// code = 编译状态, BUILD_STATE_ 开头常量
// lParam = 保留参数
int EWindow_Build(int code, int lParam);



inline bool CALLBACK ___scroll_calc_rect___(IDrawScroll* pObj, RECT* prc)
{
    LPOBJSTRUCT pData = s_info->pfnGetDataFromHwnd(pObj->m_hWnd);
    if (!pData) return false;
    SCROLL_CALLBACK_SET_DATA* p = (SCROLL_CALLBACK_SET_DATA*)GetPropW(pData->hWnd, GUID_SCROLL_CALLBACK);
    if (!p || !p->pCalc) return false;
    std::unordered_set<pfn_CalcScroll>& set = *p->pCalc;
    for (auto it = set.begin(); it != set.end(); ++it)
    {
        pfn_CalcScroll pfn = *it;
        if (pfn(pObj, pData, prc))
            return true;
    }
    return false;
}
inline bool CALLBACK ___scroll_get_rect___(IDrawScroll* pObj, bool isVScroll, ISCROLL_TYPE type, RECT* prc)
{
    if (type == ISCROLL_GETRECT_BORDER)
    {
        LPOBJSTRUCT pData = s_info->pfnGetDataFromHwnd(pObj->m_hWnd);
        if (!pData) return false;
        SCROLL_CALLBACK_SET_DATA* p = (SCROLL_CALLBACK_SET_DATA*)GetPropW(pData->hWnd, GUID_SCROLL_CALLBACK);
        if (!p || !p->pGet) return false;

        std::unordered_set<pfn_GetScrollRect>& set = *p->pGet;
        for (auto it = set.begin(); it != set.end(); ++it)
        {
            pfn_GetScrollRect pfn = *it;
            if (pfn(pObj, pData, isVScroll, prc))
                return true;
        }
        return false;
    }
    return false;
}
inline bool CALLBACK ___scroll_draw___(IDrawScroll* pObj, bool isVScroll, HDC hdc, Gdiplus::Graphics* gp, ISCROLL_TYPE type, RECT_F* rcDraw)
{
    LPOBJSTRUCT pData = s_info->pfnGetDataFromHwnd(pObj->m_hWnd);
    if (!pData) return false;
    SCROLL_CALLBACK_SET_DATA* p = (SCROLL_CALLBACK_SET_DATA*)GetPropW(pData->hWnd, GUID_SCROLL_CALLBACK);
    if (!p || !p->pGet) return false;

    std::unordered_set<pfn_DrawScroll>& set = *p->pDraw;
    for (auto it = set.begin(); it != set.end(); ++it)
    {
        pfn_DrawScroll pfn = *it;
        if (pfn(pObj, pData, isVScroll, hdc, type, rcDraw))
            return true;
    }
    return false;
}

// 插件滚动条类, 内部会初始化好配色
inline IDrawScroll* ScrollCreate(
    HWND hWnd,                  // 要绑定的窗口
    COLORREF crBorder,          // 窗口边框颜色, 绘画时判断有无边框, 有的话就使用这个颜色来绘画边框
    int nListViewItemHeight,    // 如果是超级列表框的话这个值就是超级列表框的表项高度
    bool isTreeView,            // 是否为树型框, 如果为真, 则先设置滚动条位置, 后发送滚动消息, 为假则先发送滚动消息后设置位置
    bool isCallSetScrollInfo,   // 是否调用原来的设置滚动条位置函数
    bool isSubClass,            // 是否子类化, 如果为true, 内部会子类化这个窗口, 如果为假, 则必须外部把消息传递到 OnWndProc 方法里
    bool isReCalcSize,          // 是否重新计算尺寸, 为true则内部会调用 SetWindowPos 进行重新计算非客户区
    bool isMDIScroll = false    // 是否是易语言代码区的滚动条, 是的话会选择配置里的滚动条宽度
)
{
    SCROLL_RECT_INFO v;

    if ( isMDIScroll )
    {
        v.btn1Size = (float)s_info->config->scroll_btn_size();
        v.btnThumbSize = (float)s_info->config->scroll_width();
    }


    if ( v.btn1Size <= 0 )
        v.btn1Size = 10;
    if ( v.btnThumbSize <= 0 )
        v.btnThumbSize = 12;
    v.btn2Size = v.btn1Size;

    v.btnMinSize = 20;
    
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

    IDrawScroll* iScroll = new IDrawScroll(crBorder, v);
    iScroll->m_isSysListView = nListViewItemHeight;
    iScroll->m_isSysTreeView = isTreeView;
    iScroll->m_isCallSetScroll = isCallSetScrollInfo;
    iScroll->bind(hWnd, isSubClass, isReCalcSize);

    iScroll->OnCalcScroll(___scroll_calc_rect___);
    iScroll->OnDrawScroll(___scroll_draw___);
    iScroll->OnGetScrollRect(___scroll_get_rect___);

    return iScroll;
}

template<typename T, typename R>
inline int _nes_run_fun(int nCode, T wParam, R lParam)
{
    struct RUNFUN_ARG
    {
        int wParam;
        int lParam;
    };
    RUNFUN_ARG arg = { 0 };
    arg.wParam = (int)wParam;
    arg.lParam = (int)lParam;
    return s_info->pfnIDE(NES_RUN_FUNC, (DWORD)nCode, (DWORD)&arg);
}

// 易语言引用模块
inline bool E_IncludeEc(LPCSTR file)
{
    BOOL ret = 0;
    int count = 0;
    int r = _nes_run_fun(FN_GET_NUM_ECOM, &count, 0);
    char ecFile[260];
    for (int i = 0; i < count; i++)
    {
        _nes_run_fun(FN_GET_ECOM_FILE_NAME, i, ecFile);
        if (stricmp_api(file, ecFile) == 0)
        {
            // 易语言当前已经有加载这个模块, 重新加载一下
            _nes_run_fun(FN_REMOVE_SPEC_ECOM, i, 0);
            break;
        }
    }
    //r = _nes_run_fun(FN_INPUT_ECOM, file, &ret);
    r = _nes_run_fun(FN_ADD_NEW_ECOM2, file, &ret);
    return ret != 0 && r > 0;
}


// 重画对话框, 在对话框的 WM_INITDIALOG 消息中调用, 这个对话框是 CInputBox.h 头文件里的对话框
void Dialog_DrawWindow(HWND hDlg);