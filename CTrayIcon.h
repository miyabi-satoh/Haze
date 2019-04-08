/*=============================================================================
	
	File:         CTrayIcon.h

	Description:  Defintions for CTrayIcon

=============================================================================*/

#ifndef _CTRAYICON_H
#define _CTRAYICON_H

/*=============================================================================
	#include statements
=============================================================================*/

#include <CMyTray.h>
#include "CMainDlg.h"
#include "General.h"

/*=============================================================================
  class CTrayIcon
  トレイアイコン用クラス
=============================================================================*/

class CTrayIcon : public CMyTray
{
private:
	BOOL  m_bMode;
	CMainDlg  m_MainDlg;
	HANDLE  m_hThread;
	BOOL    m_bThreadFlag;

	CMyFileData  m_data;

	HICON m_hIcon[6];
	BYTE  m_bIconIndex;

	LONG  m_lCount;
public:
	CTrayIcon();
	~CTrayIcon()  {}

	int InitApplication();

protected:
	HND_OnCreate();
	HND_OnTimer();
	HND_OnClose();
	HND_OnCommand();
	HND_OnTaskTray();

	void OnWallTimer();
	void OnChangePaper();
	void TrayAnimation();
	int OnSuspend();
	void OnTray2Click();

	static DWORD WINAPI SetWallPaper(LPVOID);
	static int CALLBACK StretchProgress(int n, int end, long lData);

	int GetSourcePicture(CMyImage& cpDst);
	int Set_DeskTop();
	int MBox(const char *);

};

#endif /* _CTRAYICON_H */

/*
 * $Log: CTrayIcon.h,v $
 * Revision 1.2  1999/07/10 05:13:00  MIYABI
 * CVS導入
 *
 */
