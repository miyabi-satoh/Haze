/*=============================================================================
	
	File:         CSumNail.h

	Description:  Defintions for CImgData, CWindowView, CSumNail.

=============================================================================*/

#ifndef _CSUMNAIL_H
#define _CSUMNAIL_H

/*=============================================================================
	#include statements
=============================================================================*/

#include <CMyWnd.h>
#include <CMyControl.h>
#include <CMyVW.h>

#include <list>
#include <vector>

/*=============================================================================
	#define statements
=============================================================================*/

#define TP_SEL   0x01 /* 0000 0001 */
#define TP_MARK  0x02 /* 0000 0010 */
#define TP_SHIFT 0x04 /* 0000 0100 */

#define TP_LIST     0
#define TP_ARCHIVE  1
#define TP_FOLDER   2


/*=============================================================================
  class CImgData
  イメージに関するデータ
=============================================================================*/

class CImgData
{
public:
	std::string m_szArc, m_szFile;

private:
	CMyImage m_cpImage;
	BYTE m_bySelFlag, m_byType;
	cdn::Rect m_rcImage, m_rcText;

	static cdn::Rect m_rcMaxText;
public:
	CImgData();
	~CImgData();

	void SetImage(CMyImage& cp);
	void SetFileName(const char *szFile, const char *szArc);
	void SetType(BYTE byType) { m_byType = byType; }
	void GetImageRect(RECT* pRect);
	void SetImageRect(int x, int y, int w, int h);
	void GetTextRect(RECT* pRect);
	void SetTextRect(HDC hdc, int x, int y, int w, int h);
	BYTE GetType() { return m_byType; }

	std::string GetFileName();

	BYTE& Flag() { return m_bySelFlag; }

	static void SetMaxTextRect(int w, int h)
	{ m_rcMaxText.SetRect(0, 0, w, h); }

	void Blt(HDC hdc, RECT* pRect);
};

/*=============================================================================
  class COrgWindow
  実寸表示ウィンドウクラス
=============================================================================*/

class COrgWindow : public CMyWnd
{
private:
	CMyVW m_vw;
	int m_iVScrollPos, m_iHScrollPos;

public:
	COrgWindow() : CMyWnd() {}
	~COrgWindow() {}

	int InitWindow(const char *szTitle, CMyImage& cp);

protected:
	HND_OnClose();
	HND_OnHScroll();
	HND_OnPaint();
	HND_OnSize();
	HND_OnVScroll();
};

/*=============================================================================
  class CWindowView
  ビューウィンドウクラス
=============================================================================*/

class CWindowView : public CMyWnd
{
private:
	CMyVW m_vw;
	std::vector<CImgData> m_ImgList;
	std::list<COrgWindow> m_OrgList;

	int m_MaxHeight, m_MaxWidth, m_MaxStringHeight;
	int m_OldValue;
	int m_iScrollPos;
	int m_iPPos;

	RECT m_rcText;
	BOOL m_bAcceptMsg;

public:
	CWindowView();
	~CWindowView() {}

	int InitWindow(HWND hWndMain);
	void CalcMaxSize(PATHDATA_T& data, BYTE byType);
	int CreateView(int flag);
	void AutoScroll();
	void AddData(CMyImage& cp, const char *szFile, const char *szArc,
							 BYTE byType);

	void Begin()  { m_bAcceptMsg = FALSE; }
	void End() { m_bAcceptMsg = TRUE; }
	int size() { return m_ImgList.size(); }

protected:
	typedef std::vector<CImgData>::iterator itImg;

	void OnSelectAll();
	void OnTurnSelect();
	void OnSetStatusText();
	void OnOriginSize();
	void OnDeleteItem();
	void OnAddItem();

	HND_OnCommand();
	HND_OnContextMenu();
	HND_OnPaint();
	HND_OnLButtonDown();
	HND_OnLButtonDblClk();
	HND_OnSize();
	HND_OnVScroll();

	void ClearFlag4All(BYTE flag = TP_SEL);
	void SetFlag4All(BYTE flag = TP_SEL);

	itImg GetIndex(int X, int Y, int* pRet = NULL);
	itImg GetMarkNode(int* pRet = NULL);
	itImg GetShiftMarkNode(int* pRet = NULL);
	int ExistSelected();
	int GetNumSelected();

	void CreateOrgWindow(itImg& pn);

	static int PASCAL spiOrg(int iNum, int iDenom, long lData);
};

/*=============================================================================
  class CCatalogWindow
  画像一覧ウィンドウクラス(容れモノ)
=============================================================================*/

class CCatalogWindow : public CMyWnd
{
private:
	CMyControl m_Status;
	CMyProgressBar m_PBar;
	CWindowView m_View;
	std::list<PATHDATA_T> m_tempList;

	int m_ipgStart;
	BOOL m_bStop;

public:
	CCatalogWindow();
	~CCatalogWindow() {}

	void AddData(PATHDATA_T& data);
	HWND InitWindow();
	void CreateSumNail(const char *szTitle, BYTE byType);

protected:
	HND_OnCreate();
	HND_OnClose();
	HND_OnCommand();
	HND_OnSize();

	static int PASCAL spiCallBack(int iNum, int iDenom, long lData);

	void SendCommand(int id);
};

#endif /* _CSUMNAIL_H */

/*
 * $Log: CSumNail.h,v $
 * Revision 1.2  1999/07/10 05:13:00  MIYABI
 * CVS導入
 *
 */
