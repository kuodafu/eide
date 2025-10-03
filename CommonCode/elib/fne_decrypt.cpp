#include "fne_decrypt.h"
#include <assist/assist.h>

BOOL Deserialize(const _str& SaveStr40, int& offset, std::vector<_str>* arr);
void fneGetData(_str& leftData, _str& rightData, std::vector<_str>& fneNameArr, int& num);

_str _get_SaveStr40()
{
    HKEY hKey;
    _str ret;
    LSTATUS status = RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\FlySky\\E\\EInf40", 0, KEY_READ, &hKey);
    if (ERROR_SUCCESS == status)
    {
        DWORD size = 0;
        status = RegQueryValueExW(hKey, L"SaveStr40", 0, NULL, (LPBYTE)0, &size);
        if (ERROR_SUCCESS == status)
        {
            ret.resize(size);
            status = RegQueryValueExW(hKey, L"SaveStr40", 0, NULL, (LPBYTE)ret.data(), &size);
        }

        RegCloseKey(hKey);
    }
    return ret;
}

// 返回错误码
int _set_SaveStr40(const _str& data)
{
    HKEY hKey;

    LSTATUS status = RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\FlySky\\E\\EInf40", 0, KEY_SET_VALUE, &hKey);
    if (ERROR_SUCCESS == status)
    {
        status = RegSetValueExW(hKey, L"SaveStr40", NULL, REG_BINARY, (LPBYTE)data.c_str(), (DWORD)data.size());
        RegCloseKey(hKey);
    }
    return status;
}

// 拼接数据, 拼接成 支持库名字长度 + 支持库名字 + 支持库名字长度 + 支持库名字
// 支持库名字没有\0结尾, 名字后面紧跟着下一个支持库名字的长度
_str Serializable(std::vector<_str>& arr)
{
    _str buf;
    int dwLen = (int)arr.size();
    buf.append((LPSTR)&dwLen, 4);

    for (int i = 0; i < dwLen; i++)
    {
        _str& name = arr[i];
        int len = (int)name.size();
        buf.append((LPCSTR)&len, 4);
        buf.append(name.c_str(), len);
    }
    return buf;
}

//ok
void encode(LPSTR data, int a, int b)
{
    //__asm
    //{
    //    push esi
    //    mov esi,a
    //    mov eax,1
    //    cmp esi,eax
    //    jl exit
    //    mov ecx,data
    //    mov ecx,[ecx]
    //    add ecx,8
    //    push ebx
    //    loop1:
    //    mov bl,byte ptr ds:[ecx]
    //    mov edx,eax
    //    and edx,3
    //    mov dl,byte ptr ss:[ebp+edx+16]
    //    xor bl,dl
    //    inc eax
    //    mov byte ptr ds:[ecx],bl
    //    inc ecx
    //    cmp eax,esi
    //    jle loop1
    //    pop ebx
    //    exit:
    //    pop esi

    //}
    if (a < 1)
    {
        return;
    }
    int i;
    int edx = 0;
    BYTE* pb = (BYTE*)&b;
    BYTE bl = 0;
    for (i = 0; i < a; i++)
    {
        edx = ((i + 1) & 3);
        bl = *(pb + edx);
        data[i] = data[i] ^ bl;
    }
}

//ok
_str DecFne(_str& data, int& num)
{
    int a = 0, b = 0;
    int* tmp = (int*)data.c_str();
    size_t dataLen = data.size();
    _str en;
    if (dataLen >= 12)
    {
        a = tmp[1];
        num = tmp[2];
        if (a <= (int)dataLen)
        {
            b = a ^ 19731115;
            a -= 12;
            en.assign((LPCSTR)data.c_str() + 12, a);
            encode(en.data(), a, b);
        }
    }
    return en;
}

// 配置信息加密
_str EncryFne(const _str& data, int num)
{
    if (num == NULL)
    {
        int offset = 0;
        while (Deserialize(data, offset, 0))
        {
            num++;
        }
    }
    int dwLen = (int)data.size();
    int a = dwLen + 12;
    int b = (a ^ 19731115);

    char tmp[4] = { 5, 0, 5, 0 };
    encode(data.data(), dwLen, b);
    _str ret;
    ret.append(tmp, 4);
    ret.append((LPCSTR)&a, 4);
    ret.append((LPCSTR)&num, 4);
    ret.append(data.c_str(), dwLen);
    return ret;
}

