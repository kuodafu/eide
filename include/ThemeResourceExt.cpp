#include "ThemeResourceExt.h"
#include <zlib/Czlib.h>
#include <vector>
#include <xml2/tinyxml2.h>
#include <control/ReadColor.h>


THEME_BEGIN_NAMESPACE


// �����ڴ��Ƿ��㹻д��, �������д��, ������һ���µ��ڴ�
inline void _ext_serialize_xml_calcsize(_str& pData, int& bufSize, int offset, int writeSize)
{
    if (offset + writeSize <= bufSize)return;
    int newSize = (writeSize + bufSize) * 2;
    pData.resize(newSize);
    bufSize = newSize;
}
// ���ı�д�뵽��ַ��, �ı��ı���ṹ�� �ı����� + ������ʶ, ���ٻ�д�������ֽ�
// pData = Ҫд��ĵ�ַ
// bufSize = ��������С, ��С����ʱ��Ҫ���������ڴ�
// offset = ��ַƫ��
// pstr = Ҫд����ı�
inline void _ext_serialize_xml_write_string(_str& pData, int& bufSize, int& offset, LPCWSTR pstr)
{
    if (pstr && pstr[0])
    {
        int len = (int)wcslen(pstr) + 1;
        len *= sizeof(wchar_t);

        _ext_serialize_xml_calcsize(pData, bufSize, offset, len);
        memcpy(pData.data() + offset, pstr, len);
        offset += len;
        return;
    }
    const int len = 2;
    _ext_serialize_xml_calcsize(pData, bufSize, offset, len);
    offset += len;
}

// �Ѿ���д�뵽��ַ��, ���д��ľ���(pszRect)��Ϊ��, �򷵻�true
// pData = Ҫд��ĵ�ַ
// bufSize = ��������С, ��С����ʱ��Ҫ���������ڴ�
// offset = ��ַƫ��
// pszRect = �ı���ʽ��rect����
inline bool _ext_serialize_xml_write_rect(_str& pData, int& bufSize, int& offset, LPCWSTR pszRect)
{
    bool isOk = false;
    _ext_serialize_xml_calcsize(pData, bufSize, offset, sizeof(RECT));
    LONG* ptr = (LONG*)(pData.data() + offset);
    if (pszRect && pszRect[0])
    {
        swscanf_s(pszRect, L"%d,%d,%d,%d", &ptr[0], &ptr[1], &ptr[2], &ptr[3]);
        if (ptr[2] - ptr[0] > 0 && ptr[3] - ptr[1] > 0) // �ұߴ������, �ױߴ��ڶ���
            isOk = true;
    }
    else
    {
        memset(ptr, 0, sizeof(RECT));
    }
    offset += sizeof(RECT);
    return isOk;
}

// ����ֵд�뵽��ַ��, ����д���ֵ
// pData = Ҫд��ĵ�ַ
// bufSize = ��������С, ��С����ʱ��Ҫ���������ڴ�
// offset = ��ַƫ��
// value = Ҫд�����ֵ, ռ��4���ֽ�
inline int _ext_serialize_xml_write_int(_str& pData, int& bufSize, int& offset, int value)
{
    _ext_serialize_xml_calcsize(pData, bufSize, offset, sizeof(int));
    LONG* ptr = (LONG*)(pData.data() + offset);
    memcpy(ptr, &value, sizeof(int));
    
    offset += sizeof(int);
    return value;
}

// ��ָ�����ȵ�����д�뵽��ַ��
// pData = Ҫд��ĵ�ַ
// bufSize = ��������С, ��С����ʱ��Ҫ���������ڴ�
// offset = ��ַƫ��
// value = Ҫд������ݵ�ַ
// size = Ҫд������ݴ�С
inline void _ext_serialize_xml_write(_str& pData, int& bufSize, int& offset, const void* value, int size)
{
    _ext_serialize_xml_calcsize(pData, bufSize, offset, size);
    memcpy(pData.data() + offset, value, size);
    offset += size;
}

