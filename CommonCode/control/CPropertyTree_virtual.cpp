#include "CPropertyTree_virtual.h"
#include "CPropertyTree.h"
#include <unordered_map>

template <typename TKey, typename TValue, typename T>
class CMap : public ICMap
{
    T* m_map;
    int m_addref;
public:
    CMap() : m_map(0), m_addref(1)
    {
        m_map = new T;
    }
    ~CMap()
    {
        delete m_map;
        m_map = 0;
    }
    virtual int Addref()
    {
        return ++m_addref;
    }
    virtual int Release()
    {
        if (--m_addref)
        {
            delete this;
            return 0;
        }
        return m_addref;
    }
    // ��map���ȡָ��KEY��Ӧ��ֵ, ���û�����key, �ڲ��ᴴ��һ��
    virtual TValue& GetMapValue(TKey key)
    {
        T& m = *m_map;
        return m[key];
    }
    // ��ָ��key����һ���µ�ֵ
    virtual void SetMapValue(TKey key, TValue value)
    {
        T& m = *m_map;
        m[key] = value;
    }
    // ��map���ѯָ��KEY��Ӧ��ֵ, ���û�����key���ᴴ��
    virtual TValue QueryMapValue(TKey key)
    {
        T& m = *m_map;
        auto it = m.find(key);
        if (it == m.end())
            return 0;
        return it->second;
    }

    // ��map��ɾ��ָ��KEY��Ӧ��ֵ
    virtual bool erase(LPVOID key)
    {
        T& m = *m_map;
        auto it = m.find(key);
        if (it == m.end())
            return false;
        m.erase(it);
        return true;
    }
    // ��ȡmap��Ա��
    virtual int size()
    {
        T& m = *m_map;
        return (int)m.size();
    }

    // ���map
    virtual void clear()
    {
        T& m = *m_map;
        m.clear();
    }

    // ö�ٵ�ǰ���м����ͼ�ֵ, ����д�뻺�����ĳ�Ա��
    // buffer ������, ��Ҫ�ⲿ���뻺����, Ϊ0�򷵻���Ҫ�Ļ���������
    // size = ����������
    virtual int enumValue(LPMAP_DATA_STRUCT buffer, int size)
    {
        T& m = *m_map;
        if (!buffer || size <= 0)
            return (int)m.size();

        int ret = 0;
        for (auto it = m.begin(); it != m.end(); ++it)
        {
            buffer[ret].name = it->first;
            buffer[ret].value = it->second;
            if (++ret >= size)
                break;
        }
        
        return ret;
    }

    // ö�ٵ�ǰ���м����ͼ�ֵ, ����д�뻺�����ĳ�Ա��
    // pfnCallback = ö�ٵ���Ա����õĻص�����, Ϊ0�򷵻س�Ա��
    // pArg = ���ݵ��ص�������Ĳ���
    virtual int enumValueEx(PFN_EnumMap pfnCallback, LPVOID pArg)
    {
        T& m = *m_map;
        int ret = (int)m.size();
        if (pfnCallback)
        {
            ret = 0;
            for (auto it = m.begin(); it != m.end(); ++it)
            {
                ret++;
                if (!pfnCallback(pArg, it->first, it->second))
                    break;
            }
        }
        return ret;

    }
};

class ICPropertyTree_Realization : public ICPropertyTree
{
    CPropertyTree tree;
    int m_addref;
public:
    ICPropertyTree_Realization() { m_addref = 1; }

    virtual LPCWSTR className() { return tree.className(); };
    virtual int Addref() { return ++m_addref; }
    virtual int Release() {
        if (--m_addref)
        {
            delete this;
            return 0;
        }
        return m_addref;
    }
public:
    virtual bool SetIcon(PROPERTY_ICONTYPE type, LPCVOID pIcon, int size) { return tree.SetIcon(type, pIcon, size); }
public:
    virtual void SetItemHeight(int height, bool isUpdate = false) { tree.SetItemHeight(height, isUpdate); }
    virtual int GetItemHeight() { return tree.GetItemHeight(); }

    // �Ƿ�Ϊ����
    virtual bool isGroup(PROPERTY_TYPE type) { return tree.IsGroup(type); }
    // �Ƿ��а�ť
    virtual bool isButton(PROPERTY_TYPE type) { return tree.IsButton(type); }
    // �Ƿ���������ť
    virtual bool isButtonCombobox(PROPERTY_TYPE type) { return tree.IsButtonCombobox(type); }
    // �Ƿ�Ϊ��ֵ
    virtual bool isNumber(PROPERTY_TYPE type) { return tree.IsNumber(type); }
    // �Ƿ�Ϊ�ı�, ���ı�, ����ť�ı�������true
    virtual bool isText(PROPERTY_TYPE type) { return tree.IsText(type); }

