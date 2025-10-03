#include "EWindow_Fne_Header.h"
#include <EIDE_Interface.h>
#include <EIDE_InternalMethods.h>
#include <hook/apiHook.h>


static CRITICAL_SECTION s_lockCaretChange;     // lock���䶯
static int* g_pLineSpacing;     // ���м��ָ��
static int* g_pLineHeight;      // ���и߶�ָ��
static void* m_oldHook;

// ideǰ������ջ��Ϣ
struct EIDE_STACK_INFO
{
    HWND hWnd;      // �������, ��Ҫʹ�ô����������ʹ����������ȡ, �����ܱ�֤��ָ��һ������
    POINT ptPos;    // ��ǰ����
    //ECODETYPE TYPE; // ��������
};

static std::vector<EIDE_STACK_INFO> s_stack;    // ǰ������ջ, �������0x1000 ��ǰ������, �����Ļ���Ҫɾ��ǰ�������
//static EIDE_STACK_INFO s_stack[0x1000];         // ǰ������ջ, �������0x1000 ��ǰ������, �����Ļ���Ҫɾ��ǰ�������
static int s_stack_index;                       // ǰ������ջ, ��ǰָ���ĸ�λ��
static int s_stack_count;                       // ǰ������ջ��ǰ�Ѿ�����ĳ�Ա��, ��ǰ����ʱ����Ҫ��������ж��Ƿ���ǰ��

// �ѵ�ǰ�����к�ѹ��ǰ������ջ
size_t EIDE_PushStack(IEIDE_CodeManage* ide, HWND hChild1, bool isDown)
{
    if (s_stack.empty())return s_stack_count;
    if (!isDown && s_stack_index != s_stack_count)  // ���ǰ��´����Ĳ���������
    {
        const bool isLButton = (GetAsyncKeyState(VK_LBUTTON) & 0xFF00) > 0;
        const bool isRButton = (GetAsyncKeyState(VK_RBUTTON) & 0xFF00) > 0;
        const int index = TabCtrl_GetCurSel(s_info->hEWnd);    // ��ǰѡ���ѡ����
        TCHITTESTINFO info = { 0 };
        GetCursorPos(&info.pt);
        ScreenToClient(s_info->hEWnd, &info.pt);
        int testIndex = TabCtrl_HitTest(s_info->hEWnd, &info);

        // ����Ҽ���û�а���, ������겻���ڵ�ǰѡ�е�ѡ����, ����, ������
        if (!isLButton && !isRButton && index != testIndex)
            return s_stack_count;
    }
    if (s_stack_index == 0)
        s_stack_index++;
    if (s_stack_index + 1 >= 0x1000)
    {
        EIDE_STACK_INFO* ptr = s_stack.data();
        EIDE_STACK_INFO* ptrMid = ptr + 0x800;
        memcpy(ptr, ptrMid, 0x800 * sizeof(EIDE_STACK_INFO));
        s_stack_index = 0x800;
    }

    EIDE_STACK_INFO& push = s_stack[s_stack_index++];
    if (ide)
    {
        push.hWnd = ide->GetWindow();
        push.ptPos = ide->GetCursorTablePos();
    }
    else
    {
        push.hWnd = hChild1;
        push.ptPos = { 0, 0 };
    }
    if (s_stack_index > 1)
    {
        EIDE_STACK_INFO& prev = s_stack[s_stack_index - 2];
        EIDE_STACK_INFO& next = s_stack[s_stack_index - 1];

        if (prev.ptPos.x == next.ptPos.x && next.hWnd == prev.hWnd)
            s_stack_index--;
    }
    s_stack_count = s_stack_index;  // ÿ��ѹ���µĲ���, ��ǰ����������ջ�ĳ�Ա��
    return s_stack_index;
}

__declspec(naked) int __stdcall  call_3(LPVOID pfn, UINT nEvent, LONG_PTR param, LPVOID pArg)
{
    __asm
    {
        push ebp
        mov ebp, esp
        mov eax, dword ptr[ebp + 0x08]
        mov ebx, dword ptr[ebp + 0x04]
        mov dword ptr[ebp + 0x08], ebx
        pop ebp
        pop ebx
        jmp eax
    }
}

