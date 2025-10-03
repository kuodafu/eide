#include <control/ReadColor.h>
#include <control/CMenu.h>
#include <control/Ctips.h>
#include "../EMenu.h"
#include <iDraw_const.h>
#include <Shlwapi.h>



#define MDICLIENT_CUSTOMTAB_INIAPPNAME  L"CustomCodeTab"    // ѡ��еĽ���


#define _query_lstate(_s)   ((s_info->tab.lState & _s) == _s)
#define _add_lstate(_s)     (s_info->tab.lState |= _s)
#define _del_lstate(_s)     (s_info->tab.lState &= ~_s)

#define _query_rstate(_s)   ((s_info->tab.rState & _s) == _s)
#define _add_rstate(_s)     (s_info->tab.rState |= _s)
#define _del_rstate(_s)     (s_info->tab.rState &= ~_s)

#define _query_state(_s)    ((_s == STATE::HOT) || (_s & STATE::DOWN))



LPCWSTR _get_MDIClient_child_title(LPCSTR szTitle, wstr& str, CUSTOMTAB_DATA& data, LPOBJSTRUCT pData);
// �ػ�ѡ���, index=�ػ�����Ŀ, ���Ϊ-1���ػ�����ѡ���
void MDIClient_CustomTabCtrl_InvalidateRect(LPTAB_INFO tabInfo, int index);
void _tab_updown_click(LPTAB_INFO tabInfo, bool isLeft);

// ѡ��еĵ������Ƿ���������ߵĵ��ڰ�ť
// pItemWidth = ���������Ӽе��ܿ��
bool _tab_updown_isleft_click(LPTAB_INFO tabInfo);
// ѡ��еĵ������Ƿ��������ұߵĵ��ڰ�ť
// pIsSub = �Ƿ���Ҫ����-1, �ڵ����ߴ��ʱ���ȡ���ֵ
bool _tab_updown_isright_click(LPTAB_INFO tabInfo, bool* pIsSub = 0);

// ѡ���ѡ�б��ı�, ���ظı�������, ��������¼���ʱ��ѡ�л�û�б��ı�, ���øı�ľͷ��� indexOld
// pData = ѡ��д�������
// index = �µı�ѡ�е�����
// indexOld = �ı�ǰѡ�е�����
// selMode = ѡ��ģʽ, �������ѡ�л��Ǵ���ѡ��, TAB_SEL_MODE ö�ٳ���
int _tab_sel_change(LPTAB_INFO tabInfo, int index, int indexOld, int selMode);

// �Ի�ѡ���ʹ�õĶ���, �ڲ���ʹ������֧��
class CustabQueue
{
    HWND* m_arr;
    int m_count;
    int m_bufCount;
public:
    CustabQueue()
    {
        m_bufCount = 100;
        m_arr = new HWND[m_bufCount];
        memset(m_arr, 0, sizeof(HWND) * m_bufCount);
        m_count = 0;
    }
    ~CustabQueue()
    {
        delete m_arr;
        memset(this, 0, sizeof(*this));
    }

    inline bool empty() { return !m_count || !m_arr; }
    // ��ȡ��һ����Ա��ŵľ��
    inline HWND front()
    {
        if (empty())return 0;
        return m_arr[0];
    }

    // ��ȡ���һ����Ա��ŵľ��
    inline HWND back()
    {
        if (empty())return 0;
        return m_arr[m_count - 1];
    }

    // ��һ���������������, �ڲ��������Ա����, ����ڲ�û�м�¼������, �������¼
    // �����һ����Ա���Ǵ���ľ��, �򲻽��в���, ֱ�ӷ��ظþ��, ���򷵻�ԭ�ȵĵ�һ����Ա�ľ��
    inline HWND push(HWND hWnd)
    {
        if (!hWnd)return 0;
        HWND hFront = front();
        if (hFront == hWnd) return hWnd;

        const bool isWnd = isHwnd(hWnd) != -1;
        const int oldCount = m_count;
        if (!isWnd) // ���Ǳ�������Ĵ���, ��Ҫ���뵽������, ��Ա����+1
        {
            if (m_count + 1 >= m_bufCount)
            {
                m_bufCount *= 2;
                HWND* ptr = new HWND[m_bufCount];
                memset(ptr, 0, sizeof(HWND) * m_bufCount);
                memcpy(ptr, m_arr, sizeof(HWND) * (m_bufCount / 2));
                delete[] m_arr;
                m_arr = ptr;
            }
            m_count++;
        }
        else
        {
            erase(hWnd);
            m_count++;
        }
        for (int i = m_count - 1; i > 0; --i)   // ����Ա��������ƶ�
        {
            m_arr[i] = m_arr[i - 1];
        }
        m_arr[0] = hWnd;
        return hFront;
    }

    // ������һ����Ա, �����һ����Ա
    inline HWND pop()
    {
        if (empty())return 0;
        HWND hFront = front();
        for (int i = 0; i < m_count - 1; i++)   // ������ĳ�Ա�����ǰ�ƶ�
        {
            m_arr[i] = m_arr[i + 1];
        }
        m_arr[m_count - 1] = 0;
        m_count--;
        return hFront;
    }

    // �ӱ�������ɾ��ָ����Ա
    inline bool erase(HWND hWnd)
    {
        const int index = isHwnd(hWnd);
        if (index == -1)return false;
        for (int i = index; i < m_count - 1; i++)
        {
            m_arr[i] = m_arr[i + 1];
        }
        m_arr[m_count - 1] = 0;
        m_count--;
        return true;
    }

    // �ж�һ�������Ƿ��Ǳ������еĳ�Ա, ���ش�������������±�, û���򷵻�-1
    inline int isHwnd(HWND hWnd)
    {
        if (empty() || !hWnd)return -1;
        for (int i = 0; i < m_count; ++i)
        {
            if (m_arr[i] == hWnd)
                return i;
        }
        return -1;
    }

    // ��������, ��ѡ��е����ݼ��뵽������
    inline void attach(std::vector<CUSTOMTAB_DATA>* hWnds)
    {
        if (!hWnds) return;
        int size = (int)hWnds->size();
        if (!size)return;

        if (size > m_bufCount)
        {
            const int oldBufCount = m_bufCount;
            m_bufCount = size + 100;
            HWND* ptr = new HWND[m_bufCount];
            memset(ptr, 0, sizeof(HWND) * m_bufCount);
            memcpy(ptr, m_arr, sizeof(HWND) * oldBufCount);
            delete[] m_arr;
            m_arr = ptr;
        }

        m_count = size;
        int i = 0;
        for (auto it = hWnds->begin(); it != hWnds->end(); ++it)
        {
            m_arr[i++] = (*it).hWnd;
        }

    }

};

static CustabQueue m_queue;
static CMenu m_menu;



void MDIClient_CustomTabCtrl_GetTabType(CUSTOMTAB_DATA& CustomTabData, const wchar_t* text);



LPOBJSTRUCT _subclass_MDIClient_CustomTab(HWND hWnd)
{
    //LPOBJSTRUCT data = s_info->iDraw->_subclass_tool(hWnd, WndProc_MDIClient_CustomTabCtrl, false, false, 0);      // �������л�����ѡ���, �Լ�������ѡ���
    //COLORINFO& cr = data->draw.color;
    ////const int offsetHot = 35;
    ////const int offsetDown = 20;
    ////const int offsetSel = 50;

    ////cr.crHot        = RGB(45 + offsetHot,   45 + offsetHot,     48 + offsetHot);
    ////cr.crDown       = RGB(45 + offsetDown,  45 + offsetDown,    48 + offsetDown);
    ////cr.crSelected   = RGB(45 + offsetSel,   45 + offsetSel,     48 + offsetSel);



    ////cr.hbrHot       = s_info->pfnCreateBrush(cr.crHot);
    ////cr.hbrDown      = s_info->pfnCreateBrush(cr.crDown);
    ////cr.hbrSelected  = s_info->pfnCreateBrush(cr.crSelected);
    return 0;
}




// ����رհ�ť��λ��
inline void MDIClient_CustomTabCtrl_CalcCloseRect(const RECT& rcItem, RECT& rcClose)
{
    const int itemHeight = rcItem.bottom - rcItem.top;
    int cxIcon, cyIcon;
    cyIcon = itemHeight - 2;
    cxIcon = cyIcon;

    rcClose.left = rcItem.right - cxIcon - 2;
    rcClose.top = rcItem.top + 2;
    rcClose.right = rcClose.left + cxIcon;
    rcClose.bottom = rcClose.top + cyIcon;
}
// �ߴ类�ı�, ��Ҫ�ж��Ƿ�滭���Ҽ�ͷ
inline void _tab_size(LPTAB_INFO tabInfo, int cxClient, int cyClient)
{
    const int tabRight = s_info->rcCustomTab.right;
    const int tabWidth = s_info->rcCustomTab.right - s_info->rcCustomTab.left;  // ѡ��п��
    bool isDraw = false;
    ICustomCodeTab& arr = *s_info->hWnds;
    int count = arr.size();
    int nItemWidth = 0; // �����ӼеĿ���ܺ�
    for (int i = 0; i < count; i++)
    {
        CUSTOMTAB_DATA& data = arr[i];
        nItemWidth += s_info->scale(data.width);
    }

    // ��������ӼеĿ�ȼ�������ѡ��еĿ��С, �Ǿ��ǲ���Ҫ��ʾ������
    if (nItemWidth < tabWidth)
    {
        if (tabInfo->isUpdown || tabInfo->indexFirst > 0)
            isDraw = true;  // ���������֮ǰ����ʾ״̬, �Ǿ���Ҫ�ػ�ѡ���
        tabInfo->isUpdown = false;
        tabInfo->indexFirst = 0;
        if (isDraw)
            InvalidateRect(s_info->hEWnd, &s_info->rcCustomTab, 0); // �ػ�ѡ���
        return;
    }
    // �ߵ��������Ҫ��ʾ������
    if (!tabInfo->isUpdown)
        isDraw = true;  // ���������֮ǰ������״̬, �Ǿ���Ҫ�ػ�ѡ���

    tabInfo->isUpdown = true;
    bool isSub = false;
    tabInfo->lState = _tab_updown_isleft_click(tabInfo) ? STATE::NORMAL : STATE::BAN;
    tabInfo->rState = _tab_updown_isright_click(tabInfo, &isSub) ? STATE::NORMAL : STATE::BAN;
    if (isSub)
        tabInfo->indexFirst--;
    if (isDraw || isSub)
        InvalidateRect(s_info->hEWnd, &s_info->rcCustomTab, 0); // �ػ�ѡ���
}
// ������
// tabInfo = ��ǰѡ�����Ϣ
// index = �����������, �ܴ��ݵ������һ������-1, ��С���Ӽ���
// pt = ������ʱ��λ��
// rcItem = �Ӽ�λ��
inline void MDIClient_CustomTabCtrl_MouseHover(LPTAB_INFO tabInfo, int index, const POINT& pt, RECT& rcItem)
{
    const bool isShowClose = s_info->config->IsShowCodeTabClose();

    RECT rcClose;
    MDIClient_CustomTabCtrl_CalcCloseRect(rcItem, rcClose);
    const bool isHoverClose = isShowClose ? PtInRect(&rcClose, pt) : false;
    if (tabInfo->indexHot == index)   // �ϴλ����������Ŀ��
    {
        if (isHoverClose)     // �ڹرհ�ť��
        {
            if (tabInfo->state != STATE::HOT)
            {
                tabInfo->state = STATE::HOT;
                MDIClient_CustomTabCtrl_InvalidateRect(tabInfo, index);
            }
        }
        else
        {
            if (tabInfo->state != STATE::NORMAL)
            {
                tabInfo->state = STATE::NORMAL;
                MDIClient_CustomTabCtrl_InvalidateRect(tabInfo, index);
            }
        }
        return;
    }

    if (tabInfo->indexHot != -1)
        MDIClient_CustomTabCtrl_InvalidateRect(tabInfo, tabInfo->indexHot);   // �ػ�����Ŀ
    
    tabInfo->indexHot = index;
    tabInfo->state = isHoverClose ? STATE::HOT : STATE::NORMAL;
    //TODO ��Ҫ�ж��Ƿ��ڹرհ�ť��
    MDIClient_CustomTabCtrl_InvalidateRect(tabInfo, tabInfo->indexHot);       // �ػ�����Ŀ

}

