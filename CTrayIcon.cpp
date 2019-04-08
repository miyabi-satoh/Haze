/*=============================================================================

	FILE: CTrayIcon.cpp
 
=============================================================================*/

/*=============================================================================
	#include statements
=============================================================================*/

#include <stdinc.h>
#include <MyFunction.h>
#include <CMyImage.h>
#include "gSPIChain.h"
#include "CTrayIcon.h"
#include "ini.h"
#include "res/resource.h"

/*=============================================================================
	#define statements
=============================================================================*/

#define VERSION_NAME          "�ް�ޮݏ��#"HAZE_NAME
#define VERSION_MAKE          "Producted by �݂��"
#define TRAY_MSG_WAIT         "�ǎ����쐬��...���΂炭���҂�������"
#define TRAY_MSG_LISTNOTFOUND "���X�g\"%s\"���J���܂���"
#define TRAY_MSG_ISILLEGAL    "�t�@�C��\"%s\"�͐��������X�g�ł͂���܂���"
#define TRAY_MSG_SUSPEND      "�T�X�y���h��Ԃł�"
#define MSG_ALREADY           "���łɋN�����Ă��܂�"
#define MSG_NOTINFILE         "%s �ɂ͉摜̧�ق��o�^����Ă��܂���B"
#define MSG_STILLACTIVE       "�O��̕ύX�������I�����Ă��܂���B"
#define MSG_NOTFOUND          "%s ��������܂���"
#define MSG_ERR_READ          "���摜�̓ǂݍ��݂܂��͕ϊ��Ɏ��s���܂���"
#define MSG_ERR_CREATE        "�ǎ��p�̃t�@�C�����쐬�ł��܂���"
#define FILE_WALL             "%s%swall.bmp"
#define REG_DESKTOP           "Control Panel\\desktop"
#define REG_TILE              "TileWallpaper"

/*=============================================================================
  �O���Q��
=============================================================================*/

extern CSPIChain g_SPI;
extern HANDLE g_hSema;

/*-----------------------------------------------------------------------------
  local function
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  void Size_A(int*, int*)
  ��ʂ����ς��̃T�C�Y
-----------------------------------------------------------------------------*/
void
Size_A(int* pW, int* pH)
{
  *pW = GSM(SM_CXSCREEN);
  *pH = GSM(SM_CYSCREEN);
}

/*-----------------------------------------------------------------------------
  void Size_B(int*, int*, CMyImage&)
  �c����Œ�ŉ�ʂ����ς��̃T�C�Y
-----------------------------------------------------------------------------*/
void
Size_B(int* pW, int* pH, CMyImage& cpSrc)
{
  double Per, XPer, YPer;

  XPer = (double)GSM(SM_CXSCREEN) / (double)cpSrc.GetWidth();
  YPer = (double)GSM(SM_CYSCREEN) / (double)cpSrc.GetHeight();
  Per = (XPer > YPer) ? YPer : XPer;

  *pW = (int)(cpSrc.GetWidth() * Per);
  *pH = (int)(cpSrc.GetHeight() * Per);
}

/*-----------------------------------------------------------------------------
  void Size_C(int*, int *, CMyImage&, const CSetting*)
  �w�肳�ꂽ���l�ƒP�ʂɂ��T�C�Y
-----------------------------------------------------------------------------*/
void
Size_C(int* pW, int* pH, CMyImage& cpSrc, const CSetting* pSet)
{
  if(pSet->GetUnit()){
    *pW = cpSrc.GetWidth() * pSet->GetWidth() / 100;
    *pH = cpSrc.GetHeight() * pSet->GetHeight() / 100;
  }
  else{
    *pW = pSet->GetWidth();
    *pH = pSet->GetHeight();
  }
}

/*-----------------------------------------------------------------------------
  int WriteWallPaperFile(CMyImage&, std::string&)
  �t�@�C���ɏ����o��
-----------------------------------------------------------------------------*/
int
WriteWallPaperFile(CMyImage& cp, std::string& szFile)
{
  char szPath[_MAX_PATH], szDrive[_MAX_DRIVE], szDir[_MAX_DIR];

		/* haze.exe �̃p�X */
  ::GetModuleFileName(NULL, szPath, _MAX_PATH);
		/* �h���C�u���ƃf�B���N�g�������擾 */
  _splitpath(szPath, szDrive, szDir, NULL, NULL);
		/* �t�H���_�� + "wall.bmp" */
  ::wsprintf(szPath, FILE_WALL, szDrive, szDir);

  szFile = szPath;
  return cp.WriteFile(szPath);
}