// ���л�ext��Ҫ��xml����
// xmlText = xml����, �ڲ���������ݱ��浽serializeData������
// serializeData = ���ս����õĽ��
// theme = ���ս�������Ļ�����Ϣ, ���Ҳ��Ƥ���ļ�ͷ
inline EXT_ERR _ext_serialize_xml(const wchar_t* xmlText, _str& serializeData, THEME_FILE_HEADER_1_2& theme)
{
    CTinyXml2W xml;
    if (!xml.Parse(xmlText))
    {
        // ��������, ����xmlʧ��
        //MessageBoxW(g_hWnd, L"��������, ����xmlʧ��", L"����", 0);
        return EXT_ERR::EXT_ERR_PARSEXML;
    }

    SIZE maxSize = { 0 };
    SIZE imgSize = { 0 };
    LPCWSTR lpszMaxSize = xml.GetAttrValue(L"@1", L"maxSize");
    LPCWSTR lpszImgSize = xml.GetAttrValue(L"@1", L"imgSize");
    if (lpszMaxSize && lpszMaxSize[0]) swscanf_s(lpszMaxSize, L"%d,%d", &maxSize.cx, &maxSize.cy);
    if (lpszImgSize && lpszImgSize[0]) swscanf_s(lpszImgSize, L"%d,%d", &imgSize.cx, &imgSize.cy);
    if (maxSize.cx == 0)maxSize.cx = 16;
    if (maxSize.cy == 0)maxSize.cy = 16;

    theme.cbSize        = sizeof(theme);
    theme.dwMaxSize     = MAKELONG(maxSize.cx, maxSize.cy);
    theme.dwImageSize   = MAKELONG(imgSize.cx, imgSize.cy);
    theme.dwReserved    = 0;
    theme.dwCount       = (int)xml.GetChildNodeCount(L"@1");
    theme.dwDataLength  = 0;
    

    // ÿ����Ա��¼һ��RECT��һ���ı���id��һ��˵��, �ı���20�ַ���, ˵����40�ַ���
    int bufSize = theme.dwCount * (sizeof(RECT) + 120);
    serializeData.resize(bufSize);
    int offset = 0;

    tinyxml2::XMLNode* node = 0;
    int count = 0;
    for (DWORD i = 0; i < theme.dwCount; i++)
    {
        wchar_t nodeName[50];
        swprintf_s(nodeName, 50, L"@1/@%d", i + 1);
        xml.GetNode(nodeName, node);
        assert(node);
        LPCWSTR id = xml.GetAttrValue(node, L"id");
        if (!id || !id[0])
        {
            //MessageBoxW(g_hWnd, L"��������idΪ��, ������ID�ٱ���", L"����", 0);
            return EXT_ERR::EXT_ERR_NULLID;
        }
        LPCWSTR text        = xml.GetAttrValue(node, L"text");      // �滻��ʾ���ı�
        LPCWSTR remarks     = xml.GetNodeValue(node);               // ��ע

        int oldOffset   = offset;
        const int crText[4] =
        {
            (int)str2argb(xml.GetAttrValue(node, L"crText1"), 0),  // �����ı���ɫ
            (int)str2argb(xml.GetAttrValue(node, L"crText2"), 0),  // �ȵ��ı���ɫ
            (int)str2argb(xml.GetAttrValue(node, L"crText3"), 0),  // �����ı���ɫ
            (int)str2argb(xml.GetAttrValue(node, L"crText4"), 0),  // ��ֹ�ı���ɫ
        };
        const int crBack[4] =
        {
            (int)str2argb(xml.GetAttrValue(node, L"crBack1"), 0),  // ����������ɫ
            (int)str2argb(xml.GetAttrValue(node, L"crBack2"), 0),  // �ȵ㱳����ɫ
            (int)str2argb(xml.GetAttrValue(node, L"crBack3"), 0),  // ���±�����ɫ
            (int)str2argb(xml.GetAttrValue(node, L"crBack4"), 0),  // ��ֹ������ɫ
        };
        const LPCWSTR szRect[] =
        {
            xml.GetAttrValue(node, L"szRect1"),     // ��������
            xml.GetAttrValue(node, L"szRect2"),     // �ȵ����
            xml.GetAttrValue(node, L"szRect3"),     // ���¾���
            xml.GetAttrValue(node, L"szRect4"),     // ��ֹ����

            xml.GetAttrValue(node, L"szRect1_125"),     // ��������
            xml.GetAttrValue(node, L"szRect2_125"),     // �ȵ����
            xml.GetAttrValue(node, L"szRect3_125"),     // ���¾���
            xml.GetAttrValue(node, L"szRect4_125"),     // ��ֹ����

            xml.GetAttrValue(node, L"szRect1_150"),     // ��������
            xml.GetAttrValue(node, L"szRect2_150"),     // �ȵ����
            xml.GetAttrValue(node, L"szRect3_150"),     // ���¾���
            xml.GetAttrValue(node, L"szRect4_150"),     // ��ֹ����

            xml.GetAttrValue(node, L"szRect1_175"),     // ��������
            xml.GetAttrValue(node, L"szRect2_175"),     // �ȵ����
            xml.GetAttrValue(node, L"szRect3_175"),     // ���¾���
            xml.GetAttrValue(node, L"szRect4_175"),     // ��ֹ����

            xml.GetAttrValue(node, L"szRect1_200"),     // ��������
            xml.GetAttrValue(node, L"szRect2_200"),     // �ȵ����
            xml.GetAttrValue(node, L"szRect3_200"),     // ���¾���
            xml.GetAttrValue(node, L"szRect4_200"),     // ��ֹ����
        };
        bool isWrite = crText[0] != 0 || crText[1] != 0 || crText[2] != 0 || crText[3] != 0;        // �ı���ɫ��һ����Ϊ�ն�д������
        isWrite = crBack[0] != 0 || crBack[1] != 0 || crBack[2] != 0 || crBack[3] != 0 || isWrite;  // ������ɫ��һ����Ϊ�ն�д������

        _ext_serialize_xml_write_string(serializeData, bufSize, offset, id);        // д��ID
        _ext_serialize_xml_write_string(serializeData, bufSize, offset, text);      // д���޸ĵ��ı�
        _ext_serialize_xml_write_string(serializeData, bufSize, offset, remarks);   // д�뱸ע

        _ext_serialize_xml_write_int(serializeData, bufSize, offset, crText[0]);    // д�������ı���ɫ
        _ext_serialize_xml_write_int(serializeData, bufSize, offset, crText[1]);    // д���ȵ��ı���ɫ
        _ext_serialize_xml_write_int(serializeData, bufSize, offset, crText[2]);    // д�밴���ı���ɫ
        _ext_serialize_xml_write_int(serializeData, bufSize, offset, crText[3]);    // д���ֹ�ı���ɫ

        _ext_serialize_xml_write_int(serializeData, bufSize, offset, crBack[0]);    // д������������ɫ
        _ext_serialize_xml_write_int(serializeData, bufSize, offset, crBack[1]);    // д���ȵ㱳����ɫ
        _ext_serialize_xml_write_int(serializeData, bufSize, offset, crBack[2]);    // д�밴�±�����ɫ
        _ext_serialize_xml_write_int(serializeData, bufSize, offset, crBack[3]);    // д���ֹ������ɫ

        _ext_serialize_xml_write_int(serializeData, bufSize, offset, 0);    // ����1
        _ext_serialize_xml_write_int(serializeData, bufSize, offset, 0);    // ����2
        _ext_serialize_xml_write_int(serializeData, bufSize, offset, 0);    // ����3
        _ext_serialize_xml_write_int(serializeData, bufSize, offset, 0);    // ����4
        _ext_serialize_xml_write_int(serializeData, bufSize, offset, 0);    // ����5

        for (int i = 0; i < sizeof(szRect) / sizeof(szRect[0]); i++)
            isWrite = _ext_serialize_xml_write_rect(serializeData, bufSize, offset, szRect[i]) || isWrite;  // ֻҪ��һ��д��, ����Ҫд������

        count++;    // ����+1
        // ֻҪû��д���κξ���, ���ع���д��ID֮ǰ, �´μ��������λ�ÿ�ʼд��
        if (!isWrite)
        {
            count--;
            const int fillSize = offset - oldOffset;
            memset(serializeData.data() + oldOffset, 0, fillSize);  // �ع���0
            offset = oldOffset;
        }

    }

    theme.dwCount = count;
    serializeData.resize(offset);
    return EXT_ERR::EXT_ERR_OK;
}



