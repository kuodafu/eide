#include "ThemeResource.h"
#include <zlib/Czlib.h>
#include <map>
#include <vector>
#include <queue>

THEME_BEGIN_NAMESPACE
static std::vector<char*> s_hash_array;      // �ı���ϣ��һ���ڴ�, �����ϣ�ͷ��������ַ, ��Ҫ�ͷ�

LONG_PTR _hash_fun(const char* pstr)
{
    if (!pstr || !pstr[0])return 0;
    for (auto i = s_hash_array.begin(); i != s_hash_array.end(); ++i)
    {
        const char* p = *i;
        if (p && strcmp(p, pstr) == 0)
            return (LONG_PTR)p;
    }
    size_t len = strlen(pstr) + 1;
    char* p = new char[len];
    memcpy(p, pstr, len);
    s_hash_array.push_back(p);
    return (LONG_PTR)p;
}

typedef struct THEME_ARRAY
{
    LPTHEMERESOURCE_FILE_HEADER hHeader;    // ���ص���Դ�ļ�
    std::vector<LPTHEMERESOURCE>* arr;      // ��Դ�ļ���Ӧ����������
}*LPTHEME_ARRAY;

static std::map<const void*, THEME_ARRAY> s_map_uncompress;    // ��¼�Ѿ���ѹ������

template<typename T> T* alloc(size_t size)
{
    T* ret = (T*)malloc(size * sizeof(T));
    if (!ret)throw;
    memset(ret, 0, size * sizeof(T));
    return ret;
}

typedef void (WINAPI* pfn_free_memory_callback)(void* p, int type);
#define FREE_CHAR   1
#define FREE_ARR    2
inline void WINAPI _free_memory_char(void* p, int type)
{
    if (type == FREE_CHAR)
    {
        char* ptr = (char*)p;
        delete[] ptr;
    }
    else if (type == FREE_ARR)
    {
        std::vector<LPTHEMERESOURCE>* pArr = (std::vector<LPTHEMERESOURCE>*)p;
        pArr->clear();
        delete pArr;
    }
}
class _free_memory
{
    struct free_data
    {
        int type;
        void* ptr;
        free_data(void* ptr, int type)
        {
            this->ptr = ptr;
            this->type = type;
        }
    };
    std::queue<free_data> m_data;
public:
    _free_memory()
    {

    }
    ~_free_memory()
    {
        while (m_data.size())
        {
            free_data& data = m_data.front();
            _free_memory_char(data.ptr, data.type);
            m_data.pop();
        }

    }

    inline void push_back(void* ptr, int type)
    {
        m_data.push(free_data(ptr, type));
    }

};

// ��һ����Ҫ�ͷŵ��ڴ���ӵ��ṹ��, ����ʱ���ò���2����Ҫ�ͷŵĵ�ַ���ݹ�ȥ, �Լ����������ͷ�
// ptr = ��Ҫ�ͷŵ�ָ��
// type = ָ������, ���ûص�����ʱ���ݵ��ص�������, �����Ϊ����ͷ�ָ��ı�ʶ
// pfnCallback = �ͷ��ڴ�ص�����, ����ʱ��������ص�����
inline void _append_free_memory(void* ptr, int type)
{
    static _free_memory mem;
    mem.push_back(ptr, type);
}

// �����������, ��N�������زĴ����һ������, ��ѹ������, ��ʹ��ʱ��Ҫ���� Theme_free() �ͷ�����
// ���ݽṹ = THEME_FILE_HEADER + (�ز����� + ͼƬ����)
// pData = �زĽṹ����
// dwSize = �ز�����
// pImage = ͼƬ����
// dwImageSize = ͼƬ���ݴ�С
LPTHEME_FILE_HEADER PackThemeData(LPTHEME_DATA pData, DWORD dwSize, const void* pImage, DWORD dwImageSize)
{
    if (!pData || !dwSize || !pImage || !dwImageSize) return 0;

    DWORD dwDataLength = sizeof(THEME_DATA) * dwSize + dwImageSize; // �����ز����� + ͼƬ����
    DWORD len = dwDataLength + sizeof(THEME_FILE_HEADER);   // �ܳߴ� = �����ز����� + ͼƬ���� + �����ļ�ͷ��С

    DWORD offset = sizeof(THEME_FILE_HEADER);
    char* ptr = new char[len];
    LPTHEME_FILE_HEADER head = (LPTHEME_FILE_HEADER)ptr;
    head->cbSize = sizeof(THEME_FILE_HEADER);
    head->dwCount = dwSize;
    head->dwDataLength = dwDataLength;
    head->dwMaxSize = dwImageSize;
    head->dwReserved = 0;

    LPTHEME_DATA data = (LPTHEME_DATA)(head + 1);   // �ļ�ͷ���������ز�����
    memcpy(ptr + offset, pData, dwSize * sizeof(THEME_DATA));   // ֱ�ӿ�������
    offset += (dwSize * sizeof(THEME_DATA));

    memcpy(ptr + offset, pImage, dwImageSize);      // �ز�����������ͼƬ����
    offset += dwImageSize;
    return head;
}

