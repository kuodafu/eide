#include "../EWindow_Fne_Header.h"
#include <iDraw_const.h>
#include <control/CInputBox.h>


struct TAB_ICON_INFO
{
    int count;
    LPEX_IMAGE hImage;
};
static std::map<HIMAGELIST, TAB_ICON_INFO> m_map_tab;


LRESULT CALLBACK WndProc_CustomTab(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
void OnTabSelChanged(HWND hWnd, int index);

// �������Դ���ѡ���, 
LPOBJSTRUCT _subclass_CustomTab(HWND hWnd)
{
    LPOBJSTRUCT pData = _subclass(hWnd, WndProc_CustomTab, true);
    pData->draw.color.crTextHot = RGB(85, 170, 255);
    pData->draw.color.crTextDown = RGB(0, 151, 251);
    return pData;
}
inline void WINAPI _tab_crChange(LPOBJSTRUCT pData)
{
    pData->draw.color.crTextHot = RGB(85, 170, 255);
    pData->draw.color.crTextDown = RGB(0, 151, 251);
}
LPOBJSTRUCT _subclass_Tab(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
{
    LPOBJSTRUCT pData = _subclass(hWnd, WndProc_Tab, reMoveBorder, isSetBack, reMoveClassStyle);
    pData->draw.hCanvas      = _canvas_create(pData->hWnd, 0, 0); // dx�滭

    _tab_crChange(pData);
    pData->fnCrChange = _tab_crChange;
    pData->fnNotify = WndProc_OnNotify;
    std::vector<int>* pArr = new std::vector<int>;
    const int ItemCount = TabCtrl_GetItemCount(hWnd);   // ѡ�����Ŀ��
    const int bufSize = 0x1000;
    wchar_t text[bufSize] = { 0 };
    TCITEMW item = { 0 };
    const int fmt = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;
    float width = 0, height = 0;
    static HWND hTabState = s_info->eWnd->GetStateTab();
    static HWND hTabWork = s_info->eWnd->GetWorkTab();
    static HWND hTabControl = s_info->hWndTabControl;
    for (int i = 0; i < ItemCount; i++)
    {
        item.mask = TCIF_TEXT;
        item.pszText = text;
        item.cchTextMax = bufSize;
        TabCtrl_GetItem(hWnd, i, &item);

#define _state_cmp_str(_if, _param, _index, _s) _if(wcscmp(text, _s) == 0) { item.mask = TCIF_PARAM; item.lParam = _param; TabCtrl_SetItem(hWnd, i, &item); }
#ifdef _DEBUG
  #define _state_cmp_dbg else { wstr::dbg(L"��Ŀ[%d] \"%s\" û�м�����Ŀ��ֵ\n", i, text); }
#else
  #define _state_cmp_dbg 
#endif
        if (hWnd == hTabState)
        {
            // 1=��ʾ, 2=���, 3=���ñ�, 4=���ӱ�, 5=������, 6=��Ѱ1, 7=��Ѱ2, 8=������ʷ
            _state_cmp_str(if     , TAB_PARAM_TIPS        , 0, L"��ʾ")
            _state_cmp_str(else if, TAB_PARAM_OUTPUT      , 1, L"���")
            _state_cmp_str(else if, TAB_PARAM_CALLTABLE   , 2, L"���ñ�")
            _state_cmp_str(else if, TAB_PARAM_MONITORTABLE, 3, L"���ӱ�")
            _state_cmp_str(else if, TAB_PARAM_VARTABLE    , 4, L"������")
            _state_cmp_str(else if, TAB_PARAM_FIND1       , 5, L"��Ѱ1")
            _state_cmp_str(else if, TAB_PARAM_FIND2       , 6, L"��Ѱ2")
            _state_cmp_str(else if, TAB_PARAM_CLIP        , 7, L"������ʷ")
            _state_cmp_dbg
        }
        else if (hWnd == hTabWork)
        {
            // 9=֧�ֿ�, 10=����, 11=����
            _state_cmp_str(if     , TAB_PARAM_FNE    , 0, L"֧�ֿ�")
            _state_cmp_str(else if, TAB_PARAM_PROGRAM, 1, L"����")
            _state_cmp_str(else if, TAB_PARAM_PROPER , 2, L"����")
            _state_cmp_dbg
        }
        else if (hWnd == hTabControl)
        {
            // 12 �����
            _state_cmp_str(if, TAB_PARAM_CONTROLBOX, 0, L"�����")
            _state_cmp_dbg
        }
        _canvas_calctextsize(s_info->window_d2d_info.hCanvas, pData->draw.hFont, text, -1, fmt, 80, 20, &width, &height);
        pArr->push_back((int)width + 10);
    }

    Tab_GetArr(pData, pArr, true);
    return pData;
}
// ���û��߻�ȡѡ����������, ��������¼ѡ�����Ŀ�Ŀ��
// pData = ѡ��еĴ�������
// vl = ���õ�ֵ, isSet Ϊtrueʱ����Ч
// isSet = �Ƿ�Ϊ����ֵ
std::vector<int>* Tab_GetArr(LPOBJSTRUCT pData, std::vector<int>* vl, bool isSet)
{
    if (!pData) return 0;
    if (isSet)
        pData->param = vl;
    return (std::vector<int>*)pData->param;
}
// ���û��߻�ȡѡ��еĵ���������
inline HWND _Tab_GetChildHwnd(LPOBJSTRUCT pData, HWND hWnd = 0, bool isSet = false)
{
    if (isSet)
        pData->param1 = hWnd;
    return (HWND)pData->param1;
}
// ��ȡ��ʱ����, ����ǻ滭��ת�Ļ���
inline LPCANVAS& _Tab_GetCanvas(LPOBJSTRUCT pData)
{
    return (LPCANVAS&)pData->param2;
}


inline int _tab_calc_text(LPCWSTR text)
{
    const int fmt = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;
    float width = 0, height = 0;
    _canvas_calctextsize(s_info->window_d2d_info.hCanvas, s_info->defData->draw.hFont, text, -1, fmt, 80, 20, &width, &height);
    return ((int)width + 10);
}
struct TAB_SHOWWINDOW   // ��¼ѡ����Ӽ������еĴ���, ʹ�����鱣��
{
    HWND* hWnd;
    int count;
    TAB_SHOWWINDOW() :hWnd(0), count(0) { }
    TAB_SHOWWINDOW(int count) :hWnd(0), count(0) { operator()(count); }
    ~TAB_SHOWWINDOW() { Destroy(); }

    inline void Destroy()
    {
        if (hWnd) delete[] hWnd;
        hWnd = 0;
        count = 0;
    }
    inline TAB_SHOWWINDOW* operator()(int count)
    {
        Destroy();
        if (count) return this;
        
        this->hWnd = new HWND[count];
        memset(this->hWnd, 0, count * sizeof(HWND));
        this->count = count;
        return this;
    }
    inline operator int() const { return count; }
    inline HWND operator[](int index) const
    {
        if (!hWnd || index < 0 || index >= count) throw index;
        return hWnd[index];
    }
};


//static std::vector<TAB_SHOWWINDOW> m_work;
//static std::vector<std::vector<HWND>> m_state;
void _Tab_Work_init(HWND hTab, bool isMove)
{
    if (!isMove)
    {
        int index = TabCtrl_GetCurSel(hTab);
        HWND hWndShow = index == 0 ? s_info->eWnd->GetWorkFneTree() :
            index == 1 ? s_info->eWnd->GetWorkProgramTree() : s_info->eWnd->GetWorkProperty();
        SetPropW(hTab, L"oldWnd", hWndShow);
    }
    NMHDR hdr = { 0 };
    hdr.hwndFrom = hTab;
    hdr.code = TCN_SELCHANGE;
    hdr.idFrom = GetDlgCtrlID(hdr.hwndFrom);
    SendMessageW(s_info->eWnd->GetWork32770(), WM_NOTIFY, 0, (LPARAM)&hdr);

    //WndProc_OnWorkTabItemChanged(s_info->hEWnd, WM_NOTIFY, 0, (LPARAM)&hdr, GetDataFromHwnd(hTab));
}
inline static HWND Wnd_State_GetWindowFromIndex(int index)
{
    HWND hWndShow = 0;
    switch (index)
    {
    case 0:     // ��ʾ
        hWndShow = s_info->eWnd->GetStateTabTips();
        break;
    case 1:     // ���
        hWndShow = s_info->eWnd->GetStateTabOutput();
        break;
    case 2:     // ���ñ�
        hWndShow = s_info->eWnd->GetStateTabCallTable();
        break;
    case 3:     // ���ӱ�
        hWndShow = s_info->eWnd->GetStateTabMonitorTable();
        break;
    case 4:     // ������
        hWndShow = s_info->eWnd->GetStateTabVarTableStatic();
        if (IsWindowVisible(hWndShow)) break;   // ��ʾ��������ʾ���ڲ��ǵ���״̬, ���λ�ú��������һ��

        hWndShow = s_info->eWnd->GetStateVarTableTree();
        break;
    case 5:     // ��Ѱ1
        hWndShow = s_info->eWnd->GetStateTabFind1();
        break;
    case 6:     // ��Ѱ2
        hWndShow = s_info->eWnd->GetStateTabFind2();
        break;
    case 7:     // ������ʷ
        hWndShow = s_info->eWnd->GetStateTabClip();
        break;
    default:
        return 0;
    }
    return hWndShow;
}
void _Tab_State_init(HWND hTab, bool isMove)
{
    if (!isMove)
    {
        const int index = TabCtrl_GetCurSel(hTab); // ѡ���ѡ��ı�
        HWND hWndShow = Wnd_State_GetWindowFromIndex(index);
        SetPropW(hTab, L"oldWnd", hWndShow);
    }
    NMHDR hdr = { 0 };
    hdr.hwndFrom = hTab;
    hdr.code = TCN_SELCHANGE;
    hdr.idFrom = GetDlgCtrlID(hdr.hwndFrom);
    SendMessageW(s_info->eWnd->GetState32770(), WM_NOTIFY, 0, (LPARAM)&hdr);
}

inline LPTOPSTATE_INFO _tab_get_topstate_info(HWND hWnd, LPOBJSTRUCT* ppData = 0)
{
    if (!hWnd) return 0;
    LPOBJSTRUCT pData = s_info->pfnGetDataFromHwnd(hWnd);
    if (!pData) return 0;
    LPTOPSTATE_INFO info = (LPTOPSTATE_INFO)pData->param;
    if (ppData)*ppData = pData;
    return info;
}
struct TAB_CHILD_DATA
{

};
LRESULT CALLBACK WndProc_TabParentNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    const int code = LOWORD(wParam);
    const int id = HIWORD(wParam);
    HWND hChild = (HWND)lParam;
    switch (code)
    {
    case WM_CREATE:
    {
        wchar_t cls[260];
        GetClassNameW(hChild, cls, 260);
        if (wcsicmp_api(cls, L"msctls_updown32") == 0)
        {
            //if (hWnd == s_info->eWnd->GetWorkTab())
            //{
            //    LPOBJSTRUCT pWork;
            //    LPTOPSTATE_INFO info = _tab_get_topstate_info(s_info->hWork, &pWork);
            //    if (__query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE))
            //    {
            //        ShowWindowEx(hChild, SW_HIDE);
            //    }
            //}
            //else if (hWnd == s_info->eWnd->GetStateTab())
            //{
            //    LPOBJSTRUCT pState;
            //    LPTOPSTATE_INFO info = _tab_get_topstate_info(s_info->hState, &pState);
            //    if (__query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE))
            //    {
            //        ShowWindowEx(hChild, SW_HIDE);
            //    }
            //}
            _Tab_GetChildHwnd(pData, hChild, true);
            _subclass_msctls_updown32(hChild, true, false, 0);
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_DESTROY:
    {
        _Tab_GetChildHwnd(pData, 0, true);
        break;
    }
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}

// ��ѡ������߿����, 4��������Ҫ
inline void _tab_draw_line_sel(LPCANVAS hCanvas, int direction, RECT_F& rcItem, LPEX_PEN hPen)
{
    LINEPOINT_F pt[3];
    switch (direction)
    {
    case 0:     // ���, ��Ҫ�����, ����, �ױ�, �ұ�����
    {
        pt[0](rcItem.right, rcItem.bottom, rcItem.left, rcItem.bottom);
        pt[1](rcItem.left, rcItem.bottom, rcItem.left, rcItem.top);
        pt[2](rcItem.left, rcItem.top, rcItem.right, rcItem.top);
        //MoveToEx(hdc, rcItem.right, rcItem.bottom, 0);  // ���½���Ϊ���
        //LineTo(hdc, rcItem.left, rcItem.bottom);        // �������½�
        //LineTo(hdc, rcItem.left, rcItem.top);           // �������Ͻ�
        //LineTo(hdc, rcItem.right, rcItem.top);          // �������Ͻ�
        break;
    }
    case 1:     // ����, ��Ҫ�����, ����, �ұ�, �ױ�����
    {
        pt[0](rcItem.left + 1, rcItem.bottom, rcItem.left + 1, rcItem.top);
        pt[1](rcItem.left + 1, rcItem.top, rcItem.right, rcItem.top);
        pt[2](rcItem.right, rcItem.top, rcItem.right, rcItem.bottom);
        //MoveToEx(hdc, rcItem.left, rcItem.bottom, 0);   // ���½���Ϊ���
        //LineTo(hdc, rcItem.left, rcItem.top);           // �������Ͻ�
        //LineTo(hdc, rcItem.right, rcItem.top);          // �������Ͻ�
        //LineTo(hdc, rcItem.right, rcItem.bottom);       // �������½�
        break;
    }
    case 2:     // �ұ�, ��Ҫ������, �ұ�, �ױ�, �������
    {
        pt[0](rcItem.left, rcItem.top, rcItem.right, rcItem.top);
        pt[1](rcItem.right, rcItem.top, rcItem.right, rcItem.bottom);
        pt[2](rcItem.right, rcItem.bottom, rcItem.left, rcItem.bottom);

        //MoveToEx(hdc, rcItem.left, rcItem.top, 0);      // ���Ͻ���Ϊ���
        //LineTo(hdc, rcItem.right, rcItem.top);          // �������Ͻ�
        //LineTo(hdc, rcItem.right, rcItem.bottom);       // �������½�
        //LineTo(hdc, rcItem.left, rcItem.bottom);        // �������½�
        break;
    }
    default:    // �ױ�, ��Ҫ�����, �ױ�, �ұ�, ��������
    {
        pt[0](rcItem.left + 1, rcItem.top, rcItem.left + 1, rcItem.bottom);
        pt[1](rcItem.left + 1, rcItem.bottom, rcItem.right, rcItem.bottom);
        pt[2](rcItem.right, rcItem.bottom, rcItem.right, rcItem.top);

        //MoveToEx(hdc, rcItem.left, rcItem.top, 0);      // ���Ͻ���Ϊ���
        //LineTo(hdc, rcItem.left, rcItem.bottom - 1);        // �������½�
        //LineTo(hdc, rcItem.right, rcItem.bottom - 1);       // �������½�
        //LineTo(hdc, rcItem.right, rcItem.top);          // �������Ͻ�
        break;
    }
    }

    hCanvas->DrawLine(hPen, &pt[0]);
    hCanvas->DrawLine(hPen, &pt[1]);
    hCanvas->DrawLine(hPen, &pt[2]);
}
// ���׸��Ӽе���߿����, 4��������Ҫ
inline void _tab_draw_line_first(LPCANVAS hCanvas, int direction, RECT_F& rcItem, RECT& rcClient, LPOBJSTRUCT pData)
{
    RECT_F rcTmp = rcClient;
    LPEX_PEN hPen = s_info->pfnCreatePenDx(RGB2ARGB(pData->draw.color.crBorder, 255), 1);
    LPEX_BRUSH hbr = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crNormal, 255));
    LINEPOINT_F pt;
    switch (direction)
    {
    case 0:     // ���, ��Ҫ���Ӽ��ұ߻�һ������, �����Ҫ���������ɫ
    {
        rcItem.top -= 2;
        rcTmp.right = rcItem.right;
        //rcTmp.left++;
        // ���Ӽ��ұ߻�һ������
        pt((float)rcItem.right - 1, (float)rcItem.top, (float)rcItem.right - 1, (float)rcClient.bottom);
        break;
    }
    case 1:     // ����, ��Ҫ���Ӽеױ߻�һ������, ������Ҫ���������ɫ
    {
        rcItem.left -= 2;
        rcTmp.bottom = rcItem.bottom;
        //rcTmp.left++; rcTmp.right--;
        // ���Ӽеױ߻�һ������
        pt((float)rcItem.left, (float)rcItem.bottom - 1, (float)rcClient.right, (float)rcItem.bottom - 1);
        break;
    }
    case 2:     // �ұ�, ��Ҫ���Ӽ���߻�һ������, �ұ���Ҫ���������ɫ
    {
        rcItem.top -= 2;
        rcTmp.left = rcItem.left;
        //rcTmp.right--;
        // ���Ӽ���߻�һ������
        pt((float)rcItem.left + 1, (float)rcItem.top, (float)rcItem.left, (float)rcClient.bottom);
        break;
    }
    default:    // �ױ�, ��Ҫ���Ӽж��߻�һ������, �ױ���Ҫ���������ɫ
    {
        rcItem.left -= 2;
        rcTmp.top = rcItem.top;
        //rcTmp.left++; rcTmp.right--;
        // ���Ӽж��߻�һ������
        pt((float)rcItem.left, (float)rcItem.top + 1, (float)rcClient.right, (float)rcItem.top + 1);
        break;
    }
    }
    hCanvas->FillRectangle(hbr, &rcTmp);
    hCanvas->DrawLine(hPen, &pt);
}
inline static void CALLBACK TimerProc_LeaveTab(HWND hWnd, UINT message, UINT_PTR id, DWORD timer)
{
    LPOBJSTRUCT pData = (LPOBJSTRUCT)id;
    if (pData->draw.indexHot == -1)
    {
        KillTimer(hWnd, id);
        return;
    }
    POINT pt;
    RECT rcItem;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);
    TabCtrl_GetItemRect(hWnd, pData->draw.indexHot, &rcItem);   // ȡ�ȵ����λ��
    // �����겻���ȵ����о���Ҫ�ػ��ȵ���
    if (!PtInRect(&rcItem, pt))
    {
        KillTimer(hWnd, id);
        pData->draw.indexHot = -1;
        InvalidateRect(hWnd, 0, 0);
    }
}
inline void _tab_draw_icon(LPCANVAS hCanvas, HIMAGELIST hList, TCITEMW& item, RECT_F& rcItem, int direction)
{
    if (!hList)
        return;

    TAB_ICON_INFO& iconInfo = m_map_tab[hList];
    //float cx = rcItem.width();
    //float cy = rcItem.height();
    float cx = (float)s_info->scale(20);
    float cy = (float)s_info->scale(20);
    int cxIcon = 0;
    int cyIcon = 0;
    const int count = ImageList_GetImageCount(hList);
    ImageList_GetIconSize(hList, &cxIcon, &cyIcon);

    if (iconInfo.count != count)    // ��Ҫ���´���ͼƬ��
    {
        _img_destroy(iconInfo.hImage);
        iconInfo.count = count;

        LPMEMDCBITMAP md = md_create(cxIcon * count, cyIcon);
        Gdiplus::Graphics gp(md->hdc);
        for (int i = 0; i < count; i++)
        {
            HICON hIcon = ImageList_GetIcon(hList, i, ILD_NORMAL);
            Gdiplus::Bitmap bmp(hIcon);
            gp.DrawImage(&bmp, i * cxIcon, 0, cxIcon, cyIcon);
            DestroyIcon(hIcon);
        }
        int size = 0;
        LPBYTE pngData = GetPngDataFromBitmap(md->hBitmap, &size, md->hdc);
        iconInfo.hImage = _img_create_frommemory(pngData, size);
        delete[] pngData;
        md_destroy(md);
    }

    LPEX_IMAGE hImg = iconInfo.hImage;
    if (!hImg) return;
    // ͼ����ͼƬ���λ��
    RECT_F rcDrawIcon((float)(cxIcon * item.iImage), 0, (float)hImg->GetWidth(), (float)cyIcon);
    rcDrawIcon.right = rcDrawIcon.left + cxIcon;


    const int offsetX = ((int)cx - s_info->scale(cxIcon)) / 2;
    const int offsetY = ((int)cy - s_info->scale(cyIcon)) / 2;

    RECT_F rc;  // �滭������ϵ�λ��
    rc.left     = (float)(rcItem.left + (float)offsetX);
    rc.top      = (float)(rcItem.top  + (float)offsetY);
    rc.right    = rc.left + (float)s_info->scale(cxIcon);
    rc.bottom   = rc.top  + (float)s_info->scale(cyIcon);


    _canvas_drawimagerectrect(hCanvas, hImg, rc.left, rc.top, rc.right, rc.bottom,
        rcDrawIcon.left, rcDrawIcon.top, rcDrawIcon.right, rcDrawIcon.bottom, 255);
    if (direction == 1 || direction == 3)
        rcItem.left += (float)s_info->scale(cxIcon);
    else
        rcItem.top += (float)s_info->scale(cxIcon);

    //HICON hIcon = ImageList_GetIcon(hList, item.iImage, ILD_TRANSPARENT);
    //if (hIcon)
    //{
    //    int cxIcon, cyIcon;
    //    ImageList_GetIconSize(hList, &cxIcon, &cyIcon);
    //    DrawIconEx(hdc, rcItem.left + 1, rcItem.top + 2, hIcon, cxIcon, cyIcon, 0, 0, DI_NORMAL);
    //    DestroyIcon(hIcon);
    //    switch (direction)
    //    {
    //    case 0:     // ���
    //    case 2:     // �ұ�
    //        rcItem.top += cyIcon + 1;
    //        break;
    //    case 1:     // ����
    //    case 3:     // �ױ�
    //        rcItem.left += cxIcon + 2;
    //        break;
    //    default:
    //        return;
    //    }
    //}
}

struct INPUT_TAB_DATA
{
    HWND hBtn;          // ȷ�ϰ�ť, ��������ȷ���Ƿ�Ϸ�, �Ϸ���������, ���Ϸ��ͽ�ֹ���
    HWND hEdit_tips;    // ��ʾ��Ϣ�ı༭��
    HWND hEdit;         // �����ı༭��, ��Ҫʵʱ�ж�������Ƿ�Ϸ�
    LPWSTR retText;     // ��ʵʱ����ʱ���ֵд������, ���ȷ�ϵ�ʱ�����ֵ�����Ѿ�����ò�������Ҫ��ֵ, �����ȡ��, �������ֵҲ����ʹ��
};
inline INPUT_TAB_DATA* _new_theme_getData(HWND hDlg)
{
    const LPCWSTR paopName = L"{A3B11AE9-5979-4A1F-AD5C-1A6A30A9DDB9}";
    INPUT_TAB_DATA* data = (INPUT_TAB_DATA*)GetPropW(hDlg, paopName);
    if (!data)
    {
        data = new INPUT_TAB_DATA;
        memset(data, 0, sizeof(INPUT_TAB_DATA));
        SetPropW(hDlg, paopName, data);
    }
    return data;
}
// �����ֵ�Ƿ���ѡ��е���Ŀ��ֵ
inline LPCWSTR _tab_check_tabparam(int param)
{
    switch (param)
    {
    case TAB_PARAM_TIPS:            // ��ʾ, ״̬���������ʾ�Ӽе���Ŀ��ֵ, ���������Ŀ��ֵ�жϵ�����ĸ���Ŀ
        return L"��ʾ";
    case TAB_PARAM_OUTPUT:          // ���, ״̬�����������Ӽе���Ŀ��ֵ, ���������Ŀ��ֵ�жϵ�����ĸ���Ŀ
        return L"���";
    case TAB_PARAM_CALLTABLE:       // ���ñ�, ״̬������ĵ��ñ��Ӽе���Ŀ��ֵ, ���������Ŀ��ֵ�жϵ�����ĸ���Ŀ
        return L"���ñ�";
    case TAB_PARAM_MONITORTABLE:    // ���ӱ�, ״̬������ļ��ӱ��Ӽе���Ŀ��ֵ, ���������Ŀ��ֵ�жϵ�����ĸ���Ŀ
        return L"���ӱ�";
    case TAB_PARAM_VARTABLE:        // ������, ״̬������ı������Ӽе���Ŀ��ֵ, ���������Ŀ��ֵ�жϵ�����ĸ���Ŀ
        return L"������";
    case TAB_PARAM_FIND1:           // ��Ѱ1, ״̬���������Ѱ1�Ӽе���Ŀ��ֵ, ���������Ŀ��ֵ�жϵ�����ĸ���Ŀ
        return L"��Ѱ1";
    case TAB_PARAM_FIND2:           // ��Ѱ2, ״̬���������Ѱ2�Ӽе���Ŀ��ֵ, ���������Ŀ��ֵ�жϵ�����ĸ���Ŀ
        return L"��Ѱ2";
    case TAB_PARAM_CLIP:            // ������ʷ, ״̬������ļ�����ʷ�Ӽе���Ŀ��ֵ, ���������Ŀ��ֵ�жϵ�����ĸ���Ŀ
        return L"������ʷ";
    //case TAB_PARAM_FNE:             // ֧�ֿ�, �����������֧�ֿ��Ӽе���Ŀ��ֵ, ���������Ŀ��ֵ�жϵ�����ĸ���Ŀ
    //    return L"֧�ֿ�";
    //case TAB_PARAM_PROGRAM:         // ����, ����������ĳ����Ӽе���Ŀ��ֵ, ���������Ŀ��ֵ�жϵ�����ĸ���Ŀ
    //    return L"����";
    //case TAB_PARAM_PROPER:          // ����, ����������������Ӽе���Ŀ��ֵ, ���������Ŀ��ֵ�жϵ�����ĸ���Ŀ
    //    return L"����";
    //case TAB_PARAM_CONTROLBOX:      // �����, �ұߵ������
    //    return L"�����";
    default:
        break;
    }
    return 0;
}
// ���������Ƿ�Ϸ�
inline void tab_input_cmp_input(HWND hDlg)
{
    INPUT_TAB_DATA* data = _new_theme_getData(hDlg);
    const LPCWSTR inputText = L"1=��ʾ, 2=���, 3=���ñ�, 4=���ӱ�, 5=������, 6=��Ѱ1, 7=��Ѱ2, 8=������ʷ, ���ֵʹ��Ӣ�Ķ��ŷָ�";
    static wstr text(520);
    text.resize(520);
    int len = GetWindowTextLengthW(data->hEdit) + 1;
    len = GetWindowTextW(data->hEdit, text.data(), len);
    text.resize(len);
    strArrW arr;
    size_t arrLen = text.split(&arr, L",");
    wstr resultText(260);
    data->retText[0] = 0;
    std::map<int, LPCWSTR> map; // ȥ�ظ�������ʹ��

    for (size_t i = 0; i < arrLen; i++)
    {
        int param = (int)wstr::stoi(arr[i]);
        LPCWSTR name = _tab_check_tabparam(param);
        if (name)
        {
            map[param] = name;
        }
    }

    if (!map.empty())
    {
        // 1=��ʾ, 2=���, 3=���ñ�, 4=���ӱ�, 5=������, 6=��Ѱ1, 7=��Ѱ2, 8=������ʷ9=֧�ֿ�, 10=����, 11=����12 �����
        // ��˹�㷨 (���� + ĩ��) * ���� / 2
        const int AllValues = (1 + 8) * 8 / 2;
        int calcValue = 0;
        for (auto it = map.begin(); it != map.end(); ++it)
        {
            LPCWSTR name = it->second;
            int param = it->first;
            calcValue += param;
            resultText.append(name).append(L",");
            wcscat_s(data->retText, 260, wstr::to_string(param).c_str());
            wcscat_s(data->retText, 260, L",");
        }
        if (calcValue == AllValues)
        {
            // �ұ���ʾ�����е���Ŀ, �Ǿ�����Ϊ-1, ���������ʾ
            resultText = L"";
            wcscpy_s(data->retText, 260, L"-1");
        }
        else
        {
            resultText.pop_back();
            size_t strLen = wcslen(data->retText) - 1;
            data->retText[strLen] = 0;
        }
    }
    wstr showText;
    showText.Format(L"%s\r\nԤ�� = %s\r\n", inputText, resultText.c_str());
    SetWindowTextW(data->hEdit_tips, showText.c_str());

}
// ���¼���ѡ���, ��Ͷ�ݵ��������������ԵĴ���
inline void Tab_ReLost(LPCWSTR value)
{
    s_info->ini->write(L"iDraw", L"StateTabItems", value);      // �����µ�ѡ��
    s_info->pfnConfig(INE_CONFIG_CHANGED, 0, (LPARAM)s_info);   // ֪ͨ�滭������²���滭����, �ڲ�ʹ��
    HWND hEWnd = GetWindow(GetDesktopWindow(), GW_CHILD);
    wchar_t clsName[260] = { 0 };
    while (hEWnd)
    {
        if (hEWnd != s_info->hEWnd)
        {

            GetClassNameW(hEWnd, clsName, 260);
            if (wcsicmp_api(clsName, L"ENewFrame") == 0)
                PostMessageW(hEWnd, LVM_UPDATE, 0, 0);  // Ͷ�ݸ��µ���Ϣ�����������ԵĴ���
        }
        hEWnd = GetWindow(hEWnd, GW_HWNDNEXT);
    }
    // LVM_UPDATE �����������ڴ��������Ϣ���ǵ��� s_info->pfnConfig(INE_CONFIG_CHANGED, wParam, (LPARAM)s_info);
}
inline bool CALLBACK tab_dlg_input_proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam, INT_PTR* pRet)
{
    switch (message)
    {
    case WM_INITDIALOG: // ��ʼ���Ի���, ��Ҫ���໯����ı༭��, ʵʱ�ж�������ı��Ƿ�Ϸ�
    {
        INPUT_TAB_DATA* data = _new_theme_getData(hDlg);
        data->hBtn = GetDlgItem(hDlg, IDOK);
        data->hEdit = GetDlgItem(hDlg, 1001);
        data->hEdit_tips = GetDlgItem(hDlg, 1000);
        data->retText = (LPWSTR)lParam;
        Dialog_DrawWindow(hDlg);
        break;
    }
    case WM_COMMAND:
    {
        const int id = LOWORD(wParam);
        const int code = HIWORD(wParam);
        if (code == EN_CHANGE && id == 1001)
        {
            tab_input_cmp_input(hDlg);
            break;
        }
        break;
    }
    case WM_DESTROY:
    {
        INPUT_TAB_DATA* data = _new_theme_getData(hDlg);
        delete data;
        break;
    }
    default:
        break;
    }
    return false;
}

