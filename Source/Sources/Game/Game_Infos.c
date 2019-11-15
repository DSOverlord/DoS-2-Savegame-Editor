
//<<>-<>>---------------------------------------------------------------------()
/*
	Informations sur une sauvegarde
									      */
//()-------------------------------------------------------------------<<>-<>>//

// �������������������������������������������������������������������������� //
// ���									  ��� //
// ��� Donn�es								  ��� //
// ���									  ��� //
// �������������������������������������������������������������������������� //

#include "Application.h"
#include "Divine.h"
#include "Files.h"
#include "Infos.h"
#include "Locale.h"
#include "Texts.h"
#include "Dialogs.h"
#include "Utils.h"
#include "Requests.h"

extern APPLICATION	App;


// �������������������������������������������������������������������������� //
// ���									  ��� //
// ��� Bo�te de dialogue						  ��� //
// ���									  ��� //
// �������������������������������������������������������������������������� //

// ���� Affichage ��������������������������������������������������������

void Infos_Show()
{
	if (App.Game.Save.pszSaveName)
		{
		WCHAR*	pszPath;
		int	iResult;

		pszPath = Divine_GetSaveGamePath(App.Config.uGame,App.Config.pszProfile,App.Game.Save.pszSaveName);
		if (!pszPath)
			{
			Request_PrintError(App.hWnd,Locale_GetText(TEXT_ERR_DIALOG),NULL,MB_ICONERROR);
			return;
			}
		if (!lsv_Load(App.hWnd,pszPath,&App.Game.Save.nodeFiles,LS_MODE_SAVEINFO))
			{
			HeapFree(App.hHeap,0,pszPath);
			return;
			}
		HeapFree(App.hHeap,0,pszPath);

		iResult = DialogBox(App.hInstance,MAKEINTRESOURCE(1010),App.hWnd,Infos_Proc);
		if (iResult == -1) Request_PrintError(App.hWnd,Locale_GetText(TEXT_ERR_DIALOG),NULL,MB_ICONERROR);
		lsv_Release(&App.Game.Save.nodeFiles);
		}

	return;
}


// ���� Processus de la bo�te de dialogue ��������������������������������

INT_PTR CALLBACK Infos_Proc(HWND hDlg, UINT uMsgId, WPARAM wParam, LPARAM lParam)
{
	if (uMsgId == WM_INITDIALOG)
		{
		if (!Infos_Initialise(hDlg))
			{
			EndDialog(hDlg,-1);
			return(FALSE);
			}
		Infos_PrepareAndUpdate(hDlg,App.Game.Save.pszSaveName,&App.Game.Save.nodeFiles);
		SetWindowText(hDlg,Locale_GetText(IDM_INFOS));
		Dialog_CenterWindow(hDlg,App.hWnd);
		return(FALSE);
		}

	switch(uMsgId)
		{
		case WM_DRAWITEM:
			return(Infos_Draw(hDlg,wParam,(DRAWITEMSTRUCT *)lParam,&App.Game.Save.nodeFiles));

		case WM_COMMAND:
			switch(HIWORD(wParam))
				{
				case BN_CLICKED:
					switch(LOWORD(wParam))
						{
						case IDOK:
							EndDialog(hDlg,IDOK);
							return(TRUE);
						case IDCANCEL:
							EndDialog(hDlg,IDCANCEL);
							return(TRUE);
						}
					break;
				}
			break;

		case WM_CLOSE:
			EndDialog(hDlg,IDCANCEL);
			return(TRUE);
		}

	return(FALSE);
}


// �������������������������������������������������������������������������� //
// ���									  ��� //
// ��� Fonctions							  ��� //
// ���									  ��� //
// �������������������������������������������������������������������������� //

// ���� Initialisations ��������������������������������������������������

