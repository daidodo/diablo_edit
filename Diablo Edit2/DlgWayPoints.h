#pragma once

#include "CharacterDialogBase.h"
#include "afxwin.h"

// CDlgWayPoints 对话框

class CDlgWayPoints : public CCharacterDialogBase
{
	DECLARE_DYNAMIC(CDlgWayPoints)

public:
	CDlgWayPoints(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgWayPoints();

// 对话框数据
	enum { IDD = IDD_DIALOG_WayPoints };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
// 自定义函数
public:
	CSize GetSize() const;
	void UpdateUI(const CD2S_Struct & character);
	BOOL GatherData(CD2S_Struct & character);
	void ResetAll();
	void LoadText(void);	//加载控件的字符串内容
private:
	void GetOriginData(int level,BOOL * way);
// 自定义成员
private:
	static const int TEXT_SIZE = 43;
	CString m_sText[TEXT_SIZE];
	INT m_nLevel;	//当前选择的难度级别,0,1,2
	BYTE m_byteOriginData[3][5];
	BOOL m_bWayData[3][40];
	BOOL m_bUIData[40];
	CButton m_nSelAll;
public:
	afx_msg void OnBnClickedNormal();
	afx_msg void OnBnClickedNightmare();
	afx_msg void OnBnClickedHell();
	afx_msg void OnBnClicked_SelAll();
};
