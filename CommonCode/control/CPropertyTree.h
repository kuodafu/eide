#pragma once
#include "CPropertyTreeBase.h"

// ��Ŀ����ֵ
struct PROPERTY_ITEM_COMBOBOX
{
    wstr name;          // ������
    LPARAM param;       // ��Ŀ��ֵ
};
struct PROPERTY_ITEM : public IPROPERTY_ITEM
{
    std::vector<PROPERTY_ITEM>* comboboxData;   // ��Ͽ�����
    PROPERTY_ITEM() {
        index = -1;
        param = 0;
        comboboxData = 0;
        type = PROPERTY_TYPE_TEXT;
    }

    PROPERTY_ITEM(const PROPERTY_ITEM& obj) {
        param = obj.param;
        name = obj.name;
        value = obj.value;
        type = obj.type;
        index = obj.index;
        text = obj.text;
        comboboxData = 0;
        if (obj.comboboxData)
        {
            comboboxData = new std::vector<PROPERTY_ITEM>;
            comboboxData->assign(obj.comboboxData->begin(), obj.comboboxData->end());
        }
        return;
    }
    virtual LPCWSTR GetComboboxText(int index)
    {
        if (!comboboxData)return 0;
        int len = (int)comboboxData->size();
        if (index < 0 || index >= len)return 0;
        return comboboxData->at(index).name.c_str();
    }
    virtual LPARAM GetComboboxParam(int index)
    {
        if (!comboboxData)return 0;
        int len = (int)comboboxData->size();
        if (index < 0 || index >= len)return 0;
        return comboboxData->at(index).param;
    }

    // ��ȡ��Ͽ���������, ��Ҫ�ⲿ�ṩ������
    virtual int GetComboboxArray(IPROPERTY_ITEM** retData)
    {
        if (!comboboxData || !retData)return 0;
        int len = (int)comboboxData->size();
        if (index < 0 || index >= len)return 0;
        std::vector<PROPERTY_ITEM>& arr = *comboboxData;
        for (int i = 0; i < len; i++)
        {
            retData[i] = &(arr[i]);
        }
        return len;
    }
    // ��ȡ��ǰ��Ͽ���Ŀ��
    virtual int GetComboboxCount()
    {
        if (!comboboxData)return 0;
        return (int)comboboxData->size();
    }
    ~PROPERTY_ITEM() {
        if (this->comboboxData)
            delete this->comboboxData;
        name.Release();
        value.Release();
    }
    // ���ص�ǰ��Ͽ���Ŀ��
    inline size_t AddComboboxData(LPCWSTR name, LPARAM param) {
        if (!comboboxData)comboboxData = new std::vector<PROPERTY_ITEM>;
        PROPERTY_ITEM data;
        data.name = name;
        data.param = param;
        comboboxData->push_back(std::move(data));
        PROPERTY_ITEM& item = comboboxData->at(0);
        size_t len = comboboxData->size();
        if ((index == -1)     // ѡ������-1, ����ֵ
            || (item.name.empty() != value.empty()) // �������ֵ�ͽṹ�ﱣ���ֵ��һ��
            || (index == 0 && value != item.name))  // ����=0, ���ҽṹ���ֵ���������ֵ��һ��
        {
            index = 0;
            value = name;
        }
        return len;
    }
    inline bool DelComboboxData(int index) {
        if (!comboboxData)return false;
        int count = (int)comboboxData->size();
        if (index < 0 || index > count)return false;
        auto begin = comboboxData->begin();
        comboboxData->erase(comboboxData->begin() + index);
        //this->index--;
        //if (this->index < 0) this->index = 0;
        if ((size_t)this->index >= comboboxData->size())--this->index;
        if (this->index < 0) this->index = 0;

        return true;

    }
};


class CPropertyTree : public CTreeView
{
private:
    struct D2D_DRAW
    {
        LOGFONTW lf;                    // ��������, ����Ŀ�������Ӵ�
        LPEX_FONT hFont;                // �滭����
        HFONT hFontGdi;                 // gdi����, û�мӴ�
        LPEX_FONT hFontParent;          // ����Ŀ�滭����

        COLORREF crBackgroundGdi;       // gdi������ɫ, �༭��/��Ŀ����
        ARGB_D2D crBackground;          // ������ɫ
        LPEX_BRUSH hbrBackground;       // ��Ŀ�ı�����ɫ, hbrBackground
        HBRUSH hbrBackgroundGdi;        // �༭��/�б�� �ı�����ˢ, crBackgroundGdi

        COLORREF crBackParentGdi;       // ����Ŀ����߿հײ��ֵ���ɫ
        ARGB_D2D crBackParent;          // ����Ŀ����߿հײ��ֵ���ɫ
        LPEX_BRUSH hbrParent;           // ����Ŀ����߿հײ��ֵĻ�ˢ, crBackParent
        LPEX_PEN hpenParent;            // ����Ŀ����߿հײ��ֵĻ���, crBackParent

        COLORREF crTextGdi;             // gdi�ı���ɫ
        ARGB_D2D crText;                // �༭����ı���ɫ
        LPEX_BRUSH hbrText;             // �滭�ı���ˢ, �༭���ı���ɫ, crText

        COLORREF crListBorderGdi;       // ��Ͽ��б�߿���ɫ
        ARGB_D2D crListBorder;          // ��Ͽ��б�߿���ɫ
        LPEX_PEN hpenListBorder;      // ��Ͽ��б�߿򻭱�, crListBorder
        HPEN hpenListBorderGdi;         // ��Ͽ��б�߿���ɫ, crListBorderGdi

        COLORREF crSelectGdi;           // �ұ߰�ť�ı�����ɫ
        ARGB_D2D crSelect;              // ѡ����ɫ, ���Ϳ�ѡ��/�ұ߰�ť ����
        LPEX_BRUSH hbrSelect;           // ��Ŀ��ѡ����ɫ, crSelect
        HBRUSH hbrSelectGdi;            // ��Ŀ��ѡ����ɫ, gdi��ˢ

        COLORREF crDisabledGdi;         // ��ֹ�ı���ɫGDI��ɫ
        ARGB_D2D crDisabled;            // ��ֹ�ı���ɫD2D��ɫ
        LPEX_BRUSH hbrDisabled;         // ��ֹ�ı���ɫD2D��ˢ

