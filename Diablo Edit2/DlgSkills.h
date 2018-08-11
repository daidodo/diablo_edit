#pragma once

#include "afxcmn.h"

// CDlgSkills 对话框

class CDlgSkills : public CDialog
{
	DECLARE_DYNAMIC(CDlgSkills)
public:
	CDlgSkills(int charClass,BYTE * skills,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSkills();

// 对话框数据
	enum { IDD = IDD_DIALOG_Skills };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
// 自定义函数
public:
	void LoadText();
// 自定义成员
private:
	static const int SKILL_SIZE = CDiabloEdit2App::CLASS_SKILL_NAME_SIZE;
	static const int TAB_SIZE = CDiabloEdit2App::CLASS_SKILL_TAB_SIZE;
	static const int TEXT_SIZE = SKILL_SIZE + TAB_SIZE;
	CString m_sText[TEXT_SIZE];
	int m_nCharClass;			//人物角色
	BYTE m_pSkills[SKILL_SIZE];	//技能等级数组
	BYTE * m_pData;
	BYTE m_bAll;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
};
