/*=============================================================================

	FILE: CConfig.cpp
 
=============================================================================*/

/*=============================================================================
	#include statements
=============================================================================*/

#include <stdinc.h>
#include <string>
#include "General.h"
#include "gSpiChain.h"
#include "ini.h"
#include "CConfig.h"
#include "res/resource.h"

/*=============================================================================
	#define statements
=============================================================================*/

#define KOBETSU_TITLE1  "個別設定：<%s>:%s"
#define KOBETSU_TITLE2  "個別設定：%s"

/*=============================================================================
  外部参照
=============================================================================*/

extern CSPIChain g_tempSPI;
extern HANDLE g_hSema;

/*=============================================================================
  int DoModal(HWND, CMyFileData*)
  モーダルダイアログの作成
=============================================================================*/

int CCfgDlg::
DoModal(HWND hw, CMyFileData* pd)
{
	m_pData = pd;
	return CMyDialog::DoModal(MIR(IDD_KOBETSU), hw);
}

/*-----------------------------------------------------------------------------
  BOOL OnInitDialog()
-----------------------------------------------------------------------------*/

OVR_OnInitDialog(CCfgDlg)
{
	char temp[_MAX_PATH];
	CSetting Setting;
	PICTDATA_T *pPict;

	m_Prev.InitWindow(m_hWnd);
	m_Prev.MoveWindow(12, 140, 178, 147, TRUE);

	if(m_pData->GetFlag() == 0){
		::WaitForSingleObject(g_hSema, INFINITE);
		Setting.Read();
		::ReleaseSemaphore(g_hSema, 1, NULL);
	}
	else{
		Setting = m_pData->GetSetting();
	}

	pPict = m_pData->GetPictureData();

    /* タイトルの設定 */
	if(m_pData->GetArcName().length()){
		::wsprintf(temp, KOBETSU_TITLE1, m_pData->GetArcName().c_str(),
							 m_pData->GetFileName().c_str());
	}
	else{
		::wsprintf(temp, KOBETSU_TITLE2, m_pData->GetFileName().c_str());
		DisableItem(IDC_COMBO1);
	}
	SetWindowText(temp);

    /* コントロールの初期化 */
	InitComboBox();
	InitControls(Setting);

    /* 項目を選択 */
	CheckDlgButton(ID_BUTTON_FLAG,  int(m_pData->GetFlag()));
	CSPIChain::Node pn;

	if(m_pData->Get00IN().length() != 0){
		int i;

		for(i = 0, pn = g_tempSPI.in().begin();	pn != g_tempSPI.in().end();
				pn++, i++){
	    if(m_pData->Get00IN() == pn->GetName()){
				ComboBox_SetCurSel(IDC_COMBO2, i + 1);
				break;
	    }
		}
	}

	if(m_pData->Get00AM().length() != 0){
		int i;

		for(i = 0, pn = g_tempSPI.am().begin();	pn != g_tempSPI.am().end();
				pn++, i++){
	    if(m_pData->Get00AM() == pn->GetName()){
				ComboBox_SetCurSel(IDC_COMBO1, i + 1);
				break;
	    }
		}
	}

    /* 画像情報 */
	::wsprintf(temp, "%d", pPict->iWidth);
	SetDlgItemText(ID_LABEL_WIDTH,  temp);
	::wsprintf(temp, "%d", pPict->iHeight);
	SetDlgItemText(ID_LABEL_HEIGHT, temp);
	::wsprintf(temp, "%d bit", pPict->iColorMode);
	SetDlgItemText(ID_LABEL_COLOR,  temp);

	OnChangeFlag();
	m_Prev.DrawPreview(m_pData->GetPicture());

	return (*plr = 1);
}

/*-----------------------------------------------------------------------------
  BOOL OnCommand()
-----------------------------------------------------------------------------*/

OVR_OnCommand(CCfgDlg)
{
	switch(wID){
    case IDOK:
			on_ok();
			break;

    case IDCANCEL:
			m_Prev.SendMessage(WM_CLOSE);
			EndDialog(0);
			break;

    case ID_BUTTON_FLAG:
			OnChangeFlag();
			break;

    case ID_COMBO_EXT:
			if(wNotifyCode == CBN_SELCHANGE){
				OnChangeCombo();
				break;
			}
			return FALSE;

    default:
			return FALSE;
	}

	*plr = 0;

	return TRUE;
}

/*-----------------------------------------------------------------------------
  void on_ok()
  OKボタンが押されたときの処理
-----------------------------------------------------------------------------*/

