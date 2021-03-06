
//<<>-<>>---------------------------------------------------------------------()
/*
	Gestion des fichiers de donn嶪s
									      */
//()-------------------------------------------------------------------<<>-<>>//

// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //
// 中�									  中� //
// 中� Donn嶪s								  中� //
// 中�									  中� //
// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //

#include "Application.h"
#include "Game.h"
#include "GameEdit.h"
#include "Requests.h"
#include "Utils.h"
#include "Texts.h"
#include "Locale.h"

extern APPLICATION		App;

static GAMEDATAFLAG		GameDataFlags[] = {
						{ "ITEM", DATA_TYPE_IS_ITEM, 0 },
						{ "ARMOR", DATA_TYPE_IS_ARMOR, FILTER_ARMORS_ANY },
						{ "WEAPON", DATA_TYPE_IS_WEAPON, FILTER_WEAPONS_ANY },
						{ "ACCESSORY", DATA_TYPE_IS_ACCESSORY, 0 },
						{ "AMULET", DATA_TYPE_IS_ACCESSORY, FILTER_ARMORS_AMULET },
						{ "RING", DATA_TYPE_IS_ACCESSORY, FILTER_ARMORS_RING },
						{ "BOOTS", DATA_TYPE_IS_ARMOR, FILTER_ARMORS_BOOTS },
						{ "HELMET", DATA_TYPE_IS_ARMOR, FILTER_ARMORS_HELMET },
						{ "BELT", DATA_TYPE_IS_ARMOR, FILTER_ARMORS_BELT },
						{ "GLOVES", DATA_TYPE_IS_ARMOR, FILTER_ARMORS_GLOVES },
						{ "LEGGINGS", DATA_TYPE_IS_ARMOR, FILTER_ARMORS_PANTS },
						{ "CHEST", DATA_TYPE_IS_ARMOR, FILTER_ARMORS_UPPERBODY },
						{ "SHIELD", DATA_TYPE_IS_ARMOR, FILTER_SHIELDS },
						{ NULL, 0, 0 },
					};


// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //
// 中�									  中� //
// 中� Chargement							  中� //
// 中�									  中� //
// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //

BOOL Game_LoadDataFile(HWND hWnd, WCHAR *pszName, UINT uType, NODE *pRoot)
{
	GAMEDATAPARSER	Parser;
	DWORD		dwBytes;

	//--- Initialisations ---

	ZeroMemory(&Parser,sizeof(GAMEDATAPARSER));
	Parser.hFile = INVALID_HANDLE_VALUE;
	Parser.bSuccess = FALSE;

	switch(uType)
		{
		case DATA_TYPE_BOOSTERS:
		case DATA_TYPE_RUNES:
		case DATA_TYPE_TAGS:
			if (!Locale_Load(App.hWnd,szLangPath,App.Config.pszLocaleName,LOCALE_TYPE_MISC,(void **)&Parser.pLocale,NULL)) return(FALSE);
			break;
		}

	//--- Chargement du fichier ---

	Parser.hFile = CreateFile(pszName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,NULL);
	if (Parser.hFile == INVALID_HANDLE_VALUE) goto Done;

	Parser.dwFileSize = GetFileSize(Parser.hFile,NULL);
	if (Parser.dwFileSize == 0xFFFFFFFF) goto Done;

	Parser.pFileBuffer = HeapAlloc(App.hHeap,0,Parser.dwFileSize);
	if (!Parser.pFileBuffer) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); goto Done; }

	ReadFile(Parser.hFile,Parser.pFileBuffer,Parser.dwFileSize,&dwBytes,NULL);
	if (dwBytes != Parser.dwFileSize) goto Done;

	//--- Boucle de traitement ---

	Parser.pFilePtr = Parser.pFileBuffer;
	Parser.bSuccess = TRUE;
	while(1)
		{
		// Ignores leading spaces
		Parser.pFilePtr = Game_LoadLeadingSpaces(Parser.pFilePtr,Parser.pFileBuffer,Parser.dwFileSize);
		if (!Parser.pFilePtr) break;
		// Ignores comments
		if (*Parser.pFilePtr == '#')
			{
			Parser.pFilePtr = Game_LoadNextLine(Parser.pFilePtr,Parser.pFileBuffer,Parser.dwFileSize,FALSE);
			if (!Parser.pFilePtr) break;
			continue;
			}
		// Gets line pointers
		Parser.pLineBegin = Parser.pFilePtr;
		Parser.pLineEnd = Parser.pFilePtr = Game_LoadNextLine(Parser.pFilePtr,Parser.pFileBuffer,Parser.dwFileSize,TRUE);
		if (Parser.pLineBegin == Parser.pLineEnd) continue;
		*(Parser.pLineEnd-1) = 0;
		switch(uType)
			{
			//--- Boosters, Runes, Tags ---
			case DATA_TYPE_BOOSTERS:
			case DATA_TYPE_RUNES:
			case DATA_TYPE_TAGS:
				Parser.bSuccess = FALSE;
				Parser.pszResults[0] = NULL;
				Parser.pszResults[1] = NULL;
				Parser.pszResults[2] = NULL;
				Parser.pszResults[3] = NULL;
				//--- Alloue la structure
				Parser.pData = HeapAlloc(App.hHeap,HEAP_ZERO_MEMORY,sizeof(GAMEDATA));
				if (!Parser.pData) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); goto Done; }
				List_AddEntry((NODE *)Parser.pData,pRoot);
				//--- V廨ifie si le tag est prot嶲�
				if (uType == DATA_TYPE_TAGS)
					{
					if (*Parser.pLineBegin == '*')
						{
						Parser.pData->tag.bProtected = TRUE;
						Parser.pLineBegin++;
						}
					else if (*Parser.pLineBegin == '-')
						{
						Parser.pData->tag.bProtected = TRUE;
						Parser.pData->tag.bHidden = TRUE;
						Parser.pLineBegin++;
						}
					}
				//--- Ignore les caract鋨es superflus � la fin
				for (Parser.pLineEnd = Parser.pLineBegin; *Parser.pLineEnd != 0 && (char)*Parser.pLineEnd > ' ' && *Parser.pLineEnd != '#'; Parser.pLineEnd++);
				*Parser.pLineEnd = 0;
				if (!*Parser.pLineBegin)
					{
					List_RemEntry((NODE *)Parser.pData);
					HeapFree(App.hHeap,0,Parser.pData);
					break;
					}
				//--- Copie l'identifiant
				Parser.pData->pszId = Misc_UTF8ToWideChar((char *)Parser.pLineBegin);
				if (!Parser.pData->pszId) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); goto Done; }
				//--- Copie les traductions 憝entuelles
				switch(uType)
					{
					case DATA_TYPE_BOOSTERS:
						Locale_QueryID(Parser.pLocale->db,szDataBaseBoosters,Parser.pData->pszId,L"text,type,slot",Parser.pszResults,3);
						Parser.pData->booster.pszType = Parser.pszResults[1];
						Parser.pData->booster.pszSlot = Parser.pszResults[2];
						break;
					case DATA_TYPE_TAGS:
						Locale_QueryID(Parser.pLocale->db,szDataBaseTags,Parser.pData->pszId,L"text,description",Parser.pszResults,2);
						Parser.pData->tag.pszDescription = Parser.pszResults[1];
						break;
					case DATA_TYPE_RUNES:
						Locale_QueryID(Parser.pLocale->db,szDataBaseRunes,Parser.pData->pszId,L"text,bonus1,bonus2,bonus3",Parser.pszResults,4);
						Parser.pData->rune.pszBonus1 = Parser.pszResults[1];
						Parser.pData->rune.pszBonus2 = Parser.pszResults[2];
						Parser.pData->rune.pszBonus3 = Parser.pszResults[3];
						break;
					}
				Parser.pData->pszText = Parser.pszResults[0];
				Parser.bSuccess = TRUE;
				SetLastError(ERROR_SUCCESS);
				break;

			//--- Items ---
			case DATA_TYPE_ITEMS:
				Parser.bSuccess = FALSE;
				//--- Alloue la structure
				Parser.pItem = HeapAlloc(App.hHeap,HEAP_ZERO_MEMORY,sizeof(GAMEDATAITEM));
				if (!Parser.pItem) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); goto Done; }
				List_AddEntry((NODE *)Parser.pItem,pRoot);
				//--- R嶰up鋨e le premier mot (nom de l'ic獼e)
				Parser.pLinePtr = Game_LoadSplitWord(Parser.pLineBegin);
				Parser.pItem->pszIconName = Misc_UTF8ToWideChar((char *)Parser.pLineBegin);
				if (Parser.pItem->pszIconName)
					{
					if (!wcscmp(Parser.pItem->pszIconName,L"none"))
						{
						HeapFree(App.hHeap,0,Parser.pItem->pszIconName);
						Parser.pItem->pszIconName = NULL;
						}
					}
				else { SetLastError(ERROR_NOT_ENOUGH_MEMORY); goto Done; }
				//--- R嶰up鋨e le second mot (type d'objet) ---
				Parser.pLineBegin = Game_LoadLeadingSpaces(Parser.pLinePtr,Parser.pLinePtr,strlen((char *)Parser.pLinePtr));
				if (!Parser.pLineBegin) break;
				Parser.pLinePtr = Game_LoadSplitWord(Parser.pLineBegin);
				Game_LoadGetItemInfos((char *)Parser.pLineBegin,Parser.pItem);
				//--- Boucle de r嶰up廨ation des cha螽es de comparaison
				do {
					//--- Ignore les caract鋨es superflus au d嶵ut
					Parser.pLineBegin = Game_LoadLeadingSpaces(Parser.pLinePtr,Parser.pLinePtr,strlen((char *)Parser.pLinePtr));
					if (!Parser.pLineBegin) break;
					//--- R嶰up鋨e le prochain mot (cha螽e)
					Parser.pLinePtr = Game_LoadSplitWord(Parser.pLineBegin);
					if (Parser.pLinePtr)
						{
						char	cStringType;
						//--- Interruption s'il s'agit d'un commentaire
						if (*Parser.pLineBegin == '#') break;
						//--- V廨ifie qu'il s'agit bien d'une cha螽e de comparaison (|char|char[])
						if (*Parser.pLineBegin != '|') { SetLastError(ERROR_INVALID_DATA); goto Done; }
						Parser.pLineBegin++;
						cStringType = *Parser.pLineBegin;
						if (!*Parser.pLineBegin++) { SetLastError(ERROR_INVALID_DATA); goto Done; }
						if (*Parser.pLineBegin++ != '|') { SetLastError(ERROR_INVALID_DATA); goto Done; }
						//--- Copie la cha螽e de comparaison
						switch(cStringType)
							{
							case 'f':
								Parser.pItem->filter.pszFull = Misc_UTF8ToWideChar((char *)Parser.pLineBegin);
								if (!Parser.pItem->filter.pszFull) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); goto Done; }
								break;
							case 'b':
								Parser.pItem->filter.pszBegin = Misc_UTF8ToWideChar((char *)Parser.pLineBegin);
								if (!Parser.pItem->filter.pszBegin) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); goto Done; }
								break;
							case 'e':
								Parser.pItem->filter.pszEnd = Misc_UTF8ToWideChar((char *)Parser.pLineBegin);
								if (!Parser.pItem->filter.pszEnd) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); goto Done; }
								break;
							case 'c':
								Parser.pItem->filter.pszContent = Misc_UTF8ToWideChar((char *)Parser.pLineBegin);
								if (!Parser.pItem->filter.pszContent) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); goto Done; }
								break;
							default:SetLastError(ERROR_INVALID_DATA);
								goto Done;
							}
						}
				} while(Parser.pLinePtr);
				Parser.bSuccess = TRUE;
				SetLastError(ERROR_SUCCESS);
				break;
			}
		}

	//--- Termin� ---