    // ����Ϊ��չ����, �ɼ�����ʽ��϶���

    // �Ƿ�Ϊ����ť�ı�
    virtual bool isTextButton(PROPERTY_TYPE type) { return tree.IsTextButton(type); }
    // �Ƿ�Ϊ���ı�
    virtual bool isLongText(PROPERTY_TYPE type) { return tree.IsLongText(type); }
    // �Ƿ�Ϊ���ɱ༭��Ͽ�
    virtual bool isComboboxNoEdit(PROPERTY_TYPE type) { return tree.IsComboboxNoEdit(type); }
    // �Ƿ�Ϊ���ɱ༭��Ͽ�
    virtual bool isComboboxNoEditIndex(PROPERTY_TYPE type) { return tree.IsComboboxNoEditIndex(type); }
    // �Ƿ�Ϊ��Ͽ�, ���ɱ༭�ʹ���ŵĶ�������, ��Ҫ�ж϶�Ӧ�ľ�ʹ�ö�Ӧ�ķ���
    virtual bool isCombobox(PROPERTY_TYPE type) { return tree.IsCombobox(type); }
    // �Ƿ�Ϊ��ɫѡ����
    virtual bool isColorDialog(PROPERTY_TYPE type) { return tree.IsColorDialog(type); }
    // �Ƿ�Ϊ�ļ�ѡ����
    virtual bool isFileDialog(PROPERTY_TYPE type) { return tree.IsFileDialog(type); }
    // �Ƿ�Ϊ����ѡ����
    virtual bool isFontDialog(PROPERTY_TYPE type) { return tree.IsFontDialog(type); }
    // �Ƿ�Ϊ�ֽڼ�
    virtual bool isBinDialog(PROPERTY_TYPE type) { return tree.IsBinDialog(type); }
    // �Ƿ�ΪͼƬѡ����
    virtual bool isPicDialog(PROPERTY_TYPE type) { return tree.IsPicDialog(type); }
    // �Ƿ�Ϊ��Ƶѡ����
    virtual bool isMusicDialog(PROPERTY_TYPE type) { return tree.IsMusicDialog(type); }
    // �Ƿ�ΪͼƬ��ѡ����
    virtual bool isImgListDialog(PROPERTY_TYPE type) { return tree.IsImgListDialog(type); }
    // �Ƿ�Ϊ���ӿ�
    virtual bool IsLink(PROPERTY_TYPE type) { return tree.IsLink(type); }
    // �Ƿ�Ϊѡ���
    virtual bool IsCheckBox(PROPERTY_TYPE type) { return tree.IsCheckBox(type); }
    // �Ƿ�Ϊ3̬ѡ���
    virtual bool IsCheckBox3(PROPERTY_TYPE type) { return tree.IsCheckBox3(type); }
    // �Ƿ�Ϊ��ѡ��
    virtual bool IsRadio(PROPERTY_TYPE type) { return tree.IsRadio(type); }
    // �Ƿ�Ϊ�Զ���ѡ��
    virtual bool IsAutoRadio(PROPERTY_TYPE type) { return tree.IsAutoRadio(type); }
    // �Ƿ�Ϊ����
    virtual bool IsSwitch(PROPERTY_TYPE type) { return tree.IsSwitch(type); }
    // �Ƿ�Ϊѡ���������, ѡ���/3̬ѡ���, ��ѡ��, �Զ���ѡ��, ����, ������ѡ������
    virtual bool IsCheckType(PROPERTY_TYPE type) { return tree.IsCheckType(type); }


    // 1=�ֽ�,2=������,3=����,4=������,5=С��,6=˫����С��, ���᷵������ֵ
    virtual int  GetNumberType(PROPERTY_TYPE type) { return tree.GetNumberType(type); }
    // �Ƿ��з���
    virtual bool isUnsigned(PROPERTY_TYPE type) { return tree.IsUnsigned(type); }
    // �Ƿ�ɱ༭
    virtual bool isEdit(PROPERTY_TYPE type) { return tree.IsEdit(type); }
    // �Ƿ��б��
    virtual bool isIndex(PROPERTY_TYPE type) { return tree.IsIndex(type); }
    // �Ƿ��Զ�������
    virtual bool isCustom(PROPERTY_TYPE type) { return tree.IsCustom(type); }

