
//<<>-<>>---------------------------------------------------------------------()
/*
	Caract鋨es
									      */
//()-------------------------------------------------------------------<<>-<>>//

#ifndef _FONT_INCLUDE
#define _FONT_INCLUDE


// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //
// 中�									  中� //
// 中� Structures							  中� //
// 中�									  中� //
// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //

typedef struct FONTINFO {
	HFONT		hFont;
	UINT		uFontHeight;
	UINT		uFontMaxWidth;
	UINT		uFontAveWidth;
} FONTINFO;


// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //
// 中�									  中� //
// 中� Prototypes							  中� //
// 中�									  中� //
// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //

int			Font_GetInfo(HWND,FONTINFO *);
int			Font_CreateCopy(HWND,FONTINFO *,FONTINFO *,LONG,LONG);
HFONT			Font_Create(WCHAR *,WCHAR *,LONG);
void			Font_Destroy(WCHAR *,HFONT);

#endif