int Infos_Initialise(HWND hDlg)
{
	HIMAGELIST	hImageList;
	RECT		rcItem;
	LVCOLUMN	lvColumn;
	LVGROUP		lvGroup;
	LVTILEVIEWINFO	lvTileView;
	static UINT	uGroupsTitles[] = { TEXT_INFOS_MISC, TEXT_INFOS_TEAM, TEXT_INFOS_MODS, 0 };
	static UINT	uInfosIconsAPP[] = { APP_ICON_INFO_EMPTY, APP_ICON_INFO_SAVEGAME, APP_ICON_INFO_VERSION, APP_ICON_INFO_MODDED, APP_ICON_INFO_NOTMODDED, APP_ICON_INFO_DIFFICULTY, APP_ICON_INFO_DATE, APP_ICON_INFO_CHARACTER, APP_ICON_INFO_NOTCHARACTER,  APP_ICON_INFO_LARIAN, APP_ICON_INFO_MOD};
	static UINT	uInfosIconsIDS[] = { INFOS_ICON_EMPTY,    INFOS_ICON_SAVEGAME,    INFOS_ICON_VERSION,    INFOS_ICON_MODDED,    INFOS_ICON_NOTMODDED,    INFOS_ICON_DIFFICULTY,    INFOS_ICON_DATE,    INFOS_ICON_CHARACTER,    INFOS_ICON_NOTCHARACTER,     INFOS_ICON_LARIANMOD, INFOS_ICON_PLAYERMOD };
	HICON		hInfosIcons[INFOS_ICON_LAST];
	int		i;

	SendDlgItemMessage(hDlg,801,LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_DOUBLEBUFFER,LVS_EX_DOUBLEBUFFER);
	hImageList = ImageList_Create(48,48,ILC_COLOR32|ILC_MASK,INFOS_ICON_LAST,0);
	if (!hImageList) goto Error;

	//--- Chargement des ic�nes
	for (i = 0; i != INFOS_ICON_LAST; i++) hInfosIcons[uInfosIconsIDS[i]] = App.hIcons[uInfosIconsAPP[i]];
	for (i = 0; i != INFOS_ICON_LAST; i++) if (ImageList_ReplaceIcon(hImageList,-1,hInfosIcons[i]) == -1) goto Error;

	//--- Colonnes
	lvColumn.mask = LVCF_FMT|LVCF_ORDER|LVCF_TEXT|LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 0;
	lvColumn.pszText = NULL;
	lvColumn.cchTextMax = 0;
	lvColumn.iOrder = 0;
	if (SendDlgItemMessage(hDlg,801,LVM_INSERTCOLUMN,0,(LPARAM)&lvColumn) == -1) goto Error;
	lvColumn.iOrder = 1;
	if (SendDlgItemMessage(hDlg,801,LVM_INSERTCOLUMN,1,(LPARAM)&lvColumn) == -1) goto Error;

	//--- Groupes
	for (i = 0; uGroupsTitles[i] != 0; i++)
		{
		lvGroup.cbSize = sizeof(LVGROUP);
		lvGroup.mask = LVGF_HEADER|LVGF_GROUPID|LVGF_STATE;
		lvGroup.pszHeader = Locale_GetText(uGroupsTitles[i]);
		lvGroup.iGroupId = i;
		lvGroup.stateMask = lvGroup.state = 0;
		if (SendDlgItemMessage(hDlg,801,LVM_INSERTGROUP,(WPARAM)-1,(LPARAM)&lvGroup) == -1) goto Error;
		}

	//--- Param�tres
	GetClientRect(GetDlgItem(hDlg,801),&rcItem);
	lvTileView.cbSize = sizeof(LVTILEVIEWINFO);
	lvTileView.dwMask = LVTVIM_TILESIZE|LVTVIM_COLUMNS;
	lvTileView.dwFlags = LVTVIF_FIXEDSIZE;
	lvTileView.sizeTile.cx = rcItem.right-rcItem.left-GetSystemMetrics(SM_CXVSCROLL)-10;
	lvTileView.sizeTile.cy = (App.Font.uFontHeight*2+8) < (48+8)?(48+8):(App.Font.uFontHeight*2+8);
	lvTileView.cLines = 2;
	SendDlgItemMessage(hDlg,801,LVM_SETTILEVIEWINFO,0,(LPARAM)&lvTileView);

	//--- Affichage
	SendDlgItemMessage(hDlg,801,LVM_SETIMAGELIST,(WPARAM)LVSIL_NORMAL,(LPARAM)hImageList);
	SendDlgItemMessage(hDlg,801,LVM_SETVIEW,(WPARAM)LV_VIEW_TILE,(LPARAM)0);
	SendDlgItemMessage(hDlg,801,LVM_ENABLEGROUPVIEW,(WPARAM)TRUE,0);
	return(1);

Error:	if (hImageList) ImageList_Destroy(hImageList);
	SendDlgItemMessage(hDlg,801,LVM_DELETEALLITEMS,0,0);
	return(0);
}