    // �Ƿ��ֹ
    virtual bool isDisabled(PROPERTY_TYPE type) { return tree.IsDisabled(type); }
    // �Ƿ����
    virtual bool isEnabled(PROPERTY_TYPE type) { return tree.IsEnabled(type); }

    // ����Ӧ������Ϣ, ���Ұ���Ϣת����������
    virtual bool isNoMouseWheel(PROPERTY_TYPE type) { return tree.IsNoMouseWheel(type); }


public:
    virtual HWND create(DWORD dwExStyle, LPCWSTR lpszTitle, DWORD dwStyle, DWORD dwExStyleTree,
        int x, int y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR ID = 0, LPVOID lpParam = 0) {
        _canvas_init(true);
        return tree.create(dwExStyle, lpszTitle, dwStyle, dwExStyleTree, x, y, nWidth, nHeight, hWndParent, ID, lpParam);
    }

public:
    // д���ֽڼ�, ��������ָ��ͳ���, �ڲ���ת�������Եĸ�ʽ
    virtual bool SetBinData(IPROPERTY_ITEM& data, const void* pData, const size_t size) { return tree.SetBinData(data, pData, size); }

    // д���ֽڼ�, �������ݱ����������Եĸ�ʽ
    virtual bool SetBinData(IPROPERTY_ITEM& data, const void* pData) { return tree.SetBinData(data, pData); }

    // д������, �����ŵ����ݴ����ǹ̶���, �����ṹ, LOGFONTW + CHOOSEFONTW, �ڲ����޸������С/�ַ��߶�
    virtual bool SetFontData(IPROPERTY_ITEM& data, const LPLOGFONTW lf) { return tree.SetFontData(data, lf); }

    // д������, ����CHOOSEFONTW�ṹָ��, �ڲ����޸������С/�ַ��߶�
    // CHOOSEFONTW ָ��� LOGFONT�ṹ��ֵ�Ļ�Ҳ�������ṹ������������
    virtual bool SetFontData(IPROPERTY_ITEM& data, const LPCHOOSEFONTW pcf) { return tree.SetFontData(data, pcf); }

    // д������, ����gdi����ָ��, �ڲ����޸������С/�ַ��߶�
    virtual bool SetFontData(IPROPERTY_ITEM& data, HFONT hFont) { return tree.SetFontData(data, hFont); }


    // д������, �����ŵ����ݴ����ǹ̶���, �����ṹ, LOGFONTW + CHOOSEFONTW
    virtual bool SetFontData(IPROPERTY_ITEM& data, const wchar_t* name = L"΢���ź�", int lfHeight = -12,
        bool lfWeight = false, bool lfItalic = false, bool lfUnderline = false, bool lfStrikeOut = false) {
        return tree.SetFontData(data, name, lfHeight, lfWeight, lfItalic, lfUnderline, lfStrikeOut);
    }

    // ��ȡ��������, �ڲ��и��� LPCHOOSEFONTW �ṹָ���logfont��ַ
    virtual LPLOGFONTW GetFontData(IPROPERTY_ITEM& data, LPCHOOSEFONTW* pcf) { return tree.GetFontData(data, pcf); }

    // ������Ŀ�����ȡ��Ŀ�����Ŀ������ָ��
    virtual IPROPERTY_ITEM* GetItemData(HTREEITEM hItem) { return tree.GetItemData(hItem); }

    // ���öԻ�������, Ϊ0�����ö�Ӧ������
    // type = Ҫ���õĶԻ�������, Ŀǰֻ֧�� PROPERTY_FLAG_FILE�ļ�ѡ����, PROPERTY_FLAG_BIN�ֽڼ�ѡ����
    // filter = ������, ��ʽ����MP3�ļ�(*.mp3)|*.mp3\0ý���ļ�(*.mpg)|*.mpg��
    // defName = Ĭ���ļ���
    // title = ���ڱ���
    // path = ����Ŀ¼
    // notChengedPath = ���ı�Ŀ¼, ��Ҫ���ݱ���ָ��, ָ��Ϊ0������
    virtual bool SetOpenDialogProperty(PROPERTY_TYPE type, LPCWSTR filter = 0, LPCWSTR title = 0, LPCWSTR defName = 0, LPCWSTR path = 0, bool* noChangePath = 0)
    {
        return tree.SetOpenDialogProperty(type, filter, title, defName, path, noChangePath);
    }

