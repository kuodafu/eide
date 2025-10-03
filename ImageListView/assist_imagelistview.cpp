#include "ImageListView.h"
#include <map>

// ����λͼ��Ϣ
struct ARR_BITMAP
{

};

static std::map<LONG_PTR, Gdiplus::Bitmap*> s_map_bitmap;   // ���м��ص�ͼƬ������������
static std::vector<wchar_t*> s_hash_array;                  // �ı���ϣ��һ���ڴ�, �����ϣ�ͷ��������ַ, ��Ҫ�ͷ�


// ��·������ͼƬ, ���ؼ��غ��ͼƬָ��
// �ڲ����ͼƬ���浽 s_map_file ���map��, ���֮ǰ���ڻ��ͷ�֮ǰ�����ͼƬ
Gdiplus::Bitmap* appendBitmapToMap(const wchar_t* path)
{
    if (!path || !path[0]) return 0;
    Gdiplus::Bitmap*& pBmp = s_map_bitmap[_hash_fun(path)];
    if (pBmp)delete pBmp;
    pBmp = _img_create_from_file(path);
    return pBmp;
}

// ��ָ��·����ȡͼƬ, ���δ�����ڲ������
Gdiplus::Bitmap* GetBitmapFromMap(const wchar_t* path)
{
    if (!path || !path[0]) return 0;
    Gdiplus::Bitmap*& pBmp = s_map_bitmap[_hash_fun(path)];
    if (pBmp) return pBmp;
    pBmp = _img_create_from_file(path);
    return pBmp;
}
Gdiplus::Bitmap* QueryBitmapFromMap(const wchar_t* path)
{
    if (!path || !path[0]) return 0;
    Gdiplus::Bitmap*& pBmp = s_map_bitmap[_hash_fun(path)];
    return pBmp;
}
void DeleteBitmapFromMap(const wchar_t * path)
{
    if (!path || !path[0]) return;
    LONG_PTR hash = _hash_fun(path);
    Gdiplus::Bitmap*& pBmp = s_map_bitmap[hash];
    if (pBmp)delete pBmp;
    s_map_bitmap.erase(hash);
}
Gdiplus::Bitmap* _img_create_from_file(const wchar_t* file)
{
    FILE* f;
    errno_t err = _wfopen_s(&f, file, L"rb+");
    if (f)
    {
        fseek(f, 0, SEEK_END);
        long size = ftell(f);
        fseek(f, 0, SEEK_SET);
        _str buf(size);
        
        fread(buf.data(), 1, size, f);
        fclose(f);

        return _img_create_from_memory(buf.c_str(), size);
    }
    return 0;
}
void ModyfyTitle(bool isModify)
{
    const int len = GetWindowTextLengthW(g_hWnd) + 1;
    wstr title(len);
    LPWSTR ptr = title.data();
    GetWindowTextW(g_hWnd, ptr, len);
    if (isModify)
    {
        // ���޸�, ���ñ����һ���ַ�Ϊ�Ǻ�
        if (ptr[0] == '*')
            return;
        wstr newTitle(len + 2);
        newTitle.assign(L"* ").append(title.c_str());
        SetWindowTextW(g_hWnd, newTitle.c_str());
    }
    else
    {
        // �����޸�, ȥ������ĵ�һ���Ǻ�
        if (ptr[0] != '*')
            return;
        SetWindowTextW(g_hWnd, title.c_str() + 2);
    }
}
LONG_PTR _hash_fun(const wchar_t* pstr)
{
    if (!pstr || !pstr[0])return 0;
    for (auto i = s_hash_array.begin(); i != s_hash_array.end(); ++i)
    {
        const wchar_t* p = *i;
        if (p && wcscmp(p, pstr) == 0)
            return (LONG_PTR)p;
    }
    size_t len = wcslen(pstr) + 1;
    wchar_t* p = new wchar_t[len];
    memcpy(p, pstr, len * sizeof(wchar_t));
    s_hash_array.push_back(p);
    return (LONG_PTR)p;
}

