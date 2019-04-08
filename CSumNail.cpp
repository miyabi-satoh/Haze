/*=============================================================================

	FILE: CSumNail.cpp
 
=============================================================================*/

/*=============================================================================
	#include statements
=============================================================================*/

#include <stdinc.h>
#include <MyFunction.h>
#include "General.h"
#include "gSPIChain.h"
#include "CSumNail.h"
#include "res/resource.h"

/*=============================================================================
  �O���Q��
=============================================================================*/

extern CSPIChain g_tempSPI;
extern HFONT g_hFont;
extern HWND g_hListPageDialog;

/*=============================================================================
	#define statements
=============================================================================*/

#define MSG_INIT    "�����������ł�..."
#define MSG_ESCAPE  "ESC�L�[�Œ��~���܂�"
#define MSG_WAIT    "���΂炭���҂�������..."
#define MSG_READY   "���f�B"

#define CAT_DEFAULT_TITLE  "�T���l�C�� - "
#define CAT_CLASS   "Catalog"
#define VIEW_CLASS  "ViewWindow"
#define ORG_CLASS   "OrgWindow"

#define CAT_LIST    4000
#define CAT_STATUS  4001
#define CAT_PBAR    4002

#define CEIL(a) ((((a) - (int)(a)) > 0.0) ? ((a) + 1) : (a))

/*=============================================================================
  class CImgData
=============================================================================*/

/*=============================================================================
  static value
=============================================================================*/

cdn::Rect CImgData::m_rcMaxText(0, 0, 0, 0);

/*=============================================================================
  CImgData()
  �R���X�g���N�^
=============================================================================*/

CImgData::
CImgData()
		: m_bySelFlag(0), m_byType(0), m_rcImage(0, 0, 0, 0),
			m_rcText(0, 0, 0, 0)
{
}

/*=============================================================================
  ~CImgData()
  �f�X�g���N�^
=============================================================================*/

CImgData::
~CImgData()
{
}

/*=============================================================================
  void SetImage(CMyDIB&)
  CMyImage �I�u�W�F�N�g�Ɨ̈�̃Z�b�g
=============================================================================*/

void CImgData::
SetImage(CMyImage& cp)
{
	m_cpImage = cp;
	m_rcImage.SetRect(0, 0, cp.GetWidth(), cp.GetHeight());
}

/*=============================================================================
  void SetFileName(const char *, const char *)
  �t�@�C�����̃Z�b�g
=============================================================================*/

void CImgData::
SetFileName(const char *szFile, const char *szArc)
{
	m_szFile = (szFile && ::lstrlen(szFile)) ? szFile : "";
	m_szArc = (szArc && ::lstrlen(szArc)) ? szArc : "";
}

/*=============================================================================
  void GetImageRect(RECT*)
  �摜�\���̈���擾
=============================================================================*/

void CImgData::
GetImageRect(RECT* pRect)
{
	*pRect = (RECT)m_rcImage;
}

/*=============================================================================
	void SetImageRect(int, int, int, int)
=============================================================================*/
void CImgData::
SetImageRect(int x, int y, int w, int h)
{
	int ImgWidth, ImgHeight;

	ImgWidth = m_cpImage.GetWidth();
	ImgHeight = m_cpImage.GetHeight();

	m_rcImage.SetRect(0, 0, ImgWidth, ImgHeight);
	cdn::Offset o(x + (w - ImgWidth) / 2,
								y + (h - (ImgHeight + m_rcMaxText.bottom)) / 2);
	m_rcImage += o;
}

/*=============================================================================
  void GetTextRect(RECT*)
  �����\���̈���擾
=============================================================================*/

void CImgData::
GetTextRect(RECT* pRect)
{
	*pRect = (RECT)m_rcText;
}

/*=============================================================================
	void SetTextRect(HDC, int, int, int, int)
=============================================================================*/
void CImgData::
SetTextRect(HDC hdc, int x, int y, int w, int h)
{
	std::string szText;
	UINT uiFlag = DT_CENTER | DT_CALCRECT;
	RECT rc = (RECT)m_rcMaxText;

	/* ������ƕ`��t���O��ݒ� */
	if(m_byType == TP_LIST && m_szArc.length()){
		szText = "<" + m_szArc + ">:\n" + m_szFile;
		uiFlag |= DT_WORDBREAK;
	}
	else{
		szText = m_szFile;
		uiFlag |= DT_VCENTER;
	}

	/* �K�v�Œ���̗̈���擾 */
	::DrawText(hdc, szText.c_str(), -1, &rc, uiFlag);

	/* �\������ʒu�ɃV�t�g */
	cdn::Offset o(x + (w - rc.right) / 2, y + h - m_rcMaxText.bottom);
	m_rcText = rc;
	m_rcText += o;
}

/*=============================================================================
  std::string GetFileName()
  �t�@�C�������擾
=============================================================================*/

std::string CImgData::
GetFileName()
{
	std::string szRet;

	switch(m_byType){
    case TP_LIST:
			if(m_szArc.length()){
				szRet = "<" + m_szArc + ">:" + m_szFile;
			}
			else{
				szRet = m_szFile;
			}
			break;

    case TP_ARCHIVE:
			szRet = "<" + m_szArc + ">:" + m_szFile;
			break;

    case TP_FOLDER:
			szRet = m_szArc + m_szFile;
			break;
	}

	return szRet;
}

/*=============================================================================
  void Blt(HDC, RECT*)
  �f�[�^�̕`��
=============================================================================*/

void CImgData::
Blt(HDC hdc, RECT* pRect)
{
	UINT uiFlag = DT_CENTER;
	std::string szText;
	RECT rc;

	/* ������ƕ`��t���O��ݒ� */
	if(m_byType == TP_LIST && m_szArc.length()){
		szText = "<" + m_szArc + ">:\n" + m_szFile;
		uiFlag |= DT_WORDBREAK;
	}
	else{
		szText = m_szFile;
		uiFlag |= DT_VCENTER;
	}

	/* �ЂƂ܂��A���œh��Ԃ� */
	cdn::Rect rcOutLineImage, rcOutLineText;
	cdn::Offset o1(4, 4), o2(1, 1);
	cdn::Extent e1(8, 8), e2(2, 2);

	rcOutLineImage = (m_rcImage - o1) + e1;
	rcOutLineText = (m_rcText - o2) + e2;

	mf::FillRect1(hdc, &((RECT)rcOutLineImage), RGB(255, 255, 255));
	mf::FillRect1(hdc, &((RECT)rcOutLineText), RGB(255, 255, 255));

	/* �}�[�N������ꍇ�A�_���̒����`��`�悷�� */
	if(m_bySelFlag & TP_MARK){
		DrawFocusRect(hdc, &((RECT)rcOutLineImage));
		DrawFocusRect(hdc, &((RECT)rcOutLineText));
	}
	/* �I������Ă�����A�n�C���C�g��`�悷�� */
	if(m_bySelFlag & TP_SEL){
		cdn::Offset o3(3, 3);
		cdn::Extent e3(6, 6);

		rcOutLineImage = (m_rcImage - o3) + e3;
		rcOutLineText = m_rcText;

		mf::FillRect1(hdc, &((RECT)rcOutLineImage),
									::GetSysColor(COLOR_HIGHLIGHT));
		mf::FillRect1(hdc, &((RECT)rcOutLineText),
									::GetSysColor(COLOR_HIGHLIGHT));
	}

	/* �����F��ݒ� */
	if(m_bySelFlag & TP_SEL){
		::SetTextColor(hdc, ::GetSysColor(COLOR_HIGHLIGHTTEXT));
	}
	else{
		::SetTextColor(hdc, RGB(0, 0, 0));
	}

	/* �`�� */
	rc = (RECT)m_rcText;
	m_cpImage.Draw(hdc, m_rcImage.TopLeft(), 
								 cdn::Rect(0, 0, m_cpImage.GetWidth(), m_cpImage.GetHeight()));
	::DrawText(hdc, szText.c_str(), -1, &rc, uiFlag);
}

