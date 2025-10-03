#pragma once
#include <comdef.h>
#include <Shlobj.h>
#include <tstr.h>

class CFileDialog_Com
{
private:
    IFileDialog* m_pObj;    // �򿪶Ի���Ķ���ָ��
    HRESULT hr;             // ������, ÿ��һ������¼һ��

public:
    CFileDialog_Com() :m_pObj(0), hr(0)
    {
        hr = CoInitialize(nullptr);
    }
    ~CFileDialog_Com() { destroy(); }

public:
    // �������ļ�����
    inline bool createOpen() { return create(true); }
    // ���������ļ�����
    inline bool createSave() { return create(false); }

    // �����ļ��Ի������
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
    // ȡ��־
    inline DWORD GetOptions()
    {
        if (!m_pObj)return 0;
        FILEOPENDIALOGOPTIONS ret;
        hr = m_pObj->GetOptions(&ret);
        return ret;
    }
    // �ñ�־, ����ԭ���ı�־
    inline DWORD SetOptions(DWORD flags)
    {
        if (!m_pObj)return 0;
        FILEOPENDIALOGOPTIONS ret;
        hr = m_pObj->GetOptions(&ret);
        hr = m_pObj->SetOptions(ret | flags);
        return ret;
    }
    // ȡ��־, �˷������ܺ� GetOptions() һģһ��
    inline DWORD GetFlags()
    {
        return GetOptions();
    }
    // �ñ�־, ����ԭ���ı�־, �˷������ܺ� SetOptions() һģһ��
    inline DWORD SetFlags(DWORD flags)
    {
        return SetOptions(flags);
    }

    // ����Ĭ��Ŀ¼
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
    // ����Ŀ¼
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
    // ��ȡ��ǰѡ����ļ���, �����ǰδ��ʾ�Ի���, ���ȡ�򿪶Ի���ʱҪѡ����ļ���
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
    // ��Ĭ���ļ���
    inline bool SetFileName(const wchar_t* file)
    {
        if (!m_pObj)return 0;
        hr = m_pObj->SetFileName(file);
        return SUCCEEDED(hr);
    }
    // �öԻ������
    inline bool SetTitle(const wchar_t* title)
    {
        if (!m_pObj)return 0;
        hr = m_pObj->SetTitle(title);
        return SUCCEEDED(hr);
    }
    // �ñ�ǩ�ı�
    inline bool SetFileNameLabel(const wchar_t* text)
    {
        if (!m_pObj)return 0;
        hr = m_pObj->SetFileNameLabel(text);
        return SUCCEEDED(hr);
    }
    // �ð�ť�ı�
    inline bool SetOkButtonLabel(const wchar_t* text)
    {
        if (!m_pObj)return 0;
        hr = m_pObj->SetOkButtonLabel(text);
        return SUCCEEDED(hr);
    }
    // �ð�ť�ı�
    inline bool Show(HWND hWndParent = 0)
    {
        if (!m_pObj)return 0;
        hr = m_pObj->Show(hWndParent);
        return SUCCEEDED(hr);
    }

    // �ù�����, ÿ��������ʹ�� \0 �ָ�, �ڲ����� SetFilter() ʵ��
    // filter = ������, ��ʽ��"MP3�ļ�(*.mp3)\0*.mp3\0ý���ļ�(*.mpg)\0*.mpg\0\0"
    inline bool SetFileTypes(const wchar_t* filter, UINT filterIndex = 0)
    {
        return SetFilter(filter, filterIndex);
    }
    // ���ù�����, ������ģʽ, ÿ��������ʹ�� | �ָ�, �ڲ����� SetFilterEMode() ʵ��
    // filter = ������, ��ʽ��"MP3�ļ�(*.mp3)|*.mp3|ý���ļ�(*.mpg)|*.mpg"
    inline bool SetFileTypesEMode(const wchar_t* filter, UINT filterIndex = 0)
    {
        return SetFilterEMode(filter, filterIndex);
    }

