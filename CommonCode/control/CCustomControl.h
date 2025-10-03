#pragma once
#include "WndBase.h"

// 自定义组件
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

    // 设置窗口类名, 必须设置才能创建窗口成功
    // 使用这个类名进行注册窗口
    virtual inline bool SetClassName(LPCWSTR szClassName) {
        if (!szClassName || !szClassName[0])return false;
        if (m_szClassName)delete[] m_szClassName;
        size_t len = wcslen(szClassName) + 1;
        m_szClassName = new wchar_t[len];
        memcpy(m_szClassName, szClassName, len * sizeof(wchar_t));
        return true;
    }

    // 超类化的类名, 可以为空
    // 窗口消息发往哪个窗口类里, 默认为空, 为空则调用默认处理
    // 如果填写 Button, 则本类具有button类的所有特征
    virtual inline bool SetSuperClassName(LPCWSTR szSuperClassName) {
        if (!szSuperClassName || !szSuperClassName[0])return false;
        if (m_szSuperClassName)delete[] m_szSuperClassName;
        size_t len = wcslen(szSuperClassName) + 1;
        m_szSuperClassName = new wchar_t[len];
        memcpy(m_szSuperClassName, szSuperClassName, len * sizeof(wchar_t));
        return true;
    }

};

