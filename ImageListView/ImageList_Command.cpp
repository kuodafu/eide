#include "ImageListView.h"
#include <zlib/Czlib.h>

static std::vector<LPCWSTR> m_news_name;
static std::vector<LPCWSTR> m_news_value;
static std::vector<LPCWSTR> m_news_remarks;
static LPWSTR m_news_str;

void _save_make_savepath();
void _save_make_xml_png(PROJECT_DATA& data, LPCWSTR saveFileName);

void SaveExt(const wchar_t* saveFileName, const void* pngData, DWORD pngDataSize, const wchar_t* xmlText, _str* pCompressData = 0);
INT_PTR CALLBACK    DlgProc_NewItems(HWND, UINT, WPARAM, LPARAM);

// 设置图片被选择, 会弹出文件对话框选择图片, 根据ID替换不同的图片
inline void _listitem_command_setimage(HWND hWnd, int id)
{
    LPCWSTR szLabelText;
    wstr title(50);
    //int dpiIndex = HIBYTE(id) - 1;
    int dpiIndex = g_dpiIndex;
    int state = LOBYTE(id) - 0x20;
    switch (state)
    {
    case 1:     state = STATE::HOT;     szLabelText = L"热点图片"; title = L"请选择新的热点状态图片"; break;
    case 2:     state = STATE::DOWN;    szLabelText = L"按下图片"; title = L"请选择新的按下状态图片"; break;
    case 3:     state = STATE::BAN;     szLabelText = L"禁止图片"; title = L"请选择新的禁止状态图片"; break;
    default:    state = STATE::NORMAL;  szLabelText = L"正常图片"; title = L"请选择新的正常状态图片"; break;
    }

    switch (dpiIndex)
    {
    case 1: title.append(L" 缩放: 125"); break;
    case 2: title.append(L" 缩放: 150"); break;
    case 3: title.append(L" 缩放: 175"); break;
    case 4: title.append(L" 缩放: 200"); break;
    default:title.append(L" 缩放: 100"); break;
        break;
    }
    CFileDialog_Com file;
    file.createOpen();
    file.SetFilter(L"图片文件\0*.jpg;*.png;*.jpeg;*.bmp;\0所有文件\0*.*\0", 0);
    file.SetTitle(title.c_str());
    file.SetOkButtonLabel(L"选择图片");
    file.SetFileNameLabel(szLabelText);
    if (!file.Show(hWnd)) return;
    wstr path = file.GetResult();
    if (path.empty()) return;
    int index = s_listItem.GetIndex();

    xml_setimage_filename(m_data, index, path.c_str(), (STATE)state, dpiIndex);
    s_listItem.InvalidateRect();
}

// 判断有没有重复的ID, 有的话返回这个id对应的数据, 没有就返回0
inline XMLDATA_STRUCT* _command_get_id_data(PROJECT_DATA* projectData, LPCWSTR id)
{
    const int nChildCount = (int)projectData->arr->size();
    for (auto it = m_data->arr->begin(); it != m_data->arr->end(); ++it)
    {
        XMLDATA_STRUCT& xmlData = *it;
        if (xmlData.id && wcscmp(xmlData.id, id) == 0)
            return &xmlData;
    }
    return 0;
}

inline void _command_set_remarks(HWND hWnd, int index)
{
    if (!m_data || index == -1) return;
    XMLDATA_STRUCT& xmlData = m_data->arr->at(index);

    LPWSTR text = input_Open(hWnd, L"请输入项目说明", L"输入说明或者叫备注文本", xmlData.remarks);
    if (!text)return;

    if (xmlData.text && wcscmp(xmlData.id, text) == 0)
    {
        input_free(text);
        return; // 新输入的文本和老的文本一样, 不需要重新设置
    }

    xmlData.SetRemarks(text);
    s_listItem.InvalidateItem(index);
    ModyfyTitle(true);
    input_free(text);
}
inline void _command_set_text(HWND hWnd, int index)
{
    if (!m_data || index == -1) return;
    XMLDATA_STRUCT& xmlData = m_data->arr->at(index);

    LPWSTR text = input_Open(hWnd, L"不会改变原来的标题, 只是在显示的时候是使用这个设置的文本来显示", L"请输入要替换显示的文本", xmlData.text);
    if (!text)return;

    if (xmlData.text && wcscmp(xmlData.id, text) == 0)
    {
        input_free(text);
        return; // 新输入的文本和老的文本一样, 不需要重新设置
    }

    xmlData.SetText(text);
    s_listItem.InvalidateItem(index);
    ModyfyTitle(true);
    input_free(text);
}
inline void _command_set_id(HWND hWnd, int index)
{
    if (!m_data || index == -1) return;
    XMLDATA_STRUCT& xmlData = m_data->arr->at(index);

    LPWSTR id = input_Open(hWnd, L"请输入新的项目ID, ID必须是唯一, 主题加载器是通过ID加载的", L"输入ID", xmlData.id);
    if (!id)return;

    if (xmlData.id && wcscmp(xmlData.id, id) == 0)
    {
        input_free(id);
        return; // 新输入的ID和老的ID一样, 不需要重新设置
    }

    // 先判断有没有重复的ID, 有的话不让设置
    for (auto it = m_data->arr->begin(); it != m_data->arr->end(); ++it)
    {
        XMLDATA_STRUCT& data = *it;
        if (&xmlData != &data && data.id && wcscmp(data.id, id) == 0)
        {
            // 不是当前要改变的id, 并且新id根其他有同名的, 不允许修改
            wstr text;
            text.Format(L"ID(%s)已经存在, 不支持设置重复的ID\n已存在ID信息:\n修改文本 = %s\n备注文本 = %s\n",
                id, data.text, data.remarks);
            input_free(id);
            MessageBoxW(hWnd, text.c_str(), L"ID不支持重复", 0);
            return;
        }
    }

    xmlData.SetID(id);
    s_listItem.InvalidateItem(index);
    ModyfyTitle(true);
    input_free(id);
}
// 插入xml节点
// id = 要插入节点的id
// text = 要替换的文本, 会写在节点的 text属性里
// remarks = 备注文本, 会写在节点值这里
inline void _insert_xml_node(LPCWSTR id, LPCWSTR text, LPCWSTR remarks, bool isUpdate)
{
    if (!id || !id[0]) return;
    int index = 0;
    XMLDATA_STRUCT* pXmlData = _command_get_id_data(m_data, id);
    if (!pXmlData)
    {
        m_data->arr->push_back(XMLDATA_STRUCT(id, text, remarks));
        XMLDATA_STRUCT& xmlData = m_data->arr->back();
        pXmlData = &xmlData;
    }
    assert(pXmlData);
    pXmlData->SetID(id);
    if (remarks)    pXmlData->SetRemarks(remarks);
    if (text)       pXmlData->SetText(text);
    

    const int count = (int)m_data->arr->size();
    s_listItem.SetItemCount(count, isUpdate);
    if (isUpdate)
        s_listProject.InvalidateRect();
}

