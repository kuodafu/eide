#include "WndBase.h"

class __wnd_init_data
{
public:
	HFONT hFont;
	__wnd_init_data() :hFont(0) { ; }
	void init()
	{
		LOGFONTW lf = { 0 };
		SystemParametersInfoW(SPI_GETICONTITLELOGFONT, sizeof(LOGFONTW), &lf, 0);
		lf.lfHeight = -12;
		memcpy_s(lf.lfFaceName, sizeof(lf.lfFaceName), L"΢���ź�", 10);
		hFont = CreateFontIndirectW(&lf);
	}
	~__wnd_init_data()
	{
		if (hFont)DeleteObject(hFont);
		hFont = 0;
	}
};
HINSTANCE CWndBase::m_hInst;
HFONT CWndBase::m_hFont;
static __wnd_init_data s_init_data;

CWndBase::CWndBase():m_hObj(0)
{
	if (m_hInst == 0)
	{
		s_init_data.init();
		m_hInst = GetModuleHandleW(0);
		m_hFont = s_init_data.hFont;
	}
}

CWndBase::~CWndBase()
{
}

bool CWndBase::SetUiState(STATE state, BOOL isRemove, BOOL isUpdate)
{
	if (!m_hObj)return false;
	STATE old = m_hObj->draw.state;
	if (isRemove)
	{
		m_hObj->draw.state = (STATE)(m_hObj->draw.state | state);
	}
	else
	{
		m_hObj->draw.state = (STATE)(m_hObj->draw.state & (~state));
	}
	if (isUpdate)
		InvalidateRect(m_hObj->hWnd, 0, 0);
	return old;
}

bool CWndBase::QueryUiState(STATE state)
{
	if (!m_hObj)return false;
	return (m_hObj->draw.state & state) != 0;
}

// ���ڻ����ע������, ���໯, �´�����Ϣ����ָ�� this->WndProc
// ��������һ��, ���߾�����Ϊ��, ��ֱ��ע��������
LPCWSTR CWndBase::RegClass(LPCWSTR oldClassName, LPCWSTR newClassName)
{
	if (!newClassName)return newClassName;
	WNDCLASSEXW wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEXW);
	if (oldClassName)	// ��������Ϊ�ղŻ�ȡ����Ϣ, ��Ϊ�ձ�ʾ��Ҫ���໯
	{
		if (GetClassInfoExW(m_hInst, oldClassName, &wcex))
		{
			// ��ȡ����Ϣ�ɹ���ʾ�Ѿ�ע��, �����¾��������, ֱ�ӷ���������
			if (wcscmp(oldClassName, newClassName) == 0)
				return newClassName;
			// ������������������, ��Ҫע��������
		}
		else
		{
			// ����Ϣ��ȡʧ��, ��ʾ��û��ע��, ��������������һ���ͷ���, һ���Ļ�������ע��
			if (wcscmp(oldClassName, newClassName) != 0)
				return newClassName;
			// �¾����������, ע��һ��������, ������������
		}
	}

	// �ж��������Ƿ���ע��
	{
		WNDCLASSEXW newWcex = { 0 };
		newWcex.cbSize = sizeof(WNDCLASSEXW);
		if (GetClassInfoExW(m_hInst, newClassName, &newWcex))	// ȡ����������Ϣ
		{
			return newClassName;	// ��ȡ�ɹ���ʾ�Ѿ�ע��
		}
		// ȡ��ʧ�ܾ���ûע��, ����������
	}

	int err = GetLastError();

	WNDPROC oldProc = wcex.lpfnWndProc;
	if (!oldProc)oldProc = DefWindowProcW;	// �����ֱ��ע��, ��ԭ�ص���ַ����Ĭ�ϻص�
	wcex.lpfnWndProc = (WNDPROC)MakeProc(this, &CWndBase::WndProc, 0);
	wcex.lpszClassName = newClassName;
	ATOM atom = RegisterClassExW(&wcex);
	err = GetLastError();
	if (atom == 0) throw;
	return newClassName;
}