/*=============================================================================

	FILE: CDocument.cpp
 
=============================================================================*/

/*=============================================================================
	#include statements
=============================================================================*/

#include <stdinc.h>
#include <MyFunction.h>

#include "CDocument.h"

/*=============================================================================
	#define statements
=============================================================================*/

#define BUF_SIZE 256

/*=============================================================================
  外部参照
=============================================================================*/

extern CSPIChain g_tempSPI;

/*=============================================================================
  class CMyFileData
=============================================================================*/

/*=============================================================================
  CMyFileData()
  デフォルトコンストラクタ
=============================================================================*/
CMyFileData::
CMyFileData() : m_nIndex(0), m_byFlag(0)
{
	::ZeroMemory(&m_PictData, sizeof(PICTDATA_T));
}

/*=============================================================================
  CMyFileData(const CMyFileData&)
  コピーコンストラクタ
=============================================================================*/
CMyFileData::
CMyFileData(const CMyFileData& data)
{
	operator=(data);
}

/*=============================================================================
  ~CMyFileData()
  デストラクタ
=============================================================================*/
CMyFileData::
~CMyFileData()
{
}

/*=============================================================================
  const CMyFileData& operator=(const CMyFileData&)
  代入演算子
=============================================================================*/
const CMyFileData& CMyFileData::
operator=(const CMyFileData& RHS)
{
	if(this == &RHS){
		return (*this);
	}

	m_PictData = RHS.m_PictData;

	m_nIndex = RHS.m_nIndex;

	m_szFile = RHS.m_szFile;
	m_szArc = RHS.m_szArc;

	m_sz00IN = RHS.m_sz00IN;
	m_sz00AM = RHS.m_sz00AM;

	m_byFlag = RHS.m_byFlag;

	if(m_byFlag != 0){
		m_Setting = RHS.m_Setting;
	}

	if(RHS.m_pict.GetWidth() != 0){
		m_pict = RHS.m_pict;
	}

	return (*this);
}

/*=============================================================================
  void Write(const char *)
  データ書き込み
=============================================================================*/
void CMyFileData::
Write(const char *szPath)
{
	char szSection[BUF_SIZE];

	::wsprintf(szSection, "FILE-%d", m_nIndex);

	WPPS(szSection, "Archive", m_szArc.c_str(), szPath);
	WPPS(szSection, "Picture", m_szFile.c_str(), szPath);
	WPPS(szSection, "00AM", m_sz00AM.c_str(), szPath);
	WPPS(szSection, "00IN", m_sz00IN.c_str(), szPath);

	WPPS(szSection, "Flag", (int)m_byFlag, szPath);

	if(m_byFlag != 0){
		m_Setting.Write(szSection, szPath);
	}
}

/*=============================================================================
  void Read(const char *)
  データ読み込み
=============================================================================*/
void CMyFileData::
Read(const char * szPath)
{
	char szSection[BUF_SIZE];

	::wsprintf(szSection, "FILE-%d", m_nIndex);

	GPPS(szSection, "Archive", m_szArc, szPath);
	GPPS(szSection, "Picture", m_szFile, szPath);
	GPPS(szSection, "00AM", m_sz00AM, szPath);
	GPPS(szSection, "00IN", m_sz00IN, szPath);

	m_byFlag = (BYTE)GPPI(szSection, "Flag", 0, szPath);

	if(m_byFlag != 0){
		m_Setting.Read(szSection, szPath);
	}
}

/*=============================================================================
  int GetPicture(CSPIChain&, CSpiParam&)
  画像展開
=============================================================================*/
int CMyFileData::
GetPicture(CSPIChain& chain, CSpiParam& sp)
{
	int ret = 0;

	switch(sp.m_nType){
		case TYPE_UNKNOWN:
			sp.m_szArc = m_szArc;
			sp.m_szFile = m_szFile;
			sp.m_uiFlag = DISK;
			sp.m_len = 0;
			sp.m_nEnd = END_TYPE;
			/* ファイルタイプを判断 */
			if(chain.GetPicture(sp) == 0){
				break;
			}
			ret = GetPicture(chain, sp);
			break;

		case TYPE_ARCHIVE:
			/* 書庫を展開 */
			sp.m_nEnd = END_GETFILE;
			ret = ESP_GetFile(sp);
			if(ret == 0){
				ret = chain.GetPicture(sp);
			}
			ret = GetPicture(chain, sp);
			break;

		case TYPE_FILE:
			/* ビットマップに変換*/
			sp.m_nEnd = END_GETPICTURE;
			ret = ESP_GetPicture(sp);
			if(ret == 0){
				ret = chain.GetPicture(sp);
			}
			break;
	}

	return ret;
}

