/*=============================================================================
	
	File:         gSPIChain.h

	Description:  Defintions for CSPIData, CSPIChain

=============================================================================*/

#ifndef _GSPICHAIN_H
#define _GSPICHAIN_H

/*=============================================================================
	#include statements
=============================================================================*/

#include <stdinc.h>
#include <CMySPI.h>

#include <string>
#include <list>

/*=============================================================================
  class CSPIData
  プラグインデータ
=============================================================================*/

class CSPIData : public CMySPI
{
private:
  std::string m_szName;
  BYTE m_byUse;

public:
  CSPIData();
  CSPIData(const CSPIData& data);
  CSPIData(const char *szFile, BYTE b = 1);

  ~CSPIData() {}

  CSPIData& operator=(const CSPIData& RHS);

  int LoadSPI();
  int Initialize(const char *szFile, BYTE b = 1);

  std::string GetName() const { return m_szName; }
  BYTE Use() const { return m_byUse; }
  bool IsActive() const { return (GetHandle() == NULL) ? FALSE : TRUE; }
  void SetUsing(BYTE by) { m_byUse = by; }
};

/*=============================================================================
	class CSpiParam
	Susie Plug-in の展開関数に渡すパラメータ
=============================================================================*/

#define TYPE_UNKNOWN 0
#define TYPE_ARCHIVE 1
#define TYPE_FILE    2
#define TYPE_EXIT    3

#define END_GETPICTURE 0
#define END_TYPE       1
#define END_ARCINFO    2
#define END_GETFILE    3

class CSpiParam
{
public:
	CSpiParam();
	~CSpiParam();

	char *m_pMem;
	int m_nType;
	int m_nEnd; 
	long m_len;
	long m_lData;
	unsigned int m_uiFlag;
	HLOCAL m_hBInfo;
	HLOCAL m_hBm;
	HLOCAL m_hDest;
	HLOCAL m_hArcInfo;
	FARPROC m_fn;
	std::string m_szFile;
	std::string m_szArc;
};

/*=============================================================================
	class CSPIChan
  Plugin のチェイン
=============================================================================*/

class CSPIChain
{
public:
  typedef std::list<CSPIData> List;
  typedef List::iterator Node;
  typedef List::reference refNode;

  CSPIChain() {}
  CSPIChain(CSPIChain& rhs) { operator=(rhs); }

  ~CSPIChain()  {}

  void SetPath(const char *szPath) { m_szPath = szPath; }
  std::string&  GetPath() { return m_szPath; }

	List& in() { return m_listIN; }
	const List& in() const { return m_listIN; }
	List &am() { return m_listAM; }
	const List &am() const { return m_listAM; }

  int AllDisable_IN();
	int AllDisable_AM();

	void InitializedSPIChain();
	void WritePluginData();

  void LoadAll();
  void FreeAll();

  CSPIChain& operator=(CSPIChain& RHS);

  Node Find_IN(char *szFile, char *pMem = NULL);
  Node Find_AM(char *szFile, char *pMem = NULL);

  int GetArchiveInfo(const char *szArc, HLOCAL *pHInfo);

	int GetArchiveInfo(CSpiParam& sp);
	int GetPicture(CSpiParam& sp);

protected:
  Node IsSupport(List&, char *, HANDLE, char *);
  Node Find(List&, char *szFile, char *pMem = NULL);

private:
  /* 検索ディレクトリ */
  std::string m_szPath;
	List m_listIN;
	List m_listAM;
};

#endif /* _GSPICHAIN_H */

/*
 * $Log: gSPIChain.h,v $
 * Revision 1.2  1999/07/10 05:13:00  MIYABI
 * CVS導入
 *
 */
