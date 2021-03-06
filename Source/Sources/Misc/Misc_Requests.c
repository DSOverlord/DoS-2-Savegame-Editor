
//<<>-<>>---------------------------------------------------------------------()
/*
	Routines d'affichage des requ皻es
									      */
//()-------------------------------------------------------------------<<>-<>>//

// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //
// 中�									  中� //
// 中� Donn嶪s								  中� //
// 中�									  中� //
// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //

#include "_Global.h"
#include "Application.h"
#include "Requests.h"
#include "Texts.h"

extern APPLICATION	App;


// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //
// 中�									  中� //
// 中� Affichage des erreurs						  中� //
// 中�									  中� //
// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //

// 姣遙 Affichage d'une erreur syst鋗e avec arguments 垂垂垂垂垂垂垂垂垂姣

int Request_MessageBoxEx(HWND hWnd, WCHAR *pszWindowText, WCHAR *pszWindowTitle, UINT uFlags, ...)
{
	WCHAR*		pszErrorMsg;
	va_list		vl;
	int		iResult;

	va_start(vl,uFlags);

	if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_STRING,pszWindowText,0,0,(WCHAR *)&pszErrorMsg,1,&vl))
		{
		va_end(vl);
		return(0);
		}

	iResult = MessageBox(hWnd,pszErrorMsg,pszWindowTitle,uFlags);
	LocalFree(pszErrorMsg);
	va_end(vl);
	return(iResult);
}


// 姣遙 Affichage d'une erreur syst鋗e avec arguments 垂垂垂垂垂垂垂垂垂姣

void Request_PrintErrorEx(HWND hWnd, WCHAR *pszWindowText, WCHAR *pszWindowTitle, UINT uFlags, ...)
{
	WCHAR*		pszErrorMsg;
	va_list		vl;

	va_start(vl,uFlags);

	if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_STRING,pszWindowText,0,0,(WCHAR *)&pszErrorMsg,1,&vl))
		{
		va_end(vl);
		return;
		}

	Request_PrintError(hWnd,pszErrorMsg,pszWindowTitle,uFlags);
	LocalFree(pszErrorMsg);
	va_end(vl);
	return;
}


// 姣遙 Affichage d'une erreur syst鋗e 垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂�

void Request_PrintError(HWND hWnd, WCHAR *pszWindowText, WCHAR *pszWindowTitle, UINT uFlags)
{
	WCHAR*		pszErrorMsg;
	WCHAR*		pszFinalMsg;

	if (GetLastError() == ERROR_SUCCESS)
		goto Simple;

	if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,NULL,GetLastError(),LOCALE_USER_DEFAULT,(WCHAR *)&pszErrorMsg,1,NULL))
		goto Simple;

	pszFinalMsg = HeapAlloc(App.hHeap,HEAP_ZERO_MEMORY,(wcslen(pszWindowText)+wcslen(szLF)*2+wcslen(pszErrorMsg)+1)*sizeof(WCHAR));
	if (!pszFinalMsg)
		goto Failed;

	wcscpy(pszFinalMsg,pszWindowText);
	wcscat(pszFinalMsg,szLF);
	wcscat(pszFinalMsg,szLF);
	wcscat(pszFinalMsg,pszErrorMsg);
	MessageBox(hWnd,pszFinalMsg,pszWindowTitle,MB_OK|uFlags);

	HeapFree(App.hHeap,0,pszFinalMsg);
	LocalFree(pszErrorMsg);
	return;

//--- Affichage du message brut ---

Failed:	LocalFree(pszErrorMsg);
Simple:	MessageBox(hWnd,pszWindowText,pszWindowTitle,MB_OK|uFlags);
	return;
}
