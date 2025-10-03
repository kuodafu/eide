#include "../EMenu.h"
#include "../EWindow.h"


void EWindow_OnPaint_DrawTitle(LPCANVAS hCanvas, LPOBJSTRUCT pData, LPCWSTR const titleMid)
{
    RECT_F rcTitle(m_rcTitle);
    THEME_D2D_INFO& d2d = s_info->window_d2d_info;
    _canvas_fillRectangle(hCanvas, d2d.hbrTitleback, &rcTitle);

    size_t npos = m_eCodeShowName.rfind(titleMid);
    LPCWSTR text = m_eCodeShowName.c_str();
    if (!text)return;
    rcTitle.left += 10;
    int oldMode = 0;

    LPEX_BRUSH hbrText      = s_info->isActivation ? d2d.hbrTitle_left_act  : d2d.hbrTitle_left;
    LPEX_BRUSH hbrTextRight = s_info->isActivation ? d2d.hbrTitle_right_act : d2d.hbrTitle_right;

    LPEX_FONT hFont = g_scale.GetFont(d2d.hFont);
    LPEX_FONT hFont1 = g_scale.GetFont(d2d.hFont1);
    const int fmt = DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;
    if (npos == wstr::npos)
    {
        // 常规处理
        if (text[0] == '*' && text[1] == ' ')
        {
            RECT_F rcText;
            _canvas_drawtextex(hCanvas, hFont, hbrText,
                text, 2, fmt, &rcTitle, 0, 0, 0, 0, &rcText);

            if (rcText.right == 0)
                return;
            rcTitle.left = rcText.right;
            _canvas_drawtext(hCanvas, hFont, hbrTextRight,
                text + 2, m_eCodeShowName.size() - 2, fmt, &rcTitle);
        }
        else
        {
            _canvas_drawtext(hCanvas, hFont, hbrTextRight,
                text, m_eCodeShowName.size(), fmt | DT_CENTER, &rcTitle);
        }
    }
    else
    {
        // 特殊处理, 前面是源码名, 后面是程序类型, 程序类型字体得调整
        RECT_F rcText;
        float textWidth = 0;
        float textHeight = 0;
        _canvas_calctextsize(hCanvas, hFont1, text + npos, -1, 0, 1000, 100, &textWidth, &textHeight);
        rcTitle.right -= textWidth;

        if (rcTitle.right > rcTitle.left + 20)     // 剩余的宽度足够存放程序类型, 不够的话只绘画源码名
        {
            _canvas_drawtextex(hCanvas, hFont, hbrText,
                text, npos, fmt, &rcTitle, 0, 0, 0, 0, &rcText);

            _canvas_drawtextex(hCanvas, hFont, hbrText,
                text, npos, fmt, &rcTitle, 0, 0, 0, 0, &rcText);

            rcTitle.left = rcText.right;
            rcTitle.right = (float)m_rcTitle.right;
            _canvas_drawtextex(hCanvas, hFont1, hbrText,
                text + npos, 3, fmt, &rcTitle, 0, 0, 0, 0, &rcText);

            rcTitle.left = rcText.right;
            _canvas_drawtext(hCanvas, hFont1, hbrTextRight,
                text + npos + 3, -1, fmt, &rcTitle);
        }
        else
        {
            // 标题的宽度只够绘画文件名
            rcTitle.left = (float)m_rcTitle.left;
            rcTitle.right = (float)m_rcTitle.right;
            _canvas_drawtextex(hCanvas, hFont, hbrText, text, npos, fmt | DT_CENTER, &rcTitle, 0, 0, 0, 0, &rcText);
        }
    }
}