/*=============================================================================
  class COrgWindow
=============================================================================*/

/*=============================================================================
  int InitWindow(const char *, CMyImage&)
=============================================================================*/

int COrgWindow::
InitWindow(const char *szTitle, CMyImage& cp)
{
	m_iVScrollPos = m_iHScrollPos = 0;

    /* �E�B���h�E�쐬 */
	RegisterClassEx(ORG_CLASS, mf::LoadIcon(MIR(IDI_20CATALOG)),
									::LoadCursor(NULL, IDC_ARROW),
									(HBRUSH)(COLOR_WINDOW + 1), CS_DBLCLKS);

	if(!CreateEx(WS_EX_WINDOWEDGE, ORG_CLASS, szTitle,
							 WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
							 CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
							 NULL, NULL)){
		_Err0("CreateEx() ���s�H\n");
	}

	m_vw.init(m_hWnd, cp.GetWidth(), cp.GetHeight());
	cp.Draw(m_vw.GetDC(), cdn::Point(0, 0),
					cdn::Rect(0, 0, cp.GetWidth(), cp.GetHeight()));

	RECT            rc;
	WINDOWPLACEMENT wp;

	GetClientRect(&rc);

	wp.length = sizeof(WINDOWPLACEMENT);
	::GetWindowPlacement(m_hWnd, &wp);

	if(rc.bottom > cp.GetHeight()){
		rc.bottom = cp.GetHeight();
	}
	if(rc.right > cp.GetWidth()){
		rc.right = cp.GetWidth();
	}

	MoveWindow(wp.rcNormalPosition.left, wp.rcNormalPosition.top,
						 rc.right, rc.bottom, TRUE);

	ShowWindow(SW_SHOW);
	UpdateWindow();

	return TRUE;
}

/*-----------------------------------------------------------------------------
  BOOL OnClose()
-----------------------------------------------------------------------------*/

OVR_OnClose(COrgWindow)
{
	_Err0("COrgWindow::OnClose()\n");
	DestroyWindow();

	*plr = 0;
	return 1;
}

/*-----------------------------------------------------------------------------
  BOOL OnHScroll()
-----------------------------------------------------------------------------*/

OVR_OnHScroll(COrgWindow)
{
	SCROLLINFO  si;

	GetScrollInfo(SB_HORZ, &si);

	switch(nScrollCode){
    case SB_LINERIGHT:
			if(m_iHScrollPos == (int)(si.nMax - si.nPage)){
				return 0;
			}
			m_iHScrollPos += 1;
			break;
    case SB_LINELEFT:
			if(m_iHScrollPos == 0){
				return 0;
			}
			m_iHScrollPos -= 1;
			break;
    case SB_PAGERIGHT:
			if(m_iHScrollPos == (int)(si.nMax - si.nPage)){
				return 0;
			}
			m_iHScrollPos += 10;
			break;
    case SB_PAGELEFT:
			if(m_iHScrollPos == 0){
				return 0;
			}
			m_iHScrollPos -= 10;
			break;
    case SB_THUMBTRACK:
    case SB_THUMBPOSITION:
			if(m_iHScrollPos == si.nTrackPos){
				return 0;
			}
			m_iHScrollPos = si.nTrackPos;
			break;

    default:
			return 0;
	}

	if(m_iHScrollPos < 0){
		m_iHScrollPos = 0;
	}
	if(m_iHScrollPos > (int)(si.nMax - si.nPage)){
		m_iHScrollPos = si.nMax - si.nPage;
	}
	si.fMask = SIF_POS;
	si.nPos = m_iHScrollPos;
	SetScrollInfo(SB_HORZ, &si);

	InvalidateRect(NULL, FALSE);
	UpdateWindow();

	*plr = 0;
	return 1;
}

/*-----------------------------------------------------------------------------
  BOOL OnPaint()
-----------------------------------------------------------------------------*/

OVR_OnPaint(COrgWindow)
{
	PAINTSTRUCT ps;

	BeginPaint(&ps);

	m_vw.BitBlt(&ps, m_iHScrollPos, m_iVScrollPos);

	EndPaint(&ps);

	*plr = 0;
	return 1;
}

/*-----------------------------------------------------------------------------
  BOOL OnSize()
-----------------------------------------------------------------------------*/

OVR_OnSize(COrgWindow)
{
	SCROLLINFO  si1, si2;

	::ZeroMemory(&si1, sizeof(si1));
	::ZeroMemory(&si2, sizeof(si2));

	si1.cbSize = si2.cbSize = sizeof(si1);
	si1.fMask = si2.fMask = SIF_ALL;

	si1.nMax = m_vw.GetHeight();
	si1.nPage = nHeight;
	si1.nPos = m_iVScrollPos;

	si2.nMax = m_vw.GetWidth();
	si2.nPage = nWidth;
	si2.nPos = m_iHScrollPos;

	SetScrollInfo(SB_VERT, &si1);
	SetScrollInfo(SB_HORZ, &si2);

	*plr = 0;
	return 1;
}

/*-----------------------------------------------------------------------------
  BOOL OnVScroll()
-----------------------------------------------------------------------------*/