// �����������, �����������������زĵ�����, ������ز������ͼƬ����, �����ز�����ָ��, ����Ҫ�ֶ��ͷ�, ���صĵ�ַ�ǵ�һ������ָ��ĵ�ַ
// ͼƬ�ߴ��С�������Ա���� pHeader ָ��Ľṹ��
// ���ݽṹ = THEME_FILE_HEADER + ѹ��(�ز����� + ͼƬ����)
// pHeader = ��������
// ppImage = ����ͼƬ����ָ��, ����Ҫ�ֶ��ͷ�
LPTHEME_DATA UnPackThemeData(LPTHEME_FILE_HEADER pHeader, void** ppImage)
{
    if (!pHeader) return 0;
    LPTHEME_DATA ptr = (LPTHEME_DATA)(pHeader + 1);

    if (ppImage)
    {
        DWORD offset = sizeof(THEME_DATA) * pHeader->dwCount;   // ƫ�� = �زĽṹ�ߴ� * �ز�����
        *ppImage = ((char*)ptr) + offset;
    }
    return (LPTHEME_DATA)ptr;
}

// ����Դ�ļ���ȡ��Դ���
HRESOURCE GetResourceHandle(const void* data)
{
    const void* hResource = data;
    auto it = s_map_uncompress.find(hResource);         // ��map�������û�н��������ָ��
    bool isFind = it != s_map_uncompress.end();         // �Ƿ���ҳɹ�
    if (!isFind)    //  �ҵ�, ��ʾ�������ַ, ֱ�ӷ��ؾ���
    {
        return (HRESOURCE)it->second.hHeader;
    }

    for (auto it = s_map_uncompress.begin(); it != s_map_uncompress.end(); ++it)
    {
        if (it->second.hHeader == data)
            return (HRESOURCE)it->second.hHeader;
    }
    return 0;
}

inline LPTHEMERESOURCE_FILE_HEADER _theme_parse(const void* hResource, std::vector<LPTHEMERESOURCE>** ppArr = 0)
{
    if (!hResource) return 0;
    LPTHEMERESOURCE_FILE_HEADER hHead = (LPTHEMERESOURCE_FILE_HEADER)hResource;
    if (hHead->head != MAKELONG('R', 'S')) return 0;    // �ȼ�鴫�ݽ����������Ƿ�Ϸ�

    LPTHEMERESOURCE_FILE_HEADER hHeader_File;           // ��Դ�ļ��Ľṹ
    auto it = s_map_uncompress.find(hResource);         // ��map�������û�н��������ָ��
    bool isNewData = it == s_map_uncompress.end();      // �Ƿ�����������ڴ�
    std::vector<LPTHEMERESOURCE>* pArr;
    if (isNewData)
    {
        const void* pUncompress = (const void*)(hHead + 1);
        Czlib zlib;
        uLongf uUncompressSize;
        Bytef* pUncompressData = zlib.uncompress(pUncompress, &uUncompressSize);
        // ��ѹʧ�ܻ��߽�ѹ�����ĳ��Ȳ���ԭ���ĳ���, �ͷ���0
        if (uUncompressSize == 0 || !pUncompressData || uUncompressSize != hHead->dwDataLength) return 0;

        // �µĳ����� �ļ�ͷ�ĳ��� + ԭ��δѹ��ǰ�ĳ���
        DWORD size = sizeof(THEMERESOURCE_FILE_HEADER);
        size += hHead->dwDataLength;
        char* ptr = new char[size];
        _append_free_memory(ptr, FREE_CHAR);
        memset(ptr, 0, size);
        hHeader_File = (LPTHEMERESOURCE_FILE_HEADER)ptr;
        memcpy(hHeader_File, hHead, hHead->cbSize);     // ǰ��һ���ļ�ͷ + �����ѹ�������
        memcpy(ptr + hHead->cbSize, pUncompressData, uUncompressSize);

        THEME_ARRAY& arrData = s_map_uncompress[hResource];
        arrData.arr = new std::vector<LPTHEMERESOURCE>;
        _append_free_memory(arrData.arr, FREE_ARR);
        arrData.hHeader = hHeader_File;     // �ѽ�ѹ������ݵ�ַ��¼����, �´δ�����ͬ����Դָ���ʹ�������ַ
        pArr = arrData.arr;

        LPTHEMERESOURCE pResource = (LPTHEMERESOURCE)(hHeader_File + 1);
        for (DWORD i = 0; i < hHeader_File->dwCount; i++)   // ���ѭ������
        {
            pArr->push_back(pResource);
            char* pTmp = (char*)(pResource + 1);    // ָ����������, ���û�ҵ�, �Ǿʹ������ַ��������ݳ���
            char* pNext = pTmp + pResource->dwDataLength;   // ���ݵ�ַ + ���ݳ��� = ��һ����Դ��ַ
            pResource = (LPTHEMERESOURCE)pNext;
        }

    }
    else
    {
        THEME_ARRAY& arrData = it->second;
        hHeader_File = arrData.hHeader;
        pArr = arrData.arr;
    }
    if (ppArr)*ppArr = pArr;
    return hHeader_File;
}

