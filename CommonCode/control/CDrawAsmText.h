#pragma once
#include <windows.h>

enum ASM_TEXT_FLAGS
{
    ASM_TEXT_FLAGS_GS       = 0x00000001,   // gs�Ĵ���
    ASM_TEXT_FLAGS_SS       = 0x00000002,   // �����μĴ���
    ASM_TEXT_FLAGS_JMP      = 0x00000004,   // ��תָ��
    ASM_TEXT_FLAGS_PUSH     = 0x00000008,   // push ָ��
    ASM_TEXT_FLAGS_POP      = 0x00000010,   // pop ָ��
    ASM_TEXT_FLAGS_RET      = 0x00000020,   // ret ָ��
    ASM_TEXT_FLAGS_CALL     = 0x00000040,   // call ָ��
    ASM_TEXT_FLAGS_NUM      = 0x00000080,   // ��ֵ����
    ASM_TEXT_FLAGS_ADDR     = 0x00000100,   // ��ַ
    ASM_TEXT_FLAGS_LOOP     = 0x00000200,   // loop
    ASM_TEXT_FLAGS_BREAK    = 0x00000400,   // �ж�ָ��


    ASM_TEXT_FLAGS_ERR      = 0x80000000,   // �޷�������ָ��

};

class CDrawAsmText
{
public:
    // ���ָ����ɫ��ɫ, ���еı�����ɫ˵���Ķ�ʹ��������ɫ
    struct ASMVIEW_COLOR
    {
        COLORREF gs_back;           // gs�Ĵ���
        COLORREF ss_text;           // �����μĴ���
        COLORREF jmp_back;          // ��תָ��
        COLORREF jmp_text;          // ��תָ��
        COLORREF jicunqi_text;      // ������������ļĴ���
        COLORREF kuohaonei_text;    // ������������
        COLORREF kuohao_back;       // ���ҷ�����
        COLORREF kuohao_text;       // ���ҷ�����
        COLORREF push_pop_text;     // push pop ָ�����ɫ
        COLORREF num_back;          // ��ֵ�����ı�����ɫ
        COLORREF num_text;          // ��ֵ����
        COLORREF ret_back;          // ����
        COLORREF ret_text;          // ����
        COLORREF call_text;         // call
        COLORREF call_back;         // call
    };

    ASMVIEW_COLOR color;    // ����ı���ɫ
    LPCWSTR m_pszAddr;      // ��ַ
    int     m_nAddrLen;     // ��ַ����
    CDrawAsmText()
    {
        m_pszAddr = 0;
        m_nAddrLen = 0;
        SwitchTheme(false);
    }
    virtual ~CDrawAsmText()
    {
        
    }



    // �л�����, ����Ϊ�Ƿ�Ϊ��ɫģʽ
    void SwitchTheme(bool isDark)
    {
        if (isDark)
        {
            color.gs_back           = RGB(128, 0, 0);
            color.ss_text           = RGB(255, 0, 255);
            color.kuohao_back       = RGB(0, 255, 255);
            color.kuohao_text       = RGB(30, 30, 30);
            color.kuohaonei_text    = RGB(0, 255, 255);
            color.num_back          = RGB(255, 255, 0);
            color.num_text          = RGB(0, 0, 0);
            color.jicunqi_text      = RGB(0, 255, 0);
            color.call_back         = RGB(255, 0, 0);
            color.call_text         = RGB(255, 255, 0);
            color.jmp_back          = RGB(255, 255, 0);
            color.jmp_text          = RGB(255, 0, 0);
            color.push_pop_text     = RGB(0, 128, 255);
            color.ret_back          = RGB(0, 255, 255);
            color.ret_text          = RGB(0, 0, 0);
        }
        else
        {
            color.gs_back           = RGB(192, 0, 0);
            color.ss_text           = RGB(255, 0, 252);
            color.kuohao_back       = RGB(0, 255, 255);
            color.kuohao_text       = RGB(230, 230, 230);
            color.kuohaonei_text    = RGB(176, 52, 121);
            color.num_back          = RGB(255, 255, 0);
            color.num_text          = RGB(0, 0, 0);
            color.jicunqi_text      = RGB(58, 131, 54);
            color.call_back         = RGB(0, 255, 255);
            color.call_text         = RGB(0, 0, 255);
            color.jmp_back          = RGB(255, 255, 0);
            color.jmp_text          = RGB(255, 0, 0);
            color.push_pop_text     = RGB(0, 0, 255);
            color.ret_back          = RGB(255, 0, 0);
            color.ret_text          = RGB(0, 0, 255);
        }
    }
    // ���û���ı���ɫ
    void SetColor(const ASMVIEW_COLOR* pClr)
    {
        if (pClr)
            memcpy(&color, pClr, sizeof(ASMVIEW_COLOR));
    }

public:
    
