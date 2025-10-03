#pragma once
#include "WndBase.h"

// �Զ������
class CCustomControl : public CControlBase
{
private:
    LPWSTR m_szClassName;
    LPWSTR m_szSuperClassName;

public:
    CCustomControl() : m_szClassName(0), m_szSuperClassName(0), CControlBase() { }
    virtual ~CCustomControl() {
        if (m_szClassName)delete[] m_szClassName;
        if (m_szSuperClassName) delete[] m_szSuperClassName;
    }
public:
    virtual inline LPCWSTR className() { return m_szClassName; }
    virtual inline LPCWSTR SuperClassName() { return m_szSuperClassName; }

    // ���ô�������, �������ò��ܴ������ڳɹ�
    // ʹ�������������ע�ᴰ��
    virtual inline bool SetClassName(LPCWSTR szClassName) {
        if (!szClassName || !szClassName[0])return false;
        if (m_szClassName)delete[] m_szClassName;
        size_t len = wcslen(szClassName) + 1;
        m_szClassName = new wchar_t[len];
        memcpy(m_szClassName, szClassName, len * sizeof(wchar_t));
        return true;
    }

    // ���໯������, ����Ϊ��
    // ������Ϣ�����ĸ���������, Ĭ��Ϊ��, Ϊ�������Ĭ�ϴ���
    // �����д Button, �������button�����������
    virtual inline bool SetSuperClassName(LPCWSTR szSuperClassName) {
        if (!szSuperClassName || !szSuperClassName[0])return false;
        if (m_szSuperClassName)delete[] m_szSuperClassName;
        size_t len = wcslen(szSuperClassName) + 1;
        m_szSuperClassName = new wchar_t[len];
        memcpy(m_szSuperClassName, szSuperClassName, len * sizeof(wchar_t));
        return true;
    }

};