// ����ָ������, ָ��ID����Դ, �����ҵ�����Դ���ݾ��, ���Ե��� LoadResource ����ȡ��Դ����, SizeofResource() ��ȡ���ݴ�С
// hResource = ��Դ��ָ��, �ڲ����¼��ָ��, ��μ���ֻ���ѹһ��
// id = ��Դ��id, ��ͬ��Դ���Ͳ�������ͬid
// lpType = ��Դ����, ���� "BIN", "PNG", "TXT"....�ȵ�, ���֧��20���ֽ�, ��Դ�������ִ�Сд
HFINDRES FindResource(const void* hResource, int id, LPCSTR lpType)
{
    if (!hResource || !lpType || !lpType[0]) return 0;
    std::vector<LPTHEMERESOURCE>* pArr;
    LPTHEMERESOURCE_FILE_HEADER hHeader_File = _theme_parse(hResource, &pArr);

    if (!hHeader_File || !pArr)return 0;
    size_t len = strlen(lpType); if (len > 20)len = 20;

    for (auto it = pArr->begin(); it != pArr->end(); ++it)   // ���ѭ������
    {
        LPTHEMERESOURCE pResource = *it;
        if (pResource && strncmp(pResource->type, lpType, len) == 0 && pResource->id == id)
            return (HFINDRES)pResource;
    }

    return 0;
}



// ��ȡָ���ڴ���ָ����Դ�ĵ�һ���ֽڵ�ָ��, ���ص�ָ�벻���ͷŲ����޸�
// hResInfo = Ҫ���ص���Դ�ľ��, �þ����FindResource��������
const void* LoadResource(HFINDRES hResInfo)
{
    if (!hResInfo) return 0;
    LPTHEMERESOURCE pResource = (LPTHEMERESOURCE)hResInfo;
    if (pResource->cbSize != sizeof(THEMERESOURCE)) return 0;
    pResource++;    // ����ṹ�ĺ������������, ֱ��++����ָ��������
    return pResource;
}

// ����ָ����Դ�Ĵ�С (���ֽ�Ϊ��λ), �����ɹ�, �򷵻�ֵ����Դ�е��ֽ���
// hResInfo = ��Դ�ľ��, ����ʹ��FindResource���������˾��
DWORD SizeofResource(HFINDRES hResInfo)
{
    if (!hResInfo) return 0;
    LPTHEMERESOURCE pResource = (LPTHEMERESOURCE)hResInfo;
    if (pResource->cbSize != sizeof(THEMERESOURCE)) return 0;
    return pResource->dwDataLength;
}


// ������ݵ���Դ, ֻ����ӵ��ڴ���, ��������ļ������޸�, ��Ҫ��Ӻ����������� PackResource()
// hResource = ��Դ��ָ��, �ڲ����¼��ָ��, ��μ���ֻ���ѹһ��
// id = ��Դ��id, ��ͬ��Դ���Ͳ�������ͬid
// lpType = ��Դ����, ���� "BIN", "PNG", "TXT"....�ȵ�, ���֧��20���ֽ�, ��Դ�������ִ�Сд
// pData = Ҫ��ӵ�����
// dwDataSize = Ҫ��ӵ����ݴ�С
size_t AppendResource(const void* hResource, int id, LPCSTR lpType, const void* pData, DWORD dwDataSize)
{
    if (!hResource || !lpType || !lpType[0] || !pData || !dwDataSize) return 0;

    std::vector<LPTHEMERESOURCE>* pArr;
    LPTHEMERESOURCE_FILE_HEADER hHeader_File = _theme_parse(hResource, &pArr);
    if (!hHeader_File || !pArr)return 0;

    DWORD size = sizeof(THEMERESOURCE) + dwDataSize;    // ��������ڴ�����������ṹ��С + ���ݴ�С
    char* ptr = new char[size];
    _append_free_memory(ptr, FREE_CHAR);
    memset(ptr, 0, size);
    size_t len = strlen(lpType); if (len > 20)len = 20;

    LPTHEMERESOURCE pResource = (LPTHEMERESOURCE)ptr;
    pResource->cbSize = sizeof(THEMERESOURCE);
    pResource->id = id;
    memcpy(pResource->type, lpType, len);
    pResource->dwDataLength = dwDataSize;
    pResource->dwReserved1 = 0;
    pResource->dwReserved2 = 0;
    pArr->push_back(pResource);

    pResource++;
    memcpy(pResource, pData, dwDataSize);
    return pArr->size();
}



THEME_END_NAMESPACE