#pragma once
#include <tstr.h>
#include <vector>

// 根据需要开启或者关闭命名空间
#if 1
#define __THEME_TMPNAME Theme
#define THEME_NAMESPACE __THEME_TMPNAME :: 
#define THEME_BEGIN_NAMESPACE    namespace __THEME_TMPNAME{
#define THEME_END_NAMESPACE      }
#else
#define THEME_NAMESPACE
#define THEME_BEGIN_NAMESPACE 
#define THEME_END_NAMESPACE
#endif

THEME_BEGIN_NAMESPACE


DECLARE_HANDLE(HFINDRES);
DECLARE_HANDLE(HRESOURCE);

// 主题文件结构 = HEMERESOURCE结构 + 数据 + THEMERESOURCE结构 + 数据
// 数据 = THEME_DATA结构数组 + 图片


// 主题文件头, 记录整个文件的主题数量
typedef struct THEMERESOURCE_FILE_HEADER
{
    WORD  head;             // 文件头标识, MAKELONG('R', 'S')
    WORD  cbSize;           // 结构的尺寸, 版本升级需要根据这个结构尺寸进行判断
    DWORD dwCount;          // 成员数, 一共有多少个主题包
    DWORD dwDataLength;     // 整个文件除了本结构的长度, 压缩前的尺寸
    DWORD dwCompressSize;   // 整个文件除了本结构的长度, 压缩后的数据尺寸
    DWORD dwReserved;       // 备用
}*PTHEMERESOURCE_FILE_HEADER, *LPTHEMERESOURCE_FILE_HEADER;


// 
typedef struct tagTHEMERESOURCE
{
    DWORD cbSize;           // 结构的尺寸, 版本升级需要根据这个结构尺寸进行判断
    DWORD dwDataLength;     // 资源数据的长度, 这个结构后面跟着的数据长度
    DWORD id;               // 资源ID
    char  type[20];         // 资源类型, 存放的是ansi, 最多支持20个字节
    DWORD dwReserved1;      // 备用1
    DWORD dwReserved2;      // 备用2
}THEMERESOURCE, *PTHEMERESOURCE, *LPTHEMERESOURCE;

// 主题数据的文件头
typedef struct THEME_FILE_HEADER
{
    DWORD cbSize;           // 结构尺寸
    DWORD dwCount;          // 成员数, 一个主题是由多个小图标组合成一张大图片, 这里记录有多少个小图标
    DWORD dwDataLength;     // 数据长度, 这个结构后面跟随的长度
    DWORD dwMaxSize;        // 单个图片尺寸, 低位保存宽度, 高位保存高度
    DWORD dwReserved;       // 备用
}*PTHEME_FILE_HEADER, * LPTHEME_FILE_HEADER;

// 主题数据的文件头
typedef struct THEME_FILE_HEADER_1_1
{
    DWORD cbSize;           // 结构尺寸
    DWORD dwCount;          // 成员数, 一个主题是由多个小图标组合成一张大图片, 这里记录有多少个小图标
    DWORD dwDataLength;     // 数据长度, 这个结构后面跟随的长度
    DWORD dwMaxSize;        // 单个图片尺寸, 低位保存宽度, 高位保存高度
    DWORD dwReserved;       // 备用
    DWORD dwImageSize;      // 大图片尺寸, 低位保存宽度, 高位保存高度
}*PTHEME_FILE_HEADER_1_1, * LPTHEME_FILE_HEADER_1_1;

// 主题数据的文件头
typedef struct THEME_FILE_HEADER_1_2
{
    DWORD cbSize;           // 结构尺寸
    DWORD dwCount;          // 成员数, 一个主题是由多个小图标组合成一张大图片, 这里记录有多少个小图标
    DWORD dwDataLength;     // 数据长度, 这个结构后面跟随的长度
    DWORD dwMaxSize;        // 单个图片尺寸, 低位保存宽度, 高位保存高度
    DWORD dwReserved;       // 备用
    DWORD dwImageSize;      // 大图片尺寸, 低位保存宽度, 高位保存高度
    BYTE  majorVersion;     // 主版本号
    BYTE  minorVersion;     // 次版本号
    WORD  buildVersion;     // 编译版本号
    
}*PTHEME_FILE_HEADER_1_2, * LPTHEME_FILE_HEADER_1_2;