/*-----------------------------------------------------------------------------
  void REG_ChangeWallPaper(const char *, int)
  ���W�X�g���̕ǎ��Ɋւ��鍀�ڂ�ύX
-----------------------------------------------------------------------------*/

void REG_ChangeWallPaper(const char *szFile, int nTile)
{
  char tile[2];
  HKEY hKey;

  tile[0] = '0' + nTile;
  tile[1] = '\0';

  hKey = mf::REG_CreateKey(HKEY_CURRENT_USER, REG_DESKTOP);
  mf::REG_SetValue(hKey, REG_TILE, tile);

  ::RegCloseKey(hKey);

  ::SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void *)szFile,
                         SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);
}

/*=============================================================================
  class CTrayIcon
=============================================================================*/

/*=============================================================================
  CTrayIcon()
  �R���X�g���N�^
=============================================================================*/

CTrayIcon::
CTrayIcon() : CMyTray()
{
  m_bMode = TRUE;
  m_hThread = NULL;
  m_bThreadFlag = TRUE;
}

/*=============================================================================
  int InitApplication()
  ������
=============================================================================*/

int CTrayIcon::
InitApplication()
{
  _Err0("CTrayIcon::InitApplication()\n");

  int id[6] = {
    IDI_00MAIN, IDI_01ANIM, IDI_02ANIM, IDI_03ANIM, IDI_04ANIM, IDI_05STOP
  };

		/* �A�C�R���̓ǂݍ��� */
  for(int i = 0; i < 6; i++){
    m_hIcon[i] = (HICON)::LoadImage(GMH(), MIR(id[i]), IMAGE_ICON, 16, 16, 0);
  }

  BOOL ret = CMyTray::InitApplication(HAZE_NAME, HAZE_NAME, MIR(IDR_WC_MAIN),
																			m_hIcon[0]);

  if(ret){
    for(int i = 0; i < 6; i++){
      ::DestroyIcon(m_hIcon[i]);
    }

    if(ret == 1){
      ::MessageBox(NULL, MSG_ALREADY, HAZE_NAME, MB_OK | MB_ICONEXCLAMATION);
    }
  }

		/* �^�C�}�[��ݒ� */
  m_lCount = 0;
  SetTimer(TM_WALL, 60000, NULL);

		/* �N�����ɕύX��������A�ǎ���ύX���� */
  CTimeData TimeData;

  ::WaitForSingleObject(g_hSema, INFINITE);
  TimeData.Read();
  ::ReleaseSemaphore(g_hSema, 1, NULL);

  if(TimeData.GetMode() == 1){
    OnChangePaper();
  }

		/* �f�X�N�g�b�v�̕ύX */
  Set_DeskTop();

  return ret;
}

/*-----------------------------------------------------------------------------
  BOOL OnCreate()
-----------------------------------------------------------------------------*/
OVR_OnCreate(CTrayIcon)
{
  _Err0("CTrayIcon::OnCreate()\n");
		/*  �g���C�A�C�R���̓o�^ */
  TrayMessage(NIM_ADD, 1, m_hIcon[0], HAZE_NAME);

  return CMyTray::OnCreate(plr);
}

/*-----------------------------------------------------------------------------
  BOOL OnClose()
-----------------------------------------------------------------------------*/
OVR_OnClose(CTrayIcon)
{
  _Err0("CTrayIcon::OnClose()\n");

  if(::IsWindow(m_MainDlg.GetSafeHwnd()) != 0){
    m_MainDlg.SendMessage(WM_CLOSE);
  }

  KillTimer(TM_WALL);

  TrayMessage(NIM_DELETE, 1, NULL, NULL);
  for(int i = 0; i < 6; i++){
    ::DestroyIcon(m_hIcon[i]);
  }

  m_bThreadFlag = FALSE;
  ::WaitForSingleObject(m_hThread, INFINITE);
  ::CloseHandle(m_hThread);

  return CMyTray::OnClose(plr);
}

