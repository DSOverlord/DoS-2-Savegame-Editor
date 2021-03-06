
//<<>-<>>---------------------------------------------------------------------()
/*
	D嶨initions des listes
									      */
//()-------------------------------------------------------------------<<>-<>>//

#ifndef _LISTS_INCLUDE
#define _LISTS_INCLUDE


// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //
// 中�									  中� //
// 中� Structures							  中� //
// 中�									  中� //
// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //

typedef struct NODE {
	struct NODE*	next;
	struct NODE*	prev;
	unsigned int	type;
} NODE;


// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //
// 中�									  中� //
// 中� Prototypes							  中� //
// 中�									  中� //
// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //

long			List_EntryCount(NODE *);
void			List_Move(NODE *,NODE *);
void			List_Append(NODE *,NODE *);
void			List_AddEntry(NODE *,NODE *);
void			List_InsertEntry(NODE *,NODE *,BOOL);
void			List_RemEntry(NODE *);
void			List_ReleaseMemory(NODE *);
long			List_CompareEntries(NODE *,NODE *);

#endif