// ɾ��ĳ����Ŀ, ���������ɾ����Ա, ��xml��ɾ����Ӧ�ڵ�, ��λͼ�������ͷ�Ŀ¼�µ��ļ�
void eraseItem(int index)
{
    auto itArr = s_data.begin() + index;
    PROJECT_DATA& data = (*itArr);
    wstr imagePath, xmlFile;
    if (!isCreatePath(data.xmlName, &imagePath, &xmlFile)) return;

    // �������Ŀ¼�Ƿ��ǺϷ�Ŀ¼
    while (1)
    {
        if (!IsDirectory(imagePath.c_str()))break;

        wstr strFind(260);
        wstr path(260);
        path = imagePath;

        if (path.back() != L'\\' && path.back() != L'/')
            path.append(L"\\");

        strFind.assign(path).append(L"*.*");

        // ���ļ����ң��鿴ԴĿ¼���Ƿ����ƥ����ļ�
        // ����FindFile�󣬱������FindNextFile���ܻ�ò����ļ�����Ϣ
        WIN32_FIND_DATA wfd;
        HANDLE hFind = FindFirstFileW(strFind.c_str(), &wfd);
        wstr strSubFolder;
        if (hFind == INVALID_HANDLE_VALUE) break;

        do
        {
            if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                if (wfd.cFileName[0] == L'.') continue;

            strSubFolder.assign(path).append(wfd.cFileName);
            LONG_PTR hash = _hash_fun(strSubFolder.c_str());
            auto it = s_map_bitmap.find(hash);
            if (it != s_map_bitmap.end())
            {
                Gdiplus::Bitmap* hBitmap = it->second;
                delete hBitmap;
                s_map_bitmap.erase(it);
            }

            DeleteFileW(strSubFolder.c_str());  // ɾ���ļ�
        } while (FindNextFileW(hFind, &wfd));

        // ɾ����Ŀ¼
        FindClose(hFind);

        break;
    }
    wstr nodeName(50);
    nodeName.assign(L"@1/@").append(index + 1);
    g_xmlProject.DelNode(nodeName.c_str());
    data.free();
    s_data.erase(itArr);
    DeleteFileW(xmlFile.c_str());
    RemoveDirectoryW(imagePath.c_str());

}


// �ӵ�ǰѡ�еĹ��̻�ȡָ��״̬��ͼƬλ��
// projectData = �ĸ���Ŀ����
// index = �ڼ�����Ա, ������0��ʼ
// state = STATE ö�ٳ������ֵ, ����ֻ�� NORMAL, HOT, DOWN, BAN ��4��״̬
RECT& xml_getimage_rect(PROJECT_DATA* projectData, int index, STATE state)
{
    XMLDATA_STRUCT& xmlData = projectData->arr->at(index);
    switch (state)
    {
    case BAN:   return xmlData.rc[3];
    case DOWN:  return xmlData.rc[2];
    case HOT:   return xmlData.rc[1];
    default:    return xmlData.rc[0];
    }
}

// �ӵ�ǰѡ�еĹ��̻�ȡָ��״̬��ͼƬ��ַ, ���û���򷵻�0
// projectData = �ĸ���Ŀ����
// index = �ڼ�����Ա, ������0��ʼ
// state = STATE ö�ٳ������ֵ, ����ֻ�� NORMAL, HOT, DOWN, BAN ��4��״̬
// dpiIndex = 0=100, 1=125, 2=150, 3=175, 4=200, �ڲ����ݴ����dpi�������ö�Ӧλ�õľ���, ͼ������ = dpiIndex * 4 + ״̬
// rcIcon = ����ͼƬ�ڴ�ͼƬ���λ��
const wchar_t* xml_getimage_filename(PROJECT_DATA* projectData, int index, STATE state, int dpiIndex, RECT* rcIcon)
{
    XMLDATA_STRUCT& xmlData = projectData->arr->at(index);
    int _index = _dpiindex2index(state, dpiIndex);
    if (rcIcon)memcpy(rcIcon, &xmlData.rc[_index], sizeof(RECT));
    return xmlData.file[_index];
}


