/*=============================================================================

	FILE: CListPage.cpp
 
=============================================================================*/

/*=============================================================================
	#include statements
=============================================================================*/

#include <stdinc.h>
#include <MyFunction.h>
#include <CMyFileDialog.h>

#include "General.h"
#include "CConfig.h"
#include "gSPIChain.h"
#include "ini.h"
#include "res/resource.h"

#include "CListPage.h"

/*=============================================================================
	#define statements
=============================================================================*/

#define DEFAULT_FILTER      "Bitmap File(*.bmp)\0*.bmp\0"
#define DEFAULT_FILTER_LEN  25
#define ALL_PIC             "全ての画像ﾌｧｲﾙ\0*.bmp"
#define ALL_PIC_LEN         20
#define ALL_ARC             "全ての書庫ﾌｧｲﾙ\0"
#define ALL_ARC_LEN         15
#define ALL                 "All Files(*.*)\0*.*\0\0"
#define ALL_LEN             20
#define PV_WIDTH            161
#define PV_HEIGHT           136
#define TT_FILE             "画像ファイルの選択"
#define TT_ARC              "書庫ファイルの選択"

/*=============================================================================
  外部参照
=============================================================================*/

extern CSPIChain g_tempSPI;
extern HFONT g_hFont;
extern HWND g_hMainDlg;
extern HANDLE g_hSema;

HWND  g_hListPageDialog = NULL;

/*=============================================================================
  class CFileListView
=============================================================================*/

/*=============================================================================
  void InitWindow(HWND)
  リストビューの作成、初期化
=============================================================================*/
void CFileListView::
InitWindow(HWND hWnd)
{
	DWORD dwStyle;

	m_hWnd = ::CreateWindowEx(WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE,
														WC_LISTVIEW, (LPSTR)NULL,
														WS_TABSTOP | WS_VISIBLE | WS_CHILDWINDOW |
														LVS_REPORT | LVS_SHOWSELALWAYS |
														LVS_NOCOLUMNHEADER,
														11, 35, 346, 185, hWnd, (HMENU)IDL_LIST,
														GMH(), NULL);
	dwStyle = ListView_GetExtendedListViewStyle(m_hWnd);
	dwStyle |= LVS_EX_FULLROWSELECT;

	ListView_SetExtendedListViewStyle(m_hWnd, dwStyle);

	/* カラム幅の設定(実際には、アイテムの長さに自動調整される) */
	SetColumn(1, 100, "ファイル名");

	SetSubClass();
}

/*=============================================================================
  void SetItemForcus(int)
  アイテムを選択状態にする
=============================================================================*/
void CFileListView::
SetItemForcus(int nIndex)
{
	ListView_SetItemState(m_hWnd, nIndex, LVIS_SELECTED | LVIS_FOCUSED,
												LVIS_SELECTED | LVIS_FOCUSED);
	ListView_EnsureVisible(m_hWnd, nIndex, TRUE);
}

/*=============================================================================
  void MoveItem(int, int)
  項目の入れ替え
=============================================================================*/
void CFileListView::
MoveItem(int iPrev, int iNext)
{
	char szText[_MAX_PATH];

	GetItemText(iPrev, szText, 0, _MAX_PATH);
	DeleteItem(iPrev);

	InsertItem(szText, iNext, -1);
}

/*=============================================================================
  void Update(CDocument&)
  ドキュメントの内容をリストビューに反映
=============================================================================*/
void CFileListView::
Update(CDocument& doc)
{
	DeleteAll();
	if(doc.empty()){
		return;
	}
	CDocument::fData pn;
	for(pn = doc.begin(); pn != doc.end(); pn++){
		char  temp[_MAX_PATH];
		if(pn->GetArcName().length()){
	    ::wsprintf(temp, "<%s>:%s", pn->GetArcName().c_str(),
								 pn->GetFileName().c_str());
		}
		else{
	    ::wsprintf(temp, "%s", pn->GetFileName().c_str());
		}
		AddString(temp);
	}
	SetColumnWidth(0);
}

/*-----------------------------------------------------------------------------
  BOOL OnContextMenu()
-----------------------------------------------------------------------------*/
OVR_OnContextMenu(CFileListView)
{
	_Err0("CFileListView::OnContextMenu()\n");

	HMENU hMenu = ::LoadMenu(GMH(), MIR(IDR_CONFIG_LISTVIEW));
	HMENU hSub = ::GetSubMenu(hMenu, 0);

	if(GetItemCount() == 0){
		/* リストに項目がない */
		::EnableMenuItem(hSub, IDM_DEL_SELECTED, MF_BYCOMMAND | MF_GRAYED);
		::EnableMenuItem(hSub, ID_MNU2_PREVIEW, MF_BYCOMMAND | MF_GRAYED);
		::EnableMenuItem(hSub, ID_MNU2_SETNEXT, MF_BYCOMMAND | MF_GRAYED);
		::EnableMenuItem(hSub, IDL_CONFIG, MF_BYCOMMAND | MF_GRAYED);
		::EnableMenuItem(::GetSubMenu(hSub, 7), IDM_SUM_LIST,
										 MF_BYCOMMAND | MF_GRAYED);
	}
	if(GetNextItem() == -1){
		/* 
			 何も選択されていないとき、
			 「選択項目を削除」「プレビュー表示」「次回開始位置に指定」
			 「個別設定」「サムネイル表示」→「現在のリスト」を
			 ディスエーブルにする。
		*/
		::EnableMenuItem(hSub, IDM_DEL_SELECTED, MF_BYCOMMAND | MF_GRAYED);
		::EnableMenuItem(hSub, ID_MNU2_PREVIEW, MF_BYCOMMAND | MF_GRAYED);
		::EnableMenuItem(hSub, ID_MNU2_SETNEXT, MF_BYCOMMAND | MF_GRAYED);
		::EnableMenuItem(hSub, IDL_CONFIG, MF_BYCOMMAND | MF_GRAYED);
	}

	if(g_tempSPI.am().empty() || g_tempSPI.AllDisable_AM()){
		/*
			書庫展開用のプラグインがないときは、
			「サムネイル表示」→「書庫内のファイル」をディスエーブルにする
		*/
		::EnableMenuItem(::GetSubMenu(hSub, 7), IDM_SUM_ARCHIVE,
										 MF_BYCOMMAND | MF_GRAYED);
	}

	::TrackPopupMenu(hSub, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON,
									 X, Y, 0, GetParent(), NULL);
	::DestroyMenu(hMenu);

	*plr = 0;
	return 1;
}

/*-----------------------------------------------------------------------------
  BOOL OnLButtonDblClk()
-----------------------------------------------------------------------------*/
OVR_OnLButtonDblClk(CFileListView)
{
	_Err0("CFileListView::OnLButtonDblClk()\n");

	SendCommand(0, ID_MNU2_PREVIEW, 0);
	*plr = 0;

	return 1;
}

/*-----------------------------------------------------------------------------
  BOOL OnDropFiles()
-----------------------------------------------------------------------------*/
OVR_OnDropFiles(CFileListView)
{
	_Err0("CFileListView::OnDropFiles()\n");
  /* 親ウィンドウにメッセージ転送 */
	::SendMessage(GetParent(), WM_DROPFILES, (WPARAM)hDrop, 0L);

	*plr = 0;
	return 1;
}