    static bool _cmp_str(LPCWSTR pszAsm, LPCWSTR psz, size_t cmpSize, int& retSize)
    {
        retSize = (int)cmpSize;
        bool cmp = _wcsnicmp(pszAsm, psz, cmpSize) == 0;
        if (cmp)
        {
            // �����һ���ַ�����ĸ��������, �Ǿ��ǲ����, ���ؼ�
            wchar_t ch = pszAsm[cmpSize];
            if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9'))
                return false;
        }
        return cmp;
    }
    
    #define DRAW_TEXT_MODE   1  // 1=DrawText, 0=TextOut
    #define _cmp(_s) (_cmp_str(pszAsm, _s, ( sizeof(_s) / sizeof(wchar_t) - 1 ), retCmpSize))

    inline bool AsmView_IsHex(const wchar_t ch, bool isH)
    {
        // a-f, A-F, 0-9, h/H ����Щ�ַ��ͷ�����
        const bool isHex = ( ( ch >= 'a' && ch <= 'f' || ch >= 'A' && ch <= 'F' ) || ch >= '0' && ch <= '9' );
        if ( isH )
            return isHex || ( ch == 'h' || ch == 'H' );
        return isHex;
    };

    // �����ո�
    inline void AsmView_DrawText_SkipSpace(HDC hdc, LPCWSTR& pszAsm, RECT& rcText)
    {
        int spaceLen = 0;
        while ( pszAsm[spaceLen] && pszAsm[spaceLen] == ' ' )
            spaceLen++;
        if ( spaceLen == 0 )
            return;

    #if DRAW_TEXT_MODE
        RECT rcCalc = rcText;
        const int fmt = DT_VCENTER | DT_SINGLELINE | DT_LEFT;
        DrawTextW(hdc, pszAsm, spaceLen, &rcCalc, fmt | DT_CALCRECT);
        rcText.left += ( rcCalc.right - rcCalc.left );  // �ı����λ��, �´λ滭�ʹ����￪ʼ
    #else
        SIZE si = { 0 };
        GetTextExtentPointW(hdc, pszAsm, spaceLen, &si);
        rcText.left += si.cx;   // �ı����λ��, �´λ滭�ʹ����￪ʼ
    #endif
        pszAsm += spaceLen;     // ָ��ո���������
    };


    // �滭�ı�, �滭���ı�ָ��滭���ı���һ���ַ�
    inline void AsmView_DrawText(HDC hdc, LPCWSTR& pszAsm, int size, RECT& rcText)
    {
    #if DRAW_TEXT_MODE
        RECT rcCalc = rcText;
        const int fmt = DT_VCENTER | DT_SINGLELINE | DT_LEFT;
        DrawTextW(hdc, pszAsm, size, &rcCalc, fmt | DT_CALCRECT);
        DrawTextW(hdc, pszAsm, size, &rcText, fmt);
        rcText.left += ( rcCalc.right - rcCalc.left );  // �ı����λ��, �´λ滭�ʹ����￪ʼ
    #else

        if ( size == -1 )
            size = (int)wcslen(pszAsm);
        SIZE si = { 0 };
        GetTextExtentPointW(hdc, pszAsm, size, &si);
        TextOutW(hdc, rcText.left, rcText.top, pszAsm, size);
        rcText.left += si.cx; // �ı����λ��, �´λ滭�ʹ����￪ʼ
    #endif
        pszAsm += size;
        AsmView_DrawText_SkipSpace(hdc, pszAsm, rcText);
    };

