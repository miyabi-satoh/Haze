/*=============================================================================

	FILE: General.cpp
 
=============================================================================*/

/*=============================================================================
	#include statements
=============================================================================*/

#include <stdinc.h>
#include <cmath>
#include <MyFunction.h>
#include "General.h"

/*=============================================================================
	#define statements
=============================================================================*/

#define SIZE_BIH sizeof(BITMAPINFOHEADER)
#define SIZE_BFH sizeof(BITMAPFILEHEADER)
#define BUF_SIZE 256

/*=============================================================================
  外部参照
=============================================================================*/

extern HACCEL g_hAccel;

/*-----------------------------------------------------------------------------
  local functions prototype
-----------------------------------------------------------------------------*/

int CheckFileHeader(BITMAPFILEHEADER *);
int CheckInfoHeader(BITMAPINFOHEADER *);

/*=============================================================================
  void GetINIPath(char *)
  iniファイルのパスを取得
=============================================================================*/
void 
GetINIPath(char *szBuf)
{
	char szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szPath[_MAX_PATH];

	::GetModuleFileName(NULL, szPath, _MAX_PATH);
	_splitpath(szPath, szDrive, szDir, NULL, NULL);
	::wsprintf(szBuf, "%s%shaze.ini", szDrive, szDir);
}

/*=============================================================================
	void WPPS(const char *, const char *, const char *, const char *)
=============================================================================*/
void WPPS(const char *szSection, const char *szKey, const char *szValue,
					const char *szPath)
{
	::WritePrivateProfileString(szSection, szKey, szValue, szPath);
}

/*=============================================================================
	void WPPS(const char *, const char *, int, const char *)
=============================================================================*/
void WPPS(const char *szSection, const char *szKey, int iValue,
					const char *szPath)
{
	char szTemp[BUF_SIZE];
	::wsprintf(szTemp, "%d", iValue);
	::WritePrivateProfileString(szSection, szKey, szTemp, szPath);
}

/*=============================================================================
	void GPPS(const char *, const char *, std::string&, const char *)
=============================================================================*/
void GPPS(const char *szSection, const char *szKey, std::string& str,
          const char *szPath)
{
	char szTemp[BUF_SIZE + 1];
	::GetPrivateProfileString(szSection, szKey, "", szTemp, BUF_SIZE, szPath);
	str = szTemp;
}

/*=============================================================================
	unsigned int GPPI(const char *, const char *, int, const char *)
=============================================================================*/
unsigned int GPPI(const char *szSection, const char *szKey, int iDefault,
									const char *szPath)
{
	return ::GetPrivateProfileInt(szSection, szKey, iDefault, szPath);
}

/*=============================================================================
  void  NotifyChanged(HWND)
  メインダイアログに変更を通知
=============================================================================*/

void
NotifyChanged(HWND hWnd)
{
  ::SendMessage(hWnd, WM_CHANGE_CONFIGURE, 0L, 0L);
}

/*=============================================================================
  void TransactMessage(MSG *)
  メッセージループ
=============================================================================*/

void
TransactMessage(MSG *pMsg)
{
  if(g_hAccel &&
     ::TranslateAccelerator(::GetForegroundWindow(), g_hAccel, pMsg)){
    return;
  }

  if(::IsDialogMessage(::GetForegroundWindow(), pMsg)){
    return;
  }

  ::TranslateMessage(pMsg);
  ::DispatchMessage(pMsg);
}

/*=============================================================================
  void CreatePreview(CMyDIB&, HLOCAL *, HLOCAL *, POINT *)
  メモリハンドルから縮小画像を作成
=============================================================================*/

void
CreatePreview(CMyImage& cpDst, HLOCAL *pHInfo, HLOCAL *pHBits, POINT *pt)
{
  BITMAPINFO *pInfo;
  BYTE *pBits;
  CMyImage temp;
  double PerX, PerY;
  int CpyWidth, CpyHeight;

  pInfo = (BITMAPINFO *)::LocalLock(*pHInfo);
  pBits = (BYTE *)::LocalLock(*pHBits);

  temp.Create(pInfo, pBits);

  PerX = double(pt->x) / double(temp.GetWidth());
  PerY = double(pt->y) / double(temp.GetHeight());

  pt->x = temp.GetWidth();
  pt->y = temp.GetHeight();
  CpyWidth = int(pt->x * min(PerX, PerY));
  CpyHeight = int(pt->y * min(PerX, PerY));

  cpDst = temp;
  cpDst.Resize(CpyWidth, CpyHeight);
	cpDst.StretchCopyRect(temp, cdn::Rect(0, 0, CpyWidth, CpyHeight),
												cdn::Rect(0, 0, temp.GetWidth(), temp.GetHeight()),
												ScaleCopyPixel());

  ::LocalUnlock(*pHInfo);
  ::LocalUnlock(*pHBits);
}