/*-----------------------------------------------------------------------------
  BOOL OnTimer()
-----------------------------------------------------------------------------*/
OVR_OnTimer(CTrayIcon)
{
  if(!m_bMode){
    *plr = 0;
    return 1;
  }

  switch(wTimerID){
    case TM_WALL:
      OnWallTimer();
      break;

    case TM_ANIM:
      TrayAnimation();
      break;

    default:
			return 0;
  }

  *plr = 0;
  return 1;
}

/*-----------------------------------------------------------------------------
  void OnTaskTray()
-----------------------------------------------------------------------------*/

OVR_OnTaskTray(CTrayIcon)
{
  switch(mes){
    case WM_LBUTTONDBLCLK:
      OnTray2Click();
      break;

    default:
      CMyTray::OnTaskTray(mes);
      break;
  }
}

/*-----------------------------------------------------------------------------
  BOOL OnCommand()
-----------------------------------------------------------------------------*/
OVR_OnCommand(CTrayIcon)
{
  switch(wID){
    case ID_MNU_CONFIG:
      m_MainDlg.Create(m_hWnd);
      break;

    case ID_MNU_VERSION:
      ShellAbout(VERSION_NAME, VERSION_MAKE, MIR(IDI_00MAIN));
      break;

    case ID_MNU_STOP:
      OnSuspend();
      break;

    case ID_MNU_EXIT:
      OnClose(plr);
      break;

    case ID_MNU_SOON:
      OnChangePaper();
      break;
  }
  *plr = 0;
  return 1;
}

/*-----------------------------------------------------------------------------
  void OnWallTimer()
  �^�C�}�[�ł̏���
-----------------------------------------------------------------------------*/
void CTrayIcon::
OnWallTimer()
{
  _Err0("CTrayIcon::OnWallTimer()\n");

  CTimeData TimeData;

  ::WaitForSingleObject(g_hSema, INFINITE);
  TimeData.Read();
  ::ReleaseSemaphore(g_hSema, 1, NULL);

  _Err2("TimeData.bTime = %d, m_lCount = %d\n", TimeData.GetMode(), m_lCount);

	switch(TimeData.GetMode()){
		case 0:
		{
			SYSTEMTIME  sysT;

			::GetLocalTime(&sysT);

			if(TimeData.GetValue() != sysT.wDayOfWeek){
					/* ���t���ς���Ă��� */
				_Err2("This Time %02d:%02d\n", sysT.wHour, sysT.wMinute);
				OnChangePaper();

				TimeData.SetValue(sysT.wDayOfWeek);
				::WaitForSingleObject(g_hSema, INFINITE);
				TimeData.Write();
				::ReleaseSemaphore(g_hSema, 1, NULL);
			}
		}
		break;

		case 2:
			if(m_lCount != -1){
				m_lCount++;
				_Err2("Count = %d, uiValue = %d\n", m_lCount, TimeData.GetValue());
				if(m_lCount >= (LONG)TimeData.GetValue()){
					m_lCount = -1;
					OnChangePaper();
				}
#ifdef _DEBUG
				SYSTEMTIME  sysT;
				::GetLocalTime(&sysT);
				_Err2("This Time %02d:%02d\n", sysT.wHour, sysT.wMinute);
#endif /* _DEBUG */
			}
			break;
	}
}

/*-----------------------------------------------------------------------------
  void TrayAnimation()
  �g���C�A�C�R���̃A�j���[�V����
-----------------------------------------------------------------------------*/

void CTrayIcon::
TrayAnimation()
{
	DWORD dw = ::WaitForSingleObject(m_hThread, 1);
	if(dw  == WAIT_TIMEOUT){
		m_bIconIndex++;
		if(m_bIconIndex > 4){
			m_bIconIndex = 0;
		}

		TrayMessage(NIM_MODIFY, 1, m_hIcon[m_bIconIndex], TRAY_MSG_WAIT);
	}
	else if(dw == WAIT_OBJECT_0){
		::CloseHandle(m_hThread);
		m_hThread = NULL;
		KillTimer(TM_ANIM);
		m_lCount = 0;
		TrayMessage(NIM_MODIFY, 1, m_hIcon[0], HAZE_NAME);
	}
}

