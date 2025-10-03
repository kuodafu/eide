#include "ImageListView.h"



// 初始化项目, 从主题目录里读取已有的项目
void init_project()
{
    s_data.clear();

    wstr file(260);
    file.assign(g_runPath).append(L"data\\ImageProject.xml");   // 工程项目
    bool isSave = false;

    if (!g_xmlProject.LoadFile(file.c_str()))   // 加载项目文件失败, 需要初始化这个项目文件并写出文件
    {
        g_xmlProject.Create(L"ImageProject");
        g_xmlProject.InsertAttr(L"@1", L"Tips", L"记录工程信息, 每个节点都指向一个xml项目, 项目里记录详细的图标信息");
        g_xmlProject.SaveFile();
        //g_xmlProject.InsertAttr(L"@1", L"Imagepath", L"");
    }

    const int nChildCount = g_xmlProject.GetChildNodeCount(L"@1");
    s_data.reserve(100);
    wstr nodeName(100);
    wstr xmlPath(260);

    for (int i = 0; i < nChildCount; i++)
    {
        nodeName.assign(L"@1/@").append(i + 1);
        tinyxml2::XMLNode* node;
        g_xmlProject.GetNode(nodeName.c_str(), node);
        assert(node && "获取项目node失败");
        LPCWSTR xmlName = g_xmlProject.GetAttrValue(node, L"name");
        s_data.push_back(PROJECT_DATA(xmlName));
    }

    s_listProject.SetIndex(-1);
    s_listProject.SetItemCount(nChildCount, true);

}

bool isCreatePath(LPCWSTR name, wstr* ppath, wstr* pxmlFile, wstr* pngFile)
{
    wstr path(260);
    path.assign(g_runPath).append(L"data\\project\\").append(name);
    wstr nodeName(50);
    nodeName.assign(L"@1/").append(name);
    tinyxml2::XMLNode* node;
    g_xmlProject.GetNode(nodeName.c_str(), node);
    bool ret = PathIsDirectoryW(path.c_str());

    if (pxmlFile) pxmlFile->assign(path).append(L"\\").append(name).append(L".xml");
    if (pngFile) pngFile->assign(path).append(L"\\").append(name).append(L".png");      // 主题使用的图片数据
    if (ppath) ppath->swap(path);

    return ret;
}

// 枚举目录下的图片文件 *.jpg;*.png;*.jpeg;*.bmp;
void init_Image(const wchar_t* pszPath)
{
    if (!pszPath || !pszPath[0])return;

    wstr path(260);
    path.assign(pszPath);
    if (path.right(1) != L"\\") path.append(L"\\");

    s_dataImage.clear();
    wstr findPath(260);
    findPath.assign(path).append(L"*.*");

    WIN32_FIND_DATA fd = { 0 };
    const DWORD dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM;
    HANDLE hFind = FindFirstFileW(findPath.c_str(), &fd);
    if (INVALID_HANDLE_VALUE == hFind) return;
    int len = 0;
    wstr fileName(260);
    const wchar_t* exts[] = { L".jpg", L".png", L"jpeg", L".bmp" };
    do
    {
        if ((dwFileAttributes & fd.dwFileAttributes) == fd.dwFileAttributes)   // 匹配类型
        {
            //jpg;*.png;*.jpeg;*.bmp;
            int strlen = (int)wcslen(fd.cFileName);
            const wchar_t* pStart = fd.cFileName + strlen - 4;
            for (int i = 0; i < sizeof(exts) / sizeof(exts[0]); i++)
            {
                if (wcsicmp_api(pStart, exts[i]) == 0)
                {
                    fileName.assign(path).append(fd.cFileName);
                    s_dataImage.push_back(SHOWIMAGEDATA(fileName.c_str()));
                    len++;
                    break;
                }
            }
        }
    } while (FindNextFileW(hFind, &fd));

    FindClose(hFind);

    s_listImage.SetIndex(-1);
    s_listImage.SetItemCount((int)s_dataImage.size(), true);
}