    // ���ù�����, ÿ��������ʹ�� \0 �ָ�
    // filter = ������, ��ʽ��"MP3�ļ�(*.mp3)\0*.mp3\0ý���ļ�(*.mpg)\0*.mpg\0\0"
    // filterIndex = Ĭ�Ϲ�����, ��0��ʼ, Ĭ��Ϊ0
    inline bool SetFilter(const wchar_t* filter, UINT filterIndex = 0)
    {
        if (!m_pObj || !filter) return 0;

        _str arrBuf(sizeof(const wchar_t*) * 100);
        int i = 1;
        const wchar_t* p = filter;
        arrBuf.assign((LPCSTR)&p, sizeof(const wchar_t*));
        while (1)
        {
            if (*p++ == 0)    // ����ֽ��ж��ַ���, ����\0����һ��
            {
                if (*p++ == 0) break;    // \0\0 ���ǽ�β
                ++i;
                --p;
                arrBuf.append((LPCSTR)&p, sizeof(const wchar_t*));
                ++p;
            }
        }
        hr = m_pObj->SetFileTypes(i / 2, (COMDLG_FILTERSPEC*)arrBuf.c_str());    // ���ù�����
        if (SUCCEEDED(hr)) m_pObj->SetFileTypeIndex(filterIndex + 1);
        return SUCCEEDED(hr);
    }

