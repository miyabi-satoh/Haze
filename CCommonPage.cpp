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
  外部参照
=============================================================================*/

extern HWND   g_hMainDlg;
extern HANDLE g_hSema;

/*=============================================================================
  class CSampleRect
=============================================================================*/

/*=============================================================================
  void Paint()
  描画
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
  初期化、サイズ調整
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
  設定を保存
=============================================================================*/

int CCommonPage::
on_save()
{
	CSetting Setting;
	CTimeData TimeData;
	CCommonData CommonData;

    /* 表示関係 */
	Setting.SetExtend(ComboBox_GetCurSel(ID_COMBO_EXT));
	Setting.SetTilePattern(ComboBox_GetCurSel(ID_COMBO_TILE));
	Setting.SetUnit(ComboBox_GetCurSel(ID_COMBO_UNIT));
	Setting.SetHeight(GetDlgItemInt(ID_EDIT_HEIGHT));
	Setting.SetWidth(GetDlgItemInt(ID_EDIT_WIDTH));

    /* 時間関係 */
	TimeData.SetMode(ComboBox_GetCurSel(ID_COMBO_TIME));
	if(TimeData.GetMode() == 0){
		SYSTEMTIME sysT;
		GetLocalTime(&sysT);
		TimeData.SetValue(sysT.wDayOfWeek);
	}
	else{
		TimeData.SetValue(GetDlgItemInt(ID_EDIT_TIMEVAL));
	}

    /* ダブルクリック時の設定 */
	CommonData.Set2Click(ComboBox_GetCurSel(ID_COMBO_2CLK));
	GetDlgItemText(ID_EDIT_EXEC, CommonData.Path());

    /* デスクトップの設定 */
	CommonData.SetTrans(IsDlgButtonChecked(ID_BUTTON_TRANS));
	CommonData.SetTextColor(m_TextColor);

    /* 設定ファイルに書き出し */
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

    /* サブクラス化 */
	m_Rect.CMyControl::operator=(GetDlgItem(ID_STATIC_COLOR));
	m_Rect.SetSubClass();

    /* 初期文字列の設定 */
	InitComboBox();

    /* 項目を選択 */
	ComboBox_SetCurSel(ID_COMBO_EXT, Setting.GetExtend());
	ComboBox_SetCurSel(ID_COMBO_TILE, Setting.GetTilePattern());
	ComboBox_SetCurSel(ID_COMBO_UNIT, Setting.GetUnit());
	ComboBox_SetCurSel(ID_COMBO_TIME, TimeData.GetMode());
	ComboBox_SetCurSel(ID_COMBO_2CLK, CommonData.Get2Click());

    /* チェック状態を設定 */
	CheckDlgButton(ID_BUTTON_TRANS, CommonData.GetTrans());

    /* 数値を設定(レンジ) */
	Spin_SetRange(IDC_SPIN_WIDTH,   1, GSM(SM_CXSCREEN));
	Spin_SetRange(IDC_SPIN_HEIGHT,  1, GSM(SM_CYSCREEN));
	Spin_SetRange(IDC_SPIN_TIME,    1, SHRT_MAX);
    /* 数値を設定(ポジション) */
	Spin_SetPos(IDC_SPIN_WIDTH, Setting.GetWidth());
	Spin_SetPos(IDC_SPIN_HEIGHT, Setting.GetHeight());
    /* 数値を設定(エディットボックス) */
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

    /* 各コントロールの使用可能・不可能を設定 */
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
  コンボボックスに初期文字列を設定
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
  色の選択
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
  ダブルクリック時のファイル選択
-----------------------------------------------------------------------------*/

int CCommonPage::
OnBrowse()
{
	CMyFileDialog dlg;
	char          szFile[1000];

	szFile[0] = '\0';
	int ret = dlg.GetOpenFileName(m_hWnd, STR_FILTER_CLK2,  "ﾌｧｲﾙの選択",
																szFile, 1000);
	if(ret != 0){
		SetDlgItemText(ID_EDIT_EXEC, szFile);
		return TRUE;
	}
	return FALSE;
}

/*-----------------------------------------------------------------------------
  void Ext_ControlChange(int)
  「拡大・縮小」のセルが変更されたときの各コントロールの設定
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
  「表示間隔」のセルが変更されたときの各コントロールの設定
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
  コンボボックスのセルが変更されたときの各コントロールの設定
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
 * CVS導入
 *
 */