// imagePath = 图片保存的工程目录, 后面不带'\'
// imageFileName = 原图片的完整路径, 需要获取文件名然后判断保存的目录下有没有这个文件
inline Gdiplus::Bitmap* __temp_load_bitmap(LPCWSTR imagePath, LPCWSTR imageFileName)
{
    if (!imageFileName || !imageFileName[0])return 0;
    LPCWSTR name = _file_get_name(imageFileName);
    assert(name && "__temp_load_bitmap 获取文件名失败");
    wstr saveFile(260);
    Gdiplus::Bitmap* hBitmap = 0;
    saveFile.assign(imagePath).append(name);    // 保存的目录
    if (PathFileExistsW(saveFile.c_str()))      // 如果文件存在, 则从这里加载图片
    {
        hBitmap = GetBitmapFromMap(saveFile.c_str());
        if (hBitmap) return hBitmap;

        // 图片加载失败, 删除对应主题目录的图片文件, 然后继续往下走加载xml里记录的文件
        //xml->DelAttr(node, XML_NAME_IMAGE);
        DeleteFileW(saveFile.c_str());
    }
    hBitmap = GetBitmapFromMap(imageFileName);
    //CopyFileW(imageFileName, saveFile.c_str(), false);
    return hBitmap;
}

inline void str2rc(LPCWSTR str, RECT& rc)
{
    if (!str || !str[0]) str = L"0,0,0,0";
    swscanf_s(str, L"%d,%d,%d,%d", &rc.left, &rc.top, &rc.right, &rc.bottom);
}

