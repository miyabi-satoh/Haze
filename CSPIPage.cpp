/*=============================================================================

	FILE: CSPIPage.cpp
 
=============================================================================*/

/*=============================================================================
	#include statements
=============================================================================*/

#include <stdinc.h>
#include <cmath>
#include <commctrl.h>
#include <shlobj.h>
#include <MyFunction.h>
#include <CMyFileDialog.h>
#include "General.h"
#include "gSPIChain.h"
#include "ini.h"
#include "res/resource.h"

#include "CSPIPage.h"

/*=============================================================================
  �O���Q��
=============================================================================*/

extern CSPIChain  g_tempSPI, g_SPI;
extern HWND       g_hMainDlg;
extern HANDLE     g_hSema;

/*=============================================================================
	#define statements
=============================================================================*/

#define WM_SPI_CONFIG 5000

/*=============================================================================
  class CSPIListView
=============================================================================*/

/*=============================================================================
  void InitWindow(HWND)
  ���X�g�r���[�̍쐬�A������
=============================================================================*/

void CSPIListView::
InitWindow(HWND hWnd)
{
	DWORD dwStyle;

	m_hWnd = ::CreateWindowEx(WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE,
														WC_LISTVIEW, (LPSTR)NULL,
														WS_TABSTOP | WS_VISIBLE | WS_CHILDWINDOW |
														LVS_REPORT | LVS_SHOWSELALWAYS,
														11, 40, 525, 270, hWnd, (HMENU)IDP_LIST,
														GMH(), NULL);

	dwStyle = ListView_GetExtendedListViewStyle(m_hWnd);
	dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES;

	ListView_SetExtendedListViewStyle(m_hWnd, dwStyle);

	SetColumn(4,
						10, "̧�ٖ�",
						20, "�Ή�����摜�`��",
						30, "�ݒ�",
						40, "About...");
	SetSubClass();
}

/*=============================================================================
  void AutoSizeColumns()
  �J�������̎�������
=============================================================================*/

void CSPIListView::
AutoSizeColumns()
{
	for(int col = 0; col < 4; col++){
		SetColumnWidth(col, LVSCW_AUTOSIZE);
		int wc1 = ListView_GetColumnWidth(m_hWnd, col);
		SetColumnWidth(col, LVSCW_AUTOSIZE_USEHEADER);
		int wc2 = ListView_GetColumnWidth(m_hWnd, col);
		int wc = max(10, max(wc1, wc2));
		SetColumnWidth(col, wc);
	}
}

/*-----------------------------------------------------------------------------
  BOOL OnLButtonDblClk()
-----------------------------------------------------------------------------*/

OVR_OnLButtonDblClk(CSPIListView)
{
	_Err0("CSPIListView::OnLButtonDblClk()\n");
	SendCommand(0, WM_SPI_CONFIG, 0);
	return 0;
}

/*=============================================================================
  class CSPIPage
=============================================================================*/

/*=============================================================================
  void Init(HWND, int, RECT*)
  ������
=============================================================================*/

void CSPIPage::
Init(HWND hWndTab, int nCmdShow, RECT* pRect)
{
	_Err0("CSPIPage::Init()\n");

	Create(MIR(IDD_PLUGIN), hWndTab);
	ShowWindow(nCmdShow);
	MoveWindow(pRect->left, pRect->top, pRect->right - pRect->left,
						 pRect->bottom - pRect->top, TRUE);
}

/*=============================================================================
  int on_save()
  �ݒ��ۑ�
=============================================================================*/

BOOL CSPIPage::
on_save()
{
	_Err0("CSPIPage::on_save()\n");

	std::string szSpiDir;

    /* �{�̂��g�p����v���O�C���`�F�C���ɃR�s�[ */

	::WaitForSingleObject(g_hSema, INFINITE);

	g_SPI = g_tempSPI;

    /* �ݒ�t�@�C���ɏ����o�� */
	g_SPI.WritePluginData();

	::ReleaseSemaphore(g_hSema, 1, NULL);

	return TRUE;
}