inline void CALLBACK _pos_changed_PushStack(int param, bool isPush)
{
    IRE_STRUCT_EIDE_SELCHANGEDINFO* info = (IRE_STRUCT_EIDE_SELCHANGEDINFO*)param;
    EnterCriticalSection(&s_lockCaretChange);

    size_t size = s_stack.size();
    if (isPush || (info->pCodeManage && ((info->line != info->oldLine || info->line == 0) || (size == 0))))
    {
        if (size == 0)
        {
            s_stack.resize(0x1000);
            s_stack_index = 0;

            EIDE_STACK_INFO& push = s_stack[s_stack_index++];
            push.hWnd = info->pCodeManage->GetWindow();
            push.ptPos = { info->oldLine, 0 };
            s_stack_count = s_stack_index;
        }

        if (isPush) // �������Ϊtrue���ʾ�������������
        {
            EIDE_PushStack(info->pCodeManage, 0, isPush);
        }
        else
        {
            // �ߵ�������ǹ��Ӵ�����
            const bool isLButton = (GetAsyncKeyState(VK_LBUTTON) & 0xFF00) > 0;
            const bool isRButton = (GetAsyncKeyState(VK_RBUTTON) & 0xFF00) > 0;
            if (isLButton || isRButton)
            {
                if (!info->pCodeManage->IsSelCode())    // ��ѡ�в����������
                    EIDE_PushStack(info->pCodeManage, 0, false);
            }
        }
    }

    LPREGISTER_EVENT_INFO arr[EVENT_MAX_ITEM]; int isReturn = 0;
    int evtCount = s_info->evt.pfnGetEventInfo(IRE_ECODESELCHANGED, arr, EVENT_MAX_ITEM);
    for (int i = 0; i < evtCount; i++)
    {
        if (arr[i]->pfnCallbackBefore)
            //call_3(arr[i]->pfnCallbackBefore, IRE_ECODESELCHANGED, arr[i]->paramBefore, info);
            arr[i]->pfnCallbackBefore(IRE_ECODESELCHANGED, arr[i]->paramBefore, info);
    }

    //    int pStartCol = 0, pEndCol = 0, pNowLine = 0;
    //    POINT pSel = { 0 };
    //    IEIDE_CodeManage& ide = *info->pCodeManage;
    //    ide.GetSelInfo(&pStartCol, &pEndCol, &pNowLine, &pSel);
    //
    //    int pFlags = 0, pData = 0;
    //    int lintInfo = ide.GetLineInfo(info->line, &pFlags, &pData);
    //    POINT pt1 = ide.GetCursorTablePos();
    ////    LPCSTR text = ide.GetCodeText(pt1.x, pt1.y);
    //
    //    _str::dbg(
    //        "hWnd = %d, ��ǰ�� = %d, ������ = {%d,%d}, ѡ����Ϣ, ��ʼ���� = {%d,%d},"
    //        "pNowLine = %d, ѡ�з�Χ = {%d,%d}, IsSelCode = %d"
    //        "ѡ������ = %d, ѡ������ = %d, lintInfo = %d, pFlags=%d, pData=%d, GetCodeType = %d, ����λ�� = {%d,%d}\n"
    //        , ide.GetWindow(), info->line, info->oldLine, info->oldLineCol, pStartCol, pEndCol,
    //        pNowLine, pSel, ide.IsSelCode(),
    //        ide.GetSelLineCount(), ide.GetSelColumnCount(), lintInfo, pFlags, pData, ide.GetCodeType(), pt1);

    LeaveCriticalSection(&s_lockCaretChange);

    delete info;
}

// ���̵߳��õĻص�, �кŸı�ص�
inline void CALLBACK _pos_changed_EWindowThread_callback(int param)
{
    _pos_changed_PushStack(param, false);
}

// ���ı��¼�
void __cdecl _changed(int pThis, int line)
{
    if (!pThis) return;
    HWND hWnd = (HWND)(*((int*)(pThis + CWnd_m_hWnd)));

    IRE_STRUCT_EIDE_SELCHANGEDINFO* info = new IRE_STRUCT_EIDE_SELCHANGEDINFO;

    info->pCodeManage = GetIEIDE_CodeManage(hWnd);
    POINT pt = { 0 };
    if (info->pCodeManage)
        pt = info->pCodeManage->GetCursorPos();
    info->line = line;
    info->oldLine = pt.x;
    info->oldLineCol = pt.y;
    PostMessageA(s_info->hEWnd, s_info->uMsgEWindowThreadCallback, (WPARAM)_pos_changed_EWindowThread_callback, (LPARAM)info);
}

// �㺯��, hook�ĵ�ַ��jmp������, ����Ҳ��Ҫjmp��ȥ, ������call
void __declspec(naked) __stdcall _CaretChange_callback()
{
    __asm
    {
        push[esp + 0x04];
        push ecx;
        call _changed;
        pop ecx;
        add esp, 4;

        jmp m_oldHook;
    }
}
inline void IDraw_EIDE_Destroy()
{
    s_stack.clear();
}