inline void MDIClient_CustomTabCtrl_TrackMouse(HWND hWnd)
{
    TRACKMOUSEEVENT tme = { 0 };
    tme.cbSize = sizeof(tme);
    tme.dwFlags = TME_LEAVE;
    tme.hwndTrack = hWnd;
    tme.dwHoverTime = HOVER_DEFAULT;
    TrackMouseEvent(&tme);

}

void MDIClient_CustomTabCtrl_ClickItem(int index, bool isClose, LPOBJSTRUCT pData, int mode)
{
    if (index < 0) return;
    CUSTOMTAB_DATA& CustomTabData = s_info->hWnds->at(index);
    const bool isShowClose = s_info->config->IsShowCodeTabClose();
    if (isClose && isShowClose)
    {
        SendMessageW(CustomTabData.hWnd, WM_CLOSE, 0, 0);   // ���ú������mdi����, Ȼ����޸�ѡ���ѡ����
        InvalidateRect(s_info->hEWnd, &s_info->rcCustomTab, 0);
        return;
    }

    _tab_sel_change(&s_info->tab, index, s_info->tab.indexSel, mode);

    if (mode != TAB_SEL_MODE_CODE)
    {
        LPOBJSTRUCT pParent = s_info->pfnGetDataFromHwnd(s_info->hMDIClient);
        //LPOBJSTRUCT pParent = s_info->pfnGetDataFromHwnd(CustomTabData.hWnd);
        //SetFocus(CustomTabData.hWnd);
        //{
        //    DWORD tid1 = GetCurrentThreadId();
        //    DWORD tid2 = GetWindowThreadProcessId(CustomTabData.hWnd, 0);
        //    AttachThreadInput(tid2, tid1, true);
        //    SetActiveWindow(CustomTabData.hWnd);
        //    AttachThreadInput(tid2, tid1, false);
        //}
        CallWindowProcA(pParent->oldProc, pParent->hWnd, WM_MDIACTIVATE, (WPARAM)CustomTabData.hWnd, (WPARAM)CustomTabData.hWnd);
        //SendMessageW(CustomTabData.hWnd, WM_MDIACTIVATE, 0, (LPARAM)CustomTabData.hWnd);
    }
    if (!IsWindowVisible(s_info->eWnd->GetControl32770Parent()))
        ShowWindowEx(s_info->eWnd->GetControl32770Parent(), SW_SHOW);
}

// ���¼���ѡ���, ��Ͷ�ݵ��������������ԵĴ���
inline void __Custom_Tab_ReLost()
{
    s_info->pfnConfig(INE_CONFIG_CHANGED, 0, (LPARAM)s_info);   // ֪ͨ�滭������²���滭����, �ڲ�ʹ��
    HWND hEWnd = GetWindow(GetDesktopWindow(), GW_CHILD);
    wchar_t clsName[260] = { 0 };
    while ( hEWnd )
    {
        if ( hEWnd != s_info->hEWnd )
        {

            GetClassNameW(hEWnd, clsName, 260);
            if ( wcsicmp_api(clsName, L"ENewFrame") == 0 )
                PostMessageW(hEWnd, LVM_UPDATE, 0, 0);  // Ͷ�ݸ��µ���Ϣ�����������ԵĴ���
        }
        hEWnd = GetWindow(hEWnd, GW_HWNDNEXT);
    }
    // LVM_UPDATE �����������ڴ��������Ϣ���ǵ��� s_info->pfnConfig(INE_CONFIG_CHANGED, wParam, (LPARAM)s_info);
}

inline bool _CustomTab_Menu_Command(HWND hWnd, int id, LPOBJSTRUCT pData)
{
    switch (id)
    {
    case ID_MENU_CLOSE:         // �ر�
    {
        const int index = TabCtrl_GetCurSel(hWnd);

        CUSTOMTAB_DATA& CustomTabData = s_info->hWnds->at(index);
        SendMessageW(CustomTabData.hWnd, WM_CLOSE, 0, 0);   // ���ú������mdi����, Ȼ����޸�ѡ���ѡ����
        InvalidateRect(s_info->hEWnd, &s_info->rcCustomTab, 0);
        //MDIClient_CustomTabCtrl_ClickItem(index, true, pData);
        break;
    }
    case ID_MENU_CLOSEOTHER:    // �ر�����
    {
        ICustomCodeTab& arr = *s_info->hWnds;
        const int index = TabCtrl_GetCurSel(hWnd);
        const int count = arr.size();
        for (int i = count - 1; i >= 0; --i)
        {
            if (i == index)continue;
            MDIClient_CustomTabCtrl_ClickItem(i, true, pData);
        }
        break;
    }
    case ID_MENU_CLOSEALL:      // �ر�ȫ��
    {
        ICustomCodeTab& arr = *s_info->hWnds;
        const int count = arr.size();
        for (int i = count - 1; i >= 0; --i)
        {
            MDIClient_CustomTabCtrl_ClickItem(i, true, pData);
        }
        break;
    }
    case ID_MENU_CLOSE_LEFT:         // �ر����
    {
        const int index = TabCtrl_GetCurSel(hWnd);
        ICustomCodeTab& arr = *s_info->hWnds;
        const int count = arr.size();
        if (index > count)
            break;
        for (int i = 0; i < index; ++i)
        {
            MDIClient_CustomTabCtrl_ClickItem(0, true, pData);
        }
        break;
    }
    case ID_MENU_CLOSE_RIGHT:        // �ر��ұ�
    {
        ICustomCodeTab& arr = *s_info->hWnds;
        const int index = TabCtrl_GetCurSel(hWnd);

        const int count = arr.size();
        for (int i = count - 1; i > index; --i)
        {
            MDIClient_CustomTabCtrl_ClickItem(i, true, pData);
        }
        break;
    }
    case ID_MENU_CLOSE_TOP:          // ������ʾ
    {
        //int direction = 1;
        //s_info->config->tab_code_direction(&direction);
        s_info->pfnSetInfo(s_info, IGI_CONFIG_CODE_DIRECTION, 1, true);
        break;
    }
    case ID_MENU_CLOSE_BOTTOM:       // �ײ���ʾ
    {
        s_info->pfnSetInfo(s_info, IGI_CONFIG_CODE_DIRECTION, 3, true);
        //int direction = 3;
        //s_info->config->tab_code_direction(&direction);
        break;
    }
    case ID_MENU_CLOSE_SHOWCLOSE:   // ��ʾ�رհ�ť
    {
        BOOL b = s_info->config->IsShowCodeTabClose() == false;
        s_info->config->IsShowCodeTabClose(&b);
        __Custom_Tab_ReLost();
        break;
    }
    default:
        return false;
    }
    return true;
}


inline int _tab_calctext(LPCWSTR text)
{
    LPOBJSTRUCT pData = s_info->pWnd;
    const float width = 1000;
    const float height = 50;
    float textWidth, textHeight;
    //LPEX_FONT hFont = g_scale.GetFont(pData->draw.hFont);
    _canvas_calctextsize(pData->draw.hCanvas, pData->draw.hFont, text, -1, 0, width, height, &textWidth, &textHeight);
    int ret = ((int)textWidth + 10);
    ret += LOWORD(s_info->tab.padding);
    return ret;
}

// index = Ҫ��ȡ���ε��Ӽ�
inline bool _tab_get_item_rect(DWORD index, RECT* prc)
{
    RECT rc;
    ICustomCodeTab& arr = *s_info->hWnds;
    DWORD size = (DWORD)arr.size();
    if (!prc || index >= size) return false;
    rc.left = s_info->rcCustomTab.left;
    rc.right = rc.left;
    rc.top = s_info->rcCustomTab.top;
    rc.bottom = s_info->rcCustomTab.bottom;
    for (DWORD i = s_info->tab.indexFirst; i < size; ++i)
    {
        CUSTOMTAB_DATA& data = arr[i];
        rc.left = rc.right;
        rc.right = rc.left + s_info->scale(data.width);
        if (i == index)
        {
            memcpy(prc, &rc, sizeof(RECT));
            return true;
        }
    }
    return false;
}

// �ػ�ѡ���, index=�ػ�����Ŀ, ���Ϊ-1���ػ�����ѡ���
void MDIClient_CustomTabCtrl_InvalidateRect(LPTAB_INFO tabInfo, int index)
{
    LPOBJSTRUCT pData = s_info->pWnd;
    RECT rcItem = { 0 };
    RECT* prc = &s_info->rcCustomTab;
    if (index > 0 && _tab_get_item_rect((DWORD)index, &rcItem))
        prc = &rcItem;
    InvalidateRect(pData->hWnd, prc, 0);
}

