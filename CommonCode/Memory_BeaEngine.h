#pragma once
#include <windows.h>
extern "C"
{
    #include <BeaEngine_headers\BeaEngine.h>
}

// 设置BeaEngine的路径, 则默认为 L"BeaEngine.dll"
inline LPCWSTR SetBeaEngineDllPath(LPCWSTR BeaEngineFileName)
{
    static wchar_t file[260];
    if ((!BeaEngineFileName || !BeaEngineFileName[0]) && !file[0])    // 传入的路径为空, 且没有被赋值
        BeaEngineFileName = L"BeaEngine.dll";
    if (BeaEngineFileName)
        wcscpy_s(file, 260, BeaEngineFileName);
    return file;
}
// 设置BeaEngine的路径, 则默认为 L"BeaEngine.dll"
inline LPCWSTR GetBeaEngineDllPath()
{
    return SetBeaEngineDllPath(0);
}

// 成功返回字节数, 失败返回 UNKNOWN_OPCODE(-1)
inline int Disasm_BeaEngine(LPDISASM pDisAsm)
{
    int ret = UNKNOWN_OPCODE;
    typedef int(__cdecl* PFN_Disasm)(LPDISASM);
    static HMODULE hModule;
    static PFN_Disasm fun;
    if (!hModule)
    {
        hModule = LoadLibraryW(GetBeaEngineDllPath());
        if (!hModule)
            return ret;
        if (!fun)
            fun = (PFN_Disasm)GetProcAddress(hModule, "Disasm");
    }
    if (!fun)
        return ret;
    return fun(pDisAsm);
}

// 反汇编指定数据, 成功返回字节数, 失败返回 UNKNOWN_OPCODE(-1)
// pAddress = 需要转换成汇编的地址
// is64 = 是否解释为64位汇编
// pCompleteInstr = 反编译的汇编文本结果缓冲区, 需要至少80个字符, 如果为0则不接收结果
// err = 错误信息, 成功则为0
inline int Disasm_BeaEngine(UINT64 pAddress, bool is64, LPSTR pCompleteInstr = 0, int* err = 0)
{
    DISASM infos = { 0 };
    int len = 0;
    infos.Archi = is64 ? 0 : 1;
    infos.EIP = (UIntPtr)pAddress;

    int ret = Disasm_BeaEngine(&infos);
    if (err)
        *err = infos.Error;
    if (pCompleteInstr)
        strcpy_s(pCompleteInstr, 80, infos.CompleteInstr);
    return ret;
}