/*=============================================================================
  class CListPage
=============================================================================*/

/*=============================================================================
  void Init(HWND, int, RECT)
  初期化
=============================================================================*/
void CListPage::
Init(HWND hWndTab, int nCmdShow, RECT* pRect)
{
	_Err0("CListPage::Init()\n");

  /* フォントを作成 */
	if(g_hFont == NULL){
		LOGFONT lf;
		int     size;
		HDC     hdc;

		::ZeroMemory(&lf, sizeof(lf));
		hdc = ::GetDC(NULL);
		size = ::GetDeviceCaps(hdc, LOGPIXELSX) / 8;
		::ReleaseDC(NULL, hdc);

		lf.lfHeight = size;
		lf.lfWeight = 400;
		lf.lfCharSet = SHIFTJIS_CHARSET;
		lf.lfOutPrecision = OUT_STROKE_PRECIS;
		lf.lfClipPrecision = CLIP_STROKE_PRECIS;
		lf.lfQuality = DRAFT_QUALITY;
		lf.lfPitchAndFamily = VARIABLE_PITCH | FF_MODERN;
		lstrcpy(lf.lfFaceName, "MS UI Gothic");

		g_hFont = ::CreateFontIndirect(&lf);
	  /* MS UI Gothic がなければ、ＭＳ ゴシックで作成 */
		if(g_hFont == NULL){
			::lstrcpy(lf.lfFaceName, "ＭＳ ゴシック");
			g_hFont = ::CreateFontIndirect(&lf);
		}
	}

	g_hListPageDialog = Create(MIR(IDD_LIST), hWndTab);
	ShowWindow(nCmdShow);
	MoveWindow(pRect->left, pRect->top, pRect->right - pRect->left,
						 pRect->bottom - pRect->top, TRUE);
}

/*=============================================================================
  int OnSave()
  設定を保存
=============================================================================*/
int CListPage::
OnSave()
{
	if(GetDlgItemInt(IDL_EDIT) > (int)m_FileData.size()){
		MessageBox("次回開始位置が無効です。有効な値を入力してください！",
							 DLG_CAPTION);
		_Err0("BOOL CListPage::OnSave() return 0\n");
		return 0;
	}
	else{
		CListData ListData;

		ListData.SetOption1(IsDlgButtonChecked(IDL_OPTION1));
		ListData.SetOption2(IsDlgButtonChecked(IDL_OPTION2));
		ListData.SetStart(GetDlgItemInt(IDL_EDIT));
		ListData.SetDrawPattern(IsDlgButtonChecked(IDC_RADIO1) ? 0 : 1);

		if(m_FileData.GetName() != MUDAI){
			ListData.SetLastList(m_FileData.GetName().c_str());
		}
		else{
			ListData.SetLastList("");
		}

		::WaitForSingleObject(g_hSema, INFINITE);
		ListData.Write();
		::ReleaseSemaphore(g_hSema, 1, NULL);

		_Err0("BOOL CListPage::OnSave() return 1\n");
		return 1;
	}
}

/*=============================================================================
  int OnCancel()
  設定を保存せずに終了。ただしファイルが変更されている場合は尋ねる。
=============================================================================*/
int CListPage::
OnCancel()
{
	if(m_FileData.IsChanged()){
		int ret = AskDocumentSave();

		if(ret == IDCANCEL){
			_Err0("BOOL CListPage::OnCancel() return 0\n");
			return 0;
		}
		else if(ret == IDNO){
			_Err0("BOOL CListPage::OnCancel() return 1\n");
			return 1;
		}
		else if(ret == IDYES){
			char  szBuf[_MAX_PATH] = "";

			if(m_FileData.IsNew() && GetSaveFileName(szBuf, _MAX_PATH) == 0){
				_Err0("BOOL CListPage::OnCancel() return 0\n");
				return 0;
			}

			m_FileData.Save(szBuf);
			_Err0("ファイルを保存しました");
		}
	}

	_Err0("BOOL CListPage::OnCancel() return 1\n");
	return 1;
}

/*-----------------------------------------------------------------------------
  BOOL wm_proc()
-----------------------------------------------------------------------------*/
OVR_WM_PROC(CListPage)
{
	switch(msg){
		case WM_ADDLIST:
		{
			_Err1("WM_ADDLIST : wp = %s\n", (const char *)wp);

			if(Chk_SameName((const char *)wp)){
				CMyFileData data;
				std::string   szFile, szArc;

				SeparateFileName(szFile, szArc, (const char *)wp);
				data.SetArcName(szArc.c_str());
				data.SetFileName(szFile.c_str());

				m_FileData.push_back(data);
			}
			break;
		}

		case WM_DELETELIST:
		{
			_Err1("WM_DELETELIST : wp = %d\n", (int)wp);

			CDocument::fData pn;
			for(pn = m_FileData.begin(); pn != m_FileData.end(); ){
				if(pn->GetIndex() == (int)wp){
					pn = m_FileData.erase(pn);
				}
				else{
					pn++;
				}
			}
			break;
		}

		case WM_OPERATION_END:
	    DocumentIsChanged();
	    m_FileListView.Update(m_FileData);
	    break;

		default:
	    return CMyWnd::wm_proc(msg, wp, lp, plr);
	}

	*plr = 0;
	return 1;
}

/*-----------------------------------------------------------------------------
  BOOL OnInitDialog()
-----------------------------------------------------------------------------*/
OVR_OnInitDialog(CListPage)
{
	_Err0("CListPage::OnInitDialog\n");

	m_FileListView.InitWindow(m_hWnd);

	m_prev.InitWindow(m_hWnd);
	m_prev.MoveWindow(360, 35, 174, 147, TRUE);

	CListData ListData;

	::WaitForSingleObject(g_hSema, INFINITE);
	ListData.Read();
	::ReleaseSemaphore(g_hSema, 1, NULL);
  /* オプションの読み込み */
	CheckDlgButton(IDL_OPTION1, ListData.GetOption1());
	CheckDlgButton(IDL_OPTION2, ListData.GetOption2());
  /* 順序 */
	if(ListData.GetDrawPattern() == 0){
		CheckDlgButton(IDC_RADIO1, 1);
	}
	else{
		CheckDlgButton(IDC_RADIO2, 1);
	}

  /* カレントのリスト名取得 */
	std::string  szFile = ListData.GetLastList();

	m_FileData.Init();
	if(szFile.length() == 0 || m_FileData.Open(szFile.c_str()) == 0){
		m_FileData.Create();
	}

	SetDlgItemInt(IDL_EDIT, ListData.GetStart());

	Document_Open();

	::DragAcceptFiles(m_FileListView.GetSafeHwnd(), TRUE);

	return (*plr = 1);
}

/*-----------------------------------------------------------------------------
  BOOL OnClose()
-----------------------------------------------------------------------------*/
OVR_OnClose(CListPage)
{
	_Err0("CListPage::OnClose()\n");

	m_FileListView.ResetSubClass();
	_Err0("ファイル一覧リストビューのサブクラス化を解除しました\n");
	m_prev.SendMessage(WM_CLOSE);

	::DeleteObject(g_hFont);
	g_hFont = NULL;
	g_hListPageDialog = NULL;

	ccwNode pn;

	if((pn = m_ListCWList.begin()) != m_ListCWList.end()){
		pn->SendMessage(WM_CLOSE);
	}

	for(pn = m_CWList.begin(); pn != m_CWList.end(); pn++){
		pn->SendMessage(WM_CLOSE);
	}

	m_ListCWList.clear();
	m_CWList.clear();
	m_FileData.clear();

	*plr = 0;
	return 1;
}

