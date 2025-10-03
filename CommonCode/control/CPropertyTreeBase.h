#pragma once
#include <control/CTreeView.h>
#include <control/CEdit.h>
#include <control/CListBox.h>
#include <control/CStatic.h>
#include <commdlg.h>
#include <vector>
#include <map>
#include <CArray.h>
#include "CPropertyTree_virtual.h"
#include <d2d/d2d.h>



class CPropertyTreeBase
{
public:
    virtual HWND create(DWORD dwExStyle, LPCWSTR lpszTitle, DWORD dwStyle, DWORD dwExStyleTree,
        int x, int y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR ID = 0, LPVOID lpParam = 0) = 0;
public:
    // ����һ������, ���������Ŀ�ľ��
    virtual HTREEITEM InsertGroup(LPCWSTR name, bool isExpand, LPARAM param = 0) = 0;

    // ����һ���ı�����������, ����ֵֻ�����ı�
    // hParent = InsertGroup() ���ص�ֵ, ��Ҫ���뵽�ĸ�������
    // name = ������, ���������к����޸�
    // value = ����ֵ
    virtual HTREEITEM InsertItem(HTREEITEM hParent, LPCWSTR name, LPCWSTR value) = 0;

    // ����һ���ı�����������, ����ֵֻ�����ı�
    // hParent = InsertGroup() ���ص�ֵ, ��Ҫ���뵽�ĸ�������
    // name = ������, ���������к����޸�
    virtual HTREEITEM InsertItem(HTREEITEM hParent, LPCWSTR name, PROPERTY_TYPE type = PROPERTY_TYPE_TEXT) = 0;

    // ȡָ����Ŀ������ֵ, ʧ�ܷ���0
    virtual LPCWSTR GetItemValue(HTREEITEM hItem) = 0;

    // ����ָ����Ŀ��ֵ
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
    // Ĭ����ɫ, 0=��ɫ���, 1=��ɫ���
    virtual void DefColor(int type, bool isUpdate) = 0;

    virtual void SetItemFont(const LPLOGFONTW lf, bool isUpdate) = 0;
    // Ĭ��Ϊ΢���ź�, �����С -12, û����ʽ
    // lfItalic = б��,  lfUnderline = �»���, lfStrikeOut = ɾ����
    virtual void SetItemFont(bool isUpdate, const wchar_t* name = L"΢���ź�", int size = -12, bool lfItalic = false, bool lfUnderline = false, bool lfStrikeOut = false) = 0;

    virtual void SetColorBackground(ARGB cr, bool isUpdate = false) = 0;
    virtual void SetColorBackParent(ARGB cr, bool isUpdate = false) = 0;
    virtual void SetColorText(ARGB cr, bool isUpdate = false) = 0;
    virtual void SetColorListBorder(ARGB cr, bool isUpdate = false) = 0;
    virtual void SetColorSelect(ARGB cr, bool isUpdate = false) = 0;

    virtual bool clear() = 0;

    // �����Ի���, ���ݵ�ǰ��Ŀ����̽���Ӧ�Ի���, �ᴥ����ť����ص�
    virtual bool popDialog() = 0;

    // ��ȡ��ǰ���Կ�Ĵ��ھ��
    virtual HWND GetWindow() = 0;
};