        float chWidth;                  // �ַ�i�Ŀ��
        float chHeight;                 // �ַ�i�Ŀ��
        D2D_DRAW() {
            memset(&lf, 0, sizeof(lf));
            hFont = 0; hFontParent = 0; hFontGdi = 0;
            crBackgroundGdi = 0; hbrBackground = 0; hbrBackgroundGdi = 0;
            crBackParentGdi = 0; hbrParent = 0; hpenParent = 0;
            crTextGdi = 0; hbrText = 0;
            crListBorderGdi = 0; hpenListBorder = 0; hpenListBorderGdi = 0;
            crSelectGdi = 0; hbrSelect = 0; hbrSelectGdi = 0;
            crDisabledGdi = 0; hbrDisabled = 0;
        }
    };
    struct PROPERTY_EVENT
    {
        fnPropertyChanged pfnPropertyChanged;
        void* pfnPropertyChangedParam;

        fnItemChanged pfnItemChanged;
        void* pfnItemChangedParam;

        fnPropertyReturn pfnPropertyReturn;
        void* pfnPropertyReturnParam;

        fnComboboxMouseWheel pfnComboboxMouseWheel;
        void* pfnComboboxMouseWheelParam;

        fnComboboxChanged pfnComboboxChanged;
        void* pfnComboboxChangedParam;

        fnDropWown pfnDropWown;
        void* pfnDropWownParam;

        fnItemButtonClick pfnItemButtonClick;
        void* pfnItemButtonClickParam;

        fnItemRButtonClick pfnItemRButtonClick;
        void* pfnItemRButtonClickParam;

        fnComboboxLDoubleClick pfnComboboxLDoubleClick;
        void* pfnComboboxLDoubleClickParam;

        fnEditLDoubleClick pfnEditLDoubleClick;
        void* pfnEditLDoubleClickParam;

        fnItemChar pfnItemChar;
        void* pfnItemCharParam;

        fnItemMakeCustomValue pfnItemMakeCustomValue;
        void* pfnItemMakeCustomValueParam;


        fnItemRButtonPaint pfnItemRButtonPaint;
        void* pfnItemRButtonPaintParam;

        fnItemDelUp pfnItemDelUp;
        void* pfnItemDelUpParam;

        fnCustomValueName pfnCustomValueName;
        void* pfnCustomValueNameParam;

        fnItemReadError pfnItemReadError;
        void* pfnItemReadErrorParam;

        fnItemBeginDraw pfnItemBeginDraw;
        void* pfnItemBeginDrawParam;

        fnItemEndDraw pfnItemEndDraw;
        void* pfnItemEndDrawParam;
    };
    struct RIGHT_BUTTON_INFO
    {
        RECT rc;
        STATE state;
        int type;           // 0=...��ť, 1=������ť
        bool isVisible;
    };

    struct EXPAND_IMAGE_DATA
    {
        LPBYTE pOpen;
        int nOpen;
        LPEX_IMAGE imgOpen;
        LPBYTE pClose;
        int nClose;
        LPEX_IMAGE imgClose;
    };

    // ��¼������Դ����Ϣ, ѡ���, ��ѡ��, ��Щ��Դ��ʹ��һ������������
    struct PROPERTY_IMAGE_STRUCT
    {
        LPBYTE pData;
        int nSize;
        LPEX_IMAGE hImage;
    };

    static constexpr auto leftSize{ static_cast<int>(20) };     // ��߻滭���
    int m_itemHeight;               // ��Ŀ�߶�
    int m_offsetLeft;               // ���·ָ���ʱ��ƫ��
    int m_leftWidth;                // ����п�Ȱٷֱ�, 100-900, ����100������100, ����900������900
    int m_defColor;                 // Ĭ����ɫ����, 0=��ɫ, 1=��ɫ
    int m_oldMid;                   // �����м�ָ���ʱ�ָ��ߵ�λ��
    bool m_isCapture;               // �Ƿ񲶻������
    HCURSOR m_hCursor;              // ��ǰ���ָ��
    HTREEITEM m_hItemDown;          // ���µ���Ŀ���, û���ȵ���, ֻ�а��º�ѡ����
    HTREEITEM m_hItemSel;           // ѡ�е���Ŀ���, û���ȵ���, ֻ�а��º�ѡ����
    HTREEITEM m_hItemHot;           // �ȵ����Ŀ���, ����ƶ�ʱ��¼, 
    D2D_DRAW m_draw;
    bool m_isNoDblclk;              // �Ƿ�����˫��, ����˫��������Ϊfalse
    bool m_isFillColorBack;         // �Ƿ������ɫѡ��������
    int m_editCaret;                // �༭�������
    int m_midLineHeight;            // �м������ߵĸ߶�
    PROPERTY_EVENT m_evt;           // �ҽӵ��¼�

    CEdit m_edit;                   // �����ı��ı༭��, ��Ͽ���ں��滭��һ��������ť
    CStatic m_static;               // �ұߵİ�ť
    CListBox m_listBox;             // ��Ͽ�ר���б�, ���������ť�󵯳�����б�

    std::map<HTREEITEM, PROPERTY_ITEM> m_items;
    LPCANVAS m_hCanvas;
    RECT m_rcCalcText;              // �༭���ı�ƫ��
    RIGHT_BUTTON_INFO m_rbutton;    // �ұ߰�ť����
    EXPAND_IMAGE_DATA m_expand;     // ����չ����ťͼ������

    struct OPENDLG_DATA
    {
        wstr filter1;               // �ļ�������, ��¼�ⲿ���ô��ݽ�����ֵ
        wstr filter;                // �ļ�������, ��Ҫ�ڴ򿪶Ի���ǰ����, ����ǰ� | ������ 0
        wstr title;                 // �Ի������
        wstr defName;               // Ĭ���ļ���
        wstr path;                  // Ĭ��·��
        bool noChangePath;          // ���ı�Ŀ¼
    };