Done:	if (GetLastError() != ERROR_SUCCESS)
		{
		Request_PrintErrorEx(hWnd,Locale_GetText(TEXT_ERR_DATAFILE),NULL,MB_ICONHAND,pszName);
		Game_UnloadDataFile(uType,pRoot);
		}
	if (Parser.pFileBuffer) HeapFree(App.hHeap,0,Parser.pFileBuffer);
	if (Parser.hFile != INVALID_HANDLE_VALUE) CloseHandle(Parser.hFile);
	Locale_Unload(LOCALE_TYPE_MISC,(void **)&Parser.pLocale,NULL);
	return(Parser.bSuccess);
}

//--- S廧are un mot de la ligne ---

BYTE* Game_LoadSplitWord(BYTE *pszBuffer)
{
	if (!*pszBuffer) return(NULL);
	do { if (!*pszBuffer) return(pszBuffer); } while((char)*pszBuffer++ > ' ');
	*--pszBuffer = 0;
	return(++pszBuffer);
}

//--- Ignore les caract鋨es blancs et les lignes vides ---

BYTE* Game_LoadLeadingSpaces(BYTE *pszBuffer, BYTE *pszBegin, DWORD dwSize)
{
	do { if (pszBuffer >= pszBegin+dwSize) return(NULL); } while((char)*pszBuffer++ <= ' ');
	return(--pszBuffer);
}