// ���� Convertis les fichiers n�cessaires et met � jour l'affichage �����

void Infos_PrepareAndUpdate(HWND hDlg, WCHAR *pszSaveName, NODE *pRoot)
{
	XML_NODE*	pxnMeta;
	LSFILE*		pMetaFile;
	LSFILE*		pPNGFile;
	LVITEM		lvItem;
	LVTILEINFO	lvTileInfo;
	UINT		uColumns[1];
	WCHAR*		pszTemp = NULL;
	static UINT	uItems[] = { TEXT_INFOS_SAVEGAME, TEXT_INFOS_VERSION, TEXT_INFOS_MODDED, TEXT_INFOS_DIFFICULTY, TEXT_INFOS_DATE, 0 };
	int		i,j = 0;

	//--- Chargement des fichiers ---

	pMetaFile = lsv_FindFile(pRoot,szMetaLSF,NULL);
	if (pMetaFile) lsf_Unpack(hDlg,pMetaFile,LS_MODE_QUIET);

	pPNGFile = lsv_FindFile(pRoot,NULL,szPNGext);
	if (pPNGFile) pPNGFile->hBitmap = png_Load(pPNGFile->pData);

	if (!pMetaFile) goto Error;
	if (!pMetaFile->nodeXMLRoot.next) goto Error;

	//--- R�initialisation ---

	SendDlgItemMessage(hDlg,801,LVM_DELETEALLITEMS,0,0);

	pxnMeta = xml_GetNodeFromPathFirstChild((XML_NODE *)pMetaFile->nodeXMLRoot.next,szInfoMetaDataPath);
	if (!pxnMeta) goto Error;

	//--- Informations g�n�rales ---

	for (pszTemp = NULL, i = 0; uItems[i] != 0; i++, j++)
		{
		lvItem.mask = LVIF_IMAGE|LVIF_TEXT|LVIF_GROUPID;
		lvItem.iItem = j;
		lvItem.iSubItem = 0;
		lvItem.iImage = 0;
		lvItem.pszText = Locale_GetText(uItems[i]);
		lvItem.iGroupId = 0;
		switch(uItems[i])
			{
			case TEXT_INFOS_SAVEGAME:
				lvItem.iImage = INFOS_ICON_SAVEGAME;
				break;
			case TEXT_INFOS_VERSION:
				lvItem.iImage = INFOS_ICON_VERSION;
				break;
			case TEXT_INFOS_MODDED:
				lvItem.iImage = Infos_Get(INFOS_GROUP_MISC,uItems[i],&pszTemp,pxnMeta) == Locale_GetText(TEXT_INFOS_YES)?INFOS_ICON_MODDED:INFOS_ICON_NOTMODDED;
				break;
			case TEXT_INFOS_DIFFICULTY:
				lvItem.iImage = INFOS_ICON_DIFFICULTY;
				break;
			case TEXT_INFOS_DATE:
				lvItem.iImage = INFOS_ICON_DATE;
				break;
			}
		if (SendDlgItemMessage(hDlg,801,LVM_INSERTITEM,0,(LPARAM)&lvItem) == -1) goto Error;
		lvItem.iSubItem = 1;
		lvItem.pszText = Infos_Get(INFOS_GROUP_MISC,uItems[i],&pszTemp,pxnMeta);
		if (lvItem.pszText == (WCHAR *)-1) lvItem.pszText = pszSaveName;
		if (lvItem.pszText == NULL) goto Error;
		if (SendDlgItemMessage(hDlg,801,LVM_SETITEMTEXT,(WPARAM)lvItem.iItem,(LPARAM)&lvItem) == -1) goto Error;
		uColumns[0] = 1;
		lvTileInfo.cbSize = sizeof(LVTILEINFO);
		lvTileInfo.iItem = lvItem.iItem;
		lvTileInfo.cColumns = 1;
		lvTileInfo.puColumns = uColumns;
		lvTileInfo.piColFmt = NULL;
		if (SendDlgItemMessage(hDlg,801,LVM_SETTILEINFO,0,(LPARAM)&lvTileInfo) == -1) goto Error;
		}

	//--- �quipe ---

	for (i = 0; i != 4; i++, j++)
		{
		lvItem.mask = LVIF_IMAGE|LVIF_TEXT|LVIF_GROUPID;
		lvItem.iItem = j;
		lvItem.iSubItem = 0;
		lvItem.iGroupId = 1;
		lvItem.pszText = Infos_Get(INFOS_GROUP_TEAM,i,&pszTemp,pxnMeta);
		lvItem.iImage = lvItem.pszText?INFOS_ICON_CHARACTER:INFOS_ICON_NOTCHARACTER;
		if (lvItem.pszText == NULL) lvItem.pszText = Locale_GetText(TEXT_UNKNOWN);
		if (SendDlgItemMessage(hDlg,801,LVM_INSERTITEM,0,(LPARAM)&lvItem) == -1) goto Error;
		uColumns[0] = 1;
		lvTileInfo.cbSize = sizeof(LVTILEINFO);
		lvTileInfo.iItem = lvItem.iItem;
		lvTileInfo.cColumns = 1;
		lvTileInfo.puColumns = uColumns;
		lvTileInfo.piColFmt = NULL;
		if (SendDlgItemMessage(hDlg,801,LVM_SETTILEINFO,0,(LPARAM)&lvTileInfo) == -1) goto Error;
		}

	//--- Mods ---

	pxnMeta = xml_GetNodeFromPath(pxnMeta,szInfoGameModsPath);
	while (pxnMeta)
		{
		lvItem.mask = LVIF_IMAGE|LVIF_TEXT|LVIF_GROUPID;
		lvItem.iItem = j;
		lvItem.iSubItem = 0;
		lvItem.iGroupId = 2;
		lvItem.pszText = Infos_Get(INFOS_GROUP_MODS,0,&pszTemp,pxnMeta);
		lvItem.iImage = pszTemp?INFOS_ICON_LARIANMOD:INFOS_ICON_PLAYERMOD;
		if (lvItem.pszText != NULL)
			{
			if (SendDlgItemMessage(hDlg,801,LVM_INSERTITEM,0,(LPARAM)&lvItem) == -1) goto Error;
			lvItem.pszText = pszTemp?pszTemp:Locale_GetText(TEXT_INFOS_PLAYERMOD);
			lvItem.iSubItem = 1;
			if (SendDlgItemMessage(hDlg,801,LVM_SETITEMTEXT,(WPARAM)lvItem.iItem,(LPARAM)&lvItem) == -1) goto Error;
			uColumns[0] = 1;
			lvTileInfo.cbSize = sizeof(LVTILEINFO);
			lvTileInfo.iItem = lvItem.iItem;
			lvTileInfo.cColumns = 1;
			lvTileInfo.puColumns = uColumns;
			lvTileInfo.piColFmt = NULL;
			if (SendDlgItemMessage(hDlg,801,LVM_SETTILEINFO,0,(LPARAM)&lvTileInfo) == -1) goto Error;
			j++;
			}
		pxnMeta = xml_GetNextNode(pxnMeta);
		}

	if (pszTemp) HeapFree(App.hHeap,0,pszTemp);
	InvalidateRect(GetDlgItem(hDlg,800),NULL,FALSE);
	return;

Error:	if (pszTemp) HeapFree(App.hHeap,0,pszTemp);
	SendDlgItemMessage(hDlg,801,LVM_DELETEALLITEMS,0,0);
	InvalidateRect(GetDlgItem(hDlg,800),NULL,FALSE);
	return;
}