OVR_OnVScroll(COrgWindow)
{
	SCROLLINFO  si;

	GetScrollInfo(SB_VERT, &si);

	switch(nScrollCode){
    case SB_LINEDOWN:
			if(m_iVScrollPos == (int)(si.nMax - si.nPage)){
				return 0;
			}
			m_iVScrollPos += 1;
			break;
    case SB_LINEUP:
			if(m_iVScrollPos == 0){
				return 0;
			}
			m_iVScrollPos -= 1;
			break;
    case SB_PAGEDOWN:
			if(m_iVScrollPos == (int)(si.nMax - si.nPage)){
				return 0;
			}
			m_iVScrollPos += 10;
			break;
    case SB_PAGEUP:
			if(m_iVScrollPos == 0){
				return 0;
			}
			m_iVScrollPos -= 10;
			break;
    case SB_THUMBTRACK:
    case SB_THUMBPOSITION:
			if(m_iVScrollPos == si.nTrackPos){
				return 0;
			}
			m_iVScrollPos = si.nTrackPos;
			break;

    default:
			return 0;
	}

	if(m_iVScrollPos < 0){
		m_iVScrollPos = 0;
	}
	if(m_iVScrollPos > (int)(si.nMax - si.nPage)){
		m_iVScrollPos = si.nMax - si.nPage;
	}
	si.fMask = SIF_POS;
	si.nPos = m_iVScrollPos;
	SetScrollInfo(SB_VERT, &si);

	InvalidateRect(NULL, FALSE);
	UpdateWindow();

	*plr = 0;
	return 1;
}

/*=============================================================================
  class CWindowView
=============================================================================*/

/*=============================================================================
  CWindowView()
  �R���X�g���N�^
=============================================================================*/

CWindowView::
CWindowView() : CMyWnd()
{
	m_MaxHeight = m_MaxWidth = m_MaxStringHeight = m_OldValue = m_iScrollPos = 0;
	::SetRectEmpty(&m_rcText);
	m_bAcceptMsg = TRUE;
}

/*=============================================================================
  int InitWindow(HWND)
  �E�B���h�E�쐬
=============================================================================*/

int CWindowView::
InitWindow(HWND hWndMain)
{
    /* �E�B���h�E�쐬 */
	RegisterClassEx(VIEW_CLASS, (HICON)NULL, ::LoadCursor(NULL, IDC_ARROW),
									(HBRUSH)(COLOR_WINDOW + 1), CS_DBLCLKS);
	if(!CreateEx(WS_EX_WINDOWEDGE, VIEW_CLASS, "",
							 WS_VISIBLE | WS_OVERLAPPED | WS_CHILD | WS_VSCROLL,
							 0, 0, 0, 0, hWndMain, NULL)){
		_Err0("�E�B���h�E�쐬���s\n");
		return FALSE;
	}
	return TRUE;
}

/*=============================================================================
  void CalcMaxSize(PATHDATA_T&, BYTE)
  �C���[�W�̍ő�T�C�Y���v�Z
=============================================================================*/

void CWindowView::
CalcMaxSize(PATHDATA_T& data, BYTE byType)
{
	int   iHeight, iWidth;
	HDC   hdc;
	SIZE  size1, size2;

	size1.cx = size1.cy = size2.cx = size2.cy = 0;

	hdc = GetDC();

	::SelectObject(hdc, g_hFont);
	if(data.szFile.length()){
		GetTextExtentPoint32(hdc, data.szFile.c_str(), data.szFile.length(),
												 &size1);
	}
	if(byType == TP_LIST && data.szArc.length()){
		std::string szTemp = "<" + data.szArc + ">";
	
		GetTextExtentPoint32(hdc, szTemp.c_str(), szTemp.length(), &size2);
	}

	ReleaseDC(hdc);

	int temp = max(size1.cx, size2.cx);
	iWidth = 20 + max(80, temp);
	_Err3("size1.cx = %d, size2.cx = %d, iWidth = %d\n", size1.cx, size2.cx,
				iWidth);

	iHeight = 20 + 80 + size1.cy + size2.cy;

	if(m_MaxWidth < iWidth){
		m_MaxWidth = iWidth;
	}
	if(m_MaxHeight < iHeight){
		m_MaxHeight = iHeight;
	}
	if(m_MaxStringHeight < size1.cy + size2.cy + 10){
		m_MaxStringHeight = size1.cy + size2.cy + 10;
	}

	::SetRect(&m_rcText, 0, 0, m_MaxWidth, m_MaxStringHeight);
	m_ImgList.begin()->SetMaxTextRect(m_MaxWidth, m_MaxStringHeight);
}

/*=============================================================================
  int CreateView(int)
  ���z�E�B���h�E�̍쐬
=============================================================================*/

int CWindowView::
CreateView(int flag)
{
	RECT  rc;
	int   iSize, iNum, iWidth, iHeight;

	if((iSize = m_ImgList.size()) == 0){
		return FALSE;
	}
	else if(m_MaxWidth == 0){
		return FALSE;
	}

	GetClientRect(&rc);
	/* _Err4("rc(%d, %d, %d, %d)\n", rc.left, rc.top, rc.right, rc.bottom); */

	/* ���̖��� */
	iNum = rc.right / m_MaxWidth;
	/* �K�v�ȉ��z�E�B���h�E�̃T�C�Y */
	iWidth = m_MaxWidth * iNum;
	iHeight = m_MaxHeight * static_cast<int>(CEIL((double)iSize / (double)iNum));

	/* _Err3("iNum = %d, iWidth = %d, iHeight = %d\n", iNum, iWidth, iHeight); */

	if(iHeight == 0){
		iHeight = m_MaxHeight;
	}

	m_vw.init(m_hWnd, rc.right, iHeight);
	::SelectObject(m_vw.GetDC(), g_hFont);
	::SetBkMode(m_vw.GetDC(), TRANSPARENT);

	int x, y;
	itImg pn, end;

	pn = m_ImgList.begin();
	end = m_ImgList.end();

	for(y = 0; pn != end && y < iHeight ; y += m_MaxHeight){
		for(x = 0; pn != end && x < iWidth; x += m_MaxWidth, pn++){
			if(flag){
				/* ���W���Đݒ� */
				pn->SetImageRect(x, y, m_MaxWidth, m_MaxHeight);
				pn->SetTextRect(m_vw.GetDC(), x, y, m_MaxWidth, m_MaxHeight);
			}
			pn->Blt(m_vw.GetDC(), NULL);
		}
	}

	m_OldValue = iWidth;

	if(flag == 2){
		/* �X�N���[���o�[�͈̔͂�ύX */
		SCROLLINFO  si;

		GetScrollInfo(SB_VERT, &si);

		si.fMask = SIF_RANGE | SIF_POS;
		si.nMax = m_vw.GetHeight();
		if(m_iScrollPos > (int)(si.nMax - si.nPage)){
			si.nPos = m_iScrollPos = si.nMax - si.nPage;
		}

		SetScrollInfo(SB_VERT, &si);
	}

	return 1;
}

/*=============================================================================
  void AutoScroll()
  �T���l�C���ǉ����̎�����ʃX�N���[��
=============================================================================*/

void CWindowView::
AutoScroll()
{
	RECT rc;

	GetClientRect(&rc);

	if(rc.bottom < m_vw.GetHeight()){
		SCROLLINFO  si;

		::ZeroMemory(&si, sizeof(si));

		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		si.nMax = m_vw.GetHeight();
		si.nPage = rc.bottom;
		si.nPos = m_iScrollPos = m_vw.GetHeight() - rc.bottom;

		SetScrollInfo(SB_VERT, &si);
	}
}