// 主题的数据
typedef struct tagTHEME_DATA
{
    DWORD cbSize;           // 结构的尺寸, 版本升级需要根据这个结构尺寸进行判断
    RECT rc;                // 图标在图片里的位置, 左顶右底
    char  text1[30];        // 存放字符串, 最多支持30个字节, 某些主题可能会使用
    char  text2[30];        // 存放字符串, 最多支持30个字节, 某些主题可能会使用
    char  text3[30];        // 存放字符串, 最多支持30个字节, 某些主题可能会使用
    DWORD dwReserved1;      // 备用1
    DWORD dwReserved2;      // 备用2
}THEME_DATA, *PTHEME_DATA, *LPTHEME_DATA;










inline void Theme_free(void* p)
{
    char* ptr = (char*)p;
    if (ptr) delete[] ptr;
}






//
//// 把所有主题文件打包成一个资源文件
//// pThemes = 主题数据 = THEME_FILE_HEADER + 压缩(素材数组 + 图片数据)
//// dwThemeCount = 主题数量
//inline LPTHEMERESOURCE_FILE_HEADER PackResourceFile(LPTHEME_FILE_HEADER pThemes, DWORD dwThemeCount)
//{
//    // 1. 先计算出所有主题需要的尺寸
//    // 最终的数据大小 = 资源文件文件头大小 + 所有主题的大小
//    DWORD dwSize = sizeof(THEMERESOURCE_FILE_HEADER);
//    DWORD dwThemeSize = 0;  // 这个是主题文件总尺寸, 所有主题文件头+主题数据大小
//    DWORD i;
//    for (i = 0; i < dwThemeCount; i++)
//    {
//        dwThemeSize += sizeof(THEME_FILE_HEADER);    // 主题文件是一个文件头 + 压缩的一段数据
//        dwThemeSize += pThemes[i].dwCompressSize;    // 计算大小 = 所有的主题文件头+压缩数据的长度
//    }
//
//    dwSize += dwThemeSize;  // 资源文件文件头大小 + 所有主题大小
//    dwSize += (dwThemeCount * sizeof(THEMERESOURCE));   // 在加上主题保存需要的结构
//
//
//    char* ptr = new char[dwSize];
//    memset(ptr, 0, dwSize);
//    LPTHEMERESOURCE_FILE_HEADER pHead = (LPTHEMERESOURCE_FILE_HEADER)ptr;
//    pHead->cbSize = sizeof(THEMERESOURCE_FILE_HEADER);
//    pHead->dwCount = dwThemeCount;
//    pHead->dwDataLength = dwSize - sizeof(THEMERESOURCE_FILE_HEADER);
//    pHead->dwCompressSize = ;
//    pHead->dwReserved = 0;
//
//    LPTHEMERESOURCE pResource = (LPTHEMERESOURCE)(pHead + 1);
//    for (i = 0; i < dwThemeCount; i++)
//    {
//        pResource->cbSize = sizeof(THEMERESOURCE);
//        pResource->dwDataLength
//    }
//
//}
//




// 把所有主题文件打包成一个资源文件
// pResource = 资源数据,  THEMERESOURCE + PackThemeData()
// dwResourceCount = 主题数量
inline LPTHEMERESOURCE_FILE_HEADER PackResourceFile(LPTHEMERESOURCE pResource, DWORD dwResourceCount)
{

}




// 打包主题数据, 把N个主题素材打包成一个主题
// 数据结构 = THEME_FILE_HEADER + 压缩(素材数组 + 图片数据)
// pData = 素材结构数组
// dwSize = 素材数量
// pImage = 图片数据
// dwImageSize = 图片数据大小
LPTHEME_FILE_HEADER PackThemeData(LPTHEME_DATA pData, DWORD dwSize, const void* pImage, DWORD dwImageSize);

// 解包主题数据, 从主题包里解析各个素材的数据, 会解析素材数组和图片数据, 返回素材数组指针, 需要使用 Theme_free() 释放
// 图片尺寸大小和数组成员数在 pHeader 指向的结构里
// 数据结构 = THEME_FILE_HEADER + 压缩(素材数组 + 图片数据)
// pHeader = 主题数据
// ppImage = 接收图片数据指针, 不需要手动释放
LPTHEME_DATA UnPackThemeData(LPTHEME_FILE_HEADER pHeader, void** ppImage);

THEME_END_NAMESPACE