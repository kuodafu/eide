#pragma once
#include <Commdlg.h>
#include <tstr.h>

// �Ի���_�����ļ�
// filter = ������, ��ʽ����MP3�ļ�(*.mp3)\0*.mp3\0ý���ļ�(*.mpg)\0*.mpg\0\0��
// defName = Ĭ���ļ���
// title = ���ڱ���
// path = ����Ŀ¼
// lpstrDefExt = Ĭ����չ��
// fileName = �����ļ���
// notChengedPath = ���ı�Ŀ¼
inline wstr Dialog_Save(LPCWSTR filter = 0, LPCWSTR title = 0, LPCWSTR defName = 0, LPCWSTR path = 0, LPCWSTR lpstrDefExt = 0, wstr* fileName = 0, bool notChengedPath = false)
{
    if (!title) title = L"�����ļ�";
    WCHAR file[MAX_PATH] = { 0 };
    WCHAR FileTitle[MAX_PATH] = { 0 };
    if (defName)
        memcpy(file, defName, wcslen(defName) * sizeof(wchar_t) + sizeof(wchar_t));
    if (!filter) filter = L"�����ļ�(*.*)\0*.*\0\0";
    //wstr strFilter(wcslen(filter) * 2);
    //strArrW arr;
    //size_t len = wstr::strtok(filter, &arr, L"|");
    //for (size_t i = 0; i < len; i++)
    //{
    //    strFilter.append(arr[i]).append(L"\0", 1);
    //    if (i + 1 == len)
    //        strFilter.append(L"\0", 1);
    //}
    //filter = strFilter.c_str();

    OPENFILENAMEW ofn       = { 0 };
    ofn.lStructSize         = sizeof(ofn);
    ofn.hwndOwner           = GetActiveWindow();
    ofn.hInstance           = GetModuleHandleW(0);
    ofn.lpstrFilter         = filter;
    ofn.lpstrCustomFilter   = 0;
    ofn.nMaxCustFilter      = 0;
    ofn.nFilterIndex        = 0;
    ofn.lpstrFile           = file;
    ofn.nMaxFile            = MAX_PATH;
    ofn.lpstrFileTitle      = FileTitle;
    ofn.nMaxFileTitle       = MAX_PATH;
    ofn.lpstrInitialDir     = path;
    ofn.lpstrDefExt         = lpstrDefExt;
    ofn.lpstrTitle          = title;

    ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;    // OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    if (notChengedPath)
        ofn.Flags |= OFN_NOCHANGEDIR;

    BOOL b = GetSaveFileNameW(&ofn);
    if (!b)return wstr();
    if (fileName)fileName->assign(FileTitle);
    return file;
}

namespace dialog_namespace
{
    // ����FALSE������Ϣ���ݵ�Ĭ�϶Ի�����̣����߷���TRUE��������Ϣ
    inline UINT_PTR CALLBACK pfn_ofnDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message != WM_NOTIFY)return FALSE;
        LPOFNOTIFYEX ofn = (LPOFNOTIFYEX)lParam;
        switch (ofn->hdr.code)
        {
        case CDN_FILEOK:
        {
            wstr::dbg(L"CDN_FILEOK �û�����ȷ����ť���Ի��򼴽��ر�\n");
            break;
        }
        case CDN_FOLDERCHANGE:
        {
            wstr::dbg(L"CDN_FOLDERCHANGE ������һ���µ��ļ��л�Ŀ¼\n");
            break;
        }
        case CDN_HELP:
        {
            wstr::dbg(L"CDN_HELP �û������� ��������ť\n");
            break;
        }
        case CDN_INCLUDEITEM:
        {
            wstr::dbg(L"CDN_INCLUDEITEM ȷ���Ƿ�Ӧ��ʾ��Ŀ�����û����µ��ļ��л�Ŀ¼ʱ��ϵͳ����Ը��ļ��л�Ŀ¼�е�ÿ����Ŀ���ʹ�֪ͨ������������OFN_ENABLEINCLUDENOTIFY��־ʱ��ϵͳ�ŷ��ʹ�֪ͨ\n");
            break;
        }
        case CDN_INITDONE:
        {
            wstr::dbg(L"CDN_INITDONE ϵͳ����ɶԻ���ĳ�ʼ�������ҶԻ�������ɶ�WM_INITDIALOG��Ϣ�Ĵ���\n");
            break;
        }
        case CDN_SELCHANGE:
        {
            wstr::dbg(L"CDN_SELCHANGE �û����ļ��б���ѡ����һ�����ļ����ļ���\n");
            break;
        }
        case CDN_SHAREVIOLATION:
        {
            wstr::dbg(L"CDN_SHAREVIOLATION ͨ�öԻ�����Ҫ���ص��ļ������������ͻ\n");
            break;
        }
        case CDN_TYPECHANGE:
        {
            wstr::dbg(L"CDN_TYPECHANGE �û����ļ������б���ѡ�����µ��ļ�����\n");
            break;
        }
        }

        return FALSE;
    }
}