/*-----------------------------------------------------------------------------
  BOOL OnInitDialog()
-----------------------------------------------------------------------------*/

OVR_OnInitDialog(CSPIPage)
{
	_Err0("CSPIPage::OnInitDialog()\n");
  m_bInit = FALSE;

	std::string szTemp;

    /* ���X�g�r���[�̏����� */
	m_SPIListView.InitWindow(m_hWnd);

    /* �����t�H���_���̕\�� */
	szTemp = g_tempSPI.GetPath();
	SetDlgItemText(IDP_SEARCHDIR, szTemp.length() ? szTemp.c_str() : "�Ȃ�");

    /* Plug-in���A�g�p��ԁA�Ή��t�@�C���̍X�V */
	ListView_UpData();

  m_bInit = TRUE;
	return (*plr = 1);
}

/*-----------------------------------------------------------------------------
  BOOL OnCommand()
-----------------------------------------------------------------------------*/

OVR_OnCommand(CSPIPage)
{
	*plr = 0;

	switch(wID){

    case WM_SPI_CONFIG:
			OnConfig();
			return 1;
			break;

    case IDP_CHANGEDIR:
			if(OnChangeDir() == FALSE){
				return 1;
			}
			break;

    default:
			return 0;
	}

	NotifyChanged(g_hMainDlg);
	return 1;
}

/*-----------------------------------------------------------------------------
  BOOL OnClose()
-----------------------------------------------------------------------------*/

OVR_OnClose(CSPIPage)
{
	_Err0("CSPIPage::OnClose()\n");

	m_SPIListView.ResetSubClass();

	DestroyWindow();

	return 0;
}

/*-----------------------------------------------------------------------------
  BOOL OnNotify()
-----------------------------------------------------------------------------*/

OVR_OnNotify(CSPIPage)
{
	NMLISTVIEW* pnmv = (NMLISTVIEW*)pnmh;

	switch(pnmv->hdr.code){
    case LVN_ITEMCHANGED:
		{
	    CSPIChain::Node pn;
	    int i;

	    if((int)g_tempSPI.in().size() > pnmv->iItem){
				pn = g_tempSPI.in().begin();
				i = pnmv->iItem;
				while(i--){
					pn++;
				}
	    }
	    else{
				pn = g_tempSPI.am().begin();
				i = pnmv->iItem - g_tempSPI.in().size();
				while(i--){
					pn++;
				}
	    }

	    if(pn != NULL){
				if(pnmv->uNewState == 0x2000){
						/* �`�F�b�N */
					pn->SetUsing(1);
          if(m_bInit){
          	NotifyChanged(g_hMainDlg);
           } 
				}
				else if(pnmv->uNewState == 0x1000){
						/* �A���`�F�b�N */
					pn->SetUsing(0);
          if(m_bInit){
          	NotifyChanged(g_hMainDlg);
          }
				}
	    }
	    break;
		}
	}
	return 0;
}

/*-----------------------------------------------------------------------------
  int OnCofig()
  Plug-in �́u�ݒ�_�C�A���O�v���J��
-----------------------------------------------------------------------------*/

int CSPIPage::
OnConfig()
{
	_Err0("CSPIPage::OnConfig()\n");

	int nItem;

	if((nItem = m_SPIListView.GetNextItem()) == -1){
			/* �I�����ڂȂ� */
		return 0;
	}

	CSPIChain::Node pn;
	int i;

	if((int)g_tempSPI.in().size() > nItem){
		pn = g_tempSPI.in().begin();
		i = nItem;
	}
	else{
		pn = g_tempSPI.am().begin();
		i = nItem - g_tempSPI.in().size();
	}

	while(i--){
		pn++;
	}

	if(pn != NULL){
		pn->ConfigDlg(m_hWnd, 1);
	}

	return 1;
}

/*-----------------------------------------------------------------------------
  int OnChangeDir()
  �f�B���N�g���ύX
-----------------------------------------------------------------------------*/

