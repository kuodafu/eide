#pragma once
#include "WndBase.h"
class CHotKet : public CControlBase
{
protected:
    virtual inline LPCWSTR SuperClassName() { return HOTKEY_CLASS; }

public:
    CHotKet() :CControlBase() { ; }
    virtual ~CHotKet() { ; }
public:
    virtual inline LPCWSTR className() { return HOTKEY_CLASS; }

public:

    // 返回键代码, 参数参考返回功能键状态
    inline int GetHotKey(int* fsModifiers) {
        int ret = (int)Send(HKM_GETHOTKEY, 0, 0);
        short key = LOWORD(ret);
        ret = LOBYTE(key);
        if (fsModifiers)*fsModifiers = HIBYTE(key);
        return ret;
    }
    // 设置热键
    inline void SetHotKey(int key, int fsModifiers) {
        Send(HKM_SETHOTKEY, MAKELONG(MAKEWORD(key, fsModifiers), 0), 0);
    }
};

