/*=============================================================================

	FILE: CMainDlg.cpp
 
=============================================================================*/

/*=============================================================================
	#include statements
=============================================================================*/

#include <stdinc.h>
#include <MyFunction.h>

#include "res/resource.h"
#include "General.h"
#include "gSPIChain.h"
#include "CMainDlg.h"

/*=============================================================================
	#define statements
=============================================================================*/

#define CONFIG_MSG_INIT "Plug-in��ǂݍ���ł��܂�..."

/*=============================================================================
  �O���Q��
=============================================================================*/

extern CSPIChain g_SPI, g_tempSPI;
extern HANDLE g_hSema;

HWND g_hMainDlg = NULL;

/*=============================================================================
  HWND Create(HWND)
  ���[�h���X�_�C�A���O�̍쐬
=============================================================================*/

HWND CMainDlg::
Create(HWND hWnd)
{
	_Err0("CMainDlg::Create()\n");

	m_hWC = hWnd;

    /* �E�B���h�E���� */
	if(m_hWnd){
		RestoreWindow();
	}
	else{
		CMyDialog WaitDlg;

    /* �E�F�C�g���b�Z�[�W��\�� */
		WaitDlg.Create(MIR(IDD_WAIT), m_hWnd);
		WaitDlg.SetWindowText(HAZE_NAME);

		CMyDialog::Create(MIR(IDD_MAIN), NULL);

		WaitDlg.DestroyWindow();

		ShowWindow(SW_RESTORE);

	}

	return m_hWnd;
}

/*-----------------------------------------------------------------------------
  BOOL wm_proc()
-----------------------------------------------------------------------------*/

OVR_WM_PROC(CMainDlg)
{
	switch(msg){
    case WM_CHANGE_CONFIGURE:
			EnableItem(IDM_SAVE);
			break;

    default:
			return CMyDialog::wm_proc(msg, wp, lp, plr);
			break;
	}

	*plr = 0;
	return 1;
}

/*-----------------------------------------------------------------------------
  BOOL OnInitDialog()
-----------------------------------------------------------------------------*/

OVR_OnInitDialog(CMainDlg)
{
	_Err0("CMainDlg::OnInitDialog()\n");

	g_hMainDlg = m_hWnd;

    /* �v���O�C���`�F�C���̏����� */
	::WaitForSingleObject(g_hSema, INFINITE);
	g_SPI.InitializedSPIChain();
	g_tempSPI = g_SPI;
	::ReleaseSemaphore(g_hSema, 1, NULL);
	g_tempSPI.LoadAll();

    /* �^�u�̏����� */
	RECT rcTabClient;
	InitTabControl(&rcTabClient);

    /* �e�_�C�A���O�y�[�W�̏����� */
	m_SPIPage.Init(GetDlgItem(IDM_TAB), SW_HIDE, &rcTabClient);
	m_CommonPage.Init(GetDlgItem(IDM_TAB), SW_HIDE, &rcTabClient);
	m_ListPage.Init(GetDlgItem(IDM_TAB), SW_HIDE, &rcTabClient);

	m_nActivePage = 0;

	SetWindowText(DLG_CAPTION);
	SendMessage(WM_SETICON, (WPARAM)ICON_SMALL,
							(LPARAM)mf::LoadIcon(MIR(IDI_11CONFIG)));
	DisableItem(IDM_SAVE);

	m_ListPage.ShowWindow(SW_SHOW);
	ShowWindow(SW_MINIMIZE);

	return (*plr = 1);
}

/*-----------------------------------------------------------------------------
  BOOL OnCommand()
-----------------------------------------------------------------------------*/

OVR_OnCommand(CMainDlg)
{
	switch(wID){
    case IDM_SAVE:
			OnSave();
			break;

    case IDOK:
			_Err0("CMainDlg::OnCommand --- IDOK\n");
			if(OnSave()){
				PostMessage(WM_CLOSE);
			}
			break;

    case IDCANCEL:
			PostMessage(WM_CLOSE);
			break;

				/* 
					 �A�N�Z�����[�^�L�[�X�g���[�N
					 �Y������E�B���h�E�փ��b�Z�[�W�𑗐M
				*/
    case IDA_DELETE:
    case IDA_CTRL_N:
    case IDA_CTRL_O:
    case IDA_CTRL_S:
    case IDA_CTRL_P:
    case IDA_CTRL_A:
    case IDA_SHIFT_L:
    case IDA_SHIFT_F:
    case IDA_SHIFT_A:
    case IDA_CTRL_UP:
    case IDA_CTRL_DOWN:
    case IDA_CTRL_HOME:
    case IDA_CTRL_END:
			_Err0("CMainDlg::Accelerator Command\n");

			if(m_nActivePage == 0){
				m_ListPage.SendMessage(WM_COMMAND, MAKELONG(wID, 0), 0L);
			}
			break;
	
    default:
			return 0;
	}
    
	*plr = 0;
	return 1;
}