//--- R�cup�re les informations ---

WCHAR* Infos_Get(UINT uGroup, UINT uID, WCHAR **pszTemp, XML_NODE *pRoot)
{
	XML_NODE*	pxnInfo;

	if (!pRoot) return(NULL);

	switch(uGroup)
		{
		//--- Informations g�n�rales
		case INFOS_GROUP_MISC:
			switch(uID)
				{
				case TEXT_INFOS_SAVEGAME:
					return((WCHAR *)-1);

				case TEXT_INFOS_VERSION:
					pxnInfo = xml_GetNodeFromPath(pRoot,szInfoGameVersionPath);
					if (!pxnInfo) break;
					return(xml_GetAttrValue(pxnInfo,szXMLvalue));

				case TEXT_INFOS_MODDED:
					pxnInfo = xml_GetNodeFromPath(pRoot,szInfoGameModdedPath);
					if (!pxnInfo) break;
					return(Locale_GetText(xml_IsTrue(xml_GetAttr(pxnInfo,szXMLvalue))?TEXT_INFOS_YES:TEXT_INFOS_NO));
					break;

				case TEXT_INFOS_DIFFICULTY:
					pxnInfo = xml_GetNodeFromPath(pRoot,szInfoGameDifficultyPath);
					if (!pxnInfo) break;
					return(xml_GetAttrValue(pxnInfo,szXMLvalue));

				case TEXT_INFOS_DATE: {
					static WCHAR*	pszPattern[6] = { L"YYYY", L"MM", L"DD", L"hh", L"mm", L"ss" };
					static WCHAR*	pszValue[6] = { L"Year", L"Month", L"Day", L"Hours", L"Minutes", L"Seconds" };
					UINT		pszNumber[6];
					WCHAR		szBuffer[6];
					WCHAR*		pszDate;
					int		i,j;

					pxnInfo = xml_GetNodeFromPathFirstChild(pRoot,szInfoGameDatePath);
					if (!pxnInfo) break;
					for (i = 0; i != 6; i++)
						{
						pszDate = xml_GetThisAttrValue(xml_GetXMLValueAttr(pxnInfo,szXMLattribute,szXMLid,pszValue[i]));
						if (!pszDate) return(NULL);
						pszNumber[i] = wcstol(pszDate,NULL,10);
						if (i == 0) pszNumber[i] += 1900;
						}
					pszDate = Locale_GetText(TEXT_INFOS_DATEFMT);
					if (*pszTemp) HeapFree(App.hHeap,0,*pszTemp);
					*pszTemp = HeapAlloc(App.hHeap,0,wcslen(pszDate)*sizeof(WCHAR)+sizeof(WCHAR));
					if (!*pszTemp) break;

					wcscpy(*pszTemp,pszDate);
					for (i = 0; i != 6; i++)
						{
						swprintf(szBuffer,4,i == 0?L"%04u":L"%02u",pszNumber[i]);
						pszDate = wcsstr(*pszTemp,pszPattern[i]);
						if (!pszDate)
							{
							HeapFree(App.hHeap,0,*pszTemp);
							*pszTemp = NULL;
							return(NULL);
							}
						j = wcslen(pszPattern[i]);
						while (j--) pszDate[j] = szBuffer[j];
						}

					} return(*pszTemp);
				}
			break;

		//--- Composition de l'�quipe
		case INFOS_GROUP_TEAM: {
			int	i;

			pxnInfo = xml_GetNodeFromPathFirstChild(pRoot,szInfoGamePartyPath);
			if (!pxnInfo) break;

			for (i = 0; i != uID; i++)
				{
				pxnInfo = xml_GetNextNode(pxnInfo);
				if (!pxnInfo) return(NULL);
				}

			} return(xml_GetThisAttrValue(xml_GetXMLValueAttr((XML_NODE *)pxnInfo->children.next,szXMLattribute,szXMLid,L"CharacterName")));

		//--- Liste des mods
		case INFOS_GROUP_MODS: {
			static WCHAR*	Ignore[] = { L"1301db3d-1f54-4e98-9be5-5094030916e4", L"2bd9bdbe-22ae-4aa2-9c93-205880fc6564", L"eedf7638-36ff-4f26-a50a-076b87d53ba0", NULL };
			static WCHAR*	Larian[] = { L"9b45f7e5-d4e2-4fc2-8ef7-3b8e90a5256c", L"38608c30-1658-4f6a-8adf-e826a5295808", L"423fae51-61e3-469a-9c1f-8ad3fd349f02", L"68a99fef-d125-4ed0-893f-bb6751e52c5e", L"2d42113c-681a-47b6-96a1-d90b3b1b07d3", L"ec27251d-acc0-4ab8-920e-dbc851e79bb4", NULL };
			WCHAR*		UUID;

			if (*pszTemp)
				{
				HeapFree(App.hHeap,0,*pszTemp);
				*pszTemp = NULL;
				}

			UUID = xml_GetThisAttrValue(xml_GetXMLValueAttr((XML_NODE *)pRoot->children.next,szXMLattribute,szXMLid,L"UUID"));
			if (UUID)
				{
				int	i;

				for (i = 0; Ignore[i] != NULL; i++)
					if (!wcscmp(Ignore[i],UUID)) return(NULL);

				for (i = 0; Larian[i] != NULL; i++)
					if (!wcscmp(Larian[i],UUID))
						{
						*pszTemp = Misc_StrCpyAlloc(Locale_GetText(TEXT_INFOS_LARIANMOD));
						break;
						}
				}

			} return(xml_GetThisAttrValue(xml_GetXMLValueAttr((XML_NODE *)pRoot->children.next,szXMLattribute,szXMLid,L"Name")));

		}

	return(NULL);
}