// 提供一个xml项目路径, 内部会加载并读取这个xml的数据, 返回new 出来的xml对象
// xmlFile = xml完整路径, 是从工程xml里读出来的路径, 如果文件不存在则会创建这个文件
// name = 名字, 图片数据是复制到这个目录下
// ppArr = 接收xml里的数据
CTinyXml2W* init_IconItem(PROJECT_DATA* data)
{
    assert(data && data->xmlName && data->xmlName[0]);

    wstr xmlFile, projectPath, pngFile;
    isCreatePath(data->xmlName, &projectPath, &xmlFile, &pngFile);
    __createPath(projectPath.data());   // 先创建工程目录, 工程目录必须存在
    data->hBitmap = _img_create_from_file(pngFile.c_str());
    CTinyXml2W* xml = new CTinyXml2W;
    if (!xml->LoadFile(xmlFile.c_str()))
    {
        xml->Create(L"IconProject");
        xml->InsertAttr(L"@1", L"Tips", L"文件记录几个状态的图片路径");
        xml->SaveFile();
    }

    std::vector<XMLDATA_STRUCT>* pArr = new std::vector<XMLDATA_STRUCT>;
    data->arr = pArr;
    const int nChildCount = xml->GetChildNodeCount(L"@1");
    pArr->reserve(nChildCount ? nChildCount : 50);
    wstr nodeName(100);

    LPCWSTR maxSize = xml->GetAttrValue(L"@1", L"maxSize");
    LPCWSTR imgSize = xml->GetAttrValue(L"@1", L"imgSize");
    LPCWSTR themeIndex = xml->GetAttrValue(L"@1", L"themeIndex");
    if (!maxSize || !maxSize[0]) maxSize = L"16,16";
    if (!imgSize || !imgSize[0]) imgSize = L"16,16";
    if (!themeIndex || !themeIndex[0]) themeIndex = L"0";
    swscanf_s(maxSize, L"%d,%d", &data->maxWidth, &data->maxHeight);
    swscanf_s(imgSize, L"%d,%d", &data->imgWidth, &data->imgHeight);
    data->themeIndex = _wtol(themeIndex);
    if (data->maxWidth <= 0)data->maxWidth = 16;
    if (data->maxHeight <= 0)data->maxHeight = 16;
    if (data->themeIndex > 1 || data->themeIndex < 0)data->themeIndex = 0;

    wstr _name(50);
    for (int i = 0; i < nChildCount; i++)
    {
        nodeName.assign(L"@1/@").append(i + 1);
        tinyxml2::XMLNode* node;
        xml->GetNode(nodeName.c_str(), node);
        assert(node && "init_IconItem 获取node失败");

        LPCWSTR id      = xml->GetAttrValue(node, L"id");
        LPCWSTR remarks = xml->GetNodeValue(node);
        LPCWSTR text    = xml->GetAttrValue(node, L"text");


        pArr->push_back(XMLDATA_STRUCT(id, text, remarks));

        XMLDATA_STRUCT& xmlData = pArr->back();

        for (int i = 0; i < sizeof(xmlData.rc) / sizeof(xmlData.rc[0]); i++)
        {
            _name.assign(L"szFile").append(i + 1);
            LPCWSTR file = xml->GetAttrValue(node, _name.c_str());
            appendBitmapToMap(file);  // 把记录的图片加载到内存
            int state = 0;
            switch (i % 4)
            {
            case 1: state = STATE::HOT;     break;
            case 2: state = STATE::DOWN;    break;
            case 3: state = STATE::BAN;     break;
            default:state = STATE::NORMAL;  break;
                break;
            }
            xmlData.SetFile(file, state, i / 4);
        }

        auto isReadOldFile = [](XMLDATA_STRUCT& xmlData) -> bool
        {
            for (int i = 0; i < 4; i++)
            {
                if (xmlData.file[i])
                    return false;   // 旧版只有4个状态图片路径, 读出来4个, 只要有一个不是空的, 那就是不需要读旧的路径
            }
            return true;    // 4次都是为0, 读旧的路径
        };
        if (isReadOldFile(xmlData))
        {
            LPCWSTR normal  = xml->GetAttrValue(node, L"normal");
            LPCWSTR hot     = xml->GetAttrValue(node, L"hot");
            LPCWSTR down    = xml->GetAttrValue(node, L"down");
            LPCWSTR ban     = xml->GetAttrValue(node, L"ban");

            appendBitmapToMap(normal);  // 把记录的图片加载到内存
            appendBitmapToMap(hot);
            appendBitmapToMap(down);
            appendBitmapToMap(ban);

            xmlData.SetFile(normal  , STATE::NORMAL , 0);
            xmlData.SetFile(hot     , STATE::HOT    , 0);
            xmlData.SetFile(down    , STATE::DOWN   , 0);
            xmlData.SetFile(ban     , STATE::BAN    , 0);
        }




        for (int i = 0; i < sizeof(xmlData.rc) / sizeof(xmlData.rc[0]); i++)
        {
            _name.assign(L"szRect").append(i + 1);
            str2rc(xml->GetAttrValue(node, _name.c_str()),    xmlData.rc[i]);
        }
        auto isReadOldRect = [](XMLDATA_STRUCT& xmlData) -> bool
        {
            for (int i = 0; i < 4; i++)
            {
                RECT& rc = xmlData.rc[i];
                if (rc.bottom > rc.top && rc.right > rc.left && (rc.right - rc.left > 0 || rc.bottom - rc.top > 0))
                    return false;   // 旧版只有4个矩形位置, 读出来4个, 只要有一个不是空的, 那就是不需要读旧的矩形
            }
            return true;    // 4次都是为0, 读旧的矩形
        };
        if (isReadOldRect(xmlData))
        {
            str2rc(xml->GetAttrValue(node, L"rcnormal"),    xmlData.rc[0]);
            str2rc(xml->GetAttrValue(node, L"rchot"),       xmlData.rc[1]);
            str2rc(xml->GetAttrValue(node, L"rcdown"),      xmlData.rc[2]);
            str2rc(xml->GetAttrValue(node, L"rcban"),       xmlData.rc[3]);
        }

        xmlData.crText[0] = str2argb(xml->GetAttrValue(node, L"crText1"), 0);
        xmlData.crText[1] = str2argb(xml->GetAttrValue(node, L"crText2"), 0);
        xmlData.crText[2] = str2argb(xml->GetAttrValue(node, L"crText3"), 0);
        xmlData.crText[3] = str2argb(xml->GetAttrValue(node, L"crText4"), 0);

        xmlData.crBack[0] = str2argb(xml->GetAttrValue(node, L"crBack1"), 0);
        xmlData.crBack[1] = str2argb(xml->GetAttrValue(node, L"crBack2"), 0);
        xmlData.crBack[2] = str2argb(xml->GetAttrValue(node, L"crBack3"), 0);
        xmlData.crBack[3] = str2argb(xml->GetAttrValue(node, L"crBack4"), 0);

    }
    if (!data->hBitmap && nChildCount > 0)  // 没有图片, 且工程项目数大于0, 那就触发一下重新生成大图片
    {
        XMLDATA_STRUCT& xmlData = pArr->front();
        xml_setimage_filename(data, 0, xmlData.file[0], STATE::NORMAL, 0);
    }
    return xml;
}




