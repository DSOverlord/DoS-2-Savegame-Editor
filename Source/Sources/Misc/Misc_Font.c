
//<<>-<>>---------------------------------------------------------------------()
/*
	Routines de gestion des caract�res
									      */
//()-------------------------------------------------------------------<<>-<>>//

// �������������������������������������������������������������������������� //
// ���									  ��� //
// ��� Donn�es								  ��� //
// ���									  ��� //
// �������������������������������������������������������������������������� //

#include "_Global.h"
#include "Application.h"


// �������������������������������������������������������������������������� //
// ���									  ��� //
// ��� Fonctions							  ��� //
// ���									  ��� //
// �������������������������������������������������������������������������� //

// ���� Obtention des informations concernant des caract�res �������������

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


// ���� Cr�ation de petits caract�res ������������������������������������

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


// ���� Jeu de caract�res du jeu �����������������������������������������

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


// ���� Lib�ration d'un jeu de caract�res ��������������������������������

void Font_Destroy(WCHAR *pszFontFile, HFONT hFont)
{
	if (hFont)
		{
		DeleteObject(hFont);
		RemoveFontResourceEx(pszFontFile,FR_PRIVATE,NULL);
		}
	return;
}

// �������������������������������������������������������������������������� //
// �������������������������������������������������������������������������� //
// END