inline void _command_setcolor(HWND hWnd, bool isBack, int id, int index)
{
    if (index == -1)return;
    XMLDATA_STRUCT& xmlData = m_data->arr->at(index);
    DWORD* pcr = 0;
    LPCWSTR lpszCaption = 0;
    if (isBack)
    {
        switch (id)
        {
        case ID_MENU_SETCRBACK2:    pcr = &xmlData.crBack[1]; lpszCaption = L"输入新的热点背景ARGB颜色值"; break;
        case ID_MENU_SETCRBACK3:    pcr = &xmlData.crBack[2]; lpszCaption = L"输入新的按下背景ARGB颜色值"; break;
        case ID_MENU_SETCRBACK4:    pcr = &xmlData.crBack[3]; lpszCaption = L"输入新的禁止背景ARGB颜色值"; break;
        default:                    pcr = &xmlData.crBack[0]; lpszCaption = L"输入新的正常背景ARGB颜色值"; break;
        }
    }
    else
    {
        switch (id)
        {
        case ID_MENU_SETCRTEXT2:    pcr = &xmlData.crText[1]; lpszCaption = L"输入新的热点文本ARGB颜色值"; break;
        case ID_MENU_SETCRTEXT3:    pcr = &xmlData.crText[2]; lpszCaption = L"输入新的按下文本ARGB颜色值"; break;
        case ID_MENU_SETCRTEXT4:    pcr = &xmlData.crText[3]; lpszCaption = L"输入新的禁止文本ARGB颜色值"; break;
        default:                    pcr = &xmlData.crText[0]; lpszCaption = L"输入新的正常文本ARGB颜色值"; break;
        }
    }

    LPWSTR lpszColor = input_Open(hWnd, L"颜色支持10进制/16进制, RGB(r,g,b), ARGB(a,r,g,b)\r\n"\
        "10进制和16进制会直接当成ARGB颜色使用, 16进制必须0x开头\r\n"\
        "RGB() 内部会转成255透明度的ARGB颜色\r\n"\
        "输入不合法的数据会置0",
        lpszCaption, argb2str(*pcr).c_str());
    if (!lpszColor)return;

    DWORD argb = str2argb(lpszColor, 0);
    if (argb != *pcr) ModyfyTitle(true);
    *pcr = argb;
    input_free(lpszColor);
}
bool CALLBACK WndProc_command(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* lpResult)
{
    int id = LOWORD(wParam);
    int code = HIWORD(wParam);
    switch (id)
    {
    case ID_RADIO_100:
    case ID_RADIO_125:
    case ID_RADIO_150:
    case ID_RADIO_175:
    case ID_RADIO_200:
        g_dpiIndex = id - ID_RADIO_100;
        s_listItem.InvalidateRect();
        s_xml_info.SetAttrValue(L"@1", XML_NAME_DPIINDEX, wstr::to_string(g_dpiIndex).c_str());
        break;
    case ID_MENU_SELIMAGEPATH:  // 选择图片素材目录
    {
        CFileDialog_Com file;
        file.createOpen();
        file.SetTitle(L"请选择图片素材目录");
        file.SetOptions(FOS_PICKFOLDERS);
        if (!file.Show(hWnd)) return 0;
        wstr path = file.GetResult();
        if (path.empty()) return 0;

        s_xml_info.SetAttrValue(L"@1", XML_INI_IMAGEPATH, path.c_str());
        init_Image(path.c_str());
        break;
    }
    case ID_MENU_SAVE:
    {
        assist_destroy(false);
        break;
    }
    case ID_MENU_NEWITEM:       // 添加项目
    {
        if (!m_data) return 0;

        LPWSTR text = input_Open(hWnd, L"请输入项目ID, ID必须是唯一, 主题加载器是通过ID加载的", L"输入ID");
        if (!text)return 0;
        _insert_xml_node(text, 0, 0, true);
        ModyfyTitle(true);
        input_free(text);
        break;
    }
    case ID_MENU_NEWITEMS:      // 批量添加项目
    {
        INT_PTR ret = DialogBoxParamW(g_hInst, MAKEINTRESOURCEW(IDDIALOG_NEWITEMS), hWnd, DlgProc_NewItems, 0);
        if (!m_news_str) return 0;
        for (size_t i = 0; i < m_news_name.size(); i++)
            _insert_xml_node(m_news_name[i], m_news_value[i], m_news_remarks[i], false);

        delete[] m_news_str;
        m_news_str = 0;
        m_news_name.clear();
        m_news_value.clear();
        m_news_remarks.clear();
        s_listItem.InvalidateRect();
        s_listProject.InvalidateRect();
        ModyfyTitle(true);
        break;
    }
    case ID_MENU_DELITEM:       // 删除项目
    {
        int index = s_listItem.GetIndex();
        if (!m_data || index == -1) return 0;

        auto it = m_data->arr->begin() + index;
        it->free();
        m_data->arr->erase(it);
        int count = (int)m_data->arr->size();
        s_listItem.SetItemCount(count, true);
        s_listProject.InvalidateRect();

        assert(count == (int)m_data->arr->size());
        ModyfyTitle(true);
        break;
    }
    case ID_MENU_SETID:         // 设置ID
    {
        int index = s_listItem.GetIndex();
        _command_set_id(hWnd, index);
        break;
    }
    case ID_MENU_SETTEXT:       // 设置替换文本
    {
        int index = s_listItem.GetIndex();
        _command_set_text(hWnd, index);
        break;
    }
    case ID_MENU_SETREMARKS:       // 设置说明
    {
        int index = s_listItem.GetIndex();
        _command_set_remarks(hWnd, index);
        break;
    }
    case ID_MENU_SETCRTEXT1:        // 设置4个文本颜色
    case ID_MENU_SETCRTEXT2:
    case ID_MENU_SETCRTEXT3:
    case ID_MENU_SETCRTEXT4:
    {
        int index = s_listItem.GetIndex();
        _command_setcolor(hWnd, false, id, index);
        break;
    }
    case ID_MENU_SETCRBACK1:        // 设置4个背景颜色
    case ID_MENU_SETCRBACK2:
    case ID_MENU_SETCRBACK3:
    case ID_MENU_SETCRBACK4:
    {
        int index = s_listItem.GetIndex();
        _command_setcolor(hWnd, true, id, index);
        break;
    }
    case ID_MENU_RESETITEM:     // 重置项目
    case ID_MENU_RESETITEM_DPI: // 重置项目
    case ID_MENU_RESET_NORMAL:
    case ID_MENU_RESET_HOT:
    case ID_MENU_RESET_DOWN:
    case ID_MENU_RESET_BAN:
    {
        int index = s_listItem.GetIndex();
        if (!m_data || index == -1) return 0;
        int state = -1;
        switch (id)
        {
        case ID_MENU_RESET_NORMAL:  state = STATE::NORMAL;  break;
        case ID_MENU_RESET_HOT:     state = STATE::HOT;     break;
        case ID_MENU_RESET_DOWN:    state = STATE::DOWN;    break;
        case ID_MENU_RESET_BAN:     state = STATE::BAN;     break;
        case ID_MENU_RESETITEM_DPI: state = -2; break;
        default:                    state = -1; break;
        }

        xml_delete_image(m_data, index, state, g_dpiIndex);
        s_listItem.InvalidateItem(index);
        break;
    }
    case ID_MENU_RELOAD_NORMAL:    // 重新从文件中加载
    case ID_MENU_RELOAD_HOT:
    case ID_MENU_RELOAD_DOWN:
    case ID_MENU_RELOAD_BAN:
    {
        int index = s_listItem.GetIndex();
        if (!m_data || index == -1) return 0;
        STATE state;
        switch (id)
        {
        case ID_MENU_RESET_HOT:     state = STATE::HOT;     break;
        case ID_MENU_RESET_DOWN:    state = STATE::DOWN;    break;
        case ID_MENU_RESET_BAN:     state = STATE::BAN;     break;
        default:                    state = STATE::NORMAL;  break;
        }
        XMLDATA_STRUCT& xmlData = m_data->arr->at(index);
        xml_setimage_filename(m_data, index, xmlData.file[0], state, g_dpiIndex);
        s_listItem.InvalidateItem(index);
        break;
    }
    case ID_MENU_100_SETNORMAL:     // 设置正常状态图片
    case ID_MENU_100_SETHOT:        // 设置点燃状态图片
    case ID_MENU_100_SETDOWN:       // 设置按下状态图片
    case ID_MENU_100_SETBAN:        // 设置禁止状态图片
    //case ID_MENU_125_SETNORMAL:     // 设置正常状态图片
    //case ID_MENU_125_SETHOT:        // 设置点燃状态图片
    //case ID_MENU_125_SETDOWN:       // 设置按下状态图片
    //case ID_MENU_125_SETBAN:        // 设置禁止状态图片
    //case ID_MENU_150_SETNORMAL:     // 设置正常状态图片
    //case ID_MENU_150_SETHOT:        // 设置点燃状态图片
    //case ID_MENU_150_SETDOWN:       // 设置按下状态图片
    //case ID_MENU_150_SETBAN:        // 设置禁止状态图片
    //case ID_MENU_175_SETNORMAL:     // 设置正常状态图片
    //case ID_MENU_175_SETHOT:        // 设置点燃状态图片
    //case ID_MENU_175_SETDOWN:       // 设置按下状态图片
    //case ID_MENU_175_SETBAN:        // 设置禁止状态图片
    //case ID_MENU_200_SETNORMAL:     // 设置正常状态图片
    //case ID_MENU_200_SETHOT:        // 设置点燃状态图片
    //case ID_MENU_200_SETDOWN:       // 设置按下状态图片
    //case ID_MENU_200_SETBAN:        // 设置禁止状态图片
    {
        _listitem_command_setimage(hWnd, id);
        break;
    }
    case ID_MENU_100_SETNORMAL1:    // 设置正常状态图片
    case ID_MENU_100_SETHOT1:       // 设置点燃状态图片
    case ID_MENU_100_SETDOWN1:      // 设置按下状态图片
    case ID_MENU_100_SETBAN1:       // 设置禁止状态图片
    case ID_MENU_125_SETNORMAL1:    // 设置正常状态图片
    case ID_MENU_125_SETHOT1:       // 设置点燃状态图片
    case ID_MENU_125_SETDOWN1:      // 设置按下状态图片
    case ID_MENU_125_SETBAN1:       // 设置禁止状态图片
    case ID_MENU_150_SETNORMAL1:    // 设置正常状态图片
    case ID_MENU_150_SETHOT1:       // 设置点燃状态图片
    case ID_MENU_150_SETDOWN1:      // 设置按下状态图片
    case ID_MENU_150_SETBAN1:       // 设置禁止状态图片
    case ID_MENU_175_SETNORMAL1:    // 设置正常状态图片
    case ID_MENU_175_SETHOT1:       // 设置点燃状态图片
    case ID_MENU_175_SETDOWN1:      // 设置按下状态图片
    case ID_MENU_175_SETBAN1:       // 设置禁止状态图片
    case ID_MENU_200_SETNORMAL1:    // 设置正常状态图片
    case ID_MENU_200_SETHOT1:       // 设置点燃状态图片
    case ID_MENU_200_SETDOWN1:      // 设置按下状态图片
    case ID_MENU_200_SETBAN1:       // 设置禁止状态图片
    {
        const int index = s_listItem.GetIndex();
        const int indexImage = s_listImage.GetIndex();
        int dpiIndex = HIBYTE(id) - 1;
        if (dpiIndex < 0 || dpiIndex > 4)
            dpiIndex = 0;
        if (index == -1 || indexImage == -1)return 0;
        int state = LOBYTE(id);
        switch (state)
        {
        case 1:     state = STATE::HOT;     break;
        case 2:     state = STATE::DOWN;    break;
        case 3:     state = STATE::BAN;     break;
        default:    state = STATE::NORMAL;  break;
        }

        SHOWIMAGEDATA& imageData = s_dataImage[indexImage];
        xml_setimage_filename(m_data, index, imageData.file.c_str(), (STATE)state, g_dpiIndex);
        s_listItem.InvalidateRect();
        break;
    }
    case ID_MENU_NEWTHEME:      // 新建主题
    {
        LPWSTR text = input_Open(hWnd, L"请输入主题名字, 不允许有同名的主题", L"输入主题名字");
        if (!text)return 0;
        if (isCreatePath(text))
        {
            input_free(text);
            MessageBoxW(hWnd, L"已经有同名的工程, 创建失败, 请重新创建", L"错误", MB_OK);
            return 0;
        }

        PROJECT_DATA data(text);
        s_data.push_back(std::move(data));
        tinyxml2::XMLNode* node;
        g_xmlProject.InsertChildNode(L"@1", L"Item", 0, false, &node);
        g_xmlProject.InsertAttr(node, L"name", text);
        g_xmlProject.SaveFile();

        s_listProject.SetItemCount((int)s_data.size(), true);
        input_free(text);
        ModyfyTitle(true);
        break;
    }
    case ID_MENU_DELTHEME:      // 删除主题
    {
        int index = s_listProject.GetIndex();
        eraseItem(index);
        g_xmlProject.SaveFile();
        s_listProject.SetItemCount((int)s_data.size(), true);
        ModyfyTitle(true);
        break;
    }
    case ID_MENU_CLEAR_CACHE:   // 清除缓存
    {
        for (auto it = s_data.begin(); it != s_data.end(); ++it)
        {
            PROJECT_DATA& data = *it;
            wstr xmlFile, projectPath, pngFile;
            isCreatePath(data.xmlName, &projectPath, &xmlFile, &pngFile);
            DeleteFileW(pngFile.c_str());
            DeleteBitmapFromMap(pngFile.c_str());
        }
        s_listItem.InvalidateRect();
        break;
    }
    case ID_MENU_SETSIZE:
    {
        if (!m_data)return 0;
        wchar_t strIcon[100];
        swprintf_s(strIcon, 100, L"%d,%d", m_data->maxWidth, m_data->maxHeight);
        LPWSTR text = input_Open(hWnd, L"输入主题尺寸, 格式为 \"宽度,高度\" 比如 16,16 ", L"输入主题尺寸", strIcon);
        if (!text)return 0;

        swscanf_s(text, L"%d,%d", &m_data->maxWidth, &m_data->maxHeight);
        input_free(text);

        s_listItem.SetItemHeight(m_data->maxWidth + 10);
        s_listItem.InvalidateRect();
        ModyfyTitle(true);
        break;
    }
    case ID_MENU_MAKETHEME:
    {
        _save_make_savepath();
        break;
    }
    case ID_MENU_LOCATIONPROJECT:   // 打开目录
    {
        if (!m_data)return false;

        wstr xmlFile;
        isCreatePath(m_data->xmlName, 0, &xmlFile);

        LPITEMIDLIST pList = ILCreateFromPathW(xmlFile.c_str());
        if (!pList) break;
        HRESULT hr = SHOpenFolderAndSelectItems(pList, 0, 0, 0);
        if (FAILED(hr))
        {
            wchar_t text[520];
            swprintf_s(text, 520, L"打开工程目录失败, 当前取到的路径 = %s", xmlFile.c_str());
            MessageBoxW(hWnd, text, L"错误", 0);
        }
        SHFree(pList);

        break;
    }
    case ID_MENU_LOCATIONITEM:      // 打开素材目录
    {
        if (!m_data)return false;

        const int index = s_listItem.GetIndex();
        XMLDATA_STRUCT& xmlData = m_data->arr->at(index);
        LPCWSTR file;
        for (int i = 0; i < sizeof(xmlData.file) / sizeof(xmlData.file[0]); i++)
        {
            file = xmlData.file[i];
            if (file) break;
        }

        LPITEMIDLIST pList = ILCreateFromPathW(file);
        if (!pList) break;
        HRESULT hr = SHOpenFolderAndSelectItems(pList, 0, 0, 0);
        if (FAILED(hr))
        {
            wchar_t text[520];
            swprintf_s(text, 520, L"打开素材目录失败, 当前取到的路径 = %s", file);
            MessageBoxW(hWnd, text, L"错误", 0);
        }
        SHFree(pList);
        break;
    }
    case ID_MENU_LOCATIONITEM1:     // 打开素材目录
    {
        const int index = s_listImage.GetIndex();
        if (index == -1)return false;

        SHOWIMAGEDATA& data = s_dataImage[index];
        LPCWSTR file = data.file.c_str();

        LPITEMIDLIST pList = ILCreateFromPathW(file);
        if (!pList) break;
        HRESULT hr = SHOpenFolderAndSelectItems(pList, 0, 0, 0);
        if (FAILED(hr))
        {
            wchar_t text[520];
            swprintf_s(text, 520, L"打开素材目录失败, 当前取到的路径 = %s", file);
            MessageBoxW(hWnd, text, L"错误", 0);
        }
        SHFree(pList);
        break;
    }
    case ID_MENU_BLACK:
    case ID_MENU_WHITE:
    {
        if (!m_data)return false;
        if (id == ID_MENU_BLACK)
        {
            g_color.crBack          = RGB(45, 45, 48);
            g_color.crHot           = RGB(55, 55, 58);
            g_color.crDown          = RGB(65, 65, 68);
            g_color.crText          = RGB(222, 222, 222);
            g_color.crTextID        = RGB(222, 89, 178);
            g_color.crTextRemarks   = RGB(80, 161, 60);
            g_color.crTextProject   = RGB(61, 191, 176);
            m_data->themeIndex = 0;
        }
        else
        {
            g_color.crBack          = RGB(238, 238, 242);
            g_color.crHot           = RGB(225, 225, 228);
            g_color.crDown          = RGB(210, 210, 213);
            g_color.crText          = RGB(30, 30, 30);
            g_color.crTextID        = RGB(222, 89, 178);
            g_color.crTextRemarks   = RGB(80, 161, 60);
            g_color.crTextProject   = RGB(61, 191, 176);
            m_data->themeIndex = 1;
        }
        if (s_hbrBack)
            DeleteObject(s_hbrBack);
        s_hbrBack = CreateSolidBrush(g_color.crBack);

        s_listProject.SetBkColor(g_color.crBack);
        s_listItem.SetBkColor(g_color.crBack);
        s_listImage.SetBkColor(g_color.crBack);

        s_listProject.InvalidateRect();
        s_listItem.InvalidateRect();
        s_listImage.InvalidateRect();
        break;
    }
    default:
        return false;
    }
    return true;
}

