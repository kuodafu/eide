#pragma once
#include "iDrawControls_Subclass.h"
#include <control/DrawScroll.h>
#include <iDraw_const.h>
#include <elib/lib2.h>
#include <elib/PublicIDEFunctions.h>
#include "_scale.h"

#define GUID_SCROLL_CALLBACK L"{933F7943-E3A3-4E07-A698-0EA57A62AEAC}"


// ��ȡide�Ѿ�����ʱ��
ULONGLONG GetReadTimer();

// ��ʼ�������е�ѡ���, �޸�ѡ���λ��, ����ѡ�������
// hTab = ����ʼ����ѡ���, hTabOld = �������Դ���ѡ���, isMove = Ϊ����ֻ�ƶ�����, ���Եڶ�������
void _Tab_Work_init(HWND hTab, bool isMove);
// ��ʼ��״̬�е�ѡ���, �޸�ѡ���λ��, ����ѡ�������
// hTab = ����ʼ����ѡ���, hTabOld = �������Դ���ѡ���, isMove = Ϊ����ֻ�ƶ�����, ���Եڶ�������
void _Tab_State_init(HWND hTab, bool isMove);
// ���û��߻�ȡѡ����������, ��������¼ѡ�����Ŀ�Ŀ��
// pData = ѡ��еĴ�������
// vl = ���õ�ֵ, isSet Ϊtrueʱ����Ч
// isSet = �Ƿ�Ϊ����ֵ
std::vector<int>* Tab_GetArr(LPOBJSTRUCT pData, std::vector<int>* vl, bool isSet);

