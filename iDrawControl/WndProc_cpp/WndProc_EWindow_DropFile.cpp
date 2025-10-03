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
    {    // ��ȡ���������Ե�·��, ֱ��������, ������������
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
    // ������, ������������Ŀ, �Ǿ����Ѿ�����/�½���Դ��
    HWND hWndTree = s_info->eWnd->GetWorkProgramTree();
    UINT count = TreeView_GetCount(hWndTree);
    if (count == 0) // ������û����Ŀ�ͱ�ʾû�д򿪻����½�Դ��, ����Ϣ���������Դ���
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    IEIDE_CodeManage* ide = GetIEIDE_CodeManage(0);
    const short ctrl = GetAsyncKeyState(VK_CONTROL);
    const short shift = GetAsyncKeyState(VK_SHIFT);
    const bool isCtrl = ctrl & 0x8000;
    const bool isShift = shift & 0x8000;

    char file[0x1000];
    HDROP hDrop = (HDROP)wParam;
    count = DragQueryFileA(hDrop, -1, file, 1000);
    bool isOpenSource = false;  // �Ƿ��Ѿ�����Դ��
    for (UINT i = 0; i < count; i++)
    {
        DragQueryFileA(hDrop, i, file, 1000);
        char* pos = strrchr(file, '.');
        if (!pos) pos = file;

        // ����shift��, ����ctrl��û�а���
        if (isShift && !isCtrl)
        {
            if (!ide)
                break;
            // ������Ҫ����·����������
            _window_drawfile_file(ide, file);
            continue;
        }

        if (stricmp_api(pos, ".e") == 0)        // ��������Դ���ļ�, ���������Դ���
        {
            if (isOpenSource)continue;  // ������Դ��ֻ�򿪵�һ��
            isOpenSource = true;
            _window_drawfile_e(ide, file, isCtrl);
            //CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
        }
        else if (stricmp_api(pos, ".ec") == 0)  // ec�ļ�
        {
            E_IncludeEc(file);
        }
        else if (ide)
        {
            // ��ǰ�򿪵��Ǵ��봰��, ���Ҵ򿪵Ĳ���e����ec, ֱ�Ӱ��ļ�·�����뵽������
            _window_drawfile_file(ide, file);
        }

    }
    DragFinish(hDrop);
    return 0;
}