INT_PTR CALLBACK DlgProc_NewItems(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CTLCOLORMSGBOX:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
        return (INT_PTR)GetStockObject(WHITE_BRUSH);
    case WM_CLOSE:
        EndDialog(hDlg, 0);
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps = { 0 };
        BeginPaint(hDlg, &ps);
        RECT rc = { 0 };
        GetClientRect(hDlg, &rc);
        FillRect(ps.hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
        EndPaint(hDlg, &ps);
        break;
    }
    case WM_COMMAND:
    {
        const int id = LOWORD(wParam);
        const int code = HIWORD(wParam);
        switch (id)
        {
        case IDOK:
        {
            HWND hEdit = GetDlgItem(hDlg, IDC_EDIT1);
            const int len = GetWindowTextLengthW(hEdit) + 1;
            m_news_str = new wchar_t[len];
            memset(m_news_str, 0, sizeof(wchar_t) * len);
            GetWindowTextW(hEdit, m_news_str, len);
            wchar_t* next_token = 0;
            wchar_t* ptr = wcstok_api(m_news_str, L"\r\n", &next_token);

            // 一行一个, id和说明之间使用英文逗号分隔
            // id,说明,修改的文本
            while (ptr)
            {
                wchar_t* pos = wcschr(ptr, ',');
                if (pos)*pos++ = 0;
                m_news_name.push_back(ptr);     // 最左边是id
                m_news_remarks.push_back(pos);  // 逗号后面是说明, 说明就是备注
                if (pos)
                {
                    pos = wcschr(pos, ',');
                    if (pos)*pos++ = 0;
                }
                m_news_value.push_back(pos);    // 在后面才是值
                ptr = wcstok_api(0, L"\r\n", &next_token);
            }
            EndDialog(hDlg, (INT_PTR)m_news_str);
            break;
        }
        case IDCANCEL:
        {
            EndDialog(hDlg, 0);
            break;
        }
        default:
            return (INT_PTR)FALSE;
        }
        break;
    }
    default:
        return (INT_PTR)FALSE;
    }
    return (INT_PTR)TRUE;
}