    // �жϴ��ݽ������ַ����ǲ���һ��16���Ƶ�ַ, �ǵĻ�����16���ƴ�����, ���Ǿͷ���0
    int AsmView_DrawAsm_isHex(LPCWSTR pszAsm)
    {
        LPCWSTR str = pszAsm;
        while ( true )
        {
            wchar_t ch = *str++;
            if ( !AsmView_IsHex(ch, true) )
                break;  // ����16���ƵĴ�������ѭ��
        }

        const int len = (int)(str - pszAsm - 1);
        if (len >= 8 || pszAsm[len - 1] == 'h')
            return len;
        
        return 0;
    }

    // �Ƿ��ǼĴ���, �ж�8/16/32/64 λ�Ĵ���, ���ؼĴ����ı�����, ���ǼĴ����ͷ���0
    int IsRegister(LPCWSTR pszAsm, DWORD& flags)
    {
        const LPCWSTR reg[] = {
            L"rax", L"rbx", L"rcx", L"rdx", L"rsp", L"rbp", L"rsi", L"rdi", L"rip",
            L"r8d", L"r9d", L"r10d", L"r11d", L"r12d", L"r13d", L"r14d", L"r15d",
            L"r8l", L"r9l", L"r10l", L"r11l", L"r12l", L"r13l", L"r14l", L"r15l",
            L"r8", L"r9", L"r10", L"r11", L"r12", L"r13", L"r14", L"r15",
            L"cr0", L"cr1", L"cr2", L"cr3", L"cr4", L"cr8",
            L"dr0", L"dr1", L"dr2", L"dr3", L"dr4", L"dr5", L"dr6", L"dr7",
            L"dr8", L"dr9", L"dr10", L"dr11", L"dr12", L"dr13", L"dr14", L"dr15",
            L"st0", L"st1", L"st2", L"st3", L"st4", L"st5", L"st6", L"st7",
            L"mm0", L"mm1", L"mm2", L"mm3", L"mm4", L"mm5", L"mm6", L"mm7",
            L"xmm0", L"xmm1", L"xmm2", L"xmm3", L"xmm4", L"xmm5", L"xmm6", L"xmm7",
            L"xmm8", L"xmm9", L"xmm10", L"xmm11", L"xmm12", L"xmm13", L"xmm14", L"xmm15",
            L"eax", L"ebx", L"ecx", L"edx", L"esp", L"ebp", L"esi", L"edi", L"eip",
            L"ax", L"bx", L"cx", L"dx", L"sp", L"bp", L"si", L"di", L"ip",
            L"al", L"bl", L"cl", L"dl", L"ah", L"bh", L"ch", L"dh",
        };
        const int arrLen = ARRAYSIZE(reg);

        for ( int i = 0; i < arrLen; i++ )
        {
            size_t len = wcslen(reg[i]);
            int retCmpSize = 0;
            if ( _cmp_str(pszAsm, reg[i], len, retCmpSize) )
            {
                wchar_t ch = (wchar_t)tolower(pszAsm[len]);
                if ( ( ch >= '0' && ch <= '9' ) || ( ch >= 'a' && ch <= 'z' ) )
                    return 0;   // ����Ĵ�������һ���ַ������ֻ�����ĸ, ���жϲ��ǼĴ���
                return (int)len;
            }
        }
        return 0;
    }

