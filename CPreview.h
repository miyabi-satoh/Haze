/*=============================================================================
	
	File:         CPreview.h

	Description:  Defintions for CPreview

=============================================================================*/

#ifndef _CPREVIEW_H
#define _CPREVIEW_H

/*=============================================================================
	#include statements
=============================================================================*/

#include <CMyWnd.h>
#include <CMyImage.h>
#include <CMyVW.h>
#include "General.h"

/*=============================================================================
  class CPreview
  プレビュー画像を表示するウィンドウのクラス
=============================================================================*/

class CPreview : public CMyWnd
{
private:
	CMyVW m_vw;

public:
	CPreview() : CMyWnd() {}
	~CPreview() {}

	int InitWindow(HWND hWnd);

	void DrawPreview(CMyImage& cp);
	void DeletePreview();
	void textout(const char *szText);

protected:
	HND_OnPaint();
	HND_OnClose();
	HND_OnSize();
};

#endif /* _CPREVIEW_H */

/*
 * $Log: CPreview.h,v $
 * Revision 1.2  1999/07/10 05:12:59  MIYABI
 * CVS導入
 *
 */
