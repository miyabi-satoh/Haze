/*=============================================================================

	FILE: main.cpp
 
=============================================================================*/

/*=============================================================================
	#include statements
=============================================================================*/

#include <stdinc.h>
#include <commctrl.h>
#include "General.h"
#include "gSPIChain.h"
#include "CTrayIcon.h"
#include "res/resource.h"

/*=============================================================================
	#define statements
=============================================================================*/

#define SPI_SEMAPHORE "SPI_SEMAPHORE"

/*=============================================================================
  Link Library
=============================================================================*/

#pragma comment(lib, "comctl32")
#ifdef _DEBUG
#pragma comment(lib, "mylib_d")
#else
#pragma comment(lib, "mylib_r")
#endif /* _DEBUG */

/*=============================================================================
  global value
=============================================================================*/

CSPIChain g_SPI, g_tempSPI;
HFONT g_hFont = NULL;
HACCEL g_hAccel = NULL;
HANDLE g_hSema = NULL;

/*-----------------------------------------------------------------------------
  int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
  エントリポイント
-----------------------------------------------------------------------------*/

int WINAPI
WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	CTrayIcon app;
	MSG       msg;
	INITCOMMONCONTROLSEX  ic;

	if(app.InitApplication()){
		return 0;
	}

	ic.dwSize = sizeof(INITCOMMONCONTROLSEX);
	ic.dwICC = ICC_LISTVIEW_CLASSES;
	::InitCommonControlsEx(&ic);

	g_hAccel = ::LoadAccelerators(GMH(), MIR(IDR_ACCEL1));

    /* Plugin用のセマフォ作成 */
	g_hSema = ::CreateSemaphore(NULL, 1, 1, SPI_SEMAPHORE);

    /* メッセージループ */
	while(::GetMessage(&msg, NULL, 0, 0)){
		TransactMessage(&msg);
	}

	return msg.wParam;
}

/*
 * $Log: main.cpp,v $
 * Revision 1.2  1999/07/10 05:13:01  MIYABI
 * CVS導入
 *
 */