    bool Draw_cs_ss_ds(HDC hdc, RECT& rcText, LPCWSTR& pszAsm, DWORD& flags)
    {
        int retCmpSize = 0;
        if ( _cmp(L"ss") || _cmp(L"gs") || _cmp(L"ds") || _cmp(L"es") || _cmp(L"fs") || _cmp(L"cs") )
        {
            wchar_t ch = (wchar_t)tolower(pszAsm[2]);
            if ( ( ch >= '0' && ch <= '9' ) || ( ch >= 'a' && ch <= 'z' ) )
                return false;   // ���ss/gs  ��Щ�������ַ������ֻ�����ĸ, ���жϲ��ǶμĴ���
        
            COLORREF clr = 0, clrBack = 0;
            const bool isGs = *pszAsm == 'g';
            if ( isGs )
            {
                clrBack = SetBkColor(hdc, color.gs_back);
                flags |= ASM_TEXT_FLAGS_GS;
            }
            else
            {
                clr = SetTextColor(hdc, color.ss_text);
                flags |= ASM_TEXT_FLAGS_SS;
            }
            AsmView_DrawText(hdc, pszAsm, 2, rcText);
            if ( isGs )
            {
                SetBkColor(hdc, clrBack);
            }
            else
            {
                SetTextColor(hdc, clr);
            }

            if ( *pszAsm == L':' )
                AsmView_DrawText(hdc, pszAsm, 1, rcText);
            return true;
        }
        return false;
    }

    // �滭 [�м������] 
    void AsmView_DrawAsm_Memory(HDC hdc, RECT& rcText, LPCWSTR& pszAsm, DWORD& flags)
    {
        COLORREF clrBack = SetBkColor(hdc, color.kuohao_back);
        COLORREF clrText = SetTextColor(hdc, color.kuohao_text);
        AsmView_DrawText(hdc, pszAsm, 1, rcText);   // ��һ���ַ��϶���[
        SetBkColor(hdc, clrBack);
        SetTextColor(hdc, clrText);

        auto pfn_draw = [&](bool isFirst)
        {
            // �������ұ߿϶����в�����, Ҫô�ǵ�ַ, Ҫô�ǼĴ���
            int len = IsRegister(pszAsm, flags);
            if ( len )
            {
                // �ǼĴ���, ����������ļĴ�����Ҫ�����ı���ɫ
                clrText = SetTextColor(hdc, color.kuohaonei_text);
                AsmView_DrawText(hdc, pszAsm, len, rcText);
                SetTextColor(hdc, clrText);
                return true;
            }

            len = AsmView_DrawAsm_isHex(pszAsm);
            if ( len )
            {
                if ((flags & ASM_TEXT_FLAGS_ADDR) == 0)
                {
                    m_pszAddr = pszAsm;
                    m_nAddrLen = len;
                }
                flags |= ASM_TEXT_FLAGS_NUM;
                if (isFirst)
                {
                    // �������һ�������ǳ���
                    flags |= ASM_TEXT_FLAGS_ADDR;
                    clrBack = SetBkColor(hdc, color.num_back);
                    clrText = SetTextColor(hdc, color.num_text);
                    AsmView_DrawText(hdc, pszAsm, len, rcText);
                    SetBkColor(hdc, clrBack);
                    SetTextColor(hdc, clrText);
                }
                else
                {
                    AsmView_DrawText(hdc, pszAsm, len, rcText);
                }
                return true;
            }
            return false;
        };

        bool isOk = pfn_draw(true);
        if ( !isOk )
        {
            Draw_cs_ss_ds(hdc, rcText, pszAsm, flags);
            isOk = pfn_draw(true);

            if ( !isOk )
                __debugbreak();
        }

        // ����������滭
        while ( *pszAsm )
        {
            LPCWSTR oldStr = pszAsm;
            wchar_t ch = *pszAsm;
            switch ( ch )
            {
            case ']':
            {
                // �����Ž���, ��������������, ֱ�ӷ���
                clrBack = SetBkColor(hdc, color.kuohao_back);
                clrText = SetTextColor(hdc, color.kuohao_text);
                AsmView_DrawText(hdc, pszAsm, 1, rcText);
                SetBkColor(hdc, clrBack);
                SetTextColor(hdc, clrText);
                return;
            }
            case ',': case '+': case '-': case '*': case '/': case ':':
                AsmView_DrawText(hdc, pszAsm, 1, rcText);
                break;
            default:
            {
                isOk = pfn_draw(false);
                if ( !isOk )
                    AsmView_DrawText(hdc, pszAsm, 1, rcText);
                break;
            }
            }
            if ( oldStr == pszAsm )
                __debugbreak(); // �������û�д����κ��ַ�, �Ǿ���bug
        }

    }

