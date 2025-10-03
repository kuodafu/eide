#pragma once
#include <IDraw_Header.h>
#include <hook_detours/detours/detours.h>
#include <hook/MinHook.h>

class CIDC_Fun
{
public:
    // ����ԭhook��ַ, ����ֱ�ӵ���
    // pOldFun = ��hook�ĵ�ַ
    // pNewFun = ����hook�ĵ�ַ
    // hookMode = hook��ʽ, 0=detoursHook, 1=MinHook
    LPVOID InlineHook(LPVOID pOldFun, LPVOID pNewFun, int hookMode)
    {
        INLINEHOOK_CALLBACK* hook = prop_GetInlineHookCallback(true);
        LPVOID oldFun = pOldFun;
        switch (hookMode)
        {
        case HOOKMODE_DETOURS:
        {

            DetourTransactionBegin();               // ���ؿ�ʼ
            DetourUpdateThread(GetCurrentThread()); // ˢ�µ�ǰ�߳�
            DetourAttach(&pOldFun, pNewFun);
            DetourTransactionCommit();              // ������Ч
            INLINEHOOK_CALLBACK_DATA& data = (*hook)[pOldFun];
            data.hookMode = HOOKMODE_DETOURS;
            data.pNewFun = pNewFun;
            data.pOldFun = oldFun;
            return pOldFun;
        }
        case HOOKMODE_MINHOOK:
        {
            int ret = MH_Initialize();
            if (ret != MH_OK && ret != MH_ERROR_ALREADY_INITIALIZED)
                return 0;
            LPVOID pRetFun = 0;

            if (MH_CreateHook(pOldFun, pNewFun, &pRetFun) != MH_OK)
                return 0;
            
            MH_EnableHook(pOldFun);
            
            INLINEHOOK_CALLBACK_DATA& data = (*hook)[pRetFun];
            data.hookMode = HOOKMODE_MINHOOK;
            data.pNewFun = pNewFun;
            data.pOldFun = oldFun;

            return pRetFun;
        }
        default:
            return 0;
        }

    }

    // ����ԭhook��ַ, ����ֱ�ӵ���
    // pOldFun = ��hook�ĵ�ַ
    // hookMode = hook��ʽ, 0=detoursHook, 1=MinHook
    bool UnInlineHook(LPVOID pOldFun)
    {
        INLINEHOOK_CALLBACK* hook = prop_GetInlineHookCallback(true);
        if (!hook) return false;
        auto it = hook->find(pOldFun);
        if (it == hook->end()) return false;
        INLINEHOOK_CALLBACK_DATA& data = it->second;
        bool ret = false;

        switch (data.hookMode)
        {
        case HOOKMODE_DETOURS:
        {
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            DetourDetach(&pOldFun, data.pNewFun);           // �������غ���
            ret = DetourTransactionCommit() == NO_ERROR;    // ������Ч
            break;
        }
        case HOOKMODE_MINHOOK:
        {
            if (MH_DisableHook(data.pOldFun) != MH_OK)
                return false;
            MH_Uninitialize();
            break;
        }
        default:
            return false;
        }

        hook->erase(it);
        return ret;
    }



};