// �Ի���_�����ļ�
// filter = ������, ��ʽ����MP3�ļ�(*.mp3)|*.mp3|ý���ļ�(*.mpg)|*.mpg��
// defName = Ĭ���ļ���
// title = ���ڱ���
// path = ����Ŀ¼
// lpstrDefExt = Ĭ����չ��
// fileName = �����ļ���
// notChengedPath = ���ı�Ŀ¼
inline wstr Dialog_SelPath(LPCWSTR filter = 0, LPCWSTR title = 0, LPCWSTR defName = 0, LPCWSTR path = 0, LPCWSTR lpstrDefExt = 0, wstr* fileName = 0, bool notChengedPath = false)
{
    if (!title) title = L"�����ļ�";
    WCHAR file[MAX_PATH] = { 0 };
    WCHAR FileTitle[MAX_PATH] = { 0 };
    //memcpy(FileTitle, g_runPath, MAX_PATH * sizeof(WCHAR));
    if (defName)
        memcpy(file, defName, wcslen(defName) * sizeof(wchar_t) + sizeof(wchar_t));
    if (!filter) filter = L"�����ļ�(*.*)|*.*";
    wstr strFilter(wcslen(filter) * 2);
    strArrW arr;
    size_t len = wstr::strtok(filter, &arr, L"|");
    for (size_t i = 0; i < len; i++)
    {
        strFilter.append(arr[i]).append(L"\0", 1);
        if (i + 1 == len)
            strFilter.append(L"\0", 1);
    }
    filter = strFilter.c_str();

    OPENFILENAMEW ofn = { 0 };
    ofn.lStructSize = 76;// sizeof(ofn);
    ofn.hwndOwner = GetActiveWindow();
    ofn.hInstance = GetModuleHandleW(0);
    ofn.lpstrFilter = filter;
    ofn.lpstrCustomFilter = 0;
    ofn.nMaxCustFilter = 0;
    ofn.nFilterIndex = 0;
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFileTitle = FileTitle;
    ofn.nMaxFileTitle = MAX_PATH;
    ofn.lpstrInitialDir = path;
    ofn.lpstrDefExt = lpstrDefExt;
    ofn.lpstrTitle = title;
    ofn.lpfnHook = dialog_namespace::pfn_ofnDlgProc;
    ofn.Flags = OFN_EXPLORER;    // OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    ofn.Flags |= OFN_ENABLEHOOK;
    if (notChengedPath) ofn.Flags |= OFN_NOCHANGEDIR;
    BOOL b = GetSaveFileNameW(&ofn);
    if (!b)return wstr();
    if (fileName)fileName->assign(FileTitle);
    return file;
}

// �Ի���_���ļ�
// filter = ������, ��ʽ����MP3�ļ�(*.mp3)\0*.mp3\0ý���ļ�(*.mpg)\0*.mpg\0\0��
// defName = Ĭ���ļ���
// title = ���ڱ���
// path = ����Ŀ¼
// fileName = �����ļ���
// notChengedPath = ���ı�Ŀ¼
inline wstr Dialog_Open(LPCWSTR filter = 0, LPCWSTR title = 0, LPCWSTR defName = 0, LPCWSTR path = 0, wstr* fileName = 0, bool notChengedPath = false)
{
    if (!title) title = L"���ļ�";
    WCHAR file[MAX_PATH] = { 0 };
    WCHAR FileTitle[MAX_PATH] = { 0 };
    //memcpy(FileTitle, g_runPath, MAX_PATH * sizeof(WCHAR));
    if (defName)
        memcpy(file, defName, wcslen(defName) * sizeof(wchar_t) + sizeof(wchar_t));

    if (!filter) filter = L"�����ļ�(*.*)\0*.*\0\0";

    OPENFILENAMEW ofn = { 0 };
    ofn.lStructSize         = sizeof(ofn);
    ofn.hwndOwner           = GetActiveWindow();
    ofn.hInstance           = 0;
    ofn.lpstrFilter         = filter;
    ofn.lpstrCustomFilter   = 0;
    ofn.nMaxCustFilter      = 0;
    ofn.nFilterIndex        = 0;
    ofn.lpstrFile           = file;
    ofn.nMaxFile            = MAX_PATH;
    ofn.lpstrFileTitle      = FileTitle;
    ofn.nMaxFileTitle       = MAX_PATH;
    ofn.lpstrInitialDir     = path;
    ofn.lpstrTitle          = title;
    ofn.Flags               = OFN_EXPLORER;    // OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    if (notChengedPath)
        ofn.Flags |= OFN_NOCHANGEDIR;
    BOOL b = GetOpenFileNameW(&ofn);
    if (!b)
        return wstr();
    if (fileName)fileName->assign(FileTitle);
    return file;
}