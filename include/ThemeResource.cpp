#include "ThemeResource.h"
#include <zlib/Czlib.h>
#include <map>
#include <vector>
#include <queue>

THEME_BEGIN_NAMESPACE
static std::vector<char*> s_hash_array;      // 文本哈希是一块内存, 计算哈希就返回这个地址, 需要释放

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
    LPTHEMERESOURCE_FILE_HEADER hHeader;    // 加载的资源文件
    std::vector<LPTHEMERESOURCE>* arr;      // 资源文件对应的所有主题
}*LPTHEME_ARRAY;

static std::map<const void*, THEME_ARRAY> s_map_uncompress;    // 记录已经解压的数据

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

// 把一个需要释放的内存添加到结构里, 析构时调用参数2把需要释放的地址传递过去, 自己根据类型释放
// ptr = 需要释放的指针
// type = 指针类型, 调用回调函数时传递到回调函数中, 这个作为如何释放指针的标识
// pfnCallback = 释放内存回调函数, 析构时调用这个回调函数
inline void _append_free_memory(void* ptr, int type)
{
    static _free_memory mem;
    mem.push_back(ptr, type);
}

// 打包主题数据, 把N个主题素材打包成一个主题, 不压缩数据, 不使用时需要调用 Theme_free() 释放数据
// 数据结构 = THEME_FILE_HEADER + (素材数组 + 图片数据)
// pData = 素材结构数组
// dwSize = 素材数量
// pImage = 图片数据
// dwImageSize = 图片数据大小
LPTHEME_FILE_HEADER PackThemeData(LPTHEME_DATA pData, DWORD dwSize, const void* pImage, DWORD dwImageSize)
{
    if (!pData || !dwSize || !pImage || !dwImageSize) return 0;

    DWORD dwDataLength = sizeof(THEME_DATA) * dwSize + dwImageSize; // 所有素材数组 + 图片数据
    DWORD len = dwDataLength + sizeof(THEME_FILE_HEADER);   // 总尺寸 = 所有素材数组 + 图片数据 + 主题文件头大小

    DWORD offset = sizeof(THEME_FILE_HEADER);
    char* ptr = new char[len];
    LPTHEME_FILE_HEADER head = (LPTHEME_FILE_HEADER)ptr;
    head->cbSize = sizeof(THEME_FILE_HEADER);
    head->dwCount = dwSize;
    head->dwDataLength = dwDataLength;
    head->dwMaxSize = dwImageSize;
    head->dwReserved = 0;

    LPTHEME_DATA data = (LPTHEME_DATA)(head + 1);   // 文件头下来就是素材数组
    memcpy(ptr + offset, pData, dwSize * sizeof(THEME_DATA));   // 直接拷贝进来
    offset += (dwSize * sizeof(THEME_DATA));

    memcpy(ptr + offset, pImage, dwImageSize);      // 素材数组后面就是图片数据
    offset += dwImageSize;
    return head;
}

// 解包主题数据, 从主题包里解析各个素材的数据, 会解析素材数组和图片数据, 返回素材数组指针, 不需要手动释放, 返回的地址是第一个参数指向的地址
// 图片尺寸大小和数组成员数在 pHeader 指向的结构里
// 数据结构 = THEME_FILE_HEADER + 压缩(素材数组 + 图片数据)
// pHeader = 主题数据
// ppImage = 接收图片数据指针, 不需要手动释放
LPTHEME_DATA UnPackThemeData(LPTHEME_FILE_HEADER pHeader, void** ppImage)
{
    if (!pHeader) return 0;
    LPTHEME_DATA ptr = (LPTHEME_DATA)(pHeader + 1);

    if (ppImage)
    {
        DWORD offset = sizeof(THEME_DATA) * pHeader->dwCount;   // 偏移 = 素材结构尺寸 * 素材数量
        *ppImage = ((char*)ptr) + offset;
    }
    return (LPTHEME_DATA)ptr;
}