// �������λ���Ƿ����Ӽ���, �����Ӽ�����, ���û�����Ӽ��Ͼͷ���-1
// prc = ��������Ӽе�λ��
inline int _tab_hittest(LPTAB_INFO tabInfo, LPTCHITTESTINFO phi, RECT* prc = 0)
{
    // wParam = 0, lParam = TCHITTESTINFO �ṹָ��, �����Ӽ�����, ���û�����Ӽ��Ͼͷ���-1
    // pos->flags = 
    // TCHT_NOWHERE     ��λ�ò���ѡ��ϡ�
    // TCHT_ONITEM      ��λ��λ��ѡ��Ϸ�����δλ����ͼ������ı��Ϸ������������߻��Ƶ�ѡ��ؼ������λ����ѡ��ϵ��κ�λ�ã���ָ����ֵ��
    // TCHT_ONITEEMICON ��λ��λ��ѡ���ͼ���Ϸ���
    // TCHT_ONITEMLABEL ��λ��λ��ѡ����ı��Ϸ���
    // TCHT_ONITEM �Ƕ� TCHT_ONITEMICON �� TCHT_ONITEMLABEL �İ�λ�����㡣
    RECT rc;
    ICustomCodeTab& arr = *s_info->hWnds;
    DWORD size = (DWORD)arr.size();
    phi->flags = TCHT_NOWHERE;
    rc.left = s_info->rcCustomTab.left;
    rc.right = rc.left;
    rc.top = s_info->rcCustomTab.top;
    rc.bottom = s_info->rcCustomTab.bottom;
    if (phi->pt.y < rc.top || phi->pt.y > rc.bottom)
        return -1;
    for (DWORD i = tabInfo->indexFirst; i < size; ++i)
    {
        CUSTOMTAB_DATA& data = arr[i];
        rc.left = rc.right;
        rc.right = rc.left + s_info->scale(data.width);
        if (phi->pt.x >= rc.left && phi->pt.x < rc.right)
        {
            phi->flags = TCHT_ONITEM;
            if (prc)
                memcpy(prc, &rc, sizeof(RECT));
            return i;
        }
    }
    return -1;
}

// ��ѡ���������Ӽ�, �ɹ������Ӽ�����, ʧ�ܷ���-1
// index = Ҫ�����λ��
// item = ���������
// isUnicode = item����ṹ�Ƿ�ΪW��, ���Ϊfalse, �Ǿͻ�ת����W��
inline int _tab_insert(LPTAB_INFO tabInfo, DWORD index, TCITEMW* item, bool isUnicode)
{
    ICustomCodeTab& arr = *s_info->hWnds;
    DWORD count = (DWORD)arr.size();
    if (index > (DWORD)count)   // ����==��Ŀ��, �Ǿ��Ǽ��뵽���
        index = (DWORD)count;   // DWORD ���޷�������, ��С��0
    CUSTOMTAB_DATA info = { 0 };

    if (__query(item->mask, TCIF_IMAGE))
        info.iImage = item->iImage;
    if (__query(item->mask, TCIF_PARAM))
        info.lParam = item->lParam;
    if (__query(item->mask, TCIF_STATE))
        info.state = item->dwState;

    if (__query(item->mask, TCIF_TEXT))
    {
        if (isUnicode)
        {
            s_info->copyStrW(item->pszText, info.name);
        }
        else
        {
            wstr W = _str::A2W((LPSTR)item->pszText);
            s_info->copyStrW(W.c_str(), info.name);
        }
        info.width = _tab_calctext(info.name);
    }
    if (tabInfo->indexSel == -1)
        tabInfo->indexSel = 0;
    arr.insert(info, index);
    MDIClient_CustomTabCtrl_InvalidateRect(tabInfo, -1);
    _tab_size(tabInfo, s_info->pWnd->pos.width, s_info->pWnd->pos.height);
    return index;
}

// ��ѡ���������Ӽ�, �ɹ������Ӽ�����, ʧ�ܷ���-1
// index = Ҫ�����λ��
// item = ���������
// isUnicode = item����ṹ�Ƿ�ΪW��, ���Ϊfalse, �Ǿͻ�ת����W��
inline bool _tab_setitem(LPTAB_INFO tabInfo, DWORD index, TCITEMW* item, bool isUnicode)
{
    ICustomCodeTab& arr = *s_info->hWnds;
    DWORD count = (DWORD)arr.size();
    if (index >= count)
        return false;

    CUSTOMTAB_DATA& info = arr[index];

    if (__query(item->mask, TCIF_IMAGE))
        info.iImage = item->iImage;
    if (__query(item->mask, TCIF_PARAM))
        info.lParam = item->lParam;
    if (__query(item->mask, TCIF_STATE))
        info.state = item->dwState;

    if (__query(item->mask, TCIF_TEXT))
    {
        if (isUnicode)
        {
            s_info->copyStrW(item->pszText, info.name);
        }
        else
        {
            wstr W = _str::A2W((LPSTR)item->pszText);
            s_info->copyStrW(W.c_str(), info.name);
        }
        info.width = _tab_calctext(info.name);
        MDIClient_CustomTabCtrl_InvalidateRect(tabInfo, -1);
        _tab_size(tabInfo, s_info->pWnd->pos.width, s_info->pWnd->pos.height);
    }
    return true;
}
// ��ȡָ���Ӽе�����
// index = Ҫ�����λ��
// item = ���������
// isUnicode = item����ṹ�Ƿ�ΪW��, ���Ϊfalse, �Ǿͻ�ת����W��
inline bool _tab_getitem(LPTAB_INFO tabInfo, DWORD index, TCITEMW* item, bool isUnicode)
{
    ICustomCodeTab& arr = *s_info->hWnds;
    DWORD count = (DWORD)arr.size();
    if (index >= count)
        return false;

    CUSTOMTAB_DATA& info = arr[index];

    if (__query(item->mask, TCIF_IMAGE))
        item->iImage = info.iImage;
    if (__query(item->mask, TCIF_PARAM))
        item->lParam = info.lParam;
    if (__query(item->mask, TCIF_STATE))
        item->dwState = info.state;

    if (__query(item->mask, TCIF_TEXT) && item->cchTextMax > 0 && info.name)
    {
        if (isUnicode)
        {
            wcscpy_s((LPWSTR)item->pszText, item->cchTextMax, info.name);
        }
        else
        {
            _str A = wstr::W2A(info.name);
            strcpy_s((LPSTR)item->pszText, item->cchTextMax, A.c_str());
        }
    }
    return true;
}
// ���ù̶���ߵ�ѡ����, ���ؾɿ�Ⱥ͸߶ȡ� ���λ�ڷ���ֵ�� LOWORD �У��߶��� HIWORD��
// width = ѡ��Ŀ��, ��С20, С��20������Ϊ20
// height = ѡ��߶�, ��С�ǳ�ʼ��ʱ��ѡ��߶�
inline DWORD _tab_setitemsize(LPTAB_INFO tabInfo, int width, int height)
{
    if (height < s_info->pWnd->draw.nItemHeight)
        height = s_info->pWnd->draw.nItemHeight;
    if (width < 20)
        width = 20;
    LRESULT ret = (LRESULT)tabInfo->size;
    tabInfo->size = (DWORD)MAKELONG(width, height);
    _tab_size(tabInfo, s_info->pWnd->pos.width, s_info->pWnd->pos.height);
    return ret;
}


inline static void CALLBACK TimerProc_Leave_Tab(HWND hWnd, UINT message, UINT_PTR id, DWORD timer)
{
    LPOBJSTRUCT pData = (LPOBJSTRUCT)id;
    if (pData->draw.indexHot == -1)
    {
        // ���ѡ���û���ȵ���, ���ҵ�������״̬, ��Ҫ�ж�����Ƿ��ڵ�������
        
        if (s_info->tab.isUpdown && (_query_state(s_info->tab.lState) || _query_state(s_info->tab.rState)))
        {
            // �ߵ�����ǵ�������״̬, �ж�����Ƿ��ڵ�������
            RECT rcUpdown = s_info->rcCustomTab;
            rcUpdown.left = rcUpdown.right - 40;
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(s_info->hEWnd, &pt);
            if (PtInRect(&rcUpdown, pt))
                return; // ��껹�ڵ�������, ���ر�ʱ��
            // �ߵ�������Ѿ����ڵ�������, ��Ҫ�ػ�������
            if (_query_state(s_info->tab.lState))
                s_info->tab.lState = STATE::NORMAL;
            if (_query_state(s_info->tab.rState))
                s_info->tab.rState = STATE::NORMAL;
            InvalidateRect(s_info->hEWnd, &rcUpdown, 0);
        }
        KillTimer(hWnd, id);
        return;
    }
    POINT pt;
    RECT rcItem;
    GetCursorPos(&pt);
    ScreenToClient(s_info->hEWnd, &pt);
    TabCtrl_GetItemRect(s_info->hEWnd, pData->draw.indexHot, &rcItem);   // ȡ�ȵ����λ��
                                                                // �����겻���ȵ����о���Ҫ�ػ��ȵ���
    if (PtInRect(&rcItem, pt))
        return; // ��껹��ѡ�����, ������

    KillTimer(hWnd, id);
    pData->draw.indexHot = -1;
    InvalidateRect(s_info->hEWnd, &rcItem, 0);

    if (s_info->tab.isUpdown && (_query_state(s_info->tab.lState) || _query_state(s_info->tab.rState)))
    {
        // ��������״̬, ��Ҫȥ��״̬���ػ�
        if (_query_state(s_info->tab.lState))
            s_info->tab.lState = STATE::NORMAL;
        if (_query_state(s_info->tab.rState))
            s_info->tab.rState = STATE::NORMAL;
        RECT rcUpdown = s_info->rcCustomTab;
        rcUpdown.left = rcUpdown.right - 40;
        InvalidateRect(s_info->hEWnd, &rcUpdown, 0);
    }

}




