#pragma once
#include <windows.h>
extern "C"
{
    #include <BeaEngine_headers\BeaEngine.h>
}

// ����BeaEngine��·��, ��Ĭ��Ϊ L"BeaEngine.dll"
inline LPCWSTR SetBeaEngineDllPath(LPCWSTR BeaEngineFileName)
{
    static wchar_t file[260];
    if ((!BeaEngineFileName || !BeaEngineFileName[0]) && !file[0])    // �����·��Ϊ��, ��û�б���ֵ
        BeaEngineFileName = L"BeaEngine.dll";
    if (BeaEngineFileName)
        wcscpy_s(file, 260, BeaEngineFileName);
    return file;
}
// ����BeaEngine��·��, ��Ĭ��Ϊ L"BeaEngine.dll"
inline LPCWSTR GetBeaEngineDllPath()
{
    return SetBeaEngineDllPath(0);
}

// �ɹ������ֽ���, ʧ�ܷ��� UNKNOWN_OPCODE(-1)
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

// �����ָ������, �ɹ������ֽ���, ʧ�ܷ��� UNKNOWN_OPCODE(-1)
// pAddress = ��Ҫת���ɻ��ĵ�ַ
// is64 = �Ƿ����Ϊ64λ���
// pCompleteInstr = ������Ļ���ı����������, ��Ҫ����80���ַ�, ���Ϊ0�򲻽��ս��
// err = ������Ϣ, �ɹ���Ϊ0
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