// LineNumber_��ʼ����괦��
void EIDE_Init(bool isHook)
{
    static bool isReturn = false;
    if (isReturn && !isHook) return;
    if (!isReturn)
    {
        isReturn = true;
        IDraw_PushPluginFreeCallback(IDraw_EIDE_Destroy);
        InitializeCriticalSection(&s_lockCaretChange);
    }
    if (isHook)
    {    //const BYTE find[] = { 106, 0, 106, 1, 106, 0, 106, 0, 80, 139, 206, 232 };
        const BYTE find[] = { 0x6A,0x00,0x6A,0x01,0x6A,0x00,0x6A,0x00,0x50,0x8B,0xCE,0xE8 };
        LPBYTE address = EIDE_FindAddress(__FINDARG(find));
        if (address)
        {
            address += 12;
            address = (*((int*)address)) + address + 4;

            //address += 0x2D;
            //address = (*((int*)address)) + address + 4;
            bool isOk = false;
            LPVOID* oldFun = &(LPVOID&)address;
            LPVOID m_oldFun = *oldFun;
            while (true)
            {
                int ret = MH_Initialize();
                if (ret != MH_OK && ret != MH_ERROR_ALREADY_INITIALIZED)
                    break;
                if (MH_CreateHook(m_oldFun, _CaretChange_callback, reinterpret_cast<LPVOID*>(oldFun)) != MH_OK)
                    break;
                if (MH_EnableHook(m_oldFun) != MH_OK)
                    break;
                isOk = true;
                break;
            }

            m_oldHook = address;
            //m_oldHook = (void*)IDraw_NotifyCallback(IDC_IDRAW_INLINEHOOK, (WPARAM)address, (LPARAM)_CaretChange_callback);
        }
    }
    const BYTE find2[] = { 139, 139, 132, 3, 0, 0, 51, 192, 59, 239, 15, 148, 192, 106, 0, 80, 104, 241, 0, 0, 0, 81, 255, 214 };
    const BYTE find3[] = { 139, 208, 15, 175, 209, 184, 22, 0, 0, 0, 137, 92, 36, 24, 43, 194, 137, 92, 36, 20, 3, 200 };
    // 5.93�����������
    //0x03, 0xF8, 0x52, 0x03, 0xCF, 0x6A, 0x0B, 0x89, 0x5C, 0x24, 0x20, 0x89, 0x5C, 0x24, 0x1C, 0x89, 0x8C, 0x24, 0x88, 0x00, 0x00, 0x00

    LPBYTE address = EIDE_FindAddress(__FINDARG(find2));
    if (address)
    {
        address += 24 + 1;
        g_pLineSpacing = (int*)(*((int*)address));
    }
    address = EIDE_FindAddress(__FINDARG(find3));
    if (!address)
    {
        const BYTE find3_1[] = { 0x03, 0xF8, 0x52, 0x03, 0xCF, 0x6A, 0x0B, 0x89, 0x5C, 0x24, 0x20, 0x89, 0x5C, 0x24, 0x1C, 0x89, 0x8C, 0x24, 0x88, 0x00, 0x00, 0x00 };
        address = EIDE_FindAddress(__FINDARG(find3_1));
        if (address)
        {
            address -= 4;
            g_pLineHeight = (int*)(*((int*)address));
        }
    }
    else if (address)
    {
        address += 22 + 1;
        g_pLineHeight = (int*)(*((int*)address));
    }
    //LPOBJSTRUCT pData = GetDataFromHwnd(s_info->hEWnd);
    //if (pData)  // ��ʼ���к�hook�����µ���һ�´���λ��
    //{
    //    WPARAM wParam = IsZoomed(s_info->hEWnd) ? SIZE_MAXIMIZED : SIZE_RESTORED;
    //    LPARAM lParam = MAKELONG(pData->pos.width, pData->pos.height);
    //    SendMessageW(s_info->hEWnd, WM_SIZE, wParam, lParam);
    // SendEWindowSize(pData);
    //}
}

// ȡ���м��
int* GetLineSpacing()
{
    return g_pLineSpacing;
}

// ȡ���и߶�
int* GetLineHeight()
{
    return g_pLineHeight;
}

void EIDE_bindSelChanged(void* pfn)
{
    
}

