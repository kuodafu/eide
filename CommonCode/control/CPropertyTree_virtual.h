#pragma once
#include <tstr.h>
#include <commctrl.h>

// �ļ�ͷ
#define LIST_PROP_HEAD_FLAG_COLOR       0x00000001      // ����ɫ��Ϣ
#define LIST_PROP_HEAD_FLAG_LIST        0x00000002      // ���б�����
#define LIST_PROP_HEAD_FLAG_EXPAND      0x00000004      // �����Ϳ���ߵļӼ���ťͼ������

#define LIST_PROP_MAJORVERSION          1       // ���汾��
// 2021-01-05 ����1.1, ��������������һ��˵���ı�
#define LIST_PROP_MINORVERSION          1       // �ΰ汾��     1.0 2020-09-27, ����ı��ļ�ͷ, ��Ҫ���°汾��
#pragma pack(1)

struct LIST_PROP_HEAD
{
    unsigned char header[2];        // �ļ�ͷ��ʶ 'P', 'D'
    unsigned char MajorVersion;     // ���汾��
    unsigned char MinorVersion;     // �ΰ汾��
    short         bfReserved1;      // ����1
    short         bfReserved2;      // ����2
    unsigned int  flags;            // �ļ��ṹ��־
};



struct LIST_PROP_DATA
{
    int type;                       // ��Ŀ����
    int index;                      // ��Ͽ�ǰѡ����
    __int64 param;                  // ��Ŀ��ֵ
    int subItemCount;               // ����Ŀ��, ��Ͽ�����
    int nameLen;                    // ����������
    int valueLen;                   // ����ֵ����
};

struct LIST_PROP_DATA_1_1 : public LIST_PROP_DATA
{
    int textLen;                    // ˵���ı�����
};
#pragma pack()

enum PROPERTY_RECT
{
    PROPERTY_RECT_ITEM,     // ��Ŀ����
    PROPERTY_RECT_WINDOW,   // ���Կ����
    PROPERTY_RECT_NAME,     // ����������
    PROPERTY_RECT_VALUE,    // ����ֵ����
    PROPERTY_RECT_BUTTON,   // �ұ߰�ť����
    PROPERTY_RECT_EXPAND,   // ��չ��ť����
};

enum PROPERTY_ICONTYPE
{
    PROPERTY_ICONTYPE_TREE_ENPAND                   = 0x0000,
    PROPERTY_ICONTYPE_TREE_UNENPAND                 = 0x0001,
    PROPERTY_ICONTYPE_CHECKBOX_SEL                  = 0x0010,
    PROPERTY_ICONTYPE_CHECKBOX_UNSEL                = 0x0011,
    PROPERTY_ICONTYPE_CHECKBOX_INDETERMINATE        = 0x0012,
    PROPERTY_ICONTYPE_RADIO_SEL                     = 0x0020,
    PROPERTY_ICONTYPE_RADIO_UNSEL                   = 0x0021,
    PROPERTY_ICONTYPE_SWITCH_SEL                    = 0x0030,
    PROPERTY_ICONTYPE_SWITCH_UNSEL                  = 0x0031,
};

// ǰ1���ֽڼ�¼��������, �����ظ�, �ڶ����ֽڼ�¼��ֵ�͵�����, ������ֵ�Ͳ�ʹ��, ��16λ��¼��ʽ
enum PROPERTY_TYPE
{
    PROPERTY_FLAG_BUTTON                    = 0x00010000,   // ����ͨ��ť
    PROPERTY_FLAG_BUTTON_COMBOBOX           = 0x00020000,   // ��������ť, ��PROPERTY_FLAG_BUTTON����ͬʱʹ��, ���һ��ʹ���򵱳���PROPERTY_FLAG_BUTTON
    PROPERTY_FLAG_NOEDIT                    = 0x00040000,   // ���ɱ༭, ��Ҫ��ʾ�༭��
    PROPERTY_FLAG_INDEX                     = 0x00080000,   // �б��, ֻ����Ͽ���Ч
    PROPERTY_FLAG_CUSTOM                    = 0x00100000,   // �Զ�������ֵ, ����ֵ��ʾ���ı�ͨ���ص�����������
    PROPERTY_FLAG_UPDOWN                    = 0x00200000,   // ���µ�����
    PROPERTY_FLAG_LEFTRIGHT                 = 0x00400000,   // ���ҵ�����
    PROPERTY_FLAG_NOMOUSEWHEEL              = 0x00800000,   // ����������ʱ���������, ���������Կ����
    PROPERTY_FLAG_DISABLED                  = 0x01000000,   // ��ֹ״̬
    PROPERTY_FLAG_CHECKLINESEL              = 0x02000000,   // ѡ����������ѡ��, û�����ѡ��Ļ�ֻ����ͼ���λ�õ���Ż�ѡ��


    PROPERTY_TYPE_GROUP     = 0,    // ����, ����Ŀ
    PROPERTY_TYPE_TEXT      = 1,    // �ı�����, û�к����С��ť
    PROPERTY_TYPE_NUMBER    = 2,    // ��ֵ����, ʹ���ı���¼, �û�ȡֵʱת������Ӧ����, ��ϸ�λ ��ȷ��ʹ��ʲô����, �������ʱ��ᴦ��

    ___PROPERTY_TYPE_START = 0x7F,  // ������Ϳ�ʼ

