
#pragma once

#include <windows.h>

typedef bool(__stdcall* PFN_DLGPROC)(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam, INT_PTR* pRet);
typedef struct INPUTBOXSTRUCT
{
    HWND hParent;                   // ����򸸴���
    const wchar_t* text;            // �������ʾ����
    const wchar_t* caption;         // ��������
    const wchar_t* defText;         // �����Ĭ���ı�
    const wchar_t* pszOkButton;     // �����ȷ�ϰ�ť�ı���
    const wchar_t* pszCancelButton; // �����ȡ����ť�ı���
    PFN_DLGPROC lpDialogFunc;       // �������Ϣ�ص�����, ����true�����ڲ�����, ����false�ڲ�Ĭ�ϴ���, pRet�Ƿ���trueʱ�ڲ����ص�ֵ
    LPARAM dwInitParam;             // ���Ӳ���, �ᴫ�ݵ� WM_INITDIALOG ��Ϣ�µ� lParam ����
}*LPINPUTBOXSTRUCT, * PINPUTBOXSTRUCT;

namespace __input_namespace {

    inline INT_PTR CALLBACK DlgProc_InputBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
    {
        const LONG_PTR IDC_EDIT_STATIC_INPUTBOX = 1000;
        const LONG_PTR IDC_EDIT_INPUT = 1001;
        UNREFERENCED_PARAMETER(lParam);
        if (message == WM_INITDIALOG)   // �Ի����ʼ��, ��ָ�����ý�������
            SetWindowLongPtrW(hDlg, GWLP_USERDATA, (LONG_PTR)lParam);

        LPINPUTBOXSTRUCT pData = (LPINPUTBOXSTRUCT)GetWindowLongPtrW(hDlg, GWLP_USERDATA);  // ÿ����Ϣ��Ҫ��ȡ���ָ��
        if (!pData) return false;
        INT_PTR ret = 0;
        if (pData->lpDialogFunc && message != WM_INITDIALOG)    // ������ǶԻ����ʼ���Ǿ͵����û��Ļص�����, �����û��������ڲ�����
        {
            if (pData->lpDialogFunc(hDlg, message, wParam, lParam, &ret))
                return ret;
        }
        switch (message)
        {
        case WM_INITDIALOG:
        {
            RECT rc = { 0 };
            GetWindowRect(pData->hParent, &rc);
            if (rc.right == 0 || rc.bottom == 0)
            {
                rc.left = 0;
                rc.top = 0;
                rc.right = GetSystemMetrics(SM_CXSCREEN);
                //rc.bottom = GetSystemMetrics(SM_CYSCREEN);
                rc.bottom = GetSystemMetrics(SM_CYMAXIMIZED);
            }

            const int inputWidth = 405;
            const int inputHeight = 200;
            int width = rc.right - rc.left;
            int height = rc.bottom - rc.top;
            const int inputLeft = rc.left + (width - inputWidth) / 2;
            const int inputTop = rc.top + (height - inputHeight) / 2;

            rc = { inputLeft, inputTop, inputLeft + inputWidth, inputTop + inputHeight };

            HMONITOR hMonitor = MonitorFromWindow(pData->hParent, MONITOR_DEFAULTTONEAREST);
            if (hMonitor)
            {
                MONITORINFO mi = { 0 };
                mi.cbSize = sizeof(MONITORINFO);
                if (GetMonitorInfoW(hMonitor, &mi))
                {
                    RECT& rcWork = mi.rcWork;
                    if (rc.left < rcWork.left)
                        rc.left = rcWork.left + 10, rc.right = rc.left + inputWidth;
                    if (rc.top < rcWork.top)
                        rc.top = rcWork.top + 10, rc.bottom = rc.top + inputHeight;
                    if (rc.right > rcWork.right)
                        rc.right = rcWork.right - 10, rc.left = rc.right - inputWidth;
                    if (rc.bottom > rcWork.bottom)
                        rc.bottom = rcWork.bottom - 10, rc.top = rc.bottom - inputHeight;
                }

            }
            MoveWindow(hDlg, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, false);
            SetWindowLongPtrW(hDlg, GWLP_USERDATA, (LONG_PTR)pData);

            if (!pData->caption) pData->caption = L"�������ı�";
            if (!pData->text) pData->text = L"�������ı�";

            static HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
            DWORD style = WS_TABSTOP | WS_DISABLED | WS_VISIBLE | WS_CHILD | ES_READONLY | ES_MULTILINE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
            DWORD styleEx = WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE;

            HWND hChild = CreateWindowExW(styleEx, L"edit", pData->text, style, 11, 11, 368, 60, hDlg, (HMENU)IDC_EDIT_STATIC_INPUTBOX, 0, 0);
            SendMessageW(hChild, WM_SETFONT, (WPARAM)hFont, 0);

            style = WS_TABSTOP | WS_VISIBLE | WS_GROUP | WS_CHILD | ES_AUTOHSCROLL | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
            hChild = CreateWindowExW(styleEx, L"edit", 0, style, 11, 85, 368, 20, hDlg, (HMENU)IDC_EDIT_INPUT, 0, 0);
            SendMessageW(hChild, WM_SETFONT, (WPARAM)hFont, 0);


            style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON;
            styleEx = WS_EX_NOPARENTNOTIFY;
            hChild = CreateWindowExW(styleEx, L"button", pData->pszOkButton, style, 93, 119, 90, 31, hDlg, (HMENU)IDOK, 0, 0);
            SendMessageW(hChild, WM_SETFONT, (WPARAM)hFont, 0);

            style = WS_TABSTOP | WS_VISIBLE | WS_CHILD;
            hChild = CreateWindowExW(styleEx, L"button", pData->pszCancelButton, style, 206, 119, 90, 31, hDlg, (HMENU)IDCANCEL, 0, 0);
            SendMessageW(hChild, WM_SETFONT, (WPARAM)hFont, 0);

            LPARAM* param = (LPARAM*)pData->dwInitParam;    // pData->dwInitParam ��¼����������, [0] = �û�����, [1] = ���շ����ַ����ı���ָ��

            SetWindowTextW(hDlg, pData->caption);

            if (pData->lpDialogFunc && pData->lpDialogFunc(hDlg, message, wParam, param[0], &ret))
                return ret;

            if (pData->defText)
                SetDlgItemTextW(hDlg, IDC_EDIT_INPUT, pData->defText);

            break;
        }
        case WM_COMMAND:
        {
            int ID = LOWORD(wParam);
            if (ID == IDOK)
            {
                HWND hWndEdit = GetDlgItem(hDlg, IDC_EDIT_INPUT);
                int len = GetWindowTextLengthW(hWndEdit) + 1;
                //wchar_t* text = (wchar_t*)malloc(len * sizeof(WCHAR));
                wchar_t* text = new wchar_t[len];
                memset(text, 0, sizeof(wchar_t) * len);
                if (!text)
                {
                    EndDialog(hDlg, 0);
                    return true;
                }
                GetWindowTextW(hWndEdit, text, len);
                wchar_t*** param = (wchar_t***)pData->dwInitParam;
                *(param[1]) = text;
                EndDialog(hDlg, 1);
            }
            else if (ID == IDCANCEL)
            {
                EndDialog(hDlg, 0);
            }
            break;
        }
        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLORSCROLLBAR:
        case WM_CTLCOLORSTATIC:
            return (INT_PTR)GetStockObject(WHITE_BRUSH);
        case WM_CLOSE:
            EndDialog(hDlg, 0);
            break;
        case WM_PAINT:
        {
            PAINTSTRUCT ps = { 0 };
            BeginPaint(hDlg, &ps);
            RECT rc = { 0 };
            GetClientRect(hDlg, &rc);
            FillRect(ps.hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
            EndPaint(hDlg, &ps);
            break;
        }
        default:
            return (INT_PTR)false;
        }
        return (INT_PTR)true;
    }
}

// �ͷ��� input_OpenEx() ���� input_Open() ���ص��ַ���ָ��
inline void input_free(wchar_t* ptr)
{
    if (ptr)delete[] ptr;
}




// �������, �Զ���ȷ�Ϻ�ȡ����ť�ı���
// hParent = ����򸸴���
// text = �������ʾ����
// caption = ��������
// defText = �����Ĭ���ı�
// btn1 = �����ȷ�ϰ�ť�ı���
// btn2 = �����ȡ����ť�ı���
// outValue = �����ı�����ı���ָ��, ��Ҫ���� input_free() ���ͷ�
inline wchar_t* input_OpenEx(const INPUTBOXSTRUCT* pInfo)
{
    if (!pInfo)return 0;

    INPUTBOXSTRUCT data;
    memcpy(&data, pInfo, sizeof(INPUTBOXSTRUCT));

    if (!data.pszOkButton || !data.pszOkButton[0]) data.pszOkButton = L"ȷ������(&O)";
    if (!data.pszCancelButton || !data.pszCancelButton[0]) data.pszCancelButton = L"ȡ��(&C)";
    wchar_t* pszRetString = 0;
    LPARAM param[2] = { 0 };
    param[0] = pInfo->dwInitParam;
    param[1] = (LPARAM)&pszRetString;
    data.dwInitParam = (LPARAM)param;
    struct __DLGTEMPLATE : DLGTEMPLATE
    {
        WORD menuid;
        WORD cls;
        wchar_t caption[1024];
    };

    __DLGTEMPLATE dlg;
    memset(&dlg, 0, sizeof(dlg));
    dlg.style = WS_SYSMENU | WS_CAPTION | WS_CLIPSIBLINGS | WS_VISIBLE;
    dlg.dwExtendedStyle = WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT;
    dlg.cdit = 0;

    if (!data.caption)data.caption = L"";
    size_t len = wcslen(data.caption) + 1;
    if (len > 1024) len = 1024;
    memcpy(dlg.caption, data.caption, len * sizeof(wchar_t));

    INT_PTR bRet = DialogBoxIndirectParamW(GetModuleHandleW(0), &dlg, data.hParent, __input_namespace::DlgProc_InputBox, (LPARAM)&data);
    return pszRetString;
}



// ���ı������, ���ص��ı�ָ����Ҫ���� input_free() ���ͷ�
// hParent = ����򸸴���
// text = �������ʾ����
// caption = ��������
// defText = �����Ĭ���ı�
inline wchar_t* input_Open(HWND hParent, const wchar_t* text = L"", const wchar_t* caption = L"����������", const wchar_t* defText = L"",
    const wchar_t* pszOkButton = L"ȷ������(&O)", const wchar_t* pszCancelButton = L"ȡ��(&C)")
{
    INPUTBOXSTRUCT data = { 0 };
    data.hParent = hParent;
    data.text = text;
    data.caption = caption;
    data.defText = defText;
    data.pszOkButton = pszOkButton;
    data.pszCancelButton = pszCancelButton;
    return input_OpenEx(&data);
}

// �������
// hParent = ����򸸴���
// text = �������ʾ����
// caption = ��������
// defText = �����Ĭ���ı�
// outValue = ���ս�ִ�й��ı���ָ��
inline int input_OpenInt(HWND hParent, const wchar_t* text = L"", const wchar_t* caption = L"����������", const wchar_t* defText = L"",
    const wchar_t* pszOkButton = L"ȷ������(&O)", const wchar_t* pszCancelButton = L"ȡ��(&C)", bool* outValue = 0)
{
    INPUTBOXSTRUCT data = { 0 };
    data.hParent = hParent;
    data.text = text;
    data.caption = caption;
    data.defText = defText;
    data.pszOkButton = pszOkButton;
    data.pszCancelButton = pszCancelButton;
    wchar_t* outText = input_OpenEx(&data);
    bool isOk = outText != 0;
    int ret = isOk ? _wtoi(outText) : 0;
    input_free(outText);
    if (outValue)
        *outValue = isOk;
    return ret;
}

// �򿪳����������, ���س�������
// hParent = ����򸸴���
// text = �������ʾ����
// caption = ��������
// defText = �����Ĭ���ı�
// outValue = ���ս�ִ�й��ı���ָ��
inline LONGLONG input_OpenLonglong(HWND hParent, const wchar_t* text = L"", const wchar_t* caption = L"����������", const wchar_t* defText = L"",
    const wchar_t* pszOkButton = L"ȷ������(&O)", const wchar_t* pszCancelButton = L"ȡ��(&C)", bool* outValue = 0)
{
    INPUTBOXSTRUCT data = { 0 };
    data.hParent = hParent;
    data.text = text;
    data.caption = caption;
    data.defText = defText;
    data.pszOkButton = pszOkButton;
    data.pszCancelButton = pszCancelButton;
    wchar_t* outText = input_OpenEx(&data);
    bool isOk = outText != 0;
    LONGLONG ret = isOk ? _wtoi64(outText) : 0;
    input_free(outText);
    if (outValue)
        *outValue = isOk;
    return ret;
}

// �������
// hParent = ����򸸴���
// text = �������ʾ����
// caption = ��������
// defText = �����Ĭ���ı�
inline double input_OpenDouble(HWND hParent, const wchar_t* text = L"", const wchar_t* caption = L"����������", const wchar_t* defText = L"",
    const wchar_t* pszOkButton = L"ȷ������(&O)", const wchar_t* pszCancelButton = L"ȡ��(&C)")
{
    INPUTBOXSTRUCT data = { 0 };
    data.hParent = hParent;
    data.text = text;
    data.caption = caption;
    data.defText = defText;
    data.pszOkButton = pszOkButton;
    data.pszCancelButton = pszCancelButton;
    wchar_t* outText = input_OpenEx(&data);
    double ret = outText ? _wtof(outText) : 0;
    input_free(outText);
    return ret;
}