    // ��ȡ�Ի�������, Ϊ0�򲻻�ȡ��Ӧ������
    // type = Ҫ���õĶԻ�������, Ŀǰֻ֧�� PROPERTY_FLAG_FILE�ļ�ѡ����, PROPERTY_FLAG_BIN�ֽڼ�ѡ����
    // filter = ������, ��ʽ����MP3�ļ�(*.mp3)|*.mp3\0ý���ļ�(*.mpg)|*.mpg��
    // defName = Ĭ���ļ���
    // title = ���ڱ���
    // path = ����Ŀ¼
    // notChengedPath = ���ı�Ŀ¼
    virtual bool GetOpenDialogProperty(PROPERTY_TYPE type, LPCWSTR* filter = 0, LPCWSTR* title = 0, LPCWSTR* defName = 0, LPCWSTR* path = 0, bool* noChangePath = 0)
    {
        return tree.GetOpenDialogProperty(type, filter, title, defName, path, noChangePath);
    }



public:
    // ����һ������, ���������Ŀ�ľ��
    virtual HTREEITEM InsertGroup(LPCWSTR name, bool isExpand, LPCWSTR text = 0, LPARAM param = 0) { return tree.InsertGroup(name, isExpand, text, param); }

    // ����ָ����Ŀ��ֵ, �ֽڼ����ͺ�������һ��
    virtual bool SetItemValue(IPROPERTY_ITEM& data, const void* value) { return tree.SetItemValue(data, value); }

    // ����һ���ı�����������, ����ֵֻ�����ı�
    // hParent = InsertGroup() ���ص�ֵ, ��Ҫ���뵽�ĸ�������
    // name = ������, ���������к����޸�
    // value = ����ֵ
    // text = ��ע�ı�
    // param = ��Ŀ��ֵ
    virtual HTREEITEM InsertItem(HTREEITEM hParent, LPCWSTR name, LPCWSTR value = 0, PROPERTY_TYPE type = PROPERTY_TYPE_TEXT, LPCWSTR text = 0, LPARAM param = 0) { return tree.InsertItem(hParent, name, value, type, text, param); }

    // ����˵���ı�
    virtual bool SetItemRemarks(HTREEITEM hItem, LPCWSTR value) { return tree.SetItemRemarks(hItem, value); }
    // ��ȡ˵���ı�
    virtual LPCWSTR GetItemRemarks(HTREEITEM hItem) { return tree.GetItemRemarks(hItem); }

    // ɾ��ָ����Ŀ
    virtual BOOL DeleteItem(HTREEITEM hItem) { return tree.DeleteItem(hItem); }


    // ȡָ����Ŀ��������
    virtual LPCWSTR GetItemName(HTREEITEM hItem) { return tree.GetItemName(hItem); }
    virtual bool SetItemName(HTREEITEM hItem, LPCWSTR name) { return tree.SetItemName(hItem, name); }

    // ȡָ����Ŀ������ֵ, ʧ�ܷ���0, �ֽڼ�������Ҫ��ǰ����� {1,0,0,0,����} 8���ֽ�, ��������һ��
    virtual LPCWSTR GetItemValue(HTREEITEM hItem) { return tree.GetItemValue(hItem); }

    // ����ָ����Ŀ��ֵ, �ֽڼ����ͺ�������һ��
    virtual bool SetItemValue(HTREEITEM hItem, LPCWSTR value) { return tree.SetItemValue(hItem, value); }

    // ȡָ����Ŀ����Ŀ��ֵ, InsertItem/SetItemValue ʱ��д��ֵ
    virtual LPARAM GetItemParam(HTREEITEM hItem) { return tree.GetItemParam(hItem); }

    // ����ָ����Ŀ��ֵ
    virtual bool SetItemParam(HTREEITEM hItem, LPARAM param) { return tree.SetItemParam(hItem, param); }

    // ����Ͽ���Ŀ������Ŀ����, ���ú������Զ������Ͽ�����
    // ���óɹ����ر������Ͽ���Ŀ��, ʧ�ܷ���-1
    virtual size_t AddComboboxData(HTREEITEM hItem, LPCWSTR name, LPARAM param = 0) { return tree.AddComboboxData(hItem, name, param); }

    // ɾ����Ͽ�����, index = ɾ�����±�, ��0��ʼ, �±�Խ��/������Ͽ����ͷ���false
    virtual bool DelComboboxData(HTREEITEM hItem, int index) { return tree.DelComboboxData(hItem, index); }