    std::map<PROPERTY_TYPE, OPENDLG_DATA> m_map_openDlg;  // ����Ի���������Ϣ, �����ͷ���
    std::map<PROPERTY_ICONTYPE, LPEX_IMAGE> m_map_icon;   // ����PROPERTY_ICONTYPE���ͻ�ȡ��ͼ��

public:
    CPropertyTree();
    ~CPropertyTree();
    virtual inline LPCWSTR className() { return L"JY_PropertyBox"; }
    virtual void SetItemHeight(int height, bool isUpdate = false);
    virtual int GetItemHeight() { return m_itemHeight; }

private:
    inline bool query(int type, int n)
    {
        if (type == n)
            return true;
        if (type == PROPERTY_TYPE_UNKNOWN)
            return false;
        return (type & n) == n;
    }
    inline bool query_lobyte(int type, int n)
    {
        if (type == n)
            return true;
        if (type == PROPERTY_TYPE_UNKNOWN)
            return false;
        return (LOBYTE(type) == n);
    }

public:
    virtual bool SetIcon(PROPERTY_ICONTYPE type, LPCVOID pIcon, int size);
    virtual LPEX_IMAGE GetIcon(PROPERTY_ICONTYPE type);
    // �Ƿ�Ϊ����
    inline bool IsGroup(PROPERTY_TYPE type) { return LOWORD(type) == 0; }
    // �Ƿ��а�ť
    inline bool IsButton(PROPERTY_TYPE type) { return query(type, PROPERTY_FLAG_BUTTON); }
    // �Ƿ���������ť
    inline bool IsButtonCombobox(PROPERTY_TYPE type) { return query(type, PROPERTY_FLAG_BUTTON_COMBOBOX); }
    // �Ƿ�Ϊ��ֵ
    inline bool IsNumber(PROPERTY_TYPE type) { return query_lobyte(type, PROPERTY_TYPE_NUMBER); }
    // �Ƿ�Ϊ�ı�, ���ı�, ����ť�ı�������true
    inline bool IsText(PROPERTY_TYPE type) { return query_lobyte(type, PROPERTY_TYPE_TEXT); }

    // ����Ϊ��չ����, �ɼ�����ʽ��϶���

    // �Ƿ�Ϊ����ť�ı�
    inline bool IsTextButton(PROPERTY_TYPE type)            { return type == PROPERTY_TYPE_TEXT_BUTTON; }
    // �Ƿ�Ϊ���ı�
    inline bool IsLongText(PROPERTY_TYPE type)              { return type == PROPERTY_TYPE_TEXT_LONG; }
    // �Ƿ�Ϊ���ɱ༭��Ͽ�
    inline bool IsComboboxNoEdit(PROPERTY_TYPE type)        { return type == PROPERTY_TYPE_COMBOBOX_NOEDIT; }
    // �Ƿ�Ϊ���ɱ༭��Ͽ�
    inline bool IsComboboxNoEditIndex(PROPERTY_TYPE type)   { return type == PROPERTY_TYPE_COMBOBOX_NOEDIT_INDEX; }
    // �Ƿ�Ϊ��Ͽ�, ���ɱ༭�ʹ���ŵĶ�������, ��Ҫ�ж϶�Ӧ�ľ�ʹ�ö�Ӧ�ķ���
    inline bool IsCombobox(PROPERTY_TYPE type)              { return query_lobyte(type, PROPERTY_FLAG_COMBOBOX); }
    // �Ƿ�Ϊ��ɫѡ����
    inline bool IsColorDialog(PROPERTY_TYPE type)           { return query_lobyte(type, PROPERTY_FLAG_COLOR); }
    // �Ƿ�Ϊ�ļ�ѡ����
    inline bool IsFileDialog(PROPERTY_TYPE type)            { return query_lobyte(type, PROPERTY_FLAG_FILE); }
    // �Ƿ�Ϊ����ѡ����
    inline bool IsFontDialog(PROPERTY_TYPE type)            { return query_lobyte(type, PROPERTY_FLAG_FONT); }
    // �Ƿ�Ϊ�ֽڼ�
    inline bool IsBinDialog(PROPERTY_TYPE type)             { return query_lobyte(type, PROPERTY_FLAG_BIN); }
    // �Ƿ�ΪͼƬѡ����
    inline bool IsPicDialog(PROPERTY_TYPE type)             { return query_lobyte(type, PROPERTY_FLAG_PIC); }
    // �Ƿ�Ϊ��Ƶѡ����
    inline bool IsMusicDialog(PROPERTY_TYPE type)           { return query_lobyte(type, PROPERTY_FLAG_MUSIC); }
    // �Ƿ�ΪͼƬ��ѡ����
    inline bool IsImgListDialog(PROPERTY_TYPE type)         { return query_lobyte(type, PROPERTY_FLAG_IMGLIST); }
    // �Ƿ�Ϊ���ӿ�
    inline bool IsLink(PROPERTY_TYPE type)                  { return query_lobyte(type, PROPERTY_FLAG_LINK); }
    // �Ƿ�Ϊѡ���
    inline bool IsCheckBox(PROPERTY_TYPE type)              { return query_lobyte(type, PROPERTY_FLAG_CHECKBOX); }
    // �Ƿ�Ϊ3̬ѡ���
    inline bool IsCheckBox3(PROPERTY_TYPE type)             { return query_lobyte(type, PROPERTY_FLAG_CHECKBOX3); }
    // �Ƿ�Ϊ��ѡ��
    inline bool IsRadio(PROPERTY_TYPE type)                 { return query_lobyte(type, PROPERTY_FLAG_RADIO); }
    // �Ƿ�Ϊ�Զ���ѡ��
    inline bool IsAutoRadio(PROPERTY_TYPE type)             { return query_lobyte(type, PROPERTY_FLAG_AUTORADIO); }
    // �Ƿ�Ϊ����
    inline bool IsSwitch(PROPERTY_TYPE type)                { return query_lobyte(type, PROPERTY_FLAG_SWITCH); }

    // �Ƿ�Ϊѡ���������, ѡ���/3̬ѡ���, ��ѡ��, �Զ���ѡ��, ����, ������ѡ������
    inline bool IsCheckType(PROPERTY_TYPE type) { return IsCheckBox(type) || IsCheckBox3(type) || IsRadio(type) || IsAutoRadio(type) || IsSwitch(type); }