void CALLBACK CustomList_ItemRClick(HWND hWnd, int index)
{
    const int themeIndex = m_data ? m_data->themeIndex : 0;

    if (hWnd == s_listProject)
    {
    //    int index = s_listProject.GetIndex();
    //    s_menuFile.enabled(ID_MENU_DELTHEME, (index == -1) ? 0 : 1);
    //    wstr sizename(100);
    //    sizename.assign(L"设置尺寸(&S)");
    //    if (m_data)
    //    {
    //        wchar_t maxSize[100], imgSize[100];
    //        swprintf_s(maxSize, 100, L"%d,%d", m_data->maxWidth, m_data->maxHeight);
    //        swprintf_s(imgSize, 100, L"%d,%d", m_data->imgWidth, m_data->imgHeight);
    //        sizename.append(L"\t最大图标尺寸 = ").append(maxSize);
    //        sizename.append(L", 图片集合尺寸 = ").append(imgSize);
    //    }

    //    s_menuFile.SetTitle(ID_MENU_SETSIZE, sizename.c_str());
        s_menuFile.SetCheck(ID_MENU_BLACK, themeIndex == 0);
        s_menuFile.SetCheck(ID_MENU_WHITE, themeIndex == 1);
        s_menuFile.pop();
    }
    else if (hWnd == s_listItem)
    {
        // 显示项目数据的列表
        const int index = s_listItem.GetIndex();
        bool isEnabled = (index > -1);
        bool isEnabledLocation = false;
        if (m_data)
        {
            const int index = s_listItem.GetIndex();
            if (index != -1 && index < (int)m_data->arr->size())
            {
                XMLDATA_STRUCT& xmlData = m_data->arr->at(index);
                LPCWSTR file;
                for (int i = 0; i < sizeof(xmlData.file) / sizeof(xmlData.file[0]); i++)
                {
                    file = xmlData.file[i];
                    if (file) break;
                }
                isEnabledLocation = file != 0;
            }
        }

        s_menu.enabled(ID_MENU_DELITEM,         isEnabled);
        s_menu.enabled(ID_MENU_RESET_NORMAL,    isEnabled);     // 重置项目
        s_menu.enabled(ID_MENU_RESET_HOT,       isEnabled);
        s_menu.enabled(ID_MENU_RESET_DOWN,      isEnabled);
        s_menu.enabled(ID_MENU_RESET_BAN,       isEnabled);
        s_menu.enabled(ID_MENU_RESETITEM,       isEnabled);

        s_menu.enabled(ID_MENU_RELOAD_NORMAL,   isEnabled);     // 重新从文件加载
        s_menu.enabled(ID_MENU_RELOAD_HOT,      isEnabled);
        s_menu.enabled(ID_MENU_RELOAD_DOWN,     isEnabled);
        s_menu.enabled(ID_MENU_RELOAD_BAN,      isEnabled);

        s_menu.enabled(ID_MENU_SETID,           isEnabled);
        s_menu.enabled(ID_MENU_SETTEXT,         isEnabled);
        s_menu.enabled(ID_MENU_SETREMARKS,      isEnabled);

        s_menu.enabled(ID_MENU_100_SETNORMAL,       isEnabled);     // 设置状态图片
        s_menu.enabled(ID_MENU_100_SETHOT,          isEnabled);
        s_menu.enabled(ID_MENU_100_SETDOWN,         isEnabled);
        s_menu.enabled(ID_MENU_100_SETBAN,          isEnabled);

        //s_menu.enabled(ID_MENU_125_SETNORMAL,       isEnabled);     // 设置状态图片
        //s_menu.enabled(ID_MENU_125_SETHOT,          isEnabled);
        //s_menu.enabled(ID_MENU_125_SETDOWN,         isEnabled);
        //s_menu.enabled(ID_MENU_125_SETBAN,          isEnabled);

        //s_menu.enabled(ID_MENU_150_SETNORMAL,       isEnabled);     // 设置状态图片
        //s_menu.enabled(ID_MENU_150_SETHOT,          isEnabled);
        //s_menu.enabled(ID_MENU_150_SETDOWN,         isEnabled);
        //s_menu.enabled(ID_MENU_150_SETBAN,          isEnabled);

        //s_menu.enabled(ID_MENU_175_SETNORMAL,       isEnabled);     // 设置状态图片
        //s_menu.enabled(ID_MENU_175_SETHOT,          isEnabled);
        //s_menu.enabled(ID_MENU_175_SETDOWN,         isEnabled);
        //s_menu.enabled(ID_MENU_175_SETBAN,          isEnabled);

        //s_menu.enabled(ID_MENU_200_SETNORMAL,       isEnabled);     // 设置状态图片
        //s_menu.enabled(ID_MENU_200_SETHOT,          isEnabled);
        //s_menu.enabled(ID_MENU_200_SETDOWN,         isEnabled);
        //s_menu.enabled(ID_MENU_200_SETBAN,          isEnabled);

        s_menu.enabled(ID_MENU_SETCRTEXT1,      isEnabled);     // 设置文本颜色
        s_menu.enabled(ID_MENU_SETCRTEXT2,      isEnabled);
        s_menu.enabled(ID_MENU_SETCRTEXT3,      isEnabled);
        s_menu.enabled(ID_MENU_SETCRTEXT4,      isEnabled);

        s_menu.enabled(ID_MENU_SETCRBACK1,      isEnabled);     // 设置背景颜色
        s_menu.enabled(ID_MENU_SETCRBACK2,      isEnabled);
        s_menu.enabled(ID_MENU_SETCRBACK3,      isEnabled);
        s_menu.enabled(ID_MENU_SETCRBACK4,      isEnabled);

        s_menu.enabled(6,   isEnabled, true);   // 重置
        s_menu.enabled(7,   isEnabled, true);   // 重新从路径加载
        s_menu.enabled(8,   isEnabled, true);   // 设置状态图片
        s_menu.enabled(13,  isEnabled, true);   // 设置文本颜色
        s_menu.enabled(14,  isEnabled, true);   // 设置背景颜色

        s_menu.enabled(ID_MENU_LOCATIONITEM,    isEnabledLocation);

        s_menu.SetCheck(ID_MENU_BLACK, themeIndex == 0);
        s_menu.SetCheck(ID_MENU_WHITE, themeIndex == 1);
        s_menu.pop();
    }
    else if (hWnd == s_listImage)
    {
        // 显示图片素材的列表
        const int indexImage = s_listImage.GetIndex();
        bool isEnabled = (indexImage > -1);
        
        s_menuImage.enabled(ID_MENU_100_SETNORMAL1, isEnabled);
        s_menuImage.enabled(ID_MENU_100_SETHOT1,    isEnabled);
        s_menuImage.enabled(ID_MENU_100_SETDOWN1,   isEnabled);
        s_menuImage.enabled(ID_MENU_100_SETBAN1,    isEnabled);

        s_menuImage.enabled(ID_MENU_125_SETNORMAL1, isEnabled);
        s_menuImage.enabled(ID_MENU_125_SETHOT1,    isEnabled);
        s_menuImage.enabled(ID_MENU_125_SETDOWN1,   isEnabled);
        s_menuImage.enabled(ID_MENU_125_SETBAN1,    isEnabled);

        s_menuImage.enabled(ID_MENU_150_SETNORMAL1, isEnabled);
        s_menuImage.enabled(ID_MENU_150_SETHOT1,    isEnabled);
        s_menuImage.enabled(ID_MENU_150_SETDOWN1,   isEnabled);
        s_menuImage.enabled(ID_MENU_150_SETBAN1,    isEnabled);

        s_menuImage.enabled(ID_MENU_175_SETNORMAL1, isEnabled);
        s_menuImage.enabled(ID_MENU_175_SETHOT1,    isEnabled);
        s_menuImage.enabled(ID_MENU_175_SETDOWN1,   isEnabled);
        s_menuImage.enabled(ID_MENU_175_SETBAN1,    isEnabled);

        s_menuImage.enabled(ID_MENU_200_SETNORMAL1, isEnabled);
        s_menuImage.enabled(ID_MENU_200_SETHOT1,    isEnabled);
        s_menuImage.enabled(ID_MENU_200_SETDOWN1,   isEnabled);
        s_menuImage.enabled(ID_MENU_200_SETBAN1,    isEnabled);

        s_menuImage.SetCheck(ID_MENU_BLACK, themeIndex == 0);
        s_menuImage.SetCheck(ID_MENU_WHITE, themeIndex == 1);
        
        s_menuImage.pop();
    }

}


