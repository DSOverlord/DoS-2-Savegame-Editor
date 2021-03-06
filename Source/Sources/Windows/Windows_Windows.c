
//<<>-<>>---------------------------------------------------------------------()
/*
	Gestion des messages des fen皻res
									      */
//()-------------------------------------------------------------------<<>-<>>//

// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //
// 中�									  中� //
// 中� Donn嶪s							  	  中� //
// 中�									  中� //
// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //

#include "Application.h"
#include "About.h"
#include "Texts.h"
#include "Menus.h"
#include "Mods.h"
#include "Files.h"
#include "Infos.h"
#include "LastFiles.h"
#include "Requests.h"
#include "Divine.h"
#include "GameEdit.h"
#include "Utils.h"

extern APPLICATION		App;
extern CUSTOMMENUTEMPLATE	MainMenu;
extern UINT			TextsAttr[];
extern UINT			TextsPts[];


// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //
// 中�									  中� //
// 中� Processus principal						  中� //
// 中�									  中� //
// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //

LRESULT CALLBACK Window_Proc(HWND hWnd, UINT uMsgId, WPARAM wParam, LPARAM lParam)
{
	LONG_PTR	lptrWindowID;

	if (uMsgId == WM_CREATE)
		{
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)(((CREATESTRUCT *)lParam)->lpCreateParams));
		switch((LONG_PTR)(((CREATESTRUCT *)lParam)->lpCreateParams))
			{
			case WINDOW_MAIN:
				return(Window_Create(hWnd));
			case WINDOW_TREE:
				return(Tree_Create(hWnd));
			}
		return(-1);
		}

	lptrWindowID = GetWindowLongPtr(hWnd,GWLP_USERDATA);
	switch(lptrWindowID)
		{
		case WINDOW_MAIN:
			return(Window_ProcessMessages(hWnd,uMsgId,wParam,lParam));
		case WINDOW_TREE:
			return(Tree_ProcessMessages(hWnd,uMsgId,wParam,lParam));
		}

	return(DefWindowProc(hWnd,uMsgId,wParam,lParam));
}


// 姣遙 Lecture des messages provenant de la fen皻re principale 垂垂垂垂姣

LRESULT Window_ProcessMessages(HWND hWnd, UINT uMsgId, WPARAM wParam, LPARAM lParam)
{
	switch(uMsgId)
		{
		#if _DEBUG
		case WM_KEYDOWN:
			if (wParam != VK_F1) break;
			//Game_Talents();
			//Game_Bonus(hWnd,NULL,NULL);
			Game_EditValue(hWnd,NULL,DATA_TYPE_BOOSTERS,NULL,NULL);
			//Mods_Dialog();
			break;
		#endif

		case WM_PAINT:
			if (GetUpdateRect(hWnd,NULL,FALSE))
				{
				HDC		hDC;
				PAINTSTRUCT	paintStruct;
				RECT		rcWindow;

				hDC = BeginPaint(hWnd,&paintStruct);
				GetClientRect(hWnd,&rcWindow);
				FillRect(hDC,&rcWindow,GetSysColorBrush(COLOR_BTNFACE));
				if (App.hwndStatus)
					{
					RECT	rcStatus;
					RECT	rcTemp;
					CopyRect(&rcTemp,&rcWindow);
					GetWindowRect(App.hwndStatus,&rcStatus);
					MapWindowPoints(NULL,hWnd,(POINT *)&rcStatus,2);
					SubtractRect(&rcWindow,&rcTemp,&rcStatus);
					}
				Game_Paint(hWnd,hDC,&rcWindow);
				DrawEdge(hDC,&rcWindow,EDGE_SUNKEN,BF_BOTTOM);
				EndPaint(hWnd,&paintStruct);
				}
			break;

		case WM_MEASUREITEM:
			Window_MeasureItems(hWnd,(UINT)wParam,(MEASUREITEMSTRUCT *)lParam);
			break;

		case WM_DRAWITEM:
			Window_DrawItems((DRAWITEMSTRUCT *)lParam);
			break;

		case WM_NOTIFY:
			Window_Notify(hWnd,(UINT)wParam,(NMHDR *)lParam);
			break;

		case WM_COMMAND:
			Window_Command(hWnd,(UINT)HIWORD(wParam),(UINT)LOWORD(wParam),(HWND)lParam);
			break;

		case WM_MESSAGEBOX:
			MessageBox(hWnd,(WCHAR *)lParam,NULL,wParam);
			LocalFree((WCHAR *)lParam);
			break;

		case WM_UPDATEPROGRESS:
			Status_UpdateProgress(wParam,lParam);
			break;

		case WM_CLOSE:
			if (App.hThread)
				{
				MessageBox(hWnd,Locale_GetText(TEXT_THREAD_RUNNING),Locale_GetText(TEXT_TITLE_INFO),MB_ICONINFORMATION|MB_OK);
				break;
				}
			if (App.Config.bSaveOnExit && !Config_Save(TRUE,&App.Config)) break;
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			Divine_Close();
			Misc_SetWindowText(App.hWnd,&App.pszWindowTitle,szTitle,NULL);
			Status_SetText(NULL);
			ShowWindow(hWnd,SW_HIDE);
			SetMenu(hWnd,NULL);
			Menu_Release(App.hMenu,&MainMenu);
			App.hMenu = NULL;
			PostQuitMessage(0);
			break;

		default:
			return(DefWindowProc(hWnd,uMsgId,wParam,lParam));
		}

	return(0);
}


// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //
// 中�									  中� //
// 中� WM_CREATE -- Cr嶧tion d'une nouvelle fen皻re			  中� //
// 中�									  中� //
// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //

LRESULT Window_Create(HWND hWnd)
{
	App.Font.hFont = GetStockObject(DEFAULT_GUI_FONT);
	App.hWnd = hWnd;

	if (!App.Font.hFont) goto Error_0;
	if (!Font_GetInfo(hWnd,&App.Font)) goto Error_0;
	if (!Status_CreateWindow()) goto Error_0;
	if (!Game_CreateLayout()) goto Error_0;

	Menu_SetFlag(App.hMenu,IDM_CONFIGSAVEONEXIT,App.Config.bSaveOnExit);
	Menu_SetFlag(App.hMenu,IDM_CONFIGCAPOVERRIDE,App.Config.bCapOverride);

	LastFiles_LoadList();
	return(0);

Error_0:Request_PrintError(hWnd,Locale_GetText(TEXT_ERR_WINDOW_CREATE),NULL,MB_ICONHAND);
	App.hWnd = NULL;
	return(-1);
}


// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //
// 中�									  中� //
// 中� WM_DRAWITEM -- Affichage des diff廨ents 幨幦ents de la fen皻re	  中� //
// 中�									  中� //
// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //

//--- Dimension des 幨幦ents ---

void Window_MeasureItems(HWND hWnd, UINT uCtlId, MEASUREITEMSTRUCT *pMeasure)
{
	if (pMeasure->CtlType == ODT_MENU)
		{
		Menu_MeasureItems(hWnd,uCtlId,pMeasure);
		return;
		}

	if (pMeasure->CtlType == ODT_COMBOBOX)
		{
		if (pMeasure->CtlID == CTLID_CHARACTERS_LIST) Game_MeasureList(pMeasure);
		return;
		}

	if (pMeasure->CtlType == ODT_LISTVIEW)
		{
		if (pMeasure->CtlID == CTLID_INVENTORY) Game_MeasureInventory(pMeasure);
		return;
		}

	return;
}

//--- Affichage des 幨幦ents ---