/*-----------------------------------------------------------------------------
  void OnChangePaper()
  �ǎ��ύX�̊J�n
-----------------------------------------------------------------------------*/

void CTrayIcon::
OnChangePaper()
{
  _Err0("CTrayIcon::OnChangePaper()\n");

		/* �T�X�y���h��������ύX���Ȃ� */
  if(m_bMode == FALSE){
    return;
  }

  CListData ListData;
  std::string szFile;
  CDocument doc;
  int nMax;
  int nCurrentIndex;

  ::WaitForSingleObject(g_hSema, INFINITE);
  ListData.Read();
  ::ReleaseSemaphore(g_hSema, 1, NULL);
  szFile = ListData.GetLastList();

		/* �t�@�C�����̊m�F */
  if(szFile.length() == 0){
    return;
  }

  if(doc.Open(szFile.c_str()) == 0){
    return;
  }

  if((nMax = doc.size()) == 0){
    return;
  }

		/* �ǎ��ɂ���C���f�b�N�X���擾 */
  if(ListData.GetDrawPattern() == 0){
			/* �o�^�� */
    if(ListData.GetStart() > nMax){
      nCurrentIndex = 1;
    }
    else{
      nCurrentIndex = ListData.GetStart();
    }
  }
  else{
			/* �����_�� */
    srand((unsigned)time(NULL));
    do{
      nCurrentIndex = 1 + rand() % nMax;
    }while(nCurrentIndex == ListData.GetStart());
  }

		/* �f�[�^�ǂݍ��� */
  CDocument::fData it = doc.begin();
  m_data = *(it + (nCurrentIndex - 1));

		/* ����J�n�ʒu���X�V */
  ListData.SetStart((nMax == nCurrentIndex) ? 1 : nCurrentIndex + 1);
  _Err3("�ő� = %d, ���� = %d, �� = %d\n", nMax, nCurrentIndex,
				ListData.GetStart());
  ::WaitForSingleObject(g_hSema, INFINITE);
  ListData.Write();
  ::ReleaseSemaphore(g_hSema, 1, NULL);

		/* �X���b�h�J�n */
  DWORD dw;
  if(m_hThread && ::GetExitCodeThread(m_hThread, &dw) && dw == STILL_ACTIVE){
    MBox(MSG_STILLACTIVE);
    return;
  }

  m_hThread = mf::CreateThread((LPTSR)SetWallPaper, (LPVOID)this);
//  ::SetThreadPriority(m_hThread, THREAD_PRIORITY_LOWEST);
}

/*-----------------------------------------------------------------------------
  int OnSuspend()
  �T�X�y���h��Ԃֈڍs
-----------------------------------------------------------------------------*/

int CTrayIcon::
OnSuspend()
{
  _Err0("CTrayIcon::on_menu_stop()\n");

  UINT  flag = MF_BYCOMMAND;

  if(m_bMode == TRUE){
    TrayMessage(NIM_MODIFY, 1, m_hIcon[5], TRAY_MSG_SUSPEND);
    flag |= MF_CHECKED;
  }
  else{
    TrayMessage(NIM_MODIFY, 1, m_hIcon[0], HAZE_NAME);
    flag |= MF_UNCHECKED;
  }

  m_bMode = !m_bMode;
  ::CheckMenuItem(m_hMenu, ID_MNU_STOP, flag);

  return 1;
}

/*-----------------------------------------------------------------------------
  void OnTray2Click()
  �g���C�A�C�R�����_�u���N���b�N�����Ƃ��̏���
-----------------------------------------------------------------------------*/