/*=============================================================================
  void AddData(CMyImage&, const char *, const char *, BYTE)
  ���X�g�Ƀf�[�^��ǉ�
=============================================================================*/

void CWindowView::
AddData(CMyImage& cp, const char *szFile, const char *szArc, BYTE byType)
{
	CImgData  imgData;

	imgData.SetImage(cp);
	imgData.SetFileName(szFile, szArc);
	imgData.SetType(byType);

	m_ImgList.push_back(imgData);
}

/*-----------------------------------------------------------------------------
  CWindowView::itImg GetIndex(int, int, int* = NULL)
  �w����W�̉摜�ԍ��ƃm�[�h�|�C���^���擾
-----------------------------------------------------------------------------*/

CWindowView::itImg CWindowView::
GetIndex(int X, int Y, int *pRet)
{
	if(m_MaxWidth == 0 || m_MaxHeight == 0){
		return NULL;
	}

	int   ret, sx, sy;
	RECT  rc;

	GetClientRect(&rc);

	Y += m_iScrollPos;
	sx = X / m_MaxWidth;
	sy = Y / m_MaxHeight;

	ret = (rc.right / m_MaxWidth) * sy + sx;

/* _Err3("X = %d, Y = %d, ret = %d\n", X, Y, ret); */
	if(ret >= (int)m_ImgList.size()){
		return NULL;
	}

	RECT              rcImage, rcText;
	POINT             pt;
	itImg pn = m_ImgList.begin() + ret;

	pn->GetImageRect(&rcImage);
	pn->GetTextRect(&rcText);
/* 
	 _Err4("rcImage(%d, %d, %d, %d)\n", rcImage.left, rcImage.top, rcImage.right, rcImage.bottom);
	 _Err4("rcText(%d, %d, %d, %d)\n", rcText.left, rcText.top, rcText.right, rcText.bottom);
*/
	pt.x = X;
	pt.y = Y;

		/* �摜�̏� or �����̏�ɂ���H */
	if(::PtInRect(&rcImage, pt) != 0 || ::PtInRect(&rcText, pt) != 0){
		if(pRet){
			*pRet = ret;
		}
		return pn;
	}
	else{
		return NULL;
	}
}

/*-----------------------------------------------------------------------------
  void ClearFlag4All(BYTE = TP_SEL)
  �w�肵���t���O��S�ăN���A
-----------------------------------------------------------------------------*/

void CWindowView::
ClearFlag4All(BYTE flag)
{
	itImg pn = m_ImgList.begin();

	for(; pn != m_ImgList.end(); pn++){
		pn->Flag() &= ~flag;
	}
}

/*-----------------------------------------------------------------------------
  void SetFlag4All(BYTE = TP_SEL)
  �w�肵���t���O��S�ĂɃZ�b�g
-----------------------------------------------------------------------------*/

void CWindowView::
SetFlag4All(BYTE flag)
{
	itImg pn = m_ImgList.begin();

	for(; pn != m_ImgList.end(); pn++){
		pn->Flag() |= flag;
	}
}

/*-----------------------------------------------------------------------------
  itImg GetMarkNode(int* = NULL)
  �}�[�N�t���O�̕t�����m�[�h�ւ̃|�C���^���擾
-----------------------------------------------------------------------------*/

CWindowView::itImg CWindowView::
GetMarkNode(int* pRet)
{
	itImg pn = m_ImgList.begin();

	for(int i = 0; pn != m_ImgList.end(); pn++, i++){
		if(pn->Flag() & TP_MARK){
			if(pRet){
				*pRet = i;
			}
			return pn;
		}
	}

	return NULL;
}

/*-----------------------------------------------------------------------------
  itImg GetShiftMarkNode(int* = NULL)
  �}�[�N�t���O�̕t�����m�[�h�ւ̃|�C���^���擾
-----------------------------------------------------------------------------*/

CWindowView::itImg CWindowView::
GetShiftMarkNode(int* pRet)
{
	itImg pn = m_ImgList.begin();

	for(int i = 0; pn != m_ImgList.end(); pn++, i++){
		if(pn->Flag() & TP_SHIFT){
			if(pRet){
				*pRet = i;
			}
			return pn;
		}
	}

	return NULL;
}

/*-----------------------------------------------------------------------------
  int ExistSelected()
  �I�����ڂ����邩���ׂ�B
-----------------------------------------------------------------------------*/

int CWindowView::
ExistSelected()
{
	itImg pn = m_ImgList.begin();

	for(; pn != m_ImgList.end(); pn++){
		if(pn->Flag() & TP_SEL){
			return TRUE;
		}
	}

	return FALSE;
}

/*-----------------------------------------------------------------------------
  int GetNumSelected()
  �I�����ڂ̑������擾
-----------------------------------------------------------------------------*/

int CWindowView::
GetNumSelected()
{
	itImg pn = m_ImgList.begin();

	for(int i = 0; pn != m_ImgList.end(); pn++){
		if(pn->Flag() & TP_SEL){
			i++;
		}
	}

	return i;
}

/*-----------------------------------------------------------------------------
  void CreateOrgWindow(itImg&)
  ������\��
-----------------------------------------------------------------------------*/

void CWindowView::
CreateOrgWindow(itImg& pn)
{
	if(pn == NULL){
		return;
	}

	int       ret;
	HWND      hWndStatus, hWndProgress;
	char      msg[_MAX_PATH], msgOld[_MAX_PATH] = "";
	CSpiParam sp;

	Begin();

	hWndStatus = ::GetDlgItem(GetParent(), CAT_STATUS);
	hWndProgress = ::GetDlgItem(hWndStatus, CAT_PBAR);

	::SendMessage(hWndProgress, PBM_SETPOS, 0, 0);
	m_iPPos = 0;
	::SendMessage(hWndStatus, SB_GETTEXT, (WPARAM)0, (LPARAM)msgOld);

		/* �摜�W�J */
	{
		std::string szText;

		szText = pn->GetFileName();
		::wsprintf(msg, "%s��W�J���Ă��܂�...", szText.c_str());
		::SendMessage(hWndStatus, SB_SETTEXT, (WPARAM)0, (LPARAM)msg);

		if(szText[0] == '<'){
			::SendMessage(hWndProgress, PBM_SETRANGE32, 0, 100);
			sp.m_nType = TYPE_ARCHIVE;
			sp.m_szArc = pn->m_szArc;
			sp.m_szFile = pn->m_szFile;
		}
		else{
			::SendMessage(hWndProgress, PBM_SETRANGE32, 0, 50);
			sp.m_nType = TYPE_FILE;
			sp.m_szArc = "";
			sp.m_szFile = szText;
		}
		sp.m_lData = (long)this;
		sp.m_uiFlag = DISK;
		sp.m_fn = (FARPROC)spiOrg;
		ret = g_tempSPI.GetPicture(sp);
	}

	if(ret){
		std::list<COrgWindow>::iterator Org;

		for(Org = m_OrgList.begin(); Org != m_OrgList.end(); Org++){
			if(::IsWindow(Org->GetSafeHwnd()) == 0){
				break;
			}
		}

		if(Org == m_OrgList.end()){
			COrgWindow temp;

			m_OrgList.push_back(temp);
			Org = m_OrgList.end();
			Org--;
		}

		BITMAPINFO *pInfo = (BITMAPINFO*)::LocalLock(sp.m_hBInfo);
		BYTE *pBits = (BYTE*)::LocalLock(sp.m_hBm);
		CMyImage cp(pInfo, pBits);
		char szTitle[_MAX_PATH];

		{
			std::string szFileName;

			szFileName = pn->GetFileName();
			::wsprintf(szTitle, "%s(%d�~%d�~%dbit)", szFileName.c_str(),
								 cp.GetWidth(), cp.GetHeight(), cp.GetBitCount());
		}

		Org->InitWindow(szTitle, cp);
	}

	::SendMessage(hWndStatus, SB_SETTEXT, (WPARAM)0, (LPARAM)msgOld);
	::SendMessage(hWndProgress, PBM_SETPOS, 0, 0);
	End();
}