/*=============================================================================
  ビットマップ用ユーティリティ

  int   BM_IsSupported(STR, DWORD)
  int   BM_GetPictureInfo(STR, long, unsigned int, SpiPictureInfo *)
  int   BM_GetPicture(STR, long, unsigned int, HANDLE *, HANDLE *)
  BOOL  CheckInfoHeader(BITMAPINFOHEADER *)
  BOOL  CheckFileHeader(BITMAPFILEHEADER *)
=============================================================================*/

/*=============================================================================
  int BM_IsSupported(const char *, DWORD)
  ビットマップかどうか調べる
=============================================================================*/

int
BM_IsSupported(const char *, DWORD dw)
{
  BITMAPINFOHEADER bih;
  BITMAPFILEHEADER bfh;

  if(HIWORD(dw) == 0){
    /* File Handle */
    HANDLE hFile = (HANDLE)dw;
    DWORD readed;

    ::SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    ::ReadFile(hFile, &bfh, SIZE_BFH, &readed, NULL);
    ::ReadFile(hFile, &bih, SIZE_BIH, &readed, NULL);
  }
  else{
    char *p = (char *)dw;

    ::CopyMemory(&bfh, p,  SIZE_BFH);
    ::CopyMemory(&bih, p + SIZE_BFH, SIZE_BIH);
  }

  return CheckFileHeader(&bfh) & CheckInfoHeader(&bih);
}

/*=============================================================================
  int BM_GetPictureInfo(const char *, long, unsigned int, SpiPictureInfo *)
  画像情報を取得
=============================================================================*/

int
BM_GetPictureInfo(const char *buf, long len, unsigned int flag,
									SpiPictureInfo *lpInfo)
{
  BITMAPFILEHEADER  bfh;
  BITMAPINFOHEADER  bih;

  if((flag & MEM) == MEM){
    /* メモリ上のイメージ */
    const char *p = buf;

    /* FileHeader と InfoHeader のコピー */
    ::CopyMemory(&bfh, p,  SIZE_BFH);
    ::CopyMemory(&bih, p + SIZE_BFH, SIZE_BIH);
  }
  else{
    /* ディスクファイル */
    HANDLE hFile;
      
    hFile = mf::OpenFile(buf, GENERIC_READ);

    /* FileHeader の読み込み */
    DWORD readed;

    ::ReadFile(hFile, &bfh, SIZE_BFH, &readed, NULL);
    ::ReadFile(hFile, &bih, SIZE_BIH, &readed, NULL);
    ::CloseHandle(hFile);
  }

  ::ZeroMemory(lpInfo, sizeof(SpiPictureInfo));
  lpInfo->width = bih.biWidth;
  lpInfo->height = bih.biHeight;
  lpInfo->colorDepth = bih.biBitCount;

  return 1;
}

/*=============================================================================
  int BM_GetPicture(const char *, long, unsigned int, HLOCAL *, HLOCAL *)
  ビットマップをメモリに展開
=============================================================================*/