// ���ز�·��ת���ɹ���·��
// lpszProjectPath = ����Ŀ¼, ����'\'
// lpszImageFileName = ͼƬ�ز�����·��
LPCWSTR _srcfile_to_projectfile(LPCWSTR lpszProjectPath, LPCWSTR lpszImageFileName)
{
    static wstr file(260);
    LPCWSTR pos = _file_get_name(lpszImageFileName);
    file.assign(lpszProjectPath).append(pos);
    return file.c_str();
}

// �ͷ�ָ��������ͼƬ����
// state = Ҫɾ����״̬, -1=ɾ��ȫ��, -2=ɾ����ǰ����4��״̬, ����ֻ�� NORMAL, HOT, DOWN, BAN ��4��״̬
// dpiIndex = 0=100, 1=125, 2=150, 3=175, 4=200, �ڲ����ݴ����dpi�������ö�Ӧλ�õľ���, ͼ������ = dpiIndex * 4 + ״̬
void xml_delete_image(PROJECT_DATA* projectData, int index, int state, int dpiIndex)
{
    auto it = projectData->arr->begin() + index;
    XMLDATA_STRUCT& xmlData = *it;
    if (state == -1)
    {
        xmlData.free(); // �����¼������״̬
    }
    else if (state == -2)
    {
        xmlData.SetFile(0, STATE::NORMAL , dpiIndex);
        xmlData.SetFile(0, STATE::HOT    , dpiIndex);
        xmlData.SetFile(0, STATE::DOWN   , dpiIndex);
        xmlData.SetFile(0, STATE::BAN    , dpiIndex);
    }
    else
    {
        int _index = _dpiindex2index(state, dpiIndex);
        xmlData.SetFile(0, state, dpiIndex);
        memset(&xmlData.rc[_index], 0, sizeof(RECT));
    }

    xml_setimage_filename(projectData, index, xmlData.file[0], STATE::NORMAL, dpiIndex);
    ModyfyTitle(true);
}

// ������߻滭����ͼƬ��
// hCanvas = �滭���, Ϊ0��ֻ����ߴ�, ����rc����
// file = ͼƬ�ļ�·��
// rc = �������ͼƬ�����ڴ�ͼƬ���λ��
// rcIcon = ��ǰ��ͼƬʹ�õľ���
// projectData = ��������, �������������ߴ��ͼƬ��¼������ṹ��
inline void __draw_calc_image(LPCANVAS hCanvas, LPCWSTR file, RECT& rc, RECT& rcIcon, PROJECT_DATA* projectData)
{
    LPEX_IMAGE hBitmap = _img_create_fromfile(file);
    if (!hBitmap) return;

    int width = (int)hBitmap->GetWidth();
    int height = (int)hBitmap->GetHeight();

    rc.left = rcIcon.left;
    rc.top = rcIcon.top;
    rc.right = rcIcon.left + width;
    rc.bottom = rcIcon.top + height;

    if (hCanvas == 0)
    {
        if (width > projectData->maxWidth)
            projectData->maxWidth = width;
        if (height > projectData->maxHeight)
            projectData->maxHeight = height;

        rcIcon.right += width;
        rcIcon.bottom = projectData->maxHeight; // �ױ�ʹ�����ĵױ�, ���ʹ��ͼƬ���
        _img_destroy(hBitmap);
        return;
    }


    _canvas_drawimagerect(hCanvas, hBitmap, (float)rc.left, (float)rc.top, (float)rc.right, (float)rc.bottom, 255);

    rcIcon.left += width;
    rcIcon.top = 0;
    rcIcon.right = rcIcon.left + width;
    rcIcon.bottom = projectData->maxHeight; // �ױ�ʹ�����ĵױ�, ���ʹ��ͼƬ���

    _img_destroy(hBitmap);
}