void CALLBACK WndProc_MDIClient_CustomTabCtrl_DrawIcon(LPCANVAS hCanvas, RECT_F& rcItemF, LPOBJSTRUCT pData, CUSTOMTAB_DATA& CustomTabData)
{

    LPCWSTR programTreeId = L"default";
    switch (CustomTabData.type)
    {
    case CUSTOMTAB_INFO_WINDOW:         // ���������
        programTreeId = L"0x5000";
        break;
    case CUSTOMTAB_INFO_ASSEMBLY:       // ��ͨ����
    case CUSTOMTAB_INFO_ASSEMBLY_WINDOW:// ���ڳ���
        programTreeId = L"0x1000";
        break;
    case CUSTOMTAB_INFO_CLASS:          // ��
        programTreeId = L"0x1001";
        break;
    case CUSTOMTAB_INFO_GLOBALVAR:      // ȫ�ֱ���
        programTreeId = L"0x2000";
        break;
    case CUSTOMTAB_INFO_STRUCT:         // ��������
        programTreeId = L"0x3000";
        break;
    case CUSTOMTAB_INFO_DLL:            // DLL������
        programTreeId = L"0x4000";
        break;
    case CUSTOMTAB_INFO_CONST:          // ����
        programTreeId = L"0x6000";
        break;
    case CUSTOMTAB_INFO_IMAGE:          // ͼƬ��Դ
        programTreeId = L"0x70000001";
        break;
    case CUSTOMTAB_INFO_SOUND:          // ������Դ
        programTreeId = L"0x70000002";
        break;
    default:
        break;
    }

    RECT_F rcIcon;  // ͼ����ͼƬ���λ��
    RECT_F rcDraw;  // �滭�������ϵ�λ��
    LPWINDOW_ICON_DATA iconData = s_info->pResource->TreeProgram_GetIcon(programTreeId, &rcIcon, STATE::NORMAL, s_info->dpiX);
    LPEX_IMAGE pIcon = iconData ? iconData->hBitmapDx : 0;
    const float cxIcon = (float)s_info->scale((int)(pIcon ? rcIcon.width()  : 16.f));
    const float cyIcon = (float)s_info->scale((int)(pIcon ? rcIcon.height() : 16.f));
    const float offsetIconX = ((float)s_info->scale(20) - cxIcon) / 2;
    const float offsetIconY = (rcItemF.height() - cyIcon) / 2;

    rcDraw.left     = rcItemF.left + offsetIconX;
    rcDraw.top      = rcItemF.top  + offsetIconY;
    rcDraw.right    = rcDraw.left  + cxIcon;
    rcDraw.bottom   = rcDraw.top   + cyIcon;

    if (pIcon)  // ���Զ���ͼ��, ����ʹ���Զ���ͼ��
    {
        _canvas_drawimagerectrect(hCanvas, pIcon, rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom,
            rcIcon.left, rcIcon.top, rcIcon.right, rcIcon.bottom, 255);
        rcItemF.left += cxIcon;
    }
    else
    {
        HIMAGELIST hTreeImageList = 0;
        if (!hTreeImageList)
        {
            hTreeImageList = TreeView_GetImageList(s_info->eWnd->GetWorkProgramTree(), TVSIL_NORMAL);
            //TODO ��Ҫ����滭ͼ��
            //ImageList_DrawEx(hTreeImageList, CustomTabData.iImage, hdc,
            //    rcItem.left + offsetIcon, rcItem.top + offsetIcon, 16, 16, crBack, 0, ILD_NORMAL);
            rcItemF.left += 16;
        }
    }
}
inline static void WndProc_MDIClient_CustomTabCtrl_DrawClose(LPCANVAS hCanvas, RECT& rcItem, LPTAB_INFO tabInfo, int i, LPEX_IMAGE hIconClose, RECT_F& rcIconClose)
{
    RECT rcClose;
    MDIClient_CustomTabCtrl_CalcCloseRect(rcItem, rcClose);
    RECT_F rcCloseF(rcClose);
    if (i == tabInfo->indexHot)
    {
        COLORREF cr = 0;
        IThemeColor& theme = *s_info->themeColor;
        if (tabInfo->state == STATE::HOT)
            cr = theme.close_hot();
        else if (tabInfo->state == STATE::DOWN)
            cr = theme.close_down();

        if (cr)
        {
            LPEX_BRUSH hbr = s_info->pfnCreateBrushDx(RGB2ARGB(cr, 255));
            _canvas_fillRectangle(hCanvas, hbr, &rcCloseF);
        }
    }

    RECT_F rcDraw;  // Ҫ�滭�������ϵ�λ��
    const float cxClose = rcCloseF.width();
    const float cyClose = rcCloseF.height();
    const float cxIcon  = (float)s_info->scale((int)(hIconClose ? rcIconClose.width()  : 16.0f));
    const float cyIcon  = (float)s_info->scale((int)(hIconClose ? rcIconClose.height() : 16.0f));
    rcDraw.left         = rcClose.left  + ((cxClose - cxIcon) / 2);
    rcDraw.top          = rcClose.top   + ((cyClose - cxIcon) / 2);
    rcDraw.right        = rcDraw.left   + cxIcon;
    rcDraw.bottom       = rcDraw.top    + cyIcon;
    if (hIconClose)
    {
        _canvas_drawimagerectrect(hCanvas, hIconClose, rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom,
            rcIconClose.left, rcIconClose.top, rcIconClose.right, rcIconClose.bottom, 255);
    }
    else
    {
        static LPEX_IMAGE hImgClose = 0;
        if (!hImgClose)
        {
            const unsigned char close[] =
            {
                0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x14,
                0x00, 0x00, 0x00, 0x14, 0x08, 0x06, 0x00, 0x00, 0x00, 0x8D, 0x89, 0x1D, 0x0D, 0x00, 0x00, 0x00, 0x7F, 0x49, 0x44, 0x41,
                0x54, 0x38, 0x4F, 0x63, 0x64, 0xA0, 0x32, 0x60, 0xA4, 0xB2, 0x79, 0x0C, 0xA3, 0x06, 0x52, 0x1E, 0xA2, 0x78, 0xC3, 0xF0,
                0xD2, 0xF3, 0xEF, 0xF6, 0x7A, 0x92, 0x9C, 0x07, 0x91, 0xAD, 0xC1, 0x26, 0x86, 0x2C, 0x8F, 0xD3, 0x40, 0x90, 0xC6, 0x7F,
                0xFF, 0xFF, 0x1D, 0x60, 0x62, 0x64, 0x72, 0x80, 0x19, 0x8A, 0x4D, 0x0C, 0xDD, 0x4F, 0x04, 0x5D, 0x08, 0x33, 0x14, 0xA4,
                0x11, 0xDD, 0x02, 0x6C, 0x01, 0x44, 0x30, 0xD9, 0x80, 0x5C, 0xF5, 0xFF, 0xFF, 0xBF, 0x03, 0xFF, 0x19, 0x18, 0x18, 0x90,
                0x5D, 0x8B, 0x2B, 0xB4, 0x89, 0x32, 0x10, 0xE4, 0x32, 0x90, 0x42, 0x46, 0x24, 0xEF, 0x93, 0x65, 0x20, 0x72, 0x98, 0x51,
                0xEC, 0x65, 0x9A, 0x45, 0x0A, 0xD5, 0x92, 0x0D, 0xB9, 0x49, 0x9C, 0x60, 0xA4, 0x90, 0x6A, 0xF0, 0xA8, 0x81, 0xA4, 0x86,
                0x18, 0xA6, 0x7A, 0x00, 0x44, 0xD4, 0x59, 0x15, 0xE5, 0x08, 0x48, 0x47, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44,
                0xAE, 0x42, 0x60, 0x82,
            };

            hImgClose = _img_create_frommemory(close, sizeof(close) / sizeof(close[0]));
        }
        _canvas_drawimagerect(hCanvas, hImgClose, rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom, 255);
    }
}

