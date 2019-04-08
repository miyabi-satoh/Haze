/*=============================================================================
	
	File:         CMainDlg.h

	Description:  Defintions for CMainDlg

=============================================================================*/

#ifndef _CMAINDLG_H
#define _CMAINDLG_H

/*=============================================================================
	#include statements
=============================================================================*/

#include <CMyDialog.h>
#include <CMyControl.h>
#include "General.h"
#include "CSPIPage.h"
#include "CListPage.h"
#include "CCommonPage.h"

/*=============================================================================
  class CMainDlg
  �ݒ�_�C�A���O�̃x�[�X�ɂȂ�_�C�A���O�̃N���X
=============================================================================*/

class CMainDlg : public CMyDialog
{
private:
	CListPage m_ListPage;
	CSPIPage m_SPIPage;
	CCommonPage m_CommonPage;

	HWND m_hWC;
	int m_nActivePage;

public:
	CMainDlg() : CMyDialog() {}
	~CMainDlg() {}

	HWND Create(HWND hWnd);

protected:
	HND_WM_PROC();

	HND_OnInitDialog();
	HND_OnCommand();
	HND_OnNotify();
	HND_OnClose();

	void OnSelChange();
	void InitTabControl(RECT* pRect);
	int OnSave();
	int OnOk();
	int OnCancel();
};

#endif /* _CMAINDLG_H */

/*
 * $Log: CMainDlg.h,v $
 * Revision 1.2  1999/07/10 05:12:59  MIYABI
 * CVS����
 *
 */
