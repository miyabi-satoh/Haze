/*=============================================================================
	
	File:         ini.h

	Description:  Defintions for DataAccess function

=============================================================================*/

#ifndef _INI_H
#define _INI_H

/*=============================================================================
  #include statements
=============================================================================*/

#include <string>

/*=============================================================================
  class CListData
=============================================================================*/

class CListData
{
public:
  CListData()
    : m_nStartIndex(0), m_byOption1(0), m_byOption2(0), m_byDrawPattern(0)
    {}
  ~CListData() {}

  void Write();
  void Read();

  std::string& List() { return m_szLastList; }

  void SetStart(int n) { m_nStartIndex = n; }
  void SetOption1(int n) { m_byOption1 = (BYTE)n; }
  void SetOption2(int n) { m_byOption2 = (BYTE)n;}
  void SetDrawPattern(int n) { m_byDrawPattern = (BYTE)n; }
  void SetLastList(const char *szFile) { m_szLastList = szFile; }

  int GetStart() const { return m_nStartIndex; }
  int GetOption1() const { return (int)m_byOption1; }
  int GetOption2() const { return (int)m_byOption2; }
  int GetDrawPattern() const { return (int)m_byDrawPattern; }
	const char* GetLastList() const { return m_szLastList.c_str(); }
private:
  int m_nStartIndex;
  BYTE m_byOption1;
  BYTE m_byOption2;
  BYTE m_byDrawPattern;
  std::string m_szLastList;  
};

/*=============================================================================
  class CCommonData
=============================================================================*/

class CCommonData
{
public:
  CCommonData() : m_byTrans(0), m_n2Click(0), m_dwTextColor(0) {}
  ~CCommonData() {}

  void Read();
  void Write();

	std::string& Path() { return m_szAppPath; }
  void SetTrans(int n) { m_byTrans = (BYTE)n; }
  void Set2Click(int n) { m_n2Click = n; }
  void SetTextColor(DWORD dw) { m_dwTextColor = dw; }
  void SetAppPath(const char *szPath) { m_szAppPath = szPath; }

  int GetTrans() const { return (int)m_byTrans; }
  int Get2Click() const { return m_n2Click; }
  DWORD GetTextColor() const { return m_dwTextColor; }
  const char* GetAppPath() const { return m_szAppPath.c_str(); }

private:
  BYTE m_byTrans;
  int m_n2Click;
  DWORD m_dwTextColor;
  std::string m_szAppPath;
};

/*=============================================================================
  class CTimeData
=============================================================================*/

class CTimeData
{
public:
  CTimeData() : m_byMode(0), m_uiValue(0) {}
  ~CTimeData() {}

  void Read();
  void Write();

  void SetMode(int n) { m_byMode = (BYTE)n; }
  void SetValue(unsigned int ui) { m_uiValue = ui; }

  int GetMode() { return (int)m_byMode; }
  unsigned int GetValue() { return m_uiValue; }

private:
  BYTE m_byMode;
  unsigned int m_uiValue;
};

/*=============================================================================
  class CSetting
=============================================================================*/

class CSetting
{
public:
  CSetting() : m_byTilePattern(0), m_byExtend(0), m_byUnit(0),
    m_uiWidth(0), m_uiHeight(0)
    {}
  ~CSetting() {}

	const CSetting& operator=(const CSetting& rhs);
  void Read();
	void Read(const char *szSection, const char *szPath);
  void Write();
	void Write(const char *szSection, const char *szPath);

  void SetTilePattern(int n) { m_byTilePattern = (BYTE)n; }
  void SetExtend(int n) { m_byExtend = (BYTE)n; }
  void SetUnit(int n) { m_byUnit = (BYTE)n; }
  void SetWidth(unsigned int ui) { m_uiWidth = ui; }
  void SetHeight(unsigned int ui) { m_uiHeight = ui; }

  int GetTilePattern() const { return (int)m_byTilePattern; }
  int GetExtend() const { return (int)m_byExtend; }
  int GetUnit() const { return (int)m_byUnit; }
  unsigned int GetWidth() const { return m_uiWidth; }
  unsigned int GetHeight() const { return m_uiHeight; }

private:
  BYTE m_byTilePattern;
  BYTE m_byExtend;
  BYTE m_byUnit;
  unsigned int m_uiWidth, m_uiHeight;
};

#endif /* _INI_H */

/*
 * $Log: ini.h,v $
 * Revision 1.2  1999/07/10 05:13:01  MIYABI
 * CVS“±“ü
 *
 */
