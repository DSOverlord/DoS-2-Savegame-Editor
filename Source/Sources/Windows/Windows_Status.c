
//<<>-<>>---------------------------------------------------------------------()
/*
	Fen皻re de statut
									      */
//()-------------------------------------------------------------------<<>-<>>//

// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //
// 中�									  中� //
// 中� Donn嶪s								  中� //
// 中�									  中� //
// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //

#include "Application.h"
#include "Locale.h"
#include "Texts.h"

extern APPLICATION	App;


// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //
// 中�									  中� //
// 中� Fonctions relatives � la fen皻re					  中� //
// 中�									  中� //
// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //

// 姣遙 Cr嶧tion de la fen皻re de status 垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂�

int Status_CreateWindow()
{
	App.hwndStatus = CreateStatusWindow(WS_CHILD|WS_VISIBLE,NULL,App.hWnd,CTLID_STATUS);
	if (!App.hwndStatus) return(0);

	Status_UpdateParts();
	return(1);
}


// 姣遙 Modifie la taille des parties de la barre de statut 垂垂垂垂垂垂姣

void Status_UpdateParts()
{
	RECT	rcClient;
	int	iStatusParts[2] = {0,0};

	GetClientRect(App.hWnd,&rcClient);
	iStatusParts[0] = rcClient.right-128;
	iStatusParts[1] = rcClient.right;
	SendMessage(App.hwndStatus,SB_SETPARTS,(WPARAM)2,(LPARAM)&iStatusParts);
	SendMessage(App.hwndStatus,SB_SETTEXT,(WPARAM)STATUS_TEXT|SBT_OWNERDRAW,(LPARAM)NULL);
	SendMessage(App.hwndStatus,SB_SETTEXT,(WPARAM)STATUS_PROGRESS|SBT_OWNERDRAW,(LPARAM)NULL);
	return;
}


// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //
// 中�									  中� //
// 中� Affichage							  中� //
// 中�									  中� //
// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //

// 姣遙 Affichage du texte de statut 垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂�

void Status_DrawStatusText(DRAWITEMSTRUCT *pDraw)
{
	EnterCriticalSection(&App.CriticalSection);

	FillRect(pDraw->hDC,&pDraw->rcItem,GetSysColorBrush(COLOR_BTNFACE));
	if (App.pszStatusText)
		{
		RECT		rcDrawArea;
		COLORREF	crDefText;
		int		crDefBack;

		crDefText = SetTextColor(pDraw->hDC,GetSysColor(COLOR_BTNTEXT));
		crDefBack = SetBkMode(pDraw->hDC,TRANSPARENT);

		CopyRect(&rcDrawArea,&pDraw->rcItem);
		rcDrawArea.left += 4;
		rcDrawArea.right -= 4;
		DrawText(pDraw->hDC,App.pszStatusText,-1,&rcDrawArea,DT_LEFT|DT_NOPREFIX|DT_SINGLELINE|DT_VCENTER|DT_PATH_ELLIPSIS);

		SetBkMode(pDraw->hDC,crDefBack);
		SetTextColor(pDraw->hDC,crDefText);
		}

	LeaveCriticalSection(&App.CriticalSection);
	return;
}

// --- Modifie le texte de statut ---

void Status_SetText(WCHAR *pszText, ...)
{
	va_list		vl;

	EnterCriticalSection(&App.CriticalSection);

	if (App.pszStatusText) LocalFree(App.pszStatusText);
	App.pszStatusText = NULL;

	if (pszText)
		{
		va_start(vl,pszText);
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_STRING,pszText,0,0,(WCHAR *)&App.pszStatusText,1,&vl);
		va_end(vl);
		}

	LeaveCriticalSection(&App.CriticalSection);
	SendMessage(App.hwndStatus,SB_SETTEXT,(WPARAM)STATUS_TEXT|SBT_OWNERDRAW,(LPARAM)NULL);
	return;
}


// 姣遙 Affichage de la barre de progression 垂垂垂垂垂垂垂垂垂垂垂垂垂垂�

void Status_DrawProgressBar(DRAWITEMSTRUCT *pDraw)
{
	RECT	rcArea;
	int	iWidth;

	CopyRect(&rcArea,&pDraw->rcItem);

	iWidth = (int)(((float)rcArea.right-(float)rcArea.left)*((float)App.uProgression/100.0f));
	if (iWidth > rcArea.right-rcArea.left) iWidth = rcArea.right-rcArea.left;
	if (iWidth > 0)
		{
		rcArea.right = rcArea.left+iWidth;
		FillRect(pDraw->hDC,&rcArea,GetSysColorBrush(COLOR_HIGHLIGHT));
	//	rcArea.left = rcArea.right;
	//	rcArea.right = ItemStruct->rcItem.right;
		}
	//if (Width < 100) FillRect(ItemStruct->hDC,&rcArea,GetSysColorBrush(COLOR_BTNFACE));
	return;
}

//--- Modifie la progression ---

void Status_UpdateProgress(UINT uCurrent, UINT uMax)
{
	if (uCurrent == -1 && uMax == -1) App.uProgression = 0;
	else App.uProgression = (UINT)(((float)uCurrent/(float)uMax)*100.0f);
	if (App.uProgression < 0) App.uProgression = 0;
	if (App.uProgression > 100) App.uProgression = 100;

	// Prevents flood
	if (App.uProgression != 0 && App.uProgression != 100 && App.dwProgressionTime != 0)
		{
		DWORD dwTime = GetTickCount();
		if (dwTime-App.dwProgressionTime < STATUS_REFRESH_MS) return;
		}
	App.dwProgressionTime = GetTickCount();

	SendMessage(App.hwndStatus,SB_SETTEXT,(WPARAM)STATUS_PROGRESS|SBT_OWNERDRAW,(LPARAM)NULL);
	InvalidateRect(App.hwndStatus,NULL,FALSE);
	return;
}