    void AsmView_DrawAsm_2(HDC hdc, RECT& rcText, LPCWSTR& pszAsm, DWORD& flags)
    {
        while (*pszAsm)
        {
            LPCWSTR oldStr = pszAsm;
            wchar_t ch = *pszAsm;
            switch ( ch )
            {
            case '\0':
                return; // �ı�������, ����
            case '[':// ��������Ҫ�滭����
            {
                AsmView_DrawAsm_Memory(hdc, rcText, pszAsm, flags);
                break;
            }
            case ',': case '+': case '-': case '*': case '/': case ':':
                AsmView_DrawText(hdc, pszAsm, 1, rcText);
                break;
            default:
            {
                // dword ptr ֮����ı�, ���ߵ�����, ���滭һ��
                auto pfn_draw = [&]()
                {
                    // �������ұ߿϶����в�����, Ҫô�ǵ�ַ, Ҫô�ǼĴ���
                    int len = IsRegister(pszAsm, flags);
                    if ( len )
                    {
                        // �ǼĴ���, ������������ļĴ�����Ҫ����ͳһ�ı���ɫ
                        COLORREF clrText = SetTextColor(hdc, color.jicunqi_text);
                        AsmView_DrawText(hdc, pszAsm, len, rcText);
                        SetTextColor(hdc, clrText);
                        return true;
                    }

                    len = AsmView_DrawAsm_isHex(pszAsm);
                    if ( len )
                    {
                        if ((flags & ASM_TEXT_FLAGS_ADDR) == 0)
                        {
                            m_pszAddr = pszAsm;
                            m_nAddrLen = len;
                        }
                        COLORREF clrBack = SetBkColor(hdc, color.num_back);
                        COLORREF clrText = SetTextColor(hdc, color.num_text);
                        flags |= ASM_TEXT_FLAGS_NUM;
                        AsmView_DrawText(hdc, pszAsm, len, rcText);

                        SetBkColor(hdc, clrBack);
                        SetTextColor(hdc, clrText);
                        return true;
                    }
                    return false;
                };
                if ( !pfn_draw() )
                {
                    if ( Draw_cs_ss_ds(hdc, rcText, pszAsm, flags) )
                        break;

                    int len = 0;    // ָ���
                    while ( pszAsm[len] && pszAsm[len] != ' ' )
                        len++;
                    AsmView_DrawText(hdc, pszAsm, len, rcText);
                }

                break;
            }
            }
            if ( pszAsm == oldStr )
                __debugbreak(); // �������û�д����κ��ַ�, �Ǿ���bug
        }

    }