    PROPERTY_FLAG_COMBOBOX      = ___PROPERTY_TYPE_START + 1,   // ��Ͽ�����
    PROPERTY_FLAG_COLOR         = ___PROPERTY_TYPE_START + 2,   // ��ɫѡ����
    PROPERTY_FLAG_FILE          = ___PROPERTY_TYPE_START + 3,   // �ļ�ѡ����
    PROPERTY_FLAG_FONT          = ___PROPERTY_TYPE_START + 4,   // ����ѡ����
    PROPERTY_FLAG_BIN           = ___PROPERTY_TYPE_START + 5,   // �ֽڼ�
    PROPERTY_FLAG_PIC           = ___PROPERTY_TYPE_START + 6,   // ͼƬ
    PROPERTY_FLAG_MUSIC         = ___PROPERTY_TYPE_START + 7,   // ��Ƶѡ����
    PROPERTY_FLAG_IMGLIST       = ___PROPERTY_TYPE_START + 8,   // ͼƬ��ѡ����
    PROPERTY_FLAG_LINK          = ___PROPERTY_TYPE_START + 9,   // ����
    PROPERTY_FLAG_CHECKBOX      = ___PROPERTY_TYPE_START + 10,  // ѡ���
    PROPERTY_FLAG_CHECKBOX3     = ___PROPERTY_TYPE_START + 11,  // 3̬ѡ���
    PROPERTY_FLAG_RADIO         = ___PROPERTY_TYPE_START + 12,  // ��ѡ��
    PROPERTY_FLAG_AUTORADIO     = ___PROPERTY_TYPE_START + 13,  // �Զ���ѡ��
    PROPERTY_FLAG_SWITCH        = ___PROPERTY_TYPE_START + 14,  // ����

    PROPERTY_FLAG_BYTE          = 0x0100,   // ֻ����ֵ����Ч, ֻ���������ֽ���, 0x00 - 0xFF
    PROPERTY_FLAG_SHORT         = 0x0200,   // ֻ����ֵ����Ч, ֻ�������������, 0x0000 - 0xFFFF
    PROPERTY_FLAG_INT           = 0x0300,   // ֻ����ֵ����Ч, ֻ������������,  0x00000000 - 0xFFFFFFFF
    PROPERTY_FLAG_INT64         = 0x0400,   // ֻ����ֵ����Ч, ֻ�������볤����, 0x0000000000000000 - 0xFFFFFFFFFFFFFFFF
    PROPERTY_FLAG_FLOAT         = 0x0500,   // ֻ����ֵ����Ч, ֻ�������븡����
    PROPERTY_FLAG_DOUBLE        = 0x0600,   // ֻ����ֵ����Ч, ֻ��������˫���ȸ�����
    PROPERTY_FLAG_UNSIGNED      = 0x1000,   // ֻ����ֵ����Ч, �޷�����ֵ

    PROPERTY_TYPE_COMBOBOX      = PROPERTY_FLAG_COMBOBOX | PROPERTY_FLAG_BUTTON_COMBOBOX,       // ��Ͽ�����, ��һ������������
    PROPERTY_TYPE_COLOR         = PROPERTY_FLAG_COLOR | PROPERTY_FLAG_BUTTON,                   // ��ɫѡ����, ������ť��̽����ɫѡ����, һ��DWORD ����
    PROPERTY_TYPE_FILE          = PROPERTY_FLAG_FILE | PROPERTY_FLAG_BUTTON,                    // �ļ�ѡ����, ������̽����ļ��Ի���
    PROPERTY_TYPE_FONT          = PROPERTY_FLAG_FONT | PROPERTY_FLAG_BUTTON | PROPERTY_FLAG_NOEDIT | PROPERTY_FLAG_CUSTOM,      // ����ѡ��Ի���, ������̽���ѡ������Ի���, ���� = LPLOGFONTW + LPCHOOSEFONTW, �����ṹ, �̶�����
    PROPERTY_TYPE_BIN           = PROPERTY_FLAG_BIN | PROPERTY_FLAG_BUTTON | PROPERTY_FLAG_NOEDIT | PROPERTY_FLAG_CUSTOM,       // �ֽڼ�����, ���ݴ�ź�������һ��, {1,0,0,0,�ֽڼ�����}
    PROPERTY_TYPE_PIC           = PROPERTY_FLAG_PIC | PROPERTY_FLAG_BUTTON | PROPERTY_FLAG_NOEDIT | PROPERTY_FLAG_CUSTOM,       // ͼƬ����, �������Ի���, �Ի�����ʾͼƬ, ��Ӽ�����ťѡ��ͼƬ�ļ�
    PROPERTY_TYPE_MUSIC         = PROPERTY_FLAG_MUSIC | PROPERTY_FLAG_BUTTON | PROPERTY_FLAG_NOEDIT | PROPERTY_FLAG_CUSTOM,     // ��Ƶ����, �������Ի���, �Ի�����ʾ��Ƶ��Ϣ, ��Ӽ�����ť����, �滻ɾ��
    PROPERTY_TYPE_IMGLIST       = PROPERTY_FLAG_IMGLIST | PROPERTY_FLAG_BUTTON | PROPERTY_FLAG_NOEDIT | PROPERTY_FLAG_CUSTOM,   // ͼƬ��, �������Ի���, �Ի�����ʾͼƬ������ͼƬ, ������ɾ���滻�Ȱ�ť


    // �з�����ֵ

    PROPERTY_TYPE_NUMBER_BYTE   = PROPERTY_TYPE_NUMBER | PROPERTY_FLAG_BYTE,    // �з����ֽ�
    PROPERTY_TYPE_NUMBER_SHORT  = PROPERTY_TYPE_NUMBER | PROPERTY_FLAG_SHORT,   // �з��Ŷ�����
    PROPERTY_TYPE_NUMBER_INT    = PROPERTY_TYPE_NUMBER | PROPERTY_FLAG_INT,     // �з�������
    PROPERTY_TYPE_NUMBER_INT64  = PROPERTY_TYPE_NUMBER | PROPERTY_FLAG_INT64,   // �з��ų�����
    PROPERTY_TYPE_NUMBER_FLOAT  = PROPERTY_TYPE_NUMBER | PROPERTY_FLAG_FLOAT,   // �з���С��
    PROPERTY_TYPE_NUMBER_DOUBLE = PROPERTY_TYPE_NUMBER | PROPERTY_FLAG_DOUBLE,  // �з���˫����С��

