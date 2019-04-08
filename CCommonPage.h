/*=============================================================================
	
	File:         CCommonPage.h

	Description:  Defintions for CSampleRect, CComonPage

=============================================================================*/

#ifndef _CCOMMONPAGE_H
#define _CCOMMONPAGE_H

/*=============================================================================
	#include statements
=============================================================================*/

#include <CMyDialog.h>
#include <CMyControl.h>

/*=============================================================================
  class CSampleRect
  サンプルカラーを表示するスタティックコントロール
=============================================================================*/

class CSampleRect : public CMyControl
{
private:
	COLORREF  m_TextCol;

public:
	CSampleRect() : CMyControl() {}
	~CSampleRect() {}

	void SetColor(COLORREF col)  { m_TextCol = col;  }
	void Paint();

protected:
	HND_OnEraseBkgnd();
};

/*=============================================================================
  class CComonPage
  「共通設定」
=============================================================================*/

class CCommonPage : public CMyDialog
{
private:
	CSampleRect m_Rect;

	COLORREF  m_TextColor;

public:
	CCommonPage() : CMyDialog() {}
	~CCommonPage() {}

	void Init(HWND hWndTab, int nCmdShow, RECT* pRect);
	int on_save();

protected:
	HND_OnInitDialog();
	HND_OnPaint();
	HND_OnCommand();
	HND_OnClose();

	void InitComboBox();
	int OnColorSelect();
	int OnBrowse();

	void Ext_ControlChange(int nSel);
	void Time_ControlChange(int nSel);
	void DClk_ControlChange(int nSel);
};

#endif /* _CCOMMONPAGE_H */

/*
 * $Log: CCommonPage.h,v $
 * Revision 1.2  1999/07/10 05:12:58  MIYABI
 * CVS導入
 *
 */
