/*=============================================================================

	FILE: gSPIChain.cpp
 
=============================================================================*/

/*=============================================================================
	#include statements
=============================================================================*/

#include "gSPIChain.h"
#include "General.h"
#include "ini.h"
#include <MyFunction.h>

/*=============================================================================
	#define statements
=============================================================================*/

#define BUF_SIZE 256

/*=============================================================================
  外部参照
=============================================================================*/

//extern CSPIChain g_SPI;

/*-----------------------------------------------------------------------------
	local function
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  void _writeplugindata(const char *, CSPIData&, int)
  プラグインデータ書き出し
-----------------------------------------------------------------------------*/
void _writeplugindata(const char *szINI, CSPIData& data, int n)
{
	char szKey1[BUF_SIZE], szKey2[BUF_SIZE], szTemp[_MAX_PATH];

	::wsprintf(szKey1, "Name-%d", n);
	::wsprintf(szKey2, "Use-%d", n);

	::GetFileTitle(data.GetName().c_str(), szTemp, _MAX_PATH - 1);
	WPPS("PLUGIN", szKey1, szTemp, szINI);
	WPPS("PLUGIN", szKey2, (int)data.Use(), szINI);
}

/*=============================================================================
	class CSPIData
=============================================================================*/

/*=============================================================================
  CSPIData()
  コンストラクタ(デフォルト)
=============================================================================*/

CSPIData::
CSPIData() : CMySPI()
{
  m_byUse = TRUE;
}

/*=============================================================================
  CSPIData(const SPIData&)
  コピーコンストラクタ
=============================================================================*/

CSPIData::
CSPIData(const CSPIData& data) : CMySPI()
{
  operator=(data);
}

/*=============================================================================
  CSPIData(const char *, BOOL, FTYPE)
  初期値指定コンストラクタ
=============================================================================*/

CSPIData::
CSPIData(const char *fname, BYTE b) : CMySPI()
{
  m_szName = fname;
  if((m_byUse = b) == 1){
    LoadSPI();
  }
}

/*=============================================================================
  CSPIData& operator=(const CSPIData&)
  代入演算子
=============================================================================*/

CSPIData& CSPIData::
operator=(const CSPIData& RHS)
{
  if(this == &RHS){
    return (*this);
  }

  m_szName = RHS.m_szName;

  if((m_byUse = RHS.m_byUse) == 1){
    LoadSPI();
  }

  return (*this);
}

/*=============================================================================
  int LoadSPI()
  Plug-inをロードする
=============================================================================*/

int CSPIData::
LoadSPI()
{
  return CMySPI::LoadSPI(m_szName.c_str());
}

/*=============================================================================
  int Initialize(const char *, BOOl = TRUE)
  初期化
=============================================================================*/

int CSPIData::
Initialize(const char *szFile, BYTE b)
{
  _Err1("CSPIData::Initiaize(%s)\n", szFile);

  m_szName = szFile;

  if(LoadSPI() == 0){
    m_szName = "";
    return FALSE;
  }
  else if((m_byUse = b) == 0){
    FreeSPI();
  }

  return TRUE;
}

/*=============================================================================
	class CSpiParam
=============================================================================*/

/*=============================================================================
	CSpiParam()
	コンストラクタ
=============================================================================*/
CSpiParam::
CSpiParam()
{
	m_pMem = NULL;
	m_hBInfo = m_hBm = m_hDest = m_hArcInfo = NULL;
	m_len = m_uiFlag = m_lData = m_nType = m_nEnd = 0;
	m_fn = NULL;
}

/*=============================================================================
	~CSpiParam()
	デストラクタ
=============================================================================*/
CSpiParam::
~CSpiParam()
{
	mf::_LocalFree(m_hBInfo);
	mf::_LocalFree(m_hBm);
	mf::_LocalFree(m_hDest);
	mf::_LocalFree(m_hArcInfo);
}

/*=============================================================================
  class CSPIChain
=============================================================================*/

/*=============================================================================
  int AllDisable()
  使用するプラグインが一つでもあれば0
=============================================================================*/