// 2020-09-09 ok
// 解密后结构为 前12个字节为空, 下来是已开启支持库数量4字节
// 下来就是支持库名字长度(4字节)+支持库名字 + 下一个支持库长度(4字节) + 支持库名字, 支持库名字没有\0结尾
BOOL Deserialize(const _str& SaveStr40, int& offset, std::vector<_str>* arr)
{
    int dwLen = 0, i = 0, num = 0;
    int* pData = 0;
    HANDLE* pArr = 0;
    int SaveStr40Len = (int)SaveStr40.size();

    if (offset >= 0)
    {
        if (offset < 0)
        {
            offset = 0;
        }
        if (SaveStr40Len >= (offset + 3))
        {
            pData = (int*)(SaveStr40.c_str() + offset);
            dwLen = *pData;
            offset += 4;
            if (arr) //数组有数据
            {
                arr->resize(dwLen);
            }

            for (i = 0; i < dwLen; i++)
            {
                pData = (int*)(SaveStr40.c_str() + offset);
                num = *pData;
                offset += 4;
                if (arr)
                {
                    arr->at(i) = SaveStr40.substr(offset, num);
                }
                offset += num;
            }
            return TRUE;
        }
    }
    return FALSE;
}

void fneGetData(_str& leftData, _str& rightData, std::vector<_str>& fneNameArr, int& num)
{
    BOOL bRet = FALSE;
    int i = 0;
    _str value = _get_SaveStr40();

    _str SaveStr40 = DecFne(value, num);
    int offset = 0;
    for (i = 0; i < 4; i++)
    {
        Deserialize(SaveStr40, offset, NULL);;
    }

    leftData = SaveStr40.left(offset);
    Deserialize(SaveStr40, offset, &fneNameArr);

    rightData = SaveStr40.right(SaveStr40.size() - offset);
}

namespace fne
{
    bool deleteFne(const char* fneName)
    {
        int num = 0;
        HANDLE* pArr = 0;
        _str leftData, rightData, midData;
        std::vector<_str> fneNameArr(true, 0);
        fneGetData(leftData, rightData, fneNameArr, num);

        for (size_t i = 0; i < fneNameArr.size(); i++)
        {
            _str& name = fneNameArr[i];
            if (name.compare(fneName, false) == 0)
            {
                fneNameArr.erase(fneNameArr.begin() + i);

                _str SaveStr40;
                midData = Serializable(fneNameArr);
                SaveStr40.append(leftData.c_str(), leftData.size());
                SaveStr40.append(midData.c_str(), midData.size());
                SaveStr40.append(rightData.c_str(), rightData.size());
                SaveStr40 = EncryFne(SaveStr40, num);
                int ret = _set_SaveStr40(SaveStr40);
                return ret == 0;
            }
        }
        return false;
    }


    bool addFne(const char* fneName)
    {
        int num = 0;
        _str leftData, rightData, midData;
        std::vector<_str> fneNameArr(true, 0);
        fneGetData(leftData, rightData, fneNameArr, num);

        for (size_t i = 0; i < fneNameArr.size(); i++)
        {
            _str& name = fneNameArr[i];
            if (name.compare(fneName, false) == 0)
            {
                return true;
            }
        }
        fneNameArr.push_back(fneName);

        _str SaveStr40;
        midData = Serializable(fneNameArr);
        SaveStr40.append(leftData.c_str(), leftData.size());
        SaveStr40.append(midData.c_str(), midData.size());
        SaveStr40.append(rightData.c_str(), rightData.size());

        SaveStr40 = EncryFne(SaveStr40, num);
        int ret = _set_SaveStr40(SaveStr40);
        return ret == 0;
    }

    bool isFne(const char* fneName, std::vector<_str>* names)
    {
        int num = 0;
        _str leftData, rightData, midData;
        std::vector<_str> fneNameArr;
        fneGetData(leftData, rightData, fneNameArr, num);

        for (size_t i = 0; i < fneNameArr.size(); i++)
        {
            _str& name = fneNameArr[i];
            if (name.compare(fneName, false) == 0)
            {
                if (names)names->swap(fneNameArr);
                return true;
            }
        }
        if (names)names->swap(fneNameArr);
        return false;
    }

    size_t fneList(std::vector<_str>& names)
    {
        _str leftData, rightData, midData;
        int num;
        fneGetData(leftData, rightData, names, num);

        return num;
    }

}

