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

#define HAZE_NAME   "霞 Ver 1.00β6"
#define DLG_CAPTION HAZE_NAME" -設定-"

/* Timer ID */
#define TM_WALL 1
#define TM_ANIM 2

/* Message */
#define WM_CHANGE_CONFIGURE (WM_USER + 110)
#define WM_ADDLIST          (WM_USER + 120)
#define WM_DELETELIST       (WM_USER + 130)
#define WM_OPERATION_END    (WM_USER + 140)

/* File Dialog Extention */
#define FILE_EXE  "実行可能ﾌｧｲﾙ(*.exe, *.com)\0*.exe;*.com\0"
#define FILE_SCR  "ｽｸﾘｰﾝｾｰﾊﾞｰ(*.scr)\0*.scr\0"
#define FILE_ALL  "全てのﾌｧｲﾙ(*.*)\0*.*\0"
#define FILE_LIST "壁紙ﾘｽﾄ (*.lst)\0*.lst\0"

/* Dialog(Control) String */
#define STR_TILE    "中央に表示\0並べて表示\0\0"
#define STR_METHOD  "登録順\0ﾗﾝﾀﾞﾑ\0\0"
#define STR_TIME    "一日おき\0壁紙ﾁｪﾝｼﾞｬｰ起動時\0一定時間\0\0"
#define STR_EXTEND  "しない\0画面いっぱいに\0画面いっぱいに(縦横比固定)\0数値指定\0\0"
#define STR_UNIT    "pixels\0%\0\0"
#define STR_CLK     "何もしない\0ｽｸﾘｰﾝｾｰﾊﾞｰを起動\0Windowsを終了する\0ｺﾝﾋﾟｭｰﾀを再起動する\0指定したﾌｧｲﾙを実行する\0\0"
#define STR_TITLE   "ﾌｧｲﾙの選択"
#define STR_TILE    "中央に表示\0並べて表示\0\0"
#define STR_LIST    "壁紙ﾘｽﾄ (*.lst)\0*.lst\0\0"

#define STR_FILTER_CLK2 FILE_EXE""FILE_SCR""FILE_ALL"\0"

#define MUDAI "新規ﾘｽﾄ.lst"

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
 * CVS導入
 *
 */