void CCfgDlg::
on_ok()
{
	m_pData->SetFlag(BYTE(IsDlgButtonChecked(ID_BUTTON_FLAG)));

	if(m_pData->GetFlag() == 1){
		CSetting& Set = m_pData->GetSetting();
	
		Set.SetExtend(ComboBox_GetCurSel(ID_COMBO_EXT));
		Set.SetTilePattern(ComboBox_GetCurSel(ID_COMBO_TILE));
		Set.SetUnit(ComboBox_GetCurSel(ID_COMBO_UNIT));

		/* 数値を設定*/
		Set.SetHeight(GetDlgItemInt(ID_EDIT_HEIGHT));
		Set.SetWidth(GetDlgItemInt(ID_EDIT_WIDTH));
	}

	/* プラグイン */
	int nSel, i;
	CSPIChain::Node pn;
	if((nSel = ComboBox_GetCurSel(IDC_COMBO1)) != 0){
		pn = g_tempSPI.am().begin();
		i = nSel - 1;
		while(i--){
	    pn++;
		}
		m_pData->Set00AM(pn->GetName().c_str());
	}
	else{
		m_pData->Set00AM("");
	}

	if((nSel = ComboBox_GetCurSel(IDC_COMBO2)) != 0){
		pn = g_tempSPI.in().begin();
		i = nSel - 1;
		while(i--){
	    pn++;
		}
	
		m_pData->Set00IN(pn->GetName().c_str());
	}
	else{
		m_pData->Set00IN("");
	}

	m_Prev.SendMessage(WM_CLOSE);
	EndDialog(1);
}

/*-----------------------------------------------------------------------------
  void InitComboBox()
  コンボボックスに初期文字列を設定
-----------------------------------------------------------------------------*/

void CCfgDlg::
InitComboBox()
{
	ComboBox_InitString(ID_COMBO_TILE, STR_TILE);
	ComboBox_InitString(ID_COMBO_EXT, STR_EXTEND);
	ComboBox_InitString(ID_COMBO_UNIT, STR_UNIT);

	CSPIChain::Node pn;

	ComboBox_InsertString(IDC_COMBO1, -1, "自動");
	ComboBox_InsertString(IDC_COMBO2, -1, "自動");

	for(pn = g_tempSPI.am().begin(); pn != g_tempSPI.am().end(); pn++){
		char szPath[_MAX_PATH];

		::GetFileTitle(pn->GetName().c_str(), szPath, _MAX_PATH);
		ComboBox_InsertString(IDC_COMBO1, -1, szPath);
	}
	for(pn = g_tempSPI.in().begin(); pn != g_tempSPI.in().end(); pn++){
		char szPath[_MAX_PATH];

		::GetFileTitle(pn->GetName().c_str(), szPath, _MAX_PATH);
		ComboBox_InsertString(IDC_COMBO2, -1, szPath);
	}
	ComboBox_SetCurSel(IDC_COMBO1, 0);
	ComboBox_SetCurSel(IDC_COMBO2, 0);
}

/*-----------------------------------------------------------------------------
  void InitControls(const CSetting&)
  各コントロールの初期値を設定
-----------------------------------------------------------------------------*/

void CCfgDlg::
InitControls(const CSetting& data)
{
	Spin_SetRange(IDC_SPIN_HEGHT, 0, GSM(SM_CYSCREEN));
	Spin_SetRange(IDC_SPIN_WIDTH, 0, GSM(SM_CXSCREEN));

	ComboBox_SetCurSel(ID_COMBO_EXT,  data.GetExtend());
	ComboBox_SetCurSel(ID_COMBO_TILE, data.GetTilePattern());
	ComboBox_SetCurSel(ID_COMBO_UNIT, data.GetUnit());

	/* 数値を設定 */
	SetDlgItemInt(ID_EDIT_HEIGHT, data.GetHeight());
	SetDlgItemInt(ID_EDIT_WIDTH,  data.GetWidth());
}

/*-----------------------------------------------------------------------------
  void OnChangeFlag()
  チェックによる有効／無効の切り替え
-----------------------------------------------------------------------------*/

void CCfgDlg::
OnChangeFlag()
{
	if(IsDlgButtonChecked(ID_BUTTON_FLAG)){
		EnableItem(ID_COMBO_EXT);
		EnableItem(ID_EDIT_HEIGHT);
		EnableItem(ID_EDIT_WIDTH);
		EnableItem(ID_COMBO_UNIT);
		EnableItem(ID_COMBO_TILE);
	}
	else{
		DisableItem(ID_COMBO_EXT);
		DisableItem(ID_EDIT_HEIGHT);
		DisableItem(ID_EDIT_WIDTH);
		DisableItem(ID_COMBO_UNIT);
		DisableItem(ID_COMBO_TILE);
	}
}

/*-----------------------------------------------------------------------------
  void OnChangeCombo()
  セル切り替えによる有効／無効の切り替え
-----------------------------------------------------------------------------*/

void CCfgDlg::
OnChangeCombo()
{
	if(ComboBox_GetCurSel(ID_COMBO_EXT) == 3){
		EnableItem(ID_EDIT_HEIGHT);
		EnableItem(ID_EDIT_WIDTH);
		EnableItem(ID_COMBO_UNIT);
	}
	else{
		DisableItem(ID_EDIT_HEIGHT);
		DisableItem(ID_EDIT_WIDTH);
		DisableItem(ID_COMBO_UNIT);
	}
}

/*
 * $Log: CConfig.cpp,v $
 * Revision 1.2  1999/07/10 05:12:58  MIYABI
 * CVS導入
 *
 */
