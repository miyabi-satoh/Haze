/*=============================================================================
	
	File:         CConfig.h

	Description:  Defintions for CCfgDlg

=============================================================================*/

#ifndef _CCONFIG_H
#define _CCONFIG_H

/*=============================================================================
	#include statements
=============================================================================*/

#include <CMyDialog.h>
#include "CPreview.h"
#include "CDocument.h"

/*=============================================================================
  class CCfgDlg
  個別設定ダイアログ
=============================================================================*/

class CCfgDlg : public CMyDialog
{
private:
  CMyFileData *m_pData;
  CPreview m_Prev;

public:
  CCfgDlg() : CMyDialog() {}
  ~CCfgDlg() {}

  int DoModal(HWND, CMyFileData*);

protected:
  HND_OnInitDialog();
  HND_OnCommand();

  void on_ok();
  void InitComboBox();
  void InitControls(const CSetting&);
  void OnChangeFlag();
  void OnChangeCombo();
};

#endif /* _CCONFIG_H */

/*
 * $Log: CConfig.h,v $
 * Revision 1.2  1999/07/10 05:12:58  MIYABI
 * CVS導入
 *
 */