/*-----------------------------------------------------------------------------
  BOOL OnDropFiles()
-----------------------------------------------------------------------------*/
OVR_OnDropFiles(CListPage)
{
	_Err0("CListPage::OnDropFiles()\n");

	int num = m_FileListView.GetItemCount();

	mf::DragedFile(hDrop, (long)this, drag);
	::DragFinish(hDrop);

	if(num == m_FileListView.GetItemCount()){
		MessageBox("追加する項目はありません", "情報");
	}
	else{
	  /* リストが変更されたので、スピンコントロールなどを変更 */
		DocumentIsChanged();

	  /* カラム幅の調整 */
		m_FileListView.SetColumnWidth(0);
	}

	return TRUE;
}

/*-----------------------------------------------------------------------------
  BOOL OnCommand()
-----------------------------------------------------------------------------*/
OVR_OnCommand(CListPage)
{
	switch(wID){
	  /* 値の変更などを通知する */
		case IDL_OPTION1:
		case IDL_OPTION2:
	    if(wNotifyCode == BN_CLICKED){
		    NotifyChanged(g_hMainDlg);
			}
	    break;

		case IDL_EDIT:
	    if(wNotifyCode != EN_CHANGE){
		    return 0;
			}
	    NotifyChanged(g_hMainDlg);
	    break;

		/* Button */
		/* メニュー表示 */
		case IDL_COM_LIST:
		case IDL_ADDTION:
		case IDL_DELETE:
		case IDL_MOVE:
	    on_menu(wID);
	    break;

		case IDL_CONFIG:
	    if(on_config() == 1){
		    NotifyChanged(g_hMainDlg);
			}
	    break;

		case IDM_FILE_OPEN:
		case IDA_CTRL_O:
	    if(on_loadlist() == 1){
		    NotifyChanged(g_hMainDlg);
			}
	    break;

		case IDM_FILE_NEW:
		case IDA_CTRL_N:
	    if(on_new() == 1){
		    NotifyChanged(g_hMainDlg);
			}
	    break;

		case IDM_FILE_SAVE:
		case IDA_CTRL_S:
	    on_savelist();
	    break;

		case IDM_FILE_SAVEAS:
	    if(on_saveas() == 1){
		    NotifyChanged(g_hMainDlg);
			}
	    break;

		case IDM_ADD_PICTURE:
		case IDA_CTRL_P:
	    if(on_addtion() == 1){
		    NotifyChanged(g_hMainDlg);
			}
	    break;

		case IDM_ADD_ARCHIVE:
		case IDA_CTRL_A:
	    if(on_archive() == 1){
		    NotifyChanged(g_hMainDlg);
			}
	    break;

		case IDM_SUM_LIST:
		case IDA_SHIFT_L:
	    on_sum_list();
	    break;

		case IDM_SUM_FOLDER:
		case IDA_SHIFT_F:
	    on_sum_folda();
	    break;

		case IDM_SUM_ARCHIVE:
		case IDA_SHIFT_A:
	    on_sum_archive();
	    break;

		case IDM_DEL_SELECTED:
		case IDA_DELETE:
			if(on_delete() == 1){
		    NotifyChanged(g_hMainDlg);
			}
	    break;

		case IDM_DEL_ALL:
	    if(on_clear() == 1){
		    NotifyChanged(g_hMainDlg);
			}
	    break;

		case IDM_MOV_UP:
		case IDA_CTRL_UP:
	    _Err1("wID = %d\n", wID);
	    if(on_up() == 1){
		    NotifyChanged(g_hMainDlg);
			}
	    break;

		case IDM_MOV_DOWN:
		case IDA_CTRL_DOWN:
	    if(on_down() == 1){
		    NotifyChanged(g_hMainDlg);
			}
	    break;

		case IDM_MOV_CEIL:
		case IDA_CTRL_HOME:
	    if(on_ceil() == 1){
		    NotifyChanged(g_hMainDlg);
			}
	    break;

		case IDM_MOV_FLOOR:
		case IDA_CTRL_END:
	    if(on_floor() == 1){
		    NotifyChanged(g_hMainDlg);
			}
	    break;

		case ID_MNU2_PREVIEW:
	    on_preview();
	    break;

		case ID_MNU2_EXPLORER:
	    on_explorer();
	    break;

		case ID_MNU2_SETNEXT:
	    if(on_setnext() == 1){
		    NotifyChanged(g_hMainDlg);
			}
	    break;

		default:
	    return 0;
	}

	*plr = 0;
	return 1;
}

/*-----------------------------------------------------------------------------
  int on_menu(int)
  ポップアップメニューを表示
-----------------------------------------------------------------------------*/
int CListPage::
on_menu(int nID)
{
	_Err0("BOOL CListPage::on_menu(int)\n");

	int iMenuID;

	switch(nID){
		case IDL_COM_LIST:
	    iMenuID = IDR_CONFIG_FILE;
	    break;

		case IDL_ADDTION:
	    iMenuID = IDR_CONFIG_ADD;
	    break;

		case IDL_DELETE:
	    iMenuID = IDR_CONFIG_DELETE;
	    break;

		case IDL_MOVE:
	    iMenuID = IDR_CONFIG_MOVE;
	    break;

		default:
	    return 0;
	}

	HMENU hMenu = ::LoadMenu(GMH(), MIR(iMenuID));
	HMENU hSub = ::GetSubMenu(hMenu, 0);

	if(iMenuID == IDR_CONFIG_ADD &&
		 (g_tempSPI.am().empty() || g_tempSPI.AllDisable_AM())){
		::EnableMenuItem(hSub, IDM_ADD_ARCHIVE, MF_BYCOMMAND | MF_GRAYED);
		::EnableMenuItem(::GetSubMenu(hSub, 3), IDM_SUM_ARCHIVE,
										 MF_BYCOMMAND | MF_GRAYED);
	}

	POINT pt;

	::GetCursorPos(&pt);
	::TrackPopupMenu(hSub, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON,
									 pt.x, pt.y, 0, m_hWnd, NULL);
	::DestroyMenu(hMenu);

	return TRUE;
}

/*-----------------------------------------------------------------------------
  int on_config()
  個別設定ダイアログの表示
-----------------------------------------------------------------------------*/
int CListPage::
on_config()
{
	int nItem;

	if((nItem = m_FileListView.GetNextItem()) == -1){
		return 0;
	}

	CDocument::fData pn = m_FileData.begin();

	while(nItem--){
		pn++;
	}

	pn->CreatePreview(PV_WIDTH, PV_HEIGHT);

	CCfgDlg  dlg;
	if(dlg.DoModal(m_hWnd, pn)){
		set_filename(0);
		m_FileData.SetChangeFlag();

		_Err0("BOOL CListPage::on_config() return 1\n");
		return 1;
	}
	else{
		_Err0("BOOL CListPage::on_config() return 0\n");
		return 0;
	}
}