// ���ext�ļ�����, ���ش�����, ʵ�������� ExtData ����, pExtDataΪ0�����ʧ��
// pngData = ͼƬ����ָ��
// pngDataSize = ͼƬ���ݴ�С
// xmlText = xml�ļ�����
// pExtData = ���մ���õ�ext�ļ����� �ṹ = NEW_THEME_FILE_HEADER + ѹ��(ͼƬ���ݳߴ� + ͼƬ���� + xml���л����ݳߴ� + xml���л�����)
EXT_ERR PackExtData(const void* pngData, DWORD pngDataSize, const wchar_t* xmlText, _str* pExtData)
{
    if (!pExtData)return EXT_ERR::EXT_ERR_NULLRET;
    _str xmlData;
    THEME_FILE_HEADER_1_2 theme = { 0 };    // �������ʼ��ʹ�����°�Ľṹ, ������ݸ��ݴ��ݵ������жϰ汾
    EXT_ERR extErr = _ext_serialize_xml(xmlText, xmlData, theme);
    if (extErr != EXT_ERR::EXT_ERR_OK)      // ��xmlת���ڴ�����
        return extErr;

    theme.majorVersion = 1;
    theme.minorVersion = 2;
    theme.buildVersion = 810;
    DWORD xmlTextSize = (DWORD)xmlData.size();          // xml���ݵĳߴ�
    size_t dataSize = pngDataSize + xmlTextSize + sizeof(DWORD) * 2;

    pExtData->resize(dataSize + sizeof(theme));        // ͼƬ���ݳ��� + ͼƬ���� + �ı����� + �ı����� + �ļ�ͷ�ߴ�
    LPSTR ptr = pExtData->data();
    // ��ѹ�������� = ͼƬ�ߴ� + ͼƬ���� + xml���ݳߴ� + xml����
    memcpy(ptr, &pngDataSize, sizeof(DWORD));   ptr += sizeof(DWORD);   // д��ͼƬ�ߴ�
    memcpy(ptr, pngData, pngDataSize);          ptr += pngDataSize;     // д��ͼƬ����
    memcpy(ptr, &xmlTextSize, sizeof(DWORD));   ptr += sizeof(DWORD);   // д��xml���ݴ�С
    memcpy(ptr, xmlData.data(), xmlTextSize);   ptr += xmlTextSize;     // д��xml����

    Czlib zlib;
    uLongf compressSize = 0;
    Byte* pThemeData = zlib.compress(pExtData->c_str(), dataSize, &compressSize);    // ѹ���õ�����
    if (compressSize > dataSize)    // ���ѹ����ĳߴ��ԭ�ߴ绹��, �Ǿ͵����µ����ߴ�
        pExtData->resize(compressSize + sizeof(theme));
    ptr = pExtData->data();
    theme.dwDataLength = compressSize;

    DWORD offset = 0;
    memcpy(ptr + offset, &theme, sizeof(theme));    offset += sizeof(theme);
    memcpy(ptr + offset, pThemeData, compressSize); offset += compressSize;
    pExtData->resize(offset);
    zlib.free(&pThemeData);
    return EXT_ERR::EXT_ERR_OK;
}