    // ȡָ����Ŀ����Ͽ���Ŀ��, ʧ�ܷ���-1, ������Ͽ����ͻ�����Ŀ�����Ч������-1
    virtual size_t GetComboboxDataCount(HTREEITEM hItem) { return tree.GetComboboxDataCount(hItem); }

    // ȡָ����Ŀ����Ͽ���Ŀ�ı�, ʧ�ܷ��ؿ��ı�, ������Ͽ�����, �±�Խ��, �����Ч�����ؿ��ı�
    virtual LPCWSTR GetComboboxData(HTREEITEM hItem, int index) { return tree.GetComboboxData(hItem, index); }

    // ����ָ����Ŀ����Ͽ���Ŀ�ı�, ʧ�ܷ���false, ������Ͽ�����, �±�Խ��, �����Ч������false
    virtual bool SetComboboxData(HTREEITEM hItem, int index, LPCWSTR value) { return tree.SetComboboxData(hItem, index, value); }

    // ��ȡ��Ͽ�����ѡ����, ʧ�ܷ���-1
    virtual int GetComboboxIndex(HTREEITEM hItem) { return tree.GetComboboxIndex(hItem); }

    // ������Ͽ�����ѡ����, ������0��ʼ
    virtual bool SetComboboxIndex(HTREEITEM hItem, int index) { return tree.SetComboboxIndex(hItem, index); }

    // ȡָ����Ŀ����Ͽ���Ŀ��ֵ
    virtual LPARAM GetComboboxParam(HTREEITEM hItem, int index) { return tree.GetComboboxParam(hItem, index); }

    // ����ָ����Ŀ����Ͽ���Ŀ��ֵ
    virtual bool SetComboboxParam(HTREEITEM hItem, int index, LPARAM param) { return tree.SetComboboxParam(hItem, index, param); }

    // ����ָ����ĿΪָ������, ��Ͽ�������Ҫ��������
    virtual void SetItemType(HTREEITEM hItem, PROPERTY_TYPE type) { tree.SetItemType(hItem, type); }

    // ȡָ����Ŀ����
    virtual PROPERTY_TYPE GetItemType(HTREEITEM hItem) { return tree.GetItemType(hItem); }

    // ���ÿ��, ǧ�ֱ�, ��С100, ���900, �ؼ�δ����ǰҲ���Ե���
    // isLeft = �Ƿ�Ϊ�������, ����һ�ߺ���һ�߻�һ�����
    // value = 100-900֮���ֵ
    // isUpdate = �Ƿ��ػ�
    virtual void SetWidthRatio(bool isLeft, int value, bool isUpdate = false) { tree.SetWidthRatio(isLeft, value, isUpdate); }

    // ����ǰ�����������ݱ�����ֽڼ�����, ���صĵ�ַ��ʹ��ʱ��Ҫ���� free() �ͷ�
    // isColor = �Ƿ񱣴���ɫ������Ϣ
    virtual LPBYTE save(DWORD* size, bool isColor, bool isList) { return tree.save(size, isColor, isList); }

    // ���ڴ������������Ŀ����
    virtual bool load(const void* data, DWORD size, bool isUpdate) { return tree.load(data, size, isUpdate); }

    // ������ɫ��Ϣ
    virtual void SetColor(const PROPERTY_COLOR* color, bool isUpdate) { return tree.SetColor(color, isUpdate); }
    virtual int GetColor(PROPERTY_COLOR* color) { return tree.GetColor(color); }
    // Ĭ����ɫ, 0=��ɫ���, 1=��ɫ���
    virtual int DefColor(int type, bool isUpdate) { return tree.DefColor(type, isUpdate); }

    virtual int SetItemFont(const LPLOGFONTW lf, bool isUpdate) { return tree.SetItemFont(lf, isUpdate); }
    // Ĭ��Ϊ΢���ź�, �����С -12, û����ʽ
    // lfItalic = б��,  lfUnderline = �»���, lfStrikeOut = ɾ����
    virtual int SetItemFont(bool isUpdate, const wchar_t* name = L"΢���ź�", int size = -12, bool lfItalic = false, bool lfUnderline = false, bool lfStrikeOut = false) {
        return SetItemFont(isUpdate, name, size, lfItalic, lfUnderline, lfStrikeOut);
    }