/*-----------------------------------------------------------------------------
  BOOL OnClose()
-----------------------------------------------------------------------------*/

OVR_OnClose(CMainDlg)
{
	if(OnCancel() != 0){
		_Err0("CMainDlg::OnClose()\n");
		ShowWindow(SW_HIDE);

		m_ListPage.SendMessage(WM_CLOSE);
		m_SPIPage.SendMessage(WM_CLOSE);
		m_CommonPage.SendMessage(WM_CLOSE);

		DestroyWindow();
		g_hMainDlg = NULL;
		g_tempSPI.FreeAll();
		g_tempSPI.in().clear();
		g_tempSPI.am().clear();
	}

	*plr = 0;
	return 1;
}

/*-----------------------------------------------------------------------------
  BOOL OnNotify()
-----------------------------------------------------------------------------*/

OVR_OnNotify(CMainDlg)
{
	switch(pnmh->code){
    case TCN_SELCHANGE:
			OnSelChange();
			break;

    default:
			return 0;
	}

	*plr = 0;
	return 1;
}

/*-----------------------------------------------------------------------------
  void OnSelChange()
  �^�u�؂�ւ�
-----------------------------------------------------------------------------*/

void CMainDlg::
OnSelChange()
{
	m_ListPage.ShowWindow(SW_HIDE);
	m_SPIPage.ShowWindow(SW_HIDE);
	m_CommonPage.ShowWindow(SW_HIDE);

	switch(m_nActivePage = Tab_GetCurSel(IDM_TAB)){
    case 0:
			m_ListPage.ShowWindow(SW_SHOW);
			break;

    case 1:
			m_SPIPage.ShowWindow(SW_SHOW);
			break;

    case 2:
			m_CommonPage.ShowWindow(SW_SHOW);
			break;
	}
}

/*-----------------------------------------------------------------------------
  void InitTabControl(RECT*)
  �^�u�R���g���[���̏�����
-----------------------------------------------------------------------------*/

void CMainDlg::
InitTabControl(RECT* pRect)
{
	_Err0("CMainDlg::InitTabControl()\n");

	TC_ITEM tci;
	int     nStringID[3] = { IDS_TAB1, IDS_TAB2, IDS_TAB3 };
	char    szBuf[100];

	::ZeroMemory(&tci, sizeof(TC_ITEM));

	for(int i = 0; i < 3; i++){
		::LoadString(GMH(), nStringID[i], szBuf, 100);
		tci.mask = TCIF_TEXT;
		tci.pszText = szBuf;
		Tab_InsertItem(IDM_TAB, i, &tci);
	}

	::GetClientRect(GetDlgItem(IDM_TAB), pRect);
	Tab_AdjustRect(IDM_TAB, FALSE, pRect);
}

/*-----------------------------------------------------------------------------
  int OnSave()
  �ݒ��ۑ�
-----------------------------------------------------------------------------*/

int CMainDlg::
OnSave()
{
	_Err0("CMainDlg::on_save()\n");

	if(m_ListPage.OnSave() == 0){
		return 0;
	}

	m_SPIPage.on_save();
	m_CommonPage.on_save();

	if(m_hWC){
		::SendMessage(m_hWC, WM_CHANGE_CONFIGURE, 0, 0);
	}

	DisableItem(IDM_SAVE);

	return 1;
}

/*-----------------------------------------------------------------------------
  int OnCancel()
  �_�C�A���O�̃N���[�X
-----------------------------------------------------------------------------*/

int CMainDlg::
OnCancel()
{
	_Err0("CMainDlg::OnCancel()\n");

	if(m_ListPage.OnCancel() == 0){
		return 0;
	}
	else{
		return 1;
	}
}

/*
 * $Log: CMainDlg.cpp,v $
 * Revision 1.2  1999/07/10 05:12:59  MIYABI
 * CVS����
 *
 */
