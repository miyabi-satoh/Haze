/*=============================================================================
	
	File:         CDocument.h

	Description:  Defintions for CDocument

=============================================================================*/

#ifndef _CDOCUMENT_H
#define _CDOCUMENT_H

/*=============================================================================
	#include statements
=============================================================================*/

#include <CMyImage.h>
#include <string>
#include <vector>
#include "General.h"
#include "ini.h"
#include "gSPIChain.h"

/*=============================================================================
	#define statements
=============================================================================*/

#define STATE_NEW     0x01
#define STATE_CHANGED 0x02

/*=============================================================================
  class CMyFileData
  壁紙用データのクラス
=============================================================================*/

class CMyFileData
{
private:
  /* Bitmap Data */
	CMyImage m_pict;
	PICTDATA_T m_PictData;
  /* Index of ListView */
	int m_nIndex;
  /* ファイル名 */
	std::string m_szFile;
	std::string m_sz00IN;
	std::string m_szArc;
	std::string m_sz00AM;
  /* 個別設定 */
	BYTE m_byFlag;
	CSetting m_Setting;

public:
  /* コンストラクタ・デストラクタ・演算子 */
	CMyFileData();
	CMyFileData(const CMyFileData& data);
	~CMyFileData();
	const CMyFileData& operator=(const CMyFileData& RHS);

  /* メンバアクセス */
	std::string GetFileName() { return m_szFile; }
	std::string GetArcName() { return m_szArc; }
	std::string Get00IN() { return m_sz00IN; }
	std::string Get00AM() { return m_sz00AM; }
	CMyImage GetPicture() { return m_pict;  }
	PICTDATA_T* GetPictureData() { return &m_PictData;  }
	int GetIndex() { return m_nIndex;  }
	CSetting& GetSetting() { return m_Setting; }
	BYTE GetFlag() { return m_byFlag; }

	void SetIndex(int i) { m_nIndex = i; }
	void SetFlag(BYTE b) { m_byFlag = b; }
	void SetFileName(const char * szFile) { m_szFile = szFile; }
	void SetArcName(const char * szArc) { m_szArc = szArc; }
	void Set00IN(const char * szSPI) { m_sz00IN = szSPI; }
	void Set00AM(const char * szSPI) { m_sz00AM = szSPI; }

  /* その他 */
	void Write(const char *szPath);
	void Read(const char *szPath);
	int GetPicture(CSPIChain& chain, CSpiParam& sp);
	int CreatePreview(int iWidth, int iHeight);
protected:
	/* 指定されたプラグインによる操作 */
	int ESP_GetFile(CSpiParam& sp);
	int ESP_GetPicture(CSpiParam& sp);
};

/*=============================================================================
  class CDocument
  個々の画像データの統括、およびファイルへのアクセスを管理するクラス
=============================================================================*/

class CDocument : public std::vector<CMyFileData>
{
private:

	BYTE m_byState;
	std::string m_szFile;

public:
	typedef std::vector<CMyFileData>::iterator fData;

	CDocument() : std::vector<CMyFileData>(), m_byState(0) {}
	~CDocument() {}

	void Init() { m_byState = 0; m_szFile = ""; }
	int Create();
	int Open(const char *);
	int Save(const char * = NULL);

	CDocument& operator=(const CDocument& RHS);

	std::string GetName() const
	{ return m_szFile; }
	bool IsChanged() const
	{ return (m_byState & STATE_CHANGED) == STATE_CHANGED; }
	bool IsNew() const
	{ return (m_byState & STATE_NEW) == STATE_NEW; }
	void  SetChangeFlag() { m_byState |= STATE_CHANGED; }
};

#endif /* _CDOCUMENT_H */

/*
 * $Log: CDocument.h,v $
 * Revision 1.2  1999/07/10 05:12:58  MIYABI
 * CVS導入
 *
 */
