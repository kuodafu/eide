#include "EWindow_Fne_Header.h"
#include <EIDE_Interface.h>
#include <EIDE_InternalMethods.h>
#include <hook/apiHook.h>


static CRITICAL_SECTION s_lockCaretChange;     // lock光标变动
static int* g_pLineSpacing;     // 单行间隔指针
static int* g_pLineHeight;      // 单行高度指针
static void* m_oldHook;

// ide前进后退栈信息
struct EIDE_STACK_INFO
{
    HWND hWnd;      // 代码框句柄, 需要使用代码框管理类就使用这个句柄获取, 这样能保证类指针一定存在
    POINT ptPos;    // 当前行列
    //ECODETYPE TYPE; // 代码类型
};

static std::vector<EIDE_STACK_INFO> s_stack;    // 前进后退栈, 最大允许0x1000 次前进后退, 超过的话需要删除前面的数据
//static EIDE_STACK_INFO s_stack[0x1000];         // 前进后退栈, 最大允许0x1000 次前进后退, 超过的话需要删除前面的数据
static int s_stack_index;                       // 前进后退栈, 当前指向哪个位置
static int s_stack_count;                       // 前进后退栈当前已经加入的成员数, 在前进的时候需要根据这个判断是否还能前进

// 把当前代码行号压入前进后退栈
size_t EIDE_PushStack(IEIDE_CodeManage* ide, HWND hChild1, bool isDown)
{
    if (s_stack.empty())return s_stack_count;
    if (!isDown && s_stack_index != s_stack_count)  // 不是按下触发的才走这里面
    {
        const bool isLButton = (GetAsyncKeyState(VK_LBUTTON) & 0xFF00) > 0;
        const bool isRButton = (GetAsyncKeyState(VK_RBUTTON) & 0xFF00) > 0;
        const int index = TabCtrl_GetCurSel(s_info->hEWnd);    // 当前选择夹选中项
        TCHITTESTINFO info = { 0 };
        GetCursorPos(&info.pt);
        ScreenToClient(s_info->hEWnd, &info.pt);
        int testIndex = TabCtrl_HitTest(s_info->hEWnd, &info);

        // 左键右键都没有按下, 并且鼠标不是在当前选中的选项上, 返回, 不加入
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
    s_stack_count = s_stack_index;  // 每次压入新的操作, 当前的索引就是栈的成员数
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

        if (isPush) // 这个参数为true则表示是鼠标点击触发的
        {
            EIDE_PushStack(info->pCodeManage, 0, isPush);
        }
        else
        {
            // 走到这里就是钩子触发的
            const bool isLButton = (GetAsyncKeyState(VK_LBUTTON) & 0xFF00) > 0;
            const bool isRButton = (GetAsyncKeyState(VK_RBUTTON) & 0xFF00) > 0;
            if (isLButton || isRButton)
            {
                if (!info->pCodeManage->IsSelCode())    // 不选中才往里面加入
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
    //        "hWnd = %d, 当前行 = %d, 旧行列 = {%d,%d}, 选中信息, 起始行列 = {%d,%d},"
    //        "pNowLine = %d, 选中范围 = {%d,%d}, IsSelCode = %d"
    //        "选中行数 = %d, 选中列数 = %d, lintInfo = %d, pFlags=%d, pData=%d, GetCodeType = %d, 光标表位置 = {%d,%d}\n"
    //        , ide.GetWindow(), info->line, info->oldLine, info->oldLineCol, pStartCol, pEndCol,
    //        pNowLine, pSel, ide.IsSelCode(),
    //        ide.GetSelLineCount(), ide.GetSelColumnCount(), lintInfo, pFlags, pData, ide.GetCodeType(), pt1);

    LeaveCriticalSection(&s_lockCaretChange);

    delete info;
}

// 主线程调用的回调, 行号改变回调
inline void CALLBACK _pos_changed_EWindowThread_callback(int param)
{
    _pos_changed_PushStack(param, false);
}

// 光标改变事件
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

// 裸函数, hook的地址是jmp过来的, 这里也需要jmp回去, 而不是call
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

// LineNumber_初始化光标处理
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
    // 5.93搜索的是这个
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
    //if (pData)  // 初始化行号hook后重新调整一下窗口位置
    //{
    //    WPARAM wParam = IsZoomed(s_info->hEWnd) ? SIZE_MAXIMIZED : SIZE_RESTORED;
    //    LPARAM lParam = MAKELONG(pData->pos.width, pData->pos.height);
    //    SendMessageW(s_info->hEWnd, WM_SIZE, wParam, lParam);
    // SendEWindowSize(pData);
    //}
}

// 取单行间隔
int* GetLineSpacing()
{
    return g_pLineSpacing;
}

// 取单行高度
int* GetLineHeight()
{
    return g_pLineHeight;
}

void EIDE_bindSelChanged(void* pfn)
{
    
}

// hWnd = 代码窗口, 如果ide==0, 这个值就是mdi下第一个窗口, 否则这个值就是mdi下第4个子窗口
// ptPos = 定位的位置, 如果ide==0, 则忽略这个参数
// ide = hWnd 参数指向的代码管理类
void EIDE_ShowCodeWindow(HWND hWnd, POINT* ptPos, IEIDE_CodeManage* ide)
{
    if (ide)
    {
        HWND hChild3 = GetParent(hWnd);
        HWND hChild2 = GetParent(hChild3);
        HWND hChild1 = GetParent(hChild2);
        if (!hChild1)
            return;
        IEIDE_CodeManage* pide = GetIEIDE_CodeManage(0);    // 获取当前显示的ide类
        if (!pide || pide != ide)   // 要切换的不是当前的窗口
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

        //TODO SetCursorPos 调用这个关闭易语言就崩溃
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
// 标准工具条后退按钮被按下
void EIDE_Toolbar_Back()
{
    //s_info->pfnCallMenu(33003);     // &Z.跳回先前位置\tCtrl+J, 32851
    if (s_stack_index == 0) // 已经是第一个了, 不能在后退了
        return;

    EIDE_STACK_INFO* data = 0;  // 回退的位置
    IEIDE_CodeManage* ide = 0;
    if (s_stack_index == s_stack_count)
        s_stack_index--;
    // 因为中途有可能会销毁了某个标签, 销毁之后这个标签的所有位置都不可用
    while (s_stack_index > 0)   // 一直往回找, 直到找到上一个可回退的位置
    {
        // s_stack_index 当前是指向下一个位置, 后退需要先减后加
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
// 标准工具条前进按钮被按下
void EIDE_Toolbar_Forward()
{
    //s_info->pfnCallMenu(32885); // &S.跳转到定义处\tCtrl+B/Ctrl+单击, 32885
    if (s_stack_index + 1 >= s_stack_count) // 已经是最后一个了, 不能在前进了
        return;

    EIDE_STACK_INFO* data = 0;  // 前进的位置
    IEIDE_CodeManage* ide = 0;
    // 因为中途有可能会销毁了某个标签, 销毁之后这个标签的所有位置都不可用
    while (s_stack_index + 1 < s_stack_count)   // 一直往后找, 直到找到下一个可回退的位置
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