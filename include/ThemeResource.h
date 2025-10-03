#pragma once
#include <tstr.h>
#include <vector>

// ������Ҫ�������߹ر������ռ�
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

// �����ļ��ṹ = HEMERESOURCE�ṹ + ���� + THEMERESOURCE�ṹ + ����
// ���� = THEME_DATA�ṹ���� + ͼƬ


// �����ļ�ͷ, ��¼�����ļ�����������
typedef struct THEMERESOURCE_FILE_HEADER
{
    WORD  head;             // �ļ�ͷ��ʶ, MAKELONG('R', 'S')
    WORD  cbSize;           // �ṹ�ĳߴ�, �汾������Ҫ��������ṹ�ߴ�����ж�
    DWORD dwCount;          // ��Ա��, һ���ж��ٸ������
    DWORD dwDataLength;     // �����ļ����˱��ṹ�ĳ���, ѹ��ǰ�ĳߴ�
    DWORD dwCompressSize;   // �����ļ����˱��ṹ�ĳ���, ѹ��������ݳߴ�
    DWORD dwReserved;       // ����
}*PTHEMERESOURCE_FILE_HEADER, *LPTHEMERESOURCE_FILE_HEADER;


// 
typedef struct tagTHEMERESOURCE
{
    DWORD cbSize;           // �ṹ�ĳߴ�, �汾������Ҫ��������ṹ�ߴ�����ж�
    DWORD dwDataLength;     // ��Դ���ݵĳ���, ����ṹ������ŵ����ݳ���
    DWORD id;               // ��ԴID
    char  type[20];         // ��Դ����, ��ŵ���ansi, ���֧��20���ֽ�
    DWORD dwReserved1;      // ����1
    DWORD dwReserved2;      // ����2
}THEMERESOURCE, *PTHEMERESOURCE, *LPTHEMERESOURCE;

// �������ݵ��ļ�ͷ
typedef struct THEME_FILE_HEADER
{
    DWORD cbSize;           // �ṹ�ߴ�
    DWORD dwCount;          // ��Ա��, һ���������ɶ��Сͼ����ϳ�һ�Ŵ�ͼƬ, �����¼�ж��ٸ�Сͼ��
    DWORD dwDataLength;     // ���ݳ���, ����ṹ�������ĳ���
    DWORD dwMaxSize;        // ����ͼƬ�ߴ�, ��λ������, ��λ����߶�
    DWORD dwReserved;       // ����
}*PTHEME_FILE_HEADER, * LPTHEME_FILE_HEADER;

// �������ݵ��ļ�ͷ
typedef struct THEME_FILE_HEADER_1_1
{
    DWORD cbSize;           // �ṹ�ߴ�
    DWORD dwCount;          // ��Ա��, һ���������ɶ��Сͼ����ϳ�һ�Ŵ�ͼƬ, �����¼�ж��ٸ�Сͼ��
    DWORD dwDataLength;     // ���ݳ���, ����ṹ�������ĳ���
    DWORD dwMaxSize;        // ����ͼƬ�ߴ�, ��λ������, ��λ����߶�
    DWORD dwReserved;       // ����
    DWORD dwImageSize;      // ��ͼƬ�ߴ�, ��λ������, ��λ����߶�
}*PTHEME_FILE_HEADER_1_1, * LPTHEME_FILE_HEADER_1_1;

// �������ݵ��ļ�ͷ
typedef struct THEME_FILE_HEADER_1_2
{
    DWORD cbSize;           // �ṹ�ߴ�
    DWORD dwCount;          // ��Ա��, һ���������ɶ��Сͼ����ϳ�һ�Ŵ�ͼƬ, �����¼�ж��ٸ�Сͼ��
    DWORD dwDataLength;     // ���ݳ���, ����ṹ�������ĳ���
    DWORD dwMaxSize;        // ����ͼƬ�ߴ�, ��λ������, ��λ����߶�
    DWORD dwReserved;       // ����
    DWORD dwImageSize;      // ��ͼƬ�ߴ�, ��λ������, ��λ����߶�
    BYTE  majorVersion;     // ���汾��
    BYTE  minorVersion;     // �ΰ汾��
    WORD  buildVersion;     // ����汾��
    
}*PTHEME_FILE_HEADER_1_2, * LPTHEME_FILE_HEADER_1_2;