/*-----------------------------------------------------------------------------
  static int PASCAL spiOrg(int, int, int)
  �����\���p�摜�W�J���̃R�[���o�b�N
-----------------------------------------------------------------------------*/

int PASCAL CWindowView::
spiOrg(int iNum, int iDenom, long lData)
{
	CWindowView *wnd = (CWindowView*)lData;
	MSG msg;

	if(iDenom != 0){
		HWND hProgress = ::GetDlgItem(::GetDlgItem(wnd->GetParent(),CAT_STATUS),
																	CAT_PBAR);

		if(iNum == 0){
			wnd->m_iPPos = ::SendMessage(hProgress, PBM_GETPOS, 0, 0);
		}
		else{
			::SendMessage(hProgress, PBM_SETPOS,
										wnd->m_iPPos + ((50 * iNum) / iDenom), 0);
		}
	}

	if(::PeekMessage(&msg, (HWND)NULL, 0, 0, PM_REMOVE)){
		TransactMessage(&msg);
	}
	return 0;

}

/*-----------------------------------------------------------------------------
  void OnSelectAll()
  �S�đI��
-----------------------------------------------------------------------------*/

void CWindowView::
OnSelectAll()
{
		/* �t���O���Z�b�g */
	SetFlag4All();

		/* ���z�E�B���h�E�쐬 */
	CreateView(0);

		/* �ĕ`�� */
	InvalidateRect(NULL, FALSE);
	UpdateWindow();

	OnSetStatusText();
}

/*-----------------------------------------------------------------------------
  void OnTurnSelect()
  �I�����ڂ̔��]
-----------------------------------------------------------------------------*/

void CWindowView::
OnTurnSelect()
{
		/* �t���O�̔��] */
	itImg pn = m_ImgList.begin();

	for(; pn != m_ImgList.end(); pn++){
		if(pn->Flag() & TP_SEL){
			pn->Flag() &= ~TP_SEL;
		}
		else{
			pn->Flag() |= TP_SEL;
		}
	}

		/* ���z�E�B���h�E�쐬 */
	CreateView(0);

		/* �ĕ`�� */
	InvalidateRect(NULL, FALSE);
	UpdateWindow();

	OnSetStatusText();
}

/*-----------------------------------------------------------------------------
  void OnSetStatusText()
  �X�e�[�^�X�o�[�Ƀf�t�H���g�̕������\��
-----------------------------------------------------------------------------*/

void CWindowView::
OnSetStatusText()
{
	int   ret;
	char  msg[256];

		/* �X�e�[�^�X�o�[�ɕ\�����镶�����ݒ� */
	ret = GetNumSelected();
	if(ret == 0){
		::wsprintf(msg, "%s", MSG_READY);
	}
	else{
		::wsprintf(msg, "�I��%d�t�@�C��", ret);
	}

	::SendMessage(::GetDlgItem(GetParent(), CAT_STATUS), SB_SETTEXT,
								(WPARAM)0, (LPARAM)msg);
	End();
}

/*-----------------------------------------------------------------------------
  void OnOriginSize()
  ������\��
-----------------------------------------------------------------------------*/

void CWindowView::
OnOriginSize()
{
	itImg pn;

	for(pn = m_ImgList.begin(); pn != m_ImgList.end(); pn++){
		if(pn->Flag() & TP_SEL){
			CreateOrgWindow(pn);
		}
	}
}

/*-----------------------------------------------------------------------------
  void OnDeleteItem()
  �T���l�C�����獀�ڂ��폜
-----------------------------------------------------------------------------*/

void CWindowView::
OnDeleteItem()
{
	if(ExistSelected() == FALSE){
		return;
	}

	itImg pn;
	int nIndex;

	for(nIndex = 0, pn = m_ImgList.begin(); pn != m_ImgList.end(); nIndex++){
		if(pn->Flag() & TP_SEL){
			::SendMessage(g_hListPageDialog, WM_DELETELIST, (WPARAM)nIndex, 0L);
			pn = m_ImgList.erase(pn);
		}
		else{
			pn++;
		}
	}

	::SendMessage(g_hListPageDialog, WM_OPERATION_END, 0L, 0L);

	for(pn = m_ImgList.begin(); pn != m_ImgList.end(); pn++){
		int       iHeight, iWidth;
		SIZE      size1, size2;

		size1.cx = size1.cy = size2.cx = size2.cy = 0;

		if(pn->m_szFile.length()){
			GetTextExtentPoint32(m_vw.GetDC(), pn->m_szFile.c_str(),
													 pn->m_szFile.length(), &size1);
		}
		if(pn->GetType() == TP_LIST && pn->m_szArc.length()){
			std::string szTemp = "<" + pn->m_szArc + ">";

			GetTextExtentPoint32(m_vw.GetDC(), szTemp.c_str(), szTemp.length(),
													 &size2);
		}

		int temp = max(size1.cx, size2.cx);

		iWidth = 20 + max(80, temp);
		iHeight = 20 + 80 + size1.cy + size2.cy;

		if(m_MaxWidth < iWidth){
			m_MaxWidth = iWidth;
		}
		if(m_MaxHeight < iHeight){
			m_MaxHeight = iHeight;
		}
		if(m_MaxStringHeight < size1.cy + size2.cy + 10){
			m_MaxStringHeight = size1.cy + size2.cy + 10;
		}

		::SetRect(&m_rcText,
							0,
							0,
							m_MaxWidth,
							m_MaxStringHeight);
		m_ImgList.begin()->SetMaxTextRect(m_MaxWidth, m_MaxStringHeight);
	}

	CreateView(2);
	InvalidateRect(NULL, TRUE);
	UpdateWindow();
}

/*-----------------------------------------------------------------------------
  void OnAddItem()
  �T���l�C�����獀�ڂ�ǉ�
-----------------------------------------------------------------------------*/