/*-----------------------------------------------------------------------------
  ファイル系コマンド

  int on_loadlist()
  int on_new()
  int on_savelist()
  int on_saveas()
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  int on_loadlist()
  リストを開く
-----------------------------------------------------------------------------*/
int CListPage::
on_loadlist()
{
	if(m_FileData.IsChanged()){
		int ret = AskDocumentSave();

		if(ret == IDCANCEL){
			_Err0("BOOL CListPage::on_loadlist() return 0\n");
			return 0;
		}
		else if(ret == IDYES){
			char  szBuf[_MAX_PATH] = "";

			ret = 1;

			if(m_FileData.IsNew()){
				ret = GetSaveFileName(szBuf, _MAX_PATH);
			}

			if(ret != 0){
				m_FileData.Save(szBuf);
			}
		}
	}

	char szFile[_MAX_PATH] = "";
	CMyFileDialog dlg;
	int  ret;

	ret = dlg.GetOpenFileName(m_hWnd, STR_LIST, "壁紙リストの選択", szFile,
														_MAX_PATH);

	if(ret == 1 && m_FileData.Open(szFile)){
		m_prev.DeletePreview();
		Document_Open();

		_Err0("BOOL CListPage::on_loadlist() return 1\n");
		return 1;
	}

	_Err0("BOOL CListPage::on_loadlist() return 0\n");
	return 0;
}

/*-----------------------------------------------------------------------------
	int on_new()
  リストの新規作成
-----------------------------------------------------------------------------*/
int CListPage::
on_new()
{
	if(m_FileData.IsChanged()){
		int ret = AskDocumentSave();

		if(ret == IDCANCEL){
			_Err0("BOOL CListPage::on_new() return 0\n");
			return 0;
		}
		else if(ret == IDYES){
			char szBuf[_MAX_PATH] = "";
			ret = 1;

			if(m_FileData.IsNew()){
				ret = GetSaveFileName(szBuf, _MAX_PATH);
			}

			if(ret != 0){
				m_FileData.Save(szBuf);
			}
		}
	}

	if(m_FileData.Create()){
		m_prev.DeletePreview();
		Document_Open();

		_Err0("BOOL CListPage::on_new() return 1\n");
		return 1;
	}
	else{
		_Err0("BOOL CListPage::on_new() return 0\n");
		return 0;
	}
}

/*-----------------------------------------------------------------------------
  int on_savelist()
  リストを上書き保存
-----------------------------------------------------------------------------*/
int CListPage::
on_savelist()
{
	char szBuf[_MAX_PATH] = "";

	if(m_FileData.IsNew() && GetSaveFileName(szBuf, _MAX_PATH) == 0){
		_Err0("BOOL CListPage::on_savelist() return 0\n");
		return 0;
	}

	if(m_FileData.Save(szBuf)){
		set_filename();

		_Err0("BOOL CListPage::on_savelist() return 1\n");
		return 1;
	}
	else{
		_Err0("BOOL CListPage::on_savelist() return 0\n");
		return 0;
	}
}

/*-----------------------------------------------------------------------------
  int on_saveas()
  リストを名前を付けて保存
-----------------------------------------------------------------------------*/
int CListPage::
on_saveas()
{
	char szBuf[_MAX_PATH] = "";

	if(GetSaveFileName(szBuf, _MAX_PATH) == 0){
		_Err0("BOOL CListPage::on_saveas() return 0\n");
		return 0;
	}

	if(m_FileData.Save(szBuf)){
		set_filename();

		_Err0("BOOL CListPage::on_saveas() return 1\n");
		return 1;
	}
	else{
		_Err0("BOOL CListPage::on_saveas() return 0\n");
		return 0;
	}
}

/*-----------------------------------------------------------------------------
  追加系コマンド

  int on_addtion()
  int on_archive()
  int on_sum_folda()
  int on_sum_archive()
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  int on_addton()
  ファイルの追加
-----------------------------------------------------------------------------*/
int CListPage::
on_addtion()
{
	char szfile[1024];

	if(!FileDialog(1, szfile, sizeof(szfile))){
		DWORD dwErr = ::CommDlgExtendedError();

		if(dwErr == FNERR_BUFFERTOOSMALL){
			MessageBox("ﾌｧｲﾙ数が多すぎます。減らしてください。");
		}
		_Err0("BOOL CListPage::on_addtion() return 0\n");

		return 0;
	}

	int count = get_filename(szfile, (LONG)this, add_list);

	if(count){
		DocumentIsChanged();

	  /* カラム幅の調整 */
		m_FileListView.SetColumnWidth(0);

		_Err0("BOOL CListPage::on_addtion() return 1\n");
		return 1;
	}
	else{
		MessageBox("追加項目はありません", "情報");

		_Err0("BOOL CListPage::on_addtion() return 0\n");
		return 0;
	}
}

/*-----------------------------------------------------------------------------
  int on_archive()
  書庫ファイルの中身を追加
-----------------------------------------------------------------------------*/
int CListPage::
on_archive()
{
	char  szArchive[1000];
	int   ret;

	if(!FileDialog(2, szArchive, sizeof(szArchive) - 1)){
		_Err0("BOOL CListPage::on_archive() return 0\n");
		return 0;
	}

	ret = arc_AddList(szArchive);
	if(ret == -3){
		MessageBox("ｱｰｶｲﾌﾞ内のﾌｧｲﾙ情報を取得できません");

		_Err0("BOOL CListPage::on_archive() return 0\n");
		return 0;
	}
	else if(ret == 0){
		MessageBox("追加項目はありません", "情報");

		_Err0("BOOL CListPage::on_archive() return 0\n");
		return 0;
	}
	else{
		DocumentIsChanged();

	  /* カラム幅の調整 */
		m_FileListView.SetColumnWidth(0);

		_Err0("BOOL CListPage::on_archive() return 0\n");
		return 1;
	}
}

/*-----------------------------------------------------------------------------
  int on_sum_folda()
  フォルダ内の画像一覧を作成
-----------------------------------------------------------------------------*/
int CListPage::
on_sum_folda()
{
	char  szFolder[_MAX_PATH] = "";

	/* フォルダ選択 */
	if(GetOpenFolderName(szFolder, _MAX_PATH) == 0){
		_Err0("CListPage::on_sum_folda() return 0\n");
		return 0;
	}
	if(szFolder[::lstrlen(szFolder) - 1] != '\\'){
		::lstrcat(szFolder, "\\");
	}

	_Err1("%s\n", szFolder);

  /* 全てのファイルを取得 */
	WIN32_FIND_DATA fd;
	HANDLE hFind;
	char szFind[_MAX_PATH];

	::wsprintf(szFind, "%s*.*", szFolder);

	hFind = ::FindFirstFile(szFind, &fd);
	if(hFind == INVALID_HANDLE_VALUE){
		_Err0("CListPage::on_sum_folda() return 0\n");
		return 0;
	}

	ccwNode pn;

	if((pn = AdjustCWList()) == NULL){
		_Err0("CListPage::on_sum_folda() return 0\n");
		return 0;
	}

	PATHDATA_T  pathData;

	pathData.szArc = szFolder;

	do{
		if(fd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY){
			pathData.szFile = fd.cFileName;
			_Err2("%s, %s\n", szFolder, fd.cFileName);

			pn->AddData(pathData);
		}
	}while(::FindNextFile(hFind, &fd) != 0);

	::FindClose(hFind);

	pn->CreateSumNail(szFind, TP_FOLDER);

	_Err0("CListPage::on_sum_folda() return 1\n");
	return 1;
}