    PROPERTY_TYPE_NUMBER_UBYTE  = PROPERTY_TYPE_NUMBER | PROPERTY_FLAG_UNSIGNED | PROPERTY_FLAG_BYTE,   // �޷����ֽ�
    PROPERTY_TYPE_NUMBER_USHORT = PROPERTY_TYPE_NUMBER | PROPERTY_FLAG_UNSIGNED | PROPERTY_FLAG_SHORT,  // �޷��Ŷ�����
    PROPERTY_TYPE_NUMBER_UINT   = PROPERTY_TYPE_NUMBER | PROPERTY_FLAG_UNSIGNED | PROPERTY_FLAG_INT,    // �޷�������
    PROPERTY_TYPE_NUMBER_UINT64 = PROPERTY_TYPE_NUMBER | PROPERTY_FLAG_UNSIGNED | PROPERTY_FLAG_INT64,  // �޷��ų�����
    PROPERTY_TYPE_NUMBER_UFLOAT = PROPERTY_TYPE_NUMBER | PROPERTY_FLAG_UNSIGNED | PROPERTY_FLAG_FLOAT,  // �޷���С��
    PROPERTY_TYPE_NUMBER_UDOUBLE= PROPERTY_TYPE_NUMBER | PROPERTY_FLAG_UNSIGNED | PROPERTY_FLAG_DOUBLE, // �޷���˫����С��


    PROPERTY_TYPE_TEXT_BUTTON           = PROPERTY_TYPE_TEXT | PROPERTY_FLAG_BUTTON,                                // �ı�����, �������С��ť
    PROPERTY_TYPE_TEXT_LONG             = PROPERTY_TYPE_TEXT_BUTTON | PROPERTY_FLAG_CUSTOM | PROPERTY_FLAG_NOEDIT,  // ���ı�����, ������С��ť+�Զ�����ʾ����ֵ+���ɱ༭
    PROPERTY_TYPE_COMBOBOX_NOEDIT       = PROPERTY_TYPE_COMBOBOX | PROPERTY_FLAG_NOEDIT,                            // ���ɱ༭��Ͽ�
    PROPERTY_TYPE_COMBOBOX_NOEDIT_INDEX = PROPERTY_TYPE_COMBOBOX_NOEDIT | PROPERTY_FLAG_INDEX,                      // ���ɱ༭��Ͽ�+���


    PROPERTY_TYPE_UNKNOWN = -1,         // δ֪
};



typedef void(CALLBACK* fnPropertyChanged)(void* param, HTREEITEM hItem, PROPERTY_TYPE type);
typedef void(CALLBACK* fnItemChanged)(void* param, HTREEITEM hItem, PROPERTY_TYPE type, int downType);
typedef bool(CALLBACK* fnPropertyReturn)(void* param, HTREEITEM hItem, PROPERTY_TYPE type);
typedef bool(CALLBACK* fnComboboxMouseWheel)(void* param, HTREEITEM hItem, PROPERTY_TYPE type, int count, int pos, int key, LPARAM lParam);
typedef bool(CALLBACK* fnComboboxChanged)(void* param, HTREEITEM hItem, PROPERTY_TYPE type, int index);
typedef bool(CALLBACK* fnDropWown)(void* param, HTREEITEM hItem, PROPERTY_TYPE type);
typedef bool(CALLBACK* fnItemButtonClick)(void* param, HTREEITEM hItem, PROPERTY_TYPE type);
typedef bool(CALLBACK* fnItemRButtonClick)(void* param, HTREEITEM hItem, PROPERTY_TYPE type);
typedef bool(CALLBACK* fnComboboxLDoubleClick)(void* param, HTREEITEM hItem, PROPERTY_TYPE type, int index, int count);
typedef bool(CALLBACK* fnEditLDoubleClick)(void* param, HTREEITEM hItem, PROPERTY_TYPE type);
typedef bool(CALLBACK* fnItemChar)(void* param, HTREEITEM hItem, PROPERTY_TYPE type, int ch, int* pRet);
typedef bool(CALLBACK* fnItemMakeCustomValue)(void* param, HTREEITEM hItem, PROPERTY_TYPE type, LPWSTR buffer);
typedef bool(CALLBACK* fnItemRButtonPaint)(void* param, HTREEITEM hItem, PROPERTY_TYPE type, HDC hdc, int state, int width, int height);
typedef bool(CALLBACK* fnItemDelUp)(void* param, HTREEITEM hItem, PROPERTY_TYPE type, bool isName);
typedef void(CALLBACK* fnCustomValueName)(void* param, HTREEITEM hItem, PROPERTY_TYPE type, LPWSTR buffer);
typedef void(CALLBACK* fnItemReadError)(void* param, HTREEITEM hItem, PROPERTY_TYPE type, int errCode);
typedef bool(CALLBACK* fnItemBeginDraw)(void* param, HTREEITEM hItem, PROPERTY_TYPE type, HDC hdc);  // ��ʼ�滭
typedef bool(CALLBACK* fnItemEndDraw)(void* param, HTREEITEM hItem, PROPERTY_TYPE type, HDC hdc);    // �����滭

typedef DWORD ARGB;
// ��ɫ��Ϣ
struct PROPERTY_COLOR
{
    LOGFONTW lf;                    // ������Ϣ�ṹ
    ARGB crBackground;              // ������ɫ, �༭��/��Ŀ����
    ARGB crBackParent;              // ����Ŀ����߿հײ��ֵ���ɫ
    ARGB crText;                    // �ı���ɫ
    ARGB crListBorder;              // ��Ͽ��б�߿���ɫ
    ARGB crSelect;                  // �ұ߰�ť�ı�����ɫ
    ARGB crDisabled;                // �ı���ֹ��ɫ
    DWORD ItemHeight;               // ������Ŀ�߶�
    ARGB Reserved[12];              // ����
    int ratioWidth;                 // ���ǧ�ֱ�, ��ߵ�, �ұߵ�ǧ�ֱ� = 1000 - ��ߵ�ǧ�ֱ�
    //PROPERTY_COLOR() { sizeof(*this); }
};