int
BM_GetPicture(const char* buf, long len, unsigned int flag, HLOCAL *pHBInfo,
							HLOCAL *pHBm)
{
  BITMAPFILEHEADER  bfh;
  BITMAPINFOHEADER  bih;
  BITMAPINFO        *pInfo;
  BYTE              *pBits;
  int               headsize, bitsize;

  if(flag & MEM){
    /* メモリ上のイメージ */
    const char *p = buf;

    /* FileHeader と InfoHeader のコピー */
    ::CopyMemory(&bfh, p,  SIZE_BFH);
    ::CopyMemory(&bih, p + SIZE_BFH, SIZE_BIH);

    /* *pHBInfoに必要なサイズを計算し、アロケート */
    headsize = bfh.bfOffBits - SIZE_BFH;
    *pHBInfo = ::LocalAlloc(LHND, headsize);
    if(!*pHBInfo){
      _Err0("error BM_GetPicture() : *pHBInfo のメモリ確保に失敗\n");
      return 0;
    }

    /* BitmapInfoのコピー */
    pInfo = (BITMAPINFO*)::LocalLock(*pHBInfo);
    p = buf + SIZE_BFH;
    ::CopyMemory(pInfo, p, headsize);

    /* *pHBmに必要なサイズを計算し、アロケート
			 bmiHeader.biSizeImage == 0 の場合があるので
			 データサイズからヘッダサイズをのぞいた分を
			 ビットデータのサイズとする */
    bitsize = len - headsize - SIZE_BFH;
    *pHBm = ::LocalAlloc(LHND, bitsize);
    if(!*pHBm){
      _Err0("error BM_GetPicture() : *pHBm のメモリ確保に失敗\n");
      return 0;
    }

    pBits = (BYTE*)::LocalLock(*pHBm);
    p = buf + bfh.bfOffBits;
    ::CopyMemory(pBits, p, bitsize);
  }
  else{ /* ディスクファイル */
    HANDLE hFile;

    hFile = mf::OpenFile(buf, GENERIC_READ);
	
    bitsize = ::GetFileSize(hFile, NULL);
    /* マックバイナリ用
			 でも、呼び出しで0以外の値を渡すことは
			 今のところなし */
    if(len != 0){
      ::SetFilePointer(hFile, len, NULL, FILE_BEGIN);
      bitsize -= len;
    }

    /* FileHeader の読み込み */
    DWORD readed;

    ::ReadFile(hFile, &bfh, sizeof(bfh), &readed, NULL);
    bitsize -= readed;
    headsize = bfh.bfOffBits - sizeof(bfh);
    bitsize -= headsize;
    *pHBInfo = ::LocalAlloc(LHND, headsize);
    *pHBm = ::LocalAlloc(LHND, bitsize);

    if(!*pHBInfo || !*pHBm){
      _Err0("error BM_GetPicture() : メモリ確保に失敗\n");
      return 0;
    }

    pInfo= (BITMAPINFO*)::LocalLock(*pHBInfo);
    pBits= (BYTE*)::LocalLock(*pHBm);

    ::ReadFile(hFile, pInfo, headsize, &readed, NULL);
    ::ReadFile(hFile, pBits, bitsize, &readed, NULL);

    ::CloseHandle(hFile);
  }

  if(pInfo->bmiHeader.biSizeImage == 0){
    pInfo->bmiHeader.biSizeImage = bitsize;
  }

  ::LocalUnlock(*pHBInfo);
  ::LocalUnlock(*pHBm);

  return 1;
}

/*-----------------------------------------------------------------------------
  int CheckFileHeader(BITMAPFILEHEADER* pbfh)
  BITMAPFILEHEADER のチェック
-----------------------------------------------------------------------------*/

int
CheckFileHeader(BITMAPFILEHEADER *pbfh)
{
  if(pbfh->bfType != 0x4d42 || pbfh->bfReserved1 != 0 ||
		 pbfh->bfReserved2 != 0 ||
		 pbfh->bfOffBits < SIZE_BFH + SIZE_BIH){
    return 0;
  }

  return 1;
}

/*-----------------------------------------------------------------------------
  int CheckInfoHeader(BITMAPINFOHEADER *)
  BITMAPINFOHEADER のチェック
-----------------------------------------------------------------------------*/

int
CheckInfoHeader(BITMAPINFOHEADER *pbih)
{
  if(pbih->biSize != SIZE_BIH ||  pbih->biCompression != 0){
    return 0;
  }

  return 1;
}

/*=============================================================================
  int PASCAL PlgCallBack(int, int, long);
  デフォルトのコールバック関数
=============================================================================*/

int PASCAL
PlgCallBack(int nNum, int nDenom, long)
{
  return 0;
}

/*=============================================================================
  int GetArcResult(SpiArcFileInfo*, const char*, ArcResult*)
  目的のファイルの書庫内情報を取得
=============================================================================*/

int
GetArcResult(SpiArcFileInfo *pArc, std::string& szFile, ArcResult *pRes)
{
  SpiArcFileInfo  *p;

  for(p = pArc; p->method[0] != '\0'; p++){
    if(szFile == p->filename){
      pRes->ret = 1;
      pRes->pos = p->position;
      pRes->len = p->filesize;

      return 1;
    }
  }

  ::ZeroMemory(pRes, sizeof(ArcResult));
  return 0;
}

/*
 * $Log: General.cpp,v $
 * Revision 1.2  1999/07/10 05:13:00  MIYABI
 * CVS導入
 *
 */