// 从资源文件获取资源句柄
HRESOURCE GetResourceHandle(const void* data)
{
    const void* hResource = data;
    auto it = s_map_uncompress.find(hResource);         // 从map里查找有没有解析过这个指针
    bool isFind = it != s_map_uncompress.end();         // 是否查找成功
    if (!isFind)    //  找到, 表示有这个地址, 直接返回就行
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
    if (hHead->head != MAKELONG('R', 'S')) return 0;    // 先检查传递进来的数据是否合法

    LPTHEMERESOURCE_FILE_HEADER hHeader_File;           // 资源文件的结构
    auto it = s_map_uncompress.find(hResource);         // 从map里查找有没有解析过这个指针
    bool isNewData = it == s_map_uncompress.end();      // 是否是新申请的内存
    std::vector<LPTHEMERESOURCE>* pArr;
    if (isNewData)
    {
        const void* pUncompress = (const void*)(hHead + 1);
        Czlib zlib;
        uLongf uUncompressSize;
        Bytef* pUncompressData = zlib.uncompress(pUncompress, &uUncompressSize);
        // 解压失败或者解压出来的长度不是原来的长度, 就返回0
        if (uUncompressSize == 0 || !pUncompressData || uUncompressSize != hHead->dwDataLength) return 0;

        // 新的长度是 文件头的长度 + 原来未压缩前的长度
        DWORD size = sizeof(THEMERESOURCE_FILE_HEADER);
        size += hHead->dwDataLength;
        char* ptr = new char[size];
        _append_free_memory(ptr, FREE_CHAR);
        memset(ptr, 0, size);
        hHeader_File = (LPTHEMERESOURCE_FILE_HEADER)ptr;
        memcpy(hHeader_File, hHead, hHead->cbSize);     // 前面一个文件头 + 后面解压后的数据
        memcpy(ptr + hHead->cbSize, pUncompressData, uUncompressSize);

        THEME_ARRAY& arrData = s_map_uncompress[hResource];
        arrData.arr = new std::vector<LPTHEMERESOURCE>;
        _append_free_memory(arrData.arr, FREE_ARR);
        arrData.hHeader = hHeader_File;     // 把解压后的数据地址记录起来, 下次传递相同的资源指针就使用这个地址
        pArr = arrData.arr;

        LPTHEMERESOURCE pResource = (LPTHEMERESOURCE)(hHeader_File + 1);
        for (DWORD i = 0; i < hHeader_File->dwCount; i++)   // 逐个循环查找
        {
            pArr->push_back(pResource);
            char* pTmp = (char*)(pResource + 1);    // 指向主题数据, 如果没找到, 那就从这个地址往后加数据长度
            char* pNext = pTmp + pResource->dwDataLength;   // 数据地址 + 数据长度 = 下一个资源地址
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

// 查找指定类型, 指定ID的资源, 返回找到的资源数据句柄, 可以调用 LoadResource 来获取资源数据, SizeofResource() 获取数据大小
// hResource = 资源的指针, 内部会记录此指针, 多次加载只会解压一次
// id = 资源的id, 相同资源类型不允许相同id
// lpType = 资源类型, 比如 "BIN", "PNG", "TXT"....等等, 最大支持20个字节, 资源类型区分大小写
HFINDRES FindResource(const void* hResource, int id, LPCSTR lpType)
{
    if (!hResource || !lpType || !lpType[0]) return 0;
    std::vector<LPTHEMERESOURCE>* pArr;
    LPTHEMERESOURCE_FILE_HEADER hHeader_File = _theme_parse(hResource, &pArr);

    if (!hHeader_File || !pArr)return 0;
    size_t len = strlen(lpType); if (len > 20)len = 20;

    for (auto it = pArr->begin(); it != pArr->end(); ++it)   // 逐个循环查找
    {
        LPTHEMERESOURCE pResource = *it;
        if (pResource && strncmp(pResource->type, lpType, len) == 0 && pResource->id == id)
            return (HFINDRES)pResource;
    }

    return 0;
}



// 获取指向内存中指定资源的第一个字节的指针, 返回的指针不可释放不可修改
// hResInfo = 要加载的资源的句柄, 该句柄由FindResource函数返回
const void* LoadResource(HFINDRES hResInfo)
{
    if (!hResInfo) return 0;
    LPTHEMERESOURCE pResource = (LPTHEMERESOURCE)hResInfo;
    if (pResource->cbSize != sizeof(THEMERESOURCE)) return 0;
    pResource++;    // 这个结构的后面紧跟着数据, 直接++就是指向了数据
    return pResource;
}

// 检索指定资源的大小 (以字节为单位), 函数成功, 则返回值是资源中的字节数
// hResInfo = 资源的句柄, 必须使用FindResource函数创建此句柄
DWORD SizeofResource(HFINDRES hResInfo)
{
    if (!hResInfo) return 0;
    LPTHEMERESOURCE pResource = (LPTHEMERESOURCE)hResInfo;
    if (pResource->cbSize != sizeof(THEMERESOURCE)) return 0;
    return pResource->dwDataLength;
}


// 添加数据到资源, 只会添加到内存中, 并不会对文件进行修改, 需要添加后的数据请调用 PackResource()
// hResource = 资源的指针, 内部会记录此指针, 多次加载只会解压一次
// id = 资源的id, 相同资源类型不允许相同id
// lpType = 资源类型, 比如 "BIN", "PNG", "TXT"....等等, 最大支持20个字节, 资源类型区分大小写
// pData = 要添加的数据
// dwDataSize = 要添加的数据大小
size_t AppendResource(const void* hResource, int id, LPCSTR lpType, const void* pData, DWORD dwDataSize)
{
    if (!hResource || !lpType || !lpType[0] || !pData || !dwDataSize) return 0;

    std::vector<LPTHEMERESOURCE>* pArr;
    LPTHEMERESOURCE_FILE_HEADER hHeader_File = _theme_parse(hResource, &pArr);
    if (!hHeader_File || !pArr)return 0;

    DWORD size = sizeof(THEMERESOURCE) + dwDataSize;    // 新申请的内存数据是主题结构大小 + 数据大小
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