struct IPROPERTY_ITEM
{
    wstr name;          // ������
    wstr value;         // �ı�������ֵ
    wstr text;          // ˵���ı�, 1.1����
    LPARAM param;       // ��Ŀ��ֵ
    int index;          // ��Ͽ�ѡ����
    PROPERTY_TYPE type;
    IPROPERTY_ITEM() { param = 0; index = 0; type = PROPERTY_TYPE_GROUP; }
    // ���ص�ǰ��Ͽ���Ŀ��
    virtual size_t AddComboboxData(LPCWSTR name, LPARAM param) = 0;
    virtual bool DelComboboxData(int index) = 0;
    virtual LPCWSTR GetComboboxText(int index) = 0;
    virtual LPARAM GetComboboxParam(int index) = 0;

    // ��ȡ��Ͽ���������, ��Ҫ�ⲿ�ṩ������
    virtual int GetComboboxArray(IPROPERTY_ITEM**) = 0;
    // ��ȡ��ǰ��Ͽ���Ŀ��
    virtual int GetComboboxCount() = 0;


};

typedef BOOL(CALLBACK* PFN_EnumMap)(LPVOID ptr, LPVOID name, LPVOID value);
typedef struct MAP_DATA_STRUCT
{
    LPVOID name;
    LPVOID value;
}*LPMAP_DATA_STRUCT;

class ICMap
{
public:
    virtual int Addref() = 0;
    virtual int Release() = 0;

    // ��map���ȡָ��KEY��Ӧ��ֵ, ���û�����key, �ڲ��ᴴ��һ��
    virtual LPVOID& GetMapValue(LPVOID key) = 0;
    // ��ָ��key����һ���µ�ֵ
    virtual void SetMapValue(LPVOID key, LPVOID value) = 0;
    // ��map���ѯָ��KEY��Ӧ��ֵ, ���û�����key���ᴴ��
    virtual LPVOID QueryMapValue(LPVOID key) = 0;

    // ��map��ɾ��ָ��KEY��Ӧ��ֵ
    virtual bool erase(LPVOID key) = 0;
    // ��ȡmap��Ա��
    virtual int size() = 0;

    // ���map
    virtual void clear() = 0;


    // ö�ٵ�ǰ���м����ͼ�ֵ, ����д�뻺�����ĳ�Ա��
    // buffer ������, ��Ҫ�ⲿ���뻺����, Ϊ0�򷵻���Ҫ�Ļ���������
    // size = ����������
    virtual int enumValue(LPMAP_DATA_STRUCT buffer, int size) = 0;

    // ö�ٵ�ǰ���м����ͼ�ֵ, ����д�뻺�����ĳ�Ա��
    // pfnCallback = ö�ٵ���Ա����õĻص�����, Ϊ0�򷵻س�Ա��
    // pArg = ���ݵ��ص�������Ĳ���
    virtual int enumValueEx(PFN_EnumMap pfnCallback, LPVOID pArg) = 0;

};


class ICPropertyTree
{
public:
    virtual LPCWSTR className() = 0;
    virtual void SetItemHeight(int height, bool isUpdate = false) = 0;
    virtual int GetItemHeight() = 0;

    virtual int Addref() = 0;
    virtual int Release() = 0;

public:
    virtual bool SetIcon(PROPERTY_ICONTYPE type, LPCVOID pIcon, int size) = 0;
public:
    // �Ƿ�Ϊ����
    virtual bool isGroup(PROPERTY_TYPE type) = 0;
    // �Ƿ��а�ť
    virtual bool isButton(PROPERTY_TYPE type) = 0;
    // �Ƿ���������ť
    virtual bool isButtonCombobox(PROPERTY_TYPE type) = 0;
    // �Ƿ�Ϊ��ֵ
    virtual bool isNumber(PROPERTY_TYPE type) = 0;
    // �Ƿ�Ϊ�ı�, ���ı�, ����ť�ı�������true
    virtual bool isText(PROPERTY_TYPE type) = 0;

    // ����Ϊ��չ����, �ɼ�����ʽ��϶���

    // �Ƿ�Ϊ����ť�ı�
    virtual bool isTextButton(PROPERTY_TYPE type) = 0;
    // �Ƿ�Ϊ���ı�
    virtual bool isLongText(PROPERTY_TYPE type) = 0;
    // �Ƿ�Ϊ���ɱ༭��Ͽ�
    virtual bool isComboboxNoEdit(PROPERTY_TYPE type) = 0;
    // �Ƿ�Ϊ���ɱ༭��Ͽ�
    virtual bool isComboboxNoEditIndex(PROPERTY_TYPE type) = 0;
    // �Ƿ�Ϊ��Ͽ�, ���ɱ༭�ʹ���ŵĶ�������, ��Ҫ�ж϶�Ӧ�ľ�ʹ�ö�Ӧ�ķ���
    virtual bool isCombobox(PROPERTY_TYPE type) = 0;
    // �Ƿ�Ϊ��ɫѡ����
    virtual bool isColorDialog(PROPERTY_TYPE type) = 0;
    // �Ƿ�Ϊ�ļ�ѡ����
    virtual bool isFileDialog(PROPERTY_TYPE type) = 0;
    // �Ƿ�Ϊ����ѡ����
    virtual bool isFontDialog(PROPERTY_TYPE type) = 0;
    // �Ƿ�Ϊ�ֽڼ�
    virtual bool isBinDialog(PROPERTY_TYPE type) = 0;
    // �Ƿ�ΪͼƬѡ����
    virtual bool isPicDialog(PROPERTY_TYPE type) = 0;
    // �Ƿ�Ϊ��Ƶѡ����
    virtual bool isMusicDialog(PROPERTY_TYPE type) = 0;
    // �Ƿ�ΪͼƬ��ѡ����
    virtual bool isImgListDialog(PROPERTY_TYPE type) = 0;
    // �Ƿ�Ϊ���ӿ�
    virtual bool IsLink(PROPERTY_TYPE type) = 0;
    // �Ƿ�Ϊѡ���
    virtual bool IsCheckBox(PROPERTY_TYPE type) = 0;
    // �Ƿ�Ϊ3̬ѡ���
    virtual bool IsCheckBox3(PROPERTY_TYPE type) = 0;
    // �Ƿ�Ϊ��ѡ��
    virtual bool IsRadio(PROPERTY_TYPE type) = 0;
    // �Ƿ�Ϊ�Զ���ѡ��
    virtual bool IsAutoRadio(PROPERTY_TYPE type) = 0;
    // �Ƿ�Ϊ����
    virtual bool IsSwitch(PROPERTY_TYPE type) = 0;
    // �Ƿ�Ϊѡ���������, ѡ���/3̬ѡ���, ��ѡ��, �Զ���ѡ��, ����, ������ѡ������
    virtual bool IsCheckType(PROPERTY_TYPE type) = 0;

