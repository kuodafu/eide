#include "../EWindow_Fne_Header.h"
LRESULT CALLBACK WndProc_Menu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);


LRESULT CALLBACK WndProc_Menu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
    case WM_CREATE:
        break;
    default:
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}
