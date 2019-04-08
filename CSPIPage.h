/*=============================================================================
	
	File:         CSPIPage.h

	Description:  Defintions for CSPIListView, CSPIPage.

=============================================================================*/

#ifndef _CSPIPAGE_H
#define _CSPIPAGE_H

/*=============================================================================
	#include statements
=============================================================================*/

#include <CMyControl.h>
#include <CMyDialog.h>
#include "General.h"
#include "gSPIChain.h"

/*=============================================================================
  class CSPIListView
  カスタムリストビュー
=============================================================================*/

class CSPIListView : public CMyListView
{
public:
	CSPIListView() : CMyListView() {}
	~CSPIListView() {}

	void  InitWindow(HWND hWnd);
	void  AutoSizeColumns();
protected:

	HND_OnLButtonDblClk();
};

/*=============================================================================
  class CSPIPage
  「Plug-inの設定」
=============================================================================*/

class CSPIPage : public CMyDialog
{
private:
	CSPIListView  m_SPIListView;
  bool m_bInit;

public:
	CSPIPage()  {}
	~CSPIPage() {}

	void Init(HWND hWndTab, int nCmdShow, RECT *pRect);
	int  on_save();
protected:

	HND_OnInitDialog();
	HND_OnCommand();
	HND_OnClose();
	HND_OnNotify();

	int OnConfig();
	int OnChangeDir();

	int ListView_UpData();
	int SetItem(CSPIChain::List& list, int sel);
	int SPIChain_UpData();
};

#endif /* _CSPIPAGE_H */

/*
 * $Log: CSPIPage.h,v $
 * Revision 1.2  1999/07/10 05:12:59  MIYABI
 * CVS導入
 *
 */