    // ���ù�����, ������ģʽ, ÿ��������ʹ�� | �ָ�
    // filter = ������, ��ʽ��"MP3�ļ�(*.mp3)|*.mp3|ý���ļ�(*.mpg)|*.mpg"
    // filterIndex = Ĭ�Ϲ�����, ��0��ʼ, Ĭ��Ϊ0
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
            if (*p++ == '|')    // ����ֽ��ж��ַ���, ���� | ����һ��
            {
                ++i;
                --p;
                arrBuf.append((LPCSTR)&p, sizeof(const wchar_t*));
                ++p;
            }
        }
        hr = m_pObj->SetFileTypes(i / 2, (COMDLG_FILTERSPEC*)arrBuf.c_str());    // ���ù�����
        if (SUCCEEDED(hr)) m_pObj->SetFileTypeIndex(filterIndex + 1);
        return SUCCEEDED(hr);
    }
    // ��Ĭ����չ��
    inline bool SetDefaultExtension(const wchar_t* text)
    {
        if (!m_pObj)return 0;
        hr = m_pObj->SetDefaultExtension(text);
        return SUCCEEDED(hr);
    }
    // ȡѡ���ļ�, �ڲ����� GetResult() ʵ��
    inline wstr GetSelectFile(SIGDN sigdnName = SIGDN_DESKTOPABSOLUTEPARSING)
    {
        return GetResult(sigdnName);
    }
    // ȡ��ѡѡ���ļ�, �ڲ����� GetResultArr() ʵ��
    inline int GetSelectFiles(wstr** pArr = 0, SIGDN sigdnName = SIGDN_DESKTOPABSOLUTEPARSING)
    {
        return GetResultArr(pArr, sigdnName);
    }
    // ȡѡ���ļ�,
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
    // ȡ��ѡѡ���ļ�
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


    // �򿪶Ի���, ����ѡ�е��ļ�, ȡ����ʧ�ܷ��ؿ��ı�
    inline wstr open(
        HWND hWndParent = 0,                    // �����ھ��, ��д�����ھ����򿪻���ø�����
        const wchar_t* filter = 0,              // ������, ��ʽ��"MP3�ļ�(*.mp3)\0*.mp3\0ý���ļ�(*.mpg)\0*.mpg\0\0"
        UINT filterIndex = 0,                   // Ĭ�Ϲ�����, ��0��ʼ, Ĭ��Ϊ0
        const wchar_t* Title = 0,               // �Ի���ı���
        const wchar_t* defName = 0,             // �򿪸öԻ���ʱ������ "�ļ���" �༭���е��ļ���
        const wchar_t* NameLabel = 0,           // �ļ����༭����ߵı�ǩ�ı�
        const wchar_t* OkButtonLabel = 0,       // "��" �� "����" ��ť���ı�
        DWORD dwFlags = 0                       // _FILEOPENDIALOGOPTIONS::FOS_ ��ͷ��ö�ٳ���
    )
    {
        createOpen();
        IFileDialog* obj = m_pObj;
        if (!obj) return wstr();

        if (dwFlags) SetFlags(dwFlags);     // ���ñ�־
        SetFilter(filter, filterIndex);
        if (defName && defName[0])              // ���ô򿪸öԻ���ʱ������ "�ļ���" �༭���е��ļ���
            hr = obj->SetFileName(defName);

        if (NameLabel && NameLabel[0])          // �����ļ����༭���Աߵı�ǩ�ı�
            hr = obj->SetFileNameLabel(NameLabel);

        if (OkButtonLabel && OkButtonLabel[0])  // ���� "��" �� "����" ��ť���ı�
            hr = obj->SetOkButtonLabel(OkButtonLabel);

        if (Title && Title[0])                  // ���öԻ��������ı�
            hr = obj->SetTitle(Title);

        hr = obj->Show(hWndParent); //Show dialog

        if (SUCCEEDED(hr)) return GetResult();
        return wstr();
    }


    // ���ļ���, ����ѡ�е��ļ���, ����û�ȡ���򷵻ؿ��ı�, Ҫ��ȡ������Ϣ���Ե��� ����.ȡ������() ����ȡ
    inline wstr openFolder(
        HWND hWndParent = 0,                // �����ھ��, ��д�����ھ����򿪻���ø�����
        const wchar_t* filter = 0,          // ������, ��ʽ��"MP3�ļ�(*.mp3)\0*.mp3\0ý���ļ�(*.mpg)\0*.mpg\0\0"
        UINT filterIndex = 0,               // Ĭ�Ϲ�����, ��0��ʼ, Ĭ��Ϊ0
        const wchar_t* Title = 0,           // �Ի���ı���
        const wchar_t* defaultFolder = 0,   // Ĭ��·��
        const wchar_t* defName = 0,         // �򿪸öԻ���ʱ������ "�ļ���" �༭���е��ļ���
        const wchar_t* NameLabel = 0,       // �ļ����༭����ߵı�ǩ�ı�
        const wchar_t* OkButtonLabel = 0,   // "��" �� "����" ��ť���ı�
        DWORD dwFlags = 0                   // _FILEOPENDIALOGOPTIONS::FOS_ ��ͷ��ö�ٳ��� 
    )
    {
        createOpen();
        IFileOpenDialog* obj = (IFileOpenDialog*)m_pObj;
        if (!obj) return 0;

        SetOptions(dwFlags | FOS_PICKFOLDERS);
        SetFilter(filter, filterIndex);
        if (Title && Title[0])                  // ���öԻ���ı���
            hr = obj->SetTitle(Title);

        if (defName && defName[0])              // ���ô򿪸öԻ���ʱ������ "�ļ���" �༭���е��ļ���
            hr = obj->SetFileName(defName);

        if (NameLabel && NameLabel[0])          // �����ļ����༭���Աߵı�ǩ�ı�
            hr = obj->SetFileNameLabel(NameLabel);

        if (OkButtonLabel && OkButtonLabel[0])  // ���� "��" �� "����" ��ť���ı�
            hr = obj->SetOkButtonLabel(OkButtonLabel);

        if (defaultFolder && defaultFolder[0])  // Ĭ��·��
            SetDefaultFolder(defaultFolder);

        hr = obj->Show(hWndParent); //Show dialog

        if (SUCCEEDED(hr)) return GetResult();
        return 0;
    }
    // �򿪶�ѡ�ļ��Ի���, ����ѡ�е��ļ�����, ȡ����ʧ�ܷ���0
    inline int openMulti(
        HWND hWndParent = 0,                    // �����ھ��, ��д�����ھ����򿪻���ø�����
        wstr** retArr = 0,                  // ����ѡ�е��ļ�����, ��ʹ��ʱ��Ҫ���� delete[] �ͷ�
        const wchar_t* filter = 0,          // ������, ��ʽ��"MP3�ļ�(*.mp3)\0*.mp3\0ý���ļ�(*.mpg)\0*.mpg\0\0"
        UINT filterIndex = 0,               // Ĭ�Ϲ�����, ��0��ʼ, Ĭ��Ϊ0
        const wchar_t* Title = 0,           // �Ի���ı���
        const wchar_t* defName = 0,         // �򿪸öԻ���ʱ������ "�ļ���" �༭���е��ļ���
        const wchar_t* NameLabel = 0,       // �ļ����༭����ߵı�ǩ�ı�
        const wchar_t* OkButtonLabel = 0,   // "��" �� "����" ��ť���ı�
        DWORD dwFlags = 0                   // _FILEOPENDIALOGOPTIONS::FOS_ ��ͷ��ö�ٳ���
    )
    {
        createOpen();
        IFileOpenDialog* obj = (IFileOpenDialog*)m_pObj;
        if (!obj) return 0;

        SetOptions(dwFlags | FOS_ALLOWMULTISELECT);
        SetFilter(filter, filterIndex);
        if (Title && Title[0])                  // ���öԻ���ı���
            hr = obj->SetTitle(Title);

        if (defName && defName[0])              // ���ô򿪸öԻ���ʱ������ "�ļ���" �༭���е��ļ���
            hr = obj->SetFileName(defName);

        if (NameLabel && NameLabel[0])          // �����ļ����༭���Աߵı�ǩ�ı�
            hr = obj->SetFileNameLabel(NameLabel);

        if (OkButtonLabel && OkButtonLabel[0])  // ���� "��" �� "����" ��ť���ı�
            hr = obj->SetOkButtonLabel(OkButtonLabel);

        hr = obj->Show(hWndParent); //Show dialog

        if (SUCCEEDED(hr)) return GetResultArr(retArr);
        return 0;
    }

    // �򿪶Ի���, ����ѡ�е��ļ�, ȡ����ʧ�ܷ��ؿ��ı�
    inline wstr save(
        HWND hWndParent = 0,                    // �����ھ��, ��д�����ھ����򿪻���ø�����
        const wchar_t* filter = 0,              // ������, ��ʽ��"MP3�ļ�(*.mp3)\0*.mp3\0ý���ļ�(*.mpg)\0*.mpg\0\0"
        UINT filterIndex = 0,                   // Ĭ�Ϲ�����, ��0��ʼ, Ĭ��Ϊ0
        const wchar_t* Title = 0,               // �Ի���ı���
        const wchar_t* defName = 0,             // �򿪸öԻ���ʱ������ "�ļ���" �༭���е��ļ���
        const wchar_t* defExtension = 0,        // Ĭ����չ��, ��չ������ǰ���С����, ��С�����ʧ��
        const wchar_t* NameLabel = 0,           // �ļ����༭����ߵı�ǩ�ı�
        const wchar_t* OkButtonLabel = 0,       // "��" �� "����" ��ť���ı�
        DWORD dwFlags = 0                       // _FILEOPENDIALOGOPTIONS::FOS_ ��ͷ��ö�ٳ���
    )
    {
        createSave();
        IFileSaveDialog* obj = (IFileSaveDialog*)m_pObj;
        if (!obj) return wstr();

        if (dwFlags) SetOptions(dwFlags);
        SetFilter(filter, filterIndex);

        if (Title && Title[0])                  // ���öԻ���ı���
            hr = obj->SetTitle(Title);

        if (defExtension && defExtension[0])    // ����Ĭ����չ��    
            hr = obj->SetDefaultExtension(defExtension);
        
        if (defName && defName[0])              // ���ô򿪸öԻ���ʱ������ "�ļ���" �༭���е��ļ���
            hr = obj->SetFileName(defName);

        if (NameLabel && NameLabel[0])          // �����ļ����༭���Աߵı�ǩ�ı�
            hr = obj->SetFileNameLabel(NameLabel);

        if (OkButtonLabel && OkButtonLabel[0])  // ���� "��" �� "����" ��ť���ı�
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