int CSPIChain::
AllDisable_IN()
{
  for(Node pn = in().begin(); pn != in().end(); pn++){
    if(pn->Use() == TRUE){
      return 0;
    }
  }
	return 1;
}
int CSPIChain::
AllDisable_AM()
{
	for(Node pn = am().begin(); pn != am().end(); pn++){
    if(pn->Use() == TRUE){
      return 0;
    }
  }

  return 1;
}

/*=============================================================================
  void LoadAll()
  全てのプラグインをロード
=============================================================================*/

void CSPIChain::
LoadAll()
{
  Node pn;

  for(pn = in().begin(); pn != in().end(); pn++){
    if(pn->GetHandle() == NULL){
      pn->LoadSPI();
    }
  }
  for(pn = am().begin(); pn != am().end(); pn++){
    if(pn->GetHandle() == NULL){
      pn->LoadSPI();
    }
  }
}

/*=============================================================================
  void FreeAll()
  全てのプラグインを解放
=============================================================================*/

void CSPIChain::
FreeAll()
{
  Node pn;

  for(pn = in().begin(); pn != in().end(); pn++){
    if(pn->GetHandle()){
      pn->FreeSPI();
    }
  }
  for(pn = am().begin(); pn != am().end(); pn++){
    if(pn->GetHandle()){
      pn->FreeSPI();
    }
  }
}

/*=============================================================================
  CSPIChain& operator=(CSPIChain&)
  代入演算子
=============================================================================*/

CSPIChain& CSPIChain::
operator=(CSPIChain& RHS)
{
  if(this != &RHS){
		m_szPath = RHS.m_szPath;
		m_listIN = RHS.m_listIN;
		m_listAM = RHS.m_listAM;
	}
  return *this;
}

/*=============================================================================
	CSPIChain::Node Find_IN(char *, char * = NULL)
	展開可能なINプラグインを検索
=============================================================================*/
CSPIChain::Node CSPIChain::
Find_IN(char *szFile, char *pMem)
{
	return Find(in(), szFile, pMem);
}

/*=============================================================================
	CSPIChain::Node Find_AM(char *, char * = NULL)
	展開可能なAMプラグインを検索
=============================================================================*/
CSPIChain::Node CSPIChain::
Find_AM(char *szFile, char *pMem)
{
	return Find(am(), szFile, pMem);
}

/*=============================================================================
  int GetArchiveInfo(const char *, HLOCAL*)
  書庫情報を取得する
=============================================================================*/
int CSPIChain::
GetArchiveInfo(const char *szArc, HLOCAL* pHInfo)
{
  if(pHInfo == NULL){
    _Err0("CSPIChain::GetArchiveInfo() return 0\n");
    return 0;
  }

  Node pn = Find_AM((char *)szArc, NULL);
  if(pn != am().end()){
    pn->GetArchiveInfo((char *)szArc, 0, DISK, pHInfo);
  }

  return (*pHInfo == NULL) ? 0 : 1;
}

/*=============================================================================
  int GetArchiveInfo(CSpiParam&)
  書庫情報を取得する
=============================================================================*/
int CSPIChain::
GetArchiveInfo(CSpiParam& sp)
{
  Node pn = Find(am(), (char *)sp.m_szArc.c_str(), sp.m_pMem);
  if(pn != am().end()){
		if(sp.m_pMem){
			pn->GetArchiveInfo(sp.m_pMem, sp.m_len, sp.m_uiFlag, &(sp.m_hArcInfo));
		}
		else{
			pn->GetArchiveInfo((char *)sp.m_szArc.c_str(), sp.m_len, sp.m_uiFlag,
												 &(sp.m_hArcInfo));
		}
  }

  return (sp.m_hArcInfo == NULL) ? 0 : 1;
}

