
//<<>-<>>---------------------------------------------------------------------()
/*
	Routine des gestions des listes en tout genre
									      */
//()-------------------------------------------------------------------<<>-<>>//

// �������������������������������������������������������������������������� //
// ���									  ��� //
// ��� Donn�es								  ��� //
// ���									  ��� //
// �������������������������������������������������������������������������� //

#include "_Global.h"
#include "Application.h"
#include "Lists.h"

extern APPLICATION	App;


// �������������������������������������������������������������������������� //
// ���									  ��� //
// ��� Fonctions							  ��� //
// ���									  ��� //
// �������������������������������������������������������������������������� //

// ���� Calcul le nombre d'entr�e dans une liste �������������������������

long List_EntryCount(NODE *pEntryListRoot)
{
	NODE   *pList;
	long	lResult = 0;

	for (pList = pEntryListRoot->next; pList != 0; pList = pList->next, lResult++);
	return(lResult);
}


// ���� D�place toutes les entr�es d'une liste vers une autre liste ������

void List_Move(NODE *pSrcListRoot, NODE *pDestListRoot)
{
	if (!pSrcListRoot->next)
		{
		pDestListRoot->next = NULL;
		return;
		}

	pDestListRoot->next = pSrcListRoot->next;
	pDestListRoot->next->prev = pDestListRoot;
	pSrcListRoot->next = NULL;
	return;
}


// ���� D�place une liste � la fin d'une autre liste ���������������������

//!\ La liste source ne doit pas avoir de racine

void List_Append(NODE *pSrcList, NODE *pDestList)
{
	NODE *pList;

	if (!pSrcList) return;

	if (!pDestList->next) pList = pDestList;
	else for (pList = pDestList->next; pList->next != NULL; pList = pList->next);
	pList->next = pSrcList;
	pSrcList->prev = pList;
	return;
}


// ���� Ajoute une entr�e ������������������������������������������������

void List_AddEntry(NODE *pNewEntry, NODE *pEntryListRoot)
{
	NODE *pList;

	for (pList = pEntryListRoot; pList->next != 0; pList = pList->next);

	pList->next = pNewEntry;
	pNewEntry->prev = pList;
	pNewEntry->next = NULL;
	return;
}


// ���� Ins�re une entr�e ������������������������������������������������

void List_InsertEntry(NODE *pNewEntry, NODE *pInsEntry, BOOL bInsertAfter)
{
	NODE *pNext;
	NODE *pPrev;

	pNext = pInsEntry->next;
	pPrev = pInsEntry->prev;

	if (bInsertAfter)
		{
		pNewEntry->next = pNext;
		pNewEntry->prev = pInsEntry;
		pInsEntry->next = pNewEntry;
		if (pNext) pNext->prev = pNewEntry;
		}
	else
		{
		pNewEntry->next = pInsEntry;
		pNewEntry->prev = pPrev;
		pInsEntry->prev = pNewEntry;
		if (pPrev) pPrev->next = pNewEntry;
		}

	return;
}


// ���� Supprime une entr�e ����������������������������������������������

void List_RemEntry(NODE *pEntry)
{
	NODE *pNext;
	NODE *pPrev;

	pNext = pEntry->next;
	pPrev = pEntry->prev;

	if (pNext) pNext->prev = pPrev;
	if (pPrev) pPrev->next = pNext;

	pEntry->next = NULL;
	pEntry->prev = NULL;
	return;
}


// ���� Supprime la m�moire utilis�e par une liste �����������������������

void List_ReleaseMemory(NODE *pListRoot)
{
	NODE *pEntry;
	NODE *pNext;

	for (pEntry = pListRoot->next; pEntry != 0;)
		{
		pNext = pEntry->next;
		HeapFree(App.hHeap,0,pEntry);
		pEntry = pNext;
		}

	pListRoot->next = NULL;
	pListRoot->prev = NULL;
	return;
}


// ���� Compare deux entr�es ���������������������������������������������

long List_CompareEntries(NODE *pFirstEntry, NODE *pSecondEntry)
{
	NODE *pEntry;

	pEntry = pFirstEntry->next;
	while (pEntry)
		{
		// First entry precedes the second entry ?
		if (pEntry == pSecondEntry) return(-1);
		pEntry = pEntry->next;
		}

	// First entry follows the second entry
	return(1);
}
