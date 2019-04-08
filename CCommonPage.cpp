/*=============================================================================

	FILE: CCommonPage.cpp
 
=============================================================================*/

#include <stdinc.h>
#include <MyFunction.h>
#include <CMyFileDialog.h>
#include <limits.h>
#include "General.h"
#include "ini.h"
#include "CCommonPage.h"
#include "res/resource.h"

/*=============================================================================
  �O���Q��
=============================================================================*/

extern HWND   g_hMainDlg;
extern HANDLE g_hSema;

/*=============================================================================
  class CSampleRect
=============================================================================*/

/*=============================================================================
  void Paint()
  �`��
=============================================================================*/

void CSampleRect::
Paint()
{
	HDC hdc = GetDC();

	if(hdc){
		RECT rc;

		GetClientRect(&rc);
		mf::FillRect1(hdc, &rc, m_TextCol);
		ReleaseDC(hdc);
	}
}

/*-----------------------------------------------------------------------------
  BOOL OnEraseBkgnd()
-----------------------------------------------------------------------------*/

OVR_OnEraseBkgnd(CSampleRect)
{
	_Err0("CSampleRect::OnEraseBkgnd()\n");

	RECT rc;
	GetClientRect(&rc);
	mf::FillRect1(hdc, &rc, m_TextCol);

	*plr = 0;
	return 1;
}

/*=============================================================================
  class CCommonPage
=============================================================================*/

/*=============================================================================
  void Init(HWND, int, RECT*)
  �������A�T�C�Y����
=============================================================================*/

void CCommonPage::
Init(HWND hWndTab, int nCmdShow, RECT* pRect)
{
	Create(MIR(IDD_COMMON), hWndTab);
	ShowWindow(nCmdShow);
	MoveWindow(pRect->left, pRect->top, pRect->right - pRect->left,
						 pRect->bottom - pRect->top, TRUE);
}

/*=============================================================================
  int on_save()
  �ݒ��ۑ�
=============================================================================*/

int CCommonPage::
on_save()
{
	CSetting Setting;
	CTimeData TimeData;
	CCommonData CommonData;

    /* �\���֌W */
	Setting.SetExtend(ComboBox_GetCurSel(ID_COMBO_EXT));
	Setting.SetTilePattern(ComboBox_GetCurSel(ID_COMBO_TILE));
	Setting.SetUnit(ComboBox_GetCurSel(ID_COMBO_UNIT));
	Setting.SetHeight(GetDlgItemInt(ID_EDIT_HEIGHT));
	Setting.SetWidth(GetDlgItemInt(ID_EDIT_WIDTH));

    /* ���Ԋ֌W */
	TimeData.SetMode(ComboBox_GetCurSel(ID_COMBO_TIME));
	if(TimeData.GetMode() == 0){
		SYSTEMTIME sysT;
		GetLocalTime(&sysT);
		TimeData.SetValue(sysT.wDayOfWeek);
	}
	else{
		TimeData.SetValue(GetDlgItemInt(ID_EDIT_TIMEVAL));
	}

    /* �_�u���N���b�N���̐ݒ� */
	CommonData.Set2Click(ComboBox_GetCurSel(ID_COMBO_2CLK));
	GetDlgItemText(ID_EDIT_EXEC, CommonData.Path());

    /* �f�X�N�g�b�v�̐ݒ� */
	CommonData.SetTrans(IsDlgButtonChecked(ID_BUTTON_TRANS));
	CommonData.SetTextColor(m_TextColor);

    /* �ݒ�t�@�C���ɏ����o�� */
	::WaitForSingleObject(g_hSema, INFINITE);
	Setting.Write();
	TimeData.Write();
	CommonData.Write();
	::ReleaseSemaphore(g_hSema, 1, NULL);

	return TRUE;
}

/*-----------------------------------------------------------------------------
  BOOL OnInitDialog()
-----------------------------------------------------------------------------*/