/*=============================================================================
	int GetPicture(CSpiParam&)
=============================================================================*/
int CSPIChain::
GetPicture(CSpiParam& sp)
{
	Node pn;
	int ret = 0;

	switch(sp.m_nType){
		case TYPE_UNKNOWN:
			if(Find(am(), (char*)sp.m_szArc.c_str(), sp.m_pMem) != am().end()){
				sp.m_nType = TYPE_ARCHIVE;
			}
			else if(Find(in(), (char *)sp.m_szFile.c_str(), sp.m_pMem)
							!= in().end()){
				sp.m_nType = TYPE_FILE;
			}
			else{
				break;
			}

			if(sp.m_nEnd == END_TYPE){
				ret = 1;
				break;
			}
			ret = GetPicture(sp);
			break; /* case TYPE_UNKNOWN: */

		case TYPE_ARCHIVE:
		{
			pn = Find(am(), (char *)sp.m_szArc.c_str(), sp.m_pMem);
			if(pn == am().end()){
				break;
			}
			if(sp.m_pMem){
				pn->GetArchiveInfo(sp.m_pMem, sp.m_len, sp.m_uiFlag, &(sp.m_hArcInfo));
			}
			else{
				pn->GetArchiveInfo((char *)sp.m_szArc.c_str(), sp.m_len, sp.m_uiFlag,
													 &(sp.m_hArcInfo));
			}

			if(sp.m_hArcInfo == NULL){
				break;
			}
			if(sp.m_nEnd == END_ARCINFO){
				ret = 1;
				break;
			}
			
			ArcResult aRes;
			SpiArcFileInfo *pArc = (SpiArcFileInfo*)::LocalLock(sp.m_hArcInfo);
			if(GetArcResult(pArc, sp.m_szFile, &aRes) == 0){
				break;
			}
			
			ret = pn->GetFile((char *)sp.m_szArc.c_str(), aRes.pos,
												(char *)&(sp.m_hDest), DISK2MEM, sp.m_fn, sp.m_lData);
			if(ret == 0){
				break;
			}

			sp.m_nType = TYPE_FILE;
			sp.m_pMem = (char *)::LocalLock(sp.m_hDest);
			sp.m_len = aRes.len;
			sp.m_uiFlag = MEM;
			
			if(sp.m_nEnd == END_GETFILE){
				ret = 1;
				break;
			}
				/* 再帰 */
			ret = GetPicture(sp);
		}
		break; /* case TYPE_ARCHIVE: */

		case TYPE_FILE:
		{
			if(sp.m_pMem){
				if(BM_IsSupported(NULL, (DWORD)sp.m_pMem)){
					ret = BM_GetPicture(sp.m_pMem, sp.m_len, sp.m_uiFlag, &(sp.m_hBInfo),
															&(sp.m_hBm));
				}
			}
			else{
				HANDLE hFile = mf::OpenFile(sp.m_szFile.c_str(), GENERIC_READ);

				ret = BM_IsSupported(NULL, (DWORD)hFile);
				::CloseHandle(hFile);
				if(ret){
					ret = BM_GetPicture(sp.m_szFile.c_str(), sp.m_len, sp.m_uiFlag,
															&(sp.m_hBInfo), &(sp.m_hBm));
				}
			}
			
			if(ret){
				break;
			}
			
			pn = Find(in(), (char *)sp.m_szFile.c_str(), sp.m_pMem);
			if(pn == in().end()){
				break;
			}

			if(sp.m_pMem){
				ret = pn->GetPicture(sp.m_pMem, sp.m_len, sp.m_uiFlag, &(sp.m_hBInfo),
														 &(sp.m_hBm), sp.m_fn, sp.m_lData);
			}
			else{
				ret = pn->GetPicture((char *)sp.m_szFile.c_str(), sp.m_len,
														 sp.m_uiFlag, &(sp.m_hBInfo), &(sp.m_hBm),
														 sp.m_fn, sp.m_lData);
			}
		}
		break; /* case TYPE_FILE */

		default:
			ret = 1;
			break;
	}

	return ret;
}

/*-----------------------------------------------------------------------------
  CSPIChain::Node Find(List&, char *, char * = NULL)
  展開可能なプラグインを検索
	見つからない場合は、end()ノードを返す
-----------------------------------------------------------------------------*/
CSPIChain::Node CSPIChain::
Find(List& list, char *szFile, char *pMem)
{
  Node pn = list.end();

  if(pMem){
    pn = IsSupport(list, szFile, 0, pMem);
  }
  else if(szFile && szFile[0] != '\0'){
    HANDLE hFile = mf::OpenFile(szFile, GENERIC_READ);
    if(hFile != INVALID_HANDLE_VALUE){
      pn = IsSupport(list, szFile, hFile, NULL);
      ::CloseHandle(hFile);
    }
  }

  return pn;
}