void CALLBACK CustomList_ItemClick(HWND hWnd, int index, bool isDblClk)
{
    if (index == -1) return;
    if (hWnd == s_listImage)
    {
        int indexEil = s_listItem.GetIndex();
        if (!m_data || index == -1) return;

        SHOWIMAGEDATA& data = s_dataImage[index];

        xml_setimage_filename(m_data, indexEil, data.file.c_str(), STATE::NORMAL, g_dpiIndex);
        s_listItem.InvalidateItem(indexEil);
        return;
    }
    if (hWnd == s_listItem)
    {
        if (!m_data)return;
        // 显示项目图片的列表框
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(hWnd, &pt);

        // 先处理双击ID的问题
        XMLDATA_STRUCT& xmlData = m_data->arr->at(index);
        if (pt.x >= xmlData.left && pt.x < xmlData.right)   // 双击在了id中
        {
            _command_set_id(hWnd, index);
            return;
        }
        if (pt.x > xmlData.right)  // 双击在了说明中
        {
            _command_set_remarks(hWnd, index);
            return;
        }

        const int offet = 10;
        const int iconLeft[] =
        {
            34 + (m_data->maxWidth + offet) * 1,
            34 + (m_data->maxWidth + offet) * 2,
            34 + (m_data->maxWidth + offet) * 3,
        };
        int id;
        if (pt.x >= iconLeft[0] && pt.x < iconLeft[0] + m_data->maxWidth + offet)   // 双击在第二个图标里
        {
            id = ID_MENU_100_SETHOT;
        }
        else if (pt.x >= iconLeft[1] && pt.x < iconLeft[1] + m_data->maxWidth + offet)  // 双击在第三个图标里
        {
            id = ID_MENU_100_SETDOWN;
        }
        else if (pt.x >= iconLeft[2] && pt.x < iconLeft[2] + m_data->maxWidth + offet)  // 双击在第四个图标里
        {
            id = ID_MENU_100_SETBAN;
        }
        else
        {
            // 其他情况都视为双击在第一个图标里
            id = ID_MENU_100_SETNORMAL;
        }
        _listitem_command_setimage(hWnd, id);
        return;
    }
}