// ��xml�������ͼƬ��Ū��һ�Ŵ�ͼƬ��, �����浽 saveFile ���·��
inline void __make_image_png(std::vector<XMLDATA_STRUCT>& arr, LPCWSTR saveFile, PROJECT_DATA* projectData)
{
    projectData->maxWidth = 0;
    projectData->maxHeight = 0;
    RECT rcIcon = { 0 };
    // ��һ��ѭ��, �ǻ�ȡ������ͼƬ�滭��Ҫ�ĳߴ�
    for (auto it = arr.begin(); it != arr.end(); ++it)
    {
        XMLDATA_STRUCT& xmlData = *it;
        for (int i = 0; i < sizeof(xmlData.rc) / sizeof(xmlData.rc[0]); i++)
            __draw_calc_image(0, xmlData.file[i], xmlData.rc[i], rcIcon, projectData);
    }

    if (rcIcon.right == 0) return;  // û��ͼƬ������
    projectData->imgWidth   = rcIcon.right;
    projectData->imgHeight  = rcIcon.bottom;
    LPCANVAS hCanvas = _canvas_create(g_hWnd, projectData->imgWidth, projectData->imgHeight);
    _canvas_begindraw(hCanvas);
    memset(&rcIcon, 0,      sizeof(rcIcon));
    // �ڶ���ѭ���ǰ����е�ͼƬ���滭����ͼƬ��
    for (auto it = arr.begin(); it != arr.end(); ++it)
    {
        XMLDATA_STRUCT& xmlData = *it;
        for (int i = 0; i < sizeof(xmlData.rc) / sizeof(xmlData.rc[0]); i++)
            __draw_calc_image(hCanvas, xmlData.file[i], xmlData.rc[i], rcIcon, projectData);
    }
    _canvas_enddraw(hCanvas);
    SIZE_T size = 0;
    LPBYTE ptr = _img_save_formcanvas(hCanvas, &size);
    CFileRW f;
    f.open(saveFile, 4, -1);
    f.write(ptr, size);
    free(ptr);

    _canvas_destroy(hCanvas);
}

// ֻ���ý��ڴ���, ��Ҫ�ֶ�������ܱ��浽����
// ����xmlָ������ָ��״̬���ļ�·��
// projectData = �ĸ���Ŀ����
// index = �ڼ�����Ա, ������0��ʼ
// filename = Ҫ���õ��ļ�·��
// state = STATE ö�ٳ������ֵ, ����ֻ�� NORMAL, HOT, DOWN, BAN ��4��״̬
// dpiIndex = 0=100, 1=125, 2=150, 3=175, 4=200, �ڲ����ݴ����dpi�������ö�Ӧλ�õľ���, ͼ������ = dpiIndex * 4 + ״̬
bool xml_setimage_filename(PROJECT_DATA* projectData, int index, const wchar_t* filename, STATE state, int dpiIndex)
{
    XMLDATA_STRUCT& xmlData = projectData->arr->at(index);
    xmlData.SetFile(filename, state, dpiIndex);
    wstr pngFile;
    isCreatePath(projectData->xmlName, 0, 0, &pngFile); // ��ȡ��ͼƬ��·��
    GetBitmapFromMap(filename);
    // ��Ҫ�������ɴ�ͼƬ
    __make_image_png(*projectData->arr, pngFile.c_str(), projectData);
    if (projectData->hBitmap)delete projectData->hBitmap;
    projectData->hBitmap    = _img_create_from_file(pngFile.c_str());
    ModyfyTitle(true);
    return true;
}

inline LPCWSTR rc2str(RECT& rc)
{
    static wchar_t str[100] = { 0 };
    swprintf_s(str, 100, L"%d,%d,%d,%d", rc.left, rc.top, rc.right, rc.bottom);
    return str;
}