void CWindowView::
OnAddItem()
{
	if(ExistSelected() == FALSE){
		return;
	}

	itImg pn;
	std::string         szFile;

	for(pn = m_ImgList.begin(); pn != m_ImgList.end(); pn++){
		if(pn->Flag() & TP_SEL){
			szFile = pn->GetFileName();
			::SendMessage(g_hListPageDialog, WM_ADDLIST,
										(WPARAM)(char *)szFile.c_str(), 0L);
		}
	}

	::SendMessage(g_hListPageDialog, WM_OPERATION_END, 0L, 0L);
}

/*-----------------------------------------------------------------------------
  OnCommand()
-----------------------------------------------------------------------------*/

OVR_OnCommand(CWindowView)
{
	if(m_bAcceptMsg == FALSE){
			/* �Ȃɂ��̏����������Ȃ��Ă���B*/
		return 0;
	}

	switch(wID){
		case ID_MNU3_SELECTALL:
		case IDA_CTRL_A:
			OnSelectAll();
			break;

		case ID_MNU3_SELCHANGE:
			OnTurnSelect();
			break;

		case ID_MNU3_ORG:
			OnOriginSize();
			break;

		case ID_MNU3_DEL:
		case IDA_DELETE:
			OnDeleteItem();
			break;

		case ID_MNU3_ADD:
		case IDA_INSERT:
			OnAddItem();
			break;

		default:
			return 0;
	}

	*plr = 0;
	return 0;
}

/*-----------------------------------------------------------------------------
  BOOL OnPaint()
-----------------------------------------------------------------------------*/

OVR_OnPaint(CWindowView)
{
	PAINTSTRUCT ps;

	BeginPaint(&ps);

	m_vw.BitBlt(&ps, 0, m_iScrollPos);

	EndPaint(&ps);

	*plr = 0;
	return 0;
}

/*-----------------------------------------------------------------------------
  BOOL OnVScroll()
-----------------------------------------------------------------------------*/

OVR_OnVScroll(CWindowView)
{
	SCROLLINFO  si;

	GetScrollInfo(SB_VERT, &si);

	switch(nScrollCode){
		case SB_LINEDOWN:
			if(m_iScrollPos == (int)(si.nMax - si.nPage)){
		    return 0;
			}
			m_iScrollPos += (m_MaxHeight / 3);
			break;
		case SB_LINEUP:
			if(m_iScrollPos == 0){
		    return 0;
			}
			m_iScrollPos -= (m_MaxHeight / 3);
			break;
		case SB_PAGEDOWN:
			if(m_iScrollPos == (int)(si.nMax - si.nPage)){
		    return 0;
			}
			m_iScrollPos += m_MaxHeight;
			break;
		case SB_PAGEUP:
			if(m_iScrollPos == 0){
		    return 0;
			}
			m_iScrollPos -= m_MaxHeight;
			break;
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			if(m_iScrollPos == si.nTrackPos){
		    return 0;
			}
			m_iScrollPos = si.nTrackPos;
			break;

		default:
			return 0;
	}

	if(m_iScrollPos < 0){
		m_iScrollPos = 0;
	}
	if(m_iScrollPos > (int)(si.nMax - si.nPage)){
		m_iScrollPos = si.nMax - si.nPage;
	}
	si.fMask = SIF_POS;
	si.nPos = m_iScrollPos;
	SetScrollInfo(SB_VERT, &si);

	InvalidateRect(NULL, FALSE);
	UpdateWindow();

	*plr = 0;
	return 0;
}

/*-----------------------------------------------------------------------------
  BOOL OnSize()
-----------------------------------------------------------------------------*/

OVR_OnSize(CWindowView)
{
	if(m_MaxWidth == 0 || m_MaxHeight == 0){
			/* �X�N���[���o�[�̐ݒ� */
		SCROLLINFO  si;

		::ZeroMemory(&si, sizeof(SCROLLINFO));

		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;

		SetScrollInfo(SB_VERT, &si);
		return 0;
	}

	if(nWidth > m_OldValue){
			/* �̈�g�� */
		if((nWidth - m_OldValue) > m_MaxWidth){
			CreateView(1);
			InvalidateRect(NULL, TRUE);
		}
	}
	else if(m_OldValue / m_MaxWidth  > nWidth / m_MaxWidth){
		CreateView(1);
		InvalidateRect(NULL, FALSE);
	}

		/* �X�N���[���o�[�̐ݒ� */
	SCROLLINFO  si;

	GetScrollInfo(SB_VERT, &si);

	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	si.nMax = m_vw.GetHeight();
	si.nPage = nHeight;

	if(m_iScrollPos > (int)(si.nMax - si.nPage)){
		m_iScrollPos = si.nMax - si.nPage;
		InvalidateRect(NULL, FALSE);
	}
	else{
		m_iScrollPos = 0;
	}

	if(m_iScrollPos < 0){
		m_iScrollPos = 0;
	}

	si.nPos = m_iScrollPos;
	SetScrollInfo(SB_VERT, &si);
/* _Err1("m_iScrollPos = %d\n", m_iScrollPos); */

	UpdateWindow();
	*plr = 0;
	return 0;
}

/*-----------------------------------------------------------------------------
  BOOL OnLButtonDown()
-----------------------------------------------------------------------------*/

