#pragma once
//#include <control/WndBase.h>
#include "CTreeView.h"
#include <CArray.h>
#include <control/Ctips.h>
#include <d2d/d2d_old.h>

class CTreeList;
class CTreeListBase : public CTreeView
{
public:
    virtual LRESULT OnHeadNotify(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
    virtual LRESULT OnTreeNotify(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
    virtual LRESULT OnPaintHeader(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
    virtual LRESULT WndProc_header(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
    virtual void __create_d2d_resouce() = 0;
    virtual HWND GetTreeWindow() = 0;
};

#define TLVIF_TEXT              0x0001  // ��ȡ�ı�, ��ȡ�ı�ʱ cchText��Ҫ��ֵ
#define TLVIF_FONT              0x0002  // ��������, hFont��Ҫ��ֵ
#define TLVIF_BORDER            0x0004  // �߿�, border,isManualBorder ��Ҫ��ֵ
#define TLVIF_PARAM             0X0008  // ��Ŀ��ֵ, param��Ҫ��ֵ
#define TLVIF_TYPE              0x0010  // ������, type��Ҫ��ֵ
#define TLVIF_CRTEXT            0x0020  // �ı���ɫ, crText��Ҫ��ֵ
#define TLVIF_ABSLEFT           0x0040  // �������
#define TLVIF_ABSCOL            0x0080  // ���Կ��
#define TLVIF_TEXTFORMAT        0x0100  // �ı������ʽ
#define TLVIF_ALL               0x01FF  // ��������


#define BORDER_PEN_NULL         0x0000  // û�б߿�
#define BORDER_PEN_SOLID        0x0001  // ʵ��
#define BORDER_PEN_DASH         0x0002  // ����
#define BORDER_PEN_DOT          0x0003  // ����
#define BORDER_PEN_DASHDOT      0x0004  // �㻮��
#define BORDER_PEN_DASHDOTDOT   0x0005  // ��㻮��
#define BORDER_LEFT             0x0100  // �滭��߱߿�
#define BORDER_TOP              0x0200  // �滭���߱߿�
#define BORDER_RIGHT            0x0400  // �滭�ұ߱߿�
#define BORDER_BOTTOM           0x0800  // �滭�ױ߱߿�
#define BORDER_RECT         (BORDER_LEFT | BORDER_TOP | BORDER_RIGHT | BORDER_BOTTOM)  // (BORDER_LEFT | BORDER_TOP | BORDER_RIGHT | BORDER_BOTTOM)

#define TLVTYPE_TEXT            0       // ���ı�
#define TLVTYPE_STATIC          1       // ��̬�ı�, ����Ӧ�ȵ㰴�µĻ滭
#define TLVTYPE_BUTTON          2       // ��ť
#define TLVTYPE_EDIT            3       // �༭��
#define TLVTYPE_CHECKBOX        4       // ѡ���
#define TLVTYPE_RADIO           5       // ��ѡ��
#define TLVTYPE_COMBOBOX        6       // ��Ͽ�
#define TLVTYPE_        0

#define TREEITEM_ICONWDITH      16
#define TREESYSICON_INVALID     -1
#define TREESYSICON_EXPAND      0
#define TREESYSICON_CHECKBOX    1
#define TREESYSICON_ICON        2
typedef struct HMD
{
    HDC hdc;
    HBITMAP hBitmap;
    void* lpBits;
    int width;
    int height;
}*LPHMD;
typedef struct TLVITEM
{
    int cbSize;
    int mask;               // TLVIF_ ��־, ���ݱ�־����������Щ��Ա
    bool isAbsWidth;        // �Ƿ�����п�, Ϊ��ʱ������ͷ��ʱ���޸����п�
    bool isAbsLeft;         // �Ƿ�������, Ϊ��ʱ������ͷ��ʱ���޸������
    bool isManualBorder;    // �Ƿ��ֶ����ñ߿�
    int indexCol;           // ������
    int width;              // �п�
    int left;               // �����λ��
    DWORD border;           // BORDER_ �߿�����
    DWORD format;           // �ı������ʽ
    LPWSTR text;            // ���û��ȡ�ı�
    int cchText;            // ����������, ��ȡʱ��Ҫ����
    int type;               // TLVTYPE_, ������, ���е�������ĳ�е�ĳ��
    LONG_PTR param;         // ��Ŀ��ֵ
    ARGB_D2D crText;        // �ı���ɫ
    LPEX_FONT hFont;        // ������ _font_create() ����
}*LPTLVITEM;
typedef const TLVITEM* LPCTLVITEM;
typedef struct TREELIST_DATA
{
    CTreeListBase* pThis;   // ����ָ��
    WNDPROC oldProcStatic;  // �ɴ��ڹ���
    WNDPROC oldProcTree;    // �ɴ��ڹ���
    WNDPROC oldProcHeader;  // �ɴ��ڹ���
    HWND hHead;             // ��ͷ���
    HWND hStatic;           // ��ǩ���, ���Ϳ�ͱ�ͷ����������
    HWND hTree;             // ���Ϳ򴰿ھ��
    int indexHotHead;       // �ȵ���
    int indexDownHead;      // ������
    int nHeadHeight;        // ͷ�߶�, Ĭ��30
    HTREEITEM index;        // ��ǰѡ������
    HTREEITEM hNext;        // ��ǰѡ��������һ���ֵ���, �ڵ�������ʱ��ֵ
    HTREEITEM indexDown;    // ��갴�±���
    int indexCol;           // ��ǰѡ��������
    int indexColIcon;       // ��ǰ������ĸ�ϵͳͼ����
    HTREEITEM indexHot;     // �ȵ���, ������ƶ���ֵ
    HTREEITEM hotParent;    // �ȵ���ĸ���Ŀ
    int ptParam;            // ���µ�����, ��λX,��λY
    int width;              // ���ڿ��
    int height;             // ���ڸ߶�
    RECT rcHead;            // ��ͷλ��, ���ҵ�
    bool isHover;           // �Ƿ����ؼ�, �� NM_SETCURSOR ��ֵΪtrue, ��WM_MOUSELEAVE��ֵΪfalse
    bool isExpand;          // �Ƿ���ƼӼ���
    bool isIcon;            // �Ƿ����ͼ��
    bool isTips;            // �Ƿ�����ʾ����
    DWORD CheckType;        // �Ƿ����ѡ���, 0=û��, 1=˫̬ѡ���, 2=3̬ѡ���(����), 3=3̬ѡ���(�ų�), 4=3̬ѡ���(��ֹ)
    int state;              // ����״̬
    float Indent;           // ��������, Ĭ��=12.0
    int count;              // ��ǰ���б���Ŀ��
    int rcFirstHotLeft;     // ��һ���ȵ�������

    LPEX_FONT hFont;        // Ĭ�ϻ滭����
    LPEX_BRUSH hBrNormal;   // ������ˢ
    LPEX_BRUSH hBrSF;       // ѡ��+����
    LPEX_BRUSH hBrSelect;   // ѡ�л�ˢ
    LPEX_BRUSH hBrHot;      // �ȵ㻭ˢ
    LPEX_BRUSH hBrtext;     // �ȵ㻭ˢ
    LPEX_BRUSH hBrLine;     // ������ˢ
    LPEX_PEN hpenLine;      // ������ˢ
    LPEX_BRUSH hBrTopLine;  // ����������ˢ
    LPEX_PEN hpenTopLine;   // ����������ˢ
    LPEX_BRUSH hBrHove;     // ���뻭ˢ
    LPCANVAS hCanvas;       // �滭���
    LPCANVAS hCanvas_Head;  // ��ͷ�滭���

    // 0-128 �����Ϳ�ͼ��, 130 - 220 ��ѡ���ͼ��,  220 - 340 �ǵ�ѡ��ͼ��
    // ���Ϳ�ͼ�� 0���ؿ�ʼ, ������0,   16*16, 0=��������,1=����չ��,2=��ȼ����,3=��ȼչ��, 4-7 �Ӵ�
    // ѡ���ͼ�� 130���ؿ�ʼ, ������0, 15*15, 0=����δѡ��, 1=��ȼδѡ��, 2=����δѡ��, 3=����ѡ��, 4=��ȼѡ��, 5=����ѡ��
    // ��ѡ��ͼ�� 220���ؿ�ʼ, ������0, 15*15, 0=����δѡ��, 1=��ȼδѡ��, 2=����δѡ��, 3=��ֹδѡ��, 4=����ѡ��, 5=��ȼѡ��, 6=����ѡ��, 7=��ֹѡ��
    LPEX_IMAGE imgTree;     

}*LPTREELIST_DATA;
typedef struct TREELIST_COL_DATA
{
    DWORD width;
    RECT rc;
    wstr title;
    TREELIST_COL_DATA()
    {
        width = 0;
        rc = { 0 };
    }
}*LPTREELIST_COL_DATA;
typedef const TREELIST_COL_DATA* LPCTREELIST_COL_DATA;
typedef struct HTREELISTITEM_SUBDATA
{
    bool isAbsLeft;
    bool isAbsWidth;
    DWORD width;            // ÿ�еĿ��, ���Ը�����Ҫ�ֶ��޸�
    DWORD left;             // ÿ�е����λ��, ���Ը�����Ҫ�ֶ��޸�
    DWORD border;           // BORDER_ �߿�����
    wstr title;             // ��ǰ��ÿһ�е�����
    DWORD type;             // TLVTYPE_, ������, ÿһ�ж�����Ϊ��ͬ����, ���Ը�����Ҫ�޸�ָ����ָ����
    LPEX_FONT hFont;        // �滭����, ÿһ�ж����Բ�ͬ, Ϊ0��ʹ��Ĭ��
    ARGB_D2D crText;        // �滭������ɫ, ÿһ�ж����Բ�ͬ, Ϊ0��ʹ��Ĭ��
    LPEX_BRUSH hbrText;     // �滭���廭ˢ, ÿһ�ж����Բ�ͬ, Ϊ0��ʹ��Ĭ��
    RECT_F rcText;          // �ı�����
    DWORD format;           // �ı������ʽ
    LPARAM param;           // ����Ŀ��ֵ
    HTREELISTITEM_SUBDATA()
    {
        isAbsLeft = false;
        isAbsWidth = false;
        width = 0;
        left = 0;
        border = 0;
        type = 0;
        hFont = 0;
        hbrText = 0;
        format = 0;
        param = 0;
    }
}*LPHTREELISTITEM_SUBDATA;

// ��Ŀ����, ÿ����Ŀ����Ŀ��ֵ��������ṹ
// ��¼��ǰ��,ÿһ�е�����, �����п�,����,����,�ı���ɫ,����
typedef struct HTREELISTITEM
{
    HTREEITEM hItem;
    LONG_PTR param;         // ��Ŀ��ֵ
    //bool isAbsWidth;        // �Ƿ�����п�, Ϊ��ʱ������ͷ��ʱ���޸����п�
    //bool isAbsLeft;         // �Ƿ�������, Ϊ��ʱ������ͷ��ʱ���޸������
    bool isManualBorder;    // �Ƿ��ֶ����ñ߿�
    int count;              // ����, �������е������Ա���������ֵ
    int level;              // �������
    int state;              // ��ǰ����״̬
    int flag;               // �����־
    int stateExpand;        // �Ӽ���״̬
    int stateCheckBox;      // �Ӽ���״̬
    int stateIcon;          // �Ӽ���״̬
    int index;              // �������������
    LPHTREELISTITEM_SUBDATA draw;    // �滭��Ϣ����
    HTREELISTITEM(CArray<TREELIST_COL_DATA>& m_header)
    {
        memset(this, 0, sizeof(*this));
        count = m_header.size();
        if (count == 0)
            m_header.resize(++count);
        draw = new HTREELISTITEM_SUBDATA[count];
        memset(draw, 0, sizeof(HTREELISTITEM_SUBDATA) * count);

        //for (int i = 0; i < count; i++)
        //{
        //    draw[i].left = m_header.at(i).rc.left;
        //    draw[i].width = m_header.at(i).width;
        //}
    }
    ~HTREELISTITEM()
    {
        for (int i = 0; i < count; i++)
        {
            _font_destroy(draw[i].hFont);
            _brush_destroy(draw[i].hbrText);
        }
        delete[] draw;
        memset(this, 0, sizeof(*this));
    }

}*LPHTREELISTITEM;

typedef bool (*pfn_OnItemClick)(HTREEITEM hItem, LPHTREELISTITEM data, int& indexCol, int& indexColIcon, bool isRight);
typedef bool (*pfn_OnItemDblClk)(HTREEITEM hItem);
typedef bool (*pfn_OnIndexChenged)(HTREEITEM hItem, LPHTREELISTITEM data, int& indexCol, int& indexColIcon);
typedef void (*pfn_OnItemDelete)(HTREEITEM hItem, LPHTREELISTITEM data);
typedef void (*pfn_OnLButtonUp)(int x, int y);
typedef void (*pfn_OnRButtonUp)(int x, int y);

typedef struct tagTREELIST_EVENT
{
    pfn_OnItemClick pClick;
    pfn_OnItemDblClk pDblclk;
    pfn_OnIndexChenged pChenged;
    pfn_OnItemDelete pDelete;
    pfn_OnLButtonUp pLUp;
    pfn_OnRButtonUp pRUp;
}TREELIST_EVENT, * PTREELIST_EVENT, * LPTREELIST_EVENT;


class CTreeList : public CTreeListBase
{
private:
    typedef struct tagDWAWITEM_STRUCT
    {
        LPCANVAS hCanvas;
        HTREEITEM hItem;
        LPHTREELISTITEM data;
        HTREELISTITEM_SUBDATA subData;
        LPEX_FONT hFont;
        LPEX_BRUSH hbrText;
        tagDWAWITEM_STRUCT()
        {
            hCanvas = 0;
            hItem = 0;
            data = 0;
            hFont = 0;
            hbrText = 0;
        }
    }DWAWITEM_STRUCT, *LPDWAWITEM_STRUCT;

    // �ȵ�����ȵ���Ŀ
    typedef struct tagHOT_GROUP_ARY
    {
        HTREEITEM hItem;        // ��Ŀ���
        LPHTREELISTITEM data;   // ��Ŀ����
    }HOT_GROUP_ARY, *LPHOT_GROUP_ARY;

    int m_headerIndexHot;
    bool m_headerIsDown;
    CArray<TREELIST_COL_DATA> m_header;         // ��ͷ����
    int* m_headerIndex;                         // ����������, Ĭ���� 01234...�滭Ӧ�������Ա������������ȡ
    LPTREELIST_DATA m_data;
    CArray<HOT_GROUP_ARY> m_hotGroup;           // �ȵ���Ŀ��          
    Ctips m_tips;
    TREELIST_EVENT m_evt;                       // �����¼�
    bool m_isHotList;                           // �ȵ�ʱ�Ƿ���ʾ�����б�
    HTREEITEM* m_hItems;                        // ��ǰ������������
    int m_hItemsCount;                          // ��ǰ�������������Ա��
protected:
    virtual LRESULT WndProc(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT WndProc_header(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnHeadNotify(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnTreeNotify(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual bool OnStyleChenged(bool isEx, DWORD style);
    LRESULT IndexChanged(HTREEITEM hItem, LPHTREELISTITEM data, int indexCol, int indexColIcon, int x, int y);
    int     enumItem();
    LRESULT __OnSize(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT __OnMouseLDown(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT __OnMouseLUp(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void    __OnItemClick(HTREEITEM hItem, LPHTREELISTITEM data, int indexCol, int indexColIcon, bool isRightBtn);
    LRESULT __OnMouseMove(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT __OnMouseLeave(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT __OnPaint(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void    DrawItem(LPCANVAS hCanvas, HTREEITEM hItem, int index, int scrollPos, const RECT_F& rcParent);
    void    DrawItemLine(LPCANVAS hCanvas, HTREEITEM hItem, LPHTREELISTITEM data, const RECT_F& rcf, const TVITEMEXW& item);
    void    DrawItemBorder(LPDWAWITEM_STRUCT draw, const RECT_F& rcf);
    //void    GetDrawIInfo(LPHTREELISTITEM data);
    // ���ñ���״̬, isRemove = �Ƿ�ɾ��״̬, bDraw = �Ƿ��ػ�
    void    SetUIItem(HTREEITEM hItem, LPHTREELISTITEM data, STATE state, bool isRemove, bool bDraw);
    void    SetUIItem(HTREEITEM hItem, STATE state, bool isRemove, bool bDraw);
    int     GetUIItem(LPHTREELISTITEM data);
    int     GetUIItem(HTREEITEM hItem);
    void    SetStateIcon(LPHTREELISTITEM data, int index, STATE state, bool isRemove);
    int     GetStateIcon(LPHTREELISTITEM data, int index);

    // ���ø���Ŀ����������Ŀ�ȵ�״̬
    int     ModifyParentHot(HTREEITEM hParent, HTREEITEM hItem, bool isRemove);


    // �������
    void    FillItem(LPCANVAS hCanvas, LPHTREELISTITEM data, int scrollPos, const RECT_F& rcf);
    // �滭չ������ͼ��
    void    DrawItemExpand(LPCANVAS hCanvas, HTREEITEM hItem, LPHTREELISTITEM data, const RECT_F& rcf, const TVITEMEXW& item);
    // �滭ͼ��
    void    DrawItemIcon(LPCANVAS hCanvas, HTREEITEM hItem, LPHTREELISTITEM data, const RECT_F& rcf, const TVITEMEXW& item);
    // �滭ѡ���
    void    DrawItemCheckbox(LPCANVAS hCanvas, HTREEITEM hItem, LPHTREELISTITEM data, const RECT_F& rcf, const TVITEMEXW& item);
    // ��������, ����������Ѹ������ͷ��������������ʵ��
    void    DrawItemType(LPDWAWITEM_STRUCT draw, const RECT_F& rcf);
    // �����ı�����, ��̬�ı�
    void    DrawItemType_Text(LPDWAWITEM_STRUCT draw, const RECT_F& rcf);
    // ���ư�ť���Ϳؼ�
    void    DrawItemType_Button(LPDWAWITEM_STRUCT draw, const RECT_F& rcf);
    // ���Ʊ༭�����Ϳؼ�
    void    DrawItemType_Edit(LPDWAWITEM_STRUCT draw, const RECT_F& rcf);
    // ����ѡ������Ϳؼ�
    void    DrawItemType_CheckBox(LPDWAWITEM_STRUCT draw, const RECT_F& rcf);
    // ���Ƶ�ѡ�����Ϳؼ�
    void    DrawItemType_Radio(LPDWAWITEM_STRUCT draw, const RECT_F& rcf);
    // ������Ͽ����Ϳؼ�
    void    DrawItemTypeCombobox(LPDWAWITEM_STRUCT draw, const RECT_F& rcf);

    void __create_d2d_resouce();
    void __delete_d2d_resouce();
    void __modify_col_width(int indexCol, int cx);
    LRESULT OnHScroll(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT OnPaintHeader(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void    InvalidateItem(HTREEITEM hItem);

    // ����Ӽ����λ��, ���ص��ǼӼ����ڵ�ǰ��Ŀ�е�λ��, �����ڴ����е�λ��
    void CalcExpand(LPHTREELISTITEM data, RECT* rc, int scrollPos);
    // ����ͼ��λ��, ���ص���ͼ���ڵ�ǰ��Ŀ�е�λ��, �����ڴ����е�λ��
    void CalcIcon(LPHTREELISTITEM data, RECT* rc, int scrollPos);
    // ����ѡ���λ��, ���ص���ѡ����ڵ�ǰ��Ŀ�е�λ��, �����ڴ����е�λ��
    void CalcCheckbox(LPHTREELISTITEM data, RECT* rc, int scrollPos);
    // ����Ƿ��ڼӼ�����, x,y��������Ŀ����, ���Ǵ��ڿͻ�������
    bool ptinExpand(LPHTREELISTITEM data, int x, int y, int scrollPos);
    // ����Ƿ���ͼ����, x,y��������Ŀ����, ���Ǵ��ڿͻ�������
    bool ptinIcon(LPHTREELISTITEM data, int x, int y, int scrollPos);
    // ����Ƿ���ѡ�����, x,y��������Ŀ����, ���Ǵ��ڿͻ�������
    bool ptinCheckbox(LPHTREELISTITEM data, int x, int y, int scrollPos);
    // ����Ƿ��ڼ���ϵͳͼ����, �Ӽ���, ѡ���, ͼ��, �������⼸��ͼ���з���-1, ���򷵻ش�0��ʼ������
    // 0=�Ӽ���, 1=ѡ���, 2=ͼ��
    // x,y��������Ŀ����, ���Ǵ��ڿͻ�������
    int ptinSysIcon(LPHTREELISTITEM data, RECT* prc, int x, int y, int scrollPos);
    // ����Ƿ��ڱ�����, ���ڱ����л�ɾ���ȵ�״̬���ػ��ȵ����
    HTREEITEM MouseIsItem(int x, int y, WPARAM wParam, LPHTREELISTITEM* data, int* indexCol, int* indexColIcon, int scrollPos, bool isDown);
    
public:
    CTreeList();
    virtual ~CTreeList();
    virtual inline LPCWSTR className() { return L"TreeListView"; };

public:
    virtual HWND create(DWORD dwExStyle, LPCWSTR lpszTitle, DWORD dwStyle, DWORD dwExStyleTree,
        int x, int y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR ID = 0, LPVOID lpParam = 0);
    HWND create(HWND hWndParent, DWORD dwExStyleTree);
    HWND create(HWND hWndParent, LPCWSTR title, RECT rc, int style, int styleEx, DWORD dwExStyleTree, LONG_PTR ID, void* lpParam = 0);
    HWND GetTreeWindow() { return m_data ? m_data->hTree : 0; }
    // ���ñ���߶�, ��������ǰ�ĸ߶�
    int  SetHeaderHeight(int height);
    inline bool Move(int x, int y, int cx, int cy, bool bRepaint = true)
    {
        if (!m_data)return false;
        return MoveWindow(m_data->hStatic, x, y, cx, cy, bRepaint);
    }
    virtual HTREEITEM InsertItem(HTREEITEM hParent = TVGN_ROOT, LPCWSTR itemText = 0, LONG_PTR itemParam = 0, HTREEITEM InsertMode = 0);
    int DoInsertItem(int iInsertAfter, int nWidth, LPCWSTR lpsz);
    bool SetItem(HTREEITEM hItem, const TLVITEM* itemData, int indexCol);
    bool GetItem(HTREEITEM hItem, LPTLVITEM itemData);
    
    bool SetItemText(HTREEITEM hItem, int indexCol, LPCWSTR text);
    wstr GetItemText(HTREEITEM hItem, int indexCol);
    HIMAGELIST SetImageList(HIMAGELIST hImageList, int type = 0);

    // ȡ�б���
    wstr GetColText(int index);
    // ���б���
    bool SetColText(int index, LPCWSTR text);
    // ȡ����Ϣ, ���ص�ֵ��ò�Ҫ�޸�
    bool GetColInfo(int index, LPTREELIST_COL_DATA& info);
    // ȡ����Ŀ��ֵ
    LPARAM GetItemColParam(HTREEITEM hItem, int indexCol);
    // ֵ����Ŀ��ֵ
    bool SetItemColParam(HTREEITEM hItem, int indexCol, LPARAM lParam);
    // ȡ������Ŀ��ֵ
    LPARAM GetItemParam(HTREEITEM hItem);
    bool isItem(HTREEITEM hItem);
    // ֵ������Ŀ��ֵ
    bool SetItemParam(HTREEITEM hItem, LPARAM lParam);
    inline void IsHotList(bool b) { m_isHotList = b; }
    // ���ص�ǰѡ��������, ʧ�ܷ���-1, ������0��ʼ
    inline int GetIndexCol() { return m_data ? m_data->indexCol : -1; }
    // ȡ����ѡ����
    virtual inline HTREEITEM GetIndex() { return m_data ? m_data->index : 0; }
    virtual bool UpDateItem(HTREEITEM hItem = 0);
    // �޸�ѡ����
    virtual int SetIndex(HTREEITEM hItem = 0);
    virtual void SetColWidth(int index, int width);

public:
    // �󶨵����Ŀ�¼�, Ϊ0�����
    inline void OnItemClick(pfn_OnItemClick pfn = 0) { m_evt.pClick = pfn; }
    inline void OnItemDblclk(pfn_OnItemDblClk pfn = 0) { m_evt.pDblclk = pfn; }
    inline void OnItemDelete(pfn_OnItemDelete pfn = 0) { m_evt.pDelete = pfn; }
    inline void OnIndexChenged(pfn_OnIndexChenged pfn = 0) { m_evt.pChenged = pfn; }
    inline void OnLButtonUp(pfn_OnLButtonUp pfn = 0) { m_evt.pLUp = pfn; }
    inline void OnRButtonUp(pfn_OnRButtonUp pfn = 0) { m_evt.pRUp = pfn; }
    inline int GetHeaderWidths(int** ary) {
        int len = m_header.size();
        if (ary && len)
        {
            *ary = new int[len];
            for (int i = 0; i < len; i++)
                (*ary)[i] = m_header[i].width;
        }
        return len;
    }


public:
    inline HWND GetStatic() { return m_data ? m_data->hStatic : 0; }

    // ȡ���ڿͻ�ȡ����
    inline RECT GetClientRect() {
        RECT rc = { 0 };
        ::GetClientRect(GetStatic(), &rc);
        return rc;
    }

    // ȡ��������Ļ�е�λ��
    inline RECT GetWindowRect() {
        RECT rc = { 0 };
        ::GetWindowRect(GetStatic(), &rc);
        return rc;
    }

    // ȡ�����ڸ������е�λ��
    inline RECT GetRect() {
        RECT rc = { 0 };
        bool ret = ::GetWindowRect(GetStatic(), &rc);
        ScreenToClient(GetStatic(), (LPPOINT)(&rc));
        ScreenToClient(GetStatic(), ((LPPOINT)(&rc)) + 1);
        return rc;
    }

    // ȡ�û������
    inline bool GetClientWidth() {
        RECT rc = { 0 };
        ::GetClientRect(GetStatic(), &rc);
        return rc.right - rc.left;
    }

    // ȡ�û����߶�
    inline bool GetClientHeight() {
        RECT rc = { 0 };
        ::GetClientRect(GetStatic(), &rc);
        return rc.bottom - rc.top;
    }

    // ȡ�����ڸ������е�����
    inline POINT GetClientPos() {
        RECT rc = { 0 };
        bool ret = ::GetWindowRect(GetStatic(), &rc);
        POINT pt = { rc.left, rc.top };
        HWND hWnd = ::GetParent(GetStatic());
        if (!hWnd)hWnd = GetStatic();
        ScreenToClient(hWnd, &pt);
        return pt;
    }

    //// ���ô����ڸ������е�����
    //inline bool SetClientPos(const LPPOINT ppt) {
    //    if (!ppt) return false;
    //    return SetWindowPos(GetStatic(), 0, ppt->x, ppt->y, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE);
    //}

    // ȡ��������Ļ�е�����
    inline POINT GetPos() {
        RECT rc = { 0 };
        bool ret = ::GetWindowRect(GetStatic(), &rc);
        POINT pt = { rc.left, rc.top };
        return pt;
    }

    // ���ô�������Ļ�е�����
    inline bool SetPos(const LPPOINT ppt) {
        if (!ppt) return false;
        return SetWindowPos(GetStatic(), 0, ppt->x, ppt->y, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE);
    }

    // ȡ���ڿ��
    inline SIZE GetSize() {
        RECT rc = { 0 };
        bool ret = ::GetWindowRect(GetStatic(), &rc);
        SIZE si = { 0 };
        si.cx = rc.right - rc.left;
        si.cy = rc.bottom - rc.top;
        return si;
    }

    // ���ô��ڿ��
    inline bool SetSize(const LPSIZE psi) {
        if (!psi) return false;
        return SetWindowPos(GetStatic(), 0, 0, 0, psi->cx, psi->cy, SWP_NOOWNERZORDER | SWP_NOMOVE);
    }

    // ���ô������
    inline bool SetLeft(int left) {
        RECT rc = { 0 };
        ::GetWindowRect(GetStatic(), &rc);
        return SetWindowPos(GetStatic(), 0, left, rc.top, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE);

    }

    // ���ô��ڶ���
    inline bool SetTop(int top) {
        RECT rc = { 0 };
        ::GetWindowRect(GetStatic(), &rc);
        return SetWindowPos(GetStatic(), 0, rc.left, top, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE);
    }

    // ���ô��ڿ��
    inline bool SetWidth(int width) {
        RECT rc = { 0 };
        ::GetWindowRect(GetStatic(), &rc);
        return SetWindowPos(GetStatic(), 0, 0, 0, width, rc.bottom - rc.top, SWP_NOOWNERZORDER | SWP_NOMOVE);

    }

    // ���ô��ڸ߶�
    inline bool SetHeight(int height) {
        RECT rc = { 0 };
        ::GetWindowRect(GetStatic(), &rc);
        return SetWindowPos(GetStatic(), 0, 0, 0, rc.right - rc.left, height, SWP_NOOWNERZORDER | SWP_NOMOVE);
    }

    // ȡ���ڸ߶�
    inline int GetWidth() {
        RECT rc = { 0 };
        ::GetClientRect(GetStatic(), &rc);
        return rc.bottom - rc.top;
    }

    // ȡ���ڸ߶�
    inline int GetHeight() {
        RECT rc = { 0 };
        ::GetClientRect(GetStatic(), &rc);
        return rc.bottom - rc.top;
    }

    // ��ʾ����
    inline bool show() {
        return SetVisible(true);
    }

    // ���ش���
    inline bool hide() {
        return SetVisible(false);
    }

    // �ô��ڿ���״̬
    inline bool SetVisible(bool isVisible) {
        return ::ShowWindow(GetStatic(), isVisible ? SW_SHOW : SW_HIDE);
    }

    // ȡ���ڿ���״̬
    inline bool GetVisible() {
        return ::IsWindowVisible(GetStatic());
    }

    // ȡ���ڿ���״̬
    inline bool IsVisible() {
        return ::IsWindowVisible(GetStatic());
    }

    // �ô�������״̬, true = ����, false = ��ֹ
    inline bool SetEnabled(bool isEnable) {
        return ::EnableWindow(GetStatic(), isEnable);
    }

    // ȡ��������״̬, true = ����, false = ��ֹ
    inline bool GetEnabled() {
        return ::IsWindowEnabled(GetStatic());
    }
};
