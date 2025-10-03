#pragma once
#include "CStatic.h"
#include "CListView.h"
#include "CDrawHeader.h"
#include "CDrawListView.h"
#include "CDrawAsmText.h"
#include <assist/assist.h>
#include <CStringBuffer.h>
#include <control/DrawScroll.h>
//
//// �����ʾ�б�
//class CListViewAsmView : public CMyStatic
//{
//#define LISTVIEW_ITEM_HEIGHT 24
//#define ID_LIST     1000
//
//#define LIST_INDEX  0   // ������ʾ��ַ������
//#define LIST_ADDR   1   // ������ʾ��ַ������
//#define LIST_CODE   2   // ������ʾ����ֽ���������
//#define LIST_ASM    3   // ������ʾ���������
//#define LIST_TEXT   4   // ������ʾ�ı�������
//
//#define LIST_MAXCOLUMN   5   // �����������
//#define __query(_num, _n)    (   ( (_num) & (_n) ) == (_n)   )
//
//
//#define ID_ASM_COPY         0x1000  // ����ȫ��ѡ����Ŀ, ������ַ, �ֽ���, ���
//#define ID_ASM_COPY_ADDR    0x1001  // ֻ���Ƶ�ַ
//#define ID_ASM_COPY_CODE    0x1002  // ֻ�����ֽ���
//#define ID_ASM_COPY_CODE_C  0x1003  // ֻ�����ֽ���, C��ʽ
//#define ID_ASM_COPY_CODE_A  0x1004  // ֻ�����ֽ���, ����ʽ
//#define ID_ASM_COPY_CODE_E  0x1005  // ֻ�����ֽ���, �����Ը�ʽ
//#define ID_ASM_COPY_CODE__  0x1006  // ֻ�����ֽ���, __asm ģʽ
//
//#define ID_ASM_COPY_ASM     0x1010  // ֻ���ƻ��
//
//#define ID_ASM_JMP_OFFSET   0x1200  // ��ת��ƫ���ֽڴ�
//
//#define ID_ASM_SELALL       0x1210    // ȫѡ Ctrl+A
//#define ID_ASM_SEARCH       0x1211    // ���� Ctrl+F
//#define ID_ASM_NEXT         0x1212    // ������һ�� N
//#define ID_ASM_PREV         0x1213    // ������һ�� P
//
//#define ID_ASM_JMP          0x1220    // ��ת��ָ����ַ
//
//
//
//
//#define ID_ASM_MASM         0x1100
//#define ID_ASM_GO           0x1101
//#define ID_ASM_NASM         0x1102
//#define ID_ASM_AT           0x1103
//#define ID_ASM_IM           0x1104
//
//
//    struct ASMVIEW_LIST
//    {
//        LPCWSTR     index;  // ��ʾ����, ��0��ʼ
//        LPCWSTR     addr;   // ��ʾ�ĵ�ַ
//        LPCWSTR     code;   // ��ʾ���ֽ���
//        LPCWSTR     szAsm;  // ��ʾ�Ļ��
//        LPCWSTR     text;   // ��ʾ���ı�
//        LPBYTE      pCode;  // �ֽ�������
//        int         nSize;  // �ֽ������ݴ�С
//    };
//    // ���ָ����ɫ��ɫ, ���еı�����ɫ˵���Ķ�ʹ��������ɫ
//    struct ASMVIEW_COLOR
//    {
//        COLORREF gs_back;           // gs�Ĵ���
//        COLORREF ss_text;           // �����μĴ���
//        COLORREF jmp_back;          // ��תָ��
//        COLORREF jmp_text;          // ��תָ��
//        COLORREF jicunqi_text;      // ������������ļĴ���
//        COLORREF kuohaonei_text;    // ������������
//        COLORREF kuohao_back;       // ���ҷ�����
//        COLORREF kuohao_text;       // ���ҷ�����
//        COLORREF push_pop_text;     // push pop ָ�����ɫ
//        COLORREF num_back;          // ��ֵ�����ı�����ɫ
//        COLORREF num_text;          // ��ֵ����
//        COLORREF ret_back;          // ����
//        COLORREF ret_text;          // ����
//        COLORREF call_text;         // call
//        COLORREF call_back;         // call
//    };
//    typedef struct ASMVIEW_DATA
//    {
//        std::vector<ASMVIEW_LIST> arr;  // ��ʾ������, �б�����ʾ��������������
//        std::vector<BYTE> data; // �����Ҫת���ɻ����ֽ�����
//        CStringBufferW  buf;    // �ַ���������
//        CDrawListView drawList; // �ػ��б�Ķ���
//        SIZE_T  nDataSize;      // ���ݳߴ�, ������С
//        BOOLEAN isx64;          // ���ݽ����ĵ�ַ�ǲ���64λ��ַ
//        HWND    hWndParent;     // �����ھ��
//        SIZE_T  offset;         // ��λ���ֽ�ƫ��, ת���ɻ������ת�������ƫ���������һ��
//        HWND    hWnd;           // ���鿴�����ھ��
//        HWND    hList;          // ��ʾ�����б�
//        int     offset_index;   // ��λ���ֽ�ƫ�����б��е�����
//        HFONT   hFont;          // ʹ�õ�����
//        LPINT   pShowMode;      // �﷨ģʽ, 0=MASM, 0x100=GoAsm, 0x200=Nasm, 0x400=AT, 0x800=IntrinsicMem
//        WNDPROC oldProc_list;   // �б�ԭʼ���ڹ���
//        int     nDragStart;     // ��ק����Ŀ, -1��ʾû�б���ק
//        int     nDragEnd;       // ��ק�Ľ�����Ŀ, �п��ܱȿ�ʼ����ĿС
//        int     cxClient;       // �б�Ŀ��
//        int     cyClient;       // �б�ĸ߶�
//
//        ASMVIEW_COLOR color;
//
//        ULONG64 address;        // �����ʾ�ĵ�ַ, �����ת/call���ǻ��������ַ��ʾ��
//
//        int     addref;
//
//        CSearchBox  search;
//        CJumpBox    jmp;
//        bool        isClr;
//        THEMECOLOR_STRUCT clr;
//        IDrawScroll* iScroll;
//        ASMVIEW_DATA() : hWndParent(0), offset(0), hWnd(0), hList(0), offset_index(-1), hFont(0),
//            pShowMode(0), address(0), oldProc_list(0), nDragStart(-1), nDragEnd(-1), addref(2),
//            cxClient(0), cyClient(0), nDataSize(0), isx64(0), clr{ 0 }, color{ 0 }, isClr(false), iScroll(0)
//        {
//            search.SetSearchMode(SEARCH_MODE_NORMAL);
//            search.SetCaption(L"����");
//            search.SetTips(L"���������ַ/�ֽ���/���ָ���������");
//
//            jmp.SetCaption(L"��ת��...");
//            // �����Ҫ�����������һ����ʼֵ, ���Ǹо�û̫��ı�Ҫ
//            //jmp.SetMode((JUMPBOX_MODE)nJmpMode);
//            //jmp.SetSelHex(mem->isSelHex);
//            const LPCWSTR pszRadio1 = L"��һ������(&B)";
//            const LPCWSTR pszRadio2 = L"����λ��";
//            const LPCWSTR pszRadio3 = L"����λ������";
//            const LPCWSTR pszRadio4 = L"���һ������(&E)";
//            jmp.SetRadioText(JUMPBOX_MODE_START, pszRadio1);
//            jmp.SetRadioText(JUMPBOX_MODE_NOW, pszRadio2);
//            jmp.SetRadioText(JUMPBOX_MODE_NOW_PREV, pszRadio3);
//            jmp.SetRadioText(JUMPBOX_MODE_END, pszRadio4);
//        }
//        ~ASMVIEW_DATA()
//        {
//
//        }
//
//        inline void Release()
//        {
//            if (iScroll)
//                delete iScroll;
//            if (--addref <= 0)
//                delete this;
//            //drawList.detach();
//        }
//    }*PASMVIEW_DATA;
//
//
//    PTHEMECOLOR_STRUCT m_pClr;
//    bool m_isRef;
//    CDrawHeader drawHeader;
//    int m_indexHot;
//    int m_indexDown;
//    bool m_isHot;
//
//    ASMVIEW_DATA m_data;
//
//protected:
//    virtual inline LPCWSTR SuperClassName() { return L"AsmListView"; }
//
//public:
//    CListViewAsmView(): m_pClr(0), m_isRef(false), m_indexHot(-1), m_indexDown(-1), m_isHot(false)
//    {
//
//    }
//    ~CListViewAsmView()
//    {
//        if (!m_isRef && m_pClr)
//            delete m_pClr;
//    }
//
//public:
//    virtual inline LPCWSTR className() { return CLASS_LISTVIEW; }
//    static inline LPCWSTR propName() { return L"{AA9A5503-7471-4D24-A31B-F474F5188901}"; }
//
//public:
//    HWND create(HWND hWndParent, int x, int y, int cx, int cy, LONG_PTR ID)
//    {
//        HWND hWnd = CWndBase::create(0, 0, WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE | WS_CHILD, x, y, cx, cy, hWndParent, ID);
//        if (!hWnd)
//            return 0;
//
//        const DWORD style = WS_TABSTOP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE | WS_CHILD
//            | LVS_REPORT | LVS_OWNERDATA | LVS_SHOWSELALWAYS;
//        const DWORD styleEx = WS_EX_CLIENTEDGE;
//        const DWORD styleList = LVS_EX_DOUBLEBUFFER | LVS_EX_SUBITEMIMAGES | LVS_EX_FULLROWSELECT// | LVS_EX_GRIDLINES
//            | LVS_EX_FLATSB | LVS_EX_INFOTIP;
//
//        m_data.hList = CreateWindowExW(styleEx, WC_LISTVIEWW, 0, style, 0, 0, 0, 0, hWnd, (HMENU)(LONG_PTR)ID_LIST, 0, 0);
//
//        ::SetPropW(m_data.hList, propName(), this);
//        ListView_SetExtendedListViewStyle(m_data.hList, styleList);
//
//
//        HWND hHeader = ListView_GetHeader(m_data.hList);
//        drawHeader.bind(hHeader, m_pClr, true);
//
//        static WNDPROC list_proc = [](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) -> LRESULT
//        {
//            CListViewAsmView* pObj = (CListViewAsmView*)::GetPropW(hWnd, propName());
//            if (!pObj)
//                return DefWindowProcW(hWnd, message, wParam, lParam);
//            WNDPROC oldProc = pObj->m_data.oldProc_list;
//            if (message == WM_DESTROY)
//            {
//                ::RemovePropW(hWnd, propName());
//                SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)oldProc);
//                return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
//            }
//            return pObj->WndProc_ListView(oldProc, hWnd, message, wParam, lParam);
//        };
//        m_data.oldProc_list = (WNDPROC)SetWindowLongPtrW(m_data.hList, GWLP_WNDPROC, (LONG_PTR)list_proc);
//
//        LVCOLUMNW data = { 0 };
//        auto pfn_insert = [&](LPCWSTR name)
//        {
//            data.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
//            data.fmt = 0;
//            data.cx = 0;
//            data.pszText = (LPWSTR)name;
//            ListView_InsertColumn(m_data.hList, data.iSubItem++, &data);
//        };
//
//        pfn_insert(L"����");
//        pfn_insert(L"��ַ");
//        pfn_insert(L"�ֽ���");
//        pfn_insert(L"���");
//        pfn_insert(L"��ע");
//
//        AsmView_switch_listview_color(&m_data);
//        if (m_data.hFont)
//            SendMessageW(m_data.hList, WM_SETFONT, (WPARAM)m_data.hFont, 0);
//        AsmView_SetListItemHeight(m_data.hList);
//        AsmView_Show(&m_data);
//        return m_hWnd;
//    }
//    // ���������ɫ, û�����þ�ʹ��Ĭ����ɫ, ���ú�ʹ�����õ���ɫ
//    // pClr = ��ɫ�ṹ
//    // isRef = ����1�Ƿ�һֱ��Ч, ����Ϊ���ʱ���ڲ����������ɫ�ṹ, ��һֱʹ�ò���1, ����Ϊ��ʱ��ȷ������1ָ��Ĵ��
//    bool SetControlColor(PTHEMECOLOR_STRUCT pClr, bool isRef)
//    {
//        if (!pClr)
//            return false;
//        m_isRef = isRef;
//
//        if (isRef)
//        {
//            m_pClr = pClr;
//        }
//        else
//        {
//            if (!m_pClr)
//                m_pClr = new THEMECOLOR_STRUCT;
//            memcpy(m_pClr, pClr, sizeof(THEMECOLOR_STRUCT));
//        }
//        drawHeader.bind(0, m_pClr, true);
//        return m_pClr != 0;
//    }
//    // ��������ɫ
//    bool ClearControlColor(PCTHEMECOLOR_STRUCT pClr)
//    {
//        if (m_pClr)
//            delete m_pClr;
//        m_pClr = 0;
//        return true;
//    }
//
//private:
//    virtual LRESULT WndProc(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//    {
//        switch (message)
//        {
//        case WM_PAINT:
//        {
//            RECT rc;
//            PAINTSTRUCT ps;
//            BeginPaint(hWnd, &ps);
//            GetClientRect(hWnd, &rc);
//            COLORREF clrBack = RGB(255, 255, 255);
//            if (m_pClr)
//                clrBack = m_pClr->crBack;
//            HBRUSH hbr = CThemeColor::GetBrush(clrBack);
//            FillRect(ps.hdc, &rc, hbr);
//            EndPaint(hWnd, &ps);
//            break;
//        }
//        case WM_NOTIFY:
//        {
//            LRESULT ret = 0;
//            if (OnNotify(hWnd, message, wParam, lParam, &ret))
//                return ret;
//            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
//        }
//        default:
//            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
//        }
//        return 0;
//    }
//
//    virtual LRESULT WndProc_ListView(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//    {
//        PASMVIEW_DATA mem = &m_data;
//        if (mem->iScroll)
//        {
//            LRESULT ret = 0;
//            if (mem->iScroll->OnWndProc(oldProc, hWnd, message, wParam, lParam, &ret))
//                return ret;
//        }
//
//        switch (message)
//        {
//        case WM_MOUSEMOVE:
//        {
//            if (mem->nDragStart == -1)
//                break;
//            const int x = (short)LOWORD(lParam);
//            const int y = (short)HIWORD(lParam);
//
//            if (y < 0)
//            {
//                // ���Ϲ���, �����������, ��Ҫ�������Ϲ���ѡ��λ��
//                SendMessageW(hWnd, WM_VSCROLL, SB_LINEUP, 0);
//                break;
//            }
//
//            if (mem->cyClient == 0)
//            {
//                RECT rc;
//                GetWindowRect(hWnd, &rc);
//                mem->cyClient = rc.bottom - rc.top;
//                mem->cxClient = rc.right - rc.left;
//            }
//            if (y > mem->cyClient)
//            {
//                // ���¹���, ��������ױ�, ��Ҫ�������¹���ѡ��λ��
//                //wstr::dbg(L"����ƶ�, hWnd = %p,  x,y = %d, %d\n", hWnd, x, y);
//                SendMessageW(hWnd, WM_VSCROLL, SB_LINEDOWN, 0);
//                break;
//            }
//
//
//            break;
//        }
//        case WM_WINDOWPOSCHANGED:
//        {
//            LPWINDOWPOS pos = (LPWINDOWPOS)lParam;
//            if (pos->flags & SWP_NOSIZE)
//                break;  // �в��ı�ߴ�, �ǾͲ���¼
//            mem->cxClient = pos->cx;
//            mem->cyClient = pos->cy;
//            break;
//        }
//        case WM_DESTROY:
//        {
//            SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0);
//            SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)oldProc);
//            mem->Release();
//            break;
//        }
//        case WM_LBUTTONDOWN:
//        {
//            const int x = LOWORD(lParam);
//            const int y = HIWORD(lParam);
//
//            LVHITTESTINFO info = { 0 };
//            info.pt.x = x;
//            info.pt.y = y;
//            int index = ListView_HitTest(mem->hList, &info);
//            if (index == -1)
//                break;
//
//            UINT state = ListView_GetItemState(mem->hList, index, LVIS_SELECTED | LVIS_FOCUSED);
//
//            if (__query(state, LVIS_SELECTED) && !__query(state, LVIS_FOCUSED))
//            {
//                // ��ǰѡ�е���Ŀ�Ǳ�ѡ�����Ŀ, ���ǲ��ǽ�����Ŀ, ��Ҫȡ��ѡ��, Ȼ��ִ����갴���¼�
//                ListView_SetItemState(mem->hList, index, 0, LVIS_SELECTED);
//            }
//
//            //LRESULT r = CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
//            //SendMessageW(hWnd, WM_LBUTTONUP, wParam, lParam);
//            //return r;
//            break;
//        }
//        case WM_LBUTTONUP:
//        {
//            if (GetCapture() == hWnd)
//                ReleaseCapture();
//            break;
//        }
//        case WM_CAPTURECHANGED:
//        {
//            mem->nDragStart = -1;
//            mem->nDragEnd = -1;
//            break;
//        }
//        case WM_KEYDOWN:
//            return HexView_OnKeyDown(mem, hWnd, wParam, lParam);
//        case WM_PAINT:
//        {
//            OnPaint(hWnd);
//            break;
//        }
//        default:
//            break;
//        }
//        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
//    }
//
//private:
//    struct LV_DRAW
//    {
//        NMLVCUSTOMDRAW nmCustomDraw;
//        std::vector<int> arr_index;
//        std::vector<int> arr_widths;
//        int right;
//        LVITEMW item;
//        HWND hHeader;
//        HDC hdc;
//        RECT rcItem;
//        int cxClient;
//        int cyClient;
//        COLORREF clrBack;
//        COLORREF clrText;
//        COLORREF clrLine;
//        COLORREF clrHot ;
//        COLORREF clrDown;
//        DWORD styleList;
//        HFONT hFont;
//        DWORD dwCustom;
//        LONG_PTR id;
//        HWND hParent;
//        HIMAGELIST hImageList;
//        int cxIcon;
//        int cyIcon;
//        LV_DRAW()
//        {
//            const int offset = offsetof(LV_DRAW, nmCustomDraw);
//            const int cbSize = sizeof(LV_DRAW) - offset;
//            memset(&nmCustomDraw, 0, cbSize);
//        }
//    };
//    void OnPaint(HWND hWnd)
//    {
//        LV_DRAW lv_draw;
//        RECT rc;
//        PAINTSTRUCT ps;
//        BeginPaint(hWnd, &ps);
//        GetClientRect(hWnd, &rc);
//        lv_draw.cxClient = rc.right - rc.left;
//        lv_draw.cyClient = rc.bottom - rc.top;
//        LPMEMDCBITMAP md = md_create(lv_draw.cxClient, lv_draw.cyClient);
//        HDC hdc = md->hdc;
//
//        lv_draw.clrBack = RGB(255, 255, 255);
//        lv_draw.clrText = RGB(0, 0, 0);
//        lv_draw.clrLine = RGB(240, 240, 240);
//        lv_draw.clrHot = RGB(217, 235, 249);
//        lv_draw.clrDown = RGB(188, 220, 244);
//
//        if (m_pClr)
//        {
//            lv_draw.clrBack = m_pClr->crBack, lv_draw.clrText = m_pClr->crText, lv_draw.clrLine = m_pClr->crBorder;
//            lv_draw.clrHot = m_pClr->crHot, lv_draw.clrDown = m_pClr->crSelected;
//        }
//        FillRect(hdc, &rc, CThemeColor::GetBrush(lv_draw.clrBack));
//        COLORREF clrTextOld = ::SetTextColor(hdc, lv_draw.clrText);
//        SetBkMode(hdc, TRANSPARENT);
//
//        lv_draw.hFont = (HFONT)SendMessageW(hWnd, WM_GETFONT, 0, 0);
//        if (!lv_draw.hFont)
//            lv_draw.hFont = CWndBase::DefaultFont();
//        SelectObject(hdc, lv_draw.hFont);
//        HPEN hPenLine = CThemeColor::GetPen(PS_SOLID, 1, lv_draw.clrLine);
//        SelectObject(hdc, hPenLine);
//
//        lv_draw.hdc = hdc;
//
//        NMCUSTOMDRAW& nmcd = lv_draw.nmCustomDraw.nmcd;
//        nmcd.hdr.hwndFrom = m_hWnd;
//        nmcd.hdr.idFrom = GetDlgCtrlID(nmcd.hdr.hwndFrom);
//        lv_draw.hParent = GetParent(nmcd.hdr.hwndFrom);
//
//        lv_draw.dwCustom = CICustomDrawNotify(lv_draw, CDDS_PREPAINT, &nmcd);
//        // �����Զ���滭��������
//        if (!(lv_draw.dwCustom & CDRF_SKIPDEFAULT))
//        {
//            lv_draw.styleList = ListView_GetExtendedListViewStyle(m_hWnd);
//            const int nTopIndex = ListView_GetTopIndex(hWnd);
//            const int nItemCount = ListView_GetItemCount(hWnd);
//
//            RECT& rcItem = lv_draw.rcItem;
//            LVITEMW& item = lv_draw.item;
//            item.mask = LVIF_INDENT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
//            item.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
//            item.iSubItem = 0;
//
//            DWORD style = (DWORD)GetWindowLongPtrW(hWnd, GWL_STYLE);
//            bool isIcon = (style & LVS_LIST) == 0;  // ֻҪΪ0, �ͱ�ʾ�Ǵ�ͼ��
//            lv_draw.hImageList = ListView_GetImageList(hWnd, isIcon ? LVSIL_NORMAL : LVSIL_SMALL);
//            if (lv_draw.hImageList)
//                ImageList_GetIconSize(lv_draw.hImageList, &lv_draw.cxIcon, &lv_draw.cyIcon);
//
//            lv_draw.hHeader = ListView_GetHeader(hWnd);
//            int nColumnCount = (int)SendMessageW(lv_draw.hHeader, HDM_GETITEMCOUNT, 0, 0);
//            if (nColumnCount <= 0)
//                nColumnCount = 1;
//
//
//            int first_top = 0;
//            int itemHeight = 0;
//
//            lv_draw.right = -GetScrollPos(hWnd, SB_HORZ);
//            std::vector<int>& arr_index = lv_draw.arr_index; arr_index.resize(nColumnCount);
//            std::vector<int>& arr_widths = lv_draw.arr_widths; arr_widths.resize(nColumnCount);
//            ListView_GetColumnOrderArray(hWnd, nColumnCount, &arr_index[0]);
//            for (int& iSubItem : arr_index)
//                arr_widths[iSubItem] = ListView_GetColumnWidth(hWnd, iSubItem);
//
//            for (int i = nTopIndex; i < nItemCount; i++)
//            {
//                item.iItem = i;
//                ListView_GetItemRect(hWnd, i, &rcItem, LVIR_BOUNDS);
//                if (i == nTopIndex)
//                    first_top = rcItem.top, itemHeight = rcItem.bottom - rcItem.top;
//                if (rcItem.top > lv_draw.cyClient)
//                    break;
//                if (!CDrawHeader::Rect_isOverlap(rcItem, ps.rcPaint))    // �����ػ�����ͽ�����һ��ѭ��
//                    continue;
//                ListView_GetItem(hWnd, &item);
//                item.iItem = i;
//
//                DrawItem(lv_draw);
//
//            }
//
//            // �л滭����߲Ż滭
//            if ((lv_draw.styleList & LVS_EX_GRIDLINES) == LVS_EX_GRIDLINES)
//            {
//                for (int& iWidth : arr_widths)
//                {
//                    lv_draw.right += iWidth;
//
//                    if (lv_draw.right >= 0)
//                    {
//                        MoveToEx(hdc, lv_draw.right, rc.top, 0);
//                        LineTo(hdc, lv_draw.right, rc.bottom);
//                    }
//                }
//                //TODO ��Ŀ�߶�=0��ʱ�����û����Ŀ, ��Ҫ��ȡ����߶�, ������ô��ȡ
//                if (itemHeight == 0)
//                {
//                    //itemHeight = LOWORD(data->propNew.nItemHeight);
//                    //if (itemHeight == 0)
//                    //    itemHeight = HIWORD(data->propNew.nItemHeight);
//
//                    // Ϊ0��ʾû�б���, û�б���Ļ�rcItem�Ķ��߾���0, ������Ҫ�Ѷ��ߵ�������ͷ�ĵױ�
//                    RECT rcHead;
//                    GetWindowRect(lv_draw.hHeader, &rcHead);
//                    rcItem.top = rcHead.bottom - rcHead.top;
//                    rcItem.bottom = rcItem.top + itemHeight;
//                }
//
//                rcItem.left = 0;
//                rcItem.right = lv_draw.right;
//                rcItem.top = first_top;
//                rcItem.bottom = rcItem.top + itemHeight;
//
//                while (itemHeight)
//                {
//                    const int top = rcItem.bottom - 1;
//                    if (top > lv_draw.cyClient)
//                        break;  // �������ڳߴ�Ͳ������滭��
//
//                    MoveToEx(hdc, rcItem.left, top, 0);
//                    LineTo(hdc, rcItem.right, top);
//
//                    rcItem.top += itemHeight;
//                    rcItem.bottom += itemHeight;
//                }
//
//            }
//            // notify parent afterwards if they want us to
//            if (lv_draw.dwCustom & CDRF_NOTIFYPOSTPAINT)
//            {
//                CICustomDrawNotify(lv_draw, CDDS_POSTPAINT, &nmcd);
//            }
//        }
//        BitBlt(ps.hdc, 0, 0, lv_draw.cxClient, lv_draw.cyClient, hdc, 0, 0, SRCCOPY);
//        EndPaint(hWnd, &ps);
//        md_destroy(md);
//    }
//
//    inline void DrawItem(LV_DRAW& lv_draw)
//    {
//        HBRUSH hbrBack = 0;
//        NMLVCUSTOMDRAW& nmCustomDraw = lv_draw.nmCustomDraw;
//        NMLVDISPINFOW dispInfo = { 0 };
//
//        UINT uItemState = 0;
//        if ((lv_draw.item.state & LVIS_FOCUSED) == LVIS_FOCUSED)
//        {
//            hbrBack = CThemeColor::GetBrush(lv_draw.clrDown);
//            uItemState |= CDIS_FOCUS;
//        }
//        else if (lv_draw.item.iItem == m_indexHot)
//        {
//            hbrBack = CThemeColor::GetBrush(lv_draw.clrHot);
//            uItemState |= CDIS_HOT;
//        }
//        else if ((lv_draw.item.state & LVIS_SELECTED) == LVIS_SELECTED)
//        {
//            hbrBack = CThemeColor::GetBrush(lv_draw.clrDown);
//            uItemState |= CDIS_SELECTED;
//        }
//        if (hbrBack)
//            FillRect(lv_draw.hdc, &lv_draw.rcItem, hbrBack);
//
//        if ((lv_draw.item.state & LVIS_SELECTED) == LVIS_SELECTED)
//            uItemState |= CDIS_SELECTED;
//
//
//        //rcf.left += 5.f;
//        int fmt = DT_VCENTER | DT_SINGLELINE | DT_LEFT | DT_NOPREFIX | DT_END_ELLIPSIS;
//
//
//
//
//        HDC hdc = lv_draw.hdc;
//        COLORREF clrTextOld = ::GetTextColor(hdc);
//        HWND hParent = GetParent(nmCustomDraw.nmcd.hdr.hwndFrom);
//
//        lv_draw.dwCustom = CICustomDrawNotify(lv_draw, CDDS_ITEMPREPAINT, &nmCustomDraw.nmcd);
//        //DWORD ret_state = (DWORD)SendMessageW(hParent, WM_NOTIFY, nmCustomDraw.nmcd.hdr.idFrom, (LPARAM)&nmCustomDraw);
//        if (!(lv_draw.dwCustom & CDRF_SKIPDEFAULT))
//        {
//            RECT& rcItem = lv_draw.rcItem;
//
//            for (int nColIndex : lv_draw.arr_index)
//            {
//                DWORD dwCustom = 0;
//
//                if (nColIndex == 0)
//                {
//                    dispInfo.item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_INDENT;
//                }
//                else
//                {
//                    // Next time through, we only want text for subitems...
//                    dispInfo.item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
//                }
//
//                dispInfo.hdr = nmCustomDraw.nmcd.hdr;
//                dispInfo.item.iItem = lv_draw.item.iItem;
//                dispInfo.item.iImage = -1;
//                dispInfo.item.iSubItem = nColIndex;
//                dispInfo.item.pszText = (LPWSTR)L"";
//                dispInfo.item.cchTextMax = 0;
//                CCSendNotify(&lv_draw, LVN_GETDISPINFOW, &dispInfo.hdr);
//
//                
//                RECT rcCol; // �о���, ���Ի�ȡ��ߺ��ұ�
//                SendMessage(lv_draw.hHeader, HDM_GETITEMRECT, nColIndex, (LPARAM)&rcCol);
//
//                nmCustomDraw.nmcd.hdc = hdc;
//                nmCustomDraw.uAlign = fmt;
//                nmCustomDraw.clrText = lv_draw.clrText;
//                nmCustomDraw.clrTextBk = lv_draw.clrBack;
//                nmCustomDraw.iSubItem = nColIndex;
//
//                nmCustomDraw.nmcd.dwItemSpec = dispInfo.item.iItem;
//                nmCustomDraw.nmcd.lItemlParam = dispInfo.item.lParam;
//                nmCustomDraw.nmcd.rc.left = rcCol.left;
//                nmCustomDraw.nmcd.rc.top = rcItem.top;
//                nmCustomDraw.nmcd.rc.right = rcCol.right;
//                nmCustomDraw.nmcd.rc.bottom = rcItem.bottom;
//                if (nmCustomDraw.nmcd.rc.right <= 0)
//                    continue;   // �ұ�Ϊ0, �Ǿ��Ǻ��������������, �����û��ʾ����Ļ��, ���滭
//
//                RECT& rcText = nmCustomDraw.rcText;
//                rcText = nmCustomDraw.nmcd.rc;
//                rcText.left += 5;
//                if (dispInfo.item.iImage != -1)
//                    rcText.left += lv_draw.cxIcon;
//
//                if (lv_draw.dwCustom & CDRF_NOTIFYSUBITEMDRAW)    // ��֪ͨ�滭����
//                {
//                    // ����滭ǰ
//                    nmCustomDraw.nmcd.uItemState = uItemState;
//                    dwCustom = CICustomDrawNotify(lv_draw, CDDS_SUBITEM | CDDS_ITEMPREPAINT, &nmCustomDraw.nmcd);
//                    nmCustomDraw.nmcd.uItemState = uItemState;
//
//                    if (dwCustom & CDRF_SKIPDEFAULT)
//                        continue;   // �û����л���, ���ﲻ�������»���
//                }
//
//
//
//                ::SetTextColor(hdc, nmCustomDraw.clrText);
//                DrawTextW(hdc, dispInfo.item.pszText, -1, &rcText, fmt);
//                ::SetTextColor(hdc, clrTextOld);
//
//                if (dwCustom & CDRF_NOTIFYPOSTPAINT)
//                {
//                    // ����滭����
//                    CICustomDrawNotify(lv_draw, CDDS_SUBITEM | CDDS_ITEMPOSTPAINT, &nmCustomDraw.nmcd);
//                }
//
//            }
//            
//            if (lv_draw.dwCustom & CDRF_NOTIFYPOSTPAINT)
//            {
//                lv_draw.nmCustomDraw.iSubItem = 0;
//                CICustomDrawNotify(lv_draw, CDDS_ITEMPOSTPAINT, &lv_draw.nmCustomDraw.nmcd);
//            }
//
//            if (lv_draw.dwCustom & CDRF_NEWFONT)
//            {
//                SelectObject(hdc, lv_draw.hFont);
//            }
//
//        }
//
//        // DrawItem ��������
//    }
//
//    LRESULT SendGetDispInfo(NMLVDISPINFOW& info) const
//    {
//        return CCSendNotify(0, LVN_GETDISPINFOW, (LPNMHDR)&info);
//    }
//
//    LRESULT CCSendNotify(LV_DRAW* lv_draw, int code, LPNMHDR hdr) const
//    {
//        hdr->code = code;
//        hdr->hwndFrom = m_hWnd;
//        HWND hParent = 0;
//        if (lv_draw)
//        {
//            hdr->idFrom = lv_draw->id;
//            hParent = lv_draw->hParent;
//        }
//        else
//        {
//            hdr->idFrom = GetDlgCtrlID(hdr->hwndFrom);
//            hParent = GetParent(hdr->hwndFrom);
//        }
//        return SendMessageW(hParent, WM_NOTIFY, hdr->idFrom, (LPARAM)hdr);
//    }
//
//    DWORD CICustomDrawNotify(LV_DRAW& lv_draw, DWORD dwStage, LPNMCUSTOMDRAW lpnmcd) const
//    {
//        DWORD dwRet = CDRF_DODEFAULT;
//
//
//        // bail if...
//
//
//        // this is an item notification, but an item notification wasn't asked for
//        if ((dwStage & CDDS_ITEM) && !(lv_draw.dwCustom & CDRF_NOTIFYITEMDRAW)) {
//            return dwRet;
//        }
//
//        lpnmcd->dwDrawStage = dwStage;
//        dwRet = (DWORD)CCSendNotify(&lv_draw, NM_CUSTOMDRAW, &lpnmcd->hdr);
//
//        return dwRet;
//    }
//
//private:
//    bool OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
//    {
//        LPNMHDR hdr = (LPNMHDR)lParam;
//        PASMVIEW_DATA mem = &m_data;
//        switch (hdr->code)
//        {
//        case LVN_GETDISPINFOW:
//        {
//            LPNMLVDISPINFOW info = (LPNMLVDISPINFOW)lParam;
//            const int index = info->item.iItem;
//            if (index < 0 || index >= (int)mem->arr.size())
//                break;
//            ASMVIEW_LIST& item = mem->arr[index];
//            switch (info->item.iSubItem)
//            {
//            case LIST_INDEX:
//                info->item.pszText = (LPWSTR)item.index;
//                break;
//            case LIST_ADDR:
//                info->item.pszText = (LPWSTR)item.addr;
//                break;
//            case LIST_CODE:
//                info->item.pszText = (LPWSTR)item.code;
//                break;
//            case LIST_ASM:
//                //info->item.pszText = (LPWSTR)item.szAsm;
//                break;
//            case LIST_TEXT:
//                info->item.pszText = (LPWSTR)item.text;
//                break;
//            default:
//                break;
//            }
//            break;
//        }
//        case NM_RCLICK:
//        {
//            LPNMITEMACTIVATE item = (LPNMITEMACTIVATE)lParam;
//            if (item->hdr.hwndFrom != mem->hList)
//                return false;
//
//            const int index = item->iItem;
//            if (index < 0 || index >= (int)mem->arr.size())
//                break;
//
//            AsmView_PopupMenu(mem);
//            break;
//        }
//        case LVN_BEGINDRAG:
//        {
//            LPNMLISTVIEW item = (LPNMLISTVIEW)lParam;
//            mem->drawList.OnBeginDrag(hdr->hwndFrom, item->iItem, mem->nDragStart, mem->nDragEnd);
//
//            break;
//        }
//        case NM_CLICK:
//        {
//            LPNMITEMACTIVATE item = (LPNMITEMACTIVATE)lParam;
//
//            break;
//        }
//        case LVN_HOTTRACK:
//        {
//            LPNMLISTVIEW item = (LPNMLISTVIEW)lParam;
//            const int index = item->iItem;
//            mem->drawList.OnDragIng(hdr->hwndFrom, item->iItem, mem->nDragStart, mem->nDragEnd);
//            break;
//        }
//        default:
//            return false;
//        }
//        return true;
//    }
//
//
//
//
//    inline void AsmView_Copy(PASMVIEW_DATA mem, int id)
//    {
//        int len = ListView_GetItemCount(mem->hList);
//        if (len != (int)mem->arr.size())
//        {
//            MessageBoxW(mem->hWnd, L"�б����ݴ���, �������ⲿ�޸��б�����", L"����", 0);
//            return;
//        }
//
//        wchar_t buf[260];
//        std::wstring text;
//        text.reserve(0x1000ll * 0x1000ll);
//
//        struct _TEST_STRUCT
//        {
//            int index;
//            int size;
//        };
//        std::vector<_TEST_STRUCT> arr_sel;
//        arr_sel.reserve(len);
//        int maxLen = 0;
//        for (int i = 0; i < len; i++)
//        {
//            if (ListView_GetItemState(mem->hList, i, LVIS_SELECTED))
//            {
//                ASMVIEW_LIST& item = mem->arr[i];
//                _TEST_STRUCT push;
//                push.index = i;
//                push.size = (int)wcslen(item.code);
//
//                if (push.size > maxLen)
//                    maxLen = push.size;
//                arr_sel.push_back(push);
//            }
//        }
//
//        auto pfn_copy = [&](int index)
//        {
//            ASMVIEW_LIST& item = mem->arr[index];
//            switch (id)
//            {
//            case ID_ASM_COPY:           // ����ȫ��ѡ����Ŀ, ������ַ, �ֽ���, ���
//            {
//                const size_t start = text.size();
//                text.append(item.addr).append(L"  ").append(item.code);
//                int strlen = (int)(text.size() - start - 20);
//                for (int n = strlen; n < maxLen; n++)
//                    text.push_back(L' ');
//                text.append(L"  ").append(item.szAsm).append(L"\r\n");
//                break;
//            }
//            case ID_ASM_COPY_ADDR:      // ֻ���Ƶ�ַ
//                text.append(item.addr).append(L"\r\n");
//                break;
//            case ID_ASM_COPY_CODE:      // ֻ�����ֽ���
//                text.append(item.code).append(L"\r\n");
//                break;
//            case ID_ASM_COPY_CODE_C:    // ֻ�����ֽ���, C��ʽ
//            {
//                int offset = 0;
//                text.append(L"    ");
//                for (int i = 0; i < item.nSize; i++)
//                {
//                    const int _BufferCount = 100 - offset;
//                    offset += swprintf_s(&buf[offset], _BufferCount, L"0x%02X, ", item.pCode[i]);
//                    if ((i + 1) % 20 == 0)
//                    {
//                        buf[offset++] = L'\r';
//                        buf[offset++] = L'\n';
//                        for (int n = 0; n < 4; n++)
//                            buf[offset++] = L' ';
//                    }
//                }
//                text.append(buf).append(L"\r\n");
//                //text.append(L"{\r\n    ").append(buf).append(L"};\r\n");
//                break;
//            }
//            case ID_ASM_COPY_CODE_A:    // ֻ�����ֽ���, ����ʽ
//            {
//                int offset = 0;
//                for (int i = 0; i < item.nSize; i++)
//                {
//                    const int _BufferCount = 100 - offset;
//                    offset += swprintf_s(&buf[offset], _BufferCount, L"0%02Xh, ", item.pCode[i]);
//                    if ((i + 1) % 20 == 0)
//                    {
//                        buf[offset++] = L'\r';
//                        buf[offset++] = L'\n';
//                        for (int n = 0; n < 4; n++)
//                            buf[offset++] = L' ';
//                    }
//                }
//                text.append(buf).append(L"\r\n");
//                //text.append(L"{\r\n    ").append(buf).append(L"};\r\n");
//                break;
//            }
//            case ID_ASM_COPY_CODE__:    // __asm _emit
//            {
//                for (int i = 0; i < item.nSize; i++)
//                {
//                    swprintf_s(buf, 20, L"0x%02X", item.pCode[i]);
//                    text.append(L"__asm _emit ").append(buf).append(L"\r\n");
//                }
//                break;
//            }
//            case ID_ASM_COPY_CODE_E:    // ֻ�����ֽ���, �����Ը�ʽ
//            {
//                int offset = 0;
//                for (int i = 0; i < item.nSize; i++)
//                {
//                    const int _BufferCount = 100 - offset;
//                    offset += swprintf_s(&buf[offset], _BufferCount, L"%u, ", item.pCode[i]);
//                }
//                text.append(buf);
//                //text.append(L"{ ").append(buf).append(L" }\r\n");
//                break;
//            }
//            case ID_ASM_COPY_ASM:       // ֻ���ƻ��
//                text.append(item.szAsm).append(L"\r\n");
//                break;
//            default:
//                break;
//            }
//        };
//
//        switch (id)
//        {
//        case ID_ASM_COPY:           // ����ȫ��ѡ����Ŀ, ������ַ, �ֽ���, ���
//        case ID_ASM_COPY_CODE:      // ֻ�����ֽ���
//            break;
//        case ID_ASM_COPY_CODE_C:    // ֻ�����ֽ���, C��ʽ
//            text.append(L"{\r\n");
//            break;
//        case ID_ASM_COPY_CODE_A:    // ֻ�����ֽ���, ����ʽ
//            break;
//        case ID_ASM_COPY_CODE__:    // __asm _emit
//            break;
//        case ID_ASM_COPY_CODE_E:    // ֻ�����ֽ���, �����Ը�ʽ
//            text.append(L"{");
//            break;
//        case ID_ASM_COPY_ASM:       // ֻ���ƻ��
//            break;
//        default:
//            break;
//        }
//
//        for (_TEST_STRUCT& i : arr_sel)
//        {
//            pfn_copy(i.index);
//
//        }
//
//        switch (id)
//        {
//        case ID_ASM_COPY:           // ����ȫ��ѡ����Ŀ, ������ַ, �ֽ���, ���
//        case ID_ASM_COPY_CODE:      // ֻ�����ֽ���
//            break;
//        case ID_ASM_COPY_CODE_C:    // ֻ�����ֽ���, C��ʽ
//            text.append(L"}\r\n");
//            break;
//        case ID_ASM_COPY_CODE_A:    // ֻ�����ֽ���, ����ʽ
//            break;
//        case ID_ASM_COPY_CODE__:    // __asm _emit
//            break;
//        case ID_ASM_COPY_CODE_E:    // ֻ�����ֽ���, �����Ը�ʽ
//            text.pop_back();
//            text.pop_back();
//            text.append(L"}\r\n");
//            break;
//        case ID_ASM_COPY_ASM:       // ֻ���ƻ��
//            break;
//        default:
//            break;
//        }
//
//        if (text.empty())
//        {
//            MessageBoxW(mem->hWnd, L"���Ƴ���, û�ܻ�ȡ�κ�����", L"���ƴ���", 0);
//            return;
//        }
//        SetClipboard(text.c_str(), (int)text.size());
//    }
//    inline void AsmView_SelAll(PASMVIEW_DATA mem)
//    {
//        int len = ListView_GetItemCount(mem->hList);
//        for (int i = 0; i < len; i++)
//            ListView_SetItemState(mem->hList, i, LVIS_SELECTED, LVIS_SELECTED);
//    }
//    bool CALLBACK AsmView_OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, PASMVIEW_DATA mem, LRESULT* pRet)
//    {
//        const int id = LOWORD(wParam);
//        switch (id)
//        {
//        case ID_ASM_MASM:
//            *mem->pShowMode = MasmSyntax;
//            AsmView_SwitchMode(mem);
//            break;
//        case ID_ASM_GO:
//            *mem->pShowMode = GoAsmSyntax;
//            AsmView_SwitchMode(mem);
//            break;
//        case ID_ASM_NASM:
//            *mem->pShowMode = NasmSyntax;
//            AsmView_SwitchMode(mem);
//            break;
//        case ID_ASM_AT:
//            *mem->pShowMode = ATSyntax;
//            AsmView_SwitchMode(mem);
//            break;
//        case ID_ASM_IM:
//            *mem->pShowMode = IntrinsicMemSyntax;
//            AsmView_SwitchMode(mem);
//            break;
//        case ID_ASM_COPY:           // ����ȫ��ѡ����Ŀ, ������ַ, �ֽ���, ���
//        case ID_ASM_COPY_ADDR:      // ֻ���Ƶ�ַ
//        case ID_ASM_COPY_CODE:      // ֻ�����ֽ���
//        case ID_ASM_COPY_CODE_C:    // ֻ�����ֽ���, C��ʽ
//        case ID_ASM_COPY_CODE_A:    // ֻ�����ֽ���, ����ʽ
//        case ID_ASM_COPY_CODE__:    // __asm _emit
//        case ID_ASM_COPY_CODE_E:    // ֻ�����ֽ���, �����Ը�ʽ
//        case ID_ASM_COPY_ASM:       // ֻ���ƻ��
//            AsmView_Copy(mem, id);
//            break;
//        case ID_ASM_JMP_OFFSET:     // ��ת��ƫ���ֽڴ�
//        {
//            AsmView_SetListViewIndex(mem, mem->offset_index);
//            break;
//        }
//        case ID_ASM_SELALL:         // ȫѡ Ctrl+A
//            AsmView_SelAll(mem);
//            break;
//        case ID_ASM_SEARCH:         // ���� Ctrl+F
//            AsmView_Search(mem);
//            break;
//        case ID_ASM_NEXT:           // ������һ�� N
//            AsmView_Search_Next(mem);
//            break;
//        case ID_ASM_PREV:           // ������һ�� P
//            AsmView_Search_Prev(mem);
//            break;
//        case ID_ASM_JMP:            // ��ת��ָ����ַ
//            AsmView_Jump(mem);
//            break;
//        default:
//            return DefWindowProcW(hWnd, message, wParam, lParam);
//        }
//        return 0;
//    }
//
//    
//    inline void AsmView_SetListItemHeight(HWND hList)
//    {
//        DWORD style = (DWORD)GetWindowLongPtrW(hList, GWL_STYLE);
//        DWORD newStyle = style | LVS_OWNERDRAWFIXED;
//        SetWindowLongPtrW(hList, GWL_STYLE, newStyle);
//        WINDOWPOS pos = { 0 };
//        pos.hwnd = hList;
//        SendMessageA(hList, WM_WINDOWPOSCHANGED, 0, (LPARAM)&pos);
//
//
//        SetWindowLongPtrW(hList, GWL_STYLE, style);
//        SetWindowPos(hList, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOCOPYBITS);
//    }
//
//    static void AsmView_switch_listview_color(PASMVIEW_DATA mem)
//    {
//        if (!mem->isClr)
//            return;
//
//        THEMECOLOR_STRUCT& clr = mem->clr;
//
//        SCROLL_RECT_INFO v;
//        v.btn1Size = 10;
//        v.btn2Size = v.btn1Size;
//
//        //v.btn1Size = 0;
//        //v.btn2Size = 0;
//        v.btnThumbSize = 12;
//        v.btnMinSize = 20;
//
//        v.crBtn1        = RGB2ARGB(clr.crScrollBtn1Normal, 255);
//        v.crBtnHot1     = RGB2ARGB(clr.crScrollBtn1Hot, 255);
//        v.crBtnDown1    = RGB2ARGB(clr.crScrollBtn1Down, 255);
//        v.crBtn2        = RGB2ARGB(clr.crScrollBtn2Normal, 255);
//        v.crBtnHot2     = RGB2ARGB(clr.crScrollBtn2Hot, 255);
//        v.crBtnDown2    = RGB2ARGB(clr.crScrollBtn2Down, 255);
//        v.crThumb       = RGB2ARGB(clr.crScrollThumbNormal, 255);
//        v.crThumbHot    = RGB2ARGB(clr.crScrollThumbHot, 255);
//        v.crThumbDown   = RGB2ARGB(clr.crScrollThumbDown, 255);
//        v.crBack        = RGB2ARGB(clr.crScrollBack, 255);
//
//
//        IDrawScroll* iScroll = mem->iScroll;
//        if (!iScroll)
//        {
//            iScroll = new IDrawScroll(clr.crBorder, v);
//            iScroll->m_isSysListView = LISTVIEW_ITEM_HEIGHT;
//            iScroll->m_isSysTreeView = false;
//            iScroll->m_isCallSetScroll = true;
//            iScroll->bind(mem->hList, false, true);
//            mem->iScroll = iScroll;
//        }
//
//        iScroll->m_vInfo = v;
//        iScroll->m_hInfo = v;
//        iScroll->SetBorderColor(clr.crBorder);
//
//        ListView_SetBkColor(mem->hList, clr.crBack);
//        ListView_SetTextColor(mem->hList, clr.crText);
//
//    }
//
//
//
//    void AsmView_Show(PASMVIEW_DATA mem)
//    {
//        DISASM infos = { 0 };
//        int len = 0;
//        const int showMode = *mem->pShowMode;
//        infos.Archi = mem->isx64 ? 0 : 1;       // 1 = ��ʾ�����32λ / 0 = ��ʾ�����64λ
//        infos.Options = showMode;               // ָ���﷨��ʽ MASM
//        infos.EIP = 0;                          // �ֽڵ�ַ, �ڲ��Ǵ������ȡ����ת���ɻ���
//        infos.VirtualAddr = mem->address;       // ��ʾ�ĵ�ַ, ���ɵĻ������Ǹ�����������ַ����, ������ת/call, ���ǻ��������ַ����
//
//        UInt64 pStart = (UInt64)(&mem->data[0]);
//        UInt64 pEnd = pStart + mem->nDataSize;
//        int selIndex = -1;
//        int addr_offset = 0;
//        wchar_t buf[100];
//        wchar_t buf_addr[24];
//        char buf_asm[100];
//
//        auto pfn_push_back = [&](LPBYTE pCode, int size, LPCSTR pszAsm)
//        {
//            ASMVIEW_LIST item = { 0 };
//
//            const int len_addr = swprintf_s(buf_addr, mem->isx64 ? L"0x%016llX" : L"0x%08X", infos.VirtualAddr);
//            buf[0] = 0;
//            int offset = 0;
//            for (int i = 0; i < size; i++)
//            {
//                const int _BufferCount = 100 - offset;
//                offset += swprintf_s(&buf[offset], _BufferCount, L"%02X ", pCode[i]);
//            }
//
//            item.addr = mem->buf.AddString(buf_addr, len_addr);
//            item.code = mem->buf.AddString(buf, offset);
//            item.szAsm = mem->buf.AddStringWFromAnsi(pszAsm);
//            item.text = L"";
//            item.pCode = pCode;
//            item.nSize = size;
//            const int arrSize = (int)mem->arr.size();
//            const int len_index = swprintf_s(buf, 20, L"%d ", arrSize);
//            item.index = mem->buf.AddString(buf, len_index);
//            mem->arr.push_back(std::move(item));
//        };
//
//        //while ( infos.Error == 0 )
//        while (pStart < pEnd)
//        {
//            //if ( pStart >= pEnd )
//            //    break;
//            infos.EIP = (UIntPtr)pStart;
//            len = Disasm(&infos);
//
//            if (selIndex == -1 && len > 0)
//            {
//                if (mem->offset >= (SIZE_T)(addr_offset) && mem->offset < (SIZE_T)(addr_offset + len))
//                    selIndex = (int)mem->arr.size();
//                addr_offset += len;
//            }
//
//            switch (infos.Error)
//            {
//            case OUT_OF_BLOCK:
//            {
//                //break;
//                infos.Error = 0;
//                pfn_push_back((LPBYTE)pStart, 1, "????");
//                pStart++;
//                break;
//            }
//            case UNKNOWN_OPCODE:    // �޷��������ֽ�, �����ʺ�
//            {
//                infos.Error = 0;
//                pfn_push_back((LPBYTE)pStart, 1, "??");
//                pStart++;
//                break;
//            }
//            default:
//            {
//                LPCSTR pAsm = infos.CompleteInstr;
//                LPCSTR pos = strchr(pAsm, ' ');
//                if (pos)
//                {
//                    // �пո�, ˵���в�����, �Ѳ�������ָ��ֿ�
//                    int asm_len = (int)(pos - pAsm);
//                    memcpy(buf_asm, pAsm, asm_len);
//
//                    const int kg = 7;    // �������λ���ո�
//                    for (int i = asm_len; i < kg; i++)
//                        buf_asm[i] = ' ';
//                    asm_len = kg;
//                    buf_asm[asm_len] = 0;
//                    pos++;
//                    int len_pos = (int)strlen(pos);
//                    memcpy(&buf_asm[asm_len], pos, len_pos);
//                    asm_len += len_pos;
//                    buf_asm[asm_len] = 0;
//                    pAsm = buf_asm;
//                }
//                pfn_push_back((LPBYTE)pStart, len, pAsm);
//                infos.VirtualAddr += len;
//                pStart += len;
//                infos.Error = 0;
//                break;
//            }
//            }
//
//        }
//
//        int listCount = ListView_GetItemCount(mem->hList);
//        ListView_SetItemCount(mem->hList, mem->arr.size());
//        if (selIndex != -1 && listCount == 0) // �ڶ��β���ʾ��ѡ����
//        {
//            AsmView_SetListViewIndex(mem, selIndex);
//            mem->offset_index = selIndex;
//
//        }
//        ::InvalidateRect(mem->hList, 0, 0);
//    }
//
//    void AsmView_SwitchMode(PASMVIEW_DATA mem)
//    {
//        // ���������ʾģʽ, �Ѿ���ģʽ���õ��ṹ����, ����Ͱ��������, �ѻ��������þ�����
//        mem->arr.clear();
//        mem->buf.clear();
//        AsmView_Show(mem);  // ������ʾ
//    }
//
//    bool CALLBACK ListView_OnCustomDraw(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, PASMVIEW_DATA mem, LRESULT* pResult)
//    {
//        LPNMLVCUSTOMDRAW lpNMCustomDraw = (LPNMLVCUSTOMDRAW)lParam;
//        if (lpNMCustomDraw->nmcd.hdr.hwndFrom != mem->hList)
//            return false;
//
//        switch (lpNMCustomDraw->nmcd.dwDrawStage)
//        {
//        case CDDS_PREPAINT:
//        {
//            *pResult = CDRF_NOTIFYITEMDRAW;
//            return true;
//        }
//        case CDDS_ITEMPREPAINT:
//        {
//            *pResult = CDRF_NOTIFYITEMDRAW;
//            *pResult |= CDRF_NOTIFYSUBITEMDRAW | CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW;
//            return true;    // ������, �滭�����������ﴦ��, ���ﷵ����Ҫ�滭����
//        }
//        case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
//        {
//            // ����Ļ����������ǽṹ���ȡ
//            const int iSubItem = (lpNMCustomDraw->iSubItem >= LIST_MAXCOLUMN || lpNMCustomDraw->iSubItem < 0) ? 0 : lpNMCustomDraw->iSubItem;
//            return ListView_PaintItemBegin(hWnd, message, wParam, lParam, mem, pResult, iSubItem);
//        }
//        case CDDS_ITEMPOSTPAINT:
//        {
//            *pResult = CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYSUBITEMDRAW;
//            return true;    // ������, �滭�����������ﴦ��, ���ﷵ����Ҫ�滭����
//
//        }
//        case CDDS_SUBITEM | CDDS_ITEMPOSTPAINT:
//        {
//            const int iSubItem = (lpNMCustomDraw->iSubItem >= LIST_MAXCOLUMN || lpNMCustomDraw->iSubItem < 0) ? 0 : lpNMCustomDraw->iSubItem;
//            return ListView_PaintItemEnd(hWnd, message, wParam, lParam, mem, pResult, iSubItem);
//        }
//        default:
//            return false;
//        }
//        return false;
//    }
//
//
//
//    bool CALLBACK ListView_PaintItemBegin(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, PASMVIEW_DATA mem, LRESULT* pResult, int iSubItem)
//    {
//        *pResult = CDRF_NOTIFYSUBITEMDRAW;// | CDRF_SKIPPOSTPAINT;
//
//        LPNMLVCUSTOMDRAW lpNMCustomDraw = (LPNMLVCUSTOMDRAW)lParam;
//        std::vector<ASMVIEW_LIST>& arr = mem->arr;
//        const int iItem = (int)lpNMCustomDraw->nmcd.dwItemSpec;
//        HDC hdc = lpNMCustomDraw->nmcd.hdc;
//        RECT& rc = lpNMCustomDraw->nmcd.rc;
//
//
//        const int state = ListView_GetItemState(mem->hList, iItem, 0xffff);  // ��Ŀ��״̬
//        const bool isFocus = __query(state, (LVIS_SELECTED | LVIS_FOCUSED));
//        const bool isSel = __query(state, (LVIS_SELECTED));
//
//        THEMECOLOR_STRUCT& clr = mem->clr;
//
//        if (mem->offset_index == iItem)
//            lpNMCustomDraw->clrTextBk = clr.crSelected;
//        else if (isSel)
//            lpNMCustomDraw->clrTextBk = clr.crHot;
//        else
//            lpNMCustomDraw->clrTextBk = clr.crBack;
//        lpNMCustomDraw->clrText = clr.crText;
//
//        lpNMCustomDraw->nmcd.uItemState &= ~(CDIS_SELECTED);  // ȥ��ѡ��״̬, �ñ�ѡ����Ŀ��ʾ�Լ����õ���ɫ
//        lpNMCustomDraw->nmcd.uItemState &= ~(CDIS_FOCUS);     // ȥ������״̬, �ñ�ѡ����Ŀû�������Ǹ���
//
//
//
//
//        //if ( iSubItem == LIST_ASM )
//        if (iSubItem > 0)
//        {
//            *pResult |= CDRF_NOTIFYPOSTPAINT;
//
//        }
//
//        if (0)    //TODO Ӧ��Ҫ�ж��Ƿ���Ҫ�滭����¼�
//            *pResult |= CDRF_NOTIFYPOSTPAINT;
//        return true;
//    }
//
//    bool CALLBACK ListView_PaintItemEnd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, PASMVIEW_DATA mem, LRESULT* pResult, int iSubItem)
//    {
//        LPNMLVCUSTOMDRAW lpNMCustomDraw = (LPNMLVCUSTOMDRAW)lParam;
//        const int iItem = (int)lpNMCustomDraw->nmcd.dwItemSpec;
//        std::vector<ASMVIEW_LIST>& arr = mem->arr;
//        HDC hdc = lpNMCustomDraw->nmcd.hdc;
//        RECT& rc = lpNMCustomDraw->nmcd.rc;
//
//        if (iSubItem == LIST_ASM && iItem < (int)arr.size())
//        {
//            UINT old = SetTextAlign(hdc, TA_NOUPDATECP);
//            COLORREF crBack = GetBkColor(hdc);
//            COLORREF crText = GetTextColor(hdc);
//            int mode = SetBkMode(hdc, OPAQUE);
//            ASMVIEW_LIST& arr_item = arr[iItem];
//
//            //AsmView_DrawAsm(mem, hdc, &rc, arr_item);
//
//            SetBkColor(hdc, crBack);
//            SetTextColor(hdc, crText);
//            SetTextAlign(hdc, old);
//            SetBkMode(hdc, mode);
//        }
//
//        if (iSubItem > 0)
//        {
//            // �滭��ߵ���, ��һ�в���Ҫ����ߵ�
//            THEMECOLOR_STRUCT& clr = mem->clr;
//            HPEN hPen = HexView_GetPen(PS_SOLID, 1, clr.crBorder);
//            HGDIOBJ hOldPen = SelectObject(hdc, hPen);
//            MoveToEx(hdc, rc.left, rc.top, 0);
//            LineTo(hdc, rc.left, rc.bottom);
//            SelectObject(hdc, hOldPen);
//        }
//
//
//        return false;
//    }
//
//    void AsmView_PopupMenu(PASMVIEW_DATA mem)
//    {
//
//        HMENU hMenu = CreatePopupMenu();
//        HMENU hCopy = CreatePopupMenu();
//        HMENU hMode = CreatePopupMenu();
//
//        LVITEMW lv_item = { 0 };
//        lv_item.mask = LVIF_STATE;
//        int sel_all = MF_DISABLED;
//
//        int len = ListView_GetItemCount(mem->hList);
//        for (int i = 0; i < len; i++)
//        {
//            if (ListView_GetItemState(mem->hList, i, LVIS_SELECTED) == 0)
//            {
//                sel_all = MF_STRING;
//                break;
//            }
//        }
//
//
//        AppendMenuW(hMenu, MF_STRING, ID_ASM_COPY, L"&C.����\tCtrl + C");
//        AppendMenuW(hCopy, MF_STRING, ID_ASM_COPY_ASM, L"���ƻ��");
//        AppendMenuW(hCopy, MF_STRING, ID_ASM_COPY_ADDR, L"���Ƶ�ַ");
//        AppendMenuW(hCopy, MF_STRING, ID_ASM_COPY_CODE, L"�����ֽ���");
//        AppendMenuW(hCopy, MF_STRING, ID_ASM_COPY_CODE_C, L"�����ֽ���(C��ʽ)");
//        AppendMenuW(hCopy, MF_STRING, ID_ASM_COPY_CODE_A, L"�����ֽ���(����ʽ)");
//        AppendMenuW(hCopy, MF_STRING, ID_ASM_COPY_CODE__, L"�����ֽ���(_emit)");
//        AppendMenuW(hCopy, MF_STRING, ID_ASM_COPY_CODE_E, L"�����ֽ���(�����Ը�ʽ)");
//        AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hCopy, L"����ʽ����");
//
//
//        const int showMode = *mem->pShowMode;
//        AppendMenuW(hMode, showMode == MasmSyntax ? MF_CHECKED : MF_STRING, ID_ASM_MASM, L"MASM�﷨");
//        AppendMenuW(hMode, showMode == GoAsmSyntax ? MF_CHECKED : MF_STRING, ID_ASM_GO, L"GoAsm�﷨");
//        AppendMenuW(hMode, showMode == NasmSyntax ? MF_CHECKED : MF_STRING, ID_ASM_NASM, L"NASM�﷨");
//        //AppendMenuW(hMode, showMode == ATSyntax ? MF_CHECKED : MF_STRING, ID_ASM_AT, L"AT�﷨");
//        AppendMenuW(hMode, showMode == IntrinsicMemSyntax ? MF_CHECKED : MF_STRING, ID_ASM_IM, L"IntrinsicMem�﷨");
//        AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hMode, L"�л�����﷨");
//
//        if (mem->offset > 0)  // ����ת, ������ת��ƫ���ֽ�λ��
//            AppendMenuW(hMenu, MF_STRING, ID_ASM_JMP_OFFSET, L"��ת��ƫ���ֽڴ�");
//
//        AppendMenuW(hMenu, MF_SEPARATOR, 0, L"");
//
//        LPCWSTR pSearchText = mem->search.GetResult();
//        const int flag_search = (!pSearchText || !*pSearchText) ? MF_DISABLED : MF_STRING;
//        AppendMenuW(hMenu, sel_all, ID_ASM_SELALL, L"&A.ȫѡ\tCtrl + A");
//        AppendMenuW(hMenu, MF_STRING, ID_ASM_SEARCH, L"&F.����\tCtrl + F");
//        AppendMenuW(hMenu, flag_search, ID_ASM_NEXT, L"&N.������һ��\tF3");
//        AppendMenuW(hMenu, flag_search, ID_ASM_PREV, L"&P.������\tShift + F3");
//        AppendMenuW(hMenu, MF_SEPARATOR, 0, L"");
//        AppendMenuW(hMenu, MF_STRING, ID_ASM_JMP, L"&G.��ת\tCtrl + G");
//        POINT pt;
//        GetCursorPos(&pt);
//        TrackPopupMenu(hMenu, TPM_LEFTALIGN, pt.x, pt.y, 0, mem->hWnd, 0);
//        DestroyMenu(hMenu);
//
//    }
//
//    void AsmView_SetListViewIndex(PASMVIEW_DATA mem, int index)
//    {
//        const int start = ListView_GetNextItem(mem->hList, -1, (LVNI_FOCUSED | LVNI_SELECTED));
//        LVITEMW lv_item;
//        lv_item.mask = LVIF_STATE;
//        if (start >= 0)
//        {
//            lv_item.state = 0;
//            lv_item.stateMask = LVIS_SELECTED;
//            SendMessageW(mem->hList, LVM_SETITEMSTATE, start, (LPARAM)&lv_item);
//        }
//
//        lv_item.state = LVIS_SELECTED | LVIS_FOCUSED;
//        lv_item.stateMask = lv_item.state;
//        SendMessageW(mem->hList, LVM_SETITEMSTATE, index, (LPARAM)&lv_item);
//        SendMessageW(mem->hList, LVM_ENSUREVISIBLE, index, (LPARAM)1);
//    }
//
//    inline void AsmView_search_msgbox(PASMVIEW_DATA mem)
//    {
//        // û������������
//        LPCWSTR pSearchText = mem->search.GetResult();
//        size_t len = wcslen(pSearchText) + 260;
//        LPWSTR dbg = (LPWSTR)HexViewAlloc(len * sizeof(wchar_t));
//        swprintf_s(dbg, len,
//                   L"�����ı�: \"%s\"\r\n"
//                   L"û��������ƥ������", pSearchText);
//        MessageBoxW(mem->hWnd, dbg, L"û������������", 0);
//        HexViewFree(dbg);
//    }
//
//
//    inline BOOLEAN AsmView_Search_find(PASMVIEW_DATA mem, LPCWSTR text, int start, bool isNext)
//    {
//        const int len = ListView_GetItemCount(mem->hList);
//        if (len != (int)mem->arr.size())
//        {
//            MessageBoxW(mem->hWnd, L"�б����ݴ���, �������ⲿ�޸��б�����", L"����", 0);
//            return false;
//        }
//
//        auto pfn_cmp = [&](int i)
//        {
//            ASMVIEW_LIST& item = mem->arr[i];
//            if (wcsstr(item.addr, text) != 0 || wcsstr(item.code, text) != 0 || wcsstr(item.szAsm, text) != 0)
//            {
//                AsmView_SetListViewIndex(mem, i);
//                return true;
//            }
//            return false;
//        };
//
//        if (isNext)
//        {
//            for (int i = start; i < len; i++)
//            {
//                if (pfn_cmp(i))
//                    return true;
//            }
//        }
//        else
//        {
//            for (int i = start; i >= 0; i--)
//            {
//                if (pfn_cmp(i))
//                    return true;
//            }
//        }
//
//
//        AsmView_search_msgbox(mem);
//        return false;
//    }
//
//
//    void AsmView_Search(PASMVIEW_DATA mem)
//    {
//        CSearchBox& search = mem->search;
//
//        LPCWSTR str = search.Show(mem->hWnd);
//        if (!str) return; // �Ի���ȡ��������
//
//        // �ߵ����������������������������
//
//        AsmView_Search_find(mem, str, 0, true);
//    }
//    void AsmView_Search_Next(PASMVIEW_DATA mem)
//    {
//        int start = ListView_GetNextItem(mem->hList, -1, (LVNI_FOCUSED | LVNI_SELECTED)) + 1;
//        LPCWSTR pSearchText = mem->search.GetResult();
//        AsmView_Search_find(mem, pSearchText, start, true);
//    }
//    void AsmView_Search_Prev(PASMVIEW_DATA mem)
//    {
//        int start = ListView_GetNextItem(mem->hList, -1, (LVNI_FOCUSED | LVNI_SELECTED)) - 1;
//        LPCWSTR pSearchText = mem->search.GetResult();
//        AsmView_Search_find(mem, pSearchText, start, false);
//    }
//
//    void AsmView_Jump(PASMVIEW_DATA mem)
//    {
//        int start = ListView_GetNextItem(mem->hList, -1, (LVNI_FOCUSED | LVNI_SELECTED));
//        CJumpBox& jmp = mem->jmp;
//
//        if (!jmp.Show(mem->hWnd))
//            return; // �Ի���ȡ��������
//
//        LPCWSTR str = jmp.GetResult();
//
//        const int maxPos = (int)ListView_GetItemCount(mem->hList);
//        const int mode = jmp.GetMode();
//        const int retPos = (int)jmp.GetResultNumber();
//        int pos = 0;
//        if (mode == 0)    // �б�ʼ
//        {
//            pos = retPos;
//            if (pos >= maxPos)
//                pos = (retPos > 0) ? maxPos - 1 : 0;
//        }
//        else if (mode == 1)   // ��ǰλ��
//        {
//            pos = start + retPos;
//            if (pos >= maxPos)
//                pos = (retPos > 0) ? maxPos - 1 : 0;
//
//        }
//        else if (mode == 2)   // ��ǰλ������
//        {
//            pos = start - retPos;
//            if (pos >= maxPos)
//                pos = (retPos > 0) ? 0 : maxPos - 1;
//        }
//        else if (mode == 3)   // �ļ�����
//        {
//            pos = maxPos - retPos;
//            if (pos > maxPos)
//                pos = (retPos > 0) ? 0 : maxPos - 1;
//        }
//
//        AsmView_SetListViewIndex(mem, pos);
//    }
//
//    LRESULT HexView_OnKeyDown(PASMVIEW_DATA mem, HWND hWnd, WPARAM wParam, LPARAM lParam)
//    {
//        BOOLEAN IsCtrlKeyDown = (GetKeyState(VK_CONTROL) & 0x8000) ? TRUE : FALSE;
//        BOOLEAN IsShiftKeyDown = (GetKeyState(VK_SHIFT) & 0x8000) ? TRUE : FALSE;
//        switch (wParam)
//        {
//        case 'C':
//        {
//            if (!IsCtrlKeyDown)
//                break;
//            AsmView_Copy(mem, ID_ASM_COPY);
//            break;
//        }
//        case 'A':
//        {
//            if (!IsCtrlKeyDown)
//                break;
//            AsmView_SelAll(mem);
//            break;
//        }
//        case 'F':   // ����
//        {
//            if (!IsCtrlKeyDown)
//                break;
//            AsmView_Search(mem);
//            break;
//        }
//        case VK_F3: // ������/��һ��
//        {
//            if (IsCtrlKeyDown)
//                break;
//            if (IsShiftKeyDown)
//                AsmView_Search_Prev(mem);
//            else
//                AsmView_Search_Next(mem);
//            return 0;
//        }
//        case 'G':   // ��ת
//        {
//            if (!IsCtrlKeyDown)
//                break;
//            AsmView_Jump(mem);
//            break;
//        }
//        default:
//            return CallWindowProcW(mem->oldProc_list, hWnd, WM_KEYDOWN, wParam, lParam);
//        }
//
//
//        return 0;
//    }
//
//
//
//};
//
