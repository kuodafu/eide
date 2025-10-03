#pragma once
#include "EWindow_Fne_Header.h"
#include "EWindow.h"
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include <assist/assist_Resource.h>

LRESULT _draw_top_state_lbuttondown     (LPOBJSTRUCT pData, LPTOPSTATE_INFO info, WPARAM wParam, int x, int y);
LRESULT _draw_top_state_lbuttonup       (LPOBJSTRUCT pData, LPTOPSTATE_INFO info, WPARAM wParam, int x, int y);
LRESULT _draw_top_state_capture_changed (LPOBJSTRUCT pData, LPTOPSTATE_INFO info, WPARAM wParam, LPARAM lParam);
LRESULT _draw_top_state_move            (LPOBJSTRUCT pData, LPTOPSTATE_INFO info, WPARAM wParam, int x, int y);