/*-----------------------------------------------------------------------------
  int on_sum_archive()
  書庫内の画像一覧を作成
-----------------------------------------------------------------------------*/
int CListPage::
on_sum_archive()
{
	char szArc[_MAX_PATH] = "";

  /* 書庫ファイル選択 */
	if(FileDialog(2, szArc, _MAX_PATH) == 0){
		_Err0("CListPage::on_sum_archive() return 0\n");
		return 0;
	}

	_Err1("%s\n", szArc);

	HLOCAL hInfo = NULL;

	if(g_tempSPI.GetArchiveInfo(szArc, &hInfo) == 0){
		_Err1("%sの書庫データ取得に失敗？\n", szArc);
		return 0;
	}

	ccwNode pn;

	if((pn = AdjustCWList()) == NULL){
		mf::_LocalFree(hInfo);
		return 0;
	}

	SpiArcFileInfo *pArc, *p;
	PATHDATA_T pathData;

	pathData.szArc = szArc;
	pArc = (SpiArcFileInfo*)::LocalLock(hInfo);
	for(p = pArc; p->method[0] != '\0'; p++){
		pathData.szFile = p->filename;
		pn->AddData(pathData);
	}

	mf::_LocalFree(hInfo);

	char szTitle[_MAX_PATH];

	::wsprintf(szTitle, "<%s>:*.*", szArc);
	pn->CreateSumNail(szTitle, TP_ARCHIVE);

	return TRUE;
}

/*-----------------------------------------------------------------------------
  削除系コマンド
  int on_delete()
  int on_clear()
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  int on_delete()
  選択項目の削除
-----------------------------------------------------------------------------*/
int CListPage::
on_delete()
{
	int num;
	CDocument::fData pn, begin, end;

	num = m_FileListView.GetSelectedCount();
	if(num == 0){
		_Err0("BOOL CListPage::on_delete() return 0\n");
		return 0;
	}

	int *index = new int[num + 1];

  /* 選択項目のインデックスを取得 */
	index[0] = m_FileListView.GetNextItem();
	for(int i = 1; i < num; i++){
		index[i] = m_FileListView.GetNextItem(index[i - 1]);
	}

	begin = m_FileData.begin();
	end = m_FileData.end();

	for(; i >= 0; i--){
		m_FileListView.DeleteItem(index[i]);
		for(pn = begin; pn != end; pn++){
			if(pn->GetIndex() == index[i]){
				m_FileData.erase(pn);
				break;
			}
		}
	}

	m_prev.DeletePreview();
	DocumentIsChanged();

	_Err0("BOOL CListPage::on_delete() return 1\n");
	return 1;
}

/*-----------------------------------------------------------------------------
  int on_clear()
  全て削除
-----------------------------------------------------------------------------*/
int CListPage::
on_clear()
{
	if(m_FileListView.GetItemCount() <= 0){
		_Err0("BOOL CListPage::on_clear() return 0\n");
		return 0;
	}

	m_FileListView.DeleteAll();
	m_FileData.clear();

	DocumentIsChanged();
	m_prev.DeletePreview();

	_Err0("BOOL CListPage::on_clear() return 1\n");
	return 1;
}

/*-----------------------------------------------------------------------------
  移動系コマンド
  int on_up()
  int on_down()
  int on_ceil()
  int on_floor()
  int on_sort()
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  int on_up()
  一つ上へ
-----------------------------------------------------------------------------*/
int CListPage::
on_up()
{
	LONG lSelect = m_FileListView.GetNextItem();

	if(lSelect == -1 || lSelect == 0){
		_Err0("BOOL CListPage::on_up() return 0\n");
		return 0;
	}

	CDocument::fData  pn1, pn2;
	CMyFileData temp;

	pn2 = m_FileData.begin() + lSelect;
	pn1 = pn2 - 1;

	temp = *pn2;
	*pn2 = *pn1;
	*pn1 = temp;

	m_FileListView.MoveItem(lSelect - 1, lSelect);
	m_FileListView.SetItemForcus(lSelect - 1);

	DocumentIsChanged();

	_Err0("BOOL CListPage::on_up() return 1\n");
	return 1;
}

/*-----------------------------------------------------------------------------
  int on_down()
  一つ下へ
-----------------------------------------------------------------------------*/
int CListPage::
on_down()
{
	LONG lSelect, lMax;

	lSelect = m_FileListView.GetNextItem();
	lMax = m_FileListView.GetItemCount();

	if(lSelect == -1 || lSelect + 1 == lMax){
		_Err0("BOOL CListPage::on_down() return 0\n");
		return 0;
	}

	CDocument::fData  pn1, pn2;
	CMyFileData temp;

	pn1 = m_FileData.begin() + lSelect;
	pn2 = pn1 + 1;

	temp = *pn1;
	*pn1 = *pn2;
	*pn2 = temp;

	m_FileListView.MoveItem(lSelect, lSelect + 1);
	m_FileListView.SetItemForcus(lSelect + 1);

	DocumentIsChanged();

	_Err0("BOOL CListPage::on_down() return 1\n");
	return 1;
}

/*-----------------------------------------------------------------------------
  int on_ceil()
  一番上へ
-----------------------------------------------------------------------------*/
int CListPage::
on_ceil()
{
	LONG lSelect = m_FileListView.GetNextItem();

	if(lSelect == -1 || lSelect == 0){
		_Err0("BOOL CListPage::on_ceil() return 0\n");
		return 0;
	}

	CDocument::fData  pn1;

	pn1 = m_FileData.begin() + lSelect;

	CMyFileData data = *pn1;

	m_FileData.erase(pn1);
	m_FileData.insert(m_FileData.begin(), data);

	m_FileListView.MoveItem(lSelect, 0);

	m_FileListView.SetItemForcus(0);

	DocumentIsChanged();

	_Err0("BOOL CListPage::on_ceil() return 1\n");
	return 1;
}

/*-----------------------------------------------------------------------------
  int on_floor()
  一番下へ
-----------------------------------------------------------------------------*/
int CListPage::
on_floor()
{
	LONG lSelect, lMax;

	lSelect = m_FileListView.GetNextItem();
	lMax = m_FileListView.GetItemCount() - 1;
	if(lSelect == -1 || lSelect == lMax){
		_Err0("BOOL CListPage::on_floor() return 0\n");
		return 0;
	}

	CDocument::fData  pn1;

	pn1 = m_FileData.begin() + lSelect;
	CMyFileData data = *pn1;

	m_FileData.erase(pn1);
	m_FileData.push_back(data);

	m_FileListView.MoveItem(lSelect, MLV_LAST);

	m_FileListView.SetItemForcus(lMax);

	DocumentIsChanged();

	_Err0("BOOL CListPage::on_floor() return 1\n");
	return 1;
}

/*-----------------------------------------------------------------------------
  その他のコマンド
  int on_preview()
  int on_setnext()
  int on_exploror()
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  int on_preview()
  プレビューを表示
-----------------------------------------------------------------------------*/
int CListPage::
on_preview()
{
	int nItem;

	if((nItem = m_FileListView.GetNextItem()) == -1){
		_Err0("BOOL CListPage::on_preview() return 0\n");
		return 0;
	}

	std::vector<CMyFileData>::reference data = m_FileData[nItem];

	m_prev.textout("Now Loading...");

	if(data.CreatePreview(PV_WIDTH, PV_HEIGHT) != 0){
		m_prev.DrawPreview(data.GetPicture());

		_Err0("BOOL CListPage::on_preview() return 1\n");
		return 1;
	}

	m_prev.textout("無理でした…");

	_Err0("BOOL CListPage::on_preview() return 0\n");
	return 0;
}