    virtual int SetColorBackground(ARGB cr, bool isUpdate = false) { return tree.SetColorBackground(cr, isUpdate); }
    virtual int SetColorBackParent(ARGB cr, bool isUpdate = false) { return tree.SetColorBackParent(cr, isUpdate); }
    virtual int SetColorText(ARGB cr, bool isUpdate = false) { return tree.SetColorText(cr, isUpdate); }
    virtual int SetColorListBorder(ARGB cr, bool isUpdate = false) { return tree.SetColorListBorder(cr, isUpdate); }
    virtual int SetColorSelect(ARGB cr, bool isUpdate = false) { return tree.SetColorSelect(cr, isUpdate); }
    virtual int SetColorDisabled(ARGB cr, bool isUpdate = false) { return tree.SetColorDisabled(cr, isUpdate); }
    virtual void SetIsFillColorBack(bool isFill) { return tree.SetIsFillColorBack(isFill); }

    virtual bool SetItemDisabledState(HTREEITEM hItem, bool isDisabled) { return tree.SetItemDisabledState(hItem, isDisabled); }
    // ��ֹ��Ŀ
    virtual bool DisabledItem(HTREEITEM hItem) { return tree.DisabledItem(hItem); }
    // ������Ŀ
    virtual bool EnabledItem(HTREEITEM hItem) { return tree.EnabledItem(hItem); }
    // ��Ŀ�Ƿ��ֹ
    virtual bool IsItemDisabled(HTREEITEM hItem) { return tree.IsItemDisabled(hItem); }
    // ��Ŀ�Ƿ�����
    virtual bool IsItemEnabled(HTREEITEM hItem) { return tree.IsItemEnabled(hItem); }

    virtual bool clear() { return tree.clear(); }

    // �����Ի���, ���ݵ�ǰ��Ŀ����̽���Ӧ�Ի���, �ᴥ����ť����ص�
    virtual void popDialog() { return tree.popDialog(); }

    // ��ȡ���Կ����
    virtual void GetPropertyItemRect(HTREEITEM hItem, RECT* prc, PROPERTY_RECT type) { tree.GetPropertyItemRect(hItem, prc, type); }

    virtual LPBYTE GetExpand_Open(int* size) { return tree.GetExpand_Open(size); }
    virtual LPBYTE GetExpand_Close(int* size) { return tree.GetExpand_Close(size); }
    virtual bool SetExpand_Open(const void* pData, int size) { return tree.SetExpand_Open(pData, size); }
    virtual bool SetExpand_Close(const void* pData, int size) { return tree.SetExpand_Close(pData, size); }

public:


    // ��Ŀ���Ա��ı�, Ϊ����ȡ���ҽ�����¼�, �¼���������, ����1�Ǵ����¼�����Ŀ���, ����2����Ŀ������
    virtual void OnPropertyChanged(fnPropertyChanged pfn = 0, void* param = 0) { return tree.OnPropertyChanged(pfn, param); }

    // ��Ŀѡ��ı�, Ϊ����ȡ���ҽ�����¼�, �¼���������, ����1�Ǵ����¼�����Ŀ���, ����2�����ĸ�λ�õ��, -1=չ��������ť, 1=������, 2=����ֵ
    virtual void OnItemChanged(fnItemChanged pfn = 0, void* param = 0) { return tree.OnItemChanged(pfn, param); }

    // ��Ŀ����ֵ�����˻س���, �ص����������Ƿ�����, Ϊ����ȡ���ҽ�����¼�, ����ص���������true�򲻶԰��»س�������һ������
    // �¼���������, ����1�Ǵ����¼�����Ŀ���, ����2����Ŀ������
    virtual void OnPropertyReturn(fnPropertyReturn pfn = 0, void* param = 0) { return tree.OnPropertyReturn(pfn, param); }

    // ��Ͽ���Ŀ������, �ص����������Ƿ�����, ����ص���������true�򲻶�����������һ������
    // count = ��Ͽ���Ŀ����, pos = �������븺�����¹���, �������Ϲ���, key = ����ʱ�������ĸ�����, lParam = �������
    virtual void OnComboboxMouseWheel(fnComboboxMouseWheel pfn = 0, void* param = 0) { return tree.OnComboboxMouseWheel(pfn, param); }

    // ��Ͽ���Ŀ�ı�, �ص����������Ƿ�����, ����ص���������true�򲻻�ı���Ŀ
    // index = Ҫ�ı����Ŀ����, ����false����Ͽ�����ѡ��������������
    virtual void OnComboboxChanged(fnComboboxChanged pfn = 0, void* param = 0) { return tree.OnComboboxChanged(pfn, param); }

    // ��Ͽ�������ͷ������, ����̽���б�, �ص����������Ƿ�����, ����ص���������true�򲻻ᵯ���б�
    virtual void OnDropDown(fnDropWown pfn = 0, void* param = 0) { return tree.OnDropDown(pfn, param); }