// ���ext�ļ�����, ���ش�����
// pExtData = ext�ļ�����
// pngData = ����pngͼƬ����
// xmlData = ����xml���л��������
EXT_ERR UnExtData(LPTHEME_FILE_HEADER pExtData, _str* pngData, _str* xmlData)
{
    if (!pngData && !xmlData)return EXT_ERR::EXT_ERR_NULLRET;
    // �ṹ = THEME_FILE_HEADER + ѹ��(ͼƬ���ݳߴ� + ͼƬ���� + xml���л����ݳߴ� + xml���л�����)
    if (!pExtData) return EXT_ERR::EXT_ERR_NULLEXTDATA;
    void* pCompressData = 0;
    if (pExtData->cbSize == sizeof(THEME_FILE_HEADER))  // 1.0 �汾
    {
        if (pExtData->dwReserved != 0 || pExtData->dwCount == 0
            || pExtData->dwDataLength == 0 || pExtData->dwMaxSize == 0)
            return EXT_ERR::EXT_ERR_NULLEXTDATA;
        pCompressData = (void*)(pExtData + 1);
    }
    else if (pExtData->cbSize == sizeof(THEME_FILE_HEADER_1_1))  // 1.1�汾
    {
        THEME_FILE_HEADER_1_1* data = (THEME_FILE_HEADER_1_1*)pExtData;

        if (data->dwReserved != 0 || data->dwCount == 0
            || data->dwDataLength == 0 || data->dwMaxSize == 0 || data->dwImageSize == 0)
            return EXT_ERR::EXT_ERR_NULLEXTDATA;
        pCompressData = (void*)(data + 1);  // 1.1��ƫ������1.0��Ҫ��, ƫ��������Ĵ���ʽ��һ��
    }
    else if (pExtData->cbSize == sizeof(THEME_FILE_HEADER_1_2))  // 1.2�汾
    {
        THEME_FILE_HEADER_1_2* data = (THEME_FILE_HEADER_1_2*)pExtData;

        if (data->dwReserved != 0 || data->dwCount == 0
            || data->dwDataLength == 0 || data->dwMaxSize == 0 || data->dwImageSize == 0)
            return EXT_ERR::EXT_ERR_NULLEXTDATA;
        pCompressData = (void*)(data + 1);  // 1.2��ƫ������1.1��Ҫ��, ƫ��������Ĵ���ʽ��һ��
    }
    else
    {
        return EXT_ERR::EXT_ERR_NULLEXTDATA;    // �����汾��ʱ������
    }
    Czlib zlib;
    Bytef* p = zlib.uncompress(pCompressData, 0);
    if (!p) return EXT_ERR::EXT_ERR_UNCOMPRESS;   // ��ѹʧ�ܾͷ���

    LPBYTE ptr = p;
    DWORD pngSize = (*((DWORD*)ptr));   // ǰ4���ֽڵ�ͼƬ�ߴ�
    ptr += sizeof(DWORD);   // ָ������ƫ��, ָ��ͼƬ����
    if (pngData)pngData->assign((LPCSTR)ptr, pngSize);  // ��ͼƬ���ݿ�����������

    ptr += pngSize;     // ƫ�Ƽ���ͼƬ�ߴ�, �ߵ���ptrָ��xml���ݳ���
    DWORD textSize = (*((DWORD*)ptr));  // xml���ݳ���
    ptr += sizeof(DWORD);
    if (xmlData)xmlData->assign((LPCSTR)ptr, textSize); // ��xml���ݿ�����������
    zlib.free(&p);
    return EXT_ERR::EXT_ERR_OK;

}