/*-----------------------------------------------------------------------------
  int on_setnext()
  次回開始位置を設定
-----------------------------------------------------------------------------*/
int CListPage::
on_setnext()
{
	int nIndex;

	if((nIndex = m_FileListView.GetNextItem()) == -1){
		_Err0("CListPage::on_setnext() return 0\n");
		return 0;
	}
	else{
		Spin_SetPos(IDC_SPIN_NEXT, nIndex + 1);
		SetDlgItemInt(IDL_EDIT, nIndex + 1);

		_Err0("CListPage::on_setnext() return 1\n");
		return 1;
	}
}

/*-----------------------------------------------------------------------------
  int on_exploror()
  エクスプローラを起動
-----------------------------------------------------------------------------*/
int CListPage::
on_explorer()
{
	static std::string szDir = "";

	if(szDir == ""){
		char szTemp[_MAX_PATH + 1];

		::GetWindowsDirectory(szTemp, _MAX_PATH);
		szDir = szTemp;
	}

	::ShellExecute(NULL, "explore", szDir.c_str(), NULL, NULL, SW_SHOWNORMAL);

	return TRUE;
}

/*-----------------------------------------------------------------------------
  int FileDialog(int, char *, size_t)
  「ファイルを開く」ダイアログの表示
-----------------------------------------------------------------------------*/
int CListPage::
FileDialog(int mode, char *szFile, size_t size)
{
	_Err0("CListPage::FileDialog()\n");

	std::string      stOpenFilter;
	CMyFileDialog  dlg;

	/* フィルタの設定 */
	SetOpenFilter(mode, stOpenFilter);

  _Err1("length = %d\n", stOpenFilter.length());

	szFile[0] = 0;

	int ret;
	if(mode == 1){
		ret = dlg.GetOpenFileName(m_hWnd, stOpenFilter.c_str(), TT_FILE, szFile,
															size, MFD_MULTI);
	}
	else{
		ret = dlg.GetOpenFileName(m_hWnd, stOpenFilter.c_str(), TT_ARC, szFile,
															size);
	}

	return ret;
}

/*-----------------------------------------------------------------------------
  void SetOpenFilter(int, std::string&)
  フィルタの設定
-----------------------------------------------------------------------------*/
void CListPage::
SetOpenFilter(int mode, std::string& szOpenFilter)
{
	_Err0("CListPage::SetOpenFilter()\n");

	strList format, extend;

  /* 利用可能なプラグインから、対応するフォーマットを取得 */
	GetAllFormat(format, extend, mode);

	/* 可能な全てのフォーマット */
	Filter_All(extend, szOpenFilter, mode);

	/* 個別に */
	Filter_Sep(extend, format, szOpenFilter);

	/* 全てのファイル */
	szOpenFilter += "All Files(*.*)\t*.*\t\t";

	std::string::iterator p;

	/* TABをNULLに変換する */
	for(p = szOpenFilter.begin(); p != szOpenFilter.end(); p++){
		if(*p == '\t'){
			*p = '\0';
		}
	}

	return;
}

/*-----------------------------------------------------------------------------
  bool ChkSameName(const char *)
  同一ファイル名のチェック
-----------------------------------------------------------------------------*/
bool CListPage::
Chk_SameName(const char *szName)
{
	_Err0("CListPage::Chk_SameName()\n");

	if(IsDlgButtonChecked(IDL_OPTION1) == 0){
		return TRUE;
	}

	int   i, max;

	if((max = m_FileListView.GetItemCount()) == 0){
		return TRUE;
	}

  /* 重複してたら追加しない */
	std::string  stName;
	stName = szName;
	for(i = 0; i < max; i++){
		char szTemp[1000];
		m_FileListView.GetItemText(i, szTemp, 0, 1000);
		if(stName == szTemp){
			return FALSE;
		}
	}

	return TRUE;
}

/*-----------------------------------------------------------------------------
  bool Chk_Supported(const char *)
  展開可能か調べる
-----------------------------------------------------------------------------*/
int CListPage::
Chk_Supported(const char *str)
{
	if(IsDlgButtonChecked(IDL_OPTION2) == 0){
		_Err0("int CListPage::Chk_Supported() return 1\n");
		return 1;
	}

	HANDLE  hFile = mf::OpenFile(str, GENERIC_READ);

	if(BM_IsSupported(NULL, (DWORD)hFile)){
		::CloseHandle(hFile);
		_Err0("int CListPage::Chk_Supported() return 1\n");
		return 1;
	}

	::CloseHandle(hFile);

	if(g_tempSPI.Find_IN((char*)str, NULL) != g_tempSPI.in().end()){
		_Err0("int CListPage::Chk_Supported() return 1\n");
		return 1;
	}
	else{
		_Err0("int CListPage::Chk_Supported() return 0\n");
		return 0;
	}
}

/*-----------------------------------------------------------------------------
  bool Chk_Supported(const char *, SpiArcFileInfo*)
  展開可能か調べる(書庫ファイル版)
-----------------------------------------------------------------------------*/
int CListPage::
Chk_Supported(const char *szArc, SpiArcFileInfo* pArc)
{
	_Err0("int CListPage::Chk_Supported()\n");

	/* 展開不可でも追加する */
	if(IsDlgButtonChecked(IDL_OPTION2) == 0){
		return 1;
	}

	int       ret = 0;
	HLOCAL    hdest = NULL;
	CSPIChain::refNode spi = g_tempSPI.am().front();

	if(spi.GetFile((char *)szArc, pArc->position, (char *)&hdest, DISK2MEM,
								 (FARPROC)_ProgressCallback, 0)){
		char *pdest = (char *)::LocalLock(hdest);

		if(BM_IsSupported(NULL, (DWORD)pdest)
			 || g_tempSPI.Find_IN(pArc->filename, pdest) != g_tempSPI.in().end()){
			ret = 1;
		}
		else{
			ret = 0;
		}
	}

	mf::_LocalFree(hdest);

	return ret;
}

/*-----------------------------------------------------------------------------
  int file_AddList(const char *)
  ファイルをリストに追加
-----------------------------------------------------------------------------*/
int CListPage::
file_AddList(const char *str)
{
	_Err0("int CListPage::file_AddList()\n");

	/* 重複と展開可否のチェック */
	if(Chk_SameName(str) == 0 || Chk_Supported(str) == 0){
		return 0;
	}

	m_FileListView.AddString((char *)str);

	_Err1("File Name --- %s\n", str);

	CMyFileData  data;

	data.SetFileName(str);
	m_FileData.push_back(data);

	return 1;
}