int CSPIPage::
OnChangeDir()
{
	_Err0("CSPIPage::OnChangeDir()\n");

	char          buf[256];
	CMyFileDialog dlg;

	if(dlg.GetOpenFolderName(m_hWnd, NULL, buf, 256, NULL) == IDOK){
		if(buf[::lstrlen(buf) - 1] != '\\'){
	    ::lstrcat(buf, "\\");
		}
		SetDlgItemText(IDP_SEARCHDIR, buf);

		SPIChain_UpData();
		ListView_UpData();

		return 1;
	}
	else{
		return 0;
	}
}

/*-----------------------------------------------------------------------------
  int ListView_UpData()
  ���X�g�r���[�̍X�V
-----------------------------------------------------------------------------*/

int CSPIPage::
ListView_UpData()
{
	_Err0("CSPIPage::ListView_UpData()\n");

	int sel = 0;

	m_SPIListView.DeleteAll();
	sel = SetItem(g_tempSPI.in(), sel);
	SetItem(g_tempSPI.am(), sel);

    /* �J�������𒲐� */
	m_SPIListView.AutoSizeColumns();

	return 1;
}

/*=============================================================================
	int SetItem(CSPIChain::List&, int)
=============================================================================*/

int CSPIPage::
SetItem(CSPIChain::List& list, int sel)
{
	CSPIChain::Node pn;
	int iCheck;
	int i = sel;

	for(pn = list.begin(); pn != list.end(); pn++, i++){
		std::string szExt;
		char szInfo[256];

			/* �g�p��� */
		iCheck = (pn->Use() == FALSE) ? 0 : 1;

		::GetFileTitle(pn->GetName().c_str(), szInfo, 255);

		m_SPIListView.InsertItem(szInfo, i, -1);
		ListView_SetItemState(m_SPIListView.GetSafeHwnd(), i,
													UINT((iCheck + 1) << 12),	LVIS_STATEIMAGEMASK);

		pn->_GetPluginExtention(szExt);
		m_SPIListView.SetSubItem(szExt.length() ? (char*)szExt.c_str() : "�s��",
														 i);

		m_SPIListView.SetSubItem(pn->ConfigDlg(NULL, 2) ? "����" : "", i, 2);

		pn->GetPluginInfo(1, szInfo, 255);
		m_SPIListView.SetSubItem(szInfo, i, 3);
	}

	return i;
}

/*=============================================================================
  int SPIChain_UpData()
  Plug-in �`�F�C���̍X�V
=============================================================================*/

int CSPIPage::
SPIChain_UpData()
{
	_Err0("CSPIPage::SPIChain_UpData()\n");

	WIN32_FIND_DATA fd;
	char            plg[256];
	HANDLE          hFind;
	std::string       szDir;

    /* ���X�g�̃N���A */
	g_tempSPI.in().clear();
	g_tempSPI.am().clear();

    /* �����p�̃t���p�X���쐬 */
	GetDlgItemText(IDP_SEARCHDIR, szDir);

	g_tempSPI.SetPath(szDir.c_str());
	::wsprintf(plg, "%s*.spi", szDir.c_str());

	hFind = ::FindFirstFile(plg, &fd);
	if(hFind == INVALID_HANDLE_VALUE){
		return 0;
	}

	do{
		CSPIData  dat;

			/* �v���O�C���̃t���p�X���쐬 */
		::wsprintf(plg, "%s%s", szDir.c_str(), fd.cFileName);

		if(dat.Initialize(plg, TRUE)){
	    int type = dat._GetPluginType();
	    if(type == SPI_00IN){
				g_tempSPI.in().push_back(dat);
	    }
	    else if(type == SPI_00AM){
				g_tempSPI.am().push_back(dat);
	    }
		}
	}while(::FindNextFile(hFind, &fd) != 0);

	::FindClose(hFind);

	return 1;
}

/*
 * $Log: CSPIPage.cpp,v $
 * Revision 1.2  1999/07/10 05:12:59  MIYABI
 * CVS����
 *
 */