LPCWSTR GetErrTextW(EXT_ERR err)
{
    switch (err)
    {
    case THEME_NAMESPACE EXT_ERR::EXT_ERR_OK:           return L"";
    case THEME_NAMESPACE EXT_ERR::EXT_ERR_PARSEXML:     return L"���к�xmlʱ����xmlʧ��";
    case THEME_NAMESPACE EXT_ERR::EXT_ERR_NULLID:       return L"���к�xmlʱ��⵽�п�ID";
    case THEME_NAMESPACE EXT_ERR::EXT_ERR_NULLRET:      return L"û�н��շ���ֵ";
    case THEME_NAMESPACE EXT_ERR::EXT_ERR_NULLEXTDATA:  return L"���ʱ�����˲��Ϸ���ext����";
    case THEME_NAMESPACE EXT_ERR::EXT_ERR_UNCOMPRESS:   return L"���ʱ��ѹ����ʧ��";
    default: return L"";
    }
}
LPCSTR GetErrTextA(EXT_ERR err)
{
    switch (err)
    {
    case THEME_NAMESPACE EXT_ERR::EXT_ERR_OK:           return "";
    case THEME_NAMESPACE EXT_ERR::EXT_ERR_PARSEXML:     return "���к�xmlʱ����xmlʧ��";
    case THEME_NAMESPACE EXT_ERR::EXT_ERR_NULLID:       return "���к�xmlʱ��⵽�п�ID";
    case THEME_NAMESPACE EXT_ERR::EXT_ERR_NULLRET:      return "û�н��շ���ֵ";
    case THEME_NAMESPACE EXT_ERR::EXT_ERR_NULLEXTDATA:  return "���ʱ�����˲��Ϸ���ext����";
    case THEME_NAMESPACE EXT_ERR::EXT_ERR_UNCOMPRESS:   return "���ʱ��ѹ����ʧ��";
    default: return "";
    }
}

THEME_END_NAMESPACE