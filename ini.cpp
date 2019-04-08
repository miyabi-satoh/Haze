/*=============================================================================

	FILE: ini.cpp
 
=============================================================================*/

/*=============================================================================
	#include statements
=============================================================================*/

#include <stdinc.h>
#include "gSPIChain.h"
#include "General.h"
#include "ini.h"

/*=============================================================================
	#define statements
=============================================================================*/

#define BUF_SIZE  256

/*=============================================================================
  äOïîéQè∆
=============================================================================*/

//extern CSPIChain g_SPI;

/*=============================================================================
  class CListData
=============================================================================*/

/*=============================================================================
  void Write()
=============================================================================*/
void CListData::
Write()
{
  char szPath[_MAX_PATH];

  GetINIPath(szPath);

  WPPS("LIST", "LastList", GetLastList(), szPath);
  WPPS("LIST", "StartIndex", GetStart(), szPath);
  WPPS("LIST", "Option1", GetOption1(), szPath);
  WPPS("LIST", "Option2", GetOption2(), szPath);
  WPPS("LIST", "Pattern", GetDrawPattern(), szPath);
}

/*=============================================================================
  void Read()
=============================================================================*/
void CListData::
Read()
{
  char szPath[_MAX_PATH];

  GetINIPath(szPath);

  GPPS("LIST", "LastList", m_szLastList, szPath);
  m_nStartIndex = (int)GPPI("LIST", "StartIndex", 0, szPath);
  m_byOption1 = (BYTE)GPPI("LIST", "Option1", 0, szPath);
  m_byOption2 = (BYTE)GPPI("LIST", "Option2", 0, szPath);
  m_byDrawPattern = (BYTE)GPPI("LIST", "Pattern", 0, szPath);
}

/*=============================================================================
  class CCommonData
=============================================================================*/

/*=============================================================================
  void Write()
=============================================================================*/
void CCommonData::
Write()
{
  char szPath[_MAX_PATH];

  GetINIPath(szPath);

  WPPS("COMMON", "DblClick", Get2Click(), szPath);
  WPPS("COMMON", "Trans", GetTrans(), szPath);
  WPPS("COMMON", "TextColor", (int)GetTextColor(), szPath);
  WPPS("COMMON", "AppPath", GetAppPath(), szPath);
}

/*=============================================================================
  void Read()
=============================================================================*/
void CCommonData::
Read()
{
  char szPath[_MAX_PATH];

  GetINIPath(szPath);

  m_n2Click = (int)GPPI("COMMON", "DblClick", 0, szPath);
  m_byTrans = (BYTE)GPPI("COMMON", "Trans", 0, szPath);
  m_dwTextColor = (DWORD)GPPI("COMMON", "TextColor", 0, szPath);
  GPPS("COMMON", "AppPath", m_szAppPath, szPath);
}

/*=============================================================================
  class CTimeData
=============================================================================*/

/*=============================================================================
  void Write()
=============================================================================*/
void CTimeData::
Write()
{
  char szPath[_MAX_PATH];

  GetINIPath(szPath);

  WPPS("Time", "Type", GetMode(), szPath);
  WPPS("Time", "Value", (int)GetValue(), szPath);
}

/*=============================================================================
  void Read()
=============================================================================*/
void CTimeData::
Read()
{
  char szPath[_MAX_PATH];

  GetINIPath(szPath);
    
  m_byMode = (BYTE)GPPI("Time", "Type", 0, szPath);
  m_uiValue = (unsigned int)GPPI("Time", "Value", 0, szPath);
}

/*=============================================================================
  class CSetting
=============================================================================*/
const CSetting& CSetting::
operator=(const CSetting& rhs)
{
	if(this != &rhs){
		m_byTilePattern = rhs.m_byTilePattern;
		m_byExtend = rhs.m_byExtend;
		m_byUnit = rhs.m_byUnit;
		m_uiWidth = rhs.m_uiWidth;
		m_uiHeight = rhs.m_uiHeight;
	}
	return *this;
}

/*=============================================================================
  void Write()
=============================================================================*/
void CSetting::
Write()
{
  char szPath[_MAX_PATH];

  GetINIPath(szPath);
	Write("SETTING", szPath);
}

/*=============================================================================
	void Write(const char *, const char *)
=============================================================================*/
void CSetting::
Write(const char *szSection, const char *szPath)
{
	WPPS(szSection, "TilePattern", NULL, szPath);
	WPPS(szSection, "Extend", NULL, szPath);
	WPPS(szSection, "Unit", NULL, szPath);
	WPPS(szSection, "Width", NULL, szPath);
	WPPS(szSection, "Height", NULL, szPath);

  WPPS(szSection, "TilePattern", GetTilePattern(), szPath);
  WPPS(szSection, "Extend", GetExtend(), szPath);
  WPPS(szSection, "Unit", GetUnit(), szPath);
  WPPS(szSection, "Width", GetWidth(), szPath);
  WPPS(szSection, "Height", GetHeight(), szPath);
}

/*=============================================================================
  void Read()
=============================================================================*/
void CSetting::
Read()
{
  char szPath[_MAX_PATH];
  
  GetINIPath(szPath);

	Read("SETTING", szPath);
}

/*=============================================================================
	void Read(const char *, const char *)
=============================================================================*/
void CSetting::
Read(const char *szSection, const char *szPath)
{
  m_byTilePattern = (BYTE)GPPI(szSection, "TilePattern", 0, szPath);
  m_byExtend = (BYTE)GPPI(szSection, "Extend", 0, szPath);
  m_byUnit = (BYTE)GPPI(szSection, "Unit", 0, szPath);
  m_uiWidth = (unsigned int)GPPI(szSection, "Width", 0, szPath);
  m_uiHeight = (unsigned int)GPPI(szSection, "Height", 0, szPath);
}

/*
 * $Log: ini.cpp,v $
 * Revision 1.2  1999/07/10 05:13:00  MIYABI
 * CVSì±ì¸
 *
 */
