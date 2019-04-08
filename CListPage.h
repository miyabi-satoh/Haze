/*=============================================================================
	
	File:         CListPage.h

	Description:  Defintions for CFileListView, CListPage.

=============================================================================*/

#ifndef _CLISTPAGE_H
#define _CLISTPAGE_H

/*=============================================================================
	#include statements
=============================================================================*/

#include <CMyDialog.h>
#include <CMyControl.h>
#include <list>
#include "CPreview.h"
#include "CDocument.h"
#include "General.h"
#include "CSumNail.h"

/*=============================================================================
  class CFileListView
  ファイル一覧用リストビュー
=============================================================================*/

class CFileListView : public CMyListView
{
public:
	CFileListView() : CMyListView() {}
	~CFileListView() {}

	void InitWindow(HWND hWnd);
	void SetItemForcus(int nIndex);
	void MoveItem(int iPrev, int iNext);
	void Update(CDocument& doc);
protected:

	HND_OnContextMenu();
	HND_OnLButtonDblClk();
	HND_OnDropFiles();
};

/*=============================================================================
  class CListPage
  「リストの設定」
=============================================================================*/

class CListPage : public CMyDialog
{
private:
	CPreview m_prev;
	CFileListView m_FileListView;

	CDocument m_FileData;

	std::list<CCatalogWindow> m_CWList, m_ListCWList;

public:
	CListPage() {}
	~CListPage() {}

	void Init(HWND hWndTab, int nCmdShow, RECT* pRect);
	int OnSave();
	int OnCancel();

protected:
	typedef std::list<std::string> strList;
	typedef std::list<CCatalogWindow>::iterator ccwNode;

	HND_WM_PROC();
	HND_OnInitDialog();
	HND_OnClose();
	HND_OnDropFiles();
	HND_OnCommand();

	int on_menu(int nID);
	int on_preview();
	int on_loadlist();
	int on_new();
	int on_saveas();
	int on_savelist();
	int on_addtion();
	int on_archive();
	int on_delete();
	int on_clear();
	int on_up();
	int on_down();
	int on_ceil();
	int on_floor();
	int on_config();
	int on_sum_list();
	int on_sum_folda();
	int on_sum_archive();
	int on_explorer();
	int on_setnext();

	int FileDialog(int, char *, size_t);
	void SetOpenFilter(int, std::string&);
	bool Chk_SameName(const char *);
	int Chk_Supported(const char *);
	int Chk_Supported(const char *, SpiArcFileInfo*);
	int file_AddList(const char *);
	int arc_AddList(const char *);
	int SeparateFileName(std::string&, std::string&, const char *);

	static int CALLBACK drag(const char *, int, int, long);
	static int add_list(const char *, LONG);

	void set_filename(BOOL = 1);

	int GetOpenFolderName(char *, int);

	void GetAllFormat(strList&, strList&, int);
	void Filter_All(strList&, std::string&, int);
	void Filter_Sep(strList&, strList&, std::string&);

	void DocumentIsChanged();

	void Document_Open();

	ccwNode AdjustCWList();

	int AskDocumentSave();
	int GetSaveFileName(char * szBuf, size_t size);
};

#endif /* _CLISTPAGE_H */

/*
 * $Log: CListPage.h,v $
 * Revision 1.2  1999/07/10 05:12:59  MIYABI
 * CVS導入
 *
 */