void Window_DrawItems(DRAWITEMSTRUCT *pDraw)
{
	if (pDraw->hwndItem == App.hwndStatus)
		{
		if (pDraw->itemID == STATUS_TEXT) Status_DrawStatusText(pDraw);
		else if (pDraw->itemID == STATUS_PROGRESS) Status_DrawProgressBar(pDraw);
		return;
		}

	if (pDraw->CtlType == ODT_BUTTON)
		{
		Game_PaintButton(pDraw);
		return;
		}

	if (pDraw->CtlType == ODT_STATIC)
		{
		Game_PaintBag(pDraw);
		return;
		}

	if (pDraw->CtlType == ODT_MENU)
		{
		Menu_DrawItems(pDraw);
		return;
		}

	if (pDraw->CtlType == ODT_COMBOBOX)
		{
		if (pDraw->CtlID == CTLID_CHARACTERS_LIST) Game_DrawList(pDraw);
		return;
		}

	if (pDraw->CtlType == ODT_LISTVIEW)
		{
		if (pDraw->CtlID == CTLID_INVENTORY) Game_DrawInventory(pDraw);
		return;
		}

	return;
}


// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //
// 中�									  中� //
// 中� WM_NOTIFY -- Messages des contr犨es				  中� //
// 中�									  中� //
// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //

void Window_Notify(HWND hWnd, UINT idCtrl, NMHDR *pStruct)
{
	switch(pStruct->code)
		{
		case NM_DBLCLK:
			switch(idCtrl)
				{
				case CTLID_INVENTORY:
					Game_InventoryMenu(NULL,-1);
					break;
				}
			break;
		case NM_RCLICK:
			switch(idCtrl)
				{
				case CTLID_INVENTORY:
					Game_InventoryMenu(NULL,0);
					break;
				}
			break;
		}
	return;
}


// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //
// 中�									  中� //
// 中� WM_COMMAND -- D廧lacement de la souris				  中� //
// 中�									  中� //
// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //

void Window_Command(HWND hWnd, UINT uCode, UINT idCtrl, HWND hwndCtrl)
{
	switch(uCode)
		{
		case BN_CLICKED:
		case CBN_SELCHANGE:
			switch(idCtrl)
				{
				case IDM_OPENSAVEGAME:
					Divine_Select();
					break;
				case IDM_OPENSAVEAS:
					Divine_OpenAs();
					break;
				case IDM_RECENTREMOVEOBSOLETE:
					LastFiles_RemoveObsolete();
					break;
				case IDM_RECENTREMOVEALL:
					LastFiles_RemoveAll();
					break;
				case IDM_INFOS:
					Infos_Show();
					break;
				case IDM_WRITESAVEGAME:
					Divine_Write();
					break;
				case IDM_WRITESAVEAS:
					Divine_WriteAs();
					break;
				case IDM_CLOSE:
					Divine_Close();
					break;
				case IDM_QUIT:
					PostMessage(hWnd,WM_CLOSE,0,0);
					break;
				case IDM_SHOWSAVETREE:
					if (MessageBox(hWnd,Locale_GetText(TEXT_BIGTREE_WARNING),Locale_GetText(TEXT_TITLE_WARNING),MB_ICONWARNING|MB_OKCANCEL) != IDOK) break;
					Tree_Open((XML_NODE *)App.Game.Save.nodeXMLRoot.next);
					break;
				case IDM_SHOWMETATREE:
					Infos_Tree();
					break;
				case IDM_SHOWCHARTREE:
					Tree_Open((XML_NODE *)App.Game.pdcCurrent->pxnRoot->children.next);
					break;
				case IDM_REMOVEMODS:
					Mods_Dialog();
					break;
				case IDM_CONVERTER:
					Divine_RunConverter();
					break;
				case IDM_INVNAME:
					Game_InventoryMenu(NULL,IDM_INVNAME);
					break;
				case IDM_INVBACKPACKCLOSE:
					Game_InventoryMenu(NULL,IDM_INVBACKPACKCLOSE);
					break;
				case IDM_INVNODES:
					Game_InventoryMenu(NULL,IDM_INVNODES);
					break;
				case IDM_CONFIGCAPOVERRIDE:
					App.Config.bCapOverride = Menu_GetFlag(App.hMenu,IDM_CONFIGCAPOVERRIDE);
					if (App.Config.bCapOverride) App.Config.bCapOverride = FALSE;
					else App.Config.bCapOverride = TRUE;
					Menu_SetFlag(App.hMenu,IDM_CONFIGCAPOVERRIDE,App.Config.bCapOverride);
					break;
				case IDM_CONFIGSAVE:
					Config_Save(FALSE,&App.Config);
					break;
				case IDM_CONFIGSAVEONEXIT:
					App.Config.bSaveOnExit = Menu_GetFlag(App.hMenu,IDM_CONFIGSAVEONEXIT);
					if (App.Config.bSaveOnExit) App.Config.bSaveOnExit = FALSE;
					else App.Config.bSaveOnExit = TRUE;
					Menu_SetFlag(App.hMenu,IDM_CONFIGSAVEONEXIT,App.Config.bSaveOnExit);
					break;
				case IDM_CONFIGLANGUAGE:
					Config_SelectLanguage();
					break;
				case IDM_CONFIGTEMPLOCATION:
					Config_SelectTempLocation();
					break;
				case IDM_CONFIGSAVELOCATION:
					Config_SelectSaveLocation();
					break;
				case IDM_CONFIGRESETLOCATIONS:
					Config_ResetLocations();
					break;
				case IDM_ABOUT:
					About_Display(hWnd);
					break;
				case CTLID_CHARACTERS_LIST:
					Game_SaveTopIndex();
					Game_CharacterChanged(TRUE);
					break;
				case CTLID_ABILITIES:
					Game_Abilities();
					break;
				case CTLID_TAGS:
					Game_Tags();
					break;
				case CTLID_TALENTS:
					Game_Talents();
					break;
				case CTLID_MENU:
					Game_InventoryMenu(hwndCtrl,0);
					break;
				default:
					if (idCtrl >= IDM_LASTFILES) LastFiles_Reload(idCtrl);
					else if (idCtrl >= CTLID_ATTRIBUTES && idCtrl <= CTLID_ATTRIBUTES+6)
						{
						if (wcstol(xml_GetThisAttrValue(App.Game.pdcCurrent->pxaAttributes[idCtrl-CTLID_ATTRIBUTES]),NULL,10) > GAME_ATTRIBUTE_MAX && !App.Config.bCapOverride)
							{
							if (MessageBox(hWnd,Locale_GetText(TEXT_OVERRIDE_ATTRIBUTE),Locale_GetText(TEXT_TITLE_REQUEST),MB_YESNO|MB_ICONQUESTION) == IDYES)
								{
								App.Config.bCapOverride = TRUE;
								Menu_SetFlag(App.hMenu,IDM_CONFIGCAPOVERRIDE,App.Config.bCapOverride);
								}
							}
						if (Game_EditSetValue(hWnd,Locale_GetText(TextsAttr[idCtrl-CTLID_ATTRIBUTES]),App.Game.pdcCurrent->pxaAttributes[idCtrl-CTLID_ATTRIBUTES],GAME_ATTRIBUTE_MIN,App.Config.bCapOverride?GAME_ATTRIBUTE_OVERRIDE:GAME_ATTRIBUTE_MAX))
							InvalidateRect(App.hWnd,NULL,FALSE);
						}
					else if (idCtrl >= CTLID_POINTS && idCtrl <= CTLID_POINTS+4)
						{
						if (Game_EditSetValue(hWnd,Locale_GetText(TextsPts[idCtrl-CTLID_POINTS]),App.Game.pdcCurrent->pxaPoints[idCtrl-CTLID_POINTS],GAME_POINTS_MIN,GAME_POINTS_MAX))
							InvalidateRect(App.hWnd,NULL,FALSE);
						}
					break;
				}
			break;
		}

	return;
}