/// 判断一个路径是否是已存在的目录
bool IsDirectory(const wchar_t* pstrPath)
{
    DWORD dw = GetFileAttributesW(pstrPath);
    if (dw == INVALID_FILE_ATTRIBUTES)
        return false;
    return (dw & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*复制目录及目录中的所有内容*/
// pstrFolder = 要拷贝的目录
// pstrDest = 拷贝到的目录
bool CopyFolder(const wchar_t* pstrFolder, const wchar_t* pstrDest)
{
    // 检查输入目录是否是合法目录
    if (!IsDirectory(pstrFolder)) return false;

    wstr strFind(260);  // 查找的路径, "c:\*.*"
    wstr strDest(260);  // 拷贝到的路径, "d:\"
    wstr path(260);     // 查找的路径, "c:\"
    path = pstrFolder;
    strDest = pstrDest;
    if (!IsDirectory(pstrDest))
        __createPath(strDest.data());

    if (path.back() != L'\\' && path.back() != L'/')
        path.append(L"\\");
    strFind.assign(path).append(L"*.*");

    if (strDest.back() != L'\\' && strDest.back() != L'/')
        strDest.append(L"\\");


    // 打开文件查找，查看源目录中是否存在匹配的文件
    // 调用FindFile后，必须调用FindNextFile才能获得查找文件的信息
    WIN32_FIND_DATA wfd;
    HANDLE hFind = FindFirstFileW(strFind.c_str(), &wfd);
    if (hFind == INVALID_HANDLE_VALUE) return false;

    wstr strSubFolder(260);
    wstr strDestFolder(260);
    do
    {
        if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (wfd.cFileName[0] == L'.')
                continue;

            strSubFolder.assign(path).append(wfd.cFileName);            // 被拷贝的目录
            strDestFolder.assign(strDest).append(wfd.cFileName);        // 拷贝到的目录
            CopyFolder(strSubFolder.c_str(), strDestFolder.c_str());    // 递归拷贝
        }
        else
        {
            strSubFolder.assign(path).append(wfd.cFileName);            // 要拷贝的文件路径
            strDestFolder.assign(strDest).append(wfd.cFileName);        // 拷贝到的文件路径
            CopyFileW(strSubFolder.c_str(), strDestFolder.c_str(), FALSE);
        }
    } while (FindNextFileW(hFind, &wfd));

    FindClose(hFind);
    return true;
}

// 删除目录及目录中的所有内容
// pstrFolder = 文件夹路径, 后面可加可不加反斜杠
// recursive = 是否删除子目录, 为true则递归删除子目录
bool DeleteFolder(const wchar_t* pstrFolder, bool recursive)
{
    // 检查输入目录是否是合法目录
    if (!IsDirectory(pstrFolder))
        return false;

    wstr strFind(260);
    wstr path(260);
    path = pstrFolder;

    if (path.back() != L'\\' && path.back() != L'/')
        path.append(L"\\");

    strFind.assign(path).append(L"*.*");

    // 打开文件查找，查看源目录中是否存在匹配的文件
    // 调用FindFile后，必须调用FindNextFile才能获得查找文件的信息
    WIN32_FIND_DATA wfd;
    HANDLE hFind = FindFirstFileW(strFind.c_str(), &wfd);
    wstr strSubFolder;
    if (hFind == INVALID_HANDLE_VALUE) return false;

    do
    {
        if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (wfd.cFileName[0] == L'.' || !recursive) continue;

            // 递归继续删除下面的目录
            strSubFolder.assign(path).append(wfd.cFileName);
            DeleteFolder(strSubFolder.c_str(), recursive);
        }
        else
        {
            strSubFolder.assign(path).append(wfd.cFileName);
            DeleteFileW(strSubFolder.c_str());  // 删除文件
        }
    } while (FindNextFileW(hFind, &wfd));

    // 删除空目录
    FindClose(hFind);
    return RemoveDirectoryW(pstrFolder) == TRUE;
}
