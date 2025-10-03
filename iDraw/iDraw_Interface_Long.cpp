#if 1
#include "EWindow_Fne_Main.h"


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ���໯���ص�ָ�������, ���������໯������Ϣ�ص�������ĵ��������
// 
// ����:
//      ����1: ���໯���ص�ָ��, ���������໯������Ϣ�ص�������ĵ��������
//      ����2: ��������, ������ο� IGL_ ��ͷ����
// 
// ����ֵ:
//      ��ȡ����ֵ
EXTERN_C _declspec(dllexport) LONG_PTR WINAPI iDraw_GetLong(LPOBJSTRUCT pData, int index)
{
    if (!pData)return 0;
    switch (index)
    {
    case IGL_HWND:       // ���ھ��
        return (LONG_PTR)pData->hWnd;
    case IGL_WNDPROC:       // �������໯�ĺ�����ַ, iDraw_SubclassWindow() �����ĵڶ���������¼����
        return (LONG_PTR)pData->fnWndproc;
    case IGL_CALLBACK_NOTIFY:       // ֪ͨ��Ϣ����, �ɸ����ڵ���, ����ԭ�� LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
        return (LONG_PTR)pData->fnNotify;
    case IGL_CALLBACK_DRAWITEM:       // �滭��Ϣ����, �ɸ����ڵ���, ����ԭ�� LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
        return (LONG_PTR)pData->fnDrawItem;
    case IGL_CALLBACK_MEASUREITEM:       // ����ߴ���Ϣ����, �ɸ����ڵ���, ����ԭ�� LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
        return (LONG_PTR)pData->fnMeasureItem;
    case IGL_CALLBACK_COMMAND:       // �¼���Ϣ����, �ɸ����ڵ���, ����ԭ�� LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
        return (LONG_PTR)pData->fnCommand;
    case IGL_CALLBACK_CTLCOLOR:       // ��ɫ��Ϣ����, �ɸ����ڵ���, ����ԭ�� LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
        return (LONG_PTR)pData->fnCtlcolor;
    case IGL_CALLBACK_CALLWINDOWPROC:       // CallWindowProc ʹ��W�滹��A��, �ڵ������໯ʱ����ݴ����Ƿ�Ϊunicode���¼
        return (LONG_PTR)pData->fnCallProc;
    case IGL_INTERCEPT:       // �Ƿ����ش���Ϣ, �ҽ���Ϣʱ�Ż�ʹ�����ֵ
        return (LONG_PTR)pData->isReturn;
    case IGL_OLDPROC:       // ���ڵ�ԭ�ص�����
        return (LONG_PTR)pData->oldProc;
    case IGL_CALLPROC_RETURN:       // ����ԭ�ص������ķ���ֵ, ע�ᴦ�����Ϣ�¼��¿���ͨ���������ȡ������ֵ
        return (LONG_PTR)pData->lResult;
    case IGL_CANVAS:            // �봰�ڹ�����dx�滭���, ������ڲ���ʹ��dx��Ⱦ, ���ֵΪ0
        return (LONG_PTR)pData->draw.hCanvas;
    case IGL_USERDATA:          // �봰�ڹ������û�����
        return (LONG_PTR)pData->userdata;

    case IGL_DRAW_NONCPAINT:       // ���ػ��ǿͻ���, Ϊtrueʱ�������໯��Ϣ�ĺ��������յ� WM_NCPAINT ��Ϣ
        return (LONG_PTR)pData->draw.isNoNcPaint;
    case IGL_DRAW_NOCALCSIZE:       // ������ǿͻ���, Ϊtrueʱ�������໯��Ϣ�ĺ��������յ� WM_NCCALCSIZE ��Ϣ, ���ڲ����б߿�
        return (LONG_PTR)pData->draw.isNcCalcSize;
    case IGL_DRAW_ISTRANSPARENT:       // �Ƿ�͸��, ���Ϊtrue, �����õ� CTLCOLOR �л���� SetMode() ����Ϊ͸��
        return (LONG_PTR)pData->draw.isTransparent;
    case IGL_DRAW_ISHOT:       // �Ƿ������ȵ�, һ�����б���ص����ʹ��
        return (LONG_PTR)pData->draw.isHot;
    case IGL_DRAW_ISCLIENT:       // �Ƿ�����������Ϣ����Ϊ�ͻ���, Ϊtrue�򲻻��յ� WM_NCHITTEST��Ϣ, �ڲ�ȫ������ HTCLIENT
        return (LONG_PTR)pData->draw.isClient;
    case IGL_DRAW_ISERASEBKG:       // �Ƿ����ػ�������Ϣ, ���Ϊtrue, �򴰿ڻ��յ� WM_ERASEBACKGROUND ��Ϣ, �����ڲ�����1
        return (LONG_PTR)pData->draw.isErasebkg;
    case IGL_DRAW_STATE:       // ��ǰ�����״̬
        return (LONG_PTR)pData->draw.state;
    case IGL_DRAW_FONT_DX:       // ��ȡDX����
        return (LONG_PTR)pData->draw.hFont;
    case IGL_DRAW_FONT:       // ��ȡGDI����
        return (LONG_PTR)(pData->draw.hFont ? pData->draw.hFont->GetFont() : 0);
    case IGL_DRAW_TEXTFORMAT:       // �滭�ı������ʽ
        return (LONG_PTR)pData->draw.textFormat;

    case IGL_DRAW_CRBACKGROUND:       // ������ɫ, ÿ���������ʹ�������ɫ���滭����
        return (LONG_PTR)pData->draw.color.crBackGround;
    case IGL_DRAW_CRTEXT:       // �ı���ɫ, ÿ������������ı���ɫ����ʹ�������ɫ�滭
        return (LONG_PTR)pData->draw.color.crText;
    case IGL_DRAW_CRTEXTHOT:       // �ı��ȵ���ɫ, �ĸ�����õ���ο�����
        return (LONG_PTR)pData->draw.color.crTextHot;
    case IGL_DRAW_CRTEXTDOWN:       // �ı�������ɫ, �ĸ�����õ���ο�����
        return (LONG_PTR)pData->draw.color.crTextDown;
    case IGL_DRAW_CRTEXTBAN:       // �ı���ֹ��ɫ, �ĸ�����õ���ο�����
        return (LONG_PTR)pData->draw.color.crTextBan;
    case IGL_DRAW_CRNORMAL:       // ���������ɫ, �ĸ�����õ���ο�����
        return (LONG_PTR)pData->draw.color.crNormal;
    case IGL_DRAW_CRHOVER:       // ���������ɫ, �ĸ�����õ���ο�����
        return (LONG_PTR)pData->draw.color.crHover;
    case IGL_DRAW_CRDOWN:       // ���������ɫ, �ĸ�����õ���ο�����
        return (LONG_PTR)pData->draw.color.crDown;
    case IGL_DRAW_CRFOCUS:       // ���������ɫ, �ĸ�����õ���ο�����
        return (LONG_PTR)pData->draw.color.crFocus;
    case IGL_DRAW_CRCHECKED:       // ���ѡ����ɫ, �ĸ�����õ���ο�����
        return (LONG_PTR)pData->draw.color.crChecked;
    case IGL_DRAW_CRSELECTED:       // ���ѡ����ɫ, �ĸ�����õ���ο�����
        return (LONG_PTR)pData->draw.color.crSelected;
    case IGL_DRAW_CRHOT:       // ����ȵ���ɫ, �ĸ�����õ���ο�����
        return (LONG_PTR)pData->draw.color.crHot;
    case IGL_DRAW_CRBAN:       // �����ֹ��ɫ, �ĸ�����õ���ο�����
        return (LONG_PTR)pData->draw.color.crBan;
    case IGL_DRAW_CRVISTED:       // 
        return (LONG_PTR)pData->draw.color.crVisted;
    case IGL_DRAW_CRSHADOW:       // ���������ɫ, �ĸ�����õ���ο�����
        return (LONG_PTR)pData->draw.color.crShadow;
    case IGL_DRAW_CRBORDER:       // ����߿���ɫ, ���л��Ʊ߿�ʹ�õ���ɫ
        return (LONG_PTR)pData->draw.color.crBorder;

    case IGL_DRAW_HBRBACKGROUND:       // ����GDI��ˢ, ÿ���������ʹ�������ˢ����䱳��
        return (LONG_PTR)pData->draw.color.hbrBackGround;
    case IGL_DRAW_HBRTEXT:       // �ı�GDI��ˢ
        return (LONG_PTR)pData->draw.color.hbrText;
    case IGL_DRAW_HBRTEXTHOT:       // �ı��ȵ�GDI��ˢ, �ĸ�����õ���ο�����
        return (LONG_PTR)pData->draw.color.hbrTextHot;
    case IGL_DRAW_HBRTEXTDOWN:       // �ı�����GDI��ˢ, �ĸ�����õ���ο�����
        return (LONG_PTR)pData->draw.color.hbrTextDown;
    case IGL_DRAW_HBRTEXTBAN:       // �ı���ֹGDI��ˢ, �ĸ�����õ���ο�����
        return (LONG_PTR)pData->draw.color.hbrTextBan;
    case IGL_DRAW_HBRNORMAL:       // �������GDI��ˢ, �ĸ�����õ���ο�����
        return (LONG_PTR)pData->draw.color.hbrNormal;
    case IGL_DRAW_HBRHOVER:       // �������GDI��ˢ, �ĸ�����õ���ο�����
        return (LONG_PTR)pData->draw.color.hbrHover;
    case IGL_DRAW_HBRDOWN:       // �������GDI��ˢ, �ĸ�����õ���ο�����
        return (LONG_PTR)pData->draw.color.hbrDown;
    case IGL_DRAW_HBRFOCUS:       // �������GDI��ˢ, �ĸ�����õ���ο�����
        return (LONG_PTR)pData->draw.color.hbrFocus;
    case IGL_DRAW_HBRCHECKED:       // ���ѡ��GDI��ˢ, �ĸ�����õ���ο�����
        return (LONG_PTR)pData->draw.color.hbrChecked;
    case IGL_DRAW_HBRSELECTED:       // ���ѡ��GDI��ˢ, �ĸ�����õ���ο�����
        return (LONG_PTR)pData->draw.color.hbrSelected;
    case IGL_DRAW_HBRHOT:       // ����ȵ�GDI��ˢ, �ĸ�����õ���ο�����
        return (LONG_PTR)pData->draw.color.hbrHot;
    case IGL_DRAW_HBRBAN:       // �����ֹGDI��ˢ, �ĸ�����õ���ο�����
        return (LONG_PTR)pData->draw.color.hbrBan;
    case IGL_DRAW_HBRVISTED:       // 
        return (LONG_PTR)pData->draw.color.hbrVisted;
    case IGL_DRAW_HBRSHADOW:       // �����ӰGDI��ˢ, �ĸ�����õ���ο�����
        return (LONG_PTR)pData->draw.color.hbrShadow;
    case IGL_DRAW_HBRBORDER:       // ����߿�GDI����, ���л��Ʊ߿�ʹ�õĻ���
        return (LONG_PTR)pData->draw.color.hpenBorder;

    case IGL_POS_RECTSCREEN:       // ��������Ļ�е�λ��, ����RECT�ṹָ��, �������޸����ָ��, ֻ�ܻ�ȡ��������
        return (LONG_PTR)&pData->pos.rc;
    case IGL_POS_RECTCLIENT:       // �����ڸ������е�λ��, ����RECT�ṹָ��, �������޸����ָ��, ֻ�ܻ�ȡ��������
        return (LONG_PTR)&pData->pos.rcClient;
    case IGL_POS_X:       // �����ڸ������е����λ��, ֻ�ܻ�ȡ��������
        return (LONG_PTR)pData->pos.x;
    case IGL_POS_Y:       // �����ڸ������еĶ���λ��, ֻ�ܻ�ȡ��������
        return (LONG_PTR)pData->pos.y;
    case IGL_POS_WIDTH:       // ���ڵĿ�� ֻ�ܻ�ȡ��������
        return (LONG_PTR)pData->pos.width;
    case IGL_POS_HEIGHT:       // ���ڵĸ߶�, ֻ�ܻ�ȡ��������
        return (LONG_PTR)pData->pos.height;
    default:
        break;
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �������໯���ص�ָ�������, ���������໯������Ϣ�ص�������ĵ��������, ��������ǰ������
// 
// ����:
//      ����1: ���໯���ص�ָ��, ���������໯������Ϣ�ص�������ĵ��������
//      ����2: ��������, ������ο� IGL_ ��ͷ����
//      ����3: Ҫ���õ�����
// 
// ����ֵ:
//      ��������ǰ������
EXTERN_C _declspec(dllexport) LONG_PTR WINAPI iDraw_SetLong(LPOBJSTRUCT pData, int index, LONG_PTR value)
{
    if (!pData)return 0;
    LONG_PTR oldData = 0;
    switch (index)
    {
    case IGL_HWND:       // ���ھ��
        return 0;
    case IGL_WNDPROC:       // �������໯�ĺ�����ַ, iDraw_SubclassWindow() �����ĵڶ���������¼����
        oldData = (LONG_PTR)pData->fnWndproc;
        pData->fnWndproc = (pfn_SubClassWndProc)value;
        break;
    case IGL_CALLBACK_NOTIFY:       // ֪ͨ��Ϣ����, �ɸ����ڵ���, ����ԭ�� LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
        oldData = (LONG_PTR)pData->fnNotify;
        pData->fnNotify = (pfn_SubClassWndProc)value;
        break;
    case IGL_CALLBACK_DRAWITEM:       // �滭��Ϣ����, �ɸ����ڵ���, ����ԭ�� LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
        oldData = (LONG_PTR)pData->fnDrawItem;
        pData->fnDrawItem = (pfn_SubClassWndProc)value;
        break;
    case IGL_CALLBACK_MEASUREITEM:       // ����ߴ���Ϣ����, �ɸ����ڵ���, ����ԭ�� LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
        oldData = (LONG_PTR)pData->fnMeasureItem;
        pData->fnMeasureItem = (pfn_SubClassWndProc)value;
        break;
    case IGL_CALLBACK_COMMAND:       // �¼���Ϣ����, �ɸ����ڵ���, ����ԭ�� LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
        oldData = (LONG_PTR)pData->fnCommand;
        pData->fnCommand = (pfn_SubClassWndProc)value;
        break;
    case IGL_CALLBACK_CTLCOLOR:       // ��ɫ��Ϣ����, �ɸ����ڵ���, ����ԭ�� LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
        oldData = (LONG_PTR)pData->fnCtlcolor;
        pData->fnCtlcolor = (pfn_SubClassWndProc)value;
        break;
    case IGL_CALLBACK_CALLWINDOWPROC:       // CallWindowProc ʹ��W�滹��A��, �ڵ������໯ʱ����ݴ����Ƿ�Ϊunicode���¼
        oldData = (LONG_PTR)pData->fnCallProc;
        pData->fnCallProc = (pfn_CallWindowProc)value;
        break;
    case IGL_INTERCEPT:       // �Ƿ����ش���Ϣ, �ҽ���Ϣʱ�Ż�ʹ�����ֵ
        oldData = (LONG_PTR)pData->isReturn;
        pData->isReturn = value;
        break;
    case IGL_OLDPROC:       // ���ڵ�ԭ�ص�����
        oldData = (LONG_PTR)pData->oldProc;
        pData->oldProc = (WNDPROC)value;
        break;
    case IGL_CANVAS:            // �봰�ڹ�����dx�滭���, ������ڲ���ʹ��dx��Ⱦ, ���ֵΪ0
        oldData = (LONG_PTR)pData->draw.hCanvas;
        pData->draw.hCanvas = (LPCANVAS)value;
        break;
    case IGL_USERDATA:          // �봰�ڹ������û�����
        oldData = (LONG_PTR)pData->userdata;
        pData->userdata = (LPVOID)value;
        break;

    case IGL_DRAW_NONCPAINT:       // ���ػ��ǿͻ���, Ϊtrueʱ�������໯��Ϣ�ĺ��������յ� WM_NCPAINT ��Ϣ
        oldData = (LONG_PTR)pData->draw.isNoNcPaint;
        pData->draw.isNoNcPaint = value;
        break;
    case IGL_DRAW_NOCALCSIZE:       // ������ǿͻ���, Ϊtrueʱ�������໯��Ϣ�ĺ��������յ� WM_NCCALCSIZE ��Ϣ, ���ڲ����б߿�
        oldData = (LONG_PTR)pData->draw.isNcCalcSize;
        pData->draw.isNcCalcSize = value;
        break;
    case IGL_DRAW_ISTRANSPARENT:       // �Ƿ�͸��, ���Ϊtrue, �����õ� CTLCOLOR �л���� SetMode() ����Ϊ͸��
        oldData = (LONG_PTR)pData->draw.isTransparent;
        pData->draw.isTransparent = value;
        break;
    case IGL_DRAW_ISHOT:       // �Ƿ������ȵ�, һ�����б���ص����ʹ��
        oldData = (LONG_PTR)pData->draw.isHot;
        pData->draw.isHot = value;
        break;
    case IGL_DRAW_ISCLIENT:       // �Ƿ�����������Ϣ����Ϊ�ͻ���, Ϊtrue�򲻻��յ� WM_NCHITTEST��Ϣ, �ڲ�ȫ������ HTCLIENT
        oldData = (LONG_PTR)pData->draw.isClient;
        pData->draw.isClient = value;
        break;
    case IGL_DRAW_ISERASEBKG:       // �Ƿ����ػ�������Ϣ, ���Ϊtrue, �򴰿ڻ��յ� WM_ERASEBACKGROUND ��Ϣ, �����ڲ�����1
        oldData = (LONG_PTR)pData->draw.isErasebkg;
        pData->draw.isErasebkg = value;
        break;
    case IGL_DRAW_STATE:       // ��ǰ�����״̬
        oldData = (LONG_PTR)pData->draw.state;
        pData->draw.state = (STATE)value;
        break;
    case IGL_DRAW_FONT_DX:       // ��ȡDX����
    case IGL_DRAW_FONT:       // ��ȡGDI����
        break;
    case IGL_DRAW_TEXTFORMAT:       // �滭�ı������ʽ
        oldData = (LONG_PTR)pData->draw.textFormat;
        pData->draw.textFormat = (DWORD)value;
        break;

    case IGL_DRAW_CRBACKGROUND:       // ������ɫ, ÿ���������ʹ�������ɫ���滭����
        oldData = (LONG_PTR)pData->draw.color.crBackGround;
        pData->draw.color.crBackGround = (COLORREF)value;
        break;
    case IGL_DRAW_CRTEXT:       // �ı���ɫ, ÿ������������ı���ɫ����ʹ�������ɫ�滭
        oldData = (LONG_PTR)pData->draw.color.crText;
        pData->draw.color.crText = (COLORREF)value;
        break;
    case IGL_DRAW_CRTEXTHOT:       // �ı��ȵ���ɫ, �ĸ�����õ���ο�����
        oldData = (LONG_PTR)pData->draw.color.crTextHot;
        pData->draw.color.crTextHot = (COLORREF)value;
        break;
    case IGL_DRAW_CRTEXTDOWN:       // �ı�������ɫ, �ĸ�����õ���ο�����
        oldData = (LONG_PTR)pData->draw.color.crTextDown;
        pData->draw.color.crTextDown = (COLORREF)value;
        break;
    case IGL_DRAW_CRTEXTBAN:       // �ı���ֹ��ɫ, �ĸ�����õ���ο�����
        oldData = (LONG_PTR)pData->draw.color.crTextBan;
        pData->draw.color.crTextBan = (COLORREF)value;
        break;
    case IGL_DRAW_CRNORMAL:       // ���������ɫ, �ĸ�����õ���ο�����
        oldData = (LONG_PTR)pData->draw.color.crNormal;
        pData->draw.color.crNormal = (COLORREF)value;
        break;
    case IGL_DRAW_CRHOVER:       // ���������ɫ, �ĸ�����õ���ο�����
        oldData = (LONG_PTR)pData->draw.color.crHover;
        pData->draw.color.crHover = (COLORREF)value;
        break;
    case IGL_DRAW_CRDOWN:       // ���������ɫ, �ĸ�����õ���ο�����
        oldData = (LONG_PTR)pData->draw.color.crDown;
        pData->draw.color.crDown = (COLORREF)value;
        break;
    case IGL_DRAW_CRFOCUS:       // ���������ɫ, �ĸ�����õ���ο�����
        oldData = (LONG_PTR)pData->draw.color.crFocus;
        pData->draw.color.crFocus = (COLORREF)value;
        break;
    case IGL_DRAW_CRCHECKED:       // ���ѡ����ɫ, �ĸ�����õ���ο�����
        oldData = (LONG_PTR)pData->draw.color.crChecked;
        pData->draw.color.crChecked = (COLORREF)value;
        break;
    case IGL_DRAW_CRSELECTED:       // ���ѡ����ɫ, �ĸ�����õ���ο�����
        oldData = (LONG_PTR)pData->draw.color.crSelected;
        pData->draw.color.crSelected = (COLORREF)value;
        break;
    case IGL_DRAW_CRHOT:       // ����ȵ���ɫ, �ĸ�����õ���ο�����
        oldData = (LONG_PTR)pData->draw.color.crHot;
        pData->draw.color.crHot = (COLORREF)value;
        break;
    case IGL_DRAW_CRBAN:       // �����ֹ��ɫ, �ĸ�����õ���ο�����
        oldData = (LONG_PTR)pData->draw.color.crBan;
        pData->draw.color.crBan = (COLORREF)value;
        break;
    case IGL_DRAW_CRVISTED:       // 
        oldData = (LONG_PTR)pData->draw.color.crVisted;
        pData->draw.color.crVisted = (COLORREF)value;
        break;
    case IGL_DRAW_CRSHADOW:       // ���������ɫ, �ĸ�����õ���ο�����
        oldData = (LONG_PTR)pData->draw.color.crShadow;
        pData->draw.color.crShadow = (COLORREF)value;
        break;
    case IGL_DRAW_CRBORDER:       // ����߿���ɫ, ���л��Ʊ߿�ʹ�õ���ɫ
        oldData = (LONG_PTR)pData->draw.color.crBorder;
        pData->draw.color.crBorder = (COLORREF)value;
        break;

    case IGL_DRAW_HBRBACKGROUND:       // ����GDI��ˢ, ÿ���������ʹ�������ˢ����䱳��
        oldData = (LONG_PTR)pData->draw.color.hbrBackGround;
        pData->draw.color.hbrBackGround = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRTEXT:       // �ı�GDI��ˢ
        oldData = (LONG_PTR)pData->draw.color.hbrText;
        pData->draw.color.hbrText = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRTEXTHOT:       // �ı��ȵ�GDI��ˢ, �ĸ�����õ���ο�����
        oldData = (LONG_PTR)pData->draw.color.hbrTextHot;
        pData->draw.color.hbrTextHot = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRTEXTDOWN:       // �ı�����GDI��ˢ, �ĸ�����õ���ο�����
        oldData = (LONG_PTR)pData->draw.color.hbrTextDown;
        pData->draw.color.hbrTextDown = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRTEXTBAN:       // �ı���ֹGDI��ˢ, �ĸ�����õ���ο�����
        oldData = (LONG_PTR)pData->draw.color.hbrTextBan;
        pData->draw.color.hbrTextBan = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRNORMAL:       // �������GDI��ˢ, �ĸ�����õ���ο�����
        oldData = (LONG_PTR)pData->draw.color.hbrNormal;
        pData->draw.color.hbrNormal = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRHOVER:       // �������GDI��ˢ, �ĸ�����õ���ο�����
        oldData = (LONG_PTR)pData->draw.color.hbrHover;
        pData->draw.color.hbrHover = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRDOWN:       // �������GDI��ˢ, �ĸ�����õ���ο�����
        oldData = (LONG_PTR)pData->draw.color.hbrDown;
        pData->draw.color.hbrDown = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRFOCUS:       // �������GDI��ˢ, �ĸ�����õ���ο�����
        oldData = (LONG_PTR)pData->draw.color.hbrFocus;
        pData->draw.color.hbrFocus = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRCHECKED:       // ���ѡ��GDI��ˢ, �ĸ�����õ���ο�����
        oldData = (LONG_PTR)pData->draw.color.hbrChecked;
        pData->draw.color.hbrChecked = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRSELECTED:       // ���ѡ��GDI��ˢ, �ĸ�����õ���ο�����
        oldData = (LONG_PTR)pData->draw.color.hbrSelected;
        pData->draw.color.hbrSelected = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRHOT:       // ����ȵ�GDI��ˢ, �ĸ�����õ���ο�����
        oldData = (LONG_PTR)pData->draw.color.hbrHot;
        pData->draw.color.hbrHot = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRBAN:       // �����ֹGDI��ˢ, �ĸ�����õ���ο�����
        oldData = (LONG_PTR)pData->draw.color.hbrBan;
        pData->draw.color.hbrBan = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRVISTED:       // 
        oldData = (LONG_PTR)pData->draw.color.hbrVisted;
        pData->draw.color.hbrVisted = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRSHADOW:       // �����ӰGDI��ˢ, �ĸ�����õ���ο�����
        oldData = (LONG_PTR)pData->draw.color.hbrShadow;
        pData->draw.color.hbrShadow = (HBRUSH)value;
        break;
    case IGL_DRAW_HBRBORDER:       // ����߿�GDI����, ���л��Ʊ߿�ʹ�õĻ���
        oldData = (LONG_PTR)pData->draw.color.hpenBorder;
        pData->draw.color.hpenBorder = (HPEN)value;
        break;

    case IGL_POS_RECTSCREEN:    // ��������Ļ�е�λ��, ����RECT�ṹָ��, �������޸����ָ��, ֻ�ܻ�ȡ��������
    case IGL_POS_RECTCLIENT:    // �����ڸ������е�λ��, ����RECT�ṹָ��, �������޸����ָ��, ֻ�ܻ�ȡ��������
    case IGL_POS_X:             // �����ڸ������е����λ��, ֻ�ܻ�ȡ��������
    case IGL_POS_Y:             // �����ڸ������еĶ���λ��, ֻ�ܻ�ȡ��������
    case IGL_POS_WIDTH:         // ���ڵĿ�� ֻ�ܻ�ȡ��������
    case IGL_POS_HEIGHT:        // ���ڵĸ߶�, ֻ�ܻ�ȡ��������
        break;
    default:
        break;
    }
    return oldData;
}

#endif