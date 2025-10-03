#pragma once
#include <IDraw_Header.h>

// ��Դģ��
class EWindowResource : public IEWindowResource
{
public:

    // ��ȡ������������
    virtual LPBYTE GetELogo(DWORD* size);

    // ��ȡĬ��������ɫ��clr�ļ�, �����ļ�����, ����+����, Ŀǰһ����6���ļ�
    virtual LPBYTE GetDefThemeData(DWORD* size);

    void SetModule(HINSTANCE hInst);


    // ��ȡ������������
    virtual LPSYSBUTTONIMAGE GetNskn();

    // ��ȡ������ͼ��, pIconType = �Ƿ��Ǵ�Ĭ��ֵ��õ���ͼ��, ���ڵ��óɹ�ʱ��ֵ
    // pIconType = ���ջ�ȡ����ͼ����Դ, 0=�û�ͼ��, 1=�û�Ĭ��, 2=Ĭ��Ƥ����ͼ��, 3=Ĭ�����Ĭ��ͼ��
    virtual LPWINDOW_ICON_DATA Tool_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96);

    // ��ȡ֧�ֿ����Ϳ�ͼ��, pIconType = �Ƿ��Ǵ�Ĭ��ֵ��õ���ͼ��, ���ڵ��óɹ�ʱ��ֵ
    // pIconType = ���ջ�ȡ����ͼ����Դ, 0=�û�ͼ��, 1=�û�Ĭ��, 2=Ĭ��Ƥ����ͼ��, 3=Ĭ�����Ĭ��ͼ��
    virtual LPWINDOW_ICON_DATA TreeFne_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96);

    // ��ȡ�������Ϳ�ͼ��, ����ͼ������, pIconType = �Ƿ��Ǵ�Ĭ��ֵ��õ���ͼ��, ���ڵ��óɹ�ʱ��ֵ
    // pIconType = ���ջ�ȡ����ͼ����Դ, 0=�û�ͼ��, 1=�û�Ĭ��, 2=Ĭ��Ƥ����ͼ��, 3=Ĭ�����Ĭ��ͼ��
    virtual LPWINDOW_ICON_DATA TreeProgram_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96);

    // ��ȡ�����ͼ��, pIconType = �Ƿ��Ǵ�Ĭ��ֵ��õ���ͼ��, ���ڵ��óɹ�ʱ��ֵ
    // pIconType = ���ջ�ȡ����ͼ����Դ, 0=�û�ͼ��, 1=�û�Ĭ��, 2=Ĭ��Ƥ����ͼ��, 3=Ĭ�����Ĭ��ͼ��
    virtual LPWINDOW_ICON_DATA TreeControl_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96);

    // ��ȡ���Ϳ�Ӽ���ͼ��, pIconType = �Ƿ��Ǵ�Ĭ��ֵ��õ���ͼ��, ���ڵ��óɹ�ʱ��ֵ
    // pIconType = ���ջ�ȡ����ͼ����Դ, 0=�û�ͼ��, 1=�û�Ĭ��, 2=Ĭ��Ƥ����ͼ��, 3=Ĭ�����Ĭ��ͼ��
    virtual LPWINDOW_ICON_DATA TreeExpand_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96);

    // ��ȡ��Ͽ�ͼ��, pIconType = �Ƿ��Ǵ�Ĭ��ֵ��õ���ͼ��, ���ڵ��óɹ�ʱ��ֵ
    // pIconType = ���ջ�ȡ����ͼ����Դ, 0=�û�ͼ��, 1=�û�Ĭ��, 2=Ĭ��Ƥ����ͼ��, 3=Ĭ�����Ĭ��ͼ��
    virtual LPWINDOW_ICON_DATA Combobox_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96);

    // ��ȡ�˵����ͼ��, pIconType = �Ƿ��Ǵ�Ĭ��ֵ��õ���ͼ��, ���ڵ��óɹ�ʱ��ֵ
    // pIconType = ���ջ�ȡ����ͼ����Դ, 0=�û�ͼ��, 1=�û�Ĭ��, 2=Ĭ��Ƥ����ͼ��, 3=Ĭ�����Ĭ��ͼ��
    virtual LPWINDOW_ICON_DATA Menu_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96);

    // ��ȡ���ҵ�������ͼ��, pIconType = �Ƿ��Ǵ�Ĭ��ֵ��õ���ͼ��, ���ڵ��óɹ�ʱ��ֵ
    // pIconType = ���ջ�ȡ����ͼ����Դ, 0=�û�ͼ��, 1=�û�Ĭ��, 2=Ĭ��Ƥ����ͼ��, 3=Ĭ�����Ĭ��ͼ��
    virtual LPWINDOW_ICON_DATA MsctlsUpdowm32_GetIcon_lr(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96);

    // ��ȡ���µ�������ͼ��, pIconType = �Ƿ��Ǵ�Ĭ��ֵ��õ���ͼ��, ���ڵ��óɹ�ʱ��ֵ
    // pIconType = ���ջ�ȡ����ͼ����Դ, 0=�û�ͼ��, 1=�û�Ĭ��, 2=Ĭ��Ƥ����ͼ��, 3=Ĭ�����Ĭ��ͼ��
    virtual LPWINDOW_ICON_DATA MsctlsUpdowm32_GetIcon_tb(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96);

    // ��ȡ����״̬���İ�ťͼ��, pIconType = �Ƿ��Ǵ�Ĭ��ֵ��õ���ͼ��, ���ڵ��óɹ�ʱ��ֵ
    // pIconType = ���ջ�ȡ����ͼ����Դ, 0=�û�ͼ��, 1=�û�Ĭ��, 2=Ĭ��Ƥ����ͼ��, 3=Ĭ�����Ĭ��ͼ��
    virtual LPWINDOW_ICON_DATA TopState_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96);

    // �ӳ���ͼ�����ȡͼ��, ����ͼ������, û�����ͼ�귵��0
    virtual LPWINDOW_ICON_DATA GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96);


    // �ͷ���Դ
    virtual void EWindow_Resource_Free();

    // ���ļ�������Դ, ������Դ���, ���ػ�Ӱ��Ч��, һ��ext�ļ�����һ�ξ���, ����ʱ���Զ�����
    virtual IDRAW_HRESOURCE LoadFromFile(LPCWSTR szFile);
    // ���ڴ������Դ, ������Դ���, ���ػ�Ӱ��Ч��, һ��ext�ļ�����һ�ξ���, ����ʱ���Զ�����
    virtual IDRAW_HRESOURCE LoadFromMemory(const void* pData, int size);

    // ���û����ص���Դ���ȡָ��ID��ͼ��, ����idָ�������
    virtual LPWINDOW_ICON_DATA User_GetIcon(IDRAW_HRESOURCE hResource, LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96);


    // ȡ��������а󶨴�����Դ�Ļص�
    virtual bool UnBindWindow(LPCSTR guid);

    // ��ȡĬ���������Դ���, ���ݴ��ھ����ȡ
    // hWnd = Ҫ��ȡ��Դ�Ĵ��ھ��, ��ȡ�˵�ʹ�� HRESOURCE_MENU, ��ȡ���Ϳ�չ��������ťʹ�� HRESOURCE_TREEEXPAND
    virtual IDRAW_HRESOURCE GetDefControlIcon(HWND hWnd);

    // ����Դ�󶨵�������, �´��ػ��������ʱ��������Դ��ȡͼ��
    // hWnd = Ҫ�󶨵Ĵ��ھ��, �󶨲˵�ʹ�� HRESOURCE_MENU, �����Ϳ�չ��������ťʹ�� HRESOURCE_TREEEXPAND
    // hResource = ���ص���Դ���, ����������Ϊ0, �����һ������Ϊ0, �򲻻�����趨��ͼ��
    // callback = �ص�����, �����������Ҫ����ͼ��ʱ����, Ϊ0��ʹ��Ĭ�ϵ���Դ����, ���û��Ĭ�ϵ���Դ�򲻼���
    //              �ص���������Ҫ�����Ƿ�Ϊdx��Ⱦ��д�� prc �ͷ���ֵ
    virtual bool BindWindow(LPCSTR guid, HWND hWnd, IDRAW_HRESOURCE hResource, pfn_GetWindowIcon callback);


    // �Ӵ��ھ����ȡ�ص�����, ���Ե��ûص���������ȡͼ��, ����д�뻺����������, bufCallback Ϊ0�򷵻���Ҫ�Ļ�������Ա��
    // hWnd = ���ĸ������ϻ�ȡ�ص���Ϣ
    // bufCallback = ���հ�ʱ���ݵĻص���������, ���Ϊ0�򷵻ذ󶨵�����
    // bufCount = ��ʾ bufCallback ����������ж��ٸ���Ա
    virtual int GetResourceCallbackFromHwnd(HWND hWnd, BINDWINDOW_INFO* bufCallback = 0, int bufCount = 0);

    // �Ӵ��ھ�����ȡͼ��, ����ڰ󶨵�ʱ����˻ص�����, �����ûص���������
    // �ڲ����� GetResourceFromHwnd() + User_GetIcon()/User_GetIconDx() ��ʵ��
    // hWnd = �󶨵Ĵ��ھ��, ���û�а��򷵻�0
    // pszID = Ҫȡ��ͼ��ID, ����һ������Ŀ�ı�
    // arg = ������ݲ�ͬ������ݲ�ͬ�ṹ, ���Ϳ򴫵� TVITEMEXW�ṹָ��, 
    // prc = ����ͼ����ͼƬ���λ��
    // state = ͼƬ״̬, STATE ö�ٳ������ֵ, Ŀǰֻ֧�� STATE::HOT, STATE::DOWN, STATE::BAN, STATE::NORMAL ��4��
    // pIconType = ����ͼ����Դ, 0=��ID�л�ȡ, 1=��Ĭ��ͼ�����ȡ
    virtual LPWINDOW_ICON_DATA GetIconFromHwnd(HWND hWnd, LPCWSTR pszID, void* arg, RECT_F* prc, STATE state, UINT dpi = 96);

    // ��gdi+λͼ����dxλͼ��������Դ����ֵ, ����ָ����뱣֤���, �ڲ����������ڴ������洫�ݽ������ı���λͼ
    // pImg = ͼƬ���ݵ�ַ
    // pImgSize = ͼƬ���ݴ�С
    // text = ���Ƶ�ǰ��Ŀ��ʾ���ı�, �����޸�ԭ����ı�, ֻ�ǻ滭ʱ��ʾ����ı�, ����Ϊ0
    // remarks = ��ע�ı�, ����Ϊ0
    // crText = �ı�ARGB��ɫ, ����Ϊ0, ��Ϊ0�������4����Ա, �ֱ��� ����, �ȵ�, ����, ��ֹ
    // crBack = ����ARGB��ɫ, ����Ϊ0, ��Ϊ0�������4����Ա, �ֱ��� ����, �ȵ�, ����, ��ֹ
    virtual LPWINDOW_ICON_DATA MakeResourceReturn(const void* pImg, int pImgSize, LPCWSTR text, LPCWSTR remarks, DWORD* crText, DWORD* crBack);

    // �ͷ� MakeResourceReturn() ���ɵķ���ֵ
    virtual bool FreeResourceReturn(LPWINDOW_ICON_DATA pData);

    // ���ڴ����gdi+ͼƬ
    // pData = ͼƬ����
    // size = ͼƬ���ݴ�С
    virtual Gdiplus::Bitmap* LoadGpImageFromMemory(const void* pData, int size);

    // �ͷŴ� LoadGpImageFromMemory() ���ص�ֵ
    virtual bool FreeGpImage(Gdiplus::Bitmap*);

    // ��ȡmarlett����, ͼ������, �滭������
    virtual HFONT GetMarlettFont(int size);

    // ��ȡ��������
    virtual LPBYTE GetMarlettData(int* size);
};