/*-----------------------------------------------------------------------------
  int arc_AddList(const char *)
  書庫内のファイルをリストに追加
-----------------------------------------------------------------------------*/
int CListPage::
arc_AddList(const char *file)
{
	_Err0("int CListPage::arc_AddList()\n");

	HLOCAL hInfo = NULL;

	if(g_tempSPI.GetArchiveInfo(file, &hInfo) == 0){
		_Err0("CListPage::arc_AddList() return 0\n");
		return 0;
	}

	int count = 0;
	char szAll[512];
	SpiArcFileInfo  *p, *arc;

	arc = (SpiArcFileInfo*)::LocalLock(hInfo);

	for(p = arc; p->method[0] != '\0'; ++p){
		CMyFileData  data;

		::wsprintf(szAll, "<%s>:%s", file, p->filename);
		if(Chk_SameName(szAll) == 0 || Chk_Supported(file, p) == 0){
			continue;
		}

		m_FileListView.AddString(szAll);

		data.SetArcName(file);
		data.SetFileName(p->filename);

		m_FileData.push_back(data);
		count++;
	}

	mf::_LocalFree(hInfo);

	return count;
}

/*-----------------------------------------------------------------------------
  int SeparateFileName(std::string&, std::string&, const char *)
  <ARC>:FILE 形式を ARC と FILE に分割
-----------------------------------------------------------------------------*/
int CListPage::
SeparateFileName(std::string& file, std::string& arc, const char *str)
{
  _Err0("BOOL CListPage::SeparateFileName()\n");

	if(str[0] != '<'){
		file = str;
		arc = "";
		return 1;
	}
	else{
		int   n;
		char  temp[256] = "";

		for(n = 0; str[n] != '>'; n++) ;

		::lstrcpyn(temp, str + 1, n);
		arc = temp;
		file = str + n + 2;

		return 0;
	}
}

/*-----------------------------------------------------------------------------
  static int CALLBACK drag(const char *, int, int, long)
  Drag & Drop の処理
-----------------------------------------------------------------------------*/
int CALLBACK CListPage::
drag(const char *szFile, int n, int max, long lData)
{
	_Err0("int CALLBACK CListPage::drag()\n");

	CListPage  *temp = (CListPage*)lData;

	_Err1("Drag File : %s\n", szFile);

	/* フォルダだったら */
	if((::GetFileAttributes(szFile) & FILE_ATTRIBUTE_DIRECTORY)
		 == FILE_ATTRIBUTE_DIRECTORY)
	{
		/* Folder + *.* で検索 */
		char szFind[_MAX_PATH];

		if(szFile[::lstrlen(szFile) - 1] != '\\'){
			::wsprintf(szFind, "%s\\*.*", szFile);
		}
		else{
			::wsprintf(szFind, "%s*.*", szFile);
		}

		WIN32_FIND_DATA fd;
		HANDLE          hFind;

		hFind = ::FindFirstFile(szFind, &fd);
		if(hFind == INVALID_HANDLE_VALUE){
			return 0;
		}

		do{
			if(fd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY){
				if(temp->arc_AddList(fd.cFileName) == 0){
					temp->file_AddList(fd.cFileName);
				}
			}
		}while(::FindNextFile(hFind, &fd) != 0);

		::FindClose(hFind);
	}
	else{
		if(temp->arc_AddList(szFile)){
			return 0;
		}
		temp->file_AddList(szFile);
	}

	return 0;
}

/*-----------------------------------------------------------------------------
  static int add_list(const char *, LONG)
  複数ファイル選択ダイアログでのコールバック関数
-----------------------------------------------------------------------------*/
int CListPage::
add_list(const char *szFile, LONG lData)
{
	_Err0("int CALLBACK CListPage::add_list()\n");

	CListPage* list = (CListPage*)lData;

	return list->file_AddList(szFile);
}

/*-----------------------------------------------------------------------------
  void set_filename(int = 1)
  スタティックコントロールにリストファイル名を表示
-----------------------------------------------------------------------------*/
void CListPage::
set_filename(int b)
{
	_Err0("void CListPage::set_filename()\n");

	std::string  szTitle;

	szTitle = m_FileData.GetName() + ((b) ? "" : " *");
	SetDlgItemText(IDL_FILE, szTitle.c_str());
}

/*-----------------------------------------------------------------------------
  void DocumentIsChanged()
  ファイルの追加・削除などを行ったときに呼び出す
-----------------------------------------------------------------------------*/
void CListPage::
DocumentIsChanged()
{
	_Err0("BOOL CListPage::DocumentIsChanged()\n");

	/* ファイル名の脇に'*'を追加 */
	std::string  szTitle;

	szTitle += m_FileData.GetName() + " *";
	SetDlgItemText(IDL_FILE, szTitle.c_str());
	m_FileData.SetChangeFlag();

	int max;

	if((max = m_FileData.size()) == 0){
		Spin_SetRange(IDC_SPIN_NEXT, 1, 1);
		Spin_SetPos(IDC_SPIN_NEXT, 1);
		SetDlgItemInt(IDL_EDIT, 1);
		DisableItem(IDL_EDIT);
		DisableItem(IDC_SPIN_NEXT);
		DisableItem(IDL_DELETE);
		DisableItem(IDL_MOVE);
		DisableItem(IDL_CONFIG);
	}
	else{
		int n = GetDlgItemInt(IDL_EDIT);

		Spin_SetRange(IDC_SPIN_NEXT, 1, max);
		if(n > max){
			Spin_SetPos(IDC_SPIN_NEXT, max);
			SetDlgItemInt(IDL_EDIT, max);
		}
		else if(n == 0){
			Spin_SetPos(IDC_SPIN_NEXT, 1);
			SetDlgItemInt(IDL_EDIT, 1);
		}
		EnableItem(IDL_EDIT);
		EnableItem(IDC_SPIN_NEXT);
		EnableItem(IDL_DELETE);
		EnableItem(IDL_MOVE);
		EnableItem(IDL_CONFIG);
	}

	/* インデックスを再設定 */
	CDocument::fData  pn;

	pn = m_FileData.begin();
	for(int i = 0; pn != m_FileData.end(); i++, pn++){
		pn->SetIndex(i);
	}
}

/*-----------------------------------------------------------------------------
  void GetAllFormat(strList&, strList&, int)
  展開可能な全てのフォーマットを取得して,リストにする
-----------------------------------------------------------------------------*/
void CListPage::
GetAllFormat(strList& fo, strList& ex, int mode)
{
	CSPIChain::Node pn, end;
	char                temp[_MAX_PATH];

	if(mode == 1){
		pn = g_tempSPI.in().begin();
		end = g_tempSPI.in().end();

		fo.push_back("Bitmap File(*.bmp)");
		ex.push_back("*.bmp");
	}
	else{
		pn = g_tempSPI.am().begin();
		end = g_tempSPI.am().end();
	}

	for(; pn != end; pn++){
		if(pn->Use() == 0){
			continue;
		}
		for(int i = 0; ; i++){
			if(!pn->GetPluginInfo(2 * i + 3, temp, _MAX_PATH)){
				break;
			}
			fo.push_back(temp);

			pn->GetPluginInfo(2 * i + 2, temp, _MAX_PATH);
			ex.push_back(temp);
		}
	}
}

