#pragma once

#include "CharacterDialogBase.h"
#include "afxwin.h"
#include "afxcmn.h"

// CDlgCharBasicInfo 对话框

class CDlgCharBasicInfo : public CCharacterDialogBase
{
	DECLARE_DYNAMIC(CDlgCharBasicInfo)

public:
	CDlgCharBasicInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCharBasicInfo();

// 对话框数据
	enum { IDD = IDD_DIALOG_BasicCharInfo };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
// 自定义成员
private:
	static const DWORD	LEVEL_AND_EXPERIENCE[100];
	static const INT	SKILL_SIZE = 30;	//每种人物的技能数目
	static const INT	TEXT_COUNT = 28;	//所有文本控件的内容
	CString m_sText[TEXT_COUNT];
	CButton m_btnSkills;
	CString m_sGoldinPer;
	CString m_sGoldinStash;
	CCharacterDialogBase	** m_dlgTabPage;
	CTabCtrl m_tcBasicInfo;
	int	m_nTabCurSel,m_nTabPageCount;
	//人物信息
	CString m_sVersion;
	CString m_sName;
	CComboBox m_cbCharClass;
	BOOL m_bLadder;
	BOOL m_bExpansion;
	BOOL m_bHardcore;
	BOOL m_bDiedBefore;
	CString m_sCharTitle;
	UINT m_uCharLevel;
	CComboBox m_cbWeaponSet;
	CComboBox m_cbLastDifficult;
	CComboBox m_cbLastACT;
	CTime m_tTime;
	DWORD m_dwExperience;
	CComboBox m_cbLevelAndExp;
	DWORD m_dwStrength;
	DWORD m_dwDexterity;
	DWORD m_dwEnergy;
	DWORD m_dwVitality;
	DWORD m_dwStatPointsRemaining;
	DWORD m_dwSkillPointsRemaining;
	DWORD m_dwMaximumStamina;
	DWORD m_dwCurrentStamina;
	DWORD m_dwMaximumLife;
	DWORD m_dwCurrentLife;
	DWORD m_dwMaximumMana;
	DWORD m_dwCurrentMana;
	DWORD m_dwGoldInBody;
	DWORD m_dwGoldInStash;
	BYTE m_bSkills[SKILL_SIZE];
	DWORD m_dwMaxGoldInBody;
	DWORD m_dwMaxGoldInStash;
// 自定义函数
public:
	void UpdateUI(const CD2S_Struct & character);
	BOOL GatherData(CD2S_Struct & character);
	void ResetAll();
	void LoadText(void);	//加载控件的字符串内容
private:
	void InitUI(void);
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClicked_Skills();
	afx_msg void OnEnChangeLevel();
	afx_msg void OnPaint();
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
};