    // 1=�ֽ�,2=������,3=����,4=������,5=С��,6=˫����С��, ���᷵������ֵ
    virtual int  GetNumberType(PROPERTY_TYPE type) = 0;
    // �Ƿ��з���
    virtual bool isUnsigned(PROPERTY_TYPE type) = 0;
    // �Ƿ�ɱ༭
    virtual bool isEdit(PROPERTY_TYPE type) = 0;
    // �Ƿ��б��
    virtual bool isIndex(PROPERTY_TYPE type) = 0;
    // �Ƿ��Զ�������
    virtual bool isCustom(PROPERTY_TYPE type) = 0;

    // �Ƿ��ֹ
    virtual bool isDisabled(PROPERTY_TYPE type) = 0;
    // �Ƿ����
    virtual bool isEnabled(PROPERTY_TYPE type) = 0;

    // ����Ӧ������Ϣ, ���Ұ���Ϣת����������
    virtual bool isNoMouseWheel(PROPERTY_TYPE type) = 0;


public:
    virtual HWND create(DWORD dwExStyle, LPCWSTR lpszTitle, DWORD dwStyle, DWORD dwExStyleTree,
        int x, int y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR ID = 0, LPVOID lpParam = 0) = 0;

public:
    // д���ֽڼ�, ��������ָ��ͳ���, �ڲ���ת�������Եĸ�ʽ
    virtual bool SetBinData(IPROPERTY_ITEM& data, const void* pData, const size_t size) = 0;

    // д���ֽڼ�, �������ݱ����������Եĸ�ʽ
    virtual bool SetBinData(IPROPERTY_ITEM& data, const void* pData) = 0;

    // д������, �����ŵ����ݴ����ǹ̶���, �����ṹ, LOGFONTW + CHOOSEFONTW, �ڲ����޸������С/�ַ��߶�
    virtual bool SetFontData(IPROPERTY_ITEM& data, const LPLOGFONTW lf) = 0;

    // д������, ����CHOOSEFONTW�ṹָ��, �ڲ����޸������С/�ַ��߶�
    // CHOOSEFONTW ָ��� LOGFONT�ṹ��ֵ�Ļ�Ҳ�������ṹ������������
    virtual bool SetFontData(IPROPERTY_ITEM& data, const LPCHOOSEFONTW pcf) = 0;

    // д������, ����gdi����ָ��, �ڲ����޸������С/�ַ��߶�
    virtual bool SetFontData(IPROPERTY_ITEM& data, HFONT hFont) = 0;


    // д������, �����ŵ����ݴ����ǹ̶���, �����ṹ, LOGFONTW + CHOOSEFONTW
    virtual bool SetFontData(IPROPERTY_ITEM& data, const wchar_t* name = L"΢���ź�", int lfHeight = -12,
        bool lfWeight = false, bool lfItalic = false, bool lfUnderline = false, bool lfStrikeOut = false) = 0;

    // ��ȡ��������, �ڲ��и��� LPCHOOSEFONTW �ṹָ���logfont��ַ
    virtual LPLOGFONTW GetFontData(IPROPERTY_ITEM& data, LPCHOOSEFONTW* pcf) = 0;

    // ������Ŀ�����ȡ��Ŀ�����Ŀ������ָ��
    virtual IPROPERTY_ITEM* GetItemData(HTREEITEM hItem) = 0;


    // ���öԻ�������, Ϊ0�����ö�Ӧ������
    // type = Ҫ���õĶԻ�������, Ŀǰֻ֧�� PROPERTY_FLAG_FILE�ļ�ѡ����, PROPERTY_FLAG_BIN�ֽڼ�ѡ����
    // filter = ������, ��ʽ����MP3�ļ�(*.mp3)|*.mp3\0ý���ļ�(*.mpg)|*.mpg��
    // defName = Ĭ���ļ���
    // title = ���ڱ���
    // path = ����Ŀ¼
    // notChengedPath = ���ı�Ŀ¼, ��Ҫ���ݱ���ָ��, ָ��Ϊ0������
    virtual bool SetOpenDialogProperty(PROPERTY_TYPE type, LPCWSTR filter = 0, LPCWSTR title = 0, LPCWSTR defName = 0, LPCWSTR path = 0, bool* noChangePath = 0) = 0;

    // ��ȡ�Ի�������, Ϊ0�򲻻�ȡ��Ӧ������
    // type = Ҫ���õĶԻ�������, Ŀǰֻ֧�� PROPERTY_FLAG_FILE�ļ�ѡ����, PROPERTY_FLAG_BIN�ֽڼ�ѡ����
    // filter = ������, ��ʽ����MP3�ļ�(*.mp3)|*.mp3\0ý���ļ�(*.mpg)|*.mpg��
    // defName = Ĭ���ļ���
    // title = ���ڱ���
    // path = ����Ŀ¼
    // notChengedPath = ���ı�Ŀ¼
    virtual bool GetOpenDialogProperty(PROPERTY_TYPE type, LPCWSTR* filter = 0, LPCWSTR* title = 0, LPCWSTR* defName = 0, LPCWSTR* path = 0, bool* noChangePath = 0) = 0;



public:
    // ����һ������, ���������Ŀ�ľ��
    virtual HTREEITEM InsertGroup(LPCWSTR name, bool isExpand, LPCWSTR text = 0, LPARAM param = 0) = 0;

