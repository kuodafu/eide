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
		memcpy_s(lf.lfFaceName, sizeof(lf.lfFaceName), L"微软雅黑", 10);
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

// 窗口基类的注册类名, 超类化, 新窗口消息过程指向 this->WndProc
// 两个类名一样, 或者旧类名为空, 则直接注册新类名
LPCWSTR CWndBase::RegClass(LPCWSTR oldClassName, LPCWSTR newClassName)
{
	if (!newClassName)return newClassName;
	WNDCLASSEXW wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEXW);
	if (oldClassName)	// 旧类名不为空才获取类信息, 不为空表示需要超类化
	{
		if (GetClassInfoExW(m_hInst, oldClassName, &wcex))
		{
			// 获取类信息成功表示已经注册, 并且新旧类名相等, 直接返回新类名
			if (wcscmp(oldClassName, newClassName) == 0)
				return newClassName;
			// 旧类名不等于新类名, 需要注册新类名
		}
		else
		{
			// 类信息获取失败, 表示类没有注册, 并且两个类名不一样就返回, 一样的话走下面注册
			if (wcscmp(oldClassName, newClassName) != 0)
				return newClassName;
			// 新旧类名不相等, 注册一个新类名, 旧类名不处理
		}
	}

	// 判断新类名是否已注册
	{
		WNDCLASSEXW newWcex = { 0 };
		newWcex.cbSize = sizeof(WNDCLASSEXW);
		if (GetClassInfoExW(m_hInst, newClassName, &newWcex))	// 取出新类名信息
		{
			return newClassName;	// 获取成功表示已经注册
		}
		// 取出失败就是没注册, 继续往下走
	}

	int err = GetLastError();

	WNDPROC oldProc = wcex.lpfnWndProc;
	if (!oldProc)oldProc = DefWindowProcW;	// 如果是直接注册, 那原回调地址就是默认回调
	wcex.lpfnWndProc = (WNDPROC)MakeProc(this, &CWndBase::WndProc, 0);
	wcex.lpszClassName = newClassName;
	ATOM atom = RegisterClassExW(&wcex);
	err = GetLastError();
	if (atom == 0) throw;
	return newClassName;
}