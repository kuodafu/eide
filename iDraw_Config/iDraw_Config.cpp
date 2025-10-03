// iDraw_Config.cpp : 定义应用程序的入口点。
//

#include "iDraw_Config.h"





static CMapMemFile m_mapFileIn;     // 输入共享文件, 外部程序调用配置程序功能时把参数数据写入这里
static CMapMemFile m_mapFileOut;    // 输出共享文件, 外部程序获取配置时把读取到的配置结果写入到这个文件里
static CSemaphore m_semaphore;      // 进程信号量, 需要在信号量为0的时候销毁配置程序

// 配置进程的消息处理函数
void IConfigProc(UINT message, WPARAM wParam, LPARAM lParam);
// 线程消息回调, 需要调用功能的可以投递线程消息来处理
DWORD CALLBACK ThreadProc_onMessage(LPVOID pArg);

// 初始化信号量, 初始化失败返回false
inline bool _init_semaphore()
{
    m_semaphore.setname(L"{228F36F9-3246-44B5-8E2B-C513A3B7435C}");
    if (!m_semaphore.open())    // 先打开, 打开失败就表示还没有创建这个信号量, 需要创建一个
    {
        return m_semaphore.create(1, 0x1000);
    }
    return true;
}

// 共享文件数据结构
struct MAPFILE_INFO
{
    DWORD tid;      // 配置程序的线程id, 投递消息可以往这个id里投递
    DWORD pid;      // 配置程序的进程ID
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    m_mapFileIn.setname(L"{228F36F9-3246-44B5-8E2B-C513A3B7435B}");
    m_mapFileOut.setname(L"{228F36F9-3246-44B5-8E2B-D513A3B7435B}");

    bool isOpen = m_mapFileIn.open();   // 如果能打开成功表示已经打开了配置进程了, 直接返回, 程序不往下执行
    if (isOpen)
    {
        m_semaphore++;
        return 1;
    }

    m_mapFileIn.create(0x900);
    m_mapFileOut.create(0x900);
    if (!_init_semaphore())
        return -2;  // 信号量初始化失败
    MAPFILE_INFO data;
    data.pid = GetCurrentProcessId();
    HANDLE hThread = CreateThread(0, 0, ThreadProc_onMessage, 0, 0, &data.tid);
    if (!hThread) return -100;

    while (true)
    {
        if (WaitForSingleObject(hThread, 1000) == 0)    // 每秒出来看一下信号量是否为0
            break;
    }
    
    CloseHandle(hThread);
    return 0;
}

// 线程消息回调, 需要调用功能的可以投递线程消息来处理
DWORD CALLBACK ThreadProc_onMessage(LPVOID pArg)
{
    MSG msg;
    // 主消息循环:
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        if (msg.message == WM_CLOSE)
            break;
        IConfigProc(msg.message, msg.wParam, msg.lParam);
    }
    return 0;
}

// 配置进程的消息处理函数
void IConfigProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message <= 0XFFFF)  // 消息都是IGI_ 开头的
    {

    }
    void* ptr = m_mapFileIn.data();


}