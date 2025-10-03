#include "../EWindow_Fne_Header.h"
#include "../EWindow.h"
#include <process_muster.h>



inline int _window_drawfile_e(IEIDE_CodeManage* ide, LPCSTR file, bool isCtrl)
{
    wchar_t eFile[260];
    //eFile.assign(s_info->path_e).append(L"e.exe");

    HMODULE hModule = GetModuleHandleW(0);
    GetModuleFileNameW(hModule, eFile, 260);
    if (isCtrl)
    {    // 获取到了易语言的路径, 直接运行它, 并给个命令行
        wstr f = _str::A2W(file);
        return process_create(eFile, f.c_str(), 0);
    }
    return _nes_run_fun(FN_OPEN_FILE2, file, 0);
}

inline int _window_drawfile_file(IEIDE_CodeManage* ide, LPCSTR file)
{
    return _nes_run_fun(FN_SET_AND_COMPILE_PRG_ITEM_TEXT, file, false);
    return _nes_run_fun(FN_INSERT_TEXT, file, false);
}

LRESULT EWndProc_OnDropFile(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    // 程序树, 如果这个树有项目, 那就是已经打开了/新建了源码
    HWND hWndTree = s_info->eWnd->GetWorkProgramTree();
    UINT count = TreeView_GetCount(hWndTree);
    if (count == 0) // 程序树没有项目就表示没有打开或者新建源码, 把消息发给易语言处理
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    IEIDE_CodeManage* ide = GetIEIDE_CodeManage(0);
    const short ctrl = GetAsyncKeyState(VK_CONTROL);
    const short shift = GetAsyncKeyState(VK_SHIFT);
    const bool isCtrl = ctrl & 0x8000;
    const bool isShift = shift & 0x8000;

    char file[0x1000];
    HDROP hDrop = (HDROP)wParam;
    count = DragQueryFileA(hDrop, -1, file, 1000);
    bool isOpenSource = false;  // 是否已经打开了源码
    for (UINT i = 0; i < count; i++)
    {
        DragQueryFileA(hDrop, i, file, 1000);
        char* pos = strrchr(file, '.');
        if (!pos) pos = file;

        // 按下shift键, 并且ctrl键没有按下
        if (isShift && !isCtrl)
        {
            if (!ide)
                break;
            // 这里需要加入路径到代码中
            _window_drawfile_file(ide, file);
            continue;
        }

        if (stricmp_api(pos, ".e") == 0)        // 是易语言源码文件, 丢给易语言处理
        {
            if (isOpenSource)continue;  // 拖入多个源码只打开第一个
            isOpenSource = true;
            _window_drawfile_e(ide, file, isCtrl);
            //CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
        }
        else if (stricmp_api(pos, ".ec") == 0)  // ec文件
        {
            E_IncludeEc(file);
        }
        else if (ide)
        {
            // 当前打开的是代码窗口, 并且打开的不是e或者ec, 直接把文件路径加入到代码中
            _window_drawfile_file(ide, file);
        }

    }
    DragFinish(hDrop);
    return 0;
}