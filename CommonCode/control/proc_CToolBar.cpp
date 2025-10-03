#include "CToolBar.h"
#include <windowsx.h>
#include <MemDC.h>

LRESULT CALLBACK ToolWndProc(LPSUBCLASSSTRUCT pData, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		s_hWnd = pData->hWnd;
		s_toolData.hWnd = pData->hWnd;
		LPCREATESTRUCTW pos = (LPCREATESTRUCTW)lParam;
		s_toolData.pos.height = pos->cx;
		s_toolData.pos.width = pos->cy;
		s_toolData.pos.x = pos->x;
		s_toolData.pos.y = pos->y;
		s_toolData.pos.rc.right = pos->x + pos->cx;
		s_toolData.pos.rc.bottom = pos->y + pos->cy;

		s_sub.resize(1);
		wstr s(L"测试工具条");
		s.Addref();
		TOOLCHILD& a = s_sub[0];
		a.rc = { 0,0,80,80 };
		a.state = STATE::NORMAL;
		a.text = s.data();

		break;
	}
	case WM_SIZE:
	{
		s_toolData.pos.height = LOWORD(lParam);
		s_toolData.pos.width = HIWORD(lParam);
		s_toolData.pos.rc.right = s_toolData.pos.height;
		s_toolData.pos.rc.bottom = s_toolData.pos.width;
		break;
	}
	case WM_PAINT:
		return OnPaint(pData, message, wParam, lParam);
	case WM_MOUSEMOVE:
		return OnMouseMove(pData, message, wParam, lParam);
	case WM_MOUSEHOVER:
		break;
	case WM_MOUSELEAVE:
	{
		if (QueryUiState(STATE::HOVER))
		{
			SetUiState(STATE::HOVER, true, false);
			__set(s_indexHot, STATE::HOT, true, true);
			s_indexHot = -1;
			return 0;
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		s_indexDown = s_indexHot;
		s_ptDown = lParam;
		__set(s_indexHot, STATE::DOWN, false, true);
		SetCapture(pData->hWnd);
		break;
	}
	case WM_LBUTTONUP:
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		// 鼠标放开的位置还是在按下的按钮上,触发个单击事件
		if (s_indexDown != -1 && pt2index(pt, false) == s_indexDown)
		{
			bool isBan = __query(s_indexDown, STATE::BAN);
			if (!isBan)	// 不是禁止状态才触发单击事件
			{
				// TODO 触发单击事件
			}
			// TODO 判断按钮类型, 是选择按钮就改变选中状态
		}
		s_indexDown = -1;
		__set(s_indexHot, STATE::DOWN, true, true);
		ReleaseCapture();
		return 0;
	}
	default:
		return CallWindowProcW(pData->oldProc, pData->hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CToolBar::OnPaint(LPSUBCLASSSTRUCT pData, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	BeginPaint(pData->hWnd, &ps);
	const RECT& rc = s_toolData.pos.rc;
	int cx = rc.right - rc.left;
	int cy = rc.bottom - rc.top;
	CMemDC dc;
	Gdiplus::Graphics* gp = 0;
	HDC hDCMem = dc.create(cx, cy, &gp);
	FillRect(hDCMem, &rc, GetStockBrush(WHITE_BRUSH));
	size_t len = s_sub.size();
	size_t first = GetFirstVisible();
	if (len && first == -1)
		assert(0);
	if (len == 0 || first == -1)
	{
		dc.show(ps.hdc, 0, 0, cx, cy);
		EndPaint(pData->hWnd, &ps);
		return 0;
	}
	int width = s_sub[0].rc.left - 2;
	int height = cy;

	for (size_t i = first; i < len; i++)
	{
		if (drawitem(hDCMem, gp, i))
			break;
	}
	dc.show(ps.hdc, rc);

	EndPaint(pData->hWnd, &ps);
	return 0;
}

LRESULT CToolBar::OnMouseMove(LPSUBCLASSSTRUCT pData, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (s_sub.empty())return 0;
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	if (!QueryUiState(STATE::HOVER))	// 没有进入的状态
	{
		SetUiState(STATE::HOVER, false, false);
		TRACKMOUSEEVENT tm;
		tm.cbSize = sizeof(tm);
		tm.dwHoverTime = 10;
		tm.hwndTrack = pData->hWnd;
		tm.dwFlags = TME_HOVER | TME_LEAVE;
		TrackMouseEvent(&tm);
	}
	size_t i = 0;
	size_t len = s_sub.length();
	for (i = 0; i < len; i++)
	{
		if (PtInRect(&s_sub[i].rc, pt))
		{
			__set(s_indexHot, STATE::HOT, true, false);
			__set(i, STATE::HOT, false, true);
			s_indexHot = i;
		}
	}
	if (i == len)	// 鼠标没在任何按钮上
	{
		if (s_indexHot != -1)
		{
			__set(s_indexHot, STATE::HOT, true, true);
			s_indexHot = -1;
			return 0;
		}
	}
	return CallWindowProcW(pData->oldProc, pData->hWnd, message, wParam, lParam);
}

BOOL CToolBar::drawitem(HDC hdc, Gdiplus::Graphics* gp, size_t i)
{
	static Gdiplus::StringFormat* format;
	static Gdiplus::Pen* pen;
	if (format == 0)
	{
		pen = new Gdiplus::Pen(Gdiplus::Color(180, 128, 128, 128));
		format = new Gdiplus::StringFormat;
		format->SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
		format->SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
	}
	RECT rc = s_sub[i].rc;
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;
	BOOL b = 0;
	rc.left -= (int)s_pos;
	rc.right = rc.left + width;
	b = (rc.bottom >= (LONG)s_toolData.pos.width);

	gp->DrawRectangle(pen, rc.left, rc.top, width, height);
	if (s_sub[i].text)
	{
		DrawTextW(hdc, s_sub[i].text, -1, &rc, DT_CENTER);
	}
	return b;
}

size_t CToolBar::pt2index(const POINT& pt, bool b)
{
	int x = pt.x;
	if (!b)x += (int)s_pos;

	size_t len = s_sub.length();
	for (size_t i = 0; i < len; i++)
	{
		if (i == len - 1)
		{
			if (x >= s_sub[i].rc.left)return i;
			continue;
		}
		if (x >= s_sub[i].rc.left && x < s_sub[i + 1].rc.left)
			return i;
	}

	for (size_t i = 0; i < len; i++)
	{
		if (PtInRect(&s_sub[i].rc, pt))
		{
			//TODO 判断工具条按钮类型, 如果是分隔条就不处理
			return i;
		}
	}
	return -1;
}

bool CToolBar::__query(size_t index, STATE state)
{
	if (s_sub.empty() || index >= s_sub.length()) return false;
	return (s_sub[index].state & state) != 0;
}

bool CToolBar::__set(size_t index, STATE state, BOOL isRemove, BOOL isUpdate)
{
	size_t len = s_sub.length();
	if (index >= len)return false;
	TOOLCHILD& data = s_sub[index];
	if (isRemove)
	{
		data.state = (STATE)(data.state | state);
	}
	else
	{
		data.state = (STATE)(data.state & (~state));
	}
	if (isUpdate)
		InvalidateRect(s_hWnd, 0, 0);
	return true;
}

bool CToolBar::SetUiState(STATE state, BOOL isRemove, BOOL isUpdate)
{
	STATE old = s_toolData.draw.state;
	if (isRemove)
	{
		s_toolData.draw.state = (STATE)(s_toolData.draw.state | state);
	}
	else
	{
		s_toolData.draw.state = (STATE)(s_toolData.draw.state & (~state));
	}
	if (isUpdate)
		InvalidateRect(s_toolData.hWnd, 0, 0);
	return old;
}

bool CToolBar::QueryUiState(STATE state)
{
	return (s_toolData.draw.state & state) != 0;
}

size_t CToolBar::GetFirstVisible()
{
	size_t len = s_sub.length();
	if (len == 0) return -1;
	if (s_pos < s_sub[0].rc.right)
		return 0;
	for (size_t i = 0; i < len; i++)
	{
		if (i == len - 1)
		{
			if (s_pos >= s_sub[i].rc.left)
				return i;
			continue;

		}
		if (s_pos >= s_sub[i].rc.left && s_pos < s_sub[i + 1].rc.left)
			return i;
	}
	if (s_pos <= s_sub[0].rc.right) return 0;
	if (s_pos >= s_sub[len - 1].rc.left) return len - 1;
	DWORD cx = s_toolData.pos.width;
	if (s_pos + cx > s_sub[len - 1].rc.left)
	{
		POINT pt = { 1, 1 };
		return pt2index(pt, false);
	}
	assert(false);	// 走到这是出错
	return -1;
}

LPOBJSTRUCT CToolBar::create(HWND hParent)
{
	static HINSTANCE hInst;
	if (hInst == 0)
	{
		hInst = GetModuleHandleW(0);
		WNDCLASSEXW wcex = { 0 };
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInst;
		wcex.hIcon = 0;
		wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;

		//GetClassInfoExW(hInst, L"static", &wcex);
		LPSUBCLASSSTRUCT newProc = MakeProc(this, &CToolBar::WndProc, &s_toolData);
		//newProc->oldProc = wcex.lpfnWndProc;
		newProc->oldProc = DefWindowProcW;
		wcex.lpfnWndProc = (WNDPROC)newProc;
		wcex.hInstance = hInst;
		wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		wcex.lpszClassName = L"tool";

		ATOM atom = RegisterClassExW(&wcex);
		if (!atom)return 0;
	}
	
	HWND hWnd = __create(0, L"tool", 0,
		WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_OWNERDRAW,
		0, 30, 0, 80, hParent, 0, hInst, 0);

	return &s_toolData;
}