LRESULT CALLBACK WndProc_Tab(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

// �Ӿ�����ȡ��������, ���map��û���������, �򷵻�0, ���򷵻ض�Ӧ������ָ��
LPOBJSTRUCT GetDataFromHwnd(HWND hWnd);
LPOBJSTRUCT SetDataFromHwnd(HWND hWnd);

// hook��Ϣ�������ﴰ��
void EWindow_Hook_SetWindowsHook();

enum TAB_SEL_MODE : int
{
    TAB_SEL_MODE_CODE    = 0,    // ����ѡ��
    TAB_SEL_MODE_LBUTTON = 1,    // ���������ѡ��
    TAB_SEL_MODE_RBUTTON = 2,    // ����Ҽ����ѡ��
    TAB_SEL_MODE_MBUTTON = 3,    // ����м����ѡ��
};

void MDIClient_CustomTabCtrl_ClickItem(int index, bool isClose, LPOBJSTRUCT pData, int mode = TAB_SEL_MODE_LBUTTON);
LRESULT CALLBACK WndProc_MDIClient_CustomTabCtrl(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


bool save_eil(HWND hWnd, HIMAGELIST hList, LPCWSTR name = L"");


// ����һ���滭ͼ��, �滭λ��, �����ɫ �ڲ������hdc�ϻ滭��������+��䱳��+�滭�ı�
// hCanvas = �滭ͼ��
// rcDraw = �滭�ķ�Χ, �ڲ�����������λ滭, ���һ���Ƕ���0,0 ��ʼ�� �ұ�,WINDOW_TOP_STATE ��λ��
// pData = 
// rcClient = ���ڵĿͻ��� GetClientRect ��ֵ
void _draw_top_state(LPCANVAS hCanvas, const RECT& rcDraw, LPOBJSTRUCT pData, const RECT& rcClient);


// ����һ���滭ͼ��, �滭λ��, �����ɫ �ڲ������hdc�ϻ滭��������
// hCanvas = �滭ͼ��
// rcDraw = �滭�ķ�Χ, �ڲ�����������λ滭
// crDot = �����ɫ
// isTransverse = �Ƿ����, Ϊ true ��������һ滭, false ��������»滭
void _draw_top_state_dot(LPCANVAS hCanvas, const RECT_F& rcDraw, ARGB crDot, bool isTransverse);

// ���µ�ǰ����Ĵ���, ���ػ������������ں�֮ǰ����Ĵ���
void TopState_UpdateStateWindow(HWND hWnd);
// ��ȡ��ǰ����Ĵ���
HWND TopState_GetActivateWindow();

void TopState_SetWindowText(HWND hWnd, LPCWSTR lpszText, bool isUpdate);



LRESULT CALLBACK WndProc_OncePaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_MenuMenu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_Control_32770Parent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_Control_32770(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

LRESULT CALLBACK WndProc_PropProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

// ���Ϳ�İ��������¼�, ֻ�ᴫ�� WM_KEYDOWN ��Ϣ
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
// Ĭ�ϴ�����Ϣ
LRESULT CALLBACK WndProc_DefNcPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_DefPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_CtlColor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
// ������������໯
LRESULT CALLBACK WndProc_EraseBkgnd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
//////////////////////////////////////////////////////////////////////////


// ��ʾ��ʾ����
// rc = ����뿪��������������ʾ
// title = ��ʾ�ı���
// tips = ��ʾ������
// x,y = ��ʾ������, Ĭ���� rc����ߺ͵ױ�
void EWidnow_ShowTips(const RECT& rc, LPCWSTR title, LPCWSTR tips, int x = -1, int y = -1);

// ����ԭʼ������ߴ�, ���ݵ�ǰDPI��������µ�����
void EWindow_DPIChange(LPOBJSTRUCT pData);
// ������������, һ��Ҫ�����´�������ǰ����
void EWindow_ResetFont();

// ����������������ť, ��Ҫ��ֹͣ��ǰ����Ȼ���ڵ������
void EWindow_ReStart(int idRun = TOOL_STANDARD_RUN, int idStop = TOOL_STANDARD_STOP);

std::vector<int>* _drawtab_getarr(LPOBJSTRUCT pData, std::vector<int>* pArr, bool isSet);
LPTOPSTATE_INFO _drawtab_gettopstate_info(LPOBJSTRUCT pData, LPTOPSTATE_INFO info, bool isSet);
HWND    _draw_top_gettab(LPOBJSTRUCT pData, HWND hTab, bool isSet);

// ��ѯָ��ֵ�Ƿ��ǹ��߲˵�ID, ���ز˵�ID�������ļ�·��, ���ǹ��߲˵�����0
LPCWSTR _menu_istool_(WPARAM id);

typedef struct MENUEXT_ID_INFO
{
    int id;                         // �û��Զ����ID
    LPARAM param;                   // �˵���Ŀ��ֵ
    LPPLUGIN_INFO info;             // ����˵����ڵĲ����Ϣ
}*LPMENUEXT_ID_INFO;
// ��ѯָ��ֵ�Ƿ�����չ�˵�ID, ���ز˵�ID����������, ������չ�˵�����0
LPMENUEXT_ID_INFO _menu_isext_(WPARAM id);

// cbt���ӵĴ������, �ڲ����������������ȴ���
bool _menu_cbthook_proc(int code, WPARAM wParam, LPARAM lParam, LRESULT& ret);
void MenuExt_Command(LPMENUEXT_ID_INFO info);
void _hook_CreateWindowEx();
void _Unhook_CreateWindowEx();
void _hook_deal_wnd(HWND hWnd, LPCWSTR lpClassName = 0);
void _hook_ThemeChange(bool isDark);

// ��֧�ֿ⽻���Ļص�����, ֧�ֿ���������Ĺ��ܻ�ͨ�������������
int CALLBACK IDraw_NotifyCallback(UINT nCode, WPARAM wParam, LPARAM lParam);
int CALLBACK IDraw_Menu_Function(UINT nCode, WPARAM wParam, LPARAM lParam);

typedef void(__cdecl* pfn_PluginFree)();
// ����һ�������ص�����, �ڲ����ж�ص�ʱ�����
void IDraw_PushPluginFreeCallback(pfn_PluginFree pfn);

// ֪ͨ�˵���߳����Ѿ�����, ��Ҫ�ͷ���Դ
void IDraw_Menu_Destroy();

// ��ȡ��ǰ�������, �����ǰ��ʾ������������ڻ�����ʼҳ������0
// hWnd = MDI�µ�һ�㴰��, ���Ϊ0���Զ���ȡ
HWND GetIEIDE_CodeWindow(HWND hWnd);

// ��ȡָ�����봰�ڵĴ��������, ������Ϊ0���ȡ��ǰ�Ĵ������, �����ǰ��ʾ������������ڻ�����ʼҳ������0
IEIDE_CodeManage* GetIEIDE_CodeManage(HWND hWnd);
// ��ʼ���к�, ��hook�кŵ���ĵ�ַ
void EIDE_Init(bool isHook = false);
// ȡ���м��
int* GetLineSpacing();

// ȡ���и߶�
int* GetLineHeight();

// ��׼���������˰�ť������
void EIDE_Toolbar_Back();
// ��׼������ǰ����ť������
void EIDE_Toolbar_Forward();
// �������갴��
void EIDE_LButtonDown(IRE_STRUCT_EIDE_SELCHANGEDINFO* info);

// ���¼������л滭����, ����/��ˢ��....
void IDraw_ReloadDrawObject(THEME_D2D_INFO& d2d, IThemeColor& theme);

// �ѵ�ǰ�����к�ѹ��ǰ������ջ
size_t EIDE_PushStack(IEIDE_CodeManage* ide, HWND hChild1, bool isDown = false);

// ����򼴽��滭, ��Ҫhook����API�Լ��滭
LRESULT MDIClientCode_BeginPaint(HWND hWnd, UINT message, WPARAM wparam, LPARAM lParam, LPOBJSTRUCT pData, IDrawScroll* pObj);
void MDIClientCode_Hook();
// �滭�к���, �ڲ�ֱ�ӵ���, �����б����滭���������, ���ᱻ����, 5.93��֧��, ��Ҫ���ж�
void MDIClientCode_DrawLine(IRE_STRUCT_BACKGROUND_DRAW* arg);

// �����¼�, ������֪ͨ, ���ش�����
// code = ����״̬, BUILD_STATE_ ��ͷ����
// lParam = ��������
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

// �����������, �ڲ����ʼ������ɫ
inline IDrawScroll* ScrollCreate(
    HWND hWnd,                  // Ҫ�󶨵Ĵ���
    COLORREF crBorder,          // ���ڱ߿���ɫ, �滭ʱ�ж����ޱ߿�, �еĻ���ʹ�������ɫ���滭�߿�
    int nListViewItemHeight,    // ����ǳ����б��Ļ����ֵ���ǳ����б��ı���߶�
    bool isTreeView,            // �Ƿ�Ϊ���Ϳ�, ���Ϊ��, �������ù�����λ��, ���͹�����Ϣ, Ϊ�����ȷ��͹�����Ϣ������λ��
    bool isCallSetScrollInfo,   // �Ƿ����ԭ�������ù�����λ�ú���
    bool isSubClass,            // �Ƿ����໯, ���Ϊtrue, �ڲ������໯�������, ���Ϊ��, ������ⲿ����Ϣ���ݵ� OnWndProc ������
    bool isReCalcSize,          // �Ƿ����¼���ߴ�, Ϊtrue���ڲ������ SetWindowPos �������¼���ǿͻ���
    bool isMDIScroll = false    // �Ƿ��������Դ������Ĺ�����, �ǵĻ���ѡ��������Ĺ��������
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

// ����������ģ��
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
            // �����Ե�ǰ�Ѿ��м������ģ��, ���¼���һ��
            _nes_run_fun(FN_REMOVE_SPEC_ECOM, i, 0);
            break;
        }
    }
    //r = _nes_run_fun(FN_INPUT_ECOM, file, &ret);
    r = _nes_run_fun(FN_ADD_NEW_ECOM2, file, &ret);
    return ret != 0 && r > 0;
}


// �ػ��Ի���, �ڶԻ���� WM_INITDIALOG ��Ϣ�е���, ����Ի����� CInputBox.h ͷ�ļ���ĶԻ���
void Dialog_DrawWindow(HWND hDlg);