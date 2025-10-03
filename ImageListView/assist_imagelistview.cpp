#include "ImageListView.h"
#include <map>

// 所有位图信息
struct ARR_BITMAP
{

};

static std::map<LONG_PTR, Gdiplus::Bitmap*> s_map_bitmap;   // 所有加载的图片都保存在这里
static std::vector<wchar_t*> s_hash_array;                  // 文本哈希是一块内存, 计算哈希就返回这个地址, 需要释放


// 从路径加载图片, 返回加载后的图片指针
// 内部会把图片保存到 s_map_file 这个map里, 如果之前存在会释放之前保存的图片
Gdiplus::Bitmap* appendBitmapToMap(const wchar_t* path)
{
    if (!path || !path[0]) return 0;
    Gdiplus::Bitmap*& pBmp = s_map_bitmap[_hash_fun(path)];
    if (pBmp)delete pBmp;
    pBmp = _img_create_from_file(path);
    return pBmp;
}

// 从指定路径获取图片, 如果未加载内部会加载
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
        // 是修改, 设置标题第一个字符为星号
        if (ptr[0] == '*')
            return;
        wstr newTitle(len + 2);
        newTitle.assign(L"* ").append(title.c_str());
        SetWindowTextW(g_hWnd, newTitle.c_str());
    }
    else
    {
        // 不是修改, 去掉标题的第一个星号
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

// 删除某个项目, 会从数组里删除成员, 从xml里删除对应节点, 从位图数组里释放目录下的文件
void eraseItem(int index)
{
    auto itArr = s_data.begin() + index;
    PROJECT_DATA& data = (*itArr);
    wstr imagePath, xmlFile;
    if (!isCreatePath(data.xmlName, &imagePath, &xmlFile)) return;

    // 检查输入目录是否是合法目录
    while (1)
    {
        if (!IsDirectory(imagePath.c_str()))break;

        wstr strFind(260);
        wstr path(260);
        path = imagePath;

        if (path.back() != L'\\' && path.back() != L'/')
            path.append(L"\\");

        strFind.assign(path).append(L"*.*");

        // 打开文件查找，查看源目录中是否存在匹配的文件
        // 调用FindFile后，必须调用FindNextFile才能获得查找文件的信息
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

            DeleteFileW(strSubFolder.c_str());  // 删除文件
        } while (FindNextFileW(hFind, &wfd));

        // 删除空目录
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


// 从当前选中的工程获取指定状态的图片位置
// projectData = 哪个项目数据
// index = 第几个成员, 索引从0开始
// state = STATE 枚举常量里的值, 现在只有 NORMAL, HOT, DOWN, BAN 这4种状态
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

// 从当前选中的工程获取指定状态的图片地址, 如果没有则返回0
// projectData = 哪个项目数据
// index = 第几个成员, 索引从0开始
// state = STATE 枚举常量里的值, 现在只有 NORMAL, HOT, DOWN, BAN 这4种状态
// dpiIndex = 0=100, 1=125, 2=150, 3=175, 4=200, 内部根据传入的dpi索引设置对应位置的矩形, 图标索引 = dpiIndex * 4 + 状态
// rcIcon = 接收图片在大图片里的位置
const wchar_t* xml_getimage_filename(PROJECT_DATA* projectData, int index, STATE state, int dpiIndex, RECT* rcIcon)
{
    XMLDATA_STRUCT& xmlData = projectData->arr->at(index);
    int _index = _dpiindex2index(state, dpiIndex);
    if (rcIcon)memcpy(rcIcon, &xmlData.rc[_index], sizeof(RECT));
    return xmlData.file[_index];
}


// 把素材路径转换成工程路径
// lpszProjectPath = 工程目录, 不带'\'
// lpszImageFileName = 图片素材完整路径
LPCWSTR _srcfile_to_projectfile(LPCWSTR lpszProjectPath, LPCWSTR lpszImageFileName)
{
    static wstr file(260);
    LPCWSTR pos = _file_get_name(lpszImageFileName);
    file.assign(lpszProjectPath).append(pos);
    return file.c_str();
}

// 释放指定索引的图片数据
// state = 要删除的状态, -1=删除全部, -2=删除当前缩放4个状态, 现在只有 NORMAL, HOT, DOWN, BAN 这4种状态
// dpiIndex = 0=100, 1=125, 2=150, 3=175, 4=200, 内部根据传入的dpi索引设置对应位置的矩形, 图标索引 = dpiIndex * 4 + 状态
void xml_delete_image(PROJECT_DATA* projectData, int index, int state, int dpiIndex)
{
    auto it = projectData->arr->begin() + index;
    XMLDATA_STRUCT& xmlData = *it;
    if (state == -1)
    {
        xmlData.free(); // 清除记录的所有状态
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

// 计算或者绘画到大图片上
// hCanvas = 绘画句柄, 为0则只计算尺寸, 忽略rc参数
// file = 图片文件路径
// rc = 画出这个图片保存在大图片里的位置
// rcIcon = 当前大图片使用的矩形
// projectData = 工程数据, 这个函数会把最大尺寸的图片记录到这个结构里
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
        rcIcon.bottom = projectData->maxHeight; // 底边使用最大的底边, 宽度使用图片宽度
        _img_destroy(hBitmap);
        return;
    }


    _canvas_drawimagerect(hCanvas, hBitmap, (float)rc.left, (float)rc.top, (float)rc.right, (float)rc.bottom, 255);

    rcIcon.left += width;
    rcIcon.top = 0;
    rcIcon.right = rcIcon.left + width;
    rcIcon.bottom = projectData->maxHeight; // 底边使用最大的底边, 宽度使用图片宽度

    _img_destroy(hBitmap);
}



// 把xml里的所有图片都弄到一张大图片里, 并保存到 saveFile 这个路径
inline void __make_image_png(std::vector<XMLDATA_STRUCT>& arr, LPCWSTR saveFile, PROJECT_DATA* projectData)
{
    projectData->maxWidth = 0;
    projectData->maxHeight = 0;
    RECT rcIcon = { 0 };
    // 第一次循环, 是获取到所有图片绘画需要的尺寸
    for (auto it = arr.begin(); it != arr.end(); ++it)
    {
        XMLDATA_STRUCT& xmlData = *it;
        for (int i = 0; i < sizeof(xmlData.rc) / sizeof(xmlData.rc[0]); i++)
            __draw_calc_image(0, xmlData.file[i], xmlData.rc[i], rcIcon, projectData);
    }

    if (rcIcon.right == 0) return;  // 没有图片不保存
    projectData->imgWidth   = rcIcon.right;
    projectData->imgHeight  = rcIcon.bottom;
    LPCANVAS hCanvas = _canvas_create(g_hWnd, projectData->imgWidth, projectData->imgHeight);
    _canvas_begindraw(hCanvas);
    memset(&rcIcon, 0,      sizeof(rcIcon));
    // 第二次循环是把所有的图片都绘画到大图片上
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

// 只设置进内存里, 需要手动保存才能保存到磁盘
// 设置xml指定索引指定状态的文件路径
// projectData = 哪个项目数据
// index = 第几个成员, 索引从0开始
// filename = 要设置的文件路径
// state = STATE 枚举常量里的值, 现在只有 NORMAL, HOT, DOWN, BAN 这4种状态
// dpiIndex = 0=100, 1=125, 2=150, 3=175, 4=200, 内部根据传入的dpi索引设置对应位置的矩形, 图标索引 = dpiIndex * 4 + 状态
bool xml_setimage_filename(PROJECT_DATA* projectData, int index, const wchar_t* filename, STATE state, int dpiIndex)
{
    XMLDATA_STRUCT& xmlData = projectData->arr->at(index);
    xmlData.SetFile(filename, state, dpiIndex);
    wstr pngFile;
    isCreatePath(projectData->xmlName, 0, 0, &pngFile); // 获取大图片的路径
    GetBitmapFromMap(filename);
    // 需要重新生成大图片
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

// 把记录在内存里的数据写到xml里
// xml = 需要写到的xml
// index = 第几个成员
// imageFileName = 
inline static void __write_to_xml(PROJECT_DATA& data)
{
    wstr _name(50);
    for (auto it = data.arr->begin(); it != data.arr->end(); ++it)
    {
        XMLDATA_STRUCT& xmlData = *it;
        tinyxml2::XMLNode* node;
        CTinyXml2W& xml = *(data.GetXml());
        xml.InsertChildNode(L"@1", L"Item", xmlData.remarks, false, &node);     // 备注(说明)文本是节点值, 其他都是节点里的属性

        // 把所有数据写出xml
        // 写出ID, 4个状态的路径, 4个状态的矩形
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

// 销毁或者保存数据, isFree 为true时会保存并销毁数据
void assist_destroy(bool isFree)
{
    wstr imagePath;
    BOOL isOk = false;
    int index = 0;
    for (auto i = s_data.begin(); i != s_data.end(); ++i, ++index)  // 每个工程都保存一遍
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
            xml->InsertAttr(L"@1", L"Tips", L"文件记录几个状态的图片路径和在图片中的位置");
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