void _save_make_savepath()
{
    int index = s_listProject.GetIndex();
    if (index == -1) return;
    PROJECT_DATA& data = s_data[index];
    if (data.arr->empty())
    {
        MessageBoxW(0, L"没有数据, 生成主题失败", 0, 0);
        return;
    }
    CFileDialog_Com file;
    wstr saveFileName = file.save(0, L"皮肤数据(*.ires)\0*.ires\0所有文件(*.*)\0*.*\0", 0, L"请选择保存路径", data.xmlName, L"ires", L"保存的文件名");
    if (saveFileName.empty()) return;

    XMLDATA_STRUCT& xmlData = data.arr->front();
    xml_setimage_filename(&data, 0, xmlData.file[0], STATE::NORMAL, 0); // 生成前先重新生成图片

    _save_make_xml_png(data, saveFileName.c_str());

    assist_destroy(false);  // 保存, 把标题的星号去掉, 把内存的数据写到磁盘上
}

inline static RECT& str2rc(LPCWSTR str, RECT& rc)
{
    if (!str || !str[0]) str = L"0,0,0,0";
    swscanf_s(str, L"%d,%d,%d,%d", &rc.left, &rc.top, &rc.right, &rc.bottom);
    return rc;
}

inline static LPCWSTR rc2str(RECT& rc)
{
    static wchar_t str[100] = { 0 };
    swprintf_s(str, 100, L"%d,%d,%d,%d", rc.left, rc.top, rc.right, rc.bottom);
    return str;
}