/*-----------------------------------------------------------------------------
  CSPIChain::Node IsSupport(List&, char *, HANDLE, char *)
  サポートしているプラグインを検索
	見つからない場合はend()ノードを返す
-----------------------------------------------------------------------------*/
CSPIChain::Node CSPIChain::
IsSupport(List& list, char *szFile, HANDLE hFile, char *pMem)
{
  /* パラメーターチェック */
  if((hFile == INVALID_HANDLE_VALUE && !pMem) || list.empty()){
    return list.end();
  }

  DWORD dw;
  Node pn;

  for(pn = list.begin(); pn != list.end(); pn++){
    if(pn->Use() == 0){
      _Err1("Plugin %s は使用しないのでパスします\n", pn->GetName().c_str());
      continue;
    }

    if(pn->IsActive() == FALSE){
      pn->LoadSPI();
    }

    if(pMem){
      char *p = pMem;
      dw = (DWORD)p;
    }
    else{
      ::SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
      dw = (DWORD)hFile;
    }

    _Err2("Call IsSupported(%s, %#010X)\n", szFile, dw);
    if(pn->IsSupported(szFile, dw)){
      if(pn != list.begin()){
				list.push_front(*pn);
				list.erase(pn);
      }
      return list.begin();
    }
    else{
      _Err2("%s は %s をサポートしてません\n", pn->GetName().c_str(),
						szFile);
    }
    _Err0("IsSupported() return 0\n");
  }

  return pn;
}

/*=============================================================================
  global functions
=============================================================================*/

/*=============================================================================
  void InitializedSPIChain()
  プラグインチェインの初期化
=============================================================================*/

void CSPIChain::
InitializedSPIChain()
{
  if(in().empty() == FALSE || am().empty() == FALSE){
    return;
  }

  _Err0("InitializedSPIChain()\n");

  in().clear();
  am().clear();

	char szPath[_MAX_PATH], szKey1[BUF_SIZE], szKey2[BUF_SIZE];
	int nCount, nUse, nType;
	std::string szSpi, szSpiDir, szTemp;

	GetINIPath(szPath);

	GPPS("PLUGIN", "Path", szSpiDir, szPath);
	SetPath(szSpiDir.c_str());

	nCount = (int)GPPI("PLUGIN", "Count", 0, szPath);

	for(int i = 0; i < nCount; i++) {
		::wsprintf(szKey1, "Name-%d", i);
		::wsprintf(szKey2, "Use-%d", i);
	
		nUse = (int)GPPI("PLUGIN", szKey2, 1, szPath);
		GPPS("PLUGIN", szKey1, szTemp, szPath);
		szSpi = szSpiDir + szTemp;

    CSPIData data;

		if(data.Initialize(szSpi.c_str(), nUse)){
	    nType = data._GetPluginType();

	    if(nType == SPI_00IN){
				in().push_back(data);
	    }
	    else{
				am().push_back(data);
	    }
		}
	}
}

/*=============================================================================
  void WritePluginData()
=============================================================================*/
void CSPIChain::
WritePluginData()
{
	char szPath[_MAX_PATH];
	char szKey1[BUF_SIZE], szKey2[BUF_SIZE];
	int nCount;

	GetINIPath(szPath);
    
	nCount = (int)GPPI("PLUGIN", "Count", 0, szPath);

  /* 既存のキーを全て削除する */
	for(int i = 0; i < nCount; i++){
		::wsprintf(szKey1, "Name-%d", i);
		::wsprintf(szKey2, "Use-%d", i);

		WPPS("PLUGIN", szKey1, NULL, szPath);
		WPPS("PLUGIN", szKey2, NULL, szPath);
	}

	WPPS("PLUGIN", "Path", GetPath().c_str(), szPath);
	nCount = in().size() + am().size();
	WPPS("PLUGIN", "Count", nCount, szPath);

	Node pn;
	for(pn = in().begin(), i = 0; pn != in().end(); i++, pn++){
		_writeplugindata(szPath, *pn, i);
	}
	for(pn = am().begin(); pn != am().end(); i++, pn++){
		_writeplugindata(szPath, *pn, i);
	}
}

/*
 * $Log: gSPIChain.cpp,v $
 * Revision 1.2  1999/07/10 05:13:00  MIYABI
 * CVS導入
 *
 */
