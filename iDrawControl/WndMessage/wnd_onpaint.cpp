#include "wnd_header.h"


LRESULT WndProc_OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
	PAINTSTRUCT ps;
	BeginPaint(pData->hObj, &ps);
	Gdiplus::Graphics* gp = 0;
	LPSYSBTN sysBtn = (LPSYSBTN)pData->param;
	const RECT& rc = pData->pos.rcClient;
	//HDC hDCMem = dc.create(rc.right - rc.left, rc.bottom - rc.top, &gp);
	LPCANVAS hCanvas = pData->draw.hCanvas;
	_canvas_begindraw(hCanvas);
	_canvas_clear(hCanvas, ARGB_D2D(255, 255, 255, 255));

#ifdef _DEBUG
	LPEX_PEN hPen = _pen_create(ARGB_D2D(255, 255, 0, 0), 2);
	LPEX_BRUSH brush = _brush_create(ARGB_D2D(255, 255, 0, 0));
	// 整个客户区位置
	RECT_F rcTmp = pData->pos.rcClient;
	_canvas_drawRectangle(hCanvas, hPen, &rcTmp);

	// 程序图标画出位置
	rcTmp = sysBtn->rcIcon;
	_canvas_drawRectangle(hCanvas, hPen, &rcTmp);

	// 标题画出位置
	rcTmp = sysBtn->rcCaption;
	_canvas_drawRectangle(hCanvas, hPen, &rcTmp);
	rcTmp.offset(5, 0);
	_canvas_drawtext(hCanvas, pData->draw.hFont, brush, L"标题栏画出位置", 0, DT_VCENTER | DT_SINGLELINE, &rcTmp);

	// 系统按钮绘画位置
	LPEX_BRUSH brush1 = _brush_create(ARGB_D2D(255, 128, 128, 128));
	for (DWORD i = 0; i < sysBtn->count; i++)
	{
		RECT_F rcSysBtn = sysBtn->btn[i].rc;
		_canvas_fillRectangle(hCanvas, brush1, &rcSysBtn);
		_canvas_drawRectangle(hCanvas, hPen, &rcSysBtn);
	}
	_brush_destroy(brush1);
	_brush_destroy(brush);
	_pen_destroy(hPen);
#endif

	for (DWORD i = 0; i < sysBtn->count; i++)
	{
		RECT_F rc = sysBtn->btn[i].rc;
		DWORD id = sysBtn->btn[i].ID;
		if (sysBtn->btn[i].flag == SC_RESTORE)
			id = SYSBTN_RESTORE;
		DWORD index = id * 4;

		if (i == sysBtn->index)
		{
			if (__query(sysBtn->state, STATE::HOT))
				index++;
			else if (__query(sysBtn->state, STATE::DOWN))
				index += 2;
			else if (__query(sysBtn->state, STATE::BAN))
				index += 3;
		}
		assert(index < 48);
		//const RECT_F& rcImg = sysBtn->image->rc[index];
		//_canvas_drawimagerectrect(hCanvas, sysBtn->pImage,
		//	rc.left, rc.top, rc.right, rc.bottom,
		//	rcImg.left, rcImg.top, rcImg.right, rcImg.bottom, 255);
	}
	HDC hdc = _canvas_getdc(hCanvas);
	assert(hdc != 0);
	BitBlt(ps.hdc, 0, 0, hCanvas->GetWidth(), hCanvas->GetHeight(), hdc, 0, 0, SRCCOPY);
	_canvas_releasedc(hCanvas);
	bool bdraw = _canvas_enddraw(hCanvas);
	assert(bdraw);
	if (!bdraw && ExGetLastError() == D2DERR_RECREATE_TARGET)
	{
		// 需要清除原有资源并重新创建资源, DiscardDeviceResources();
	}
	EndPaint(pData->hObj, &ps);
	return 0;
}