// �Ѽ�¼���ڴ��������д��xml��
// xml = ��Ҫд����xml
// index = �ڼ�����Ա
// imageFileName = 
inline static void __write_to_xml(PROJECT_DATA& data)
{
    wstr _name(50);
    for (auto it = data.arr->begin(); it != data.arr->end(); ++it)
    {
        XMLDATA_STRUCT& xmlData = *it;
        tinyxml2::XMLNode* node;
        CTinyXml2W& xml = *(data.GetXml());
        xml.InsertChildNode(L"@1", L"Item", xmlData.remarks, false, &node);     // ��ע(˵��)�ı��ǽڵ�ֵ, �������ǽڵ��������

        // ����������д��xml
        // д��ID, 4��״̬��·��, 4��״̬�ľ���
        xml.SetAttrValue(node, L"id",       xmlData.id);
        xml.SetAttrValue(node, L"text",     xmlData.text);

        for (int i = 0; i < sizeof(xmlData.rc) / sizeof(xmlData.rc[0]); i++)
        {
            _name.assign(L"szFile").append(i + 1);
            xml.SetAttrValue(node, _name.c_str(), xmlData.file[i]);
        }

        for (int i = 0; i < sizeof(xmlData.rc) / sizeof(xmlData.rc[0]); i++)
        {
            _name.assign(L"szRect").append(i + 1);
            xml.SetAttrValue(node, _name.c_str(), rc2str(xmlData.rc[i]));
        }

        xml.SetAttrValue(node, L"crText1",  argb2str(xmlData.crText[0]).c_str());
        xml.SetAttrValue(node, L"crText2",  argb2str(xmlData.crText[1]).c_str());
        xml.SetAttrValue(node, L"crText3",  argb2str(xmlData.crText[2]).c_str());
        xml.SetAttrValue(node, L"crText4",  argb2str(xmlData.crText[3]).c_str());

        xml.SetAttrValue(node, L"crBack1",  argb2str(xmlData.crBack[0]).c_str());
        xml.SetAttrValue(node, L"crBack2",  argb2str(xmlData.crBack[1]).c_str());
        xml.SetAttrValue(node, L"crBack3",  argb2str(xmlData.crBack[2]).c_str());
        xml.SetAttrValue(node, L"crBack4",  argb2str(xmlData.crBack[3]).c_str());

    }


}

// ���ٻ��߱�������, isFree Ϊtrueʱ�ᱣ�沢��������
void assist_destroy(bool isFree)
{
    wstr imagePath;
    BOOL isOk = false;
    int index = 0;
    for (auto i = s_data.begin(); i != s_data.end(); ++i, ++index)  // ÿ�����̶�����һ��
    {
        PROJECT_DATA& data = *i;
        CTinyXml2W* xml = data.GetXml();
        if (xml)
        {
            wchar_t maxSize[100], imgSize[100], themeIndex[100];
            swprintf_s(maxSize, 100, L"%d,%d", data.maxWidth, data.maxHeight);
            swprintf_s(imgSize, 100, L"%d,%d", data.imgWidth, data.imgHeight);
            swprintf_s(themeIndex, 100, L"%d", data.themeIndex);
            xml->Clear();
            xml->Create(L"IconProject");
            xml->InsertAttr(L"@1", L"Tips", L"�ļ���¼����״̬��ͼƬ·������ͼƬ�е�λ��");
            xml->InsertAttr(L"@1", L"maxSize", maxSize);
            xml->InsertAttr(L"@1", L"imgSize", imgSize);
            xml->InsertAttr(L"@1", L"themeIndex", themeIndex);

            __write_to_xml(data);
            xml->SaveFile();
        }
        if (isFree)
            data.free();
    }
    ModyfyTitle(false);
    if (!isFree) return;

    for (auto i = s_map_bitmap.begin(); i != s_map_bitmap.end(); ++i)
    {
        Gdiplus::Bitmap*& data = i->second;
        if (data)delete data;
        data = 0;
    }
    for (auto i = s_hash_array.begin(); i != s_hash_array.end(); ++i)
    {
        wchar_t*& p = *i;
        if (p)delete[] p;
        p = 0;
    }
    s_data.clear();
    s_map_bitmap.clear();
    s_hash_array.clear();
}



void __createPath(LPWSTR path)
{
    if (PathIsDirectoryW(path)) return;
    LPWSTR find = wcschr(path, '\\');
    wchar_t old = 0;
    while (find)
    {
        old = *find;
        *find = 0;
        if (!PathIsDirectoryW(path))
            CreateDirectoryW(path, 0);


        *find = old;
        find = wcschr(find + 1, '\\');

    }
    if (!PathIsDirectoryW(path))
        CreateDirectoryW(path, 0);
}
