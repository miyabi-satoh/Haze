/*=============================================================================
	
	File:         General.h

	Description:  Defintions for global structure, functon, constant value

=============================================================================*/

#ifndef _GENERAL_H
#define _GENERAL_H

/*=============================================================================
	#include statements
=============================================================================*/

#include <CMyImage.h>
#include <CMySPI.h>
#include <string>

#pragma warning(disable : 4786)

/*=============================================================================
	#define statements
=============================================================================*/

#define HAZE_NAME   "�� Ver 1.00��6"
#define DLG_CAPTION HAZE_NAME" -�ݒ�-"

/* Timer ID */
#define TM_WALL 1
#define TM_ANIM 2

/* Message */
#define WM_CHANGE_CONFIGURE (WM_USER + 110)
#define WM_ADDLIST          (WM_USER + 120)
#define WM_DELETELIST       (WM_USER + 130)
#define WM_OPERATION_END    (WM_USER + 140)

/* File Dialog Extention */
#define FILE_EXE  "���s�\̧��(*.exe, *.com)\0*.exe;*.com\0"
#define FILE_SCR  "��ذݾ��ް(*.scr)\0*.scr\0"
#define FILE_ALL  "�S�Ă�̧��(*.*)\0*.*\0"
#define FILE_LIST "�ǎ�ؽ� (*.lst)\0*.lst\0"

/* Dialog(Control) String */
#define STR_TILE    "�����ɕ\��\0���ׂĕ\��\0\0"
#define STR_METHOD  "�o�^��\0�����\0\0"
#define STR_TIME    "�������\0�ǎ���ݼެ��N����\0��莞��\0\0"
#define STR_EXTEND  "���Ȃ�\0��ʂ����ς���\0��ʂ����ς���(�c����Œ�)\0���l�w��\0\0"
#define STR_UNIT    "pixels\0%\0\0"
#define STR_CLK     "�������Ȃ�\0��ذݾ��ް���N��\0Windows���I������\0���߭�����ċN������\0�w�肵��̧�ق����s����\0\0"
#define STR_TITLE   "̧�ق̑I��"
#define STR_TILE    "�����ɕ\��\0���ׂĕ\��\0\0"
#define STR_LIST    "�ǎ�ؽ� (*.lst)\0*.lst\0\0"

#define STR_FILTER_CLK2 FILE_EXE""FILE_SCR""FILE_ALL"\0"

#define MUDAI "�V�Kؽ�.lst"

/*=============================================================================
	struct PICTDATA_T
=============================================================================*/

typedef struct {
  int iWidth;
  int iHeight;
  int iColorMode;
}PICTDATA_T;

/*=============================================================================
	struct PATHDATA_T
=============================================================================*/

typedef struct {
  std::string szFile;
  std::string szArc;
}PATHDATA_T;

/*=============================================================================
	struct ArcResult
=============================================================================*/

typedef struct {
  unsigned char ret;
  unsigned long pos;
  unsigned long len;
}ArcResult;

/*=============================================================================
  global function prototype
=============================================================================*/

void GetINIPath(char *szBuf);
void WPPS(const char *szSection, const char *szKey, const char *szValue,
					const char *szPath);
void WPPS(const char *szSection, const char *szKey, int iValue,
					const char *szPath);
void GPPS(const char *szSection, const char *szKey, std::string& str,
          const char *szPath);
unsigned int GPPI(const char *szSection, const char *szKey, int iDefault,
									const char *szPath);
void NotifyChanged(HWND hWnd);
void TransactMessage(MSG* pMsg);
void CreatePreview(CMyImage& cpDst, HLOCAL* pHInfo, HLOCAL* pHBits, POINT* pt);
int PASCAL PlgCallBack(int nNum, int nDenom, long);
int BM_IsSupported(const char*, DWORD);
int BM_GetPictureInfo(const char*, long, unsigned int, SpiPictureInfo*);
int BM_GetPicture(const char*, long, unsigned int, HANDLE*, HANDLE*);
int GetArcResult(SpiArcFileInfo* pArc, std::string& szFile, ArcResult* pRes);

#endif /* _GENERAL_H */

/*
 * $Log: General.h,v $
 * Revision 1.2  1999/07/10 05:13:00  MIYABI
 * CVS����
 *
 */