    // ���԰�ť������, �ص����������Ƿ�����, ����ص���������true�򲻻ᵯ�����õĶԻ���
    virtual void OnItemButtonClick(fnItemButtonClick pfn = 0, void* param = 0) { return tree.OnItemButtonClick(pfn, param); }

    // ��Ŀ���Ҽ�����, �����Ƿ�����, ���غ��Ҽ�����ı����, ���ᴥ������
    virtual void OnItemRButtonClick(fnItemRButtonClick pfn = 0, void* param = 0) { return tree.OnItemRButtonClick(pfn, param); }

    // ��Ͽ�˫���༭��, �ص����������Ƿ�����, ����ص���������true�򲻶�˫������һ������
    // index = ˫��ʱ��Ͽ������, count = ��Ͽ���Ŀ��
    virtual void OnComboboxLDoubleClick(fnComboboxLDoubleClick pfn = 0, void* param = 0) { return tree.OnComboboxLDoubleClick(pfn, param); }

    // �༭��˫��, �ص����������Ƿ�����, ����ص���������true��ȫѡ�༭��
    virtual void OnEditLDoubleClick(fnEditLDoubleClick pfn = 0, void* param = 0) { return tree.OnEditLDoubleClick(pfn, param); }

    // ��Ŀ�����ַ�, �ص����������Ƿ�����, ����ص���������true��ʹ�� pRet ���ص�ֵ��Ϊ����ֵ
    // ch = ������ַ�, pRet = ����ֵָ��, ��Ҫ�ѷ���ֵд������
    virtual void OnItemChar(fnItemChar pfn = 0, void* param = 0) { return tree.OnItemChar(pfn, param); }

    // �����Զ�������ֵ����ʾ����, ��������, ��Ŀ���, ��Ŀ����, ��ʾ�ı�������, ���100���ַ�
    virtual void OnItemMakeCustomValue(fnItemMakeCustomValue pfn = 0, void* param = 0) { return tree.OnItemMakeCustomValue(pfn, param); }

    // �ұ�С��ť���ػ�, ����true���ڲ������л滭, ��Ҫ�û��ڻص�����л滭����
    // hdc = ��ť�滭dc, �����ڴ�DC, state = ��ǰ״̬, STATE::HOT, STATE::DOWN, width = ��ť���, height = ��ť�߶�
    virtual void OnItemRButtonPaint(fnItemRButtonPaint pfn = 0, void* param = 0) { return tree.OnItemRButtonPaint(pfn, param); }

    // del���ſ�, ɾ����ǰ������, ����������һ�а��µ�Ĭ�ϴ��������ֵ, �������������µ�ֻ���˫����򿪶Ի������Ŀ
    // isName = �Ƿ�����������һ�а��°���
    virtual void OnItemKeyDelUp(fnItemDelUp pfn = 0, void* param = 0) { return tree.OnItemKeyDelUp(pfn, param); }

    // �Զ�����������
    // isName = �Ƿ�����������һ�а��°���
    virtual void OnCustomValueName(fnCustomValueName pfn = 0, void* param = 0) { return tree.OnCustomValueName(pfn, param); }

    // ��Ŀ��ȡ����ʧ��, �ֽڼ�, ͼƬ, ��Ƶ, ͼƬ����Щ....
    virtual void OnItemReadError(fnItemReadError pfn = 0, void* param = 0) { return tree.OnItemReadError(pfn, param); }

    // �����滭, �ڲ��滭ǰ�ȵ����û��Ļ滭
    virtual void OnItemBeginDraw(fnItemBeginDraw pfn = 0, void* param = 0) { tree.OnItemBeginDraw(pfn, param); }

    // �滭����, �ڲ��滭�Ѿ�����, �û�������������Ͻ��ж��λ滭
    virtual void OnItemEndDraw(fnItemEndDraw pfn = 0, void* param = 0) { tree.OnItemEndDraw(pfn, param); }

    virtual bool Expand(HTREEITEM hItem, BOOL b) { return tree.Expand(hItem, b); }
    virtual bool Expand(HTREEITEM hItem) { return tree.Expand(hItem); }
    virtual bool IsExpand(HTREEITEM hItem) { return tree.IsExpand(hItem); }
    // ��֤��ʾ
    virtual bool EnsureVisible(HTREEITEM hItem) { return tree.EnsureVisible(hItem); }

    virtual bool Move(int x, int y, int cx, int cy, bool bRepaint = true) { return tree.Move(x, y, cx, cy, bRepaint); }