void CTrayIcon::
OnTray2Click()
{
  _Err0("CTrayIcon::OnTray2Click()\n");

  CCommonData CommonData;

  ::WaitForSingleObject(g_hSema, INFINITE);
  CommonData.Read();
  ::ReleaseSemaphore(g_hSema, 1, NULL);

  switch(CommonData.Get2Click()){
    case 1:
    {
      POINT pt;
      ::GetCursorPos(&pt);
      SendMessage(WM_SYSCOMMAND, SC_SCREENSAVE, MAKELONG(pt.x, pt.y));
    }
    break;

    case 2:
      ::ExitWindowsEx(EWX_SHUTDOWN, 0);
      break;

    case 3:
      ::ExitWindowsEx(EWX_REBOOT, 0);
      break;

    case 4:
    {
      std::string str = CommonData.GetAppPath();
      HANDLE hProcess;

      if(str.length()){
        char szExt[_MAX_EXT];

        _splitpath(str.c_str(), NULL, NULL, NULL, szExt);
        if(::lstrcmp(szExt, "scr")){
          str += " -s";
        }

        hProcess = mf::CreateProcess(NULL, (char*)str.c_str());

        if(!hProcess){
          char msg[256];

          ::wsprintf(msg, MSG_NOTFOUND, str.c_str());
          MBox(msg);
        }
        else{
          ::WaitForInputIdle(hProcess, INFINITE);
          ::CloseHandle(hProcess);
        }
      }
    }
    break;
  }
}

/*----------------------------------------------------------------------------
  int GetSourcePicture(CMyImage&)
  �ǎ��̌��摜���擾
-----------------------------------------------------------------------------*/

int CTrayIcon::
GetSourcePicture(CMyImage& cpDst)
{
  std::string szFile;
  CSpiParam sp;
  int ret = 0;

	sp.m_nType = TYPE_UNKNOWN;
	sp.m_fn = (FARPROC)PlgCallBack;
  /*
		�v���O�C���`�F�C���ւ̑�������b�N���邽�߁A
		�A�N�Z�X�����o��܂őҋ@
	*/
  ::WaitForSingleObject(g_hSema, INFINITE);
	/* �v���O�C���̏����� */
  g_SPI.InitializedSPIChain();
  g_SPI.LoadAll();
	/* �摜��W�J */
	ret = m_data.GetPicture(g_SPI, sp);
	/* �v���O�C������� */
  g_SPI.FreeAll();
	/* �Z�}�t�H�̉�� */
  ::ReleaseSemaphore(g_hSema, 1, NULL);

  if(ret == 0){
    MBox(MSG_ERR_READ);
    _Err0("CTrayIcon::GetSourcePicture() --- return 0\n\n");
    return 0;
  }
  else{
    BITMAPINFO *pBInfo = (BITMAPINFO*)::LocalLock(sp.m_hBInfo);
    BYTE *pData = (BYTE*)::LocalLock(sp.m_hBm);

    cpDst.Create(pBInfo, pData);
  }

  _Err1("CTrayIcon::GetSourcePicture() --- return %d\n\n", ret);
  return ret;
}

/*-----------------------------------------------------------------------------
  DWORD WINAPI SetWallPaper(LPVOID)
  �ǎ��ύX�p�̃X���b�h
-----------------------------------------------------------------------------*/