OVR_OnLButtonDown(CWindowView)
{
	if(m_bAcceptMsg == FALSE){
			/* �Ȃɂ��̏����������Ȃ��Ă���B*/
		return 0;
	}

	itImg pMark, pNow;
	int               ret;

	pNow = GetIndex(X, Y, &ret);

	if(pNow == NULL){
			/* �Y�����鍀�ڂȂ����S�Ă̍��ڂ��I���� */
		ClearFlag4All();
	}
	else {
		pMark = GetMarkNode();
		if (nFlags & MK_CONTROL) {
		    /*
					CTRL �L�[��������Ă���ꍇ
					���̃C���f�b�N�X�̑I����Ԃ𔽓]���āA
					��t���O�A�V�t�g�t���O���Z�b�g����
				*/
			BYTE flag = pNow->Flag();

			if(flag & TP_SEL){
				flag = TP_MARK | TP_SHIFT;
			}
			else{
				flag = TP_SEL | TP_MARK | TP_SHIFT;
			}

			pNow->Flag() = flag;

		    /* �ȑO�̊�t���O���N���A */
			if(pMark){
				pMark->Flag() &= ~(TP_MARK | TP_SHIFT);
			}
		}
		else if(nFlags & MK_SHIFT){
				/* 
					 SHIFT�L�[��������Ă���ꍇ�A
					 SHIFT�}�[�N�ʒu����A�I�����ꂽ���ڂ܂ł�I����Ԃɂ��āA
					 ��t���O���X�V����B
					 SHIFT�}�[�N�͂��̂܂܁B
				*/
		    /* ��x�A�S�ẴA�C�e�����I���ɂ���B*/
			ClearFlag4All(TP_SEL);

				/* SHIFT�}�[�N�̕t�����m�[�h���擾 */
			itImg pSMark, pn;
			int               nIndex, nNum;

			pSMark = GetShiftMarkNode(&nIndex);
			if(pSMark == NULL){
				pSMark = m_ImgList.begin();
				pSMark->Flag() |= TP_SHIFT;
				nIndex = 0;
			}

		    /* �I���J�n�ʒu�ƏI���ʒu���Z�b�g */
			if(ret > nIndex){
				pn = pSMark;
				nNum = ret - nIndex + 1;
			}
			else{
				pn = pNow;
				nNum = nIndex - ret + 1;
			}

		    /* �I���t���O���Z�b�g */
			for(int i = 0; i < nNum; i++, pn++){
				pn->Flag() |= TP_SEL;
			}

		    /* �ȑO�̊�t���O���N���A���āA�Đݒ肷�� */
			if(pMark){
				pMark->Flag() &= ~TP_MARK;
			}
			pNow->Flag() |= TP_MARK;
		}
		else{
				/* �}�E�X�N���b�N�̂� */
		    /* �S�Ẵt���O���N���A */
			ClearFlag4All(TP_SEL | TP_MARK | TP_SHIFT);

		    /* �I���A�}�[�N�ASHIFT�t���O���Z�b�g */
			pNow->Flag() = TP_SEL | TP_MARK | TP_SHIFT;
		}
	}

	CreateView(0);
	InvalidateRect(NULL, FALSE);
	UpdateWindow();

	OnSetStatusText();

	*plr = 0;
	return 0;
}

/*-----------------------------------------------------------------------------
  BOOL OnLButtonDblClk()
-----------------------------------------------------------------------------*/

OVR_OnLButtonDblClk(CWindowView)
{
	if(m_bAcceptMsg == FALSE){
			/* �Ȃɂ��̏����������Ȃ��Ă���B*/
		return 0;
	}

	itImg pn = NULL;

	pn = GetIndex(X, Y);
	if(pn){
		CreateOrgWindow(pn);
	}

	*plr = 0;
	return 0;
}

/*-----------------------------------------------------------------------------
  BOOL OnContextMenu()
-----------------------------------------------------------------------------*/

OVR_OnContextMenu(CWindowView)
{
	if(m_bAcceptMsg == FALSE){
			/* �Ȃɂ��̏������s���Ă���B*/
		return 0;
	}

	HMENU     hMenu, hSub;

	hMenu = ::LoadMenu(GMH(), MIR(IDR_CATALOG));
	hSub = ::GetSubMenu(hMenu, 0);

	if(ExistSelected()){
		MENUITEMINFO  mii;
		int           nIndex = 2;

		::ZeroMemory(&mii, sizeof(mii));
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_TYPE | MIIM_STATE;
		mii.fType = MFT_SEPARATOR;
		mii.fState = MF_ENABLED;
		::InsertMenuItem(hSub, nIndex++, TRUE, &mii);

		mii.fMask = MIIM_TYPE | MIIM_STATE | MIIM_ID;
		mii.fType = MFT_STRING;
		mii.dwTypeData = "������\��\tDouble Click !";
		mii.wID = ID_MNU3_ORG;
		::InsertMenuItem(hSub, nIndex++, TRUE, &mii);

		mii.fMask = MIIM_TYPE | MIIM_STATE | MIIM_ID;
		mii.fType = MFT_STRING;
		if(m_ImgList.begin()->GetType() == TP_LIST){
			mii.dwTypeData = "�I�����ڂ�ؽĂ���폜\tDelete";
			mii.wID = ID_MNU3_DEL;
		}
		else{
			mii.dwTypeData = "�I�����ڂ�ؽĂɒǉ�\tInsert";
			mii.wID = ID_MNU3_ADD;
		}
		::InsertMenuItem(hSub, nIndex++, TRUE, &mii);
	}

	::TrackPopupMenu(hSub, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON,
									 X, Y, 0, m_hWnd, NULL);

	::DestroyMenu(hMenu);

	*plr = 0;
	return 0;
}

/*=============================================================================
  class CCatalogWindow
=============================================================================*/

/*-----------------------------------------------------------------------------
  static int PASCAL spiCallBack
  �摜�W�J���̃R�[���o�b�N�֐�
-----------------------------------------------------------------------------*/

int PASCAL CCatalogWindow::
spiCallBack(int iNum, int iDenom, long lData)
{
	CCatalogWindow  *wnd = (CCatalogWindow*)lData;
	MSG   msg;

	if(iDenom != 0){
		if(iNum == 0){
			wnd->m_ipgStart = wnd->m_PBar.SendMessage(PBM_GETPOS);
		}
		else{
			wnd->m_PBar.SetPos(wnd->m_ipgStart + ((4 * iNum) / iDenom));
		}
	}

	if(::PeekMessage(&msg, (HWND)NULL, 0, 0, PM_REMOVE)){
		TransactMessage(&msg);
	}
	return 0;
}

/*=============================================================================
  CCatalogWindow()
  �R���X�g���N�^
=============================================================================*/

CCatalogWindow::
CCatalogWindow() : CMyWnd()
{
	m_bStop = FALSE;
	m_ipgStart = 0;
}

/*=============================================================================
  void AddData(PATHDATA_T&)
  �f�[�^���X�g�̒ǉ�
=============================================================================*/

void CCatalogWindow::
AddData(PATHDATA_T& data)
{
	m_tempList.push_back(data);
}

/*=============================================================================
  HWND InitWindow()
  �E�B���h�E�쐬
=============================================================================*/

HWND CCatalogWindow::
InitWindow()
{
		/* �E�B���h�E�쐬 */
	RegisterClassEx(CAT_CLASS, mf::LoadIcon(MIR(IDI_20CATALOG)),
									::LoadCursor(NULL, IDC_ARROW),
									(HBRUSH)(COLOR_WINDOW + 1),
									CS_DBLCLKS);

	if(!CreateEx(WS_EX_WINDOWEDGE, CAT_CLASS, CAT_DEFAULT_TITLE,
							 WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
							 CW_USEDEFAULT, CW_USEDEFAULT,
							 CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL)){
		return NULL;
	}
	ShowWindow(SW_SHOW);
	UpdateWindow();
	m_Status.SendMessage(SB_SETTEXT, (WPARAM)2, (LPARAM)MSG_INIT);

	return m_hWnd;
}

/*=============================================================================
  void CreateSumNail(const char * szTitle, BYTE byType)
  �摜�ꗗ�𐶐�
=============================================================================*/