    // ����ָ����Ŀ��ֵ, �ֽڼ����ͺ�������һ��
    virtual bool SetItemValue(IPROPERTY_ITEM& data, const void* value) = 0;

    // ����һ���ı�����������, ����ֵֻ�����ı�
    // hParent = InsertGroup() ���ص�ֵ, ��Ҫ���뵽�ĸ�������
    // name = ������, ���������к����޸�
    // value = ����ֵ
    // text = ��ע�ı�
    // param = ��Ŀ��ֵ
    virtual HTREEITEM InsertItem(HTREEITEM hParent, LPCWSTR name, LPCWSTR value = 0, PROPERTY_TYPE type = PROPERTY_TYPE_TEXT, LPCWSTR text = 0, LPARAM param = 0) = 0;

    // ����˵���ı�
    virtual bool SetItemRemarks(HTREEITEM hItem, LPCWSTR value) = 0;
    // ��ȡ˵���ı�
    virtual LPCWSTR GetItemRemarks(HTREEITEM hItem) = 0;
    // ɾ��ָ����Ŀ
    virtual BOOL DeleteItem(HTREEITEM hItem) = 0;

    // ȡָ����Ŀ��������
    virtual LPCWSTR GetItemName(HTREEITEM hItem) = 0;
    virtual bool SetItemName(HTREEITEM hItem, LPCWSTR name) = 0;

    // ȡָ����Ŀ������ֵ, ʧ�ܷ���0, �ֽڼ�������Ҫ��ǰ����� {1,0,0,0,����} 8���ֽ�, ��������һ��
    virtual LPCWSTR GetItemValue(HTREEITEM hItem) = 0;

    // ����ָ����Ŀ��ֵ, �ֽڼ����ͺ�������һ��
    virtual bool SetItemValue(HTREEITEM hItem, LPCWSTR value) = 0;

    // ȡָ����Ŀ����Ŀ��ֵ, InsertItem/SetItemValue ʱ��д��ֵ
    virtual LPARAM GetItemParam(HTREEITEM hItem) = 0;

    // ����ָ����Ŀ��ֵ
    virtual bool SetItemParam(HTREEITEM hItem, LPARAM param) = 0;

    // ����Ͽ���Ŀ������Ŀ����, ���ú������Զ������Ͽ�����
    // ���óɹ����ر������Ͽ���Ŀ��, ʧ�ܷ���-1
    virtual size_t AddComboboxData(HTREEITEM hItem, LPCWSTR name, LPARAM param = 0) = 0;

    // ɾ����Ͽ�����, index = ɾ�����±�, ��0��ʼ, �±�Խ��/������Ͽ����ͷ���false
    virtual bool DelComboboxData(HTREEITEM hItem, int index) = 0;

    // ȡָ����Ŀ����Ͽ���Ŀ��, ʧ�ܷ���-1, ������Ͽ����ͻ�����Ŀ�����Ч������-1
    virtual size_t GetComboboxDataCount(HTREEITEM hItem) = 0;

    // ȡָ����Ŀ����Ͽ���Ŀ�ı�, ʧ�ܷ��ؿ��ı�, ������Ͽ�����, �±�Խ��, �����Ч�����ؿ��ı�
    virtual LPCWSTR GetComboboxData(HTREEITEM hItem, int index) = 0;

    // ����ָ����Ŀ����Ͽ���Ŀ�ı�, ʧ�ܷ���false, ������Ͽ�����, �±�Խ��, �����Ч������false
    virtual bool SetComboboxData(HTREEITEM hItem, int index, LPCWSTR value) = 0;

    // ��ȡ��Ͽ�����ѡ����, ʧ�ܷ���-1
    virtual int GetComboboxIndex(HTREEITEM hItem) = 0;

    // ������Ͽ�����ѡ����, ������0��ʼ
    virtual bool SetComboboxIndex(HTREEITEM hItem, int index) = 0;

    // ȡָ����Ŀ����Ͽ���Ŀ��ֵ
    virtual LPARAM GetComboboxParam(HTREEITEM hItem, int index) = 0;

    // ����ָ����Ŀ����Ͽ���Ŀ��ֵ
    virtual bool SetComboboxParam(HTREEITEM hItem, int index, LPARAM param) = 0;

    // ����ָ����ĿΪָ������, ��Ͽ�������Ҫ��������
    virtual void SetItemType(HTREEITEM hItem, PROPERTY_TYPE type) = 0;

    // ȡָ����Ŀ����
    virtual PROPERTY_TYPE GetItemType(HTREEITEM hItem) = 0;

    // ���ÿ��, ǧ�ֱ�, ��С100, ���900, �ؼ�δ����ǰҲ���Ե���
    // isLeft = �Ƿ�Ϊ�������, ����һ�ߺ���һ�߻�һ�����
    // value = 100-900֮���ֵ
    // isUpdate = �Ƿ��ػ�
    virtual void SetWidthRatio(bool isLeft, int value, bool isUpdate = false) = 0;

    // ����ǰ�����������ݱ�����ֽڼ�����, ���صĵ�ַ��ʹ��ʱ��Ҫ���� free() �ͷ�
    // isColor = �Ƿ񱣴���ɫ������Ϣ
    virtual LPBYTE save(DWORD* size, bool isColor, bool isList) = 0;

    // ���ڴ������������Ŀ����
    virtual bool load(const void* data, DWORD size, bool isUpdate) = 0;

    // ������ɫ��Ϣ
    virtual void SetColor(const PROPERTY_COLOR* color, bool isUpdate) = 0;
    virtual int GetColor(PROPERTY_COLOR* color) = 0;
    // Ĭ����ɫ, 0=��ɫ���, 1=��ɫ���
    virtual int DefColor(int type, bool isUpdate) = 0;

