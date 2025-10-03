#include "wnd_header.h"


LRESULT WndProc_OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
	PAINTSTRUCT ps;
	BeginPaint(pData->hObj, &ps);
	Gdiplus::Graphics* gp = 0;
	LPSYSBTN sysBtn = (LPSYSBTN)pData->param;
	const RECT& rc = pData->pos.rcClient;
	//HDC hDCMem = dc.create(rc.right - rc.left, rc.bottom - rc.top, &gp);
	LPCANVAS& hCanvas = pData->draw.hCanvas;
	_canvas_begindraw(hCanvas);
	_canvas_clear(hCanvas, ARGB_D2D(255, 255, 255, 255));

#ifdef _DEBUG
	LPBRUSHPEN brush = _brush_create(ARGB_D2D(255, 255, 0, 0));
	// �����ͻ���λ��
	RECT_F rcTmp = pData->pos.rcClient;
	_canvas_drawRectangle(hCanvas, brush, &rcTmp, 2);

	// ����ͼ�껭��λ��
	rcTmp = sysBtn->rcIcon;
	_canvas_drawRectangle(hCanvas, brush, &rcTmp, 2);

	// ���⻭��λ��
	rcTmp = sysBtn->rcCaption;
	_canvas_drawRectangle(hCanvas, brush, &rcTmp, 2);
	rcTmp.offset(5, 0);
	_canvas_drawtext(hCanvas, pData->draw.hFont, brush, L"����������λ��", 0, DT_VCENTER | DT_SINGLELINE, &rcTmp);

	// ϵͳ��ť�滭λ��
	LPBRUSHPEN brush1 = _brush_create(ARGB_D2D(255, 128, 128, 128));
	for (DWORD i = 0; i < sysBtn->count; i++)
	{
		RECT_F rcSysBtn = sysBtn->btn[i].rc;
		_canvas_fillRectangle(hCanvas, brush1, &rcSysBtn);
		_canvas_drawRectangle(hCanvas, brush, &rcSysBtn, 2);
	}
	_brush_destroy(brush1);
	_brush_destroy(brush);
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
		const RECT_F& rcImg = sysBtn->image->rc[index];
		_canvas_drawimage(hCanvas, sysBtn->pImage,
			rc.left, rc.top, rc.right, rc.bottom,
			rcImg.left, rcImg.top, rcImg.right, rcImg.bottom, 255);
	}
	HDC hdc = _canvas_getdc(hCanvas);
	assert(hdc != 0);
	BitBlt(ps.hdc, 0, 0, hCanvas->width, hCanvas->height, hdc, 0, 0, SRCCOPY);
	_canvas_releasedc(hCanvas);
	bool bdraw = _canvas_enddraw(hCanvas);
	assert(bdraw);
	if (!bdraw && ExGetLastError() == D2DERR_RECREATE_TARGET)
	{
		// ��Ҫ���ԭ����Դ�����´�����Դ, DiscardDeviceResources();
	}
	EndPaint(pData->hObj, &ps);
	return 0;
}