inline void _save_load_image(PROJECT_DATA& data, _str& pngData)
{
    wstr pngFile;
    isCreatePath(data.xmlName, 0, 0, &pngFile);

#if 1
    // 读取进来利用D2D保存, 得到的是最新的数据, 不会因为被ps过而导致图片变大的情况
    LPEX_IMAGE hImg = _img_create_fromfile(pngFile.c_str());
    SIZE_T size = 0;
    LPBYTE ptr = _img_save(hImg, &size);
    pngData.assign((LPCSTR)ptr, size);
    free(ptr);
    _img_destroy(hImg);
#else
    // 直接读入文件, 如果文件被ps修改过, 那就会导致图片变大
    FILE* f;
    errno_t err = _wfopen_s(&f, pngFile.c_str(), L"rb+");
    if (!f || err != 0)
    {
        wstr text;
        text.Format(L"生成主题文件失败\n无法打开[%s]图片文件, 读取图片数据失败\n错误码 = %d", pngFile.c_str(), err);
        return;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    pngData.resize(size);
    fread(pngData.data(), 1, size, f);
    fclose(f);
#endif
}
void _save_make_xml_png(PROJECT_DATA& data, LPCWSTR saveFileName)
{
    std::vector<XMLDATA_STRUCT>& arr = *data.arr;
    const int nItemCount = (int)arr.size();
    if (nItemCount == 0)
    {
        MessageBoxW(g_hWnd, L"项目数为0, 生成文件失败", L"错误", 0);
        return;
    }
    _str pngData;
    _save_load_image(data, pngData);    // 读取图片, 这个图片就是要打包的图片

    //////////////////////////////////////////////////////////////////////////
    // 创建xml, 写入图片总尺寸, 最大图片宽高, 打包时是根据这个xml进行序列号打包
    // 打包的文件头会记录xml根节点的 maxSize 和 imgSize 这两个属性
    wchar_t maxSize[50], imgSize[50];
    CTinyXml2W xml;
    xml.Create(L"Theme");
    swprintf_s(maxSize, 50, L"%d,%d", data.maxWidth, data.maxHeight);
    swprintf_s(imgSize, 50, L"%d,%d", data.imgWidth, data.imgHeight);
    xml.SetAttrValue(L"@1", L"maxSize", maxSize);
    xml.SetAttrValue(L"@1", L"imgSize", imgSize);
    //////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////
    // 把所有成员的数据写到xml里
    wstr _name(50);
    for (auto it = arr.begin(); it != arr.end(); ++it)
    {
        XMLDATA_STRUCT& xmlData = *it;
        tinyxml2::XMLNode* node;
        xml.InsertChildNode(L"@1", L"Item", xmlData.remarks, false, &node);
        assert(node);
        xml.SetAttrValue(node, L"id",       xmlData.id);
        xml.SetAttrValue(node, L"text",     xmlData.text);

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


    const wchar_t* xmlText = xml.GetXmlData();
    SaveExt(saveFileName, pngData.c_str(), pngData.size(), xmlText);

}


// 生成的文件结构 = THEME_FILE_HEADER + 压缩(图片数据尺寸 + 图片数据 + xml序列化数据尺寸 + xml序列化数据)
void SaveExt(const wchar_t* saveFileName, const void* pngData, DWORD pngDataSize, const wchar_t* xmlText, _str* pCompressData)
{
    _str extData;
    THEME_NAMESPACE EXT_ERR extErr = THEME_NAMESPACE PackExtData(pngData, pngDataSize, xmlText, &extData);

    if (extErr != THEME_NAMESPACE EXT_ERR::EXT_ERR_OK)
    {
        LPCWSTR errText = THEME_NAMESPACE GetErrTextW(extErr);
        MessageBoxW(g_hWnd, errText, L"保存主题失败", 0);
        return;
    }

    FILE* f;
    errno_t err = _wfopen_s(&f, saveFileName, L"wb+");
    if (!f || err != 0)
    {
        wstr text;
        text.Format(L"生成主题文件失败\n无法打开[%s]文件, 写出数据失败\n错误码 = %d", saveFileName, err);
    }
    if (f)
    {
        fwrite(extData.c_str(), 1, extData.size(), f);
        fclose(f);
    }
    if (pCompressData)pCompressData->swap(extData);

#ifdef _DEBUG   // 调试版会把图片数据和xml写入生成主题的对应目录下, 方便查看最终到底写出了什么数据
    {
        wstr pngFileName(260), xmlFileName(260);
        LPCWSTR pos = wcsrchr(saveFileName, '.');

        pngFileName.assign(saveFileName, pos - saveFileName + 1);
        pngFileName.append(L"png");
        err = _wfopen_s(&f, pngFileName.c_str(), L"wb+");
        assert(f && err == 0);
        if (f)
        {
            fwrite(pngData, 1, pngDataSize, f);
            fclose(f);
        }

        xmlFileName.assign(saveFileName, pos - saveFileName + 1);
        xmlFileName.append(L"xml");
        err = _wfopen_s(&f, xmlFileName.c_str(), L"wb+");
        assert(f && err == 0);
        if (f)
        {
            _str xmlTextA = wstr::W2A(xmlText);
            fwrite(xmlTextA.c_str(), 1, xmlTextA.size(), f);
            fclose(f);
        }
    }
#endif
}