    virtual int SetItemFont(const LPLOGFONTW lf, bool isUpdate) = 0;
    // Ĭ��Ϊ΢���ź�, �����С -12, û����ʽ
    // lfItalic = б��,  lfUnderline = �»���, lfStrikeOut = ɾ����
    virtual int SetItemFont(bool isUpdate, const wchar_t* name = L"΢���ź�", int size = -12, bool lfItalic = false, bool lfUnderline = false, bool lfStrikeOut = false) = 0;

    virtual int SetColorBackground(ARGB cr, bool isUpdate = false) = 0;
    virtual int SetColorBackParent(ARGB cr, bool isUpdate = false) = 0;
    virtual int SetColorText(ARGB cr, bool isUpdate = false) = 0;
    virtual int SetColorListBorder(ARGB cr, bool isUpdate = false) = 0;
    virtual int SetColorSelect(ARGB cr, bool isUpdate = false) = 0;
    virtual int SetColorDisabled(ARGB cr, bool isUpdate = false) = 0;
    virtual void SetIsFillColorBack(bool isFill) = 0;

    virtual bool SetItemDisabledState(HTREEITEM hItem, bool isDisabled) = 0;
    // ��ֹ��Ŀ
    virtual bool DisabledItem(HTREEITEM hItem) = 0;
    // ������Ŀ
    virtual bool EnabledItem(HTREEITEM hItem) = 0;
    // ��Ŀ�Ƿ��ֹ
    virtual bool IsItemDisabled(HTREEITEM hItem) = 0;
    // ��Ŀ�Ƿ�����
    virtual bool IsItemEnabled(HTREEITEM hItem) = 0;

    virtual bool clear() = 0;

    // �����Ի���, ���ݵ�ǰ��Ŀ����̽���Ӧ�Ի���, �ᴥ����ť����ص�
    virtual void popDialog() = 0;

    // ��ȡ���Կ����
    virtual void GetPropertyItemRect(HTREEITEM hItem, RECT* prc, PROPERTY_RECT type) = 0;

    virtual LPBYTE GetExpand_Open(int* size) = 0;
    virtual LPBYTE GetExpand_Close(int* size) = 0;
    virtual bool SetExpand_Open(const void* pData, int size) = 0;
    virtual bool SetExpand_Close(const void* pData, int size) = 0;

public:
    
    
    // ��Ŀ���Ա��ı�, Ϊ����ȡ���ҽ�����¼�, �¼���������, ����1�Ǵ����¼�����Ŀ���, ����2����Ŀ������
    virtual void OnPropertyChanged(fnPropertyChanged pfn = 0, void* param = 0) = 0;

    // ��Ŀѡ��ı�, Ϊ����ȡ���ҽ�����¼�, �¼���������, ����1�Ǵ����¼�����Ŀ���, ����2�����ĸ�λ�õ��, -1=չ��������ť, 1=������, 2=����ֵ
    virtual void OnItemChanged(fnItemChanged pfn = 0, void* param = 0) = 0;
    
    // ��Ŀ����ֵ�����˻س���, �ص����������Ƿ�����, Ϊ����ȡ���ҽ�����¼�, ����ص���������true�򲻶԰��»س�������һ������
    // �¼���������, ����1�Ǵ����¼�����Ŀ���, ����2����Ŀ������
    virtual void OnPropertyReturn(fnPropertyReturn pfn = 0, void* param = 0) = 0;

    // ��Ͽ���Ŀ������, �ص����������Ƿ�����, ����ص���������true�򲻶�����������һ������
    // count = ��Ͽ���Ŀ����, pos = �������븺�����¹���, �������Ϲ���, key = ����ʱ�������ĸ�����, lParam = �������
    virtual void OnComboboxMouseWheel(fnComboboxMouseWheel pfn = 0, void* param = 0) = 0;

    // ��Ͽ���Ŀ�ı�, �ص����������Ƿ�����, ����ص���������true�򲻻�ı���Ŀ
    // index = Ҫ�ı����Ŀ����, ����false����Ͽ�����ѡ��������������
    virtual void OnComboboxChanged(fnComboboxChanged pfn = 0, void* param = 0) = 0;

    // ��Ͽ�������ͷ������, ����̽���б�, �ص����������Ƿ�����, ����ص���������true�򲻻ᵯ���б�
    virtual void OnDropDown(fnDropWown pfn = 0, void* param = 0) = 0;

    // ���԰�ť������, �ص����������Ƿ�����, ����ص���������true�򲻻ᵯ�����õĶԻ���
    virtual void OnItemButtonClick(fnItemButtonClick pfn = 0, void* param = 0) = 0;

    // ��Ŀ���Ҽ�����, �����Ƿ�����, ���غ��Ҽ�����ı����, ���ᴥ������
    virtual void OnItemRButtonClick(fnItemRButtonClick pfn = 0, void* param = 0) = 0;

    // ��Ͽ�˫���༭��, �ص����������Ƿ�����, ����ص���������true�򲻶�˫������һ������
    // index = ˫��ʱ��Ͽ������, count = ��Ͽ���Ŀ��
    virtual void OnComboboxLDoubleClick(fnComboboxLDoubleClick pfn = 0, void* param = 0) = 0;
    
    // �༭��˫��, �ص����������Ƿ�����, ����ص���������true��ȫѡ�༭��
    virtual void OnEditLDoubleClick(fnEditLDoubleClick pfn = 0, void* param = 0) = 0;

    // ��Ŀ�����ַ�, �ص����������Ƿ�����, ����ص���������true��ʹ�� pRet ���ص�ֵ��Ϊ����ֵ
    // ch = ������ַ�, pRet = ����ֵָ��, ��Ҫ�ѷ���ֵд������
    virtual void OnItemChar(fnItemChar pfn = 0, void* param = 0) = 0;