void CCatalogWindow::
CreateSumNail(const char *szTitle, BYTE byType)
{
	int       nCurrent, nMaxSize;

	nMaxSize = m_tempList.size();
	std::list<PATHDATA_T>::iterator pn;
	std::string szStr;

	szStr = CAT_DEFAULT_TITLE;
	szStr += szTitle;

	SetWindowText(szStr.c_str());

	/* �P���̍ő�T�C�Y���v�Z */
	for(pn = m_tempList.begin(); pn != m_tempList.end(); pn++){
		m_View.CalcMaxSize(*pn, byType);
	}

	/* �W�J */
	m_PBar.SetRange(0, 10 * nMaxSize);

	m_View.Begin();
	m_Status.SendMessage(SB_SETTEXT, (WPARAM)2, (LPARAM)MSG_ESCAPE);
	for(nCurrent = 0, pn = m_tempList.begin(); pn != m_tempList.end();
			nCurrent++)
	{
		/* �E�B���h�E�̑��݂��m�F */
		if(IsWindow() == 0){
			return;
		}
		else if(m_bStop == TRUE){
			break;
		}

		CSpiParam sp;
		std::string szFile, szText;

		/* �t�@�C�����𐶐�(<ARC>:FILE�`��) */
		if(pn->szArc.length()){
			if(byType == TP_FOLDER){
				szFile = pn->szArc;
			}
			else{
				szFile = "<" + pn->szArc + ">:";
			}
		}
		szFile += pn->szFile;
		szText = szFile + " ��W�J��...";

		/* �摜��W�J */
		m_Status.SendMessage(SB_SETTEXT, (WPARAM)0, (LPARAM)szText.c_str());
		m_ipgStart = 10 * nCurrent;
		if(m_ipgStart > m_PBar.SendMessage(PBM_GETPOS)){
			m_PBar.SetPos(m_ipgStart);
		}

		int ret;
		_Err1("%s\n", szText.c_str());
		if(szText[0] == '<'){
			sp.m_nType = TYPE_ARCHIVE;
			sp.m_szArc = pn->szArc;
			sp.m_szFile = pn->szFile;
		}
		else{
			sp.m_nType = TYPE_FILE;
			sp.m_szArc = "";
			sp.m_szFile = szFile;
		}

		sp.m_lData = (long)this;
		sp.m_uiFlag = DISK;
		sp.m_fn = (FARPROC)spiCallBack;
		ret = g_tempSPI.GetPicture(sp);

		if(ret){
			POINT pt;
			CMyImage cpTemp;

			pt.x = pt.y = 80;
			CreatePreview(cpTemp, &(sp.m_hBInfo), &(sp.m_hBm), &pt);

			m_View.AddData(cpTemp,
										 pn->szFile.c_str(),
										 pn->szArc.c_str(),
										 byType);
			m_View.CreateView(1);
			m_View.AutoScroll();

		    /* �E�B���h�E�̒��g���X�V */
			m_View.InvalidateRect(NULL, FALSE);
			m_View.UpdateWindow();
		}
		else{
			szFile += pn->szFile;
			szText = szFile + " �͓W�J�ł��܂���";
			m_Status.SendMessage(SB_SETTEXT, (WPARAM)0, (LPARAM)szText.c_str());
		}

		pn = m_tempList.erase(pn);
	}

	char  msg[50];

	::wsprintf(msg, "%d�̃t�@�C��", m_View.size());
	m_View.End();
	m_Status.SendMessage(SB_SETTEXT, (WPARAM)0, (LPARAM)MSG_READY);
	m_Status.SendMessage(SB_SETTEXT, (WPARAM)2, (LPARAM)msg);
	m_PBar.SetPos(0);

	m_tempList.clear();
	return;
}

/*-----------------------------------------------------------------------------
  void SendCommand(int)
  �r���[�E�B���h�E�ɃR�}���h���M
-----------------------------------------------------------------------------*/

void CCatalogWindow::
SendCommand(int id)
{
	m_View.SendMessage(WM_COMMAND, MAKELONG(id, 0), NULL);
}

/*-----------------------------------------------------------------------------
  BOOL OnCreate()
-----------------------------------------------------------------------------*/

OVR_OnCreate(CCatalogWindow)
{
	HWND  hWnd;

	m_View.InitWindow(m_hWnd);

	hWnd = ::CreateWindowEx(0, STATUSCLASSNAME, MSG_INIT,
													WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS |
													SBT_NOBORDERS,
													0, 0, 0, 0, m_hWnd, (HMENU)CAT_STATUS,
													GMH(), NULL);

	if(hWnd){
		m_Status.CMyControl::operator=(hWnd);
	}

	hWnd = ::CreateWindowEx(0, PROGRESS_CLASS, "",
													WS_VISIBLE | WS_CHILD | PBS_SMOOTH,
													0, 0, 0, 0, hWnd, (HMENU)CAT_PBAR,
													GMH(), NULL);
	if(hWnd){
		m_PBar.CMyControl::operator=(hWnd);
	}

	m_PBar.SetPos(0);

	*plr = 0;
	return 1;
}

/*-----------------------------------------------------------------------------
  BOOL OnClose()
-----------------------------------------------------------------------------*/

OVR_OnClose(CCatalogWindow)
{
	_Err0("CCatalogWindow::OnClose()\n");

	m_PBar.DestroyWindow();
	m_Status.DestroyWindow();
	m_View.DestroyWindow();

	DestroyWindow();

	*plr = 0;
	return 1;
}

/*-----------------------------------------------------------------------------
  OnCommand()
-----------------------------------------------------------------------------*/

OVR_OnCommand(CCatalogWindow)
{
	switch(wID){
		case IDA_ESC:
			m_bStop = TRUE;
			m_Status.SendMessage(SB_SETTEXT, (WPARAM)2, (LPARAM)MSG_WAIT);
			break;

		case IDA_CTRL_A:
		case IDA_DELETE:
		case IDA_INSERT:
			SendCommand(wID);
			break;

		default:
			return 0;
	}

	*plr = 0;
	return 1;
}

/*-----------------------------------------------------------------------------
  BOOL OnSize();
-----------------------------------------------------------------------------*/

OVR_OnSize(CCatalogWindow)
{
	if(fwSizeType == SIZE_MINIMIZED){
		return 0;
	}

	RECT  rc;
	int   SBSize;
	int   width[3];

	m_Status.GetWindowRect(&rc);
	SBSize = rc.bottom - rc.top;

	width[0] = 2 * (nWidth - 200) / 3;
	width[1] = width[0] + 200;
	width[2] = nWidth;
	m_Status.SendMessage(SB_SETPARTS, 3, (LPARAM)width);
	m_Status.SendMessage(WM_SIZE, fwSizeType, MAKELONG(nWidth, nHeight));

	m_PBar.MoveWindow(width[0] + 2, 2, 200 - 2, SBSize - 2, TRUE);

	m_View.MoveWindow(0, 0, nWidth, nHeight - SBSize, TRUE);

	*plr = 0;
	return 1;
}

/*
 * $Log: CSumNail.cpp,v $
 * Revision 1.2  1999/07/10 05:12:59  MIYABI
 * CVS����
 *
 */
