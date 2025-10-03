#pragma once
#include <Commdlg.h>
#include <tstr.h>

// 对话框_保存文件
// filter = 过滤器, 格式：“MP3文件(*.mp3)\0*.mp3\0媒体文件(*.mpg)\0*.mpg\0\0”
// defName = 默认文件名
// title = 窗口标题
// path = 运行目录
// lpstrDefExt = 默认扩展名
// fileName = 接受文件名
// notChengedPath = 不改变目录
inline wstr Dialog_Save(LPCWSTR filter = 0, LPCWSTR title = 0, LPCWSTR defName = 0, LPCWSTR path = 0, LPCWSTR lpstrDefExt = 0, wstr* fileName = 0, bool notChengedPath = false)
{
    if (!title) title = L"保存文件";
    WCHAR file[MAX_PATH] = { 0 };
    WCHAR FileTitle[MAX_PATH] = { 0 };
    if (defName)
        memcpy(file, defName, wcslen(defName) * sizeof(wchar_t) + sizeof(wchar_t));
    if (!filter) filter = L"所有文件(*.*)\0*.*\0\0";
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
    // 返回FALSE来将消息传递到默认对话框过程，或者返回TRUE来丢弃消息
    inline UINT_PTR CALLBACK pfn_ofnDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message != WM_NOTIFY)return FALSE;
        LPOFNOTIFYEX ofn = (LPOFNOTIFYEX)lParam;
        switch (ofn->hdr.code)
        {
        case CDN_FILEOK:
        {
            wstr::dbg(L"CDN_FILEOK 用户单击确定按钮；对话框即将关闭\n");
            break;
        }
        case CDN_FOLDERCHANGE:
        {
            wstr::dbg(L"CDN_FOLDERCHANGE 户打开了一个新的文件夹或目录\n");
            break;
        }
        case CDN_HELP:
        {
            wstr::dbg(L"CDN_HELP 用户单击“ 帮助”按钮\n");
            break;
        }
        case CDN_INCLUDEITEM:
        {
            wstr::dbg(L"CDN_INCLUDEITEM 确定是否应显示项目。当用户打开新的文件夹或目录时，系统会针对该文件夹或目录中的每个项目发送此通知。仅当设置了OFN_ENABLEINCLUDENOTIFY标志时，系统才发送此通知\n");
            break;
        }
        case CDN_INITDONE:
        {
            wstr::dbg(L"CDN_INITDONE 系统已完成对话框的初始化，并且对话框已完成对WM_INITDIALOG消息的处理\n");
            break;
        }
        case CDN_SELCHANGE:
        {
            wstr::dbg(L"CDN_SELCHANGE 用户从文件列表中选择了一个新文件或文件夹\n");
            break;
        }
        case CDN_SHAREVIOLATION:
        {
            wstr::dbg(L"CDN_SHAREVIOLATION 通用对话框在要返回的文件上遇到共享冲突\n");
            break;
        }
        case CDN_TYPECHANGE:
        {
            wstr::dbg(L"CDN_TYPECHANGE 用户从文件类型列表中选择了新的文件类型\n");
            break;
        }
        }

        return FALSE;
    }
}


// 对话框_保存文件
// filter = 过滤器, 格式：“MP3文件(*.mp3)|*.mp3|媒体文件(*.mpg)|*.mpg”
// defName = 默认文件名
// title = 窗口标题
// path = 运行目录
// lpstrDefExt = 默认扩展名
// fileName = 接受文件名
// notChengedPath = 不改变目录
inline wstr Dialog_SelPath(LPCWSTR filter = 0, LPCWSTR title = 0, LPCWSTR defName = 0, LPCWSTR path = 0, LPCWSTR lpstrDefExt = 0, wstr* fileName = 0, bool notChengedPath = false)
{
    if (!title) title = L"保存文件";
    WCHAR file[MAX_PATH] = { 0 };
    WCHAR FileTitle[MAX_PATH] = { 0 };
    //memcpy(FileTitle, g_runPath, MAX_PATH * sizeof(WCHAR));
    if (defName)
        memcpy(file, defName, wcslen(defName) * sizeof(wchar_t) + sizeof(wchar_t));
    if (!filter) filter = L"所有文件(*.*)|*.*";
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

// 对话框_打开文件
// filter = 过滤器, 格式：“MP3文件(*.mp3)\0*.mp3\0媒体文件(*.mpg)\0*.mpg\0\0”
// defName = 默认文件名
// title = 窗口标题
// path = 运行目录
// fileName = 接受文件名
// notChengedPath = 不改变目录
inline wstr Dialog_Open(LPCWSTR filter = 0, LPCWSTR title = 0, LPCWSTR defName = 0, LPCWSTR path = 0, wstr* fileName = 0, bool notChengedPath = false)
{
    if (!title) title = L"打开文件";
    WCHAR file[MAX_PATH] = { 0 };
    WCHAR FileTitle[MAX_PATH] = { 0 };
    //memcpy(FileTitle, g_runPath, MAX_PATH * sizeof(WCHAR));
    if (defName)
        memcpy(file, defName, wcslen(defName) * sizeof(wchar_t) + sizeof(wchar_t));

    if (!filter) filter = L"所有文件(*.*)\0*.*\0\0";

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