    // 1=�ֽ�,2=������,3=����,4=������,5=С��,6=˫����С��, ���᷵������ֵ
    inline int  GetNumberType(PROPERTY_TYPE type)           { int ret = HIBYTE(type & ~PROPERTY_FLAG_UNSIGNED); return (ret == 0 || ret > 6) ? 6 : ret; }
    // �Ƿ��з���
    inline bool IsUnsigned(PROPERTY_TYPE type)              { return query(LOWORD(type), PROPERTY_FLAG_UNSIGNED); }
    // �Ƿ�ɱ༭
    inline bool IsEdit(PROPERTY_TYPE type)                  { return !query(type, PROPERTY_FLAG_NOEDIT); }
    // �Ƿ��б��
    inline bool IsIndex(PROPERTY_TYPE type)                 { return query(type, PROPERTY_FLAG_INDEX); }
    // �Ƿ��Զ�������
    inline bool IsCustom(PROPERTY_TYPE type)                { return query(type, PROPERTY_FLAG_CUSTOM); }

    // �Ƿ��ֹ
    inline bool IsDisabled(PROPERTY_TYPE type)              { return query(type, PROPERTY_FLAG_DISABLED); }
    // �Ƿ����
    inline bool IsEnabled(PROPERTY_TYPE type)               { return !IsDisabled(type); }
    // ѡ����������ѡ��, û�����״̬���ͼ��֮��ĵط�������ѡ��
    inline bool IsCheckLineSel(PROPERTY_TYPE type)          { return query(type, PROPERTY_FLAG_CHECKLINESEL); }