// hWnd = ���봰��, ���ide==0, ���ֵ����mdi�µ�һ������, �������ֵ����mdi�µ�4���Ӵ���
// ptPos = ��λ��λ��, ���ide==0, ������������
// ide = hWnd ����ָ��Ĵ��������
void EIDE_ShowCodeWindow(HWND hWnd, POINT* ptPos, IEIDE_CodeManage* ide)
{
    if (ide)
    {
        HWND hChild3 = GetParent(hWnd);
        HWND hChild2 = GetParent(hChild3);
        HWND hChild1 = GetParent(hChild2);
        if (!hChild1)
            return;
        IEIDE_CodeManage* pide = GetIEIDE_CodeManage(0);    // ��ȡ��ǰ��ʾ��ide��
        if (!pide || pide != ide)   // Ҫ�л��Ĳ��ǵ�ǰ�Ĵ���
        {
            LPOBJSTRUCT pChild1 = s_info->pfnGetDataFromHwnd(hChild1);
            if (pChild1)
            {
                SendMessageA(hChild1, WM_MDIACTIVATE, (WPARAM)hChild1, (LPARAM)hChild1);
            }
        }

        //IRE_STRUCT_EIDE_SELCHANGEDINFO* info = new IRE_STRUCT_EIDE_SELCHANGEDINFO;
        //memset(info, 0, sizeof(IRE_STRUCT_EIDE_SELCHANGEDINFO));
        //info->pCodeManage = pide;
        //POINT pt = info->pCodeManage->GetCursorPos();
        //info->oldLine = pt.x;
        //info->oldLineCol = pt.y;
        //info->line = ptPos->x;

        //TODO SetCursorPos ��������ر������Ծͱ���
        //ide->SetCursorPos(data->ptPos);
        s_info->pfnIDE(NES_RUN_FUNC, FN_MOVE_CARET, (DWORD)ptPos);
        //ide->CursorLocation(data->ptPos.x);
        ide->EnsurVisibleCursor();
        //_pos_changed_PushStack((int)info, false);

        return;
    }
    HWND hChild1 = hWnd;
    HWND hMDIClient1 = GetWindow(s_info->hMDIClient, GW_CHILD);
    if (hMDIClient1 != hChild1)
    {
        LPOBJSTRUCT pChild1 = s_info->pfnGetDataFromHwnd(hChild1);
        if (pChild1)
        {
            SendMessageA(hChild1, WM_MDIACTIVATE, (WPARAM)hChild1, (LPARAM)hChild1);
        }
    }

}
// ��׼���������˰�ť������
void EIDE_Toolbar_Back()
{
    //s_info->pfnCallMenu(33003);     // &Z.������ǰλ��\tCtrl+J, 32851
    if (s_stack_index == 0) // �Ѿ��ǵ�һ����, �����ں�����
        return;

    EIDE_STACK_INFO* data = 0;  // ���˵�λ��
    IEIDE_CodeManage* ide = 0;
    if (s_stack_index == s_stack_count)
        s_stack_index--;
    // ��Ϊ��;�п��ܻ�������ĳ����ǩ, ����֮�������ǩ������λ�ö�������
    while (s_stack_index > 0)   // һֱ������, ֱ���ҵ���һ���ɻ��˵�λ��
    {
        // s_stack_index ��ǰ��ָ����һ��λ��, ������Ҫ�ȼ����
        EIDE_STACK_INFO& push = s_stack[--s_stack_index];
        if (!IsWindow(push.hWnd))
            continue;
        ide = GetIEIDE_CodeManage(push.hWnd);
        if (ide)
        {
            data = &push;
            break;
        }
        EIDE_ShowCodeWindow(push.hWnd, &push.ptPos, ide);
        break;
    }
    if (!data || !ide) return;
    EIDE_ShowCodeWindow(data->hWnd, &data->ptPos, ide);
}
// ��׼������ǰ����ť������
void EIDE_Toolbar_Forward()
{
    //s_info->pfnCallMenu(32885); // &S.��ת�����崦\tCtrl+B/Ctrl+����, 32885
    if (s_stack_index + 1 >= s_stack_count) // �Ѿ������һ����, ������ǰ����
        return;

    EIDE_STACK_INFO* data = 0;  // ǰ����λ��
    IEIDE_CodeManage* ide = 0;
    // ��Ϊ��;�п��ܻ�������ĳ����ǩ, ����֮�������ǩ������λ�ö�������
    while (s_stack_index + 1 < s_stack_count)   // һֱ������, ֱ���ҵ���һ���ɻ��˵�λ��
    {
        EIDE_STACK_INFO& push = s_stack[++s_stack_index];
        if (!IsWindow(push.hWnd))
            continue;
        ide = GetIEIDE_CodeManage(push.hWnd);
        if (ide)
        {
            data = &push;
            break;
        }
        EIDE_ShowCodeWindow(push.hWnd, &push.ptPos, ide);
        break;
    }
    if (!data || !ide) return;
    EIDE_ShowCodeWindow(data->hWnd, &data->ptPos, ide);
}

void EIDE_LButtonDown(IRE_STRUCT_EIDE_SELCHANGEDINFO* info)
{
    _pos_changed_PushStack((int)info, true);
}