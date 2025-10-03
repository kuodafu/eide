#ifndef CSWMenu_H
#define CSWMenu_H
#include <afxtempl.h>

namespace sw {
    class CSWMenuData
    {
        wchar_t* m_szMenuText;
    public:
        CSWMenuData() {
            menuIconNormal = -1;
            xoffset = -1;
            bitmap = NULL;
            pContext = NULL;
            nFlags = 0;
            nID = 0;
            syncflag = 0;
            m_szMenuText = NULL;
            global_offset = -1;
        };
        void SetString(LPCTSTR szWideString);
        CString GetString();
        ~CSWMenuData();
        int xoffset, global_offset;
        int menuIconNormal;
        UINT nFlags, nID, syncflag;
        CImageList* bitmap;
        void* pContext;
    };
    struct CMenuItemInfo : public
#ifndef UNICODE
        tagMENUITEMINFOA
#else
        tagMENUITEMINFOW
#endif
    {
        CMenuItemInfo()
        {
            memset(this, 0, sizeof(MENUITEMINFO));
            cbSize = sizeof(MENUITEMINFO);
        }
    };
    typedef enum { CSWMenu_DRAWMODE_ORIGINAL, CSWMenu_DRAWMODE_XP } BC_MenuDrawMode;
    typedef enum { CSWMenu_NONE, CSWMenu_HEAD, CSWMenu_TAIL, CSWMenu_BOTH } BC_Seperator;
    class CSWMenu : public CMenu
    {
        DECLARE_DYNAMIC(CSWMenu)
    public:
        CSWMenu();
        virtual ~CSWMenu();
        // 加载菜单
        virtual BOOL LoadMenu(LPCTSTR lpszResourceName);
        virtual BOOL LoadMenu(int nResource);
        // 销毁菜单
        virtual BOOL DestroyMenu();
        // 菜单重绘
        virtual void DrawItem(LPDRAWITEMSTRUCT);
        // 菜单项高度重置
        virtual void MeasureItem(LPMEASUREITEMSTRUCT);
        // 从工具栏加载资源
        BOOL LoadToolbar(UINT nToolBar);
        BOOL LoadToolbars(const UINT* arID, int n);
        BOOL LoadFromToolBar(UINT nID, UINT nToolBar, int& xoffset);
        void AddFromToolBar(CToolBar* pToolBar, int nResourceID);
        BOOL AddBitmapToImageList(CImageList* list, UINT nResourceID);
        void LoadCheckmarkBitmap(int unselect, int select); // custom check mark bitmaps
        // 添加菜单项
        BOOL AppendMenu(UINT nFlags, UINT nIDNewItem = 0, TCHAR* lpszNewItem = NULL, int nIconNormal = -1);
        BOOL AppendMenu(UINT nFlags, UINT nIDNewItem, TCHAR* lpszNewItem, CImageList* il, int xoffset);
        BOOL AppendMenu(UINT nFlags, UINT nIDNewItem, TCHAR* lpszNewItem, CBitmap* bmp);
        BOOL AppendODMenu(TCHAR* lpstrText, UINT nFlags = MF_OWNERDRAW, UINT nID = 0, int nIconNormal = -1);
        BOOL AppendODMenu(TCHAR* lpstrText, UINT nFlags, UINT nID, CImageList* il, int xoffset);
        BOOL AppendMenu(CSWMenu* pMenuToAdd, BOOL add_separator = true, int num_items_to_remove_at_end = 0);
        CSWMenu* AppendODPopupMenu(TCHAR* lpstrText);
        // 插入菜单项
        BOOL InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem = 0, TCHAR* lpszNewItem = NULL, int nIconNormal = -1);
        BOOL InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem, TCHAR* lpszNewItem, CImageList* il, int xoffset);
        BOOL InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem, TCHAR* lpszNewItem, CBitmap* bmp);
        BOOL InsertODMenu(UINT nPosition, TCHAR* lpstrText, UINT nFlags = MF_OWNERDRAW, UINT nID = 0, int nIconNormal = -1);
        BOOL InsertODMenu(UINT nPosition, TCHAR* lpstrText, UINT nFlags, UINT nID, CImageList* il, int xoffset);
        // 修改菜单项
        BOOL ModifyMenu(TCHAR* lpstrText, UINT nID = 0, int nIconNormal = -1);
        BOOL ModifyMenu(TCHAR* lpstrText, UINT nID, CImageList* il, int xoffset);
        BOOL ModifyMenu(TCHAR* lpstrText, UINT nID, CBitmap* bmp);
        BOOL ModifyMenu(TCHAR* lpstrText, TCHAR* OptionText, int nIconNormal);
        BOOL ModifyMenu(TCHAR* lpstrText, UINT nID, COLORREF fill, COLORREF border, int hatchstyle = -1, CSize* pSize = NULL);

        BOOL SetImageForPopupFromToolbar(TCHAR* strPopUpText, UINT toolbarID, UINT command_id_to_extract_icon_from);

        BOOL RemoveMenu(UINT uiId, UINT nFlags);
        int RemoveMenu(TCHAR* pText, BC_Seperator sPos = CSWMenu_NONE);
        BOOL DeleteMenu(UINT uiId, UINT nFlags);
        int DeleteMenu(TCHAR* pText, BC_Seperator sPos = CSWMenu_NONE);

        BOOL GetMenuText(UINT id, CString& string, UINT nFlags = MF_BYPOSITION);
        BOOL SetMenuText(UINT id, CString string, UINT nFlags = MF_BYPOSITION);

        CSWMenu* GetSubCSWMenu(TCHAR* lpszSubMenuName);
        CMenu* GetSubMenu(LPCTSTR lpszSubMenuName);
        CMenu* GetSubMenu(int nPos);

        int GetMenuPosition(TCHAR* pText);
        void SetIconSize(int width, int height);
        void SetBitmapBackground(COLORREF color);
        void SetMenuBackColor(COLORREF clrBar, COLORREF clrBack, COLORREF clrBackSel, COLORREF clrEdge);
        void SetTextColor(COLORREF clr, COLORREF clrSel);
        void SetSeparatorHeight(int height);
        void UnSetBitmapBackground();
        BOOL GetDisableOldStyle();
        void SetDisableOldStyle();
        void UnSetDisableOldStyle();
    public:
        static HBITMAP LoadSysColorBitmap(int nResourceId);
        static void UpdateMenu(CMenu* pmenu);
        static BOOL IsMenu(CMenu* submenu);
        static BOOL IsMenu(HMENU submenu);
        static LRESULT FindKeyboardShortcut(UINT nChar, UINT nFlags, CMenu* pMenu);
        static void SetMenuDrawMode(UINT mode);
        static void SetSelectDisableMode(BOOL mode);
        static int GetMenuDrawMode();
        static BOOL GetSelectDisableMode();
        static void SetXPBitmap3D(BOOL val);
        static BOOL GetXPBitmap3D();
        static COLORREF LightenColor(COLORREF col, double factor);
        static COLORREF DarkenColor(COLORREF col, double factor);
        static BOOL IsLunaMenuStyle();
        static BOOL IsWindowsClassicTheme();
        static WORD NumBitmapColors(LPBITMAPINFOHEADER lpBitmap);
    protected:
        HHOOK m_hook;
        void InstallHook();
        void UnInstallHook();
        CSWMenu* FindMenuOption(int nId, int& nLoc);
        CSWMenu* FindAnotherMenuOption(int nId, int& nLoc, CArray<CSWMenu*, CSWMenu*>& bcsubs, CArray<int, int&>& bclocs);
        CSWMenuData* NewMenu(UINT pos, UINT nFlags, UINT nID, CString string);
        CSWMenuData* FindMenuItem(UINT nID);
        CSWMenuData* FindMenuOption(TCHAR* lpstrText);
        CSWMenuData* FindMenuList(UINT nID);
        void InsertSpaces();
        void DrawCheckMark(CDC* pDC, int x, int y, COLORREF color, BOOL narrowflag = FALSE);
        void DrawRadioDot(CDC* pDC, int x, int y, COLORREF color);
        void SynchronizeMenu();
        void InitializeMenuList(int value);
        void DeleteMenuList();
        void DrawItem_Win9xNT2000(LPDRAWITEMSTRUCT lpDIS);
        void DrawItem_WinXP(LPDRAWITEMSTRUCT lpDIS);
        BOOL Draw3DCheckmark(CDC* dc, const CRect& rc, BOOL bSelected, HBITMAP hBitmapCheck);
        BOOL DrawXPCheckmark(CDC* dc, const CRect& rc, HBITMAP hBitmapCheck, COLORREF& clrOut, BOOL bSelected);
        void DitherBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth,
            int nHeight, HBITMAP hbm, int nXSrc, int nYSrc, COLORREF bgcolor);
        void DitherBlt(CDC* pDC, int nXDest, int nYDest, int nWidth,
            int nHeight, CBitmap& bmp, int nXSrc, int nYSrc, COLORREF bgcolor);
        void DitherBlt3(CDC* pDC, int nXDest, int nYDest, int nWidth,
            int nHeight, CBitmap& bmp, COLORREF bgcolor);
        BOOL GetBitmapFromImageList(CDC* pDC, CImageList* imglist, int nIndex, CBitmap& bmp);
        BOOL ImageListDuplicate(CImageList* il, int xoffset, CImageList* newlist);
        void ColorBitmap(CDC* pDC, CBitmap& bmp, CSize bitmap_size, CSize icon_size, COLORREF fill, COLORREF border, int hatchstyle = -1);
        void RemoveTopLevelOwnerDraw();
        int GetMenuStart();
        void GetFadedBitmap(CBitmap& bmp);
        void GetTransparentBitmap(CBitmap& bmp);
        void GetDisabledBitmap(CBitmap& bmp, COLORREF background = 0);
        void GetShadowBitmap(CBitmap& bmp);
        int AddToGlobalImageList(CImageList* il, int xoffset, int nID);
        int GlobalImageListOffset(int nID);
        BOOL CanDraw3DImageList(int offset);
    protected:
        CTypedPtrArray<CPtrArray, CSWMenuData*> m_MenuList;
        CTypedPtrArray<CPtrArray, HMENU>  m_SubMenus;
        COLORREF m_clrBackgroundBitmatMask;
        BOOL m_bBitmapBackgroundFlag;
        BOOL m_bDisableOldStyle;
        CImageList* m_imagelistCheckBitmap;
        BOOL m_bCheckBitmapShare;
        int m_nSelectCheck;
        int m_bUnSelectCheck;
        BOOL m_bDynIcons;
        BOOL m_loadmenu;
        static CTypedPtrArray<CPtrArray, HMENU>  m_AllSubMenus;
        static CImageList m_AllImages;
        static CArray<int, int&> m_AllImagesID;
    public:
        static int m_iconX;
        static int m_iconY;
        static int m_iSeparatorHeight;
        static COLORREF m_clrBitmapBar;
        static COLORREF m_clrBackground;
        static COLORREF m_clrBackgroundSel;
        static COLORREF m_clrEdge;
        static COLORREF m_clrText;
        static COLORREF m_clrTextSel;

        static UINT m_nDrawMode;
        static BOOL m_bSelectDisabled;
        static BOOL m_bDraw3DBitmap;
        static BOOL m_bHighColorBitmap;
        static BOOL m_bSpaceAccelerators;
        static BOOL m_bOrgSpaceAccelerators;
    };
}
#endif