// ���� Affichage ���������������������������������������������������������

BOOL Infos_Draw(HWND hDlg, UINT uCtlId, DRAWITEMSTRUCT *pDraw, NODE *pRoot)
{
	switch(uCtlId)
		{
		case 800:
			Infos_DrawImage(pDraw,pRoot);
			return(TRUE);
		}

	return(FALSE);
}

//--- Affichage de l'image ---

void Infos_DrawImage(DRAWITEMSTRUCT *pDraw, NODE *pRoot)
{
	LSFILE*		pFile;

	pFile = lsv_FindFile(pRoot,NULL,szPNGext);
	if (pFile)
		{
		if (pFile->hBitmap)
			{
			HDC		hDC;
			HBITMAP		hDefBitmap;
			BITMAP		bm;
			int		iBltMode;

			hDC = CreateCompatibleDC(pDraw->hDC);
			if (hDC)
				{
				hDefBitmap = SelectObject(hDC,pFile->hBitmap);
				iBltMode = SetStretchBltMode(pDraw->hDC,COLORONCOLOR);
				GetObject(pFile->hBitmap,sizeof(BITMAP),&bm);
				StretchBlt(pDraw->hDC,pDraw->rcItem.left,pDraw->rcItem.top,pDraw->rcItem.right-pDraw->rcItem.left,pDraw->rcItem.bottom-pDraw->rcItem.top,hDC,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
				SetStretchBltMode(pDraw->hDC,iBltMode);
				SelectObject(hDC,hDefBitmap);
				DeleteDC(hDC);
				DrawEdge(pDraw->hDC,&pDraw->rcItem,BDR_SUNKENOUTER,BF_RECT);
				return;
				}
			}
		}

	FillRect(pDraw->hDC,&pDraw->rcItem,GetStockObject(BLACK_BRUSH));
	DrawEdge(pDraw->hDC,&pDraw->rcItem,BDR_SUNKENOUTER,BF_RECT);
	return;
}