    void AsmView_DrawAsm(HDC hdc, RECT* prc, LPCWSTR pszAsm, DWORD& flags)
    {
        flags = 0;
        m_pszAddr = 0;
        m_nAddrLen = 0;
        RECT rcText = *prc; rcText.left += 6;
        COLORREF clrBack = 0, clrText = 0;
        int retCmpSize = 0; // �Ƚϵ��ַ�������, ����ж�������Ļ��Ϳ���ʹ�����ֵ
        if ( _cmp(L"call") )
        {
            // ��call, ��Ҫ��䱳�� + ָ����ɫ
            flags |= ASM_TEXT_FLAGS_CALL;
            clrBack = SetBkColor(hdc, color.call_back);
            clrText = SetTextColor(hdc, color.call_text);
            AsmView_DrawText(hdc, pszAsm, retCmpSize, rcText);
            SetBkColor(hdc, clrBack);
            SetTextColor(hdc, clrText);
            AsmView_DrawAsm_2(hdc, rcText, pszAsm, flags);        // �滭ʣ�µĲ���
        }
        else if ( pszAsm[0] == 'j' || pszAsm[0] == 'J' )    // ��תָ��
        {
            flags |= ASM_TEXT_FLAGS_JMP;
            int len = 0;    // ָ���
            while ( pszAsm[len] && pszAsm[len] != ' ' )
                len++;
            clrBack = SetBkColor(hdc, color.jmp_back);
            clrText = SetTextColor(hdc, color.jmp_text);

            AsmView_DrawText(hdc, pszAsm, len, rcText);
            SetBkColor(hdc, clrBack);
            SetTextColor(hdc, clrText);
            AsmView_DrawAsm_2(hdc, rcText, pszAsm, flags);
        }
        else if ( _cmp(L"push") || _cmp(L"pusha") || _cmp(L"pushad") || _cmp(L"pushf") || _cmp(L"pushd"))
        {
            flags |= ASM_TEXT_FLAGS_PUSH;
            clrText = SetTextColor(hdc, color.push_pop_text);
            AsmView_DrawText(hdc, pszAsm, retCmpSize, rcText);
            SetTextColor(hdc, clrText);
            AsmView_DrawAsm_2(hdc, rcText, pszAsm, flags);
        }
        else if ( _cmp(L"pop") || _cmp(L"popa") || _cmp(L"popad") || _cmp(L"popf") || _cmp(L"popd"))
        {
            flags |= ASM_TEXT_FLAGS_POP;
            clrText = SetTextColor(hdc, color.push_pop_text);
            AsmView_DrawText(hdc, pszAsm, retCmpSize, rcText);
            SetTextColor(hdc, clrText);
            AsmView_DrawAsm_2(hdc, rcText, pszAsm, flags);
        }
        else if (_cmp(L"loop") || _cmp(L"loope") || _cmp(L"loopz") || _cmp(L"loopne") || _cmp(L"loopnz"))
        {
            flags |= ASM_TEXT_FLAGS_LOOP;
            clrText = SetTextColor(hdc, color.push_pop_text);
            AsmView_DrawText(hdc, pszAsm, retCmpSize, rcText);
            SetTextColor(hdc, clrText);
            AsmView_DrawAsm_2(hdc, rcText, pszAsm, flags);
        }
        else if (_cmp(L"int") || _cmp(L"iret"))
        {
            flags |= ASM_TEXT_FLAGS_BREAK;
            int len = 0;    // ָ���
            while (pszAsm[len] && pszAsm[len] != ' ')
                len++;
            AsmView_DrawText(hdc, pszAsm, len, rcText);
            SetTextColor(hdc, clrText);
            AsmView_DrawAsm_2(hdc, rcText, pszAsm, flags);
        }
        else if ( _cmp(L"ret") || _cmp(L"retf") || _cmp(L"retn"))
        {
            flags |= ASM_TEXT_FLAGS_RET;
            clrBack = SetBkColor(hdc, color.ret_back);
            clrText = SetTextColor(hdc, color.ret_text);

            int len = 0;    // ָ���
            while ( pszAsm[len] && pszAsm[len] != ' ' )
                len++;
            AsmView_DrawText(hdc, pszAsm, len, rcText);
            SetBkColor(hdc, clrBack);
            SetTextColor(hdc, clrText);
            AsmView_DrawText(hdc, pszAsm, -1, rcText);
        }
        else if ( *pszAsm == '?' )
        {
            // ���ʺ�, ���ܽ������ֽ�
            flags |= ASM_TEXT_FLAGS_ERR;
            AsmView_DrawText(hdc, pszAsm, (int)wcslen(pszAsm), rcText);
        }
        else
        {
            // ����ָ���Ĭ�ϻ滭
            AsmView_DrawAsm_2(hdc, rcText, pszAsm, flags);
        }


    }
    #undef _cmp


};


