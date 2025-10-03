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

    // ���ؼ�����, �����ο����ع��ܼ�״̬
    inline int GetHotKey(int* fsModifiers) {
        int ret = (int)Send(HKM_GETHOTKEY, 0, 0);
        short key = LOWORD(ret);
        ret = LOBYTE(key);
        if (fsModifiers)*fsModifiers = HIBYTE(key);
        return ret;
    }
    // �����ȼ�
    inline void SetHotKey(int key, int fsModifiers) {
        Send(HKM_SETHOTKEY, MAKELONG(MAKEWORD(key, fsModifiers), 0), 0);
    }
};

