#pragma once

#define RGB2ARGB(_cr,a)     ((DWORD)(BYTE)(a) << 24) | ((_cr & 0x000000ff) << 16) | ((_cr & 0x0000ff00)) | ((_cr & 0x00ff0000) >> 16)
#define ARGB2RGB(_cr)       ((COLORREF)( ((BYTE)((_cr & 0xff0000) >> 16)|((WORD)((BYTE)((_cr & 0xff00) >> 8))<<8)) | (((DWORD)(BYTE)(_cr & 0xff))<<16) ))


#define MAKERGBA(r,g,b,a)   ((COLORREF)( (((BYTE)(b) | ((WORD)((BYTE)(g))<<8)) | (((DWORD)(BYTE)(r))<<16)) | (((DWORD)(BYTE)(a))<<24) ))
#define MAKEBGRA(b,g,r,a)   ((COLORREF)( (((BYTE)(b) | ((WORD)((BYTE)(g))<<8)) | (((DWORD)(BYTE)(r))<<16)) | (((DWORD)(BYTE)(a))<<24) ))
#define MAKEABGR(a,b,g,r)   ((COLORREF)( (((BYTE)(b) | ((WORD)((BYTE)(g))<<8)) | (((DWORD)(BYTE)(r))<<16)) | (((DWORD)(BYTE)(a))<<24) ))
#define MAKEARGB(a,r,g,b)   ((COLORREF)( (((BYTE)(b) | ((WORD)((BYTE)(g))<<8)) | (((DWORD)(BYTE)(r))<<16)) | (((DWORD)(BYTE)(a))<<24) ))

#define MAKERGB(r,g,b)      ((DWORD)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#define MAKEBGR(b,g,r)      ((DWORD)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))


#define ARGB_GETA(_cr)      ((BYTE)((_cr & 0xff000000) >> 24))  //(LOBYTE(((DWORD)(_cr)) >> 24))
#define ARGB_GETR(_cr)      ((BYTE)((_cr & 0x00ff0000) >> 16))  //(LOBYTE(((DWORD)(_cr)) >> 16))
#define ARGB_GETG(_cr)      ((BYTE)((_cr & 0x0000ff00) >> 8))   //(LOBYTE(((WORD)(_cr)) >> 8))
#define ARGB_GETB(_cr)      ((BYTE)((_cr & 0x000000ff)))        //(LOBYTE(rgb))

#define ARGB_SETA(_cr, _v)  (((DWORD)_cr & 0x00ffffff) | ((DWORD)(BYTE)(_v) << 24))
#define ARGB_SETR(_cr, _v)  (((DWORD)_cr & 0xff00ffff) | ((DWORD)(BYTE)(_v) << 16))
#define ARGB_SETG(_cr, _v)  (((DWORD)_cr & 0xffff00ff) | ((WORD)(BYTE)(_v) << 8))
#define ARGB_SETB(_cr, _v)  (((DWORD)_cr & 0xffffff00) | ((BYTE)(_v)))

#define RGB_GETR(_cr)       ((BYTE)((_cr & 0x000000ff)))        //(LOBYTE(rgb))
#define RGB_GETG(_cr)       ((BYTE)((_cr & 0x0000ff00) >> 8))   //(LOBYTE(((WORD)(rgb)) >> 8))
#define RGB_GETB(_cr)       ((BYTE)((_cr & 0x00ff0000) >> 16))  //(LOBYTE((rgb)>>16))

#define RGB_SETR(_cr, _v)   (((DWORD)_cr & 0xffffff00) | ((BYTE)(_v)))
#define RGB_SETG(_cr, _v)   (((DWORD)_cr & 0xffff00ff) | ((WORD)(BYTE)(_v) << 8))
#define RGB_SETB(_cr, _v)   (((DWORD)_cr & 0xff00ffff) | ((DWORD)(BYTE)(_v) << 16))

// RGB 颜色反转, 灰色反转后还是灰色
#define RGB_REVERSE(_cr)    MAKERGB( (255 - RGB_GETR(_cr)), (255 - RGB_GETG(_cr)), (255 - RGB_GETB(_cr)) )
// ARGB 颜色反转, 灰色反转后还是灰色, 透明度不变
#define ARGB_REVERSE(_cr)   MAKEARGB( (ARGB_GETA(_cr)), (255 - ARGB_GETR(_cr)), (255 - ARGB_GETG(_cr)), (255 - ARGB_GETB(_cr)) )