    // �����Զ�������ֵ����ʾ����, ��������, ��Ŀ���, ��Ŀ����, ��ʾ�ı�������, ���100���ַ�
    virtual void OnItemMakeCustomValue(fnItemMakeCustomValue pfn = 0, void* param = 0) = 0;

    // �ұ�С��ť���ػ�, ����true���ڲ������л滭, ��Ҫ�û��ڻص�����л滭����
    // hdc = ��ť�滭dc, �����ڴ�DC, state = ��ǰ״̬, STATE::HOT, STATE::DOWN, width = ��ť���, height = ��ť�߶�
    virtual void OnItemRButtonPaint(fnItemRButtonPaint pfn = 0, void* param = 0) = 0;

    // del���ſ�, ɾ����ǰ������, ����������һ�а��µ�Ĭ�ϴ��������ֵ, �������������µ�ֻ���˫����򿪶Ի������Ŀ
    // isName = �Ƿ�����������һ�а��°���
    virtual void OnItemKeyDelUp(fnItemDelUp pfn = 0, void* param = 0) = 0;

    // �Զ�����������
    // isName = �Ƿ�����������һ�а��°���
    virtual void OnCustomValueName(fnCustomValueName pfn = 0, void* param = 0) = 0;

    // ��Ŀ��ȡ����ʧ��, �ֽڼ�, ͼƬ, ��Ƶ, ͼƬ����Щ....
    virtual void OnItemReadError(fnItemReadError pfn = 0, void* param = 0) = 0;
    // �����滭, �ڲ��滭ǰ�ȵ����û��Ļ滭
    virtual void OnItemBeginDraw(fnItemBeginDraw pfn = 0, void* param = 0) = 0;

    // �滭����, �ڲ��滭�Ѿ�����, �û�������������Ͻ��ж��λ滭
    virtual void OnItemEndDraw(fnItemEndDraw pfn = 0, void* param = 0) = 0;

    virtual bool Expand(HTREEITEM hItem, BOOL b) = 0;
    virtual bool Expand(HTREEITEM hItem) = 0;
    virtual bool IsExpand(HTREEITEM hItem) = 0;


    virtual DWORD GetItemCount() = 0;
    virtual void InvalidateItem(HTREEITEM hItem) = 0;

    // ������ѡ����
    virtual bool SetIndex(HTREEITEM hItem) = 0;

    // ȡ������Ŀ��
    virtual unsigned int GetVisibleCount() = 0;

    // ȡ��һ����Ŀ
    virtual HTREEITEM GetFirstItem() = 0;

    // ȡ����Ŀ
    virtual HTREEITEM GetRootItem() = 0;

    // ȡ����Ŀ, ��Ŀ���Ϊ0��ȡ��ǰѡ����Ŀ���
    virtual HTREEITEM GetChild(HTREEITEM hItem = 0) = 0;

    // ȡ��һ���ֵ���, ��Ŀ���Ϊ0��ȡ��ǰѡ����Ŀ���
    virtual HTREEITEM GetNextSibling(HTREEITEM hItem = 0) = 0;

    // ȡ��һ���ֵ���, ��Ŀ���Ϊ0��ȡ��ǰѡ����Ŀ���
    virtual HTREEITEM GetPrevSibling(HTREEITEM hItem = 0) = 0;

    // ȡ����Ŀ, ��Ŀ���Ϊ0��ȡ��ǰѡ����Ŀ���
    virtual HTREEITEM GetParent(HTREEITEM hItem = 0) = 0;

    // ȡ��һ��������Ŀ
    virtual HTREEITEM GetFirstVisible() = 0;

    // ȡ��һ������, ��Ŀ���Ϊ0��ȡ��ǰѡ����Ŀ���
    virtual HTREEITEM GetNextVisible(HTREEITEM hItem = 0) = 0;

    // ȡ��һ������, ��Ŀ���Ϊ0��ȡ��ǰѡ����Ŀ���
    virtual HTREEITEM GetPrevVisible(HTREEITEM hItem = 0) = 0;

    // ȡ����ѡ����
    virtual HTREEITEM GetSelection() = 0;

    // ȡ����ѡ����
    virtual HTREEITEM GetIndex() = 0;
    // ȡ����Ŀ
    virtual HTREEITEM GetRoot() = 0;

    // ȡ���һ��������Ŀ
    virtual HTREEITEM GetLastVisible() = 0;

    // ȡ��һ����Ŀ, ��Ŀ���Ϊ0��ȡ��ǰѡ����Ŀ���
    virtual HTREEITEM GetNextItem(HTREEITEM hItem = 0, int code = TVGN_NEXT) = 0;



    // ȡ��һ��ѡ����, ��Ŀ���Ϊ0��ȡ��ǰѡ����Ŀ���
    virtual HTREEITEM GetNextSelectedItem(HTREEITEM hItem = 0) = 0;

    virtual HWND GetWindow() = 0;
    // ȡ����Ŀ��
    virtual int GetSubCount(HTREEITEM hItem) = 0;
    // ȡ��Ŀ����, isTitle = �Ƿ�ֵ��������
    virtual RECT GetItemRect(HTREEITEM hItem, bool isTitle = FALSE) = 0;
    // �����м������ߵ�λ��
    virtual int calcMidLine(int cxClient = 0) = 0;



    // ��֤��ʾ
    virtual bool EnsureVisible(HTREEITEM hItem) = 0;

    virtual bool Move(int x, int y, int cx, int cy, bool bRepaint = true) = 0;

public:
    virtual void free(void* ptr) = 0;

};

#ifdef _EXPORTING
#define API_DECLSPEC __declspec(dllexport)
#else
#define API_DECLSPEC __declspec(dllimport)
#endif

typedef ICPropertyTree*(__cdecl* pfn_CProperty_Create)();
typedef ICMap*(__cdecl* pfn_map_Create)(bool isHash);
extern "C" API_DECLSPEC ICPropertyTree* CProperty_Create();
extern "C" API_DECLSPEC ICMap* CMap_Create(bool isHash);
