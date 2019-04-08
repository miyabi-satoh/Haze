/*=============================================================================

	FILE: CPreview.cpp
 
=============================================================================*/

/*=============================================================================
	#include statements
=============================================================================*/

#include <stdinc.h>
#include "CPreview.h"

/*=============================================================================
  �O���Q��
=============================================================================*/

extern HFONT g_hFont;

/*=============================================================================
	#define statements
=============================================================================*/

#define PREVIEW_CLASS "Preview_Wnd"
#define PREVIEW_ID 6000

/*=============================================================================
  int InitWindow()
  �E�B���h�E������
=============================================================================*/

int CPreview::
InitWindow(HWND hWnd)
{
    /* �E�B���h�E�쐬 */
	RegisterClassEx(PREVIEW_CLASS, (HICON)NULL, ::LoadCursor(NULL, IDC_ARROW),
									(HBRUSH)(COLOR_WINDOW + 1), 0);

	if (!CreateEx(WS_EX_CLIENTEDGE, PREVIEW_CLASS, "",
								WS_VISIBLE | WS_OVERLAPPED | WS_CHILD, 0, 0, 0, 0, hWnd,
								(HMENU)PREVIEW_ID)){
		_Err0("CreateEx() ���s�H\n");
		return 0;
	}

	return 1;
}

/*=============================================================================
  void DrawPreview(CMyImage&)
  ���z�E�B���h�E�ɃC���[�W���Z�b�g
=============================================================================*/

void CPreview::
DrawPreview(CMyImage& cp)
{
	RECT  rc;
	int   w, h;

	GetClientRect(&rc);
	w = cp.GetWidth();
	h = cp.GetHeight();

	m_vw.clear();
	cp.Draw(m_vw.GetDC(),
					cdn::Point((rc.right - w) / 2, (rc.bottom - h) / 2),
					cdn::Rect(0, 0, w, h));

	InvalidateRect(NULL, FALSE);
	UpdateWindow();
}

/*=============================================================================
  void DeletePreview()
  �摜����(�w�i�u���V�œh��Ԃ�)
=============================================================================*/

void CPreview::
DeletePreview()
{
	m_vw.clear();

	InvalidateRect(NULL, FALSE);
	UpdateWindow();
}

/*=============================================================================
  void textout(const char *)
  ������`��
=============================================================================*/

void CPreview::
textout(const char *szText)
{
	RECT rc;
	SIZE  size;
	int   length = ::lstrlen(szText);

	GetClientRect(&rc);

	m_vw.clear();

	::GetTextExtentPoint32(m_vw.GetDC(), szText, length, &size);
	::TextOut(m_vw.GetDC(), (rc.right - size.cx) / 2,
						(rc.bottom - size.cy) / 2, szText, length);

	InvalidateRect(NULL, FALSE);
	UpdateWindow();
}

/*-----------------------------------------------------------------------------
  BOOL OnPaint()
-----------------------------------------------------------------------------*/

OVR_OnPaint(CPreview)
{
	PAINTSTRUCT ps;

	BeginPaint(&ps);

	m_vw.BitBlt(&ps, 0, 0);

	EndPaint(&ps);

	*plr = 0;
	return 1;
}

/*-----------------------------------------------------------------------------
  BOOL OnClose()
-----------------------------------------------------------------------------*/

OVR_OnClose(CPreview)
{
	_Err0("*** CPreview::OnClose() ***\n");
	m_vw.erase();
	DestroyWindow();

	*plr = 0;
	return 1;
}

/**************************************************************************

  BOOL OnSize()

**************************************************************************/
OVR_OnSize(CPreview)
{
	m_vw.init(m_hWnd, nWidth, nHeight);
	m_vw.SetBkMode(TRANSPARENT);
	m_vw.SelectObject(g_hFont);

	*plr = 0;
	return 1;
}

/*
 * $Log: CPreview.cpp,v $
 * Revision 1.2  1999/07/10 05:12:59  MIYABI
 * CVS����
 *
 */