    // ����Ӧ������Ϣ, ���Ұ���Ϣת����������
    inline bool IsNoMouseWheel(PROPERTY_TYPE type)          { return query(type, PROPERTY_FLAG_NOMOUSEWHEEL); }


public:
    // �ⲿ��������໯֮����Ե������������Ĭ�ϴ���
    virtual LRESULT WndProc(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    // ����������ұߵİ�ť��, ������ڰ�ť���򷵻�true
    bool OnMouseMove(TVHITTESTINFO& info);
    void modify_value_from_edit();

    void InvalidateItem(HTREEITEM hItem);
    void CreateD2DResouce();
    void __create_d2d_resouce();
    void __delete_d2d_resouce(bool isDestroyCanvas);

    virtual LRESULT OnSize(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnPaint(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void    DrawItem(LPCANVAS hCanvas, HTREEITEM hItem, RECT& rc, int index, int scrollPos, int cxClient, int cyClient);
    void    DrawLink(LPCANVAS hCanvas, IPROPERTY_ITEM& data, HTREEITEM hItem, RECT_F& rc, int index, int scrollPos, int cxClient, int cyClient);
    void    DrawCheckBox_Radio(LPCANVAS hCanvas, IPROPERTY_ITEM& data, HTREEITEM hItem, RECT_F& rc, int index, int scrollPos, int cxClient, int cyClient);
    virtual LRESULT OnPaintListBox(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnListCommand(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual void PaintStatic(RECT& rcItem, IPROPERTY_ITEM& data, HDC hdc);
    virtual LRESULT OnEditProc(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT OnPaintStatic(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


    // �������¼�, pos = �������븺�����¹���, �������Ϲ���, key = ����ʱ�������ĸ�����, lParam = �������
    virtual bool OnEditMouseWheel(short pos, short key, LPARAM lParam);

    // �༭���Ҽ�����
    virtual bool OnEditRButtonDown(int mk, LPARAM pt);

    // �������, hItem=�ܵ����ı�����, col=����������, lParam����
    virtual void OnItemClick(HTREEITEM hItem, int col, LPARAM lParam);
    // �༭���»س�
    void OnEditReturn();
    // �༭���������
    void OnEditLDown(int key, DWORD pt);
    // �༭��˫��
    void OnEditDblClk();
    bool OnEditNcCalcSize(WPARAM wParam, LPARAM lParam);

    inline void OnItemPropChanged(HTREEITEM hItem, PROPERTY_ITEM& data) {
        if (m_evt.pfnPropertyChanged)
            m_evt.pfnPropertyChanged(m_evt.pfnPropertyChangedParam, hItem, data.type);
    }

    // �༭������, ���������ַ�, �����Ƿ���������, true = ��������, false = ������
    bool __Edit_insert_char(int ch, bool bUnsigned, bool isDouble, const DWORD start, const DWORD end, wstr& text, wstr& insertText);

    // ���Ʊ༭�����������Сֵ
    bool __Edit_insert_char_min_max(int ch, bool bUnsigned, double min, double max, double umax, LPCWSTR minstr, LPCWSTR maxstr, LPCWSTR umaxstr);
    // �ַ�����, �����Ƿ�����, �������true, ���ڲ���ʹ�� pRet ��ֵ��Ϊ����ֵ
    bool OnEditChar(int ch, int* pRet);
    // �༭����ʾ����
    void OnEditShow(bool isShow);
    // �༭���ȡ����
    void OnEditSetFocus();
    // �༭��ʧȥ����
    void OnEditKillFocus();
    // �б��ʧȥ����
    void OnListBoxKillFocus();
    // ��Ͽ���Ŀ��ѡ��
    void OnComboboxChanged(int index);

    // ɾ��������
    bool OnItemDelDown();
    // ɾ�����ſ�, isName = �Ƿ�����������һ�а��°���
    void OnItemDelUp(bool isName);

    // �ƶ��༭��,���ұߵİ�ť
    // TODO ���޸�, ����������ʾ�ĸ�
    void moveEdit_Static(HTREEITEM hItem);

    // ���Եİ�ť������, ��ť����, ��Ŀ����ǵ�ǰ��ѡ�е���Ŀ
    virtual void OnValueButtonClick(int type);

    virtual void CalcListRect(PROPERTY_ITEM& data, std::vector<PROPERTY_ITEM>& arr, const int len, RECT& ret);

    virtual int CalcListWidth(PROPERTY_ITEM& data, std::vector<PROPERTY_ITEM>& arr, const int len, const int maxWidth, int width, const int cxClient, const int cyClient);


    // �����м������ߵ�λ��
    inline int calcMidLine(int cxClient = 0) {
        if (cxClient <= 0) {
            RECT rc;
            GetClientRect(&rc);
            cxClient = rc.right - rc.left;
        }
        int width = cxClient * m_leftWidth / 1000;
        return width;
    }

    // �����Զ����������ı�, ���ݲ�ͬ���ͷ��ض�Ӧ���ı�, ���е��Զ��嶼Ӧ�ý������ﴦ��
    wstr MakeCustomText(IPROPERTY_ITEM& data, HTREEITEM hItem);

    // �����б��λ��, 
    // rc = ���ռ���Ľ��
    // isMove = Ϊtrue�������Ϻ��ƶ�����
    bool calcListBoxRect(RECT* rc, bool isMove, int mid, const RECT& selItemRect, const RECT& treeClientRect);
    LRESULT OnVScroll(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


public:
    virtual HWND create(DWORD dwExStyle, LPCWSTR lpszTitle, DWORD dwStyle, DWORD dwExStyleTree,
        int x, int y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR ID = 0, LPVOID lpParam = 0);

public:
    // д���ֽڼ�, ��������ָ��ͳ���, �ڲ���ת�������Եĸ�ʽ
    inline bool SetBinData(IPROPERTY_ITEM& data, const void* pData, const size_t size) {
        if (!pData || !size)return false;
        data.value.resize(size / 2 + 8);
        int* ptr = (int*)data.value.data();

        ptr[0] = 1;
        ptr[1] = (int)(LONG_PTR)size;
        memcpy(&ptr[2], pData, size);
        return true;
    }

    // д���ֽڼ�, �������ݱ����������Եĸ�ʽ
    inline bool SetBinData(IPROPERTY_ITEM& data, const void* pData) {
        if (!pData || ((int*)pData)[0] != 1)
            return false;
        const int size = ((int*)pData)[1];
        const int reSize = size / 2 * 8;
        data.value.resize(reSize);
        LPWSTR ptr = data.value.data();
        const int cpySize = size + 8;
        memcpy(ptr, pData, cpySize);
        return true;
    }

    // д������, �����ŵ����ݴ����ǹ̶���, �����ṹ, LOGFONTW + CHOOSEFONTW, �ڲ����޸������С/�ַ��߶�
    inline bool SetFontData(IPROPERTY_ITEM& data, const LPLOGFONTW lf) {
        if (!lf)return false;
        data.value.resize((sizeof(LOGFONTW) + sizeof(CHOOSEFONTW)) / 2 + 8);

        LPLOGFONTW ptr = (LPLOGFONTW)data.value.data();
        LPCHOOSEFONTW cf = (LPCHOOSEFONTW)(ptr + 1);
        memcpy(ptr, lf, sizeof(LOGFONTW));

        cf->lpLogFont = (LPLOGFONTW)ptr;
        cf->iPointSize = _font_height2size(ptr->lfHeight) * 10;
        return true;
    }

    // д������, ����CHOOSEFONTW�ṹָ��, �ڲ����޸������С/�ַ��߶�
    // CHOOSEFONTW ָ��� LOGFONT�ṹ��ֵ�Ļ�Ҳ�������ṹ������������
    inline bool SetFontData(IPROPERTY_ITEM& data, const LPCHOOSEFONTW pcf) {
        if (!pcf)return false;
        data.value.resize((sizeof(LOGFONTW) + sizeof(CHOOSEFONTW)) / 2 + 8);

        LPLOGFONTW ptr = (LPLOGFONTW)data.value.data();
        LPCHOOSEFONTW cf = (LPCHOOSEFONTW)(ptr + 1);
        memcpy(cf, pcf, sizeof(CHOOSEFONTW));
        ptr->lfHeight = _font_size2height(cf->iPointSize / 10);
        if (cf->lpLogFont)
        {
            memcpy(ptr, cf->lpLogFont, sizeof(LOGFONTW));
        }

        cf->lpLogFont = (LPLOGFONTW)ptr;
        return true;
    }

    // д������, ����gdi����ָ��, �ڲ����޸������С/�ַ��߶�
    inline bool SetFontData(IPROPERTY_ITEM& data, HFONT hFont) {
        if (!hFont)return false;
        data.value.resize((sizeof(LOGFONTW) + sizeof(CHOOSEFONTW)) / 2 + 8);

        LPLOGFONTW ptr = (LPLOGFONTW)data.value.data();
        LPCHOOSEFONTW cf = (LPCHOOSEFONTW)(ptr + 1);
        GetObjectW(hFont, sizeof(LOGFONTW), ptr);

        cf->lpLogFont = (LPLOGFONTW)ptr;
        cf->iPointSize = _font_height2size(ptr->lfHeight) * 10;
        return true;
    }


    // д������, �����ŵ����ݴ����ǹ̶���, �����ṹ, LOGFONTW + CHOOSEFONTW
    inline bool SetFontData(IPROPERTY_ITEM& data, const wchar_t* name = L"΢���ź�", int lfHeight = -12,
        bool lfWeight = false, bool lfItalic = false, bool lfUnderline = false, bool lfStrikeOut = false) {

        data.value.resize((sizeof(LOGFONTW) + sizeof(CHOOSEFONTW)) / 2 + 8);

        LPLOGFONTW lf = (LPLOGFONTW)data.value.data();
        LPCHOOSEFONTW cf = (LPCHOOSEFONTW)(lf + 1);

        if (name)
        {
            size_t len = wcslen(name) + 1;
            if (len > 32)len = 32;
            memcpy(lf->lfFaceName, name, len * sizeof(wchar_t));
        }

        lf->lfHeight = lfHeight;
        lf->lfWeight = lfWeight ? FW_BOLD : FW_NORMAL;
        lf->lfItalic = lfItalic;
        lf->lfUnderline = lfUnderline;
        lf->lfStrikeOut = lfStrikeOut;

        cf->lpLogFont = (LPLOGFONTW)lf;
        cf->iPointSize = _font_height2size(lf->lfHeight) * 10;
        return true;
    }

    // ��ȡ��������, �ڲ��и��� LPCHOOSEFONTW �ṹָ���logfont��ַ
    inline LPLOGFONTW GetFontData(IPROPERTY_ITEM& data, LPCHOOSEFONTW* pcf) {
        LPLOGFONTW lf = (LPLOGFONTW)data.value.data();
        if (pcf)*pcf = 0;
        if (lf)
        {
            LPCHOOSEFONTW cf = (LPCHOOSEFONTW)(lf + 1);
            cf->lpLogFont = lf;
            if (pcf) *pcf = cf;
        }
        return lf;
    }

    // ������Ŀ�����ȡ��Ŀ�����Ŀ������ָ��
    inline IPROPERTY_ITEM* GetItemData(HTREEITEM hItem) {
        if (!hItem)return 0;
        auto f = m_items.find(hItem);
        if (f == m_items.end()) return 0;
        PROPERTY_ITEM& data = f->second;
        return &data;
    }
    // ȡ����ѡ����
    inline HTREEITEM GetIndex() {
        return m_hItemSel;
    }

    // ���öԻ�������, Ϊ0�����ö�Ӧ������
    // type = Ҫ���õĶԻ�������, Ŀǰֻ֧�� PROPERTY_FLAG_FILE�ļ�ѡ����, PROPERTY_FLAG_BIN�ֽڼ�ѡ����
    // filter = ������, ��ʽ����MP3�ļ�(*.mp3)|*.mp3\0ý���ļ�(*.mpg)|*.mpg��
    // defName = Ĭ���ļ���
    // title = ���ڱ���
    // path = ����Ŀ¼
    // notChengedPath = ���ı�Ŀ¼, ��Ҫ���ݱ���ָ��, ָ��Ϊ0������
    bool SetOpenDialogProperty(PROPERTY_TYPE type, LPCWSTR filter = 0, LPCWSTR title = 0, LPCWSTR defName = 0, LPCWSTR path = 0, bool* noChangePath = 0);

    // ��ȡ�Ի�������, Ϊ0�򲻻�ȡ��Ӧ������
    // type = Ҫ���õĶԻ�������, Ŀǰֻ֧�� PROPERTY_FLAG_FILE�ļ�ѡ����, PROPERTY_FLAG_BIN�ֽڼ�ѡ����
    // filter = ������, ��ʽ����MP3�ļ�(*.mp3)|*.mp3\0ý���ļ�(*.mpg)|*.mpg��
    // defName = Ĭ���ļ���
    // title = ���ڱ���
    // path = ����Ŀ¼
    // notChengedPath = ���ı�Ŀ¼
    bool GetOpenDialogProperty(PROPERTY_TYPE type, LPCWSTR* filter = 0, LPCWSTR* title = 0, LPCWSTR* defName = 0, LPCWSTR* path = 0, bool* noChangePath = 0);

public:
    // ����һ������, ���������Ŀ�ľ��
    HTREEITEM InsertGroup(LPCWSTR name, bool isExpand, LPCWSTR text = 0, LPARAM param = 0);

    // ����ָ����Ŀ��ֵ, �ֽڼ����ͺ�������һ��
    bool SetItemValue(IPROPERTY_ITEM& data, const void* value);

    // ����˵���ı�
    bool SetItemRemarks(HTREEITEM hItem, LPCWSTR value);
    // ��ȡ˵���ı�
    LPCWSTR GetItemRemarks(HTREEITEM hItem);

    // ����һ���ı�����������, ����ֵֻ�����ı�
    // hParent = InsertGroup() ���ص�ֵ, ��Ҫ���뵽�ĸ�������
    // name = ������, ���������к����޸�
    // value = ����ֵ
    // text = ��ע�ı�
    // param = ��Ŀ��ֵ
    HTREEITEM InsertItem(HTREEITEM hParent, LPCWSTR name, LPCWSTR value = 0, PROPERTY_TYPE type = PROPERTY_TYPE_TEXT, LPCWSTR text = 0, LPARAM param = 0);

    // ɾ��ָ����Ŀ
    BOOL DeleteItem(HTREEITEM hItem);

    // ȡָ����Ŀ��������
    LPCWSTR GetItemName(HTREEITEM hItem);
    bool SetItemName(HTREEITEM hItem, LPCWSTR name);

    // ȡָ����Ŀ������ֵ, ʧ�ܷ���0, �ֽڼ�������Ҫ��ǰ����� {1,0,0,0,����} 8���ֽ�, ��������һ��
    LPCWSTR GetItemValue(HTREEITEM hItem);

    // ����ָ����Ŀ��ֵ, �ֽڼ����ͺ�������һ��
    bool SetItemValue(HTREEITEM hItem, LPCWSTR value);

    // ȡָ����Ŀ����Ŀ��ֵ, InsertItem/SetItemValue ʱ��д��ֵ
    LPARAM GetItemParam(HTREEITEM hItem);

    // ����ָ����Ŀ��ֵ
    bool SetItemParam(HTREEITEM hItem, LPARAM param);

    // ����Ͽ���Ŀ������Ŀ����, ���ú������Զ������Ͽ�����
    // ���óɹ����ر������Ͽ���Ŀ��, ʧ�ܷ���-1
    size_t AddComboboxData(HTREEITEM hItem, LPCWSTR name, LPARAM param = 0);

    // ɾ����Ͽ�����, index = ɾ�����±�, ��0��ʼ, �±�Խ��/������Ͽ����ͷ���false
    bool DelComboboxData(HTREEITEM hItem, int index);

    // ȡָ����Ŀ����Ͽ���Ŀ��, ʧ�ܷ���-1, ������Ͽ����ͻ�����Ŀ�����Ч������-1
    size_t GetComboboxDataCount(HTREEITEM hItem);

    // ȡָ����Ŀ����Ͽ���Ŀ�ı�, ʧ�ܷ��ؿ��ı�, ������Ͽ�����, �±�Խ��, �����Ч�����ؿ��ı�
    LPCWSTR GetComboboxData(HTREEITEM hItem, int index);

    // ����ָ����Ŀ����Ͽ���Ŀ�ı�, ʧ�ܷ���false, ������Ͽ�����, �±�Խ��, �����Ч������false
    bool SetComboboxData(HTREEITEM hItem, int index, LPCWSTR value);

    // ��ȡ��Ͽ�����ѡ����, ʧ�ܷ���-1
    int GetComboboxIndex(HTREEITEM hItem);

    // ������Ͽ�����ѡ����, ������0��ʼ
    bool SetComboboxIndex(HTREEITEM hItem, int index);

    // ȡָ����Ŀ����Ͽ���Ŀ��ֵ
    LPARAM GetComboboxParam(HTREEITEM hItem, int index);

    // ����ָ����Ŀ����Ͽ���Ŀ��ֵ
    bool SetComboboxParam(HTREEITEM hItem, int index, LPARAM param);

    // ����ָ����ĿΪָ������, ��Ͽ�������Ҫ��������
    void SetItemType(HTREEITEM hItem, PROPERTY_TYPE type);

    // ȡָ����Ŀ����
    PROPERTY_TYPE GetItemType(HTREEITEM hItem);

    // ���ÿ��, ǧ�ֱ�, ��С100, ���900, �ؼ�δ����ǰҲ���Ե���
    // isLeft = �Ƿ�Ϊ�������, ����һ�ߺ���һ�߻�һ�����
    // value = 100-900֮���ֵ
    // isUpdate = �Ƿ��ػ�
    void SetWidthRatio(bool isLeft, int value, bool isUpdate = false);

    // ����ǰ�����������ݱ�����ֽڼ�����, ���صĵ�ַ��ʹ��ʱ��Ҫ���� free() �ͷ�
    // isColor = �Ƿ񱣴���ɫ������Ϣ
    LPBYTE save(DWORD* size, bool isColor, bool isList);

    void Load_Color(LIST_PROP_HEAD* head, LPBYTE& pData, bool isUpdate);
    // pData ���봫�ݶ�ȡ�б�֮��ĵ�ַ
    void Load_Expand(LIST_PROP_HEAD* head, LPBYTE& pData);
    // ���ڴ������������Ŀ����
    bool load(const void* data, DWORD size, bool isUpdate);

    // �Ӽ��ص���������ӵ����Ϳ���
    bool Load_Insert_Tree(LIST_PROP_HEAD* head, LPBYTE& pData);

    // ������ɫ��Ϣ
    void SetColor(const PROPERTY_COLOR* color, bool isUpdate);
    int GetColor(PROPERTY_COLOR* color);
    // Ĭ����ɫ, 0=��ɫ���, 1=��ɫ���
    int DefColor(int type, bool isUpdate);

    int SetItemFont(const LPLOGFONTW lf, bool isUpdate);
    // Ĭ��Ϊ΢���ź�, �����С -12, û����ʽ
    // lfItalic = б��,  lfUnderline = �»���, lfStrikeOut = ɾ����
    int SetItemFont(bool isUpdate, const wchar_t* name = L"΢���ź�", int size = -12, bool lfItalic = false, bool lfUnderline = false, bool lfStrikeOut = false);

    int SetColorBackground(ARGB cr, bool isUpdate = false);
    int SetColorBackParent(ARGB cr, bool isUpdate = false);
    int SetColorText(ARGB cr, bool isUpdate = false);
    int SetColorListBorder(ARGB cr, bool isUpdate = false);
    int SetColorSelect(ARGB cr, bool isUpdate = false);
    int SetColorDisabled(ARGB cr, bool isUpdate = false);
    inline void SetIsFillColorBack(bool isFill) { m_isFillColorBack = isFill; InvalidateRect(); }

    inline bool SetItemDisabledState(HTREEITEM hItem, bool isDisabled) { return isDisabled ? DisabledItem(hItem) : EnabledItem(hItem); }
    // ��ֹ��Ŀ
    bool DisabledItem(HTREEITEM hItem);
    // ������Ŀ
    bool EnabledItem(HTREEITEM hItem);
    // ��Ŀ�Ƿ��ֹ
    bool IsItemDisabled(HTREEITEM hItem);
    // ��Ŀ�Ƿ�����
    bool IsItemEnabled(HTREEITEM hItem);

    inline bool clear() {
        m_items.clear();
        m_listBox.hide();
        m_edit.hide();
        m_rbutton.isVisible = false;
        InvalidateRect(&m_rbutton.rc);
        return TreeView_DeleteItem(m_hWnd, TVI_ROOT);
    }

    // �����Ի���, ���ݵ�ǰ��Ŀ����̽���Ӧ�Ի���, �ᴥ����ť����ص�
    inline void popDialog() {
        IPROPERTY_ITEM* data = GetItemData(m_hItemSel);
        if (!data)return;
        const bool isBtn = IsButton(data->type);
        const bool isCbb = IsCombobox(data->type);
        if (!isBtn && !isCbb)return;
        const int type = isCbb ? 1 : (isBtn ? 0 : 0);   // ������ʱֻ����Ͽ�Ͱ�ť
        OnValueButtonClick(type);
    }

    // ��ȡ���Կ����
    void GetPropertyItemRect(HTREEITEM hItem, RECT* prc, PROPERTY_RECT type);

    LPBYTE GetExpand_Open(int* size);
    LPBYTE GetExpand_Close(int* size);
    bool SetExpand_Open(const void* pData, int size);
    bool SetExpand_Close(const void* pData, int size);
private:
    // �����б�����
    _str make_listdata();

public:


    // ��Ŀ���Ա��ı�, Ϊ����ȡ���ҽ�����¼�, �¼���������, ����1�Ǵ����¼�����Ŀ���, ����2����Ŀ������
    inline void OnPropertyChanged(fnPropertyChanged pfn = 0, void* param = 0) {
        m_evt.pfnPropertyChanged = pfn;
        m_evt.pfnPropertyChangedParam = param;
    }

    // ��Ŀѡ��ı�, Ϊ����ȡ���ҽ�����¼�, �¼���������, ����1�Ǵ����¼�����Ŀ���, ����2�����ĸ�λ�õ��, -1=չ��������ť, 1=������, 2=����ֵ
    inline void OnItemChanged(fnItemChanged pfn = 0, void* param = 0) {
        m_evt.pfnItemChanged = pfn;
        m_evt.pfnItemChangedParam = param;
    }

    // ��Ŀ����ֵ�����˻س���, �ص����������Ƿ�����, Ϊ����ȡ���ҽ�����¼�, ����ص���������true�򲻶԰��»س�������һ������
    // �¼���������, ����1�Ǵ����¼�����Ŀ���, ����2����Ŀ������
    inline void OnPropertyReturn(fnPropertyReturn pfn = 0, void* param = 0) {
        m_evt.pfnPropertyReturn = pfn;
    }

    // ��Ͽ���Ŀ������, �ص����������Ƿ�����, ����ص���������true�򲻶�����������һ������
    // count = ��Ͽ���Ŀ����, pos = �������븺�����¹���, �������Ϲ���, key = ����ʱ�������ĸ�����, lParam = �������
    inline void OnComboboxMouseWheel(fnComboboxMouseWheel pfn = 0, void* param = 0) {
        m_evt.pfnComboboxMouseWheel = pfn;
        m_evt.pfnComboboxMouseWheelParam = param;
    }

    // ��Ͽ���Ŀ�ı�, �ص����������Ƿ�����, ����ص���������true�򲻻�ı���Ŀ
    // index = Ҫ�ı����Ŀ����, ����false����Ͽ�����ѡ��������������
    inline void OnComboboxChanged(fnComboboxChanged pfn = 0, void* param = 0) {
        m_evt.pfnComboboxChanged = pfn;
        m_evt.pfnComboboxChangedParam = param;
    }

    // ��Ͽ�������ͷ������, ����̽���б�, �ص����������Ƿ�����, ����ص���������true�򲻻ᵯ���б�
    inline void OnDropDown(fnDropWown pfn = 0, void* param = 0) {
        m_evt.pfnDropWown = pfn;
        m_evt.pfnDropWownParam = param;
    }

    // ���԰�ť������, �ص����������Ƿ�����, ����ص���������true�򲻻ᵯ�����õĶԻ���
    inline void OnItemButtonClick(fnItemButtonClick pfn = 0, void* param = 0) {
        m_evt.pfnItemButtonClick = pfn;
        m_evt.pfnItemButtonClickParam = param;
    }

    // ��Ŀ���Ҽ�����, �����Ƿ�����, ���غ��Ҽ�����ı����, ���ᴥ������
    inline void OnItemRButtonClick(fnItemRButtonClick pfn = 0, void* param = 0) {
        m_evt.pfnItemRButtonClick = pfn;
        m_evt.pfnItemRButtonClickParam = param;
    }

    // ��Ͽ�˫���༭��, �ص����������Ƿ�����, ����ص���������true�򲻶�˫������һ������
    // index = ˫��ʱ��Ͽ������, count = ��Ͽ���Ŀ��
    inline void OnComboboxLDoubleClick(fnComboboxLDoubleClick pfn = 0, void* param = 0) {
        m_evt.pfnComboboxLDoubleClick = pfn;
        m_evt.pfnComboboxLDoubleClickParam = param;
    }

    // �༭��˫��, �ص����������Ƿ�����, ����ص���������true��ȫѡ�༭��
    inline void OnEditLDoubleClick(fnEditLDoubleClick pfn = 0, void* param = 0) {
        m_evt.pfnEditLDoubleClick = pfn;
        m_evt.pfnEditLDoubleClickParam = param;
    }

    // ��Ŀ�����ַ�, �ص����������Ƿ�����, ����ص���������true��ʹ�� pRet ���ص�ֵ��Ϊ����ֵ
    // ch = ������ַ�, pRet = ����ֵָ��, ��Ҫ�ѷ���ֵд������
    inline void OnItemChar(fnItemChar pfn = 0, void* param = 0) {
        m_evt.pfnItemChar = pfn;
        m_evt.pfnItemCharParam = param;
    }

    // �����Զ�������ֵ����ʾ����, ��������, ��Ŀ���, ��Ŀ����, ��ʾ�ı�������, ���100���ַ�
    inline void OnItemMakeCustomValue(fnItemMakeCustomValue pfn = 0, void* param = 0) {
        m_evt.pfnItemMakeCustomValue = pfn;
        m_evt.pfnItemMakeCustomValueParam = param;
    }

    // �ұ�С��ť���ػ�, ����true���ڲ������л滭, ��Ҫ�û��ڻص�����л滭����
    // hdc = ��ť�滭dc, �����ڴ�DC, state = ��ǰ״̬, STATE::HOT, STATE::DOWN, width = ��ť���, height = ��ť�߶�
    inline void OnItemRButtonPaint(fnItemRButtonPaint pfn = 0, void* param = 0) {
        m_evt.pfnItemRButtonPaint = pfn;
        m_evt.pfnItemRButtonPaintParam = param;
    }

    // del���ſ�, ɾ����ǰ������, ����������һ�а��µ�Ĭ�ϴ��������ֵ, �������������µ�ֻ���˫����򿪶Ի������Ŀ
    // isName = �Ƿ�����������һ�а��°���
    inline void OnItemKeyDelUp(fnItemDelUp pfn = 0, void* param = 0) {
        m_evt.pfnItemDelUp = pfn;
        m_evt.pfnItemDelUpParam = param;
    }

    // �Զ�����������
    // isName = �Ƿ�����������һ�а��°���
    inline void OnCustomValueName(fnCustomValueName pfn = 0, void* param = 0) {
        m_evt.pfnCustomValueName = pfn;
        m_evt.pfnCustomValueNameParam = param;
    }

    // ��Ŀ��ȡ����ʧ��, �ֽڼ�, ͼƬ, ��Ƶ, ͼƬ����Щ....
    inline void OnItemReadError(fnItemReadError pfn = 0, void* param = 0) {
        m_evt.pfnItemReadError = pfn;
        m_evt.pfnItemReadErrorParam = param;
    }

    // �����滭, �ڲ��滭ǰ�ȵ����û��Ļ滭
    inline void OnItemBeginDraw(fnItemBeginDraw pfn = 0, void* param = 0) {
        m_evt.pfnItemBeginDraw = pfn;
        m_evt.pfnItemBeginDrawParam = param;
    }

    // �滭����, �ڲ��滭�Ѿ�����, �û�������������Ͻ��ж��λ滭
    inline void OnItemEndDraw(fnItemEndDraw pfn = 0, void* param = 0) {
        m_evt.pfnItemEndDraw = pfn;
        m_evt.pfnItemEndDrawParam = param;
    }

};