LRESULT CALLBACK WndProc_MDIClient_CustomTabCtrl_OnPaint(LPTAB_INFO tabInfo, LPCANVAS hCanvas)
{
    if (!s_info->hWnds)
        return 0;
    LPOBJSTRUCT pData = s_info->pWnd;
    HWND hWnd = pData->hWnd;
    ICustomCodeTab& arr = *s_info->hWnds;
    int count = arr.size();
    const bool isShowClose = s_info->config->IsShowCodeTabClose();

    RECT_F rc(s_info->rcCustomTab);
    bool isTop = s_info->config->tab_code_direction() != 3;    // ����ѡ��е��Ӽз���, 3=�ױ�, ���� = ����
    RECT_F rcIconClose;
    LPWINDOW_ICON_DATA retData = s_info->pResource->TreeProgram_GetIcon(L"CodeTabClose", &rcIconClose, STATE::NORMAL, s_info->dpiX);
    LPEX_IMAGE hIconClose = retData ? retData->hBitmapDx : 0;
    int retType = retData ? retData->nIconType : 0;
    if (retType != 0 && retType != 2)   // �����û�ͼ��, Ҳ����Ĭ���������ͼ��, �Ǿ�ʹ�����õĹرհ�ť�ػ�
        hIconClose = 0;

    // �滭ѡ��еױ�/���ߵĺ���

    COLORREF crBack = s_info->themeColor->crBackExtern();
    LPEX_BRUSH hbrBack = s_info->pfnCreateBrushDx(RGB2ARGB(crBack, 255));
    _canvas_fillRectangle(hCanvas, hbrBack, &rc);

    RECT_F rcTmp = rc;
    if (isTop)  rcTmp.top = rcTmp.bottom - 2;
    else        rcTmp.bottom = rcTmp.top + 2;
    _canvas_fillRectangle(hCanvas, s_info->window_d2d_info.hbrBorder, &rcTmp);

    const int index = tabInfo->indexSel;    // ��ǰѡ���ѡ����
    const int ItemCount = (int)arr.size();  // ѡ�����Ŀ��

    HIMAGELIST hList = TabCtrl_GetImageList(hWnd);
    RECT rcItem = { s_info->rcCustomTab.left, s_info->rcCustomTab.top, s_info->rcCustomTab.left, s_info->rcCustomTab.bottom };
    if (isTop)  rcItem.bottom -= 2;
    else        rcItem.top += 2;
    int i = 0;
    if (tabInfo->isUpdown)
    {
        // ��Ҫ�滭������, ���òü���
        _canvas_setClip(hCanvas, rc.left, rc.top, rc.right - 40, rc.bottom, 0);
    }

    for (int i = tabInfo->indexFirst; i < count; ++i)
    {
        CUSTOMTAB_DATA& item = arr[i];
        rcItem.left = rcItem.right;
        rcItem.right = rcItem.left + s_info->scale(item.width);
        LPCWSTR text = item.name;

        RECT_F rcItemF(rcItem);
        //rcItemF.top = rcTmp.top;
        //rcItemF.bottom = rcTmp.bottom;
        bool isDrawClose = false;
        if (i == tabInfo->indexDown)        // ��ǰ���Ƶ��ǰ�����
        {
            //if (isTop)  rcItemF.top += 2;
            //else        rcItemF.bottom -= 2;
            if (tabInfo->state == STATE::DOWN)
            {
                LPEX_BRUSH hbr = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crHot, 255));
                _canvas_fillRectangle(hCanvas, hbr, &rcItemF);
            }
            else
            {
                LPEX_BRUSH hbr = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crDown, 255));
                _canvas_fillRectangle(hCanvas, hbr, &rcItemF);
            }
            //if (isTop)  rcItemF.top -= 2;
            //else        rcItemF.bottom += 2;
            isDrawClose = true;
        }
        else if (i == tabInfo->indexHot) // ��ǰ���Ƶ���Ŀ���ȵ���
        {
            //if (isTop)  rcItemF.top += 2;
            //else        rcItemF.bottom -= 2;
            LPEX_BRUSH hbr = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crHot, 255));
            _canvas_fillRectangle(hCanvas, hbr, &rcItemF);
            //if (isTop)  rcItemF.top -= 2;
            //else        rcItemF.bottom += 2;
            isDrawClose = true;
        }
        else if (i == index) // ��ǰ���Ƶ���Ŀ��ѡ����
        {
            LPEX_BRUSH hbr = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crSelected, 255));
            _canvas_fillRectangle(hCanvas, hbr, &rcItemF);
            isDrawClose = true;
        }

        if (isDrawClose && isShowClose) // �ȵ����ѡ������Ҫ�滭�رհ�ť
            WndProc_MDIClient_CustomTabCtrl_DrawClose(hCanvas, rcItem, tabInfo, i, hIconClose, rcIconClose);

        CUSTOMTAB_DATA& CustomTabData = s_info->hWnds->at(i);
        if (i == index) // ��ǰ���Ƶ���Ŀ��ѡ����
            s_info->pfnMove(IDC_IMOVE_GETCODETYPE, MAKELONG(CustomTabData.type, 1211), MAKELONG(CustomTabData.type, 1211));
        COLORREF crText = pData->draw.color.crText;
        if (CustomTabData.type != CUSTOMTAB_INFO::CUSTOMTAB_INFO_UNDEFINE)
        {
            crText = CustomTabData.crText;
        }
        int offset = 0, strLen = -1;
        if (!text[0]) continue;

        LPEX_BRUSH hbrText = s_info->pfnCreateBrushDx(RGB2ARGB(crText, 255));

        //rcItem.top += 2;
        // �ж��Ƿ�滭ͼ��
        if (s_info->config->tab_code_isIcon() && CustomTabData.type != 0)
            WndProc_MDIClient_CustomTabCtrl_DrawIcon(hCanvas, rcItemF, pData, CustomTabData);
        rcItemF.left += 5;
        LPEX_FONT hFont = g_scale.GetFont(pData->draw.hFont);
        _canvas_drawtext(hCanvas, hFont, hbrText, text + offset, -1, DT_VCENTER | DT_SINGLELINE, &rcItemF);
    }


    if (!tabInfo->isUpdown)
        return 0;

    // ��Ҫ�滭������, ��ԭ�ü���, Ȼ��滭������, ������ռ��40������
    _canvas_resetClip(hCanvas);

    RECT_F rcLeft;
    RECT_F rcRight;
    LPWINDOW_ICON_DATA retl = s_info->pResource->MsctlsUpdowm32_GetIcon_lr(L"0", &rcLeft, (STATE)tabInfo->lState, s_info->dpiX);
    LPWINDOW_ICON_DATA retr = s_info->pResource->MsctlsUpdowm32_GetIcon_lr(L"1", &rcRight, (STATE)tabInfo->rState, s_info->dpiX);

    const float cyItem = rc.height();
    if (retl)
    {
        RECT_F rcDraw(rc);
        RECT_F& rcIcon = rcLeft;
        const float offsetX = ((float)((int)(20 - rcIcon.width()) / 2));
        const float offsetY = ((float)((int)(cyItem - rcIcon.height()) / 2));
        rcDraw.left     = rcDraw.right - 40 + offsetX;  // ������һ��ռ��20����
        rcDraw.top      = rcDraw.top + offsetY;         // ������һ��ռ��20����
        rcDraw.right    = rcDraw.left + rcIcon.width();
        rcDraw.bottom   = rcDraw.top + rcIcon.height();
        _canvas_drawimagerectrect(hCanvas, retl->hBitmapDx, rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom,
            rcIcon.left, rcIcon.top, rcIcon.right, rcIcon.bottom, 255);
    }
    if (retr)
    {
        RECT_F rcDraw(rc);
        RECT_F& rcIcon = rcRight;
        const float offsetX = ((float)((int)(20 - rcIcon.width()) / 2));
        const float offsetY =  ((float)((int)(cyItem - rcIcon.height()) / 2));
        rcDraw.left     = rcDraw.right - 20 + offsetX;  // ������һ��ռ��20����
        rcDraw.top      = rcDraw.top + offsetY;         // ������һ��ռ��20����
        rcDraw.right    = rcDraw.left + rcIcon.width();
        rcDraw.bottom   = rcDraw.top + rcIcon.height();
        _canvas_drawimagerectrect(hCanvas, retr->hBitmapDx, rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom,
            rcIcon.left, rcIcon.top, rcIcon.right, rcIcon.bottom, 255);
    }
    return 0;
}