OVR_OnInitDialog(CCommonPage)
{
	_Err0("CCommonPage::OnInitDialog()\n");

	CSetting Setting;
	CTimeData TimeData;
	CCommonData CommonData;

	::WaitForSingleObject(g_hSema, INFINITE);
	Setting.Read();
	TimeData.Read();
	CommonData.Read();
	::ReleaseSemaphore(g_hSema, 1, NULL);

    /* �T�u�N���X�� */
	m_Rect.CMyControl::operator=(GetDlgItem(ID_STATIC_COLOR));
	m_Rect.SetSubClass();

    /* ����������̐ݒ� */
	InitComboBox();

    /* ���ڂ�I�� */
	ComboBox_SetCurSel(ID_COMBO_EXT, Setting.GetExtend());
	ComboBox_SetCurSel(ID_COMBO_TILE, Setting.GetTilePattern());
	ComboBox_SetCurSel(ID_COMBO_UNIT, Setting.GetUnit());
	ComboBox_SetCurSel(ID_COMBO_TIME, TimeData.GetMode());
	ComboBox_SetCurSel(ID_COMBO_2CLK, CommonData.Get2Click());

    /* �`�F�b�N��Ԃ�ݒ� */
	CheckDlgButton(ID_BUTTON_TRANS, CommonData.GetTrans());

    /* ���l��ݒ�(�����W) */
	Spin_SetRange(IDC_SPIN_WIDTH,   1, GSM(SM_CXSCREEN));
	Spin_SetRange(IDC_SPIN_HEIGHT,  1, GSM(SM_CYSCREEN));
	Spin_SetRange(IDC_SPIN_TIME,    1, SHRT_MAX);
    /* ���l��ݒ�(�|�W�V����) */
	Spin_SetPos(IDC_SPIN_WIDTH, Setting.GetWidth());
	Spin_SetPos(IDC_SPIN_HEIGHT, Setting.GetHeight());
    /* ���l��ݒ�(�G�f�B�b�g�{�b�N�X) */
	SetDlgItemInt(ID_EDIT_HEIGHT, Setting.GetHeight());
	SetDlgItemInt(ID_EDIT_WIDTH,  Setting.GetWidth());

	if(TimeData.GetMode() == 0){
		SetDlgItemInt(ID_EDIT_TIMEVAL, 60);
		Spin_SetPos(IDC_SPIN_TIME, 60);
	}
	else{
		SetDlgItemInt(ID_EDIT_TIMEVAL, TimeData.GetValue());
		Spin_SetPos(IDC_SPIN_TIME, TimeData.GetValue());
	}
	SetDlgItemText(ID_EDIT_EXEC, CommonData.GetAppPath());

	m_TextColor = CommonData.GetTextColor();
	m_Rect.SetColor(m_TextColor);

    /* �e�R���g���[���̎g�p�\�E�s�\��ݒ� */
	Ext_ControlChange(ComboBox_GetCurSel(ID_COMBO_EXT));
	Time_ControlChange(ComboBox_GetCurSel(ID_COMBO_TIME));
	DClk_ControlChange(ComboBox_GetCurSel(ID_COMBO_2CLK));

	return (*plr = 1);
}

/*-----------------------------------------------------------------------------
  BOOL OnCommnad()
-----------------------------------------------------------------------------*/

OVR_OnCommand(CCommonPage)
{
	*plr = 0;

	if(wNotifyCode != EN_CHANGE){
		switch(wID){
			case ID_BUTTON_TRANS:
				break;

			case ID_BUTTON_COLSEL:
				if(OnColorSelect() == FALSE){
					return 0;
				}
				break;

			case ID_BUTTON_BROWSE:
				if(OnBrowse() == FALSE){
					return 0;
				}
				break;

			case ID_COMBO_EXT:
				if(wNotifyCode != CBN_SELCHANGE){
					return 0;
				}
				Ext_ControlChange(ComboBox_GetCurSel(wID));
				break;

			case ID_COMBO_TIME:
				if(wNotifyCode != CBN_SELCHANGE){
					return 0;
				}
				Time_ControlChange(ComboBox_GetCurSel(wID));
				break;

			case ID_COMBO_2CLK:
				if(wNotifyCode != CBN_SELCHANGE){
					return 0;
				}
				DClk_ControlChange(ComboBox_GetCurSel(wID));
				break;


			default:
				return 0;
				break;
		}
	}

	NotifyChanged(g_hMainDlg);
	return 1;
}

/*-----------------------------------------------------------------------------
  BOOL OnPaint()
-----------------------------------------------------------------------------*/

OVR_OnPaint(CCommonPage)
{
	_Err0("CCommonPage::OnPaint()\n");

	m_Rect.Paint();
	return 0;
}

/*-----------------------------------------------------------------------------
  BOOL OnClose()
-----------------------------------------------------------------------------*/

OVR_OnClose(CCommonPage)
{
	_Err0("CCommonPage::OnClose()\n");

	m_Rect.ResetSubClass();

	return 0;
}

