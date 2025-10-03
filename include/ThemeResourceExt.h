#pragma once
#include "ThemeResource.h"

THEME_BEGIN_NAMESPACE

enum class EXT_ERR
{
    EXT_ERR_OK,
    EXT_ERR_PARSEXML,       // ���к�xmlʱ����xmlʧ��
    EXT_ERR_NULLID,         // ���к�xmlʱ��⵽�п�ID
    EXT_ERR_NULLRET,        // û�н��շ���ֵ
    EXT_ERR_NULLEXTDATA,    // ���ʱ�����˲��Ϸ���ext����
    EXT_ERR_UNCOMPRESS,     // ���ʱ��ѹ����ʧ��

};

// ���ext�ļ�����, ���ش�����, ʵ�������� ExtData ����, pExtDataΪ0�����ʧ��
// pngData = ͼƬ����ָ��
// pngDataSize = ͼƬ���ݴ�С
// xmlText = xml�ļ�����
// pExtData = ���մ���õ�ext�ļ����� �ṹ = NEW_THEME_FILE_HEADER + ѹ��(ͼƬ���ݳߴ� + ͼƬ���� + xml���л����ݳߴ� + xml���л�����)
EXT_ERR PackExtData(const void* pngData, DWORD pngDataSize, const wchar_t* xmlText, _str* pExtData);

// ���ext�ļ�����, ���ش�����
// pExtData = ext�ļ�����
// pngData = ����pngͼƬ����
// xmlData = ����xml���л��������
EXT_ERR UnExtData(LPTHEME_FILE_HEADER pExtData, _str* pngData, _str* xmlData);

LPCWSTR GetErrTextW(EXT_ERR err);
LPCSTR GetErrTextA(EXT_ERR err);


THEME_END_NAMESPACE