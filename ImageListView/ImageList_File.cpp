#include "ImageListView.h"



// ��ʼ����Ŀ, ������Ŀ¼���ȡ���е���Ŀ
void init_project()
{
    s_data.clear();

    wstr file(260);
    file.assign(g_runPath).append(L"data\\ImageProject.xml");   // ������Ŀ
    bool isSave = false;

    if (!g_xmlProject.LoadFile(file.c_str()))   // ������Ŀ�ļ�ʧ��, ��Ҫ��ʼ�������Ŀ�ļ���д���ļ�
    {
        g_xmlProject.Create(L"ImageProject");
        g_xmlProject.InsertAttr(L"@1", L"Tips", L"��¼������Ϣ, ÿ���ڵ㶼ָ��һ��xml��Ŀ, ��Ŀ���¼��ϸ��ͼ����Ϣ");
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
        assert(node && "��ȡ��Ŀnodeʧ��");
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
    if (pngFile) pngFile->assign(path).append(L"\\").append(name).append(L".png");      // ����ʹ�õ�ͼƬ����
    if (ppath) ppath->swap(path);

    return ret;
}

// ö��Ŀ¼�µ�ͼƬ�ļ� *.jpg;*.png;*.jpeg;*.bmp;
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
        if ((dwFileAttributes & fd.dwFileAttributes) == fd.dwFileAttributes)   // ƥ������
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


// imagePath = ͼƬ����Ĺ���Ŀ¼, ���治��'\'
// imageFileName = ԭͼƬ������·��, ��Ҫ��ȡ�ļ���Ȼ���жϱ����Ŀ¼����û������ļ�
inline Gdiplus::Bitmap* __temp_load_bitmap(LPCWSTR imagePath, LPCWSTR imageFileName)
{
    if (!imageFileName || !imageFileName[0])return 0;
    LPCWSTR name = _file_get_name(imageFileName);
    assert(name && "__temp_load_bitmap ��ȡ�ļ���ʧ��");
    wstr saveFile(260);
    Gdiplus::Bitmap* hBitmap = 0;
    saveFile.assign(imagePath).append(name);    // �����Ŀ¼
    if (PathFileExistsW(saveFile.c_str()))      // ����ļ�����, ����������ͼƬ
    {
        hBitmap = GetBitmapFromMap(saveFile.c_str());
        if (hBitmap) return hBitmap;

        // ͼƬ����ʧ��, ɾ����Ӧ����Ŀ¼��ͼƬ�ļ�, Ȼ����������߼���xml���¼���ļ�
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

// �ṩһ��xml��Ŀ·��, �ڲ�����ز���ȡ���xml������, ����new ������xml����
// xmlFile = xml����·��, �Ǵӹ���xml���������·��, ����ļ���������ᴴ������ļ�
// name = ����, ͼƬ�����Ǹ��Ƶ����Ŀ¼��
// ppArr = ����xml�������
CTinyXml2W* init_IconItem(PROJECT_DATA* data)
{
    assert(data && data->xmlName && data->xmlName[0]);

    wstr xmlFile, projectPath, pngFile;
    isCreatePath(data->xmlName, &projectPath, &xmlFile, &pngFile);
    __createPath(projectPath.data());   // �ȴ�������Ŀ¼, ����Ŀ¼�������
    data->hBitmap = _img_create_from_file(pngFile.c_str());
    CTinyXml2W* xml = new CTinyXml2W;
    if (!xml->LoadFile(xmlFile.c_str()))
    {
        xml->Create(L"IconProject");
        xml->InsertAttr(L"@1", L"Tips", L"�ļ���¼����״̬��ͼƬ·��");
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
        assert(node && "init_IconItem ��ȡnodeʧ��");

        LPCWSTR id      = xml->GetAttrValue(node, L"id");
        LPCWSTR remarks = xml->GetNodeValue(node);
        LPCWSTR text    = xml->GetAttrValue(node, L"text");


        pArr->push_back(XMLDATA_STRUCT(id, text, remarks));

        XMLDATA_STRUCT& xmlData = pArr->back();

        for (int i = 0; i < sizeof(xmlData.rc) / sizeof(xmlData.rc[0]); i++)
        {
            _name.assign(L"szFile").append(i + 1);
            LPCWSTR file = xml->GetAttrValue(node, _name.c_str());
            appendBitmapToMap(file);  // �Ѽ�¼��ͼƬ���ص��ڴ�
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
                    return false;   // �ɰ�ֻ��4��״̬ͼƬ·��, ������4��, ֻҪ��һ�����ǿյ�, �Ǿ��ǲ���Ҫ���ɵ�·��
            }
            return true;    // 4�ζ���Ϊ0, ���ɵ�·��
        };
        if (isReadOldFile(xmlData))
        {
            LPCWSTR normal  = xml->GetAttrValue(node, L"normal");
            LPCWSTR hot     = xml->GetAttrValue(node, L"hot");
            LPCWSTR down    = xml->GetAttrValue(node, L"down");
            LPCWSTR ban     = xml->GetAttrValue(node, L"ban");

            appendBitmapToMap(normal);  // �Ѽ�¼��ͼƬ���ص��ڴ�
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
                    return false;   // �ɰ�ֻ��4������λ��, ������4��, ֻҪ��һ�����ǿյ�, �Ǿ��ǲ���Ҫ���ɵľ���
            }
            return true;    // 4�ζ���Ϊ0, ���ɵľ���
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
    if (!data->hBitmap && nChildCount > 0)  // û��ͼƬ, �ҹ�����Ŀ������0, �Ǿʹ���һ���������ɴ�ͼƬ
    {
        XMLDATA_STRUCT& xmlData = pArr->front();
        xml_setimage_filename(data, 0, xmlData.file[0], STATE::NORMAL, 0);
    }
    return xml;
}




/// �ж�һ��·���Ƿ����Ѵ��ڵ�Ŀ¼
bool IsDirectory(const wchar_t* pstrPath)
{
    DWORD dw = GetFileAttributesW(pstrPath);
    if (dw == INVALID_FILE_ATTRIBUTES)
        return false;
    return (dw & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*����Ŀ¼��Ŀ¼�е���������*/
// pstrFolder = Ҫ������Ŀ¼
// pstrDest = ��������Ŀ¼
bool CopyFolder(const wchar_t* pstrFolder, const wchar_t* pstrDest)
{
    // �������Ŀ¼�Ƿ��ǺϷ�Ŀ¼
    if (!IsDirectory(pstrFolder)) return false;

    wstr strFind(260);  // ���ҵ�·��, "c:\*.*"
    wstr strDest(260);  // ��������·��, "d:\"
    wstr path(260);     // ���ҵ�·��, "c:\"
    path = pstrFolder;
    strDest = pstrDest;
    if (!IsDirectory(pstrDest))
        __createPath(strDest.data());

    if (path.back() != L'\\' && path.back() != L'/')
        path.append(L"\\");
    strFind.assign(path).append(L"*.*");

    if (strDest.back() != L'\\' && strDest.back() != L'/')
        strDest.append(L"\\");


    // ���ļ����ң��鿴ԴĿ¼���Ƿ����ƥ����ļ�
    // ����FindFile�󣬱������FindNextFile���ܻ�ò����ļ�����Ϣ
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

            strSubFolder.assign(path).append(wfd.cFileName);            // ��������Ŀ¼
            strDestFolder.assign(strDest).append(wfd.cFileName);        // ��������Ŀ¼
            CopyFolder(strSubFolder.c_str(), strDestFolder.c_str());    // �ݹ鿽��
        }
        else
        {
            strSubFolder.assign(path).append(wfd.cFileName);            // Ҫ�������ļ�·��
            strDestFolder.assign(strDest).append(wfd.cFileName);        // ���������ļ�·��
            CopyFileW(strSubFolder.c_str(), strDestFolder.c_str(), FALSE);
        }
    } while (FindNextFileW(hFind, &wfd));

    FindClose(hFind);
    return true;
}

// ɾ��Ŀ¼��Ŀ¼�е���������
// pstrFolder = �ļ���·��, ����ɼӿɲ��ӷ�б��
// recursive = �Ƿ�ɾ����Ŀ¼, Ϊtrue��ݹ�ɾ����Ŀ¼
bool DeleteFolder(const wchar_t* pstrFolder, bool recursive)
{
    // �������Ŀ¼�Ƿ��ǺϷ�Ŀ¼
    if (!IsDirectory(pstrFolder))
        return false;

    wstr strFind(260);
    wstr path(260);
    path = pstrFolder;

    if (path.back() != L'\\' && path.back() != L'/')
        path.append(L"\\");

    strFind.assign(path).append(L"*.*");

    // ���ļ����ң��鿴ԴĿ¼���Ƿ����ƥ����ļ�
    // ����FindFile�󣬱������FindNextFile���ܻ�ò����ļ�����Ϣ
    WIN32_FIND_DATA wfd;
    HANDLE hFind = FindFirstFileW(strFind.c_str(), &wfd);
    wstr strSubFolder;
    if (hFind == INVALID_HANDLE_VALUE) return false;

    do
    {
        if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (wfd.cFileName[0] == L'.' || !recursive) continue;

            // �ݹ����ɾ�������Ŀ¼
            strSubFolder.assign(path).append(wfd.cFileName);
            DeleteFolder(strSubFolder.c_str(), recursive);
        }
        else
        {
            strSubFolder.assign(path).append(wfd.cFileName);
            DeleteFileW(strSubFolder.c_str());  // ɾ���ļ�
        }
    } while (FindNextFileW(hFind, &wfd));

    // ɾ����Ŀ¼
    FindClose(hFind);
    return RemoveDirectoryW(pstrFolder) == TRUE;
}