//--- Prochaine ligne ---

BYTE* Game_LoadNextLine(BYTE *pszBuffer, BYTE *pszBegin, DWORD dwSize, BOOL bEOFvalid)
{
	do { if (pszBuffer >= pszBegin+dwSize) return(bEOFvalid?pszBuffer:NULL); } while(*pszBuffer++ != '\n');
	return(pszBuffer);
}

//--- Recup鋨e les informations de l'objet ---

void Game_LoadGetItemInfos(char *pszFlags, GAMEDATAITEM *pItem)
{
	char*	pszNextFlags;

	pItem->uType = DATA_TYPE_IS_ITEM;
	pItem->uFlags = 0;

	pszNextFlags = pszFlags;
	while ((pszNextFlags = strchr(pszNextFlags,'|')) != NULL)
		{
		*pszNextFlags = 0;
		Game_LoadGetItemInfo(pszFlags,pItem);
		pszFlags = ++pszNextFlags;
		}
	if (strlen(pszFlags)) Game_LoadGetItemInfo(pszFlags,pItem);

	return;
}

void Game_LoadGetItemInfo(char *pszFlag, GAMEDATAITEM *pItem)
{
	int	i;

	for (i = 0; GameDataFlags[i].pszFlagName; i++)
		{
		if (strcmp(GameDataFlags[i].pszFlagName,pszFlag)) continue;
		pItem->uType = GameDataFlags[i].uType;
		pItem->uFlags |= GameDataFlags[i].uFlags;
		break;
		}

	return;
}


// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //
// 中�									  中� //
// 中� Lib廨ation							  中� //
// 中�									  中� //
// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //

void Game_UnloadDataFile(UINT uType, NODE *pRoot)
{
	GAMEDATAITEM*	pItem;
	GAMEDATA*	pData;

	switch(uType)
		{
		case DATA_TYPE_BOOSTERS:
		case DATA_TYPE_TAGS:
		case DATA_TYPE_RUNES:
			for (pData = (GAMEDATA *)pRoot->next; pData != NULL; pData = (GAMEDATA *)pData->node.next)
				{
				if (pData->pszId) HeapFree(App.hHeap,0,pData->pszId);
				if (pData->pszText) HeapFree(App.hHeap,0,pData->pszText);
				switch(uType)
					{
					case DATA_TYPE_BOOSTERS:
						if (pData->booster.pszType) HeapFree(App.hHeap,0,pData->booster.pszType);
						if (pData->booster.pszSlot) HeapFree(App.hHeap,0,pData->booster.pszSlot);
						break;
					case DATA_TYPE_TAGS:
						if (pData->tag.pszDescription) HeapFree(App.hHeap,0,pData->tag.pszDescription);
						break;
					case DATA_TYPE_RUNES:
						if (pData->rune.pszBonus1) HeapFree(App.hHeap,0,pData->rune.pszBonus1);
						if (pData->rune.pszBonus2) HeapFree(App.hHeap,0,pData->rune.pszBonus2);
						if (pData->rune.pszBonus3) HeapFree(App.hHeap,0,pData->rune.pszBonus3);
						break;
					}
				}
			List_ReleaseMemory(pRoot);
			break;

		case DATA_TYPE_ITEMS:
			for (pItem = (GAMEDATAITEM *)pRoot->next; pItem != NULL; pItem = (GAMEDATAITEM *)pItem->node.next)
				{
				if (pItem->pszIconName) HeapFree(App.hHeap,0,pItem->pszIconName);
				if (pItem->filter.pszFull) HeapFree(App.hHeap,0,pItem->filter.pszFull);
				if (pItem->filter.pszBegin) HeapFree(App.hHeap,0,pItem->filter.pszBegin);
				if (pItem->filter.pszEnd) HeapFree(App.hHeap,0,pItem->filter.pszEnd);
				if (pItem->filter.pszContent) HeapFree(App.hHeap,0,pItem->filter.pszContent);
				}
			List_ReleaseMemory(pRoot);
			break;
		}

	return;
}


// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //
// 中�									  中� //
// 中� Fonctions							  中� //
// 中�									  中� //
// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //

// 姣遙 Retrouve le nom d'un ic獼e 垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂�

WCHAR* Game_GetItemIconName(WCHAR *pszObjectName)
{
	GAMEDATAITEM *pItem = Game_GetItemData(pszObjectName);
	return(pItem?pItem->pszIconName:NULL);
}


// 姣遙 Retrouve le nom du type d'objet 垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂姣

WCHAR* Game_GetItemTypeName(WCHAR *pszObjectName)
{
	GAMEDATAITEM*	pItem;
	UINT		uLocaleID;

	uLocaleID = TEXT_LAST;
	pItem = Game_GetItemData(pszObjectName);

	if (pItem)
		{
		switch(pItem->uType)
			{
			case DATA_TYPE_IS_ARMOR:
				uLocaleID = TEXT_ITEM_IS_ARMOR;
				break;
			case DATA_TYPE_IS_WEAPON:
				uLocaleID = TEXT_ITEM_IS_WEAPON;
				break;
			case DATA_TYPE_IS_ACCESSORY:
				uLocaleID = TEXT_ITEM_IS_ACCESSORY;
				break;
			}
		}

	return(uLocaleID != TEXT_LAST?Locale_GetText(uLocaleID):szBlank);
}


// 姣遙 Retrouve le type d'objet 垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂�

UINT Game_GetItemType(WCHAR *pszObjectName)
{
	GAMEDATAITEM *pItem = Game_GetItemData(pszObjectName);
	return(pItem?pItem->uType:DATA_TYPE_IS_ITEM);
}


// 姣遙 Retrouve les drapeaux de l'objet 垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂�

UINT64 Game_GetItemFlags(WCHAR *pszObjectName)
{
	GAMEDATAITEM *pItem = Game_GetItemData(pszObjectName);
	return(pItem?pItem->uFlags:0);
}


// 姣遙 Retrouve le type d'objet 垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂�

GAMEDATAITEM* Game_GetItemData(WCHAR *pszObjectName)
{
	if (pszObjectName)
		{
		GAMEDATAITEM*	pItem = NULL;
		UINT		uNameLen = wcslen(pszObjectName);
		BOOL		bComp[4];

		for (pItem = (GAMEDATAITEM *)App.Game.nodeDataItems.next; pItem != NULL; pItem = (GAMEDATAITEM *)pItem->node.next)
			{
			bComp[0] = Game_CompareStrings(pItem->filter.pszFull,pszObjectName,uNameLen,CMP_TYPE_FULL);
			bComp[1] = Game_CompareStrings(pItem->filter.pszBegin,pszObjectName,uNameLen,CMP_TYPE_BEGIN);
			bComp[2] = Game_CompareStrings(pItem->filter.pszContent,pszObjectName,uNameLen,CMP_TYPE_CONTENT);
			bComp[3] = Game_CompareStrings(pItem->filter.pszEnd,pszObjectName,uNameLen,CMP_TYPE_END);
			if (bComp[0] && bComp[1] && bComp[2] && bComp[3]) return(pItem);
			}
		}
	return(NULL);
}


// 姣遙 Comparaisons 垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂�

BOOL Game_CompareStrings(WCHAR *pszCmp, WCHAR *pszName, UINT uNameLen, UINT uType)
{
	UINT	uCmpLen;

	if (!pszCmp) return(TRUE);
	uCmpLen =  wcslen(pszCmp);

	switch(uType)
		{
		case CMP_TYPE_FULL:
			if (uNameLen != uCmpLen) break;
			if (wcscmp(pszCmp,pszName)) break;
			return(TRUE);
		case CMP_TYPE_BEGIN:
			if (uNameLen < uCmpLen) break;
			if (wcsncmp(pszCmp,pszName,uCmpLen)) break;
			return(TRUE);
		case CMP_TYPE_END:
			if (uNameLen < uCmpLen) break;
			if (wcsncmp(pszCmp,pszName+uNameLen-uCmpLen,uCmpLen)) break;
			return(TRUE);
		case CMP_TYPE_CONTENT:
			if (uNameLen < uCmpLen) break;
			if (!wcsstr(pszName,pszCmp)) break;
			return(TRUE);
		}

	return(FALSE);
}