/*-----------------------------------------------------------------------------
  void InitComboBox()
  �R���{�{�b�N�X�ɏ����������ݒ�
-----------------------------------------------------------------------------*/

void CCommonPage::
InitComboBox()
{
	ComboBox_InitString(ID_COMBO_2CLK, STR_CLK);
	ComboBox_InitString(ID_COMBO_TIME, STR_TIME);
	ComboBox_InitString(ID_COMBO_TILE, STR_TILE);
	ComboBox_InitString(ID_COMBO_EXT, STR_EXTEND);
	ComboBox_InitString(ID_COMBO_UNIT, STR_UNIT);
}

/*-----------------------------------------------------------------------------
  int OnColorSelect()
  �F�̑I��
-----------------------------------------------------------------------------*/

int CCommonPage::
OnColorSelect()
{
	CHOOSECOLOR cc;
	DWORD       cust[16];

	::ZeroMemory(&cc, sizeof(cc));

	for(int i = 0; i < 1; i++){
		cust[i] = m_TextColor;
	}
	for(; i < 16; i++){
		cust[i] = (DWORD)RGB(255, 255, 255);
	}
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = cust;
	cc.Flags = CC_RGBINIT;
	cc.rgbResult = m_TextColor;

	if(::ChooseColor(&cc)){
		m_TextColor = cc.rgbResult;
		m_Rect.SetColor(cc.rgbResult);
		m_Rect.Paint();
		return 1;
	}

	return 0;
}

/*-----------------------------------------------------------------------------
  int OnBrowse()
  �_�u���N���b�N���̃t�@�C���I��
-----------------------------------------------------------------------------*/

int CCommonPage::
OnBrowse()
{
	CMyFileDialog dlg;
	char          szFile[1000];

	szFile[0] = '\0';
	int ret = dlg.GetOpenFileName(m_hWnd, STR_FILTER_CLK2,  "̧�ق̑I��",
																szFile, 1000);
	if(ret != 0){
		SetDlgItemText(ID_EDIT_EXEC, szFile);
		return TRUE;
	}
	return FALSE;
}

/*-----------------------------------------------------------------------------
  void Ext_ControlChange(int)
  �u�g��E�k���v�̃Z�����ύX���ꂽ�Ƃ��̊e�R���g���[���̐ݒ�
-----------------------------------------------------------------------------*/

void CCommonPage::
Ext_ControlChange(int nSel)
{
	if(nSel  == 3){
		EnableItem(ID_EDIT_HEIGHT);
		EnableItem(ID_EDIT_WIDTH);
		EnableItem(ID_COMBO_UNIT);
		EnableItem(IDC_SPIN_HEIGHT);
		EnableItem(IDC_SPIN_WIDTH);
	}
	else{
		DisableItem(ID_EDIT_HEIGHT);
		DisableItem(ID_EDIT_WIDTH);
		DisableItem(ID_COMBO_UNIT);
		DisableItem(IDC_SPIN_HEIGHT);
		DisableItem(IDC_SPIN_WIDTH);
	}
}

/*-----------------------------------------------------------------------------
  void Time_ControlChange(int)
  �u�\���Ԋu�v�̃Z�����ύX���ꂽ�Ƃ��̊e�R���g���[���̐ݒ�
-----------------------------------------------------------------------------*/

void CCommonPage::
Time_ControlChange(int nSel)
{
	if(nSel  == 2){
		EnableItem(ID_EDIT_TIMEVAL);
		EnableItem(IDC_SPIN_TIME);
	}
	else{
		DisableItem(ID_EDIT_TIMEVAL);
		DisableItem(IDC_SPIN_TIME);
	}
}

/*-----------------------------------------------------------------------------
  void DClk_ControlChange(int)
  �R���{�{�b�N�X�̃Z�����ύX���ꂽ�Ƃ��̊e�R���g���[���̐ݒ�
-----------------------------------------------------------------------------*/

void CCommonPage::
DClk_ControlChange(int nSel)
{
	if(nSel == 4){
		EnableItem(ID_EDIT_EXEC);
		EnableItem(ID_BUTTON_BROWSE);
	}
	else{
		DisableItem(ID_EDIT_EXEC);
		DisableItem(ID_BUTTON_BROWSE);
	}
}

/*
 * $Log: CCommonPage.cpp,v $
 * Revision 1.2  1999/07/10 05:12:58  MIYABI
 * CVS����
 *
 */
