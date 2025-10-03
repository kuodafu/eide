#pragma once
#include "WndBase.h"
#include <windowsx.h>
class CCombobox : public CControlBase
{
protected:
    virtual inline LPCWSTR SuperClassName() { return L"combobox"; };

public:
    CCombobox() :CControlBase() { ; }
    virtual ~CCombobox() { ; }
public:
    virtual inline LPCWSTR className() { return CLASS_COMBOBOX; };

public:
    inline int AddString(LPCWSTR lpsz, LPARAM param = 0, int index = -1) {
        int ret = ((int)(DWORD)Send(CB_INSERTSTRING, (WPARAM)(int)index, (LPARAM)lpsz));
        if (ret >= 0 && param)
        {
            Send(CB_SETITEMDATA, ret, param);
        }
        return ret;
    }
    inline bool DelString(int index) {
        return ((int)(DWORD)Send(CB_DELETESTRING, (WPARAM)(int)(index), 0L)) != CB_ERR;
    }
    inline bool DelAll() {
        return Send(CB_RESETCONTENT, 0, 0) == CB_OKAY;
    }
    inline bool SetSel(int index) {
        int ret = (int)Send(CB_SETCURSEL, index, 0);
        return ret != index && ret != CB_ERR;
    }
    inline int  GetSel() {
        return (int)Send(CB_GETCURSEL, 0, 0);
    }
    inline LPARAM GetParam(int index) {
        return Send(CB_GETITEMDATA, index, 0);
    }
    inline LPARAM SetParam(int index, LPARAM param) {
        return Send(CB_SETITEMDATA, index, param);
    }
    inline int GetCount() {
        return (int)Send(CB_GETCOUNT, 0, 0);
    }
    inline wstr GetItemText(int index) {
        const int len = (int)Send(CB_GETLBTEXTLEN, (WPARAM)(int)(index), 0) + 1;
        if (len == 1)return wstr();
        wstr ret(len);
        const int r = (int)Send(CB_GETLBTEXT, (WPARAM)(int)(index), (LPARAM)ret.data());
        if (r == CB_ERR)return wstr();
        ret.resize(r);
        return ret;
    }
    
};