DWORD WINAPI CTrayIcon::
SetWallPaper(LPVOID pdata)
{
  _Err1("Thread Start : %d\n", ::GetTickCount());

  CSetting Setting;
  CTrayIcon *App = (CTrayIcon*)pdata;
  CMyImage  cpSrc, cpDst;
  int       iDesW, iDesH, iSrcW, iSrcH;
  std::string szFile;

		/* �g���C�A�C�R���̃A�j���[�V�������J�n */
  App->m_bIconIndex = 1;
  App->SetTimer(TM_ANIM, 1000, NULL);

		/* �ݒ���擾 */
  if(App->m_data.GetFlag() != 0){
    Setting = App->m_data.GetSetting();
  }
  else{
			/* �ʐݒ肪���� */
    ::WaitForSingleObject(g_hSema, INFINITE);
    Setting.Read();
    ::ReleaseSemaphore(g_hSema, 1, NULL);
  }

		/* ��������ɉ摜��W�J */
  if(App->GetSourcePicture(cpSrc) == 0){
    goto END_THREAD;
  }

		/* �쐬����T�C�Y���擾 */
  iSrcW = cpSrc.GetWidth();
  iSrcH = cpSrc.GetHeight();
  switch(Setting.GetExtend()){
    case 1: /* ��ʂ����ς� */
      Size_A(&iDesW, &iDesH);
      break;

    case 2: /* ��ʂ����ς��i�c����Œ�j */
      Size_B(&iDesW, &iDesH, cpSrc);
      break;

    case 3: /* ���l�w�� */
      Size_C(&iDesW, &iDesH, cpSrc, &Setting);
      break;

    default:  /* ���̂܂� */
      iDesW = iSrcW = cpSrc.GetWidth();
      iDesH = iSrcH = cpSrc.GetHeight();
      break;
  }

		/*  �ǎ��p�r�b�g�}�b�v�쐬 */
  if(iSrcW == iDesW && iSrcH == iDesH){
    cpDst = cpSrc;
  }
  else{
    cdn::Rect dest_rect(0, 0, iDesW, iDesH);
		cdn::Rect src_rect(0, 0, iSrcW, iSrcH);

    _out1("StretchBits �J�n --- %d\n", ::GetTickCount());
    if(iSrcW > iDesW && iSrcH > iDesH){
      cpDst = cpSrc;
      cpDst.Resize(iDesW, iDesH);
			cpDst.StretchCopyRect(cpSrc, dest_rect, src_rect,
														ScaleCopyPixel((STRETCH)StretchProgress, (long)pdata));
    }
    else{
      cpDst.Create(iDesW, iDesH, 24);
      cpSrc.to24bpp();
			cpDst.StretchCopyRect(cpSrc, dest_rect, src_rect,
														BiscaleCopyPixel((STRETCH)StretchProgress, (long)pdata));
    }
    _out1("StretchBits �I�� --- %d\n\n", ::GetTickCount());
  }

  if(App->m_bThreadFlag == FALSE){
    goto END_THREAD;
  }

		/* �t�@�C���ɏ������� */
  if(WriteWallPaperFile(cpDst, szFile) == FALSE){
    App->MBox(MSG_ERR_CREATE);
    goto END_THREAD;
  }

		/*  �ǎ��ύX�A���W�X�g���X�V */
  REG_ChangeWallPaper(szFile.c_str(), Setting.GetTilePattern());

		/* �f�X�N�g�b�v�ĕ`�� */
  App->Set_DeskTop();

END_THREAD:

//  App->KillTimer(TM_ANIM);
	cpDst.clear();
	cpSrc.clear();
  _Err1("Thread Exit : %d\n", ::GetTickCount());
  ::ExitThread(1);
  return 0;
}

/*-----------------------------------------------------------------------------
  static int CALLBACK StretchProgress(int, int, long)
  �g�又�����̃R�[���o�b�N�֐�
-----------------------------------------------------------------------------*/

int CALLBACK CTrayIcon::
StretchProgress(int n, int end, long lData)
{
  CTrayIcon *obj = (CTrayIcon*)lData;

  if(obj->m_bThreadFlag == FALSE){
    return 0;
  }
  return 1;
}

/*-----------------------------------------------------------------------------
  int Set_DeskTop()
  �f�X�N�g�b�v�Ɋւ���ݒ�𔽉f
-----------------------------------------------------------------------------*/
int CTrayIcon::
Set_DeskTop()
{
  CCommonData CommonData;
  HWND sys = NULL;

  ::WaitForSingleObject(g_hSema, INFINITE);
  CommonData.Read();
  ::ReleaseSemaphore(g_hSema, 1, NULL);

  sys = mf::GetDeskTopListView();
  if(sys && CommonData.GetTrans() == 1){
    CMyListView   lDesk(sys);

		lDesk.SetTextBkColor(CLR_NONE);
		lDesk.SetTextColor((COLORREF)CommonData.GetTextColor());
		lDesk.GetBkColor();
		::InvalidateRect(0, NULL, FALSE);
		::UpdateWindow(0);

    return 1;
  }

  return 0;
}

/*-----------------------------------------------------------------------------
  int MBox(const char *)
  ���b�Z�[�W�{�b�N�X
-----------------------------------------------------------------------------*/

int CTrayIcon::
MBox(const char *lpszText)
{
  return CMyTray::MessageBox(lpszText, HAZE_NAME);
}

/*
 * $Log: CTrayIcon.cpp,v $
 * Revision 1.2  1999/07/10 05:13:00  MIYABI
 * CVS����
 *
 */