    virtual DWORD GetItemCount() { return tree.GetItemCount(); }
    virtual void InvalidateItem(HTREEITEM hItem) { tree.InvalidateItem(hItem); }

    // ������ѡ����
    virtual bool SetIndex(HTREEITEM hItem) { return tree.SetIndex(hItem); }

    // ȡ������Ŀ��
    virtual unsigned int GetVisibleCount() { return tree.GetVisibleCount(); }

    // ȡ��һ����Ŀ
    virtual HTREEITEM GetFirstItem() { return tree.GetFirstItem(); }

    // ȡ����Ŀ
    virtual HTREEITEM GetRootItem() { return tree.GetRootItem(); }

    // ȡ����Ŀ, ��Ŀ���Ϊ0��ȡ��ǰѡ����Ŀ���
    virtual HTREEITEM GetChild(HTREEITEM hItem = 0) { return tree.GetChild(hItem); }

    // ȡ��һ���ֵ���, ��Ŀ���Ϊ0��ȡ��ǰѡ����Ŀ���
    virtual HTREEITEM GetNextSibling(HTREEITEM hItem = 0) { return tree.GetNextSibling(hItem); }

    // ȡ��һ���ֵ���, ��Ŀ���Ϊ0��ȡ��ǰѡ����Ŀ���
    virtual HTREEITEM GetPrevSibling(HTREEITEM hItem = 0) { return tree.GetPrevSibling(hItem); }

    // ȡ����Ŀ, ��Ŀ���Ϊ0��ȡ��ǰѡ����Ŀ���
    virtual HTREEITEM GetParent(HTREEITEM hItem = 0) { return tree.GetParent(hItem); }

    // ȡ��һ��������Ŀ
    virtual HTREEITEM GetFirstVisible() { return tree.GetFirstVisible(); }

    // ȡ��һ������, ��Ŀ���Ϊ0��ȡ��ǰѡ����Ŀ���
    virtual HTREEITEM GetNextVisible(HTREEITEM hItem = 0) { return tree.GetNextVisible(hItem); }

    // ȡ��һ������, ��Ŀ���Ϊ0��ȡ��ǰѡ����Ŀ���
    virtual HTREEITEM GetPrevVisible(HTREEITEM hItem = 0) { return tree.GetPrevVisible(hItem); }

    // ȡ����ѡ����
    virtual HTREEITEM GetSelection() { return tree.GetSelection(); }

    // ȡ����ѡ����
    virtual HTREEITEM GetIndex() { return tree.GetIndex(); }
    // ȡ����Ŀ
    virtual HTREEITEM GetRoot() { return tree.GetRoot(); }

    // ȡ���һ��������Ŀ
    virtual HTREEITEM GetLastVisible() { return tree.GetLastVisible(); }

    // ȡ��һ����Ŀ, ��Ŀ���Ϊ0��ȡ��ǰѡ����Ŀ���
    virtual HTREEITEM GetNextItem(HTREEITEM hItem = 0, int code = TVGN_NEXT) { return tree.GetNextItem(); }



    // ȡ��һ��ѡ����, ��Ŀ���Ϊ0��ȡ��ǰѡ����Ŀ���
    virtual HTREEITEM GetNextSelectedItem(HTREEITEM hItem = 0) { return tree.GetNextSelectedItem(hItem); }

    virtual HWND GetWindow() { return tree.GetWindow(); }
    // ȡ����Ŀ��
    virtual int GetSubCount(HTREEITEM hItem) { return tree.GetSubCount(hItem); }
    // ȡ��Ŀ����, isTitle = �Ƿ�ֵ��������
    virtual RECT GetItemRect(HTREEITEM hItem, bool isTitle = FALSE) { return tree.GetItemRect(hItem, isTitle); }
    // �����м������ߵ�λ��
    virtual int calcMidLine(int cxClient = 0) { return tree.calcMidLine(cxClient); }


public:
    virtual void free(void* ptr) { if (!ptr)return; ::free(ptr); }




};


extern "C" API_DECLSPEC ICPropertyTree * CProperty_Create()
{
    return new ICPropertyTree_Realization;
}

extern "C" API_DECLSPEC ICMap* CMap_Create(bool isHash)
{
    typedef LPVOID Tname;
    typedef LPVOID Tvalue;
    if (isHash)
        return new CMap<Tname, Tvalue, std::unordered_map<Tname, Tvalue>>;
    return new CMap<Tname, Tvalue, std::map<Tname, Tvalue>>;

}