LRESULT CALLBACK WndProc_Tab(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    static HWND hWndWork = s_info->eWnd->GetWorkTab();
    static HWND hWndState = s_info->eWnd->GetStateTab();
    const int TMPMENU_SETTING = 10086;  // ��ʱʹ�õ����ò˵�
    switch (message)
    {
    case WM_DPICHANGED:
        EWindow_DPIChange(pData);
        break;
    case WM_RBUTTONDOWN:
    {
        TCHITTESTINFO tti = { 0 };
        tti.pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        int index = TabCtrl_HitTest(hWnd, &tti);
        if (index >= 0 && (hWnd == s_info->hTabStateLeft || hWnd == s_info->hTabStateRight || hWnd == s_info->eWnd->GetStateTab()) )
        {
            static HMENU hMenu;
            if (!hMenu)
            {
                hMenu = (HMENU)s_info->pfnControls(IDC_IDRAW_CREATEPOPUPMENU, 0, 0);
                AppendMenuW(hMenu, MF_STRING, TMPMENU_SETTING, L"�����ұ���ʾ�Ĵ���");
            }
            POINT pt;
            GetCursorPos(&pt);
            return TrackPopupMenu(hMenu, TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, 0);
        }

        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_COMMAND:
    {
        
        const int id = LOWORD(wParam);
        if (id == TMPMENU_SETTING)
        {
            wstr StateTabItems = s_info->ini->read(L"iDraw", L"StateTabItems");

            wchar_t retText[260] = { 0 };
            INPUTBOXSTRUCT input = { 0 };
            input.hParent = s_info->hEWnd;
            input.text = L"1=��ʾ, 2=���, 3=���ñ�, 4=���ӱ�, 5=������, 6=��Ѱ1, 7=��Ѱ2, 8=������ʷ, ���ֵʹ��Ӣ�Ķ��ŷָ�";
            input.caption = L"�������ұ���Ҫ��ʾ�Ĵ���";
            input.defText = StateTabItems.c_str();
            input.pszOkButton = L"ȷ������(&O)";
            input.pszCancelButton = L"ȡ��(&C)";
            input.lpDialogFunc = tab_dlg_input_proc;
            input.dwInitParam = (LPARAM)retText;

            wchar_t* ret = input_OpenEx(&input);
            if (!ret) return 0;
            input_free(ret);
            Tab_ReLost(retText);

            return 0;
        }
        if (hWnd == s_info->hTabStateLeft || hWnd == s_info->hTabStateRight)
            return SendMessageA(s_info->eWnd->GetStateTab(), message, wParam, lParam);
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_MOUSEMOVE:
    {
        TCHITTESTINFO tti = { 0 };
        tti.pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        int index = TabCtrl_HitTest(hWnd, &tti);
        if (index != -1)
        {
            if (index != pData->draw.indexHot)
            {
                pData->draw.indexHot = index;
                InvalidateRect(hWnd, 0, 0);
                SetTimer(hWnd, (UINT_PTR)pData, 50, TimerProc_LeaveTab);

            }
        }
        else
        {
            // ��겻��ѡ�����
            if (pData->draw.indexHot != -1)
            {
                pData->draw.indexHot = -1;
                InvalidateRect(hWnd, 0, 0);
            }
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_SIZE:
    {
        if (hWnd == s_info->eWnd->GetWorkTab())
        {
            LPOBJSTRUCT pWork;
            LPTOPSTATE_INFO info = _tab_get_topstate_info(s_info->hWork, &pWork);
            if (__query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE))
            {
                return 0;
            }
        }
        else if (hWnd == s_info->eWnd->GetStateTab())
        {
            LPOBJSTRUCT pState;
            LPTOPSTATE_INFO info = _tab_get_topstate_info(s_info->hState, &pState);
            if (__query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE))
            {
                return 0;
            }
        }
        if (hWnd == s_info->hTabStateLeft || hWnd == s_info->hTabStateRight)
        {
            HWND hLr =  _Tab_GetChildHwnd(pData);

            if (hLr && IsWindowVisible(hLr))
            {
                RECT rcLr, rcTab;
                GetWindowRect(hLr, &rcLr);
                GetWindowRect(hWnd, &rcTab);
                if (rcLr.right < rcTab.right)
                    ShowWindowEx(hLr, SW_HIDE);
            }

        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_PAINT:
    {
        BEGIN_END_PAINT_STRUCT ps;
        s_info->iDraw->BeginPaint(pData, ps, true, false);

        LPCANVAS hCanvas = pData->draw.hCanvas;
        hCanvas->SetTextRenderingHint(1);
        hCanvas->SetAntialiasMode(0);
        RECT rcItem;
        LPEX_PEN hPen = s_info->pfnCreatePenDx(RGB2ARGB(pData->draw.color.crBorder, 255), 1);
        LPEX_BRUSH hbrBack = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crBackGround, 255));
        LPEX_BRUSH hbrHot = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crHot, 255));

        HWND hParent = 0;
        if (hWnd == s_info->eWnd->GetWorkTab())
            hParent = s_info->hWork;
        else if (hWnd == s_info->eWnd->GetStateTab() || hWnd == s_info->hTabStateLeft || hWnd == s_info->hTabStateRight)
            hParent = s_info->hState;
        else if (hWnd == s_info->hWndTabControl)
            hParent = s_info->hControl;
        LPTOPSTATE_INFO info = _tab_get_topstate_info(hParent);
        RECT_F rcF(ps.rc);
        hCanvas->FillRectangle(hbrBack, &rcF);
        // ���Զ�����, �����ǵ�ǰ����Ĵ���, ֻ�滭���߾���
        if (info && __query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE))
        {
            HWND hActivate = s_info->iDraw->TopState_GetActivateWindow();
            
            if (hParent && hActivate == hParent)  // ��ǰ�滭���Ǽ���Ĵ���
            {
                RECT_F rcFill(1, 1, (float)ps.cxClient, (float)ps.cyClient);
                hCanvas->DrawRectangle(hPen, &rcFill);
                s_info->iDraw->EndPaint(ps);
                break;
            }
        }

        rcF.left += 1;
        rcF.top += 1;
        hCanvas->DrawRectangle(hPen, &rcF);
        rcF.left -= 1;
        rcF.top -= 1;
        {
            RECT_F rcTmp = rcF;
            rcTmp.bottom = rcTmp.top + 1;
            rcTmp.left++;
            rcTmp.right--;
            hCanvas->FillRectangle(hbrBack, &rcTmp);
        }
        const int index = TabCtrl_GetCurSel(hWnd);          // ��ǰѡ���ѡ����
        const int ItemCount = TabCtrl_GetItemCount(hWnd);   // ѡ�����Ŀ��
        //const DWORD dwStyle = GetWindowLongPtrW(hWnd, GWL_STYLE);
        const DWORD dwStyle = pData->style;
        const bool isBottom = (dwStyle & TCS_BOTTOM) == TCS_BOTTOM; // ���ڵױ�, ����ұ�û����   
        int direction = (dwStyle & TCS_VERTICAL) == TCS_VERTICAL;   // ѡ�����, 0=���, 1=����, 2=�ұ�, 3=�ױ�
        if (direction)  direction = isBottom ? 2 : 0;
        else            direction = isBottom ? 3 : 1;


        //if (direction != 1 && direction != 3)
        //{
        //    SelectObject(hdc, s_hFont);
        //}

        const int bufSize = 0x1000;
        wchar_t text[bufSize] = { 0 };
        TCITEMW item = { 0 };
        item.mask = TCIF_TEXT | TCIF_PARAM | TCIF_IMAGE;
        item.pszText = text;
        item.cchTextMax = bufSize;


        HIMAGELIST hList = TabCtrl_GetImageList(hWnd);
        RECT rcSel;
        TabCtrl_GetItemRect(hWnd, index, &rcSel);
        for (int i = 0; i < ItemCount; i++)
        {
            TabCtrl_GetItemRect(hWnd, i, &rcItem);
            TabCtrl_GetItem(hWnd, i, &item);
            RECT_F rcItemF(rcItem);
            //rcItem.right -= 2;
            if (i == 0)
            {
                // �������Ӽ��������/�߶ȸ����� hbrNormal ��ˢ��ɫ
                _tab_draw_line_first(hCanvas, direction, rcItemF, ps.rc, pData);
            }
            LPEX_BRUSH hbr = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crNormal, 255));

            COLORREF crText = pData->draw.color.crText;
            if (i == index) // ��ǰ���Ƶ���Ŀ��ѡ����
            {
                hCanvas->FillRectangle(hbrBack, &rcItemF);
                hbr = hbrBack;
                if (direction == 0)
                    rcItemF.left++;
                _tab_draw_line_sel(hCanvas, direction, rcItemF, hPen);
                crText = pData->draw.color.crTextDown;
                if (direction == 0)
                    rcItemF.left--;
            }
            else if (i == pData->draw.indexHot)
            {
                RECT_F rcTmp = rcItem;
                if (direction == 1 || direction == 3)   // ����
                {
                    if (i + 1 == index)
                        rcTmp.right = (float)rcSel.left;
                    else if (i - 1 == index)
                        rcTmp.left = (float)rcSel.right;
                }
                else
                {
                    if (i + 1 == index)
                        rcTmp.bottom = (float)rcSel.top;
                    else if (i - 1 == index)
                        rcTmp.top = (float)rcSel.bottom;
                }
                hCanvas->FillRectangle(hbrHot, &rcTmp);
                hbr = hbrHot;

                //_tab_draw_line_sel(hCanvas, direction, rcItem);
                crText = pData->draw.color.crTextHot;
            }

            LPEX_BRUSH hbrText = s_info->pfnCreateBrushDx(RGB2ARGB(crText, 255));
            DWORD argb = hbrText->GetColor();

            _tab_draw_icon(hCanvas, hList, item, rcItemF, direction);

            if (!text[0]) continue;

            LPEX_FONT hFont = g_scale.GetFont(pData->draw.hFont);
            const int fmt = DT_VCENTER | DT_CENTER | DT_SINGLELINE;
            if (direction != 1 && direction != 3)
            {
                // ��Ҫ��ת
                LPCANVAS& hCanvasRotate = _Tab_GetCanvas(pData);
                if (hCanvasRotate == 0)
                    hCanvasRotate = _canvas_create(hWnd, 1, 1);
                
                RECT_F rcRotate(0, 0, rcItemF.height(), rcItemF.width());
                const float nRotate = rcRotate.bottom;
                int maxWidth = (int)max(rcRotate.right, rcRotate.bottom);
                if ((int)hCanvasRotate->GetWidth() < maxWidth || (int)hCanvasRotate->GetHeight() < maxWidth)
                    hCanvasRotate->Resize(maxWidth + 20, maxWidth + 20);

                hCanvasRotate->BeginDraw();
                hCanvasRotate->clear(0);

                hCanvasRotate->SetTextRenderingHint(1);
                hCanvasRotate->SetAntialiasMode(0);
                hCanvasRotate->rotate(nRotate, nRotate, 90);
                argb = hbr->GetColor();
                hCanvasRotate->FillRectangle(hbr, &rcRotate);
                hCanvasRotate->drawtext(hFont, hbrText, text, -1, fmt, &rcRotate);
                hCanvasRotate->rotate(0, 0, 0);
                hCanvasRotate->EndDraw();

                if (0)
                {
                    _canvas_alphablend(hCanvas,
                        rcItemF.left, rcItemF.top, rcItemF.width(), rcItemF.height(),
                        hCanvasRotate, rcRotate.bottom, 0, rcItemF.width(), rcItemF.height(), 255);
                }
                else
                {
                    //bug ���͸����������ɫ.....
                    if (direction == 0)
                    {
                        _canvas_bitblt(hCanvas,
                            (int)rcItemF.left + 1, (int)rcItemF.top + 1, (int)rcItemF.width() - 3, (int)rcItemF.height() - 2,
                            hCanvasRotate, (int)rcRotate.bottom + 1, 1, SRCCOPY);
                    }
                    else
                    {
                        _canvas_bitblt(hCanvas,
                            (int)rcItemF.left + 1, (int)rcItemF.top + 1, (int)rcItemF.width() - 2, (int)rcItemF.height() - 2,
                            hCanvasRotate, (int)rcRotate.bottom + 1, 1, SRCCOPY);
                    }
                }
            }
            else
            {
                hCanvas->drawtext(hFont, hbrText, text, -1, fmt, &rcItemF);
            }
        }

        s_info->iDraw->EndPaint(ps);
        break;
    }
    case WM_ERASEBKGND:
        return WndProc_EraseBkgnd(hWnd, message, wParam, lParam, pData);
    case WM_CTLCOLORMSGBOX:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLOR:
    {
        const HDC hdc = (HDC)wParam;
        const HWND hControl = (HWND)lParam;
        LPOBJSTRUCT pCtlData = GetDataFromHwnd(hControl);
        if (!pCtlData)
            return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

        if (pCtlData->draw.isTransparent)
            SetBkMode(hdc, TRANSPARENT);
        RECT rc;
        GetClientRect(hControl, &rc);

        SetBkColor(hdc, pCtlData->draw.color.crBackGround);
        SetTextColor(hdc, pCtlData->draw.color.crText);
        static HWND hCtl = s_info->eWnd->GetWorkControlComboBox();
        static HWND hEvt = s_info->eWnd->GetWorkEventComboBox();
        static HWND hWorkTab = s_info->eWnd->GetWorkTab();
        if (hWnd == hWorkTab && (hControl == hCtl || hControl == hEvt))
        {
            InvalidateRect(hControl, 0, 0);
        }
        return (LRESULT)pCtlData->draw.color.hbrBackGround;
    }
    case TCM_SETCURSEL:
    {
        if (hWnd == s_info->eWnd->GetStateTab() || hWnd == s_info->eWnd->GetWorkTab())
        {
            int vl = (int)(LONG_PTR)GetPropW(hWnd, L"tab_set_sel");
            if (LOWORD(vl) == 1211 && HIWORD(vl) == 1)
            {
                SetPropW(hWnd, L"tab_set_sel", 0);
                return 0;
            }

            LRESULT ret = WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

            OnTabSelChanged(hWnd, (int)wParam);     // ����״̬�����ı�
            NMHDR hdr;
            hdr.code = TCN_SELCHANGE;
            hdr.hwndFrom = hWnd;
            hdr.idFrom = GetDlgCtrlID(hWnd);
            SendMessageW(GetParent(hWnd), WM_NOTIFY, hdr.idFrom, (LPARAM)&hdr);
            return ret;
        }
        LRESULT ret = WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        return ret;
    }
    case WM_PARENTNOTIFY:
    {
        return WndProc_TabParentNotify(hWnd, message, wParam, lParam, pData);
    }
    case TCM_INSERTITEMA:
    case TCM_INSERTITEMW:
    {
        const int index = (int)wParam;
        LPTCITEMA item = (LPTCITEMA)lParam;
        LPTCITEMW itemW = (LPTCITEMW)lParam;
        auto arr = Tab_GetArr(pData, 0, 0);
        if(!arr)
            return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

        int width;
        if (message == TCM_INSERTITEMA)
            width = _tab_calc_text(_str::A2W(item->pszText).c_str());
        else
            width = _tab_calc_text(itemW->pszText);
        int indexRet = (int)WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        arr->insert(arr->begin() + indexRet, width);

        return (LRESULT)index;
    }
    case TCM_DELETEALLITEMS:
    {
        auto arr = Tab_GetArr(pData, 0, 0);
        if (arr)
            arr->clear();
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case TCM_DELETEITEM:
    {
        auto arr = Tab_GetArr(pData, 0, 0);
        if (arr)
            arr->erase(arr->begin() + wParam);
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_STYLECHANGING:
    {
        if (wParam == GWL_STYLE)
        {
            LPSTYLESTRUCT ss = (LPSTYLESTRUCT)lParam;
            if ((hWndWork == hWnd || hWndState == hWnd))
            {
                if (ss->styleNew & TCS_MULTILINE)
                    ss->styleNew &= ~(TCS_MULTILINE); // ���ܼ��������ʽ

                //int oldDirection = _tab_getdirection(ss->styleOld);
                //int newDirection = _tab_getdirection(ss->styleNew);

                //if (newDirection != 3)  // ǿ�Ƶױ�
                //{
                //    ss->styleNew |= TCS_BOTTOM;
                //    ss->styleNew &= ~TCS_VERTICAL;
                //}

                //if (oldDirection != newDirection)   // ��ʽ�б䶯
                //{
                //    if (newDirection != 1 && newDirection != 3) // ���Ƕ���, Ҳ���ǵױ�, �Ǿ�ǿ���϶���
                //    {
                //        ss->styleNew = ss->styleOld;   // ���ı���ʽ, ����ʹ��ԭ������ʽ
                //        //ss->styleNew &= ~(TCS_VERTICAL | TCS_BOTTOM);   // ȥ����ߺ͵ױߵ���ʽ
                //    }
                //}
            }
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_STYLECHANGED:
    {
        LRESULT ret = WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        if (wParam == GWL_STYLE)
        {
            LPSTYLESTRUCT ss = (LPSTYLESTRUCT)lParam;
            int oldDirection = _tab_getdirection(ss->styleOld);
            int newDirection = _tab_getdirection(ss->styleNew);

            if (oldDirection != newDirection && (hWndWork == hWnd || hWndState == hWnd))
            {
                NMHDR hdr = { 0 };
                LPCWSTR name = 0;
                bool isSave = false;
                if (hWndWork == hWnd)
                {
                    hdr.hwndFrom = hWndWork;
                    //s_info->pfnSetInfo(s_info, IGI_CONFIG_TABWORK_DIRECTION, newDirection);

                    //int oldDirection = s_info->config->tab_work_direction(&newDirection);
                    //isSave = oldDirection != newDirection;
                    hdr.code = TCN_SELCHANGE;
                    hdr.idFrom = GetDlgCtrlID(hdr.hwndFrom);
                    SendMessageW(GetParent(hdr.hwndFrom), WM_NOTIFY, 0, (LPARAM)&hdr);
                }
                else
                {
                    //int oldDirection = s_info->config->tab_state_direction(&newDirection);
                    //isSave = oldDirection != newDirection;
                    SetWindowLongPtrW(s_info->hTabStateLeft, GWL_STYLE, ss->styleNew);
                    SetWindowLongPtrW(s_info->hTabStateRight, GWL_STYLE, ss->styleNew);

                    hdr.hwndFrom = s_info->hTabStateLeft;
                    hdr.code = TCN_SELCHANGE;
                    hdr.idFrom = 0;
                    SendMessageW(GetParent(hdr.hwndFrom), WM_NOTIFY, 0, (LPARAM)&hdr);
                    hdr.hwndFrom = s_info->hTabStateRight;
                    SendMessageW(GetParent(hdr.hwndFrom), WM_NOTIFY, 0, (LPARAM)&hdr);

                }

            }
        }
        return ret;
    }
    case TCM_SETIMAGELIST:
    {
        if (lParam) // ��0�Ĳ�����, �����µ�ͼƬ�����Ҫ����һ��
        {
            if (hWnd == hWndState && !s_info->config->tab_state_isIcon()) // ״̬�в�����ͼ��
                return 0;
            if (hWnd == hWndWork && !s_info->config->tab_work_isIcon())   // �����в�����ͼ��
                return 0;
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_DESTROY:
    {
        std::vector<int>* arr = Tab_GetArr(pData, 0, 0);
        delete arr;
        Tab_GetArr(pData, 0, true);
        _canvas_destroy(pData->draw.hCanvas);
        _canvas_destroy(_Tab_GetCanvas(pData));
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_NCCALCSIZE:
    {
        if (hWnd != s_info->hTabStateLeft)
            return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        LPOBJSTRUCT pState = s_info->pfnGetDataFromHwnd(s_info->hState);
        LPTOPSTATE_INFO info = (LPTOPSTATE_INFO)pState->param;
        if (__query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE))
            return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

        if (wParam)
        {
            if (TabCode_GetRightItemCount() < 1)
                return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

            LPNCCALCSIZE_PARAMS cs = (LPNCCALCSIZE_PARAMS)lParam;
            RECT& rc = cs->rgrc[0];
            rc.right -= WINDOW_SPACING;
            return WVR_ALIGNTOP | WVR_ALIGNLEFT | WVR_ALIGNBOTTOM | WVR_ALIGNRIGHT;
        }
        return 0;
    }
    case WM_NCPAINT:
    {
        if (hWnd != s_info->hTabStateLeft)
            return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        LPOBJSTRUCT pState = s_info->pfnGetDataFromHwnd(s_info->hState);
        LPTOPSTATE_INFO info = (LPTOPSTATE_INFO)pState->param;
        if (__query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE))
            return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        if (TabCode_GetRightItemCount() < 1)
            return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

        HDC hdcWindow = GetWindowDC(hWnd);
        RECT rc;
        GetWindowRect(hWnd, &rc);
        ScreenToClient(hWnd, (LPPOINT)&rc);
        ScreenToClient(hWnd, ((LPPOINT)&rc) + 1);
        rc.left = rc.right - WINDOW_SPACING;
        FillRect(hdcWindow, &rc, pData->draw.color.hbrNormal);
        ReleaseDC(hWnd, hdcWindow);
        return 0;
    }
    case WM_NCHITTEST:
    {
        if (hWnd != s_info->hTabStateLeft)
            return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        LPOBJSTRUCT pState = s_info->pfnGetDataFromHwnd(s_info->hState);
        LPTOPSTATE_INFO info = (LPTOPSTATE_INFO)pState->param;
        if (__query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE))
            return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        if (TabCode_GetRightItemCount() < 1)
            return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

        RECT rc;
        GetWindowRect(hWnd, &rc);
        const int x = LOWORD(lParam);
        if (x > rc.right - WINDOW_SPACING)
            return HTRIGHT;
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_GETMINMAXINFO:
    {
        LPMINMAXINFO pos = (LPMINMAXINFO)lParam;
        if (hWnd == s_info->hTabStateLeft)  // 
        {
            if (TabCode_GetRightItemCount() < 1)
                return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

            LPOBJSTRUCT pState = s_info->pfnGetDataFromHwnd(s_info->hState);
            LPTOPSTATE_INFO info = (LPTOPSTATE_INFO)pState->param;
            if (__query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE))
                return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

            RECT rc;
            GetClientRect(s_info->eWnd->GetStateTab(), &rc);

            // �����������С�ߴ�
            pos->ptMinTrackSize.x = 100;    // ��С�϶����
            pos->ptMaxTrackSize.x = rc.right - rc.left;  // ����϶����
            return 0;
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    default:
        //return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return 0;
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}

LRESULT CALLBACK WndProc_CustomTab(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
    case WM_PAINT:
    {
        BEGIN_END_PAINT_STRUCT ps;
        HDC hdc = s_info->iDraw->BeginPaint(pData, ps, false, true);
        s_info->iDraw->EndPaint(ps);
        break;
    }
    case WM_COMMAND:
    {
        SendMessageW(s_info->hEWnd, WM_COMMAND, wParam, lParam);
        break;
    }
    default:
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
}


void OnTabSelChanged(HWND hWnd, int index)
{
    HWND hState = 0;
    if (hWnd == s_info->hTabStateLeft || hWnd == s_info->hTabStateRight)
        hState = s_info->hState;
    else if (s_info->eWnd->GetStateTab() == hWnd)
        hState = s_info->hState;
    else if (s_info->eWnd->GetWorkTab() == hWnd)
        hState = s_info->hWork;
    else if (s_info->hWndTabControl == hWnd)
        hState = s_info->hControl;
    if (hState)
    {
        LPOBJSTRUCT pData = GetDataFromHwnd(hWnd);

        const int bufSize = 0x1000;
        wchar_t text[bufSize] = { 0 };
        TCITEMW item = { 0 };
        item.mask = TCIF_TEXT;
        item.pszText = text;
        item.cchTextMax = bufSize;
        TabCtrl_GetItem(hWnd, index, &item);
        int width = _tab_calc_text(text);
        std::vector<int>* arr = Tab_GetArr(pData, 0, false);
        int& nItemWidth = arr->at(index);
        if (nItemWidth != width)
            nItemWidth = width;
        TopState_SetWindowText(hState, text, true);
    }
}

LRESULT CALLBACK WndProc_OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    LPNMHDR hdr = (LPNMHDR)lParam;
    switch (hdr->code)
    {
    case TCN_SELCHANGE:
    {
        OnTabSelChanged(hdr->hwndFrom, TabCtrl_GetCurSel(hdr->hwndFrom));
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    default:
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}