// ���������
typedef struct tagTHEME_DATA
{
    DWORD cbSize;           // �ṹ�ĳߴ�, �汾������Ҫ��������ṹ�ߴ�����ж�
    RECT rc;                // ͼ����ͼƬ���λ��, ���ҵ�
    char  text1[30];        // ����ַ���, ���֧��30���ֽ�, ĳЩ������ܻ�ʹ��
    char  text2[30];        // ����ַ���, ���֧��30���ֽ�, ĳЩ������ܻ�ʹ��
    char  text3[30];        // ����ַ���, ���֧��30���ֽ�, ĳЩ������ܻ�ʹ��
    DWORD dwReserved1;      // ����1
    DWORD dwReserved2;      // ����2
}THEME_DATA, *PTHEME_DATA, *LPTHEME_DATA;










inline void Theme_free(void* p)
{
    char* ptr = (char*)p;
    if (ptr) delete[] ptr;
}






//
//// �����������ļ������һ����Դ�ļ�
//// pThemes = �������� = THEME_FILE_HEADER + ѹ��(�ز����� + ͼƬ����)
//// dwThemeCount = ��������
//inline LPTHEMERESOURCE_FILE_HEADER PackResourceFile(LPTHEME_FILE_HEADER pThemes, DWORD dwThemeCount)
//{
//    // 1. �ȼ��������������Ҫ�ĳߴ�
//    // ���յ����ݴ�С = ��Դ�ļ��ļ�ͷ��С + ��������Ĵ�С
//    DWORD dwSize = sizeof(THEMERESOURCE_FILE_HEADER);
//    DWORD dwThemeSize = 0;  // ����������ļ��ܳߴ�, ���������ļ�ͷ+�������ݴ�С
//    DWORD i;
//    for (i = 0; i < dwThemeCount; i++)
//    {
//        dwThemeSize += sizeof(THEME_FILE_HEADER);    // �����ļ���һ���ļ�ͷ + ѹ����һ������
//        dwThemeSize += pThemes[i].dwCompressSize;    // �����С = ���е������ļ�ͷ+ѹ�����ݵĳ���
//    }
//
//    dwSize += dwThemeSize;  // ��Դ�ļ��ļ�ͷ��С + ���������С
//    dwSize += (dwThemeCount * sizeof(THEMERESOURCE));   // �ڼ������Ᵽ����Ҫ�Ľṹ
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




// �����������ļ������һ����Դ�ļ�
// pResource = ��Դ����,  THEMERESOURCE + PackThemeData()
// dwResourceCount = ��������
inline LPTHEMERESOURCE_FILE_HEADER PackResourceFile(LPTHEMERESOURCE pResource, DWORD dwResourceCount)
{

}




// �����������, ��N�������زĴ����һ������
// ���ݽṹ = THEME_FILE_HEADER + ѹ��(�ز����� + ͼƬ����)
// pData = �زĽṹ����
// dwSize = �ز�����
// pImage = ͼƬ����
// dwImageSize = ͼƬ���ݴ�С
LPTHEME_FILE_HEADER PackThemeData(LPTHEME_DATA pData, DWORD dwSize, const void* pImage, DWORD dwImageSize);

// �����������, �����������������زĵ�����, ������ز������ͼƬ����, �����ز�����ָ��, ��Ҫʹ�� Theme_free() �ͷ�
// ͼƬ�ߴ��С�������Ա���� pHeader ָ��Ľṹ��
// ���ݽṹ = THEME_FILE_HEADER + ѹ��(�ز����� + ͼƬ����)
// pHeader = ��������
// ppImage = ����ͼƬ����ָ��, ����Ҫ�ֶ��ͷ�
LPTHEME_DATA UnPackThemeData(LPTHEME_FILE_HEADER pHeader, void** ppImage);

THEME_END_NAMESPACE