/*-----------------------------------------------------------------------------
	int ESP_GetFile(CSpiParam&)
-----------------------------------------------------------------------------*/
int CMyFileData::
ESP_GetFile(CSpiParam& sp)
{
	if(m_sz00AM.length()){
		CMySPI spi;

		spi.LoadSPI(m_sz00AM.c_str());
		spi.GetArchiveInfo((char *)m_szArc.c_str(), 0, DISK, &(sp.m_hArcInfo));
		if(sp.m_hArcInfo){
			ArcResult aRes;
			SpiArcFileInfo *pArc = (SpiArcFileInfo*)::LocalLock(sp.m_hArcInfo);

			if(GetArcResult(pArc, m_szFile, &aRes)){
				int ret = spi.GetFile((char *)m_szArc.c_str(), aRes.pos,
															(char *)&(sp.m_hDest), DISK2MEM, sp.m_fn,
															sp.m_lData);
				if(ret){
					sp.m_nType = TYPE_FILE;
					sp.m_pMem = (char *)::LocalLock(sp.m_hDest);
					sp.m_len = aRes.len;
					sp.m_uiFlag = MEM;
					return 1;
				}
			}
		}
	}

	mf::_LocalFree(sp.m_hArcInfo);
	mf::_LocalFree(sp.m_hDest);
	return 0;
}

/*-----------------------------------------------------------------------------
	int ESP_GetPicture(CSpiParam&)
-----------------------------------------------------------------------------*/
int CMyFileData::
ESP_GetPicture(CSpiParam& sp)
{
	int ret = 0;
	if(m_sz00IN.length()){
		CMySPI spi;
    spi.LoadSPI(m_sz00IN.c_str());
		if(sp.m_pMem){
			ret = spi.GetPicture(sp.m_pMem, sp.m_len, sp.m_uiFlag, &(sp.m_hBInfo),
													 &(sp.m_hBm), sp.m_fn, sp.m_lData);
		}
		else{
			ret = spi.GetPicture((char *)sp.m_szFile.c_str(), sp.m_len,
													 sp.m_uiFlag, &(sp.m_hBInfo), &(sp.m_hBm),
													 sp.m_fn, sp.m_lData);
		}
	}

	return ret;
}

/*=============================================================================
  int CreatePreview(int, int)
  プレビュー画像を作成
=============================================================================*/
int CMyFileData::
CreatePreview(int iWidth, int iHeight)
{
	if(m_pict.GetWidth() != 0){
		_Err0("CMyFileData::CreatePreview() return 1\n");
		return 1;
	}

	int err;
	CSpiParam sp;

	sp.m_fn = (FARPROC)PlgCallBack;
	sp.m_nType = TYPE_UNKNOWN;
	err = GetPicture(g_tempSPI, sp);

	if(err != 0){
		POINT pt;

		pt.x = iWidth;
		pt.y = iHeight;
		::CreatePreview(m_pict, &(sp.m_hBInfo), &(sp.m_hBm), &pt);
		m_PictData.iWidth = pt.x;
		m_PictData.iHeight = pt.y;
		m_PictData.iColorMode = m_pict.GetBitCount();
	}

	return err;
}

/*=============================================================================
  class CDocument
=============================================================================*/

/*=============================================================================
  int Create()
  新規作成
=============================================================================*/
int CDocument::
Create()
{
  /* 現在のリストが変更されている */
	if(IsChanged()){
		return 0;
	}

	clear();
	m_szFile = MUDAI;
	m_byState = STATE_NEW;

	return 1;
}

/*=============================================================================
  int Open(const char *)
  既存のファイルを開く
=============================================================================*/
int CDocument::
Open(const char * szFile)
{
  /* 現在のリストが変更されている */
	if(IsChanged()){
		return 0;
	}

  /* ファイルの存在を確認 */
	HANDLE hFind;
	WIN32_FIND_DATA fd;

	hFind = ::FindFirstFile(szFile, &fd);
	if(hFind == INVALID_HANDLE_VALUE){
		return 0;
	}
	::FindClose(hFind);

	std::string szTemp;
	int   nCount;

  /* ヘッダのチェック */
	GPPS("HEADER", "Type", szTemp, szFile);

	if(szTemp != "LIST"){
		return 0;
	}

	nCount = (int)GPPI("HEADER", "Count", 0, szFile);

	clear();

	for(int i = 0; i < nCount; i++){
		CMyFileData data;

		data.SetIndex(i);
		data.Read(szFile);

		push_back(data);
	}

	m_szFile = szFile;
	m_byState = 0;

	return 1;
}

/*=============================================================================
  int Save(const char * = NULL)
  保存
=============================================================================*/
int CDocument::
Save(const char * szFile)
{
  char  szPath[_MAX_PATH];

	if(szFile != NULL && ::lstrlen(szFile) != 0){
		::lstrcpy(szPath, szFile);
	}
	else{
		::lstrcpy(szPath, m_szFile.c_str());
	}

	WPPS("HEADER", "Type", "LIST", szPath);
	WPPS("HEADER", "Count", size(), szPath);

	fData pn;

	for(pn = begin(); pn != end(); pn++){
		pn->Write(szPath);
	}

	m_szFile = szPath;
	m_byState = 0;

	return 1;
}

/*
 * $Log: CDocument.cpp,v $
 * Revision 1.2  1999/07/10 05:12:58  MIYABI
 * CVS導入
 *
 */