LRESULT CALLBACK WndProc_MDIClient_CustomTabCtrl(HWND hWndMessage, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HMENU hMenu = 0;
    LPTAB_INFO tabInfo = &s_info->tab;
    LPOBJSTRUCT pData = s_info->pWnd;
    HWND hWnd = pData->hWnd;
    ICustomCodeTab* pArr = s_info->hWnds;
    switch (message)
    {
    case WM_PAINT:
    {
        WndProc_MDIClient_CustomTabCtrl_OnPaint(tabInfo, (LPCANVAS)wParam);
        break;
    }
    case WM_DPICHANGED:
    {

        break;
    }
    case WM_SIZE:
        _tab_size(tabInfo, LOWORD(lParam), HIWORD(lParam));
        return 0;
    case WM_MOUSEMOVE:
    {
        if ((wParam & MK_LBUTTON) == MK_LBUTTON || (wParam & MK_RBUTTON) == MK_RBUTTON)
        {
            break;  // ����Ҽ�����ʱ������
        }
        TCHITTESTINFO hi = { 0 };
        POINT& pt = hi.pt;
        pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };


        if (tabInfo->isUpdown)
        {
            RECT rcUpdown = { s_info->rcCustomTab.right - 40, s_info->rcCustomTab.top, s_info->rcCustomTab.right, s_info->rcCustomTab.bottom };
            bool isDraw = false;
            if (PtInRect(&rcUpdown, pt))
            {
                SetTimer(s_info->hWndTabControl, (UINT_PTR)pData, 50, TimerProc_Leave_Tab);
                if (pt.x < rcUpdown.left + 20)
                {
                    if (_query_state(s_info->tab.rState))
                        s_info->tab.rState = STATE::NORMAL, isDraw = true;
                    // ���������ߵĵ�������
                    if (tabInfo->lState != STATE::BAN)
                        tabInfo->lState = STATE::HOT, isDraw = true;

                }
                else
                {
                    if (_query_state(s_info->tab.lState))
                        s_info->tab.lState = STATE::NORMAL, isDraw = true;
                    // ��������ұߵĵ�������
                    if (tabInfo->rState != STATE::BAN)
                        tabInfo->rState = STATE::HOT, isDraw = true;
   
                }
                if (isDraw)
                    InvalidateRect(s_info->hEWnd, &rcUpdown, 0);
                // ����ڵ��������ƶ�, �����¼�������
                return 0;
            }
            else
            {
                // �ſ����ʱ��겻��������������, ��������״̬������Ϊ����
                // ���������ť��һ����״̬, �Ǿ���Ҫȥ��״̬���ػ�
                if (_query_state(s_info->tab.lState))
                    s_info->tab.lState = STATE::NORMAL, isDraw = true;
                if (_query_state(s_info->tab.rState))
                    s_info->tab.rState = STATE::NORMAL, isDraw = true;
                if (isDraw)
                    InvalidateRect(s_info->hEWnd, &rcUpdown, 0);
            }
        }


        RECT rcItem;
        const int index = _tab_hittest(tabInfo, &hi, &rcItem);
        if (index > -1)
        {
            // �ߵ����������ĳ����Ŀ��
            SetTimer(s_info->hWndTabControl, (UINT_PTR)pData, 50, TimerProc_Leave_Tab);
            MDIClient_CustomTabCtrl_TrackMouse(hWnd);
            MDIClient_CustomTabCtrl_MouseHover(tabInfo, index, pt, rcItem);
            return 0;
        }

        // ��겻���Ӽ���, �ж�ԭ����û���ȵ���Ӽ�, ���ȵ��Ӽиı������״̬
        if (tabInfo->indexHot != -1)
        {
            MDIClient_CustomTabCtrl_InvalidateRect(tabInfo, tabInfo->indexHot);
            tabInfo->indexHot = -1;
        }
        // �ߵ�������ǲ������κ���Ŀ��...
        //__debugbreak();
        break;
    }
    case WM_MOUSELEAVE:
    {
        if (tabInfo->indexHot != -1)
            MDIClient_CustomTabCtrl_InvalidateRect(tabInfo, tabInfo->indexHot);   // �ػ�����Ŀ
        tabInfo->indexHot = -1;
        break;
    }
    case WM_CAPTURECHANGED:
    {
        TCHITTESTINFO hi = { 0 };
        POINT& pt = hi.pt;
        GetCursorPos(&pt);
        ScreenToClient(hWnd, &pt);
        if (tabInfo->indexDown == -2 && tabInfo->isUpdown)
        {
            tabInfo->indexDown = -1;
            RECT rcUpdown = { s_info->rcCustomTab.right - 40, s_info->rcCustomTab.top, s_info->rcCustomTab.right, s_info->rcCustomTab.bottom };
            if (PtInRect(&rcUpdown, pt))
            {
                if (pt.x < rcUpdown.left + 20)
                {
                    // ���������ߵĵ�������
                    if (tabInfo->lState == STATE::DOWN)
                    {
                        // ���ºͷſ�������������ڰ�ť��, ��Ҫ�����������������¼�
                        _tab_updown_click(tabInfo, true);
                    }
                    if (tabInfo->lState != STATE::BAN)
                        tabInfo->lState = STATE::HOT;
                    if (_query_state(s_info->tab.rState))
                        s_info->tab.rState = STATE::NORMAL;
                }
                else
                {
                    // ��������ұߵĵ�������
                    // ���������ߵĵ�������
                    if (tabInfo->rState == STATE::DOWN)
                    {
                        // ���ºͷſ�������������ڰ�ť��, ��Ҫ�����������������¼�
                        _tab_updown_click(tabInfo, false);
                    }
                    if (_query_state(s_info->tab.lState))
                        s_info->tab.lState = STATE::NORMAL;
                    if (tabInfo->rState != STATE::BAN)
                        tabInfo->rState = STATE::HOT;
                }
                InvalidateRect(s_info->hEWnd, &rcUpdown, 0);
                // ��������ǵ�����, �����¼�������
                return 0;
            }
            else
            {
                // �ſ����ʱ��겻��������������, ��������״̬������Ϊ����
                // ���������ť��һ����״̬, �Ǿ���Ҫȥ��״̬���ػ�
                bool isDraw = false;
                if (_query_state(s_info->tab.lState))
                    s_info->tab.lState = STATE::NORMAL, isDraw = true;
                if (_query_state(s_info->tab.rState))
                    s_info->tab.rState = STATE::NORMAL, isDraw = true;
                if (isDraw)
                    InvalidateRect(s_info->hEWnd, &rcUpdown, 0);
            }
        }
        RECT rcItem;
        const int index = _tab_hittest(tabInfo, &hi, &rcItem);  // ��ǰ���������Ŀ
        const int indexOld = tabInfo->indexSel;                 // ��һ��ѡ�е���Ŀ, ��Ҫ�ػ������Ŀ
        if (index == tabInfo->indexDown)        // �ſ�ʱ��껹��ԭ�����µ���Ŀ��
        {
            tabInfo->indexDown = -1;            // �����������
            if (tabInfo->state == STATE::DOWN)
            {
                // �ߵ������֮ǰ���µ��ǹرհ�ť
                tabInfo->state = STATE::NORMAL; // ����رհ�ť����״̬
                RECT rcClose;
                MDIClient_CustomTabCtrl_CalcCloseRect(rcItem, rcClose);
                if (PtInRect(&rcClose, pt)) // �ſ��Ļ��ǹرհ�ť
                {

                    MDIClient_CustomTabCtrl_ClickItem(index, true, pData); // �����رհ�ť�������¼�
                    return 0;
                }
                // �ߵ�������ǰ���ʱ�ڹرհ�ť��, �ſ�ʱ����
            }
            else
            {
                // ��Ŀ������
                MDIClient_CustomTabCtrl_ClickItem(index, false, pData); // ����ѡ��������¼�
            }
        }
        else
        {
            // ���ſ�����ԭ������Ŀ��
            //pInfo->indexHot = -1;
        }

        if (indexOld != -1)
            MDIClient_CustomTabCtrl_InvalidateRect(tabInfo, indexOld);  // �ػ�����Ŀ
        tabInfo->indexDown = -1;
        break;
    }
    case WM_LBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
    {
        const POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        if (tabInfo->isUpdown)
        {
            RECT rcUpdown = { s_info->rcCustomTab.right - 40, s_info->rcCustomTab.top, s_info->rcCustomTab.right, s_info->rcCustomTab.bottom };
            if (PtInRect(&rcUpdown, pt))
            {
                bool isDraw = false, isCapture = false;
                if (pt.x < rcUpdown.left + 20)
                {
                    // ���������ߵĵ�������
                    if (tabInfo->lState != STATE::BAN)
                        tabInfo->lState = STATE::DOWN, isDraw = true, isCapture = true;
                    if (_query_state(s_info->tab.rState))
                        s_info->tab.rState = STATE::NORMAL, isDraw = true;
                }
                else
                {
                    // ��������ұߵĵ�������
                    if (_query_state(s_info->tab.lState))
                        s_info->tab.lState = STATE::NORMAL, isDraw = true;
                    if (tabInfo->rState != STATE::BAN)
                        tabInfo->rState = STATE::DOWN, isDraw = true, isCapture = true;
                }
                if (isCapture)
                {
                    tabInfo->indexDown = -2;
                    SetCapture(hWnd);
                }
                if (isDraw)
                    InvalidateRect(s_info->hEWnd, &rcUpdown, 0);
                //HACK ��Ҫ���¾͵���λ�õĿ������������, ���߳�������������, ��Ҫ�����ﴴ����ʱ��
                //_tab_updown_click(tabInfo, pt.x < rcUpdown.left + 20);
                return DefWindowProcW(hWnd, message, wParam, lParam);
            }

            // �ſ����ʱ��겻��������������, ��������״̬������Ϊ����
            // ���������ť��һ����״̬, �Ǿ���Ҫȥ��״̬���ػ�
            bool isDraw = false;
            if (_query_state(s_info->tab.lState))
                s_info->tab.lState = STATE::NORMAL, isDraw = true;
            if (_query_state(s_info->tab.rState))
                s_info->tab.rState = STATE::NORMAL, isDraw = true;
            if (isDraw)
                InvalidateRect(s_info->hEWnd, &rcUpdown, 0);
        }

        if (tabInfo->indexHot == -1)
            return 0;

        tabInfo->indexDown = tabInfo->indexHot;
        if ( s_info->config->IsShowCodeTabClose() )
        {
            RECT rcClose, rcItem;
            _tab_get_item_rect(tabInfo->indexHot, &rcItem);
            MDIClient_CustomTabCtrl_CalcCloseRect(rcItem, rcClose);
            const bool isHoverClose = PtInRect(&rcClose, pt);
            if ( isHoverClose )
                tabInfo->state = STATE::DOWN;   // ���ڹرհ�ť��
        }

        MDIClient_CustomTabCtrl_InvalidateRect(tabInfo, tabInfo->indexDown);   // �ػ�����Ŀ
        SetCapture(hWnd);
        return DefWindowProcW(hWnd, message, wParam, lParam);
        //return WndProc_Tab(hWnd, message, wParam, lParam, pData);
    }
    case WM_LBUTTONUP:
    {
        ReleaseCapture();
        break;
    }
    case WM_RBUTTONDOWN:
    {
        // �Ҽ����ҲҪ�л������ѡ��
        TCHITTESTINFO tti = { 0 };
        tti.pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        const int indexDown = _tab_hittest(tabInfo, &tti);
        tabInfo->indexDown = indexDown;
        WndProc_MDIClient_CustomTabCtrl(hWnd, WM_CAPTURECHANGED, 0, (LPARAM)hWnd);
        tabInfo->indexDown = indexDown;
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    case WM_RBUTTONUP:
    {
        if (!hMenu)
        {
            HMENU hMenu = CreatePopupMenu();
            EMenu_InsertMenu(hMenu);

            AppendMenuW(hMenu, MF_STRING, ID_MENU_CLOSE         , L"�ر�(&C)");
            AppendMenuW(hMenu, MF_STRING, ID_MENU_CLOSEOTHER    , L"�ر�����(&O)");
            AppendMenuW(hMenu, MF_STRING, ID_MENU_CLOSEALL      , L"ȫ���ر�(&L)");
            AppendMenuW(hMenu, MF_STRING, ID_MENU_CLOSE_LEFT    , L"�ر��������");
            AppendMenuW(hMenu, MF_STRING, ID_MENU_CLOSE_RIGHT   , L"�ر��ұ�����");
            AppendMenuW(hMenu, MF_SEPARATOR, 0, 0);
            AppendMenuW(hMenu, MF_STRING, ID_MENU_CLOSE_TOP     , L"������ʾ");
            AppendMenuW(hMenu, MF_STRING, ID_MENU_CLOSE_BOTTOM  , L"�ײ���ʾ");
            AppendMenuW(hMenu, MF_SEPARATOR, 0, 0);
            const bool isShowClose = s_info->config->IsShowCodeTabClose();
            AppendMenuW(hMenu, isShowClose ? MF_CHECKED : MF_STRING, ID_MENU_CLOSE_SHOWCLOSE, L"��ʾ�رհ�ť");
            m_menu(hWnd, hMenu);
        }
        
        bool isTop = s_info->config->tab_code_direction() != 3;
        TCHITTESTINFO tti = { 0 };
        tti.pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        int index = TabCtrl_HitTest(hWnd, &tti);
        int count = TabCtrl_GetItemCount(hWnd);

        m_menu.enabled(ID_MENU_CLOSE_TOP    , !isTop);
        m_menu.enabled(ID_MENU_CLOSE_BOTTOM , isTop);

        m_menu.enabled(ID_MENU_CLOSEOTHER   , 1);
        m_menu.enabled(ID_MENU_CLOSE_LEFT   , 1);
        m_menu.enabled(ID_MENU_CLOSE_RIGHT  , 1);
        m_menu.enabled(ID_MENU_CLOSE        , 1);
        m_menu.enabled(ID_MENU_CLOSEALL     , 1);
        if (index == -1 || count == 1)
        {
            // �Ҽ�����Ĳ���ѡ����, ����ֻ��һ��ѡ�, �ر�����ұ߶���Ҫ��ֹ
            m_menu.enabled(ID_MENU_CLOSEOTHER   , 0);
            m_menu.enabled(ID_MENU_CLOSE_LEFT   , 0);
            m_menu.enabled(ID_MENU_CLOSE_RIGHT  , 0);
            if (index == -1)
            {
                m_menu.enabled(ID_MENU_CLOSE    , 0);
                m_menu.enabled(ID_MENU_CLOSEALL , 0);
            }

        }
        else if (index == 0)
        {
            // ����˵�һ��ѡ�, �ر������Ҫ��ֹ
            m_menu.enabled(ID_MENU_CLOSE_LEFT   , 0);
        }
        else if (index + 1 == count)
        {
            // ��������һ��ѡ�, �ر��ұ���Ҫ��ֹ
            m_menu.enabled(ID_MENU_CLOSE_RIGHT  , 0);
        }
        m_menu.pop();
        break;
    }
    case WM_COMMAND:
        return (LRESULT)_CustomTab_Menu_Command(hWnd, (int)wParam, pData);
    
    case WM_ERASEBKGND:
        return 1;
    case WM_MDIDESTROY:
    {
        if (lParam != 121007124)
            return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
        m_queue.erase((HWND)wParam);
        HWND hFront = m_queue.front();
        const int count = s_info->hWnds->size();
        for (int i = 0; i < count; i++)
        {
            if (s_info->hWnds->at(i).hWnd == hFront)
            {
                MDIClient_CustomTabCtrl_ClickItem(i, false, pData);
                SetWindowPos(hFront, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOCOPYBITS);
                ShowWindowEx(hFront, SW_SHOW);
                return 0;
            }
        }
        return 0;
    }
    case WM_SETTEXT:
    {
        const int index = tabInfo->indexSel;
        if (index == -1) return 0;

        CUSTOMTAB_DATA* pTabData = 0;
        for ( int i = 0; i < s_info->hWnds->size(); i++ )
        {
            CUSTOMTAB_DATA& data = s_info->hWnds->at(index);
            if ( hWndMessage == data.hWnd )
            {
                pTabData = &data;
                break;
            }
        }
        

        if ( !pTabData )
        {
            pTabData = &(s_info->hWnds->at(index));
            return 0;
        }

        TCITEMW item = { 0 };
        wstr str;
        LPCWSTR text = _get_MDIClient_child_title((LPCSTR)lParam, str, *pTabData, pData);
        item.mask = TCIF_TEXT | TCIF_PARAM;
        item.pszText = (LPWSTR)text;
        item.lParam = (LPARAM)pTabData->hWnd;
        _tab_setitem(tabInfo, index, &item, true);
        InvalidateRect(s_info->hEWnd, &s_info->rcCustomTab, 0);
        //WndProc_MDIClient_CustomTabCtrl(WM_SIZE, 0, MAKELONG(s_info->pWnd->pos.width, s_info->pWnd->pos.height));
        _tab_size(tabInfo,s_info->pWnd->pos.width, s_info->pWnd->pos.height);

        return 0;
    }
    case WM_STYLECHANGING:
    {
        if (wParam == GWL_STYLE)
        {
            LPSTYLESTRUCT ss = (LPSTYLESTRUCT)lParam;
            int oldDirection = _tab_getdirection(ss->styleOld);
            int newDirection = _tab_getdirection(ss->styleNew);
            if (ss->styleNew & TCS_MULTILINE) ss->styleNew &= ~(TCS_MULTILINE); // ���ܼ��������ʽ

            if (oldDirection != newDirection)   // ��ʽ�б䶯
            {
                if (newDirection != 1 && newDirection != 3) // ���Ƕ���, Ҳ���ǵױ�, �Ǿ�ǿ���϶���
                {
                    ss->styleNew = ss->styleOld;   // ���ı���ʽ, ����ʹ��ԭ������ʽ
                    //ss->styleNew &= ~(TCS_VERTICAL | TCS_BOTTOM);   // ȥ����ߺ͵ױߵ���ʽ
                }
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
            if (oldDirection != newDirection)
            {
                newDirection = newDirection == 3 ? newDirection : 1;
                //int oldDirection = s_info->config->tab_code_direction(&newDirection);
                LPOBJSTRUCT wnd = GetDataFromHwnd(s_info->hEWnd);
                if (wnd)
                    SendMessageW(s_info->hEWnd, WM_SIZE, 0, MAKELONG(wnd->pos.width, wnd->pos.height));  // ���µ����ߴ�

            }
        }
        return ret;
    }
    case TCM_DELETEALLITEMS:     // wParam, lParam == 0;
    {
        pArr->clear();
        tabInfo->indexSel = -1;
        MDIClient_CustomTabCtrl_InvalidateRect(tabInfo, -1);
        _tab_size(tabInfo, s_info->pWnd->pos.width, s_info->pWnd->pos.height);
        return 1;
    }
    case TCM_DELETEITEM:     // wParam = ��Ŀ����, lParam == 0;
    {
        size_t count = pArr->size();
        if (wParam < 0 || wParam >= count)
            return 0;
        int indexNew = tabInfo->indexSel;
        if (count == 1)
        {
            indexNew = -1;
        }
        else
        {
            if (indexNew - 1 < 0)
                indexNew = 0;
            else if (indexNew + 1 >= (int)count)
                indexNew = count - 2;
            else
                indexNew--;
        }

        pArr->erase(wParam);
        _tab_sel_change(tabInfo, indexNew, tabInfo->indexSel, TAB_SEL_MODE_CODE);
        MDIClient_CustomTabCtrl_InvalidateRect(tabInfo, -1);
        _tab_size(tabInfo, s_info->pWnd->pos.width, s_info->pWnd->pos.height);
        return 1;
    }
    case TCM_GETCURFOCUS:   // wParam, lParam == 0;
        return tabInfo->indexDown;
    case TCM_GETCURSEL:     // wParam, lParam == 0;
        return tabInfo->indexSel;
        //case TCM_GETIMAGELIST:
    case TCM_GETITEMA:
    case TCM_GETITEMW:      // wParam = ����, lParam = TCITEM �ṹָ��, ����ɹ����򷵻� TRUE ; ���򷵻� FALSE
        return (LRESULT)_tab_getitem(tabInfo, (DWORD)wParam, (LPTCITEMW)lParam, message == TCM_GETITEMW);
    case TCM_GETITEMCOUNT:
        return (LRESULT)pArr->size();
    case TCM_GETITEMRECT:   // wParam = ����, lParam = RECT �ṹָ��, �������
        return (LRESULT)_tab_get_item_rect((DWORD)wParam, (RECT*)lParam);
    case TCM_HITTEST:       // wParam = 0, lParam = TCHITTESTINFO �ṹָ��, �����Ӽ�����, ���û�����Ӽ��Ͼͷ���-1
        return (LRESULT)_tab_hittest(tabInfo, (LPTCHITTESTINFO)lParam);

    case TCM_INSERTITEMA:
    case TCM_INSERTITEMW:   // wParam = ����, lParam = TCITEM �ṹָ��, ����ɹ����򷵻��µ�ѡ�������; ���򷵻�-1
        return (LRESULT)_tab_insert(tabInfo, (DWORD)wParam, (LPTCITEMW)lParam, message == TCM_INSERTITEMW);
    case TCM_SETCURFOCUS:   // wParam = ����, lParam = 0, û�з���ֵ
        tabInfo->indexDown = (int)wParam;
        break;
    case TCM_SETCURSEL:     // wParam = ����, lParam = 0, û�з���ֵ
        tabInfo->indexSel = (int)wParam;
        //tabInfo->indexSel = _tab_sel_change(tabInfo, (int)wParam, tabInfo->indexSel, TAB_SEL_MODE::TAB_SEL_MODE_CODE);
        break;
    case TCM_SETITEMA:
    case TCM_SETITEMW:      // wParam = ����, lParam = TCITEM �ṹָ��, ����ɹ����򷵻� TRUE ; ���򷵻� FALSE
        return (LRESULT)_tab_setitem(tabInfo, (DWORD)wParam, (LPTCITEMW)lParam, message == TCM_SETITEMW);
    case TCM_SETITEMSIZE:   // wParam = 0, lParam = LOWORD=���, HIWORD=�߶�, ���ؾɿ�Ⱥ͸߶ȡ� ���λ�ڷ���ֵ�� LOWORD �У��߶��� HIWORD��
        return (LRESULT)_tab_setitemsize(tabInfo, LOWORD(lParam), HIWORD(lParam));
    case TCM_SETMINTABWIDTH:
    {
        // wParam = 0
        // lParam = Ϊѡ��ؼ������õ���С��ȡ� ������˲�������Ϊ-1����ؼ���ʹ��Ĭ��ѡ����
        // ������һ����С���
        LRESULT ret = (LRESULT)tabInfo->minWidth;
        tabInfo->minWidth = lParam;
        _tab_size(tabInfo, s_info->pWnd->pos.width, s_info->pWnd->pos.height);
        return ret;
    }
    case TCM_SETPADDING:
    {
        // wParam = 0
        // lParam = ÿ��ѡ������ռ�, ��λ = �����, ��λ = ���߶�
        // û�з���ֵ
        tabInfo->padding = lParam;
        _tab_size(tabInfo, s_info->pWnd->pos.width, s_info->pWnd->pos.height);
        break;
    }
    default:
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}
LPCWSTR _get_MDIClient_child_title(LPCSTR szTitle, wstr& str, CUSTOMTAB_DATA& data, LPOBJSTRUCT pData)
{
    if (pData->fnCallProc == CallWindowProcW)
        str = (LPCWSTR)szTitle;
    else
        str = _str::A2W(szTitle);
    LPWSTR text = str.data();

    MDIClient_CustomTabCtrl_GetTabType(data, text);

    wchar_t* npos = wcschr(text, ':');
    int offset = 0;
    if (npos)   // ��ð�ű�ʾ����Ǵ��������
    {
        ++npos; ++npos;
        offset = npos - text;

        wchar_t* npos2 = wcschr(npos, '/');
        if (npos2)
        {
            data.type = CUSTOMTAB_INFO::CUSTOMTAB_INFO_ASSEMBLY_WINDOW;
            IThemeColor& theme = *s_info->themeColor;
            data.crText = theme.crTab_text_assembly_window();
            --npos2;
            npos2[0] = 0;
        }
    }
    return text + offset;
}

void MDIClient_CustomTabCtrl_GetTabType(CUSTOMTAB_DATA& CustomTabData, const wchar_t* text)
{
    IThemeColor& theme = *s_info->themeColor;
    
    if (wcsnicmp_api(text, L"����", 2) == 0)
    {
        CustomTabData.type = CUSTOMTAB_INFO::CUSTOMTAB_INFO_WINDOW;
        CustomTabData.iImage = 6;
        CustomTabData.crText = theme.crTab_text_window();
    }
    else if (wcsnicmp_api(text, L"����", 3) == 0)
    {
        CustomTabData.type = CUSTOMTAB_INFO::CUSTOMTAB_INFO_ASSEMBLY;
        CustomTabData.iImage = 3;
        CustomTabData.crText = theme.crTab_text_assembly();
    }
    else if (wcsnicmp_api(text, L"��ģ��", 3) == 0)
    {
        CustomTabData.type = CUSTOMTAB_INFO::CUSTOMTAB_INFO_CLASS;
        CustomTabData.iImage = 0;
        CustomTabData.crText = theme.crTab_text_class();
    }
    else if (wcsnicmp_api(text, L"ȫ�ֱ�����", 5) == 0)
    {
        CustomTabData.type = CUSTOMTAB_INFO::CUSTOMTAB_INFO_GLOBALVAR;
        CustomTabData.iImage = 1;
        CustomTabData.crText = theme.crTab_text_globalvar();
    }
    else if (wcsnicmp_api(text, L"�Զ����������ͱ�", 8) == 0)
    {
        CustomTabData.type = CUSTOMTAB_INFO::CUSTOMTAB_INFO_STRUCT;
        CustomTabData.iImage = 0;
        CustomTabData.crText = theme.crTab_text_struct();
    }
    else if (wcsnicmp_api(text, L"Dll������", 8) == 0)
    {
        CustomTabData.type = CUSTOMTAB_INFO::CUSTOMTAB_INFO_DLL;
        CustomTabData.iImage = 4;
        CustomTabData.crText = theme.crTab_text_dll();
    }
    else if (wcsnicmp_api(text, L"�������ݱ�", 5) == 0)
    {
        CustomTabData.type = CUSTOMTAB_INFO::CUSTOMTAB_INFO_CONST;
        CustomTabData.iImage = 9;
        CustomTabData.crText = theme.crTab_text_const();
    }
    else if (wcsnicmp_api(text, L"ͼƬ��Դ��", 5) == 0)
    {
        CustomTabData.type = CUSTOMTAB_INFO::CUSTOMTAB_INFO_IMAGE;
        CustomTabData.iImage = 11;
        CustomTabData.crText = theme.crTab_text_image();
    }
    else if (wcsnicmp_api(text, L"������Դ��", 5) == 0)
    {
        CustomTabData.type = CUSTOMTAB_INFO::CUSTOMTAB_INFO_SOUND;
        CustomTabData.iImage = 10;
        CustomTabData.crText = theme.crTab_text_sound();
    }

}

//#define CUSTOMTAB_INFO_WINDOW               1   // ���������
//#define CUSTOMTAB_INFO_ASSEMBLY             2   // ��ͨ����
//#define CUSTOMTAB_INFO_ASSEMBLY_WINDOW      3   // ���ڳ���
//#define CUSTOMTAB_INFO_CLASS                4   // ��
//#define CUSTOMTAB_INFO_GLOBALVAR            5   // ȫ�ֱ���
//#define CUSTOMTAB_INFO_STRUCT               6   // ��������
//#define CUSTOMTAB_INFO_DLL                  7   // DLL������
//#define CUSTOMTAB_INFO_CONST                8   // ����
//#define CUSTOMTAB_INFO_IMAGE                9   // ͼƬ��Դ
//#define CUSTOMTAB_INFO_SOUND                10  // ������Դ





// ѡ���ѡ�б��ı�, ���ظı�������, ��������¼���ʱ��ѡ�л�û�б��ı�, ���øı�ľͷ��� indexOld
// pData = ѡ��д�������
// index = �µı�ѡ�е�����
// indexOld = �ı�ǰѡ�е�����
// selMode = ѡ��ģʽ, �������ѡ�л��Ǵ���ѡ��, TAB_SEL_MODE ö�ٳ���
int _tab_sel_change(LPTAB_INFO tabInfo, int index, int indexOld, int selMode)
{
    if (indexOld == index)   // ��һ��ѡ�к�����ѡ����һ��
        return index;

    ICustomCodeTab& arr = *s_info->hWnds;
    int size = (int)arr.size();
    if (size <= index)
        return index;

#ifdef _DEBUG
    if (index >= size)
        __debugbreak();
#endif

    tabInfo->indexSel = index;
    //wstr::dbg(L"------------�����л�ѡ����л��¼�, �л�ǰ���� = %d, �л������� = %d, �ɾ�� = %d, �¾�� = %d\n", indexOld, index, hOld, hWndShow);

    IRE_STRUCT_TAB_SELCHANGE evt = { 0 };
    evt.indexOld = indexOld;
    evt.index = index;
    HWND hWndShow = 0, hOld = 0;
    if (index > -1)
        hWndShow = arr[index].hWnd;    // ��Ҫ�޸�λ�õĴ���
    
    if (hWndShow)
        hOld = m_queue.push(hWndShow);

    evt.hWnd = hWndShow;
    evt.hWndOld = hOld;

    TABCHANGE_INFO hdr = { 0 };
    hdr.hdr.code = TCN_SELCHANGING;
    hdr.hdr.hwndFrom = s_info->hEWnd;
    hdr.hdr.idFrom = 0;
    hdr.index = index;
    hdr.indexOld = indexOld;
    hdr.selMode = selMode;

    // ���������ı��¼�, ���ط�0ֵ������
    if (SendMessageW(s_info->hEWnd, WM_NOTIFY, hdr.hdr.idFrom, (LPARAM)&hdr))
        return indexOld;


    LPREGISTER_EVENT_INFO arrEvt[EVENT_MAX_ITEM]; int isReturn = 0; int evtCount = 0;
    // ����ѡ����ı�ǰ�¼�
    if (hOld != hWndShow)
    {
        evtCount = s_info->evt.pfn_Event_callback_before(arrEvt, IRE_TAB_CODE_SELCHANGED, &evt, &isReturn, false);
        if (isReturn) return indexOld;  // ���������, ��������
    }

    if (selMode == TAB_SEL_MODE_CODE)
    {
        // �Ǵ����л��Ӽ�, ��Ҫ�ػ�
        //InvalidItem(tabInfo, 0);
    }


    s_info->tab.indexSel = index;   // ѡ�����Ѿ����޸���, ��Ҫ��ֵ
    // ����ѡ���Ѹı��¼�, �޷���ֵ
    hdr.hdr.code = TCN_SELCHANGE;
    SendMessageW(s_info->hEWnd, WM_NOTIFY, hdr.hdr.idFrom, (LPARAM)&hdr);


    BOOL isOk = false;
    if (hOld && IsWindow(hOld))
        isOk = ShowWindowEx(hOld, SW_HIDE);

    if (hOld != hWndShow)
    {
        // ����ѡ����ı���¼�
        s_info->evt.pfn_Event_callback_after(arrEvt, IRE_TAB_CODE_SELCHANGED, &evt, evtCount, false);
        InvalidateRect(s_info->hEWnd, 0, 0);
    }
    return index;
}

void _tab_updown_click(LPTAB_INFO tabInfo, bool isLeft)
{
    ICustomCodeTab& arr = *s_info->hWnds;
    const int count = arr.size();
    bool isDraw = false;
    bool isRight = _tab_updown_isright_click(tabInfo);
    if (isLeft)
    {
        if (tabInfo->indexFirst == 0)
        {
            if (tabInfo->rState != STATE::BAN)
                tabInfo->rState = STATE::BAN, isDraw = true;

            // ����ұ��ǿ��Ե��״̬, ���Ҳ�������״̬, �Ǿ��޸ĳ�����״̬
            // ����ұ��ǲ��ܵ��״̬, ���Ҳ��ǽ�ֹ״̬, �Ǿ��޸ĳɽ�ֹ״̬
            if (isRight && tabInfo->rState != STATE::NORMAL)
                tabInfo->rState = STATE::NORMAL, isDraw = true;
            else if (!isRight && tabInfo->rState != STATE::BAN)
                tabInfo->rState = STATE::BAN, isDraw = true;
            if (isDraw)
                InvalidateRect(s_info->hEWnd, &s_info->rcCustomTab, 0);
            return;
        }
        // ֻҪ���������, ���ұ�һ����Ҫ��ʾ
        if (tabInfo->rState != STATE::NORMAL)
            tabInfo->rState = STATE::NORMAL, isDraw = true;

        tabInfo->indexFirst--;

        // ������������0, ��߰�ť��Ҫ���Ͻ�ֹ״̬
        if (tabInfo->indexFirst == 0)
            tabInfo->lState = STATE::BAN, isDraw = true;
        if (isDraw)
            InvalidateRect(s_info->hEWnd, &s_info->rcCustomTab, 0);
        return;
    }

    //! �������ұߵĵ�����, ���ҵ�����������
    if (isRight)
    {
        tabInfo->indexFirst++;
        isDraw = true;
    }
    else if (tabInfo->rState != STATE::BAN)
    {
        // �ұ߰�ť��������, ��û�н�ֹ״̬, ������Ҫ����
        tabInfo->rState = STATE::BAN;
        isDraw = true;
    }
    // ֻҪ����λ�ô���0����ߵ�������ť���ǿ��Ե����
    if (tabInfo->indexFirst > 0 && tabInfo->lState != STATE::NORMAL)
        tabInfo->lState = STATE::NORMAL, isDraw = true;

    // �����֮����Ҫ�ٴμ����ұ��Ƿ�ɵ��
    isRight = _tab_updown_isright_click(tabInfo);
    if (isRight && tabInfo->rState != STATE::NORMAL)
        tabInfo->rState = STATE::NORMAL, isDraw = true;
    else if (!isRight && tabInfo->rState != STATE::BAN)
        tabInfo->rState = STATE::BAN, isDraw = true;

    if (isDraw)  
        InvalidateRect(s_info->hEWnd, &s_info->rcCustomTab, 0);
}

// ѡ��еĵ������Ƿ���������ߵĵ��ڰ�ť
bool _tab_updown_isleft_click(LPTAB_INFO tabInfo)
{
    return (tabInfo->indexFirst > 0);
}

// ѡ��еĵ������Ƿ��������ұߵĵ��ڰ�ť
// pIsSub = �Ƿ���Ҫ����-1, �ڵ����ߴ��ʱ���ȡ���ֵ
bool _tab_updown_isright_click(LPTAB_INFO tabInfo, bool* pIsSub)
{
    int nextWidth = 0;      // ������ε����Ч�Ļ�, �����ֵ������Ҫ���ӵĿ��
    int nItemWidth = 0;     // ��������������Ŀ��
    int pos = 0;
    ICustomCodeTab& arr = *s_info->hWnds;
    const int count = arr.size();
    for (int i = 0; i < count; i++)
    {
        CUSTOMTAB_DATA& data = arr[i];
        nItemWidth += s_info->scale(data.width);
        if (i < tabInfo->indexFirst)
            pos += s_info->scale(data.width);
        else if (i == tabInfo->indexFirst)
            nextWidth = s_info->scale(data.width);
    }

    // �׸���ʾ���Ӽе���һ���Ӽп��, �����0��ʾû��ǰһ���Ӽ�
    const int prveWidth = tabInfo->indexFirst > 0 ? s_info->scale(arr[tabInfo->indexFirst - 1].width) : 0;
    const int width = s_info->rcCustomTab.right - s_info->rcCustomTab.left - 40;

    // ��ǰ��ʾ�Ӽе��ܿ�� = �����Ӽп�� - ���λ��
    // ��ǰ��ʾ���Ӽ�һ��ռ�õĿ��
    int showWidth = nItemWidth - pos;
    int surplusWidth = width - showWidth;   // ʣ����
    if (pIsSub) *pIsSub = false;
    if (pIsSub && prveWidth > 0 && surplusWidth > prveWidth)
        *pIsSub = true;
    // ��ǰ��ʾ�Ӽе��ܿ�� > (ѡ��п�� - ���������), �Ǿ���������
    if (showWidth > width)
        return true;

    
    return false;
}