/*-----------------------------------------------------------------------------
  void FilTer_All(strList&, std::string&, int)
  展開可能な全てのフォーマットを表示するフィルタを作成
-----------------------------------------------------------------------------*/
void  CListPage::
Filter_All(strList& ex, std::string& szOpenFilter, int mode)
{
	strList::iterator  p;

	int                 i = 0;

	if(mode == 1){
		szOpenFilter = "全ての画像ファイル\t";
	}
	else{
		szOpenFilter = "全ての書庫ファイル\t";
	}

	for(p = ex.begin(); p != ex.end(); p++){
		if(p != ex.begin()){
			szOpenFilter += ";";
		}
		szOpenFilter += *p;
		_Err1("ext : %s\n", p->c_str());
	}

	szOpenFilter += "\t";
	_Err1("All : %s\n", szOpenFilter.c_str());
}

/*-----------------------------------------------------------------------------
  void Filter_Sep(strList&, strList&, std::string&)
  １ファイルのフィルターを作成
-----------------------------------------------------------------------------*/
void CListPage::
Filter_Sep(strList& ex, strList& fo, std::string& szOpenFilter)
{
	strList::iterator  px, pf;

	pf = fo.begin();
	px = ex.begin();
	for(; px != ex.end() && pf != fo.end(); px++, pf++){
		szOpenFilter += *pf;
		szOpenFilter += "\t";
		szOpenFilter += *px;
		szOpenFilter += "\t";
	}
}

/*-----------------------------------------------------------------------------
  int on_sum_list()
  リストのサムネイルを作成する
-----------------------------------------------------------------------------*/
int CListPage::
on_sum_list()
{
	ccwNode pn;

	pn = m_ListCWList.begin();
	if(pn != m_ListCWList.end()){
		if(::IsWindow(pn->GetSafeHwnd())){
			pn->RestoreWindow();
			return 1;
		}
		else{
			m_ListCWList.erase(pn);
		}
	}

	int num;

	if((num = m_FileListView.GetItemCount()) == 0){
		return 0;
	}
	else{
		CCatalogWindow  wnd;

		m_ListCWList.push_back(wnd);
		pn = m_ListCWList.begin();

		if(pn->InitWindow() != NULL){
			for(int i = 0; i < num; i++){
				/* ファイル名を取得 */
				char        szName[_MAX_PATH];
				PATHDATA_T  pathData;

				m_FileListView.GetItemText(i, szName);

				/* データリスト更新 */
				SeparateFileName(pathData.szFile, pathData.szArc, szName);
				pn->AddData(pathData);
			}
			pn->CreateSumNail(m_FileData.GetName().c_str(), TP_LIST);
		}
		else{
			m_ListCWList.clear();
		}
	}

	return TRUE;
}

/*-----------------------------------------------------------------------------
  int GetOpenFolderName(char *, int)
  サムネイルを表示するフォルダの取得
-----------------------------------------------------------------------------*/
int CListPage::
GetOpenFolderName(char *szBuffer, int size)
{
	CMyFileDialog  dlg;
	static std::string   def_folder;

	if(def_folder.length() == 0){
		char buf[_MAX_PATH];
		::GetCurrentDirectory(_MAX_PATH, buf);
		def_folder = buf;
	}

	if(dlg.GetOpenFolderName(m_hWnd, def_folder.c_str(), szBuffer, 1000, NULL) == IDCANCEL){
		return 0;
	}

	def_folder = szBuffer;
	return 1;
}

/*-----------------------------------------------------------------------------
  void Document_Open()
  リストを開いたときの各コントロールのセット
-----------------------------------------------------------------------------*/
void CListPage::
Document_Open()
{
	_Err0("void CListPage::Document_Open()\n");
	/* ファイル名の表示 */
	std::string  szTitle;

	szTitle += m_FileData.GetName();
	SetDlgItemText(IDL_FILE, szTitle.c_str());

	/* リストビューの更新 */
	CDocument::fData pn;

	m_FileListView.DeleteAll();
	for(pn = m_FileData.begin(); pn != m_FileData.end(); pn++){
		std::string    name;

		if(pn->GetArcName().length()){
			name = "<" + pn->GetArcName() + ">:";
		}
		name += pn->GetFileName();

		m_FileListView.AddString((char*)name.c_str());
	}
	m_FileListView.SetColumnWidth(0);

  /* スピンコントロールの設定 */
	int max;

	if((max = m_FileData.size()) == 0){
		Spin_SetRange(IDC_SPIN_NEXT, 1, 1);
		Spin_SetPos(IDC_SPIN_NEXT, 1);
		SetDlgItemInt(IDL_EDIT, 1);
		DisableItem(IDL_EDIT);
		DisableItem(IDC_SPIN_NEXT);
	}
	else{
		Spin_SetRange(IDC_SPIN_NEXT, 1, max);

		if(GetDlgItemInt(IDL_EDIT) > (unsigned int)max){
			Spin_SetPos(IDC_SPIN_NEXT, max);
			SetDlgItemInt(IDL_EDIT, max);
		}

		EnableItem(IDL_EDIT);
		EnableItem(IDC_SPIN_NEXT);
	}

	/* ボタンの設定 */
	if(max == 0){
		DisableItem(IDL_DELETE);
		DisableItem(IDL_MOVE);
		DisableItem(IDL_CONFIG);
	}
	else{
		EnableItem(IDL_DELETE);
		EnableItem(IDL_MOVE);
		EnableItem(IDL_CONFIG);
	}
}

/*-----------------------------------------------------------------------------
  ccwNode AdjustCWList()
  CCatalogWindowリストの調整
-----------------------------------------------------------------------------*/
CListPage::ccwNode CListPage::
AdjustCWList()
{
	ccwNode pn;

	for(pn = m_CWList.begin(); pn != m_CWList.end(); ){
		if(::IsWindow(pn->GetSafeHwnd()) == 0){
			pn = m_CWList.erase(pn);
		}
		else{
			pn++;
		}
	}

	CCatalogWindow  wnd;

	m_CWList.push_front(wnd);
	pn = m_CWList.begin();

	if(pn->InitWindow() == NULL){
		m_CWList.erase(pn);
		return NULL;
	}

	return pn;
}

/*-----------------------------------------------------------------------------
  int AskDocumentSave()
  リストの保存を訊ねる
-----------------------------------------------------------------------------*/

int CListPage::
AskDocumentSave()
{
	char  szBuf[_MAX_PATH];

	::wsprintf(szBuf, "ファイル\"%s\"への変更を保存しますか？",
						 m_FileData.GetName().c_str());

	return MessageBox(szBuf, DLG_CAPTION,	MB_YESNOCANCEL | MB_ICONEXCLAMATION);
}

/*-----------------------------------------------------------------------------
  int GetSaveFileName(char *, size_t)
-----------------------------------------------------------------------------*/
int CListPage::
GetSaveFileName(char *szBuf, size_t size)
{
	CMyFileDialog dlg;

	return dlg.GetSaveFileName(m_hWnd, STR_LIST, "名前を付けて保存", "lst",
														 szBuf, size);
}

/*-----------------------------------------------------------------------------
	Memo
	
	[ListView style]
	WS_VISIBLE
	WS_TABSTOP
	WS_CHILDWINDOW
	LVS_REPORT
	LVS_SHOWSELALWAYS
	
	WS_EX_LEFT
	WS_EX_LTRREADING
	WS_EX_RIGHTSCROLLBAR
	WS_EX_NOPARENTNOTIFY
	WS_EX_CLIENTEDGE
	
-----------------------------------------------------------------------------*/

/*
 * $Log: CListPage.cpp,v $
 * Revision 1.2  1999/07/10 05:12:58  MIYABI
 * CVS導入
 *
 */
