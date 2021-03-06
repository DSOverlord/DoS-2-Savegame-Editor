
//<<>-<>>---------------------------------------------------------------------()
/*
	Fen皻res
									      */
//()-------------------------------------------------------------------<<>-<>>//

#ifndef _APPWINDOWS_INCLUDE
#define _APPWINDOWS_INCLUDE


// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //
// 中�									  中� //
// 中� D嶨initions							  中� //
// 中�									  中� //
// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //

#include "XML.h"

#define STATUS_REFRESH_MS		25

#define MAIN_WINDOW_WIDTH		1024
#define MAIN_WINDOW_HEIGHT		828

enum {
	WINDOW_MAIN			= 0,
	WINDOW_TREE
};

enum {
	WM_MESSAGEBOX			= WM_APP,
	WM_UPDATEPROGRESS
};

enum {
	STATUS_TEXT			= 0,
	STATUS_PROGRESS
};

enum {
	CTLID_CHARACTERS_LIST		= 10000,
	CTLID_ATTRIBUTES,
	CTLID_POINTS			= CTLID_ATTRIBUTES+7,
	CTLID_INVENTORY			= CTLID_POINTS+5,
	CTLID_MENU,
	CTLID_ABILITIES,
	CTLID_TAGS,
	CTLID_TALENTS,
	CTLID_INVENTORYNAME,
	CTLID_RUNES,
	CTLID_EDIT,
	CTLID_TREEVIEW,
	CTLID_STATUS,
	CTLID_TREESAVE,
	CTLID_TREEHELP,
	CTLID_TREECLOSE
};


// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //
// 中�									  中� //
// 中� Prototypes							  中� //
// 中�									  中� //
// 中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中中 //

// 姣遙 Main 垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂�

LRESULT CALLBACK	Window_Proc(HWND,UINT,WPARAM,LPARAM);
LRESULT			Window_ProcessMessages(HWND,UINT,WPARAM,LPARAM);

LRESULT			Window_Create(HWND);
void			Window_MeasureItems(HWND,UINT,MEASUREITEMSTRUCT *);
void			Window_DrawItems(DRAWITEMSTRUCT *);
void			Window_Notify(HWND,UINT,NMHDR *);
void			Window_Command(HWND,UINT,UINT,HWND);

// 姣遙 Status 垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂垂�

int			Status_CreateWindow(void);
void			Status_UpdateParts(void);

void			Status_DrawStatusText(DRAWITEMSTRUCT *);
void			Status_SetText(WCHAR *,...);

void			Status_DrawProgressBar(DRAWITEMSTRUCT *);
void			Status_UpdateProgress(UINT,UINT);

#endif
