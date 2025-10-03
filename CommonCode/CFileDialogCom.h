#pragma once
#include <comdef.h>
#include <Shlobj.h>
#include <tstr.h>

class CFileDialog_Com
{
private:
    IFileDialog* m_pObj;    // 打开对话框的对象指针
    HRESULT hr;             // 最后错误, 每走一步都记录一次

public:
    CFileDialog_Com() :m_pObj(0), hr(0)
    {
        hr = CoInitialize(nullptr);
    }
    ~CFileDialog_Com() { destroy(); }

public:
    // 创建打开文件对象
    inline bool createOpen() { return create(true); }
    // 创建保存文件对象
    inline bool createSave() { return create(false); }

    // 创建文件对话框对象
    inline bool create(bool isOpen)
    {
        destroy();
        CLSID clsid, iid;
        if (isOpen)
        {
            clsid = CLSID_FileOpenDialog;
            iid = __uuidof(IFileOpenDialog);
        }
        else
        {
            clsid = CLSID_FileSaveDialog;
            iid = __uuidof(IFileSaveDialog);
        }
        hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, iid, IID_PPV_ARGS_Helper(&m_pObj));
        bool ret = SUCCEEDED(hr);
        if (ret) m_pObj->SetFileTypeIndex(1);
        return ret;
    }
    inline void destroy()
    {
        SafeRelease(m_pObj);
        hr = 0;
    }
    // 取标志
    inline DWORD GetOptions()
    {
        if (!m_pObj)return 0;
        FILEOPENDIALOGOPTIONS ret;
        hr = m_pObj->GetOptions(&ret);
        return ret;
    }
    // 置标志, 返回原来的标志
    inline DWORD SetOptions(DWORD flags)
    {
        if (!m_pObj)return 0;
        FILEOPENDIALOGOPTIONS ret;
        hr = m_pObj->GetOptions(&ret);
        hr = m_pObj->SetOptions(ret | flags);
        return ret;
    }
    // 取标志, 此方法功能和 GetOptions() 一模一样
    inline DWORD GetFlags()
    {
        return GetOptions();
    }
    // 置标志, 返回原来的标志, 此方法功能和 SetOptions() 一模一样
    inline DWORD SetFlags(DWORD flags)
    {
        return SetOptions(flags);
    }

    // 设置默认目录
    inline bool SetDefaultFolder(LPCWSTR path)
    {
        if (!m_pObj)
            return false;
        IShellItem* pCurFolder = NULL;
        HRESULT hr = SHCreateItemFromParsingName(path, NULL, IID_PPV_ARGS(&pCurFolder));
        if (FAILED(hr))
            return false;
        
        hr = m_pObj->SetDefaultFolder(pCurFolder);
        pCurFolder->Release();
        return SUCCEEDED(hr);
    }
    // 设置目录
    inline bool SetFolder(LPCWSTR path)
    {
        if (!m_pObj)
            return false;
        IShellItem* pCurFolder = NULL;
        HRESULT hr = SHCreateItemFromParsingName(path, NULL, IID_PPV_ARGS(&pCurFolder));
        if (FAILED(hr))
            return false;
        
        hr = m_pObj->SetFolder(pCurFolder);
        pCurFolder->Release();
        return SUCCEEDED(hr);
    }
    // 获取当前选择的文件夹, 如果当前未显示对话框, 则获取打开对话框时要选择的文件夹
    inline wstr GetFolder(LPCWSTR path)
    {
        wstr ret;
        if (!m_pObj)
            return ret;
        IShellItem* pCurFolder = NULL;
        HRESULT hr = m_pObj->GetFolder(&pCurFolder);
        if (FAILED(hr))
            return ret;
        PWSTR pszPath = NULL;
        hr = pCurFolder->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);
        if (SUCCEEDED(hr))
        {
            ret = pszPath;
            CoTaskMemFree(pszPath);
        }
        pCurFolder->Release();
        return ret;
    }
    // 置默认文件名
    inline bool SetFileName(const wchar_t* file)
    {
        if (!m_pObj)return 0;
        hr = m_pObj->SetFileName(file);
        return SUCCEEDED(hr);
    }
    // 置对话框标题
    inline bool SetTitle(const wchar_t* title)
    {
        if (!m_pObj)return 0;
        hr = m_pObj->SetTitle(title);
        return SUCCEEDED(hr);
    }
    // 置标签文本
    inline bool SetFileNameLabel(const wchar_t* text)
    {
        if (!m_pObj)return 0;
        hr = m_pObj->SetFileNameLabel(text);
        return SUCCEEDED(hr);
    }
    // 置按钮文本
    inline bool SetOkButtonLabel(const wchar_t* text)
    {
        if (!m_pObj)return 0;
        hr = m_pObj->SetOkButtonLabel(text);
        return SUCCEEDED(hr);
    }
    // 置按钮文本
    inline bool Show(HWND hWndParent = 0)
    {
        if (!m_pObj)return 0;
        hr = m_pObj->Show(hWndParent);
        return SUCCEEDED(hr);
    }

    // 置过滤器, 每个过滤器使用 \0 分隔, 内部调用 SetFilter() 实现
    // filter = 过滤器, 格式："MP3文件(*.mp3)\0*.mp3\0媒体文件(*.mpg)\0*.mpg\0\0"
    inline bool SetFileTypes(const wchar_t* filter, UINT filterIndex = 0)
    {
        return SetFilter(filter, filterIndex);
    }
    // 设置过滤器, 易语言模式, 每个过滤器使用 | 分隔, 内部调用 SetFilterEMode() 实现
    // filter = 过滤器, 格式："MP3文件(*.mp3)|*.mp3|媒体文件(*.mpg)|*.mpg"
    inline bool SetFileTypesEMode(const wchar_t* filter, UINT filterIndex = 0)
    {
        return SetFilterEMode(filter, filterIndex);
    }

    // 设置过滤器, 每个过滤器使用 \0 分隔
    // filter = 过滤器, 格式："MP3文件(*.mp3)\0*.mp3\0媒体文件(*.mpg)\0*.mpg\0\0"
    // filterIndex = 默认过滤器, 从0开始, 默认为0
    inline bool SetFilter(const wchar_t* filter, UINT filterIndex = 0)
    {
        if (!m_pObj || !filter) return 0;

        _str arrBuf(sizeof(const wchar_t*) * 100);
        int i = 1;
        const wchar_t* p = filter;
        arrBuf.assign((LPCSTR)&p, sizeof(const wchar_t*));
        while (1)
        {
            if (*p++ == 0)    // 逐个字节判断字符串, 遇到\0就是一段
            {
                if (*p++ == 0) break;    // \0\0 就是结尾
                ++i;
                --p;
                arrBuf.append((LPCSTR)&p, sizeof(const wchar_t*));
                ++p;
            }
        }
        hr = m_pObj->SetFileTypes(i / 2, (COMDLG_FILTERSPEC*)arrBuf.c_str());    // 设置过滤器
        if (SUCCEEDED(hr)) m_pObj->SetFileTypeIndex(filterIndex + 1);
        return SUCCEEDED(hr);
    }

    // 设置过滤器, 易语言模式, 每个过滤器使用 | 分隔
    // filter = 过滤器, 格式："MP3文件(*.mp3)|*.mp3|媒体文件(*.mpg)|*.mpg"
    // filterIndex = 默认过滤器, 从0开始, 默认为0
    inline bool SetFilterEMode(const wchar_t* filter, UINT filterIndex = 0)
    {
        if (!m_pObj || !filter) return 0;
        wstr buf(filter);
        _str arrBuf(sizeof(const wchar_t*) * 100);
        int i = 1;
        const wchar_t* p = buf.c_str();
        arrBuf.assign((LPCSTR)&p, sizeof(const wchar_t*));

        while (*p)
        {
            if (*p++ == '|')    // 逐个字节判断字符串, 遇到 | 就是一段
            {
                ++i;
                --p;
                arrBuf.append((LPCSTR)&p, sizeof(const wchar_t*));
                ++p;
            }
        }
        hr = m_pObj->SetFileTypes(i / 2, (COMDLG_FILTERSPEC*)arrBuf.c_str());    // 设置过滤器
        if (SUCCEEDED(hr)) m_pObj->SetFileTypeIndex(filterIndex + 1);
        return SUCCEEDED(hr);
    }
    // 置默认扩展名
    inline bool SetDefaultExtension(const wchar_t* text)
    {
        if (!m_pObj)return 0;
        hr = m_pObj->SetDefaultExtension(text);
        return SUCCEEDED(hr);
    }
    // 取选中文件, 内部调用 GetResult() 实现
    inline wstr GetSelectFile(SIGDN sigdnName = SIGDN_DESKTOPABSOLUTEPARSING)
    {
        return GetResult(sigdnName);
    }
    // 取多选选中文件, 内部调用 GetResultArr() 实现
    inline int GetSelectFiles(wstr** pArr = 0, SIGDN sigdnName = SIGDN_DESKTOPABSOLUTEPARSING)
    {
        return GetResultArr(pArr, sigdnName);
    }
    // 取选中文件,
    inline wstr GetResult(SIGDN sigdnName = SIGDN_DESKTOPABSOLUTEPARSING)
    {
        if (!m_pObj)return wstr();

        wstr ret;
        IShellItem* pSelItem;
        hr = m_pObj->GetResult(&pSelItem);
        if (SUCCEEDED(hr))
        {
            LPWSTR pszFilePath = NULL;
            hr = pSelItem->GetDisplayName(sigdnName, &pszFilePath);
            ret = pszFilePath;
            CoTaskMemFree(pszFilePath);
            pSelItem->Release();
        }
        return ret;
    }
    // 取多选选中文件
    inline int GetResultArr(wstr** retArr, SIGDN sigdnName = SIGDN_DESKTOPABSOLUTEPARSING)
    {
        if (!m_pObj)return 0;
        IFileOpenDialog* pObj = (IFileOpenDialog*)m_pObj;
        IShellItemArray* pSelResultArray;
        hr = pObj->GetResults(&pSelResultArray);
        if (FAILED(hr)) return 0;


        DWORD dwNumItems = 0;
        hr = pSelResultArray->GetCount(&dwNumItems);
        if (dwNumItems == 0 || !retArr)
        {
            SafeRelease(pSelResultArray);
            return (int)dwNumItems;
        }
        wstr* arr = new wstr[dwNumItems];
        *retArr = arr;

        for (DWORD i = 0; i < dwNumItems; i++)
        {
            IShellItem* pSelOneItem = NULL;
            PWSTR pszFilePath = NULL;
            hr = pSelResultArray->GetItemAt(i, &pSelOneItem);
            if (SUCCEEDED(hr))
            {
                hr = pSelOneItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &pszFilePath);
                if (SUCCEEDED(hr))
                {
                    arr[i] = pszFilePath;
                    CoTaskMemFree(pszFilePath);
                }
                pSelOneItem->Release();
            }
        }
        pSelResultArray->Release();
        return dwNumItems;
    }


    // 打开对话框, 返回选中的文件, 取消或失败返回空文本
    inline wstr open(
        HWND hWndParent = 0,                    // 父窗口句柄, 填写父窗口句柄后打开会禁用父窗口
        const wchar_t* filter = 0,              // 过滤器, 格式："MP3文件(*.mp3)\0*.mp3\0媒体文件(*.mpg)\0*.mpg\0\0"
        UINT filterIndex = 0,                   // 默认过滤器, 从0开始, 默认为0
        const wchar_t* Title = 0,               // 对话框的标题
        const wchar_t* defName = 0,             // 打开该对话框时出现在 "文件名" 编辑框中的文件名
        const wchar_t* NameLabel = 0,           // 文件名编辑框左边的标签文本
        const wchar_t* OkButtonLabel = 0,       // "打开" 或 "保存" 按钮的文本
        DWORD dwFlags = 0                       // _FILEOPENDIALOGOPTIONS::FOS_ 开头的枚举常量
    )
    {
        createOpen();
        IFileDialog* obj = m_pObj;
        if (!obj) return wstr();

        if (dwFlags) SetFlags(dwFlags);     // 设置标志
        SetFilter(filter, filterIndex);
        if (defName && defName[0])              // 设置打开该对话框时出现在 "文件名" 编辑框中的文件名
            hr = obj->SetFileName(defName);

        if (NameLabel && NameLabel[0])          // 设置文件名编辑框旁边的标签文本
            hr = obj->SetFileNameLabel(NameLabel);

        if (OkButtonLabel && OkButtonLabel[0])  // 设置 "打开" 或 "保存" 按钮的文本
            hr = obj->SetOkButtonLabel(OkButtonLabel);

        if (Title && Title[0])                  // 设置对话框标题的文本
            hr = obj->SetTitle(Title);

        hr = obj->Show(hWndParent); //Show dialog

        if (SUCCEEDED(hr)) return GetResult();
        return wstr();
    }


    // 打开文件夹, 返回选中的文件夹, 如果用户取消则返回空文本, 要获取错误信息可以调用 对象.取最后错误() 来获取
    inline wstr openFolder(
        HWND hWndParent = 0,                // 父窗口句柄, 填写父窗口句柄后打开会禁用父窗口
        const wchar_t* filter = 0,          // 过滤器, 格式："MP3文件(*.mp3)\0*.mp3\0媒体文件(*.mpg)\0*.mpg\0\0"
        UINT filterIndex = 0,               // 默认过滤器, 从0开始, 默认为0
        const wchar_t* Title = 0,           // 对话框的标题
        const wchar_t* defaultFolder = 0,   // 默认路径
        const wchar_t* defName = 0,         // 打开该对话框时出现在 "文件名" 编辑框中的文件名
        const wchar_t* NameLabel = 0,       // 文件名编辑框左边的标签文本
        const wchar_t* OkButtonLabel = 0,   // "打开" 或 "保存" 按钮的文本
        DWORD dwFlags = 0                   // _FILEOPENDIALOGOPTIONS::FOS_ 开头的枚举常量 
    )
    {
        createOpen();
        IFileOpenDialog* obj = (IFileOpenDialog*)m_pObj;
        if (!obj) return 0;

        SetOptions(dwFlags | FOS_PICKFOLDERS);
        SetFilter(filter, filterIndex);
        if (Title && Title[0])                  // 设置对话框的标题
            hr = obj->SetTitle(Title);

        if (defName && defName[0])              // 设置打开该对话框时出现在 "文件名" 编辑框中的文件名
            hr = obj->SetFileName(defName);

        if (NameLabel && NameLabel[0])          // 设置文件名编辑框旁边的标签文本
            hr = obj->SetFileNameLabel(NameLabel);

        if (OkButtonLabel && OkButtonLabel[0])  // 设置 "打开" 或 "保存" 按钮的文本
            hr = obj->SetOkButtonLabel(OkButtonLabel);

        if (defaultFolder && defaultFolder[0])  // 默认路径
            SetDefaultFolder(defaultFolder);

        hr = obj->Show(hWndParent); //Show dialog

        if (SUCCEEDED(hr)) return GetResult();
        return 0;
    }
    // 打开多选文件对话框, 返回选中的文件数量, 取消或失败返回0
    inline int openMulti(
        HWND hWndParent = 0,                    // 父窗口句柄, 填写父窗口句柄后打开会禁用父窗口
        wstr** retArr = 0,                  // 返回选中的文件数组, 不使用时需要调用 delete[] 释放
        const wchar_t* filter = 0,          // 过滤器, 格式："MP3文件(*.mp3)\0*.mp3\0媒体文件(*.mpg)\0*.mpg\0\0"
        UINT filterIndex = 0,               // 默认过滤器, 从0开始, 默认为0
        const wchar_t* Title = 0,           // 对话框的标题
        const wchar_t* defName = 0,         // 打开该对话框时出现在 "文件名" 编辑框中的文件名
        const wchar_t* NameLabel = 0,       // 文件名编辑框左边的标签文本
        const wchar_t* OkButtonLabel = 0,   // "打开" 或 "保存" 按钮的文本
        DWORD dwFlags = 0                   // _FILEOPENDIALOGOPTIONS::FOS_ 开头的枚举常量
    )
    {
        createOpen();
        IFileOpenDialog* obj = (IFileOpenDialog*)m_pObj;
        if (!obj) return 0;

        SetOptions(dwFlags | FOS_ALLOWMULTISELECT);
        SetFilter(filter, filterIndex);
        if (Title && Title[0])                  // 设置对话框的标题
            hr = obj->SetTitle(Title);

        if (defName && defName[0])              // 设置打开该对话框时出现在 "文件名" 编辑框中的文件名
            hr = obj->SetFileName(defName);

        if (NameLabel && NameLabel[0])          // 设置文件名编辑框旁边的标签文本
            hr = obj->SetFileNameLabel(NameLabel);

        if (OkButtonLabel && OkButtonLabel[0])  // 设置 "打开" 或 "保存" 按钮的文本
            hr = obj->SetOkButtonLabel(OkButtonLabel);

        hr = obj->Show(hWndParent); //Show dialog

        if (SUCCEEDED(hr)) return GetResultArr(retArr);
        return 0;
    }

    // 打开对话框, 返回选中的文件, 取消或失败返回空文本
    inline wstr save(
        HWND hWndParent = 0,                    // 父窗口句柄, 填写父窗口句柄后打开会禁用父窗口
        const wchar_t* filter = 0,              // 过滤器, 格式："MP3文件(*.mp3)\0*.mp3\0媒体文件(*.mpg)\0*.mpg\0\0"
        UINT filterIndex = 0,                   // 默认过滤器, 从0开始, 默认为0
        const wchar_t* Title = 0,               // 对话框的标题
        const wchar_t* defName = 0,             // 打开该对话框时出现在 "文件名" 编辑框中的文件名
        const wchar_t* defExtension = 0,        // 默认扩展名, 扩展名不带前面的小数点, 带小数点会失败
        const wchar_t* NameLabel = 0,           // 文件名编辑框左边的标签文本
        const wchar_t* OkButtonLabel = 0,       // "打开" 或 "保存" 按钮的文本
        DWORD dwFlags = 0                       // _FILEOPENDIALOGOPTIONS::FOS_ 开头的枚举常量
    )
    {
        createSave();
        IFileSaveDialog* obj = (IFileSaveDialog*)m_pObj;
        if (!obj) return wstr();

        if (dwFlags) SetOptions(dwFlags);
        SetFilter(filter, filterIndex);

        if (Title && Title[0])                  // 设置对话框的标题
            hr = obj->SetTitle(Title);

        if (defExtension && defExtension[0])    // 设置默认扩展名    
            hr = obj->SetDefaultExtension(defExtension);
        
        if (defName && defName[0])              // 设置打开该对话框时出现在 "文件名" 编辑框中的文件名
            hr = obj->SetFileName(defName);

        if (NameLabel && NameLabel[0])          // 设置文件名编辑框旁边的标签文本
            hr = obj->SetFileNameLabel(NameLabel);

        if (OkButtonLabel && OkButtonLabel[0])  // 设置 "打开" 或 "保存" 按钮的文本
            hr = obj->SetOkButtonLabel(OkButtonLabel);

        hr = obj->Show(hWndParent); //Show dialog
        if (SUCCEEDED(hr)) return GetResult();
        return wstr();
    }

public:
    inline HRESULT GetLastError() const { return hr; }





private:
    template<typename T>inline static void SafeRelease(T*& pObj)
    {
        if (pObj)
            pObj->Release();
        pObj = NULL;
    }
};
