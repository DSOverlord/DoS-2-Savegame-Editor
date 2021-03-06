
//<<>-<>>---------------------------------------------------------------------()
/*
	Routines de gestion des caract鋨es
									      */
//()-------------------------------------------------------------------<<>-<>>//

// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //
// 中�									  中� //
// 中� Donn嶪s								  中� //
// 中�									  中� //
// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //

#include "_Global.h"
#include "Application.h"


// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //
// 中�									  中� //
// 中� Fonctions							  中� //
// 中�									  中� //
// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //

// 姣遙 Obtention des informations concernant des caract鋨es 垂垂垂垂垂垂�

int Font_GetInfo(HWND hWnd, FONTINFO *pfiFont)
{
	HDC		hDC;
	HFONT		hOldFont;
	TEXTMETRIC	tmFontInfo;
	int		iResult = 0;

	hDC = GetDC(hWnd);
	if (hDC)
		{
		hOldFont = SelectObject(hDC,pfiFont->hFont);
		if (hOldFont)
			{
			GetTextMetrics(hDC,&tmFontInfo);
			pfiFont->uFontHeight = tmFontInfo.tmExternalLeading+tmFontInfo.tmHeight;
			pfiFont->uFontMaxWidth = tmFontInfo.tmMaxCharWidth;
			pfiFont->uFontAveWidth = tmFontInfo.tmAveCharWidth;
			SelectObject(hDC,hOldFont);
			iResult = 1;
			}
		ReleaseDC(hWnd,hDC);
		}
	return(iResult);
}


// 姣遙 Cr嶧tion de petits caract鋨es 垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂姣

int Font_CreateCopy(HWND hWnd, FONTINFO *pfiFromFont, FONTINFO *pfiToFont, LONG lHeight, LONG lWeight)
{
	LOGFONT		lf;

	GetObject(pfiFromFont->hFont,sizeof(LOGFONT),&lf);
	if (lHeight) lf.lfHeight = lHeight;
	if (lWeight) lf.lfWeight = lWeight;
	pfiToFont->hFont = CreateFontIndirect(&lf);
	if (!pfiToFont->hFont) return(0);
	return(Font_GetInfo(hWnd,pfiToFont));
}


// 姣遙 Jeu de caract鋨es du jeu 垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂�

HFONT Font_Create(WCHAR *pszFontFile, WCHAR *pszFontName, LONG lHeight)
{
	LOGFONT	lf;

	if (!AddFontResourceEx(pszFontFile,FR_PRIVATE,NULL)) return(NULL);
	lf.lfHeight = lHeight;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = FW_REGULAR;
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = CLEARTYPE_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH;
	wcscpy(lf.lfFaceName,pszFontName);
	return(CreateFontIndirect(&lf));
}


// 姣遙 Lib廨ation d'un jeu de caract鋨es 垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂姣

void Font_Destroy(WCHAR *pszFontFile, HFONT hFont)
{
	if (hFont)
		{
		DeleteObject(hFont);
		RemoveFontResourceEx(pszFontFile,FR_PRIVATE,NULL);
		}
	return;
}

// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //
// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //
// END
